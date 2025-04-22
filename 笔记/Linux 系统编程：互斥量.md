# 保护共享变量的访问：互斥量

互斥量是 Pthreads API 中用于线程同步的工具。线程同步主要涉及两个工具：**互斥量** 和 **条件变量**

> [!tip] **互斥量**：可以帮助线程同步对共享资源的使用
> 
> 互斥量可以防如下情况发生：线程某甲试图访问一共享变量时，线程某乙正在对其进行修改
> 

> [!tip] **条件变量**：允许线程相互通知共享变量（或其他共享资源）的状态发生了变化
> 
> 必须和互斥量一起使用
> 

线程的优势就是 **能够通过全局变量来共享信息**。不过，这种便捷的共享是有代价的：必须确保 **多个线程不会同时修改同一变量**，或者 **某一线程不会读取正由其他线程修改的变量**。程序代码中访问共享资源的代码片段称为 **临界区**。任何时刻只有一个线程执行临界区内的代码，当线程正在执行临界区代码时不能被其他线程中断。也就是说，**临界区的执行时原子的**

> [!tip] **临界区**：访问共享资源的代码片段
> 
> 指访问某一共享资源的代码片段，并且这段代码的执行应为 **原子** 操作
> 
> 换句话说就是，**同时访问同一共享资源的其他线程不应中断该片段的执行**
> 

如下示例程序，展示了以非原子方式访问共享资源时发生的问题。该例程创建两个线程执行同一个函数。该函数执行一个循环(循环次数通过命令行参数指令)，重复以下步骤
+ 将 `glob` 复制到本地变量 `loc`
+ `loc` 增加
+ 把 `loc` 复制回 `glob`

```c title:threads/thread_incr.c

#include <pthread.h>

#include "base.h"

static int glob = 0;

void *incr(void *arg) {
    const int loops = *(int *)arg;
    for(int i = 0; i < loops; i++) {
        int loc = glob;
        loc++;
        glob = loc;
    }
    return nullptr;
}


int main(int argc, char *argv[]) {
    if(argc != 2 || strcmp(argv[1], "--help") == 0) {
        usageErr("%s loops\n", argv[0]);
    }

    long loops = strtol(argv[1], nullptr, 10);
    pthread_t tids[2];
    int err = pthread_create(tids, nullptr, incr, &loops);
    if(err != 0) {
        fprintf(stderr, "pthread_create() failed with error %s\n", strerror(err));
        return -1;
    }
    err = pthread_create(tids+1, nullptr, incr, &loops);
    if(err != 0) {
        fprintf(stderr, "pthread_create() failed with error %s\n", strerror(err));
        return -1;
    }
    err = pthread_join(tids[0], nullptr);
    if(err != 0) {
        fprintf(stderr, "pthread_join() failed with error %s\n", strerror(err));
        return -1;
    }
    err = pthread_join(tids[1], nullptr);
    if(err != 0) {
        fprintf(stderr, "pthread_join() failed with error %s\n", strerror(err));
        return -1;
    }
    printf("glob = %d\n", glob);
    return 0;
}
```

下面的 shell 会话展示了不同循环次数下全局变量 `glob` 的值

```shell
➜  build ./thread_incr 100
glob = 200
➜  build ./thread_incr 1000
glob = 2000
➜  build ./thread_incr 10000
glob = 16836
➜  build ./thread_incr 10000
glob = 10398
➜  build ./thread_incr 10000
glob = 13328
```

当循环次数增加到 $10000$ 时，每次执行的得到的结果均不相同，并且结果还是不正确的。执行到最后，`glob` 的值本应为 $2$ 万。问题的原因是由于线程的执行序列如下图导致的


![[Pasted image 20241111203752.png]]

假设全局变量 `blog` 的当前值为 $2000$。线程 $1$ 和线程 $2$ 按如下顺序执行

+ 线程 $1$ 将 `glob` 值赋给局部变量 `loc`。此时，线程 $1$ 的时间片期满，线程 $2$ 开始执行

+ 线程 $2$ 执行多次循环：将变量 `glob` 的置于局部变量 `loc`，递增 `loc`，再将结果写回变量 `glob`。假设线程 $2$ 时间片到期时，`glob` 的值增加到 $3000$

+  线程 $1$ 获得另一个时间片，并 **从上次停止处恢复执行**。此时 `loc` 的值为 $2000$，现在递增 `loc`，再将 `loc` 的值 $2001$ 写回 `glob`。此时，线程 $1$ 将线程 $2$ 的结果覆盖了

上述两个线程的执行顺序引发了 **竞争条件**：多个线程或进程 **并发访问和修改** 共享资源时，**结果依赖于操作的执行顺序或调度顺序** 的不确定性

> [!warning] 竞争条件：程序的结果依赖于操作的执行顺序或调度顺序，可能导致意料之外的错误行为
> 

