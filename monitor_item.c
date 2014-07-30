#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include <ev.h>
#include "functions.h"
#include "monitor_item.h"

extern pthread_mutex_t item_lock;
MONITOR_ITEM item_array[MAX_ITEMID];

void init_item()
{
    memset(item_array, 0 ,sizeof(item_array));
    return;
}

int get_item_status(unsigned int itemid)
{
    return item_enable == item_array[itemid].status ? 1 : 0;
}

struct ev_loop* get_item_loop(unsigned int itemid)
{
    if(item_enable == item_array[itemid].status)
        return item_array[itemid].loop;
    else
        return NULL;
}

int get_item_interval(unsigned int itemid)
{
    if(item_enable == item_array[itemid].status)
        return item_array[itemid].interval;
    else
        return -1;
}

char* get_item_name(unsigned int itemid)
{
    if(item_enable == item_array[itemid].status)
        return item_array[itemid].name;
    else
        return NULL;
}

void disable_item(unsigned int itemid)
{
    if(itemid > 0)
    {
        pthread_mutex_lock(&item_lock);

        item_array[itemid].status = item_disable;
        if(item_array[itemid].loop != NULL)
        {
            ev_async_send(item_array[itemid].loop, item_array[itemid].async_watcher);
        }
        item_array[itemid].loop = NULL;
        item_array[itemid].async_watcher = NULL;

        pthread_mutex_unlock(&item_lock);
    }
    return;
}

void enable_item(unsigned int itemid)
{
    if(itemid > 0)
    {
        pthread_mutex_lock(&item_lock);

        item_array[itemid].status = item_enable;
        if(item_array[itemid].loop != NULL)
        {
            ev_async_send(item_array[itemid].loop, item_array[itemid].async_watcher);
        }
        item_array[itemid].loop = NULL;
        item_array[itemid].async_watcher = NULL;

        pthread_mutex_unlock(&item_lock);
    }
    return;
}

int update_item(unsigned int itemid, char* name, int interval, int status)
{
    if(itemid > 0 && item_enable == item_array[itemid].status)
    {
        pthread_mutex_lock(&item_lock);

        item_array[itemid].id = itemid;
        item_array[itemid].status = status;
        item_array[itemid].interval = interval;
        snprintf(item_array[itemid].name, sizeof(item_array[itemid].name), "%s", name);

        pthread_mutex_unlock(&item_lock);
        return 0;
    }
    return -1;
}

int update_item_loop(unsigned int itemid, struct ev_loop* loop, struct ev_async* a)
{
    if(itemid > 0 && item_enable == item_array[itemid].status)
    {
        pthread_mutex_lock(&item_lock);

        item_array[itemid].loop = loop;
        item_array[itemid].async_watcher = a;

        pthread_mutex_unlock(&item_lock);
        return 0;
    }
    return -1;
}

