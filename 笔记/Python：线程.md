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

> [!attention] 
> 
> 请注意：下面的例子，我们必须在 Python3.13 的实验版本，这个版本移除了 Python 的全局解释器锁，才能演示我们想要出现的效果
> 

```python hl:11
import sys
from threading import Thread


glob = 0


def incr(loops, name):
    global glob
    while loops > 0:
        glob += 1   # 线程修改全局变量，竞争条件
        loops -= 1
    
    print(f"Thread {name} finished")


def main():
    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument("-l", '--loops', type=int, default=100, help="循环次数")
    parser.add_argument("-t", "--threads", type=int, default=10, help="线程数")
    args = parser.parse_args()
    
    threads = []
    for i in range(args.threads):
        t = Thread(target=incr, args=(args.loops, i + 1))
        threads.append(t)
        t.start()
    
    for t in threads:
        t.join()
    
    print("All thread end! glob is %s" % glob)

if __name__ == "__main__":
    main()
```

使用 Python3.13 实验版本执行上述代码输出的结果如下

```shell
# 两个线程，每个线程循环10次
$ python3.13t race.py -l 10 -t 2
Thread 1 finished
Thread 2 finished
All thread end! glob is 20

# 两个线程，每个线程循环100次
$ python3.13t race.py -l 100 -t 2
Thread 1 finished
Thread 2 finished
All thread end! glob is 200

# 两个线程，每个线程循环1000次
$ python3.13t  race.py -l 1000 -t 2
Thread 1 finished
Thread 2 finished
All thread end! glob is 2000

# 两个线程，每个线程循环10000次
$ python3.13t race.py -l 10000 -t 2
Thread 1 finished
Thread 2 finished
All thread end! glob is 14561

$ python3.13t race.py -l 10000 -t 2
Thread 1 finished
Thread 2 finished
All thread end! glob is 17177
```

当循环次数增加到 $10000$ 时，每次执行的得到的结果均不相同，并且结果还是不正确的。执行到最后，`glob` 的值本应为 $2$ 万。问题的原因是由于线程的执行序列如下图导致的

![[Pasted image 20241111203752.png]]

为了解决上述问题，Python 提供了许多线程同步工具，下面我们将开始介绍 **线程同步工具**

### 互斥锁：Lock

为了解决上面的竞态条件，我们需要找到一种方法，使得 **一次只允许一个线程进入** 您的代码的 **读-修改-写** 部分(这个部分称为 **临界区域**)。在 Python 中，最常见的方法称为锁。在有些其他语言中，这个概念被称为 **互斥锁**(`mutex`)。互斥锁来源于 `MUTual EXclusion`（互斥），这正是锁所做的事情

一个锁是一个像通行证一样的对象。同一时间只能有一个线程拥有锁。任何想要获取锁的其他线程都必须等待锁的所有者释放它。

在 Python 使用互斥锁非常简单，不用像 [[Linux 系统编程：互斥量]] 中那样进行复杂的初始化操作

```python hl:2,11,13
import sys
from threading import Thread, Lock


glob = 0


def incr(loops, name, lock:Lock):
    global glob
    while loops > 0:
        lock.acquire()  # 加锁
        glob += 1
        lock.release()  # 释放锁
        loops -= 1
    
    print(f"Thread {name} finished")


def main():
    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument("-l", '--loops', type=int, default=100, help="循环次数")
    parser.add_argument("-t", "--threads", type=int, default=10, help="线程数")
    args = parser.parse_args()
    lock = Lock()


    threads = []
    for i in range(args.threads):
        t = Thread(target=incr, args=(args.loops, i + 1, lock))
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
def incr(loops, name, lock:Lock):
    global glob
    while loops > 0:
        with lock:
            glob += 1
        loops -= 1
    
    print(f"Thread {name} finished")
```

在使用互斥锁时一定要非常小心，并且仔细判断那些是临界区。**线程在执行临界区代码之前获取互斥锁，离开临界区代码时是否互斥锁**。必须按照这种要求使用互斥锁，否则可能造成 **死锁问题**！

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
> 只要破坏其中一个死锁的必要条件，那么死锁就会被解除
>

### 条件变量：Condition

