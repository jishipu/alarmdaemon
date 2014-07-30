#ifndef FUNCTIONS_H_INCLUDED
#define FUNCTIONS_H_INCLUDED

void check_item_cb(struct ev_loop *loop, ev_periodic * w, int revents);
void print_current_time();
pthread_t create_worker(void *(*func)(void *), void *arg);
void time_cb(struct ev_loop *loop, ev_periodic * w, int revents);
void* periodic_monitor(void * arg);
void run_item(unsigned int itemid, char* name, int interval, int status);
int business_process(unsigned int itemid);
void async_cb(struct ev_loop *loop, ev_async * w, int revents);

#endif // FUNCTIONS_H_INCLUDED
