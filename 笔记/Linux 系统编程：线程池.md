# 线程池

与进程池类似，创建和回收线程也是比较损耗性能的操作。通常，**在应用程序启动时就会创建一批线程**。每当任务到来时，就会唤醒一个线程用于执行该任务

与进程池不同的是，由于 **多线程是共享地址空间** 的，所以主线程和工作线程 **天然地** 通过共享文件描述符数值的形式 **共享网络文件对象**。但是这种共享也会带来麻烦：每当有客户端发起请求时，主线程会分配一个空闲的工作线程完成任务，而任务正是在多个线程之间共享的资源，所以需要采用一定的 **互斥** 和 **同步** 的机制来避免竞争

我们可以 **将任务设计成一个队列**，任务队列就成为 **多个线程同时访问的共享资源**，此时问题就转化成了一个典型的 **生产者-消费者** 问题：任务队列中的任务就是商品，主线程是生产者，每当有连接到来的时候，就将一个任务放入任务队列，即生产商品，而各个工作线程就是消费者，每当队列中任务到来的时候，就负责取出任务并执行。

下面是线程池的基本设计方案：

![[Pasted image 20241214232418.png]]

> [!tip] 主线程
> 
> 主线程执行与 [[Linux 系统编程：进程池]] 中主进程相似的任务：接收连接，生成任务，并将任务放入任务队列
> 

> [!tip] 工作线程
> 
> 工作线程与 [[Linux 系统编程：进程池]] 中的工作进程执行的任务类似：从任务队列中读取任务，并执行任务
> 

显然，在线程池中 **任务队列** 是一个非常重要的数据结构。下面我们主要开始设计并实现任务队列

## 任务队列

在线程池中，任务就是 **网络套接字+处理套接字的函数** 就形成了一个任务对象。我们将任务保存为一个 **链式队列**。由于系统资源的限制，进程文件描述符数量有限。因此，我们现在最大未处理任务数量(`MAX_TASK_QUEUE_SIZE`)为 $1024$ 

```c title:server/task.h
#ifndef TASK_H
#define TASK_H

#include <pthread.h>
#include <stdint.h>

#define MAX_TASK_QUEUE_SIZE 1024

typedef struct task task_t;

/**
 * @brief task handler function
 * 
 * @param fd the file descriptor of the network communication
 * @return on success, return 0; on error, return -1
 */
typedef int(*task_handler_t)(int fd);

/**
 * @brief task queue structure
 * 
 * + head: the head of the task queue
 * 
 * + tail: the tail of the task queue
 * 
 * + mutex: the mutex lock for the task queue
 * 
 * + cond: the condition variable for the task queue
 * 
 */
typedef struct task_queue {
    task_t *head;      // 任务队列的头
    task_t *tail;      // 任务队列的尾
    uint64_t size;       // 任务队列的大小
    pthread_mutex_t mutex;  // 互斥锁
    pthread_cond_t cond;    // 条件变量
} task_queue_t;

struct task {
    int fd;                 // 网络通信的文件描述符
    task_handler_t handler; // 任务处理函数
    task_t *next;           // 下一个任务
};

/**
 * @brief initiate a empty task queue
 * 
 * @param queue the task queue to be initiated
 * @return on success, return 0; on error, return -1
 */
int task_queue_init(task_queue_t * queue);

/**
 * @brief add a task to the task queue
 * 
 * @param queue the task queue, where the task will be added
 * @param fd the file descriptor of the network communication
 * @param handler the task handler function
 * @return on success, return 0; on error, return -1
 */
int task_queue_push(task_queue_t * queue, int fd, task_handler_t handler);

/**
 * @brief pop a task from the task queue
 * 
 * @param queue the task queue, where the task will be popped
 * @param t the task to be popped
 * @return on success, return 0; on error, return -1
 */
int task_queue_pop(task_queue_t * queue, task_t * t);
#endif // TASK_H
```

