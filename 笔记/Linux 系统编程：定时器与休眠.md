# 定时器与休眠

**定时器** 是进程规划自己 **在未来某一时刻接获通知的一种机制**。休眠则能使进程或线程 **暂停** 执行一段时间

## 间隔定时器

### 创建间隔定时器

系统调用 `setitimer()` 创建一个 **间隔式定时器**，这种定时器会在 **未来某个时间点到期**，并于此后 **每隔一段时间到期一次**

```c
#include <sys/time.h>

int setitimer(int which, const struct itimerval *new_vale, struct itimerval *old_value);

/* 成功返回 0；失败返回 -1 */
```

首先了解参数 `new_value` 和 `old_value`，它们都是 `itimerval` 结构的指针。`itimerval` 结构的定义如下

```c
struct itimerval
{
	struct timeval it_interval;   // 周期计时器的时间间隔
	struct timeval it_value;      // 当前值到下一次过期时间
};
```

结构 `itimervl` 的两个字段均是 `timeval` 类型，该类型在 [[Linux 系统编程：时间#日历时间]] 中介绍，`timeval` 类型有两个字段分别代表 **秒** 和 **微秒**

```c
struct timeval {
	time_t tv_sec;        // 秒
	suseconds_t tv_usec;  // 微秒
};
```

> [!tip] 参数 `new_value`：指定间隔定时器的间隔
> 
> 字段 `it_interval` 说明该定时器是否为周期性定时器。如果 `it_interval` 有一个字段不为 $0$，则该定时器时周期性定时器，周期间隔为 `it_interval` 字段指定的时间
> 
> + 如果是周期性定时器，那么每次定时器 **到期之后**，都会将定时器 **重置为在 _指定间隔_(`it_interval`) 后再次到期**
> 
> 字段 `it_value` 指定了距离定时器到期的延迟时间。如果 `it_value` 的两个字段均为 $0$，则会屏蔽任何已有的定时器
> 
> 

> [!tip] 参数 `old_value`：获取定时器的前一个设置，即上次设置的 `new_value`
> 
> 若参数 `old_value` 不为 `NULL`，则以其所指向的 `itimerval` 结构来返回定时器的前一设置
> 
> + `old_value.it_value` 的两个字段均为 $0$，该定时器之前处于屏蔽状态
> + `old_value.it_interval` 的两个字段均为 $0$，该定时器之前被设置为经历 `old_value.it_value` 指定时间而到期的一次性定时器
> 
> 

间隔定时器的工作流程如下图

![Pasted image 20241110210122|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755783979138-927a221e0f344fe9aefb080fdb6620a4.png)

+ 当调用 `setitimer()` 时，定时器会根据 `it_value` 的值开始倒计时
+ 如果倒计时结束，会根据定时器的类型向进程发送一个信号（如 `SIGALRM`）
+ 如果 `it_interval` 为非零，则倒计时会自动重置为 `it_interval` 的值，继续倒计时，形成周期性触发信号
+ 如果 `it_interval` 为零，则计时器到期后不再重置，不会再触发信号


> [!tip] 参数 `which`：控制定时器类型
> 
> 进程可以创建 $3$ 种定时器类型
> + `ITIMER_REAL`：真实时间定时器。到期后发送 `SIGALARM` 信号给进程
> + `ITIMER_VIRTUAL`：虚拟时间定时器。用户模式下的 CPU 时间倒计时的定时器。到期后发送 `SIGVTALRM` 信号给进程
> + `ITIME_PROF`: 性能分析定时器。以进程时间(用户态和内核态 CPU 时间和)倒计时。到期发送 `SIGPROF` 信号给进程
> 
> **进程只能拥有上述 $3$ 种定时器中的一种**。当第 $2$ 次调用 `setitimer()` 时，修改已有定时器的属性要符合 `which` 中的类型
> 

下表对 $3$ 种定时器类型进行了总结。间隔定时器到期时发送给进程的信号，默认处置都是 **终止进程**

| 类型               | 描述                             |
| :--------------- | ------------------------------ |
| `ITIMER_REAL`    | 真实时间定时器。到期发送 `SIGALARM` 信号给进程  |
| `ITIMER_VIRTUAL` | 虚拟时间定时器。到期发送 `SIGVRALRM` 信号给进程 |
| `ITIMER_PROF`    | 性能分析定时器。到期发送 `SIGPROF` 信号给进程   |

> [!attention] 
> 
> `setitimer()` 和 `alam()` 创建的定时器 **在 `exec()` 后得以保存**。但是，`fork()` **子进程不会继承该定时器**
> 

### 获取间隔定时器状态

可以在任何时刻调用 `getitimer()`，以了解定时器的当前状态、距离下次到期的剩余时间

```c
#include <sys/time.h>

int getitimer(int which, struct itimerval *curr_value);
/* 成功返回 0; 失败返回 -1 */
```

> [!tip] 参数 `which`：指定定时器类型
> 

> [!tip] 参数 `curr_value`：指向 `itimerval` 结构的缓冲区
> 
> 通过 `curr_value` 指针返回 `which` 指定定时器的当前状态
> + `curr_value.it_value`：距离下一次到期 **剩余的总时间**
> + `curr_value.it_interval`：定时器的 **时间间隔**
> 

`curr_value` 与 `setitimer()` 的 `old_value` 返回的信息完全相同，区别在于 `getitimer()` 无需改变定时器的设置

### alarm() 

系统调用 `alarm()` 为 **创建一次性真实时间定时器提供了一个简单接口**

```c
#include <unistd.h>

unsigned int alarm(unsigned int seconds);
/* 总是成功的。返回上一次设置定时的剩余时间，没有设置定时器则返回 0*/
```

> [!tip] 参数 `seconds`：表示定时器到期的秒数
> 
> 到期时，会向调用进程发送 `SIGALRM` 信号
> 

## 为阻塞操作设置超时

**真实时间定时器** 的用途之一是 **为某个阻塞系统调用设置其处于阻塞状态的时间上限**

例如，当用户在一段时间内没有输入整行命令时，可能希望取消对终端的 `read()` 操作。处理如下

> [!tip] 第一步：调用 `sigaction()` 为 `SIGALRM` 信号注册信号处理器函数
> 
> 排除 `SA_RESTART` 标志以 **确保系统调用不会重新启动**
> 

> [!tip] 第二步：设置真实时间定时器
> 
> 调用 `alarm()` 或 `setitimer()` 来创建一个定时器，同时设定希望系统调用阻塞的时间上限
> 

> [!tip] 第三步：调用可能阻塞的系统调用，例如 `read()` 套接字
>

> [!tip] 第四步：屏蔽定时器
> 
> 系统调用返回后，再次调用 `alarm()`或 `setitimer()`以屏蔽定时器（以防止系统调用在定时器到期之前就已完成的情况）

> [!tip] 第五步：检查系统调用失败的原因是否为 `EINTR`
> 
> 检查系统调用失败时是否将 `errno` 置为 `EINTR`（系统调用遭到中断）
> 

## 暂停

暂停进程或线程的执行，可以使用两个函数

```c
#include <unistd.h>

unsigned int sleep(unsigned int seconds);
/*休眠完成返回 0；中断则返回剩余为暂停时间*/

int nanosleep(const struct timespec *request, struct timespec *remain);
/* 休眠完成返回 0；错误或中断返回 -1*/
```

> [!tip] 
> 
> `sleep()` 和 `nanosleep()` 的区别在于精度问题。`nanosleep()` 可以指定高分辨的时间 
> 

`nanosleep()` 的参数 `request` 指定了休眠的持续时间，是一个指向如下结构的指针

```c
struct timespec
{
	time_t tv_sec;
	long tv_nsec;
};
```

> [!important] 
> 
> SUSv3 明文规定了不得使用信号来实现 `nanosleep()`。因此 `nanosleep()` 可以与 `alarm()` 或 `setitimer()` 混用
> 

某些实现中，`sleep()` 使用 `alarm()` 和 `pause()` 两个系统调用实现
