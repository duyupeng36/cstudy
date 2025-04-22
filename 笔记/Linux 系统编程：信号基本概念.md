# 信号基本概念

**信号是事件发生时对进程的通知机制。有时也称之为软件中断**。与硬件中断类似的地方就是打断程序执行的正常流程。

> [!tip] 
> 
> 在大多数情形下，进程无法预测信号到达的精确时间。信号处理是 **异步** 的
> 

进程可以向另一个进程发送信号，当然发送信号的进程必须具有合适的权限。进程甚至可以向自身发送信号

> [!tip] 进程向另一个进程发送信号，可以用作同步，甚至是 IPC
> 
> 信号的这一用法可作为一种 **同步技术**，甚至是 **进程间通信（IPC）的原始形式**
> 

> [!attention] 
> 
> **发往进程的诸多信号，通常都是源于内核**。引发内核为进程产生信号的各类事件如下
> 
> + **硬件发生异常**，即硬件检测到一个错误条件并通知内核，随即再由内核发送相应信号给相关进程
> 	+ **执行一条异常的机器语言指令**。例如，除数为零；引用无法访问的内存
> + **在会话的控制终端中键入特殊字符**，会向会话的前台进程组中的进程发送信号
> 	+ 中断字符: `Control-c`
> 	+ 暂停字符: `Control-z`
> 	+ 退出字符: `Control-\`
> + **发生了软件事件**
> 	+ 针对文件描述符的输出变为有效
> 	+ 调整了终端窗口大小
> 	+ 定时器到期
> 	+ 进程执行的 CPU 时间超限
> 	+ 进程的某个子进程退出
> 

**针对每个信号，都定义了一个唯一的（小）整数，从 $1$ 开始顺序展开**。`<signal.h>`以 `SIG<XXX>` 形式的符号名对这些整数做了宏定义

> [!attention] 
> 
> 由于 **每个信号的实际编号随系统不同而不同**，所以在程序中总是使用这些符号名
> 

信号分为两类：**标准信号** 和 **实时信号**

> [!tip] 标准信号
> 
> 内核向进程通知事件的信号。在 Linux 内核中，标准信号的编号为 $1\sim 31$
> 

信号是因某些事件而产生。信号产生后，会于 **稍后** 被传递给某一进程，而进程也会采取某些措施来响应信号

> [!important] 信号何时送达进程？
> 
> 在信号产生时和传递给某个进程时，这两个时间点之间，信号处于 **等待状态**
> 
> ![[Pasted image 20241108165915.png]]
> 
> 信号通过在下面两个时刻送达进程
> + **内核决定调度该进程执行时**，等待状态的信号立即送达
> + **进程正在执行时**，信号立即送达
> 

有时候需要 **确保一段代码不会被传递过来的信号中断**。为了做到这一点，采用 **信号掩码** 技术，只要 **将需要屏蔽的信号添加到进程的信号掩码中**，该进程就不会指定信号中断

> [!tip] 
> 
> 如果所产生的信号属于阻塞（信号在信号掩码中）之列，那么信号将保持等待状态，直至稍后对其解除阻塞（从信号掩码中移除）
> 

> [!hint] 信号到达进程后，进程视具体信号执行如下 **默认操作** 之一
> + 忽略信号：内核丢弃信号，信号不会对进程产生任何影响，进程甚至不知道出现过该信号
> + 终止进程：进程被异常终止
> + 生成核心转储文件并终止进程
> + 停止进程：暂停进程的执行
> + 恢复进程：暂停的进程被恢复
> 

程序可以变化信号的到达进程时的默认行为，这称为 **信号处置设置**。程序可以将信号的处置设置为如下情形之一
+ **采取默认行为**：适用于撤销信号的处置
+ **忽略信号**：适用于默认信号时终止进程的信号
+ **执行信号处理程序**：适用于对某种信号的特殊处理

> [!important] **信号处理器程序**：由程序员编写的函数，**用于为响应传递来的信号而执行适当任务**
> 
> + 通知内核应当去调用某一处理器程序的行为，通常称之为 **安装** 或者 **建立信号处理器程序**
> 
> + 调用信号处理器程序以响应传递来的信号，则称之为 **信号已处理**（handled），或者 **已捕获**（caught）
> 


> [!attention] 注意
> 
> 请注意，**无法将 _信号处置_ 设置为 _终止进程_ 或者 _转储核心_**，除非这是对信号的默认处置
> 
> **也就是在信号的处理程序，并于其中调用 `exit()` 或者 `abort()`**。`abort()` 函数为进程产生一个 `SIGABRT` 信号，该信号将引发进程转储核心文件并终止
> 

## 信号类型和默认行为

**Linux 对标准信号的编号为 $1\sim 31$**。然而，Linux 于 `signal(7)` 手册页中列出的信号名称却超出了 $31$ 个。名称超出的原因有多种。**有些名称只是其他名称的同义词**，之所以定义是为了与其他 UNIX 实现保持源码兼容。其他名称虽然有定义，但却并未使用

以下列表介绍了各种信号：详细可参考 `man 7 signal`

| 名称          | 信号值                                         | 描述                  | 默认处置 |
| :---------- | :------------------------------------------ | ------------------- | ---- |
| `SIGABRT`   | $6$                                         | `abrot()` 函数        | 核心转储 |
| `SIGBUS`    | $7(\text{SAMP}=10)$                         | 内存访问错误              | 核心转储 |
| `SIGFPE`    | $8$                                         | 算术异常                | 核心转储 |
| `SIGILL`    | $4$                                         | 非法指令                | 核心转储 |
| `SIGQUIT`   | $3$                                         | 终端退出信号`ctrl-\`      | 核心转储 |
| `SIGSEGV`   | $11$                                        | 无效内存访问              | 核心转储 |
| `SIGSYS`    | $31(\text{SAMP}=12)$                        | 无效系统调用              | 核心转储 |
| `SIGTRAP`   | $5$                                         | 实现断点调试              | 核心转储 |
| `SIGXCPU`   | $24(\text{M}=30,\text{P}=33)$               | 突破 CPU 时间限制         | 核心转储 |
| `SIGXFSZ`   | $25(\text{M}=31,\text{P}=34)$               | 突破文件大小限制            | 核心转储 |
|             |                                             |                     |      |
| `SIGALRM`   | $14$                                        | 定时器到期，参考 [[Linux 系统编程：定时器与休眠]]    | 终止   |
| `SIGEMT`    | $\text{undef}(\text{SAMP}=7)$               | 依赖于实现的硬件错误          | 终止   |
| `SIGHUP`    | $1$                                         | 终端断开                | 终止   |
| `SIGINT`    | $2$                                         | 终端中断信号(`ctrl-c`)    | 终止   |
| `SIGIO`     | $29(\text{SA}=23,\text{MP}=22)$             | `fcntl()`           | 终止   |
| `SIGPOLL`   |                                             | Linux 上同 `SIGIO`    |      |
| `SIGKILL`   | $9$                                         | 无法 阻塞, 忽略和捕获。必杀     | 终止   |
| `SIGPIPE`   | $13$                                        | 管道读端关闭时写入管道         | 终止   |
| `SIGPROF`   | $27(\text{M}=29,\text{P}=21)$               | 性能分析定时器过期           | 终止   |
| `SIGPWR`    | $30(\text{SA}=29, \text{MP}=19)$            | 电源故障信号              | 终止   |
| `SIGSTKFLT` | $16(\text{SAM}=\text{undef}, \text{P}=36)$  | 协处理器栈错误             | 终止   |
| `SIGTERM`   | $15$                                        | 终止进程                | 终止   |
| `SIGVTALRM` | $26(\text{M}=28,\text{P}=20)$               | 虚拟定时器过期             | 终止   |
|             |                                             |                     |      |
| `SIGURG1`   | $10(\text{SA}=30, \text{MP}=16)$            | 相互通知事件发生            | 终止   |
| `SIGURG2`   | $12(\text{SA}=31, \text{MP}=17)$            | 相互通知事件发生            | 终止   |
|             |                                             |                     |      |
| `SIGSTOP`   | $19(\text{SA}=17, \text{M}=23,\text{P}=24)$ | 无法 阻塞, 忽略和捕获。必停     | 暂停   |
| `SIGTSTP`   | $20(\text{SA}=18, \text{M}=24,\text{P}=25)$ | 作业控制的停止信号(`ctrl-z`) | 暂停   |
| `SIGTTIN`   | $21(\text{M}=26, \text{P}=27)$              | 后台进程从终端读            | 暂停   |
| `SIGTTOU`   | $21(\text{M}=27, \text{P}=28)$              | 后台进程向终端写            | 暂停   |
| `SIGCONT`   | $18(\text{SA}=19, \text{M}=25,\text{P}=26)$ | 暂停进程恢复执行            | 继续执行 |
|             |                                             |                     |      |
| `SIGCHLD`   | $17(\text{SA}=20, \text{MP}=18)$            | 子进程终止或暂停            | 忽略   |
| `SIGURG`    | $23(\text{SA}=16, \text{M}=21,\text{P}=29)$ | 套接字上紧急数据            | 忽略   |
| `SIGWINCH`  | $28(\text{M}=20,\text{P}=23)$               | 终端窗口尺寸变化            | 忽略   |

> [!important] 
> 
> 信号 `SIGKILL` 和 `SIGSTOP` 不能被捕获 阻塞或忽略
> 

## 改变信号处置

UNIX 系统提供了两种方法来改变信号处置：`signal()` 和 `sigaction()`。这里我们首先学习 `signal()` 系统调用

```c
#include <signal.h>

