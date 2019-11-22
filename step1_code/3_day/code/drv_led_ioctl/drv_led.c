#include <linux/init.h>
#include <linux/module.h>
#include <linux/gpio.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <asm/uaccess.h>
#include <asm-generic/ioctl.h>

#define ALL_LED_ON					_IO('A', 0x01)
#define ALL_LED_OFF					_IO('A', 0x02)
#define SEL_LED_ON					_IOW('S', 0x03, int)
#define SEL_LED_OFF					_IOW('S', 0x04, int)

#define SEL_LED1					0x01
#define SEL_LED2					0x02
#if 0
/* used to create numbers */
// type :填充到类型域中的内容,一般填充字母'A'~'Z'或'a'~'z'
// nr 	:具体的魔幻数
// size :填充的是携带的参数的类型
#define _IO(type,nr)		_IOC(_IOC_NONE,(type),(nr),0)
#define _IOR(type,nr,size)	_IOC(_IOC_READ,(type),(nr),(_IOC_TYPECHECK(size)))
#define _IOW(type,nr,size)	_IOC(_IOC_WRITE,(type),(nr),(_IOC_TYPECHECK(size)))
#define _IOWR(type,nr,size)	_IOC(_IOC_READ|_IOC_WRITE,(type),(nr),(_IOC_TYPECHECK(size)))
#endif

struct drv_led{
	unsigned int major;
	struct file_operations fops;
	struct class *cls;
	struct device *dev;
};

int drv_led_open (struct inode *inode, struct file *filep)
{
	return 0;
}

int drv_led_release (struct inode *inode, struct file *filep)
{
	return 0;
}

long drv_led_unlocked_ioctl (struct file *filep, unsigned int cmd, unsigned long args)
{
	switch (cmd)
	{
	case ALL_LED_ON:
		if (0 == gpio_request(S5PV210_GPC0(3), "led1"))
		{
			gpio_set_value(S5PV210_GPC0(3), 1);
			gpio_free(S5PV210_GPC0(3));
		}

		if (0 == gpio_request(S5PV210_GPC0(4), "led2"))
		{
			gpio_set_value(S5PV210_GPC0(4), 1);
			gpio_free(S5PV210_GPC0(4));
		}
		break;

	case ALL_LED_OFF:
		if (0 == gpio_request(S5PV210_GPC0(3), "led1"))
		{
			gpio_set_value(S5PV210_GPC0(3), 0);
			gpio_free(S5PV210_GPC0(3));
		}

		if (0 == gpio_request(S5PV210_GPC0(4), "led2"))
		{
			gpio_set_value(S5PV210_GPC0(4), 0);
			gpio_free(S5PV210_GPC0(4));
		}
		break;

	case SEL_LED_OFF:
		if (SEL_LED1 == args)
		{
			if (0 == gpio_request(S5PV210_GPC0(3), "led1"))
			{
				gpio_set_value(S5PV210_GPC0(3), 0);
				gpio_free(S5PV210_GPC0(3));
			}
		}
		else if (SEL_LED2 == args)
		{
			if (0 == gpio_request(S5PV210_GPC0(4), "led2"))
			{
				gpio_set_value(S5PV210_GPC0(4), 0);
				gpio_free(S5PV210_GPC0(4));
			}
		}
		break;
	
	case SEL_LED_ON:
		if (SEL_LED1 == args)
		{
			if (0 == gpio_request(S5PV210_GPC0(3), "led1"))
			{
				gpio_set_value(S5PV210_GPC0(3), 1);
				gpio_free(S5PV210_GPC0(3));
			}
		}
		else if (SEL_LED2 == args)
		{
			if (0 == gpio_request(S5PV210_GPC0(4), "led2"))
			{
				gpio_set_value(S5PV210_GPC0(4), 1);
				gpio_free(S5PV210_GPC0(4));
			}
		}
		break;
	
	default:
		printk("undefined cmd!\n");
		break;
	}

	return 0;
}

