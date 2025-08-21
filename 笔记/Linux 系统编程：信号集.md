# 信号集

之前我们提过信号的 $3$ 种处理方法：忽略，注册信号处理器程序和 **阻塞**。阻塞某一个或者是某一类信号非常重要

> [!tip] 
> 
> 例如，我们使用信号处理器程序处理某一个信号时，不希望其它的信号打断这个信号处理器程序的执行
> 

例如系统调用 `sigaction()` 和 `sigprocmask()` 允许程序指定 **一组** 将被进程阻塞的信号，而 `sigpending()` 则返回一组目前正在 **等待送达** 给进程的信号

多个信号可以使用一个称为 **信号集** 的数据结构表示，系统数据类型为 `sigset_t`

> [!quote] 
> 
> `sigset_t` 在 Unix/Linux 中使用一个 **位掩码**(位图)，然而 SUSv3 并未要求如此。使用其他类型的结构来表示信号集也是可能得
> 
> SUSv3 仅要求 `sigset_t` 类型 **可以进行赋值操作** 即可。因此，必须使用 **标量数据类型**
> 

`sigset_t` 类型定义在头文件 `<sigset.h>` 中，其定义如下

```c
#define _SIGSET_NWORDS (1024 / (8 * sizeof (unsigned long int)))
typedef struct
{
  unsigned long int __val[_SIGSET_NWORDS];
} __sigset_t;
```

> [!tip] 无论在 $32$ 位还是在 $64$ 位平台上，`sigset_t` 类型都占 $128$ 字节
> 
> 在 $32$ 位平台上，`unsigned long int` 占 $4$ 字节，`1024 / (8 * 4) = 32`
> 
> 在 $64$ 位平台式，`unsigned long int` 占 $8$ 字节，`1024 / (8 * 8) = 16`
> 

`sigset_t` 类型使用 **每个位的值** 表示信号集中 **是否存在该信号**
+ $0$ 表示不存在
+ $1$ 表示存在

![Pasted image 20241108230300|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755783897427-df1bfdf2d1c841718fd8d274de4a9714.png)

SUSv3 规定了一系列函数来操纵信号集

## 信号集操作函数

### 初始化信号集

`sigemptyset()` 函数初始化一个未包含任何成员的信号集。`sigfillset()` 函数则初始化一个信号集，使其包含所有信号(包括所有实时信号)

```c
#include <signal.h>

int sigemptyset(sigset_t *set); 
int sigfillset(sigset_t *set);
/* 成功返回 0；失败返回-1 */
```

> [!tip] 
> + 函数 `sigemptyset(&set)`：将 `sigset_t` 类型变量 `set` 中 **所有位** 置为 $0$
> 
> + 函数 `sigfillset(&set)`：将 `sigset_t` 类型变量 `set` 中 **所有位** 置为 $1$
> 

请注意：必须使用 `sigemptyset()` 或者 `sigfillset()` 函数初始化 `sigset_t` 类型的变量，原因如下
+ C 语言不会初始化自动变量
+ 借助未初始化的静态变量默认初始化为 $0$ 不具有可移植性，可能使用位掩码之外的数据结构。处于同一个原因，`memset()` 函数也是不正确

### 添加或移除信号

信号集初始化之后，可以使用 `sigaddset()` 和 `sigdelset()` 函数向集合中添加信号或移除信号

```c
#include <signal.h>

int sigaddset(sigset_t *set, int sig); // 添加一个信号
int sigdelset(sigset_t *set, int sig); // 移除一个信号
/* 成功返回0；错误返回-1 */
```

> [!tip] 参数 `sig`: 添加到信号集中的信号编号

### 测试信号集

`sigismember()` 函数用来测试信号 `sig` 是否是信号集 `set` 的成员

```c
#include <signal.h>

int sigismember(const sigset_t *set, int sig);
/* 是返回 1；不是返回 0 */
```

### GNU C 非标函数

GNU C 库还实现了 $3$ 个非标准函数，是对上述信号集标准函数的补充

