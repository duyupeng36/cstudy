# 后端开发工程师

## 第零阶段：计算机组成

| 内容                | 描述                  |
| :---------------- | :------------------ |
| [[计算机组成：计算机模型]]   | 图灵模型和冯洛伊曼模型，存储程序概念  |
| [[计算机组成：数字系统]]    | 十进制、二进制、八进制和十六进制    |
| [[计算机组成：数据存储]]    | 位模式，无符号编码，补码编码，原码编码 |
| [[计算机组成：数据运算]]    | 逻辑运算，移位运算           |
| [[计算机组成：计算机组成原理]] | 理解程序是如何执行的          |

## 第一阶段：C 语言

### 1.1 C 语言基础

| 内容               | 描述                              |
| :--------------- | :------------------------------ |
| [[C 语言：环境安装]]    | 安装 GCC，visual studio 2022，CLion |
| [[C 语言：基本概念]]    | 变量，常量和常量表达式，类型(数据编码方式)，表达式， 语句  |
| [[C 语言：数组]]      | 一维数组，二维数组，变长数组(C99)             |
| [[C 语言：函数]]      | 定义与调用，函数原型，实际参数值，程序终止，递归，作用域    |
| [[C 语言：指针]]      | 指针概念，悬空指针和空指针，指针作为参数和返回值        |
| [[C 语言：指针与数组]]   | 指针与数组的关系，指针的算术运算                |
| [[C 语言：字符串]]     | 字符串字面值，字符串变量，字符串的输入输出，字符串处理函数   |
| [[C 语言：程序的结构]]   | 预处理器和 C 代码组织                    |
| [[C 语言：结构体]]     | 结构体，联合，枚举                       |
| [[C 语言：指针高级]]    | 动态内存分配，指向函数的指针，受限指针，弹性数组成员      |
| [[C 语言：声明]]      | 变量的属性，静态断言                      |
| [[C 语言：标准 IO 流]] | 标准 IO 函数                        |
| [[C 语言：错误处理]]    | 断言 错误码 和 非局部跳转                  |
### 1.2 数据结构与算法

| 内容        | 描述                          |
| :-------- | :-------------------------- |
| 基本数据结构    |                             |
| [[数据结构：线性表]]   | 线性表概念，链表，双链表                |
| [[数据结构：栈和队列]]  | 栈：后进先出；队列：先进先出              |
| [[数据结构：散列表]]   | 也称为哈希表，通过 `key` 直接访问元素所在的位置 |
|           |                             |
| 基本算法      |                             |
| [[算法：复杂度]] | 时间复杂度定义，常见的时间复杂度，时间复杂度的估计   |
| [[算法：排序]]  | 简单排序，希尔排序，归并排序，快速排序         |
| [[算法：查找]]  | 线性查找，二分查找                   |
|           |                             |
| 树形结构      |                             |
| [[数据结构：树与二叉树]] | 树和二叉树的概念，二叉树的分类             |
| [[数据结构：二叉查找树]] | 查找树                         |
| [[数据结构：AVL 树]] | 平衡查找树                       |
| [[数据结构：B 树]]   | 2-3树                        |
| [[数据结构：优先队列]]  | 二叉堆，堆排序                     |

## 第二阶段：Linux 系统

### 2.1 命令 和 vim

| 内容                | 描述                    |
| :---------------- | --------------------- |
| Linux 命令和 vim 操作  | 了解linux常用命令           |
| [[Linux：命令和 vim]] |                       |
|                   |                       |
| [[GNU：GCC]]       | GCC 生产可执行文件的步骤，及涉及的命令 |
| [[GNU：GDB]]       | 代码调试，必须会使用            |
| [[GNU：Makefile]]  | 使用 `make` 构建工具        |

### 2.2 数据和程序的表示

