

#include <linux/module.h>
#include <linux/init.h>
#include <linux/input.h>
#include <linux/interrupt.h>



static struct input_dev *btn_input;
int irqno;
static irqreturn_t button_interrupt(int irq, void *dummy)
{
	printk("----------^_^ %s-----------------\n",__FUNCTION__);
        //input_report_key(button_dev, BTN_0, inb(BUTTON_PORT) & 1);
        //input_sync(button_dev);
        return IRQ_HANDLED;
}
static int __init simple_input_btn_init(void)
{
    int ret;
	printk("----------^_^ %s-----------------\n",__FUNCTION__);

	//1������һ��input device����ռ�
	btn_input = input_allocate_device();
    if (!btn_input) {
            printk(KERN_ERR "input_allocate_device error\n");
            return -ENOMEM;
    }
	
	//2����ʼ��input device���� ---ʹ��
	btn_input->evbit[0] = BIT_MASK(EV_KEY);   //��ʾҪ���������豸����
    btn_input->keybit[BIT_WORD(KEY_DOWN)] = BIT_MASK(KEY_DOWN);  //��ʾҪ������Щ����

	//3��ע��input device����
	ret = input_register_device(btn_input);
    if (ret) {
            printk(KERN_ERR "input_register_device error\n");
            goto err_free_dev;
    }

	//4��Ӳ����ʼ��---�����ж�
	irqno = IRQ_EINT(1);
    ret = request_irq(irqno, button_interrupt, IRQF_TRIGGER_FALLING|IRQF_TRIGGER_RISING, "key_input_eint1", NULL);
	if(ret != 0){
            printk(KERN_ERR "request_irq error\n");
            goto err_unregister;
    }


    return 0;
err_unregister:
	input_unregister_device(btn_input);
err_free_dev:
	input_free_device(btn_input);	 
	return ret;
}
	
static void __exit simple_input_btn_exit(void)
{
	printk("---------^_^ %s-----------------\n",__FUNCTION__);
	free_irq(irqno, NULL);
	input_unregister_device(btn_input);
	input_free_device(btn_input);
}
		
module_init(simple_input_btn_init);
module_exit(simple_input_btn_exit);



