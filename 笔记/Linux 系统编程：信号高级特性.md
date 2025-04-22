# 信号高级特性

## 核心转储文件

在 [[Linux 系统编程：信号基本概念#信号类型和默认行为]] 中介绍了某些信号会引发进程创建一个 **核心转储文件** 并终止运行

> [!tip] 
> 
> 所谓核心转储是内含 **进程终止时内存映像的一个文件**
> 

引发程序生成核心转储文件的方式之一是键入退出字符(通常为 `Control+\`)，`从而生成SIGQUIT` 信号。

核心转储文件创建于进程的工作目录中，名为 `core`。这是核心转储文件的默认位置和名称

> [!attention] 不产生核心转储文件的情况
> 
> + 进程对于核心转储文件没有写权限
> + 存在一个同名且可写的硬链接数超过 $1$ 的普通文件
> + 创建核心转储文件的所在目录并不存在，或者文件系统空间已满，又或者 i-node资源耗尽
> + `Set-user-ID` 和`set-group-ID` 程序在由非文件属主（或属组）执行
> + 以只读方式挂载当前工作目录所在的文件系统
> + 把进程 **“核心转储文件大小**” 这一资源限制置为 $0$
> + 将进程 **“可创建文件的大小”** 这一资源限制设置为 $0$
> + 对进程正在执行的二进制可执行文件没有读权限
> 

> [!tip] 为核心转储文件命名：`/proc/sys/kernel/core_pattern`
> 
> 根据 Linux 特有的 `/proc/sys/kernel/core_pattern` 文件所包含的格式化字符串来控制对系统上生成的所有核心转储文件的命名。默认情况下，该文件所含字符串为 `core`。
> 
> ![[Pasted image 20241109234253.png]]
> 

## 信号传递-处置-处理的特殊情况


### SIGKILL 和 SIGSTOP

`SIGKILL` 信号的默认行为是终止一个进程，`SIGSTOP` 信号的默认行为是停止一个进程，二者的默认行为均无法改变

> [!tip] `SIGKILL` 和 `SIGSTOP` 的默认行为无法修改
> 
> 当试图用 `signal()` 和 `sigaction()` 来改变对这些信号的处置时，将 **总是返回错误**。同样，也 **不能将这两个信号阻塞**
> 
> 不允许修改这些信号的默认行为，这也意味着总是可以利用这些信号来杀死或者停止一个失控进程
> 

### SIGCONT 和停止信号

可使用 `SIGCONT` 信号来使某些处于停止状态的进程得以继续运行。由于这些停止信号具有独特目的，所以在某些情况下内核对它们的处理方式将有别于其他信号

> [!tip] 
> 
> 通常是 `SIGSTOP` `SIGTSTP` `SIGTTIN` 和  `SIGTTOU` 信号会导致进程停止运行
> 

如果一个进程处于停止状态，那么一个 `SIGCONT` 信号的到来总是会促使其恢复运行，即使该进程正在阻塞或者忽略 `SIGCONT` 信号

> [!tip] `SIGCONT` 信号总是可以使暂停进程恢复运行，**即使正在阻塞或被忽略**
> 
> 该特性之所以必要，是因为如果要恢复这些处于停止状态的进程，舍此之外别无他法
> 

如果处于停止状态的进程正在阻塞 `SIGCONT` 信号，并且已经为 `SIGCONT` 信号建立了处理器函数，那么在进程恢复运行后，只有当取消了对 `SIGCONT` 的阻塞时，进程才会去调用相应的处理器函数

每当进程收到 `SIGCONT` 信号时，会将处于等待状态的停止信号丢弃（即进程根本不知道这些信号）。相反，如果任何停止信号传递给了进程，那么进程将自动丢弃任何处于等待状态的 `SIGCONT` 信号。之所以采取这些步骤，意在防止之前发送的一个停止信号会在随后撤销 `SIGCONT` 信号的行为，反之亦然

> [!tip] 进程收到 `SIGCONT` 信号，就会丢弃正在等待的停止信号。反过来，也会丢弃正在阻塞的 `SIGCONT` 信号

### 终端产生的信号

如果程序在执行时发现，已将对由终端产生信号的处置置为了 `SIG_IGN`（忽略），那么程序通常不应试图去改变信号处置

> [!tip] 被忽略的终端信号，程序不应该试图去改变信号处置
> 
> 相关的信号有
> + `SIGHUP`：控制终端断开
> + `SIGINT` ：`Ctrl + c`
> + `SIGQUIT`：`Ctrl + \`
> + `SIGTTIN`：后台进程从终端读
> + `SIGTTOU`：后台进程向终端写
> + `SIGTSTP`：`Ctrl + z`
> 

## 进程睡眠状态

`SIGKILL` 和 `SIGSTOP` 信号对进程的作用是立竿见影的。对于这一论断，此处要加入一条限制。**内核经常需要令进程进入休眠，而休眠状态又分为两种**

> [!tip] `TASK_INTERRUPTIBLE`：进程等待某一事件，**可中断的睡眠状态**
> 
> 例如，正在等待终端输入；等待数据写入当前的空管道；等待 System V 信号量值的增加
> 
> **进程在该状态下所耗费的时间可长可短**。如果为这种状态下的进程发送一个信号，那么操作将中断，而传递来的信号将唤醒进程
> 
> `ps` 对进程状态标识为 `S`
> 

> [!tip] `TASK_UNINTERRUPTIBLE`：进程正在等待某些特定类型的事件，**不可中断的睡眠**
> 
> 例如，磁盘 I/O 的完成
> 
> 如果为这种状态下的进程发送一个信号，那么**在进程摆脱这种状态之前**，系统将 **不会把信号传递给进程**
> 
> `ps` 对进程状态标识为 `D`
> 

因为进程处于 `TASK_UNINTERRUPTIBLE` 状态的时间通常转瞬即逝，所以系统在进程脱离该状态时传递信号的现象也不易于被发现。然而，在极少数情况下，进程可能会因硬件故障，或者NFS 问题，或者内核缺陷而在该状态下保持挂起。这时，`SIGKILL` 将不会终止挂起进程。如果问题诱因无法得到解决，那么就只能通过重启系统来消灭该进程。

在 Linux 2.6.25 开始，加入第三种状态解决进程保持在 `TASK_UNINTERRUPTIBLE` 状态的问题

> [!tip] `TASK_KILLABLE`：不可中断的睡眠。但是会 **响应致命信号**
> 
> 接收到 `SIGKILL` 信号时被唤醒
> 

## 硬件产生的信号

硬件异常可以产生 `SIGBUS` `SIGFPE` `SIGILL` 和 `SIGSEGV` 信号。通过调用 `kill()` 函数来发送此类信号是另一种途径，但较为少见。

SUSv3 规定，在硬件异常的情况下，如果进程从此类信号的处理器函数中返回，亦或进程忽略或阻塞了此类信号，那么进程的行为未定义

> [!warning] 从信号处理器函数中返回：**可能导致进程重复调用信号处理器函数**
> 
> 假设机器语言指令产生了上述信号之一，并且调用了信号处理器函数。当从处理器函数正常返回后，程序会 **尝试从其中断处恢复执行**
> 
> 当 **引发信号产生的恰恰正是中断处的这条指令**，信号会再次“光临”。 从而导致，程序进入无限循环，重复调用信号处理器函数
> 

> [!tip] 忽略信号：程序如何执行时不明确的
> 
> 当由于硬件异常而产生上述信号之一时，**Linux 会强制传递信号**，即使程序已经请求忽略此类信号
> 

> [!warning] 阻塞信号：程序后续如何执行不明确
> 
> Linux 内核会将阻塞硬件产生信号的企图忽略。**信号无论如何都会传递给进程**。随后进程要么终止，要么信号处理器捕捉信号
> 
> 始于 Linux 2.6，如果硬件产生的信号被阻塞，那么该 **信号总是会立刻杀死进程**，即使进程已经为此信号安装了处理器函数
> 

正确处理硬件产生信号的方法有二：要么 **接受信号的默认行为**，即进程终止；要么为注册 **不会正常返回的处理器函数**

## 信号的同步生成和异步生成


**进程一般无法预测其接收信号的时间**。要证实这一点，需要对信号的 **同步生成** 和 **异步生成** 加以区分。

> [!tip] 信号的异步生成： **引发信号产生的事件，与进程的执行无关**
> 
> 目前所探讨的均属于信号的异步生成。可能是内核发送的信号，也可能是其他进程发送的信号
> 

> [!tip] 信号的同步生成：由进程本身执行造成的，会立即递送给进程
> 
> 执行特定的机器语言指令，可导致 **硬件异常**，并因此硬件产生信号
> + `SIGBUS`、`SIGFPE`、`SIGILL`、`SIGSEGV` 和 `SIGEMT`
> 
> 使用 `raise()`、`kill()` 或者 `killpg()` **向自身发送信号**
> 

> [!WARNING] 同步和异步是 **对信号产生方式的描述**
> **同步是对信号产生方式的描述**，不针对信号本身。所有的信号既可以同步产生（例如，进程使用 `kill()` 向自身发送信号），也可以异步产生（由另一进程使用 `kill()` 来发送信号）
> 

对于同步产生的信号而言，其传递不但可以预测，而且可以重现

## 信号传递的时机与顺序

下面探讨两个问题

> [!question] 
> + 何时传递一个处于等待状态的信号? 
> + **多个遭到阻塞信号处于等待状态**，如果 **同时解除阻塞**，将会发生什么情况？

### 何时传递一个处等待状态的信号？

**同步产生的信号会立即传递**。例如，硬件异常会触发一个即时信号，而当进程使用 `raise()` 向自身发送信号时，信号会在 `raise()` 调用返回前就已经发出

**当异步产生一个信号时，即使并未将其阻塞，在信号产生和实际传递之间仍可能会存在一个（瞬时）延迟**。在此期间，信号处于等待状态

> [!tip] 
> 内核将等待信号传递给进程的时机是，该 **进程正在执行**，且 **发生由内核态到用户态的下一次切换时**
> 

实际上，这意味着在以下时刻才会传递信号

+ **进程在前度超时后，再度获得调度时**。即，在一个时间片的开始处
+ **系统调用完成时**。信号的传递可能引起正在阻塞的系统调用过早完成

### 同时解除多个阻塞信号，信号的传递顺序

如果进程使用 `sigprocmask()` 解除了对多个等待信号的阻塞，那么所有这些信号会立即传递给该进程

> [!tip] 就目前的 Linux 实现而言，**Linux 内核按照信号编号的升序来传递信号**
> 
> 例如，如果对处于等待状态的信号 `SIGINT`（信号编号为 `2`）和 `SIGQUIT`（信号编号为 `3`）同时解除阻塞，那么无论这两个信号的产生次序如何，`SIGINT` 都将先于 `SIGQUIT` 而传递
> 
> 进程收到 `SIGCONT` 信号，就会丢弃正在等待的停止信号。反过来，也会丢弃正在阻塞的 `SIGCONT` 信号
> 

> [!warning] 
> 
> **不能对传递（标准）信号的特定顺序产生任何依赖**，因为 SUSv3 规定，多个信号的传递顺序由系统实现决定

当多个解除了阻塞的信号正在等待传递时，如果在信号处理器函数执行期间发生了内核态和用户态之间的切换，那么将中断此处理器函数的执行，转而去调用第二个信号处理器函数（如此递进）

![[Pasted image 20241110085029.png]]

## 实时信号

定义于 POSIX.1b 中的实时信号，意在弥补对标准信号的诸多限制。较之于标准信号，其优势如下所示

> [!tip] 实时信号的信号范围有所扩大，可应用与应用程序自定义的目录
> 
>  **标准信号** 中可供应用随意使用的信号仅有两个：`SIGUSR1` 和 `SIGUSR2`。
> 

> [!tip] 对实时信号所采取的是队列化管理
> 
> 如果将某一实时信号的多个实例发送给一进程，那么将会多次传递信号
> 
> 如果某一标准信号已经在等待某一进程，而此时即使再次向该进程发送此信号的实例，信号也只会传递一次
> 

> [!tip] 实时信号可以为信号指定伴随数据
> 
> 伴随数据可以是一个整数或指针值，供接收进程的信号处理器函数获取
> 

> [!tip] 不同实时信号的传递顺序得到保障：率先传递具有最小编号的信号
> 
> **信号的编号越小，其优先级越高**。如果是同一类型的多个信号在排队，那么信号以及伴随数据的传递顺序与信号发送来时的顺序保持一致
> 

SUSv3 要求，实现所提供的各种实时信号不得少于 `_POSIX_RTSIG_MAX`（定义为 $8$）个。Linux 内核则定义了 $32$ 个不同的实时信号，编号范围为 $32\sim 63$。`<signal.h>` 头文件所定义的 `RTSIG_MAX` 常量则表征实时信号的可用数量。此外还定义的常量 `SIGRTMIN` 和 `SIGRTMAX` 则分别表示可用实时信号编号的最小值和最大值

 > [!attention] 
>  LinuxThreads 线程实现的系统将 `SIGRTMIN` 定义为 $35$，因为 LinuxThreads 内部使用了前三个实时信号
>  
>  NPTL 线程实现的系统则将 `SIGRTMIN` 定义为 $34$，因为 NPTL 内部使用了前两个实时信号

>[!warning] 
>
> SUSv3 并未要求 `SIGRTMAX` 和 `SIGRTMIN` 是简单的整数值，可以将其定义为函数，就像 Linux 中那样
>

排队的实时信号及其相关数据需要内核维护相应的数据结构，用于罗列每个进程的排队信号。由于这些数据结构会消耗内核内存，故而内核对排队实时信号的数量设置了限制

> [!tip] 排队实时信号的数量限制
> 
> SUSv3 允许实现为每个进程中可排队的各类实时信号数量设置上限，并要求其不得少于`_POSIX_SIGQUEUE_MAX`（定义为 $32$）
> 
> 要获取这一限制使用系统调用 `lim = sysconf(_SC_SIGQUEUE_MAX)`
> 

### 发送实时信号

系统调用 `sigqueue()` 将由 `sig` 指定的实时信号发送给由 `pid` 指定的进程

```c
#include <signal.h>

int sigqueue(pid_t pid, int sig, const union sigval value);
/* 成功返回 0; 失败返回 -1 */
```

> [!tip] 参数 `pid`: 指定发送信号的目标进程
> 
> + 与 `kill()` 不同的是，`pid` 不能指定为负数 
> 

> [!tip] 参数 `sig`：要发送实时信号的编号
> 
> 可以发送空信号(编号为 $0$)，含义与 `kill()` 一致
> 

> [!tip] 参数 `value`：实时信号的伴随数据
> 
> 实时信号的伴随数据时一个联合，定义如下
> 
> ```c
> union sigval
> {
> 	int sival_int; /*伴随数据是整数*/
> 	void *sival_ptr; /*伴随数据时指针*/
> };
> ```
> 对该参数的解释则取决于应用程序，由其选择对联合中的 `sival_int` 属性还是 `sival_ptr` 属性进行设置。
> 
> 不过很少使用 `sigval_ptr`，因为指针的作用返回是进程内部，对于另一个进程几乎没有任何作用
> 

向进程发送实时信号也有权限限制，其限制于 [[Linux 系统编程：信号基本概念#kill()]] 中介绍的一致
### 处理实时信号

可以像标准信号一样，**使用常规（单参数）信号处理器来处理实时信号**。此外，也可以**用带有 $3$ 个参数的信号处理器函数来处理实时信号**，其建立则会用到 `SA_SIGINFO` 标志

```c
struct sigaction act;
sigemptyset(&act.sa_mask);
act.sa_sigaction = handler;
act.sa_flags = SA_RESTART | SA_SIGINFO;

if(sigaction(SIGRTMIN + 5, &act, nullptr) == -1) {
	// 错误处理
}
```

关于 `sigaction()` 的 `SA_SIGINFO` 标志，在 [[Linux 系统编程：信号处理器函数#SA_SIGINFO 标志]] 以详细介绍

对于一个实时信号而言，会在 `siginfo_t` 结构中设置如下字段

| 字段         | 描述                                                    |
| :--------- | :---------------------------------------------------- |
| `si_signo` | 信号编号。与信号处理器函数的第一个参数相同                                 |
| `si_code`  | 信号来源。`sigqueue()` 发送的信号来源是 `SI_QUEUE`                 |
| `si_value` | 伴随数据。`sigqueue()` 发送信号时的第三个参数 `value`(`union sigval`) |
| `si_pid`   | 发送信号进程的 PID                                           |
| `si_uid`   | 发送信号进程的 UID                                           |

## 使用掩码等待信号

在信号编程时偶尔会遇到如下情况 

> [!tip] 防止信号中断关键代码：需要临时阻塞一个信号
> 
> 例如 [[Linux 系统编程：信号集#信号掩码]] 介绍的系统调用，可以完成这样的操作
> 

> [!tip] 等待信号到达：解除阻塞并暂停执行直到信号到达

为了让进程等待信号的到达，可以尝试如下代码片段

```c

sigset_t preMask, intMask;
sigemptyset(&intMask);
sigaddset(&intMask, SIGINT);

struct sigaction sa;
sigemptyset(&sa.sa_mask);
sa.sa_flags =  0;
sa.sa_handler = handler;

// 注册信号处理器函数
if(sigaction(SIGINT, &sa, nullptr) == -1) {
	// 错误处理
}

/* .... */

// 进入临界区之前阻塞SIGINT。(此时，我们假设SIGINT还没有被阻塞。)
if(siprocmask(SIG_BLOCK, &intMask, &preMask) == -1) {
	// 错误处理
}

/* 临界区：此处不能被 SIGINT 中断 */

// 临界区结束-恢复旧掩码以解除对 SIGINT 的阻塞

if(sigprocmaks(SIG_SETMASK, &preMask, nullptr) == -1) {
	// 错误处理
}

/* BUG: 信号在调用 pause() 之前到达 */

pause(); // 等待 SIGINT 信号
```

上述代码片段存在一个问题：假设 `SIGINT` 信号的传递发生在第二次调用 `sigprocmask()` 之后，调用 `pause()` 之前。

> [!tip] 
> 
> 实际上，该信号可能产生于执行关键片段期间的任一时刻，仅当解除对信号的阻塞后才会随之而传递
> 

`SIGINT` 信号的传递将导致对处理器函数的调用，而当处理器返回后，主程序恢复执行，`pause()` 调用将陷入阻塞，直到 `SIGINT` 信号的第二个实例到达为止。这有违代码的本意：解除对 `SIGINT` 阻塞并等待其第一次出现

要避免这一问题，需要将 **解除信号阻塞** 和 **挂起进程** 这两个动作封装成一个原子操作。系统调用 `sigsuspend()` 完成了这一封装

```c
#include <signal.h>

int sigsuspend(const sigset_t *mask);

/* 通常返回 -1 并且 errno 设置为 EINTR*/
```

> [!tip] 参数 `mask`：指向信号集的指针
> 
> 用于替换进程的信号掩码。解除阻塞的信号不在该信号集中
> 

调用 `sigsuspend()` 会 **挂起进程** 直到其捕获到信号，并从信号处理器中返回

> [!important] 
> 
> 一旦处理器返回，`sigsuspend()` 会 **将进程信号掩码恢复为调用前的值**
> 

调用 `sigsuspend()`，相当于 **以不可中断方式** 执行如下操作

```c

// 设置新的信号掩码
if(siprocmask(SIG_SETMASK, &mask, &preMask) == -1) {
	// 错误处理
}
// 等待信号到达
pause(); 

// 恢复原来的信号掩码
if(sigprocmaks(SIG_SETMASK, &preMask, nullptr) == -1) {
	// 错误处理
}
```

### 利用信号来同步进程

在 [[Linux系统编程：进程创建#fork() 之后的竞争条件]] 知道，调用 `fork()` 之后，如果 **进程甲** 需等待 **进程乙** 完成某一动作，那么 **进程乙（即活动进程）可在动作完成后向进程甲发送信号**；进程甲则等待即可 

下列程序 **假设父进程必须等待子进程完成某些动作**。如果是子进程反过来要等待父进程，那么将父、子进程中与信号相关的调用对掉即可。父子进程甚至可能多次互发信号以协调彼此行为

> [!tip] 
> 
> 进程间同步实际上更有可能采用 **信号量** **文件锁** 或 **消息传递** 等技术来进行此类协调
> 

这里我们使用标准信号中的用户自定义信号 `SIGUSR1`，在 `fork()` 之前阻塞该信号。如果在 `fork()` 之后才阻塞该信号，子进程可能在信号阻塞前就向父进程发送了该信号

> [!important] 信号掩码被 `fork()` 子进程继承，并且在 `exec()` 后保持
> 
> 如有必要，可以在`fork()` 之后的子进程中解除对 `SIGUSR1` 的阻塞
> 

```c title:signals/fork_sig_sync.c
#include <signal.h>
#include <unistd.h>

#include "base.h"
#include "current_time.h"

static void handler(int sig) {
    printf("[PID %ld] catch signal %d(%s) at %s\n", (long)getpid(), sig, strsignal(sig), current_time());
}

int main() {

    // 立即禁用 stdout 上的缓冲区
    setvbuf(stdout, nullptr, _IONBF, 0);
    // 立即阻塞 SIGUSR1 信号
    sigset_t blockMask, origMask, emptyMask;
    sigemptyset(&blockMask);
    sigaddset(&blockMask, SIGUSR1);
    if(sigprocmask(SIG_BLOCK, &blockMask, &origMask) == -1) {
        errExit("sigprocmask error:");
    }

    // 注册信号处理器函数
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if(sigaction(SIGUSR1, &sa, nullptr) == -1) {
        errExit("sigaction error:");
    }

    // 调用 fork() 创建子进程
    switch (fork()) {
        case -1:
            errExit("fork error:");
        case 0: // 子进程
            // 子进程率先完成某些事情
            printf("%s: Child[PID %ld] started - doing some work\n", current_time(), (long)getpid());
            sleep(2);
            // 子进程完成工作，向父进程发送信号通知父进程可以继续执行
            printf("%s: Child[PID %ld] about to signal Parent[PID %ld]\n", current_time(), (long)getpid(), (long)getppid());
            if(kill(getppid(), SIGUSR1) == -1) {
                errExit("kill error:");
            }
            _exit(EXIT_SUCCESS);
        default: // 父进程
            // 父进程需要执行一些操作，但是必须等待子进程先完成
            printf("%s: Parent[PID %ld] about to wait for signal\n", current_time(), (long)getpid());
            sigemptyset(&emptyMask);
            // 使用 emptyMask 替换进程的信号掩码，已达到解除阻塞的目的
            // sigsuspend() 会挂起进程，并等待信号到达。被信号中断返回 -1 并设置 errno == EINTR
            if(sigsuspend(&emptyMask) == -1 && errno != EINTR) {
                // 不是目标错误
                errExit("sigsuspend error:");
            }
            // 父进程收到信号继续执行
            printf("%s: Parent[PID %ld] got signal\n", current_time(), (long)getpid());

            // 父进程可以继续执行，此时恢复原始的信号掩码
            if(sigprocmask(SIG_SETMASK, &origMask, nullptr) == -1) {
                errExit("sigprocmask error:");
            }
            exit(EXIT_SUCCESS);
    }
    return 0;
}
```

下面的 shell 会话展示了该程序的执行过程

```shell
➜  build ./fork_sig_sync
2024-11-10 12:38:50: Parent[PID 293252] about to wait for signal
2024-11-10 12:38:50: Child[PID 293253] started - doing some work
2024-11-10 12:38:52: Child[PID 293253] about to signal Parent[PID 293252]
2024-11-10 12:38:52: Parent[PID 293252] catch signal 10(User defined signal 1)
2024-11-10 12:38:52: Parent[PID 293252] got signal
```

## 同步等待信号

上一节，介绍了如何结合信号处理器和 `sigsuspend()` 来挂起一个进程的执行，直至传来一个信号。然而，这需要编写信号处理器函数，还需要应对信号异步传递所带来的复杂性

对于某些应用而言，上述方法过于繁复。作为替代方案，可以利用 `sigwaitinfo()` 系统调用来 **同步接收信号**

```c
#include <signal.h>

int sigwaitinfo(const sigset_t * set, siginfo_t * info);
				
int sigtimedwait(const sigset_t * set,siginfo_t *  info, const struct timespec * timeout);
/* 成功返回递送的信号编号；失败返回 -1 */
```

> [!tip] 参数 `set`：信号集指针，指向进程等待的信号集
> 

> [!tip] 参数 `info`：`siginfo_t` 结构的指针，指向一个缓冲区，用于获取信号的伴随数据
> 
> `siginfo_t` 结构的介绍参考 [[Linux 系统编程：信号处理器函数#SA_SIGINFO 标志]]
> 

> [!tip] 返回值
> 
> `sigwaitinfo()` 的返回值就是传递过来的信号编号
> 

调用 `sigwaitinfo()` 会挂起进程的执行，直到 `set` 指向信号集中的某个信号到达。参数 `info` 用于获取某个信号的伴随数据

> [!attention] 
> 
> 如果调用 `sigwaitinfo()` 时，`set` 指向信号集中的某个信号处于等待状态，那么 `sigwaitinfo()` 立即返回
> 
> `sigwaitinfo()` 不对标准信号进行排队处理，但是对实时信号进程排队处理并且实时信号的传递顺序遵循低编号优先的原则
> 

