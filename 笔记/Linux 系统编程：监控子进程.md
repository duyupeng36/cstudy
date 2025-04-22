# 监控子进程

父进程需要知道其某个子进程于何时改变了状态

> [!tip] 
> 
> 子进程终止或因收到信号而停止
> 

有两种方式监控子进程：系统调用 `wait` 和信号 `SIGCHLD`

## 等待子进程

对于许多需要创建子进程的应用来说，父进程能够监测子进程的终止时间和过程是很有必要的。`wait()` 以及若干相关的系统调用提供了这一功能。
### wait()

系统调用 `wait()` 等待调用进程的任一 **子进程终止**，同时在参数 `status` 所指向的缓冲区中返回该子进程的终止状态

```c
#include <sys/wait.h>

pid_t wait(int *status);
/** 成功返回终止进程的 PID；失败返回 -1 /
```

系统调用 `wait()` 执行如下动作

> [!tip] 
> 
> 如果调用 `wait()` 时**没有子进程终止**，则 **阻塞** 直到某个子进程终止。如果调用 `wait()` 时已有子进程终止，则立即返回
> 
> 如果 `status` 非空，那么关于子进程如何终止的信息则会通过 `status` 指向的整型变量返回
> 
> 内核将会为父进程下所有子进程的运行总量追加进程 **CPU 时间** 以及资源使用数据
> 
> 将终止子进程的 PID 作为 `wait()` 的结果返回
> 

出错时，`wait()` 返回 $-1$。可能的错误原因之一是 调用 `wait()` 的进程没有子进程，此时会将 `errno` 置为 `ECHILD`。换言之，可使用如下代码中的循环来等待调用进程的所有子进程退出

```c
while(pid = wait(NULL) != -1) {
	continue;
}

// 发送错误：没有任何子进程
if(errno != ECHILD) {
	// 错误处理
}
```

> [!important] 
> 
> `wait()` 每次返回只会等待一个子进程，如果调用 `wait()` 的进程有多个子进程，应该在循环中调用 `wait()`
> 

下面的例程演示了 `wait()` 的用法。该程序 **创建多个子进程**，每个子进程对应一个命令行参数(整数)，代表该子进程应该 `sleep` 的秒数，然后直接退出。在创建所有的子进程之后，**父进程循环调用 `wait()` 来监控这些子进程的终止**。而直到 `wait()` 返回 $-1$ 时才会退出循环

```c
#include <unistd.h>
#include <sys/wait.h>

#include "base.h"
#include "current_time.h"

int main(int argc, char *argv[]) {
    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        usageErr("%s <sleep seconds> ...", argv[0]);
    }

    // 由于 stdio 的缓冲区在用户空间，fork 会复制，这里关闭 stdio 的缓冲
    setvbuf(stdout, nullptr, _IONBF, 0);

    for(int i = 1; i < argc; i++) {
        switch (fork()) {
            case -1:
                errExit("fork");
            case 0:
                printf("[%s] child %d started with PID %ld, sleep %s seconds\n", current_time(), i, (long)getpid(),argv[i]);
                sleep(strtoul(argv[i], nullptr, 10));
                exit(EXIT_SUCCESS); 
                // 子进程睡眠结束后直接退出，否则子进程也会再创建子进程
            default:
                break;
        }
    }
    
    // 循环等待所有子进程结束
    int numDead = 0;
    while (true) {
        pid_t pid = wait(nullptr);
        if(pid == -1) {
	        // 如果没有任何子进程，wait 返回 -1 并将 errno 设置为 ECHILD
            if(errno == ECHILD) {
                printf("No more child processes - bye!\n");
                exit(EXIT_SUCCESS);
            }
            errExit("wait");
        }
        numDead++;
        printf("[%s] wait() returned child PID %ld (numDead = %d)\n", current_time(), (long)pid, numDead);
    }
}
```

上述例程编译运行的结果为

