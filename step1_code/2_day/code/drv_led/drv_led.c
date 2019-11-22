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

// static����,�޶��˺�����������
// int ����ֵ������Ϊint��
// __init ��__init���εĺ������ջᱻ�������������ӵ��ض��Ĵ����(init.txt),
//		ͬʱ��(.initcall.init)������лᱣ����һ��ָ���ģ����غ�����ָ��,
//		������__init���ε�ģ����غ���ͳһ����,������(.initcall.init)
//		��������γɺ���ָ���б�,��ô���ں�����֮��,ͨ����������ָ���б�,����ʵ��ģ��ļ��غ����ĵ���
static int __init drv_led_init(void)
{
	int ret = 0;
	
	// printk��������ȼ����Ƶ�,ֻ�б�ϵͳ���õ�����ȼ��ߵĴ�ӡ��Ϣ�Żᱻ���
	// printk��ǰ��ӡ�ȼ��鿴  ͨ���鿴/proc/sys/kernel/printk���ļ���Ϣ���
    printk(KERN_INFO"---------%s---------\n", __FUNCTION__);

	// 1\�豸�ŵ�ע��
	// �豸��32bits(���豸��12bits+���豸��20bits)
	// ���豸��:���Ա�ʾ���������								eg:����ͷ
	// ���豸��:���Ա�ʾͬ�������еľ���ĳһ��							eg:ǰ��   ����
	
	// ע�����豸��
	// ����1:���豸��,���major���0��ʾ��ϵͳ����,���صľ��Ƿ��䵽�����豸��,�����������ָ,��ô��ʾָ��ע������豸��,����0��ʾ�ɹ�,���ظ�����ʾʧ��
	// ����2:ע�����豸�Ŷ�Ӧ�������ַ���
	// ����3:�豸�ļ��Ĳ����ӿڼ���
	led_major = 250;
	ret = register_chrdev(led_major, "LED", &fops);
	if (ret < 0)
	{
		printk(KERN_ERR"register_chrdev error!\n");

		return -EINVAL;
	}
	
	// 2\�����豸�ļ�
	// ���ַ�ʽ:1\�ֶ�����  2\�Զ�����
	// �ֶ�����:
	//			mknod /dev/led c 250 0

	// �Զ������豸�ļ�
	// �����豸��
	// ����1:���������---THIS_MODULE
	// ����2:��������������ַ���
	// ����ֵ:���ش�������
	led_cls = class_create(THIS_MODULE, "led_cls");
	if (IS_ERR(led_cls))
	{
		printk(KERN_ERR"class_create error!\n");
		
		ret = PTR_ERR(led_cls);
		goto class_create_err;
	}
	
	// �����豸�ļ�
	// ����1:�豸��
	// ����2:�豸�ĸ���,���û����NULL
	// ����3:�豸��(��+��)
	// ����4:�豸�ļ���˽������,û�о���NULL
	// ����5:�豸�ļ���,�����������Ϊ"led",������/dev/·���½��ῴ�����豸�ļ�����Ϊ/dev/led
	// ����ֵ:���ش������豸�ļ�ָ��
	led_dev = device_create(led_cls, NULL, MKDEV(led_major, 0), NULL, "led");
	if (IS_ERR(led_dev))
	{
		printk(KERN_ERR"device_create error!\n");
		
		ret = PTR_ERR(led_dev);
		goto device_create_err;
	}

	// 3\Ӳ���ĳ�ʼ��
	// ����1:Ҫӳ��������ַ����ʼ
	// ����2:Ҫӳ��Ĵ�С
	// ����ֵ:����ӳ�����ɵ������ַ����ʼ
	GPC0_CON = ioremap(0xe0200060, 8);
	GPC0_DAT = GPC0_CON + 1;

	*GPC0_CON &= ~(0xff<<12);
	*GPC0_CON |= (0x11<<12);

	// ��LED�豸���ڷǹ���״̬
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
// ���GPL��֤
MODULE_LICENSE("GPL");
// ģ��ı���
MODULE_ALIAS("module_demo");
// ģ�������
MODULE_AUTHOR("farsight");
// ģ�������
MODULE_DESCRIPTION("This is a module demo");
// ģ��İ汾
MODULE_VERSION("V0.0.0");

