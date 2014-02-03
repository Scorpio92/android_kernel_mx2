/*
 * Bluetooth Broadcom GPIO and Low Power Mode control
 *
 *  Copyright (C) 2011 Samsung Electronics Co., Ltd.
 *  Copyright (C) 2011 Google, Inc.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/hrtimer.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/rfkill.h>
#include <linux/serial_core.h>
#include <linux/wakelock.h>

#include <asm/mach-types.h>

#include <mach/gpio.h>
#include <plat/gpio-cfg.h>
#include <mach/gpio-common.h>

#define BT_LPM_ENABLE
#define BT_FACTORY_MODE

static int gpio_bt_power;//BT_POWER
static int gpio_bt_reset;//BT_RESET
static int gpio_bt_wake;//BT_WAKE
static int gpio_bt_host_wake;//BT_HOST_WAKE
#define BT_RTS EXYNOS4_GPA0(3)

static struct rfkill *bt_rfkill;

struct bcm_bt_lpm {
	int host_wake;

	struct hrtimer enter_lpm_timer;
	ktime_t enter_lpm_delay;

	struct uart_port *uport;

	struct wake_lock wake_lock;
	char wake_lock_name[100];
} bt_lpm;

static volatile int bt_is_running = 0;

int check_bt_running(void) {
	return bt_is_running;
}
EXPORT_SYMBOL(check_bt_running);

void bt_uart_rts_ctrl(int flag)
{
	if (!gpio_get_value(gpio_bt_power))
		return;
	if (flag) {
		// BT RTS Set to HIGH
		s3c_gpio_cfgpin(BT_RTS, S3C_GPIO_OUTPUT);
		s3c_gpio_setpull(BT_RTS, S3C_GPIO_PULL_NONE);
		gpio_set_value(BT_RTS, 1);
	} else {
		// BT RTS Set to LOW
		s3c_gpio_cfgpin(BT_RTS, S3C_GPIO_OUTPUT);
		gpio_set_value(BT_RTS, 0);

		s3c_gpio_cfgpin(BT_RTS, S3C_GPIO_SFN(2));
		s3c_gpio_setpull(BT_RTS, S3C_GPIO_PULL_NONE);
	}
}
EXPORT_SYMBOL(bt_uart_rts_ctrl);

static int bcm4330_bt_rfkill_set_power(void *data, bool blocked)
{
	/* rfkill_ops callback. Turn transmitter on when blocked is false */
	if (!blocked) {
		pr_info("[BT] Bluetooth Power On.\n");

		gpio_set_value(gpio_bt_power, 1);
		msleep(20);
		gpio_set_value(gpio_bt_reset, 1);
		msleep(50);
	} else {
		pr_info("[BT] Bluetooth Power Off.\n");
		bt_is_running = 0;
		gpio_set_value(gpio_bt_power, 0);
		gpio_set_value(gpio_bt_reset, 0);
	}
	return 0;
}

static const struct rfkill_ops bcm4330_bt_rfkill_ops = {
	.set_block = bcm4330_bt_rfkill_set_power,
};

#ifdef BT_LPM_ENABLE
static void set_wake_locked(int wake)
{

	if (!wake)
		wake_unlock(&bt_lpm.wake_lock);

	gpio_set_value(gpio_bt_wake, wake);
}

static enum hrtimer_restart enter_lpm(struct hrtimer *timer)
{
	unsigned long flags;
	spin_lock_irqsave(&bt_lpm.uport->lock, flags);
	bt_is_running = 0;
	set_wake_locked(0);
	spin_unlock_irqrestore(&bt_lpm.uport->lock, flags);

	return HRTIMER_NORESTART;
}

void bt_uart_wake_peer(struct uart_port *uport)
{
	bt_lpm.uport = uport;

	hrtimer_try_to_cancel(&bt_lpm.enter_lpm_timer);
	bt_is_running = 1;
	set_wake_locked(1);

	hrtimer_start(&bt_lpm.enter_lpm_timer, bt_lpm.enter_lpm_delay,
		HRTIMER_MODE_REL);
}
EXPORT_SYMBOL(bt_uart_wake_peer);

static void update_host_wake_locked(int host_wake)
{
	if (host_wake == bt_lpm.host_wake)
		return;

	bt_lpm.host_wake = host_wake;

	bt_is_running = 1;
	if (host_wake) {
		wake_lock(&bt_lpm.wake_lock);
	} else  {
		/* Take a timed wakelock, so that upper layers can take it.
		 * The chipset deasserts the hostwake lock, when there is no
		 * more data to send.
		 */
		wake_lock_timeout(&bt_lpm.wake_lock, HZ/2);
	}
}

static irqreturn_t host_wake_isr(int irq, void *dev)
{
	int host_wake;
	unsigned long flags;

	host_wake = gpio_get_value(gpio_bt_host_wake);
	irq_set_irq_type(irq, host_wake ? IRQF_TRIGGER_LOW : IRQF_TRIGGER_HIGH);

	if (!bt_lpm.uport) {
		bt_lpm.host_wake = host_wake;
		return IRQ_HANDLED;
	}

	spin_lock_irqsave(&bt_lpm.uport->lock, flags);
	update_host_wake_locked(host_wake);
	spin_unlock_irqrestore(&bt_lpm.uport->lock, flags);

	return IRQ_HANDLED;
}

