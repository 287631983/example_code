#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <asm-generic/ioctl.h>

#define ALL_LED_ON					_IO('A', 0x01)
#define ALL_LED_OFF					_IO('A', 0x02)
#define SEL_LED_ON					_IOW('S', 0x03, int)
#define SEL_LED_OFF					_IOW('S', 0x04, int)

#define SEL_LED1					0x01
#define SEL_LED2					0x02


int main(int argc, char *argv[])
{
	int ctrl_cmd = 0;
	
	int fd = open("/dev/led", O_RDWR);
	if (fd < 0)
	{
		perror("open");
		exit(1);
	}

	while (1)
	{
		ioctl(fd, ALL_LED_ON);
		sleep(1);
		ioctl(fd, ALL_LED_OFF);
		sleep(1);
		ioctl(fd, SEL_LED_ON, SEL_LED1);
		sleep(1);
		ioctl(fd, SEL_LED_OFF, SEL_LED1);
		sleep(1);
		ioctl(fd, SEL_LED_ON, SEL_LED2);
		sleep(1);
		ioctl(fd, SEL_LED_OFF, SEL_LED2);
	}

	close(fd);

	return 0;
}
