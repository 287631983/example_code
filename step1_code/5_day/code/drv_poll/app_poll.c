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
			int fd; 				// �ļ�������
			short events;			// ��ص��¼� POLLIN
			short revents;			// ʵ�ʷ������¼�
		};
#endif

	while (1)
	{
		// ����1:��ض���ļ���
		// ����2:��ص�·��
		// ����3:��ʱʱ��,�������Ϊ����,һֱ�ȴ�
		// ����ֵ:���ش���0������ʾ�м���¼�����
		ret = poll(fds, (sizeof(fds)/sizeof(fds[0])), -1);
		if (ret > 0)
		{
			// ��ѯ���ֵ�������һ��fds��ض���������
			if (fds[0].revents & POLLIN)
			{
				// ��Ӧ�����׼����Ĳ���
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