为了避免更新共享变量时出现的竞争条件，必须使用 **互斥量(mutual exclusion, mutex)** 来确保同时仅有一个线程可以访问某个共享资源

> [!tip] 互斥量：保证执行临界区的线程只有一个
> 
> 更全面的说法是，互斥量能 **保证对任意共享资源的原子访问**
> 

互斥量有 $2$ 种状态：**已锁定(locked)** 和 **未锁定(unlocked)**

> [!important] 
> 
> 任何时候，至多只有一个线程可以锁的该互斥量。视图对已锁定的互斥量再次加锁，可能阻塞线程或者报错失败
> 

**一旦线程锁定互斥量，随即成为该互斥量的所有者**。**只有所有者才能给互斥量解锁**。这一属性改善了使用互斥量的代码结构，也顾及到对互斥量实现的优化。因为所有权的关系，有时会使用术语 **获取(acquire)** 和 **释放(release)** 来替代加锁和解锁。

一般情况下，对每一共享资源，可能由多个相关变量组成，会使用不同的互斥量，每一线程在访问同一资源时将采用如下协议

> [!tip] 线程访问同一资源时采用如下协议
> 
> + 首先，针对共享资源锁定互斥量
> + 然后，访问共享资源
> + 最后，对互斥量解锁
> 

如果多个线程试图执行这一代码块（一个临界区），事实上只有一个线程能够持有该互斥量（其他线程将遭到阻塞），即同时只有一个线程能够进入这段代码区域。如下图所示

![[Pasted image 20241111212505.png]]

## 初始化和销毁互斥量

互斥量既可以像静态变量那样分配，也可以在运行时动态创建

### 静态初始化

互斥量是属于 `pthread_mutex_t` 类型的变量。**在使用之前必须对其初始化**。对于静态分配的互斥量而言，将 `PTHREAD_MUTEX_INITIALIZER` 赋给互斥量即可

```c
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
```

> [!warning] 
> 
> SUSv3 标准规定，**对某一互斥量的副本执行加锁和解锁操作将导致未定义行为**。这些操作只能施加于如下两类互斥量本身
> + `PTHREAD_MUTEX_INITIALIZER` 初始化的静态互斥量
> + 经由 `pthread_mutex_init()` 初始化的动态互斥量
> 

### 动态初始化

静态初始值 `PTHREAD_MUTEX_INITIALIZER`，只能用于对 **经由静态分配且携带默认属性** 的互斥量进行初始化：。其他情况下，必须调用 `pthread_mutex_init()` 对互斥量进行动态初始化

```c
#include <pthread.h>

int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr);
/* 成功返回 0；错误返回 errno*/
```

> [!tip] 参数 `mutex`：初始化操作的目标互斥量

> [!tip] 参数 `attr`：施加在目标互斥量上的属性的指针
> 
> 互斥量属性的类型为 `pthread_mutexattr_t`
> 

> [!warning]
> SUSv3 规定，**初始化一个已初始化的互斥量将导致未定义的行为**，应当避免这一行为
> 

在如下情况下，必须使用函数 `pthread_mutex_init()`，而非静态初始化互斥量

+ 动态分配于堆中的互斥量
+ 互斥量是在栈中分配的自动变量
+ 初始化经由静态分配，且不使用默认属性的互斥量

当不再需要经由自动或动态分配的互斥量时，应使用 `pthread_mutex_destroy()` 将其销毁

```c
#include <pthread.h>

int pthread_mutex_destroy(pthread_mutex_t *mutex);
/* 成功返回 0；失败返回 errno */
```

> [!attention] 销毁互斥量的一些注意事项
> 
> 只有当互斥量处于 **未锁定** 状态，且后续也 **无任何线程企图锁定** 它时，将其**销毁才是安全的**
> 
> 若互斥量驻留于动态分配的一片内存区域中，应在释放此内存区域前将其销毁
> 
> 对于自动分配的互斥量，也应在宿主函数返回前将其销毁
> 
> 经由 `pthread_mutex_destroy()` 销毁的互斥量，可调用 `pthread_mutex_init()` 对其重新初始化
> 

## 加锁和解锁互斥量

初始化之后，互斥量处于未锁定状态。函数 `pthread_mutex_lock()` 可以锁定某一互斥量，而函数 `pthread_mutex_unlock()` 则可以将一个互斥量解锁

```c
#include <pthread.h>

int pthread_mutex_lock(pthread_mutex_t *mutex);
int pthread_mutex_unlock(pthrea_mutex_t *mutex);
/* 成功返回 0；错误返回 errno */
```

> [!tip] 参数 `mutex`：指向 `pthread_mutex_t` 类型变量的指针
> 

