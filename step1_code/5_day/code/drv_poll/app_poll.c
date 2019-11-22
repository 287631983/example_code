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

struct key_board_data{
	unsigned char button_press;
	unsigned int code_value;
};



int main(int argc, char *argv[])
{
	struct key_board_data key_data;
	struct pollfd fds[2];
	int ret;
	char buff[250] = {0};
	int ret_size;
	
	int fd = open("/dev/button", O_RDWR | O_NONBLOCK);
	if (fd < 0)
	{
		perror("open");
		exit(1);
	}

	fds[0].fd = 0;
	fds[0].events = POLLIN;

	fds[1].fd = fd;
	fds[1].events = POLLIN;
#if 0
struct pollfd {
			int fd; 				// 文件描述符
			short events;			// 监控的事件 POLLIN
			short revents;			// 实际发生的事件
		};
#endif

	while (1)
	{
		// 参数1:监控对象的集合
		// 参数2:监控的路数
		// 参数3:超时时间,如果填充的为负数,一直等待
		// 返回值:返回大于0的数表示有监控事件发生
		ret = poll(fds, (sizeof(fds)/sizeof(fds[0])), -1);
		if (ret > 0)
		{
			// 轮询区分到底是哪一个fds监控对象有内容
			if (fds[0].revents & POLLIN)
			{
				// 对应处理标准输入的操作
				ret_size = read(0, buff, sizeof(buff));
				buff[ret_size] = '\0';

				printf("%s\n", buff);
				
			}

			if (fds[1].revents & POLLIN)
			{
				read(fd, &key_data, sizeof(struct key_board_data));
				
				switch (key_data.code_value)
				{
				case KEY_UP:
					if (key_data.button_press)
					{
						printf("KEY_UP press!\n");
					}
					else
					{
						printf("KEY_UP release!\n");
					}
					break;
				
				case KEY_DOWN:
					if (key_data.button_press)
					{
						printf("KEY_DOWN press!\n");
					}
					else
					{
						printf("KEY_DOWN release!\n");
					}
					break;
				
				case KEY_LEFT:
					if (key_data.button_press)
					{
						printf("KEY_LEFT press!\n");
					}
					else
					{
						printf("KEY_LEFT release!\n");
					}
					break;

				case KEY_RIGHT:
					if (key_data.button_press)
					{
						printf("KEY_RIGHT press!\n");
					}
					else
					{
						printf("KEY_RIGHT release!\n");
					}
					break;
				
				default:
					printf("unkonwn key!\n");
					break;
				}
			}
		}
			
		
	}

	close(fd);

	return 0;
}