| 内容            | 描述                          |
| :------------ | --------------------------- |
| 理论：硬件         |                             |
| [[计算机组成：信息存储]]      | 寻址与字节序                      |
| [[计算机组成：整数的表示与运算]]  | 无符号编码和补码编码                  |
| [[计算机组成：浮点数表示与运算]]  | IEEE 754 binary 编码          |
|               |                             |
| 理论：指令体系结构     |                             |
| [[X86-64汇编：程序编码与数据格式]] | Intel x86-64 中字代表16 位       |
| [[X86-64汇编：访问信息]]      | MOV 指令；push和pop指令           |
| [[X86-64汇编：算术和逻辑操作]]   | 算术运算与逻辑运算指令                 |
| [[X86-64汇编：控制]]        | 条件寄存器，比较和测试指令，跳转指令          |
| [[X86-64汇编：过程]]        | callee saved 和 caller saved |
| [[X86-64汇编：浮点代码]]      | 浮点体系结构                      |

### 2.3 Linux 系统编程

在正式学习 Linux 系统编程之前，我们首先需要对 [[系统编程概念]] 的概念有一个简单的了解

#### 2.3.1 共享库

| 内容                     | 描述                 | 系统调用/库函数                                           |
| :--------------------- | ------------------ | -------------------------------------------------- |
| [[Linux 系统编程：库基础]]     | 制作和使用静态库和共享库       |                                                    |
| [[Linux 系统编程：共享库高级特性]] | 延迟加载库<br>符号可见性与版本化 | 打开共享库：`dlopen`<br>加载符号: `dlsym`<br>关闭共享库：`dlclose` |

#### 2.3.2 文件系统

| 内容                      | 系统调用/库函数                                                                                                                                                                              |
| :---------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| 理论：操作系统                 |                                                                                                                                                                                       |
| [[操作系统：文件系统]]           |                                                                                                                                                                                       |
| [[操作系统：文件系统的实现]]        |                                                                                                                                                                                       |
|                         |                                                                                                                                                                                       |
| Linux 系统调用              |                                                                                                                                                                                       |
| [[Linux 系统编程：文件 IO]]    | 打开和关闭：`open` `close` <br>读写：`read` `write` <br>文件指针：`lseek` <br>`ioctl`                                                                                                               |
| [[Linux 系统编程：深入文件 IO]]  | 文件描述符控制: `fcntl`<br>文件描述符复制:  `dup` `dup2` `dup3`                                                                                                                                     |
| [[Linux 系统编程：文件 IO 缓冲]] | 文件完整性:`fsync` <br>数据完整性: `fdatasync`<br>IO完整性: `sync`                                                                                                                                 |
| [[Linux 系统编程：文件属性]]     | 文件属性：`stat` `utime` `access`<br> 权限：`umask` `chmod`                                                                                                                                   |
| [[Linux 系统编程：目录与链接]]    | 硬链接：`link` `unlink`<br>符号链接：`symlink` `readlink`<br>目录：`mkdir` `rmdir`<br>删除和重命名`remove` `rename` <br>目录流：`opendir` `fdopendir` `readdir` `dirfd`<br>当前工作目录：`getcwd` `chdir` `chroot` |

#### 2.3.3 进程 用户 时间

| 内容                  | 系统调用/库函数                                                                                                                                                                                                                                  |
| :------------------ | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| 理论：操作系统             |                                                                                                                                                                                                                                           |
| [[操作系统：进程概念]]       | 进程内存结构<br>进程状态转换                                                                                                                                                                                                                          |
| [[操作系统：进程调度]]       | 先来先服务<br>短作业优先<br>最短剩余时间优先<br><br>时间片轮转<br>优先级调度<br>多级队列<br>多级反馈队列                                                                                                                                                                        |
|                     |                                                                                                                                                                                                                                           |
| Linux 系统调用          |                                                                                                                                                                                                                                           |
| [[Linux 系统编程：进程属性]] | PID和PPID: `getpid` `getppid` <br>环境变量: `getenv` `putenv` `setenv` `unsetenv` `clearenv`                                                                                                                                                   |
| [[Linux 系统编程：用户和组]] | 读密码文件：`getpwnam` `getpwuid` <br>读组文件：`getgrnam` `getgrgid` <br>读影子文件：`getspnam`<br>遍历密码文件：`getpwent` `setpwent` `getpwent`<br>遍历组文件：`getgrent` `setgrent` `endgrent`<br>遍历影子文件：`getspent` `setspent` `endspent`<br>密码验证：`crypt` `getpass` |
| [[Linux 系统编程：时间]]   | 日历时间：`gettimeofday` `settimeofday`<br>获取当前时间：`time`  `mktime` <br>分解时间: `gmtime` `localtime` `strptime`<br>打印时间: `ctime` `asctime` `strftime` <br>进程时间: `times` `clock`                                                                   |

