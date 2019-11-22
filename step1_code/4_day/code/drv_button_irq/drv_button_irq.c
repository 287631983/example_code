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

//#define REGISTER_CHRDEV_ALLOC	

#define ALL_LED_ON					_IO('A', 0x01)
#define ALL_LED_OFF					_IO('A', 0x02)
#define SEL_LED_ON					_IOW('S', 0x03, int)
#define SEL_LED_OFF					_IOW('S', 0x04, int)

#define SEL_LED1					0x01
#define SEL_LED2					0x02

// ����������Ϣ�Ľṹ��
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
		.flags = IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING;
		.name = "key_up",
		.gpio = S5PV210_GPH0(0),
		.code_value = KEY_UP,
	},
	[1] = {
		.irq = IRQ_EINT(1),
		.flags = IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING;
		.name = "key_down",
		.gpio = S5PV210_GPH0(1),
		.code_value = KEY_DOWN,
	},
	[2] = {
		.irq = IRQ_EINT(2),
		.flags = IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING;
		.name = "key_left",
		.gpio = S5PV210_GPH0(2),
		.code_value = KEY_LEFT,
	},
	[3] = {
		.irq = IRQ_EINT(3),
		.flags = IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING;
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
};

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
	
	// 1\��ȡӦ�ÿռ䴫�ݽ�����������Ϣ
	// ���û��ռ俽�����ݵ��ں˿ռ�
	// ����1:�ں˿ռ��buff��ַ
	// ����2:�û��ռ��buff��ַ
	// ����3:ָ���������ݵĸ���
	// ����ֵ:����0��ʾ���ݿ����ɹ�,������������쳣,���ص���δ�ɹ������ĸ���
	ret = copy_from_user(&ctrl_cmd, buff, count);
	if (ret > 0)
	{
		printk("copy_from_user error!\n");
		return ret;
	}

	// 2\����Ӧ�ÿռ䴫�ݽ�����������Ϣ����ִ�ж�Ӧ�Ĳ���
	// Լ�����ctrl_cmd��ȡ��������Ϊ0x01��ʾҪ�򿪷�����,����رշ�����
	if (0x01 == ctrl_cmd)
	{
		// �򿪷�����
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
		// �رշ�����
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

irqreturn_t drv_button_irq_handler_t(int no, void *data)
{
	printk("-----%s------\n", __FUNCTION__);

	return IRQ_HANDLED;
}

struct drv_button_irq *drv_button_irq_dev;

static int __init drv_button_irq_init(void)
{
	int ret;

	// 0\ʵ��������
	// ����1:����ռ�Ĵ�С
	// ����2:���з���ռ�ʱ���û���㹻�Ŀռ��Ƿ���еȴ�
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
	
	// 1\ע��/�����豸��
	
#ifndef REGISTER_CHRDEV_ALLOC
	// ��̬ע���豸��
	// ����1:ָ��Ҫע����豸��
	// ����2:Ҫע��ĸ���
	// ����3:Ҫע����豸�ŵ������ַ���
	// ����ֵ:����0��ʾע��ɹ�,���ظ�����ʾʧ��
	drv_button_irq_dev->dev_no = MKDEV(250, 0);
	ret = register_chrdev_region(drv_button_irq_dev->dev_no, 1, "button");
	if (ret < 0)
	{
		printk("register_chrdev_region error!\n");
		goto register_chrdev_region_err;
	}
#else
	// ��̬ע���豸��
	// ����1:��̬ע�����ע��ɹ�,����ע�ᵽ���豸��
	// ����2:���豸�ŵ���ʼ
	// ����3:ָ��Ҫע���豸�ŵĸ���
	// ����4:Ҫע����豸�ŵ������ַ���
	// ����ֵ:����0��ʾע��ɹ�,���ظ�����ʾʧ��
	ret = alloc_chrdev_region(&drv_button_irq_dev->dev_no, 0, 1, "button");
	if (ret < 0)
	{
		printk("alloc_chrdev_region error!\n");
		goto register_chrdev_region_err;
	}

#endif

	// ����һ��cdev����
	drv_button_irq_dev->cdev = cdev_alloc();
	if (IS_ERR(drv_button_irq_dev->cdev))
	{
		ret = PTR_ERR(drv_button_irq_dev->cdev);
		goto cdev_alloc_err;
	}
	// cdev��ʼ��
	// ����1:����ʼ����cdev
	// ����2:��ǰ�豸�������豸�ļ������ӿڼ���
	cdev_init(drv_button_irq_dev->cdev, &drv_button_irq_dev->fops);
	// cdevע�ᵽ�ں���
	// ����1:��ע�ᵽ�ں˵�cdev
	// ����2:�豸��
	// ����3:ע�ᵽ�ں˵ĸ���
	cdev_add(drv_button_irq_dev->cdev, drv_button_irq_dev->dev_no, 1);

	// 2\�����豸�ļ�
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

	// 3\Ӳ����ʼ��
	// ����ָ����GPIO��
	// ����1:GPIO����(�ú���ͨ��S5PV210_GPD0(_nr)ת���õ�,����_nr��������Ϊ�����pin����)
	ret = gpio_request(S5PV210_GPC0(3), "led1");
	if (ret < 0)
	{
		printk("gpio_request error!\n");
		goto gpio_request_err;
	}
	
	// ����ָ����GPIO��Ϊ���,����ͨ������valueָ�������ƽ
	gpio_direction_output(S5PV210_GPC0(3), 0);		// ����Ϊ���ͬʱ����͵�ƽ��ledΪ�ǹ���״̬

	// �ͷ��������GPIO��Դ
	gpio_free(S5PV210_GPC0(3));

	ret = gpio_request(S5PV210_GPC0(4), "led2");
	if (ret < 0)
	{
		printk("gpio_request error!\n");
		goto gpio_request_err;
	}
	
	// ����ָ����GPIO��Ϊ���,����ͨ������valueָ�������ƽ
	gpio_direction_output(S5PV210_GPC0(4), 0);		// ����Ϊ���ͬʱ����͵�ƽ��ledΪ�ǹ���״̬

	// �ͷ��������GPIO��Դ
	gpio_free(S5PV210_GPC0(4));

	// �����ж�
	// ����1:�ж�ͨ����(��ֵͨ��IRQ_EINT(x)ת�����,����x��ֵΪ�����vic��ͨ����Ӧ��NOֵ)
	// ����2:�жϴ�������(������õ��ж�Դ�����˸��ж��ҵõ���Ӧ,��ô�ú������ᱻ����)
	// ����3:��������
	// ����4:���ж�Դ�������ַ���
	// ����5:���жϴ����������ݵ�˽������
	// ����ֵ:0��ʾ����ɹ�,������ʾ����ʧ��
	ret = request_irq(IRQ_EINT(0), drv_button_irq_handler_t, IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING, "key1_irq", NULL);
	if (ret < 0)
	{
		printk("request_irq error!\n");
		goto request_irq_err;
	}
	return 0;

request_irq_err:
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
	free_irq(IRQ_EINT(0), NULL);
	device_destroy(drv_button_irq_dev->cls, drv_button_irq_dev->dev_no);
	class_destroy(drv_button_irq_dev->cls);
	cdev_del(drv_button_irq_dev->cdev);
	unregister_chrdev_region(drv_button_irq_dev->dev_no, 1);
	kfree(drv_button_irq_dev);
}

module_init(drv_button_irq_init);
module_exit(drv_button_irq_exit);

MODULE_LICENSE("GPL");