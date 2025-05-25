# 通知状态改变：条件变量

[[Linux 系统编程：互斥量]] 防止多个线程同时访问同一共享变量。**条件变量** 允许一个线程就某个共享变量或其他共享资源的状态变化 **通知** 其他线程，并让其他线程等待 **阻塞** 于这一通知

一个未使用条件变量的简单例子有助于展示条件变量的重要性。假设由若干线程生成一些 “产品单元” 供主线程消费。还使用了一个由互斥量保护的变量 `avail` 来代表待消费产品的数量

```c title:threads/producer_consumer.c
static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
static int avail = 0;
```

生产者线程的源码如下

```c title:threads/producer_consumer.c
// 生产者代码片段
if(pthread_mutex_lock(&mtx) != 0)
{
	// 错误处理
}

avail++; 

if(pthread_mutex_unlock(&mtx) != 0)
{
	// 错误处理
}
```

消费者的代码如下

```c title:threads/producer_consumer.c
// 消费者代码片段
for(;;)
{
	if(pthread_mutex_lock(&mtx) != 0)
	{
		// 错误处理
	}
	while(avail > 0)
	{
		// 做一些事情
		avail--;
	}
	if(pthread_mutex_unlock(&mtx) != 0)
	{
		// 错误处理
	}
}
```

> [!warning] 
> 
> 上述方案由于 **主线程不停地循环检查变量 `avail` 的状态**，故而造成 `CPU` 资源的浪费
> 

为了解决上述问题，采用 **条件变量**：**允许一个线程休眠（等待）直至接获另一线程的通知去执行某些操作**。出现一些“情况”后，等待者必须立即做出响应

> [!tip] 条件变量总是结合互斥量使用
> 
> + 条件变量：就共享变量的状态改变发出通知
> 
> + 互斥量：提供对该共享变量访问的互斥
> 

## 初始化和销毁条件变量

### 静态初始化

条件变量的数据类型是 `pthread_cound_t`。类似于互斥量，**使用条件变量前必须对其初始化**

对于经由 **静态分配** 的条件变量，将其赋值为 `PTHREAD_COND_INITALIZER` 即完成初始化操作。可参考下面的例子

```c
ptread_cound_t cond = PTHREAD_COND_INITALIZER;
```

> [!attention] 
> 
> 静态分配的条件变量无需显示销毁
> 

### 动态初始化

使用函数 `pthread_cond_init()` 可以对条件变量进行动态初始化

```c
#include <pthread.h>

int pthread_cond_init(pthread_cond_t *cond, const pthead_condattr_t *attr);
/* 成功返回 0; 失败返回 errno */
```

