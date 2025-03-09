# 并发编程

**并行是并发的一种特殊情况**。所有并行系统都是并发的，但不是所有并发系统都是并行的。在 $21$ 世纪初，我们使用单核设备在 GNU Linux 上同时处理 $100$ 个进程。一台拥有 $4$ 个 CPU 核的现代笔记本计算机，在正常情况下，任何时间段内运行的进程数随随便便都会超过 $200$ 个。如果并行执行 $200$ 个任务，则需要 $200$ 个核。因此，多数计算实际是并发的，而不是并行的

> [!tip] 
> 
> 操作系统管理着数百个进程，确保每个进程都有机会取得进展，即使 CPU 本身同时做的事情不能超过 $4$ 件
> 

导致并发编程困难的因素很多，但我想谈谈最基本的因素：启动线程或进程十分容易，**关键是如何跟踪线程或进程**。

调用一个函数，发出调用的代码开始阻塞，直到函数返回。因此，你知道函数什么时候执行完毕，而且能轻松地得到函数的返回值。如果函数可能抛出异常，则把函数调用放在 `try/except` 块中，捕获错误

然而，这些熟悉的概念在动线程或进程后都不可用了：你无法轻松地得知操作何时结束，若想获取结果或捕获错误，则需要设置某种通信信道

> [!seealso] 
> 
> 进程间通信可以参考 [[System V 共享内存]]  和 [[System V 消息队列]]
> 

启动进程或线程需要一定的消耗，仅仅为了计算一个结果就退出，肯定得不偿失。通常，更好的选择是让各个线程或进程进入一个“职程”（worker），循环等待要处理的输入，以此分摊启动成本。但是，这又进一步增加了通信难度，还会引起更多问题。如果不需要职程了，那么如何退出呢？怎样退出才能做到不中断作业，避免留下未处理完毕的数据和未释放的资源（例如打开的文件）呢？同样，解决这些问题通常涉及消息和队列。

> [!seealso] 
> 
> 参考 [[进程池]] 和 [[线程池]]
> 

协程的启动成本很低。使用 `await` 关键字启动的协程，返回值容易获取，可以安全取消，捕获异常的位置也明确。但是，**协程通常由异步框架启动，因此监控难度与线程或进程相当**

## 一些术语的定义

> [!tip] 并发
> 
> 处理多个待定任务，一次处理一个或并行处理多个（如果条件允许），直到所有任务最终都成功或失败。对于单核 CPU，如果操作系统的调度程序支持交叉执行待定任务，也能实现并发。并发也叫多任务处理（multitasking）
> 

> [!tip] 并行
> 
> 同时执行多个计算任务的能力。需要一个多核 CPU、多个 CPU、一个 GPU 或一个集群中的多台计算机。
> 

> [!tip] 执行单元
> 
> **并发执行代码的对象的统称**，每个对象的状态和调用栈是独立的。Python 原生支持 $3$ 种执行单元：**进程**、**线程** 和 **协程**。
> 

> [!tip] 进程
> 
> 参考 [[进程概念]] 
> 

> [!tip] 线程
> 
> 参考 [[多线程概念]]
> 

> [!tip] 协程
> 
> Python 还支持 **协程**。协程本质上就是 **可以挂起自身并在以后恢复的函数**。Python 中的协程在 **事件循环** 的监督下在单线程中执行
> 

协程支持协作式多任务处理：一个协程必须使用 `yield` 或 `await` 关键字显式放弃控制权，另一个协程才可以并发（而非并行）开展工作

> [!tip] 
> 
> 这意味着，协程中只要有导致阻塞的代码，事件循环和其他所有协程的执行就都会受到阻塞，这是与进程和线程的抢占式任务处理的不同之处
> 
> 另外，对于同一份作业，协程消耗的资源比线程或进程少。
> 

## 进程

在 Python 中创建进程非常简单，使用 `multiprocessing` 标准库中 `Process` 类就可以创建一个进程对象，通过进程对象的 `start()` 方法启动一个进程。

