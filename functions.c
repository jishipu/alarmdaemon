#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <ev.h>
#include <pthread.h>
#include "common.h"
#include "monitor_item.h"
#include "mysql_pu.h"
#include "logger.h"
#include "functions.h"

void check_item_cb(struct ev_loop *loop, ev_periodic * w, int revents)
{
    info("check the database\n");
    db_select_items();
}

void run_item(unsigned int itemid, char* name, int interval, int status)
{
    if(itemid >= MAX_ITEMID || itemid <=0 || interval <= 0) {
        return;
    }
    int status_old = get_item_status(itemid);
    if(status == 1) {
        int interval_old = get_item_interval(itemid);
        if(status_old == 1 && interval_old == interval)
        {
            update_item(itemid, name, interval, status);
            return;
        }
        enable_item(itemid);
        update_item(itemid, name, interval, status);
        int* t = malloc(sizeof(int));
        *t = itemid;
        pthread_t threadid = create_worker(periodic_monitor, t);
        if(threadid == 0) {
            disable_item(itemid);
            free(t);
            return;
        }
    } else if(status == 0) {
        disable_item(itemid);
    }
}

pthread_t create_worker(void *(*func)(void *), void *arg)
{
	pthread_t       thread;
	pthread_attr_t  attr;
	int             ret;

	pthread_attr_init(&attr);

	if ((ret = pthread_create(&thread, &attr, func, arg)) != 0)
	{
		fatal("can't create thread: %s\n", strerror(ret));
		return 0;
	}
	return thread;
}

void* periodic_monitor(void * arg)
{
    pthread_detach(pthread_self());

    unsigned int itemid = *(unsigned int*)arg;

    int interval = get_item_interval(itemid);

    if(interval > 0) {
        struct ev_loop *loop = ev_loop_new(0);
        ev_periodic p;
        ev_async a;
        p.data = itemid;
        a.data = &p;
        ev_periodic_init(&p, time_cb, 0, interval, 0);
        ev_periodic_start(loop, &p);
        ev_async_init(&a, async_cb);
        ev_async_start(loop, &a);
        update_item_loop(itemid, loop, &a);

        ev_run(loop, 0);  //while循环

        ev_loop_destroy(loop);
    }

    free(arg);
    info("threadid: %u itemid: %d interval: %d pthread exit\n", pthread_self(), itemid, interval);
    pthread_exit(0);
}

void async_cb(struct ev_loop *loop, ev_async * w, int revents)
{
    ev_periodic* p = (struct ev_periodic*)w->data;
    ev_periodic_stop(loop, p); 
    ev_async_stop(loop, w);
}

void time_cb(struct ev_loop *loop, ev_periodic * w, int revents)
{
    unsigned int itemid = w->data;
    business_process(itemid);
}

int business_process(unsigned int itemid)
{
    FILE *fp;
    char cmd[1024];
    char buf[BUFSIZE];
    memset(cmd, 0, sizeof(cmd));

    snprintf(cmd, sizeof(cmd), "curl -s --connect-timeout 60 %s/monitor/do?itemid=%d ", CURL_URL, itemid);

    if((fp=popen(cmd, "r")) == NULL)
    {
        fatal("popen: %s", strerror(errno));
        return 0;
    }

    while((fgets(buf, BUFSIZE, fp)) != NULL);

    pclose(fp);

    if(atoi(buf) == 1) {
       info("threadid: %u itemid: %d name: '%s' interval: %d business_process ok\n", pthread_self(), itemid, get_item_name(itemid), get_item_interval(itemid));
       return 1;
    }

    info("threadid: %u itemid: %d name: '%s' interval: %d business_process error: %s\n", pthread_self(), itemid, get_item_name(itemid), get_item_interval(itemid), buf);
    return 0;
}

void print_current_time(char* s)
{
    time_t rawtime;
    struct tm * timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    printf("%s, print_current_time: %u %s\n", s, (unsigned int)pthread_self(), asctime(timeinfo));
}


