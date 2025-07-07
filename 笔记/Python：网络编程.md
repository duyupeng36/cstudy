# 网络编程

我们在 [[网络：链路层协议]] [[网络：网络层协议]] 和 [[网络：传输层协议]] 中介绍了常用的 **网络协议**

操作系统将这些协议封装并向应用程序提供访问网络的接口，我们在 [[Linux 系统编程：SOCKET 概览]] 中介绍了 Unix/Linux 提供的网络编程接口 SOCKET。

Python 在操作系统提供的网络编程接口 SOCKET 上进一步封装，以标准库 `socket` 的形式提供 Python 的网络编程接口

## 协议

单纯的电信号 $0$ 和 $1$ 没有任何意义，必须规定电信号多少位一组，每组什么意思，这就是 **协议**。

在之前的内容中，我们已经介绍了网络协议
+ 在 [[网络：链路层协议]] 中介绍了 **以太网协议** 和 **ARP 协议**
+ 在 [[网络：网络层协议]] 中介绍了 **IP协议**
+ 在 [[网络：传输层协议]] 中介绍了 **TCP 协议** 和 **UDP 协议**

## SOCKET 概览

在 Python 中，使用 `socket` 标准库进行网络编程，它提供了访问 **BSD 套接字** 的接口。在所有现代 Unix 系统、Windows、macOS 和其他一些平台上可用。

