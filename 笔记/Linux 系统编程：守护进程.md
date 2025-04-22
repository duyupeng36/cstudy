# 守护进程

所谓 **守护进程(daemon)**，就是在 **默默运行在后台的进程**，也称作 **后台服务进程**，通常守护进程的命名会以`d` 结尾。

> [!tip] 
> 
> 要了解守护进程的概念，需要前置知识 [[Linux 系统编程：进程间关系]]
> 

守护进程具备以下特征

> [!important] 
> 
> + **生命周期很长**：可能是在系统启动时创建，一直运行到系统被关闭
> + 它在后台运行并且 **不拥有控制终端**。确保了内核永远不会为 **daemon** 自动生成任何任务控制信号以及终端相关的信号
> 

daemon 是用来执行特殊任务的

+ `cron`：一个在规定时间执行命令的 daemon
+ `sshd`：安全 `shell daemon`，允许在远程主机上使用一个安全的通信协议登录系统
+ `httpd`：HTTP 服务器 daemon，它用于服务 Web 页面。
+ `inetd`：Internet 超级服务器 daemon，它监听从指定的 TCP/IP 端口上进入的网络连接并启动相应的服务器程序来处理这些连接

很多标准的 daemon 会作为特权进程运行。因此，编写 daemon 程序时应该遵循一些指南

## 创建一个 daemon

要变成守护进程，一个程序需要完成下面的步骤

> [!tip] 第一步：调用 `fork()` 创建子进程，然后父进程退出，子进程继续执行
> 
> 父进程退出，导致子进程的父进程变成了 `init` 进程
> 
> 执行这一步的原因
> 
> + 假设 daemon 是从命令行启动的，父进程的终止会被 shell 发现，shell 在发现之后会显示出另一个 shell 提示符并让子进程继续在后台运行
> + **子进程被确保不会成为一个进程组首进程**，因为它从其父进程那里**继承了进程组 ID** 并且 **拥有了自己的进程 ID**，而这个进程 ID 与继承而来的进程组 ID 是不同的
> 

> [!tip] 第二步：子进程调用 `setsid()` 开启一个新会话并释放它与控制终端之间的所有关联关系
> 