ssize_t drv_led_write (struct file *filep, const char __user *buff, size_t count, loff_t *offset)
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
		if (0 == gpio_request(S5PV210_GPD0(1), "led"))
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
		if (0 == gpio_request(S5PV210_GPD0(1), "led"))
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

struct drv_led *drv_led_dev;

static int __init drv_led_init(void)
{
	int ret;

	// 0\实例化对象
	// 参数1:分配空间的大小
	// 参数2:进行分配空间时如果没有足够的空间是否进行等待
	drv_led_dev = kzalloc(sizeof(struct drv_led), GFP_KERNEL);
	if (drv_led_dev == NULL)
	{
		printk("kzalloc error!\n");
		return -ENOMEM;
	}

	drv_led_dev->fops.owner = THIS_MODULE;
	drv_led_dev->fops.open = drv_led_open;
	drv_led_dev->fops.release = drv_led_release;
	drv_led_dev->fops.write = drv_led_write;
	drv_led_dev->fops.unlocked_ioctl = drv_led_unlocked_ioctl;
	
	// 1\注册/申请设备号
	ret = register_chrdev(0, "led", &drv_led_dev->fops);
	if (ret < 0)
	{
		printk("register_chrdev error!\n");
		goto register_chrdev_err;
	}

	drv_led_dev->major = ret;

	// 2\创建设备文件
	drv_led_dev->cls = class_create(THIS_MODULE, "led_cls");
	if (IS_ERR(drv_led_dev->cls))
	{
		printk("class_create error!\n");
		ret = PTR_ERR(drv_led_dev->cls);
		goto class_create_err;
	}

	drv_led_dev->dev = device_create(drv_led_dev->cls, NULL, MKDEV(drv_led_dev->major, 0), NULL, "led");
	if (IS_ERR(drv_led_dev->dev))
	{
		printk("device_create error!\n");
		ret = PTR_ERR(drv_led_dev->dev);
		goto device_create_err;
	}

	// 3\硬件初始化
	// 请求指定的GPIO口
	// 参数1:GPIO号码(该号码通过S5PV210_GPD0(_nr)转换得到,其中_nr填充的内容为具体的pin引脚)
	ret = gpio_request(S5PV210_GPC0(3), "led1");
	if (ret < 0)
	{
		printk("gpio_request error!\n");
		goto gpio_request_err;
	}
	
	// 设置指定的GPIO口为输出,并且通过参数value指定输出电平
	gpio_direction_output(S5PV210_GPC0(3), 0);		// 设置为输出同时输出低电平让led为非工作状态

	// 释放所请求的GPIO资源
	gpio_free(S5PV210_GPC0(3));

	ret = gpio_request(S5PV210_GPC0(4), "led2");
	if (ret < 0)
	{
		printk("gpio_request error!\n");
		goto gpio_request_err;
	}
	
	// 设置指定的GPIO口为输出,并且通过参数value指定输出电平
	gpio_direction_output(S5PV210_GPC0(4), 0);		// 设置为输出同时输出低电平让led为非工作状态

	// 释放所请求的GPIO资源
	gpio_free(S5PV210_GPC0(4));

	return 0;
gpio_request_err:
	device_destroy(drv_led_dev->cls, MKDEV(drv_led_dev->major, 0));
device_create_err:
	class_destroy(drv_led_dev->cls);
class_create_err:
	unregister_chrdev(drv_led_dev->major, "led");
register_chrdev_err:
	kfree(drv_led_dev);
	
	return ret;
}

static void __exit drv_led_exit(void)
{
	device_destroy(drv_led_dev->cls, MKDEV(drv_led_dev->major, 0));
	class_destroy(drv_led_dev->cls);
	unregister_chrdev(drv_led_dev->major, "led");
	kfree(drv_led_dev);
}

module_init(drv_led_init);
module_exit(drv_led_exit);

MODULE_LICENSE("GPL");
