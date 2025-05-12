# 线程

在 [[操作系统：多线程概念]] 中我们介绍过，**线程是一个独立的执行流**。对于内核线程由操作系统调度；而用户线程由进程自己的运行时管理，操作系统并不能知晓用户线程的存在。

> [!tip] 
> 
> 换句话说，用户线程想要执行就必须绑定一个内核线程
> 

在大多数语言中，线程是可以并行执行的；然而，由于 CPython 实现是采用了**全局解释器锁**(**GIL**)：**只允许一个 Python 解释器进程中只有一个线程执行**

这意味着在 Python 中使用线程并不能提供所有任务的效率。就行在 [[Python：并发模型]] 中介绍的 CPU 密集型任务就不适合使用线程处理

>[!important] 
>
>在 Python 中，对于那些需要花费大量时间等待外部事件的任务，使用线程加速就非常合适
>

## 创建线程

在 Python 中创建线程需要使用标准库 `threading` 中的 `Thread` 类。使用方式与 `multiprocessing.Process` 类似

还是使用 [[Python：进程]] 中为计算第 $n$ 为斐波拉契数列添加动画的例子

```python hl:30
import sys
import threading
import time
import itertools

from threading import Thread


def fib(n):
    if n < 0:
        raise ValueError('n cannot be negative')

    if n < 2:
        return 1
    return fib(n - 1) + fib(n - 2)

def wrap(n):
    result = fib(n)
    print("\rresult = ", result)


def spinner(delay=0.05):
    for r in itertools.cycle(r"-\|/"):
        print(f"\rresult = {r}" , end="")
        time.sleep(delay)


def main():
    start = time.perf_counter()
    spinner_thread = Thread(target=spinner, args=(0.2, ), daemon=True)  # 创建一个线程对象
    spinner_thread.start()
    result = fib(42)
    end = time.perf_counter()
    print("\rresult = ", result)
    print("calculation time: ", end - start, "seconds")


if __name__ == '__main__':
    main()
```

与 `Process` 一样，我们 `Thread` 只是返回一个线程对象，想要启动线程必须调用线程对象的 `start()` 方法。

> [!important] 
> 
> 如果我们继承了 `Thread` 类，那么我们可能需要重写 `run()` 方法
> 

`Thread` 对象还有下表列出的几个 **方法** 或 **数据属性** 用于操作 `Thread` 对象

| 属性                | 描述                    |
| :---------------- | :-------------------- |
| `run()`           | 表示线程活动的方法，即线程执行的任务    |
| `start()`         | 创建线程，并在该线程中执行 `run()` |
| `join([timeout])` | 阻塞调用线程                |
| `is_alive()`      | 检查线程是否存活              |
| `daemon`          | 线程的守护标志               |
### 守护线程

设置了 `daemon` 标志的线程成为 **守护线程**

> [!attention] 
> 
> 请注意，在计算机科学中，**守护进程** 是指在后台运行的进程
> 

Python 线程对守护进程有更具体的含义。**守护线程将在程序退出时立即关闭**。如果一个程序正在运行非守护线程，那么程序将在这些线程完成之前终止。然而，守护线程在程序退出时，无论它们在何处，都会被终止。

> [!tip] 
> 
> 主线程退出后，所有的守护线程都会被终止
> 

如果一个进程将要退出时，还有非守护线程在执行，那么主线程将会等待所有非守护线程结束

### 等待线程结束

守护线程很有用，但当你想要等待一个线程停止时怎么办？当你想要这样做而不退出你的程序时怎么办？

只需要调用线程对象的 `join()` 方法即可，它会阻塞调用线程，直到线程对象结束

## 终止线程

在 Python 中，想要终止一个线程需要调用 `_thread.exit()` 函数，该函数会抛出 `SystemExit` 异常，该异常没有被不会就会导致线程退出