```shell
$ ./multi_wait 1 2 3 4 5 6 7 8 9 10
[2024-11-03 15:35:42] child 1 started with PID 2066705, sleep 1 seconds
[2024-11-03 15:35:42] child 2 started with PID 2066706, sleep 2 seconds
[2024-11-03 15:35:42] child 7 started with PID 2066711, sleep 7 seconds
[2024-11-03 15:35:42] child 3 started with PID 2066707, sleep 3 seconds
[2024-11-03 15:35:42] child 8 started with PID 2066712, sleep 8 seconds
[2024-11-03 15:35:42] child 9 started with PID 2066713, sleep 9 seconds
[2024-11-03 15:35:42] child 4 started with PID 2066708, sleep 4 seconds
[2024-11-03 15:35:42] child 5 started with PID 2066709, sleep 5 seconds
[2024-11-03 15:35:42] child 6 started with PID 2066710, sleep 6 seconds
[2024-11-03 15:35:42] child 10 started with PID 2066714, sleep 10 seconds
[2024-11-03 15:35:43] wait() returned child PID 2066705 (numDead = 1)
[2024-11-03 15:35:44] wait() returned child PID 2066706 (numDead = 2)
[2024-11-03 15:35:45] wait() returned child PID 2066707 (numDead = 3)
[2024-11-03 15:35:46] wait() returned child PID 2066708 (numDead = 4)
[2024-11-03 15:35:47] wait() returned child PID 2066709 (numDead = 5)
[2024-11-03 15:35:48] wait() returned child PID 2066710 (numDead = 6)
[2024-11-03 15:35:49] wait() returned child PID 2066711 (numDead = 7)
[2024-11-03 15:35:50] wait() returned child PID 2066712 (numDead = 8)
[2024-11-03 15:35:51] wait() returned child PID 2066713 (numDead = 9)
[2024-11-03 15:35:52] wait() returned child PID 2066714 (numDead = 10)
No more child processes - bye!
```
### waitpid()

`wait()` 系统调用存在诸多限制

+ `wait()` **无法等待某个特定的子进程完成**，只能按顺序等待下一个子进程终止
+ `wait()` 会一直 **阻塞直到有子进程退出**
+ `wait()` 只能发现那些已经 **终止** 的子进程。子进程因为某个信号 **停止**(例如，`SIGSTOP` 或 `SIGTTIN`)，或者已经停止子进程收到 `SIGCONT` 信号恢复执行后 `wait()` 就无法监视了 

为了解决上述限制，设计了 `waitpid()`系统调用

```c
#include <sys/wait.h>

pid_t waitpid(pid_t pid, int *status, int options);
/* 成功返回子进程的 PID; 失败返回 -1; 其他情况返回 0 */
```

`waitpid()` 与 `wait()` 的返回值以及参数 `status` 的意义相同。下面我们介绍 参数 `pid` 和 `options`

> [!tip] 参数 `pid`
> 
> + 如果 `pid` 大于 $0$，表示等待进程ID为 `pid` 的子进程
> + 如果 `pid` 等于 $0$，表示等待与调用进程（父进程）同一个 **进程组** 的所有子进程
> + 如果 `pid` 小于 $-1$，表示等待 **进程组标识符** 与 `pid` 绝对值相等的所有子进程
> + 如果 `pid` 等于 $-1$，表示 **等待任意子进程**。`wait(&status)` 等价于 `waitpid(-1, &status, 0)`
> 

> [!tip] 参数 `options`
> 
> 参数 `options` 是一个位掩码，用于指定 `waitpid` 的行为。取值参考下表
> 

| options                        | 描述                         |
| :----------------------------- | -------------------------- |
| `WUNTRACED: (Wait Untraced)`   | 等待终止的或者因信号停止的子进程           |
| `WCONTINUED: (Wait Continued)` | 等待因 `SIGCONT` 而恢复执行的已停止子进程 |
| `WNOHANG: (Wait No Hang)`      | 非阻塞等待                      |

> [!important] 
> 
> 如果指定 `options` 中的位掩码包含了 `WNOHANG`，那么通过参数 `pid` 指定的子进程状态未发生变化，`waitpid()` 会立即返回
> 
> 也就是，`WNOHANG` 告诉 `waitpid()` 执行非阻塞等待。如果被等待的子进程正在执行，`waitpid` 返回 $0$。如果指定的 `pid` 不存在子进程，则返回 $-1$ 并将 `errno` 设置为 `ECHILD`
> 

> [!tip] 返回值
> 
> `waitpid()` 返回值与 `wait()` 类似。但是，`waitpid()` 执行非阻塞等待时，可以返回 $0$，其原因是被等待的子进程正在执行 
> 

