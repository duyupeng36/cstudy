# 作业41： [[SOCKET 概览]] 和 [[SOCKET：地址转换]] 

## 问答题

什么是 **大端模式** 和 **小端模式**，什么是 **主机字节序** 和 **网络字节序**？

> [!tip] 大端模式：存储整数时，按照整数的最高有效字节到最低有效字节的顺序进行存储
> 
> 换句话说，就是高有效字节存储在整数对象的低地址位置
> 

> [!tip] 小端模式：存储整数时，按照整数的最低有效字节到最高有效字节的顺序进行存储
> 
> 换句话说，就是低有效字节存储在整数对象的低地址位置
> 

> [!tip] 主机字节序：CPU 选择使用的字节序
> 

> [!tip] 网络字节序：网络设备选择使用的字节序，通常是大端字节序

---
`accept()` 对应 TCP 三次握手的第几次？

> [!tip] `accept()` 与 TCP 三次握手没有对应关系，它发生在三次握手之后
> 
> 服务器通过 `accpet()` 来取出连接新建一个新的 SOCKET 文件对象，若没有连接上，`accept()` 会阻塞
> 

---

使用 `recv()` 和 `send()` 函数时发生了什么？

> [!tip] 
> 
> `send()` 和 `recv()` 函数相当于 `write()` 和 `read()`，将数据从用户态拷贝到缓冲区和从缓冲区拷贝到用户态，相当于 `write()` 和 `read()` 的网络特殊版本
> 

---

如何解决编程中遇见网络问题？

> [!tip] 
> 
> 首先，可以使用 `netstat -an` 命令查看 `IP:PORT` 对应的 **网络状态**，关于网络状态参考 [[网络：传输层协议]]
> 
> 如果通过网络状态无法确定问题，可以抓包分析请求过程。使用命令 `tcpdump -i 网卡名 port 端口号 -w 文件` 将抓到的网络包存储到文件中。之后，可以使用 wireshark 工具分析
> 

## 编程题

编写代码，验证一下自己的机器是大端模式还是小端模式？

```c title:sockets/byte_order.c
#include "base.h"

int main() {
    int a = 0x12345678;
    /*
     *    0x12   34 56    78
     * 高有效字节         低有效字节
     *
     * 小端 a: [0x78 0x56 0x34 0x12]
     *        低地址           高地址
     * 大端 a: [0x12 0x34 0x56 0x78]
     */

    // 获取高地址位置保存的数据
    if (a & 0xff == 0x78) {
        printf("大端字节序\n");
    } else {
        printf("小端字节序\n");
    }

    if (((unsigned char *)&a)[0] == 0x78) {
        printf("小端字节序\n");
    } else {
        printf("大端字节序\n");
    }

    return 0;
}
```

---

根据域名，**获取一下知名互联网厂商的 IP 地址信息**，如阿里、京东、腾讯、百度

```c title:sockets/t_getaddrinfo.c
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "base.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        usageErr("%s hostname [service]\n", argv[0]);
    }
    struct addrinfo hints = {.ai_flags = AI_PASSIVE | AI_CANONNAME, .ai_family = AF_UNSPEC, .ai_socktype = SOCK_STREAM};
    struct addrinfo *result;

    const char *service = argc > 2 ? argv[2] : "https";
    
    // 解析地址信息
    if (getaddrinfo(argv[1], service, &hints, &result) == -1) {
        errExit("getaddrinfo error: ");
    }

    // 遍历 addrinfo 链表
    for (struct addrinfo *ai = result; ai != nullptr; ai = ai->ai_next) {
        printf("规范名称: %s\n", ai->ai_canonname ? ai->ai_canonname:"");
        char ipstr[INET6_ADDRSTRLEN];
        uint16_t port;
        // IPv4 地址
        if (ai->ai_family == AF_INET) {
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)ai->ai_addr;
            inet_ntop(ai->ai_family, &ipv4->sin_addr, ipstr, INET6_ADDRSTRLEN);
            port = ntohs(ipv4->sin_port);
        }
        // IPv6 地址
        else {
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)ai->ai_addr;
            inet_ntop(ai->ai_family, &ipv6->sin6_addr, ipstr, INET6_ADDRSTRLEN);
            port = ntohs(ipv6->sin6_port);
        }
        printf("\t address: %s:%d\n", ipstr, port);
    }

    freeaddrinfo(result);
}
```

---

