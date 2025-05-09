# 优先级和调度

在 [[操作系统：进程调度]] 中介绍了几种进程调度算法。这里我们来看 Linux 内核提供的进程调度控制

## 优先级和 nice 值

Unix/Linux 系统调度进程使用 CPU 的默认模型是 **循环时间共享**。

Linux 的优先级总共的范围有 $140$ 个取值，其范围是 $-40$ 到 $99$，**优先级的数值越低，表示其优先级越高。**

Linux 内核中采取了两种类型的调度策略，分别是 **实时调度策略** 和 **普通调度策略**。在 [[操作系统：进程调度]] 中介绍的都是普通调度策略。

> [!tip] Linux 普通调度策略
> 
> 采用 **完全公平调度算法**，其实现效果与 **时间片轮转** 调度算法效果类似。也就是是说，**不一定保证某个进程会在规定的时间执行完毕**
> 

普通调度策略的优先级是从 $60 \sim 99$，一个用户进程在创建时默认的优先级就是 $80$。为了控制用户进程的优先级，采用 `nice` 值

> [!tip] `nice` 值
> 
> 进程的 `nice` 值是用于衡量该进程对其他进程是否友好
> + 如果进程的**优先级越高**，占用的 CPU 时间就越多，对其他进程就越不友好，**`nice` 值就越低**
> + 如果进程的**优先级越低**，占用的 CPU 时间就越少，对其他进程就越友好，**`nice` 值就越高**
> 
> 换句话说，就是进程的优先级越高，`nice` 值就越低；进程的优先级越低，`nice` 值就越高
> 

由于用户进程在创建时，其优先级默认是 $80$，而普通调度策略使用的优先级范围为 $60 \sim 99$，所以 `nice` 值的范围是 $-20 \sim 19$。下图展示了进程 `nice` 值的范围和解释

![[Pasted image 20241104111726.png]]

> [!important] 
> 
> 进程的调度不是严格按照 `nice` 值的层次进行的，**`nice` 值是一个权重因素，它导致内核调度器倾向于调度拥有高优先级的进程**
> 
> + 一个进程具有非常低的优先级（高 `nice` 值）并不会导致它完全无法用到 CPU，只是占用 CPU 的时间变少
> 

### 获取和修改优先级


`getpriority()` 和 `setpriority()` 系统调用允许一个进程获取和修改自身或其他进程的 `nice` 值

```c
#include <sys/resource.h>

int getpriority(int which, id_t who);
/* 成功返回调用进程的 nice 值，可能是负数；失败返回 -1*/

int setpriority(int which, id_t who, int prio);
/* 成功返回 0；失败返回 -1 */
```

> [!tip] 参数 `which` 和 `who`：标识需要读取和修改优先级的进程
> 
> `which` 参数确定 `who` 参数如何被解释。`which` 的取值是下表中的一个
> 

| `which`        | 描述                                                 |
| :------------- | -------------------------------------------------- |
| `PRIO_PROCESS` | 操作 PID 为 `who` 的进程。 `who` 为 $0$，操作调用进程             |
| `PRIO_PGRP`    | 操作 进程组 ID 为 `who` 中的所有进程。 `who` 为 $0$，操作调用者的进程组    |
| `PRIO_USER`    | 操作所有真实用户 ID 为 `who` 的进程。 `who` 为 $0$，使用调用者的真实用户 ID |

> [!important] 
> 
> `setpriority()` 会将 `which` 和 `who` 指定的进程的 `nice` 值设置为 `prio`。如果将 `nice` 值设置为超范围的值时，会直接使用最近的边界值
> 

特权进程能够修改任意进程的优先级。非特权进程可以 **修改自己** 的优先级和与自己有 **相同有效用 ID** 的进程的优先级

> [!attention] 
> 
> Linux 中 `setpriority()` 的权限规则与 SUSv3 中的规则不同，它规定当 **非特权进程的 _真实或有效用户 ID_ 与目标进程的 _有效用户 ID_ 匹配**时，该进程就能修改目标进程的优先级
> 

## 实时进程调度

在一个系统上一般会同时运行 **交互式进程(前台进程)** 和 **后台进程**，标准的内核调度算法一般能够为这些进程提供足够的 **性能** 和 **响应度**。但 **实时应用** 对调度器有更加严格的要求

> [!tip] 快速响应外部输入
> 
> 实时应用必须要为外部输入提供 **担保最大响应时间**。在很多情况下，这些担保最大响应时间必须非常短（如低于秒级）
> + 如交通导航系统的慢速响应可能会使一个灾难。
> 
> 为了满足这种要求，内核必须要提供工具让高优先级进程能快速地取得 CPU 的控制权，抢占当前运行的所有进程
> 