下面的程序创建了一个子进程，该子进程会循环调用 `pause()`（在此期间可以向子进程发送信号），但如果在命令行中指定了整型参数，则子进程会立即退出，并以该整型值作为退出状态。同时，父进程通过 `waitpid()` 监控子进程，打印子进程返回的状态值并将其作为参数传递给 `print_wait_status()`。一旦发现子进程已正常退出，亦或因某一信号而终止，父进程会随即退出

```c title:procexec/t_waitpid.c
#include <sys/wait.h>
#include <unistd.h>

#include "base.h"

int main(int argc, char *argv[]) {
    int status;

    pid_t childPid;

    if(argc > 1 && strcmp(argv[1], "--help") == 0) {
        usageErr("%s [exit-status]\n", argv[0]);
    }

    switch(fork()) {
        case -1:
            errExit("fork");
        case 0:  // 子进程
            printf("Child started with PID = %ld\n",(long) getpid());
            if(argc > 1) {
                exit((int)strtol(argv[1], nullptr, 10));
            }
            for(;;) {
                pause();
            }
            exit(EXIT_FAILURE);
        default:
            for(;;) {
                //waitpid(-1, &status, WUNTRACED|WCONTINUED): 
                // 等待任意子进程，并且监控因信号停止和恢复的子进程
                childPid = waitpid(-1, &status, WUNTRACED
                #ifdef __WIFCONTINUED
                | WCONTINUED  // 在旧版本的 Linux 中不存在
                #endif
                );
                if(childPid == -1) {
                    fprintf(stderr, "waitpid() failed:%s\n", strerror(errno));
                    exit(EXIT_FAILURE);
                }
                // 打印 status 的十六进制值
                printf("waitpid() returned: PID=%ld; status=%#04x(%d, %d)\n",
                    (long)childPid,
                    (unsigned int)status,
                    status >>8,
                    status & 0xff);
                print_wait_status(nullptr, status);
                // 子进程正常终止或被信号终止
                if(WIFEXITED(status) || WIFSIGNALED(status))
                {
                    exit(EXIT_SUCCESS);
                }
            }
    }
}

```

> [!tip] 
> 
> 上述例程中的 `print_wait_status()` 函数在下面的内容中介绍
> 

### 等待状态

由 `wait()` 和 `waitpid()` 返回的 `status` 的值，可用来区分以下子进程事件

> [!summary] 
> 
> + 子进程调用`_exit()`或 `exit()` 而终止，并指定一个整型值作为退出状态
> + 子进程收到未处理信号而终止
> + 子进程因为信号而停止，并以 `WUNTRACED` 标志调用 `waitpid()`
> + 子进程因收到信号 `SIGCONT` 而恢复，并以 `WCONTINUED` 标志调用 `waitpid()`
> 

术语 **等待状态** 来涵盖上述所有情况，而使用 **终止状态** 的称谓来指代前两种情况。

虽然将变量 `status` 定义为整型（`int`），但实际上 **仅使用了其最低的 $2$ 个字节**。对这 $2$ 个字节的填充方式取决于子进程所发生的具体事件

![[Pasted image 20241103160400.png]]


头文件 `<sys/wait.h>` 定义了用于解析等待状态值的一组标准宏。下表列出来这些宏

| 宏函数                                        | 描述                    |
| :----------------------------------------- | :-------------------- |
| `WIFEXITED(status)` `With IF EXITED`       | 子进程是否正常退出             |
| `WEXITSTATUS(status)`                      | 获取子进程的退出状态            |
|                                            |                       |
| `WIFSIGNALED(status)` `With IF SIGNALED`   | 子进程是否被信号终止            |
| `WTERMSIG(status)`                         | 获取终止子进程的信号编号          |
| `WCOREDUMP(status)`                        | 子进程是否产生核心转储文件         |
|                                            |                       |
| `WIFSTOPPED(status)` `With IF STOPPED`     | 子进程是否被信号停止            |
| `WSTOPSIG(status)`                         | 获取停止子进程的信号变化          |
|                                            |                       |
| `WIFCONTINUED(status)` `With IF CONTINUED` | 子进程是否被 `SIGCONT` 信号恢复 |

尽管上述宏的参数也以 `status` 命名，不过 **此处所指只是简单的整型变量**，而非像 `wait()` 和 `waitpid()` 所要求的那样是指向整型的指针

下面的函数 `print_wait_status()` 用于显示子进程的等待状态

