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
		// �رշ�����
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

	// 0\ʵ��������
	// ����1:����ռ�Ĵ�С
	// ����2:���з���ռ�ʱ���û���㹻�Ŀռ��Ƿ���еȴ�
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
	
	// 1\ע��/�����豸��
	ret = register_chrdev(0, "buzzer", &drv_buzzer_dev->fops);
	if (ret < 0)
	{
		printk("register_chrdev error!\n");
		goto register_chrdev_err;
	}

	drv_buzzer_dev->major = ret;

	// 2\�����豸�ļ�
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

	// 3\Ӳ����ʼ��
	// ����ָ����GPIO��
	// ����1:GPIO����(�ú���ͨ��S5PV210_GPD0(_nr)ת���õ�,����_nr��������Ϊ�����pin����)
	ret = gpio_request(S5PV210_GPD0(1), "buzzer");
	if (ret < 0)
	{
		printk("gpio_request error!\n");
		goto gpio_request_err;
	}
	
	// ����ָ����GPIO��Ϊ���,����ͨ������valueָ�������ƽ
	gpio_direction_output(S5PV210_GPD0(1), 0);		// ����Ϊ���ͬʱ����͵�ƽ��buzzerΪ�ǹ���״̬

	// �ͷ��������GPIO��Դ
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
