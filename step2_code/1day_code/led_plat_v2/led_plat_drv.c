#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/device.h>


#include <asm/io.h>
#include <asm/uaccess.h>


unsigned long *gpc0_conf;
unsigned long *gpc0_data;

//���һ���豸����
struct s5pv210_led{
	int major;
	struct class *clz;
	struct device *dev;
};

struct s5pv210_led *led_dev;
unsigned value;
int led_plat_open(struct inode *inode, struct file *filp)
{
	printk("----------^_^ %s----------------\n",__FUNCTION__);
	//����gpc0Ϊ��� 
	//*gpc0_conf &= ~(0xff<<12);
	//*gpc0_conf |= 0x11<<12;

	value = __raw_readl(gpc0_conf);
	value  &= ~(0xff<<12);
	value  |= 0x11<<12;
	__raw_writel(value, gpc0_conf);
	return 0;
}
ssize_t led_plat_write(struct file *filp, const char __user *buf, size_t size , loff_t * flags)
{
	unsigned int value,vl;
	printk("----------^_^ %s----------------\n",__FUNCTION__);

	if(copy_from_user(&value, buf, size) > 0){
		printk(" copy_from_user error!\n");
		return -EINVAL;
	}

	if(value){
		//���
		//*gpc0_data |= 0x3 << 3;
		vl = readl(gpc0_data);
		vl |= 0x3 << 3;
		writel(vl, gpc0_data);
	}else{
		//���
		//*gpc0_data &= ~(0x3 << 3);
#if 0
		vl = readl(gpc0_data);
		vl &= ~(0x3 << 3);
		writel(vl, gpc0_data);
#else
		writel(readl(gpc0_data) & ~(0x3<<3), gpc0_data);
#endif
	}

	return size;
}

int led_plat_close(struct inode *inode, struct file *filp)
{
	printk("----------^_^ %s----------------\n",__FUNCTION__);
	//*gpc0_data &= ~(0x3 << 3);
	value = readl(gpc0_data);
	value &= ~(0x3 << 3);
	writel(value, gpc0_data);
	return 0;
}



const struct file_operations  led_fops = {
	.open = led_plat_open,
	.write = led_plat_write,
	.release = led_plat_close,
};


//2,ʵ��probe
int led_plat_drv_probe(struct platform_device * pdev )
{
	struct resource *addr_res1,*addr_res2;
	struct resource *irq_res;
	int irqno;
	int ret;

	printk("----------^_^ %s----------------\n",__FUNCTION__);
	/*
		1,ʵ����ȫ�ֶ���
		2,�����豸��
		3,��������豸�ڵ�
		4,Ӳ����ʼ��-----��ȡpdev�е���Դ
		5,ʵ��fops�еĽӿ�
	*/

	// 1, ʵ����ȫ�ֶ���
	led_dev = kzalloc(sizeof(struct s5pv210_led), GFP_KERNEL);
	if(IS_ERR(led_dev)){
		printk(" kzalloc error!\n");
		return PTR_ERR(led_dev);
	}

	// 2,�����豸��
	led_dev->major = register_chrdev(0, "led_drv", &led_fops);
	if(led_dev->major < 0){
		printk(" register_chrdev error!\n");
		ret = led_dev->major;
		goto err_kfree;
	}

	//3,��������豸�ڵ�
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

	//4,Ӳ����ʼ��-----��ȡpdev�е���Դ
	//��ȡpdev�е���Դ
	//����1 --- pdev����
	//����2 -- ����Դ������
	//����3 --  ��Դ�ı��,��Ŵ�0��ʼ
	addr_res1 = platform_get_resource(pdev, IORESOURCE_MEM, 0);

	//Ӳ����ʼ��
	gpc0_conf = ioremap(addr_res1->start, resource_size(addr_res1));
	gpc0_data = gpc0_conf + 1;

	

	addr_res2 = platform_get_resource(pdev, IORESOURCE_MEM, 1);
	printk("addr_res2->start = 0x%x\n",addr_res2->start);


	//��ȡ�ж���Դ
	irq_res = platform_get_resource(pdev, IORESOURCE_IRQ,0);  //����һ
	printk("irq_res.start = %d\n",irq_res->start);

	irqno = platform_get_irq(pdev, 0);  //������
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
	iounmap(gpc0_conf);
	return 0;
}

const struct platform_device_id led_id_table[] = {
	{"s5pv210_led",0x1234},
	{"s3c2410_led",0x1122},
	{"s3c6410_led",0x3344},
};


// 1������pdrv 
struct platform_driver led_pdrv = {
	.probe = led_plat_drv_probe,
	.remove = led_plat_drv_remove,
	.driver = {
		.name = "samsung led_drv", //�Զ��壬����Ҫд,ƥ��ɹ�ʱ������:ls /sys/bus/platform/drivers ����
	},
	.id_table = led_id_table,
};

static int __init led_plat_drv_init(void)
{
	printk("----------^_^ %s----------------\n",__FUNCTION__);
	// 3,ע��pdrv����
	return platform_driver_register(&led_pdrv);
}

static void __exit led_plat_drv_exit(void)
{
	printk("----------^_^ %s----------------\n",__FUNCTION__);
	//ע��pdrv����
	platform_driver_unregister(&led_pdrv);
}

module_init(led_plat_drv_init);
module_exit(led_plat_drv_exit);
MODULE_LICENSE("GPL");


