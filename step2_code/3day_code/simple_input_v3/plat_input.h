
#ifndef __PLAT_INPUT_H__
#define __PLAT_INPUT_H__ 

#include <linux/input.h>

//���һ��������Ϣ����������
struct btn_info{
	char *name;
	char gpiono;
	struct input_event event;
};

//���һ��ƽ̨�Զ�����������
struct btn_platdata{
	int key_size;			//��ʾ���������Ԫ�ظ���
	struct btn_info *key_set;  //ָ�򰴼������ָ�� 
};

//��ư�����ƽ̨��������
struct btn_input_dev{
	struct input_dev *i_dev;
	struct btn_platdata *pd;
	struct device dev;
};

#endif


