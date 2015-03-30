#ifndef MONITOR_ITEM_H_INCLUDED
#define MONITOR_ITEM_H_INCLUDED

#define MAX_ITEMID  100000
#define BUFSIZE 1024

enum status_item {item_disable, item_enable};

//每一个监控项
typedef struct item MONITOR_ITEM;

struct item {
	unsigned int        id;
	char	            name[60];
	int		            interval;      //执行间隔
	struct ev_loop*     loop;
    struct ev_async*    async_watcher;
	enum status_item    status;
};

void init_item();
void enable_item(unsigned int itemid);
void disable_item(unsigned int itemid);
int  update_item(unsigned int itemid, char* name, int interval, int status);
int update_item_loop(unsigned int itemid, struct ev_loop* loop, struct ev_async* a);
int get_item_status(unsigned int itemid);
struct ev_loop* get_item_loop(unsigned int itemid);
int get_item_interval(unsigned int itemid);
char* get_item_name(unsigned int itemid);

#endif // MONITOR_ITEM_H_INCLUDED