void ( * signal(int sig, void (*handler)(int) ) ) (int);
//   |   |     |-------  -------------------| |        |
//   |   |------------------------------------|        |
//   |-------------------------------------------------|
/* 成功返回上一次信号处理程序；失败返回 SIG_ERR */
```

这里需要对 `signal()` 函数的原型做一些解释。参考上述注释的分割，`signal()` 系统调用需要两个参数 `sig` 和 `handler`。为了更好理解该原型，可以做如下改写

```c
typedef void(*sighandler_t)(int);

sighandler_t signal(int sig, sighandler_t handler);
```

> [!tip] 参数 `sig`: 信号编号
> 
> `sig` 指定给信号安装信号处理程序
> 

> [!tip] 参数 `handler`：信号处理函数
> 
> 进程收到 `sig` 信号时调用的函数。该函数的原型如下
>
> ```c
> void handler(int sig);
> ```
> 
> 或者使用下面两个预定义宏替代
> + `SIG_DFL`：信号的默认处置
> + `SIG_IGN`：忽略该信号
> 

> [!tip] 返回值：上一个信号处理程序
> 
> 返回一个指针，指向一个函数，其原型与信号处理函数一致。实际上，返回的就是上一个信号处理程序
> 

如果需要暂时修改某个信号的处理程序，可以编写如下代码

```c
void(*old_handler)(int);

