# Linux epoll

在 [[IO 多路复用]] 中介绍了 `select()` 和 `poll()`，但是它们有非常巨大的缺陷

> [!warning] `select()` 和 `poll()` 的缺点
> 
> + `fd_set` 这一个 **位图** 使用数组实现，**大小是固定的**。如果需要修改它的长度，需要重新编译
> + 需要将 `fd_set` 从用户空间拷贝到内核空间。如果监控的文件描述符过多，拷贝是耗时的
> + 监听集合和就绪集合使用同一个变量，从而出现 **耦合**。在一个循环中，每次监听的文件描述符不同，**会带来编程上的复杂性**
> + 内核使用 **轮询** 方式获取就绪文件描述符集合。如果 **监控的文件描述符很多**，但是 **就绪的文件描述符很少** 时，发现这些就绪的文件描述符将 **浪费 CPU**
> 
> 更严重的缺点：
> 
> + 每当 `select()` 返回，我们 **并不知道哪个文件描述符就绪** 了，还需要一个个的询问
> 

之后，介绍了 [[信号驱动 IO]]，它可以避免轮询，相比较 `select()` 和 `poll()` 具有更高的性能

> [!warning] 信号驱动 IO 的缺陷：可排队的实时信号的数量有限
> 
> 如果达到这个上限，内核对于 “I/O 就绪” 的通知将恢复为默认的 SIGIO 信号
> 
> 出现这种现象表示信号队列溢出了。当出现这种情况时，我们将失去有关文件描述符上发生 I/O 事件的信息，因为 SIGIO 信号是不会排队的
> 

现在，我们介绍 **Linux 特有的 IO 多路复用机制 epoll(event poll)**。它和 I/O 多路复用和信号驱动 I/O 一样可以检查多个文件描述符上的 I/O 就绪状态

> [!tip] Linux epoll 是 Linux 特有的 IO 多路复用机制

epoll API 的主要优点如下

> [!tip] 优点一：当检查大量的文件描述符时，`epoll` 的性能延展性比 `select()` 和 `poll()` 高很多
> 
> 检查大量的文件描述符时，epoll 提供了更好的性能
> 

> [!tip] 优点二：epoll API 既支持水平触发也支持边缘触发
> 
> + `select()` 和 `poll()` 只支持水平触发
> + 信号驱动 IO 只支持边缘触发
>
> **水平触发**：如果文件描述符上 **可以非阻塞地执行 I/O 系统调用**，此时认为它已经就绪
> 
> **边缘触发**：如果文件描述符自上次状态检查以来 **有了新的 I/O 活动**，此时需要触发通知
> 

> [!tip] 优点三：可以避免复杂的信号处理流程
> 
> 比如信号队列溢出时的处理
> 

> [!tip] 优点四：灵活性高，可以指定我们希望检查的事件类型
> 
> 检查套接字文件描述符的读就绪、写就绪或者两者同时指定
> 

---
## epoll 模型

epoll API 的核心数据结构称作 **epoll 实例**，**和一个打开的文件描述符相关联**

> [!tip] epoll 实例：epoll API 的核心数据结构，通过 **文件描述符** 引用
> 
> 这个文件描述符不是用来做 I/O 操作的，相反，它是 **内核数据结构的句柄**
>

