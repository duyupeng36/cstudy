# 线程

关于线程的概念，请参考 [[操作系统：多线程概念]]。同时，在 [[Python：并发执行]] 中介绍了在 Python 中何时使用 **线程**、**进程**、 **异步任务**。线程运行你的程序的不同部分同时执行。使用线程可以简化你的程序设计

**线程是一个独立的执行流**。这意味着您的 **程序将同时发生两件事**。但对于大多数 Python3 实现，不同的线程实际上并不真正同时执行：它们只是看起来如此。

线程可以想象成有多个不同的处理器在运行你的程序，每个处理器同时执行一个独立任务。这几乎是正确的。线程可能在不同处理器上运行，但它们一次只会运行一个。

> [!tip] 
> 
> 线程要执行必须占用一个 CPU 核心。这操作由操作系统决定
> 

同时运行多个任务需要 Python 的非标准实现，用不同的语言编写一些代码，或者使用多处理（这会带来一些额外的开销）。

由于 CPython 的实现方式，线程可能无法加快所有任务的执行。这是由于与 **GIL** 的交互，实际上 **限制了每次只能有一个 Python 线程运行**

**任务花费大量时间等待外部事件通常适合使用线程**。需要大量 CPU 计算且很少等待外部事件的难题可能根本不会运行得更快。

> [!tip] 
> 
> 这是适用于用 Python 编写并在标准 CPython 实现上运行的代码的情况。如果你的线程是用 C 编写的，它们有释放 GIL 并并发运行的能力。如果你在运行不同的 Python 实现，请查阅文档了解它是如何处理线程的
> 

## 启动线程

现在你已经了解了线程的概念，让我们学习如何创建一个线程。Python 标准库提供了线程模块(`threading`)。在这个模块中，`Thread` 类很好地封装了线程，提供了一个干净的工作接口

```python hl:16,18
import logging
import threading
import time

def thread_function(name):
    logging.info("Thread %s: starting", name)
    time.sleep(2)
    logging.info("Thread %s: finishing", name)

if __name__ == "__main__":
    format = "%(asctime)s: %(message)s"
    logging.basicConfig(format=format, level=logging.INFO,
                        datefmt="%H:%M:%S")

    logging.info("Main    : before creating thread")
    x = threading.Thread(target=thread_function, args=(1,))
    logging.info("Main    : before running thread")
    x.start()
    logging.info("Main    : wait for the thread to finish")
    x.join()  # 阻塞调用该方法的线程，等待线程 x 结束
    logging.info("Main    : all done")
```

> [!tip] 
> 
> 请关注第 $16$ 行和第 $18$ 行代码。这两行代码是 Python 中 **创建和启动线程** 的关键代码
> 

当你创建一个线程对象时，你传递给它一个 **函数** 和一个包含 **该函数参数的列表**。在这种情况下，你正在告诉线程运行 `thread_function()` 并将 $1$ 作为参数传递给它。

线程函数本身并不做什么。它只是在 `time.sleep()` 前后记录一些消息。当你以当前形式运行此程序时，输出将如下所示

```shell
23:46:14: Main    : before creating thread
23:46:14: Main    : before running thread
23:46:14: Thread 1: starting
23:46:14: Main    : wait for the thread to finish
23:46:16: Thread 1: finishing
23:46:16: Main    : all done
```

## 守护线程

Python 线程对守护进程有更具体的含义。**守护线程将在程序退出时立即关闭**。考虑这些定义的一种方式是将守护线程视为在后台运行而不必担心关闭的线程。

程序在运行非守护线程时，将等待这些线程完成后再终止。然而，守护线程在程序退出时将在任何位置被终止。

> [!tip] 
> 
> 也就是说，主线程退出，所有守护子线程都会被强制终止；但是，主线程在退出前会等待所有非守护子线程正常结束
> 

让我们更仔细地看看上面程序的结果。最后两行是有趣的部分。当您运行程序时，您会注意到在 `__main__` 打印出 `"all done"` 消息后，线程完成之前有一个暂停（大约2秒）。

```shell hl:5-6
00:55:12: Main    : before creating thread
00:55:12: Main    : before running thread
00:55:12: Thread 1: starting
00:55:12: Main    : wait for the thread to finish
00:55:12: Main    : all done
00:55:14: Thread 1: finishing
```

这段暂停是 Python 在等待非守护线程完成。**当你的 Python 程序结束时，关闭过程的一部分是清理线程例程**

如果你查看 Python 线程的源代码，你会看到 `threading._shutdown()` 会遍历所有正在运行的线程，并对每个没有设置守护进程标志的线程调用 `.join()`

