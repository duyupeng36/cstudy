# 作业28：[[Linux 系统编程：FIFO]] 和 [[Linux 系统编程：IO 多路复用]]


## 编程题

### 聊天服务器与客户端

通过 FIFO 实现一个本地广播聊天软件。下图展示了这个聊天服务器与客户端的通信过程

![Pasted image 20241102010342|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755788456301-7d0ca4d679d247c3ba91a67ee7a4db40.png)


```c title:chat.h
#ifndef CHAT_H
#define CHAT_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <linux/limits.h>

#define SERVER_CONNECTION_FIFO "/tmp/server.connection.fifo"       // 与服务端建立连接的 FIFO 名字
#define SERVER_MESSAGE_FIFO "/tmp/server.message.fifo"  // 服务端读取消息的 FIFO 名字
#define CLIENT_FIFO_TEMPLATE "/tmp/client.%ld.fifo"     // 客户端 FIFO 名字的模板
#define CLIENT_FIFO_NAME_LEN (sizeof(CLIENT_FIFO_TEMPLATE) + 20)

#define MESSAGE_MAX_LEN 102400

struct request {
    pid_t pid;  // 客户端的 PID
};

struct message {
    pid_t pid;  // 发送消息的客户端
    size_t len;
    char message[1024];
};


struct response {
    size_t len;
    char message[2048];
};


// 服务端接收客户端的连接后就会创建一个 connection 结构体
struct connection {
    pid_t pid;
    int fd;  // 用于通信客户端 FIFO
    struct connection *next;   // 下一个 connection
};

#endif // CHAT_H
```

下面代码是服务端示例代码
```c title:server.c
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <fcntl.h>
#include <unistd.h>

#include "chat.h"

struct connection *head = NULL; // 处理连接的头

struct connection *accept(int *fd);

void appendConnection(struct connection *conn);
void removeConnection(pid_t pid);

int main()
{

    if (mkfifo(SERVER_CONNECTION_FIFO, 0666) == -1)
    {
        perror("mkfifo");
        return EXIT_FAILURE;
    }
    printf("server[PID %ld] create %s success\n", (long)getpid(), SERVER_CONNECTION_FIFO);

    if (mkfifo(SERVER_MESSAGE_FIFO, 0666) == -1)
    {
        perror("mkfifo");
        return EXIT_FAILURE;
    }
    printf("server[PID %ld] create %s success\n", (long)getpid(), SERVER_MESSAGE_FIFO);

    int serverConnectionFd = open(SERVER_CONNECTION_FIFO, O_RDONLY | O_NONBLOCK);
    if (serverConnectionFd == -1)
    {
        perror("open");
        return EXIT_FAILURE;
    }
    printf("server[PID %ld] open %s read side success\n", (long)getpid(), SERVER_CONNECTION_FIFO);

    int serverMessageFd = open(SERVER_MESSAGE_FIFO, O_RDONLY | O_NONBLOCK);
    if (serverMessageFd == -1)
    {
        perror("open");
        return EXIT_FAILURE;
    }
    printf("server[PID %ld] open %s read side success\n", (long)getpid(), SERVER_MESSAGE_FIFO);

    fd_set readfds;

    while (true)
    {

        FD_ZERO(&readfds);
        FD_SET(serverConnectionFd, &readfds);
        FD_SET(serverMessageFd, &readfds);

        int ret = select(serverMessageFd + 1, &readfds, NULL, NULL, NULL);
        if (ret == -1)
        {
            break;
        }

        if (FD_ISSET(serverConnectionFd, &readfds))
        {
            struct connection *conn = accept(&serverConnectionFd); // accpet 阻塞客户端连接
            if (conn != NULL)
            {
                appendConnection(conn); // 将连接对象插入到链表尾部
            }
        }

        if (FD_ISSET(serverMessageFd, &readfds))
        {
            // 从 serverMessageFd 中读取数据
            struct message msg;
            memset(&msg, 0, sizeof(struct message));
            ssize_t bytes = read(serverMessageFd, &msg, sizeof(struct message));
            if (bytes == -1)
            {
                continue;
            }
            // 客户端退出
            struct response resp;
            memset(&resp, 0, sizeof(struct response));
            resp.len = snprintf(resp.message, sizeof(resp.message), "client[PID %ld]: %s\n", (long)msg.pid, "退出");
            if (msg.len == 0 || msg.len == -1)
            {
                removeConnection(msg.pid);
                for (struct connection *cur = head; cur != NULL; cur = cur->next)
                {
                    write(cur->fd, &resp, sizeof(struct response));
                }
            }

            // 广播到所有客户端，发送消息的客户端不需要广播
            memset(&resp, 0, sizeof(struct response));
            resp.len = snprintf(resp.message, sizeof(resp.message), "client[PID %ld]: %s\n", (long)msg.pid, msg.message);
            for (struct connection *cur = head; cur != NULL; cur = cur->next)
            {
                if (cur->pid == msg.pid)
                {
                    continue;
                }
                write(cur->fd, &resp, sizeof(struct response));
            }
        }
    }

    close(serverMessageFd);
    close(serverConnectionFd);
    unlink(SERVER_CONNECTION_FIFO);
    unlink(SERVER_MESSAGE_FIFO);
    return EXIT_SUCCESS;
}

struct connection *accept(int *fd)
{
    struct request req;
    memset(&req, 0, sizeof(struct request));
    ssize_t numRead = read(*fd, &req, sizeof(struct request));
    if (numRead == -1)
    {
        perror("read");
        return NULL;
    }

    if (numRead == 0)
    {
        close(*fd);
        *fd = open(SERVER_CONNECTION_FIFO, O_RDONLY | O_NONBLOCK);
        if (*fd == -1)
        {
            exit(0);
        }
        printf("server[PID %ld] reopen %s read side success\n", (long)getpid(), SERVER_CONNECTION_FIFO);
        return NULL;
    }

    struct connection *conn = malloc(sizeof(struct connection));
    if (conn == NULL)
    {
        perror("malloc");
        return NULL;
    }
    conn->pid = req.pid;
    conn->next = NULL;

    // 拼接 client　FIFO 路径
    char clientFIFOPath[CLIENT_FIFO_NAME_LEN] = {0};
    snprintf(clientFIFOPath, CLIENT_FIFO_NAME_LEN, CLIENT_FIFO_TEMPLATE, (long)req.pid);

    // 以 O_WRONLY 方式打开 client FIFO
    conn->fd = open(clientFIFOPath, O_WRONLY);
    if (conn->fd == -1)
    {
        perror(clientFIFOPath);
        return NULL;
    }
    printf("server[PID %ld] open %s write side success\n", (long)getpid(), clientFIFOPath);
    return conn;
}

void appendConnection(struct connection *conn)
{
    // 连接返回后插入到连接链表尾部
    if (head == NULL)
    {
        head = conn;
    }
    else
    {
        struct connection *cur = head;
        while (cur->next != NULL)
        {
            cur = cur->next;
        }
        cur->next = conn;
    }
}

void removeConnection(pid_t pid)
{
    if (head == NULL)
    {
        return;
    }

    struct connection *pre = NULL, *cur = head;

    while (cur != NULL && cur->pid != pid)
    {
        pre = cur;
        cur = cur->next;
    }

    if (pre != NULL)
    {
        pre->next = cur->next;
    }
    else
    {
        head = NULL;
    }
    close(cur->fd);
    free(cur);
}
```

