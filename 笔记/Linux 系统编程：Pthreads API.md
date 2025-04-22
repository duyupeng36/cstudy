# Pthreads API 介绍

$20$ 世纪 $80$ 年代末 $90$ 年代初，存在着数种不同的线程接口。$1995$ 年 POSIX.1c 对 POSIX 线程 API 进行了标准化，该标准后来为 SUSv3 所接纳

有几个概念贯穿整个 Pthreads API，在学在深入探讨 API 之前，将简单予以介绍

## Pthreads API 简介

### 线程数据类型

Pthreads API 定义了下表列出的几个数据类型

| 数据类型                  | 描述                             |
| --------------------- | ------------------------------ |
| `pthread_t`           | 线程 ID                          |
| `pthread_mutex_t`     | 互斥对象(`Mutex`)                  |
| `pthread_mutexattr_t` | 互斥属性对象                         |
| `pthread_cond_t`      | 条件变量(`condition variable`)     |
| `pthread_condattr_t`  | 条件变量的属性对象                      |
| `pthread_key_t`       | 线程特有数据的键(Key)                  |
| `pthread_once_t`      | 一次性初始化控制上下文(`control context`) |
| `pthread_attr_t`      | 线程的属性对象                        |

**SUSv3 并未规定如何实现这些数据类型，可移植的程序应将其视为 “不透明” 数据**。亦即，程序应避免对此类数据类型变量的结构或内容产生任何依赖。尤其是，不能使用 C 语言的比较操作符(`==`)去比较这些类型的变量

### 线程和 errno

在传统 UNIX  API 中，`errno` 是一全局整型变量。然而，这无法满足多线程程序的需要。

如果线程调用的函数通过全局 `errno` 返回错误时，会与其他发起函数调用并检查 `errno` 的线程混淆在一起。换言之，这将引发 **竞争条件**（race condition）

因此，在多线程程序中，每个线程都有属于自己的 `errno`。在 Linux 中，线程特有 `errno` 的实现方式与大多数 UNIX 实现相类似

> [!important] 线程特有 `errno` 的实现：`errno` 定义为宏，展开为函数
> 
> 将 `errno` 定义为一个宏，**可展开为函数调用**，该函数返回一个可修改的左值(`lvalue`)，且为每个线程所独有
> 
> 因为左值可以修改，多线程程序依然能以 `errno=value` 的方式对 `errno` 赋值
> 

`errno` 机制在保留传统 UNIX API 报错方式的同时，也适应了多线程环境

> [!attention] 
> 
> SUSv3 不允许程序将 `errno` 声明为 `extern int errno`。现如今，需要声明 `errno` 的程序必须包含`<errno.h>`，以启用对 `errno` 的线程级实现
> 

### Pthreads 函数返回值

系统调用和库函数中返回状态采用 **返回 $0$ 表示成功，返回 $-1$ 表示失败**，并设置 `errno` 以标识错误原因

Pthreads API 采用与其相反设计：**返回 $0$ 表示成功，返回整数标识错误原因** 。由于多线程程序对 `errno` 的每次引用都会带来函数调用的开销，因此我们使用中间变量来接受 Pthread API 的返回值，而不是对 `errno` 进行赋值

```c
pthread_t thread;
int err;

err = pthread_create(&thread, null, func, &arg);
if(err != 0) {
	// 错误处理
}
```

### 编译 Pthreads 程序

在 Linux 平台上，在编译调用了 Pthreads API 的程序时，需要设置 `cc -pthread` 的编译选项。使用该选项的效果如下

### Pthreads 的两种实现

在 Linux 操作系统中，线程实现经历了多个发展阶段，其中主要有 **LinuxThreads** 和 **NPTL (Native POSIX Thread Library)**

> [!tip] LinuxThreads：LinuxThreads 是 Linux 系统最早的 POSIX 线程实现，诞生于 1996 年。自 glibc 2.4 开始，该实现不在被支持
> 

> [!tip] NPTL：NPTL 是 Linux 中新的 POSIX 线程实现库，由 Red Hat 于 2003 年发布，并在 Linux 2.6 内核版本中集成
> 

NPTL 实现 **完全支持了 POSIX 标准**；通过**快速用户态互斥锁(`futex`)和条件变量实现同步**，降低了线程同步开销；提供更好的信号处理机制，信号可以被单个线程接收和处理；支持 **线程本地存储(TLS)**，运行每个线程拥有自己独立的本地存储空间

这两者都是所谓的 $1:1$ 实现，意味着 **每个线程对应一个内核调度实体**。两种线程实现都使用了 Linux 的 `clone()` 系统调用。在 NPTL 中，线程同步原语（如互斥锁、线程连接等）是通过 Linux 的 `futex()` 系统调用实现的

## 创建线程

