# 进程池

## 设计背景

如果要实现一个 **网络文件传输存储（即网盘）** 应用程序，需要涉及 **文件**、**进程**、**线程** 和 **网络** 等等。但是之前所学习的内容都是零散的知识点，我们需要综合这些所有的知识点，然后从一个更高的层面，也就是 **从整体设计的角度来思考应用程序的架构**，这样才能构建一个高质量的网盘项目。

一个良好的架构需要满足许多侧面的要求，其中最基本的要求是 **可维护性** 和 **性能**：

> [!tip] 可维护性：指应用程序 **对开发者应该足够友好**，**开发** 和 **维护** 的程序员可以很 **快速的就能理解程序架构** 并进行后续开发
> 
> 
> 

为了提供可维护性，**网盘的各个部分的功能应当彼此分离**。在这里，我们将网盘设计成一个 **多进程的应用**：存在一个 **主进程** 和 **多个子进程**

> [!attention] 
> 
> **主进程** 负责处理启动程序、管理子进程和处理客户端请求
> 
> **子进程** 也叫工作进程，负责真正的传输文件
> 

> [!tip] 性能：应用程序应当 **充分利用操作系统资源**，并且 **减少不必要资源消耗**
> 

在多进程程序中，一种 **非常浪费资源的操作就是创建进程**。在网盘项目中，如果每有一个传输文件的请求，程序就要去创建一个进程，那么大量的时间都消耗在进程的创建和销毁当中

> [!tip] 一种优化的思路就是 **“池化”**
> 
> 在应用程序启动的时候就创建多个子进程，这些子进程不会在执行过程中动态的创建和回收，它们一直存在直到应用程序终止
> + 如果没有任务时，子进程睡眠
> + 每当有传输文件的请求到来时，会唤醒某个子进程来完成任务
> + 文件传输完成之后，子进程会重新陷入睡眠
> 

## 网盘整体结构：进程池版本

下图展示了进程池版本的网盘结构

![[Pasted image 20241207210443.png]]

首先，就需要布局好代码

```shell
~/processPoll$ tree 
.
├── client
│   ├── main.c
│   ├── Makefile
│   └── README.md
└── server
    ├── main.c
    ├── Makefile
    ├── README.md
    ├── server.h
    └── worker.c
```

`server` 或 `client` 中的 `Makefile` 的内容如下

```shell
# 可执行程序名
EXECNAME := server # client  
# 源文件名
SRCS := $(wildcard *.c)
# 目标文件名
OBJECTS := $(patsubst %.c,%.o, $(SRCS))
# gcc 编译器
CC := $(shell which gcc)
# gcc 编译选项
CFLAGS := -Wall -g -Og

# 第一个目标：可执行文件
$(EXECNAME): $(OBJECTS)
	$(CC) $^ -o $@

%.o:%.c
	$(CC) $(CFLAGS) -c $^ -o $@

.PHONY: clean rebuild
clean:
	$(RM) $(OBJECTS)

rebuild: clean $(EXECNAME)
```

然后，设计数据结构。进程池主要围绕两种结构：**工作进程** 和 **进程状态**

```c title:server/server.h
/** worker_state_t: define worker states
 */
typedef enum {
    WORKER_STATE_IDLE,  // Worker is idle and can accept new tasks
    WORKER_STATE_BUSY   // Worker is busy and cannot accept new tasks
} worker_state_t;

/** worker_t: define worker
 */
typedef struct {
    pid_t pid;
    worker_state_t state;
} worker_t;
```

最后，设计相关的函数，对数据结构进行增删查改

> [!tip] 函数设计
> 
> 项目组需要开会讨论确定 **函数名**、**函数参数类型** 和 **返回值类型**。一个函数可能被多个模块使用，不能随便改动。
> 
> 在进行函数设计时，涉及两类人员：**实现者** 和 **使用者**
> + 实现者：决定主要参数个数和类型
> + 使用者：提供建议和补充
> 

## 进程池实现

### 创建工作进程 createProcessPoll()

应用程序启动时，创建一批工作进程，并等待任务到来。创建工作进程的接口 `make_workers()`

```c title:server/worker.h
/**
 * @brief Create process pool
 * 
 * @param workers An array that stores sub-process information.
 * @param maxlength Maximum length of workers
 * @return on success return 0, otherwise return -1
 */
int createProcessPoll(worker_t *workers, size_t maxlength);
```

实现 `make_worker()` 需要完成两个任务：**调用 fork() 创建子进程** 和 修改 `workers` 数组

```c title:server/worker.c
#include <unistd.h>
#include "worker.h"

int createProcessPoll(worker_t *workers, size_t maxlength) {
    for(int i = 0; i < maxlength; i++) {
        pid_t pid;
        switch (pid = fork())
        {
        case -1: // fork 失败
            return -1;
        case 0: // 子进程
            /* 让子进程陷入循环，不要脱离这里，否则子进程也会调用 fork() */
            while (true)
            {
                sleep(1);
            } 
            break;
        default:  // 父进程
            workers[i].pid = pid;
            workers[i].state = WORKER_STATE_IDLE;
            break;
        }
    }
    return 0;
}
```

### 父进程处理网络连接

在创建完所有的子进程之后，父进程的下一个工作目标是准备接受客户端的TCP连接，这个工作和之前网络编程时的工作内容差不多，按照 `socket`  `bind` 和 `listen` 的顺序执行系统调用即可。我们将它们封装到一个函数中 `listenTCPSocket()` 中，该函数的原型定义如下

```c title:server/tcp.h
#ifndef TCP_H
#define TCP_H

#include <sys/types.h>
#include <sys/socket.h>
#undef __USE_XOPEN2K
#define __USE_XOPEN2K
#include <netdb.h>

#define PROTOCOL_IP 0x01
#define PROTOCOL_IPV6 0x02

/**
 * @brief Return listening socket
 * 
 * @param hostname this is ip address or domain name
 * @param service  this is port number or service name
 * @param protocol 0x01 for IPv4, 0x02 for IPv6
 * @return on success, return listening socket file descriptor; on error, return -1
 */
int listenTCPSocket(const char *hostname, const char *service, int protocol);
#endif
```

参数 `protocol` 的取值为 `PROTOCOL_IP` 或者 `PROTOCOL_IPV6` 。下面是 `listenTCPSocket()` 的实现

```c title:server/tcp.c
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include "tcp.h"

int listenTCPSocket(const char *hostname, const char *service, int protocol) {
    struct addrinfo hints, *result = NULL;
    memset(&hints, 0, sizeof(struct addrinfo));    
    switch (protocol)
    {
    case PROTOCOL_IP:
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_family = AF_INET;
        hints.ai_flags = AI_PASSIVE;
        break;
    case PROTOCOL_IPV6:
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_family = AF_INET6;
        hints.ai_flags = AI_PASSIVE;
        break;
    default:
        return -1;
    }
    // 获取地址信息
    if(getaddrinfo(hostname, service, &hints, &result) == -1) {
        return -1;
    }

    int sockfd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if(sockfd == -1) {
        freeaddrinfo(result);
        return -1;
    }

    if(bind(sockfd, result->ai_addr, result->ai_addrlen) == -1) {
        freeaddrinfo(result);
        close(sockfd);
        return -1;
    }
    if(listen(sockfd, SOMAXCONN) == -1) {
        freeaddrinfo(result);
        close(sockfd);
        return -1;
    }
    freeaddrinfo(result);
    return sockfd;
}
```

如果需要使用 epoll 边缘触发监控 LISTEN SOCKET 和 CONN SOCKET，就需要将文件描述符设置为非阻塞的

```c title:server/tcp.h
/**
 * @brief Set fd to non-blocking
 * 
 * @param fd file descriptor
 * @return on success return 0, otherwise return -1
 */
int setNonBlock(int fd);
```

