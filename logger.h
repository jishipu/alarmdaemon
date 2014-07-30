#ifndef LOGGER_H_INCLUDED
#define LOGGER_H_INCLUDED

#include <errno.h>
#include <stdarg.h>

#define DEBUG 0

#define LOG_NAME "/var/log/yakmsg.log"
#define LOG_BUF 1024
#define LOGGER_WARN "WARN"
#define LOGGER_ERROR "ERROR"
#define LOGGER_DEBUG "DEBUG"
#define LOGGER_INFO "INFO"
#define LOGGER_FATAL "FATA"

void openlogger();
void logger(char *param,const char *fmt,...);
void closelogger();

void info(const char *fmt,...);
void debug(const char *fmt,...);
void warn(const char *fmt,...);
void fatal(const char *fmt,...);
void error(const char *fmt,...);

#endif // LOGGER_H_INCLUDED
