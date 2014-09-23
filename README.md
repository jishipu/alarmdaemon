做监控报警项目时，遇到一些需要解决的问题：

  有很多的监控项，每个监控项的轮执时间和执行时长可能都不一样，如有的每10分钟执行一次，有的每1个小时执行一次。
  要保证每个监控项在本轮次没有执行完成之前（如执行超时等），下一轮次的执行不能触发。
  每个监控项的轮执时间可以随时被修改和删除，修改后要尽快的按照新的轮执时间执行。

如果用crontab来完成工作，显然太复杂了，至少要解决频繁的修改删除问题，另外也不能保证快速的执行新的轮执时间等。
libev有ev_periodic这个功能。所以基于libev写了alarmdaemon这个小程序，目前运行已经有一个月了，运行正常。

基本思路：

 系统定义监控项数组MONITOR_ITEM。主进程每隔5分钟请求一次mysql数据库，获取最新监控项目，更新MONITOR_ITEM。
 如果是新的监控项，新建一个业务线程做轮执业务操作，如果是修改间隔时间，停止当前业务线程，新建一个业务线程。

程序安装：

 解压libev-4.15.tar.gz,进入libev-4.15

    ./configure
    make
    make install

 假定mysql安装目录在/data/webserver/mysql

    ln -s /data/webserver/mysql/include /usr/include/mysql
    ln -s /data/webserver/mysql/lib /usr/lib64/mysql  #运行在64位操作系统下
    在/etc/ld.so.conf中添加/usr/lib64/mysql  运行ldconfig

 进入alarmdaemon目录，运行make,生成的运行程序在bin目录下