一些时间关键的应用程序可能需要采取其他措施来避免不可接受的延迟。例如，为了避免缺页而引发的延迟，应用程序可能会使用 `mlock()` 或 `mlockall()` 将虚拟内存锁定在 RAM 中

> [!tip] 互斥访问 CPU
> 
> 高优先级进程应该能够保持 **互斥地访问 CPU** 直至它完成或自动释放 CPU
> 

> [!tip] 精准控制组件进程的调度
> 
> 实时应用应该能够 **精确地控制其组件进程的调度顺序**
> 

SUSv3 规定的实时进程调度 API 部分满足了这些要求。这个 API 提供了两个实时调度策略：**SCHED_RR** 和 **SCHED_FIFO**。这两种策略调度进程的优先级高于标准的循环时间分享策略

> [!cite] 
> 
> 实时调度 API 使用常量 **SCHED_OTHER** 来标识这种循环时间分享策略
> 

**每个实时策略允许一个优先级范围**。SUSv3 要求实现至少要为实时策略提供 $32$ 个离散的优先级。在每个调度策略中，拥有高优先级的可运行进程在尝试访问 CPU 时总是优先于优先级较低的进程

> [!important] 
> 
> Linux 提供了 $99$ 个实时优先级，其数值从 $1$（最低）至 $99$（最高），并且这个取值范围同时适用于两个实时调度策略
> 

**每个策略中的优先级是等价的** 。这意味着如果两个进程拥有同样的优先级，一个进程采用了 `SCHED_RR` 的调度策略，另一个进程采用了 `SCHED_FIFO` 的调度策略，那么两个都符合运行的条件，至于到底运行哪个则取决于它们被调度的顺序了。

### SCHED_RR 策略

在 **SCHED_RR**（循环）策略中，**优先级相同的进程以循环时间分享的方式执行**。进程每次使用 CPU 的时间为一个固定长度的时间片。一旦被调度执行之后，使用 SCHED_RR 策略的进程会保持对 CPU 的控制直到下列条件中的一个得到满足

> [!tip] 使用 SCHED_RR 调度策略的进程放弃 CPU 的条件
> 
> + 达到时间片的终点了
> + 自愿放弃 CPU，这可能是由于执行了一个阻塞式系统调用或调用了 `sched_yield()`系统调用
> + 终止了
> + 被一个优先级更高的进程抢占了
> 

对于上面列出的前两个事件，当运行在 SCHED_RR 策略下的进程丢掉 CPU 之后将会被放置在与其优先级级别对应的队列的队尾

在最后一种情况中，当优先级更高的进程执行结束之后，被抢占的进程会继续执行直到其时间片的剩余部分被消耗完

> [!tip] 
> 
> 被抢占的进程仍然位于与其优先级级别对应的队列的队头
> 

> [!tip] 抢占发生的原因
> 
> + 之前被阻塞的高优先级进程解除阻塞了
> + 另一个进程的优先级被提到了一个级别高于当前运行的进程的优先级的优先级
> + 当前运行的进程的优先级被降低到低于其他可运行的进程的优先级了
> 

**SCHED_RR** 与标准时间片轮转类似，但是 **SCHED_RR 严格按照优先级，高优先级的进程总是优先于优先级较低的进程**。此外，**SCHED_RR 策略允许精确控制进程被调用的顺序**。

### SCHED_FIFO 策略

**SCHED_FIFO** 与 SCHED_RR 的差别在于 **SCHED_FIFO 没有时间片**，一旦一个 SCHED_FIFO 进程获得了 CPU 的控制权之后，它就会一直执行直到下面某个条件被满足

> [!tip] 使用 SCHED_FIFO 调度策略的进程放弃 CPU 的条件
> + 自动放弃 CPU
> + 终止了
> + 被一个优先级更高的进程抢占了

## 实时进程调度 API

### 实时优先级范围

`sched_get_priority_min()` 和 `sched_get_priority_max()` 系统调用返回一个调度策略的优先级取值范围

```c
#include <sched.h>

int sched_get_priority_min(int policy);
int sched_get_priority_max(int policy);
/* 成功返回非负整数；四百返回 -1 */
```

> [!tip] 参数 `policy`：指定调度策略
> 
> 一般取值是 `SCHED_RR` 和 `SCHED_FIFO`
> 

### 修改调度策略和优先级

`sched_setscheduler()` 系统调用修改进程 ID 为 `pid` 的进程的调度策略和优先级。如果 `pid` 为 $0$，那么将会修改调用进程的特性

```c
#include <sched.h>

int sched_setscheduler(pidt_t pid, int policy, const struct sched_param *param);
/* 成功返回 0；失败返回 -1 */
```

> [!tip] 参数 `param`：指向 `sched_param` 结构的指针
> 