互斥量防止多个线程同时访问同一共享变量。**条件变量** 允许一个线程就某个共享变量或其他共享资源的状态变化 **通知** 其他线程，并让其他线程等待 **阻塞** 于这一通知

一个未使用条件变量的简单例子有助于展示条件变量的重要性。假设由若干线程生成一些 “产品单元” 供主线程消费。还使用了一个由互斥量保护的变量 `avail` 来代表待消费产品的数量

```python hl:14,16
import logging
import threading
import time


logging.basicConfig(
    format="%(asctime)s %(threadName)s %(filename)s:%(lineno)s [%(message)s]", 
    datefmt="%Y-%m-%d %H:%M:%S",
    level=logging.INFO
)


# 互斥锁
mtx = threading.Lock()
# 共享变量
avail = 0

def producer():
    global avail
    while True:
	    # 生产者没 0.5 秒生产一个产品，然后释放锁
        with mtx:
            avail += 1
            logging.info("生产一个产品")
        time.sleep(0.5)


def main():
    threading.Thread(target=producer).start()
    time.sleep(1)
    global avail
    while True:
	    # 主线程获得锁
        with mtx:
	        # 循环检查 avail 是否大于 0
            while avail > 0:
                avail -= 1
                logging.info("消费一个产品")
                time.sleep(1)
        

if __name__ == "__main__":
    main() 
```

> [!warning] 
> 
> 上述方案由于 **主线程不停地循环检查变量 `avail` 的状态**，故而造成 `CPU` 资源的浪费
> 

为了解决上述问题，采用 **条件变量**：**允许一个线程休眠（等待）直至接获另一线程的通知去执行某些操作**。出现一些“情况”后，等待者必须立即做出响应。

也就是说，**条件变量总是与某种类型的锁对象相关联**，锁对象可以通过传入获得，或者在缺省的情况下自动创建。当多个条件变量需要共享同一个锁时，传入一个锁很有用。在 Python 中，**锁是条件对象的一部分，你不必单独地跟踪它**。

> [!important] 
> 
> 条件变量遵循 上下文管理协议 ：使用 `with` 语句会在它包围的代码块内获取关联的锁。
> 
> + 此外，条件变量的 `acquire()` 和 `release()` 方法也能调用关联锁的相关方法
> 

其它方法必须在持有关联的锁的情况下调用。 `wait()` 方法释放锁，然后阻塞直到其它线程调用 `notify()` 方法或 `notify_all()` 方法唤醒它。一旦被唤醒， `wait()` 方法重新获取锁并返回。它也可以指定超时时间。
+ `notify()` 只唤醒一个线程，置于唤醒哪一个线程是不清楚的
+ `notify_all()` 唤醒所有线程 

> [!warning] 
> 
> 注意： `notify()` 方法和 `notify_all()` 方法并不会释放锁，这意味着被唤醒的线程不会立即从它们的 `wait()` 方法调用中返回，而是会在调用了 `notify()` 方法或 `notify_all()` 方法的线程最终放弃了锁的所有权后返回。
> 
> 这意味着，我们必须先调用 `notify()` 或者 `notify_all()` 方法后才能释放锁。先释放锁，后调用 `notify()` 或者 `notify_all()` 方法将会引发异常
> 

使用条件变量的典型编程风格是将锁用于同步某些共享状态的权限，那些对状态的某些特定改变感兴趣的线程，它们重复调用 `wait()` 方法，直到看到所期望的改变发生；而对于修改状态的线程，它们将当前状态改变为可能是等待者所期待的新状态后，调用 `notify()` 方法或者 `notify_all()` 方法

如下例程，展示条件变量的应用：生产者线程随机产生一些数据，然后通知消费者线程将这些数据消耗掉