#### 2.3.4 内存管理

| 内容          | 描述             | 系统调用/库函数                                                              |
| :---------- | -------------- | --------------------------------------------------------------------- |
| 理论：硬件       |                |                                                                       |
| [[计算机组成：存储器层次结构]] | 组织存储器，避免 IO 低效 |                                                                       |
|             |                |                                                                       |
| 理论：内存管理     |                |                                                                       |
| [[操作系统：内存管理]]    | 分段和分页          |                                                                       |
| [[操作系统：虚拟内存]]    | 虚拟内存管理/地址转换    |                                                                       |
|             |                |                                                                       |
| Linux 系统调用  |                |                                                                       |
| [[Linux 系统编程：内存分配]]    | 虚拟内存分配         | 堆上分配空间：`brk` `sbrk` <br>栈上分配空间：`alloca`                               |
| [[Linux 系统编程：内存映射]]    | 将文件映射到虚拟内存     | 建立映射：`mmap` <br>解除映射：`munmap`                                         |
| [[Linux 系统编程：虚拟内存操作]]  | 虚拟地址空间上执行系统调用  | 修改保护位：`mprotect`<br>锁定内存：`mlock` `mlockall`<br>建议：`mincore` `madvise` |

#### 2.3.5 进程管理

| 内容                    | 系统调用/库函数                                                                                                                                                                                   |
| :-------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| [[Linux 系统编程：进程凭证]]   | 获取实际ID: `getuid` `getgid`<br>获取有效ID: `geteuid` `getegid` <br>修改有效ID: `setuid` `setgid` `seteuid` `setegid`<br>文件系统ID: `setfsuid` `setfsgid`<br>辅助组ID: `getgroups` `setgroups` `initgroups` |
| [[Linux 系统编程：进程创建]]    | 写时复制创建子进程：`fork` <br>子进程使用父进程内存：`vfork`                                                                                                                                                    |
| [[Linux 系统编程：进程终止]]   | 系统调用：`_exit`<br>库函数: `exit` <br>注册退出处理程序：`atexit` `on_exit`                                                                                                                                |
| [[Linux 系统编程：监控子进程]]  | 等待子进程: `wait` `waitpid` <br>等待子进程状态变化：`waitid` <br>获取子进程资源使用情况：`wait3` `wait4`                                                                                                             |
| [[Linux 系统编程：程序执行]]   | 系统调用: `execve` <br>库函数: `execl`  `execle` `execlp`  `execv` `execvp`                                                                                                                       |
| [[Linux 系统编程：优先级和调度]] | 获取 nice 值：`getpriority`<br>设置 nice 值：`setpriority`                                                                                                                                         |
| [[Linux 系统编程：进程间关系]]  | 获取进程的 PGID: `getpgrp` `getpgid`<br>修改进程的 PGID: `setpgid`<br>获取进程的 SID: `getsid`<br>新建会话: `setsid`                                                                                          |
| [[Linux 系统编程：守护进程]]              | `openlog`<br>`syslog`<br>`closelog`                                                                                                                                                        |
| [[Linux 系统编程：多进程编程细节]]           | 克隆子进程: `clone`                                                                                                                                                                             |

#### 2.3.6 进程间通信(IPC)