```c title:server/tcp.c
int setNonBlock(int fd) {
    int flags = fcntl(fd, F_GETFL);
    if(flags == -1) {
        return -1;
    }
    flags |= O_NONBLOCK;
    if(fcntl(fd, F_SETFL, flags) == -1) {
        return -1;
    }
    return 0;
}
```

下面封装了将文件描述符添加到 epoll 实例中的操作

```c title:server/epoll.h
#ifndef EPOLL_H
#define EPOLL_H
#include <sys/types.h>
#include <sys/epoll.h>

/**
 * @brief Add fd to the epoll instance to listen for events events
 * 
 * @param epfd File descriptor for the epoll instance
 * @param fd  The file descriptor to add
 * @param events Events to listen fo
 * @return on success return 0, otherwise return -1
 */
int epoll_add(int epfd, int fd, int events);

/**
 * @brief delete fd from the epoll instance
 * 
 * @param epfd File descriptor for the epoll instance
 * @param fd The file descriptor to delete
 * @return on success return 0, otherwise return -1
 */
int epoll_del(int epfd, int fd);
#endif
```

```c title:server/epoll.c
#include <stddef.h>
#include "epoll.h"

int epoll_add(int epfd, int fd, int events) {
    struct epoll_event ev;
    ev.events = events;
    ev.data.fd = fd;
    if(epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1) {
        return -1;
    }
    return 0;
}

int epoll_del(int epfd, int fd) {
    if(epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL) == -1) {
        return -1;
    }
    return 0;
}
```

每当有客户端连接时，就 **接收客户端连接**，产生一个全新的用于通信的 **对等 SOCKET**

```c title:server/main.c
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <sys/socket.h>

#include "server.h"

#define WORKER_NUMBER 5
#define HOSTNAME "127.0.0.1"
#define SERVICE "8080"

int main(int argc, char *argv[]) {
    if(argc > 1 && strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "Usage: %s <hostname> <service> <worker number>\n", argv[0]);
        return 1;
    }
    char * hostname = argc > 1 ? argv[1] : HOSTNAME;
    char * service = argc > 2 ? argv[2] : SERVICE;
    int worker_number = argc > 3 ? atoi(argv[3]) : WORKER_NUMBER;

    // 创建工作线程
    worker_t workers[worker_number];
    if(make_workers(workers, worker_number) == -1) {
        fprintf(stderr, "Failed to create %d workers\n", worker_number);
        return 1; 
    }

    // 监听 IP:PORT
    int sockfd = listenTCP(hostname, service, O_NONBLOCK);
    if(sockfd == -1) {
        fprintf(stderr, "Failed to listen on %s:%s\n", hostname, service);
        return 1;
    }

    // 创建 epoll 实例
    int epfd = epoll_create(1024);
    if(epfd == -1) {
        fprintf(stderr, "Failed to create epoll instance\n");
        return 1;
    }

    if(epoll_add(epfd, sockfd, EPOLLIN|EPOLLET) == -1) {
        fprintf(stderr, "Failed to add sockfd to epoll\n");
        return 1;
    }

    // 开启事件循环
    int max_length = 1024;
    struct epoll_event *events = malloc(sizeof(struct epoll_event) * max_length);
    while (true)
    {
        int readyNumber = epoll_wait(epfd, events, max_length, -1);
        if(readyNumber == -1) {
            fprintf(stderr, "Failed to wait for events\n");
            return 1;
        }
        for(int i = 0; i < readyNumber; i++) {
            if(events[i].data.fd == sockfd) {
                // 有客户端连接
                int connfd = accept(sockfd, NULL, NULL);
                // TODO: 处理连接
            }
        }
    }
    
    return 0;
}
```

现在，我们遇见了一个问题。在 [[操作系统：虚拟内存]] 我们介绍了进程之间的虚拟地址空间是隔离的。由于进程池中的进程是在建立连接之前创建的。我们遇见的第一个问题就是 **如何将父进程中的打开文件描述传递给进程池中的进程？**

### 父子进程之间共享打开文件描述

由于连接 SOCKET 需要递交给子进程使用。然而，**父进程和子进程之间的地址空间是隔离的**。又由于 **提前创建子进程**，连接 SOCKET 不在父子进程之间共享。我们需要某种方式 **将连接 SOCKET 发送给子进程**

![[Pasted image 20241207234647.png]]

