# IO 多路复用


目前为止，我们的程序中使用的 IO 模型都是单个进程 **每次 _只在一个文件描述符_ 上执行 IO 操作**，**每次 IO 系统调用都会 _阻塞_ 直到完成数据传输**。比如，在进程间通信中使用 **FIFO**，当从一个 FIFO 中读取数据时，如果 FIFO 中恰好没有数据，那么通常 `read()` 会阻塞。而如果 FIFO 中没有足够的空间保存待写入的数据时，`write()` 也会阻塞。当在其他类型的文件如 **[[PIPE]]** 和 **[[SOCKET 概览]]** 上执行 I/O 操作时，也会出现相似的行为

> [!tip] 
> 当在某个文件描述符上读取数据时，如果无法该文件描述符上读取数据，那么 `read()` 系统调用就会阻塞
> 
> 当在某个文件描述符上写入数据时，如果文件描述关联文件没有空间写入时，`write()` 系统调用也会阻塞

> [!attention] 磁盘文件是个特例
> 内核采用缓冲区 `cache` 来加速磁盘 I/O 请求。因而一旦请求的数据传输到内核的缓冲区 `cache`，对磁盘的 `write()` 操作将立刻返回，而不用等到将数据实际写入磁盘后才返回（除非在打开文件时指定了 `O_SYNC` 标志 ^[[[文件 IO 缓冲]]]）
> 
> 与之对应的是，`read()` 调用将数据从内核缓冲区 `cache` 移动到用户的缓冲区中，如果请求的数据不在内核缓冲区 `cache`，那么内核就会让进程休眠，同时执行对磁盘的读操作
> 

对于许多应用来说，传统的 **阻塞 IO 模型** 已经足够了，但这不代表所有的应用都能得到满足。特别的，有些应用需要处理以下某项任务，或者两者都需要兼顾

> [!tip] 
> 
> 如果可能的话，**以 _非阻塞_ 的方式检查文件描述符上是否可进行 I/O 操作**
> 
> **同时检查多个文件描述符**，看它们中的任何一个是否可以执行 I/O 操作
> 

目前，我们可以使用 **非阻塞 IO** 技术检查文件描述符上是否可以执行 IO 操作。我们马上将介绍 **IO 多路复用**，用于同时检查多个文件描述符。后续，我们还会介绍 **多进程** 或 **多线程** 技术

