#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <asm-generic/ioctl.h>
#include <linux/input.h>
#include <stdio.h>
#include <poll.h>
#include <sys/mman.h>
#include <string.h>

#define PAGE_SIZE			(1<<12)

int main(int argc, char *argv[])
{
	char buff[250] = {0};
	char *addr;
	char *test_str = "hgehogiherioghrioegio";
	
	int fd = open("/dev/button", O_RDWR);
	if (fd < 0)
	{
		perror("open");
		exit(1);
	}

	// 参数1:指定映射的起始地址,如果填NULL表示由系统来指定
	// 参数2:映射的大小
	// 参数3:访问权限
	// 参数4:当前的映射区域私有还是共享
	// 参数5:执行映射所对应的文件描述符
	// 参数6:映射相对起始的偏移量
	// 返回值:返回映射对应的虚拟地址的起始
	addr = (char *)mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	// 将测试数据写入映射返回的指向的区域
	memcpy(addr, test_str, strlen(test_str));
	printf("writ:%s\n", test_str);

	// 读取驱动程序所申请的空间当前对应的存储内容
	read(fd, buff, strlen(test_str));
	buff[strlen(test_str)] = '\0';
	printf("read:%s\n", buff);

	while (1)
	{
		;
	}

	close(fd);

	return 0;
}