| 内容                                            | 系统调用/库函数                                                                                                                                                                                      |
| :-------------------------------------------- | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| [[Linux 系统编程：进程间通信简介]]                        |                                                                                                                                                                                               |
|                                               |                                                                                                                                                                                               |
| 通信工具：管道                                       |                                                                                                                                                                                               |
| [[Linux 系统编程：PIPE]]                           | 创建 PIPE: `pipe`                                                                                                                                                                               |
| [[Linux 系统编程：FIFO]]                           | 创建 FIFO: `mkfifo`                                                                                                                                                                             |
|                                               |                                                                                                                                                                                               |
| 通信工具：System V                                 |                                                                                                                                                                                               |
| [[Linux 系统编程：System V IPC 介绍]]                | 生成 IPC key: `ftok`                                                                                                                                                                            |
| [[Linux 系统编程：System V 共享内存]]                  | 申请共享内存: `shmget`<br>附加共享内存: `shmat`<br>分离共享内存: `shmdt`<br>控制共享内存: `shmctl`                                                                                                                    |
| [[Linux 系统编程：System V 信号量]]                   | 创建信号量集: `semget`<br>控制信号量集: `semctl`<br>操作信号量: `semop` <br>`semtimedop`                                                                                                                       |
| [[Linux 系统编程：System V 消息队列]]                  | 创建消息队列: `msgget`<br>发送消息: `msgsnd`<br>接收消息: `msgrcv`<br>控制消息队列: `msgctl`                                                                                                                      |
|                                               |                                                                                                                                                                                               |
| 通信工具：POSIX                                    |                                                                                                                                                                                               |
| [[POSIX 共享内存]]                                |                                                                                                                                                                                               |
| [[POSIX 信号量]]                                 |                                                                                                                                                                                               |
| [[POSIX 消息队列]]                                |                                                                                                                                                                                               |
|                                               |                                                                                                                                                                                               |
| 通信工具：内存映射                                     |                                                                                                                                                                                               |
| [[Linux 系统编程：内存映射]] <br>[[Linux 系统编程：虚拟内存操作]] | 类似于共享内存                                                                                                                                                                                       |
|                                               |                                                                                                                                                                                               |
| 通信工具：文件锁                                      |                                                                                                                                                                                               |
| [[Linux 系统编程：文件锁]]                            | 文件加锁/解锁: `flock`<br>区域加锁/解锁: `fcntl`                                                                                                                                                          |
|                                               |                                                                                                                                                                                               |
| 通信工具：信号                                       |                                                                                                                                                                                               |
| [[Linux 系统编程：信号基本概念]]                                    | 注册处理函数: `signal`<br>发送信号: `kill` `raise`  `killpg`<br>信号描述: `strsignal`                                                                                                                       |
| [[Linux 系统编程：信号集]]                                       | 初始化信号集: `sigemptyset` `sigfillset`<br>添加或移除信号: `sigaddset` `sigdelset`<br>测试信号集: `sigismember`<br>添加/获取信号掩码: `sigprocmask`<br>获取等待信号集: `sigpending`<br>改变信号处置: `sigaction`<br>暂停等待信号: `pause` |
| [[Linux 系统编程：信号处理器函数]]                                   | 保存跳转环境: `sigsetjmp`<br>执行非局部跳转: `siglongjmp`                                                                                                                                                  |
| [[Linux 系统编程：信号高级特性]]                                    | 发送实时信号: `sigqueue`<br>等待信号: `sigsuspend` `sigwaitinfo`                                                                                                                                        |
| [[Linux 系统编程：定时器与休眠]]                                    | 创建定时器: `setitimer` `alam` <br>获取定时器状态: `getitimer`<br>暂停运行: `sleep` `nanosleep`                                                                                                               |

#### 2.3.7 多线程

