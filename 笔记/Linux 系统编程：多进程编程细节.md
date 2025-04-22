# 多进程编程细节

对 [[Linux系统编程：进程创建]] [[Linux 系统编程：进程终止]] [[Linux 系统编程：监控子进程]] [[Linux 系统编程：程序执行]] 四部分内容的拓展

## 进程记账

打开 **进程记账** 功能后，内核会在每个进程终止时将一条记账信息写入 **系统级的进程记账文件**。这条账单记录包含了内核为该进程所维护的多种信息，包括 **终止状态** 以及 **进程消耗的CPU 时间**

> [!tip] 分析记账信息工具
> 
> Unix/Linux 通过了两个标准记账分析工具：`sa(8) `对账单文件进行汇总，`lastcomm(1)`则就先前执行的命令列出相关信息
> 

进程记账主要用于 **在多用户 UNIX/Linux 系统上针对用户所消耗的系统资源进行计费**。不过，如果进程的信息并未由其父进程进行监控和报告，那么就可以使用进程记账来获取

> [!tip] 
> 
> 父进程可以通过 `waitpid()` `waitid()` 和 `wait3()` `wait4()` 获取子进程的记账信息
> 
> 如果父进程没有监控和报告，则可以使用 **进程记账** 来获取
> 

**SUSv3 并未对 进程记账 进行规范**：账单记录的格式、记账文件的位置也随系统实现的不同而多少存在差别

> Linux 系统的进程记账功能属于可选内核组件，可以通过 `CONFIG_BSD_PROCESS_ACCT` 选项进行配置

### 打开或关闭记账

**特权进程** 可利用系统调用 `acct()` 来打开和关闭进程记账功能

```c
#include <unistd.h>

int acct(const char *_Nullable filename);
/* 成功返回 0; 失败返回 -1 */
```

> [!attention] 
> 
> **应用程序很少使用这一系统调用**。一般会将相应命令置于系统启动脚本中，在系统每次重启时开启进程记账功能
> 

> [!tip] 参数 `acctfile`：指定进程记账的文件路径名
> 
> 通常的路径名是 `/var/log/pacct` 或 `/usr/account/pacct`
> 

下面的例程展示如何打开和关闭进程记账功能

```c title:procexec/acct_on.c
#include <unistd.h>

#include "base.h"

int main(int argc, char *argv[]) {
    if(argc > 2 || (argc > 1 && strcmp(argv[1], "--help") == 0)) {
        usageErr("%s [file]\n", argv[0]);
    }

    if(acct(argv[1]) == -1) {
        errExit("acct");
    }
    printf("Process accounting %s\n", (argv[1] == NULL)? "disabled": "enabled");
    exit(0);
}
```

### 进程账单记录

一旦打开进程记账功能，每当一进程终止时，就会有一条 `acct` 记录写入记账文件。`acct` 结构定义于头文件 `<sys/acct.h>` 中，具体如下

```c
typedef uint16_t comp_t;

struct acct
{
  char ac_flag;			/* 进程事件为掩码.  */
  uint16_t ac_uid;		/* Real user ID.  */
  uint16_t ac_gid;		/* Real group ID.  */
  uint16_t ac_tty;		/* Controlling terminal.  */
  uint32_t ac_btime;		/* Beginning time.  */
  comp_t ac_utime;		/* User time. 用户态占用的时间 */
  comp_t ac_stime;		/* System time. 内核态占用时间 */
  comp_t ac_etime;		/* Elapsed time.  */
  comp_t ac_mem;		/* Average memory usage. 平均内存使用量 */
  comp_t ac_io;			/* Chars transferred.  字符转换 */
  comp_t ac_rw;			/* Blocks read or written.  读或写块数 */
  comp_t ac_minflt;		/* Minor pagefaults. 页面错误 */
  comp_t ac_majflt;		/* Major pagefaults.  */
  comp_t ac_swaps;		/* Number of swaps.  交换页面 */
  uint32_t ac_exitcode;		/* Process exitcode. 退出状态 */
  char ac_comm[ACCT_COMM+1];	/* Command name. 最后执行的命令 */
  char ac_pad[10];		/* Padding bytes.  */
};
```

`ac_flag` 字段是为进程记录多种 **事件的位掩码**。下表展示了在该字段中可能出现的位。如表中所示，并非所有的 UNIX 实现都支持这些位。另有少数实现为该字段还提供了一些附加的位

| `ac_flag` | 描述                                |
| :-------: | :-------------------------------- |
|  `AFORK`  | 由 `fork()` 创建的进程，终止前并未调用 `exec()` |
|   `ASU`   | 拥有超级用户特权的进程                       |
|  `AXSIG`  | 进程因信号而终止（有些实现未支持）                 |
|  `ACORE`  | 进程产生了核心转储（有些实现未支持）                |

