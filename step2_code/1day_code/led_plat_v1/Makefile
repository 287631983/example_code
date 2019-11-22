KERNEL_DIR = /home/farsight/fs210/linux-3.0.8
CUR_DIR = $(shell pwd)

MODULE_NAME1 = led_plat_dev
MODULE_NAME2 = led_plat_drv

all:
	make -C $(KERNEL_DIR) M=$(CUR_DIR) modules 

clean:
	make -C $(KERNEL_DIR) M=$(CUR_DIR) clean

install:
	cp -raf *.ko /opt/rootfs/drv_module

obj-m += $(MODULE_NAME1).o
obj-m += $(MODULE_NAME2).o