要锁定互斥量就需要在调用 `pthread_mutex_lock()` 时通过参数 `mutex` 指定互斥量。根据互斥量的当前状态，`pthread_mutex_lock()` 产生不同的行为
+ 互斥量当前处于 **未锁定状态** 时，该调用将 **锁定互斥量并立即返回**
+ 互斥量当前处于 **锁定状态** 时，该调用将 **阻塞**，直到其他线程释放该互斥量

> [!warning] 在 Linux 上，线程两次锁定同一个目标互斥量，线程会发生死锁
> 
> 如果调用 `pthread_mutex_lock()` 的线程已然锁的了目标互斥量，对于互斥量的默认类型而言，不同的实现的行为可能不同
> + 某些实现可能会直接失败
> + 某些实现可能会陷入 **死锁**
>   

函数 `pthread_mutex_unlock()` 将解锁之前已被调用线程锁定的互斥量

> [!warning] 解锁的两个错误行为
> 
> + 解锁处于未锁定的互斥量
> + 解锁其他线程锁定的互斥量
> 

### 示例程序

使用互斥量修改 `thread_incr.c` 中的代码，确保能得到正确的结果

```c title:threads/thread_incr_mutex.c

#include <pthread.h>

#include "base.h"

static int glob = 0;

// 声明互斥量，并初始化
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *incr(void *arg) {
    const int loops = *(int *)arg;
    for(int i = 0; i < loops; i++) {
        // 加锁
        int err = pthread_mutex_lock(&mutex);
        if(err != 0) {
            fprintf(stderr, "pthread_mutex_lock() failed with error %s\n", strerror(err));
            exit(EXIT_FAILURE);
        }
        int loc = glob;
        loc++;
        glob = loc;
        // 解锁
        err = pthread_mutex_unlock(&mutex);
        if(err != 0) {
            fprintf(stderr, "pthread_mutex_unlock() failed with error %s\n", strerror(err));
            exit(EXIT_FAILURE);
        }
    }
    return nullptr;
}


int main(int argc, char *argv[]) {
    if(argc != 2 || strcmp(argv[1], "--help") == 0) {
        usageErr("%s loops\n", argv[0]);
    }

    long loops = strtol(argv[1], nullptr, 10);
    pthread_t tids[2];
    int err = pthread_create(tids, nullptr, incr, &loops);
    if(err != 0) {
        fprintf(stderr, "pthread_create() failed with error %s\n", strerror(err));
        return -1;
    }
    err = pthread_create(tids+1, nullptr, incr, &loops);
    if(err != 0) {
        fprintf(stderr, "pthread_create() failed with error %s\n", strerror(err));
        return -1;
    }
    err = pthread_join(tids[0], nullptr);
    if(err != 0) {
        fprintf(stderr, "pthread_join() failed with error %s\n", strerror(err));
        return -1;
    }
    err = pthread_join(tids[1], nullptr);
    if(err != 0) {
        fprintf(stderr, "pthread_join() failed with error %s\n", strerror(err));
        return -1;
    }
    printf("glob = %d\n", glob);
    return 0;
}
```

编译运行上述程序，可以看到无论循环多少次最终结构都能保持正确

```shell
➜  build ./thread_incr_mutex 10000
glob = 20000
➜  build ./thread_incr_mutex 10000000
glob = 20000000
```

此外，Pthreads API 提供了 `pthread_mutex_lock()` 的两个变体 `ptread_mutex_trylock()` 和 `pthread_mutex_timedlock()`

```c
#include <pthread.h>
int pthread_mutex_trylock(pthread_mutex_t *mutex);
/* 成功返回 0; 失败返回 errno */
```

`pthread_mutex_trylock()` 执行非阻塞加锁，如果参数 `mutex` 指向的互斥锁已锁定，那么 `pthread_mutex_trylock()` 失败并返回 `EBUSY` 错误

```c
#include <pthread.h>
#include <time.h>

int pthread_mutex_timedlock(pthread_mutex_t * mutex,  const struct timespec * abstime);
/* 成功返回 0; 失败返回 errno */
```

`pthread_mutex_timedlock()` 通过参数 `abstime` 指定一个线程等待获取互斥量时休眠的时间限制。如果参数 `abstime` 指定的时间间隔期满，而调用线程又没有获得互斥量的所有权，那么 `pthread_mutex_timedlock()` 返回 `ETIMEDOUT`

## 互斥量的死锁

有时，一个线程需要 **同时访问多个不同的共享资源**，并且 **每个资源由不同的互斥量管理**。当超过一个线程加锁同一组互斥量时，就有可能发生死锁

下图展示了两个线程分别锁定两个互斥量所导致的死锁的例子

![[Pasted image 20241111232243.png]]

