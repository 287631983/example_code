#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>

#include <asm/io.h>

volatile unsigned long *GPC0_CON;
volatile unsigned long *GPC0_DAT;

unsigned int led_major;
struct class *led_cls;
struct device *led_dev;

int led_open (struct inode *inode, struct file *filep)
{
    printk(KERN_INFO"---------%s---------\n", __FUNCTION__);

	*GPC0_DAT |= (0x03<<3);

	return 0;
}

int led_release (struct inode *inode, struct file *filep)
{
    printk(KERN_INFO"---------%s---------\n", __FUNCTION__);
	
	*GPC0_DAT &= ~(0x03<<3);
	
	return 0;
}

const struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = led_open,
	.release = led_release,
};

// static修饰,限定了函数的作用域
// int 返回值的类型为int型
// __init 用__init修饰的函数最终会被编译器编译链接到特定的代码段(init.txt),
//		同时在(.initcall.init)代码段中会保留有一份指向该模块加载函数的指针,
//		所有用__init修饰的模块加载函数统一放置,最终在(.initcall.init)
//		代码段中形成函数指针列表,那么在内核启动之后,通过遍历函数指针列表,最终实现模块的加载函数的调用
static int __init drv_led_init(void)
{
	int ret = 0;
	
	// printk是有输出等级控制的,只有比系统设置的输出等级高的打印信息才会被输出
	// printk当前打印等级查看  通过查看/proc/sys/kernel/printk的文件信息获得
    printk(KERN_INFO"---------%s---------\n", __FUNCTION__);

	// 1\设备号的注册
	// 设备号32bits(主设备号12bits+次设备号20bits)
	// 主设备号:可以表示物体的种类								eg:摄像头
	// 次设备号:可以表示同类物体中的具体某一个							eg:前置   后置
	
	// 注册主设备号
	// 参数1:主设备号,如果major填充0表示由系统分配,返回的就是分配到的主设备号,如果是填充具体指,那么表示指定注册该主设备号,返回0表示成功,返回负数表示失败
	// 参数2:注册主设备号对应的描述字符串
	// 参数3:设备文件的操作接口集合
	led_major = 250;
	ret = register_chrdev(led_major, "LED", &fops);
	if (ret < 0)
	{
		printk(KERN_ERR"register_chrdev error!\n");

		return -EINVAL;
	}
	
	// 2\创建设备文件
	// 两种方式:1\手动创建  2\自动创建
	// 手动创建:
	//			mknod /dev/led c 250 0

	// 自动创建设备文件
	// 创建设备类
	// 参数1:类的所属者---THIS_MODULE
	// 参数2:创建的类的描述字符串
	// 返回值:返回创建的类
	led_cls = class_create(THIS_MODULE, "led_cls");
	if (IS_ERR(led_cls))
	{
		printk(KERN_ERR"class_create error!\n");
		
		ret = PTR_ERR(led_cls);
		goto class_create_err;
	}
	
	// 创建设备文件
	// 参数1:设备类
	// 参数2:设备的父类,如果没有填NULL
	// 参数3:设备号(主+次)
	// 参数4:设备文件的私有数据,没有就填NULL
	// 参数5:设备文件名,如果填充的内容为"led",最终在/dev/路径下将会看到的设备文件名即为/dev/led
	// 返回值:返回创建的设备文件指针
	led_dev = device_create(led_cls, NULL, MKDEV(led_major, 0), NULL, "led");
	if (IS_ERR(led_dev))
	{
		printk(KERN_ERR"device_create error!\n");
		
		ret = PTR_ERR(led_dev);
		goto device_create_err;
	}

	// 3\硬件的初始化
	// 参数1:要映射的物理地址的起始
	// 参数2:要映射的大小
	// 返回值:返回映射生成的虚拟地址的起始
	GPC0_CON = ioremap(0xe0200060, 8);
	GPC0_DAT = GPC0_CON + 1;

	*GPC0_CON &= ~(0xff<<12);
	*GPC0_CON |= (0x11<<12);

	// 让LED设备处于非工作状态
	*GPC0_DAT &= ~(0x03<<3);
	
    return 0;
	
device_create_err:
	class_destroy(led_cls);
class_create_err:
	unregister_chrdev(led_major, "LED");
	
	return ret;
}

static void __exit drv_led_exit(void)
{
    printk("---------%s---------\n", __FUNCTION__);
	iounmap(GPC0_CON);
	device_del(led_dev);
	class_destroy(led_cls);
	unregister_chrdev(led_major, "LED");
}

module_init(drv_led_init);
module_exit(drv_led_exit);
// 添加GPL认证
MODULE_LICENSE("GPL");
// 模块的别名
MODULE_ALIAS("module_demo");
// 模块的作者
MODULE_AUTHOR("farsight");
// 模块的描述
MODULE_DESCRIPTION("This is a module demo");
// 模块的版本
MODULE_VERSION("V0.0.0");