```c
struct sched_param {
	int sched_priority;  // 调度优先级
};
```

> [!tip] 参数 `policy`：调度策略
> 
> 取值如下表
> 
> ![[Pasted image 20241104124748.png]]
> 

> [!important] 
> 
> 通过 `fork()` 创建的 **子进程会继承父进程的调度策略和优先级**，并且在 `exec()` 调用中会保持这些信息。
> 

`sched_setparam()` 系统调用提供了 `sched_setscheduler()` 函数的一个功能子集。它 **修改一个进程的调度策略**，但不会修改其优先级

```c
#include <sched.h>

int sched_setparam(pid_t pid, const struct sched_pram *param);
/* 成功返回 0；失败返回 - 1 */
```

> [!important] 
> 
> **进程必须要先变成特权进程才能够修改调度策略和优先级**
> 

### 获取调度策略和优先级

`sched_getscheduler()` 和 `sched_getparam()` 系统调用获取进程的调度策略和优先级

```c
#include <sched.h>

int sched_getscheduler(pid_t pid);
/* 成功返回调度策略；失败返回 -1 */

int sched_getparam(pid_t pid, struct sched_param * param);
/* 成功返回 0；失败返回 -1 */
```

### 释放 CPU 

**实时进程** 可以通过两种方式自愿释放 CPU：通过调用一个 **阻塞进程的系统调用** 或调用 `sched_yield()`。

```c
#include <sched.h>

int sched_yield(void);
/* 成功返回 0；失败返回 -1 */
```

### SCHED_RR 时间片

`sched_rr_get_interval()` 系统调用能够找出 `SCHED_RR` 进程在每次被授权使用 CPU 时分配到的时间片的长度

```c
#include <sched.h>

int sched_rr_get_interval(pid_t pid, struct timespec *tp);
/* 成功返回 0；失败返回 -1 */
```

## CPU 亲和力

当一个进程在一个多处理器系统上被重新调度时无需在上一次执行的 CPU 上运行。之所以会在另一个 CPU 上运行的原因是原来的 CPU 处于忙碌状态

> [!tip] 进程切换 CPU 对性能的影响：需要将原来 CPU 高速缓存中的数据加载的新 CPU 的高速缓存中
> 
> 为了将进程的一行数据加载进新 CPU 的高速缓冲器中，首先必须使这行数据失效
> + 在没有修改的情况下，丢弃数据
> + 在被修改的情况下，写入内存。原因在于：为了放在高速缓存不一致，在多处理器架构中，**某个时刻只允许数据被存放在一个 CPU 高速缓存中**
> 
> 写入内存的过程将花费时间。这就是切换 CPU 带来的性能影响
> 

Linux 内核尝试给进场保证软 **CPU 亲和力**

> [!tip] CPU 亲和力
> 
> 在条件允许的情况下进程重新被调度到原来的 CPU 上运行。
> 

有时候需要 **为进程设置硬 CPU 亲和力**，这样就能显式地 **将其限制在可用 CPU 中的一个或一组 CPU 上运行**

> [!important] 
> 
> 设置 CPU 亲和力的好处
> + 避免由使高速缓冲器中的数据失效所带来的性能影响
> 

`sched_setaffinity()` 系统调用设置了 `pid` 指定的进程的 CPU 亲和力。如果 `pid` 为 $0$，那么调用进程的 CPU 亲和力就会被改变。

```c
#define _GNU_SOURCE
#include <sched.h>
int sched_setaffinity(pid_t pid, size_t cpusetsize, const cpu_set_t *set);
/* 成功返回 0；四百返回 -1*/
```

赋给进程的 CPU 亲和力由 `set` 指向的 `cpu_set_t` 结构来指定

虽然 `cpu_set_t` 数据类型实现为一个位掩码，但应该将其看成是一个不透明的结构。对这个结构的操作都应该使用下表列出的宏

| 宏函数                    | 描述                   |
| :--------------------- | :------------------- |
| `CPU_ZERO(&set)`       | 将 `set` 初始化为空        |
| `CPU_SET(cpu, &set)`   | 将 `cpu` 添加到 `set` 中  |
| `CPU_CLR(cpu, &set)`   | 将 `cpu` 从 `set` 中移除  |
| `CPU_ISSET(cpu, &set)` | 判断 `cpu` 是否在 `set` 中 |

`sched_getaffinity()` 系统调用获取 `pid` 指定的进程的 CPU 亲和力掩码。如果 `pid` 为 $0$，那么就返回调用进程的 CPU 亲和力掩码

```c
#define _GNU_SOURCE
#include <sched.h>
int sched_getaffinity(pid_t pid, size_t cpusetsize, cpu_set_t *set);
/* 成功返回 0；失败返回 -1 */
```