static int bcm_bt_lpm_init(struct platform_device *pdev)
{
	int irq;
	int ret;

	hrtimer_init(&bt_lpm.enter_lpm_timer, CLOCK_MONOTONIC,
			HRTIMER_MODE_REL);
	bt_lpm.enter_lpm_delay = ktime_set(1, 0);  /* 1 sec */
	bt_lpm.enter_lpm_timer.function = enter_lpm;

	bt_lpm.host_wake = 0;
	bt_is_running = 0;

	snprintf(bt_lpm.wake_lock_name, sizeof(bt_lpm.wake_lock_name),
			"BTLowPower");
	wake_lock_init(&bt_lpm.wake_lock, WAKE_LOCK_SUSPEND,
			 bt_lpm.wake_lock_name);

	irq = gpio_to_irq(gpio_bt_host_wake);
	ret = request_irq(irq, host_wake_isr, IRQF_TRIGGER_HIGH,
		"bt host_wake", NULL);
	if (ret) {
		pr_err("[BT] Request_host wake irq failed.\n");
		return ret;
	}

	ret = irq_set_irq_wake(irq, 1);
	if (ret) {
		pr_err("[BT] Set_irq_wake failed.\n");
		return ret;
	}

	return 0;
}
#endif

#ifdef BT_FACTORY_MODE
static struct delayed_work bt_test_dwork;
static int bt_in_test_mode = 0;
#define BT_LED_DELAY (12 * 1000)

static void bt_test_func(struct work_struct *work)
{
	static int gpio_value = 0;

	mx_set_factory_test_led(gpio_value);
	gpio_value = !gpio_value;
	schedule_delayed_work(&bt_test_dwork, msecs_to_jiffies(250));

	return;
}

static ssize_t bt_test_mode_show(struct device *dev,
     							struct device_attribute *attr, char *buf)
{
	if(bt_in_test_mode)
		return sprintf(buf, "1\n");

	if(mx_is_factory_test_mode(MX_FACTORY_TEST_BT)) {
		msleep(100);
		if(mx_is_factory_test_mode(MX_FACTORY_TEST_BT)) {
			printk("in BT_TEST_MODE\n");
			bt_in_test_mode = 1;            							//test mode

			mx_set_factory_test_led(1);
			INIT_DELAYED_WORK_DEFERRABLE(&bt_test_dwork, bt_test_func);
		}
	}

	return sprintf(buf, "%d\n",  bt_in_test_mode);
	
}

static ssize_t bt_test_mode_store(struct device *dev,
      						struct device_attribute *attr,      const char *buf, size_t count)
{
	unsigned long flash = simple_strtoul(buf, NULL, 10);

	if(bt_in_test_mode) {
		if(flash) {
			cancel_delayed_work_sync(&bt_test_dwork);
			schedule_delayed_work(&bt_test_dwork, 0);
		} else {
			cancel_delayed_work_sync(&bt_test_dwork);
			mx_set_factory_test_led(0);
		}
	}
	return count;
}

static DEVICE_ATTR(bt_test_mode, S_IRUGO|S_IWUSR|S_IWGRP,
		   				bt_test_mode_show, bt_test_mode_store);

#endif

static int bcm4330_bluetooth_probe(struct platform_device *pdev)
{
	int rc = 0;
	int ret;

	gpio_bt_power		= EXYNOS4_GPF3(5);
	gpio_bt_reset		= EXYNOS4_GPF0(3);	
	gpio_bt_wake		= EXYNOS4_GPF0(1);
	gpio_bt_host_wake	= EXYNOS4_GPX2(1);

	gpio_set_value(gpio_bt_power, 0);
	gpio_set_value(gpio_bt_reset, 0);
	gpio_set_value(gpio_bt_wake, 0);

	bt_rfkill = rfkill_alloc("bcm4330 Bluetooth", &pdev->dev,
				RFKILL_TYPE_BLUETOOTH, &bcm4330_bt_rfkill_ops,
				NULL);

	if (unlikely(!bt_rfkill)) {
		pr_err("[BT] bt_rfkill alloc failed.\n");
		return -ENOMEM;
	}

	rfkill_init_sw_state(bt_rfkill, 0);

	rc = rfkill_register(bt_rfkill);

	if (unlikely(rc)) {
		pr_err("[BT] bt_rfkill register failed.\n");
		rfkill_destroy(bt_rfkill);
		return -1;
	}

	rfkill_set_sw_state(bt_rfkill, true);

#ifdef BT_LPM_ENABLE
	ret = bcm_bt_lpm_init(pdev);
	if (ret) {
		rfkill_unregister(bt_rfkill);
		rfkill_destroy(bt_rfkill);
	}
#endif

#ifdef BT_FACTORY_MODE
	ret = device_create_file(&pdev->dev, &dev_attr_bt_test_mode);
	if (ret) {
		pr_info("[BT] bcm4330 factory sys file create failed\n");
	}
#endif
	pr_info("[BT] bcm4330 probe END\n");
	return rc;
}

static int bcm4330_bluetooth_remove(struct platform_device *pdev)
{
	rfkill_unregister(bt_rfkill);
	rfkill_destroy(bt_rfkill);

	wake_lock_destroy(&bt_lpm.wake_lock);
	return 0;
}

static struct platform_driver bcm4330_bluetooth_platform_driver = {
	.probe = bcm4330_bluetooth_probe,
	.remove = bcm4330_bluetooth_remove,
	.driver = {
		   .name = "bcm4330_bluetooth",
		   .owner = THIS_MODULE,
		   },
};

static int __init bcm4330_bluetooth_init(void)
{
	return platform_driver_register(&bcm4330_bluetooth_platform_driver);
}

static void __exit bcm4330_bluetooth_exit(void)
{
	platform_driver_unregister(&bcm4330_bluetooth_platform_driver);
}


module_init(bcm4330_bluetooth_init);
module_exit(bcm4330_bluetooth_exit);

MODULE_ALIAS("platform:bcm4330");
MODULE_DESCRIPTION("bcm4330_bluetooth");
MODULE_LICENSE("GPL");
