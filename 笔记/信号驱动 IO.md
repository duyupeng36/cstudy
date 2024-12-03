# 信号驱动 IO

在 I/O 多路复用中，进程是通过系统调用（`select()`或 `poll()`）来检查文件描述符上是否可以执行 I/O 操作

而在 **信号驱动 I/O** 中，当文件描述符上 **可执行 I/O 操** 作时，**进程请求内核为自己发送一个信号**。之后进程就可以执行任何其他的任务直到 I/O 就绪为止，此时内核会发送信号给进程

> [!tip] 信号驱动 IO：当文件描述符上可执行 IO 操作时，内核会向进程发送一个信号
> 
>在信号到来之前，进程可以执行其他的操作。而当信号到来后，进程就转而去执行 IO 操作
> 

要使用信号驱动 I/O，程序需要按照如下步骤来执行。

> [!tip] 第一步：为内核发送的通知信号安装一个信号处理例程。默认情况下，这个通知信号为 `SIGIO`
> 
> 由于接收到 `SIGIO` 信号的默认行为是终止进程运行，因此我们应该在启动信号驱动 I/O 前先为 `SIGIO` 信号安装处理例程
> 
> 如果我们在安装 `SIGIO` 信号处理例程之前先启动了信号驱动 I/O，那么会存在一个时间间隙，此时如果 I/O 就绪的话内核发送过来的 `SIGIO` 信号就会使进程终止运行
> 

> [!tip] 第二步：设定文件描述符的 **属主**，也就是当文件描述符上可执行 I/O 时会接收到通知信号的 **进程或进程组**
> 
> 通常我们 **让调用进程成为属主**。设定属主可通过 `fcntl()` 的 `F_SETOWN` 操作来完成
> 
> ```c
> fcntl(fd, F_SETOWN, pid);
> ```
> + 如果参数 `pid` 为正整数，就解释为进程 ID 号
> + 如果参数 `pid` 是负数，它的绝对值就指定了进程组 ID 号
> 
> 通常会在 `pid` 中 **指定调用进程的进程 ID 号**（这样信号就会发送给打开这个文件描述符的进程）。但是，也可以将其指定为另一个进程或进程组（例如，调用者进程组），而信号会发送给这个目标
> 
> 当指定的文件描述符上可执行 I/O 时，`fcntl()` 的 `F_GETOWN` 操作会返回接收到信号的进程或进程组 ID 号
> 

> [!tip] 第三步：通过设定 `O_NONBLOCK` 标志使能非阻塞 I/O
> 
> 信号驱动 IO 提供了边缘触发通知
> 

> [!tip] 第四步：通过打开 `O_ASYNC` 标志使能信号驱动 I/O
> 
> 这可以和上一步合并为一个操作，因为它们都需要用到 `fcntl()` 的 `F_SETFL` 操作
> 
> ```c
> flags = fcntl(fd, F_GETFL);
> fcntl(fd, F_SETFL, flags|O_ASYNC|O_NONBLOCK);
> ```
> 

经过上述四步之后，**调用进程现在可以执行其他的任务了**。当 I/O 操作就绪时，内核为进程发送一个信号，然后调用在第 $1$ 步中安装好的信号处理例程

**信号驱动 I/O 提供的是 _边缘触发_ 通知**。这表示 **一旦进程被通知 I/O 就绪**，它就**应该尽可能多地执行 I/O**

> [!attention] 
> 
> 假设文件描述符是非阻塞式的，这表示需要在循环中执行 I/O 系统调用直到失败为止，此时错误码为 `EAGAIN` 或 `EWOULDBLOCK`
> 

## 何时发送 IO 就绪信号

### 管道和 FIFO

对于管道或 FIFO 的 **读端**，信号会在下列情况中产生

> [!tip] 情形一：数据写入到管道中
> 
> 即使已经有未读取的输入存在
> 

> [!tip] 情形二：管道的写端关闭

对于管道或 FIFO 的 **写端**，信号会在下列情况中产生

> [!tip] 情形一：对管道的 **读操作增加了管道中的空余空间大小**，因此现在可以写入 `PIPE_BUF` 个字节而不被阻塞

> [!tip] 情形二：管道的读端关闭
> 

### 套接字

信号驱动 I/O 可适用于 UNIX 和 Internet 域下的 **数据报套接字**。信号会在下列情况中产生

