#include <linux/init.h>
#include <linux/module.h>
#include <linux/gpio.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <asm/uaccess.h>
#include <asm-generic/ioctl.h>
#include <linux/cdev.h>

//#define REGISTER_CHRDEV_ALLOC	

#define ALL_LED_ON					_IO('A', 0x01)
#define ALL_LED_OFF					_IO('A', 0x02)
#define SEL_LED_ON					_IOW('S', 0x03, int)
#define SEL_LED_OFF					_IOW('S', 0x04, int)

#define SEL_LED1					0x01
#define SEL_LED2					0x02

struct drv_led{
	dev_t dev_no;
	struct file_operations fops;
	struct class *cls;
	struct device *dev;
	struct cdev *cdev;
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

struct drv_led *drv_led_dev;

static int __init drv_led_init(void)
{
	int ret;

	// 0\ʵ��������
	// ����1:����ռ�Ĵ�С
	// ����2:���з���ռ�ʱ���û���㹻�Ŀռ��Ƿ���еȴ�
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
	
	// 1\ע��/�����豸��
	
#ifndef REGISTER_CHRDEV_ALLOC
	// ��̬ע���豸��
	// ����1:ָ��Ҫע����豸��
	// ����2:Ҫע��ĸ���
	// ����3:Ҫע����豸�ŵ������ַ���
	// ����ֵ:����0��ʾע��ɹ�,���ظ�����ʾʧ��
	drv_led_dev->dev_no = MKDEV(250, 0);
	ret = register_chrdev_region(drv_led_dev->dev_no, 1, "led");
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
	ret = alloc_chrdev_region(&drv_led_dev->dev_no, 0, 1, "led");
	if (ret < 0)
	{
		printk("alloc_chrdev_region error!\n");
		goto register_chrdev_region_err;
	}

#endif

	// ����һ��cdev����
	drv_led_dev->cdev = cdev_alloc();
	if (IS_ERR(drv_led_dev->cdev))
	{
		ret = PTR_ERR(drv_led_dev->cdev);
		goto cdev_alloc_err;
	}
	// cdev��ʼ��
	// ����1:����ʼ����cdev
	// ����2:��ǰ�豸�������豸�ļ������ӿڼ���
	cdev_init(drv_led_dev->cdev, &drv_led_dev->fops);
	// cdevע�ᵽ�ں���
	// ����1:��ע�ᵽ�ں˵�cdev
	// ����2:�豸��
	// ����3:ע�ᵽ�ں˵ĸ���
	cdev_add(drv_led_dev->cdev, drv_led_dev->dev_no, 1);

	// 2\�����豸�ļ�
	drv_led_dev->cls = class_create(THIS_MODULE, "led_cls");
	if (IS_ERR(drv_led_dev->cls))
	{
		printk("class_create error!\n");
		ret = PTR_ERR(drv_led_dev->cls);
		goto class_create_err;
	}

	drv_led_dev->dev = device_create(drv_led_dev->cls, NULL, drv_led_dev->dev_no, NULL, "led");
	if (IS_ERR(drv_led_dev->dev))
	{
		printk("device_create error!\n");
		ret = PTR_ERR(drv_led_dev->dev);
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

	return 0;
	
gpio_request_err:
	printk("device_destroy");
	device_destroy(drv_led_dev->cls, drv_led_dev->dev_no);
device_create_err:
	class_destroy(drv_led_dev->cls);
class_create_err:
	cdev_del(drv_led_dev->cdev);
cdev_alloc_err:
	unregister_chrdev_region(drv_led_dev->dev_no, 1);
register_chrdev_region_err:
	kfree(drv_led_dev);
	
	return ret;
}

static void __exit drv_led_exit(void)
{
	device_destroy(drv_led_dev->cls, drv_led_dev->dev_no);
	class_destroy(drv_led_dev->cls);
	cdev_del(drv_led_dev->cdev);
	unregister_chrdev_region(drv_led_dev->dev_no, 1);
	kfree(drv_led_dev);
}

module_init(drv_led_init);
module_exit(drv_led_exit);

MODULE_LICENSE("GPL");