> [!attention] 这些内核数据结构实现了两个目的
> 
> + 记录了在进程中声明过的 **感兴趣的文件描述符列表**，即：**interest list（兴趣列表）**
> 	+ 是通过 **红黑树** 实现的。参考 [[红黑树]]
> 	+ 可以无限制的存储文件描述符
> 
> + 维护了 **处于 I/O 就绪态的文件描述符列表**，即：**ready list（就绪列表）**
> 	+ 使用线性表，通常是 [[栈和队列#队列]] 实现
>
> ready list 中的成员是 interest list 的子集
> 

对于由 epoll 检查的每一个文件描述符，我们可以指定一个 **位掩码** 来表示我们 **感兴趣的事件**。这些位掩码同 `poll()` 所使用的位掩码有着紧密的关联

epoll API 由以下 $3$ 个系统调用组成

+ `epoll_create()`：创建 epoll 实例，返回一个代表 epoll 实例的文件描述符
+ `epoll_ctl()`：操作同 epoll 实例关联的 interest list
	+ 添加新的文件描述到 interest list
	+ 从 interest list 中移除文件描述符
	+ 修改代表文件描述符上事件的位掩码
+ `epoll_wait()`：返回与 epoll 实例相关联的就绪列表中的成员

## 创建 epoll 实例

系统调用 `epoll_create()` 创建了一个新的 epoll 实例，其对应的兴趣列表初始化为空

```c
#include <sys/epoll.h>

int epoll_create(int size);
/* 成功返回文件描述符，失败返回 -1 */
```

> [!tip] 参数 `size`：通过 epoll 实例检查文件描述符的个数
> 
> **该参数并不是一个上限**，而是告诉内核应该如何为内部数据结构划分初始大小
> 
> 从 Linux 2.6.8 开始，该参数被忽略，内核实现做了修改意味着该参数之前提供的信息已经不再需要了
> 

> [!tip] 返回值
> 
> 成功返回代表新建 **epoll 实例的文件描述符**。这是后续 epoll API 中用来表示 epoll 实例
> 

当不在需要时，应该通过 `close()` 来关闭。当所有与 epoll 实例相关的文件描述符都被关闭时，实例被销毁，相关的资源都返还给系统

## 修改 epoll 的兴趣列表

系统调用 `epoll_ctl()` 能够修改由文件描述符 `epfd` 所代表的 `epoll` 实例中的兴趣列表

```c
#include <sys/epoll.h>

int epoll_ctl(int epfd, int op, int fd, struct poll_event *ev);
/* 成功返回 0；失败返回 -1 */
```

> [!tip] 参数 `epfd`：epoll 实例的文件描述符
> 
> 通常是 `epoll_create()` 的返回值
> 

> [!tip] 参数 `fd`：interest list 中 的文件描述符
> 
> 该参数 **不能是普通文件或者目录** 的文件描述符。除此之外，可以是
> + PIPE 和 FIFO
> + SOCKET
> + POSIX 消息队列
> + 设备
> + 另一个 epoll 实例
> 

> [!tip] 参数 `op`：指定 `epoll_ctl()` 执行的操作
> 
> + `EPOLL_CTL_ADD`：将参数 `fd` 指定的文件描述符添加到 interest list 中
> 	+ 对于在 `fd` 上感兴趣的事件通过参数 `ev` 指定
> 	+ 如果 interest list 中存在 `fd`，则 `epoll_ctl()` 将出现 `EEXIST` 错误
> + `EPOLL_CTL_MOD`：修改参数 `fd` 上设定的感兴趣的事件
> 	+ 新感兴趣的事件通过参数 `ev` 指定
> 	+ 如果 interest list 中不存在 `fd`，则 `epoll_ctl()` 将出现 `ENOENT` 错误
> + `EPOLL_CTL_DE`：从 interest list 中删除参数 `fd` 指定的文件描述符
> 	+ 如果 interest list 中不存在 `fd`，则 `epoll_ctl()` 将出现 `ENOENT` 错误
> 

下表总结了参数 `op` 的取值

| `op`            | 描述                              |
| :-------------- | :------------------------------ |
| `EPOLL_CTL_ADD` | 向 interest list 中添加文件描述符        |
| `EPOLL_CTL_MOD` | 修改 interest list 中指定文件描述符的感兴趣事件 |
| `EPOLL_CTL_DEL` | 删除 interest list 中指定的文件描述符      |

> [!tip] 参数 `ev`：指向结构体 `epoll_event` 的指针
> 
> 结构体的定义如下
> 
> ```c
> struct epoll_event {
> 	uint32_t events;  // epoll events，待检查事件的集合
> 	epoll_data_t data; // 用户数据，fd 就绪时返回给调用进程的信息
> }
> ```
> 
> `epoll_data_t` 定义如下
> 
> ```c
> typedef union epoll_data {
> 	void * ptr;  // 指向用户定义的数据
> 	int fd;      // 文件描述符
> 	uint32_t u32; // 32 位整数
> 	uint64_t u64; // 64 位整数
> } epoll_data_t
> ```
> 
> 参数 `ev` 为文件描述符 `fd` 所做的设置如下
> 
> + 结构体 `epoll_event` 中的 `events` 字段是一个位掩码，它指定了我们为 **待检查的描述符 fd 上所感兴趣的事件集合**
> + `data` 字段是一个联合体，当 **描述符 fd 稍后成为就绪态时**，联合体的成员可 **用来指定传回给调用进程的信息**
> 	+ 唯一可获知同这个事件相关的文件描述符号的途径
> 
> 

下面片段展示了如何使用 `epoll_create()` 和 `epoll_ctl()` 的例子

```c
int epfd;
struct epoll_event ev;

epfd = epoll_create(5);
if(epfd == -1) {
	// 错误处理
}

ev.data.fd = fd;
ev.events = EPOLLIN;
if(epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1) {
	// 错误处理
}
```

> [!warning] `max_user_watches`
> 
> 每个 **注册到 epoll 实例上的文件描述符** 需要占用一小段 **不能被交换的内核内存空间**，因此内核提供了一个接口用来定义每个用户可以注册到 epoll 实例上的文件描述符总数
> 
> 这个上限值可以通过 `max_user_watches` 来查看和修改，它是专属于 Linux 系统的 `/proc/sys/fd/epoll` 目录下的一个文件
> 
> 默认的上限值根据可用的系统内存来计算得出
> 

## 事件等待

系统调用 `epoll_wait()` 返回 epoll 实例中 **处于就绪态的文件描述符信息**。单个 `epoll_wait()` 调用能 **返回多个就绪态文件描述符的信息**。

```c
#include <sys/epoll.h>

int epoll_wait(int epfd, struct epoll_event *evlist, int maxevents, int timeout);
/* 成功返回就绪文件描述符数量；失败返回 -1 */
```

> [!tip] 参数 `epfd`：epoll 实例的文件描述符
> 
> 通常是 `epoll_create()` 的返回值
> 

> [!tip] 参数 `evlist`：指向的 **结构体 `epoll_event` 数组**，用于返回 **就绪态文件描述符** 的信息
> 
> 在数组 `evlist` 中，**每个元素返回的都是单个就绪态文件描述符的信息**，每个元素的字段表示的信息如下
> + `events` 字段：在该描述符上 **已经发生的事件掩码**
> + `data` 字段返回的是我们在描述符上使用 `cpoll_ctl()` 注册感兴趣的事件时在 `ev.data` 中所指定的值
> 	+ `data` 字段是 **唯一可获知同这个事件相关的文件描述符号的途径**
> 
> 因此，当我们调用 `epoll_ctl()` 将文件描述符添加到兴趣列表中时
> + 要么将 `ev.data.fd` 设为文件描述符号
> + 要么将`ev.data.ptr` 设为指向包含文件描述符号的结构体
> 

> [!tip] 参数 `maxevents`：`evlist` 结构体数组的长度
> 
> `evlist` 数组的最大长度
> 

> [!tip] 参数 `timeout`：确定 `epoll_wait()` 的阻塞行为
> 
> + 如果 `timeout` 等于 $−1$，调用将一直 **阻塞**
> 	+ 直到兴趣列表中的文件描述符上有事件产生
> 	+ 直到捕获到一个信号为止
> + 如果 `timeout` 等于 $0$，执行一次 **非阻塞** 式的检查，看兴趣列表中的文件描述符上产生了哪个事件
> + 如果 `timeout` 大于 $0$，调用将 **阻塞至多 `timeout` 毫秒**
> 	+ 直到文件描述符上有事件发生
> 	+ 直到捕获到一个信号为止
> 

下表总结了 `timeout` 取值

| `timeout` | 描述                               |
| :-------- | -------------------------------- |
| $-1$      | `epoll_wait()` 阻塞，直到文件描述符上有事件产生  |
| $0$       | `epoll_wait()` 非阻塞               |
| $\gt 0$   | `epoll_wait()` 阻塞至多 `timeout` 毫秒 |

> [!tip] 返回值
> 
> + 成功返回数组 `evlist` 中的元素个数
> + 如果在 `timeout` 毫秒后，没有任何文件描述符处于就绪状态话，返回 $0$
> + 出错返回 $-1$，并在 `errno` 中设定错误码以表示错误原因
> 

在多线程程序中，**可以在一个线程中使用 `epoll_ctl()` 将文件描述符添加到另一个线程中由 `epoll_wait()` 所监视的 epoll 实例的兴趣列表中去**。这些对兴趣列表的修改将立刻得到处理，而 `epoll_wait()` 调用将返回有关新添加的文件描述符的就绪信息

### epoll 事件

下表中给出了我们调用 `epoll_ctl()` 时可以在 `ev.events` 中指定的位掩码以及由 `epoll_wait()` 返回的 `evlist[i].events` 中的值

| 位掩码            | 是否输入 | 是否返回 | 描述            |
| :------------- | ---- | ---- | ------------- |
| `EPOLLIN`      | 是    | 是    | 可读取非高优先级的数据   |
| `EPOLLPRI`     | 是    | 是    | 可读取高优先级数据     |
| `EPOLLRDHUP`   | 是    | 是    | 套接字对端关闭       |
| `EPOLLOUT`     | 是    | 是    | 普通数据可写        |
|                |      |      |               |
| `EPOLLET`      | 是    |      | 采用边缘触发事件通知    |
| `EPOLLONESHOT` | 是    |      | 在完成事件通知之后禁用检查 |
|                |      |      |               |
| `EPOLLERR`     |      | 是    | 有错误发生         |
| `EPOLLHUP`     |      | 是    | 出现挂断          |

除了有一个额外的前缀 `E` 外，大多数这些位掩码的名称同 `poll()` 中对应的事件掩码名称相同。这种名称上有着对应关系的原因是当我们在 `epoll_ctl()` 中指定输入，或通过 `epoll_wait()` 得到输出时，这些比特位表达的意思同对应的 `poll()` 的事件掩码所表达的意思一样

### EPOLLONESHOT 标志

默认情况下，通过 `epoll_ctl()` 的 `EPOLL_CTL_ADD` 将文件描述符 `fd` 添加到 interest list 中时，后续对 `epoll_wait()` 的调用会在描述符 `fd` 处于就绪态时通知我们，直到显示通过 `epoll_ctl()`的 `EPOLL_CTL_DEL` 操作将其从 interest list 中移出

如果我们希望在某个特定的文件描述符上只得到一次通知，那么可以在传给 `epoll_ctl()` 的 `ev.events` 中指定 `EPOLLONESHOT` 标志。**这个文件描述符的事件只会触发一次**。事件被触发并处理后，该文件描述符将不再被 `epoll` 监视，除非你显式地重新将其添加到 `epoll` 实例中

> [!summary] `EPOLLNESHOT` 标志，表示只监视文件描述符一次
> 
> 这个选项的主要用途是 **在你只希望处理事件一次的场景** 中，它可以 **避免每次事件触发时都重新检查该文件描述符**
> 
> 处理完事件后，必须手动将该文件描述符重新添加到 `epoll` 中，以便继续监视它。
> 

下面的代码给出了 `EPOLLONESHOT` 标志的使用

```c
struct epoll_event event;

event.events = EPOLLIN | EPOLLONESHOT;
event.data.fd = fd;

epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event);
```

## 示例程序

下面的示例程序是 [[IO 多路复用]] 中两个进程通信的例子使用 epoll 的实现

```c title:programA_epoll.c
#include <sys/types.h>  
#include <sys/epoll.h>  
#include <sys/stat.h>  
#include <fcntl.h>  
#include <unistd.h>  
  
#include "base.h"  
#include "rdwrn.h"  
  
int setNonBlock(int fd) {  
    int flags = fcntl(fd, F_GETFL);  
    if (flags == -1) {  
        return -1;  
    }  
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {  
        return -1;  
    }  
    return 0;  
}  
  
int main(int argc, char *argv[]) {  
  
    // programA_epoll A2B_FIFO B2A_FIFO  
    if(argc != 3) {  
        usageErr("%s <fifo 1> <fifo 2>", argv[0]);  
    }  
  
    // 以 O_RDONLY 打开 B2A_FIFO    int b2a = open(argv[2], O_RDONLY);  
    if (b2a == -1) {  
        errExit("open: ");  
    }  
    printf("programA open %s read side success\n", argv[2]);  
  
    // 以 O_WRONLY | O_NONBLOCK 打开 A2B_FIFO    int a2b = open(argv[1], O_WRONLY);  
    if (a2b == -1) {  
        errExit("open: ");  
    }  
    printf("programA open %s write side success\n", argv[1]);  
  
    // 创建 epoll 实例  
    int epfd = epoll_create(5);  
    if (epfd == -1) {  
        errExit("epoll_create: ");  
    }  
    printf("programA create epoll instance success\n");  
  
    // 将 STDIN_FILENO 添加到 interest list 中  
    struct epoll_event ev;  
    ev.data.fd = STDIN_FILENO;  
    ev.events = EPOLLIN;  
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, STDIN_FILENO, &ev) == -1) {  
        errExit("epoll_ctl: ");  
    }  
  
    // 将 b2a 添加到 interest list 中  
    ev.data.fd = b2a;  
    ev.events = EPOLLIN | EPOLLHUP;  
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, b2a, &ev) == -1) {  
        errExit("epoll_ctl: ");  
    }  
  
    while (true) {  
        struct epoll_event events[2];  
        int nfds = epoll_wait(epfd, events, 2, -1);  
        if (nfds == -1) {  
            if (errno == EINTR) {  
                continue;  
            }  
            fprintf(stderr, "epoll_wait: %s\n", strerror(errno));  
            break;  
        }  
  
        for (int i =0; i < nfds; i++) {  
            // 读事件就绪  
            if (events[i].events & EPOLLIN) {  
                char buffer[BUFSIZ];  
                ssize_t numRead = read(events[i].data.fd, buffer, BUFSIZ);  
                if (numRead == -1) {  
                    errExit("read: ");  
                }  
                // STDIN_FILENO 就绪  
                if (events[i].data.fd == STDIN_FILENO) {  
                    if (numRead == 0) {  
                        exit(EXIT_SUCCESS);  
                    }  
                    // 读取的内容写入到 a2b 中  
                    if (writen(a2b, buffer, numRead) == -1) {  
                        errExit("write: ");  
                    }  
                } else {  // b2a 就绪  
                    // 读取的内容写入到标准输出  
                    if (writen(STDOUT_FILENO, buffer, numRead) == -1) {  
                        errExit("write: ");  
                    }  
                }  
            }  
            // 挂起事件  
            if (events[i].events & EPOLLOUT) {  
                fprintf(stderr, "Peer exit\n");  
            }  
        }  
    }  
    return 0;  
}
```

```c title:programB_epoll.c
  #include <sys/types.h>  
#include <sys/epoll.h>  
#include <sys/stat.h>  
#include <fcntl.h>  
#include <unistd.h>  
  
#include "base.h"  
#include "rdwrn.h"  
  
int setNonBlock(int fd) {  
    int flags = fcntl(fd, F_GETFL);  
    if (flags == -1) {  
        return -1;  
    }  
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {  
        return -1;  
    }  
    return 0;  
}  
  
int main(int argc, char *argv[]) {  
  
    // programB_epoll A2B_FIFO B2A_FIFO  
    if(argc != 3) {  
        usageErr("%s <fifo 1> <fifo 2>", argv[0]);  
    }  
  
    // 以 O_WRONLY 打开 B2A_FIFO    int b2a = open(argv[2], O_WRONLY);  
    if (b2a == -1) {  
        errExit("open: ");  
    }  
    printf("programA open %s write side success\n", argv[2]);  
  
    // 以 O_RDONLY 打开 A2B_FIFO    int a2b = open(argv[1], O_RDONLY);  
    if (a2b == -1) {  
        errExit("open: ");  
    }  
    printf("programB open %s read side success\n", argv[1]);  
  
    // 创建 epoll 实例  
    int epfd = epoll_create(5);  
    if (epfd == -1) {  
        errExit("epoll_create: ");  
    }  
    printf("programA create epoll instance success\n");  
  
    // 将 STDIN_FILENO 添加到 interest list 中  
    struct epoll_event ev;  
    ev.data.fd = STDIN_FILENO;  
    ev.events = EPOLLIN;  
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, STDIN_FILENO, &ev) == -1) {  
        errExit("epoll_ctl: ");  
    }  
  
    // 将 a2b 添加到 interest list 中  
    ev.data.fd = a2b;  
    ev.events = EPOLLIN | EPOLLHUP;  
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, a2b, &ev) == -1) {  
        errExit("epoll_ctl: ");  
    }  
  
    while (true) {  
        struct epoll_event events[2];  
        int nfds = epoll_wait(epfd, events, 2, -1);  
        if (nfds == -1) {  
            if (errno == EINTR) {  
                continue;  
            }  
            fprintf(stderr, "epoll_wait: %s\n", strerror(errno));  
            break;  
        }  
  
        for (int i =0; i < nfds; i++) {  
            // 读事件就绪  
            if (events[i].events & EPOLLIN) {  
                char buffer[BUFSIZ];  
                ssize_t numRead = read(events[i].data.fd, buffer, BUFSIZ);  
                if (numRead == -1) {  
                    errExit("read: ");  
                }  
                // STDIN_FILENO 就绪  
                if (events[i].data.fd == STDIN_FILENO) {  
                    if (numRead == 0) {  
                        exit(EXIT_SUCCESS);  
                    }  
                    // 读取的内容写入到 b2a 中  
                    if (writen(b2a, buffer, numRead) == -1) {  
                        errExit("write: ");  
                    }  
                } else {  // a2b 就绪  
                    // 读取的内容写入到标准输出  
                    if (writen(STDOUT_FILENO, buffer, numRead) == -1) {  
                        errExit("write: ");  
                    }  
                }  
            }  
            // 挂起事件  
            if (events[i].events & EPOLLOUT) {  
                fprintf(stderr, "Peer exit\n");  
            }  
        }  
    }  
    return 0;  
}
```

## 深入探究 epoll 的语义

现在我们来看看 **打开的文件同文件描述符以及 epoll 之间交互的一些细微之处**。基于本次讨论的目的，回顾一下在 [[深入文件 IO#文件描述符和打开文件的关系]] 中展示的 **文件描述符**，**打开的文件描述**（file description），以及整个系统的 **文件 i-node 表** 之间的关系

![[Pasted image 20241015143221.png]]

> [!tip] epoll_create()：首先创建一个 $\text{i-node}$；然后 **打开文件描述**；最后分配一个文件描述符
> 
> 内核在内存中 **创建了一个新的 $\text{i-node}$** 并 **打开文件描述**，随后在调用进程中为打开的这个文件描述 **分配一个新的文件描述符**
> 

> [!attention] 注意：interest list 关联的是 **打开文件描述**，而不是 epoll 文件描述符
> 
> 同 epoll 实例的兴趣列表相关联的是打开的文件描述，而不是 epoll 文件描述符。这将产生下列结果
> 
> + 如果我们使用 `dup()`，或类似的函数，复制一个 `epoll` 文件描述符，那么被复制的描述符所指代的 `epoll` 兴趣列表和就绪列表同原始的 `epoll` 文件描述符相同
> 	+ 若要修改兴趣列表，在 `epoll_ctl()` 的参数 `epfd` 上设定文件描述符可以是原始的也可以是复制的
> + 同样也适用于 `fork()` 调用之后的情况。此时子进程通过继承复制了父进程的 `epoll` 文件描述符，而这个复制的文件描述符所指向的 `epoll` 数据结构同原始的描述符相同
> 

> [!tip] epoll_ctl() 的 `EPOLL_CTL_ADD`：内核在 epoll 的 interest list 中添加一个元素
> 
> 这个元素同时记录了需要检查的文件描述符数量以及对应的打开文件描述的引用
> 

> [!tip] epoll_wait()：**让内核负责监视打开的文件描述**
> 
> 之前的观点就有必要更新一下
> + **更新前** 的观点：如果一个文件描述符是 `epoll` 兴趣列表中的成员，当关闭它后会自动从列表中移除
> + **更新后** 的观点：一旦所有指向 **打开的文件描述** 的文件描述符都被关闭后，这个打开的文件描述将从 `epoll` 的兴趣列表中移除
> 
> 这表示如果我们通过 `dup()`，或者 `fork()`为打开的文件创建了描述符副本，那么这个打开的文件只会在原始的描述符以及所有其他的副本都被关闭时才会移除
> 

如果文件描述有多个文件描述符，那么上述语义可导致出现某些令人惊讶的行为。如下面的代码片段

```c

int epfd, fd1, fd2;

struct epoll_event ev, events[10];

// 创建 epoll 文件描述符
// fd1 = open()

ev.data.fd = fd1;
ev.events = EPOLLIN;

if(epoll_ctl(epfd, EPOLL_CTL_ADD, fd1, ev) == -1) {
	// 错误处理
}

// fd1 变为读就绪

fd2 = dup(fd1);
close(fd1);

int ready = epoll_wait(epfd, events, 10, -1);
if(ready == -1) {
	// 错误处理
}
```

即使文件描述符 `fd1` 已经被关闭了，这段代码中的 `epoll_wait()` 调用也会告诉我们 `fd1` 已就绪（换句话说，`evlist[0].data.fd` 的值等于 `fd1`）

> [!tip] 
> 
> 这是因为还有一个打开的文件描述符 `fd2`  存在，它所指向的文件描述信息仍包含在 `epoll` 的兴趣列表中。当两个进程持有对同一个打开文件的文件描述符副本时（一般是由于 `fork()`调用），也会出现相似的场景
> 
> 执行 `epoll_wait()` 操作的进程已经关闭了文件描述符，但另一个进程仍然持有打开的文件描述符副本
> 

## 边缘触发通知

 默认情况下 epoll 提供的是 **水平触发通知**。这表示 epoll 会告诉我们 **何时能在文件描述符上以非阻塞的方式执行 I/O 操作**。这同 `poll()` 和 `select()` 所提供的通知类型相同

> [!tip] 水平触发通知
> 
> 如果文件描述符上 **可以非阻塞地执行 I/O 系统调用**，此时认为它已经就绪。允许我们 **在 _任意时刻_ 检查文件描述符的就绪状态**
> 
> 这样，我们就可以在文件描述符就绪时，可以对其 **执行一些 IO 操作**，然后 **重复检查文件描述符**，看看是否仍然处于就绪状态
> 

epoll API 还能以 **边缘触发** 方式进行通知—也就是说，会告诉我们 **自从上一次调用 `epoll_wait()` 以来文件描述符上是否已经有 I/O 活动了**。使用 epoll 的边缘触发通知在语义上 **类似于信号驱动 I/O**，只是如果 **有多个 I/O事件发生** 的话，epoll 会将它们 **合并成一次单独的通知**，通过 `epoll_wait()` 返回，而在信号驱动I/O 中则可能会产生多个信号

> [!tip] 边缘触发通知
> 
> 如果文件描述符 **自上次状态检查以来有了新的 I/O 活动**，此时需要触发通知。**只有当 I/O 事件发生时我们才会收到通知**，并且在另一个 IO 事件到来前，我们不会收到任何新的通知
> 

由于边缘触发通知 **只有在有 IO 事件发生时才通知**，通常我们是 **不知道要处理多少 IO**(即可以读取多少字节)。

> [!summary] 边缘触发通知程序设计
> 
> 文件描述符接 **收到 IO 事件通知** 后，程序在某个时刻 **应该尽可能多的执行 IO**
> + 如果程序没有这么做，那么就失去执行 IO 的机会。**在下一个 IO 事件到来之前，程序不会再接收到通知**
> + 当确定了文件描述符就绪时，此时可能 **并不适合马上执行所有的 IO 操作**。原因在于，如果我们 **仅对一个文件描述符执行大量的 I/O 操作**，可能会 **让其他文件描述符处于饥饿状态**
> 
> **每个被检查的 _文件描述符_ 通常都应该置为 _非阻塞模式_**，在得到 I/O 事件通知后重复执行I/O 操作，直到相应的系统调用（比如 read()，write()）以错误码 `EAGAIN` 或`EWOULDBLOCK` 的形式失败
> + 如果程序采用 **循环** 来对文件描述符执行尽可能多的 I/O，而文件描述符又被置为 **可阻塞** 的，那么最终 **当没有更多的 I/O 可执行时，I/O 系统调用就会阻塞**
> 

要 **使用边缘触发通知**，我们在调用 `epoll_ctl() `时在` ev.events` 字段中 **指定 EPOLLET 标志**

```c
struct epoll_event ev;
ev.data.fd = fd;
ev.events = EPOLLIN | EPOLLET
if(epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1) {
	// 错误处理
}
```

> [!example] 
> 
> 假设我们使用 epoll 来监视一个套接字上的输入（`EPOLLIN`），接下来会发生如下的事件
> +  套接字上有输入到来
> + 我们调用一次 `epoll_wait()`
> + 再次调用 `epoll_wait()`
> 
> 在第一次调用 `epoll_wait()` 时，无论我们采用的是水平触发还是边缘触发通知，该调用都会告诉我们套接字已经处于就绪态了
> 
> 然而，在第二次调用 `epoll_wait()` 时水平触发与边缘触发的行为将不同
> + 水平触发通知：第二个 `epoll_wait()` 调用将告诉我们套接字处于就绪态。因为，目前套接字上存在可读数据
> + 边缘触发通知：第二个 `epoll_wait()` 调用将阻塞，因为自从上一次调用 `epoll_wait()` 以来并没有新的输入到来

> [!important] 水平触发通知和边缘触发通知之间的区别
> 
> 总结起来就是
> 
> + **水平触发**：每次调用 `epoll_wait()` 只要文件描述符处于就绪状态，就会通知一次
> + **边缘触发**：只在文件描述符的状态发生变化时通知（即当有新数据到来，或者有新的事件发生）
> 

**_边缘触发_ 通知通常和 _非阻塞_ 的文件描述符结合使用**。因而，采用 epoll 的边缘触发通知机制的程序基本框架如下

+ 第一步：让所有待监视的文件描述符都成为非阻塞的

+  第二步：通过 `epoll_ctl()` 构建 epoll 的兴趣列表

+  第三步：通过如下的循环处理 I/O 事件
	+ 通过 `epoll_wait()` 取得处于就绪态的描述符列表
	+ 针对每一个处于就绪态的文件描述符，不断进行 I/O 处理直到相关的系统调用，直到返回 `EAGAIN` 或 `EWOULDBLOCK` 错误

如果需要使用边缘触发，只需要将 [[#示例程序]] 中感兴趣的文件描述符修改为非阻塞模式，并在 `ev.events` 上附加上 `EPOLLET` 标志即可

> [!important] 边缘触发的使用场景
> 
> + 最初的 epoll 只支持边缘触发
> + 执行 `epoll_wait()` 和 `read()/write()` 是不同的线程
> 
> 在 eventloop 类型的程序中，处理逻辑和 `epoll_wait()` 都在一个线程，边缘触发和水平触发没有太大的差别。反而由于水平触发提醒的更频繁，可能性能更好
> 
> 在更高并发的实现中，为了对大消息的反序列化也可以并行，消息的读取和分割和 `epoll_wait()` 可能运行在不同的线程中。这时边缘触发是必需的，否则在读完数据前，`epoll_wait()` 会不停地被唤醒
> 

### 避免文件描述符饥饿

在使用 **边缘触发**（Edge Triggered, ET）模式监视多个文件描述符时，如果 **其中一个文件描述符上有大量的输入数据**（例如不间断的输入流），且我们通过 **非阻塞式** 的读操作 **尝试将所有数据读取完**，那么可能会导致以下问题

> [!question] 
> 
> **长时间占用处理时间**：由于输入数据量大，**处理该文件描述符的 I/O 操作可能会消耗较长时间**，从而导致其他文件描述符长时间无法得到处理
> 
> **饥饿风险**：在长时间集中处理某个文件描述符时，其他文件描述符就 **可能会处于饥饿状态**，无法及时被检查和执行 I/O 操作，特别是在边缘触发模式下，由于只有在状态变化时才会通知，**导致可能错过它们的就绪通知**
> 

为了避免这种情况，解决方案通常包括

> [!tip] 限时读取：对每个文件描述符 **设置一个最大读取量**
> 
> 避免因一个文件描述符的长时间 I/O 操作影响到其他文件描述符
> 

> [!tip] 调度策略：采用类似轮转调度（round-robin）等策略
> 
> **确保每个就绪文件描述符都有机会被处理**，而不是集中处理某一个
> 

> [!tip] 优化 I/O 操作：可以对输入数据进行 **分批读取**
> 
> 避免一次性读取过多数据，确保其他文件描述符也能得到及时的处理
> 

解决文件描述符饥饿问题的一种方法是 **应用程序维护一个列表**，**存放处于就绪态的文件描述符**，并通过以下步骤处理

> [!tip] 第一步：监视就绪文件描述符
> 
> 使用 `epoll_wait()` 检测文件描述符的状态，并将就绪态的描述符添加到列表中
> 
> 如果描述符已在列表中，设定较短的超时时间（如 $0$），避免等待时间过长，快速进入下一次操作
> 

> [!tip] 第二步：限制 I/O 操作频率
> 
> 对于已就绪的文件描述符，应用程序在列表中进行 **有限的 I/O 操作**，避免每次调用 `epoll_wait()` 后从头开始处理。
> 
> 采用 **轮转调度**（round-robin）方式，平衡每个描述符的处理频次
> 

> [!tip] 第三步：移除无效文件描述符
> 
> 当对某些文件描述符进行 **非阻塞 I/O** 时，如果返回 `EAGAIN` 或 `EWOULDBLOCK` 错误，表明该文件描述符 **无法继续进行 I/O 操作**，应 **从列表中移除**
> 
> 

## 在信号和文件描述符上等待

有时候，进程**既要在一组文件描述符上等待 I/O 就绪**，**也要等待待发送的信号**。我们可以尝试通过 `select()` 来执行这样的操作

非阻塞信号和 `select()` 调用的错误用法

```c
sig_atomic_t gotSig = 0;
void handler(int sig) {
	gotSig = 1;
}

int main() {
	struct sigaction sa;
	...
	sa.sa_handler = handler
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	if(sigaction(SIGUSR1, &sa, nullptr) == -1) {
		// 错误处理
	}
	
	// 信号可能在此处到来
	
	ready = select(nfds, &readfds, nullptr, nullptr, nullptr);
	if(ready > 0) {
		printf("%d file descriptors ready\n", ready);
	} else if(ready == -1 && errno == EINTR) {
		if(gotSig) {
			printf("Got signal\n");
		}
	} else {
		//其他错误
	}
	
	//...
}
```

这段代码的问题在于，**如果信号到来的时机刚好是在安装信号处理例程之后且在 `select()` 调用之前，那么 `select()` 依然会阻塞**。（这是 **竞态条件** 的一种形式。）现在我们来看看对于这个问题有什么解决方案

### pselect() 

系统调用 `pselect()` 执行的任务同 `select()` 相似。它们语义上的主要区别在于一个附加的参数— `sigmask`。该参数指定了 **当调用被阻塞时有哪些信号可以 _不被过滤掉_**

```c
#define _XOPEN_SOURCE 600
#include <sys/select.h>

int pselect(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timespec *timeout, const sigset_t *sigmask);
/* 成功返回就绪的文件描述符数量；超时返回 0；错误返回 -1 */
```

更准确地说，假设我们这样调用 `pselect()`：
```c
ready = pselect(nfds, &readfds, &writefds, &exceptfds, &timeout, &sigmask);
```

这个调用等同于以原子方式执行下列步骤

```c
sigset_t origmask;

sigprocmask(SIG_SETMASK, &sigmask, &origmask);
ready = select(nfds, &readfds, &writefds, &exceptfds, &timeout);
sigprocmask(SIG_SETMASK, &origmask, nullptr);
```

>[!tip] 
> `select()` 中的 `timeout` 参数是一个 `timespec` 结构体参加 [[时间#日历时间]]，允许将超时时间精度指定为 **纳秒级**（`select()` 为毫秒级）
>
> SUSv3 中明确说明 `pselect()` 在返回时不会修改 `timeout` 参数
>

> [!tldr] 类似的
> + 在 Linux 2.6.16 版中还新增了一个非标准的系统调用 `ppoll()`
> + 从 2.6.19 版内核开始，Linux 也新增了 `epoll_pwait()`，这是对 `epoll_wait()` 的扩展
> 

### self-pipe 技巧

由于 `pselect()` 并没有被广泛实现，为了避免在等待信号并同时调用 `select()` 时发生竞态条件，通常采用以下步骤来实现一个可移植的解决方案：

> [!tip] 第一步：创建非阻塞管道
> 
> 创建一个管道，并将管道的读端和写端都设置为非阻塞模式。这可以确保信号到达时不会阻塞进程
> 

> [!tip] 第二步：将管道的 **读端** 添加到 `select()` 的监听列表中
> 
> 在监视其他文件描述符时，也将管道的读端包含在 `readfds` 中传递给 `select()`。
> 

> [!tip] 第三步：为感兴趣的信号安装信号处理函数
> 
> 在信号处理函数中，当信号到达时，向管道的 **写端写入一个字节**。这样可以通知主循环有信号到来
> 
> 注意事项
> + 管道的**写端被设置为非阻塞**，**防止** 因为信号到来得太快，重复的信号导致 **写操作阻塞**，进而阻塞整个进程。
> + **信号处理函数应该在管道创建之后安装**，避免出现由于管道未创建就发送信号导致的竞态条件
> + 在信号处理函数中使用 `write()` 是安全的，因为 `write()` 是异步信号安全的
> 

> [!tip] 第四步：在主循环中调用 `select()`
> 
> 在主循环中调用 `select()` 来监视文件描述符。当信号处理函数通过管道通知信号到来时，`select()` 会被重新调用（可以通过监视管道的读端来检查信号是否到来）
> 
> 严格来说在这种方式下重新调用 `select()` 并不是必须的。这只是表示我们可以通过监视 `readfds` 来检查是否有信号到来，而不是通过检查返回的 `EINTR` 错误码。
> 

> [!tip] 第五步：检查管道读端是否在 `readfds` 中
> 
> 如果 `select()` 调用成功后，检查管道的读端是否出现在 `readfds` 中。如果在，则说明信号到来
> 

> [!tip] 第六步：处理信号
> 
> 当信号到来时，通过读取管道中的数据来确认信号的到来。由于管道读端是非阻塞的，使用循环读取直到返回 `EAGAIN` 错误码，表明管道中已无数据。然后进行必要的操作来响应信号。


> [!summary] 
> + **非阻塞管道**：确保信号到达时不会因管道空间满而导致阻塞。
> + **信号处理函数**：通过写入管道的字节来通知主程序有信号到来。
> + **使用 `select()` 监视管道**：可以有效避免 `select()` 被信号中断而导致的竞态条件。
> + **避免竞态条件**：确保信号处理函数和管道创建的时序正确，避免信号在管道创建之前到达。

这种方式保证了当信号发生时，程序可以及时处理，同时避免了信号处理和 `select()` 之间的竞态条件。