old_handler = signal(SIGINT, new_handler);
if(old_hander == SIG_ERR)
{
	// 错误处理
}
....

// 恢复原来的处理函数
if(signal(SIGINT, old_handler) == SIG_ERR)
{
	// 错误处理
}
```

##  信号处理器简介

**信号处理器程序**，也称为**信号捕捉器**，是 **当指定信号传递给进程时将会调用的函数**。下图展示了信号处理器的基本原理

![[Pasted image 20241108185318.png]]

调用信号处理器程序，可能会 **随时打断主程序流程**；内核代表进程来 **调用处理器程序**，当处理器返回时，主程序会在处理器打断的位置恢复执行

> [!example] 
> 
> 假设程序执行完第 $m$ 条指令时，信号 `sig` 到达进程。此时，内核代表进程调用 `handler` 函数。当 `handler` 返回后，程序继续从第 $m+1$ 条指令开始执行
> 
> 意味着，默认情形下，**如果系统调用被信号中断，则不会重启系统调用**
> 

如下例程，为 `SIGINT` 信号安装一个信号处理器程序

> [!tip] `SIGINT`：会话控制终端响应 `ctrl-c` 时，内核向会话前台进程发送 `SIGINT` 中断程序
> 

```c title:signals/sigint.c
#include <signal.h>
#include <unistd.h>

#include "base.h"
#include "current_time.h"