下面给出了任务队列的实现

```c title:server/task.c
#include <stdlib.h>
#include "task.h"


int task_queue_init(task_queue_t * queue) {
    if(queue == NULL) {
        return -1;
    }
    queue->head = NULL;
    queue->tail = NULL;
    queue->size = 0;
    // 初始化互斥锁
    if(pthread_mutex_init(&queue->mutex, NULL) != 0) {
        return -1;
    }
    // 初始化条件变量
    if(pthread_cond_init(&queue->cond, NULL) != 0) {
        return -1;
    }
    return 0;
}

int task_queue_push(task_queue_t * queue, int fd, task_handler_t handler) {
    if(queue == NULL) {
        return -1;
    }
    task_t *t = (task_t *)malloc(sizeof(task_t));
    if(t == NULL) {
        return -1;
    }
    t->fd = fd;
    t->handler = handler;
    t->next = NULL;
    // 加锁: 保护队列的操作
    if(pthread_mutex_lock(&queue->mutex) != 0) {
        return -1;
    }
    // 队列满了，等待子线程处理任务
    while (queue->size == MAX_TASK_QUEUE_SIZE)
    {
        // 等待条件变量发生变化
        if(pthread_cond_wait(&queue->cond, &queue->mutex) != 0) {
            return -1;
        }
    }
    
    if(queue->head == NULL) {
        queue->head = t;
        queue->tail = t;
    } else {
        queue->tail->next = t;
        queue->tail = t;
    }
    queue->size++;
    // 解锁：释放锁
    if(pthread_mutex_unlock(&queue->mutex) != 0) {
        return -1;
    }
    // 通知等待的线程条件变量发生变化
    if(pthread_cond_signal(&queue->cond) != 0) {
        return -1;
    }
    return 0;
}

int task_queue_pop(task_queue_t * queue, task_t * t) {
    if(queue == NULL || t == NULL) {
        return -1;
    }
    // 加锁: 保护队列的操作
    if(pthread_mutex_lock(&queue->mutex) != 0) {
        return -1;
    }
    // 队列为空，等待主线程将任务添加到队列
    while (queue->size == 0) 
    {
        // 等待条件变量发生变化
        if(pthread_cond_wait(&queue->cond, &queue->mutex) != 0) {
            return -1;
        }
    }
    // 从队列中取出任务
    *t = *(queue->head);
    
    // 释放队列中被取出的任务
    task_t *tmp = queue->head;
    queue->head = queue->head->next;
    free(tmp);
    queue->size--;
    // 队列为空
    if(queue->size == 0) {
        queue->tail = NULL;
    }
    // 解锁：释放锁
    if(pthread_mutex_unlock(&queue->mutex) != 0) {
        return -1;
    }
    // 通知主线程条件变量发生变化
    if(pthread_cond_signal(&queue->cond) != 0) {
        return -1;
    }
    return 0;
}
```

>[!important] 
>
> 我们将同步操作封装在任务队列中，后续业务代码就无需处理同步操作
>

## 线程池设计

### 子线程

一个线程池需要保存 **线程 ID** 和 **线程数量**。因此，线程池的数据结构设计如下。由于线程执行需要任务队列，因此在线程池初始化的时候，就需要提供任务队列，子线程从任务队列中获取任务

```c title:server/pool.h
#ifndef POOL_H
#define POOL_H

#include <pthread.h>
#include "task.h"

typedef struct thread_pool {
    int num_threads;
    pthread_t *threads;
} thread_pool_t;

/**
 * @brief Initialize a thread pool
 * 
 * @param pool pointer to the thread pool, must be allocated, which will be initialized
 * @param num_threads number of threads to create
 * @param queue pointer to the task queue, which will be used by the threads
 * @return on success, return 0, otherwise return -1
 */
int thread_pool_init(thread_pool_t *pool, int num_threads, task_queue_t *queue);

/**
 * @brief Destroy a thread pool
 * 
 * @param pool pointer to the thread pool to destroy, which will be destroyed
 * @return on success, return 0, otherwise return -1
 */
int thread_pool_destroy(thread_pool_t *pool);
#endif
```

