

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

	// 0，获取当前触发中断的按键信息
	tmp = (struct btn_info *)dev_id;
	event = tmp->event;

	// 1, 获取按键的数据
	value = gpio_get_value(tmp->gpiono);

	// 2，将数据上报给hander层
	if(value){
		//松开
		input_report_key(keydev->i_dev, event.code, event.value);   //方法1
        input_sync(keydev->i_dev);
	}else{
		//按下
		//参数1 --- input dev对象的地址
		//参数2 --- 哪种设备
		//参数3 --- 上报的数据具体是什么
		//参数4 --- 上报的数据状态
		event.value = 1;
		input_event(keydev->i_dev, event.type, event.code, event.value);   //方法2
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
		1，实例化按键的驱动设备对象
		2, 获取平台自定义数据
		3, 分配一个input device对象空间
		4，初始化input device对象 ---使能
		5，注册input device对象
		6，硬件初始化---申请中断
	*/

	//1，实例化按键的驱动设备对象
	keydev = kzalloc(sizeof(struct btn_input_dev),GFP_KERNEL);
	if(IS_ERR(keydev)){
		printk("kzalloc error");
		return -ENOMEM;
	}
	// 2, 获取平台自定义数据
	keydev->pd = (struct btn_platdata*)pdev->dev.platform_data;
	keydev->dev = pdev->dev;
	
	//3, 分配一个input device对象空间
	keydev->i_dev= input_allocate_device();
    if (!keydev->i_dev) {
            printk(KERN_ERR "input_allocate_device error\n");
            ret =  -ENOMEM;
			goto err_kfree;
    }
	
	//4，初始化input device对象 ---使能
	keydev->i_dev->evbit[0] = BIT_MASK(EV_KEY);   //表示要产生哪种设备数据
   // btn_input->keybit[BIT_WORD(KEY_DOWN)] = BIT_MASK(KEY_DOWN);  //表示要产生哪些数据

	//5，注册input device对象
	ret = input_register_device(keydev->i_dev);
    if (ret) {
            printk(KERN_ERR "input_register_device error\n");
            goto err_free_dev;
    }

	//6，硬件初始化---申请中断
	for(i = 0; i < keydev->pd->key_size;i++){
		irqno = gpio_to_irq(keydev->pd->key_set[i].gpiono);
		kinfo = &keydev->pd->key_set[i];
	    ret = request_irq(irqno, button_interrupt, IRQF_TRIGGER_FALLING|IRQF_TRIGGER_RISING, kinfo->name, kinfo);
		if(ret != 0){
	            printk(KERN_ERR "request_irq error\n");
	            goto err_unregister;
	    }

		//参数1 ----- 那个数据
		//参数2 ----- 那个设备对应的数组:keybit,absbit,ralbit等
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
		.name = "s5pv210_key",   //如果没有对id_table初始化，则用driver.name与pdev匹配
	},
};

static int __init plat_input_drv_init(void)
{
	printk("---------^_^ %s-----------------\n",__FUNCTION__);
	//注册pdrv
	return platform_driver_register(&input_pdrv);
}	
static void __exit plat_input_drv_exit(void)
{
	printk("---------^_^ %s-----------------\n",__FUNCTION__);
	//注销pdrv
	platform_driver_unregister(&input_pdrv);
}
		
module_init(plat_input_drv_init);
module_exit(plat_input_drv_exit);
MODULE_LICENSE("GPL");