```c
#include <signal.h>

// 交集 and; 并集 or
int sigandset(sigset_t *dset, sigset_t *left, sigset_t *right); 
int sigorset(sigset_t *dset, sigset_t *left, sigset_t *right);
/* 成功返回0；错误返回-1 */

// 判断信号集是否为空
int sigisemptyset(const sigset_t *set)l;
/* 是返回1；否返回0 */
```

下面的示例程序实现的函数供后续例程使用。其中
+ 第一个函数 `printSigset()` 显示指定信号集的成员信号
+ 第二个函数 `printSigMask()` 显示进程信号掩码中的信号
+ 第三个函数 `printPendingSigs()` 显示进程正在阻塞的信号

```c title:include/signal_functions.h
#ifndef SIGNAL_FUNCTIONS_H
#define SIGNAL_FUNCTIONS_H

#include <signal.h>
#include <stdio.h>

void printSigset(FILE * of, const char * prefix, sigset_t *sigset);
int printSigMask(FILE * of, const char * prefix);
int printPendingSignals(FILE * of, const char * prefix);

#endif //SIGNAL_FUNCTIONS_H
```

```c title:lib/signal_functions.c
#include "signal_functions.h"

#include <string.h>

void printSigset(FILE * of, const char * prefix, sigset_t *sigset) {
    int cnt = 0;
    for(int sig = 1; sig < NSIG; sig++) {
        if(sigismember(sigset, sig)) {
            cnt++;
            fprintf(of, "%s%d(%s)\n", prefix, sig, strsignal(sig));
        }
    }
    if(cnt == 0) {
        fprintf(of, "%s<empty signal set>\n>", prefix);
    }
}

int printSigMask(FILE * of, const char * msg) {
    sigset_t currMask;
    if(msg != nullptr) {
        fprintf(of, "%s", msg);
    }
    if(sigprocmask(SIG_BLOCK, nullptr, &currMask) == -1) {
        return -1;
    }
    printSigset(of, "\t\t", &currMask);
    return 0;
}


int printPendingSignals(FILE * of, const char * msg) {
    sigset_t pendingMask;
    if(msg != nullptr) {
        fprintf(of, "%s", msg);
    }
    if(sigpending(&pendingMask) == -1) {
        return -1;
    }
    printSigset(of, "\t\t", &pendingMask);
    return 0;
}
```

## 信号掩码

内核为每个进程维护了一个 **信号掩码**，代表一组 **被该进程阻塞的信号**

> [!important] 信号掩码的作用就是 **阻塞信号的传递**
> 
> 除了 `SIGKILL` 和 `SIGSTOP` 这两个 **必杀/必停** 的信号无法被阻塞以外，其余的信号都可以被阻塞
> 

> [!tip] 
> 
> 内核会为每一个进程都维护一个 **信号掩码**，也就是 **一组信号**，**阻塞其针对该进程的传递**，直到进程从信号掩码中将该信号移除
> 

### 向信号掩码添加信号

向信号掩码中添加一个信号，有如下几种方式

> [!tip] 方式 $1$：将引发信号处理程序调用的信号 **自动添加到信号掩码** 中
> 
> 使用 `sigaction()` 注册信号处理程序时，可以使用 `SA_NODEFER` 取消这个行为
> 

> [!tip] 方式 $2$：`sigaction()` 注册信号处理程序是添加
> 
> 调用信号处理程序时会阻塞信号的传递
> 

> [!tip] 方式 $3$：使用 `sigprocmask()` 系统调用
> 
> 随时向信号掩码中添加或删除信号
> 

此处，我们首先介绍 `sigprocmask()` 系统调用。该系统调用可随时修改或获取进程的信号掩码

```c
#include <signal.h>

int sigprocmask(int how, const sigset_t *set, sigset_t *oldeset);
/* 成功返回 0；错误返回 -1 */
```

