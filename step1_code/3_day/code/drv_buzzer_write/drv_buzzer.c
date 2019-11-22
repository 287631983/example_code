#include <linux/init.h>
#include <linux/module.h>
#include <linux/gpio.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <asm/uaccess.h>

struct drv_buzzer{
	unsigned int major;
	struct file_operations fops;
	struct class *cls;
	struct device *dev;
};

int drv_buzzer_open (struct inode *inode, struct file *filep)
{
	return 0;
}

int drv_buzzer_release (struct inode *inode, struct file *filep)
{
	return 0;
}

ssize_t drv_buzzer_write (struct file *filep, const char __user *buff, size_t count, loff_t *offset)
{
	ssize_t ret;
	int ctrl_cmd;
	
	// 1\获取应用空间传递进来的数据信息
	// 从用户空间拷贝数据到内核空间
	// 参数1:内核空间的buff地址
	// 参数2:用户空间的buff地址
	// 参数3:指定拷贝数据的个数
	// 返回值:返回0表示数据拷贝成功,如果拷贝出现异常,返回的是未成功拷贝的个数
	ret = copy_from_user(&ctrl_cmd, buff, count);
	if (ret > 0)
	{
		printk("copy_from_user error!\n");
		return ret;
	}

	// 2\解析应用空间传递进来的数据信息并且执行对应的操作
	// 约定如果ctrl_cmd获取到的内容为0x01表示要打开蜂鸣器,否则关闭蜂鸣器
	if (0x01 == ctrl_cmd)
	{
		// 打开蜂鸣器
		if (0 == gpio_request(S5PV210_GPD0(1), "buzzer"))
		{
			gpio_set_value(S5PV210_GPD0(1), 1);
			gpio_free(S5PV210_GPD0(1));
		}
		else
		{
			printk("gpio_request busy!\n");
		}
	}
	else
	{
		// 关闭蜂鸣器
		if (0 == gpio_request(S5PV210_GPD0(1), "buzzer"))
		{
			gpio_set_value(S5PV210_GPD0(1), 0);
			gpio_free(S5PV210_GPD0(1));
		}
		else
		{
			printk("gpio_request busy!\n");
		}
	}

	return count;
}

struct drv_buzzer *drv_buzzer_dev;

static int __init drv_buzzer_init(void)
{
	int ret;

	// 0\实例化对象
	// 参数1:分配空间的大小
	// 参数2:进行分配空间时如果没有足够的空间是否进行等待
	drv_buzzer_dev = kzalloc(sizeof(struct drv_buzzer), GFP_KERNEL);
	if (drv_buzzer_dev == NULL)
	{
		printk("kzalloc error!\n");
		return -ENOMEM;
	}

	drv_buzzer_dev->fops.owner = THIS_MODULE;
	drv_buzzer_dev->fops.open = drv_buzzer_open;
	drv_buzzer_dev->fops.release = drv_buzzer_release;
	drv_buzzer_dev->fops.write = drv_buzzer_write;
	
	// 1\注册/申请设备号
	ret = register_chrdev(0, "buzzer", &drv_buzzer_dev->fops);
	if (ret < 0)
	{
		printk("register_chrdev error!\n");
		goto register_chrdev_err;
	}

	drv_buzzer_dev->major = ret;

	// 2\创建设备文件
	drv_buzzer_dev->cls = class_create(THIS_MODULE, "buzeer_cls");
	if (IS_ERR(drv_buzzer_dev->cls))
	{
		printk("class_create error!\n");
		ret = PTR_ERR(drv_buzzer_dev->cls);
		goto class_create_err;
	}

	drv_buzzer_dev->dev = device_create(drv_buzzer_dev->cls, NULL, MKDEV(drv_buzzer_dev->major, 0), NULL, "buzzer");
	if (IS_ERR(drv_buzzer_dev->dev))
	{
		printk("device_create error!\n");
		ret = PTR_ERR(drv_buzzer_dev->dev);
		goto device_create_err;
	}

	// 3\硬件初始化
	// 请求指定的GPIO口
	// 参数1:GPIO号码(该号码通过S5PV210_GPD0(_nr)转换得到,其中_nr填充的内容为具体的pin引脚)
	ret = gpio_request(S5PV210_GPD0(1), "buzzer");
	if (ret < 0)
	{
		printk("gpio_request error!\n");
		goto gpio_request_err;
	}
	
	// 设置指定的GPIO口为输出,并且通过参数value指定输出电平
	gpio_direction_output(S5PV210_GPD0(1), 0);		// 设置为输出同时输出低电平让buzzer为非工作状态

	// 释放所请求的GPIO资源
	gpio_free(S5PV210_GPD0(1));

	return 0;
gpio_request_err:
	device_destroy(drv_buzzer_dev->cls, MKDEV(drv_buzzer_dev->major, 0));
device_create_err:
	class_destroy(drv_buzzer_dev->cls);
class_create_err:
	unregister_chrdev(drv_buzzer_dev->major, "buzzer");
register_chrdev_err:
	kfree(drv_buzzer_dev);
	
	return ret;
}

static void __exit drv_buzzer_exit(void)
{
	device_destroy(drv_buzzer_dev->cls, MKDEV(drv_buzzer_dev->major, 0));
	class_destroy(drv_buzzer_dev->cls);
	unregister_chrdev(drv_buzzer_dev->major, "buzzer");
	kfree(drv_buzzer_dev);
}

module_init(drv_buzzer_init);
module_exit(drv_buzzer_exit);

MODULE_LICENSE("GPL");