| 内容                          | 系统调用/库函数                                                                                                                                                                                             |
| :-------------------------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| 理论：多线程                      |                                                                                                                                                                                                      |
| [[操作系统：多线程概念]]              |                                                                                                                                                                                                      |
|                             |                                                                                                                                                                                                      |
| Pthreads：线程管理               |                                                                                                                                                                                                      |
| [[Linux 系统编程：Pthreads API]] | 创建线程: `pthread_create`<br>线程终止:`pthread_exit`<br>线程ID: `pthread_self` `pthread_equal`<br>连接终止线程: `pthread_join`<br>线程分离: `pthread_detach`                                                            |
| [[Linux 系统编程：互斥量]]          | 互斥量初始化: `pthread_mutex_init`<br>互斥量销毁: `pthread_mutex_destroy`<br>锁定互斥量: `pthread_mutex_lock` <br>非阻塞加锁: `pthread_mutex_trylock`<br>加锁超时: `pthread_mutex_timedlock`<br>解锁互斥量: `pthread_mutex_unlock` |
| [[Linux 系统编程：条件变量]]         | 初始化条件变量：`pthread_cond_init`<br>销毁条件变量：`pthread_cond_destroy`<br>通知一个线程：`pthread_cond_signal`<br>通知所有线程：`pthread_cond_broadcast`<br>等待条件变量通知：`pthread_cond_wait`                                      |
|                             |                                                                                                                                                                                                      |
| [[Linux 系统编程：线程安全]]         | 创建key: `pthread_key_create`<br>设置线程特有数据：`pthread_setsprcific`<br>获取线程特有数据：`pthread_getsprcific`                                                                                                      |
| [[Linux 系统编程：线程取消]]         | 发送取消请求: `pthread_cancle`<br>测试取消请求: `pthread_testcancle`<br>设置取消状态: `pthread_setcanclestate`<br>设置取消类型: `pthread_setcancletype`<br>添加清理函数: `pthread_cleanup_push`<br>移除清理函数: `pthread_cleanup_pop`   |
| [[Linux 系统编程：线程细节]]         | 线程信号掩码: `pthread_sigmask`<br>向线程发送信号: `pthread_kill` `pthread_sigqueue`                                                                                                                              |

#### 2.3.8 网络编程

| 内容                         | 系统调用/库函数                                                                                                                             |
| :------------------------- | ------------------------------------------------------------------------------------------------------------------------------------ |
| 理论：TCP/IP                  |                                                                                                                                      |
| [[网络：链路层协议]]               | 以太网帧格式<br> ARP 协议                                                                                                                    |
| [[网络：网络层协议]]               | IP 地址的结构<br>Internet 协议，IP 数据报格式<br>Internet 校验和<br>路由表                                                                              |
| [[网络：传输层协议]]               | UDP 协议<br>TCP 协议，TCP 报文段格式<br>TCP 连接：三次握手<br>TCP 终止：四次挥手<br>TCP 半关闭<br>TCP 状态转换                                                      |
|                            |                                                                                                                                      |
| socket 概览                  |                                                                                                                                      |
| [[Linux 系统编程：SOCKET 概览]]   | 创建 SOCKET：`socket`<br>绑定地址：`bind`<br>监听连接：`listen`<br>接受连接：`accept`<br>连接对等套接字: `connect`                                            |
| [[Linux 系统编程：SOCKET 地址转换]] | 主网字节序: `hton{s, l}`<br>网主字节序: `ntoh{s, l}`<br>地址转换: `inet_pton` `inet_ntop`<br>地址转换: `getaddrinfo` <br>`getnameinfo` <br>            |
| [[Linux 系统编程：SOCKET 高级主题]] | 关闭一端：`shutdown`<br>发送文件：`sendfile`<br>本地套接字地址：`getsockname`<br>对端套接字地址：`getpeername`<br>获取套接字选项：`getsockopt`<br>设置套接字选项：`setsockopt` |
| [[Linux 系统编程：服务器设计]]                  |                                                                                                                                      |

#### 2.3.9 备选 IO 模型

| 内容                     | 描述      | 系统调用/库函数                                                                      |
| :--------------------- | ------- | ----------------------------------------------------------------------------- |
| [[Linux 系统编程：IO 多路复用]] | 监控多路IO  | IO多路复用：`select` `poll`                                                        |
| [[Linux 系统编程：信号驱动 IO]] | 信号驱动 IO | `fcntl(fd, F_SETSIG, sig)`                                                    |
| [[Linux 系统编程： epoll]]  | IO 多路复用 | 创建 epoll 实例：`epoll_create`<br>修改 epoll 的兴趣列表：`epoll_ctl`<br>事件等待：`epoll_wait` |

#### 2.3.10 进程池和线程池

| 内容      | 描述              |
| :------ | --------------- |
| [[Linux 系统编程：进程池]] | 基于进程池实现文件上传下载任务 |
| [[Linux 系统编程：线程池]] | 基于线程池实现文件上传下载任务 |

## 第三阶段：关系型数据库

