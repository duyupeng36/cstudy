# 作业42：[[Linux epoll]]

下面的代码是 [[作业41：SOCKET 概览 和 SOCKET 地址转换#聊天服务器]] 中的服务端，使用 epoll 实现

```c title:sockets/chat_server.c
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <unistd.h>
#include <fcntl.h>
#include <rdwrn.h>
#include <sys/epoll.h>

#include "base.h"
#include "become_daemon.h"

enum sock_type {LISTEN, CONNECTION};

struct node {
    int fd;  // 对等套接字文件描述符
    enum sock_type type;
    time_t lastActive; // 上次活跃时间
    bool isActive;
    struct node *next;
};

struct conn {
    struct node *head;
    struct node *tail;
    int size;
};

static struct conn conns = {};

static struct node * newNode(const int fd, const enum sock_type type) {
    struct node *node = nullptr;
    // 尝试 3 次
    for (int i = 0; i < 3 && node == nullptr; i++) {
        node = malloc(sizeof(struct node));
    }
    if (node == nullptr) {
        return nullptr;
    }
    node->fd = fd;
    node->type = type;
    if (type == LISTEN) {
        node->lastActive = 0;
        node->isActive = false;
    } else {
        node->isActive = true;
        node->lastActive = time(nullptr);
    }
    node->next = nullptr;
    return node;
}

static struct node * appendConn(const int fd, const enum sock_type type) {
    struct node *node = newNode(fd, type);
    if (node == nullptr) {
        return nullptr;
    }
    if (conns.head == nullptr || conns.tail == nullptr) {
        conns.head = node;
        conns.tail = node;
    } else {
        conns.tail->next = node;
        conns.tail = node;
    }
    conns.size++;
    return node;
}

static int removeConn(const int fd) {
    struct node *curr = conns.head;
    struct node *prev = nullptr;
    while (curr != nullptr && curr->fd != fd) {
        prev = curr;
        curr = curr->next;
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
    conns.size--;
    free(curr);
    close(fd);
    return 0;
}

static int setNonBlock(const int fd) {
    const int flags = fcntl(fd, F_GETFL);
    if (flags == -1) {
        return -1;
    }
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        return -1;
    }
    return 0;
}

int main(int argc, char *argv[]) {

    if (argc > 1 && strcmp(argv[1], "--help") == 0) {
        usageErr("Usage: %s [hostname] [service]\n", argv[0]);
    }
    struct addrinfo hints = {.ai_family = AF_INET, .ai_socktype = SOCK_STREAM, .ai_protocol = 0};
    struct addrinfo *result;
    // 解析服务端地址
    if (getaddrinfo(argc > 2 ? argv[1] : "127.0.0.1", argc > 3 ? argv[2] : "8080", &hints, &result) == -1) {
        errExit("getaddrinfo: ");
    }

    // 创建 socket
    int sock = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (sock == -1) {
        errExit("socket: ");
    }

    // 设置为非阻塞模式
    if (setNonBlock(sock) == -1) {
        errExit("setNonBlock: ");
    }

    // 绑定地址
    if (bind(sock, result->ai_addr, result->ai_addrlen) == -1) {
        errExit("bind: ");
    }

    // 转为监听套接字
    if (listen(sock, SOMAXCONN) == -1) {
        errExit("listen: ");
    }

    // 创建 epoll 实例
    int epfd = epoll_create(1024);
    if (epfd == -1) {
        errExit("epoll_create: ");
    }
    // 创建 SOCK 节点
    struct node *sockNode = newNode(sock, LISTEN);
    if (sockNode == nullptr) {
        errExit("appendConn: ");
    }
    // 将 sock 添加到 epoll 实例的 interest list 中
    struct epoll_event sockEv;
    sockEv.events = EPOLLIN | EPOLLET;
    sockEv.data.ptr = sockNode;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, sock, &sockEv) == -1) {
        errExit("epoll_ctl: ");
    }
    // 开始监控就绪列表
    int max_length = 1024;
    struct epoll_event *events = malloc(sizeof(struct epoll_event) * max_length);
    while (true) {
        const int numFds = epoll_wait(epfd, events, max_length, 1000);
        if (numFds == -1) {
            fprintf(stderr, "epoll_wait: %s\n", strerror(errno));
            break;
        }
        // 超时
        if (numFds == 0) {
            for (const struct node *curr = conns.head; curr != nullptr; curr = curr->next) {
                const time_t now = time(nullptr);
                // 现在活着的连接
                if (curr->isActive && now - curr->lastActive < 30) {
                    continue;
                }
                // 开始踢人：已经断开的连接和长时间没有活动的连接
                if (epoll_ctl(epfd, EPOLL_CTL_DEL, curr->fd, nullptr) == -1) {
                    fprintf(stderr, "epoll_ctl: %s\n", strerror(errno));
                }
                if (removeConn(curr->fd) == -1) {
                    fprintf(stderr, "removeConn: can't find file descriptor %d\n", curr->fd);
                }
            }
            continue;
        }

        for (int i = 0; i < numFds; i++) {
            // 监听套接字就绪
            if (events[i].data.ptr == sockNode) {
                const int fd = accept(sock, nullptr, nullptr);
                if (fd == -1) {
                    fprintf(stderr, "accept: %s\n", strerror(errno));
                    continue;
                }
                struct node *connNode = appendConn(fd, CONNECTION);
                if (connNode == nullptr) {
                    errExit("appendConn: ");
                }
                struct epoll_event connEv;
                connEv.events = EPOLLIN | EPOLLET;
                connEv.data.ptr = connNode;
                if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &connEv) == -1) {
                    fprintf(stderr, "epoll_ctl: %s\n", strerror(errno));
                }
                // 已有连接数量大于最大用户列表长度
                if (conns.size >= max_length) {
                    max_length *= 2;
                    struct epoll_event *newEvents = nullptr;
                    for (int j = 0; j < 3 && newEvents == nullptr; j++) {
                        newEvents = realloc(events, sizeof(struct epoll_event) * max_length);
                    }
                    if (newEvents != nullptr) {
                        events = newEvents;
                    } else {
                        fprintf(stderr, "realloc: %s\n", strerror(errno));
                    }
                }
            } else {
                // 其他情形：客户端发送消息过来
                struct node *connNode = events[i].data.ptr;
                // 获取当前客户端的 hostname:port
                struct sockaddr_in addr;
                socklen_t addrLen = sizeof(addr);
                if (getpeername(connNode->fd, (struct sockaddr *)&addr, &addrLen) == -1) {
                    fprintf(stderr, "getpeername: %s\n", strerror(errno));
                }
                char host[INET_ADDRSTRLEN];
                char service[8];
                if (getnameinfo((struct sockaddr *)&addr, addrLen, host, INET_ADDRSTRLEN, service, 8, NI_NUMERICHOST | NI_NUMERICSERV) == -1) {
                    fprintf(stderr, "getnameinfo: %s\n", strerror(errno));
                    strncpy(host, "unknown", INET_ADDRSTRLEN);
                    strncpy(service, "unknown", 8);
                }
                char address[INET_ADDRSTRLEN+30];
                snprintf(address, INET_ADDRSTRLEN+30, "Client(%s:%s):", host, service);

                // 读取客户端传递的消息
                char buffer[BUFSIZ];
                const ssize_t numRead = read(connNode->fd, buffer, BUFSIZ);
                if (numRead == -1 || numRead == 0) {
                    fprintf(stderr, "read: %s\n", strerror(errno));
                    connNode->isActive = false;
                    continue;
                }
                connNode->lastActive = time(nullptr);

                // 广播给其他客户端
                for (struct node * curr = conns.head; curr != nullptr; curr = curr->next) {
                    // 跳过自身和已关闭的套接字
                    if (curr == connNode || !curr->isActive) {
                        continue;
                    }
                    if (writen(curr->fd, address, strlen(address)) == -1) {
                        fprintf(stderr, "writen: %s\n", strerror(errno));
                        curr->isActive = false;
                        continue;
                    }
                    if (writen(curr->fd, buffer, numRead) == -1) {
                        fprintf(stderr, "writen: %s\n", strerror(errno));
                        curr->isActive = false;
                    }
                }
            }
        }
    }
    return 0;
}
```

