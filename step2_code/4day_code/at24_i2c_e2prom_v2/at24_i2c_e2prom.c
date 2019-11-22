#include <linux/init.h>
#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/device.h>

#include <asm/uaccess.h>

//���һ��ȫ���豸����
struct i2c_e2prom{
	int major;
	struct class *clz;
	struct device *dev;
	struct i2c_client *client;  //��¼��ƥ��ɹ�ʱ�����ݸ�probe��client����ĵ�ַ
};

struct i2c_e2prom *at24_dev;

//5, ʵ��fops
int at24_e2prom_drv_open(struct inode *inode , struct file *filp)
{
	printk("------^_^ %s----------\n",__FUNCTION__);
	return 0;
}

//����i2c_transter��װ�Լ��Ĳ���Ӳ���Ķ����ݵĺ���
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

	//����1 --- i2c_adapter����ĵ�ַ
	//����2 --- ���ݰ�
	//����3 --- ���ݰ��ĸ���
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

	//����1 --- i2c_adapter����ĵ�ַ
	//����2 --- ���ݰ�
	//����3 --- ���ݰ��ĸ���
	ret = i2c_transfer(adap, &msg, 1);

	return (ret == 1) ? count : ret;
}

ssize_t at24_e2prom_drv_read(struct file *filp, char __user * buf , size_t size , loff_t *flags)
{
	int ret;
	char *tmp;
	printk("------^_^ %s----------\n",__FUNCTION__);
	// 0���������ں˿ռ䱣���ȡ���Ĵ��豸����
	tmp = kzalloc(size,GFP_KERNEL);
	if(tmp == NULL){
		printk("kzalloc error!\n");
		return -ENOMEM;
	}
	
	// 1, ��ȡ���豸�е�����
	ret = at24_i2c_read(at24_dev->client,tmp,size);
	if(ret < 0){
		printk("at24_i2c_read error");
		goto err_kfree;
	}

	// 2, ������ת����Ӧ�ÿռ������
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
	// 0���������ں˿ռ䱣���ȡ���Ĵ��豸����
	tmp = kzalloc(size,GFP_KERNEL);
	if(tmp == NULL){
		printk("kzalloc error!\n");
		return -ENOMEM;
	}
	
	// 1, ��Ӧ�ÿռ䴫�ݵ�����ת�����ں˿ռ������
	ret = copy_from_user(tmp,buf,size);
	if(ret > 0){
		printk("copy_from_user error");
		goto err_kfree;
	}
	
	
	// 2, �����ݷ��͸����豸
	
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
		
		1��ʵ����ȫ���豸����
		2, �����豸��
		3, �����豸���
		4, Ӳ����ʼ��
		5, ʵ��fops
		6,����ƥ��ɹ�ʱ������probe��client����ĵ�ַ
	*/

	// 1��ʵ����ȫ���豸����
	at24_dev = kzalloc(sizeof(struct i2c_e2prom),GFP_KERNEL);

	// 2, �����豸��
	at24_dev->major = register_chrdev(0,"at24_drv",&fops);


	// 3, �����豸���
	at24_dev->clz = class_create(THIS_MODULE,"at24_clz");
	at24_dev->dev = device_create(at24_dev->clz,NULL,MKDEV(at24_dev->major,3),NULL,"at24_e2prom");

	// 4, Ӳ����ʼ��----e2prom�ϵ��Ϳ��Թ���

	//6,����ƥ��ɹ�ʱ������probe��client����ĵ�ַ
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