static void sigintHandler(int sig) {
    printf("Process[PID:%d] receive signal %d(%s) at %s\n", getpid(), sig, strsignal(sig), current_time());
}

int main() {
	// 安装 SIGINT 的信号处理器
    if(signal(SIGINT, sigintHandler) == SIG_ERR) {
        errExit("signal error: ");
    }
    while (true) {
        pause();
    }
}
```

## 发送信号

### kill()

一个进程能够使用 `kill()` 系统调用向另一进程发送信号

```c
#include <sys/types.h>
#include <signal.h>

int kill(pid_t pid, int sig);
/* 成功返回 0；失败返回 -1 */
```

> [!tip] 参数 `sig`：发送给目标进程的信号

> [!tip] 参数 `pid`：指定发送信号的目标进程
> 

下表列出了参数 `pid` 是如何确定发送给那个/那些目标进程的

| `pid`  | 描述                                           |
| :----- | :------------------------------------------- |
| $> 0$  | `pid` 指定的 **目标进程**                           |
| $= 0$  | **调用进程所在进程组中每个进程**                           |
| $< -1$ | PGID 等于 $\vert \text{pid} \vert$ 的进程组中的每个进程  |
| $= -1$ | **广播信号**，除了 `init` 进程和调用进程不会收到该信号。前提是调用进程有权限 |

> [!tip] 返回值
> 
> 成功发送信号 `kill()` 则返回 $0$。信号发送失败返回 $-1$ 并设置 `errno`
> 
> 发送信号失败的原因：通常是没有进程与指定的 `pid` 相匹配。`errno` 将被设置为 `ESRCH`，查无此进程
> 

进程要发送信号给另一进程，还需要适当的 **权限**，其权限规则如下

> [!tip] 权限规则
> 
> 特权级进程可以向任何进程发送信号
> 
> `root` 用户组运行的 `init` 进程，**仅能接收已安装了处理器函数的信号**。原因是 **防止系统管理员意外杀死 `init` 进程**
> 
> 如果 **发送者的实际或有效用户 ID** 匹配于 **接受者的实际用户 ID 或者保存设置用户 ID(saved set-user-id)**，那么非特权进程也可以向另一进程发送信号
> 
> ![[Pasted image 20241108195358.png]]
> 
> `SIGCONT` 信号需要特殊处理。无论对用户 ID 的检查结果如何，**非特权进程可以向同一会话中的任何其他进程发送这一信号**
> 

如果 **进程无权发送信号** 给所请求的 `pid`，那么 `kill()` 调用将失败，且 **将 errno 置为 EPERM**。若 `pid` 所指为一系列进程（即 `pid` 是负值）时，只要可以向其中之一发送信号，则 `kill()` 调用成功

### kill() 检查进程是否存在

`kill()` 系统调用还有另一重功用。若将 **参数 `sig` 指定为 `0`（即所谓空信号），则无信号发送**。相反，**`kill()` 仅会去执行错误检查**，查看是否可以向目标进程发送信号

从另一角度来看，这意味着，**可以使用空信号来检测具有特定 PID 的进程是否存在**
- 若 **发送空信号失败**，且 **errno 为 ESRCH**，则表明 **目标进程不存在**
- 如果 **调用失败**，且 **errno 为 EPERM**，则表示 **进程存在**，但无权向目标进程发送信号）
- **调用成功**（有权向进程发送信号），那么就表示 **进程存在**

> [!attention] 
> 
> 上述方法 **仅能检查进程是否存在**，不能判断程序是否正在执行
> 

如下例程演示了 `kill()` 的使用，该程序接受两个命令行参数，分别为 **信号编号** 和 **进程ID**，并使用 `kill()` 将该信号发送给指定进程。如果指定了信号 $0$（空信号），那么程序将报告目标进程是否存在

```c title:signals/t_kill.c
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>

#include "base.h"

