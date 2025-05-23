# 进程间关系

目前我们学习了如果创建进程和执行程序，我们首先探讨 **进程间关系** ：进程组和会话

> [!tip] 
> 
> **进程组** 和 **会话** 在进程之间形成了一种 **两级层次关系**
>+ 进程组是一组相关进程的集合
>+ 会话是一组相关进程组的集合
> 

## 进程组

**进程组** 由一个或多个共享同一 **进程组标识符** 的进程组成，即 **进程组 ID(PGID)**。PGID 是一个 `pid_t` 类型的整数

> [!important] 
> 
> 进程组拥有一个 **首进程**，该进程是创建组的进程，**其 PID 就是该组的 PGID**。新进程会继承其父进程所属的进程组的 PGID
> 
> 
> 

进程组拥有一个生命周期，其开始时间为首进程创建组的时刻，结束时间为最后一个成员进程退出组的时刻。**进程组 _首进程_ 无需是最后一个离开进程组的成员**

> [!tip] 退出进程组条件
> + 终止
> + 加入其他组
> 

每个进程都拥有一个以数字表示的 **进程组 ID**，表示 **该进程所属的进程组**。新进程会继承其父进程的进程组 ID，使用 `getpgrp()` 能够获取一个进程的进程组 ID

```c
#include <unistd.h>

pid_t getpgrp(void);
/* 总是成功返回当前进程的 PGID */
```

> 如果 `getpgrp()` 的返回值与调用进程的 PID 匹配的话就说明该调用进程是其进程组的首进程

`setpgid()` 系统调用将进程 ID 为 `pid` 的进程的进程组 ID 修改为 `pgid`。

```c
#include <unistd.h>

int setpgid(pid_t pid, pid_t pgid);
/* 成功返回 0; 失败返回 -1 */
```

> [!tip] 
> + 如果将 `pid` 的值设置为 $0$，那么调用进程的进程组 ID 就会被改变
> + 如果将 `pgid` 的值设置为 $0$，那么 ID 为 `pid` 的进程的进程组 ID 会被设置成 `pid` 的值
> + 如果 `pid` 和 `pgid` 参数指定了同一个进程，那么就会创建一个新进程组，并且指定的进程会成为这个新组的首进程
> + 如果两个参数的值不同，那么 `setpgid()` 调用会将一个进程从一个进程组中移到另一个进程组中

```c
setpgid(0, 0);
setpgid(getpid(), 0); 
setpgid(getpid(), getpid());
```

通常调用 `setpgid()` 以及后续介绍的 `setsid()` 的进程是 `shell` 或 `login`

> 一个程序在使自己变成 [[Linux 系统编程：守护进程]](`daemon`) 的过程中也会调用 `setsid()`

在调用 `setpgid()` 时存在以下限制

> [!warning] 
> 
> + `pid` 参数可以仅指定调用进程或其中一个子进程。违反这条规则会导致 `ESRCH` 错误
> + 在组之间 **移动进程** 时，**调用进程**、由 **pid 指定的进程** 以及 **目标进程组** 必须要属于 **同一个会话**
> + `pid` 参数所指定的进程 **不能是会话首进程**。违反这条规则会导致 `EPERM` 错误
> + 一个进程在其子进程已经执行 `exec()` 后就无法修改该子进程的进程组 ID 了
> 

> [!summary] 总结
> 
> + 进程组：共享同一个进程组 ID 的进程集合
> + 子进程会继承父进程的 PGID
> + 当使用 shell 运行程序时候，被创建进程是 shell 的子进程。然后，这个进程将会创建一个进程组，再使用 `fork` 派生的进程都属于这个进程组
> + 进程的子进程在执行 `exec()` 后无法修改子进程的 PGID
> 

## 会话

**会话** 是一组 **进程组的集合**。进程的会话成员关系是由其 **会话标识符(SID)** 确定的，SID 是一个 `pid_t` 类型的整数

> [!important] 
> 
> 会话 **首进程** 是创建该新会话的进程，**其进程 ID 会成为会话 ID**。新进程会继承其父进程的会话 ID
> 

`getsid()` 系统调用会返回 `pid` 指定的进程的会话 ID

```c
#include <unistd.h>

pid_t getsid(pid_t pid);
/* 返回指定进程的 SID; 失败返回 (pit_t) -1 */
```

> [!tip] 参数 `pid`：指定获取哪个进程的 SID
>  
>  如果 `pid` 的值为 $0$，那么 `getsid()` 会返回调用进程的会话 ID
>  

系统调用 `setsid()` 用于创建一个新会话

```c
#include <unistd.h>

pid_t setsid(void);
/* 成功返回新会话的 SID；错误返回 (pid_t) -1 */
```

`setsid()` 系统调用会按照下列步骤创建一个新会话
+ **调用进程成为新会话的首进程** 和该会话中新进程组的首进程组
	+ 调用进程的进程组 ID和会话 ID 会被设置成该进程的进程 ID
+ 调用进程没有控制终端。所有之前到控制终端的连接都会被断开


