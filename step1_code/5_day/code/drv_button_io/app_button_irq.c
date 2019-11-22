#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <asm-generic/ioctl.h>
#include <linux/input.h>
#include <stdio.h>

struct key_board_data{
	unsigned char button_press;
	unsigned int code_value;
};



int main(int argc, char *argv[])
{
	struct key_board_data key_data;
	
	int fd = open("/dev/button", O_RDWR | O_NONBLOCK);
	if (fd < 0)
	{
		perror("open");
		exit(1);
	}

	while (1)
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

	close(fd);

	return 0;
}
