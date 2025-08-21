# SOCKET 概览

socket 是一种 IPC 方法，它允许 **位于同一主机** 或 **使用网络连接起来的不同主机** 上的应用程序之间交换数据。TCP/IP 协议族只是网络通信标准，而具体实现由各个操作系统实现。首个被广泛接受的 socket API 实现于 $1983$ 年，出现在了 4.2 BSD 中，称为 **伯克利套接字**

> [!tip] 
> 
> 实际上这组 API 已经被移植到了所有 UNIX 实现以及其他大多数操作系统上了。
> 
> 实现了传输层及以下的所有层次
> 

在一个典型的 **客户端-服务器** 场景中，应用程序使用 `socket` 进行通信的方式如下

> [!tip] 创建 SOCKET
> 
> 各个应用程序创建一个 `socket`。`socket` 是一个允许通信的 “设备”，两个应用程序都需要用到它
> 

> [!tip] 服务端将 SOCKET 绑定
> 
> 服务器将自己的 `socket` 绑定到一个众所周知的地址（名称）上使得客户端能够定位到它的位置
> 

使用 `socket()` 系统调用能够创建一个 `socket`，它返回一个用来在后续系统调用中引用该 `socket` 的文件描述符

```c
fd = socket(domain, type, protocol);
```

> [!tip] 参数 `domain`：指定通信域
> 
> `socket` 存在于一个通信 `domain` 中，它确定：**地址格式** 和 **通信范围**。现代操作系统支持下列 domain
> + **UNIX domain(AF_UNIX)**： 同一主机上的进程之间进行通信
> + **IPv4 domain(AF_INET)**：IPv4 网络连接起来的不同主机上的进程之间进行通信
> + **IPv6 domain(AF_INET6)**：IPv6 网络连接起来的不同主机上的进程之间进行通信
> 
> 下表总结了这些 domain 的通信范围和地址格式
> 
> ![Pasted image 20241120185018|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755784569446-4629a9f548064d31b338ec96289cd67e.png)
> 

> [!tip] 参数 `type`：指定 SOCKET 类型
> 
> 每个 SOCKET 实现都至少提供了两种 SOCKET 类型：**流** 和 **数据报**。这两种类型在上述通信域中得到支持
> 
> + **流式 SOCKET(SOCK_STREAM)**：**可靠** 的面向 **字节流** 的 **全双工** 通信信道
> 	+ 可靠：保证发送者传输的数据会完整无缺地到达接收者
> 	+ 字节流：与 [[Linux 系统编程：PIPE]] 和 [[Linux 系统编程：FIFO]] 类似，消息不存在边界的概念
> 	+ 全双工：数据可以在两个 SOCKET 之间的 **任意方向** 上传递
> 
> + **数据报 SOCKET(SOCK_DGRAM)**：数据以消息形式进行传递。消息边界得到了保留，但是数据传输是不可靠的
> 	+ 数据报 SOCKET 是无连接 SOCKET 的一个示例
> 	+ 一个数据报 SOCKET 在使用时无需与另一个 SOCKET 建立连接
> 
> ![Pasted image 20241120191433|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755784569447-80714f68585043828d8e1e8f0ba5d8ee.png)
> 

## SOCKET API

关键的 `socket` 系统调用包括以下几种
+ `socket()` 系统调用创建一个新 SOCKET
+ `bind()` 系统调用将一个 SOCKET 绑定到一个地址上
+ `listen()` 系统调用允许一个流 SOCKET 接受来自其他 SOCKET 的接入连接
+ `accept()` 系统调用在一个监听流 SOCKET 上接受来自一个对等应用程序的连接，并可选地返回对等 SOCKET 的地址
+ `connect()` 系统调用建立与另一个 SOCKET 之间的连接

通过上述几个系统调用，SOCKET I/O 可以使用传统的 `read()` 和 `write()` 系统调用或使用一组 `socket` 特有的系统调用（如 `send()`、`recv()`、`sendto()` 以及 `recvfrom()`）来完成

