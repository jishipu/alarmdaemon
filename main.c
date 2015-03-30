#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "logger.h"
#include "process.h"

static void err_quit(char *msg){
	fatal("%s: %s\n",msg,strerror(errno));
	errno=0;
	exit(EXIT_FAILURE);
}

int main()
{
    int i;
	for(i=0;i<5;i++){
		if(isrunning(YAKMSG_PID)==-1) break;
		sleep(1);
	}
	if(isrunning(YAKMSG_PID)==0){
		puts("yakmsg can't start because it has run");
		exit(0);
	}

	pid_t pid,sid;
	if((pid=fork())<0) {
		perror("fork");
		exit(1);
	}
	if(pid>0) exit(EXIT_SUCCESS); //exit parent process

	if((sid=setsid())<0){
		perror("setsid");
		exit(1);
	}
	//get current dir,then change it
	getcwd(CURRENT_DIR,256);
	printf("get current dir: %s\n",CURRENT_DIR);
	if(chdir("/")<0){
		perror("chdir");
		exit(1);
	}
	umask(0);
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);

	openlogger();
	info("yakmsg starting...\n");
	debug("pid1=%d,sid=%d\n",pid,sid);
	debug("getpid()=%d\n",getpid());
	if(savepid(YAKMSG_PID,sid)==-1) err_quit("savepid");

	if((pid=fork())<0) err_quit("fork again");
	if(pid==0){
		//enter child proc
		/********/
		process();
        //do something long time
        /********/
		exit(EXIT_SUCCESS);
	}
	debug("pid2=%d\n",pid);
    /********/
    //do something long time
    /********/
	waitpid(pid,NULL,0);
	removepid(YAKMSG_PID);

	info("yakmsg stop\n");
	closelogger();

	exit(EXIT_SUCCESS);
}
