
#ifndef __PLAT_INPUT_H__
#define __PLAT_INPUT_H__ 

#include <linux/input.h>

//设计一个按键信息的数据类型
struct btn_info{
	char *name;
	char gpiono;
	struct input_event event;
};

//设计一个平台自定义数据类型
struct btn_platdata{
	int key_size;			//表示按键数组的元素个数
	struct btn_info *key_set;  //指向按键数组的指针 
};

//设计按键的平台驱动类型
struct btn_input_dev{
	struct input_dev *i_dev;
	struct btn_platdata *pd;
	struct device dev;
};

#endif


