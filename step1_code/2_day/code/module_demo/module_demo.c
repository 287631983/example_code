#include <linux/init.h>
#include <linux/module.h>

// static����,�޶��˺�����������
// int ����ֵ������Ϊint��
// __init ��__init���εĺ������ջᱻ�������������ӵ��ض��Ĵ����(init.txt),
//		ͬʱ��(.initcall.init)������лᱣ����һ��ָ���ģ����غ�����ָ��,
//		������__init���ε�ģ����غ���ͳһ����,������(.initcall.init)
//		��������γɺ���ָ���б�,��ô���ں�����֮��,ͨ����������ָ���б�,����ʵ��ģ��ļ��غ����ĵ���
static int __init module_demo_init(void)
{
	// printk��������ȼ����Ƶ�,ֻ�б�ϵͳ���õ�����ȼ��ߵĴ�ӡ��Ϣ�Żᱻ���
	// printk��ǰ��ӡ�ȼ��鿴  ͨ���鿴/proc/sys/kernel/printk���ļ���Ϣ���
    printk(KERN_INFO"---------%s---------\n", __FUNCTION__);
	
    return 0;
}

static void __exit module_demo_exit(void)
{
    
    printk("---------%s---------\n", __FUNCTION__);
}
module_init(module_demo_init);
module_exit(module_demo_exit);
// ���GPL��֤
MODULE_LICENSE("GPL");
// ģ��ı���
MODULE_ALIAS("module_demo");
// ģ�������
MODULE_AUTHOR("farsight");
// ģ�������
MODULE_DESCRIPTION("This is a module demo");
// ģ��İ汾
MODULE_VERSION("V0.0.0");