> [!tip] 父子进程之间传递 **打开文件描述**
> 
> 这里，我们需要使用 [[Linux 系统编程：SOCKET 高级主题#sendmsg() 和 recvmsg()]] 系统调用，可以将一个进程的打开文件描述发送给另一个进程
> 

基于 **本地套接字** 的 **全双工** 管道，可以完成上述说明的在父子进程之间传递打开文件描述。系统调用 `socketpair()` 创建一对已连接的 SOCKET 对象

```c
#include <sys/socket.h>

int socketpair(int domain, int type, int protocol, int sv[2]);
```

> [!tip] 参数 `domain`：指定通信域
> 
> 该参数通常选择 `AF_UNIX` 或者 `AF_LOCAL`，代表本地计算机通信域。通常是同一个计算机内的多个进程之间传递消息
> 
> 更多通信域参考： [[Linux 系统编程：SOCKET 概览#创建 SOCKET]]
> 

> [!tip] 参数 `type`：SOCKET 类型
> 
> 通常是 `SOCK_STREAM` 或者 `SOCK_DGRAM`
> 
> 更多 SOCKET 类型参考： [[Linux 系统编程：SOCKET 概览#创建 SOCKET]]
> 

> [!tip] 参数 `protocol`：额外协议编号
> 
> 通常取 $0$
> 

> [!tip] 参数 `sv`：长度为 $2$ 数组，用来存储管道两端的文件描述符
> 
> 与 [[Linux 系统编程：PIPE]] 不同的是，`sv[0]` 和 `sv[1]` 没有任何区别
> + 如果从 `sv[0]` 写入，那么就必须从 `sv[1]` 读取
> + 如果从 `sv[0]` 读取，那么就必须从 `sv[1]` 写入
> 

一般 `socketpair()` 之后会调用 `fork()`，从而实现现父子进程之间的通信。下面将传递文件描述符封装为下面两个函数 `sendfd()` 和 `recvfd()`

```c title:server/unix.h
#ifndef UNIX_H
#define UNIX_H
#include <sys/types.h>
#include <sys/socket.h>

/**
 * @brief Pass open file descriptions via UNIX domain sockets
 * 
 * @param sockfd UNIX domain socket
 * @param fd The file descriptor to be passed
 * @return on success, return 0; on error, return -1 
 */
int sendfd(int sockfd, int fd);

/**
 * @brief Receive file descriptions via UNIX domain sockets
 * 
 * @param sockfd UNIX domain socket
 * @return int The file descriptor received; on error, return -1
 */
int recvfd(int sockfd);

#endif
```

```c title:server/unix.c
/* 本地 socket 通信 */
#include <string.h>
#include <sys/uio.h>
#include <stdlib.h>

#include "unix.h"

int sendfd(int sockfd, int fd) {
    struct msghdr hdr;
    memset(&hdr, 0, sizeof(struct msghdr));
    
    // 消息体：必须的 
    struct iovec iov[1];
    iov[0].iov_base = " ";
    iov[0].iov_len = 1;
    hdr.msg_iov = iov;
    hdr.msg_iovlen = 1;

    // 控制信息
    struct cmsghdr *cmsg = calloc(1, CMSG_LEN(sizeof(int)));
    if(cmsg == NULL) {
        return -1;
    }
    cmsg->cmsg_len = CMSG_LEN(sizeof(int));  // 控制信息的总长度
    cmsg->cmsg_level = SOL_SOCKET; // 套接字层次的控制信息
    cmsg->cmsg_type = SCM_RIGHTS;  // 传递文件描述符
    *((int *)CMSG_DATA(cmsg)) = fd; // 文件描述符
    hdr.msg_control = cmsg;
    hdr.msg_controllen = cmsg->cmsg_len;

    // 发送信息
    if(sendmsg(sockfd, &hdr, 0) == -1) {
        return -1;
    }
    return 0;
}


int recvfd(int sockfd) {
    struct msghdr hdr;
    memset(&hdr, 0, sizeof(struct msghdr));

    // 接收消息体的缓冲区
    struct iovec iov[1];
    char buf[2];
    iov[0].iov_base = buf;
    iov[0].iov_len = 2;
    // 
    hdr.msg_iov = iov;
    hdr.msg_iovlen = 1;
    
    // 接收控制信息的缓冲区
    struct cmsghdr *cmsg = calloc(1, CMSG_LEN(sizeof(int)));
    if(cmsg == NULL) {
        return -1;
    }
    cmsg->cmsg_len = CMSG_LEN(sizeof(int));
    cmsg->cmsg_level = SOL_SOCKET;
    cmsg->cmsg_type = SCM_RIGHTS;

    // 将控制信息的缓冲区与 msghdr 关联
    hdr.msg_control = cmsg;
    hdr.msg_controllen = cmsg->cmsg_len;

    // 接收信息
    if(recvmsg(sockfd, &hdr, 0) == -1) {
        return -1;
    }

    return *((int *)CMSG_DATA(cmsg));
}
```

要特别注意的是，传递的 **文件描述符在数值上完全可能是不相等** 的，但是它们 **对应的文件对象确实是同一个**，自然文件读写偏移量也是共享的，和之前使用 `dup` 或者是先打开文件再 `fork` 的情况是一致的

为了让进程池中的可以接收父进程传递的打开文件描述，我们需要对进程池的数据结构和 `createProcessPoll()` 函数进行修改

```c title:server/worker.h
#ifndef WORKER_H
#define WORKER_H
#include <stdbool.h>
#include <sys/types.h>

/**
 * @brief 工作进程的状态的描述
 * 
 */
typedef enum {
    WORKER_STATE_IDLE,  // 空闲
    WORKER_STATE_BUSY   // 忙碌
} worker_state_t;


/**
 * @brief 工作进程的结构
 * 
 */
typedef struct {
    pid_t pid;             // 子进程的 pid
    worker_state_t state;  // 子进程的状态
    int sendSockFd;        // 与子进程通信的 socket 文件描述符
} worker_t;

/**
 * @brief Create process pool
 * 
 * @param workers An array that stores sub-process information.
 * @param maxlength Maximum length of workers
 * @return on success return 0, otherwise return -1
 */
int createProcessPoll(worker_t *workers, size_t maxlength);

/**
 * @brief The tasks handling function of the process
 * 
 * @param sockfd The socket file descriptor, which is used to receive tasks
 * @return on success return 0, otherwise return -1
 */
int handler(int sockfd);
#endif
```

```c title:server/worker.c
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>


#include "worker.h"
#include "unix.h"


int createProcessPoll(worker_t *workers, size_t maxlength) {
    for(int i = 0; i < maxlength; i++) {

        int sv[2];
        if(socketpair(AF_UNIX, SOCK_STREAM, 0, sv) == -1) {
            return -1;
        }

        pid_t pid;
        switch (pid = fork())
        {
        case -1: // fork 失败
            return -1;
        case 0: // 子进程
            /* 关闭子进程的写入端 */
            if(close(sv[0]) == -1) {
                _exit(EXIT_FAILURE);
            }
            /* 让子进程陷入循环，不要脱离这里，否则子进程也会调用 fork() */
            while (true)
            {
                printf("PROCESS[PID %ld:no %d]: waiting for task\n", (long)getpid(), i);
                // 从 sv[1] 中读取任务
                if(handler(sv[1]) == -1) {
                    fprintf(stderr, "handler() failed\n");
                }
                // 从 handler() 返回，说明任务处理完毕
                // 通过 sv[1] 将 pid 发送给 main 进程
                pid_t pid = getpid();
                if(send(sv[1], &pid, sizeof(pid_t), 0) == -1) {
                    fprintf(stderr, "send() failed\n");
                }
            }
            break;
        default:  // 父进程
            /* 关闭父进程的读取端 */
            if(close(sv[1]) == -1) {
                return -1;
            }
            workers[i].pid = pid;                   // 记录子进程的 pid
            workers[i].state = WORKER_STATE_IDLE;   // 设置状态为 WORKER_STATE_IDLE
            workers[i].no = i;                      // 记录子进程的编号
            workers[i].unixSockFd = sv[0];          // 记录用于给子进程传递任务的文件描述符
            break;
        }
    }
    return 0;
}

int handler(int sockfd) {
    // 读取任务
    int connfd = recvfd(sockfd);
    if (connfd == -1 || connfd == 0) {
        return -1;
    }
    printf("PROCESS[PID %ld]: connfd = %d\n", (long)getpid(), connfd);

    // 处理任务：这里只是简单的回显
    while (true)
    {
        char buf[BUFSIZ] = {0};
        ssize_t numRead = read(connfd, buf, BUFSIZ);
        if(numRead == -1) {
            return -1;
        }
        // 对端关闭连接
        if(numRead == 0) {
            break;
        }

        for(int i = 0; i < numRead; i++) {
            buf[i] = (char)toupper(buf[i]);
        }
        char *fmt = "PROCESS[PID %ld]: %s\n";
        char result[BUFSIZ + 100];
        int size = snprintf(result, BUFSIZ + 100, fmt, (long)getpid(), buf);

        if(write(connfd, result, size) != size) {
            return -1;
        }
    }
    // 关闭连接
    if(close(connfd) == -1) {
        return -1;
    }
    return 0;
}
```

> [!tip] 
> 
> + 进程池中的进程执行一个循环，等待任务从主进程传递过来
> + 接收任务，并执行
> + 当任务完成后，向主进程发送自己的 PID
> 

然后，在主进程中，通过 `sendfd()` 发送与客户端连接的对等套接字给进程池中的空闲进程

```c title:server/main.c
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <errno.h>

#include "worker.h"
#include "tcp.h"
#include "epoll.h"
#include "unix.h"

#define HOSTNAME "127.0.0.1"
#define SERVICE "8080"
#define WORKER_NUMBER 5

int main(int argc, char *argv[])
{
    if (argc > 1 && strcmp(argv[1], "--help") == 0)
    {
        fprintf(stderr, "Usage: %s [hostname] [service name] [worker number]\n", argv[0]);
        return 1;
    }
    // 获取命令行参数
    char *hostname = argc > 1 ? argv[1] : HOSTNAME;
    char *service = argc > 2 ? argv[2] : SERVICE;
    int worker_number = argc > 3 ? atoi(argv[3]) : WORKER_NUMBER;

    // 创建工作进程池
    worker_t *workers = calloc(worker_number, sizeof(worker_t));
    if (workers == NULL)
    {
        fprintf(stderr, "calloc() failed\n");
        return 1;
    }
    if (createProcessPoll(workers, worker_number) == -1)
    {
        fprintf(stderr, "createProcessPoll() failed\n");
        return 1;
    }

    // 创建监听套接字：构建地址结构体-> 创建套接字-> 绑定地址-> 监听
    // 调用封装好的创建监听套接字函数
    int sockfd = listenTCPSocket(hostname, service, PROTOCOL_IP);
    if (sockfd == -1)
    {
        fprintf(stderr, "listenTCPSocket() failed: %s\n", strerror(errno));
        return 1;
    }
    // 创建 epoll 实例
    int epfd = epoll_create(1);
    if (epfd == -1)
    {
        fprintf(stderr, "epoll_create() failed\n");
        return 1;
    }

    // 将监听套接字添加到 epoll 实例中
    if (epoll_add(epfd, sockfd, EPOLLIN) == -1)
    {
        fprintf(stderr, "epoll_add() failed\n");
        return 1;
    }

    // 将工作进程的文件描述符添加到 epoll 实例中
    for (int i = 0; i < worker_number; i++)
    {
        if (epoll_add(epfd, workers[i].unixSockFd, EPOLLIN) == -1)
        {
            fprintf(stderr, "epoll_add() failed\n");
            return 1;
        }
    }

    // 开启事件循环
    int listen_size = worker_number + 1; // 监听套接字和工作进程的文件描述符
    struct epoll_event *readys = malloc(sizeof(struct epoll_event) * listen_size);

    while (true)
    {
        int readyNumber = epoll_wait(epfd, readys, listen_size, -1);
        if (readyNumber == -1)
        {
            fprintf(stderr, "Failed to wait for events\n");
            return 1;
        }
        for (int i = 0; i < readyNumber; i++)
        {
            printf("ready file descriptor: %d\n", readys[i].data.fd);
            if (readys[i].data.fd == sockfd)
            {
                // 有客户端连接
                int connfd = accept(sockfd, NULL, NULL);
                // 寻找一个空闲的工作进程
                for (int j = 0; j < worker_number; j++)
                {
                    if (workers[j].state == WORKER_STATE_IDLE)
                    {
                        printf("Send task to process[PID %ld:no %d]\n", (long)workers[j].pid, workers[j].no);
                        // 将任务发送给工作进程
                        if (sendfd(workers[j].unixSockFd, connfd) == -1)
                        {
                            fprintf(stderr, "sendfd() failed\n");
                        }
                        // 设置工作进程状态为忙碌
                        workers[j].state = WORKER_STATE_BUSY;
                        break;
                    }
                }
                close(connfd);
            }
            else
            {
                // 有工作进程完成任务
                for(int j = 0; j < worker_number; j++) {
                    if(workers[j].unixSockFd == readys[i].data.fd) {
                        // 读取工作进程的 pid
                        pid_t pid;
                        if(recv(workers[j].unixSockFd, &pid, sizeof(pid_t), 0) == -1) {
                            fprintf(stderr, "recv() failed\n");
                        }
                        printf("PROCESS[PID %ld:no %d] finished task\n", (long)workers[j].pid, workers[j].no);
                        // 设置工作进程状态为空闲
                        workers[j].state = WORKER_STATE_IDLE;
                        break;
                    }
                }
            }
        }
    }
}
```

> [!tip] 主进程执行如下任务
> 
> + 首先创建一些列的子进程，并将子进程设置为空闲状态
> + 然后处理网络连接，使用 epoll 监视 LISTEN SOCKET 和 与进程池中进程的通信的 UNIX SOCKET
> + 每当有客户端连接的时候，就向进程池中的空闲检查传递连接 SOCKET 文件描述，并将进程修改为忙碌状态
> + 当主进程接收到子进程的通知时，将完成任务的进程设置为 空闲状态
> 

## 文件的传输

### 自定义协议

由于 TCP 是流式协议，消息没有边界。传输文件需要传递文件名和文件内容，这两个消息需要分开传递。因此，需要解决消息边界的问题。因此，我们定义一个 **头**，里面包含携带信息的 **长度** 已经携带的 **信息**

```c title:server/worker.h
typedef struct {
    size_t length;
    byte *data;  
} hdr_t;
```

我们应该先发送长度，然后在发送信息

```c title:server/worker.h
/**
 * @brief Handle partial write issue
 * 
 * @param connfd Connection socket file descriptor
 * @param hdr contains the data to be sent
 * @return on success return number of send success, otherwise return -1
 */
int sendn(int connfd, const hdr_t *hdr);
```

信息发送可能存在 **部分写问题**。通过循环的方式解决该问题

```c title:server/worker.c

int sendn(int connfd, const hdr_t *hdr) {
    // 发送数据长度
    ssize_t total = 0;
    ssize_t totalSend = 0;
    ssize_t bytesleft = sizeof(hdr->length);
    while (totalSend < bytesleft)
    {
        ssize_t numSend = send(connfd, (byte *)&hdr->length + totalSend, bytesleft - totalSend, 0);
        if(numSend == -1) {
            // 被信号中断，继续发送
            if(errno == EINTR) {
                continue;
            }
            return -1;
        }
        totalSend += numSend;
    }
    total += totalSend;
    // 发送真实数据
    bytesleft = hdr->length;
    totalSend = 0;
    while(totalSend < bytesleft) {
        ssize_t numSend = send(connfd, hdr->data + totalSend, bytesleft - totalSend, 0);
        if(numSend == -1) {
            // 被信号中断
            if(errno == EINTR) {
                continue;
            }
            // 其他错误
            return -1;
        }
        totalSend += numSend;
    }
    total += totalSend;
    return total;    
}
```

### 发送文件内容

现在，我们就可以完成基本的文件发送了

```c title:server/worker.h
/**
 * @brief Send file
 * 
 * @param connfd Connection socket file descriptor
 * @param path file path
 * @return on success return 0, otherwise return -1
 */
int sendfile(int connfd, const char *path);
```

首先，发送文件名。然后循环读取文件数据，并发送。

```c title:server/worker.c
int sendfile(int connfd, const char *path) {
    hdr_t path_hdr;
    path_hdr.length = strlen(path);
    path_hdr.data = (byte *)path;
    // 发送文件名
    if(sendn(connfd, &path_hdr) == -1) {
        return -1;
    }

    // 打开文件
    int fd = open(path, O_RDONLY);
    if(fd == -1) {
        return -1;
    }

    byte buffer[BUFSIZ] = {0};
    ssize_t readBytesNumber = 0;

    // 循环读取文件内容
    while ((readBytesNumber = read(fd, buffer, BUFSIZ)) > 0)
    {
        hdr_t conent_hdr;
        conent_hdr.length = readBytesNumber;
        conent_hdr.data = buffer;
        // 发送文件内容
        if(sendn(connfd, &conent_hdr) == -1) {
            printf("sendn() failed\n");
            return -1;
        }
        sleep(1);
    }
    // 文件内容发送完毕，发送一个空的 hdr_t 结构体表示数据传输完毕
    if(send(connfd, &(hdr_t){0, NULL}, sizeof(size_t), 0) == -1) {
        return -1;
    }
    return 0;
}
```

### 客户端接收文件

客户端在接收文件时，应该首先接收当前信息的长度。然后在接收当前信息的内容。

```c title:client/client.h
#ifndef CLIENT_T
#define CLIENT_T
#include <stddef.h>
#include <sys/types.h>

typedef unsigned char byte;

typedef struct {
    size_t length;
    byte *data;  
} hdr_t;

/**
 * @brief Handle partial recv issue
 * 
 * @param sockfd SOCKET file descriptor
 * @param hdr The header structure
 * @return on success return number of bytes received, otherwise return -1 
 */
int recvn(int sockfd, hdr_t *hdr);
#endif
```

> [!warning] 半包问题：部分读问题
> 
> 在接收数据的时，`recv(...,...,len, 0)` 的第三个参数 `len` 是缓冲区的最大长度。该次 `recv()` 接收的字节数可能不足 `len`。导致，下一次接收出现问题。如下图所示
> 
> ![[Pasted image 20241210233618.png]]
> 
> 解决该问题可以使用 [[Linux 系统编程：SOCKET 高级主题]] 中介绍的 `recv()` 的第 $4$ 个参数，将其设置为 `MSG_WAITALL`，这样可以让 `recv()`  等待所有数据到达之后才返回。
> 
> 此外，我们可以通过一个循环，调用 `recv()` 直到所有的数据被接收后才退出循环
> 

每次读取应该要读取到 `hdr->length` 的只之后才返回。而不能读一部分就返回

```c title:client/client.c
int recvn(int sockfd, hdr_t *hdr) {
    if(hdr == NULL || hdr->data == NULL) {
        return -1;
    }

    ssize_t total = 0;
    ssize_t totalRecv = 0;
    ssize_t bytesleft = sizeof(hdr->length);
    while(totalRecv < bytesleft) {
        ssize_t numRecv = recv(sockfd, (byte *)&hdr->length + totalRecv, bytesleft - totalRecv, 0);
        if(numRecv == -1) {
            if(errno == EINTR) {
                continue;
            }
            return -1;
        }
        totalRecv += numRecv;
    }
    total += totalRecv;
    totalRecv = 0;
    bytesleft = hdr->length;
    while(totalRecv < bytesleft) {
        ssize_t numRecv = recv(sockfd, hdr->data + totalRecv, bytesleft - totalRecv, 0);
        if(numRecv == -1) {
            if(errno == EINTR) {
                continue;
            }
            return -1;
        }
        totalRecv += numRecv;
    }
    total += totalRecv;
    return total;
}
```

完成上述准备工作之后，可以开始完成客户端入口程序的编写了

```c title:client/main.c
#include <sys/types.h>
#include <sys/socket.h>
#undef __USE_XOPEN2K
#define __USE_XOPEN2K
#include <netdb.h>

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "client.h"

#define HOSTNAME "127.0.0.1"
#define SERVICE "8080"

int main(int argc, char *argv[]) {
    if(argc >  1 && strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "Usage: %s <hostname> <service>\n", argv[0]);
        return 1;
    }
    char *hostname = argc > 1 ? argv[1]: HOSTNAME;
    char *service = argc > 2 ?  argv[2]: SERVICE;
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = 0;
    hints.ai_flags = AI_NUMERICHOST | AI_NUMERICSERV;
    struct addrinfo *result;
    if(getaddrinfo(hostname, service, &hints, &result) == -1) {
        fprintf(stderr, "getaddrinfo() failed\n");
        return 1;
    }
    int sockfd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if(sockfd == -1) {
        fprintf(stderr, "socket() failed\n");
        return 1;
    }
    if(connect(sockfd, result->ai_addr, result->ai_addrlen) == -1) {
        fprintf(stderr, "connect() failed\n");
        return 1;
    }
    freeaddrinfo(result);

    hdr_t path_hdr;
    memset(&path_hdr, 0, sizeof(path_hdr));
    byte filename[1024] = {0};
    path_hdr.data = filename;
    if(recvn(sockfd, &path_hdr) == -1) {
        fprintf(stderr, "recvn() failed\n");
        return 1;
    }
    
    int fd = open((char *)path_hdr.data, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if(fd == -1) {
        fprintf(stderr, "open() failed\n");
        return 1;
    }
    
    while (true)
    {
        byte buf[BUFSIZ] = {0};
        hdr_t hdr;
        memset(&hdr, 0, sizeof(hdr));
        hdr.data = buf;
        if(recvn(sockfd, &hdr) == -1) {
            fprintf(stderr, "recvn() failed\n");
            return 1;
        }
        if(hdr.length == 0) {
            break;
        }
        if(write(fd, hdr.data, hdr.length) == -1) {
            fprintf(stderr, "write() failed\n");
            return 1;
        }
    }
    close(fd);
    close(sockfd);
    return 0;
}
```

### 工作进程意外终止问题

工作进程与客户端通信的过程中，如果 **客户端提前退出**，此时工作进程会继续向对端 `send()` 数据。由于对端关闭导致 SOCKET 管道连接的读端关闭，工作进程继续 `send()`  会接收到 `SIGPIPE` 信号。由于我们没有处理 `SIGPIPE` 信号，它的默认行为就是 **终止进程**

为了确保工作进程不会意外终止，就需要让进程忽略 `SIGPIPE` 信号。或者，指定 `send()` 的第 $4$ 个参数为 `MSG_NOSIGNAL`，这样 `send()` 就不会产生 `SIGPIPE` 信号。而是返回 $-1$ 并设置 `errno` 为 `EPIPE` 

### 进度条

客户端在下载文件的时候，并不知道此时文件下载了多少。显示一个进度条是非常有必要的。

为了让客户端显示进度条，客户端应该首先知道文件的大小。因此，服务端首先将文件大小发送给客户端

```c title:server/worker.c
int sendfile(int connfd, const char *path)
{
	// ...
    // 发送文件大小
    struct stat st;
    if(fstat(fd, &st) == -1) {
        return -1;
    }
    hdr_t file_hdr;
    file_hdr.length = sizeof(st.st_size);
    file_hdr.data = (byte *)&st.st_size;
    if (sendn(connfd, &file_hdr) == -1)
    {
        return -1;
    }
    // ...
}
```

客户端接收文件大小后才开始接收文件内容，每次接受完成就计算当前进度

```c title:client/main.c
int main(int argc, char *argv[])
{
	// ...
	// 接收文件大小
	hdr_t file_hdr;
    size_t file_size = 0;
    file_hdr.length = sizeof(file_size);
    file_hdr.data = (byte *)&file_size;
    if (recvn(sockfd, &file_hdr) == -1)
    {
        fprintf(stderr, "recv() failed\n");
        return 1;
    }
    // ...
    // 显示进度
	ssize_t totalRecv = 0;
    printf("%10s%10s%10s\n", "Total", "Received", "Rate");
    while (true)
    {
        byte buf[BUFSIZ] = {0};
        hdr_t hdr;
        memset(&hdr, 0, sizeof(hdr));
        hdr.data = buf;
        if (recvn(sockfd, &hdr) == -1)
        {
            fprintf(stderr, "recvn() failed\n");
            return 1;
        }
        totalRecv += hdr.length;
        printf("\r%10ld%10ld%10.2lf%%", file_size, totalRecv, (double)totalRecv / file_size * 100);
        if (hdr.length == 0)
        {
            break;
        }
        if (write(fd, hdr.data, hdr.length) == -1)
        {
            fprintf(stderr, "write() failed\n");
            return 1;
        }
    }
    // ....
	

```

## 零拷贝传输

服务端在发送文件时，首先从文件对象的缓冲区拷贝到用户空间的缓冲区，然后再从用户空间的缓冲区拷贝到对等 SOCKET 的发送缓冲区。这里不会对文件进行任何修改
 
 ![[Pasted image 20241128215758.png]]
 
对于只是简单复制，这种拷贝显然是一种浪费。由于 **文件缓冲区** 和 **SOCKET 发送缓冲区** 都在内核，如果可以直接将文件缓冲区中的内容拷贝到 SOCKET 发送缓冲区，那么就可以避免拷贝到用户空间。这称为 **零拷贝传输**

> [!tip] 零拷贝：减少用户态和内核态之间的来回拷贝

 ![[Pasted image 20241128220049.png]]

为了实现这样零拷贝传输，可以直接使用 `<sys/sendfile.h>` 中的 `sendfile()` 函数，也可以使用 [[Linux 系统编程：内存映射]] 技术实现。[[Linux 系统编程：SOCKET 高级主题]] 中，我们介绍了 `<sys/sendfile.h>` 中的 `sendfile()` 函数，由于该函数的使用存在诸多限制。因此，我们选择使用 **内存映射** 实现我们自定的 `sendfile()` 函数

```c title:server/worker.c
int sendfile(int connfd, const char *path)
{
    hdr_t path_hdr;
    path_hdr.length = strlen(path);
    path_hdr.data = (byte *)path;
    // 发送文件名
    if (sendn(connfd, &path_hdr) == -1)
    {
        return -1;
    }

    // 打开文件
    int fd = open(path, O_RDONLY);
    if (fd == -1)
    {
        return -1;
    }
    // 发送文件大小
    struct stat st;
    if(fstat(fd, &st) == -1) {
        return -1;
    }
    hdr_t file_hdr;
    file_hdr.length = sizeof(st.st_size);
    file_hdr.data = (byte *)&st.st_size;
    if (sendn(connfd, &file_hdr) == -1)
    {
        return -1;
    }
    
    // 使用 mmap 读取文件内容
    byte *fileContent = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if(fileContent == MAP_FAILED) {
        return -1;
    }
    for(off_t i = 0; i < st.st_size; i += BUFSIZ) {
        hdr_t conent_hdr;
        // 当前发送的字节数
        conent_hdr.length = st.st_size - i > BUFSIZ ? BUFSIZ : st.st_size - i;
        // 起始位置
        conent_hdr.data = fileContent + i;
        if (sendn(connfd, &conent_hdr) == -1)
        {
            printf("sendn() failed\n");
            return -1;
        }
    }
    // 解除映射
    munmap(fileContent, st.st_size);
    // 文件内容发送完毕，发送一个空的 hdr_t 结构体表示数据传输完毕
    if (sendn(connfd, &(hdr_t){0, NULL}) == -1)
    {
        return -1;
    }
    // 关闭文件
    if (close(fd) == -1)
    {
        return -1;
    }
    return 0;
}
```

### 自定义协议

由于已经将文件大小发送给了客户端。此时服务端可以直接发送文件数据，而不是将文件数据切成小段发送。为了实现这个功能，我们需要修改之前定义的接口。这里，我们将文件名和文件大小作为 **文件头** 一起发送给客户端

```c title:server/worker.h
typedef struct {
    size_t nameLength;    // 文件名长度
    off_t filesize;   // 文件大小
    byte *filename;   // 文件名
} hdr_t;
```

在发送文件的内容之前，首先发送这个文件头。将发送文件头的行为封装成一个函数 `sendhdr()`

```c title:server/worker.h
/**
 * @brief Send file header
 * 
 * @param connfd Connection socket file descriptor
 * @param hdr contains the data to be sent
 * @return on success return 0, otherwise return -1 
 */
int sendhdr(int connfd, const hdr_t *hdr);
```

这个函数依赖于另一个解决部分发送问题的函数 `sendn()`

```c title:server/worker.h
/**
 * @brief send date to the socket，which is resloved partially send problem
 * 
 * @param connfd Connection socket file descriptor
 * @param buffer The data to be sent
 * @param size The size of the data to be sent
 * @return int 
 */
int sendn(int connfd, const void *buffer, size_t size);
```

这两个函数的定义如下

```c title:server/worker.c
int sendhdr(int connfd, const hdr_t *hdr)
{
    // 发送文件名长度
    if (sendn(connfd, &hdr->nameLength, sizeof(hdr->nameLength)) == -1)
    {
        return -1;
    }
    // 发送文件大小
    if (sendn(connfd, &hdr->filesize, sizeof(hdr->filesize)) == -1)
    {
        return -1;
    }
    // 发送文件名
    if (sendn(connfd, hdr->filename, hdr->nameLength) == -1)
    {
        return -1;
    }
    return 0;
}

int sendn(int connfd, const void *buffer, size_t size)
{
    // 发送数据长度
    ssize_t totalSend = 0;
    // 发送真实数据
    const byte *data = (const byte *)buffer;
    totalSend = 0;
    while (totalSend < size)
    {
        ssize_t numSend = send(connfd, data + totalSend, size - totalSend, MSG_NOSIGNAL);
        if (numSend == -1)
        {
            // 被信号中断
            if (errno == EINTR)
            {
                continue;
            }
            // 其他错误
            return -1;
        }
        totalSend += numSend;
    }
    return 0;
}
```

到此，文件发送函数 `sendfile()` 修改为如下定义

```c title:server/worker.c
int sendfile(int connfd, const char *path)
{
    // 发送文件头
    hdr_t fileHdr;
    fileHdr.nameLength = strlen(path);
    fileHdr.filename = (byte *)path;
    struct stat st;
    if (stat(path, &st) == -1)
    {
        return -1;
    }
    fileHdr.filesize = st.st_size;

    if (sendhdr(connfd, &fileHdr) == -1)
    {
        return -1;
    }

    // 打开文件
    int fd = open(path, O_RDONLY);
    if (fd == -1)
    {
        return -1;
    }

    // 建立私有文件映射：只读取文件内容，并不修改文件
    byte *fileContent = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (fileContent == MAP_FAILED)
    {
        return -1;
    }
    // 发送文件内容
    if (sendn(connfd, fileContent, st.st_size) == -1)
    {
        return -1;
    }
    // 解除映射
    munmap(fileContent, st.st_size);
    // 关闭文件
    if (close(fd) == -1)
    {
        return -1;
    }
    return 0;
}
```

相应的客户端也需要进行修改

```c title:client/client.h
#ifndef CLIENT_T
#define CLIENT_T
#include <stddef.h>
#include <sys/types.h>

typedef unsigned char byte;

typedef struct {
    size_t nameLength; // 文件名长度
    off_t filesize;    // 文件大小
    byte *filename;    // 文件名
} hdr_t;

/**
 * @brief receive file header
 * 
 * @param sockfd SOCKET file descriptor
 * @param hdr store the file header information buffer
 * @return on success return 0, otherwise return -1
 */
int recvhdr(int sockfd, hdr_t *hdr);

/**
 * @brief receive n bytes data
 * 
 * @param sockfd SOCKET file descriptor
 * @param buffer store the data buffer
 * @param size data size
 * @return ssize_t return the number of bytes received, otherwise return -1
 */
ssize_t recvn(int sockfd, void *buffer, size_t size);
#endif
```

```c title:client/client.c
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>

#include "client.h"

int recvhdr(int sockfd, hdr_t *hdr) {
    if(hdr == NULL) {
        return -1;
    }
    // 接收文件名长度
    if(recvn(sockfd, &hdr->nameLength, sizeof(hdr->nameLength)) == -1) {
        return -1;
    }
    // 接收文件大小
    if(recvn(sockfd, &hdr->filesize, sizeof(hdr->filesize)) == -1) {
        return -1;
    }
    // 接收文件名
    if(recvn(sockfd, hdr->filename, hdr->nameLength) == -1) {
        return -1;
    }
    return 0;
}

ssize_t recvn(int sockfd, void *buffer, size_t size) {
    if(buffer == NULL || size == 0) {
        return -1;
    }

    ssize_t totalRecv = 0;
    byte * data = (byte *)buffer;
    while(totalRecv < size) {
        ssize_t numRecv = recv(sockfd, data + totalRecv , size - totalRecv, 0);
        if(numRecv == -1) {
            if(errno == EINTR) {
                continue;
            }
            return -1;
        }
        totalRecv += numRecv;
    }
    return totalRecv;
}
```

客户端的入口程序也需要进行相应的修改

```c
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/mman.h>
#undef __USE_XOPEN2K
#define __USE_XOPEN2K
#include <netdb.h>

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>


#include "client.h"

#define HOSTNAME "127.0.0.1"
#define SERVICE "8080"

int main(int argc, char *argv[])
{
    if (argc > 1 && strcmp(argv[1], "--help") == 0)
    {
        fprintf(stderr, "Usage: %s <hostname> <service>\n", argv[0]);
        return 1;
    }
    char *hostname = argc > 1 ? argv[1] : HOSTNAME;
    char *service = argc > 2 ? argv[2] : SERVICE;
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = 0;
    hints.ai_flags = AI_NUMERICHOST | AI_NUMERICSERV;
    struct addrinfo *result;
    if (getaddrinfo(hostname, service, &hints, &result) == -1)
    {
        fprintf(stderr, "getaddrinfo() failed\n");
        return 1;
    }
    int sockfd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (sockfd == -1)
    {
        fprintf(stderr, "socket() failed\n");
        return 1;
    }
    if (connect(sockfd, result->ai_addr, result->ai_addrlen) == -1)
    {
        fprintf(stderr, "connect() failed\n");
        return 1;
    }
    freeaddrinfo(result);
    // 接收文件头
    hdr_t file_hdr;
    char filename[BUFSIZ];
    file_hdr.filename = (byte *)filename;
    if (recvhdr(sockfd, &file_hdr) == -1)
    {
        fprintf(stderr, "recvhdr() failed\n");
        return 1;
    }

    int fd = open((char *)file_hdr.filename, O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (fd == -1)
    {
        fprintf(stderr, "open() failed\n");
        return 1;
    }
    // 修改文件大小
    if(ftruncate(fd, file_hdr.filesize) == -1) {
        fprintf(stderr, "ftruncate() failed\n");
        return 1;
    }
    // 共享内存映射
    byte * fileContentBuffer = mmap(NULL, file_hdr.filesize, PROT_WRITE, MAP_SHARED, fd, 0);
    if(fileContentBuffer == MAP_FAILED) {
        fprintf(stderr, "mmap() failed\n");
        return 1;
    }

    ssize_t totalRecv = 0;
    ssize_t lastRecv = 0;
    printf("%10s%10s%10s\n", "Total", "Received", "Rate");
    for(off_t curr = 0; curr < file_hdr.filesize; curr += BUFSIZ) {
 
        if ((lastRecv = recvn(sockfd, fileContentBuffer + curr, file_hdr.filesize - curr > BUFSIZ ? BUFSIZ : file_hdr.filesize - curr)) == -1)
        {
            fprintf(stderr, "recvn() failed\n");
            return 1;
        }
        totalRecv += lastRecv;
        printf("\r%10ld%10ld%10.2lf%%", file_hdr.filesize, totalRecv, (double)totalRecv / file_hdr.filesize * 100);
    }
    printf("\r%10ld%10ld%10.2lf%%\n", file_hdr.filesize, totalRecv, 100.0);
    close(fd);
    close(sockfd);
    return 0;
}
```

### splice()

系统调用 `sendfile()` 只能将普通文件数据发送到网络设备中，那么如何才能在接收方避免使用 `mmap()` 的情况下使用 **零拷贝** 技术呢？

解决方案就是使用 `splice()` 系统调用。该系统调用可以将数据**从内核管道文件缓冲区发送到另一个内核文件缓冲区**，也可以反之，**将一个内核文件缓冲区的数据直接发送到内核管道缓冲区中**

```c 
#define _GNU_SOURCE         /* See feature_test_macros(7) */
#define _FILE_OFFSET_BITS 64
#include <fcntl.h>

ssize_t splice(int fd_in, off_t * off_in,int fd_out, off_t * off_out,size_t len, unsigned int flags);
/* 成功返回传送的字节数; EOF 返回 0； 失败返回 -1 */
```

> [!tip] 参数 `fd_id`：输入文件描述符（读操作的源）
> 
> 指向输入文件或设备的文件描述符，用于指定要读取数据的源
> + 可以是一个**打开的文件**、**管道**、**网络套接字** 等
> 

> [!tip] 参数 `off_in`：输入偏移量的指针（可选，指示读取数据的起始位置）
> 
> 这是一个指向 `off_t` 类型的指针，用于指定从 `fd_in` 中读取数据的起始偏移量
> + 如果该参数为 `NULL`，则表示使用文件描述符 `fd_in` 的当前文件偏移量。
> + 如果不为 `NULL`，则表示从指定的偏移量开始读取
> 
> 偏移量通常在文件描述符的读写操作中自动更新，但你可以显式地指定一个偏移量
> 
   
> [!tip] 参数 `fd_out`：输出文件描述符（写操作的目标）
> 
> 指向输出文件或设备的文件描述符，用于指定将数据写入的目标位置
> + 可以是一个**打开的文件**、**管道**、**网络套接字** 等
> 

> [!tip] 参数 `off_out`：输出偏移量的指针（可选，指示写入数据的起始位置）
> 
> 这是一个指向 `off_t` 类型的指针，用于指定将数据写入 `fd_out` 的偏移量。
> + 如果该参数为 `NULL`，则表示将数据写入到文件描述符 `fd_out` 的当前偏移量
> + 如果不为 `NULL`，则表示将数据写入到指定的偏移量。
> 

> [!tip] 参数 `len`：要传输的数据字节数
> 
> 这是一个无符号整数，表示从输入文件描述符读取和向输出文件描述符写入的字节数。
> 

> [!tip] 参数 `flags`：控制传输的行为
> 
> + `SPLICE_F_MOVE`：表示直接将数据从源 **移动** 到目标，而不是复制。这通常用于管道或套接字
> + `SPLICE_F_MORE`：表示数据 **传输将继续**。这个标志与 `splice` 的返回值结合使用，指示是否还有更多数据需要传输
> + `SPLICE_F_NONBLOCK`：使得 `splice` 操作在 **非阻塞** 模式下执行。如果源或目标不可用，则该操作会立即返回，而不是阻塞
> 

下表总结参数 `flags` 的取值

| `flags`             | 描述                                 |
| :------------------ | ---------------------------------- |
| `SPLICE_F_MOVE`     | 从源 **移动** 到目标。常用于 **管道** 或 **套接字** |
| `SPLICE_F_MORE`     | 有数据需要 **继续传送**                     |
| `SPLICE_F_NONBLOCK` | **非阻塞** 传送                         |

## 进程池退出

有时候我们需要终止应用程序。如果简单的退出可能会导致应用程序的资源回收不完全，从而引发一些问题。为了在应用程序终止时，回收掉应用程序的所有资源。我们需要设计一种程序退出策略

进程池什么时候退出是不确定。想要异步推出进程池，目前唯一能够使用的策略就是信号。通常，需要给进程池的主进程发送一个信号，主进程根据信号处理进程池退出逻辑。

进程池需要退出的时候，子进程可能正在和客户端进行通信，如果直接终止进程池中的所有进程，那么可能会导致某些 **数据并未传输完成** 或者 **待保存的数据没有成功保存**。因此，工作进程需要 **在没有任何工作的退出点退出**。

工作进程处于一个 `while` 循环中，它一直等待主进程向其发送任务。因此，工作进程的退出点就是 **接收任务的地方**。也就是在 `handler()` 中读取任务的位置，也就是 `recvfd()`。只要从 `recvfd()` 返回，就能够判断是否接受到了任务

```c title:server/worker.c
int handler(int sockfd)
{
    // 读取任务：两个用途
    int connfd = recvfd(sockfd);
    if (connfd == -1 || connfd == 0)
    {
        return -1;
    }
    printf("PROCESS[PID %ld]: start working\n", (long)getpid());

    // 处理任务：发送文件
	if (sendfile(connfd, "file.txt") == -1)
	{
		fprintf(stderr, "sendfile() failed\n");
	}
    // 关闭连接
    if (close(connfd) == -1)
    {
        return -1;
    }
    return 0;
}
```

因此，我们需要修改 `sendfd()` 和 `recvfd()`

```c title:server/unix.h
#ifndef UNIX_H
#define UNIX_H
#include <sys/types.h>
#include <sys/socket.h>


#define UNIX_EXIT 0x1

/**
 * @brief Pass open file descriptions via UNIX domain sockets
 * 
 * @param sockfd UNIX domain socket
 * @param fd The file descriptor to be passed
 * @param flag Send exit signal
 * @return on success, return 0; on error, return -1 
 */
int sendfd(int sockfd, int fd, int flag);

/**
 * @brief Receive file descriptions via UNIX domain sockets
 * 
 * @param sockfd sockfd UNIX domain socket
 * @param fd pointer to the buffer that receives the file descriptor
 * @param flag pointer to the buffer that receives the flag
 * @return on success, return 0; on error, return -1
 */
int recvfd(int sockfd, int *fd, int *flag);
#endif
```

```c title:server/unix.c hl:8,14-15,43,49-50,72
#include <string.h>
#include <sys/uio.h>
#include <stdlib.h>
#include <stdio.h>

#include "unix.h"

int sendfd(int sockfd, int fd, int flag) {
    struct msghdr hdr;
    memset(&hdr, 0, sizeof(struct msghdr));
    
    // 消息体：必须的 
    struct iovec iov[1];
    iov[0].iov_base = &flag;
    iov[0].iov_len = sizeof(int);
    hdr.msg_iov = iov;
    hdr.msg_iovlen = 1;

    // 控制信息
    struct cmsghdr *cmsg = malloc(CMSG_LEN(sizeof(int)));
    if(cmsg == NULL) {
        return -1;
    }
    cmsg->cmsg_len = CMSG_LEN(sizeof(int));  // 控制信息的总长度
    cmsg->cmsg_level = SOL_SOCKET;           // 套接字层次的控制信息
    cmsg->cmsg_type = SCM_RIGHTS;            // 传递文件描述符
    *((int *)CMSG_DATA(cmsg)) = fd;          // 文件描述符

    // 将控制信息与 msghdr 关联
    hdr.msg_control = cmsg;
    hdr.msg_controllen = CMSG_LEN(sizeof(int));

    // 发送信息
    if(sendmsg(sockfd, &hdr, 0) == -1) {
        return -1;
    }
    // 释放控制信息的缓冲区
    free(cmsg);
    return 0;
}


int recvfd(int sockfd, int *fd, int *flag) {
    struct msghdr hdr;
    memset(&hdr, 0, sizeof(struct msghdr));

    // 接收消息体的缓冲区
    struct iovec iov[1];
    iov[0].iov_base = flag;
    iov[0].iov_len = sizeof(int);
    hdr.msg_iov = iov;
    hdr.msg_iovlen = 1;
    
    // 接收控制信息的缓冲区
    struct cmsghdr *cmsg = malloc(CMSG_LEN(sizeof(int)));
    if(cmsg == NULL) {
        return -1;
    }
    cmsg->cmsg_len = CMSG_LEN(sizeof(int));
    cmsg->cmsg_level = SOL_SOCKET;
    cmsg->cmsg_type = SCM_RIGHTS;

    // 将控制信息的缓冲区与 msghdr 关联
    hdr.msg_control = cmsg;
    hdr.msg_controllen = cmsg->cmsg_len;

    // 接收信息
    if(recvmsg(sockfd, &hdr, 0) == -1) {
        return -1;
    }
    // 获取文件描述符
    *fd = *((int *)CMSG_DATA(cmsg));
    // 释放控制信息的缓冲区
    free(cmsg);
    return 0;
}
```

对服务端入口程序的修改。
```c title:server/main.c hl:24,26-32,60-63,99-102,152-166
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#undef __USE_POSIX
#define __USE_POSIX
#undef __USE_XOPEN2K8
#define __USE_XOPEN2K8
#include <signal.h>

#include "worker.h"
#include "tcp.h"
#include "epoll.h"
#include "unix.h"

#define HOSTNAME "127.0.0.1"
#define SERVICE "8080"
#define WORKER_NUMBER 5

int exitPipe[2];  

void sigHandler(int sig)
{
    // 向 exitPipe 写入数据
    if(write(exitPipe[1], " ", 1) == -1) {
        fprintf(stderr, "write() failed\n");
    }
}

int main(int argc, char *argv[])
{
    if (argc > 1 && strcmp(argv[1], "--help") == 0)
    {
        fprintf(stderr, "Usage: %s [hostname] [service name] [worker number]\n", argv[0]);
        return 1;
    }
    // 获取命令行参数
    char *hostname = argc > 1 ? argv[1] : HOSTNAME;
    char *service = argc > 2 ? argv[2] : SERVICE;
    int worker_number = argc > 3 ? atoi(argv[3]) : WORKER_NUMBER;

    // 创建工作进程池
    worker_t *workers = calloc(worker_number, sizeof(worker_t));
    if (workers == NULL)
    {
        fprintf(stderr, "calloc() failed\n");
        return 1;
    }
    if (createProcessPoll(workers, worker_number) == -1)
    {
        fprintf(stderr, "createProcessPoll() failed\n");
        return 1;
    }

    // 创建 exit 管道
    if(pipe(exitPipe) == -1) {
        fprintf(stderr, "pipe() failed\n");
        return 1;
    }

    struct sigaction sa;
    sa.sa_handler = sigHandler;
    sa.sa_flags = SA_RESTART;
    sigemptyset(&sa.sa_mask);
    // 注册信号处理函数
    if(sigaction(SIGUSR1, &sa, NULL) == -1) {
        fprintf(stderr, "sigaction() failed\n");
        return 1;
    }

    // 创建监听套接字：构建地址结构体-> 创建套接字-> 绑定地址-> 监听
    // 调用封装好的创建监听套接字函数
    int sockfd = listenTCPSocket(hostname, service, PROTOCOL_IP);
    if (sockfd == -1)
    {
        fprintf(stderr, "listenTCPSocket() failed: %s\n", strerror(errno));
        return 1;
    }
    // 创建 epoll 实例
    int epfd = epoll_create(1);
    if (epfd == -1)
    {
        fprintf(stderr, "epoll_create() failed\n");
        return 1;
    }

    // 将监听套接字添加到 epoll 实例中
    if (epoll_add(epfd, sockfd, EPOLLIN) == -1)
    {
        fprintf(stderr, "epoll_add() failed\n");
        return 1;
    }

    // 将 exitPipe[0] 添加到 epoll 实例中
    if(epoll_add(epfd, exitPipe[0], EPOLLIN) == -1) {
        fprintf(stderr, "epoll_add() failed\n");
        return 1;
    }

    // 将工作进程的文件描述符添加到 epoll 实例中
    for (int i = 0; i < worker_number; i++)
    {
        if (epoll_add(epfd, workers[i].unixSockFd, EPOLLIN) == -1)
        {
            fprintf(stderr, "epoll_add() failed\n");
            return 1;
        }
    }

    // 开启事件循环
    int listen_size = worker_number + 2; // 监听套接字和工作进程的文件描述符
    struct epoll_event *readys = malloc(sizeof(struct epoll_event) * listen_size);

    while (true)
    {
        int readyNumber = epoll_wait(epfd, readys, listen_size, -1);
        if (readyNumber == -1)
        {
            if(errno == EINTR) {
                continue;
            }
            fprintf(stderr, "Failed to wait for events: %s\n", strerror(errno));
            return 1;
        }
        for (int i = 0; i < readyNumber; i++)
        {
            if (readys[i].data.fd == sockfd)
            {
                // 有客户端连接
                int connfd = accept(sockfd, NULL, NULL);
                // 寻找一个空闲的工作进程
                for (int j = 0; j < worker_number; j++)
                {
                    if (workers[j].state == WORKER_STATE_IDLE)
                    {
                        printf("Send task to process[PID %ld:no %d]\n", (long)workers[j].pid, workers[j].no);
                        // 将任务发送给工作进程
                        if (sendfd(workers[j].unixSockFd, connfd, 0) == -1)
                        {
                            fprintf(stderr, "sendfd() failed\n");
                        }
                        // 设置工作进程状态为忙碌
                        workers[j].state = WORKER_STATE_BUSY;
                        break;
                    }
                }
                close(connfd);
            } else if(readys[i].data.fd == exitPipe[0]) {
                // 通知所有工作进程退出
                for(int j = 0; j < worker_number; j++) {
                    if(sendfd(workers[j].unixSockFd, 0, UNIX_EXIT) == -1) {
                        fprintf(stderr, "sendfd() failed\n");
                    }
                }
                // 等待所有工作进程退出
                for(int j = 0; j < worker_number; j++) {
                    pid_t pid = wait(NULL);
                    printf("PROCESS[PID %ld] exited\n", (long)pid);
                }
                printf("All processes exited\n");
                return 0;
            }
            else
            {
                // 有工作进程完成任务
                for(int j = 0; j < worker_number; j++) {
                    if(workers[j].unixSockFd == readys[i].data.fd) {
                        // 读取工作进程的 pid
                        pid_t pid;
                        if(recv(workers[j].unixSockFd, &pid, sizeof(pid_t), 0) == -1) {
                            fprintf(stderr, "recv() failed\n");
                        }
                        printf("PROCESS[PID %ld:no %d] finished task\n", (long)workers[j].pid, workers[j].no);
                        // 设置工作进程状态为空闲
                        workers[j].state = WORKER_STATE_IDLE;
                        break;
                    }
                }
            }
        }
    }
}
```