> [!tip] 第三步：确保守护进程没有控制终端
> 
> 第二步保证了子进程没有控制终端。然而，如果 daemon 后续如果需要打开终端设备，就需要采取一些措施确保这个终端设备不会成为控制终端
> + 在所有可能应用到一个终端设备上的 `open()` 调用中指定 `O_NOCTTY` 标记
> + 在 `setsid()` 调用之后执行第二个 `fork()`，然后再次让 **父进程退出** 并让 **孙子进程继续执行**。这样，确保了子进程不会成为会话组组长，进程永远不会请求一个控制终端。参考 [[Linux 系统编程：进程间关系#控制终端和控制进程]]，**只有会话组组长首次打开终端时，才会成为控制终端**
>

> [!tip] 第四步：清除进程的 `umask` 值
> 
> 以确保当 daemon 创建文件和目录时拥有所需的权限
> 

> [!tip] 第五步：修改进程的当前工作目录
> 
> 通常会改为根目录(`/`)。这样做是有必要的，因为 daemon 通常会一直运行直至系统关闭为止。如果 daemon 的当前工作目录为不包含 `/` 的文件系统，那么就无法卸载该文件系统
> 
> 或者 daemon 可以将工作目录改为完成任务时所在的目录或在配置文件中定义的一个目录，只要包含这个目录的文件系统永远不会被卸载即可。如 `cron` 会将自身放在 `/var/spool/cron` 目录下
> 

> [!tip] 第六步：关闭 daemon 从其父进程继承而来的所有打开文件描述符
> 
> daemon 可能需要保持继承而来的文件描述的打开状态，因此这一步是 **可选的** 或者是 **可变更的**
> 
> 由于 **daemon 失去了控制终端并且是在后台运行的**，因此让 daemon 保持 $\text{fd 0}, \text{fd 1}$ 和 $\text{fd 2}$ 的打开状态毫无意义，因为它们指向的就是控制终端。此外，**无法卸载长时间运行的 daemon 打开的文件所在的文件系统**。因此，通常的做法是关闭所有无用的打开着的文件描述符，因为文件描述符是一种有限的资源。

> [!tip] 第七步：将 $\text{fd 0}, \text{fd 1}, \text{fd 2}$ 指向打开的 `/dev/null` 
> 
> 执行这一步的原因如下
> + 它确保了当 daemon 调用了在这些描述符上执行 I/O 的库函数时不会出乎意料地失败
> + 它防止了 daemon 后面使用 $\text{fd 1}$ 或 $\text{fd 2}$ 打开一个文件的情况，因为库函数会将这些描述符当做标准输出和标准错误来写入数据
> 

## syslog 记录日志

在编写 daemon 时碰到的一个问题是如何显示错误消息。由于 daemon 是在后台运行的，因此通常无法像其他程序那样将消息输出到关联终端上。这个问题的一种解决方式是 **将消息写入到一个特定于应用程序的日志文件中**

这种方式存在的一个主要问题是让系统管理员管理多个应用程序日志文件和监控其中是否存在错误消息比较困难，`syslog` 工具就用于解决这个问题

`syslog` 工具提供了一个 **集中式日志工具**，系统中的所有应用程序都可以使用这个工具来记录日志消息

syslog 工具有两个主要组件：`syslogd daemon` 和 `syslog` 库函数。如下图提供了 `syslog` 工具的概览

![[Pasted image 20241105121024.png]]

上图中的 `syslogd` 从两个不同的源接收日志消息：一个是 UNIX domain socket  和 Internet domain socket

每条由 `syslogd` 处理的消息都具备几个特性，其中包括一个 `facility`，它指定了产生消息的 **程序类型**；还有一个是 `level`，它指定了消息的 **严重程度(日志等级)**。syslogd daemon 会检查每条消息的 `facility` 和 `level`，然后根据一个相关配置文件`/etc/syslog.conf` 中的指令将消息传递到几个可能目的地中的一个

**任意进程都可以使用 `syslog()` 库函数来记录消息**。这个函数会使用传入的参数以标准的格式构建一条消息，然后将这条消息写入 `/dev/log socket` 以供 `syslogd` 读取

`/dev/log` 中的消息的另一个来源是 Kernel Log daemon klogd，它会收集内核日志消息，内核使用 `printk()` 函数生成的消息。然后使用 `syslog()` 库函数将它们写入 `/dev/log`

> [!tip] 
> 
> 内核日志消息的收集可以通过两个等价的 Linux 特有的接口中的一个来完成，即 `/proc/kmsg` 文件和 `syslog`(`man 2 syslog`) 系统调用
> 

### syslog API

syslog API 由以下三个主要函数构成

> [!tip] `openlog()`为 `syslog()` 建立默认设置
> 
> 如果省略了 `openlog()` 调用，首次调用 `syslog()`  时会使用默认设置来建立日志记录工具的连接
> 

> [!tip] `syslog()` 记录一条日志消息

> [!tip] `closelog()` 拆除与日志之间的连接

`openlog()` 函数的调用是可选的，它建立一个到系统日志工具的连接并为后续的  `syslog()` 调用设置默认设置

```c
#include <syslog.h>

void openlog(const char *ident, int log_options, int facility);
```

> [!tip] 参数 `ident`：字符指针
> 
> `syslog()` 会保证消息中包含 `ident` 指向的字符串
> 

> [!tip] 参数 `log_options`：下面常量的位或
> 
> + `LOG_CONS`: 当向系统日志发送消息发生错误时将消息写入到系统控制台`/dev/console`
> + `LOG_NDELAY`: 立即打开到日志系统的连接
> + `LOG_ODELAY`：延迟连接(默认)，首次调用 `syslog()` 时才打开到日志系统的连接
> + `LOG_NOWAIT`: 不要 `wait()` 被创建来记录日志消息的子进程
> + `LOG_PERROR`: 将消息写入标准错误和系统日志
> + `LOG_PID`: 在每条消息中加上调用者的进程 ID
> 

> [!tip] 参数 `facility` ：指定了后续的 `syslog()` 调用中使用的默认的 `facility` 值
> 
> 下表给出了 `openlog()` 的 `facility` 值和 `syslog()` 的 `priority` 参数
> 
> ![[Pasted image 20241105144415.png]]
> 
> + `LOG_KERN` facility 值用于内核消息。用户空间的程序是无法用这个工具记录日志消息的。`LOG_KERN` 常量的值为 $0$。如果在 `syslog()` 中使用，代表了使用 **默认级别**
> 

要写入一条日志消息可以调用 `syslog()`

```c
#include <syslog.h>

void syslog(int priority, const char *format, ...);
```

> [!tip] 参数 `priority`： `facility` 值和 `level` 值的 位或值
> 
> `facility` 表示记录日志消息的应用程序的类别，取值与 `openlog` 中的取值一样
> 
> 如果省略了这个参数，那么 `facility` 的默认值为前面一个 `openlog()` 调用中指定的 `facility` 值，或者当那个调用中也省略了 `facility` 值的话为 `LOG_USER`。
> 
> `level` 表示消息的严重程度取值入下表
> 
> ![[Pasted image 20241105145420.png]]
> 

当完成日志记录之后可以调用 `closelog()` 来释放分配给 `/dev/log socket` 的文件描述符

```c
#include <syslog.h>

void closelog(void);
```

> [!tip] 
> 
> 由于 daemon 通常会持续保持与系统日志之间的连接的打开状态，因此通常会省略对 `closelog()` 的调用
> 

`setlogmask()` 函数设置了一个能过滤由 `syslog()` 写入的消息的掩码

```c
#include <syslog.h>

int setlogmask(int mask_priority);
/* 返回上一个 mask_priority */
```
