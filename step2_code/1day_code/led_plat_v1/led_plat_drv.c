#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>

#include <asm/io.h>

unsigned long *gpc0_conf;
unsigned long *gpc0_data;

//2,实现probe
int led_plat_drv_probe(struct platform_device * pdev )
{
	struct resource *addr_res1,*addr_res2;
	struct resource *irq_res;
	int irqno;

	printk("----------^_^ %s----------------\n",__FUNCTION__);
	/*
		1,实例化全局对象
		2,申请设备号
		3,创建类和设备节点
		4,硬件初始化-----获取pdev中的资源
		5,实现fops中的接口
	*/

	//获取pdev中的资源
	//参数1 --- pdev对象
	//参数2 -- 　资源的类型
	//参数3 --  资源的编号,编号从0开始
	addr_res1 = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	gpc0_conf = ioremap(addr_res1->start, resource_size(addr_res1));
	gpc0_data = gpc0_conf + 1;

	//配置gpc0为输出
	*gpc0_conf &= ~(0xff<<12);
	*gpc0_conf |= 0x11<<12;

	*gpc0_data |= 0x3 << 3;

	addr_res2 = platform_get_resource(pdev, IORESOURCE_MEM, 1);
	printk("addr_res2->start = 0x%x\n",addr_res2->start);


	//获取中断资源
	irq_res = platform_get_resource(pdev, IORESOURCE_IRQ,0);  //方法一
	printk("irq_res.start = %d\n",irq_res->start);

	irqno = platform_get_irq(pdev, 0);  //方法二
	printk("irqno = %d\n",irqno);

	return 0;
	
}
int led_plat_drv_remove(struct platform_device * pdev)
{
	printk("----------^_^ %s----------------\n",__FUNCTION__);
	iounmap(gpc0_conf);
	return 0;
}

const struct platform_device_id led_id_table[] = {
	{"s5pv210_led",0x1234},
	{"s3c2410_led",0x1122},
	{"s3c6410_led",0x3344},
};


// 1，创建pdrv 
struct platform_driver led_pdrv = {
	.probe = led_plat_drv_probe,
	.remove = led_plat_drv_remove,
	.driver = {
		.name = "samsung led_drv", //自定义，必须要写,匹配成功时，会在:ls /sys/bus/platform/drivers 看到
	},
	.id_table = led_id_table,
};

static int __init led_plat_drv_init(void)
{
	printk("----------^_^ %s----------------\n",__FUNCTION__);
	// 3,注册pdrv对象
	return platform_driver_register(&led_pdrv);
}

static void __exit led_plat_drv_exit(void)
{
	printk("----------^_^ %s----------------\n",__FUNCTION__);
	//注销pdrv对象
	platform_driver_unregister(&led_pdrv);
}

module_init(led_plat_drv_init);
module_exit(led_plat_drv_exit);
MODULE_LICENSE("GPL");


