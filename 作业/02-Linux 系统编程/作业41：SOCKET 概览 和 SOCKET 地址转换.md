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

如何排查编程中遇见网络问题？

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
### 聊天服务器

这是 [[作业28：FIFO和IO多路复用]] 中聊天室的 SOCKET 版本

![[Pasted image 20241123212427.png]]

下面的代码给出了服务端的实现。为了处理多个客户端连接和接收多个客户端的消息。首先，就需要 **保存与客户端通信的套接字文件描述符**，这里我们使用一个 **链表存储**，链表中的每个 **结点定义如下**

```c title:sockets/chat.h
#ifndef CHAT_H
#define CHAT_H

#include <sys/socket.h>

struct node {
    int fd;  // 对等套接字文件描述符
    time_t lastActive; // 上次活跃时间
    bool isActive;
    struct sockaddr addr;
    socklen_t addrlen;
    struct node *next;
};

#endif //CHAT_H
```

然后，通过 `select` 监控 **监听套接字文件描述符** 和 **与客户端通信套接字文件描述符**。这里，我们手动将监听集合和就绪集合分开，简化代码。

```c title:sockes/chat_server.c
#include <netdb.h>  
#include <sys/types.h>  
#include <sys/socket.h>  
#include <sys/select.h>  
#include <unistd.h>  
  
#include "chat.h"  
#include "base.h"  
#include "become_daemon.h"  

struct conn {  
    struct node * head;  
    struct node * tail;  
    int len;  
};  

static struct conn conns = {};  

static struct node * creatNode(const int fd, const struct sockaddr *addr, const socklen_t addrLen) {  
    struct node *node = malloc(sizeof(struct node));  
    if (node == nullptr) {  
        return nullptr;  
    }  
    node->fd = fd;  
    node->next = nullptr;  
    node->isActive = true;  
    node->lastActive = time(nullptr);  
    node->addr = *addr;  
    node->addrlen = addrLen;  
    return node;  
}  

static int appendConn(const int fd, const struct sockaddr *addr, const socklen_t addrlen) {  
    struct node *node = creatNode(fd, addr, addrlen);  
    if (node == nullptr) {  
        return -1;  
    }  
    if (conns.tail == nullptr || conns.head == nullptr) {  
        conns.head = node;  
        conns.tail = node;  
    } else {  
        conns.tail->next = node;  
        conns.tail = node;  
    }  
    conns.len++;  
    return 0;  
}  
  
static int removeConn(const int fd) {  
    struct node *curr = conns.head;  
    struct node *prev = nullptr;  
    while (curr != nullptr && curr->fd != fd) {  
        prev = curr;  
    }  
    // 没找到  
    if (curr == nullptr) {  
        return -1;  
    }  
    // 删除的是头结点  
    if (prev == nullptr) {  
        conns.head = curr->next;  
    } else {  
        prev->next = curr->next;  
    }  
    conns.len--;  
    free(curr);  
    close(fd);  
    return 0;  
}  
  
int main(int argc, char *argv[]) {  
  
    if (strcmp(argv[1], "--help") == 0) {  
        usageErr("%s [hostname] [service]\n", argv[0]);  
    }  
  
    // 将服务端改为守护进程  
    become_daemon(BDNOREOPNSTDFD);  
  
    // 解析地址  
    char *hostname = argc > 1 ? argv[1] : "127.0.0.1";  
    char *service = argc > 2 ? argv[2] : "8080";  
    struct addrinfo *result;  
    int netErr = getaddrinfo(hostname, service, &(struct addrinfo){.ai_family = AF_INET, .ai_socktype = SOCK_STREAM, .ai_flags = AI_PASSIVE | AI_NUMERICHOST | AI_NUMERICSERV}, &result);  
    if (netErr != 0) {  
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(netErr));  
        exit(EXIT_FAILURE);  
    }  
  
    // 创建 IPv4 的 流式 SOCKET    int sockfd = socket(AF_INET, SOCK_STREAM, 0);  // 0  
    if (sockfd == -1) {  
        errExit("socket: ");  
    }  

    // 绑定地址  
    if (bind(sockfd, (struct sockaddr *)result->ai_addr, result->ai_addrlen) == -1) {  
        errExit("bind: ");  
    }  

    // 监听客户端连接  
    if (listen(sockfd, SOMAXCONN) == -1) {  
        errExit("listen: ");  
    }  

    // 监控集合  
    fd_set monitorSet;  
    FD_ZERO(&monitorSet);  
    FD_SET(sockfd, &monitorSet);  

    // 就绪集合  
    fd_set readySet;  
    FD_ZERO(&readySet);  

    while (true) {  
        // 将监控集合拷贝到就绪集合  
        memcpy(&readySet, &monitorSet, sizeof(fd_set));  
        int ret = select(1024, &readySet, nullptr, nullptr, &(struct timeval){.tv_sec = 1, .tv_usec = 0});  
        if (ret == -1) {  
            errExit("select: ");  
        }  
        // 超时  
        if (ret == 0) {  
            time_t now = time(nullptr);  
            for (struct node *curr = conns.head; curr != nullptr; curr = curr->next) {  
                if (curr->isActive && now - curr->lastActive < 10) {  
                    continue;  
                }  
                // 开始踢人  
                FD_CLR(curr->fd, &monitorSet);  
                removeConn(curr->fd);  
            }  
        }  
        // 客户端连接  
        if (FD_ISSET(sockfd, &readySet)) {  
            struct sockaddr_in addr;  
            socklen_t addrLen = sizeof(addr);  
            int netfd = accept(sockfd, (struct sockaddr *)&addr, &addrLen);  
            if (netfd == -1) {  
                fprintf(stderr, "accept: %s\n", strerror(errno));  
            } else {  
                FD_SET(netfd, &monitorSet);  
                if (appendConn(netfd, (struct sockaddr *)&addr, addrLen) == -1) {  
                    fprintf(stderr, "maintain file descriptors error\n");  
                }  
                printf("client connected\n");  
            }  
        }  
        // 其他客户端发送来消息，需要将它们转发出去  
        for (struct node *curr = conns.head; curr != nullptr; curr = curr->next) {  
            if (!FD_ISSET(curr->fd, &readySet)) {  
                continue;  
            }  
            // 当前客户端发来消息，转发到其他客户端  
            char buf[BUFSIZ];  
            ssize_t len = recv(curr->fd, buf, BUFSIZ, 0);  
            if (len == -1 || len == 0) {  
                curr->isActive = false;  
                continue;  
            }  
            char host[INET_ADDRSTRLEN];  
            char port[20];  
            getnameinfo(&curr->addr, curr->addrlen, host, sizeof(host), port, sizeof(port), NI_NUMERICHOST | NI_NUMERICSERV);  
            char prefix[1024];  
            snprintf(prefix, sizeof(prefix), "client(%s:%s): ", host, port);  
            curr->lastActive = time(nullptr);  
  
            // 广播给存活的所有客户端  
            for (struct node *receiver = conns.head; receiver != nullptr; receiver = receiver->next) {  
                if (receiver == curr || receiver->isActive == false) {  
                    continue;  
                }  
                send(receiver->fd, prefix, strlen(prefix)+1, 0);  
                send(receiver->fd, buf, len, 0);  
            }  
        }  
    }  
}
```