线程 $A$ 持有互斥量 $\text{mutex1}$，线程 $B$ 持有互斥量 $\text{mutex2}$。此时，线程 $A$ 请求互斥量 $\text{mutex2}$，由于互斥量 $\text{mutex2}$ 被线程 $B$ 持有并且等待线程 $A$ 释放互斥量 $\text{mutex1}$。线程 $B$ 请求互斥量 $\text{mutex1}$，由于互斥量 $\text{mutex1}$ 被线程 $A$ 持有并且等待线程 $B$ 释放互斥量 $\text{mutex2}$。这样就形成了 **循环等待**，从而导致两个线程间无限期等待下去，也就是 **死锁**

> [!tip] 死锁的 $4$ 个必要条件：**只有当下面 $4$ 个条件同时成立时才会引起死锁**
> 
> + **互斥(mutual exclusion)**：共享资源一次只能被一个进程或线程使用
> + **占用并等待(hold and wait)**：进程或线程必须占用一个共享资源，并等待另一个共享资源
> + **非抢占(no preemption)**：进程或线程持有的资源不能被抢占
> + **循环等待(circular wait)**：有一组等待进程或线程$\{T_0, T_1, \cdots, T_{n}\}$，$T_0$ 等待的资源为 $T_1$ 占有，$T_1$ 等待的资源为 $T_2$ 占有，$\cdots$，$T_{n-1}$ 等待的资源为 $T_n$ 占有，$T_n$ 等待的资源为 $T_0$ 占有
> 

解决死锁的方法就是打破上述四个条件中的任何一个就能避免死锁。更详细的内容参考 [[死锁]]

## 互斥量的属性

POSIX 线程库中用于定义 **互斥锁属性** 的数据类型是 `pthread_mutexattr_t`。通过设置 `pthread_mutexattr_t` 的属性，可以 **控制互斥锁的行为**，以适应不同的并发场景

互斥锁属性对象主要用于配置 **互斥锁的类型**、**共享属性** 等。设置好属性后，可以在创建互斥锁时使用这些属性

在使用 `pthread_mutexattr_t` 之前，必须先初始化它；使用完后再销毁：

```c
#include <pthread.h>

int pthread_mutexattr_init(pthread_mutexattr_t *attr);
int pthread_mutexattr_destroy(pthread_mutexattr_t *attr);
/* 成功返回 0；失败返回 errno */ 
```
+ 函数 `pthread_mutexattr_init()` 用于初始互斥量属性
+ 函数 `pthread_mutexattr_destroy()` 用于销毁互斥量属性

初始化完成之后就可以通过 `pthread_mutex_init()` 函数的参数 `attr` 使用，控制互斥量的属性

我们只会讨论互斥量的一个属性：**类型**

## 互斥量类型

先前通过默认互斥量讨论的互斥量的行为
+ 同一线程不应对同一互斥量加锁两次
+ 线程不应对不为自己所拥有的互斥量解锁（亦即，尚未锁定互斥量）
+ 线程不应对一尚未锁定的互斥量做解锁动作

上述行为都是默认互斥量的行为。不同的 **互斥量类型** 可能有不同的结果。SUSv3 定义下面几个互斥量类型

| 互斥量类型                      | 描述        |
| :------------------------- | :-------- |
| `PTHREAD_MUTEX_NORMAL`     | 常规互斥量，默认的 |
| `PTHREAD_MUTEX_ERRORCHECK` | 带错误检查的互斥量 |
| `PTHREAD_MUTEX_RECURSIVE`  | 递归互斥量     |

> [!tip] 常规互斥量: `PTHREAD_MUTEX_NORMAL`。默认的互斥量类型
> 
> 该类型的互斥量 **不具有死锁检测（自检）功能**。也就是说
> + 线程试图对已由自己锁定的互斥量加锁，则发生死锁
> + 互斥量处于未锁定状态，或者已由其他线程锁定，对其解锁会导致不确定的结果
> 
> 在 Linux 上，对这类互斥量的上述两种操作都会成功
> 

> [!tip] 带错误检查的互斥量: `PTHREAD_MUTEX_ERRORCHECK`。每次操作均检查错误
> 
> 对此类互斥量的所有操作都会执行错误检查。也就是说，下面的三种行为都会导致 Pthreads 函数返回错误
> + 线程试图对已由自己锁定的互斥量加锁
> + 对未锁定状态的互斥锁解锁
> + 对其他线程持有的互斥锁解锁
> 

> [!tip] 递归互斥量： `PTHREAD_MUTEX_RECURSIVE`
> 
> 递归互斥量维护有一个 **锁计数器**。当线程第 $1$ 次取得互斥量时，会将锁计数器置 $1$。后续由同一线程执行的 每次加锁操作会递增锁计数器的数值，而 每次解锁操作则递减计数器计数。 **只有当锁计数器值降为 $0$ 时，线程才会释放该互斥量**
> 
> 对未锁定状态的互斥锁解锁 或 解锁其他线程持有的互斥锁的操作都会失败
> 
