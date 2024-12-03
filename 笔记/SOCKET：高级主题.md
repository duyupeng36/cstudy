# SOCKET：高级主题

## 流式 SOCKET ：部分读和部分写

使用 `read()` 和 `write()` 时，它们传输的数据可能会比请求的少。当在流式套接字上执行 IO 操作时，也会出现这种 **部分传输的现象**

> [!tip] 部分读：**请求的数据比 SOCKET 上可用数据少**
> 
> 如果套接字上可用的数据比在 `read()` 调用中请求的数据要少，那就可能会出现部分读的现象
> 

> [!tip] 部分写：**没有足够的缓冲区空间来传输所有请求的字节**
> 
> 此外，还有几条造成部分写原因
> + 在 `write()` 调用传输了部分请求的字节后 **被信号处理器函数中断**
> + 套接字工作在**非阻塞**模式下（`O_NONBLOCK`），可能当前 **只能传输一部分** 请求的字节
> + 在部分请求的字节已经完成传输后出现了一个 **异步错误**
> 
> 注意：所谓的异步错误是指应用程序使用的 **套接字 API 调用中出现了一个异步错误**，比如，由于 **TCP 连接出现问题**，可能就会使对端的应用程序崩溃
> 
> 上述情况中，假设缓冲区空间**至少能传输 $1$ 字节数据**，**`write()` 调用成功**，并返回传输到输出缓冲区中的字节数
> 

出现部分读和部分写问题时，**需要重新调用系统调用来完成全部数据的传输**。下面给出了两个函数 `readn()` 和 `writen()`，它们封装了 `read()` 和 `write()` 从而解决部分传输问题

```c
#include "rdwrn.h"

ssize_t readn(int fd, void *buffer, size_t count);
/* 成功返回读取的字节数；EOF 返回 0；失败返回 -1 */

ssize_t writen(int fd, void *buffer, size_t count);
/* 成功返回写入的字节数；失败返回 -1 */
```

函数 `readn()` 和 `writen()` 的参数与 `read()` 和 `write()` 相同。但是，这两个函数**使用循环来重新启用这些系统调用**，因此 **确保了请求的字节数总是能够全部得到传输**

> [!attention] 除非出现错误或者在 `read()` 中检测到了文件结尾符


```c title:include/rdwrn.h
#ifndef RDWRN_H
#define RDWRN_H

#include <sys/types.h>

ssize_t readn(int fd, void *buffer, size_t count);
/* 成功返回读取的字节数；EOF 返回 0；失败返回 -1 */

ssize_t writen(int fd, void *buffer, size_t count);
/* 成功返回写入的字节数；失败返回 -1 */

#endif //RDWRN_H
```

```c title:lib/rdwrn.c

#include "rdwrn.h"

#include <errno.h>
#include <unistd.h>

/* 成功返回读取的字节数；EOF 返回 0；失败返回 -1 */
ssize_t readn(int fd, void *buffer, size_t n) {
    ssize_t numRead = 0;
    ssize_t totalRead = 0;
    char *buf = buffer;
    while (totalRead < n) {
        numRead = read(fd, buf, n - totalRead);
        if (numRead == 0) {
            return totalRead;
        }
        if (numRead == -1) {
            if (errno == EINTR) {
                continue;
            }
            return -1;
        }
        totalRead += numRead;
        // 下一次写入 buf 的起点
        buf += numRead;
    }
    return totalRead;
}

/* 成功返回写入的字节数；失败返回 -1 */
ssize_t writen(int fd, void *buffer, size_t n) {
    ssize_t numWritten = 0;
    ssize_t totalWritten = 0;
    const char *buf = buffer;
    while (totalWritten < n) {
        numWritten = write(fd, buf, n - totalWritten);
        if (numWritten <= 0) {
            // 被信号处理器程序中断
            if (numWritten == -1 && errno == EINTR) {
                continue;
            }
            return -1;
        }
        totalWritten += numWritten;
        // 下次开始写入的位置
        buf += numWritten;
    }
    return totalWritten;
}
```

## shutdown()