```c
#include <sys/wait.h>
#include <unistd.h>

#include "base.h"


/**
 * @msg: 消息
 * @status: 信号
*/
void print_wait_status(const char *msg, int status)
{
    if(msg != NULL)
    {
        printf("%s", msg);
    }
    // W: With; IF: if; EXITED: exited -> W-IF-EXITED:
    // 检查进程是否为正常终止
    if(WIFEXITED(status))
    {
        printf("child exited, status=%d\n", WEXITSTATUS(status)); // W-EXIT-STATUS
    }
    // W: With; IF: if; SIGNALED: signaled -> W-IF-SIGNALED
    // 检查进程是否被信号杀死
    else if (WIFSIGNALED(status))
    {
        printf("child killed by signal %d(%s)\n", WTERMSIG(status), strsignal(WTERMSIG(status)));
    }

#ifdef __USE_MISC  // 在SUSv3中没有，在某些系统上可能不存在
    // W: With; CORE: core; EDUMP: dump -> W-CORE-EDUMP: 
    // 检查进程是否以核心转储
    else if (WCOREDUMP(status))
    {
        printf("(core dumped)");
    }
#endif

    // W: With; IF: if; STOPPED: stopped -> W-IF-STOPPED: 
    // 检查进程是否被信号停止
    else if (WIFSTOPPED(status))
    {
        printf("child stopped by signal %d(%s)\n", WSTOPSIG(status), strsignal(WSTOPSIG(status)));
    }
#ifdef __WIFCONTINUED  // 自 Linux 2.6.10 之后开始支持该宏。
    // W: With; IF: if; CONTINUED: stopped -> W-IF-CONTINUED: 
    // 检查进程是否被信号重启
    else if (WIFCONTINUED(status))
    {
        printf("childe continued\n");
    }
#endif
    // 没有任何事发生
    else
    {
        printf("what happend to this child? (status=%x)\n", (unsigned int)status);
    }
}
```

### 信号处理程序中终止进程

默认情况下某些信号会终止进程。有时，可能希望在进程终止之前执行一些清理操作，可以设置一个 **信号处理程序** 来捕捉信号，随即执行清理步骤，再终止进程

> [!attention] 
> 
> 通过 `wait()` 和 `waitpid()`，父进程也可以获得子进程的终止状态。例如，在信号处理程序中调用 `_exit(EXIT_SUCCESS)`，父进程会认为子进程正常终止
> 

如果需要通知父进程自己因为某个信号而终止，那么子进程的信号处理程序应该首先将信号处理程序清理，然后再次发出相同的信号，该信号这次将终止子进程

```c
void handler(int sig) 
{
	// 将 sig 的处理程序设置为默认处理程序
	signal(sig, SIG_DFL);

	// 执行清理操作
	
	// 再次发出 sig 信号
	raise(sig);
}
```

### waitid()

与 `waitpid()` 类似，`waitid()` 返回 **子进程的状态**。不过，`waitid()` 提供了 `waitpid()` 所没有的扩展功能

```c
#include <sys/wait.h>

int waitid(idtype_t idtype, id_t id, siginfo_t *infop, int options);
/* 成功或者没有子进程返回 0，失败返回 -1 */
```

> [!tip] 参数 `idtype` 和 `id` 指定需要等待哪些子进程
> 

| idtype   | 描述                          |
| :------- | --------------------------- |
| `P_ALL`  | 等待任意子进程，忽略参数 `id`           |
| `P_PID`  | 等待 PID 为参数 `id` 的进程         |
| `P_PGID` | 等待进程组 ID 为参数 `id` 的进程组中的子进程 |

> [!tip] 参数 `options` 指定 `waitid()` 的行为
> 

| options                      | 描述                       |
| :--------------------------- | :----------------------- |
| `WEXITED(With EXITED)`       | 等待已终止的子进程，无论是否正常返回       |
| `WSTOPPED(With STOPPED)`     | 等待已通过信号而停止的子进程           |
| `WCONTINUED(With CONTINUED)` | 等待经由信号 `SIGCONT` 而恢复的子进程 |
|                              |                          |
| 附加标识                         |                          |
| `WNOHANG(With NO HANG)`      | 非阻塞获取子进程的状态              |
| `WNOWAIT(With NO WAIT)`      | 获取子进程状态，子进程依然处于可等待的状态    |

> [!tip] 参数 `infop`：执行一个存储子进程状态的结构
> 
> `siginfo_t` 是包含子进程相关信息的结构
> 

