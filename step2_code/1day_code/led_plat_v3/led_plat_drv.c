#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/device.h>


#include <asm/io.h>
#include <asm/uaccess.h>

#include "led_info.h"


struct s5pv210_led *led_dev;
unsigned value;
int led_plat_open(struct inode *inode, struct file *filp)
{
	int shift;
	int conf_reg_data;
	int conf_reg_clear;
	printk("----------^_^ %s----------------\n",__FUNCTION__);
	//配置gpc0为输出 
	//*gpc0_conf &= ~(0xff<<12);
	//*gpc0_conf |= 0x11<<12;

	shift = led_dev->pdata->shift*4;
	conf_reg_data = led_dev->pdata->conf_reg_data;
	conf_reg_clear = led_dev->pdata->conf_reg_clear;

	value = __raw_readl(led_dev->reg_base);
	value  &= ~(conf_reg_clear<<shift);
	value  |= conf_reg_data<<shift;
	__raw_writel(value, led_dev->reg_base);
	return 0;
}
ssize_t led_plat_write(struct file *filp, const char __user *buf, size_t size , loff_t * flags)
{
	unsigned int value,vl;
	int data_reg,shift;
	printk("----------^_^ %s----------------\n",__FUNCTION__);

	if(copy_from_user(&value, buf, size) > 0){
		printk(" copy_from_user error!\n");
		return -EINVAL;
	}

	data_reg = led_dev->pdata->data_reg;
	shift = led_dev->pdata->shift;

	if(value){
		//点灯
		//*gpc0_data |= 0x3 << 3;
		vl = readl(led_dev->reg_base+GPC0_DATA);
		vl |= data_reg << shift;
		writel(vl, led_dev->reg_base+GPC0_DATA);
	}else{
		//灭灯
		//*gpc0_data &= ~(0x3 << 3);
#if 0
		vl = readl(gpc0_data);
		vl &= ~(0x3 << 3);
		writel(vl, gpc0_data);
#else
		writel(readl(led_dev->reg_base+GPC0_DATA) & ~(data_reg<<shift), led_dev->reg_base+GPC0_DATA);
#endif
	}

	return size;
}

int led_plat_close(struct inode *inode, struct file *filp)
{
	int data_reg,shift;
	printk("----------^_^ %s----------------\n",__FUNCTION__);

	
	data_reg = led_dev->pdata->data_reg;
	shift = led_dev->pdata->shift;
	
	//*gpc0_data &= ~(0x3 << 3);
	value = readl(led_dev->reg_base+GPC0_DATA);
	value &= ~(data_reg << shift);
	writel(value, led_dev->reg_base+GPC0_DATA);
	return 0;
}



const struct file_operations  led_fops = {
	.open = led_plat_open,
	.write = led_plat_write,
	.release = led_plat_close,
};


//2,实现probe
int led_plat_drv_probe(struct platform_device * pdev )
{
	struct resource *addr_res1,*addr_res2;
	struct resource *irq_res;
	int irqno;
	int ret;

	printk("----------^_^ %s----------------\n",__FUNCTION__);
	/*
		1,实例化全局对象
		2,申请设备号
		3,创建类和设备节点
		4,硬件初始化-----获取pdev中的资源
		5,实现fops中的接口
	*/

	// 1, 实例化全局对象
	led_dev = kzalloc(sizeof(struct s5pv210_led), GFP_KERNEL);
	if(IS_ERR(led_dev)){
		printk(" kzalloc error!\n");
		return PTR_ERR(led_dev);
	}

	// 2,申请设备号
	led_dev->major = register_chrdev(0, "led_drv", &led_fops);
	if(led_dev->major < 0){
		printk(" register_chrdev error!\n");
		ret = led_dev->major;
		goto err_kfree;
	}

	//3,创建类和设备节点
	led_dev->clz = class_create(THIS_MODULE, "led_class");
	if(IS_ERR(led_dev->clz)){
		printk(" kzalloc error!\n");
		ret =  PTR_ERR(led_dev->clz);
		goto err_unregister;
	}

	led_dev->dev = device_create(led_dev->clz, NULL, MKDEV(led_dev->major, 5), NULL, "led01");
	if(IS_ERR(led_dev->dev)){
		printk(" kzalloc error!\n");
		ret =  PTR_ERR(led_dev->dev);
		goto err_class;
	}

	//4,硬件初始化-----获取pdev中的资源

	//获取平台自定义数据
	led_dev->pdata = pdev->dev.platform_data;
	
	//获取pdev中的资源
	//参数1 --- pdev对象
	//参数2 -- 　资源的类型
	//参数3 --  资源的编号,编号从0开始
	addr_res1 = platform_get_resource(pdev, IORESOURCE_MEM, 0);

	//硬件初始化
	led_dev->reg_base = ioremap(addr_res1->start, resource_size(addr_res1));


	

	addr_res2 = platform_get_resource(pdev, IORESOURCE_MEM, 1);
	printk("addr_res2->start = 0x%x\n",addr_res2->start);


	//获取中断资源
	irq_res = platform_get_resource(pdev, IORESOURCE_IRQ,0);  //方法一
	printk("irq_res.start = %d\n",irq_res->start);

	irqno = platform_get_irq(pdev, 0);  //方法二
	printk("irqno = %d\n",irqno);

	return 0;
	
err_class:
	class_destroy(led_dev->clz);
err_unregister:
	unregister_chrdev(led_dev->major, "led_drv");
err_kfree:
	kfree(led_dev);
	return ret;

	
}
int led_plat_drv_remove(struct platform_device * pdev)
{
	printk("----------^_^ %s----------------\n",__FUNCTION__);
	iounmap(led_dev->reg_base);
	device_destroy(led_dev->clz, MKDEV(led_dev->major, 5));
	class_destroy(led_dev->clz);
	unregister_chrdev(led_dev->major, "led_drv");
	kfree(led_dev);
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


