#include <linux/init.h>
#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/device.h>


//���һ��ȫ���豸����
struct i2c_e2prom{
	int major;
	struct class *clz;
	struct device *dev;
	struct i2c_client *client;  //��¼��ƥ��ɹ�ʱ�����ݸ�probe��client����ĵ�ַ
};

struct i2c_e2prom *at24_dev;

//5, ʵ��fops
const struct file_operations fops = {

};

int at24_i2c_drv_probe(struct i2c_client *client , const struct i2c_device_id * id)
{
	printk("------^_^ %s----------\n",__FUNCTION__);	
	/*
		1��ʵ����ȫ���豸����
		2, �����豸��
		3, �����豸���
		4, Ӳ����ʼ��
		5, ʵ��fops
	*/

	// 1��ʵ����ȫ���豸����
	at24_dev = kzalloc(sizeof(struct i2c_e2prom),GFP_KERNEL);

	// 2, �����豸��
	at24_dev->major = register_chrdev(0,"at24_drv",&fops);


	// 3, �����豸���
	at24_dev->clz = class_create(THIS_MODULE,"at24_clz");
	at24_dev->dev = device_create(at24_dev->clz,NULL,MKDEV(at24_dev->major,3),NULL,"at24_e2prom");

	// 4, Ӳ����ʼ��----e2prom�ϵ��Ϳ��Թ���

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

//1��ʵ����i2c_driver
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
	// 2,ע��i2c_driver
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


