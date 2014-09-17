#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <syslog.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "logger.h"

int logfd=-1;
char curdate[9], logdate[9]; //判断日志是否应该写到下一天的日志中
static void ntoa();
static void error_ret();
static void err_quit(char *msg);
static void logging();

static void err_quit(char *msg){
	error_ret(msg);
        exit(EXIT_FAILURE);
}

static void error_ret(char *msg){
	openlog("logger",LOG_PID,LOG_DAEMON);
	syslog(LOG_ERR,"%s:%s\n",msg,strerror(errno));
	closelog();
        errno=0;
}

static int gettime(char *buf){
        time_t t=time(NULL);
	struct tm *time=localtime(&t);
	char mon[3],mday[3],hour[3],min[3],sec[3];
	int year = time->tm_year + 1900;
	ntoa(time->tm_mon + 1,mon);
	ntoa(time->tm_mday,mday);
	ntoa(time->tm_hour,hour);
	ntoa(time->tm_min,min);
	ntoa(time->tm_sec,sec);
	snprintf(curdate, sizeof(curdate), "%d%s%s", year, mon, mday);
	int len=snprintf(buf,20,"%d-%s-%s %s:%s:%s",year,mon,mday,hour,min,sec);
	return len;
}

static int getlogname(char *buf){
        time_t t=time(NULL);
	struct tm *time=localtime(&t);
	char mon[3],mday[3];
	int year = time->tm_year + 1900;
	ntoa(time->tm_mon + 1,mon);
	ntoa(time->tm_mday,mday);
	snprintf(logdate, sizeof(logdate), "%d%s%s", year, mon, mday);
	int len=snprintf(buf, LOG_BUF, "%s.%d%s%s",LOG_NAME, year,mon,mday);
	return len;
}

static void ntoa(int n,char *ch){
	if(n<10) {ch[0]='0';ch[1]=n+48;ch[2]='\0';}
	else snprintf(ch,3,"%d",n);
}

void openlogger(){
        char buf[LOG_BUF];
        getlogname(buf);
	logfd=open(buf,O_CREAT|O_WRONLY|O_APPEND,0644);
	if(logfd<0) error_ret("open");
}

void logger(char *param,const char *fmt,...){
	va_list ap;
	va_start(ap,fmt);
	logging(param,fmt,ap);
	va_end(ap);
}

static void logging(char *param,const char *fmt,va_list ap){
	if(logfd==-1) {
	   openlogger();
	} else if(strcmp(curdate, logdate)>0) {
	   closelogger();
	   openlogger();
	}

	struct flock lock;
	lock.l_whence=SEEK_SET;
	lock.l_start=0;
	lock.l_len=1;
	lock.l_type=F_WRLCK;
	if(fcntl(logfd,F_SETLKW,&lock)<0) error_ret("fcntl lock");

	char buf[LOG_BUF];
	gettime(buf);
	snprintf(buf+strlen(buf),LOG_BUF,"\t%s\t",param);
	vsnprintf(buf+strlen(buf),LOG_BUF,fmt,ap);
	if(write(logfd,buf,strlen(buf))<0) error_ret("write");

	lock.l_type=F_UNLCK;
	if(fcntl(logfd,F_SETLKW,&lock)<0) error_ret("fcntl unlock");
}

void closelogger(){
	close(logfd);
}

void info(const char *fmt,...){
	va_list ap;
	va_start(ap,fmt);
	logging(LOGGER_INFO,fmt,ap);
	va_end(ap);
}

void debug(const char *fmt,...){
	if(DEBUG){
		va_list ap;
		va_start(ap,fmt);
		logging(LOGGER_DEBUG,fmt,ap);
		va_end(ap);
	}
}

void warn(const char *fmt,...){
	va_list ap;
	va_start(ap,fmt);
	logging(LOGGER_WARN,fmt,ap);
	va_end(ap);
}

void fatal(const char *fmt,...){
	va_list ap;
	va_start(ap,fmt);
	logging(LOGGER_FATAL,fmt,ap);
	va_end(ap);
}

void error(const char *fmt,...){
	va_list ap;
	va_start(ap,fmt);
	logging(LOGGER_ERROR,fmt,ap);
	va_end(ap);
}

