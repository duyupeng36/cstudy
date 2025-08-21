# 线程取消

正常情况下，程序中的多个线程会并发执行，每个线程各司其职，直到其决意退出

> [!tip] 
> 
> 线程主动调用 `pthread_exit()` 或者从线程启动函数中返回
> 

有时候，需要将一个 **线程取消**(cancel)。亦即，向线程发送一个 **取消请求**，要求其立即退出。

> [!example] 
> 
> 比如，一组线程正在执行一个运算，一旦某个线程检测到错误发生，需要其他线程退出，取消线程的功能这时就派上用场
> 

> [!example] 
> 
> 还有一种情况，一个由图形用户界面（GUI）驱动的应用程序可能会提供一个“取消”按钮，以便用户可以终止后台某一线程正在执行的任务。这种情况下，主线程（控制图形用户界面）需要请求后台线程退出。
> 

## 取消线程

函数 `pthread_cancel()` 向由 `thread` 指定的线程发送一个取消请求

```c
#include <pthread.h>

int pthread_cancel(pthread_t thread);
/* 成功返回 0；错误返回 errno */
```

> [!tip] 参数 `thread`：指定需要取消的线程
> 

发出取消请求后，函数 `pthread_cancel()` 当即返回，不会等待目标线程的退出

准确地说，目标线程会发生什么？何时发生？这些都取决于 **线程取消状态**（state）和 **类型**（type）

## 取消状态和类型

函数 `pthread_setcancelstate()` 控制线程是的 **取消状态**。

```c
#include <pthread.h>

int pthread_setcancelstate(int state, int *oldstate);
/* 成功返回 0；错误返回 errno */
```

> [!tip] 参数 `state`：指定调用线程是否可以取消。取值只能两个
> 
> + `PTHREAD_CANCEL_ENABLE`：线程 **可以被取消**。这些新建线程的可取消状态的默认值
> + `PTHREAD_CANCEL_DISABLE`：线程 **不可以被取消**。如果处于该状态的线程收到取消请求，则 **取消请求被挂起**，直到线程将取消状态设置为可以取消
> 

|         `state`          | 描述               |
| :----------------------: | :--------------- |
| `PTHREAD_CANCEL_ENABLE`  | 线程可以被取消，新建线程的默认值 |
| `PTHREAD_CANCEL_DISABLE` | 线程不可以被取消         |

> [!tip] 参数 `oldstate`：指向 `int` 类型的缓冲区指针，用于存储线程的上一个取消状态
> 
> 在 Linux 上，允许将 `oldstate` 置为 `NULL`，表示我们不期望获取上一个取消类型。然而，SUSv3 标准没有规定 `oldstate` 是否可以置为 `NULL`。为了保存程序的可移植性，最好将该参数设置为非 `NULL`
> 


函数 `pthread_setcanceltype()` 控制线程 **取消类型**

```c
#include <pthread.h>

int pthread_setcanceltype(int type, int *oldtype);
/* 成功返回 0；错误返回 errno */
```

> [!tip] 参数 `type`：指定线程的取消类型。取值如下
> 
> + `PTHREAD_CANCEL_ASYNCHRONOUS`：异步取消。可能在任何时间点取消线程
> + `PTHREAD_CANCEL_DEFERED`：延迟取消。取消请求被挂起，直到线程代码执行到 **取消点**
>  

> [!tip] 参数 `oldtype`：指向 `int` 类型的缓冲区，用于存储线程的上一个取消状态
> 
> 该参数与 `pthread_setcanclestate()` 中的 `oldstate` 参数类似
> 

> [!important] 
> 
> 当某线程调用 `fork()` 时，子进程会继承调用线程的取消状态以类型
> 
> 而当某线程调用 `exec()` 时，会将新程序主线程的取消性类型及状态分别重置为 `PTHREAD_CANCEL_ENABLE` 和 `PTHREAD_CANCEL_DEFERRED`

## 取消点

若将线程的取消状态和取消类型分别置为 `PTHREAD_CANCLE_ENABLE`(可取消) 和 `PTHREAD_CANCLE_DEFFERED`(延迟取消)，那么仅当线程执行某个 **取消点** 时，取消请求才会起作用

> [!tip] 取消点：由实现定义的一组函数的调用

下表列出的这些函数是 SUSv3 规定取消点

![Pasted image 20241113140021|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755784187350-53fc8cd63b804e54954edc4369137d93.png)

> [!attention] 
> 
> 任何 **可能造成阻塞的函数** 都是取消点的候选对象
> 

线程一旦收到取消请求，且启用了取消性状态并将类型置为延迟，则其会在下次抵达取消点时终止。如果该线程 **尚未分离**，那么为防止其变为僵尸线程，必须由其他线程 **对其进行连接**。连接之后，返回至函数 `pthread_join()` 中第二个参数的将是一个特殊值：`PTHREAD_CANCELED`