下表列出来 `siginfo_t` 中一些值得关注字段

| 字段          | 描述                                                                                                                                |
| :---------- | --------------------------------------------------------------------------------------------------------------------------------- |
| `si_code`   | `CLD_EXITED`：子进程调用 `_exit()` 终止<br>`CLD_KILLED`：子进程被某个信号杀死<br>`CLD_STOPPED`：子进程被某个信号暂停<br>`CLD_CONTINUED`：被暂停的子进程被 `SIGCONT` 恢复执行 |
| `si_pid`    | 状态发生变化子进程的 PID                                                                                                                    |
| `si_signo`  | 总是将该字段置为 `SIGCHLD`                                                                                                                |
| `si_status` | 要么是传递给 `_exit()` 的终止状态<br>要么是导致子进程停止、继续或终止的信号值<br>通过 `si_code` 字段区分                                                               |
| `si_uid`    | 子进程的 **真实用户ID**                                                                                                                   |

> [!important] 
> 
> 如果在 `options` 中指定了 `WNOHANG`，那么 `waitid()` 返回 $0$ 意味着以下两种情况之一
> 
> + 在调用时子进程的状态已经改变，并且子进程的状态信息存储在了 `infop` 指向的结构中
> + 没有任何子进程的状态发生改变
> 

> [!question] 
> 如何区分指定 `WNOHANG` 选项下 `waitid()` 返回 $0$ 是发生那种情形呢？
> 
> + 某些 Unix 实现将 `infop` 指向的 `siginfo_t` 结构使用 $0$ 填充
> + 某些 Unix 实现保持 `infop` 指向的 `siginfo_t` 结构中的内容保持不变
> 
> 然而，上述两种行为都没有在 SUSv3 标准中规范。一种可移植方式就是在调用 `waitip()` 之前将 `siginfo_t` 结构填充为 $0$。这样，无论 Unix 实现采用那种行为，`siginfo_t` 结构中的所有字段的值都是 $0$
>   

```c
siginfo_t info;

memset(&info, 0, sizeof(siginfo_t));

if(waitid(idtyp, id, &info, options | WNOHANG) == -1) {
	// 错误处理
}

if(info.si_pid == 0) {
	// 没有子进程状态发生改变
} else {
	// 子进程状态发生改变，子进程的状态信息存储在 infop 中
}
```

### wait3() 和 wait4()

系统调用 `wait3()` 和 `wait4()` 执行与 `waitpid()` 类似的工作。主要的语义差别在于，`wait3()` 和 `wait4()` 在参数 `rusage` 所指向的结构中 **返回终止子进程的资源使用情况**。包括进程使用的 `CPU` 时间总量以及内存管理的统计数据

```c
#define _BSD_SOURCE  /* #define _XOPEN_SOURCE 500 for wait3 */
#include <sys/resource.h>
#include <sys/wait.h>

pid_t wait3(int *status, int options, struct rusage *rusage);
pid_t wait4(pid_t pid, int *status, int options, struct rusage *rusage);
/* 成功返回子进程的 PID；失败返回 -1 */
```

除了对参数 `rusage` 的使用之外，调用 `wait3()` 等同于以如下方式调用 `waitpid()`

```c
waitpid(-1, &status, options);
```

与之相类似，对 `wait4()` 的调用等同于对 `waitpid()` 的如下调用

```c
waitpid(pid, &status, options);
```

>[!ssummary] 
>+ `wait3()` 等待的是任意子进程
>+ `wait4()` 则可以用于等待选定的一个或多个子进程
>

在一些 `UNIX` 实现中，`wait3()` 和 `wait4()` 仅返回已终止子进程的资源使用情况。而对于 `Linux` 系统，如果在 `options` 中指定了 `WUNTRACED` 选项，则还可以获取到停止子进程的资源使用信息

## 僵尸进程和孤儿进程

**父进程与子进程的生命周期一般都不相同**，父子进程间互有长短。这就引出了下面两个问题

> [!tip] 孤儿进程
> 
> 如果父进程先于子进程退出，则子进程成为 **孤儿进程**。
> 

当一个进程称为孤儿进程后，哪个进程会是孤儿进程的父进程呢？我们通过 `ps -elf` 命令查看内核启动后创建进程的顺序