> [!tip] 
> 
> `sys.exit()` 也是抛出一个 `SystemExit`，因此调用 `sys.exit()` 与调用 `_thread.exit()` 时等效的。
> 

```python

import sys
import threading
import time
import itertools
import queue


from threading import Thread

q = queue.Queue()


def fib(n):
    if n < 0:
        raise ValueError('n cannot be negative')

    if n < 2:
        return 1
    return fib(n - 1) + fib(n - 2)

def wrap(n):
    result = fib(n)
    print("\rresult = ", result)


def spinner(delay=0.05):
    for r in itertools.cycle(r"-\|/"):
        print(f"\rresult = {r}" , end="")
        time.sleep(delay)
        try:
            q.get_nowait()
        except queue.Empty:
            continue
        else:
            sys.exit(0)  # 只会退出当前调用的线程


def main():
    start = time.perf_counter()
    spinner_thread = Thread(target=spinner, args=(0.2, ))  # 创建一个线程对象
    spinner_thread.start()
    result = fib(42)
    q.put(0)
    end = time.perf_counter()
    print("\rresult = ", result)
    print("calculation time: ", end - start, "seconds")


if __name__ == '__main__':
    main()
```

## 线程同步

线程的优势就是 **能够通过全局变量来共享信息**。不过，这种便捷的共享是有代价的：必须确保 **多个线程不会同时修改同一变量**，或者 **某一线程不会读取正由其他线程修改的变量**。程序代码中访问共享资源的代码片段称为 **临界区**。任何时刻只有一个线程执行临界区内的代码，当线程正在执行临界区代码时不能被其他线程中断。也就是说，**临界区的执行时必须是原子的**

### 竞争条件

**竞态条件可能在两个或多个线程访问共享数据或资源时发生**。在这个例子中，你将创建一个每次都会发生的巨大竞态条件，但请注意，大多数竞态条件并不这么明显。通常，它们只偶尔发生，并且可能会产生令人困惑的结果。正如你可以想象的那样，这使得它们相当难以调试

```python
import sys
from threading import Thread
import time

glob = [0]


def incr(loops, name):

    while loops > 0:
        loc = glob[0]
        print(f"Thread {name} get loc={loc}")
        loc += 1
        glob[0] = loc
        print(f"Thread {name} set glob[0]={loc}")
        loops -= 1
    
    print(f"Thread {name} finished")


def main():

    if len(sys.argv) != 3:
        print("Usage: python race.py <thread_number> <loops>", file=sys.stderr)
    
    threads = []
    for i in range(int(sys.argv[1])):
        t = Thread(target=incr, args=(int(sys.argv[2]), i + 1))
        threads.append(t)
        t.start()
    
    for t in threads:
        t.join()
    
    print("All thread end! glob is %s" % glob)

if __name__ == "__main__":
    main()
```

执行上述代码输出的结果如下

```
$ python race.py 2 10 
Thread 1 get loc=0
Thread 1 set glob[0]=1
Thread 2 get loc=1
Thread 1 get loc=1
Thread 1 set glob[0]=2
Thread 2 set glob[0]=2
Thread 1 get loc=2
Thread 2 get loc=2
Thread 1 set glob[0]=3
....
Thread 1 get loc=12
Thread 2 set glob[0]=12
Thread 1 set glob[0]=13
Thread 2 finished
Thread 1 get loc=13
Thread 1 set glob[0]=14
Thread 1 finished
All thread end! glob is [14]
```

为了解决上述问题，Python 提供了许多线程同步工具，我们主要使用两个：**互斥锁** 和 **条件变量**

### 互斥锁

为了解决上面的竞态条件，我们需要找到一种方法，使得 **一次只允许一个线程进入** 您的代码的 **读-修改-写** 部分(这个部分称为 **临界区域**)。在 Python 中，最常见的方法称为锁。在有些其他语言中，这个概念被称为 **互斥锁**(mutex)。互斥锁来源于MUTual EXclusion（互斥），这正是锁所做的事情