> [!important] 
> 
> 在默认情况下，这些系统调用在 **I/O 操作无法被立即完成时会阻塞**。通过使用 `fcntl() F_SETFL` 操作来启用 `O_NONBLOCK` 打开文件状态标记可以执行非阻塞 I/O
> 

## 创建 SOCKET

`socket()` 系统调用用于创建一个新 SOCKET

```c
#include <sys/socket.h>

int socket(int domain, int type, int protocol);
/* 成功返回 socket 文件描述符；失败返回 -1 */
```

> [!tip] 参数 `domain`：指定通信域
> 
> domain 参数的取值为下列预定义宏之一
> + `AF_UNIX(AF_LOCAL)`：**局部通信**，同一主机内
> + `AF_INET`：**IPv4 网络协议**
> + `AF_AX25`：业余无线电 AX.25协议
> + `AF_IPX`：IPX -诺维尔协议
> + `AF_APPLETALK`：可路由协议组
> + `AF_X25`：ITU-T X.25 / ISO/IEC 8208协议
> + `AF_INET6`：**IPv6 网络协议**
> + `AF_DECnet`：DECet协议套接字
> + `AF_KEY`：密钥管理协议，最初是为IPsec开发的
> + `AF_NETLINK`：内核用户界面设备
> + `AF_PACKET`：**低级分组接口**，以太网
> + `AF_RDS`：可靠数据报套接字(RDS)协议
> + `AF_PPPOX`：通用 PPP 传输层，用于建立 L2 隧道(L2TP和PPPoE)
> + `AF_LLC`：逻辑链路控制(IEEE 802.2 LLC)协议
> + `AF_IB`：InfiniBand本地寻址
> + `AF_MPLS`：多协议标签交换
> + `AF_CAN`：控制器局域网络（CAN）：汽车总线协议
> + `AF_TIPC`：TIPC，“集群域套接字”协议
> + `AF_BLUETOOTH`：蓝牙低层套接字协议
> + `AF_ALG`：内核加密API的接口
> + `AF_VSOCK`：虚拟机管理程序，来宾通信的 VSOCK(最初为“VMWare VSockets”)协议象征式互动
> + `AF_KCM`：KCM(内核连接多路复用器)接口
> + `AF_XDP`：XDP(快速数据通路)接口
> 
> `AF` 表示 **地址族(Address Family)**，另外还有 `PF_` 开头的常量，表示 **协议族(Protocol Family)**。然而，SUSv3 标准没有规定 `PF_` 常量，为了可移植性，不建议使用 `PF_` 常量 
> 

下表总结了常用 domain 的取值

| domain             | 描述        |
| :----------------- | :-------- |
| `AF_UNIX/AF_LOCAL` | 本地主机通信    |
| `AF_INET`          | IPv4 网络通信 |
| `AF_INET6`         | IPv6 网络通信 |
| `AF_PACKET`        | 以太网通信     |