```shell
$ ps -elf
F S UID          PID    PPID  C PRI  NI ADDR SZ WCHAN  STIME TTY          TIME CMD
4 S root           1       0  0  80   0 -  5596 -      Sep21 ?        00:08:10 /usr/lib/systemd/systemd --system --deser
1 S root           2       0  0  80   0 -     0 -      Sep21 ?        00:00:00 [kthreadd]
```

内核启动创建的第一个进程便是 $0$ 号进程，它只是创建出了 $1$ 号和 $2$ 号进程。$2$ 号进程只负责创建内核进程，而 $1$ 号进程负责系统初始化，管理用户进程

> [!tip] $1$ 号进程是系统初始化进程
> 
> Unix/Linux 使用的初始化系统有两套：`init` 初始化和 `systemd` 初始化
> 

初始化进程会创建一个终端进程，用于提供给用户登录。当用户被成功验证登录后，终端进程会创建出 `shell` 进程。下图展示了内核进程创建过程

![[Pasted image 20241103181910.png]]

后续用户启动的进程都是通过 `shell` 启动。因此，**$1$ 号进程是所有用户进程的祖先**。**孤儿进程由 $1$ 号进程收养**

> [!important] 
> 
> 孤儿进程由 `init` 进程，即 $1$ 号进程收养。收养时机时在其父进程 **终止时立即** 被 $1$ 号进程收养
> 

> [!tip] 僵尸进程
> 
> 父进程在执行 `wait()` 或 `waitpid()` 之前，子进程就终止了。内核将终止的子进程转换 **僵尸进程**(zombie)，从而允许在子进程结束后，其父进程可以在某一个时刻执行 `wait()` 从而确定子进程是如何退出的
> 
> 内核会释放僵尸进程持有的大部分资源，以便供其他进程重新使用。**唯一保留的是内核进程表中的一条记录**。该记录中保存了子进程的 PID，终止状态，记账信息等
> 

至于 **僵尸进程** 名称的由来，则源于 UNIX 系统对电影情节的效仿：**无法通过信号来杀死僵尸进程**，即便是 `SIGKILL`。这就确保了父进程总是可以执行 `wait()` 方法

当父进程执行 `wait()` 后，由于不再需要子进程所剩余的最后信息，故而内核将删除僵尸进程。另一方面，如果父进程未执行 `wait()` 随即退出，那么 `init` 进程将接管子进程并自动调用 `wait()`，从而从系统中移除僵尸进程

**如果父进程创建了某一子进程，但并未执行 `wait()`，那么在内核的进程表中将为该子进程永久保留一条记录**。如果存在大量此类 **僵尸进程**，它们势必将填满内核进程表，从而阻碍新进程的创建。**既然无法用信号杀死僵尸进程，那么从系统中将其移除的唯一方法就是杀掉它们的父进程（或等待其父进程终止），让僵尸进程变成了孤儿僵尸进程**，此时 **$1$ 号进程** 将接管和等待这些僵尸进程，从而从系统中将它们清理掉

## SIGCHLD 信号


+ [x] 🔺 ➕ 2024-11-03  首先需要了解什么是信号？如何捕获信号？此外，还需要了解信号信号处理函数的设计规则。详细内容参考 [[Linux 系统编程：信号基本概念]]  [[Linux 系统编程：信号处理器函数]] [[Linux 系统编程：信号高级特性]]  ✅2024-11-10
### 为 SIGCHLD 注册信号处理器函数

子进程的终止属异步事件。父进程无法预知其子进程何时终止

> [!tip] 
> 
> 即使父进程向子进程发送 `SIGKILL` 信号，子进程终止的确切时间还依赖于系统的调度：子进程下一次在何时使用CPU
> 

父进程应使用 `wait()`（或类似调用）来防止僵尸子进程的累积，以及采用如下两种方法来避免这一问题

> [!tip] 调用 `wait()` 或不带 `WNOHANG` 标志调用 `waitpid()`
> 
> 此时，如果尚无子进程终止，父进程会陷入阻塞。可能并不希望以阻塞的方式等待子进程终止
> 

> [!tip] 以 `WNOHANG` 标志调用 `waitpid()` 
> 
> 执行针对已终止子进程的非阻塞式检查，即 **轮询**。反复调用非阻塞的 `waitpid()` 会造成 CPU 资源浪费
> 

---

由于子进程终止，内核会向父进程发送 `SIGCHILD` 信号。因此，**可以为 `SIGCHILD` 信号建立信号处理器程序，在信号处理器函数中回收子进程**