线程池的实现如下

```c title:server/pool.c
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#include "pool.h"

// 线程入口函数
static void *routine(void *arg) {
    fprintf(stdout, "Thread[TID %#lx] is starting, who is waiting for task\n", (long int)pthread_self());
    task_queue_t *queue = (task_queue_t *)arg;
    while(true) {
        task_t task;
        // 从任务队列中获取任务
        if(task_queue_pop(queue, &task) == -1) {
            // get task error
            fprintf(stderr, "get task error\n");
            continue;
        }
        fprintf(stdout, "Thread[TID %#lx] is handling task for fd: %d\n", (long int)pthread_self(), task.fd);
        
        // 执行任务
        if(task.handler(task.fd) == -1) {
            // handle error
            fprintf(stderr, "handle error for fd: %d\n", task.fd);
            continue;
        }
        fprintf(stdout, "Thread[TID %#lx] has handled task for fd: %d\n", (long int)pthread_self(), task.fd);
    }
    return NULL;
}

int thread_pool_init(thread_pool_t *pool, int num_threads, task_queue_t *queue) {
    if(pool == NULL) {
        return -1;
    }

    pool->num_threads = num_threads;
    pool->threads = (pthread_t *)malloc(num_threads * sizeof(pthread_t));
    if(pool->threads == NULL) {
        return -1;
    }
    // 创建线程
    for(int i =0; i < num_threads; i++) {
	    // 将任务队列作为 routine 的参数传递给线程
        if(pthread_create(&pool->threads[i], NULL, routine, queue) != 0) {
            return -1;
        }
    }
    return 0;
}

int thread_pool_destroy(thread_pool_t *pool) {
    if(pool == NULL) {
        return -1;
    }

    free(pool->threads);
    return 0;
}
```

到此，线程池的基本结构就完成了。剩下的就是关于业务代码的实现了

### 主线程

主线程接收客户端连接，并将 **连接 SOCKET 和 处理函数** 添加到任务队列中

```c title:server/main.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <sys/epoll.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>


#include "tcp.h"
#include "epoll.h"
#include "task.h"
#include "pool.h"
#include "service.h"

#define HOSTNAME "127.0.0.1"
#define SERVERICE "8080"
#define WORKER_THREADS 5


int main(int argc, char *argv[]) {
    // 关闭标准输出的缓冲区
    setvbuf(stdout, NULL, _IONBF, 0);

    if(argc > 1 && strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "Usage: %s [hostname] [serviec] [worker number]\n", argv[0]);
        return 1;
    }
    char *hostname = argc > 1 ? argv[1] : HOSTNAME;
    char *service = argc > 2 ? argv[2] : SERVERICE;
    int worker_number = argc > 3 ? atoi(argv[3]) : WORKER_THREADS;

    // 初始化任务队列
    task_queue_t task_queue;
    if(task_queue_init(&task_queue) == -1) {
        fprintf(stderr, "Error: task_queue_init()\n");
        return 1;
    }

    // 创建线程池
    thread_pool_t pool;
    if(thread_pool_init(&pool, worker_number, &task_queue) == -1) {
        fprintf(stderr, "Error: thread_pool_init()\n");
        return 1;
    }

    // 建立监听套接字
    int sockfd = listenTCPSocket(hostname, service, PROTOCOL_IP);
    if (sockfd == -1) {
        fprintf(stderr, "Error: listenTCPSocket() %s\n", strerror(errno));
        return 1;
    }

    // 创建epoll实例
    int epfd = epoll_create1(EPOLL_CLOEXEC);
    if(epfd == -1) {
        fprintf(stderr, "Error: epoll_create1() %s\n", strerror(errno));
        return 1;
    }

    // 将监听套接字添加到epoll实例中
    if(epoll_add(epfd, sockfd, EPOLLIN) == -1) {
        fprintf(stderr, "Error: epoll_add() %s\n", strerror(errno));
        return 1;
    }

    // 将 exitPipe 的读端添加到 epoll 实例中
    if(epoll_add(epfd, exitPipe[0], EPOLLIN) == -1) {
        fprintf(stderr, "Error: epoll_add() %s\n", strerror(errno));
        return 1;
    }

    // 事件循环
    size_t max_events = 1024;
    struct epoll_event *events = (struct epoll_event *)malloc(sizeof(struct epoll_event) * max_events);
    while (true)
    {
        // 等待事件
        int readyNumber = epoll_wait(epfd, events, max_events, -1);
        if(readyNumber == -1) {
            fprintf(stderr, "Error: epoll_wait() %s\n", strerror(errno));
            break;
        }
        // 处理事件
        for(int i = 0; i < readyNumber; i++) {
            if(events[i].data.fd == sockfd) {
                // 有连接请求
                int connfd = accept(sockfd, NULL, NULL);
                if(connfd == -1) {
                    fprintf(stderr, "Error: accept() %s\n", strerror(errno));
                }
                // 添加任务
                if(task_queue_push(&task_queue, connfd, handler) == -1) {
                    fprintf(stderr, "Error: task_queue_push()\n");
                }
            }
        }
    }
    return 0;
}
```

