KERNEL_DIR = /home/farsight/fs210/linux-3.0.8
CUR_DIR = $(shell pwd)
all:
	#make -C 表示进入内核模块所在的路径,利用内核编译规则将当前路径下的文件内容编译成模块
	make -C $(KERNEL_DIR) M=$(CUR_DIR) modules 

clean:
	make -C $(KERNEL_DIR) M=$(CUR_DIR) clean

install:
	cp -raf *.ko /opt/rootfs/drv_module

obj-m += module_demo.o