> [!tip] 情形一：一个输入数据报到达套接字
> 
> 即使已经有未读取的数据报正等待读取
> 

> [!tip] 情形二：套接字上发生了异步错误

信号驱动 I/O 可适用于 UNIX 和 Internet 域下的 **流式套接字**。信号会在下列情况中产生

> [!tip] 情形一：监听套接字上接收到了新的连接
> 

> [!tip] 情形二：TCP `connect()` 请求完成，也就是 TCP 连接的主动端进入 ESTABLISHED 状态
> 
> 对于 UNIX 域套接字，类似情况下是不会发出信号的
> 

> [!tip] 情形三：套接字上接收到了新的输入
> 
> 即使已经有未读取的输入存在
> 

> [!tip] 情形四：套接字对端使用 `shutdown()`关闭了写连接（半关闭），或者通过 `close()` 完全关闭
> 

> [!tip] 情形五：套接字上输出就绪
> 
> 例如套接字发送缓冲区中有了空间
> 

> [!tip] 情形六：套接字上发生了异步错误

## 优化信号驱动 IO 的使用

在需要检查大量文件描述符的应用程序中，信号驱动 IO 能够提供显著的性能优势

> [!tip] 
>
> 内核可以“记住”要检查的文件描述符，且仅当 I/O 事件实际发生在这些文件描述符上时才会向程序发送信号
> 

采用信号驱动 I/O 的程序性能可以 **根据发生的 I/O 事件的数量来扩展**，而 **与被检查的文件描述符的数量无关**

要想全部利用信号驱动 I/O 的优点，我们必须执行下面两个步骤