启动程序时，产生的进程只有单条线程，称之为 **主线程**。函数 `pthread_create()` 负责创建一条新线程

```c
#include <pthread.h>

int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void*(*start)(void *), void *arg);

/* 成功返回 0；失败返回 errno */
```

> [!tip] 参数 `thread`：指向一个 `pthead_t` 类型的缓冲区，表示线程 ID
> 
> 参数 `thread` 指向 `pthread_t` 类型的缓冲区，在 `pthread_create()` 返回前，会在此保存一个该 **线程的唯一标识**。后续的 `Pthreads` 函数将使用该标识来引用此线程
>

> [!tip] 参数 `attr`：指向线程属性的指针
> 
> 参数 `attr` 是指向 `pthread_attr_t` 对象的指针，该对象指定了 **新线程的各种属性**
> 
> 如果将 `attr` 设置为 `NULL`，那么创建新线程时将使用各种默认属性
> 

> [!tip] 参数 `start`：函数指针。函数接收一个 `void *` 指针，并返回一个 `void *` 指针
>
> 线程函数的原型如下
> 
> ```c
> void * thread_function(void * );
> ```
> 
> 该函数的返回值类型为 `void *`，后续在 `pthread_join()` 中会介绍该返回值如何获取
> 

> [!tip] 参数 `arg`：传递给 `start` 指向函数的参数
> 
> 将参数 `arg` 声明为 `void *` 类型，意味着可以将 **指向任意对象的指针** 传递给 `start()` 函数
> 
> 一般情况下，`arg` 指向一个全局或堆变量，也可将其置为 `NULL`
> 
> 如果需要向 `start()` 传递多个参数，可以将 `arg` 指向一个结构，该结构的各个字段则对应于待传递的参数
> 
> 通过审慎的类型强制转换，`arg` 甚至可以传递 `int` 类型的值
> 

> [!tip] 返回值
> 
> 线程创建成功返回 $0$，创建失败返回错误码 `errno`
> 

SUSv3 标准明确支持，新线程可能会在 `pthread_create()` 返回之前就已经开始运行。应用程序无法确定系统会调度哪个线程来使用 CPU 资源。程序如隐含了特定调度顺序的依赖，那么非常容易出现竞争条件

下面例程展示了 `pthread_create()` 函数的使用

```c title:threads/t_pthread_create.c
#include <pthread.h>
#include <unistd.h>

#include "base.h"

void * routine(void *arg) {

    char *s = (char *)arg;
    sleep(2);
    printf("[TID %ld]: %s\n", (long)pthread_self(), s);
    return (void *)strlen(s);
}

int main() {

    // 创建线程
    pthread_t tid;
    int err = pthread_create(&tid, nullptr, routine, "hello, world\1");
    if(err != 0) {
        fprintf(stderr, "pthread_create: %s\n", strerror(err));
        return -1;
    }
    printf("[TID %lu]: create thread[TID %lu] success\n", pthread_self(), tid);

    // 连接线程：阻塞等待 tid 指定的线程返回。retval 获取返回值
    void *retval;
    err = pthread_join(tid, &retval);
    if(err != 0) {
        fprintf(stderr, "pthread_join: %s\n", strerror(err));
        return -1;
    }
    printf("[TID %lu]: thread[TID %lu] return value: %ld\n", pthread_self(), tid, (long)retval);
    return 0;
}
```

`pthread_join()` 用于连接 `tid` 指定的线程，该调用会阻塞等待 `tid` 线程结束

## 终止线程

线程终止可以如下几种方式
+ 线程函数执行 `retun` 语句
+ 线程调用 `pthread_exit()`
+ 调用 `pthread_cancel()` 取消线程
+ 任意线程调用 `exit()` 或者主线程在 `main()` 中执行 `return` 语句，都会导致进程中的所有线程立即终止

`pthread_exit()` 函数将终止调用线程，且其返回值可由另一线程通过调用 `pthread_join()` 来获取

```c
#include <pthread.h>

void pthread_exit(void *retval);
```

调用 `pthread_exit()` 相当于在线程的 `start` 函数中执行 `return`，不同之处在于，可在线程 `start` 函数所调用的任意函数中调用 `pthread_exit()` 

> [!tip] 参数 `retval`：指向线程的返回值对象的地址
> 
> 注意：`retval` 指向的内容不应该在线程栈上分配，因为线程终止后，无法确定线程栈的内容是否有效
> 
> 出于同样的理由，线程 `start` 函数的返回值也不应在线程栈中分配
> 

如果主线程调用了 `pthread_exit()`，而非调用 `exit()` 或是执行 `return` 语句，那么其他线程将继续运行

## 线程 ID

进程内部的 **每个线程都有一个唯一标识**，称为 **线程 ID**

线程 ID 会返回给 `pthread_create()` 的调用者，一个线程可以通过 `pthread_self()` 来获取自己的线程 ID