---

从内核 2.6.8 开始，Linux 引入了另一版本的进程记账文件以备选用，意在突破传统记账文件的一些限制。若有意使用这种被称为版本 3 的备选格式，**需要在编译内核前打开内核配置选项 `CONFIG_BSD_PROCESS_ACCT_V3

使用 v3 版本，操作进程记账时 **唯一的差别** 在于，**写入记账文件的记录格式不同**。新格式的定义如下

```c
struct acct_v3
{
  char ac_flag;			/* Flags */
  char ac_version;		/* Always set to ACCT_VERSION */
  uint16_t ac_tty;		/* Control Terminal */
  uint32_t ac_exitcode;		/* Exitcode */
  uint32_t ac_uid;		/* Real User ID */
  uint32_t ac_gid;		/* Real Group ID */
  uint32_t ac_pid;		/* Process ID */
  uint32_t ac_ppid;		/* Parent Process ID */
  uint32_t ac_btime;		/* Process Creation Time */
  float ac_etime;		/* Elapsed Time */
  comp_t ac_utime;		/* User Time */
  comp_t ac_stime;		/* System Time */
  comp_t ac_mem;		/* Average Memory Usage */
  comp_t ac_io;			/* Chars Transferred */
  comp_t ac_rw;			/* Blocks Read or Written */
  comp_t ac_minflt;		/* Minor Pagefaults */
  comp_t ac_majflt;		/* Major Pagefaults */
  comp_t ac_swaps;		/* Number of Swaps */
  char ac_comm[ACCT_COMM];	/* Command Name */
};
```

## 系统调用 clone()


系统调用 `clone()` 是 Linux 特有的系统调用，与 `fork()` 和 `vfork()` 类似，也能用于创建一个新进程。不同的是，`clone()` 在进程创建期间对步骤的控制更为精准

```c
#define _GNU_SOURCE
#include <sched.h>

 int clone(int (*fn)(void *), void *stack, int flags, void * arg, ...  /* pid_t * parent_tid, void * tls, pid_t * child_tid */ );