## 业务：下载文件

这里业务与 [[Linux 系统编程：进程池]] 中的业务代码类似。

```c title:server/service.h
#ifndef SERVICE_H
#define SERVICE_H
#include <stdbool.h>
#include <sys/types.h>


typedef unsigned char byte;

typedef struct {
    size_t nameLength;    // 文件名长度
    off_t filesize;   // 文件大小
    byte *filename;   // 文件名
} hdr_t;

/**
 * @brief The tasks handling function of the process
 * 
 * @param connfd Connection socket file descriptor
 * @return on success return 0, otherwise return -1
 */
int handler(int connfd);

/**
 * @brief Send file
 * 
 * @param connfd Connection socket file descriptor
 * @param path file path
 * @return on success return 0, otherwise return -1
 */
int sendfile(int connfd, const char *path);

/**
 * @brief Send file header
 * 
 * @param connfd Connection socket file descriptor
 * @param hdr contains the data to be sent
 * @return on success return 0, otherwise return -1 
 */
int sendhdr(int connfd, const hdr_t *hdr);

/**
 * @brief send date to the socket，which is resloved partially send problem
 * 
 * @param connfd Connection socket file descriptor
 * @param buffer The data to be sent
 * @param size The size of the data to be sent
 * @return int 
 */

int sendn(int connfd, const void *buffer, size_t size);

#endif  // SERVICE_H
```

```c title:server/service.c
/* 业务相关函数 */
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

#include "service.h"

int handler(int fd)
{
    
    // 处理任务：发送文件
    if (sendfile(fd, "file.txt") == -1)
    {
        fprintf(stderr, "sendfile() failed\n");
    }
    
    // 关闭连接
    if (close(fd) == -1)
    {
        return -1;
    }
    return 0;
}

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

## 线程池的退出

**在多线程应用中，尽量不要使用信号机制**。原因是当多线程进程执行过程中，一旦产生了信号，则 **被中断的线程是未知的**，有可能是主线程也有可能是子线程。这样的话，线程终止的处理就非常繁琐，也不够清晰明了

解决这个问题方式是在原来设计的基础上 **引入多进程机制**。在引用程序启动的时候，创建一个子进程。子进程负责创建线程池，父子进程之间通过一个管道通信。

> [!tip] 
> 
> 信号产生时，父进程处理该信号，并在信号处理函数中向管道写入一个消息。
> 
> 此时，子进程通过 IO 多路复用机制监听管道的读端，这样一旦管道就绪，子进程的主线程得知程序将要终止，随后依次关闭子线程
> 

线程退出可以 **正常终止** 或者 **被取消**。 如果线程被取消，当线程执行到取消点时，线程就会结束。然而，对于我们的业务，`send()` 系统调用本身就是一个取消点。为了保证数据的完整性，因此我们不能使用 `pthread_cancle()` 函数

为了避免使用取消线程，即不使用 `pthread_cancle()` 函数。我们可以 **向任务队列中添加终止任务**，当线程执行终止任务之后，线程就会被终止

```c title:server/main.c hl:26,28-36,42-80,124-128,153-172
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <sys/epoll.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>