所以你的程序等待退出，因为线程本身正在睡眠中。一旦它完成并打印了消息，`.join()` 将返回，程序可以退出。

通常，这种行为是您想要的，但还有其他可用的选项。让我们首先用守护线程重复程序。您可以通过改变构造 `Thread` 的方式，添加守护线程标志来实现：

```python hl:16,21
import logging
import threading
import time

def thread_function(name):
    logging.info("Thread %s: starting", name)
    time.sleep(2)
    logging.info("Thread %s: finishing", name)

if __name__ == "__main__":
    format = "%(asctime)s: %(message)s"
    logging.basicConfig(format=format, level=logging.INFO,
                        datefmt="%H:%M:%S")

    logging.info("Main    : before creating thread")
    x = threading.Thread(target=thread_function, args=(1,), daemon=True)  # 设置守护线程
    logging.info("Main    : before running thread")
    x.start()

    logging.info("Main    : wait for the thread to finish")
    # x.join()
    logging.info("Main    : all done")

```

当你现在运行程序时，你应该看到这个输出：

```
01:02:02: Main    : before creating thread
01:02:02: Main    : before running thread
01:02:02: Thread 1: starting
01:02:02: Main    : wait for the thread to finish
01:02:02: Main    : all done
```

这里的区别在于输出文件的最后一行丢失了。`thread_function()` 没有机会完成。它是一个守护线程，当 `__main__` 到达代码的末尾并且程序想要结束时，守护线程被终止了

## 连接线程

守护线程很有用，但当您想等待线程停止时怎么办？当您想这么做而不退出程序时怎么办？现在让我们回到您的原始程序，看看那条被注释掉的二十行：

```python
# x.join()
```

调用 `.join()` 可以让一个线程等待另一个线程完成。如果你取消注释那一行，主线程将暂停等待线程 `x` 完成。此时，运行程序时，输出的结果为

```
01:05:11: Main    : before creating thread
01:05:11: Main    : before running thread
01:05:11: Thread 1: starting
01:05:11: Main    : wait for the thread to finish
01:05:13: Thread 1: finishing
01:05:13: Main    : all done
```

> [!tip] 
> 
> 调用 `.join()` 就是等待线程结束，无论线程是非守护线程还是守护线程
> 

## 启动多个线程

到目前为止仅使用了两个线程：**主线程** 和 使用 `threading.Thread` 对象启动的一个线程

通常，您会想要 **启动多个线程** 并让它们执行有趣的工作。让我们先看看这种做法的难点，然后再过渡到更简单的方法。

```python
import logging
import threading
import time

def thread_function(name):
    logging.info("Thread %s: starting", name)
    time.sleep(2)
    logging.info("Thread %s: finishing", name)

if __name__ == "__main__":
    format = "%(asctime)s: %(message)s"
    logging.basicConfig(format=format, level=logging.INFO,
                        datefmt="%H:%M:%S")

    threads = list()
    for index in range(3):  # 创建 3 个线程
        logging.info("Main    : create and start thread %d.", index)
        x = threading.Thread(target=thread_function, args=(index,))
        threads.append(x)
        x.start()

    for index, thread in enumerate(threads):  # 等待这 3 个线程结束
        logging.info("Main    : before joining thread %d.", index)
        thread.join()
        logging.info("Main    : thread %d done", index)
```

这段代码使用了与上面相同的方法来启动一个线程，创建一个 `Thread` 对象，然后调用 `.start()`。程序保留一个 `Thread` 对象列表，以便稍后使用 `.join()` 等待它们。

执行上述代码会产生一些有趣的结果

```shell hl:8-10
01:31:58: Main    : create and start thread 0.
01:31:58: Thread 0: starting
01:31:58: Main    : create and start thread 1.
01:31:58: Thread 1: starting
01:31:58: Main    : create and start thread 2.
01:31:58: Thread 2: starting
01:31:58: Main    : before joining thread 0.
01:32:00: Thread 2: finishing
01:32:00: Thread 0: finishing
01:32:00: Thread 1: finishing
01:32:00: Main    : thread 0 done
01:32:00: Main    : before joining thread 1.
01:32:00: Main    : thread 1 done
01:32:00: Main    : before joining thread 2.
01:32:00: Main    : thread 2 done
```

如果你仔细查看输出，你会看到所有三个线程按照你可能预期的顺序启动，但在这种情况下，它们结束的顺序却不是启动顺序！多次运行会产生不同的顺序

