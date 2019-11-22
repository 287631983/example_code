#include <linux/init.h>
#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/device.h>

#include <asm/uaccess.h>

//设计一个全局设备类型
struct i2c_e2prom{
	int major;
	struct class *clz;
	struct device *dev;
	struct i2c_client *client;  //记录当匹配成功时，传递给probe的client对象的地址
};

struct i2c_e2prom *at24_dev;

//5, 实现fops
int at24_e2prom_drv_open(struct inode *inode , struct file *filp)
{
	printk("------^_^ %s----------\n",__FUNCTION__);
	return 0;
}

//利用i2c_transter封装自己的操作硬件的读数据的函数
int at24_i2c_read(const struct i2c_client *client, char *buf, int count)
{
	struct i2c_adapter *adap = client->adapter;
	struct i2c_msg msg;
	int ret;
	printk("------^_^ %s----------\n",__FUNCTION__);
	
	msg.addr = client->addr;
	msg.flags = 1;
	msg.len = count;
	msg.buf = buf;

	//参数1 --- i2c_adapter对象的地址
	//参数2 --- 数据包
	//参数3 --- 数据包的个数
	ret = i2c_transfer(adap, &msg, 1);

	return (ret == 1) ? count : ret;
}

int at24_i2c_write(const struct i2c_client *client, char *buf, int count)
{
	struct i2c_adapter *adap = client->adapter;
	struct i2c_msg msg;
	int ret;
	printk("------^_^ %s----------\n",__FUNCTION__);
	
	msg.addr = client->addr;
	msg.flags  = 0;
	msg.len = count;
	msg.buf = buf;

	//参数1 --- i2c_adapter对象的地址
	//参数2 --- 数据包
	//参数3 --- 数据包的个数
	ret = i2c_transfer(adap, &msg, 1);

	return (ret == 1) ? count : ret;
}

ssize_t at24_e2prom_drv_read(struct file *filp, char __user * buf , size_t size , loff_t *flags)
{
	int ret;
	char *tmp;
	printk("------^_^ %s----------\n",__FUNCTION__);
	// 0，先申请内核空间保存读取到的从设备数据
	tmp = kzalloc(size,GFP_KERNEL);
	if(tmp == NULL){
		printk("kzalloc error!\n");
		return -ENOMEM;
	}
	
	// 1, 读取从设备中的数据
	ret = at24_i2c_read(at24_dev->client,tmp,size);
	if(ret < 0){
		printk("at24_i2c_read error");
		goto err_kfree;
	}

	// 2, 将数据转化成应用空间的数据
	ret = copy_to_user(buf,tmp,size);
	if(ret > 0){
		printk("copy_to_user error");
		goto err_kfree;
	}

	kfree(tmp);
	return size;
err_kfree:
	kfree(tmp);
	return ret;
}
ssize_t at24_e2prom_drv_write(struct file *filp , const char __user *buf , size_t size , loff_t * flags)
{
	int ret;
	char *tmp;

	printk("------^_^ %s----------\n",__FUNCTION__);
	// 0，先申请内核空间保存读取到的从设备数据
	tmp = kzalloc(size,GFP_KERNEL);
	if(tmp == NULL){
		printk("kzalloc error!\n");
		return -ENOMEM;
	}
	
	// 1, 将应用空间传递的数据转换成内核空间的数据
	ret = copy_from_user(tmp,buf,size);
	if(ret > 0){
		printk("copy_from_user error");
		goto err_kfree;
	}
	
	
	// 2, 将数据发送给从设备
	
	ret = at24_i2c_write(at24_dev->client,tmp,size);
	if(ret < 0){
		printk("at24_i2c_read error");
		goto err_kfree;
	}

	kfree(tmp);
	return size;
err_kfree:
	kfree(tmp);
	return ret;
}
int at24_e2prom_drv_close(struct inode *inode , struct file *filp)
{
	printk("------^_^ %s----------\n",__FUNCTION__);
	return 0;
}

const struct file_operations fops = {
	.open = at24_e2prom_drv_open,
	.read = at24_e2prom_drv_read,
	.write = at24_e2prom_drv_write,
	.release =at24_e2prom_drv_close,
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
		6,保存匹配成功时，传给probe的client对象的地址
	*/

	// 1，实例化全局设备对象
	at24_dev = kzalloc(sizeof(struct i2c_e2prom),GFP_KERNEL);

	// 2, 申请设备号
	at24_dev->major = register_chrdev(0,"at24_drv",&fops);


	// 3, 创建设备结点
	at24_dev->clz = class_create(THIS_MODULE,"at24_clz");
	at24_dev->dev = device_create(at24_dev->clz,NULL,MKDEV(at24_dev->major,3),NULL,"at24_e2prom");

	// 4, 硬件初始化----e2prom上电后就可以工作

	//6,保存匹配成功时，传给probe的client对象的地址
	at24_dev->client = client;

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
		{"at24c02a",0x1234},
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