| 内容                            | 描述                                                                                                                                                                                                                                                                     |
| :---------------------------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| [[数据库：MySQL 和 PostgreSQL 安装]] | 安装 MySQL 和 PostgreSQL                                                                                                                                                                                                                                                  |
| [[MySQL9.1-en.a4.pdf]]        | MySQL 参考文档                                                                                                                                                                                                                                                             |
| [[postgresql-17-A4.pdf]]      | PostgreSQL 参考文档                                                                                                                                                                                                                                                        |
|                               |                                                                                                                                                                                                                                                                        |
| **数据操作语言 DML**：查询             |                                                                                                                                                                                                                                                                        |
| [[数据库：查询数据]]                  | 查询：`SELECT ... FROM`<br>别名：`AS`<br>排序：`ORDER BY ...[ASC DESC]`<br>自定义列表：`FILED(value, value1,...)`<br>含NULL值：`NULLS FIRST`  `NULLS LAST`<br>去重：`DISTINCT` `DISTINCT ON`                                                                                                |
| [[数据库：过滤数据]]                  | 条件: `WHERE`<br>运算符：`AND` `OR` `NOT` <br>运算符：`LIKE` `IN` `BETWEEN` `IS NULL`<br>运算符： `LIMIT ... OFFSET..` <br>运算符：`OFFSET ... FETCH ...`                                                                                                                                |
| [[数据库：连接表]]                   | 内连接：`INNER JOIN`<br>外连接：`LEFT OUTER JOIN` `RIGHT OUTER JOIN`<br>自连接：SELF-JOIN，自选连接语法<br>全连接：`FULL OUTER JOIN`<br>笛卡尔连接：`CROSS JOIN`<br>自然连接：`NATURAL JOIN`                                                                                                             |
| [[数据库：分组与聚合]]                     | 分组：`GROUP BY`<br>过滤：`HAVING`<br>聚合函数：`SUM` `AVG` `COUNT` `MAX` `MIN`<br>分组集：`GROUPING SETS` `CUBE` `ROLLUP`                                                                                                                                                            |
| [[数据库：集合运算符]]                     | 并集：`UNION` `UNION ALL`<br>交集：`INTERSECT`<br>差集：`EXCEPT`                                                                                                                                                                                                                |
| [[数据库：子查询]]                       | 不相关子查询<br>相关子查询                                                                                                                                                                                                                                                        |
| [[数据库：CTE]]                       | 公用表表达式                                                                                                                                                                                                                                                                 |
|                               |                                                                                                                                                                                                                                                                        |
| **查询练习**                      |                                                                                                                                                                                                                                                                        |
| [[作业43：SQL 练习 Part1]]         | 基本的查询和过滤                                                                                                                                                                                                                                                               |
| [[作业44：SQL 练习 Part2]]         | 多表查询，即连接表                                                                                                                                                                                                                                                              |
| [[作业45：SQL 练习 Part3]]         | 分组与聚合                                                                                                                                                                                                                                                                  |
| [[作业46：SQL 练习 Part4]]         | 子查询                                                                                                                                                                                                                                                                    |
|                               |                                                                                                                                                                                                                                                                        |
| **数据定义语言 DDL**：库和表            |                                                                                                                                                                                                                                                                        |
| [[数据库：管理数据库]]                     | 显示数据库：`SHOW DATABASES`<br>创建数据库：`CREATE DATABASE`<br>删除数据库：`DROP DATABASE`                                                                                                                                                                                             |
| [[数据库：管理表]]                       | 创建表：`CREATE TABLE` `CREATE TABLE AS`<br>生成列约束：`GENERATE ALWAYS AS (expr)`<br>修改表：`ALTER TABLE`<br>删除表：`DROP TABLE`                                                                                                                                                     |
| [[数据库：约束]]                        | 主键约束：`PRIMARY KEY`<br>外键约束：`FOREIGN KEY`<br>CHECK 约束：`CHECK(expr)`<br>UNIQUE 约束：`UNIQUE KEY`<br>NOT NULL 约束：`NOT NULL`<br>DEFAULT 约束：`DEFAULT value/expr`                                                                                                              |
|                               |                                                                                                                                                                                                                                                                        |
| **数据操作语言 DML**：修改数据           |                                                                                                                                                                                                                                                                        |
| [[数据库：修改数据]]                      | 插入数据：`INSERT INTO tb_name VALUES (...) ...`<br>修改数据：`UPDATE tb_name SET column=value WHERE condition`<br>删除数据：`DELETE FROM tb_name WHERE condition`                                                                                                                    |
|                               |                                                                                                                                                                                                                                                                        |
| **MySQL/PostgreSQL C API**    |                                                                                                                                                                                                                                                                        |
| [[MySQL C API]]               | 初始化库: `mysql_library_init()`<br>初始化 MYSQL: `mysql_init()`<br>建立连接: `mysql_real_connect()`<br>关闭连接: `mysql_close()`<br>执行 SQL: `mysql_query()`<br>获取一行： `mysql_fetch_row()`<br>获取列元数据: `mysql_fetch_fields()`<br>获取行数: `mysql_num_rows()`<br>获取列数: `mysql_num_fields()` |
| [[PostgreSQL C API]]          |                                                                                                                                                                                                                                                                        |