#include "tcp.h"
#include "epoll.h"
#include "task.h"
#include "pool.h"
#include "service.h"

#define HOSTNAME "127.0.0.1"
#define SERVERICE "8080"
#define WORKER_THREADS 5

int exitPipe[2];

void sigusr1_handler(int sig) {
    int err = errno;
    fprintf(stdout, "Receive SIGUSR1\n");
    if(write(exitPipe[1], "exit", 4) == -1) {
        fprintf(stderr, "Error: write() %s\n", strerror(errno));
    }
    fprintf(stdout, "Send exit signal\n");
    errno = err;
}

int main(int argc, char *argv[]) {
    // 关闭标准输出的缓冲区
    setvbuf(stdout, NULL, _IONBF, 0);
    // 创建管道
    if(pipe(exitPipe) == -1) {
        fprintf(stderr, "Error: pipe() %s\n", strerror(errno));
        return 1;
    }
    // 创建子进程
    switch (fork())
    {
    case -1:
        fprintf(stderr, "Error: fork() %s\n", strerror(errno));
        break;
    case 0:  // 子进程
        // 关闭 exitPipe 的写端
        close(exitPipe[1]);
        break;
    default:  // 父进程
        // 关闭 exitPipe 的读端
        close(exitPipe[0]);
        // 处理信号
        signal(SIGUSR1, sigusr1_handler);
        // 等待子进程结束
        while (true)
        {
            int status;
            // 等待子进程结束
            pid_t pid = wait(&status);
            if(pid == -1) {
                if(errno == EINTR) {
                    continue;
                } else if (errno == ECHILD) {
                    fprintf(stdout, "All child process is exited\n");
                    break;
                }
                fprintf(stderr, "Error: wait() %s\n", strerror(errno));
                break;
            }
        }
        exit(0);
        break;
    }

    if(argc > 1 && strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "Usage: %s [hostname] [serviec] [worker number]\n", argv[0]);
        return 1;
    }
    char *hostname = argc > 1 ? argv[1] : HOSTNAME;
    char *service = argc > 2 ? argv[2] : SERVERICE;
    int worker_number = argc > 3 ? atoi(argv[3]) : WORKER_THREADS;

    // 初始化任务队列
    task_queue_t task_queue;
    if(task_queue_init(&task_queue) == -1) {
        fprintf(stderr, "Error: task_queue_init()\n");
        return 1;
    }

    // 创建线程池
    thread_pool_t pool;
    if(thread_pool_init(&pool, worker_number, &task_queue) == -1) {
        fprintf(stderr, "Error: thread_pool_init()\n");
        return 1;
    }

    // 建立监听套接字
    int sockfd = listenTCPSocket(hostname, service, PROTOCOL_IP);
    if (sockfd == -1) {
        fprintf(stderr, "Error: listenTCPSocket() %s\n", strerror(errno));
        return 1;
    }

    // 创建epoll实例
    int epfd = epoll_create1(EPOLL_CLOEXEC);
    if(epfd == -1) {
        fprintf(stderr, "Error: epoll_create1() %s\n", strerror(errno));
        return 1;
    }

    // 将监听套接字添加到 epoll 实例中
    if(epoll_add(epfd, sockfd, EPOLLIN) == -1) {
        fprintf(stderr, "Error: epoll_add() %s\n", strerror(errno));
        return 1;
    }

    // 将 exitPipe 的读端添加到 epoll 实例中
    if(epoll_add(epfd, exitPipe[0], EPOLLIN) == -1) {
        fprintf(stderr, "Error: epoll_add() %s\n", strerror(errno));
        return 1;
    }

    // 事件循环
    size_t max_events = 1024;
    struct epoll_event *events = (struct epoll_event *)malloc(sizeof(struct epoll_event) * max_events);
    while (true)
    {
        // 等待事件
        int readyNumber = epoll_wait(epfd, events, max_events, -1);
        if(readyNumber == -1) {
            fprintf(stderr, "Error: epoll_wait() %s\n", strerror(errno));
            break;
        }
        // 处理事件
        for(int i = 0; i < readyNumber; i++) {
            if(events[i].data.fd == sockfd) {
                // 有连接请求
                int connfd = accept(sockfd, NULL, NULL);
                if(connfd == -1) {
                    fprintf(stderr, "Error: accept() %s\n", strerror(errno));
                }
                // 添加任务
                if(task_queue_push(&task_queue, connfd, handler) == -1) {
                    fprintf(stderr, "Error: task_queue_push()\n");
                }
            } else if(events[i].data.fd == exitPipe[0]) {
                // 退出事件
                for(int j = 0; j < worker_number; j++) {
                    // 添加终止任务
                    if(task_queue_push(&task_queue, EXIT_TASK_FD, NULL) == -1) {
                        fprintf(stderr, "Error: task_queue_push()\n");
                    }
                }
                // 等待线程结束
                for(int j = 0; j < worker_number; j++) {
                    // 等待线程结束
                    pthread_join(pool.threads[j], NULL);
                    fprintf(stdout, "Thread[TID %#lx] is exited\n", (long int)pool.threads[j]);
                }
                // 释放任务队列
                task_queue_destroy(&task_queue);
                // 释放线程池
                thread_pool_destroy(&pool);
                _exit(0);
            }
        }
    }
    return 0;
}
```

上述代码中 **高亮部分** 就是为完善线程池退出 **在主线程中添加的代码**

> [!important] 在线程入口函数中，每当获取任务之后，就 **检查任务是否是退出任务**。
> + 如果是，则退出线程
> + 如果不是，则执行任务

```c title: title:server/pool.c hl:11-15
static void *routine(void *arg) {
    fprintf(stdout, "Thread[TID %#lx] is starting, who is waiting for task\n", (long int)pthread_self());
    task_queue_t *queue = (task_queue_t *)arg;
    while(true) {
        task_t task;
        if(task_queue_pop(queue, &task) == -1) {
            // get task error
            fprintf(stderr, "get task error\n");
            continue;
        }
        if(task.fd == EXIT_TASK_FD) {
            // 线程终止
            fprintf(stdout, "Thread[TID %#lx] is exiting\n", (long int)pthread_self());
            break;
        }
        fprintf(stdout, "Thread[TID %#lx] is handling task for fd: %d\n", (long int)pthread_self(), task.fd);
        if(task.handler(task.fd) == -1) {
            // handle error
            fprintf(stderr, "handle error for fd: %d\n", task.fd);
            continue;
        }
        fprintf(stdout, "Thread[TID %#lx] has handled task for fd: %d\n", (long int)pthread_self(), task.fd);
    }
    return NULL;
}
```

`EXIT_TASK_FD` 是在 `pool.h` 中定义的宏

```c title:server/pool.h
#define EXIT_TASK_FD -1
```

在添加任务时，只需要将与任务关联的 `fd` 设置为 `EXIT_TASK_FD` 就能导致线程正常终止

> [!tip] 
> 
> 如果需要使用 `pthread_cancle()`，请参考 [[Linux 系统编程：线程取消]]
> 
