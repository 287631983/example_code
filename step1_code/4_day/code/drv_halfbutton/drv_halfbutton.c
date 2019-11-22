#include <linux/init.h>
#include <linux/module.h>
#include <linux/gpio.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <asm/uaccess.h>
#include <asm-generic/ioctl.h>
#include <linux/cdev.h>
#include <linux/interrupt.h>
#include <linux/input.h>

//#define REGISTER_CHRDEV_ALLOC	

#define ALL_LED_ON					_IO('A', 0x01)
#define ALL_LED_OFF					_IO('A', 0x02)
#define SEL_LED_ON					_IOW('S', 0x03, int)
#define SEL_LED_OFF					_IOW('S', 0x04, int)

#define SEL_LED1					0x01
#define SEL_LED2					0x02

// 描述传递给应用程序的结构体
struct key_board_data{
	unsigned char button_press;
	unsigned int code_value;
};

// 描述按键信息的结构体
struct button_attribute{
	unsigned int irq;
	unsigned long flags;
	char * name;
	unsigned int gpio;
	unsigned int code_value;
};

struct button_attribute key_board[] = {
	[0] = {
		.irq = IRQ_EINT(0),
		.flags = IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING,
		.name = "key_up",
		.gpio = S5PV210_GPH0(0),
		.code_value = KEY_UP,
	},
	[1] = {
		.irq = IRQ_EINT(1),
		.flags = IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING,
		.name = "key_down",
		.gpio = S5PV210_GPH0(1),
		.code_value = KEY_DOWN,
	},
	[2] = {
		.irq = IRQ_EINT(2),
		.flags = IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING,
		.name = "key_left",
		.gpio = S5PV210_GPH0(2),
		.code_value = KEY_LEFT,
	},
	[3] = {
		.irq = IRQ_EINT(3),
		.flags = IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING,
		.name = "key_right",
		.gpio = S5PV210_GPH0(3),
		.code_value = KEY_RIGHT,
	},
};

struct drv_button_irq{
	dev_t dev_no;
	struct file_operations fops;
	struct class *cls;
	struct device *dev;
	struct cdev *cdev;
	struct key_board_data key_board_data;
	struct tasklet_struct tasklet;
	struct work_struct work;
};

struct drv_button_irq *drv_button_irq_dev;

int drv_button_irq_open (struct inode *inode, struct file *filep)
{
	return 0;
}

int drv_button_irq_release (struct inode *inode, struct file *filep)
{
	return 0;
}

long drv_button_irq_unlocked_ioctl (struct file *filep, unsigned int cmd, unsigned long args)
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

ssize_t drv_button_irq_write (struct file *filep, const char __user *buff, size_t count, loff_t *offset)
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

ssize_t drv_button_irq_read (struct file *filep, char __user *buff, size_t count, loff_t *offset)
{
	ssize_t ret;

	ret = copy_to_user(buff, &drv_button_irq_dev->key_board_data, count);
	if (ret > 0)
	{
		printk("copy_to_user error!\n");

		return ret;
	}

	return count;
}

irqreturn_t drv_button_irq_handler_t(int no, void *data)
{
	struct button_attribute *key_board = (struct button_attribute *)data;

	drv_button_irq_dev->key_board_data.code_value = key_board->code_value;
	// 约定好,传递的button_press为1表示按键按下,为0表示按键释放
	drv_button_irq_dev->key_board_data.button_press = (!gpio_get_value(key_board->gpio));

	printk("-------%s-------\n", __FUNCTION__);

	// 3\调度tasklet(此处的调度并不等于执行)
	tasklet_schedule(&drv_button_irq_dev->tasklet);
	// 3\调度工作队列
	schedule_work(&drv_button_irq_dev->work);
#if 0
	switch (key_board->code_value)
	{
	case KEY_UP:
		if (gpio_get_value(key_board->gpio))
		{
			printk("KEY_UP release!\n");
		}
		else
		{
			printk("KEY_UP press!\n");
		}
		break;

	case KEY_DOWN:
		if (gpio_get_value(key_board->gpio))
		{
			printk("KEY_DOWN release!\n");
		}
		else
		{
			printk("KEY_DOWN press!\n");
		}
		break;
	
	case KEY_LEFT:
		if (gpio_get_value(key_board->gpio))
		{
			printk("KEY_LEFT release!\n");
		}
		else
		{
			printk("KEY_LEFT press!\n");
		}
		break;
	
	case KEY_RIGHT:
		if (gpio_get_value(key_board->gpio))
		{
			printk("KEY_RIGHT release!\n");
		}
		else
		{
			printk("KEY_RIGHT press!\n");
		}
		break;
	}
#endif	
	return IRQ_HANDLED;
}

void tasklet_func (unsigned long data)
{
	printk("-------%s-------\n", __FUNCTION__);
}

void work_func (struct work_struct *work)
{
	printk("-------%s-------\n", __FUNCTION__);
}