## 第四阶段：C++ 


| 内容                      | 描述                                   |
| :---------------------- | ------------------------------------ |
| [[C++：与 C 的差异]]         | 名字空间 引用 命名类型转换 函数重载                  |
| [[C++：抽象机制概览]]          | 具体类 抽象类 类层次 模板                       |
| [[C++：类与对象]]            | 面向过程与面向对象概念 类基础                      |
| [[C++：具体类]]             | 讨论如何设计类：以 Date 类为例                   |
| [[C++：构造与清理]]           | 详细讨论构造函数和析构函数                        |
| [[C++：拷贝语义]]            | 拷贝构造函数和拷贝赋值运算符                       |
| [[C++：深入探究对象创建]]        | 构造/析构和 `operator new/delete` 对象创建的影响 |
| [[C++：移动语义和默认操作]]       | 移动构造和移动赋值运算符。右值才能触发移动                |
| [[C++：单例模式]]            | 单例模式的实现方式。推荐局部静态对象和 `std::call_once` |
| [[C++：string类和vector类]] | C++ 风格的字符串和动态数组                      |
| [[C++：IO 流]]            | C++ 风格的 IO 流                         |
| [[C++：友元与运算符重载]]        | 友元与常规运算符重载                           |
| [[C++：特殊运算符]]           | 下标 输入输出 函数调用 解引用  成员指针               |
| [[C++：单例对象自动释放]]        | 单例对象的自动释放策略                          |
| [[C++：string 的底层实现]]    | 写时复制  短字符串优化                         |
| [[C++：继承]]              | 继承方式 单继承 多继承(菱形继承问题)                 |
| [[C++：多态]]              | 虚函数 虚函数表 抽象类 纯虚函数 虚析构函数              |
| [[C++：模板]]              | 模板的概念 函数模板                           |
| [[C++：资源管理技术]]          | 资源管理技术：RAII                          |

## 第五阶段：Python 语言

