#include <linux/module.h>
#include <linux/init.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>
#include <linux/platform_device.h>


#include "plat_input.h"


struct btn_info btn_set[] = {
	[0] = {
		.name = "key_up",
		.gpiono = S5PV210_GPH0(0),
		.event = {
			.type = EV_KEY,
			.code = KEY_UP,
			.value = 0,
		},
	},
	[1] = {
		.name = "key_down",
		.gpiono = S5PV210_GPH0(1),
		.event = {
			.type = EV_KEY,
			.code = KEY_DOWN,
			.value = 0,
		},
	},
	[2] = {
		.name = "key_left",
		.gpiono = S5PV210_GPH0(2),
		.event = {
			.type = EV_KEY,
			.code = KEY_LEFT,
			.value = 0,
		},
	},
	[3] = {
		.name = "key_right",
		.gpiono = S5PV210_GPH0(3),
		.event = {
			.type = EV_KEY,
			.code = KEY_RIGHT,
			.value = 0,
		},
	},
};

//��ʼ���Զ�������
struct btn_platdata btn_input_pdata = {
	.key_size = ARRAY_SIZE(btn_set),
	.key_set = btn_set,
};

void plat_input_release(struct device *dev)
{
	printk("---------^_^ %s-----------------\n",__FUNCTION__);
}


//����һ��pdev
struct platform_device input_dev = {
	.name = "s5pv210_key",
	.id = -1,
	.dev = {
		//.platform_data = &btn_input_pdata,  //���Զ������ݱ��浽 pdev��
		.release = plat_input_release,
	},
};

void __init s3c_btn_set_platdata(struct btn_platdata *pd)
{
	struct btn_platdata *npd;

	if (!pd) {
		printk(KERN_ERR "%s: no platform data\n", __func__);
		return;
	}

	// 1,��pd��ΪNULLʱ�������Զ����������ʹ�С�Ŀռ��npd,ͬʱ��pdָ����Զ��������е����ݸ��Ƶ�����Ŀռ���
	npd = kmemdup(pd, sizeof(struct btn_platdata), GFP_KERNEL);
	if (!npd)
		printk(KERN_ERR "%s: no memory for platform data\n", __func__);

	// 2��
	input_dev.dev.platform_data = npd;
}



static int __init plat_input_dev_init(void)
{
	printk("---------^_^ %s-----------------\n",__FUNCTION__);
	// 1�� ����ƽ̨�Զ�������
	s3c_btn_set_platdata(&btn_input_pdata);

	// 2��ע��pdev
	return platform_device_register(&input_dev);
}

static void __exit plat_input_dev_exit(void)
{
	printk("---------^_^ %s-----------------\n",__FUNCTION__);
	platform_device_unregister(&input_dev);
}


module_init(plat_input_dev_init);
module_exit(plat_input_dev_exit);
MODULE_LICENSE("GPL");


