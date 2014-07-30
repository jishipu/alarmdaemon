#ifndef COMMON_H_INCLUDED
#define COMMON_H_INCLUDED

#define CURL_URL "http://www.example.com/alarm"
#define MESSAGE_BUF 1024
#define YAKMSG_PID "/var/run/yakmsg.pid"

int savepid(char *path,int pid);
int removepid(char *path);
int isrunning(char *path);

char CURRENT_DIR[256];

#endif // COMMON_H_INCLUDED
