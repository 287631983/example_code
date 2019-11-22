#ifndef __LED_INFO__
#define __LED_INFO__

#define LED_GPC0_CONF 0xe0200060
#define LED_GPC0_SIZE 8
#define GPC0_DATA 4

//���һ��ƽ̨�Զ�����������
struct led_platdata{
	char * name;
	int shift;  //��λ��
	int conf_reg_data;   //���üĴ�����ֵ��0x11 
	int conf_reg_clear; //���üĴ�����ֵ���:0xff 
	int data_reg;		//���ݼĴ�����ֵ
};

//���һ���豸����
struct s5pv210_led{
	int major;
	struct class *clz;
	struct device *dev;

	void * reg_base;
	struct led_platdata *pdata;  //ָ��ƽ̨�Զ������ݵ�ָ��
};



#endif