## IO 模型

 [[文件 IO：通用 IO 模型]] 中我们介绍了 **同步阻塞 IO** 模型；[[深入文件 IO#非阻塞 IO]] 和 [[FIFO#非阻塞 IO]] 我们介绍了 **同步非阻塞 IO** 模型。下面我们加强这两个概念，并介绍另外一种 IO 模型

> [!tip] IO 操作的两个阶段：**数据准备** 和 **获取数据**
> 
> **数据准备阶段**：将数据从设备 _搬运_ 到内核空间中的缓冲区
> 
> **进程获取数据阶段**：从内核缓冲区 _复制_ 到用户空间的缓冲区
> 

### 同步 IO

> [!tip] 同步：所谓的同步就是 **等待结果的返回**
> 
> 通俗来说，**不达目的誓不罢休**，如果是函数调用，函数必须返回的最终结果。等待函数返回这段时间称为 **阻塞**
> 

#### 阻塞 IO 

**阻塞 IO** 是指在执行 IO 操作时，**调用线程会被阻塞**，**等待 _IO 操作完成后_ 才能继续执行后续代码**。此时，**线程处于等待状态**，无法执行其他任务

> [!tip]
> 当一个阻塞 IO 操作（如读取文件或从网络接收数据）被发起时，调用线程会暂停，直到数据读取或写入操作完成。操作完成后，线程才会继续执行后续代码
> 

![[Pasted image 20241030205651.png]]


> [!tip] 阻塞 IO 特点
> 
> 进程等待（阻塞），直到读写完成。（全程等待)
> 

为了避免进程/线程被 IO 操作阻塞，可以创建一个新的进程/线程来执行 IO 操作。父进程/线程就可以处理其他任务，而子进程/线程阻塞等待 IO 操作完成

> [!attention] 进程和线程处理阻塞 IO 的局限性
> 
> 使用进程会 **带来额外的开销**，创建进程和维护进程对系统来说都有开销。而且子进程需要通过 IPC 对象来通知父进程 IO 操作的状态，使得 **编程变得复杂**
> 
> 使用线程可以减少占用的资源。但是线程也需要通信，使得 **编程工作变得复杂**
> 

#### 非阻塞 IO

**非阻塞 I/O** 可以让我们 **周期性** 地检查（“轮询”）某个文件描述符上是否可执行 I/O 操作

> [!tip]
> 比如，我们可以让一个输入文件描述符成为非阻塞式的，然后周期性地执行非阻塞式的读操作

如果我们需要 **同时检查多个文件描述符**，那么就需要将它们 **都设为非阻塞**，然后依次对它们 **轮询**

> [!tip]
> 在一个紧凑的循环中 **做轮询就是在浪费 CPU**

下图描述了 **非阻塞 IO** 的轮询过程

![[Pasted image 20241030205845.png]]

> [!attention] 非阻塞 IO 的局限性
> 
> 使用非阻塞 IO 可以 **轮询** 检查多个文件描述是否可以执行 IO 操作。如果 **轮询的频率较低**，应用程序 **响应 IO 时间可能会延迟**。如果 **轮向频率较高**，可能 **带来 CPU 的浪费**
> 

### 备选 IO 模型

由于非阻塞 IO 和多进程/多线程的局限性，其他的备选的 IO 模型往往更可取

> [!tip] 
> 
> + **IO 多路复用**：允许我们 **同时检查多个文件描述符**，看其中任意一个是否可执行 I/O 操作。有两个功能几乎相同的 **系统调用** 来执行 I/O 多路复用操作：`select()` 和 `poll()`
>  + **epoll** API 是 Linux 专有的特性。与 IO 多路复用一样，可以同时检查多个文件描述。当同时检查的文件描述符非常多时，epoll 可以提供更好的性能
> + **信号驱动 IO**：当有输入或者数据可以写到指定的文件描述符上时，内核向请求数据的进程发送一个信号。进程可以处理其他的任务，当 IO 操作可执行时通过接收信号来获得通知。当同时检查大量的文件描述符时，信号驱动 IO 相比 `select()` 和 `poll()` 有显著的性能提升
> + **异步 IO**：
>
> 下面我们将马上介绍 IO 多路复用。其他 IO 模型，会陆续被介绍 [[信号驱动 IO]] [[Linux epoll]]
> 

这些备选都是用来实现同一个目标技术：**同时检查多个文件描述符**，看看它们是否准备好了执行 IO 操作。准确说，是看 IO 系统调用是否可以非阻塞地执行

> [!tip] 
> 
> 文件描述符就绪状态的转化是通过一些 I/O 事件来触发的
> + 输入数据以到达
> + 套接字连接建立完成
> + 之前满载的套接字发送缓冲区中的数据传送到了对端
> 

同时检查多个文件描述符在网络服务程序中非常有用，或者那些必须同时检查终端以及管道或套接字输入的应用程序

> [!attention] 
> 
> 这些技术都 **不会执行实际的 I/O 操作**。它们只是 **告诉我们某个文件描述符已经处于就绪状态了**。这时 **需要调用其他的系统调用来完成实际的 I/O 操作**。
> 

### 水平触发和边缘触发

在学习 IO 多路复用之前，先了解两种 文件描述符准备就绪的 **通知模式**：**水平触发** 和 **边缘触发**

> [!tip] 水平触发：如果文件描述符上 **可以非阻塞地执行 I/O 系统调用**，此时认为它已经就绪
> 
> 下图展示文件描述符处于 **读就绪状态** 的缓冲区中的字节数
> 
> ![[Pasted image 20241205222456.png]]
> 
> + 只要缓冲区中的字节数 **在虚线上方**，就会通知 **文件描述符就绪**
> 
> 允许我们 **在 _任意时刻_ 检查文件描述符的就绪状态**。这样，我们就可以在文件描述符就绪时，可以对其 **执行一些 IO 操作**，然后 **重复检查文件描述符**，看看是否仍然处于就绪状态
> 
> 换句话说，由于水平触发模式允许我们在任意时刻重复检查 I/O 状态，**没有必要每次当文件描述符就绪后需要尽可能多地执行 I/O**（也就是尽可能多地读取字节，亦或是根本不去执行任何 I/O）
> 

> [!tip] 边缘触发：如果文件描述符自上次状态检查以来 **有了新的 I/O 活动**，此时需要触发通知
> 
> 下图展示文件描述符处于 **读就绪状态** 的缓冲区中的字节数
> 
> ![[Pasted image 20241205223355.png]]
> 
> + 只有当缓冲区中的 **字节数增加**，就会通知 **文件描述符就绪**
> 
> **只有当 I/O 事件发生时我们才会收到通知**，并且在另一个 IO 事件到来前，我们不会收到任何新的通知
> 

> [!attention] 边缘触发通知的程序的设计规范
> 
>  然而，当文件描述符 **收到 IO 事件通知** 时，通常我们是 **不知道要处理多少 IO**(即可以读取多少字节)。因此，采用边缘触发通知的程序应该按照如下设计
> 
> 文件描述符接 **收到 IO 事件通知** 后，程序在某个时刻 **应该尽可能多的执行 IO**。如果程序没有这么做，那么就失去执行 IO 的机会。因为在产生另一个 I/O 事件到来前，程序都不会再接收到通知了，因此也就不知道此时应该执行 I/O 操作。从而导致数据丢失或程序出现阻塞
>  + 当确定了文件描述符就绪时，此时可能并不适合马上执行所有的 IO 操作。原因在于，如果我们仅对一个文件描述符执行大量的 I/O 操作，可能会让其他文件描述符处于饥饿状态
> 
> **每个被检查的 _文件描述符_ 通常都应该置为 _非阻塞模式_**，在得到 I/O 事件通知后重复执行I/O 操作，直到相应的系统调用（比如 read()，write()）以错误码 `EAGAIN` 或`EWOULDBLOCK` 的形式失败
> + 如果程序采用循环来对文件描述符执行尽可能多的 I/O，而文件描述符又被置为可阻塞的，那么最终当没有更多的 I/O 可执行时，I/O 系统调用就会阻塞
> 

## IO 多路复用

**IO 多路复用** 允许我们同时检查多个文件描述符，看其中任意一个是否可执行 I/O 操作。有两个功能几乎相同的 **系统调用** 来执行 I/O 多路复用操作：`select()` 和 `poll()`

> [!tip]
> 
> 同时监控多个IO，称为多路IO，哪怕只有一路准备好了，就不需要等了就可以开始处理这一路的数据。这种方式提高了同时处理 IO 的能力
> 

> [!tip] 
> `select()` 和 `poll()` 可以检查普通文件、[[PIPE]] 、[[FIFO]]、[[SOCKET 概览]] 等其他类型文件的文件描述符

这两个系统调用都允许进程 **要么一直等待文件描述符成为就绪态**，要么在调用中 **指定一个超时时间**。

![[Pasted image 20241030230515.png]]

> [!tip] 
> 
> 通过 `select/poll` 系统调用将需要监视的文件描述符提交给 **内核**；内核不断 **轮询检测** 这些文件描述符是否就绪；此时用户进程等待 `select/poll` 的返回，内核发现至少有一个文件描述符就绪时，`select/poll` 就立即返回
> 

### select

系统调用 `select()` 会一直阻塞，直到一个或多个文件描述符集合成为就绪态

```c
#include <sys/time.h>
#include <sys/select.h>

int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *execptfds, struct timeval *timeout);
/* 成功返回就绪文件描述符的数量；超时返回 0；错误返回 -1*/
```

> [!tip] 参数 `readfds` `writefds` `execptfds` ：指向文件描述符集合的指针
> 
> 所指向的数据类型是 `fd_set`
> 

下表列出了参数 `readfds` `writefds` `execptfds` 的含义

| 参数          | 描述                 |
| :---------- | ------------------ |
| `readfds`   | 检测输入是否就绪的文件描述符集合   |
| `writefds`  | 检测输出是否就绪的文件描述符集合   |
| `execptfds` | 检测异常情况是否发生的文件描述符集合 |

> [!tip] 
> 
> **异常情况** 不是文件描述符上出现了一些错误。在 Linux 上，异常情况只会出在下面两种情形下发生
> + 连接到处于信包模式下的伪终端主设备上的从设备状态发生了改变，参考 [[伪终端]]
> + 流式套接字上接收到了带外数据，参考 [[SOCKET：高级主题]]
> 

**数据类型 `fd_set` 以位掩码的形式来实现**，不需要知道其中的细节，所有关于文件描述符集合的操作都是通过下表中的四个宏函数来完成的

| 宏                                 | 描述                           |
| :-------------------------------- | ---------------------------- |
| `FD_ZERO(fd_set *fdset)`          | 将集合 `fdset` 初始化为空            |
| `FD_SET(int fd, fd_set *fdset)`   | 将文件描述符 `fd` 添加到集合 `fdset`    |
| `FD_CLR(int fd, fd_set *fdset)`   | 将文件描述符 `fd` 从集合 `fdset` 中移除  |
| `FD_ISSET(int fd, fd_set *fdset)` | 判断文件描述符 `fd` 是否在集合 `fdset` 中 |

> [!tip] 
> 
> **文件描述符集合有一个最大容量限制**，由常量 `FD_SETSIZE` 来决定。在 **Linux 上，该常量的值为 $1024$**
> 

参数 `readfds` `writefds` 和 `exceptfds` 所指向的结构体都是保存结果值的地方。在调用 `select()` 之前，这些参数指向的结构体必须初始化(通过 `FD_ZERO()` 和 `FD_SET()`)，以包含我们感兴趣的文件描述符集合

> [!important] 
> 
> `select()` 调用会修改这些结构体。当 `select()` 返回时，它们包含的就是已处于就绪态的文件描述符集合了
> 
> 由于这些结构体会在调用中被修改，**如果要在循环中重复调用 `select()`，我们必须保证每次都要重新初始化它们**
> 

如果我们对某一类型的事件不感兴趣，那么相应的 `fd_set` 参数可以指定为 `NULL`

> [!tip] 参数 `nfds`：必须设为比 $3$ 个文件描述符集合中所包含的 **最大文件描述符号** 还要大 $1$
> 

> [!tip] 参数 `timeout`：控制 `select()` 的阻塞行为
> 
> 该参数可指定为 `NULL`，此时 `select()` 会一直阻塞
> 
> 如果非空，`select()` 返回时会更新 `timeout` 所指向的结构体以此来表示 **剩余的超时时间**。这种行为与具体实现有关，SUSv3 标准不允许系统修改 `timeout` 指向的结构体。**在循环中使用了 `select()` 并且指定 `timeout` 参数，应该在每次调用 `select()` 之前重新初始化 `timeout`**
>  

> [!tip] 返回值
> 
> 返回 $-1$ 表示有错误发生。`errno` 被设置为 `EBADF` 和 `EINTR`
> + `EBADF` 表示  `readfds` `writefds` 和 `exceptfds`  中有一个文件描述符非法(例如，文件描述符没有被打卡)
> + `EINTR` 表示该调用被信号处理程序中断
> 
> 返回 $0$ 在任何文件描述符就绪之前 `select()` 调用已超时。此时，文件描述符集合被清空
> 
> 返回正数表示有文件描述符处于就绪状态，该值表示 **就绪文件描述符的个数**
> 

下面将 [[FIFO#使用 FIFO 实现全双工通信]] 中的代码改造为使用 `select()` IO 多路复用的代码

```c title:altio/programA_select.c
#include <sys/stat.h>
#include <sys/select.h>
#include <fcntl.h>
#include <unistd.h>
#include "base.h"

int main(int argc, char *argv[]) {
    // programA_select A2B_FIFO B2A_FIFO
    if(argc != 3) {
        usageErr("%s <fifo 1> <fifo 2>", argv[0]);
    }

    // B2A_FIFO 以 O_RDONLY 打开
    int b2a = open(argv[2], O_RDONLY);
    if(b2a == -1) {
        errExit("open %s error: ", argv[2]);
    }
    printf("%s read side opened\n", argv[2]);

    // A2B_FIFO 以 O_WRONLY 打开
    int a2b = open(argv[1], O_WRONLY);
    if(a2b == -1) {
        errExit("open %s error: ", argv[1]);
    }
    printf("%s write side opened\n", argv[1]);

    fd_set readfds;

    while(true) {
        FD_ZERO(&readfds); // 情况 readfds
        FD_SET(b2a, &readfds);  // 从 B2A_FIFO 中读
        FD_SET(STDIN_FILENO, &readfds); // 从 STDIN_FILENO 中读

        int ret = select(b2a + 1, &readfds, nullptr, nullptr, nullptr);
        if(ret == -1) {
            errExit("select error");
        }

        // 到此，readfds 现在为就绪集合
        char buffer[BUFSIZ] = {};
        ssize_t bytes;
        // STDIN_FILENO 就绪
        if(FD_ISSET(STDIN_FILENO, &readfds)) {
            bytes = read(STDIN_FILENO, buffer, BUFSIZ);
            if(bytes == -1 || bytes == 0) {
                errExit("read error");
            }
            if(write(a2b, "programA: ", 10) < 0) {
                errExit("write error");
            }
            if(write(a2b, buffer, bytes) < 0) {
                errExit("write error");
            }
        }

        // B2A_FIFO 就绪
        if(FD_ISSET(b2a, &readfds)) {
            bytes = read(b2a, buffer, BUFSIZ);
            if(bytes == -1 || bytes == 0) {
                errExit("read error");
            }
            if(write(STDOUT_FILENO, buffer, bytes) < 0) {
                errExit("write error");
            }
        }
    }
}
```

```c title:altio/programB_select.c
#include <sys/stat.h>
#include <sys/select.h>
#include <fcntl.h>
#include <unistd.h>

#include "base.h"

int main(int argc, char *argv[]) {
    // programB_select A2B_FIFO B2A_FIFO
    if(argc != 3) {
        usageErr("%s <fifo 1> <fifo 2>", argv[0]);
    }

    // B2A_FIFO 以 O_WRONLY 打开
    int b2a = open(argv[2], O_WRONLY);
    if(b2a == -1) {
        errExit("open %s error: ", argv[2]);
    }
    printf("%s write side opened\n", argv[2]);

    // A2B_FIFO 以 O_RDONLY 打开
    int a2b = open(argv[1], O_RDONLY);
    if(a2b == -1) {
        errExit("open %s error: ", argv[1]);
    }
    printf("%s read side opened\n", argv[1]);

    fd_set readfds;

    while(true) {
        FD_ZERO(&readfds);
        FD_SET(a2b, &readfds);// 在 A2B_FIFO 中读
        FD_SET(STDIN_FILENO, &readfds); // 在 STDIN_FILENO 中读

        int ret = select(a2b + 1, &readfds, nullptr, nullptr, nullptr);
        if(ret == -1) {
            errExit("select error");
        }
        char buffer[BUFSIZ] = {};
        ssize_t bytes;
        // STDIN_FILENO 就绪
        if(FD_ISSET(STDIN_FILENO, &readfds)) {
            bytes = read(STDIN_FILENO, buffer, BUFSIZ);
            if(bytes == -1 || bytes == 0) {
                errExit("read error");
            }
            if(write(b2a, "programB: ", 10) < 0) {
                errExit("write error");
            }
            if(write(b2a, buffer, bytes) < 0) {
                errExit("write error");
            }
        }
        // A2B_FIFO 就绪
        if(FD_ISSET(a2b, &readfds)) {
            bytes = read(a2b, buffer, BUFSIZ);
            if(bytes == -1 || bytes == 0) {
                errExit("read error");
            }
            if(write(STDOUT_FILENO, buffer, bytes) < 0) {
                errExit("write error");
            }
        }
    }
}
```

#### select 的底层原理

`select()` 使用了一个核心数据结构 `fd_set`，定义如下

```c
typedef long int __fd_mask;

typedef struct  
{  
	__fd_mask fds_bits[1024 / (8 * (int) sizeof (__fd_mask))];  
} fd_set;
```

> [!tip] 在 $64$ 位系统中
> 
> `__fd_mask` 是 `long int` 的别名。`long int` 占据 $8$ 字节($64$ 位)，所以 `1024 / (8 * (int)sizeof(__fd_mask)) = 1024 / (8 * 8) = 16` 
> 
> 综上，`__fd_mask fds_bits[...]` 等价于 `__fd_mask fds_bits[16]`，总计占 $1024$ 位
> 

> [!tip] 在 $32$ 位系统中
> `__fd_mask` 是 `long int` 的别名。`long int` 占据 $4$ 字节($32$ 位)，所以 `1024 / (8 * (int)sizeof(__fd_mask)) = 1024 / (4 * 8) = 32` 
> 
> 综上，`__fd_mask fds_bits[...]` 等价于 `__fd_mask fds_bits[32]` ，总计占 $1024$ 位
> 

> [!important] 
> 
> 由于系统资源限制，默认情形下，一个进程最多打开 $1024$ 个文件描述
> 

`select()` 使用 `fd_set` 的第 $i$ 位表示 `fd i` 是否在 `fd_set` 中。如下图，展示 `fd_set` 的概念

![[Pasted image 20241101125046.png]]

根据 **位图** 中的位 **是否置位** 判断文件描述符 $i$ 是否在 `fd_set` 中


`select()` 执行过程如下
+ `fd_set` 拷贝到内核中，并清空
+ 内核开始轮询：从 `0` 到 `nfds` 遍历每个文件描述符，检查每个文件描述符是否就绪
	+ 如果已就绪，就添加到 `fd_set` 中
	+ 如果为就绪，继续遍历
+ 如果 `fd_set` 中有就绪的文件描述符，`select()` 返回，否则继续遍历


#### select 的缺陷

`fd_set` 这一个位图使用数组实现，大小是固定的。如果需要修改它的长度，需要重新编译

涉及到达将 `fd_set` 的拷贝，如果监控的文件描述符太多，拷贝将浪费大量的时间

监控文件描述符集合和就绪文件描述符的集合 **耦合** 在一起。在一个循环中，每次监听的文件描述符不同，会带来编程上的复杂性

内核使用 **轮询** 方式获取就绪文件描述符集合。如果 **监控的文件描述符很多**，但是 **就绪的文件描述符很少** 时，发现这些就绪的文件描述符将浪费 CPU 

> [!tip] 
> 
> select 的 **轮询** 机制，导致 **获取就绪文件描述符集合的性能较低**
> 

> [!attention] 更严重的问题
> 
> 每当 `select()` 返回，我们并不知道哪个文件描述符就绪了，还需要挨个循环
> 

### poll

系统调用 `poll()` 执行的任务同 `select()` 很相似。两者间主要的区别在于我们要如何指定待检查的文件描述符

在 `poll()` 中我们提供一列文件描述符，并在每个文件描述符上标明我们感兴趣的事件

```c
#include <poll.h>

int poll(struct pollfd fds[], nfds_t nfds, int timeout);
/* 返回就绪的文件描述符个数; 超时返回 0; 错误返回 -1*/
```

参数 `nfds` 是 `fds` 列表的长度。参数 `fds` 列出了我们需要 `poll()` 检查的文件描述符。该参数是 `pollfd` 结构体数组，定义如下

```c
struct pollfd {
	int fd;  // 文件描述符
	short events; // 监控文件描述符的事件，一个位掩码
	short revents; // 文件描述符就绪的事件，一个位掩码
};
```

> [!tip] 
> 
> `events` 字段：指定在文件描述符 `fd` 上监控的事件
> 
> `revents` 字段：指定文件描述符 `fd` 上就绪的事件
> 

下表列出来 `events` 和 `revents` 字段中出现的掩码值(符号常量)

| 位掩码          | `events` 中的输入 | 返回到 `revents` | 描述                  |
| :----------- | :------------ | ------------- | ------------------- |
| `POLLIN`     | 是             | 是             | 可读取非高优先级的数据         |
| `POLLRDNORM` | 是             | 是             | 等价于 `POLLIN`        |
| `POLLRDBAND` | 是             | 是             | 读取优先数据(Linux 中无法使用) |
| `POLLPRI`    | 是             | 是             | 读取高优先级数据            |
| `POLLRDHUP`  | 是             | 是             | 对端套接字关闭             |
|              |               |               |                     |
| `POLLOUT`    | 是             | 是             | 普通数据可写              |
| `POLLWRNORM` | 是             | 是             | 等同于 `POLLOUT`       |
| `POLLWRBAND` | 是             | 是             | 优先级数据可写入            |
|              |               |               |                     |
| `POLLERR`    |               | 是             | 有错误发生               |
| `POLLHUB`    |               | 是             | 出现挂断                |
| `POLLNVAL`   |               | 是             | 文件描述符未打卡            |
|              |               |               |                     |
| `POLLMSG`    |               |               | Linux 中不使用          |

`poll()` 真正关心的标志位就是 `POLLIN` `POLLOUT` `POLLPRI` `POLLRDHUP` `POLLHUP` 以及 `POLLERR`

> [!attention] 
> 
> 在提供 STREAMS 设备的 UNIX 实现中，`POLLMSG` 表示包含有 `SIGPOLL` 信号的消息已到达 stream 头部
> 
> Linux 没有实现 STREAMS
> 

> [!tip] 关闭对某个文件描述符的检查
> 
> 如果我们对某个文件描述符上的事件不感兴趣，可以将 `events` 设为 $0$
> 
> 给 `fd` 字段指定一个负数将导致对应的 `events` 字段被忽略，且 `revents` 字段总是返回 $0$
> 
> 这两种方式都不需要从新建立整个 `fds` 列表
> 

> [!tip] 参数 `timeout`：决定了 `poll()` 的阻塞行为
> 
> + `timeout == -1`，`poll()` 阻塞直到 `fds` 中的文件描述就绪或者捕获到一个信号
> + `timeout == 0`，`poll()` 不会阻塞，只执行一次检查看看哪个文件描述符处于就绪状态
> + `timeout > 0`，`poll()` 阻塞 `timeout` 毫秒，直到 `fds` 中的文件描述符中有一个就绪，或者捕获一个信号为止
> 

`poll()` 的返回值与 `select()` 类似

+ 返回 $-1$ 表示错误发生
+ 返回 $0$ 表示任意文件描述符就绪前超时
+ 返回正整数表示至少有 $1$ 个文件描述符就绪，该返回表示就绪文件描述符的个数

> [!attention] 
> 
> `poll()` 返回的正数是 `fds` 列表中就绪的文件描述符个数，注意任何文件描述符只会统计一次
> 

```c title:altio/programA_poll.c
#include <sys/stat.h>  
#include <poll.h>  
#include <fcntl.h>  
#include <unistd.h>  
#include "base.h"  

int main(int argc, char *argv[]) {  
    // programA_poll A2B_FIFO B2A_FIFO  
    if(argc != 3) {  
        usageErr("%s <fifo 1> <fifo 2>", argv[0]);  
    }  
  
    // B2A_FIFO 以 O_RDONLY 打开  
    int b2a = open(argv[2], O_RDONLY);  
    if(b2a == -1) {  
        errExit("open %s error: ", argv[2]);  
    }  
    printf("%s read side opened\n", argv[2]);  
  
    // A2B_FIFO 以 O_WRONLY 打开  
    int a2b = open(argv[1], O_WRONLY);  
    if(a2b == -1) {  
        errExit("open %s error: ", argv[1]);  
    }  
    printf("%s write side opened\n", argv[1]);  
  
    while(true) {  
        struct pollfd pfds[2] = {{.fd=STDIN_FILENO, .events = POLLIN, .revents = 0},{.fd = b2a, .events = POLLIN | POLLHUP, .revents = 0}};  
  
        int ret = poll(pfds, 2, -1);  
        if(ret == -1) {  
            errExit("select error");  
        }  
  
        char buffer[BUFSIZ] = {};  
        ssize_t bytes;  
        for (int i = 0; i < 2; i++) {  
            // 可读  
            if (pfds[i].revents & POLLIN) {  
                if (pfds[i].fd == STDIN_FILENO) {  
                    bytes = read(pfds[i].fd, buffer, BUFSIZ);  
                    if (bytes == -1 || bytes == 0) {  
                        errExit("read error: ");  
                    }  
                    write(a2b, buffer, bytes);  
                }  
                if (pfds[i].fd == b2a) {  
                    bytes = read(pfds[i].fd, buffer, BUFSIZ);  
                    if (bytes == -1) {  
                        errExit("read error: ");  
                    }  
                    write(STDOUT_FILENO, buffer, bytes);  
                }  
            }
            // 关闭
            if (pfds[i].revents & POLLHUP) {  
                errExit("poll hup error: ");  
            }  
        }  
    }  
}
```

```c title:altio/programB_poll.c
#include <sys/stat.h>
#include <sys/poll.h>
#include <fcntl.h>
#include <unistd.h>

#include "base.h"

int main(int argc, char *argv[]) {
    // programB_select A2B_FIFO B2A_FIFO
    if(argc != 3) {
        usageErr("%s <fifo 1> <fifo 2>", argv[0]);
    }

    // B2A_FIFO 以 O_WRONLY 打开
    int b2a = open(argv[2], O_WRONLY);
    if(b2a == -1) {
        errExit("open %s error: ", argv[2]);
    }
    printf("%s write side opened\n", argv[2]);

    // A2B_FIFO 以 O_RDONLY 打开
    int a2b = open(argv[1], O_RDONLY);
    if(a2b == -1) {
        errExit("open %s error: ", argv[1]);
    }
    printf("%s read side opened\n", argv[1]);


    while(true) {
        struct pollfd pfds[2] = {{.fd=STDIN_FILENO, .events = POLLIN, .revents = 0},{.fd = a2b, .events = POLLIN | POLLHUP, .revents = 0}};

        int ret = poll(pfds, 2, -1);
        if(ret == -1) {
            errExit("select error");
        }

        char buffer[BUFSIZ] = {};
        ssize_t bytes;
        for (int i = 0; i < 2; i++) {
            // 可读
            if (pfds[i].revents & POLLIN) {
                if (pfds[i].fd == STDIN_FILENO) {
                    bytes = read(pfds[i].fd, buffer, BUFSIZ);
                    if (bytes == -1 || bytes == 0) {
                        errExit("read error");
                    }
                    write(b2a, buffer, bytes);
                }
                if (pfds[i].fd == a2b) {
                    bytes = read(pfds[i].fd, buffer, BUFSIZ);
                    if (bytes == -1 || bytes == 0) {
                        errExit("read error");
                    }
                    write(STDOUT_FILENO, buffer, bytes);
                }
            }
            if (pfds[i].revents & POLLHUP) {
                errExit("poll hup error: ");
            }
        }
    }
}
```

## 文件描述符何时就绪


> [!tip] 正确使用 `select()` 和 `poll()` 需要理解在什么情况下文件描述符会表示为就绪态
> 
> SUSv3 中说，如果 **对 I/O 函数的调用不会阻塞**，而不论该函数是否能够实际传输数据，此时文件描述符（未指定 `O_NONBLOCK` 标志）**被认为是就绪的**
> 

**`select()` 和 `poll()` 只会告诉我们 IO 操作是否会阻塞**，而不是告诉我们到底能否成功传输数据

下面我们考虑 `select()` 和 `poll()` 在不同类型的文件描述符上所做的操作

### 管道和 FIFO

下表总结了`select()` 和 `poll()` 在管道或 FIFO 读端上的通知。

![[Pasted image 20241128232422.png]]

下表总结了 `select()` 和 `poll()` 在管道或 FIFO 写端上的通知。

![[Pasted image 20241128232531.png]]

### 套接字

![[Pasted image 20241128232548.png]]
