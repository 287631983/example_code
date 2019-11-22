#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/input.h>


int main(void)
{
    int fd;
    int ret;
    struct input_event event;

    fd = open("/dev/event0",O_RDWR);
    if(fd < 0){
	perror("open");
	exit(1);
    }

    while(1){
	ret = read(fd,&event,sizeof(event));
	if(ret < 0){
	    perror("read");
	    exit(1);
	}
	if(event.type == EV_KEY){
	    if(event.code == KEY_DOWN){
		if(event.value){
		    //按下
		    printf("app-->KEY_DOWN: pressed!\n");
		}else{
		    //松开
		    printf("app-->KEY_DOWN: up!\n");
		}
	    }
	}
    }

    close(fd);
    return 0;
}
