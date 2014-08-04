<p>
    1.解压libev-4.15.tar.gz,进入libev-4.15
</p>
<p>
    &nbsp;&nbsp;&nbsp;&nbsp;./configure
</p>
<p>
    &nbsp;&nbsp;&nbsp;&nbsp;make
</p>
<p>
    &nbsp;&nbsp;&nbsp;&nbsp;make install
</p>
<p>
    2.假如mysql安装目录在/data/webserver/mysql
</p>
<p>
    &nbsp;&nbsp;&nbsp;&nbsp;ln -s /data/webserver/mysql/include /usr/include/mysql
</p>
<p>
    &nbsp;&nbsp;&nbsp;&nbsp;ln -s /data/webserver/mysql/lib &nbsp; &nbsp; /usr/lib64/mysql &nbsp;#运行在64位操作系统下
</p>
<p>
    &nbsp;&nbsp;&nbsp;&nbsp;在/etc/ld.so.conf中添加/usr/lib64/mysql &nbsp;运行ldconfig
</p>
<p>
    3.进入alarmdaemon目录，运行make,生成的运行程序在bin目录下
</p>