> [!tip] `SIGCHLD` 信号属于标准信号，不会对阻塞的 `SIGCHILD` 进行排队处理
> 
> 在 [[Linux 系统编程：信号集]] 中我们知道，当调用信号处理器程序时，**会暂时将引发调用的信号阻塞起来**，且 **不会对 `SIGCHLD` 之流的标准信号进行排队处理**
> 

因此，当 `SIGCHILD` 信号处理程序正在为一个终止的子进程运行时，如果相继有两个子进程终止，即使产生了两次 `SIGCHLD` 信号，父进程也只能捕获到一个。结果是，如果父进程的 `SIGCHLD` 信号处理程序每次只调用一次 `wait()`，那么一些僵尸子进程可能会成为“漏网之鱼”

> [!warning] 
> 
>  `SIGCHILD` 在被阻塞时，可能产生多次，但是只会会被传递一次。这样可能导致 `wait()` 调用次数比子进程终止个数少，使得系统中存留僵尸进程
> 

解决方案是：在 `SIGCHLD` 处理程序内部循环以 `WNOHANG` 标志来调用 `waitpid()`，直至再无其他终止的子进程需要处理为止。通常 `SIGCHLD` 处理程序都简单地由以下代码组成，仅仅捕获已终止子进程而不关心其退出状态

```c
while(waitpid(-1, NULL, WNOHANG) > 0) {
	continue;
}
```

上述循环会一直持续下去，直至 `waitpid()` 返回 $0$，表明再无僵尸子进程存在，或 $-1$，表示有错误发生（可能是 `ECHILD`，意即再无更多的子进程）。

---

假设创建 `SIGCHLD` 处理程序的时候，该进程已经有子进程终止。那么内核会立即为父进程产生 `SIGCHLD` 信号吗？SUSv3 对这一点并未规定。

> [!tip] System V 的实现会立即产生 `SIGCHLD`

> [!tip] 包括 Linux 在内的其他实现，不会立即产生 `SIGHLD`

为保障可移植性，应用应**在创建任何子进程之前就设置好 SIGCHLD 处理程序**，将这一隐患消解于无形。（无疑，这也是顺其自然的处事之道。）

> [!warning] 注意信号处理器函数的可重入性
> 
> 在信号处理器函数内部调用 `waitpid()` 等系统调用会修改全局变量 `errno`。如果主程序中需要检查 `errno` 可能会出现竞争条件。
> 
> 解决 `errno` 产生的竞争条件方案之一：**进入信号处理器函数时立即保存 `errno`，并在退出信号处理器函数时恢复**
> 

下面的示例程序演示了父进程通过 `SIGCHLD` 信号回收子进程

```c title:signal/multi_SIGCHLD.c
#include <current_time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include  <unistd.h>
#include <signal.h>

#include "base.h"

static volatile int numLiveChildren = 0;  // 子进程存活的数量

static void handleSIGCHLD(int sig) {
    int saveErrno = errno;  // 第一步：保存 errno 值

    printf("%s handler: Caught SIGCHLD\n", current_time());
    pid_t childPid;
    int status;
    while ((childPid = waitpid(-1, &status, WNOHANG)) > 0) {
        printf("%s handler: Reaped child %ld -", current_time(), (long)childPid);
        print_wait_status(nullptr, status);  // 输出子进程退出状态
        numLiveChildren--;
    }
    // waitpid 失败，并且 不是因为 ECHILD
    if(childPid == -1 && errno != ECHILD) {
        errExit("waitpid error: ");
    }
    sleep(5);
    printf("%s handler: returning\n", current_time());

    // 恢复错误码
    errno = saveErrno;
}


int main(int argc, char *argv[]) {
    // 关闭进程的 stdout 缓冲
    setvbuf(stdout, nullptr, _IONBF, 0);

    if(argc < 2 || strcmp(argv[1], "--help") == 0) {
        usageErr("%s child-sleep-seconds ...\n", argv[0]);
    }


    numLiveChildren = argc - 1;  // 当前存活的子进程
    int sigCnt = 0;  // 收到信号的数量

    // 注册信号处理函数
    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = handleSIGCHLD;
    if(sigaction(SIGCHLD, &sa, nullptr) == -1) {
        errExit("sigaction error: ");
    }

    // 首先阻塞 SIGCHLD，防止父进在开始调用 sigsuspend() 之前终止，从而传递信号
    sigset_t blockMask;
    sigemptyset(&blockMask);
    sigaddset(&blockMask, SIGCHLD);
    if(sigprocmask(SIG_BLOCK, &blockMask, nullptr) == -1) {
        errExit("sigprocmask error: ");
    }

    // 开始创建子进程
    for(int i = 1; i < argc; i++) {
        switch (fork()) {
            case -1:
                errExit("fork error: ");
            case 0:
                // 子进程 sleep argv[i] 秒后结束
                sleep(strtol(argv[i], nullptr, 10));
                printf("%s : Child[PID %ld, num %d] existing\n", current_time(), (unsigned long)getpid(), i);
                _exit(EXIT_SUCCESS);
            default:  // 父进程循环的创建子进程
                break;
        }
    }

    sigset_t emptyMask;
    sigemptyset(&emptyMask);
    while (numLiveChildren > 0) {
        // 暂时解除对 SIGCHLD 的阻塞。每次接收的 SIGCHLD 都会被中断，返回 -1 并设置 errno 为 EINTR
        if(sigsuspend(&emptyMask) == -1 && errno != EINTR) {
            errExit("sigsuspend error: ");
        }
        sigCnt++;
    }
    printf("%s All %d children have terminated; SIGCHLD was caught %d times\n", current_time(), argc -1, sigCnt);
    exit(EXIT_SUCCESS);
}
```