```python
import logging
import random
import threading
import time

logging.basicConfig(
    format="%(asctime)s %(threadName)s %(filename)s:%(lineno)s [%(message)s]", 
    datefmt="%Y-%m-%d %H:%M:%S",
    level=logging.INFO
)

lock = threading.Lock()
condition = threading.Condition(lock=lock)
avail = 0


def producer(condition: threading.Condition):
    global avail
    while True:
        time.sleep(random.random())
        # 自动获取锁
        with condition:
            rand = random.randint(1, 10)
            avail += rand
            logging.info(f"生产 {rand} 个产品，目前产品数量为 {avail}")  
            # 通知所有消费者线程，数据生产完毕
            condition.notify_all()
	    # 自动释放锁


def consumer(condition: threading.Condition):
    global avail

    while True:
        # 自动获取锁
        with condition:
            # 检查共享变量，等待生产者通知
            while avail == 0:
                # 释放锁，并让线程阻塞等待其他线程通知
                condition.wait()
                # 其他线程通知到来时，会重新加锁，然后从 wait() 返回
        
            # 消费数据
            rand = random.randint(1, 10)
            while avail - rand < 0:
                rand = random.randint(0, 10)
            avail -= rand
            logging.info(f"消费者 {rand} 个产品，剩余 {avail} 个产品")
            time.sleep(random.random())
        # 自动释放锁

if __name__ == "__main__":
    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument("-p", "--producer", type=int, default=10, help="生产者线程数")
    parser.add_argument("-c", "--consumer", type=int, default=2, help="消费者线程数")
    args = parser.parse_args()

    for i in range(args.producer):
        threading.Thread(target=producer, args=(condition, ), name=f"producer-{i}").start()
    
    for i in range(args.consumer):
        threading.Thread(target=consumer, args=(condition, ), name=f"consumer-{i}").start()
```

### 事件：Event

Event 是 Python 中线程同步工具中最简单工具之一：一个线程发出事件信号，而其他线程等待该信号

一个 Event 对象管理一个 **内部标识**，调用 `Event.set()` 方法可将其设置为 `True` ，调用 `Event.clear()` 方法可将其设置为 `False` ，调用 `Event.wait()` 方法将进入阻塞直到标识为 `True` 。

> [!tip] 
> 
> 此外，还有一个 `Event.is_set()` 方法用于检查内部标识是否为 `True`
> 

这个过程非常像我们在路上 **等待红绿灯**。当我们要通过路口时，如果红灯亮起，那么我们就必须等待。只有当绿灯亮起的时候，才能通过。

> [!tip] 
> 
> 某些线程必须等待另一个线程完成工作后，它们才能继续。此时，使用 Event 就非常合适
> 

```python
import time
import logging
import sys
from threading import Thread, Event, current_thread

logging.basicConfig(
    format="%(asctime)s %(threadName)s %(filename)s:%(lineno)s [%(message)s]", 
    datefmt="%Y-%m-%d %H:%M:%S",
    level=logging.INFO
)

def light(e: Event):
    while True:
        sys.stdout.flush()
        e.clear()
        logging.info("红灯")
        time.sleep(10)
        e.set()  # 发送信号
        logging.info("绿灯")
        time.sleep(5)


def car(e: Event):
    if e.is_set():
        logging.info("通过路口")
    else:
        logging.info("在路口等待")
        e.wait() 
    


if __name__ == '__main__':

    event = Event()  # 创建等待事件
    # 首先，创建红绿灯线程
    t = Thread(target=light, args=(event, ), name="红绿灯")
    t.start()

    while True:
        # 没 0.5 s 来一辆车
        time.sleep(1)
        Thread(target=car, args=(event, )).start()
```

### 信号量：Semaphore

信号量是一个非常古老的同步源语。参加 [[Linux 系统编程：System V 信号量]]。在 Python 中，一个信号量管理一个内部计数器，该计数器因 `acquire()` 方法的调用而递减，因 `release()` 方法的调用而递增。 **计数器的值永远不会小于零**；当 `acquire()` 方法发现计数器为零时，将会阻塞，直到其它线程调用 `release()` 方法。

> [!tip] 
> 
> 信号量通常用于保护有限资源。例如，打印机。当有多个线程要调用打印机打印数据时，首先应该检查打印机数量是否大于 $0$；只有在数量大于 $0$ 时才能打印数据。否则，线程就应该等待其他线程释放打印机
> 

