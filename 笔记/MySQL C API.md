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

下表列出来 MySQL C API 涉及的常用数据结构

| 数据结构          | 描述                                                  |
| :------------ | :-------------------------------------------------- |
| `MYSQL`       | 代表数据连接，不要尝试复制它                                      |
| `MYSQL_RES`   | 代表返回行的查询结果. `SELECT` `SHOW` `DESCRIBE` `EXPLAIN` 语句 |
| `MYSQL_ROW`   | 代表类型安全的一行数据。目前被实现为一个字符串数据                           |
| `MYSQL_FIELD` | 代表字段信息的元数据。例如，字段名，类型，大小                             |

## MySQL C API 连接 MySQL Server

与 MySQL Server 建立连接需要按顺序调用下面几个接口

| 函数                     | 描述                 |
| :--------------------- | :----------------- |
| `mysql_library_init()` | 初始化 MySQL 客户端库     |
| `mysql_init()`         | 仅仅为 `MYSQL` 结构分配内存 |
| `mysql_real_connect()` | 建立真正的连接            |
|                        |                    |
| `mysql_close()`        | 关闭连接               |
| `mysql_library_end()`  | 结束使用 MySQL 客户端库    |

在调用任何其他 MySQL 函数之前，需要调用函数 `mysql_library_init()` 用于初始化 MySQL 客户端库

```c
#include <mysql/mysql.h>

int mysql_library_init(int argc, char **argv, char **groups)
/* 成功返回 0；失败返回 非零值 */
```

> [!attention] 
> 
> 参数 `argc`, `argv` 和 `groups` 未使用。在旧版的 MySQL 中，它们被用于与嵌入式服务器链接的应用程序，现在不再受支持
> 
> 现在的调用应该被写成 `mysql_library(0, NULL, NULL)`
> 

> [!tip] 函数 `mysql_library_init()` 不是线程安全函数：因此，必须在创建其他线程之前调用 `mysql_library_init()`函数
> 
> 在非多线程环境中，可以省略 `mysql_library_init()` 的调用，因为 `mysql_init()` 会在必要时自动调用 `mysql_library_init()` 
> 
> 在多线程环境中，由于 `mysql_library_init()` 不是线程安全函数，因此 `mysql_init()` 也不是线程安全函数。所以，必须在创建其他线程之前调用 `mysql_library_init()`
> 

为了避免应用程序使用库完成后发生内存泄漏，例如，在关闭与 MySQL 服务端连接之后，需要显示调用 `mysql_library_end()`

```c
#include <mysql/mysql.h>

void mysql_library_end(void);
```

下面是初始化 MySQL 客户端库的示例代码

```c title:mysql/t_mysql_library_init.c
#include <stdio.h>

#include <mysql/mysql.h>


int main() {
    if (mysql_library_init(0, NULL, NULL) != 0) {
        fprintf(stderr, "mysql_library_init: could not initialize MySQL client library");
        return 1;
    }

    /*
     * 调用其他 MySQL 函数
     */

    mysql_library_end();
}
```

函数 `mysql_init()` 用于分配或初始化一个 `MYSQL` 结构，该结构适用于 `mysql_real_connect()`

```c
#include <mysql/mysql.h>

MYSQL * mysql_init(MYSQL *mysql);
/* 成功返回 指向 MYSQL 结构的指针；失败返回 NULL*/
```

> [!tip] 参数 `mysql`：指向一个待初始化的 `MYSQL` 结构
> 
> 如果该参数为 `NULL`，则该函数分配并初始化一个 `MYSQL` 结构；否则初始化 `mysql` 执行的 `MYSQL` 结构
> 

> [!attention] 
> 
> 请注意，`mysql_init()` 之后初始化一个 `MYSQL` 结构，并不会与 MySQL Server 建立真正的连接
> 

---

为了与 MySQL Server 建立真正的连接，需要调用 `mysql_real_connect()` 函数，该函的原型如下