| 内容                       | 描述                                         |
| :----------------------- | ------------------------------------------ |
| [[Python：环境搭建]]          | 安装 Python 解释器；传递参数；Python 解释器的执行环境         |
| [[Python：基本概念]]          | 数据类型 运算符 控制流程                              |
| [[Python：内建数据结构]]        | 序列(list tuple str bytes 和 bytearray) 集合 字典 |
| [[Python：推导式]]           | 列表生成式 集合生成式 字典生成式 生成器生成式                   |
|                          |                                            |
| [[Python：函数基础]]          | 函数的定义与调用 名字空间与作用域 global和nonlocal          |
| [[Python：函数高级]]          | 函数对象 匿名函数 递归 生成器函数 高阶函数                    |
| [[Python：装饰器与偏函数]]       | 装饰器 偏函数                                    |
| [[Python：内置函数]]          | 内置函数大总结                                    |
|                          |                                            |
| [[Python：文件 IO]]         | 文件对象                                       |
| [[Python：目录]]            | 目录操作                                       |
| [[Python：CLI（argparse）]] | 命令行参数解析                                    |
|                          |                                            |
| [[Python：正则表达式]]         | 介绍 re 模块中常用的函数                             |
| [[Python：序列化]]           | pickle  JSON  MessagePack Base64           |
| [[Python：异常处理]]          | `try...except...else...finaly` 语句          |
|                          |                                            |
| [[Python：类与对象]]          | 类定义 类对象 实例对象 构造与析构                         |
| [[Python：继承]]            | 单继承 多继承  C3算法(MRO) 多态                      |
| [[Python：基本魔术方法]]        | 实例化 可视化 可哈希 逻辑值                            |
| [[Python：模拟数字]]          | 模拟数字：比较运算符 算术运算符                           |
| [[Python：模拟容器]]          | 模拟 Python 内置容器的行为。实现双向链表                   |
| [[Python：模拟可调用对象]]       | 让对象可以当做函数调用                                |
| [[Python：上下文管理器]]        | 让对象支持 `with` 语句                            |
| [[Python：反射]]            | 反射 自定义属性访问                                 |
| [[Python：描述器]]           | 自定义属性访问                                    |
| [[Python：对象模型]]          | 标准库`tracemalloc`  特殊属性`__slots__`          |
|                          |                                            |
| [[Python：模块和包]]          | Python 代码的组织与管理                            |
| [[Python：包管理]]           | 管理 Python 项目的依赖                            |
| [[Python：日志处理]]          | 标准库 `logging`                              |
|                          |                                            |
| [[Python：并发模型]]          | 并发模型 和 queue                               |
| [[Python：进程]]            | 进程                                         |
| [[Python：线程]]            | 线程                                         |
| [[Python：网络编程]]          | socket 模块                                  |
|                          |                                            |
|                          |                                            |
| [[Python：元类]]            | 自定义类创建                                     |

## 第四阶段：Go 语言


| 内容                  | 描述                             |
| :------------------ | ------------------------------ |
| [[Go 语言：开发环境搭建]]    | 安装 Go 编译器和代码编辑器                |
| [[Go 语言：基本概念]]      | 变量与常量、数据类型、运算符、流程控制            |
| [[Go 语言：内建数据结构]]    | 数组 切片 Map 结构体                  |
| [[Go 语言：字符串]]       | 字符串的实现原理 strings包 strcov包      |
| [[Go 语言：函数]]        | 函数 程序结构 闭包 函数栈帧布局              |
| [[Go 语言：结构体]]       | 结构体类型 匿名成员                     |
| [[Go 语言：方法]]        | 方法 值接受者 封装                     |
| [[Go 语言：接口]]        | 接口即约定 接口变量 类型断言                |
| [[Go 语言：面向对象]]      | 面向对象的概念                        |
| [[Go 语言：泛型]]        | 泛型函数 类型集合 泛型类型                 |
| [[Go 语言：异常处理]]      | error 接口 errors包 panic和recover |
| [[Go 语言：依赖管理]]      | 包定义与导入 依赖管理                    |
| [[Go 语言：时间]]        | 时间日期的解析与格式化 时间间隔               |
| [[Go 语言：文件IO]]      | Open Read Write Close Seek     |
| [[Go 语言：通用 IO 接口]]  | 标准包 `io` 提供的接口和实用函数            |
| [[Go 语言：缓冲IO]]      | 用户态缓冲 IO                       |
| [[Go 语言：字节缓冲]]      | 用户空间字节缓冲                       |
| [[Go 语言：文件属性]]      | FileInfo接口和FileMode            |
| [[Go 语言：路径与目录]]     |                                |
| [[Go 语言：Groutines]] |                                |



## 第五阶段：Web 前端

| 内容  | 描述  |
| :-- | --- |
|     |     |
|     |     |

## 第六阶段：项目

| 项目  | 描述  |
| :-- | --- |
|     |     |
|     |     |

## 每日刷题

| 内容              | 考点   | 思路         |
| :-------------- | ---- | ---------- |
| [[刷题：HJ16 购物单]] | 动态规划 | `0-1` 背包问题 |
|                 |      |            |



## 扩展：工具

| 内容           | 描述                    |
| :----------- | :-------------------- |
| [[工具：git]]   | 分布式版本控制工具             |
| [[通用：正则表达式]] | 正则表达式语法               |

```
type $env:USERPROFILE\.ssh\id_rsa.pub | ssh dyp@47.117.190.192 "cat >> .ssh/authorized_keys"
```