```python
import logging
import random
import threading
import time

logging.basicConfig(
    format="%(asctime)s %(threadName)s %(filename)s:%(lineno)s [%(message)s]", 
    datefmt="%Y-%m-%d %H:%M:%S",
    level=logging.INFO
)


def routine(semaphore: threading.Semaphore):
    # 自动调用 semaphore.acquire() 让信号量减少 1
    with semaphore:
        # 模拟打印耗时
        time.sleep(1)
        logging.info("打印完成")
    # 自动调用 semaphore.release() 让信号量增加 1

if __name__ == "__main__":
    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument("-p", "--printer", type=int, default=5, help="打印机数量")
    parser.add_argument("-t", "--threads", type=int, default=20, help="线程数")
    args = parser.parse_args()

    # 信号量，用于保护打印机这种有限资源
    sem = threading.Semaphore(value=args.printer)
    
    for i in range(args.threads):
        threading.Thread(target=routine, args=(sem, ), name=f"routine-{i}").start()
```

## 线程安全

若函数 **可同时供多个线程安全调用**，则称之为 **线程安全函数**；反之，如果函数不是线程安全的，则不能并发调用

如下示例，演示了一个线程安全的 `incr()` 函数

```python

import argparse
import logging
import threading
import time


logging.basicConfig(
    format="%(asctime)s %(threadName)s %(filename)s:%(lineno)s [%(message)s]", 
    datefmt="%Y-%m-%d %H:%M:%S",
    level=logging.INFO
)


def incr(loops):
    x = 0
    for _ in range(loops):
        time.sleep(0.001)
        x += 1
    logging.info("x = %d", x)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="test thread local"
    )
    parser.add_argument("-l", "--loops", dest='loops', type=int, default=100, help="loop number for pear thread")
    parser.add_argument("-t", "--threads", dest='threads',type=int, default=10, help="thread number")
    
    args = parser.parse_args()

    for i in range(args.threads):
        threading.Thread(target=incr, args=(args.loops, )).start()
```

当我们执行上述代码时，每个线程最后输出的结果都是一样的

```shell
➜ python test.py -l 1000 -t 10                               
2025-05-14 22:34:52 Thread-10 (incr) test.py:19 [x = 1000]
2025-05-14 22:34:52 Thread-4 (incr) test.py:19 [x = 1000]
2025-05-14 22:34:52 Thread-9 (incr) test.py:19 [x = 1000]
2025-05-14 22:34:52 Thread-7 (incr) test.py:19 [x = 1000]
2025-05-14 22:34:52 Thread-8 (incr) test.py:19 [x = 1000]
2025-05-14 22:34:52 Thread-3 (incr) test.py:19 [x = 1000]
2025-05-14 22:34:52 Thread-1 (incr) test.py:19 [x = 1000]
2025-05-14 22:34:52 Thread-6 (incr) test.py:19 [x = 1000]
2025-05-14 22:34:52 Thread-5 (incr) test.py:19 [x = 1000]
2025-05-14 22:34:52 Thread-2 (incr) test.py:19 [x = 1000]
```

> [!tip] 
> 
> 无论多少个线程调用一个函数，该函数始终都能获得正确的结果。这样的函数就是线程函数的
> 

上述 `incr()` 函数之所以安全时因为它只是使用了局部变量(变量 `x` 是函数 `incr()` 的局部变量)，对于局部变量，函数每次调用都是不同的

**如果我们将局部变量 `x` 改为全局变量，则函数将变得不在安全**。例如

```python
import argparse
import logging
import threading
import time


logging.basicConfig(
    format="%(asctime)s %(threadName)s %(filename)s:%(lineno)s [%(message)s]", 
    datefmt="%Y-%m-%d %H:%M:%S",
    level=logging.INFO
)


x = 0

def incr(loops):
    global x
    for _ in range(loops):
        time.sleep(0.001)
        x += 1
    logging.info("x = %d", x)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="test thread local"
    )
    parser.add_argument("-l", "--loops", dest='loops', type=int, default=100, help="loop number for pear thread")
    parser.add_argument("-t", "--threads", dest='threads',type=int, default=10, help="thread number")
    
    args = parser.parse_args()

    for i in range(args.threads):
        threading.Thread(target=incr, args=(args.loops, )).start()
```

此时，我们在执行上述代码。每个线程输出的结果将时不同的

