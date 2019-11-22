

#include <linux/module.h>
#include <linux/init.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>
#include <linux/slab.h>
#include <linux/platform_device.h>


#include "plat_input.h"

struct btn_input_dev *keydev;
//static struct input_dev *btn_input;
int irqno;
static irqreturn_t button_interrupt(int irq, void *dev_id)
{
	int value;
	struct btn_info *tmp;
	struct input_event event;
	printk("----------^_^ %s-----------------\n",__FUNCTION__);

	// 0����ȡ��ǰ�����жϵİ�����Ϣ
	tmp = (struct btn_info *)dev_id;
	event = tmp->event;

	// 1, ��ȡ����������
	value = gpio_get_value(tmp->gpiono);

	// 2���������ϱ���hander��
	if(value){
		//�ɿ�
		input_report_key(keydev->i_dev, event.code, event.value);   //����1
        input_sync(keydev->i_dev);
	}else{
		//����
		//����1 --- input dev����ĵ�ַ
		//����2 --- �����豸
		//����3 --- �ϱ������ݾ�����ʲô
		//����4 --- �ϱ�������״̬
		event.value = 1;
		input_event(keydev->i_dev, event.type, event.code, event.value);   //����2
		input_sync(keydev->i_dev);
	}
       
        return IRQ_HANDLED;
}

int input_btn_probe(struct platform_device *pdev)
{
    int ret,i;
	struct btn_info *kinfo;
	
	printk("----------^_^ %s-----------------\n",__FUNCTION__);

	/*
		1��ʵ���������������豸����
		2, ��ȡƽ̨�Զ�������
		3, ����һ��input device����ռ�
		4����ʼ��input device���� ---ʹ��
		5��ע��input device����
		6��Ӳ����ʼ��---�����ж�
	*/

	//1��ʵ���������������豸����
	keydev = kzalloc(sizeof(struct btn_input_dev),GFP_KERNEL);
	if(IS_ERR(keydev)){
		printk("kzalloc error");
		return -ENOMEM;
	}
	// 2, ��ȡƽ̨�Զ�������
	keydev->pd = (struct btn_platdata*)pdev->dev.platform_data;
	keydev->dev = pdev->dev;
	
	//3, ����һ��input device����ռ�
	keydev->i_dev= input_allocate_device();
    if (!keydev->i_dev) {
            printk(KERN_ERR "input_allocate_device error\n");
            ret =  -ENOMEM;
			goto err_kfree;
    }
	
	//4����ʼ��input device���� ---ʹ��
	keydev->i_dev->evbit[0] = BIT_MASK(EV_KEY);   //��ʾҪ���������豸����
   // btn_input->keybit[BIT_WORD(KEY_DOWN)] = BIT_MASK(KEY_DOWN);  //��ʾҪ������Щ����

	//5��ע��input device����
	ret = input_register_device(keydev->i_dev);
    if (ret) {
            printk(KERN_ERR "input_register_device error\n");
            goto err_free_dev;
    }

	//6��Ӳ����ʼ��---�����ж�
	for(i = 0; i < keydev->pd->key_size;i++){
		irqno = gpio_to_irq(keydev->pd->key_set[i].gpiono);
		kinfo = &keydev->pd->key_set[i];
	    ret = request_irq(irqno, button_interrupt, IRQF_TRIGGER_FALLING|IRQF_TRIGGER_RISING, kinfo->name, kinfo);
		if(ret != 0){
	            printk(KERN_ERR "request_irq error\n");
	            goto err_unregister;
	    }

		//����1 ----- �Ǹ�����
		//����2 ----- �Ǹ��豸��Ӧ������:keybit,absbit,ralbit��
		set_bit(kinfo->event.code,keydev->i_dev->keybit);
	}

    return 0;
err_unregister:
	input_unregister_device(keydev->i_dev);
err_free_dev:
	input_free_device(keydev->i_dev);	
err_kfree:
	kfree(keydev);
	return ret;
}

int input_btn_remove(struct platform_device * pdev)
{
	int i;
	struct btn_info *kinfo;
	for(i = 0; i < keydev->pd->key_size;i++){
		irqno = gpio_to_irq(keydev->pd->key_set[i].gpiono);
		kinfo = &keydev->pd->key_set[i];
		free_irq(irqno, kinfo);
	}
	input_unregister_device(keydev->i_dev);
	input_free_device(keydev->i_dev);
	kfree(keydev);

	return 0;
}
struct platform_driver input_pdrv = {
	.probe = input_btn_probe,
	.remove = input_btn_remove,
	.driver = {
		.name = "s5pv210_key",   //���û�ж�id_table��ʼ��������driver.name��pdevƥ��
	},
};

static int __init plat_input_drv_init(void)
{
	printk("---------^_^ %s-----------------\n",__FUNCTION__);
	//ע��pdrv
	return platform_driver_register(&input_pdrv);
}	
static void __exit plat_input_drv_exit(void)
{
	printk("---------^_^ %s-----------------\n",__FUNCTION__);
	//ע��pdrv
	platform_driver_unregister(&input_pdrv);
}
		
module_init(plat_input_drv_init);
module_exit(plat_input_drv_exit);
MODULE_LICENSE("GPL");