**在套接字上调用 `close()` 会将双向通信通道的两端都关闭**。有时候，**只关闭连接的一端**也是有用处的，这样数据只能在一个方向上通过套接字传输。系统调用 `shutdown()` 提供了这种功能

```c
#include <sys/socket.h>

int shutdown(int sockfd, int how);
/* 成功返回 0；错误返回 -1 */
```

> [!tip] 参数 `sockfd`：待关闭的 SOCKET 文件描述符
> 
> 该参数通常是 `socket()` 的返回值
> 


> [!tip] 参数 `how`：选择关闭套接字通道的一端还是两端
> 
> + `SHUT_RD`：**关闭连接的读端**。后续执行 `read()` 操作将返回文件结尾($0$)。
> 	+ 对于流式 SOCKET，**对端** 应用程序应该收到 `SIGPIPE` 信号
> 	+ 继续在 **对端** SOCKET 上执行 `write()` 则返回错误并设置 `errno` 为 `EPIPE`
> 
> + `SHUT_WR`：**关闭写端**。后续如果执行 `write()` 则会接收到 `SIGPIPE` 信号和设置 `errno`为 `EPIPE`
> 	+ 对端应用程序将剩余数据读取完毕后，检测到文件结尾
> 	+ 对端应用程序写入的数据仍然可以在套接字上读取
> 
> + `SHUT_RDWR`：**将连接的读端和写端都关闭**
> 

下表总结参数 `how` 的取值和作用

| `how`       | 描述                                        |
| :---------- | ----------------------------------------- |
| `SHUT_RD`   | 关闭 SOCKET 的读端。**对端不可写但可读**。本端写不受限，读返回 $0$ |
| `SHUT_WR`   | 关闭 SOCKET 的写端。**本端不可写但可读**。对端写不受限，读返回 $0$ |
| `SHUT_RDWR` | 关闭 SOCKET 的读端和写端                          |

> [!warning] `shutdown()` 与 `close()` 之间的差异
> + `shutdown()`立即关闭套接字通道。换句话说，**根据系统打开文件描述符来执行操作的**，而非进程级打开文件描述符
> + `close()` 不一定会关闭套接字通道。根据进程级打开文件描述符来进行操作。只要进程中还有引用，就会立即关闭
> 
> 需要注意的是，**`shutdown()` 并不会关闭文件描述符**，就算参数 `how` 指定为 `SHUT_RDWR` 时也是如此。**要关闭文件描述符，必须另外调用 `close()`**
> 

假设 `sockfd` 指向一个已连接的流式套接字，如果执行下列调用，那么连接依然会保持打开状态，我们仍然可以通过文件描述符 `fd2` 在该连接上做 I/O 操作

```c
fd2 = dup(sockfd);  // 复制文件描述符
close(sockfd); // 关闭 sockfd。sockfd 指向的打开文件描述符并未关闭
```

如果执行 `shutdown()`，那么该连接的双向通道都会关闭，通过 `fd2` 也无法再执行 I/O 操作了

```c
fd2 = dup(sockfd);  // 复制文件描述符
shutdown(sockfd, SHUT_RDWR); 
```

## recv() 和 send()

`recv()` 和 `send()` 系统调用可在已连接的套接字上执行 I/O 操作。它们提供了专属于套接字的功能，而这些功能在传统的 `read()` 和 `write()` 系统调用中是没有的

```c
#include <sys/socket.h>

ssize_t send(int sockfd, const void *buf, size_t len, int flags);

ssize_t recv(int sockfd, void *buf, size_t len, int flags);
/* 成功返回读取的字节数，失败返回 -1 */
```

在 [[SOCKET 概览]] 中介绍了这两个函数。下面我介绍参数 `flags` 的取值。这里两个函数的 `flags` 参数取值不同。下面分别介绍

