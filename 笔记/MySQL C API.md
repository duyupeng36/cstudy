# MySQL C API

MySQL 官方提供了 C API ，我们可以通过一下命令安装

```shell
sudo apt install libmysqlclient-dev
```

安装完成后，头文件 `<mysql.h>` 就存储在了 `/usr/include/mysql` 目录中

```shell
dyp@ubuntu:~$ locate mysql.h  # 头文件
/usr/include/mysql/mysql.h

dyp@ubuntu:~$ locate libmysqlclient.so  # 库文件
/usr/lib/x86_64-linux-gnu/libmysqlclient.so
/usr/lib/x86_64-linux-gnu/libmysqlclient.so.21
/usr/lib/x86_64-linux-gnu/libmysqlclient.so.21.2.40
```

使用 MySQL C API 时，就需要包含头文件 `<mysql.h>`。详细内容可以参考文档  [[MySQL-c-api-9.1-en.a4.pdf]]

## MySQL C API 数据结构