> [!tip] 
> 
> 这个 Python 接口是将 Unix 系统调用和套接字库接口直接转写为 Python 的面向对象风格：函数 [`socket()`](https://docs.python.org/zh-cn/3.13/library/socket.html#socket.socket "socket.socket") 返回一个 _套接字对象_，其方法是对各种套接字系统调用的实现。 形参类型相比 C 接口更高级一些：如同在 Python 文件上的 `read()` 和 `write()` 操作那样，**接受操作的缓冲区分配是自动进行的**，**发送操作的缓冲区长度则是隐式的**。
> 

### 创建套接字对象

使用函数 `socket.socket(family=AF_INET, type=SOCK_STREAM, proto=0, fileno=None)` 创建 SOCKET 对象。

> [!tip] 参数 `family`：指定通信域
> 
> 它确定 **地址格式** 和 **通信范围**。常用的通信域如下 
> + **UNIX domain(AF_UNIX)**： 同一主机上的进程之间进行通信
> + **IPv4 domain(AF_INET)**：IPv4 网络连接起来的不同主机上的进程之间进行通信
> + **IPv6 domain(AF_INET6)**：IPv6 网络连接起来的不同主机上的进程之间进行通信
> 
> 下表总结了这些 domain 的通信范围和地址格式
> 
> ![[Pasted image 20241120185018.png]]

> [!tip] 参数 `type`：指定 SOCKET 类型
> 
> type 参数的取值下列预定义宏之一
> + `SOCK_STREAM`：**流式 SOCKET**。实现了 [[网络：传输层协议#传输控制协议 TCP|TCP]] 协议 
> + `SOCK_DGRAM`：**数据报 SOCKET**。实现了 [[网络：传输层协议#用户数据报协议 UDP|UDP]] 协议
> + `SOCK_SEQPACKET`：类似于 `SOCK_STREAM`，但是保留了消息边界
> + `SOCK_RAW`：**裸 SOCKET**。允许程序自定义协议，或者直接发送 [[网络：网络层协议#Internet 协议 IP|IP]] 数据包
> + `SOCK_RDM`：可靠数据报，类似于 `SOCK_DGRAM`，但是保证了消息可靠性
> 
> 此外，还有一些可选的预定义宏，与上述值取位或
> + `SOCK_CLOEXEC`：为 SOCKET 文件描述符启用 close-on-exec 标记(执行时关闭)
> + `SOCK_NONBLOCK`：为 SOCKET 文件描述符设置 `O_NONBLOCK` 标记(非阻塞 IO)
> 

> [!tip] 参数 `protocol`：协议族中特定的 **套接字类型** 支持协议的协议号
> 
> 通常，在给定 **协议族**(参数 `domain` 指定的协议族)中，**只有一个协议** 存在以支持特定的**套接字类型**(参数 `type` 指定的套接字类型)，在这种情形下，该参数的取值为 $0$
> 
> 如果给定协议族中存在许多协议，就必须通过该参数指定协议号
> + 要使用的协议号与进行通信的 “通信域 ”有关；请参阅 `man 5 protocols`
> + 请参阅 `man 3 getprotoent` 了解如何将协议名称字符串映射到协议编号
> 

> [!tip] 参数 `fileno`：指定在哪个文件描述符上创建 SOCKET 套接字
> 
> 注意：请将该参数保持默认，让操作系统自行选择文件描述符
> 

> [!seealso] 
> 
> 更多内容，参考 [[Linux 系统编程：SOCKET 概览#创建 SOCKET]]
> 

请注意：从 Python3.4 开始，由 Python 创建的文件描述符默认是 **不可继承的**，即设置了 `close-on-exec` 标志位

### 绑定地址

要给某个 SOCKET 绑定地址时，需要调用 SOCKET 对象的 `socket.bind(address)` 方法。参数 `address` 需要根据创建 SOCKET 对象使用参数 `family` 提供对应的地址结构

| `family`          | `address`                              | 描述           |
| :---------------- | -------------------------------------- | ------------ |
| `socket.AF_UNIX`  | 路径名，必须是一个字符串或 bytes 对象                 | 用于本地通信       |
| `socket.AF_INET`  | 元组形式 `(host, port)`                    | 用于 IPv4 网络通信 |
| `socket.AF_INET6` | 元组形式`(host, port, flowinfo, scope_id)` | 用于 IPv6 网络通信 |

> [!tip] 
> 
> `host` 是一个表示互联网域名标记形式的 **主机名** 或者 **IPv4 地址** 的字符串
> + 例如 `'100.50.200.5'`
> + 例如 `'daring.cwi.nl'` 
> 
> `port` 是一个整数值。取值范围在 $0\sim 65535$，通常取大于 $1024$ 的值
> 
> `flowinfo` 和 `scope_id` 代表了 C 库 `struct sockaddr_in6` 中的 `sin6_flowinfo` 和 `sin6_scope_id` 成员。对于 Python 标准库 `socket` 而言，这些可以省略
> 

### 流式 SOCKET 编程模型

下图显示了流式 SOCKET 通信双方需要编程模型和使用到的函数或方法

![[Pasted image 20241120225128.png]]

#### 监听接入连接

调用 SOCKET 对象的 `socket.listen([backlog])` 方法用将流式 SOCKET 对象标记为 **被动对象**。这个 SOCKET 后面会被用来 **接受来自其他（主动的）SOCKET 的连接**

> [!tip] 参数 `backlog`：未决连接数
> 
> 下面两个情形会产生 **未决连接**
> 
> + 客户端可能会在服务器调用 `accept()` 前调用 `connect()`
> + 服务器正忙于处理其他客户端
> 
> 内核必须要记录所有未决的连接请求的相关信息，这样后续的 accept()就能够处理这些请求了
> 
> SUSv3 规定实现应该通过在 `<sys/socket.h>` 中定义 `SOMAXCONN` 常量来限制未决连接数。在 Python 的标准库 `socket` 中提供相同的常量 `socket.SOMAXCONN` 
> + 在 Linux 上，这个常量值被定义为 $128$
> 
> 从内核 2.4.25 起，Linux 允许运行时通过 Linux 特有的 `/proc/sys/net/core/somaxconn` 文件修改 `SOMAXCONN` 值
> 

`socket.listen()` 会修改 SOCKET 对象，建立两个队列：**半连接队列** 和 **全连接队列**。相当于告诉系统内核，这个 SOCKET 对象可以与客户端建立连接了

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
> 请注意：`socket.listen()` 之后，SOCKET 对象就只能用于建立连接，不能用于收发数据
> 

#### 接受连接

方法 `socket.accept()` 用于在 **监听 SOCKET 对象** 的全连接队列中 **取出一个未决连接** 并构建一个 **全新的 SOCKET 对象**，这个新的 SOCKET 对象成为 **通信 SOCKET 对象**。如果在调用 `socket.accept()` 时 **不存在未决的连接**，那么调用就会 **阻塞直到有连接请求到达为止**

> [!tip] 
> 
> 也就是说，`socket.accept()` 返回一个全新的 SOCKET 对象，这个 SOCKET 对象用于与客户端通信
> 

请注意，方法 `socket.accept()` 会返回一个元组 `(socket, address)`，其中 `address` 也是一个元组，代表了对端地址

#### 连接被动 SOCKET 对象

方法 `socket.connect(address)` 用于向被动 SOCKET 对象发起连接请求，其中 `address` 参数是被动 SOCKET 对象所在的主机地址和端口号。参数 `address` 取决于地址簇，参考 [[#创建套接字对象]]

> [!attention] 
> 
> 请注意：`socket.connect()` 方法被调用时会发起第一次挥手，并在发送第三次挥手后返回
> 

#### 流式 SOCKET IO


一对连接的流式 SOCKET 在两个端点之间提供了一个 **双向通信信道**，下图给出了 UNIX domain 的情形

![[Pasted image 20241120232734.png]]

下表列出了流式 SOCKET IO 使用的方法

| 方法                              | 描述                    |
| :------------------------------ | :-------------------- |
| `socket.recv(bufsize[, flags])` | 从接收缓冲区拷贝 `bufsize` 字节 |
|                                 |                       |
| `socket.send(bytes[, flags])`   | 将 `bytes` 对象拷贝到发送缓冲区  |

> [!tip] 
> 
> 参数 `bufsize` 是要从接收缓冲区拷贝的最大字节数。
> 
> 参数 `flags` 用于控制 SOCKET 的特定 IO 特性。参考 [[Linux 系统编程：SOCKET 高级主题]]
> 

#### 连接终止

终止一个流式 SOCKET 连接的常见方式是调用 `socke.close()`。如果多个文件描述符引用了同一个 SOCKET，那么当所有描述符被关闭之后连接就会终止

> [!tip] 和关闭文件的语义是一样的：关闭只是减少 SOCKET 的引用计数

### 数据报 SOCKET

下图显示了数据报 SOCKET 的相关系统调用的使用

![[Pasted image 20241120233738.png]]

#### 数据报 SOCKET IO

方法 `socket.recvfrom(bufsize[, flags])`和 `socket.sendto(bytes, flags, address)` 在一个数据报 SOCKET 上接收和发送数据报


> [!tip] 参数 `flags`：控制 SOCKET 特定的 IO 的特性
> 
> 这些特性会在 [[Linux 系统编程：SOCKET 高级主题]] 中进行介绍。如果无需使用其中任何一种特性，那么可以将 `flags` 指定为 $0$。
> 

## SOCKET 地址转换

### 网络字节序

在 [[计算机组成：信息存储#寻址和字节序]] 中介绍了字节序：**大端字节序** 和 **小端字节序**。下图展示了 $2$ 字节和 $4$ 字节整数的大端和小端字节序

![[Pasted image 20241121174150.png]]

**IP 地址和端口号是整数值**。在将这些值在网络中传递时碰到的一个问题是 **不同的硬件结构会以不同的顺序来存储一个多字节整数的字节**

由于端口号和 IP 地址必须在网络中的所有主机之间传递并且需要被它们所理解，因此必须要使用一个标准的字节序。这种字节序被称为 **网络字节序**，它是 **大端的**

在程序中使用 IP 地址和端口号时，它们是按照 **主机字节序** 在内存中存储。因此，需要将它们转换为网络字节序，才能正确使用它们。

> [!attention] 
> 
> 请注意，进行字节序转换的工作，Python 已经帮我们完成了
> 
> 如果有需要，参考 [[Linux 系统编程：SOCKET 地址转换#网络字节序]]，其中介绍了相关的系统调用函数，Python 在标准库 `socket` 中提供了对应的函数
> 

### IP 地址格式转换函数

`socket.inet_pton()` 和 `socket.inet_ntop()` 函数允许在 IPv4 和 IPv6 地址的 **二进制形式** 和**点分十进制表示法(IPv4)** 或 **十六进制字符串表示法(IPv6)** 之间进行转换

上述两个函数名中的 `p` 表示 **展现(presentation)**，`n` 表示 **网络(network)**。其中，展现形式是人类可读的字符串，例如
+ `204.152.189.116`（IPv4 点分十进制地址）
+ `::1`（IPv6 冒号分隔的十六进制地址）
+ `::FFFF:204.152.189.116`（IPv4 映射的 IPv6 地址）

> [!attention] 
> 
> 请注意：Python 内部帮我们完成了转换，通常我们不需要自行调用这两个函数。
> 
> 更多内容，参考 [[Linux 系统编程：SOCKET 地址转换#IP 地址格式转换函数]]
> 

### 域名系统

IP 地址太过繁琐，难以记忆和使用，因此互联网支持使用 **主机名称(host name)** 来识别包括客户机和服务器在内的主机。为了使用 TCP 和 IP 等协议，主机名称通过 **名称解析** 的过程转换成 IP 地址

> [!tip] 名称解析：查询 IP 地址好比查询通讯录一样
> 
> 比如我们打电话的时候，必须要知道对方的电话号码，但由于电话号码难以记忆，所以通常我们会将对方 **电话号 + 姓名** 保存在通讯录里
> 

所以，有⼀种服务器就专⻔保存了 Web  服务器域名与 IP  的对应关系，它就是  **DNS  服务器**

> [!tip] DNS 服务器的 IP 是在配置网络时就知晓的
> 
> 本地 DNS 服务器通常由路由器充当
> 

DNS 中的域名都是用句点来分隔的，比如 `www.server.com `，这里的句点代表了不同层次之间的界限。在域名中，**越靠右的位置表示其层级越⾼**

> [!tip]
> 
> **实际上域名最后还有⼀个点**，比如 `www.server.com.` ，这个最后的⼀个点代表根域名
> 

也就是， `.`  根域是在最顶层，它的下⼀层就是 `.com`  顶级域，再下面是 `server.com`。所以域名的层级关系类似⼀个树状结构。DNS 服务器也是这样的树状结构

+ 根 DNS 服务器（.）
+ 顶级域 DNS 服务器（.com）
+ 权威 DNS 服务器（server.com）

![[Pasted image 20241123195028.png]]

> [!tip]
> 
>根域的 DNS 服务器信息保存在互联网中所有的 DNS 服务器
>
>这样⼀来，任何 DNS 服务器就都可以找到并访问根域 DNS 服务器了
>
>因此，客户端只要能够找到任意⼀台 DNS 服务器，就可以通过它找到根域 DNS 服务器，然后再⼀路顺藤摸瓜找到位于下层的某台目标 DNS 服务器
>

下图展示 DNS 域名解析的过程

![[Pasted image 20241122102938.png]]


### 服务名

在 [[网络：链路层协议#地址#端口]] 中介绍那样，众所周知的端口号是由 IANA 集中注册的，其中 **每个端口都有一个对应的服务名**

> [!tip] 
> 
> 由于服务号是集中管理并且不会像 IP 地址那样频繁变化，因此没有必要采用 DNS 服务器来管理它们
> 

端口号和服务名会记录在文件 `/etc/services` 中

### 协议无关的地址解析函数

#### getaddrinfo() 函数

给定一个**主机名(域名)** 和 **服务器名**，`socket.getaddrinfo(host, port, family=AF_UNSPEC, type=0, proto=0, flags=0)` 函数返回一个 SOCKET 地址结构列表，每个结构都包含一个地址和端口号

> [!tip] 
> 
> 参数 `host` 是 **域名** 或者 **IP 地址的展现形式** 
> 
> 参数 `port` 是服务名或者端口号
> 
> 参数 `family` 指定通信域，即协议族
> + `AF_INET`：IPv4 地址
> + `AF_INET6`：IPv6 地址
> + `AF_UNSPEC`：任何地址，包括 IPv4 和 IPv6 地址
> 
> 参数 `type`: 指定 SOCKET 类型
> + `SOCK_STREAM`：流式 SOCKET 
> + `SOCK_DGRAM`：数据报 SOCKET
> 
> 参数 `proto`: 指定 SOCKET 类型。通常为 $0$
> 
> 参数 `flags` 设置查询标志
> + `AI_PASSIVE`：查询通配地址，通常用于服务器
> 	+ 如果 `host` 为 NULL，则该标志得到的 SOCKET 地址可用于 `bind()` 操作
> + `AI_CANONNAME`：请求返回主机的规范名称
> 	+ `host` 必须是一个合法的 IP 地址展示形式
> 	+ 禁止主机名解析，避免使用 DNS
> + `AI_NUMERICSERV`：强制 `service` 参数为数字端口号字符串
> 	+ 参数 `service` 必须是数字字符串
> 	+ 禁止服务名解析
> + `AI_V4MAPPED`：在仅支持 IPv6 的环境中，生成 IPv4 映射的 IPv6 地址
> 	+ 在支持 IPv6 的程序中兼容 IPv4
> 	+ 如果指定了 `AI_V4MAPPED` 且 `ai_family` 为 `AF_INET6`，但没有找到 IPv6 地址，则返回 IPv4 映射地址
> + `AI_ALL`：返回 IPv4 和 IPv6 地址
> 	+ 必须与 `AI_V4MAPPED` 一起使用
> + `AI_ADDRCONFIG`：仅返回本地系统可用的地址
> 	+ 本地系统未配置 IPv4，则不会返回 IPv4 地址
> 	+ 本地系统未配置 IPv6，则不会返回 IPv6 地址
>

该函数会返回一个列表，其中的元素时一个 $5$ 元组：`(family, type, proto, canonname, sockaddr)`
+ `family` 会被设置为 `AF_INET` 或 `AF_INET6`
+ `type` 会被设置为 `SOCK_STREAM` 或 `SOCK_DGRAM`
+ `proto` 会被设置为 `family` 和 `type` 匹配的协议号
+ `canoname` 在 `flag` 设置了  `AI_CANONAME` 是，该参数有效，表示 `host` 的规范名称的字符
+ `sockaddr` 是一个描述套接字地址的元组

> [!tip] 
> 
> 在这些元组中，`family`, `type`, `proto` 都是整数且其作用是被传给 `socket()` 函数
> 
> `sockaddr` 传递给 `socket.bind()` 方法
> 

#### getnameinfo() 函数

`socket.getnameinfo(sockaddr, flags)` 函数是 `socket.getaddrinfo()` 的逆函数。给定一个 SOCKET 地址结构（IPv4 或 IPv6），它会返回一个包含对应的主机和服务名的字符串或者在无法解析名字时返回一个等价的数值。

> [!tip] 参数 `flags`：指定 `getnameinfo()` 的行为
> 
> 该参数的取值是如下常量的位或
> + `NI_DGRAM`：获取数据报 SOCKET 服务名
> + `NI_NAMEREQD`：无法解析主机名，返回错误 `EAI_NONAME`
> + `NI_NOFQDN`：主机位于局域网时，返回名字的第一部分
> + `NI_NUMERICHOST`：强制在 `host` 中返回一个数值地址字符串
> + `NI_NUMERICSERV`：强制在 `service` 中返回一个十进制端口号字符串
> 

## 示例

在下面的示例中，我们将实现一个群聊服务器及其客户端。

### 版本1：服务单个客户端的回显服务器