static int __init drv_button_irq_init(void)
{
	int ret;
	unsigned int i;

	// 0\实例化对象
	// 参数1:分配空间的大小
	// 参数2:进行分配空间时如果没有足够的空间是否进行等待
	drv_button_irq_dev = kzalloc(sizeof(struct drv_button_irq), GFP_KERNEL);
	if (drv_button_irq_dev == NULL)
	{
		printk("kzalloc error!\n");
		return -ENOMEM;
	}

	drv_button_irq_dev->fops.owner = THIS_MODULE;
	drv_button_irq_dev->fops.open = drv_button_irq_open;
	drv_button_irq_dev->fops.release = drv_button_irq_release;
	drv_button_irq_dev->fops.write = drv_button_irq_write;
	drv_button_irq_dev->fops.unlocked_ioctl = drv_button_irq_unlocked_ioctl;
	drv_button_irq_dev->fops.read = drv_button_irq_read;
	
	// 1\注册/申请设备号
	
#ifndef REGISTER_CHRDEV_ALLOC
	// 静态注册设备号
	// 参数1:指定要注册的设备号
	// 参数2:要注册的个数
	// 参数3:要注册的设备号的描述字符串
	// 返回值:返回0表示注册成功,返回负数表示失败
	drv_button_irq_dev->dev_no = MKDEV(250, 0);
	ret = register_chrdev_region(drv_button_irq_dev->dev_no, 1, "button");
	if (ret < 0)
	{
		printk("register_chrdev_region error!\n");
		goto register_chrdev_region_err;
	}
#else
	// 动态注册设备号
	// 参数1:动态注册如果注册成功,返回注册到的设备号
	// 参数2:次设备号的起始
	// 参数3:指定要注册设备号的个数
	// 参数4:要注册的设备号的描述字符串
	// 返回值:返回0表示注册成功,返回负数表示失败
	ret = alloc_chrdev_region(&drv_button_irq_dev->dev_no, 0, 1, "button");
	if (ret < 0)
	{
		printk("alloc_chrdev_region error!\n");
		goto register_chrdev_region_err;
	}

#endif

	// 申请一个cdev对象
	drv_button_irq_dev->cdev = cdev_alloc();
	if (IS_ERR(drv_button_irq_dev->cdev))
	{
		ret = PTR_ERR(drv_button_irq_dev->cdev);
		goto cdev_alloc_err;
	}
	// cdev初始化
	// 参数1:欲初始化的cdev
	// 参数2:当前设备驱动的设备文件操作接口集合
	cdev_init(drv_button_irq_dev->cdev, &drv_button_irq_dev->fops);
	// cdev注册到内核中
	// 参数1:欲注册到内核的cdev
	// 参数2:设备号
	// 参数3:注册到内核的个数
	cdev_add(drv_button_irq_dev->cdev, drv_button_irq_dev->dev_no, 1);

	// 2\创建设备文件
	drv_button_irq_dev->cls = class_create(THIS_MODULE, "button_cls");
	if (IS_ERR(drv_button_irq_dev->cls))
	{
		printk("class_create error!\n");
		ret = PTR_ERR(drv_button_irq_dev->cls);
		goto class_create_err;
	}

	drv_button_irq_dev->dev = device_create(drv_button_irq_dev->cls, NULL, drv_button_irq_dev->dev_no, NULL, "button");
	if (IS_ERR(drv_button_irq_dev->dev))
	{
		printk("device_create error!\n");
		ret = PTR_ERR(drv_button_irq_dev->dev);
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

	// 请求中断
	// 参数1:中断通道号(该值通过IRQ_EINT(x)转换获得,其中x的值为具体的vic的通道对应的NO值)
	// 参数2:中断处理函数(如果配置的中断源触发了该中断且得到响应,那么该函数将会被调用)
	// 参数3:触发条件
	// 参数4:该中断源的描述字符串
	// 参数5:给中断处理函数传递的私有数据
	// 返回值:0表示请求成功,负数表示请求失败
	for (i=0; i<ARRAY_SIZE(key_board); i++)
	{
		ret = request_irq(key_board[i].irq, drv_button_irq_handler_t, key_board[i].flags, key_board[i].name, &key_board[i]);
		if (ret < 0)
		{
			printk("request_irq error!\n");
			goto request_irq_err;
		}
	}

	// 1\初始化tasklet
	// 参数1:待初始化的tasklet对象
	// 参数2:tasklet对应的下半部处理函数
	// 参数3:传递给tasklet下半部处理函数的参量
	tasklet_init(&drv_button_irq_dev->tasklet, tasklet_func, 0);

	// 1\初始化工作队列
	// 参数1:待初始化的工作队列对象
	// 参数2:工作队列下半部处理函数
	INIT_WORK(&drv_button_irq_dev->work, work_func);

	

	
	return 0;

request_irq_err:
	for (; i>0; i--)
	{
		free_irq(key_board[i-1].irq, &key_board[i-1]);
	}
gpio_request_err:
	device_destroy(drv_button_irq_dev->cls, drv_button_irq_dev->dev_no);
device_create_err:
	class_destroy(drv_button_irq_dev->cls);
class_create_err:
	cdev_del(drv_button_irq_dev->cdev);
cdev_alloc_err:
	unregister_chrdev_region(drv_button_irq_dev->dev_no, 1);
register_chrdev_region_err:
	kfree(drv_button_irq_dev);
	
	return ret;
}

static void __exit drv_button_irq_exit(void)
{
	unsigned int i;
	
	for (i=0; i<ARRAY_SIZE(key_board); i++)
	{
		free_irq(key_board[i].irq, &key_board[i]);
	}
	device_destroy(drv_button_irq_dev->cls, drv_button_irq_dev->dev_no);
	class_destroy(drv_button_irq_dev->cls);
	cdev_del(drv_button_irq_dev->cdev);
	unregister_chrdev_region(drv_button_irq_dev->dev_no, 1);
	kfree(drv_button_irq_dev);
}

module_init(drv_button_irq_init);
module_exit(drv_button_irq_exit);

MODULE_LICENSE("GPL");