```shell
➜  python python test.py -l 1000 -t 10
2025-05-14 22:39:35 Thread-9 (incr) test.py:21 [x = 9956]
2025-05-14 22:39:35 Thread-6 (incr) test.py:21 [x = 9968]
2025-05-14 22:39:35 Thread-4 (incr) test.py:21 [x = 9974]
2025-05-14 22:39:35 Thread-8 (incr) test.py:21 [x = 9980]
2025-05-14 22:39:35 Thread-2 (incr) test.py:21 [x = 9982]
2025-05-14 22:39:35 Thread-1 (incr) test.py:21 [x = 9983]
2025-05-14 22:39:35 Thread-10 (incr) test.py:21 [x = 9984]
2025-05-14 22:39:35 Thread-3 (incr) test.py:21 [x = 9997]
2025-05-14 22:39:35 Thread-7 (incr) test.py:21 [x = 9998]
2025-05-14 22:39:35 Thread-5 (incr) test.py:21 [x = 10000]
```

> [!attention] 
> 
> 注意到，输出的结果中，全局变量 `x` 的值远远超出了目标值 `1000`。这是因为这 $10$ 线程访问和修改的是同一个变量
> 

我们看另一个例子

```python
import argparse
import logging
import threading
import time


logging.basicConfig(
    format="%(asctime)s %(threadName)s %(filename)s:%(lineno)s [%(message)s]", 
    datefmt="%Y-%m-%d %H:%M:%S",
    level=logging.INFO
)

class A:
    pass


a = A()


def incr(loops):
    a.x = 0
    for _ in range(loops):
        time.sleep(0.001)
        a.x += 1
    logging.info("x = %d", a.x)


if __name__ == "__main__":

    parser = argparse.ArgumentParser(
        description="test thread local"
    )
    parser.add_argument("-l", "--loops", dest='loops', type=int, default=100, help="loop number for pear thread")
    parser.add_argument("-t", "--threads", dest='threads',type=int, default=10, help="thread number")
    
    args = parser.parse_args()

    for i in range(args.threads):
        threading.Thread(target=incr, args=(args.loops, )).start()
```

> [!tip] 
> 
> 上述例子展示了多个线程共享一个实例对象。在每个线程中都会在这个共享的实例上设置和访问属性 `x`
> 
> 默认情况下，这些线程访问共享实例的属性 `x` 会访问到同一个对象
> 

现在，**我们期望每个线程访问和修改它自己独有的数据**。为了实现这样的效果，Python 提供了 `threading.local` 类用于让每个线程存储它自己的数据

