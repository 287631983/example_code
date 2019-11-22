#include <linux/init.h>
#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/device.h>


//设计一个全局设备类型
struct i2c_e2prom{
	int major;
	struct class *clz;
	struct device *dev;
	struct i2c_client *client;  //记录当匹配成功时，传递给probe的client对象的地址
};

struct i2c_e2prom *at24_dev;

//5, 实现fops
const struct file_operations fops = {

};

int at24_i2c_drv_probe(struct i2c_client *client , const struct i2c_device_id * id)
{
	printk("------^_^ %s----------\n",__FUNCTION__);	
	/*
		1，实例化全局设备对象
		2, 申请设备号
		3, 创建设备结点
		4, 硬件初始化
		5, 实现fops
	*/

	// 1，实例化全局设备对象
	at24_dev = kzalloc(sizeof(struct i2c_e2prom),GFP_KERNEL);

	// 2, 申请设备号
	at24_dev->major = register_chrdev(0,"at24_drv",&fops);


	// 3, 创建设备结点
	at24_dev->clz = class_create(THIS_MODULE,"at24_clz");
	at24_dev->dev = device_create(at24_dev->clz,NULL,MKDEV(at24_dev->major,3),NULL,"at24_e2prom");

	// 4, 硬件初始化----e2prom上电后就可以工作

	return 0;
}
int at24_i2c_drv_remove(struct i2c_client * client)
{
	printk("------^_^ %s----------\n",__FUNCTION__);	
	device_destroy(at24_dev->clz,MKDEV(at24_dev->major,3));
	class_destroy(at24_dev->clz);
	unregister_chrdev(at24_dev->major,"at24_drv");
	kfree(at24_dev);
	return 0;
}


const struct i2c_device_id at24_id_table[] = {
		{"at24c02x",0x1234},
		{"at24c04a",0x1122},
		{"at24c08a",0x2233},
		{"at24c16a",0x3344},
};

//1，实例化i2c_driver
struct i2c_driver ag24_drv = {
	.probe = at24_i2c_drv_probe,
	.remove = at24_i2c_drv_remove,
	.driver = {
		.name = "at24_e2prom_drv",
	},
	.id_table = at24_id_table,
};

static int __init at24_i2c_drv_init(void)
{
	printk("------^_^ %s----------\n",__FUNCTION__);	
	// 2,注册i2c_driver
	return i2c_add_driver(&ag24_drv);
}
static void __exit at24_i2c_drv_exit(void)
{
	printk("------^_^ %s----------\n",__FUNCTION__);
	i2c_del_driver(&ag24_drv);
}
module_init(at24_i2c_drv_init);
module_exit(at24_i2c_drv_exit);
MODULE_LICENSE("GPL");