int main(int argc, char *argv[]) {
    if(argc !=3 || strcmp(argv[1], "--help") == 0) {
        usageErr("%s <sig> <pid>\n", argv[0]);
    }
    int sig = (int)strtol(argv[1], nullptr, 10);
    pid_t pid = (pid_t)strtol(argv[2], nullptr, 10);
    // 发送信号
    int s = kill(pid, sig);

    if(sig != 0) {   // 发送非空信号
        // kill() 失败并且错误码为 ESRCH，目标进程不存在
        if(s == -1 && errno == ESRCH) {
            fprintf(stderr, "[PID %ld] not existed\n", (long)pid);
            return EXIT_FAILURE;
        }

        // kill() 失败并且 错误码为 EPERM，无权向目标进程发送信号
        if(s == -1 && errno == EPERM) {
            fprintf(stderr, "[PID %ld] not authorized by user\n", (long)pid);
            return EXIT_FAILURE;
        }
    } else {    // 发送空信号
        // kill() 失败并且错误码为 ESRCH，目标进程不存在
        if(s == -1 && errno == ESRCH) {
            fprintf(stderr, "[PID %ld] not existed\n", (long)pid);
            return EXIT_FAILURE;
        }
        // kill() 失败并且错误码为 EPERM，目标进程存在，但是无权发送信号
        if(s == -1 && errno == EPERM) {
            fprintf(stderr, "[PID %ld] exists but not authorized by user\n", (long)pid);
            return EXIT_FAILURE;
        }

        // kill 成功
        if(s == 0) {
            fprintf(stderr, "[PID %ld] exists\n", (long)pid);
            return EXIT_SUCCESS;
        }
    }
    return EXIT_SUCCESS;
}
```

下面的 shell 会话展示了该程序的使用

```shell
➜  build python &
[1] 231173
➜  build ./t_kill 0 231173
[PID 231173] exists
➜  build ./t_kill 9 231173
[1]  + 231173 killed     python
➜  build ./t_kill 0 231173
[PID 231173] not existed
```

### raise()

有时，**进程需要向自身发送信号**。`raise()` 函数就执行了这一任务

```c
#include <signal.h>

int raise(int sig);
/* 成功返回 0； 错误返回非零*/
```

在 **单线程** 程序中，调用 `raise()`相当于对 `kill()` 的如下调用

```c
kill(getpid(), sig);
```

在线程的系统会将 `raise(sig)`实现为

```c
pthread_kill(pthread_self(), sig);
```

> [!tip] 
> 
> 当进程使用 `raise()`（或者 `kill()`）**向自身发送信号** 时，**信号将立即传递**，即，在 `raise()` 返回调用者之前
> 
> 调用 `raise()` 唯一可能发生的错误为 `EINVAL`，即 `sig` 无效
> 

### killpg()

`killpg()` 函数向 **进程组** 中的 **所有成员** 发送一个信号

```c
#include <signal.h>

int killpg(pid_t pgrp, int sig);
/* 成功返会 0；失败返回 -1*/
```

> [!tip] 
> 如果指定 `pgrp` 的值为 `0`，那么会向调用者所属进程组的所有进程发送此信号
> 

`killpg()` 调用相当于对 `kill()`的如下调用

```c
kill(-pgrp, sig);
```

### abort()

`abort()` 库函数向进程自身发送 `SIGABRT` 信号

```c
#include <stdlib.h>

[[noreturn]] void abort(void);
```

`abort()` 等价于如下调用

```c
raise(SIGABRT);
```

## 显示信号描述

**每个信号都有一串与之相关的可打印说明**。这些描述位于数组 `sys_siglist` 中。例如，可以用 `sys_siglist[SIGPIPE]` 来获取对 `SIGPIPE` 信号（管道断开）的描述。然而，较之于直接引用 `sys_siglist` 数组，还是推荐使用 `strsignal()` 函数

```c
#define _BSD_SOURCE
#include <signal.h>

extern const char *const sys_siglist[];

#define _GNU_SOURCE
#include <string.h>

char *strsignal(int sig);
```

`strsignal()` 函数对 `sig` 参数进行边界检查，然后返回 **指向信号的可打印描述字符串**

`psignal()` 函数在标准错误设备上显示示为 `msg` 参数所给定的字符串，后面跟有一个冒号，随后是对应于 `sig` 的信号描述。和 `strsignal()`一样，`psignal()` 函数也对本地设置敏感

```c
#include <signal.h>

void psignal(int sig, const char *msg);
```