需要使用 `pthread_cond_init()` 的情形类似于 [[Linux 系统编程：互斥量#初始化和销毁互斥量#动态初始化]] 中的情况

- 动态分配于堆中
- 在栈中分配的自动变量
- 初始化经由静态分配，且不使用默认属性


> [!warning] 
> 
> **对已初始化的条件变量进行再次初始化，将导致未定义的行为**。应当避免这一做法
> 

当不再需要一个经由自动或动态分配的条件变量时，应调用 `pthread_cond_destroy()` 函数予以销毁

```c
#include <pthread.h>

int pthread_cond_destroy(pthread_cond_t *cond);
/* 成功返回 0；错误返回正数*/
```

## 通知和等待条件变量

条件变量的主要操作是 **通知(notify)** 和 **等待(wait)通知**

> [!tip] 通知(notify)：向处于等待状态的线程发送信号，告知某个共享变量的状态发生改变
> 
> 注意，此处的信号并非 [[Linux 系统编程：信号基本概念]] 中介绍的信号
> 

> [!tip] 等待(wait)：在收到通知之前一直阻塞等待

函数 `pthread_cond_signal()` 和 `pthread_cond_broadcast()` 均可针对由参数 `cond` 所指定的条件变量而发送信号。它们之间的差别在于 `pthread_cond_signal()` 能保证**至少唤醒一条线程**；而 `pthread_cond_broadcast()` 会 **唤醒所有阻塞线程**

```c
#include <pthread.h>

int pthread_cond_signal(pthread_cond_t *cond);
int pthread_cond_broadcast(pthread_cond_t *cond);
/* 成功返回 0；错误返回 errno */
```

使用函数 `pthread_cond_broadcast()` 总能产生正确结果，但函数 `pthread_cond_signal()` 会更为高效。不过，只有当仅需 **唤醒 _任意_ 一条等待线程** 来处理共享变量的状态变化时，才应使用 `pthread_cond_signal()`

> [!tip] 应用 `pthread_cond_signal()` 的典型情况是：**所有等待线程都在执行完全相同的任务**
> 
> 函数 `pthread_cond_signal()` 会比 `pthread_cond_broadcast()` 更具效率，因为这可以 **避免** 发生如下情况
> 
> + 同时唤醒所有等待线程
> + 某一线程首先获得调度。此线程完成了后续所有工作后释放互斥量
> + 剩余线程轮流锁定互斥量并检测共享变量状态。由于第一个线程完成了所有工作，剩余线程无事可做，随即解锁互斥量转而进行等待
> 

> [!tip] 应用 `pthread_cond_broadcast()` 的典型情况是：**处于等待状态的所有线程执行的任务不同**

函数 `pthread_cond_wait()` 将阻塞一线程，直至收到条件变量 `cond` 的通知

```c
#include <pthread.h>

int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex);
/* 成功返回 0；错误返回 errno */
```

函数 `pthread_cond_timedwait()` 与函数 `pthread_cond_wait()` 几近相同，唯一的区别在于，由参数 `abstime` 来指定一个线程等待条件变量通知时休眠时间的上限

```c
#include <pthread.h>

int pthread_cond_timedwait(pthread_cond_t * cond, pthread_mutex_t * mutex, const struct timespec * abstime);
/* 成功返回 0；错误返回 errno */
```

> [!attention] 
> 
> 条件变量并不保存状态信息，只是传递应用程序状态信息的一种通讯机制。**发送信号时若无任何线程在等待该条件变量，这个信号也就会不了了之**。线程如在此后等待该条件变量，只有当再次收到此变量的下一信号时，方可解除阻塞状态
> 

### 示例程序：生产者-消费者模型

最开始，我们介绍了使用互斥量同步生产者和消费。然而，这会导致消费者不停地循环检查变量 `avail` 的状态，造成 CPU 资源的浪费

我们使用条件变量解决上述问题。对全局变量、相关互斥量以及条件变量的声明代码如下：

```c title:thread/producer_consumer_cond.c
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;  
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;  // 初始化条件变量  
static int avail = 0;
```

除了增加对函数 `pthread_cond_signal()` 的调用外，生产者线程的代码与之前并无变化:

```c title:thread/producer_consumer_cond.c
void * producer(void * arg) {

    while (true) {
	    // 加锁
        int err = pthread_mutex_lock(&mutex);
        if(err != 0) {
            errExit("pthread_mutex_lock: ");
        }
        avail++;
        printf("producer[TID %ld]: avail = %d\n", pthread_self(), avail);
        // 解锁
        err = pthread_mutex_unlock(&mutex);
        if(err != 0) {
            errExit("pthread_mutex_unlock: ");
        }
        // 通知消费者数据生产完毕
        err = pthread_cond_signal(&cond);
        if(err != 0) {
            errExit("pthread_cond_signal: ");
        }
        unsigned int seed = (unsigned int)time(nullptr);
        usleep(rand_r(&seed) % (1000 * 500 * 2));
    }
    return NULL;
}
```

在分析消费者线程的代码之前，我们首先对 `pthread_cond_wait()` 函数做详细解释

先前已经指出，**条件变量总是要与一个互斥量相关**，通过函数参数传递给函数 `pthread_cond_wait()`。然后，`pthread_cond_wait()` 函数执行如下步骤

> [!tip] 首先，解锁互斥量
> + 保证其他线程可以对互斥量进行加锁
> 

> [!tip] 然后，阻塞调用线程，直到另一个线程就条件变量 `cond` 发出信号
> 
> + 等待其他线程的通知
> 

> [!tip] 最后，重新锁定互斥量
> 
> + 保证后续对共享资源访问是原子性的
> 

设计 `pthread_cond_wait()` 执行上述步骤，是因为通常情况下代码会以如下方式访问共享变量

```c
err = pthread_mutex_lock(&mutex);

if(err != 0) {
	// 错误处理
}

while(/* 检查共享变量是否处于我们想要的状态 */) {
	pthread_cond_wait(&cond, &mutex);
}

/* 共享变量处于所需要的状态，执行一些工作 */

err = pthread_mutex_unlock(&mutex);
if (err != 0){
	// 错误处理
}
```

> [!important]  将 `pthread_cond_wait()` 放在 `while` 循环中时必要的。其原因如下
> 
> 每个条件变量都有与之相关的判断条件，涉及一个或多个共享变量。在上述消费者线程中，共享变量 `avail == 0` 时，消费者线程就应该等待。与条件变量相关的判断就是 `avali == 0`
> 
> 必须使用 `while()` 而非 `if()` 来控制 `pthread_cond_wait()` 的原因是：**当线程被唤醒时，并不能确定判断条件的状**，应该立即重新检查判断条件，条件不满足的情况下继续休眠等待
> 
> 下面 $3$ 种原因造成了 `pthread_cond_wait()` 返回时判断条件状态不能确定
> + **其他线程率先醒来**，并获取互斥量，从而改变了判断条件的状态
> + 就条件变量发送信号意味着 **“可能有些事情”** 需要接收信号的线程去响应，**而不是 “一定有一些事情” 要做**
> + **可能会发生虚假唤醒的情况**。某些实现中，没有任何线程就条件变量发出信号，等待此条件变量的线程仍然有可能醒来。这是一些多处理器系统上，为了确保高效实现而采用技术
> 

在以上代码中，两处对共享变量的访问都必须置于互斥量的保护之下，其原因之前已做了解释。换言之，**条件变量与互斥量之间存在着天然的关联关系**

使用 `pthread_cond_wait()` 等待其他线程就条件变量发送通知时应该采用下面步骤

> 第一步：线程在准备 **检查共享变量状态时锁定互斥量**
> 
> 第二步：检查共享变量的状态
> 
> 第三步：如果 **共享变量未处于预期状态**，线程应在 **等待条件变量并进入休眠前解锁互斥量**
> 
> 第四步：当线程因为条件变量的通知而被**再度唤醒时**，**必须对互斥量再次加锁**，因为在典型情况下，线程会立即访问共享变量。
> 

> [!important] 函数 `pthread_cond_wait()` 会自动执行最后两步中对互斥量的解锁和加锁动作
> 
> 第三步中互斥量的释放与陷入条件变量的等待属于同一个原子操作
> 
> 换句话说 ， 在函数 `pthread_cond_wait()` 的调用线程陷入对条件变量的等待之前，其他线程不可能获取到该互斥量，也不可能就该条件变量发出信号
> 

通过上述细节描述，主线程使用 `pthread_cond_wait()` 等待生产者线程通知的代码如下

```c title:threads/producer_consumer_cond.c
  
while (true) {
	// 锁定互斥量
	err = pthread_mutex_lock(&mutex);
	if(err != 0) {
		errExit("pthread_mutex_lock: ");
	}

	// 检查共享变量，等待生产者通知
	while (avail == 0) {
		err = pthread_cond_wait(&cond, &mutex);
		if(err != 0) {
			errExit("pthread_cond_wait: ");
		}
	}
	// 消费数据
	while (avail > 0) {
		printf("consumer[TID %ld]: avail = %d\n", pthread_self(), avail);
		avail--;
	}
	
	// 释放互斥量
	err = pthread_mutex_unlock(&mutex);
	if(err != 0) {
		errExit("pthread_mutex_unlock: ");
	}
}
```
