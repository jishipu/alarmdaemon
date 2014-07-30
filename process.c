#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ev.h>
#include "common.h"
#include "logger.h"
#include "functions.h"
#include "monitor_item.h"

static void monitor();
static void worker();

static void err_quit(char *msg){
	fatal("%s: %s\n",msg,strerror(errno));
	errno=0;
	exit(EXIT_FAILURE);
}

pthread_t monthrd, workthrd;
pthread_mutex_t		item_lock;

void process(){
    sigset_t set;
	sigemptyset(&set);
	sigaddset(&set,SIGTERM);
	sigaddset(&set,SIGHUP);
	sigprocmask(SIG_BLOCK,&set,NULL);

	if(pthread_create(&monthrd,NULL,(void*)monitor,NULL))
        err_quit("pthread_create: monitor");
	if(pthread_create(&workthrd,NULL,(void*)worker,NULL))
	    err_quit("pthread_create: worker");

	pthread_join(monthrd,NULL);
	pthread_join(workthrd,NULL);
}

static void monitor(){
	sigset_t pendset;
	struct sigaction action;

	while(1){
        sigpending(&pendset);
		if(sigismember(&pendset,SIGTERM)){
			info("catch SIGTERM in process\n");
			sigemptyset(&action.sa_mask);
			action.sa_handler=SIG_IGN;
			sigaction(SIGTERM,&action,NULL);

			info("pthread_cancel in process\n");
			pthread_cancel(workthrd);
			break;
		}else if(sigismember(&pendset,SIGHUP)){
			info("catch SIGHUP in process\n");
			sigemptyset(&action.sa_mask);
			action.sa_handler=SIG_IGN;
			sigaction(SIGHUP,&action,NULL);
		    //SIGHUP 一般是重新加载配置信息
		}
		sleep(1);
		debug("monitor in process\n");
	}
}

static void worker(){
    pthread_mutex_init(&item_lock, NULL);

    init_item();

    ev_periodic p;
    struct ev_loop *loop = ev_default_loop(0);
    ev_periodic_init(&p, check_item_cb, 0, 300, 0); //5分钟检查一次数据库，看是否有更改配置
    ev_periodic_start(loop, &p);
    ev_run(loop, 0);
}
