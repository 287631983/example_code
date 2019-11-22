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

	// ����1:ָ��ӳ�����ʼ��ַ,�����NULL��ʾ��ϵͳ��ָ��
	// ����2:ӳ��Ĵ�С
	// ����3:����Ȩ��
	// ����4:��ǰ��ӳ������˽�л��ǹ���
	// ����5:ִ��ӳ������Ӧ���ļ�������
	// ����6:ӳ�������ʼ��ƫ����
	// ����ֵ:����ӳ���Ӧ�������ַ����ʼ
	addr = (char *)mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	// ����������д��ӳ�䷵�ص�ָ�������
	memcpy(addr, test_str, strlen(test_str));
	printf("writ:%s\n", test_str);

	// ��ȡ��������������Ŀռ䵱ǰ��Ӧ�Ĵ洢����
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