```

如同 `fork()`，由 `clone()` 创建的新进程几近于父进程的翻版。不同的是，子进程继续运行时不以调用处为起点，而是参数 `fn` 指定的函数入口处为起点

> [!tip] 参数 `fn`：子进程开始执行的函数起始位置
> 

> [!tip] 参数 `arg`: 函数 `fn` 需要的参数
> 
> `arg` 是无类型的指针，经过适当转换可以自由解读
> 

> [!tip] 参数 `stack`：指定函数 `fn` 的执行栈。通常执行分配内存的高地址处
> 
> `clone()` 产生的子进程可能共享父进程的内存，类似于 `vfrok()`，所以不能使用父进程的栈
> 
> 调用者必须分配一块大小合适的内存空间供子进程栈使用。在大多数硬件架构中，栈空间的增长方向是向下的，所以参数 `stack` 应当指向所分配内存块的高端
> 

> [!tip] 参数 `flags`：两重目的：存储子进程的终止信号 和 控制 `clone()` 的行为
> 
> **低字节**存放子进程的终止信号，**子进程退出时父进程将收到该信号**
> 
> + 如果克隆产生的子进程因信号而终止，父进程依然会收到 `SIGCHLD` 信号
> 
> + 该字节也可能为 $0$，这时将不会产生任何信号
> 
> 剩余字节则存放了位掩码，用于控制 `clone()` 的操作。下表给出了这些位掩码的取值
> 
> ![[Pasted image 20241105225827.png]]
> 
> 
> 

剩余参数 `parent_tid` `tls` `child_tid` 与线程实现有关

---

为等待由 `clone()` 产生的子进程，`waitpid()` ^[[[Linux 系统编程：监控子进程#waitpid()]]]、`wait3()` 和 `wait4()`  ^[[[Linux 系统编程：监控子进程#wait3() 和 wait4()]]]  的位掩码参数 `options` 可以包含如下附加值 

| 额外 `options`  | 描述                                |
| :------------ | :-------------------------------- |
| `__WCLONE`    | 只会等待 `clone()` 子进程。未设置，只会等待非克隆子进程 |
| `__WALL`      | 等待所有子进程，无论类型                      |
| `__WNOTHREAD` | 限制调用者只能等待自己的子进程                   |

> [!attention] `waitid()` 不能使用上述标志
> 


## exec() 和 fork() 对进程属性的影响

进程有多种属性，我们已经介绍了大部分内容。下面讨论 `fork()` 和 `exec()` 相关的两个问题

> [!question] 
> 
> + 当执行 `fork()` 时，子进程会继承哪些属性？
> + 当进程执行 `exec()` 时，这些属性将发生怎样的变化？
> 

下表回答了这些问题。`exec()` 列注明，调用 `exec()` 期间哪些属性得以保存。`fork()` 列则表明调用 `fork()` 之后子进程继承或共享了哪些属性

### 进程地址空间


| 进程属性   | `exec()`<br> 是否保存        | `fork()` <br>继承/共享     |
| :----- | ------------------------ | ---------------------- |
| 文本段    | 否                        | 共享                     |
| 栈段     | 否                        | 继承                     |
| 数据段和堆段 | 否                        | 继承                     |
| 环境变量   | `execle` `execve` 重写环境变量 | 继承                     |
| 内存映射   | 否                        | `MAP_NORESERVE` 标志得以继承 |
| 内存锁    | 否                        | 否                      |

### 进程标识符和凭证

| 进程属性                        | `exec()`<br> 是否保存 | `fork()` <br>继承/共享 |
| :-------------------------- | ----------------- | ------------------ |
| `PID`                       | 是                 | 否                  |
| `PPID`                      | 是                 | 否                  |
| `PGID`                      | 是                 | 继承                 |
| `SID`                       | 是                 | 继承                 |
| `RUID(real user id)`        | 是                 | 继承                 |
| `EID` 和 `SSID(save set-ID)` | 依赖于可执行文件          | 继承                 |
| 补充组 ID                      | 是                 | 继承                 |

### 文件和目录


| 进程属性     | `exec()`<br> 是否保存 | `fork()` <br>继承/共享 |
| :------- | ----------------- | ------------------ |
| 打开文件描述符  | 依赖于 `FD_CLOEXEC`  | 继承                 |
| 执行时关闭标志  | 是                 | 继承                 |
| 文件偏移     | 是                 | 共享                 |
| 文件状态标志   | 是                 | 共享                 |
| 异步 IO 操作 | 未完成的操作被取消         | 否                  |
| 目录流      | 否                 | 继承                 |

### 文件系统

| 进程属性     | `exec()`<br> 是否保存 | `fork()` <br>继承/共享 |
| :------- | ----------------- | ------------------ |
| 当前工作目录   | 是                 | 继承                 |
| 根目录      | 是                 | 继承                 |
| 文件模式创建掩码 | 是                 | 继承                 |

### 信号

| 进程属性  | `exec()`<br> 是否保存 | `fork()` <br>继承/共享 |
| :---- | ----------------- | ------------------ |
| 信号处置  | 恢复为默认处置           | 继承                 |
| 信号掩码  | 是                 | 继承                 |
| 挂起集合  | 是                 | 否                  |
| 备选信号栈 | 否                 | 继承                 |

### 定时器

| 进程属性          | `exec()`<br> 是否保存 | `fork()` <br>继承/共享 |
| :------------ | ----------------- | ------------------ |
| 间隔定时器         | 是                 | 否                  |
| `alarm()` 定时器 | 是                 | 否                  |
| POSIX 定时器     | 否                 | 否                  |
### POSIX 线程

| 进程属性       | `exec()`<br> 是否保存 | `fork()` <br>继承/共享 |
| :--------- | ----------------- | ------------------ |
| 线程         | 否                 | 复制调用 `fork` 的线程    |
| 线程可撤销状态与类型 | 否                 | 是                  |
| 互斥量与条件变量   | 否                 | 是                  |

### 资源与 CPU 时间

| 进程属性              | `exec()`<br> 是否保存 | `fork()` <br>继承/共享 |
| :---------------- | ----------------- | ------------------ |
| 资源限制              | 是                 | 是                  |
| 进程和子进程的 CPU<br>时间 | 是                 | 否                  |
| 资源使用量             | 是                 | 否                  |

### 进程间通信

| 进程属性           | `exec()`<br> 是否保存   | `fork()` <br>继承/共享 |
| :------------- | ------------------- | ------------------ |
| System V 共享内存段 | 否                   | 继承                 |
| POSIX 共享内存     | 否                   | 继承                 |
| POSIX 消息队列     | 否                   | 继承                 |
| POSIX 命名信号量    | 否                   | 共享                 |
| POSIX 未命名信号量   | 否                   | 位于共享内存区域，则共享信号量    |
| System V 信号量调整 | 是                   | 否                  |
| 文件锁            | 是                   | 继承                 |
| 记录锁            | 保护，除非设置`FD_CLOEXEC` | 否                  |