> [!tip] 
> 
> 对一个已经取消的线程进行连接(`pthread_join()`)，线程函数的返回值为 `PTHREAD_CANCLED`
>  
>  注意 `PTHREAD_CACNLED` 这个值是一个整数。如果线程正常结束也返回一个整数，可能会导致冲突
> 
 
## 线程可取消性的检测

假设线程执行的是一个 **不含取消点** 的循环（计算密集型循环），这时，线程永远也不会响应取消请求

 Pthreads API 中的函数 `pthread_testcancle()` 的目的很简单，就是产生一个取消点。线程如果已有处于挂起状态的取消请求，那么只要调用该函数，线程就会随之终止。

```c
#include <pthread.h>

void pthread_testcacnle(void);
```

当线程执行的代码未包含取消点时，可以周期性地调用 `pthread_testcancel()`，以确保对其他线程向其发送的取消请求做出及时响应。

> [!summary] 
> 
> 函数 `pthread_testcacnle()` 本质就是一个取消点，检查到线程接收到取消请求，就会结束线程
> 

## 线程清理函数

一旦有处于挂起状态的取消请求，线程在执行到取消点时如果只是草草收场，这会将共享变量以及 Pthreads 对象置于一种不一致状态，可能导致进程中其他线程产生错误结果、死锁，甚至造成程序崩溃

> [!important] 
> 
> 线程会持有资源，**如果线程被取消时只是简单的终止，那么会造成资源泄露**。
> 
> 假设线程持有互斥锁，并且线程在 **临界区内接收到取消请求**，同时也 **执行到取消点**。如果线程只是简单终止，那么互斥锁就得不到释放，从而造成死锁现象
> 

为规避这一问题，线程可以设置一个或多个 **清理函数**，当线 **程被取消时会自动运行这些函数**，在线程终止之前可执行诸如修改全局变量，解锁互斥量等动作。

**每个线程都可以拥有一个 _清理函数栈_**。当线程遭取消时，会沿该栈 **自顶向下依次执行清理函数**，首先会执行最近设置的函数，接着是次新的函数，以此类推。当执行完所有清理函数后，线程终止

函数 `pthread_cleanup_push()` 负责向调用线程的清理函数栈顶 **添加清理函数**

```c
#include <pthread.h>

void pthread_cleanup_push(void(*cleanup)(void *), void *arg);
```

> [!tip] 参数 `cleanup`：执行清理函数的指针
> 
> 清理函数的格式如下
> 
> ```c
> void cleanup(void * arg)
> {
> 	// 执行清理操作
> }
> ```

> [!tip] 参数 `arg`：传递给清理函数的参数
> 

>[!attention] 
> 
> 线程资源申请资源成功之后立即调用 `pthread_cleanup_push()` 添加清理函数。期间不要做任何操作
>

线程如果需要主动释放资源必须调用函数 `pthread_cleanup_pop()`，该函负责从清理函数栈中 **移除** 并 **执行** 最后添加的清理函数

```c
#include <pthread.h>

void pthread_cleanup_pop(int execute);
```

> [!tip] 参数 `execute`：指定是否执行清理函数
> 
> + `execute` 如果非 $0$，那么无论如何都会执行清理函数
> 
> 该特性在函数未遭取消而又希望执行清理动作的情况下，这会非常方便
> 

通常，**线程如在执行一段特殊代码时 _遭到取消_，才需要执行清理动作**。如果线程顺利执行完这段代码而未遭取消，那么就不再需要清理。所以，每个对 `pthread_cleanup_push()` 的调用都会伴随着对 `pthread_cleanup_pop()` 的调用。

> [!warning] 
> 
> SUSv3 允许将 `pthread_cleanup_push()` 和 `pthread_cleanup_pop()` 实现为宏，展开为 `{}` 包围的语句序列。在 Linux 中这两个函数的实现如下
> 
>```c
> #  define pthread_cleanup_push(routine, arg) \
>   do {									      \
>     __pthread_cleanup_class __clframe (routine, arg)
>
> #  define pthread_cleanup_pop(execute) \
>     __clframe.__setdoit (execute);					      \
>   } while (0)
> ```
> 
> 因此，**必须在同一个作用域中成对出现**
> 

若线程因调用 `pthread_exit()` 而终止，则也会 **自动** 执行尚未从清理函数栈中弹出(`pop`)的清理函数。线程正常返回(`return`)时不会执行清理函数

> [!summary] 
> 
> + 线程取消往往伴随其持有资源的泄露
> + `pthread_cleanup_push()` 和 `pthread_cleanup_pop()` 用于在线程清理函数栈中添加或移除清理函数
> +  `pthread_cleanup_push()` 和 `pthread_cleanup_pop()`  可能被实现为宏，展开为 `{}` 包围的语句序列
> + 线程执行 `pthread_exit()` 或者被收到取消请求并且执行到取消点，则会自动执行清理函数栈中的所有函数
> + 线程入口函数执行 `return` 语句则不会触发线程清理函数中的函数执行
> 
