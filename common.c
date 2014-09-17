#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <fcntl.h>
#include "common.h"

int savepid(char *path,int pid){
	int fd;
	if((fd=open(path,O_WRONLY|O_CREAT,0666))<0) return -1;
	if(flock(fd,LOCK_EX|LOCK_NB)==-1) return -1;
	char buf[10];
	sprintf(buf,"%d",pid);
	if(write(fd,buf,strlen(buf))<0) return -1;
	close(fd);
	return 0;
}

int removepid(char *path){
	return remove(path);
}

int isrunning(char *path){
	int fd;
	int rs;
	if((fd=open(path,O_RDONLY))<0) return -1;
	if(flock(fd,LOCK_EX|LOCK_NB)==-1)
            rs=0;
	else{
            flock(fd,LOCK_UN|LOCK_NB);
            rs=-1;
        }
	close(fd);
	return rs;
}