> [!tip] 步骤一：在文件描述符上指定一个 **实时信号**
> 
> 通过专属于 Linux 的 `fcntl()` 的 `F_SETSIG` 操作来指定一个实时信号，当文件描述符上的 IO 就绪时，这个实时信号应该取代 `SIGIO` 被发送
> 
> ```c
> if(fcntl(fd, F_SETSIG, sig) == -1) {
> 	// 错误处理
> }
> ```
> 
> `F_GETSIG` 操作完成的任务同 `F_SETSIG` 相反，它取回当前为文件描述符指定的信号
> 
> 为了在头文件 `<fcntl.h>` 中得到 `F_SETSIG` 和 `F_GETSIG` 的定义，我们必须定义测试宏 `_GNU_SOURCE`
> 
> ---
> 
> 默认的 IO 就绪信号 `SIGIO` 是标准信号，即 **不会对 `SIGIO` 进行排队处理**。如果有多个 I/O 事件发送了信号，而 `SIGIO` 被阻塞了，除了第一个通知外，其他后序的通知都会丢失
> 
> 如果通过 `fcntl()` 的 `F_SETSIG` 来指定一个 **实时信号** 作为 IO 就绪的通知信号，那么**多个通知就能排队处理**
> 
>参考 [[信号高级特性#实时信号]]
>

> [!tip] 步骤二：使用 `sigaction()` 安装信号处理程序
> 
> 为前一步中使用的实时信号指定 `SA_ SIGINFO` 标记。那么 **结构体 `siginfo_t` 会作为第二个参数传递给信号处理例程**。这个结构体包含的字段标识出了在哪个文件描述符上发生了事件，以及事件的类型
> 
> 参考 [[信号处理器函数#SA_SIGINFO 标志]]
>
>

> [!warning] 
> 
> 注意，需要同时使用 `F_SETSIG` 以及 `SA_SIGINFO` 才能将一个合法的 `siginfo_t` 结构体传递到信号处理例程中去
> 
> 如果我们做 `F_SETSIG` 操作时将参数 `sig` 指定为 $0$，那么将导致退回到默认的行为：发送的信号仍然是 `SIGIO`，而且结构体 `siginfo_t` 将不会传递给信号处理例程。
> 

对于“I/O 就绪”事件，传递给信号处理例程的结构体 `siginfo_t` 中与之相关的字段如下

+ `si_signo`：引发信号处理例程得到调用的 **信号值**。这个值同信号处理例程的第一个参数一致
+ `si_fd`：发生 I/O 事件的 **文件描述符**
+ `si_code`：表示发生 **事件类型** 的代码
+ `si_band`：一个 **位掩码**。其中包含的位和系统调用 `poll()` 中返回的 `revents` 字段中的位相同

下表列出了结构体 `siginfo_t` 中 `si_code` 和 `si_band` 字段的可能值

![[Pasted image 20241129220601.png]]

下面的代码展示了信号驱动 IO 的一个示例。例程 A 和例程 B 通过 [[FIFO]] 相互发送消息

```c title:programA_sigio.c
#include <sys/types.h>
#include <sys/stat.h>
#define __USE_GNU
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>

#include "base.h"
#include "rdwrn.h"

#define SIGRTIO (SIGRTMIN + 1)

void handler(int sig, siginfo_t *si, void *ucontext) {
    if (sig != SIGRTIO) return;

    int errnoSaved = errno;
    if (si->si_band & POLL_HUP) {
        _exit(EXIT_FAILURE);
    }
    char buf[BUFSIZ];
    ssize_t numRead = read(si->si_fd, buf, BUFSIZ);
    if (numRead == -1) {
        return;
    }
    if (writen(STDIN_FILENO, buf, numRead) == -1) {
        return;
    }
    errno = errnoSaved;
}

int main(int argc, char *argv[]) {
    // programA_sigio A2B_FIFO B2A_FIFO
    if(argc != 3) {
        usageErr("%s <fifo 1> <fifo 2>", argv[0]);
    }

    // B2A_FIFO 以 O_RDONLY | O_NONBLOCK 打开
    int b2a = open(argv[2], O_RDONLY | O_NONBLOCK);
    if(b2a == -1) {
        errExit("open %s error: ", argv[2]);
    }
    printf("%s read side opened\n", argv[2]);

    // A2B_FIFO 以 O_WRONLY 打开
    int a2b;
    while (true) {
        a2b = open(argv[1], O_WRONLY);
        if(a2b == -1) {
            if (errno == ENXIO) {
                continue;
            }
            errExit("open %s error: ", argv[1]);
        }
        break;
    }
    printf("%s write side opened\n", argv[1]);

    // b2a 设置为 O_ASYNC
    int flags = fcntl(b2a, F_GETFL);
    if (flags == -1) {
        errExit("fcntl error: ");
    }
    if (fcntl(b2a, F_SETFL, flags | O_ASYNC) == -1) {
        errExit("fcntl error: ");
    }
    // 设置属主
    if (fcntl(b2a, F_SETOWN, getpid()) == -1) {
        errExit("fcntl error: ");
    }
    // 设置信号
    if (fcntl(b2a, F_SETSIG, SIGRTIO) == -1) {
        errExit("fcntl error: ");
    }

    struct sigaction act = {0};
    act.sa_sigaction = handler;
    act.sa_flags = SA_SIGINFO | SA_RESTART;
    sigemptyset(&act.sa_mask);

    if (sigaction(SIGRTIO, &act, nullptr) == -1) {
        errExit("sigaction error: ");
    }

    // 主程序从标准输入中读取，并写入到 a2b 中
    while (true) {
        char buf[BUFSIZ];
        ssize_t numRead = read(STDIN_FILENO, buf, BUFSIZ);
        if (numRead == -1) {
            errExit("read error: ");
        }
        if (numRead == 0) {
            break;
        }

        // 处理部分写问题
        if (writen(a2b, buf, numRead) == -1) {
            errExit("writen error: ");
        }
    }
    return 0;
}
```

```c title:programB_sigio
#include <sys/types.h>
#include <sys/stat.h>
#define __USE_GNU
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>

#include "base.h"
#include "rdwrn.h"

#define SIGRTIO (SIGRTMIN + 1)

void handler(int sig, siginfo_t *si, void *ucontext) {
    if (sig != SIGRTIO) return;

    int errnoSaved = errno;
    if (si->si_band & POLL_HUP) {
        _exit(EXIT_FAILURE);
    }

    char buf[BUFSIZ];
    ssize_t numRead = read(si->si_fd, buf, BUFSIZ);
    if (numRead == -1) {
        return;
    }
    if (writen(STDIN_FILENO, buf, numRead) == -1) {
        return;
    }
    errno = errnoSaved;
}

int main(int argc, char *argv[]) {
    // programB_sigio A2B_FIFO B2A_FIFO
    if(argc != 3) {
        usageErr("%s <fifo 1> <fifo 2>", argv[0]);
    }

    // B2A_FIFO 以 O_WRONLY | O_NONBLOCK打开
    int b2a;
    while (true) {
        b2a = open(argv[2], O_WRONLY | O_NONBLOCK);
        if(b2a == -1) {
            if (errno == ENXIO) {
                continue;
            }
            errExit("open %s error: ", argv[2]);
        }
        break;
    }
    printf("%s write side opened\n", argv[2]);

    // A2B_FIFO 以 O_RDONLY | O_NONBLOCK 打开
    int a2b = open(argv[1], O_RDONLY | O_NONBLOCK);
    if(a2b == -1) {
        errExit("open %s error: ", argv[1]);
    }
    printf("%s read side opened\n", argv[1]);

    // a2b 设置为 O_ASYNC
    int flags = fcntl(a2b, F_GETFL);
    if (flags == -1) {
        errExit("fcntl error: ");
    }
    if (fcntl(a2b, F_SETFL, flags | O_ASYNC) == -1) {
        errExit("fcntl error: ");
    }
    // 设置属主
    if (fcntl(a2b, F_SETOWN, getpid()) == -1) {
        errExit("fcntl error: ");
    }
    // 设置信号
    if (fcntl(a2b, F_SETSIG, SIGRTIO) == -1) {
        errExit("fcntl error: ");
    }

    struct sigaction act = {0};
    act.sa_sigaction = handler;
    act.sa_flags = SA_SIGINFO | SA_RESTART;
    sigemptyset(&act.sa_mask);

    if (sigaction(SIGRTIO, &act, nullptr) == -1) {
        errExit("sigaction error: ");
    }

    // 主程序从标准输入中读取，并写入到 b2a 中
    while (true) {
        char buf[BUFSIZ];
        ssize_t numRead = read(STDIN_FILENO, buf, BUFSIZ);
        if (numRead == -1) {
            errExit("read error: ");
        }
        if (numRead == 0) {
            break;
        }
        // 处理部分写问题
        if (writen(b2a, buf, numRead) == -1) {
            errExit("writen error: ");
        }
    }
}
```

### 信号队列溢出的处理

**可以排队的实时信号的数量是有限的**。如果达到这个上限，内核对于 “I/O 就绪” 的通知将恢复为默认的 `SIGIO` 信号。出现这种现象表示 **信号队列溢出** 了。当出现这种情况时，我们将**失去有关文件描述符上发生 I/O 事件的信息**，因为 `SIGIO` 信号是不会排队的

> [!tip]
> 此外，SIGIO 的信号处理例程不接受 `siginfo_t` 结构体参数，这意味着信号处理例程不能确定是哪一个文件描述符上产生了信号
> 

我们可以 **增加可排队的实时信号数量** 的限制来减小信号队列溢出的可能性。但是这并不能完全消除溢出的可能。一个设计良好的采用 `F_SETSIG` 来建立实时信号作为“I/O 就绪”通知的程序必须也要为信号 `SIGIO` 安装处理例程

如果发送了 `SIGIO` 信号，那么应用程序可以先通过 `sigwaitinfo()` 将队列中的实时信号全部获取，然后临时切换到 `select()` 或 `poll()`，通过它们获取剩余的发生 I/O 事件的文件描述符列表

### 在多线程程序中使用信号驱动 I/O

`F_SETOWN_EX` 和 `F_GETOWN_EX` 是 Linux 提供的非标准 `fcntl()` 操作，它们用于 **设定接收 IO 就绪信号的目标**

`F_SETOWN_EX` 操作类似于 `F_SETOWN`，但除了允许指定进程或进程组作为接收信号的目标外，它还可以 **指定一个线程作为“I/O 就绪”信号的目标**。对于这个操作，`fcntl()` 的第三个参数为指向如下结构体的指针

```c
struct f_owner_ex {
	int type;
	pid_t pid;
};
```

结构体中 `type` 字段定义了 `pid` 的类型，它可以有如下几种值。

| `type`         | 描述                                        |
| :------------- | ----------------------------------------- |
| `F_OWNER_PGRP` | 字段 `pid` 代表了接收 IO 就绪信号的 **进程组 ID**，这里是个正数 |
| `F_OWNER_PID`  | 字段 pid 指定了作为接收 IO 就绪 信号的 **进程 ID**        |
| `F_OWNER_TID`  | 字段 `pid` 指定了作为接收“I/O 就绪”信号的 **线程 ID**     |