Python 将进程封装为了一个 `Process` 对象，创建进程对象时可能需要提供下面几个参数

| 参数       | 描述                           |
| :------- | ---------------------------- |
| `group`  | 始终为 `None`，这是为了兼容 `Thread` 类 |
| `target` | 进程执行的目标函数                    |
| `name`   | 进程的名字，这是一个字符串                |
| `args`   | 函数 `target` 需要的位置参数          |
| `kwargs` | 函数 `target` 需要的关键字参数         |
| `daemon` | 必须是一个 `bool` 类型，代表进程是否为守护进程  |

> [!tip] 
> 
> 请注意：Python 中的 `daemon` 进程并 **不是** Unix 守护进程。对于设置了 `daemon` 标志的进程，在其父进程退出时，会尝试终止其 `daemon` 子进程
> 
> 此外，**不允许在守护进程中创建子进程**。这是因为当守护进程由于父进程退出而中断时，其子进程会变成孤儿进程。
> 

下表列出来 `Process` 类为其实例对象定义了一些方法和数据属性

| 属性                    | 描述                                |
| :-------------------- | --------------------------------- |
| `process.run()`       | 调用 `target`                       |
| `process.start()`     | 启动进程，将对象的 `run()` 方法安排在一个单独的进程中调用 |
| `process.name`        | 进程的名字                             |
| `process.is_alive()`  | 检查进程是否处于存活状态                      |
| `process.daemon`      | 进程的 `deaemon` 标志                  |
| `process.pid`         | 进程标识符。调用 `start()` 前，该值为 `None`   |
| `process.exitcode`    | 进程的退出标志。`run()` 正常退出，该值为 $0$      |
| `process.sentinel`    | 系统对象的数字句柄，当进程结束时将变为  `"ready"`    |
| `process.terminate()` | 终止进程                              |
| `process.kill()`      | 终止进程                              |
| `process.close()`     | 关闭 `Process` 对象，释放与之关联的所有资源       |
| `process.join()`      | 等待子进程结束，必须 `start()` 之后才能调用       |

### 创建进程

