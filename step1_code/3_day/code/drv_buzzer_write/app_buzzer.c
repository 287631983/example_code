#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	int ctrl_cmd = 0;
	
	int fd = open("/dev/buzzer", O_RDWR);
	if (fd < 0)
	{
		perror("open");
		exit(1);
	}

	while (1)
	{
		ctrl_cmd = 1;
		write(fd, &ctrl_cmd, sizeof(int));
		sleep(1);
		ctrl_cmd = 0;
		write(fd, &ctrl_cmd, sizeof(int));
		sleep(1);
	}

	close(fd);

	return 0;
}
