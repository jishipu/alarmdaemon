1.解压libev-4.15.tar.gz,进入libev-4.15
  ./configure
  make
  make install
2.假如mysql安装目录在/data/webserver/mysql
  ln -s /data/webserver/mysql/include /usr/include/mysql
  ln -s /data/webserver/mysql/lib     /usr/lib64/mysql  #运行在64位操作系统下
  在/etc/ld.so.conf中添加/usr/lib64/mysql  运行ldconfig
3.进入alarmdaemon目录，运行make,生成的运行程序在bin目录下