/*
 * UBLOX gps driver, Author: Jerry Mo [jerrymo@meizu.com]
 * Copyright (c) 2010 meizu Corporation
 * 
 */
 
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/earlysuspend.h>
#include <linux/err.h>
#include <linux/regulator/consumer.h>

#include <linux/gpio.h>
#include <linux/slab.h>
#include <mach/gpio-common.h>

/*gps driver private data struct*/
struct gps_data {
	struct delayed_work delay_wq;
	int enabled;
	
#ifdef CONFIG_HAS_EARLYSUSPEND
	struct early_suspend	early_suspend;
#endif
};


static int gps_power(int onff)
{
	struct regulator *regulator;

	regulator = regulator_get(NULL, "gps_1.8v");//gps 1.8V
	if (IS_ERR(regulator)) {
		pr_err("%s()->%d:regulator get fail !!\n", __FUNCTION__, __LINE__);
		return -1;
	}
	
	if (onff) 
		regulator_enable(regulator);		
	else 
		regulator_disable(regulator);	
	
	regulator_put(regulator);

	return 0;
}


static ssize_t gps_enable_show(struct device *dev,
				     struct device_attribute *attr, char *buf)
{
	struct gps_data *data = dev_get_drvdata(dev);
	
	return sprintf(buf, "%d\n",  data->enabled);
}


static ssize_t gps_enable_store(struct device *dev,
				      struct device_attribute *attr,
				      const char *buf, size_t count)
{
	unsigned long enable = simple_strtoul(buf, NULL, 10);
	struct gps_data *data = dev_get_drvdata(dev);

	pr_debug("%s():enable %ld.\n", __FUNCTION__, enable);

	if (enable != data->enabled) {
		int ret = gps_power(enable);

		if (ret) {
			pr_err("%s()->%d:set power %ld fail !!\n", __FUNCTION__, __LINE__, 
				enable);
			return -1;
		}

		data->enabled = enable;
	}
		
	return count;
}

static DEVICE_ATTR(enable, S_IRUGO | S_IWUSR,
		   gps_enable_show, gps_enable_store);

static struct attribute *gps_attributes[] = {
	&dev_attr_enable.attr,	
	NULL
};


static struct attribute_group gps_attribute_group = {
	.attrs = gps_attributes
};


#ifdef CONFIG_HAS_EARLYSUSPEND
static void gps_early_suspend(struct early_suspend *h)
{
	struct gps_data *data = container_of(h, struct gps_data, early_suspend);

	if (data->enabled) gps_power(0);
}


static void gps_late_resume(struct early_suspend *h)
{
	struct gps_data *data = container_of(h, struct gps_data, early_suspend);

	if (data->enabled) gps_power(1);
}
#endif


static int __devinit gps_probe(struct platform_device *pdev)
{
	int ret;
	struct gps_data *data;

	data = kzalloc(sizeof(struct gps_data), GFP_KERNEL);
	if(!data) {
		pr_err("%s()->%d:kzalloc fail !!\n", __FUNCTION__, __LINE__);
		return -ENOMEM;
	}

	dev_set_drvdata(&pdev->dev, data);
	
	ret = sysfs_create_group(&pdev->dev.kobj, &gps_attribute_group);
	if (ret < 0) {
		pr_err("%s()->%d:sys create group fail !!\n", __FUNCTION__, __LINE__);
		kfree(data);
		return ret;
	}

#ifdef CONFIG_HAS_EARLYSUSPEND
	data->early_suspend.suspend = gps_early_suspend;
	data->early_suspend.resume = gps_late_resume;
	data->early_suspend.level = EARLY_SUSPEND_LEVEL_BLANK_SCREEN;
	register_early_suspend(&data->early_suspend);
#endif
	if(mx_is_factory_test_mode(MX_FACTORY_TEST_BT)) {
		pr_info("%s():now is in factory test mode!\n", __func__);
		if (!gps_power(1)) 
			data->enabled = 1;
	}

	pr_info("gps successfully probed!\n");
	
	return 0;
}


static int __devexit gps_remove(struct platform_device *pdev)
{
	struct gps_data *data = dev_get_drvdata(&pdev->dev);

	if (data->enabled) gps_power(0);
	sysfs_remove_group(&pdev->dev.kobj, &gps_attribute_group);
	kfree(data);
	
	return 0;
}


/*platform driver data*/
static struct platform_driver gps_driver = {
	.driver = {
		.name = "ublox-gps",
		.owner = THIS_MODULE,
	},
	.probe =   gps_probe,
	.remove = __devexit_p(gps_remove),
};

	
static int __init gps_init(void)
{
	return platform_driver_register(&gps_driver);
}


static void __exit gps_exit(void)
{
	platform_driver_unregister(&gps_driver);
}

module_init(gps_init);
module_exit(gps_exit);

MODULE_AUTHOR("Jerry Mo");
MODULE_DESCRIPTION("ublox gps driver");
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");
