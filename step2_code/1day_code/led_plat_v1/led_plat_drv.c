#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>

#include <asm/io.h>

unsigned long *gpc0_conf;
unsigned long *gpc0_data;

//2,ʵ��probe
int led_plat_drv_probe(struct platform_device * pdev )
{
	struct resource *addr_res1,*addr_res2;
	struct resource *irq_res;
	int irqno;

	printk("----------^_^ %s----------------\n",__FUNCTION__);
	/*
		1,ʵ����ȫ�ֶ���
		2,�����豸��
		3,��������豸�ڵ�
		4,Ӳ����ʼ��-----��ȡpdev�е���Դ
		5,ʵ��fops�еĽӿ�
	*/

	//��ȡpdev�е���Դ
	//����1 --- pdev����
	//����2 -- ����Դ������
	//����3 --  ��Դ�ı��,��Ŵ�0��ʼ
	addr_res1 = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	gpc0_conf = ioremap(addr_res1->start, resource_size(addr_res1));
	gpc0_data = gpc0_conf + 1;

	//����gpc0Ϊ���
	*gpc0_conf &= ~(0xff<<12);
	*gpc0_conf |= 0x11<<12;

	*gpc0_data |= 0x3 << 3;

	addr_res2 = platform_get_resource(pdev, IORESOURCE_MEM, 1);
	printk("addr_res2->start = 0x%x\n",addr_res2->start);


	//��ȡ�ж���Դ
	irq_res = platform_get_resource(pdev, IORESOURCE_IRQ,0);  //����һ
	printk("irq_res.start = %d\n",irq_res->start);

	irqno = platform_get_irq(pdev, 0);  //������
	printk("irqno = %d\n",irqno);

	return 0;
	
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