一个锁是一个像通行证一样的对象。同一时间只能有一个线程拥有锁。任何想要获取锁的其他线程都必须等待锁的所有者释放它。

在 Python 使用互斥锁非常简单，不用像 [[Linux 系统编程：互斥量]] 中那样进行复杂的初始化操作

```python hl:2,7,13,19
import sys
from threading import Thread, Lock
import time

glob = [0]

lock = Lock()

def incr(loops, name):

    while loops > 0:

        lock.acquire()  # 获取互斥锁
        loc = glob[0]
        print(f"Thread {name} get loc={loc}")
        loc += 1
        glob[0] = loc
        print(f"Thread {name} set glob[0]={loc}")
        lock.release()  # 释放互斥锁
        loops -= 1
    
    print(f"Thread {name} finished")


def main():

    if len(sys.argv) != 3:
        print("Usage: python race.py <thread_number> <loops>", file=sys.stderr)
    
    threads = []
    for i in range(int(sys.argv[1])):
        t = Thread(target=incr, args=(int(sys.argv[2]), i + 1))
        threads.append(t)
        t.start()
    
    for t in threads:
        t.join()
    
    print("All thread end! glob is %s" % glob)

if __name__ == "__main__":
    main()
```

> [!attention] 
> 
> 请注意：线程在进入临界区之前**获得互斥锁**，在离开临界区是必须**释放互斥锁**。简单来讲，获取和释放是一对操作，
> 

`Lock` 类是支持上下文管理的，因此，我们可以使用 `with` 语句，这样可以自动在进入临界区之前获取锁，在离开临界区时释放锁

```python
lock = Lock()

def incr(loops, name):

    while loops > 0:

        with lock:
            loc = glob[0]
            print(f"Thread {name} get loc={loc}")
            loc += 1
            glob[0] = loc
            print(f"Thread {name} set glob[0]={loc}")
        
        loops -= 1
    
    print(f"Thread {name} finished")
```

> [!tip] 
> 
> 死锁问题：有时，一个线程需要 **同时访问多个不同的共享资源**，并且 **每个资源由不同的互斥量管理**。当超过一个线程加锁同一组互斥量时，就有可能发生死锁
> 

> [!tip] 
> 
> 死锁的 $4$ 个必要条件：**只有当下面 $4$ 个条件同时成立时才会引起死锁**
> 
> + **互斥(mutual exclusion)**：共享资源一次只能被一个进程或线程使用
> + **占用并等待(hold and wait)**：进程或线程必须占用一个共享资源，并等待另一个共享资源
> + **非抢占(no preemption)**：进程或线程持有的资源不能被抢占
> + **循环等待(circular wait)**：有一组等待进程或线程$\{T_0, T_1, \cdots, T_{n}\}$，$T_0$ 等待的资源为 $T_1$ 占有，$T_1$ 等待的资源为 $T_2$ 占有，$\cdots$，$T_{n-1}$ 等待的资源为 $T_n$ 占有，$T_n$ 等待的资源为 $T_0$ 占有
> 

## 标准库：queue 

我们在 [[Python：进程#进程间通信]] 中介绍了用于进程间通信的队列，下面我们介绍标准库 `queue` 提供的队列

> [!tip] 
> 
> [`queue`](https://docs.python.org/zh-cn/3.13/library/queue.html#module-queue "queue: A synchronized queue class.") 模块实现了多生产者、多消费者队列。这特别适用于消息必须安全地在 **多线程** 间交换的线程编程。模块中的 [`Queue`](https://docs.python.org/zh-cn/3.13/library/queue.html#queue.Queue "queue.Queue") 类实现了所有所需的锁定语义。
> 

`queue.Queue()` 返回一个用于线程间交换数据的队列实例对象。`Queue()` 与 `mutilprocessing.JoinableQueue()` 类似，具有相同的方式。我们就不在复述