```c
#include <pthread.h>

pthread_t pthead_self();
/* 返回调用线程的线程 ID */
```

线程 ID 在程序中非常有用的。**利用线程 ID 来标识目标线程**；**利用线程 ID 标识某个数据结构属于某个线程**

函数 `pthread_equal()` 可检查两个线程的 ID 是否相同

```c
#include <pthread.h>

int pthread_equal(pthread_t t1, pthread_t t2);
/* t1 == t2 返回非 0 值; 否则返回 0 */
```

为了检查调用线程的线程 ID 与保存于变量 `t1` 中的线程 ID 是否一致，可以编写如下代码

```c
if(pthread_equal(t1, pthread_self())) {
	printf("tid matches self\n")
}
```

> [!attention] 
> 
> 必须将 `pthread_t` 作为一种不透明的数据类型加以对待，所以函数 `pthread_equal()` 是必须的
> 
> Linux 将 `pthread_t` 定义为无符号长整型，但在其他实现中，则有可能是一个指针或结构 
> 

## 连接已终止的线程

函数 `pthread_join()` 等待由 `thread` 标识的线程终止。如果线程已经终止，`pthread_join()` 会立即返回。这种操作被称为 **连接**

```c
#include <pthread.h>

int pthread_join(pthread_t thread, void **retval);
/* 成功返回 0；错误返回 errno */
```

> [!tip] 参数 `retval`：为一非空指针，将会保存 **线程终止时返回值的拷贝**
> 
> 由于线程函数执行 `return` 和 `pthread_exit()` 指定的值都是指针，因此 `retval` 是一个二级指针
> 

> [!warning] 向 `pthread_join()` 传入一个之前已然连接过的线程 ID，将会导致无法预知的行为
> 
> 相同的线程 ID 在参与一次连接后恰好为另一新建线程所重用，再度连接的可能就是这个新线程
> 

非分离的线程必须使用 `pthread_join()` 进行连接。如果未能连接，那么线程终止时将产生 **僵尸线程** 

> [!tip] 僵尸线程：与僵尸进程的概念类似
> 
> 释放的线程持有的资源，但是占据了管理线程的数据结构
> 

`pthread_join()` 用于等待指定线程结束，与针对进程的 `waitpid()` 类似，但是二者差异巨大

> [!tip] 任何线程都可以调用 `pthread_join()` 连接任意线程
> 
> **线程之间的关系是对等的**。进程中的任意线程均可以调用 `pthread_join()`与该进程的任何其他线程连接起来
> 

> [!tip] 无法连接任意线程，也不能以非阻塞方式连接
> 
> 后续使用 **互斥锁** 和 **条件变量** 实现类似的功能 
> 

## 线程的分离

**线程默认是可连接的**，也就是说，线程退出时，其他线程可以通过调用 `pthread_join()` 获取其返回状态

有时候，我们并 **不关心线程的返回状态**，只希望系统在线程终止时自动清理并移除。这种情况下，可以调用 `pthread_detach()` 将参数 `thread` 指定的线程标记为 **分离状态**

```c
#include <pthread.h>

int pthread_detach(pthread_t thread);
/* 成功返回 0；失败返回 errno */
```

> [!important] 分离状态的线程 **不可连接**，也 **不可恢复为可连接状态**
> 
> 一旦线程处于分离状态，就不能再使用 `pthread_join()` 来获取其状态，也无法使其重返 “可连接” 状态。
> 

其他线程调用 `exit()` 或者 主线程 `main()` 函数执行 `return` 语句，也会影响到分离的线程。此时，进程中的任何线程都会立即终止

> [!summary]
> 
> `pthread_detach()` 只是控制线程终止之后所发生的事情，而非何时或如何终止线程
> 

## 线程属性

`pthread_create()` 中类型为 `pthread_attr_t` 的 `attr` 参数，可利用其在创建线程时指定新线程的 **属性**。我们只考虑 $3$ 类属性： **线程栈的位置和大小** **线程调度策略和优先级**  和 **线程是否处于可连接或分离状态**

```c
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

void *thread_function(void *arg) {
    printf("Hello from the thread!\n");
    return NULL;
}

int main() {
    pthread_t thread;
    pthread_attr_t attr;
    size_t stacksize = 1024 * 1024; // 设置栈大小为1MB

    // 初始化属性对象
    pthread_attr_init(&attr);

    // 设置线程分离属性
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    // 设置栈大小
    pthread_attr_setstacksize(&attr, stacksize);

    // 创建线程并使用自定义属性
    pthread_create(&thread, &attr, thread_function, NULL);

    // 销毁属性对象
    pthread_attr_destroy(&attr);

    // 主线程退出，子线程继续执行
    pthread_exit(NULL);
}
```
