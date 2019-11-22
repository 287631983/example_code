#ifndef __LED_INFO__
#define __LED_INFO__

#define LED_GPC0_CONF 0xe0200060
#define LED_GPC0_SIZE 8
#define GPC0_DATA 4

//设计一个平台自定义数据类型
struct led_platdata{
	char * name;
	int shift;  //移位数
	int conf_reg_data;   //配置寄存器的值：0x11 
	int conf_reg_clear; //配置寄存器的值清空:0xff 
	int data_reg;		//数据寄存器的值
};

//设计一个设备类型
struct s5pv210_led{
	int major;
	struct class *clz;
	struct device *dev;

	void * reg_base;
	struct led_platdata *pdata;  //指向平台自定义数据的指针
};



#endif

