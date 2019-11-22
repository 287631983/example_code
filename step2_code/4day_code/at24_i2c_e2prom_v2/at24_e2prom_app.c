#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


int main(int argc,char ** argv)
{
    int fd;
    int len;
    char register_addr;   //at24c02a的内部空间地址
    char rbuf[20];
    char wbuf[20];

    if(argc < 2){
	printf("%s r -------读数据\n",argv[0]);
	printf("%s w string-------写数据\n",argv[0]);
	exit(1);
    }

    fd = open("/dev/at24_e2prom",O_RDWR);
    if(fd < 0){
	perror("open");
	exit(1);
    }

    //根据命令行传递的参数选择写数据还是读数据argv[1]='r',表示读数据，argv[1]='w'，表示写数据
    if(!strncmp(argv[1],"r",1)){  //读数据
	//1,先将at24c02a的内部地址传给适配器
	printf("请输入内部地址:");
	scanf("%hhx",&register_addr);
	if(write(fd,&register_addr,1) != 1){
	    perror("write");
	    exit(1);
	}

	bzero(rbuf,sizeof(rbuf));
	//2，再去读数据
	if(read(fd,rbuf,sizeof(rbuf)) < 0){
	    perror("read");
	    exit(1);
	}

	//3，打印读到的数据
	printf("rbuf:%s(%d)\n",rbuf,strlen(rbuf));

    }else if((argc==3) && !strncmp(argv[1],"w",1)){ //写数据

	printf("请输入内部地址:");
	scanf("%hhx",&register_addr);

	bzero(wbuf,sizeof(wbuf));
	wbuf[0] = register_addr;  //内部地址
	printf("register_addr:0x%hhx\n",wbuf[0]);
	printf("请输入数据:");
	while(getchar() != '\n');
	fgets(wbuf+1,sizeof(wbuf)-1,stdin);
	wbuf[strlen(wbuf)] = '\0';
	//strcpy(wbuf+1,argv[2]);
	len = strlen(wbuf);	
	printf("len = %d\n",len);
	if(write(fd,wbuf,strlen(wbuf)+1) != len){
	    perror("write");
	    exit(1);
	}
	printf("write OK!\n");
    }else{
	printf("input error!\n");	
    }

    close(fd);
    return 0;
}