下面代码是客户端的示例代码
```c title:client.c

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <fcntl.h>
#include <unistd.h>

#include "chat.h"

int main()
{
    int serverConnectionFd = open(SERVER_CONNECTION_FIFO, O_WRONLY | O_NONBLOCK);
    if (serverConnectionFd == -1)
    {
        perror("open");
        return EXIT_FAILURE;
    }
    printf("client[PID %ld] open %s write side success\n", (long)getpid(), SERVER_CONNECTION_FIFO);

    int serverMessageFd = open(SERVER_MESSAGE_FIFO, O_WRONLY | O_NONBLOCK);
    if(serverMessageFd == -1) {
        perror("open");
        return EXIT_FAILURE;
    }
    printf("client[PID %ld] open %s write side success\n", (long)getpid(), SERVER_MESSAGE_FIFO);


    // 拼接 client　FIFO 路径
    char clientFIFOPath[CLIENT_FIFO_NAME_LEN] = {0};
    snprintf(clientFIFOPath, CLIENT_FIFO_NAME_LEN, CLIENT_FIFO_TEMPLATE, (long)getpid());
    if (mkfifo(clientFIFOPath, 0666) == -1)
    {
        perror("mkfifo");
        return EXIT_FAILURE;
    }
    printf("client[PID %ld] create %s success\n",(long) getpid(), clientFIFOPath);

    // 发送请求建立连接
    struct request req = {.pid = getpid()};
    if (write(serverConnectionFd, &req, sizeof(struct request)) != sizeof(struct request))
    {
        fprintf(stderr, "connect error: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }
    // 等待连接成功
    int fdr = open(clientFIFOPath, O_RDONLY);
    if (fdr == -1)
    {
        perror("open");
        return EXIT_FAILURE;
    }
    printf("client[PID %ld] open %s read side success: CONNECTION SUCCESS\n", (long)getpid(), clientFIFOPath);

    // select 监听
    fd_set readfds;
    while (true)
    {
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);
        FD_SET(fdr, &readfds);
        select(fdr + 1, &readfds, NULL, NULL, NULL);
        if (FD_ISSET(STDIN_FILENO, &readfds))
        {
            // 从 stdin 读取消息并向 serverFd 写入消息
            struct message msg;
            memset(&msg, 0, sizeof(struct message));
            msg.pid = getpid();
            msg.len = read(STDIN_FILENO, msg.message, sizeof(msg.message));
            if (msg.len == -1 || msg.len == 0)
            {
                write(serverMessageFd, &msg, sizeof(struct message));
                perror("read");
                break;
            }
            write(serverMessageFd, &msg, sizeof(struct message));
        }
        if(FD_ISSET(fdr, &readfds)) {
            
            struct response resp;
            memset(&resp, 0, sizeof(struct response));
            resp.len = read(fdr, &resp, sizeof(struct response));
            if(resp.len == -1 || resp.len == 0) {
                perror("read");
                break;
            }
            write(STDOUT_FILENO, resp.message, resp.len);
        }
    }

    close(fdr);
    close(serverMessageFd);
    close(serverConnectionFd);
    unlink(clientFIFOPath);
    return EXIT_SUCCESS;
}
```