> [!tip] 对于 `recv()` 的第 $4$ 个参数 `flags` 的取值如下
> + `MSG_DONTWAIT`：**非阻塞** 的执行 `recv()`。没数据返回 $-1$，并设置 `errno` 为 `EAGAIN`
> 	+ 还可以通过 `fcntl()` 将套接字修改为 非阻塞模式(`O_NONBLOCK`)
> + `MSG_OOB`：在套接字上**接收带外数据**。后续内容介绍 **带外数据**
> + `MSG_PEEK`：请求缓冲区数据，但不移除缓冲区中的数据
> + `MSG_WAITALL`：**阻塞**，直到能够获取请求的字节数。下列情况会打断阻塞，返回字节数少于请求字节数
> 	+ 捕获到一个信号
> 	+ 流式套接字的对端终止了连接
> 	+ 遇到了带外数据字节
> 	+ 从数据报套接字接收到的消息长度小于请求的字节数
> 	+ 套接字上出现了错误
> 

下表总结了 `recv()` 的第 $4$ 个参数 `flags` 的取值

| `flags`        | 描述           |
| :------------- | :----------- |
| `MSG_DONTWAIT` | 非阻塞 `recv()` |
| `MSG_OOB`      | 接收带外数据       |
| `MSG_PEEK`     | 可重复的请求缓冲区    |
| `MSG_WAITALL`  | 阻塞等待请求的字节数   |

> [!tip] 对于 `send()` 的第 $4$ 个参数 `flags` 的取值如下
> 
> + `MSG_DONTWAI`：**非阻塞**的执行 `send()`。类似于 `recv()`
> + `MSG_MORE`：
> 	+ TCP 套接字，**对数据进行栓塞处理**，类似于套接字选项 `TCP_CORK`
> 	+ UDP 套接字，**打包成一个单独的数据报**
> + `MSG_NOSIGNAL`：在流式 SOCKET 上发送数据，如果 **对端关闭读端**，不会产生 `SIGPIPE` 信号。`send()`调用会失败，伴随的错误码为 `EPIPE`
> 	+ 与忽略 `SIGPIPE` 行为类似
> + `MSG_OOB`：在流式套接字上**发送带外数据**
> 

## 发送文件：sendfile()

像 Web 服务器和文件服务器这样的应用程序常常需要将磁盘上的文件内容 **不做修改** 地通过套接字(**已连接**)传输出去

一种方法是 **通过循环** 按照如下方式处理

```c
while((n = read(diskfilefd, buf, BUZ_SIZE)) > 0) {
	write(sockfd, buf, n);
}
```

对于许多应用程序来说，这样的循环是完全可接受的。但是，如果我们需要通过套接字频繁地传输大文件的话，这种技术就显得很不高效

> [!tip] 为了传输文件，必须使用两个系统调用，可能需要在循环中多次调用
> + 一个用来将文件内容从内核缓冲区 cache 中拷贝到用户空间
> + 另一个用来将用户空间缓冲区拷贝回内核空间，以此才能通过套接字进行传输
> 
> 如下图所示
> 
> ![[Pasted image 20241128215758.png]]
> 

如果应用程序在发起传输之前根本 **不对文件内容做任何处理** 的话，那么这种 **两步式的处理就是一种浪费**。系统调用 `sendfile()` 被设计为用来消除这种低效性

> [!tip] 
> 
> 当应用程序调用 `sendfile()` 时，**文件内容会直接传送到套接字上，而不会经过用户空间**
> 
> 这种不经过用户空间的技术称为 **零拷贝传输**。如下图所示
> 
> ![[Pasted image 20241128220049.png]]
>

```c
#include <sys/sendfile.h>

ssize_t sendfile(int sockfd, int fd, offset_t *offset, size_t count);
/* 成功返回传输的字节数，失败返回 -1 */
```

> [!tip] 参数 `sockfd`：套接字文件描述符
> 
> 通信套接字文件描述符
> + 服务端，通常是 `accept()` 的返回值
> + 客户端，通常是 `socket()` 的返回值
> 

> [!tip] 参数 `fd`：磁盘文件描述符，通常是普通文件
> 
> 注意：该文件描述符指向的 **文件必须能够进行 `mmap()` 操作**
> 
> 这些局限限制了 `sendfile()` 的使用
> + 不能将 SOCKET 上的数据写入文件
> + 也不能在两个 SOCKET 之间直接传输数据
> 

