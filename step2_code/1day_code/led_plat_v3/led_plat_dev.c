
#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>

#include "led_info.h"


// 2,实现 struct resource
struct resource led_resource[] = {
	[0] = {
		.start = LED_GPC0_CONF,
		.end = LED_GPC0_CONF + LED_GPC0_SIZE -1,
		.flags = IORESOURCE_MEM,   //第一个地址资源
	},

	//以下为测试举例
	[1] = {
		.start = 8888,
		.end =8888,
		.flags = IORESOURCE_IRQ,   //第一个中断资源
	},
	[2] = {
		.start = 0xe0200160,
		.end = 0xe0200160 + 8 -1,
		.flags = IORESOURCE_MEM,   //第二个地址资源
	},
};

void	led_plat_release(struct device *dev)
{
	printk("----------^_^ %s----------------\n",__FUNCTION__);
}

//初始化平台自定义数据
struct led_platdata led_pdata = {
	.name = "gpc0_3/4",
	.shift = 3,
	.conf_reg_data = 0x11,
	.conf_reg_clear = 0xff,
	.data_reg = 0x3,
};



// 1，创建一个pdev对象
struct platform_device led_pdev = {
	.name = "s5pv210_led",
	.id = -1,
	.num_resources = ARRAY_SIZE(led_resource),
	.resource = led_resource,
	.dev = {
		.platform_data = &led_pdata,  //自定义数据
		.release = led_plat_release,
	}
};

static int __init led_plat_dev_init(void)
{
	printk("----------^_^ %s----------------\n",__FUNCTION__);
	// 3,注册pdev对象
	return platform_device_register(&led_pdev);
}

static void __exit led_plat_dev_exit(void)
{
	printk("----------^_^ %s----------------\n",__FUNCTION__);
	//注销pdev对象
	platform_device_unregister(&led_pdev);
}

module_init(led_plat_dev_init);
module_exit(led_plat_dev_exit);
MODULE_LICENSE("GPL");