### 取消子进程因停止时发送 SIGCHLD 信号

当信号导致子进程停止时，父进程也就有可能收到 `SIGCHLD` 信号。然而，当调用 `sigaction()` 注册 `SIGCHLD` 信后的处理器函数时，如果指定了 `SA_NOCLDSTOP` 标志可以控制该行为

> [!tip] `SA_NOCLDSTOP` 标志：子进程因信号停止，内核不向其父进程发送 `SIGCHLD` 信号
> 
> 注意，`signal()` 未实现 `SA_NOCLDSTOP` 标志
> 

> 默认情况下会忽略信号 `SIGCHLD`，`SA_NOCLDSTOP` 标志仅在设置 `SIGCHLD` 信号处理程序时才有意义

SUSv3 也允许，当信号 `SIGCONT` 导致已停止的子进程恢复执行时，向其父进程发送 `SIGCHLD` 信号

### 忽略已终止的子进程

更有可能像这样处理终止子进程：将对 `SIGCHLD` 的信号处理器显式置为 `SIG_IGN`，系统从而会将其后终止的子进程立即删除，毋庸转为僵尸进程。这时，会将子进程的状态弃之不问，故而所有后续的 `wait()` 调用不会返回子进程的任何信息

> [!tip] `SIGCHLD` 信号的处置 **显示设置** 为 `SIG_IGN`，内核会将终止的子进程立即删除
> 
> 虽然对信号 `SIGCHLD` 的默认处置就是将其忽略，但显式设置对 `SIG_IGN` 标志的处置还是会导致这里所描述的行为差异
> 

> [!warning] 
> 
> 显示设置 `SIGCHLD` 的处置为 `SIG_IGN` 从而将 **已终止** 的子进程删除依赖于实现的
>  
> + 许多 Unix 或 Linux 系统中，将对 `SIGCHLD` 信号的处置置为 `SIG_IGN` 并不会影响任何既有僵尸进程的状态
> + 另外一些 Unix 实现中，将 `SIGCHLD` 信号的处置置为 `SIG_IGN` 确实会删除所有已有的僵尸进程
> 


SUSv3 规定了 `SA_NOCLDWAIT` 标志，可在调用 `sigaction()` 对 `SIGCHLD` 信号的处置进行设置时使用此标志。设置该标志的作用类似于将对 `SIGCHLD` 的处置置为 `SIG_IGN` 时的效果

> [!tip] `SA_NOCLDWAIT` 标志：子进程停止立即删除，不会产生僵尸进程。Linux 2.6 才实现该特性
> 
> 子进程终止时是否向其父进程发送 `SIGCHLD` 信号并没有规定。也就是说，当指定 `SA_NOCLDWAIT` 时允许系统发送 `SIGCHLD` 信号，则应用程序即可捕捉这一信号
> + 尽管由于内核已经丢弃了僵尸进程，造成 `SIGCHLD` 处理程序无法用 `wait()` 来获得子进程状态
> 
