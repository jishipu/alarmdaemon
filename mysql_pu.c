#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <ev.h>
#include <mysql/mysql.h>
#include "common.h"
#include "monitor_item.h"
#include "logger.h"
#include "functions.h"
#include "mysql_pu.h"

#define SOCKET_TYPE NULL
#define CLIENT_ARG 0

static MYSQL* connectToServer();
static void showMysqlError(MYSQL* connect);

static MYSQL * connectToServer()
{
    MYSQL* connect;
    connect = mysql_init(NULL);
    if(!connect)
    {
        fatal("cannot init mysql\n");
        goto ErrorConnectToServer;
    }

    FILE *fp;
    char cmd[1024];
    char buf[BUFSIZE];
    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd, sizeof(cmd), "curl -s --connect-timeout 60 %s/monitor/dbinfo ", CURL_URL);
    if((fp=popen(cmd, "r")) == NULL)
    {
        fatal("popen: %s\n", strerror(errno));
        goto ErrorConnectToServer;
    }
    while((fgets(buf, BUFSIZE, fp)) != NULL);
    pclose(fp);
    char host[20];
    char dbname[20];
    char user[50];
    char password[50];
    char port[10];
    sscanf(buf, "%[^|]|%[^|]|%[^|]|%[^|]|%[^|]", user, password, host, dbname, port);

    if(!mysql_real_connect(connect, host, user, password, dbname, atoi(port), SOCKET_TYPE, CLIENT_ARG))
    {
        showMysqlError(connect);
        mysql_close(connect);
        connect = NULL;
    }
ErrorConnectToServer:
    return connect;
}

void db_select_items()
{
    MYSQL* connect;
    MYSQL_RES* res;
    MYSQL_ROW sqlRow;
    char sql[256] = {};

    sprintf(sql, "select `wid`,`name`,`interval`,`isdel` from warn");
    connect = connectToServer();
    if(!connect) return;

    if(mysql_real_query(connect, sql, strlen(sql)))
    {
        showMysqlError(connect);
        mysql_close(connect);
        return;
    }
    res = mysql_store_result(connect);
    if(!res) {
        showMysqlError(connect);
        mysql_close(connect);
        return;
    }

    int exist[MAX_ITEMID];
    memset(exist, 0, MAX_ITEMID);
    while((sqlRow = mysql_fetch_row(res)))
    {
        int itemid = atoi(sqlRow[0]);
        int interval = atoi(sqlRow[2]);
        if(itemid >= MAX_ITEMID || itemid <= 0 || interval <= 0) continue;
        interval = interval * 60;
        int status = atoi(sqlRow[3]) == 0 ? 1 : 0;
        run_item(itemid, sqlRow[1], interval, status);
        exist[itemid] = itemid;
    }
    int j = 0;
    for(; j < MAX_ITEMID; j++)
    {
        if(exist[j] <= 0) {
            disable_item(j);
        }
    }

    mysql_free_result(res);
    mysql_close(connect);
    connect = NULL;
    return;
}

static void showMysqlError(MYSQL* connect)
{
    fatal("mysqlerror:%d:%s\n", mysql_errno(connect), mysql_error(connect));
}
