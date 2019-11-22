KERNEL_DIR = /home/farsight/fs210/linux-3.0.8
CUR_DIR = $(shell pwd)
APP = led_app
MODULE_NAME1 = led_plat_dev
MODULE_NAME2 = led_plat_drv

all:
	make -C $(KERNEL_DIR) M=$(CUR_DIR) modules 
	arm-none-linux-gnueabi-gcc -o $(APP) $(APP).c

clean:
	make -C $(KERNEL_DIR) M=$(CUR_DIR) clean
	rm -rf $(APP)

install:
	cp -raf *.ko $(APP) /opt/rootfs/drv_module

obj-m += $(MODULE_NAME1).o
obj-m += $(MODULE_NAME2).o