> [!tip] 参数 `how`：控制 `sigprocmask()` 的行为
> 
> 参数 `how` 的取值可以是下面的预定以宏
> 
> + `SIG_BLOCK`: 将 `set` 指向的信号集添加到进程当前信号掩码中
> + `SIG_UNBLOCK`: 将 `set` 指向的信号集中的信号从进程当前信号掩码中移除
> + `SIG_SETMASK`: 将 `set` 指向的信号集中的信号拷贝给进程的当前信号掩码
> 

> [!tip] 参数 `set`: 指向一个信号集
> 
> 根据参数 `how` 使用该信号集改变进程的信号掩码
> 

> [!tip] 参数 `oldset`: 指向一个信号集缓存区
> 
> 如果 `oldset` 不为 `NULL`，`sigprocmask()` 调用将返回之前的信号掩码
> 

如果需要暂时阻塞信号传递，首先使用一系列调用来 **阻塞信号**，然后再将信号掩码 **重置为先前的状态** 以解除对信号的锁定。

```c
sigset_t blockSet, prevMask;

// 初始化信号集
sigemptyset(&blockSet);
sigemptyset(&prevMask);

sigaddset(&blockSet, SIGINT);

// 阻塞进程接收 SIGINT；并获取进程的上一个掩码
if(sigprocmask(SIG_BLOCK, &blockSet, &prevMask) == -1) {
	// 错误处理
}

/*
 此处的代码不会被 SIGINT 信号中断
*/

// 恢复为进程的原来的信号掩码
if(sigprocmask(SIG_BLOCK, &prevMask, nullptr) == -1) {
	// 错误处理
}
```

SUSv3 规定，如果有任何等待信号因对 `sigprocmask()` 的调用而解除了锁定，那么在此调用返回前至少会传递一个信号

> [!hint] 
> 
> **如果解除了对某个等待信号的锁定，那么会立刻将该信号传递给进程**
> 

注意，**系统会忽略试图阻塞 SIGKILL 和 SIGSTOP 信号的请求**。如果试图阻塞这些信号，`sigprocmask()` 函数既不会予以关注，也不会产生错。这意味着如果需要阻塞除了 `SIGKILL` 和 `SIGSTOP` 之外的所有信号，可以使用如下代码片段

```c
sigfillset(&blockSet);

if(sigprocmask(SIG_BLOCK, &blockSet, nullptr) == -1) {
	// 错误处理
}
```

## 等待信号集

如果某进程 **接受** 了一个该进程 **正在阻塞的信号**，那么会将该信号 **添加** 到进程的 **等待信号集** 中。当解除了对该信号的阻塞时，会随之将信号传递给此进程。

为了确定进程中处于等待状态的是哪些信号，可以使用 `sigpending()` 函数

```c
#include <signal.h>

int sigpending(sigset_t *set); 
/* 成功返回 0；错误返回 -1 */
```

> [!tip] 参数 `set`: 指向一个信号集缓冲区
> 
> `sigpending` 通过 `set` 返回进程正在等待的信号集
> 

如果修改了对等待信号的信号处理程序，那么当后来解除对信号的锁定时，将根据新的信号处理程序来处理信号

### 阻塞信号不排队

**等待信号集只是一个掩码，仅表明一个信号是否发生，而未表明其发生的次数**。换言之，如果 **同一信号在阻塞状态下产生多次**，那么会将该信号记录在等待信号集中，并在 **稍后仅传递一次**。也就是说，假设我们向进程发送了 $1000$ 次 `SIGINT`  信号，进程可能仅接收 $200$ 次

下面的两个程序用于观察未作排队处理的信号。`sig_sender.c` 用于向目标进程发送指定数量的一个信号