下面给出聊天室客户端代码

```c title:sockets/chat_client.c
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "base.h"

int main(int argc, char *argv[]) {
    if (argc > 1 && strcmp(argv[1], "--help") == 0) {
        usageErr("%s [host] [port]", argv[0]);
    }
    char *hostname = argc>1 ? argv[1]: "127.0.0.1";
    char *service = argc>2 ? argv[2] : "8080";

    struct addrinfo *result;
    int err = getaddrinfo(hostname, service,
        &(struct addrinfo){.ai_family = AF_INET, .ai_socktype = SOCK_STREAM, .ai_protocol = 0, .ai_flags = AI_NUMERICHOST | AI_NUMERICSERV},
        &result);
    if (err != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(err));
        exit(EXIT_FAILURE);
    }

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        errExit("socket: ");
    }

    if (connect(sockfd, result->ai_addr, result->ai_addrlen) == -1) {
        errExit("connect: ");
    }

    fd_set fds;


    while (true) {
        FD_ZERO(&fds);
        FD_SET(sockfd, &fds);
        FD_SET(STDIN_FILENO, &fds);

        const int e = select(sockfd + 1, &fds, nullptr, nullptr, nullptr);
        if (e == -1) {
            errExit("select: ");
        }
        if (FD_ISSET(sockfd, &fds)) {
            char buf[BUFSIZ];
            ssize_t len = recv(sockfd, buf, BUFSIZ, 0);
            if (len == -1) {
                errExit("recv: ");
            }
            if (len == 0) {
                break;
            }
            write(STDOUT_FILENO, buf, len);
        }
        if (FD_ISSET(STDIN_FILENO, &fds)) {
            char buf[BUFSIZ];
            ssize_t len = read(STDIN_FILENO, buf, BUFSIZ);
            if (len == -1) {

                errExit("read: ");
            }
            if (len == 0) {
                close(sockfd);
                break;
            }
            send(sockfd, buf, len, 0);
        }
    }
}
```