在 [[Python：函数高级#递归函数]] 中，我们介绍使用递归计算斐波拉契数列第 $n$ 的递归函数，这个函数执行时间非常长，此时控制台没有任何提示，会导致用户任务程序处于阻塞状态。我们添加一个字符动画，从而告知用户程序正在执行

```python
import itertools
import multiprocessing
import time


def fib(n):
    if n <= 2:
        return 1

    return fib(n-1) + fib(n-2)


def spinner():
    for char in itertools.cycle(r'\|/-'):
        time.sleep(0.2)
        print(f"\rresult = {char}", end="")

if __name__ == '__main__':
    s = multiprocessing.Process(target=spinner)
    s.start()

    result = fib(42)
    print(f"\rresult = {result}", end="")
```

下图展示了上述代码的执行结果

![[spinner.gif]]

> [!bug] 
> 
> 上述代码还存在一个 bug：当 `fib(42)` 结束后，主进程就结束了，此时子进程并没有结束
> 

当主机进程完成 `fib(42)` 的计算后，就应该立即通知 `spinner` 子进程结束。通过调用 `spinner` 子进程的 `terminate()` 方法，从而结束子进程

```python hl:24
import itertools
import multiprocessing
import time


def fib(n):
    if n <= 2:
        return 1

    return fib(n-1) + fib(n-2)


def spinner():
    for char in itertools.cycle(r'\|/-'):
        time.sleep(0.2)
        print(f"\rresult = {char}", end="")

if __name__ == '__main__':
    s = multiprocessing.Process(target=spinner)
    s.start()

    result = fib(42)
    
    s.terminate()
    print(f"\rresult = {result}", end="")
```

此外，我们还可以通过继承 `Process` 类，然后重写 `run()` 方法的形式创建进程

```python
import itertools
import multiprocessing
import time


def fib(n):
    if n <= 2:
        return 1

    return fib(n-1) + fib(n-2)


class SpinnerProcess(multiprocessing.Process):

    def run(self):  # 重写 run 方法
        for char in itertools.cycle(r'\|/-'):
            time.sleep(0.2)
            print(f"\rresult = {char}", end="")

if __name__ == '__main__':
    s = SpinnerProcess()
    s.start()

    result = fib(42)
    s.terminate()  # 结束子进程
    print(f"\rresult = {result}")
```

### 进程间通信

使用多进程时，一般使用 **消息机制** 实现进程间通信，尽可能避免使用同步原语

> [!seealso] 
> 
> 操作系统提供的进程间通信机制
> + [[PIPE]]
> + [[FIFO]]
> + [[System V 消息队列]] 
> 
> 进程同步
> 
> + [[System V 信号量]]
> + [[文件锁]]，主要针对多进程共享文件
> 

#### Pipe：管道

消息机制包含： `Pipe()` (可以用于在两个进程进行流式信息的交换)，以及队列(能够在多个生产者和消费者之间通信)

> [!tip] 
> 
> 在 Python 中，用于进程间通信的 `Pipe()` 默认情况下类似于 [[进程池#父子进程之间共享打开文件描述]] 中介绍 `socketpair()`。简单来说，`Pipe()` 默认会创建一对全双工通信 `Connection` 对象
> 
> `Pipe()` 可以提供一个参数 `duplex`，如果将其指定为 `False`，即  `Pipe(duplex=False)` 将与 [[PIPE]] 类似
> 

> [!attention] 
> 
> 请注意：`Pipe()` 返回的是 `Connection` 对象
> 

下表列出了 `Connection` 对象支持的操作

| `Connection` 对象的方法 | 描述                 |
| :----------------- | ------------------ |
| `conn.send(obj)`   | 将对象发送到另一端          |
| `conn.recv()`      | 接收由 `send()` 发送的对象 |
| `conn.close()`     | 关闭连接               |

> [!attention] 
> 
> 请注意：`send(obj)` 会自动使用 `pickle` 序列化被发送的对象。因此，`obj` 必须是可序列化的。对于大对象 （接近 `32MiB+` ，这个值取决于操作系统 ) 有可能引发 `ValueError`  异常。
> 
> 此外，`recv()` 会自动反序列对象，因此需要明确对端是可信的才能使用 `recv()` 接收数据。如果对端关闭了连接或者没有东西可接收，那么 `recv()` 将会抛出 `EOFError`
> 
> 当 `Connection` 对象不再使用时，即引用计数为 $0$ 时，会自动调用 `close()` 方法
> 

| `Connection` 对象的方法                          | 描述                 |
| :------------------------------------------ | ------------------ |
| `conn.send_bytes(buffer[, offset[, size]])` | 发送字节数据             |
| `conn.recv_bytes([maxlength])`              | 读取对端发送的字节数据        |
| `conn.recv_bytes_into(buffer[, offset])`    | 读取字节数据到 `buffer` 中 |

> [!attention] 
> 
> 请注意：`send_bytes()` 将 `buffer` 中的内容传递给另一端，如果指定了 `offset` 和 `size`，就会从 `buffer` 的 `offset` 处开始，读取 `size` 个字节。和 `send()` 一样，过大的缓冲区 ( 接近 `32MiB+` ，此值依赖于操作系统 ) 有可能引发 `ValueError`  异常。
> 
> 
> 

`Connection` 对象支持上下文管理器。我们可以使用 `with` 语句管理 `Connection` 对象，当离开 `with` 语句块时会自动调用 `Connection` 对象的 `close()` 方法

```python
import itertools
import multiprocessing
import time
from multiprocessing import connection
import random


def productor(conn: connection.Connection):
    while True:
        data = random.randint(1, 100)
        print("productor: ", data)
        conn.send_bytes(data.to_bytes(8, byteorder='little'))
        time.sleep(0.5)


def consumer(conn: connection.Connection):
    while True:
        data = int.from_bytes(conn.recv_bytes(), byteorder='little')
        print("consumer: ", data)
        time.sleep(1)

if __name__ == '__main__':
    read, write = multiprocessing.Pipe(False)
    process_productor = multiprocessing.Process(target=productor, args=(write, ))
    process_consumer = multiprocessing.Process(target=consumer, args=(read, ))

    process_productor.start()
    process_consumer.start()

    process_productor.join()  # 等待子线程结束，并回收子进程的资源
    process_consumer.join()  # 等待子线程结束，并回收子进程的资源
    read.close()
    write.close()
```

#### Queue：队列

Python 在 `multiprocessing` 中提供了 $3$ 种队列：`Queue`, `SimpleQueue` 以及 `JoinableQueue` 都是多生产者，多消费者，并且实现了 `FIFO` 的队列类型

> [!seealso] 
> 
> 关于队列的概念，参考 [[栈和队列]]
> 

`Queue()` 返回一个使用一个管道和少量锁和信号量实现的共享队列实例。当一个进程将一个对象放进队列中时，一个写入线程会启动并将对象从缓冲区写入管道中。下表列出来 `Queue` 对象支持的操作

| `Queue` 对象的方法   | 描述        |
| :-------------- | --------- |
| `queue.qsize()` | 获取队列的估计长度 |
| `queue.empty()` | 判断队列是否为空  |
| `queue.full()`  | 判断队列是否满队  |

> [!tip] 
> 
>  `qsize()` `empty()` `full()` 返回的状态是不可信的，因为在多进程的上下文中，这些状态会随时发生变化
> 

| `Queue` 对象的方法                        | 描述                          |
| :----------------------------------- | --------------------------- |
| `queue.put(obj[, block[, timeout]])` | 将对象 `obj` 放入队列中             |
| `queue.put_nowait(obj)`              | 等价于 `queue.put(obj, False)` |
| `queue.get([block[, timeout]])`      | 从队列中获取对象                    |
| `queue.get_nowait()`                 | 等价于 `queue.get(False)`      |


> [!attention] 
> 
> `put()` 和 `get()` 可以提供两个额外的参数 `block` 和 `timeout`；其中 `block` 指示是否阻塞当前进程，而 `timeout` 用于表示等待的超时时间，单位为秒。换句话说，`block` 为 `True` 时，就会让进程阻塞等待 `timeout` 秒；此时，如果 `timeout` 未指定，则永久阻塞
> 

请注意：在 `multiprocessing` 中的 `Queue` 是使用管道实现的，`Queue` 会使用一个后台线程将写入到 `Queue` 缓冲区的对象写入到管道中。下面的方法可以指示进程退出时是否等待 `Queue` 的后台线程

| `Queue` 对象的方法               | 描述                        |
| :-------------------------- | ------------------------- |
| `queue.close()`             | 指示当前进程不在向队列中写入对象          |
| `queue.join_thread()`       | 阻塞当前进程，等待 `queue` 的后台线程退出 |
| `queue.cacle_join_thread()` | 这防止进程退出时自动等待后台线程退出        |

> [!warning] 
> 
> `join_thread()` 方法仅在调用了 `close()` 方法之后可用。这会 **阻塞当前进程**，直到后台线程退出，**确保所有缓冲区中的数据都被写入管道中**。
> 
> 默认情况下，如果一个 **不是队列创建者的进程试图退出**，它会尝试等待这个队列的后台线程。这个进程可以使用 `cancel_join_thread()` 让 `join_thread()` 方法什么都不做直接跳过。
> 
> 换句话说，队列的使用者而非创建者也会等待队列的后跳线程退出，然而队列的使用者可以不进行等待，因此需要调用 `cancle_join_thread()` 
> 

```python
import itertools
import multiprocessing
import time
from multiprocessing import connection
import random


def productor(q: multiprocessing.Queue):
    for i in range(1000000):
        data = random.randint(1, 100)
        print("productor: ", data)
        q.put(data)
    q.close()  # 不在写入数据
    print("productor done")
    q.cancel_join_thread()  # 生产者不用等待后台线程结束


def consumer(q: multiprocessing.Queue):
    time.sleep(1)
    for _ in range(1000000):
        data = q.get()
        print("consumer: ", data)
        time.sleep(1)

if __name__ == '__main__':
    q = multiprocessing.Queue(maxsize=1000000)

    process_productor = multiprocessing.Process(target=productor, args=(q, ))
    process_consumer = multiprocessing.Process(target=consumer, args=(q, ))

    process_productor.start()
    process_consumer.start()

    process_productor.join()  # 等待生产者
    print("===================")

    process_consumer.join()
    print("====================")
```

---

`SimpleQueue` 是 `Queue` 的简化版本，与 `Pipe()` 类似。`SimpleQueue` 只提供了下表列出的 $4$ 个方法

| `SimpleQueue` 对象的方法 | 描述               |
| :------------------ | ---------------- |
| `queue.close()`     | 关闭队列：释放内部资源      |
| `queue.empty()`     | 判断队列是否为空         |
| `queue.get()`       | 从队列中获取值          |
| `queue.put(item)`   | 将 `item` 对象放入队列中 |

> [!attention] 
> 
> 请注意：`close()` 队列后，就不可再被使用。 例如不可再调用 `get()`, `put()` 和 `empty()` 等方法。
> 

---

`JoinableQueue` 是 `Queue` 的子类，只添加了 `task_done()` 和 `join()` 两个方法。

| `JoinableQueue` 对象的方法 | 描述                   |
| :-------------------- | :------------------- |
| `queue.task_done()`   | 指出之前进入队列的任务已经完成      |
| `queue.join()`        | 阻塞至队列中所有的元素都被接收和处理完毕 |

> [!tip] 
> 
> `task_done()` 方法由队列的消费者使用，当消费者调用 `get()` 获取任务并在处理完毕之后，就应该调用 `task_done()` 告知队列
> 
> 当条目添加到队列的时候，未完成任务的计数就会增加。每当消费者进程调用 `task_done()` 表示这个条目已经被回收，该条目所有工作已经完成，未完成计数就会减少。当未完成计数降到零的时候， `join()` 阻塞被解除。
> 

---

强制停止子进程可能会造成资源泄露。一个进程持有锁或者操作了信号量，此时，如果进程被强行终止，锁和信号量的状态没有被恢复，其他进程可能永远也无法执行，从而陷入死锁。

> [!tip]
> 
> 我们不应该强制终止进程，而是给进程发送一个代表终止的信号，让进程自行选择何时终止，并释放进程持有的资源
> 

由于进程之间是相互隔离的，因此需要额外的通信机制。在 `multiprocessing` 库中提供了许多的进程间通信机制。这里，我们使用 `Queue` 完成通知子进程退出的任务

> [!tip] 
> 
> 请注意，这里还有一个小 bug：主进程通知子进程退出后，主进程和子进程谁先退出是未知，通常我们要求主进程等待全部的子进程退出后，主进程才允许退出
> 

```python
import itertools
import multiprocessing
import queue
import time

def fib(n):
    if n <= 2:
        return 1
    return fib(n-1) + fib(n-2)


def spinner(q: multiprocessing.Queue):
    for char in itertools.cycle(r'\|/-'):
        try:
            q.get_nowait()
        except queue.Empty:  # 队列中没有数据，证明主进程还在计算
            time.sleep(0.2)
            print(f"\rresult = {char}", end="")
        else:
            break


if __name__ == '__main__':

    q = multiprocessing.Queue()

	# 创建子进程对象
    s = multiprocessing.Process(target=spinner, args=(q, ))
    s.start()  # 启动子进程

    result = fib(42)
	# 计算完毕，通知子进程退出
    q.put_nowait("exit")
    s.join()  # 等待子进程结束
    print(f"\rresult = {result}")  # 主进程输出结果
```

#### 共享内存

关于共享内存的原理，参考 [[System V 共享内存]]。下面我们学习 Python 中的共享内存

`muiltprocess` 中提供了的 `Value` 和 `Array` 共享内存对象能够被子进程继承。换句话说，就是使用 `Value` 和 `Array` 对象的进程必须是有关系的

> [!important] 
> 
> 默认情况下，`Value` 和 `Array` 对象是被同步器包装过的
> 

```python
multiprocessing.Value(typecode_or_type, *args, lock=True)
```

```python
multiprocessing.Array(typecode_or_type, size_or_initializer, *, lock=True)
```

> [!tip] 参数 `typecode_or_type`：指明了返回的对象类型:
> 
> 它可能是一个 `ctypes` 类型或者 `array`  模块中每个类型对应的单字符长度的字符串
> 

```python
import ctypes
import itertools
import multiprocessing
import time
from multiprocessing import connection
import random


def worker(value: multiprocessing.Value, array:multiprocessing.Array):

    for i in range(1, 101):
        value.value += i
        array[i-1] = i * i


if __name__ == '__main__':

    value = multiprocessing.Value(ctypes.c_int64, 0)    # 类型为 int64 的共享对象
    array = multiprocessing.Array(ctypes.c_int64, 100)  # 长度为 100，元素类型为 int64 的共享数组

    w = multiprocessing.Process(target=worker, args=(value, array))
    w.start()

    w.join() # 等待子进程结束

    print(value.value)
    for i in array:
        print(i)
```

> [!tip] 
> 
> Python 中也提供与 [[System V 共享内存]] 相似的方案。这会为每个共享内存提供一个名字，其他进程就可以通过这个名字访问到共享内存了。详细参考 [shared_memary](https://docs.python.org/zh-cn/3/library/multiprocessing.shared_memory.html)
> 

```python
>>> from multiprocessing.shared_memory import SharedMemory
>>> sm = SharedMemory(name="xxxx", create=True, size=100)  # 创建共享内存
>>> sm.buf[0] = 100

############################################################

>>> from multiprocessing.shared_memory import SharedMemory
>>> sm = SharedMemory(name="xxxx")  # 获取已有的共享内存
>>> sm.buf[0]
100
```

### 同步

通常来说同步原语在多进程环境中并不像它们在多线程环境中那么必要。因此，这里我们不介绍进程同步原语

> [!tip] 
> 
> 通常，进程同步采用向进程发送消息来完成。也就是说，使用管道
> 

## 线程

有关于线程的概念，参考 [[多线程概念]]。在 Python 中，线程的 API 与进程类似。只需要将 `Process` 的位置修改为 `Thread` 即可

```python
import itertools
import threading
import time


def fib(n):
    if n <= 2:
        return 1

    return fib(n-1) + fib(n-2)


def spinner():
    for char in itertools.cycle(r'\|/-'):
        time.sleep(0.2)
        print(f"\rresult = {char}", end="")

if __name__ == '__main__':
    s = (threading.Thread(target=spinner))
    s.start()

    result = fib(42)
    print(f"\rresult = {result}")
```

当然，如果期望主线程推出时，子线程也应该全部退出，那么就应该将子线程设置为主线程的守护线程

```python
import itertools
import threading
import time


def fib(n):
    if n <= 2:
        return 1

    return fib(n-1) + fib(n-2)


def spinner():
    for char in itertools.cycle(r'\|/-'):
        time.sleep(0.2)
        print(f"\rresult = {char}", end="")

if __name__ == '__main__':
    s = threading.Thread(target=spinner, daemon=True)
    s.start()

    result = fib(42)
    print(f"\rresult = {result}")
```

> [!warning] 
> 
> 在 CPython 中，由于存在 **全局解释器锁**，**同一时刻只有一个线程可以执行 Python 代码**（虽然某些性能导向的库可能会去除此限制）。 如果你想让你的应用更好地利用多核心计算机的计算资源，推荐你使用 `multiprocessing` 或 `concurrent.futures.ProcessPoolExecutor`。 但是，如果你想要同时运行多个 I/O 密集型任务，则多线程仍然是一个合适的模型
> 

### 线程同步

由于线程之间共享进程的地址空间，因此线程之间通信只需要使用共享变量即可。但是，共享变量带来了额外的危险：**数据竞争**。为了解决竞争带来的错误结果，下面我介绍同步原语

#### 互斥锁