**线程的运行顺序由操作系统决定，可能很难预测**。它可能（并且很可能）在每次运行时都不同，因此在设计使用线程的算法时，您需要对此有所了解。幸运的是，Python 为你提供了几个原语，以帮助协调线程并使它们一起运行

## 竞争条件

在 [[操作系统：多线程概念]]   中我们介绍过，**线程共享进程虚拟内存**。也就是说，进程中的所有线程都可以访问进程持有的资源。

当多个线程同时修改共享资源时，由于修改操作不是原子的，从而就会导致 **竞争条件** 的出现。

> [!tip] 
> 
> 竞争条件可能在两个或更多线程访问共享数据或资源时发生
> 

在这个例子中，你将创建一个每次都会发生的严重竞争条件，但请注意，大多数竞争条件并不像这样明显。通常，它们只会偶尔发生，并且可能产生令人困惑的结果。正如你可以想象的，这使得它们相当难以调试。

幸运的是，这种竞争条件每次都会发生，您将详细走过这个过程来解释正在发生的事情。

这是一个例子，你需要编写一个更新数据库的类。好吧，你实际上并不会有一个数据库：你只是假装有。

您的 `FakeDatabase` 将具有 `.__init__()` 和 `.update()` 方法：


```python
import logging
import time

class FakeDatabase:
    def __init__(self):
        self.value = 0

    def update(self, name):
        logging.info("Thread %s: starting update", name)
        local_copy = self.value
        local_copy += 1
        time.sleep(0.1)
        self.value = local_copy
        logging.info("Thread %s: finishing update", name)
```

`FakeDatabase` 正在跟踪一个单一数字：`.value`。这将是您将看到的竞争条件的共享数据。`.__init__()` 简单初始化 `.value` 为零。到目前为止，一切顺利。

`.update()` 看起来有点奇怪。它模拟从数据库读取一个值，对其进行一些计算，然后将新值写回数据库。在这种情况下，从数据库读取只是将 `.value` 复制到局部变量。计算只是将值加一，然后调用 `.sleep()` 稍作等待。最后，通过将局部值复制回 `.value` 来写回值。


```python
if __name__ == '__main__':
    format = "%(asctime)s: %(message)s"
    logging.basicConfig(format=format, level=logging.INFO,
                        datefmt="%H:%M:%S")
    database = FakeDatabase()  # 所有线程都引用同一个对象
    logging.info("Testing update. Starting value is %d.", database.value)
    t1 = threading.Thread(target=database.update, args=(0,))
    t2 = threading.Thread(target=database.update, args=(1,))
    t1.start()
    t2.start()
    t1.join()
    t2.join()
    logging.info("Testing update. Ending value is %d.", database.value)
```

每个线程运行 `.update()`，而 `.update()` 会将 `.value` 加一，你可能会期望在最后打印时 `database.value` 为2。但如果你这样认为，你就不需要看这个例子了。如果你运行上面的代码，输出看起来像这样：

``` hl:6
02:30:16: Testing update. Starting value is 0.
02:30:16: Thread 0: starting update
02:30:16: Thread 1: starting update
02:30:17: Thread 0: finishing update
02:30:17: Thread 1: finishing update
02:30:17: Testing update. Ending value is 1.
```

当我们启动两个线程，并告诉它们执行 `database.update()`。**每个线程都将引用同一个 FakeDatabase 对象**，即 `database`。每个线程还将有一个独特的值，即 `index`，这使得日志语句更容易阅读

![[Pasted image 20250426152838.png]]

当线程开始运行 `.update()` 时，它会拥有函数本地所有数据的一个自己的版本。在 `.update()` 的情况下，这是 `local_copy`。这绝对是个好事。否则，运行相同函数的两个线程总是会互相混淆。这意味着局部作用域中的变量(函数内部定义的局部变量)都是线程安全的。

### 一个线程

下图展示了如果只有一个线程时执行 `.update()` 的过程。左侧显示语句，随后是显示线程的局部副本和共享数据库 `.value` 的值

![[Pasted image 20250426154341.png]]

`Thread 1` 启动时，`FakeDatabase.value` 的值为零。该方法的第一行代码，`local_copy = self.value`，将值零复制到局部变量中。接下来，它使用 `local_copy += 1` 语句增加`local_copy` 的值。您可以看到 `Thread 1` 中的 `.value` 被设置为 `1`。

调用 `time.sleep()` 函数，暂停当前线程，并允许其他线程运行。由于本例中只有一个线程，因此此函数没有任何效果。

当 `Thread 1` 醒来并继续时，它将新值从 `local_copy` 复制到 `FakeDatabase.value`，然后线程完成。你可以看到 `database.value` 被设置为 `1`