> [!attention] 调用 `setsid()` 的进程不是进程组首进程
> 
> 如果调用进程是一个进程组首进程，那么 `setsid()` 调用会报出 `EPERM` 错误
> 
> 避免这个错误发生的最简单的方式是执行一个 `fork()` 并让父进程终止以及让子进程调用 `setsid()`。由于子进程会继承其父进程的进程组 ID 并接收属于自己的唯一的进程 ID，因此它无法成为进程组首进程。
> 

> [!tip] 限制进程组首进程调用 `setsid()` 的原因
> 
> 如果没有这个约束的话，**进程组组长就能够将其自身迁移至新的会话中** 了，而该 **进程组的其他成员则仍然位于原来的会话中**。
> 
> 不会创建一个新进程组，因为根据定义，进程组首进程的进程组 ID 已经与其进程 ID 一样了
> 
> 这会破坏会话和进程组之间严格的两级层次，因此一个 **进程组的所有成员必须属于同一个会话**
> 

### 控制终端和控制进程

**终端** 是登录到 Linux 操作系统所需要的 **入口设备**。终端可以是本地的，也可以是远程的。当操作系统启动的时候，`init` 进程会创建子进程并使用 `exec` 来执行 `getty` 程序，从而打开终端设备或者等待远程登录，然后再使用 `exec` 调用 `login` 程序验证用户名和密码

**同一个会话中的所有进程共享单个 _控制终端_**。会话被创建出来时是没有控制终端的。控制终端会在会话首进程首次打开一个终端设备时被建立。

> [!attention] 会话首进程打开终端成为控制终端的条件
> 
> 被会话首进程 **首次打开的终端** ，并且此时 **尚未成为其他会话的控制终端**，因为一个终端最多可能会成为一个会话的控制终端
> 
> 如果不需要在首次打开终端时称为控制终端，在调用 `open()` 时指定 `O_NOCTTY`  标记
> 

控制终端会被由 `fork()` 创建的子进程 **继承** 并且在 `exec()` 调用中得到 **保持**

当到控制终端的连接建立起来之后，**会话首进程会成为该终端的 _控制进程_**。成为 **控制进程的主要标志** 是当 **断开与终端之间的连接** 时内核会向该进程发送一个 `SIGHUP` 信号

如果一个进程拥有一个控制终端，那么打开特殊文件 `/dev/tty` 就能够获取该终端的文件描述符。这对于一个程序在标准输入和输出被 **重定向之后** 需要 **确保自己确实在与控制终端进行通信** 是很有用的。

> [!tip] 
> 
> 进程在拥有控制终端时，打开特殊文件 `/dev/tty` 才能成功，当 **进程与控制终端的关联被删除**(调用`iocntl(fd, TIOCNOTTY)`)后，再试图打开 `/dev/tty` 文件的话就会失败
> 
> 如果调用 `iocntl(fd, TIOCNOTTU)` 是会话终端的控制进程，当控制进程终止后
> + 会话中的所有进程将会失去与控制终端之间的关联关系
> + 内核会向前台进程组的所有成员发送一个 `SIGHUP` 信号（和一个 `SIGCONT` 信号）来通知它们控制终端的丢失
> 

**对于交互式登录来讲，控制终端是用户登录的途径**。登录 shell 是 **会话首进程** 和终端的 **控制进程**，也是其自身进程组的唯一成员。

从 shell 中发出的每个命令或通过管道连接的一组命令都会导致一个或多个进程的创建，并且 shell 会把所有这些进程都放在一个新进程组中

当命令或以管道连接的一组命令以 `&` 符号结束时会在 **后台进程组** 中运行这些命令，否则就会在 **前台进程组** 中运行这些命令

> [!tip] 
> 
> 在登录会话中创建的所有进程都会成为该会话的一部分
> 

下面的 shell 会话中，涉及的进程组和控制终端如下图所示

```shell
$ echo $$  # 显示 shell 的 PID
400
$ find / 2>/dev/null | wc -l &  # 在后台进程组中创建 2 个进程
[1] 659
$ sort < longlist | uniq -c  # 在前台进程组中创建 2 个进程
```

![[Pasted image 20241104140920.png]]


> [!important] 
> 
> 在窗口环境中，控制终端是一个 **伪终端**。**每个终端窗口都有一个独立的会话**，窗口的启动 shell 是会话首进程和终端的控制进程
> 

### 前台和后台进程组

在任一时刻，会话中的其中一个进程组会成为终端的 **前台进程组**，其他进程组会成为 **后台进程组**。

只有前台进程组中的进程才能从控制终端中读取输入。并且一个会话中，只有一个前台进程组

> [!tip] 
> 
> 当用户在控制终端中输入其中一个 **信号** 生成终端字符之后，该信号会被发送到前台进程组中的所有成员
> 
> + `Control-C` 产生 `SIGINT` 信号
> + `Control-\` 产生 `SIGQUT` 信号
> + `Control-z` 产生 `SIGSTP` 信号 
> 

下图展示了前台进程和后台进程之间的切换和使用的命令

![[Pasted image 20241104174131.png]]

## 作业控制

进程组和会话的主要应用场景就是 **作业控制**。

> [!tip] 进程组的其他使用
> 
> + 在特定的进程组中父进程能够等待任意子进程
> + 信号能够被发送给进程组中的所有成员
> 