> [!tip] 参数 `offset`
> 
> 不是 `NULL` 时，应该指向一个 `off_t` 的值，该值指定了 **起始文件的偏移量**。这是一个传入传出参数，通过该值获取最后传送字节的下一个字节。此时，`sendfile()` **不会修改 `fd` 的文件偏移量**
> 
> 为 `NULL` 时，从当前文件偏移位置开始，并且会更新文件偏移量，以反映出已传输的字节数
> 

> [!tip] 参数 `count` ：指定了请求传输的字节数
> 

> [!tip] 返回值
> 
> 返回实际传输的字节数
> + 如果在 `count` 个字节完成传输前就遇到了文件结尾符，那么只有文件结尾符之前的那些字节能传输
> 

## 获取套接字地址

`getsockname()` 和 `getpeername()` 这两个系统调用分别返回 **本地套接字地址** 以及 **对端套接字地址**

```c
#include <sys/socket.h>

int getsockname(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
int getpeername(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
/* 成功返回 0；失败返回 -1 */
```

## 套接字选项

套接字选项能影响到套接字操作的多个功能。系统调用 `setsockopt()` 和 `getsockopt()` 是用来设定和获取套接字选项的

```c
#include <sys/socket.h>

int getsockopt(int sockfd, int level, int optname, void *optval, socklen_t *optlen);
int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t *optlen);

/* 成功返回 0；失败返回 -1 */
```

> [!tip] 参数 `sockfd`：套接字的文件描述符
> 
> 通常是 `socket()` 或 `accept()` 的返回值
> 

> [!tip] 参数 `level`：套接字选项所适用的协议。比如，IP 或者 TCP
> 
> 如果选项作用于 **套接字 API 层**，则 `level` 都设置为 `SOL_SOCKET`
> 

> [!tip] 参数 `optname`：套接字选项
> 
> 这个参数可以是一个指向整数或结构体的指针
> 

> [!tip] 参数 `optval`：指向缓冲区的指针，用来指定或者返回选项的值

> [!tip] 参数 `optlen`：由 `optval` 所指向的缓冲区空间大小

**套接字选项与打开的文件描述相关联**，这表示通过 `dup()` 或 `fork()` 调用复制而来的文件描述符副本同原始的文件描述符一起共享套接字选项集合

套接字选项的一个简单例子是 `SO_TYPE`，可以用来找出套接字的类型

```c
int optval;
socklen_t optlen;

optlen = sizeof(optval);
if(getsockopt(sockfd, SOL_SOCKET, SO_TYPE, &optval, &optlen) == -1) {
	// 错误处理
}
```

经过这个调用之后，`optval` 就包含了套接字类型，比如 `SOCK_STREAM` 或者 `SOCK_DGRAM`

### SO_REUSEADDR 套接字选项

`SO_REUSEADDR` 套接字选项可用作多种用途。这里只关注一种最常见的用途：**避免当 TCP 服务器重启时，尝试将套接字绑定到当前已经同 TCP 结点相关联的端口上时出现的 EADDRINUSE（地址已使用）错误**

> [!tip] 用于无视 `TIME_WAIT` 状态
> 
> + 服务端崩溃，会主动关闭与客户端的连接，次数服务端会处于 `TIME_WAIT` 状态
> 

## 高级功能

### 带外数据

**带外数据** 是流式套接字的一种特性，**允许发送端将传送的数据标记为高优先级**。接收端不需要读取字节流中所有的中间数据就能获得有可用的带外数据的通知，比如 `telnet`、`rlogin` 以及 `ftp`，它们利用该特性来 **终止之前传送的命令**

> [!tip] 
> 
> 带外数据的发送和接收需要在 `send()` 和 `recv()` 中指定 `MSG_OOB` 标记
> 

当一个套接字接收到带外数据可用的通知时，**内核为套接字的属主（通常是使用该套接字的进程）生成 `SIGURG` 信号**，如同 `fcntl()` 的 `F_SETOWN` 操作一样

当采用 TCP 套接字时，任意时刻最多只有 $1$ 字节数据可被标记为带外数据。如果在接收端处理完前一个带外数据字节之前，发送端发送了额外的带外数据，那么之前对带外数据的通知就会丢失
