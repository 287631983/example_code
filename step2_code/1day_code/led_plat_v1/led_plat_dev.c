
#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>

#define LED_GPC0_CONF 0xe0200060
#define LED_GPC0_SIZE 8


// 2,ʵ�� struct resource
struct resource led_resource[] = {
	[0] = {
		.start = LED_GPC0_CONF,
		.end = LED_GPC0_CONF + LED_GPC0_SIZE -1,
		.flags = IORESOURCE_MEM,   //��һ����ַ��Դ
	},

	//����Ϊ���Ծ���
	[1] = {
		.start = 8888,
		.end =8888,
		.flags = IORESOURCE_IRQ,   //��һ���ж���Դ
	},
	[2] = {
		.start = 0xe0200160,
		.end = 0xe0200160 + 8 -1,
		.flags = IORESOURCE_MEM,   //�ڶ�����ַ��Դ
	},
};

void	led_plat_release(struct device *dev)
{
	printk("----------^_^ %s----------------\n",__FUNCTION__);
}


// 1������һ��pdev����
struct platform_device led_pdev = {
	.name = "s5pv210_led",
	.id = -1,
	.num_resources = ARRAY_SIZE(led_resource),
	.resource = led_resource,
	.dev.release = led_plat_release,
};

static int __init led_plat_dev_init(void)
{
	printk("----------^_^ %s----------------\n",__FUNCTION__);
	// 3,ע��pdev����
	return platform_device_register(&led_pdev);
}

static void __exit led_plat_dev_exit(void)
{
	printk("----------^_^ %s----------------\n",__FUNCTION__);
	//ע��pdev����
	platform_device_unregister(&led_pdev);
}

module_init(led_plat_dev_init);
module_exit(led_plat_dev_exit);
MODULE_LICENSE("GPL");
