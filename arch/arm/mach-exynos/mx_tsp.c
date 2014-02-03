/*
 * mx_tsp.c - touch pannel driver helper for mx board
 *
 * Copyright (C) 2012 Meizu Technology Co.Ltd, Zhuhai, China
 * Author:  lvcha qiu   <lvcha@meizu.com>
 *
 * This program is not provided / owned by Maxim Integrated Products.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
 
#include <linux/platform_device.h>
#include <linux/interrupt.h>
#include <linux/i2c.h>
#include <linux/i2c/atmel_mxt_ts.h>
#include <mach/i2c-m030.h>
#include <mach/i2c-m032.h>
#include <asm/mach-types.h>

#include <plat/iic.h>

#if defined(CONFIG_TOUCHSCREEN_MX)
/* Initial register values recommended from chip vendor */
static const u8 m032_mxt_init_vals[] = {
	/*[SPT_USERDATA_T38 INSTANCE 0] 264*/
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	/*[GEN_POWERCONFIG_T7 INSTANCE 0] 272*/
	0xff, 0xff, 0x0a,
	/*[GEN_ACQUISITIONCONFIG_T8 INSTANCE 0]275*/
	0x19, 0x00, 0x14, 0x14, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00,
	/*[TOUCH_MULTITOUCHSCREEN_T9 INSTANCE 0]285*/
	0x00, 0x00, 0x00, 0x11, 0x0c, 0x01, 0x30, 0x50, 0x03, 0x05,
	0x00, 0x01, 0x06, 0x01, 0x0A, 0x0a, 0x3e, 0x0a, 0xff, 0x0e,
	0xff, 0x14, 0x00, 0x00, 0x00, 0x00, 0x97, 0x35, 0x40, 0x00,
	0x32, 0x18, 0x00, 0x00, 0x00,
	/*[TOUCH_KEYARRAY_T15 INSTANCE 0]320*/
	0x03, 0x0f, 0x0c, 0x02, 0x01, 0x01, 0x10, 0x28, 0x03, 0x00,
	0x00,
	/*[SPT_COMMSCONFIG_T18 INSTANCE 0]331*/
	0x00, 0x00,
	/*[SPT_GPIOPWM_T19 INSTANCE 0]333*/
	0x01, 0x00, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	/*[TOUCH_PROXIMITY_T23 INSTANCE 0]349*/
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	/*[SPT_SELFTEST_T25 INSTANCE 0]364*/
	0x00, 0x00, 0xE0, 0x73, 0x60, 0x4E, 0xE0, 0x73, 0x60, 0x4E,
	0xE0, 0x73, 0x00, 0x00, 
	/*[PROCI_GRIPSUPPRESSION_T40 INSTANCE 0]378*/
	0x00, 0x00, 0x00, 0x00, 0x00,
	/*[PROCI_TOUCHSUPPRESSION_T42 INSTANCE 0]383*/
	0x02, 0x32, 0x66, 0x38, 0x00, 0x00, 0x09, 0x00,
	/*[SPT_CTECONFIG_T46 INSTANCE 0]391*/
	0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	/*[PROCI_STYLUS_T47 INSTANCE 0]400*/
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	/*0x01, 0x14, 0x46, 0x05, 0x02, 0x32, 0x28, 0x00, 0x00, 0x28,*/
	/*[PROCG_NOISESUPPRESSION_T48 INSTANCE 0]410*/
	0x07, 0x02, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x0A, 0x0A, 0x00, 0x00, 0x64, 0x04, 0x40,
	0x0a, 0x00, 0x14, 0x05, 0x00, 0x2A, 0x00, 0x14, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x10, 0x60, 0x03, 0x01, 0x06, 0x01,
	0x0A, 0x0a, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x97, 0x35, 0x40, 
	0x00, 0x32, 0x18, 0x00,
};

static struct mxt_platform_data __initdata qt602240_platform_data = {
	.config = m032_mxt_init_vals, 
	.config_length = ARRAY_SIZE(m032_mxt_init_vals),
	.x_line	= 17,
	.y_line	= 12,
	.x_size	= 3840,
	.y_size	= 2560,
	.blen	= 48,
	.orient	= MXT_DIAGONAL,
	.irqflags	= IRQF_TRIGGER_LOW | IRQF_ONESHOT,
};
#endif

static struct i2c_board_info __initdata i2c_devs6[] = {
#if defined(CONFIG_TOUCHSCREEN_MX)
	[0] = {
		I2C_BOARD_INFO("qt602240_ts", (0x94 >> 1)),
		.platform_data	= &qt602240_platform_data,
		.irq = IRQ_EINT(1),
	},
#endif
};

static int  __init mx_init_tsp(void)
{
#if defined(CONFIG_TOUCHSCREEN_MX)
	if (machine_is_m030()){
		i2c_devs6[0].irq = IRQ_EINT(15);
		s3c_i2c6_set_platdata(&m030_default_i2c6_data);
	}else{
		s3c_i2c6_set_platdata(&m032_default_i2c6_data);
	}
	/* touch pannel */
	i2c_register_board_info(6, i2c_devs6, ARRAY_SIZE(i2c_devs6));
#endif
	return 0;
}

arch_initcall(mx_init_tsp);

MODULE_DESCRIPTION("mx touch pannel driver helper");
MODULE_AUTHOR("lvcha qiu <lvcha@meizu.com>");
MODULE_LICENSE("GPLV2");
