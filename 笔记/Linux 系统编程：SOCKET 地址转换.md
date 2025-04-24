# SOCKET：地址转换

## 网络字节序

在 [[计算机组成：信息存储#寻址和字节序]] 中介绍了字节序：**大端字节序** 和 **小端字节序**

> [!tip] 大端字节序：最高有效字节到最低有效字节进行排序
> 
> 换句话说，**高有效字节放在低地址处**
> 

> [!tip] 小端字节序：最低有效字节到最高有效字节进行排序
> 
> 换句话说，低有效字节放在低地址处
> 

下图展示了 $2$ 字节和 $4$ 字节整数的大端和小端字节序

![[Pasted image 20241121174150.png]]

> [!important] 
> 
> 存储整数时先存储最高有效字节的被称为大端，那些先存储最低有效字节的被称为小端
> 

**IP 地址和端口号是整数值**。在将这些值在网络中传递时碰到的一个问题是 **不同的硬件结构会以不同的顺序来存储一个多字节整数的字节**

> [!tip] 
> 
> 小端架构中最值得关注的是 x86。其他群大多数架构都是大端的
> 
> 一些硬件结构可以在这两种格式之间切换。**在特定主机上使用的字节序** 被称为 **主机字节序**
> 

由于端口号和 IP 地址必须在网络中的所有主机之间传递并且需要被它们所理解，因此必须要使用一个标准的字节序。这种字节序被称为 **网络字节序**，它是 **大端的**

> [!summary]
> 
>  网络字节序是大端的
>  

因为可能会按照下列方式使用 IP 地址和端口号
+ 整数常量
+ 通过命令行参数传
+ IPv4 地址可以使用一些预定义宏：`INADDR_ANY`(通配地址) 和 `INADDR_LOOPBACK`(本地回环地址)
+ IPv6 地址同样可以使用预定义宏：`IN6ADDR_ANY_INIT`(通配地址)和 `IN6ADDR_LOOPBACK_INIT`(本地回环地址)

> [!important] 上述这些值是在 C 语言中按照 **主机字节序** 规则表示
> 
> 为了使用这些值，需要 **将这些值转换为网络字节序**，才能将它们存入 SOCKET 地址结构中
> 

`htons()`、`htonl()`、`ntohs()`以及 `ntohl()` 函数用来在主机和网络字节序之间转换整数

```c
#include <arpa/inet.h>

uint16_t htons(uint16_t host_uint16);  // 将 16 位的主机字节序转换为网络字节序
uint32_t htonl(uint32_t host_uint32);  // 将 32 位的主机字节序转换为网络字节序

uint16_t ntohs(uint16_t net_uint16);  // 将 16 位的网络字节序转换为主机字节序
uint32_t ntohl(uint32_t net_uint32);  // 将 32 位的网络字节序转换为主机字节序
```

严格地讲，只需要在主机字节序与网络字节序不同的系统上使用这四个函数，但开发人员应该总是使用这些函数，这样程序就能够在不同的硬件结构之间移植了。在主机字节序与网络字节序一样的系统上，这些函数只是简单地原样返回传递给它们的参数

```c title:sockets/htons.c
/**
 * 字节序转换函数：<arpa/inet.h> 中的 
 * - htons: host to net short，主机字节序转换为网络字节序，类型为 uint16_t
 * - htonl: host to net long, 主机字节序转换为网络字节序，类型为 uint32_t
 * - ntohs: net to host short, 网络字节序转换为主机字节序，类型为 uint16_t
 * - ntohl: net to host long, 网络字节序转换为主机字节序，类型为 uint32_t
 * 
 * 早期这些函数的原型为：unsigned long htonl(unsigned long host)
*/

#include <arpa/inet.h>

#include "base.h"

typedef unsigned char * byte_pointer;

void show_bytes(byte_pointer start, size_t len) {
    size_t i;
    for (i = 0; i < len; i++) {
        printf("%.2x", start[i]);
    }
    printf("\n");
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        usageErr("%s port\n", argv[0]);
    }

    uint16_t port = strtoul(argv[1], nullptr, 10);
    printf("host bytes order: ");
    show_bytes((byte_pointer) &port, sizeof(uint16_t));
    uint16_t net_port = htons(port);
    printf("net bytes order: ");
    show_bytes((byte_pointer) &net_port, sizeof(uint16_t));
}
```

## Internet socket 地址

> [!tip] Internet domain 包含 `AF_INET` 和 `AF_INET6`，它们的地址结构不同 
> 
> + `AF_INET` 使用 IPv4 地址
> + `AF_INET6` 使用 IPv6 地址
> 

### IPv4 SOCKET 地址

一个 IPv4 SOCKET 地址会被存储在一个 `sockaddr_in` 结构中，该结构在 `<netinet/in.h>` 中进行定义，具体如下

```c
typedef uint32_t in_addr_t;

struct in_addr{
	in_addr_t s_addr;
};

struct sockaddr_in
{
	sa_family_t sin_family;  // 地址族(协议族)，始终是 AF_INET
	in_port_t sin_port;      // 端口号
	struct in_addr sin_addr; // IPv4 地址
	unsigned char __pad[X];  // 填充到 sockaddr 结构
};
```

在 [[Linux 系统编程：SOCKET 概览#将 SOCKET 绑定到地址]] 中，我们介绍了 `sockaddr` 结构，其中一个字段 `sockaddr.sin_family`，对应于 `sockaddr_in.sin_family`，该字段的值始终为 `AF_INET`

字段 `sockaddr_in.sin_port`  和 `sockaddr_in.sin_addr` 字段是 **端口号** 和 **IP 地址**，它们都是 **网络字节序**

> [!tip] `in_port_t` 和 `in_addr_t` 数据类型是 **无符号整型**，其长度分别为 $16$ 位和 $32$ 位
>

> [!tip] 特殊地址：IPv4 的 **通配地址** 和 **本地回环地址**
> 
> 通配地址：每一位均为 $0$，使用点分十进制表示为 `"0.0.0.0"`
> + 可以直接使用预定宏 `INADDR_ANY`  
> 
> 本地回环地址：使用点分十进制表示为 `"127.0.0.1"`
> + 可以使用预定义宏 `INADDR_LOOPBACK` 
> 
> 这两个预定义宏可以用于给 `sin_addr.s_addr` 字段进行赋值。不过需要注意转换字节序
> 

### IPv6 SOCKET 地址

一个 IPv6 SOCKET 地址包含一个 **IP 地址** 和一个 **端口号**

> [!attention] IPv6 的 IP 地址是 $128$ 位
> 

一个 IPv6 SOCKET 地址会被存储在一个 `sockaddr_in6` 结构中，该结构在 `<netinet/in.h>` 中进行定义，具体如下

```c
struct in6_addr{
	uint8_t s6_addr[16];
};

struct sockaddr_in6
{
	sa_family_t sin6_family;  // 地址族(协议族)，始终是 AF_INET6
	in_port_t sin6_port;      // 端口号
	uint32_t sin6_flowinfo;   // IPv6 流量控制信息
	struct in6_addr sin_addr; // IPv6 地址
	uint32_t sin6_scop_id     // 
};
```

+ 字段 `sockaddr_in6.sin_family` 字段始终被设置成 `AF_INET6`

+ 字段 `sockaddr_in6.sin6_port` 和 `sockaddr_in6.sin6_addr` 分别是端口号和 IP地址

+ 字段 `sockaddr_in6.sin6_flowinfo` 和 `sockaddr_in6.sin6_scope_id` 我们不研究，后续如果语句将其设置为 $0$

> [!attention] 
> 
> `uint8_t` 数据类型被用来定义 `in6_addr` 结构中字节的类型，它是一个 8 位的无符号整型
> 
> `sockaddr_in6` 结构中的所有字段都是以 **网络字节序** 存储
> 

> [!tip] IPv6 和 IPv4 一样也有 **通配** 和 **回环** 地址，但它们的用法要更加复杂一些，因为 **IPv6 地址是存储在数组中的**，并没有使用标量类型
> 
> 通配地址：每一位均为 $0$。十六进制分组表示为 `"0::0"`
> + 可以使用预定义宏 `IN6ADDR_ANY_INIT`
>   
> 本地回环地址：十六进制分组表示为 `"::1"`
> + 可以使用预定义宏 `IN6ADDR_LOOPBACK_INIT`
>
>这两个预定义宏也是 **只能用于初始化器**，不能用于赋值
>

下面是 IPv6 通配地址 `IN6ADDR_ANY_INIT` 的定义如下

```c
#define IN6ADDR_ANY_INIT {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}}
```

上述定义中，只能在变量声明的初始化器中可以使用 `IN6ADDR_ANY_INIT` 常量，不能在一个赋值语句的右边使用这个常量

> [!tip] 
> 
> C 语法并不允许在赋值语句中使用一个结构化的常量
> 

要使用 `IN6ADDR_ANY_INIT`，只能先定义一个变量，并使用这个两个预定宏进行初始化。

```c
const struct in6_addr in6addr_any = IN6ADDR_ANY_INIT;
```

然后，就可以向下面的方式使用这个通配地址来初始化一个 IPv6 SOCKET 地址

```c
struct sockaddr_in6 addr;
memset(&addr, sizeof(struct sockaddr_in6));

addr.sin6_family = AF_INET6;
addr.sin6_addr = in6addr_any;
addr.sin6_port = htons(SOME_PORT_NUM);
```

> [!important] 
> 
> 与 IPv4 中相应字段不同的是，**IPv6 的常量和变量初始化器是网络字节序的**
> 

> [!attention]  如果 **IPv4 和 IPv6**  共存于一台主机上，那么它们将 **共享同一个端口号空间**
> 
> 这意味着如果一个应用程序将一个 IPv6 socket 绑定到了 TCP 端口 2000 上（使用 IPv6 通配地址），那么 IPv4 TCP socket 将无法绑定到同一个端口上
> 
> TCP/IP 实现确保位于其他主机上的 socket 能够与这个 socket 进行通信，不管那些主机运行的是 IPv4 还是 IPv6
> 

## IP 地址格式转换函数

`inet_pton()` 和 `inet_ntop()` 函数允许在 IPv4 和 IPv6 地址的 **二进制形式** 和**点分十进制表示法(IPv4)** 或 **十六进制字符串表示法(IPv6)** 之间进行转换

上述两个函数名中的 `p` 表示 **展现(presentation)**，`n` 表示 **网络(network)**。其中，展现形式是人类可读的字符串，例如
+ `204.152.189.116`（IPv4 点分十进制地址）
+ `::1`（IPv6 冒号分隔的十六进制地址）
+ `::FFFF:204.152.189.116`（IPv4 映射的 IPv6 地址）

函数 `inet_pton()` 将 IPv4 和 IPv6 的地址的展现形式转换为网络形式

```c
#include <arpa/inet.h>

int inet_pton(int domain, const char *src_str, void *addrptr);
/* 成功转换返回 1，失败返回 -1；src_str 表示的不是地址返回 0 */
```

> [!tip] 参数 `domain`：指定通信与
> 
> 该参数指定为 `AF_INET` 或 `AF_INET6`
> 

> [!tip] 参数 `src_str`：字符串表示的 IPv4 地址或 IPv6 地址
> 
> + IPv4 采用点分十进制表示
> + IPv6 采用十六进制分组表示
> 

> [!tip] 参数 `addrptr`：指向 `in_addr` 结构或 `in6_addr` 结构的缓冲区
> 
> + `in_addr` 结构代表了 IPv4 地址
> + `in6_addr` 结构代表了 IPv6 地址
> 

> [!tip] 返回值
> 
> + 成功转换返回 $1$
> + 失败返回 $-1$
> + 参数 `src_str` 如果不是地址，返回 $0$
> 

函数 `inet_ntop()` 将 IPv4 和 IPv6 的地址的网络形式转换为展现形式

```c
#include <arpa/inet.h>

const char * inet_ntop(int domain, const void *addrptr, char *dst_str, size_t len);
/* 返回 dst_str 的指针，错误返回 NULL */
```

> [!tip] 参数 `domain`：指定通信与
> 
> 该参数指定为 `AF_INET` 或 `AF_INET6`
> 

> [!tip] 参数 `addrptr`：指向待转换 `in_addr` 结构或 `in6_addr` 结构的指针

> [!tip] 参数 `dst_str`：指向存储字符串形式的 IPv4 和 IPv6 地址的数组

> [!tip] 参数 `len`：指定 `dst_str` 数组的最大长度

> [!tip] 返回值
> + 成功返回 `dst_str`
> + 失败返回 `NULL`，并将 `errno` 设置为 `ENOSPC`。通常，是因为参数 `len` 的值太小了
> 

要正确计算 `dst_str` 指向的缓冲器的大小可以使用在 `<netinet/in.h>` 中定义的两个常量，**这些常量标识出了 IPv4 和 IPv6 地址的展现字符串的最大长度**（包括结尾的 `NULL` 字节）

```c
#define INET_ADDRSTRLEN 16  /* IPv4 点分十进制字符串长度的最大值 */
#define INET6_ADDRSTRLEN 46 /* IPv6 十六进制字符串长度的最大值*/
```

## 客户端/服务器示例（数据报 socket）

这里给出一个大小写转换服务器和客户端示例程序，使用 `AF_INET` domain 中的数据报 SOCKET

```c title:sockets/ucase.h
#ifndef UCASE_H
#define UCASE_H

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define PORT_NUM 8080

#endif //UCASE_H
```

下面的代码给出了服务端的实现。

```c title:sockets/ucase_sv.c

#include <ctype.h>

#include "ucase.h"
#include "base.h"

int main() {
	// 首先，创建一个 AF_INET 的 SOCK_DGRAM SOCKET 
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        errExit("socket: ");
    }
	
	// 构建 SOCKET 地址
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;  // IPv4
    addr.sin_addr.s_addr = htonl(INADDR_ANY);  // 通配地址
    addr.sin_port = htons(PORT_NUM); // 端口

	// 然后，给 SOCET 绑定地址
    if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        errExit("bind: ");
    }
    
    // 数据报 SOCKET 不需要进行将其转换为被动套接字，因为数据报 SOCKET 是无连接的
    char buffer[BUFSIZ];
    while (true) {
        struct sockaddr_in client;
        socklen_t len = sizeof(struct sockaddr_in);
        
        // 直接读取数据
        ssize_t numBytes = recvfrom(sockfd, buffer, BUFSIZ, 0, (struct sockaddr *)&client, &len);
        if (numBytes == -1) {
            errExit("recvfrom: ");
        }
        
        // 将 SOCKET 地址转换为展现形式
        char claddr[INET_ADDRSTRLEN];
        if (inet_ntop(AF_INET, &client.sin_addr, claddr, INET_ADDRSTRLEN ) == NULL) {
            fprintf(stderr, "inet_ntop: %s", strerror(errno));
        } else {
            printf("Server received %ld bytes form(%s,%hu)\n", (long)numBytes, claddr, ntohs(client.sin_port));
        }
        
        // 转成大写
        for (int j = 0; j < numBytes; j++) {
            buffer[j] = (char)toupper(buffer[j]);
        }
        
        // 回复给客户端
        if (sendto(sockfd, buffer, numBytes, 0, (struct  sockaddr *)&client, len) != numBytes) {
            errExit("sendto: ");
        }
    }
}
```

下面代码给出客户端实现

```c title:sockets/ucase_cl.c
#include <base.h>
#include <unistd.h>

#include "ucase.h"

int main() {

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == 0) {
        errExit("socket");
    }
    
    // 此处的地址是服务端所在的地址
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = htons(PORT_NUM);

    char buffer[BUFSIZ];
    while (true) {
        memset(buffer, 0, BUFSIZ);
        ssize_t numRead = read(STDIN_FILENO, buffer, BUFSIZ);
        if (numRead == -1 || numRead == 0) {
            errExit("read");
        }

        if (sendto(sockfd, buffer, numRead, 0, (struct sockaddr*)&addr, sizeof(addr)) != numRead) {
            errExit("sendto");
        }

        numRead = recvfrom(sockfd, buffer, BUFSIZ, 0, nullptr, nullptr);
        if (numRead == -1) {
            errExit("read");
        }
        printf("%s\n", buffer);
    }
}
```


## 域名系统

IP 地址太过繁琐，难以记忆和使用，因此互联网支持使用 **主机名称(host name)** 来识别包括客户机和服务器在内的主机。为了使用 TCP 和 IP 等协议，主机名称通过 **名称解析** 的过程转换成 IP 地址

> [!tip] 名称解析：查询 IP 地址好比查询通讯录一样
> 
> 比如我们打电话的时候，必须要知道对方的电话号码，但由于电话号码难以记忆，所以通常我们会将对方 **电话号 + 姓名** 保存在通讯录里
> 

所以，有⼀种服务器就专⻔保存了 Web  服务器域名与 IP  的对应关系，它就是  **DNS  服务器**

> [!tip] DNS 服务器的 IP 实在配置网络时就知晓的
> 
> 本地 DNS 服务器通常由路由器充当
> 

DNS 中的域名都是用句点来分隔的，比如 `www.server.com `，这里的句点代表了不同层次之间的界限。在域名中，**越靠右的位置表示其层级越⾼**

> [!tip]
> 实际上域名最后还有⼀个点，比如 `www.server.com.` ，这个最后的⼀个点代表根域名
> 

也就是， `.`  根域是在最顶层，它的下⼀层就是 `.com`  顶级域，再下面是 `server.com`。所以域名的层级关系类似⼀个树状结构。DNS 服务器也是这样的树状结构

+ 根 DNS 服务器（.）
+ 顶级域 DNS 服务器（.com）
+ 权威 DNS 服务器（server.com）

![[Pasted image 20241123195028.png]]

> [!tip]
> 
>根域的 DNS 服务器信息保存在互联网中所有的 DNS 服务器中
>
>这样⼀来，任何 DNS 服务器就都可以找到并访问根域 DNS 服务器了
>
>因此，客户端只要能够找到任意⼀台 DNS 服务器，就可以通过它找到根域 DNS 服务器，然后再⼀路顺藤摸瓜找到位于下层的某台目标 DNS 服务器
>

下图展示 DNS 域名解析的过程

![[Pasted image 20241122102938.png]]


## 服务名

在 [[网络：链路层协议#地址#端口]] 中介绍那样，众所周知的端口号是由 IANA 集中注册的，其中 **每个端口都有一个对应的服务名**

> [!tip] 
> 
> 由于服务号是集中管理并且不会像 IP 地址那样频繁变化，因此没有必要采用 DNS 服务器来管理它们
> 

端口号和服务名会记录在文件 `/etc/services` 中

## 协议无关的解析函数

### getaddrinfo()

给定一个**主机名(域名)** 和 **服务器名**，`getaddrinfo()` 函数返回一个 SOCKET 地址结构列表，每个结构都包含一个地址和端口号

```c
#include <sys/socket.h>
#include <netdb.h>

int getaddrinfo(const char *host, const char *service, const struct addrinfo *hints, struct addrinfo **result);
/* 成功返回 0；失败返回非零 */
```

> [!tip] 参数 `host`：指向一个表示主机名或地址的字符串
> + 或者是 **域名**
> + 或者是 **字符串形式的 IP 地址**（如 `"192.168.1.1"` 或 `"::1"`）
> + 或者是 `NULL`，表示绑定到通配地址
> 

> [!tip] 参数 `service`：指定服务名
> 
> + 或者是 `/etc/services` 文件的第一列，例如 `"http"` 表示端口 $80$
> + 或者是端口号字符串（如 `"8080"`）
> + 或者是 `NULL`，表示未指定服务
> 

在介绍 `hints` 参数之前，首先给出 `addrinfo` 结构的定义

```c 
struct addrinfo
{
  int ai_flags;			/* 输入标签，通常是 AI_* 常量  */
  int ai_family;		/* 地址族. AF_* 常量 */
  int ai_socktype;		/* SOCKET 类型.SOCK_* 常量  */
  int ai_protocol;		/* 地址族中 SOCKET 类型支持协议的协议号.  */
  socklen_t ai_addrlen;		/* SOCKET 地址的长度.  */
  struct sockaddr *ai_addr;	/* SOCKET 地址结构的指针.  */
  char *ai_canonname;		/* 服务位置的规范名称.  */
  struct addrinfo *ai_next;	/* 指向列表中下一个的指针.  */
};
```

> [!tip] 参数 `hints`：指向一个可选的 `addrinfo` 结构，用于 **指定函数的查询行为**。
> 
> 指定函数的查询行为主要涉及 $4$ 个重要字段
> + `ai_family`：指定协议族
> 	+ `AF_INET`：IPv4 地址
> 	+ `AF_INET6`：IPv6 地址
> 	+ `AF_UNSPEC`：任何地址，包括 IPv4 和 IPv6 地址
> + `ai_socktype`：指定 SOCKET 类型
> 	+ `SOCK_STREAM`：流式 SOCKET 
> 	+ `SOCK_DGRAM`：数据报 SOCKET
> + `ai_protocol`：SOCKET 地址的协议号
> 	+ 通常为 $0$，表示查询所有协议
> 
> 前三个参数的取值参考 [[Linux 系统编程：SOCKET 概览#创建 SOCKET]]。这三个参数的值就是创建 `socket()` 传入的参数值
> 
> + `ai_flags`：设置查询标志
> 	+ `AI_PASSIVE`：查询通配地址，通常用于服务器
> 		+ 如果 `host` 为 NULL，则该标志得到的 SOCKET 地址可用于 `bind()` 操作
> 	+ `AI_CANONNAME`：请求返回主机的规范名称
> 		+ `host` 必须是一个合法的 IP 地址展示形式
> 		+ 禁止主机名解析，避免使用 DNS
> 	+ `AI_NUMERICSERV`：强制 `service` 参数为数字端口号字符串
> 		+ 参数 `service` 必须是数字字符串
> 		+ 禁止服务名解析
> 	+ `AI_V4MAPPED`：在仅支持 IPv6 的环境中，生成 IPv4 映射的 IPv6 地址
> 		+ 在支持 IPv6 的程序中兼容 IPv4
> 		+ 如果指定了 `AI_V4MAPPED` 且 `ai_family` 为 `AF_INET6`，但没有找到 IPv6 地址，则返回 IPv4 映射地址
> 	+ `AI_ALL`：返回 IPv4 和 IPv6 地址
> 		+ 必须与 `AI_V4MAPPED` 一起使用
> 	+ `AI_ADDRCONFIG`：仅返回本地系统可用的地址
> 		+ 本地系统未配置 IPv4，则不会返回 IPv4 地址
> 		+ 本地系统未配置 IPv6，则不会返回 IPv6 地址
>
> 结构中由提示指向的所有其他字段必须包含 $0$ 或空指针，视情况而定。
> 

下表总结了 `addrinfo.ai_flags` 的取值

| `ai_flags`       | 描述                                       |
| :--------------- | ---------------------------------------- |
| `AI_PASSIVE`     | 查询通配地址。要求 `host` 参数为 `NULL`。返回结果可用于绑定操作  |
| `AI_CANONNAME`   | 查询主机的规范名。要求 `host` 参数为 IP 地址的展示形式        |
| `AI_NUMERICSERV` | 强制参数 `service` 使用数字端口号字符串                |
| `AI_V4MAPPED`    | 在仅 IPv6 环境中生成 IPv4 映射的 IPv6 地址           |
| `AI_ALL`         | 返回 IPv4 和 IPv6 地址，必须和 `AI_V4MAPPED` 一起使用 |
| `AI_ADDRCONFIG`  | 仅返回本地系统可用地址                              |

> [!tip] 参数 `result`：指向一个由该函数分配的 `addrinfo` 结构的链表
> 
> 每个 `addrinfo` 结构包含一个指向与 `host` 和 `service` 对应的 SOCKET 地址结构的指针
> + `ai_family` 字段会被设置为 `AF_INET` 或 `AF_INET6`
> + `ai_socktype` 字段会被设置为 `SOCK_STREAM` 或 `SOCK_DGRAM`
> + `ai_protocol` 字段会被为 `ai_family` 和 `ai_socktype` 匹配的协议号
> + `ai_addrlen` 字段给出了 `ai_addr` 字段指向的 SOCKET 地址的长度
> + `ai_addr` 字段指向 SOCKET 地址结构
> + `ai_flags` 字段未使用。它只用于 `hints` 参数
> + `ai_canonname` 字段仅在 `hints.ai_flags` 中指定了 `AI_CANONANME` 时有效 
> 

`result` 参数返回一个结构列表而不是单个结构，因为与在 `host` `service` 以及 `hints` 中指定的标准对应的主机和服务组合可能有多个。原因如下
+ 如查询 **拥有多个网络接口** 的主机时可能会返回多个地址结构
+ `hints.ai_socktype` 指定为 $0$，可能会返回两个结构
	+ 一个用于 `SOCK_DGRAM` 类型的 SOCKET
	+ 一个用于 `SOCK_STREAM` 类型的 SOCKET

下图展示了 `getaddrinfo()` 通过参数 `result` 返回的 `addrinfo` 结构链表

![[Pasted image 20241122140628.png]]

```c title:sockets/t_getaddrinfo.c
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "base.h"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        usageErr("%s hostname service\n", argv[0]);
    }
    struct addrinfo hints = {.ai_flags = AI_PASSIVE | AI_ADDRCONFIG, .ai_family = AF_UNSPEC, .ai_socktype = SOCK_STREAM};
    struct addrinfo *result;

    if (getaddrinfo(argv[1], argv[2], &hints, &result) == -1) {
        errExit("getaddrinfo error: ");
    }

    for (struct addrinfo *p = result; p != NULL; p = p->ai_next) {
        char ipstr[INET6_ADDRSTRLEN];
        void *addr;
        char *ipvar;
        if (p->ai_family == AF_INET) { // IPv4
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
            addr = &(ipv4->sin_addr);
            ipvar = "IPv4";
        } else { // IPv6
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
            addr = &(ipv6->sin6_addr);
            ipvar = "IPv6";
        }
        inet_ntop(p->ai_family, addr, ipstr, INET6_ADDRSTRLEN);
        printf("%s:%s\n", ipvar, ipstr);
    }
    freeaddrinfo(result);
}
```

### freeadrinfo()

`getaddrinfo()` 函数会动态地为 `result` 引用的所有结构分配内存，其结果是调用者必须要在不再需要这些结构时释放它们。

使用 `freeaddrinfo()` 函数可以方便地在一个步骤中执行这个释放任务。

```c
#include <sys/socket.h>
#include <netdb.h>

void freeaddrinfo(struct addrinfo *result);
```

如果希望保留 `addrinfo` 结构或其关联的 SOCKET 地址结构的一个副本，那么必须要在调用 `freeaddrinfo()` 之前复制这些结构

### getnameinfo()

`getnameinfo()` 函数是 `getaddrinfo()` 的逆函数。给定一个 SOCKET 地址结构（IPv4 或 IPv6），它会返回一个包含对应的主机和服务名的字符串或者在无法解析名字时返回一个等价的数值。

```c
#include <sys/socket.h>
#include <netdb.h>

int getnameinfo(const struct sockaddr *addr, socklen_t addrlen, char *host, size_t hostlen, char *service, size_t servlen, int flags);
/* 成功返回 0；失败返回非零值 */
```

> [!tip] 参数 `addr`：指向待转换的 SOCKET 地址结构
> 

> [!tip] 参数 `addrlen`：指定 `addr` 指向 SOCKET 地址结构的长度
> 

> [!tip] 参数 `host`：指向一个字符数组，用于存储 **主机名**
> 

> [!tip] 参数 `hostlen`：指定 `host` 指向数组的长度
> 
> 预定义宏 `NI_MAXHOST` 指出了返回的主机名字符串的最大字节数，其取值为 $1025$
> 
> 

> [!tip] 参数 `service`：指向一个字符数组，用于存储 **服务名**

> [!tip] 参数 `servlen`：指定 `service` 指向数组的长度
> 
> 预定义宏 `NI_MAXSERV` 指出了返回的服务名字符串的最大字节数，其取值为 $32$
> 

> [!tip] 参数 `flags`：指定 `getnameinfo()` 的行为
> 
> 该参数的取值是如下常量的位或
> + `NI_DGRAM`：获取数据报 SOCKET 服务名
> + `NI_NAMEREQD`：无法解析主机名，返回错误 `EAI_NONAME`
> + `NI_NOFQDN`：主机位于局域网时，返回名字的第一部分
> + `NI_NUMERICHOST`：强制在 `host` 中返回一个数值地址字符串
> + `NI_NUMERICSERV`：强制在 `service` 中返回一个十进制端口号字符串
> 

### 错误诊断

`getaddrinfo()` 和 `getnameinfo()` 在发生错误时会返回下表中的一个非零错误码

![[Pasted image 20241122163045.png]]

`gai_strerror()` 函数会返回一个描述该错误的字符串

```c
#include <netdb.h>

const char *gai_strerror(int errcode);
/* 返回指向错误消息的指针 */
```

## 过时的转换 AIP

### inet_aton()和 inet_ntoa()函数

```c
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
       
int inet_aton(const char *cp, struct in_addr *inp);
char *inet_ntoa(struct in_addr in);
```

### gethostbyname()和 gethostbyaddr()函数

```c
#include <netdb.h>
struct hostent *gethostbyname(const char *name);
struct hostent *gethostbyaddr(const void addr[.len], socklen_t len, int type);
```

### getservbyname()和 getservbyport()函数

```c
#include <netdb.h>

struct servent *getservbyname(const char *name, const char *proto);
struct servent *getservbyport(int port, const char *proto);
```
