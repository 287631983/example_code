#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	int fd = open("/dev/led", O_RDWR);
	if (fd < 0)
	{
		perror("open");
		exit(1);
	}
	sleep(5);

	close(fd);

	return 0;
}