> [!tip] 
> 
> 线程在 `local` 实例上创建的属性只属于当前线程，其他线程是无法获取的。
> 
> 关于 `threading.local` 类的原理，请参考 [[Linux 系统编程：线程安全#线程特有数据]] 
> 

下面的这个例子验证了每个线程在 `local` 实例上创建的属性只属于该线程

```python
import logging
import threading


logging.basicConfig(
    format="%(asctime)s %(threadName)s %(filename)s:%(lineno)s [%(message)s]", 
    datefmt="%Y-%m-%d %H:%M:%S",
    level=logging.INFO
)


local = threading.local()

x = 100  # 全局变量
local.x = 200  # 主线程数据

logging.info("x = %d \t local.x = %d", x, local.x)


def routine():
    local.y = 200  # 子线程数据

    try:
        logging.info("x = %d\t local.y = %d \t local.x = %d", x, local.y, local.x)
    except AttributeError as e:
        logging.error(e)

t = threading.Thread(target=routine)
logging.info("创建线程对象")
t.start()
logging.info("启动线程，并等待结束")
t.join()
logging.info("线程结束")

try:
    logging.info("x = %d\t local.y = %d \t local.x = %d", x, local.y, local.x)
except AttributeError as e:
    logging.error(e)
```

执行上述例程输出的结果为

```shell
➜  python python test.py
2025-05-15 22:09:36 MainThread test.py:17 [x = 100       local.x = 200]
2025-05-15 22:09:36 MainThread test.py:29 [创建线程对象]
2025-05-15 22:09:36 Thread-1 (routine) test.py:26 ['_thread._local' object has no attribute 'x']
2025-05-15 22:09:36 MainThread test.py:31 [启动线程，并等待结束]
2025-05-15 22:09:36 MainThread test.py:33 [线程结束]
2025-05-15 22:09:36 MainThread test.py:38 ['_thread._local' object has no attribute 'y']
```

观察输出结果
+ `Thread-1` 无法获取到属于 `MainThread` 的局部数据 `local.x` 
+ `MainThread` 也无法获取属于 `Thread-1` 的局部数据 `local.y`

> [!seealso] 
> 
> 关于线程安全的详细内容参考 [[Linux 系统编程：线程安全]]
> 

### local 类的实现原理

`threading.local()` 的源码如下

```python
class local:
	# 限制 local 实例只有两个属性 _local_impl 和 __dict__
    __slots__ = '_local__impl', '__dict__'

    def __new__(cls, /, *args, **kw):
        if (args or kw) and (cls.__init__ is object.__init__):
            raise TypeError("Initialization arguments are not supported")
        # 调用 object 的 __new__ 方法创建 local 类的实例
        self = object.__new__(cls)
	    
	    # 创建 _localimpl 类的实例
        impl = _localimpl()
        impl.localargs = (args, kw)
        
        # 多读-单写锁，保护 local.__dict__ 属性的操作
        impl.locallock = RLock()
        
        # 在 local 类的实例上添加一个 _local_impl 属性
        # 其属性值为 _localimpl 类的实例
        object.__setattr__(self, '_local__impl', impl)
	    
        # 我们需要在__init__被调用之前创建 **线程字典**，
        # 以确保我们自己不会再次调用它。
        impl.create_dict()
	    
	    # 最后返回 local 类的实例
        return self

    def __getattribute__(self, name):
	    #  实现 local 实例的属性访问操作
        with _patch(self):
	        # 将属性访问委托给 object 的 __getattribute__ 方法
            return object.__getattribute__(self, name)

    def __setattr__(self, name, value):
	    # 实现 local 实例的属性设置操作
        if name == '__dict__':
            raise AttributeError(
                "%r object attribute '__dict__' is read-only"
                % self.__class__.__name__)
        with _patch(self):
	        # 将属性设置委托给 object 的 __setattr__ 方法
            return object.__setattr__(self, name, value)

    def __delattr__(self, name):
	    # 实现 local 实例的属性删除操作
        if name == '__dict__':
            raise AttributeError(
                "%r object attribute '__dict__' is read-only"
                % self.__class__.__name__)
        with _patch(self):
	        # 将属性删除委托给 object 的 __delattr__ 方法
            return object.__delattr__(self, name)
```

阅读上述代码之后，我们发现实现线程局部属性的类并不是 `local` 类，而是 `_localimpl` 类。下面我们继续阅读 `_localimpl` 类

```python
class _localimpl:
    """管理线程本地字典的类"""
    # 现在 _localimpl 类的实例只允许有如下属性
    __slots__ = 'key', 'dicts', 'localargs', 'locallock', '__weakref__'

    def __init__(self):
	    # 创建 key：对于每一个 _localimpl 实例都不相同
        self.key = '_threading_local._localimpl.' + str(id(self))
        # 创建字典：它将 id(Thread):(ref(Thread), thread-local dict)
        self.dicts = {}

    def get_dict(self):
        """返回当前线程的字典。如果没有定义则引发 KeyError。"""
        thread = current_thread()
        return self.dicts[id(thread)][1]

    def create_dict(self):
        """为当前线程创建一个新字典，并返回它。"""
        localdict = {}  # 创建一个局部字典
        key = self.key  # 获取当前的 key
        thread = current_thread()  # 获取当前线程
        idt = id(thread)  # 当前线程对象的 id
        
        def local_deleted(_, key=key):
	        # 当 _localimpl 实例被删除，
	        # 则需要删除 current_thread().__dict__[key]
            thread = wrthread()
            if thread is not None:
                del thread.__dict__[key]
        
        def thread_deleted(_, idt=idt):
	        # 当线程对象被删除时，同步删除该线程的局部字典
            local = wrlocal()  # 返回 _localimpl 对象，即 self
            if local is not None:
                dct = local.dicts.pop(idt)  # 删除局部字典
        
        # ref 创建关联关系。wrlocal() 就会返回 slef
        wrlocal = ref(self, local_deleted)
        # ref 创建关联关系：wrthread() 返回 thread
        wrthread = ref(thread, thread_deleted)
	    
	    # 在当前线程对象中增加一个属性
        thread.__dict__[key] = wrlocal
        
        # 保存当前线程对应的局部字典
        self.dicts[idt] = wrthread, localdict
        return localdict
```

下面我们串联一下上述代码。首先，我们使用 `local = threading.local()` 实例化一个 `local` 实例。

```python
local = threading.local()
```

在实例化 `local()` 类时，自动创建 `_localimpl` 类的实例并保存在 `local` 实例的 `_local__impl` 属性上，然后再调用 `_localimpl.create_dict()` 创建线程的局部字典。到此，`local()` 类的实例化才算完成

接下来，我们使用 `local.x = 10` 在 `local` 实例上创建属性 `x` 并赋值为 `10`。这一步，会触发 `local.__setattr__()` 方法。`local.__setattr__()` 方法做 $3$ 间事情
+ 首先，检查创建的属性是否为 `__dict__`，如果是抛出 `AttributeError`。这个属性不允许线程直接访问
+ 由于要在 `local` 实例上设置属性，因此需要 `local` 实例由 `__dict__` 属性。然而，此时 `local` 实例并没有 `__dict__` 属性。现在要做的就是将创建 `__dict__` 属性，并设置为当前线程的局部字典。创建 `__dict__` 属性的是 `_patch()` 函数
+ 最后，将设置属性的操作委托给 `object` 的 `__setattr__()` 方法

现在，最关键的一点出现了。就是如何获取到当前线程的局部字典，并赋值给 `local.__dict__` 属性。实现这一步的关键就是 `_patch()` 函数。该函数的实现如下代码

```python
@contextmanager
def _patch(self):
	# 获取当前 local 实例的 _local__impl 属性的值。即 _localimpl 类实例
    impl = object.__getattribute__(self, '_local__impl')
    try:
	    # 获取当前线程的局部字典
        dct = impl.get_dict()
    except KeyError:
	    # 如果当前线程没有局部字典，则创建局部字典
        dct = impl.create_dict()
        args, kw = impl.localargs
        self.__init__(*args, **kw)
    # 加锁
    with impl.locallock:
	    # 将 local 实例的 __dict__ 属性设置为当前线程的局部字典
        object.__setattr__(self, '__dict__', dct)
        yield
```

## 定时任务：Timer

`Timer` 用于执行 **定时任务**：某个任务应该在等待一定的时间之后运行。 `Timer` 类是 `Thread` 类的子类，因此可以像一个自定义线程一样工作。

与线程一样，定时器也是通过调用其 `Timer.start()` 方法来启动的。 定时器可以通过调用 `cancel()` 方法来停止（在其动作开始之前）。 定时器在执行其任务之前要等待的 **时间间隔** 可能与用户指定的时间间隔不完全相同。

```python
def hello():
    print("hello, world")

t = Timer(30.0, hello)
t.start()  # 30 秒之后，将执行一次 hello() 函数，执行完毕后整个线程就结束了
```

> [!tip] 
> 
> `Timer` 就是等待一定时间后执行一次任务，然后结束
> 

下面我们来看 `Timer` 类的实现。
+ 继承 `Thread` 类，并重写 `__init__()` 和 `run()` 方法
+ 为了实现 `cancle()` 操作，还应该有一个 `event` 对象

```python
class Timer(threading.Thread):

    def __init__(self, interval, function, args=None, kwargs=None):
        super().__init__()
        self.interval = interval
        self.function = function
        self.args = args if args else ()
        self.kwargs = kwargs if kwargs else {}
        self.finished = threading.Event()
    
    def run(self):
        """执行定时任务"""
        self.finished.wait(self.interval)
        if not self.finished.is_set():
            self.function(*self.args, **self.kwargs)
        self.finished.set()

    def cacle(self):
        """用于在任务开始前取消任务"""
        self.finished.set()
```
