#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


int main(void)
{
    int fd;
    int i,on;

    fd = open("/dev/led01",O_RDWR);
    if(fd < 0){
	perror("open");
	exit(1);
    }

    for(i = 0; i < 10; i++){
	on = 1;
	write(fd,&on,sizeof(on));
	sleep(1);
	on = 0;
	write(fd,&on,sizeof(on));
	sleep(1);
    }

    close(fd);
    return 0;
}