```c title:signals/sig_sender.c

#include <signal.h>

#include "base.h"

int main(int argc, char *argv[]) {
    if(argc < 4 || strcmp(argv[1], "--help") == 0) {
        usageErr("%s pid num-sigs sig-num [sig-num-2]\n", argv[0]);
    }
    pid_t pid = (pid_t)strtol(argv[1], nullptr, 10);  // 目标进程 PID
    int numSigs = (int)strtol(argv[2], nullptr, 10);   // 信号数量
    int sig = (int)strtol(argv[3], nullptr, 10);  // 信号编号

    printf("%s: sending signal %d to process %ld %d times\n", argv[0], sig, (long) pid, numSigs);

    for(int i = 0; i < numSigs; i++) {
        if(kill(pid, sig) == -1) {
            errExit("kill error: ");
        }
    }
    if(argc > 4) {
        if(kill(pid, (int)strtol(argv[4], nullptr, 10)) == -1) {
            errExit("kill error: ");
        }
    }
    printf("%s: exiting\n", argv[0]);
    exit(EXIT_SUCCESS);
}
```

`sig_receiver.c` 用于接收进程发来的所有信号并统计数量

```c title:signals/sig_receiver.c
#include <signal.h>
#include <unistd.h>

#include "base.h"
#include "signal_functions.h"

static int sigCnt[NSIG] = {};
static volatile sig_atomic_t gotSigint = 0;

static void handler(int sig) {
    if(sig == SIGINT) {
        gotSigint = 1;
    }
    else {
        sigCnt[sig]++;
    }
}

int main(int argc, char *argv[]) {
    printf("%s: PID is %ld", argv[0], (long)getpid());

    // 为每个信号注册信号处理器程序
    for(int n = 1; n < NSIG; n++) {
        (void) signal(n, handler);
    }
    if(argc > 1) {
        // 阻塞所有信号的秒数
        int numSecs = (int)strtol(argv[1], nullptr, 10);

        sigset_t blockSet;
        sigfillset(&blockSet);
        if(sigprocmask(SIG_BLOCK, &blockSet, nullptr) == -1) {
            errExit("sigprocmask error: ");
        }
        printf("%s: sleeping for %d seconds\n", argv[0], numSecs);
        sleep(numSecs);

        sigset_t pendingSet;
        if(sigpending(&pendingSet) == -1) {
            errExit("sigpending: ");
        }
        printSigset(stdout, "\t\t", &pendingSet);

        sigset_t emptySet;
        sigemptyset(&emptySet);

        if(sigprocmask(SIG_SETMASK, &emptySet, nullptr) == -1) {
            errExit("sigprocmask: ");
        }
    }

    while (!gotSigint) {}  // 循环等待 SIGINT

    for(int i = 1; i < NSIG; i++) {
        if(sigCnt[i] != 0) {
            printf("%s: signal %d caught %d times%s\n", argv[0], i, sigCnt[i], sigCnt[i] == 1 ? "" : "s");
        }
    }
    return EXIT_SUCCESS;
}
```

`sig_receiver.c` 会等待其他进程发送 `SIGINT` 时结束，并打印接收到信号的次数。下面的 shell 会话展示了这两个程序的使用

```shell
➜  build ./sig_receiver &
[1] 236434
➜  build ./sig_sender
usage: ./sig_sender pid num-sigs sig-num [sig-num-2]

➜  build ./sig_sender 236434 1000000 10 2
./sig_sender: sending signal 10 to process 236434 1000000 times
./sig_sender: exiting
./sig_receiver: PID is 236434./sig_receiver: signal 10 caught 486538 timess
[1]  + 236434 done       ./sig_receiver
```

如果让 `sig_receiver.c` 阻塞所有信号 $15$ 秒，那么只会接收到 $10$ 号信号 $1$ 次

```shell
➜  build ./sig_receiver  15 &
[1] 236520
./sig_receiver: PID is 236520./sig_receiver: sleeping for 15 seconds
➜  build ./sig_sender 236520 1000000 10 2
./sig_sender: sending signal 10 to process 236520 1000000 times
./sig_sender: exiting
➜  build                2(Interrupt)
                10(User defined signal 1)
./sig_receiver: signal 10 caught 1 times

[1]  + 236520 done       ./sig_receiver 15
```

## 改变信号处置

在 [[Linux 系统编程：信号基本概念]] 中介绍了 `signal()` 系统调用用于设置信号处理程序。此外，还存在一个功能更丰富的系统调用 `sigaction()`