```c
#include <mysql/mysql.h>

MYSQL * mysql_real_connect(MYSQL *mysql, const char *host,  
                                  const char *user, const char *passwd,  
                                  const char *db, unsigned int port,  
                                  const char *unix_socket,  
                                  unsigned long clientflag);
/* 成功返回 参数 mysql; 失败返回 NULL */
```

> [!tip] 参数 `mysql`：执行已初始化的 `MYSQL` 结构
> 
> 该参数通常是被 `mysql_init()` 初始化的 `MYSQL` 结构
> 

> [!tip] 参数 `host`：MySQL Server 的主机名
> 
> 该参数如果为 `NULL` 或者 `"localhost"`，客户端使用 `Unix socket` 文件进行连接
> + Unix SOCKET 文件由参数 `unix_socket` 指定，或者由环境变量 `MYSQL_UNIX_PORT` 指定
> 
> 其他情况使用 TCP/IP 协议  

> [!tip] 参数 `user`：指定登录用户名
> 
> 如果该参数为 `NULL` 或者 `""`，则假设为当前用户
> + 在 Unix/Linux 中，是当前登录的用户名 
> 

> [!tip] 参数 `passwd`：指定 `user` 用户的登录密码
> 
> 如果 `passwd` 为 `NULL`，则只检查用户表中具有空密码字段的用户的条目是否匹配。
> 
 
> [!tip] 参数 `db`：指定连接的数据库名
> 
> 如果 `db` 为 `NULL`，则该连接会将默认数据库设置为此值
> 

> [!tip] 参数 `port`：指定 MySQL Server 占用的端口
> 
> 如果 `port` 不为 $0$，才将其作为 TCP/IP 连接的目标端口
> 

> [!tip] 参数 `unix_socket`：SOCKET 文件的目录，或者命名管道
> 
> 只有当参数 `host` 为 `NULL` 或者 `"localhost"` 是，才需要执行该值。否则，填 `NULL`
> 

