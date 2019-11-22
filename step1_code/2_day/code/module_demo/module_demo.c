#include <linux/init.h>
#include <linux/module.h>

// static修饰,限定了函数的作用域
// int 返回值的类型为int型
// __init 用__init修饰的函数最终会被编译器编译链接到特定的代码段(init.txt),
//		同时在(.initcall.init)代码段中会保留有一份指向该模块加载函数的指针,
//		所有用__init修饰的模块加载函数统一放置,最终在(.initcall.init)
//		代码段中形成函数指针列表,那么在内核启动之后,通过遍历函数指针列表,最终实现模块的加载函数的调用
static int __init module_demo_init(void)
{
	// printk是有输出等级控制的,只有比系统设置的输出等级高的打印信息才会被输出
	// printk当前打印等级查看  通过查看/proc/sys/kernel/printk的文件信息获得
    printk(KERN_INFO"---------%s---------\n", __FUNCTION__);
	
    return 0;
}

static void __exit module_demo_exit(void)
{
    
    printk("---------%s---------\n", __FUNCTION__);
}
module_init(module_demo_init);
module_exit(module_demo_exit);
// 添加GPL认证
MODULE_LICENSE("GPL");
// 模块的别名
MODULE_ALIAS("module_demo");
// 模块的作者
MODULE_AUTHOR("farsight");
// 模块的描述
MODULE_DESCRIPTION("This is a module demo");
// 模块的版本
MODULE_VERSION("V0.0.0");