> [!tip] `sigaction()` 的额外功能
> + 获取信号处置的同时无需修改
> + 设置各种属性对调用信号处理器程序的行为进行控制
> 

此外，`sigaction()` 的可移植性要强于 `signal()`，程序中更建议使用 `sigaction()`

```c
#include <signal.h>

int sigaction(int sig, const struct sigaction *act, struct sigaction *oldact);
/* 成功返回 0；错误返回 -1*/
```

> [!tip] 参数 `sig`：信号的编号
> 除了 `SIGKILL` 和 `SIGSTOP` 之外的所有信号都可以
> 

> [!tip] 参数 `act`：指向 `sigaction` 结构的指针
> 
> `sigaction` 结构定义如下
> 
> ```c
> struct sigaction
> {
> 	void(*sa_handler)(int);     // 信号处理函数的地址
> 	sigset_t sa_mask;           // 信号处理函数调用时阻塞的信号
> 	int sa_flags;               // 信号处理程序的调用标志
> 	void (*sa_restorer)(void);  // 不适用于应用程序，仅供内部使用
> };
> ```

`sigaction.sa_handler` 函数指针，指向信号处理器程序的地址
 
`sigaction.sa_mask` 调用信号处理器程序时需要阻塞的信号集

`sigaction.sa_flags` 信号处理过程中的各种选项。下列选项取位或

> [!tip] `sg_flags` 可使用的预定以宏，取多值进行位或
> 
> + `SA_NOCLDSTOP`：若 `sig` 为 `SIGCHLD` 信号，则当因接受一信号而停止或恢复某一子进程时，将不会产生此信号。参考 [[Linux 系统编程：监控子进程#SIGCHLD 信号]]
> + `SA_NOCLDWAIT`：若 `sig` 为 `SIGCHLD` 信号，则当子进程终止时不会将其转化为僵尸。参考 [[Linux 系统编程：监控子进程#SIGCHLD 信号]]
> + `SA_NODEFER`：捕获信号 `sig` 时，不会在执行处理器程序时将 `sig` 信号自动添加到进程掩码中
> + `SA_ONSTACK`：针对此信号调用处理器函数时，使用了由 `sigaltstack()` 安装的备选栈
> + `SA_RESETHAND`：当捕获信号 `sig` 时，会在调用处理器函数之前将信号处置重置为默认值（即 `SIG_DFL`）
> 	+ 默认情况下，信号处理器函数保持建立状态，直至进一步调用 `sigaction()` 将其显式解除。
> + `SA_RESTART`: 自动重启由信号处理器程序中断的系统调用
> + `SA_SIGINFO`: 调用信号处理器程序时携带了额外参数
> 

下表对 `sa_flags` 的取值进行了总结

| `sa_flags`     | 描述                                      |
| :------------- | --------------------------------------- |
| `SA_NOCLDSTOP` | 参考 [[Linux 系统编程：监控子进程#SIGCHLD 信号#取消发送 SIGCHLD 信号]] |
| `SA_NOCLDWAIT` | 参考 [[Linux 系统编程：监控子进程#SIGCHLD 信号]]                 |
| `SA_NODEFER`   | 不自动将 `sig` 添加到进程信号掩码中                   |
| `SA_ONSTACK`   | 在备选栈上执行信号处理器程序                          |
| `SA_RESETHAND` | 调用信号处理器程序前重置为默认值                        |
| `SA_RESTART`   | 重启被信号处理器程序中断的系统调用                       |
| `SA_SIGINFO`   | 调用信号处理器程序是携带了额外参数                       |

## 等待信号

调用 `pause()` 将暂停进程的执行，直至信号处理器函数中断该调用为止（或者直至一个未处理信号终止进程为止）

```c
#include <unistd.h>

int pause(void);
/* 总是返回 -1 */
```

> [!tip] 
> 
> 处理信号时，`pause()` 遭到中断，并总是返回`−1`，并将 `errno` 置为 `EINTR`
> 

借助于 `pause()`，进程可暂停执行，直至信号到达为止