> [!tip] 参数 `type`：指定 SOCKET 类型
> 
> type 参数的取值下列预定义宏之一
> + `SOCK_STREAM`：**流式 SOCKET**。对应 [[网络：传输层协议#传输控制协议 TCP|TCP]] 
> + `SOCK_DGRAM`：**数据报 SOCKET**。对应 [[网络：传输层协议#用户数据报协议 UDP|UDP]]
> + `SOCK_SEQPACKET`：类似于 `SOCK_STREAM`，但是保留了消息边界
> + `SOCK_RAW`：**裸 SOCKET**。允许程序自定义协议，或者直接发送 [[网络：网络层协议#Internet 协议 IP|IP]] 数据包
> + `SOCK_RDM`：可靠数据报，类似于 `SOCK_DGRAM`，但是保证了消息可靠性
> 
> 此外，还有一些可选的预定义宏，与上述值取位或
> + `SOCK_CLOEXEC`：为 SOCKET 文件描述符启用 close-on-exec 标记(执行时关闭)
> + `SOCK_NONBLOCK`：为 SOCKET 文件描述符设置 `O_NONBLOCK` 标记(非阻塞 IO)
> 

下表总结了可能使用 `type` 的取值

| `type`        | 描述                                               |
| :------------ | :----------------------------------------------- |
| `SOCK_STREAM` | 流式 SOCKET。通常是 TCP                                |
| `SOCK_DGRAM`  | 数据报 SOCKET。通常是 UDP                               |
| `SOCK_RAW`    | 裸 SOCKET。通常是 IP。如果 `domain` 指定 `AF_PACKET`，则是以太网 |

> [!tip] 参数 `protocol`：协议族中特定的 **套接字类型** 支持协议的协议号
> 
> 通常，在给定 **协议族**(参数 `domain` 指定的协议族)中，**只有一个协议** 存在以支持特定的**套接字类型**(参数 `type` 指定的套接字类型)，在这种情形下，该参数的取值为 $0$
> 
> 如果给定协议族中存在许多协议，就必须通过该参数指定协议号
> + 要使用的协议号与进行通信的 “通信域 ”有关；请参阅 `man 5 protocols`
> + 请参阅 `man 3 getprotoent` 了解如何将协议名称字符串映射到协议编号

## 将 SOCKET 绑定到地址

`bind()` 系统调用将一个 SOCKET 绑定到一个地址上

```c
#include <sys/socket.h>

int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
/* 成功返回 0；失败 -1*/
```

> [!tip] 参数 `sockfd`：SOCKET 文件描述符
> 
> 该参数通常是 `socket()` 的返回值
> 

> [!tip] 参数 `addr`：指向结构 `sockaddr` 的指针
> 
> 由于不同的 SOCKET domain 采用的地址结构不同
> + `AF_UNIX`：采用路径名
> + `AF_INET`：使用 $32$ 位 IP 地址和 $16$ 位的端口
> + `AF_INET6`：使用 $128$ 位 IP 地址和 $16$ 位的端口
> 
> 结构 `sockaddr` 是 SOCKET API 定义的 **通用的地址结构**。该结构的唯一作用就是将各自 domain 的特定地址结构转换成这个类型，从而避免编译器警告。结构 `sockaddr` 的定义如下
>
> ```c
> struct sockaddr
> {
> 	sa_family_t sa_family;  // AF_*
> 	char sa_data[14];       // 地址
> };
> ```
> 字段 `sockaddr.sa_family` 是 `socket()` 的 `domain` 参数
> 

> [!tip] 参数 `addrlen`：指定 SOCKET domain 地址结构的长度

## 流式 SOCKET

下图显示了流式 SOCKET 通信双方需要编程模型和使用到的系统调用

![Pasted image 20241120225128|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755784569447-e7122f12ae3944b9a9fae078daa1eb51.png)

### 监听接入连接

`listen()` 系统调用将文件描述符 `sockfd` 引用的流式 SOCKET 标记为被动。这个 SOCKET 后面会被用来 **接受来自其他（主动的）SOCKET 的连接**

```c
#include <sys/socket.h>

int listen(int sockfd, int blocklog);
/* 成功返回 0；失败 -1*/
```

> [!tip] 参数 `sockfd`：SOCKET 文件描述符
> 
> 该参数通常是 `socket()` 的返回值
> 

> [!tip] 参数 `blocklog`：未决连接数
> 
> 下面两个情形会产生 **未决连接**
> 
> + 客户端可能会在服务器调用 `accept()` 前调用 `connect()`
> + 服务器正忙于处理其他客户端
> 
> 内核必须要记录所有未决的连接请求的相关信息，这样后续的 accept()就能够处理这些请求了
> 
> SUSv3 规定实现应该通过在 `<sys/socket.h>` 中定义 `SOMAXCONN` 常量来限制未决连接数。
> + 在 Linux 上，这个常量值被定义为 $128$
> 
> 从内核 2.4.25 起，Linux 允许运行时通过 Linux 特有的 `/proc/sys/net/core/somaxconn` 文件修改 `SOMAXCONN` 值
> 

`listen()` 会修改 `sockfd` 指定的 SOCKET 对象，在其中建立两个队列：**半连接队列** 和 **全连接队列**。相当于告诉系统内核，这个 SOCKET 对象可以与客户端建立连接了

> [!tip] 半连接队列：连接处于 SYN_RCVD 状态
> 
>  + 客户端调用 `connect()`，会向服务端发送一个 SYN 报文段(第一次握手)
>  + 服务端会向客户端发送 ACK+SYN 报文段作为响应(第二次握手)，并将连接放入半连接队列，然后等待客户端的 ACK 报文段
> 

> [!tip] 全连接队列：连接处于 ESTABLISHED 状态
> 
> + 客户端收到服务端的 ACK+SYN 报文段之后，再次向服务端发送 ACK 报文段(第三次握手)。此时，`connect()` 函数返回
> + 收到客户端的 ACK 报文段之后，会将连接移入全连接队列
> 

此时，客户端与服务端的连接就已经建立好了。只是，该连接还处于 **未决状态**

> [!attention] 
> 
> 上述操作（三次握手，移入队列）都是由内核完成的。因此，及时应用程序在做其他事情，客户端依旧可以完成连接
> 
> `listen()` 之后，SOCKET 就只能用于建立连接，不能用于收发数据
> 

### 接受连接

`accept()` 系统调用在 **监听 SOCKET 对象** 的全连接队列中 **取出一个未决连接** 并构建一个全新的 SOCKET 对象，这个新的 SOCKET 对象成为 **通信 SOCKET 对象**。如果在调用 `accept()` 时 **不存在未决的连接**，那么调用就会 **阻塞直到有连接请求到达为止**

```c
#include <sys/socket.h>

int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
/* 成功返回文件描述符，失败返回 -1 */
```

> [!tip] 参数 `sockfd`：被动流式 SOCKET 文件描述符
> 
> 该参数通常是被 `listen()` 修改后的 `socket()` 的返回值
> 

> [!tip] 参数 `addr`：指向 `sockaddr` 的缓冲区，存储对端地址
> 
> 参数 `addr` 保存对端地址
> 

> [!tip] 参数 `addrlen`：对端地址结构的长度
> 

> [!warning] 参数 `addr` 和 `addrlen` 必须一起使用
> 
> 如果需要获取对端的 SOCKET 地址，`addrlen` 必须要给定一个 SOCKET 地址的最大值
> 

`accept()` 会创建一个全新的流式 SOCKET，并且正是这个新的流式 SOCKET 与执行 `connect()` 的对等 SOCKET 进行连接

> [!tip] 返回值
> 
> `accetp()` 返回的结果是 **已连接** 的 SOCKET 的文件描述符
> 

### 连接到对等 SOCKET

`connect()` 系统调用将文件描述符 `sockfd` 引用的主动 SOCKET 连接到地址通过 `addr` 和 `addrlen` 指定的监听 SOCKET 上

```c
#include <sys/socket.h>

int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
/* 成功返回 0，失败返回 -1 */
```

> [!tip] 参数 `sockfd`：主动 SOCKET 的文件描述符
> 
> 通常是 `socket()` 的返回值
> 

> [!tip] 参数 `addr`：被动 SOCKET 所在的地址

> [!tip] 参数 `addrlen`：被动 SOCKET 地址结构的长度

`connect()` 系统调用会发起第一次挥手，并在发送第三次挥手后返回

### 流式 SOCKET IO

一对连接的流式 SOCKET 在两个端点之间提供了一个双向通信信道，下图给出了 UNIX domain 的情形

![Pasted image 20241120232734|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755784569449-f615428df63b4ce0ae2992f36254adfc.png)

连接流式 SOCKET 上 I/O 的语义与管道上 I/O 的语义类似

> [!tip] 使用通用 IO 系统调用: `read()` 和 `write()` 或者 流式 SOCKET 专用 IO 系统调用: `send()` 和 `recv()`
> 
> 由于 socket 是双向的，因此在连接的两端都可以使用这两个调用
> 

> [!tip] SOCKET 可以使用 `close()` 系统调用关闭或者在应用程序终止之后关闭
> 当 SOCKET 关闭后，在对等 SOCKET 上执行 IO 操作会产生如下结果
> + 从对等 SOCKET 上**读取**将会在缓冲区中的数据读取完毕后**收到 EOF**
> + 向对等 SOCKET 上 **写入** 数据，那么进程将会 **收到 SIGPIPE 信号**，并且系统调用会返回 `EPIPE`
> 

`send()` 函数向 SOCKET 写入数据；`recv()` 函数从 SOCKET 读取数据

```c
#include <sys/socket.h>

ssize_t send(int sockfd, const void *buf, size_t len, int flags);

ssize_t recv(int sockfd, void *buf, size_t len, int flags);
/* 成功返回读取的字节数，失败返回 -1 */
```

`send()` 和 `recv()` 前 $3$ 个参数和返回值与通用 IO 系统调用 `read()` 和 `write()` 是一样的，这里不在重复介绍

> [!tip] 参数 `flags`：控制 SOCKET 特定的 IO 的特性
> 
> 这些特性会在 [[Linux 系统编程：SOCKET 高级主题]] 中进行介绍。。如果无需使用其中任何一种特性，那么可以将 `flags` 指定为 $0$。

> [!warning] 
> 
> `send()` 并不是真正的发送数据，只是将 `buf` 中的数据拷贝到 **发送缓冲区**
> 
> `recv()` 并不是真正的接收数据，而是将 **接收缓冲区** 中的内容拷贝到 `buf` 中
> 

### 连接终止

终止一个流式 SOCKET 连接的常见方式是调用 `close()`。如果多个文件描述符引用了同一个 SOCKET，那么当所有描述符被关闭之后连接就会终止

> [!tip] 和关闭文件的语义是一样的：关闭只是减少 SOCKET 的引用计数

## 数据报 SOCKET

下图显示了数据报 SOCKET 的相关系统调用的使用

![Pasted image 20241120233738|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755784569450-fea4ec772e1f44d0aaecfe61b422a62f.png)

### 数据报 SOCKET IO

`recvfrom()`和 `sendto()`系统调用在一个数据报 SOCKET 上接收和发送数据报

```c
#include <sys/socket.h>

ssize_t recvfrom(int sockfd, void *buffer, size_t length, int flags, struct sockaddr *src_addr, socklen_t *addrlen);
/* 返回接受的字节数。EOF 返回 0；失败返回 -1 */

ssize_t sendto(int sockfd, const void *bufferm, size_t lengthm int flags, const struct sockaddr *dest_addr, socklen_t addrlen);
/* 返回发送的字节数，失败返回 -1 */
```

`recvfrom()` 和 `sendto()` 的前 $3$ 个参数和返回值与通用 IO 系统调用 `read()` 和 `write()` 是一样的

> [!tip] 参数 `flags`：控制 SOCKET 特定的 IO 的特性
> 
> 这些特性会在 [[Linux 系统编程：SOCKET 高级主题]] 中进行介绍。。如果无需使用其中任何一种特性，那么可以将 `flags` 指定为 $0$。
> 

> [!tip] 参数 `src_addr` 和 `addrlen`：获取对等 SOCKET 的地址和地址结构的长度
> 
> `recvfrom()` 的最后两个参数
> 

> [!tip] 参数 `dest_addr` 和 `addrlen`：指定对等 SOCKET 的地址和地址结构对的长度
> 
> `sendto()` 的最后两个参数
> 

> [!warning] 
> 
> 不管 `length` 的参数值是什么，`recvfrom()` **只会数据报 SOCKET 中读取一条消息**。如果消息的大小 **超过了 `length` 字节**，那么消息会被 **静默地截断为 `length` 字节**
> 

## 总结

> [!summary] 
> SOCKET 允许在同一主机或通过一个网络连接起来的不同主机上的应用程序之间通信
> 
> 使用 **通信域(domain)** 确定通信范围和用来标识 SOCKET 的地址格式
> + `AF_UNIX`(UNIX)
> + `AF_INET`(IPv4)
> + `AF_INET6`(IPv6)
>
>大多数应用程序使用两种 SOCKET 类型：**流式 SOCKET(SOCK_STREAM)** 和 **数据报 SOCKET(SOCK_DGRAM)**
>
>+ `SOCK_STREAM`：面向连接的、可靠的、全双工的字节流通信信道。通常是 TCP 的实现
>+ `SOCK_DGRAM`：无连接的、不可靠的、面向消息的通信。通常是 UDP 的实现
>
 