> [!tip] 参数 `client_flag`：控制 `mysql_real_connect()` 的行为
> 
> 该参数通常为 `0`，但是可以指定下列位掩码启用某些功能
> + `CAN_HANDLE_EXPIRED_PASSWORDS`：处理过期密码
> + `CLIENT_COMPRESS`：启用压缩
> + `CLIENT_FOUND_ROWS`：返回已找到的行数，而不是已更改的行数
> + `CLIENT_IGNORE_SIGPIPE`：忽略 `SIGPIPE` 信号。类似与 SOCKET 的 `send()` 函数的 `MSG_NOSIGNAL` 选项
> 
> 更多选项参考 [[MySQL-c-api-9.1-en.a4.pdf#page=96&selection=211,0,216,23|MySQL-c-api-9.1-en.a4, 页面 96]]
> 

参数 `host` `user` `passwd` `db` `port` `unix_socket` 可以从配置文件中读取，而不是在 `mysql_real_connect()` 中显示指定。如需要使用配置文件，需要在调用 `mysql_real_connect()` 之前，调用 `mysql_options()` 从配置文件读取参数，然后在 `mysql_real_connect()` 中相应参数指定为 `no-value`

| 参数            | `no-value`    |
| :------------ | ------------- |
| `host`        | `NULL` 或 `""` |
| `user`        | `NULL` 或 `""` |
| `passwd`      | `NULL`        |
| `db`          | `NULL` 或 `""` |
| `unix_socket` | `NULL`        |

---

函数 `mysql_options()` 用于设置额外的连接选项和影响连接的行为。可以被多次调用用来设置多个选项

```c
#include <mysql/mysql.h>

int mysql_options(MYSQL *mysql, enum mysql_option option, const void *arg);

/* 成功返回 0；失败返回非零 */
```

> [!tip] 参数 `mysql`：被初始化后的 `MYSQL` 结构
> 

> [!tip] 参数 `option`：指定 `mysql_options()` 的行为
> 
> 下表给出某些选项。更多选项参考 [[MySQL-c-api-9.1-en.a4.pdf#page=84&selection=156,0,165,64|MySQL-c-api-9.1-en.a4, 页面 84]]

| 参数 `option`                        | 描述                                        | `arg` 类型 |
| :--------------------------------- | :---------------------------------------- | :------- |
| `MYSQL_DEFAULT_AUTH`               | 指定身份认证插件名字                                | `char *` |
| `MYSQL_ENABLE_CLEARTEXT_PLUGIN`    | 是否启用明文身份验证插件                              | `bool *` |
| `MYSQL_INIT_COMMAND`               | 初始 SQL 语句                                 | `char *` |
| `MYSQL_OPT_BIND`                   | 指定连接 MySQL 服务器的网络接口                       | `char *` |
| `MYSQL_OPT_COMPRESS`               | 启用压缩                                      | 无        |
| `MYSQL_OPT_COMPRESSION_ALGORITHMS` | 启用压缩，MySQL 8.0.18 起                       | `char *` |
| `MYSQL_READ_DEFAULT_FILE`          | 从 `arg` 指定文件的 `[client]` 段中读取选项           | `char *` |
| `MYSQL_READ_DEFAULT_GROUP`         | 从`my.cnf` 中的 `[client]`  和 `[arg]` 段中读取选项 | `char *` |

> [!tip] 参数 `arg`：根据 `option` 参数解释该参数的值
> 

---

如果 `mysql_init()` 分配了一个新对象，则在调用 `mysql_close()` 关闭连接，并同时释放它

```c
#include <mysql/mysql.h>

void mysql_close(MYSQL *mysql);

```

> [!tip] 参数 `mysql`：指定待关闭的 MySQL 连接
> 
> 如果 `mysql` 是由 `mysql_init()` 或者 `mysql_connect()` 自动分配的，则 `mysql_close()` 会释放该内存
> 

使用 MySQL 客户端库的代码结构如下。这里，我们通过 `config.cnf` 文件配置选项

```plaintext title:config.cnf
[client]
host=127.0.0.1
user=root
password=xxx
```

```c title:mysql/mysql_connect.c
#include <stdio.h>

#include <mysql/mysql.h>


int main() {
    if (mysql_library_init(0, nullptr, nullptr) != 0) {
        fprintf(stderr, "mysql_library_init: could not initialize MySQL client library");
        return 1;
    }
    // 初始化 MySQL 结构
    MYSQL mysql;
    if (!mysql_init(&mysql)) {
        fprintf(stderr, "mysql_init: could not initialize MySQL client");
        return 1;
    }
    // 读取配置文件
    if (mysql_options(&mysql, MYSQL_READ_DEFAULT_FILE, "./config.cnf") != 0) {
        fprintf(stderr, "mysql_options: could not read config from .cnf file");
        return 1;
    }
    
    // 建立连接
    if (!mysql_real_connect(&mysql, nullptr, nullptr, nullptr, nullptr, 0, nullptr, 0)) {
        fprintf(stderr, "mysql_real_connect: %s", mysql_error(&mysql));
        return 1;
    }

    printf("Connected\n");
    
    /*
    执行 SQL
    */
    
    // 关闭连接
    mysql_close(&mysql);
    mysql_library_end();

    return 0;
}
```

## 执行 SQL 语句

如果需要执行 SQL 语句，则需要调用 `mysql_query()` 函数

```c
#include <mysql/mysql.h>

int mysql_query(MYSQL *mysql, const char *query);
/* 成功返回 0；失败返回非零 */
```

> [!tip] 参数 `mysql`：与 MySQL Server 建立连接的 `MYSQL` 结构
> 
> 通常是 `mysql_real_connect()` 之后的 `MYSQL` 结构
> 

> [!tip] 参数 `query`：待执行的 SQL 语句

如果执行 SQL 出现错误，可以调用 `mysql_error()` 获取错误描述

```c
#include <mysql/mysql.h>
const char * mysql_error(MYSQL *mysql);
/* 成功返回指向错误描述的字符指针 */
```

## 读取 SQL 语句的结果

对于写入类型的 SQL 语句(例如，`INSERT`)，通常不关心返回的数据。对于读取类型的 SQL 语句(例如，`SELECT` `DESC` 等)，如果需要获取 **行集合**，请调用函数 `mysql_store_result()`

```c
#include <mysql/mysql.h>

MYSQL_RES *mysql_store_result(MYSQL *mysql);
/* 成功返回指向 MYSQL_RES 的指针；失败返回 NULL */
```

如果需要操作结果集，请使用下表列出的函数

| 函数                     | 描述        |
| :--------------------- | --------- |
| `mysql_num_fields()`   | 结果集的列数    |
| `mysql_num_rows()`     | 结果集的行数    |
| `mysql_fetch_row()`    | 从结果集中读取一行 |
| `mysql_fetch_fields()` | 获取列的元数据   |

函数 `mysql_num_fields()` 用于获取 `MYSQL_RES` 结果集包含的列数

```c
#include <mysql/mysql.h>

unsigned int mysql_num_fields(MYSQL_RES *res);
/* 始终返回结果集的列数 */

uint64_t mysql_num_rows(MYSQL_RES *res);
/* 始终返回结果集的行数 */

MYSQL_ROW mysql_fetch_row(MYSQL_RES *result);
/* 成功返回 MYSQL_ROW 指针；失败返回 NULL */

MYSQL_FIELD * mysql_fetch_fields(MYSQL_RES *res);
/* 成功返回 MYSQL_FIELD 指针；失败返回 NULL */
```

下面的示例代码给出了 C API 的使用

```c title:mysql/mysql_connect.c
#include <stdio.h>

#include <mysql/mysql.h>


int main() {
    /* 初始化 MySQL C API 库 */
    if (mysql_library_init(0, nullptr, nullptr) != 0) {
        fprintf(stderr, "mysql_library_init: could not initialize MySQL client library");
        return 1;
    }

    /* 初始化 MYSQL */
    MYSQL mysql;
    if (!mysql_init(&mysql)) {
        fprintf(stderr, "mysql_init: could not initialize MySQL client");
        return 1;
    }
    /* 读取配置选项 */
    if (mysql_options(&mysql, MYSQL_READ_DEFAULT_FILE, "./config.cnf") != 0) {
        fprintf(stderr, "mysql_options: could not read config from .cnf file");
        return 1;
    }
    /* 与 MySQL Server 建立真正的连接 */
    if (!mysql_real_connect(&mysql, nullptr, nullptr, nullptr, nullptr, 0, nullptr, 0)) {
        fprintf(stderr, "mysql_real_connect: %s", mysql_error(&mysql));
        return 1;
    }

    /* 执行 SQL */
    if (mysql_query(&mysql, "select * from employees") != 0) {
        fprintf(stderr, "mysql_query: %s", mysql_error(&mysql));
        return 1;
    }

    /* 提取结果集 */
    MYSQL_RES *result = mysql_store_result(&mysql);
    if (result == nullptr) {
        fprintf(stderr, "mysql_store_result: %s", mysql_error(&mysql));
        return 1;
    }

    /* 获取结果集的列信息 */
    MYSQL_FIELD *field = mysql_fetch_fields(result);
    if (field == nullptr) {
        fprintf(stderr, "mysql_fetch_fields: %s", mysql_error(&mysql));
        return 1;
    }
    for (unsigned int i = 0; i < mysql_num_fields(result); i++) {
        printf("%s\t", field[i].name);
    }
    printf("\n");

    /* 获取结果集的行信息 */
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result)) != nullptr) {
        for (unsigned int i = 0; i < mysql_num_fields(result); i++) {
            printf("%s\t", row[i]);
        }
        printf("\n");
    }

    /* 关闭与 MySQL Server 的连接 */
    mysql_close(&mysql);

    /* 关闭 MySQL C API 库 */
    mysql_library_end();

    return 0;
}

```
