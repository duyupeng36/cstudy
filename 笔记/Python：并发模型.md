# 并发模型

**并发** 指的是程序同时管理多个任务的能力，从而提高性能和响应速度。它包括不同的模型，如 **多线程**、**多进程** 和 **异步任务**，每个模型都提供独特的优势和权衡

> [!tip] 
> 
> 所谓的并发是指在同一个时间段同时执行多个任务的能力。参考 [[操作系统：进程概念]]
> 

在 Python 中，多线程和异步任务只在一个 CPU 核心上执行；只有多进程才可以利用多个 CPU 核心实现真正的 **并行**

> [!tip] 
> 
> 并行是在同一时刻可以同时执行多个任务，它是一种特殊并发
> 

## 并发是什么？

并发在字典中的含义是 **同时发生**。在 Python 中，同时发生的事情有不同的称呼，包括：线程、任务、进程。

本质上，**它们都指一系列按顺序执行的指令**。你可以把它们想象成不同的 _思维过程_。每一个都可以 _在特定点停止_，处理它们的 CPU 可以 _切换到另一个_。每个思维过程的 _状态被保存起来_，以便在 **中断** 的地方恢复。

> [!tip] 
> 
> 虽然 Python 对同一概念使用不同的词汇。但是，实际上，只有在从高层次角度看待时，线程、任务和进程才是相同的。
> 
> 一旦你开始深入研究细节，你会发现它们都代表略有不同的事物
> 

现在，你需要考虑定义中的并发部分。你必须小心一点，因为当你深入到细节时，你会发现只有多个系统进程才能使 Python 在实际上以相同的时间运行这些思维过程。

**线程** 和 **异步任务** 总是在单个处理器上运行，这意味着它们一次只能运行一个。它们只是巧妙地找到 **_轮流执行_** 的方式来加快整体过程。即使它们不能同时运行不同的思维过程，但它们仍然属于并发概念。

> [!attention] 
> 
>   注意：大多数其他编程语言的线程通常并行运行。由于 Python 存在的 GIL（全局解释器锁）保证 Python 解释器进程只有一个线程执行。
>   
>   当然，我们也可以使用 C/C++ 编写扩容绕过 GIL 或者使用 Python3.13 版本的实验性功能：自由线程
> 

在 [[操作系统：多线程概念#线程模型]] 中，介绍了 **用户态线程** 和 **内核线程**。现代操作系统都支持内核线程；也就是说，操作系统可以在任何时候中断它以启动运行不同的线程。这种被称 **抢占式** 多任务处理，因为操作系统可以抢占您的线程或进程以进行切换

抢占式多任务处理很方便，因为线程中的代码不需要做任何特殊操作来切换。这也可能因为任何时间点都可能发生上下文切换而变得困难。上下文切换可能发生在单个 Python 语句的中间，甚至是一个像 `x = x + 1` 这样的简单语句。这是因为 Python 语句通常由多个低级字节码指令组成

> [!tip] 
> 
> CPU 在执行 `x = x + 1` 至少会分为 $3$ 步执行
> + 读取 `x` 的值
> + 计算 `x + 1`
> + 将 `x + 1` 的结果写入内存
> 
> 操作系统可以在这 $3$ 个步骤之间的任意位置切换线程。如果有多个线程共享变量 `x`，那么就可能会得到错误的结果
> 

**异步任务** 被称为协作式多任务(也称为 **协程**)。任务必须通过通知它们何时准备好被切换出来而无需操作系统的参与来相互协作。这意味着任务中的代码需要稍作修改才能实现这一点。

前置额外工作的好处是您始终知道任务将在何处被切换，这使得推理执行流程更容易。在协作式多任务重，除非该语句被标记为让出 CPU，否则任务不会在 Python 语句的中间被切换。

## 并行是什么？

到目前为止，你已经了解了在单个处理器上发生的并发(轮流执行)。那么，关于你那酷炫的新笔记本电脑上的所有 CPU 核心呢？你如何在 Python 中利用它们呢？答案是执行单独的 **进程**

> [!tip] 
> 
> 这意味着，并行需要 CPU 的支持。一台计算机的 CPU 核心数决定了这台计算机能够并行执行的程序的数量
> 

进程可以被认为几乎是一个完全不同的程序，尽管从技术上讲，它通常被定义为包括内存、文件句柄等资源的一组。一种思考方式是，**每个进程都在自己的 Python 解释器中运行**。

因为它们是不同的进程，在利用多进程的程序中，你的每个思维过程可以 **在不同的 CPU 核心上运行**。在不同的核心上运行意味着它们 **实际上可以同时运行**，这真是太棒了。这样做会带来一些复杂性，但 Python 大多数时候都能很好地处理这些问题。

现在你已经了解了并发和并行性的概念，你可以回顾它们之间的区别，然后确定哪些 Python模块支持它们：

| Python 模块         | 使用 CPU 数 | 多任务类型 | 切换方式            |
| ----------------- | -------- | ----- | --------------- |
| `asyncio`         | $1$ 个    | 协作式   | 任务决定何时放弃控制      |
| `threading`       | $1$ 个    | 抢占式   | 操作系统决定何时切换      |
| `multiprocessing` | 多个       | 抢占式   | 同时在不同处理器上运行所有进程 |

## 并发有何用处？

在  [[操作系统：进程调度]] 中介绍了进程(线程)的执行行为有 $2$ 种：**CPU 执行** 和 **等待 IO**。当进程(线程)处于 **等待 IO** 状态时，它就不应该占用 CPU。

根据 CPU 执行 和 等待 IO 两种状态的分布，我们可以将任务分为两种：**IO 密集型任务** 和 **CPU 密集型任务**

如下图，展示了 IO 密集型任务执行行为


![[Pasted image 20250423010524.png]]

蓝色方框表示程序执行工作的时刻，红色方框表示等待 I/O 操作完成的耗时。此图不按比例绘制，因为互联网上的请求可能比 CPU 指令长几个数量级，所以程序可能会花大部分时间在等待。这就是你的网络浏览器大部分时间所做的事情。

另一方面，有一些程序在没有与网络通信或访问文件的情况下进行大量计算。这些是CPU密集型程序，因为限制你程序速度的资源是 CPU，而不是网络或文件系统。如下图，展示了 CPU 密集行任务的执行行为

![[Pasted image 20250423010734.png]]

下表对 IO 密集型任务 和 CPU 密集型任务做出了总结

| IO 密集型任务               | CPU 密集型任务            |
| ---------------------- | -------------------- |
| 程序大部分时间都在与慢速设备进行通信<br> | 你的程序大部分时间都花在执行CPU操作上 |
| 加快速度需要重叠等待这些设备的时间      | 加速涉及找到在相同时间内做更多计算的方法 |

## 加速 IO 密集型程序


下面我们将以 **网络 IO** 为例子。在这个例子中，你将下载几个网站上的网页，但实际上可以是任何网络流量。只是用网页来可视化和设置更方便。我们从 [https://www.jython.org](https://www.jython.org) 和 [http://olympus.realpython.org/dice](http://olympus.realpython.org/dice)  两个网站下载网页，每个网站下载 $80$ 次 

在 Python 中，发起 HTTP 请求可以使用标准库  [`urllib`](https://docs.python.org/zh-cn/3.13/library/urllib.html)；此外，第三方库 [`requests`](https://requests.readthedocs.io/en/latest/) 也是一个非常好用且简单的 HTTP 客户端

```python title:io_sync.py
import time
import requests

def download_site(url, session):
    with session.get(url) as response:
        print(f"Read {len(response.content)} bytes from {url}")

def download_all_sites(sites):
    with requests.Session() as session:
        for url in sites:
            download_site(url, session)

def main():
    sites = ["https://www.jython.org", "http://olympus.realpython.org/dice"] * 80
    start_time = time.perf_counter()
    download_all_sites(sites)
    duration = time.perf_counter() - start_time
    print(f"Downloaded {len(sites)} sites in {duration} seconds")


if __name__ == "__main__":
    main()
```

执行上述脚本，输出每个网站返回的数据大小，当所有网站被访问完毕之后，输出总共耗时

```shell
Read 10966 bytes from https://www.jython.org
Read 274 bytes from http://olympus.realpython.org/dice
.....
Read 10966 bytes from https://www.jython.org
Read 274 bytes from http://olympus.realpython.org/dice
Downloaded 160 sites in 22.760866600001464 seconds
```

> [!tip] 
> 
> Python 按顺序访问每个网站，只有当网站数据返回后，Python 才可以继续向下执行
> 
> 很显然，进程等待网络请求响应的这段时间被浪费了，从而使得我们的程序浪费了大量的等待时间
> 

请注意，这些结果可能会因您的互联网连接速度、网络拥塞等因素而显著不同。为了考虑这些因素，您应该在每次基准测试时重复几次，并取最快的运行结果。这样，您程序版本之间的差异仍然会很清晰

运行较慢并不总是大问题。如果您正在运行的程序使用同步版本仅需两秒，并且很少运行，那么添加并发可能并不值得。您可以在这里停止

### 使用线程

你的程序频繁运行怎么办？如果它需要几个小时才能运行结束呢？你可以通过使用 Python  **线程** 重写这个程序来转向并发处理。

> [!attention] 
> 
> 由于线程阻塞时，操作系统会执行调用其他线程执行。因此，在操作系统的帮助下，所有线程都会逐步向前推进。
> 

如您可能猜到的，编写利用多线程的程序需要更多努力。然而，您可能会惊讶于在基本情况下这需要多少额外的努力。下面是当您利用前面提到的 `concurrent.futures` 和 `threading` 模块时，同样的程序看起来是什么样子：

```python hl:2,3,7,9-12,16,21-22 title:io_thread.py
import time
import threading
from concurrent.futures import ThreadPoolExecutor

import requests

thread_local = threading.local()  # 线程局部数据

def get_session_for_thread():  # 每个线程获取自己的 session 对象
    if not hasattr(thread_local, "session"):
        thread_local.session = requests.Session()
    return thread_local.session


def download_site(url):
    session = get_session_for_thread()
    with session.get(url) as response:
        print(f"Read {len(response.content)} bytes from {url}")

def download_all_sites(sites):
    with ThreadPoolExecutor(max_workers=5) as executor:
        executor.map(download_site, sites)

def main():
    sites = ["https://www.jython.org", "http://olympus.realpython.org/dice"] * 80
    start_time = time.perf_counter()
    download_all_sites(sites)
    duration = time.perf_counter() - start_time
    print(f"Downloaded {len(sites)} sites in {duration} seconds")


if __name__ == "__main__":
    main()
```

高亮部分标识出了将同步版本的程序修改为线程版本时做出的修改。在第 $20$ 行，你创建了一个 `ThreadPoolExecutor` 实例来为你管理线程。在这种情况下，你明确请求了 $5$ 个工作线程

执行上述代码，此时输出的结果不在有顺序，因为线程合适被调用时不可控的

```
Read 10966 bytes from https://www.jython.org
Read 274 bytes from http://olympus.realpython.org/dice
Read 10966 bytes from https://www.jython.org
Read 274 bytes from http://olympus.realpython.org/dice
Read 274 bytes from http://olympus.realpython.org/dice
Read 274 bytes from http://olympus.realpython.org/dice
Downloaded 160 sites in 4.700141600013012 seconds
```

> [!attention] 
> 
> 对不同步版本的程序，使用线程并发的将程序的执行时间降低了很多
> 
> 关于线程的其他知识，参考 [[Python：线程]]

这里是其执行时序图如下：线程发起请求后被阻塞，就会调度另一个线程执行，从而让所有线程稳步推进

![[Pasted image 20250423202814.png]]

请注意：线程之间的切换也是有性能消耗的。当一个进程由许多线程时，线程的切换带来的性能消耗可能导致程序性能会更差

### 使用异步IO

并发运行线程可以使您将原始同步代码的总执行时间减少一个数量级。这已经相当了不起，但通过利用 Python 的 `asyncio` 模块，您还可以做得更好，该模块能够实现 **异步I/O**

异步处理是一种非常适合 IO密集型任务的并发模型——因此得名 `asyncio`。它通过使用 **事件循环**、**非阻塞操作** 和 **协程** 等方法，_避免了线程间上下文切换的开销_。也许有些令人惊讶的是，**异步代码只需要一个执行线程即可并发运行**。

> [!seealso] 
> 
> 关于 Python 中的异步IO，请参阅 [[Python：asyncio]]
> 

总结来说，**事件循环控制每个异步任务何时以及如何执行**。正如其名所示，它持续地遍历你的任务，同时监控它们的状态。一旦当前任务开始等待IO 操作完成，循环会挂起它并立即切换到另一个任务。相反，一旦发生预期事件，循环将在下一次迭代中最终恢复挂起的任务。

协程类似于线程但更轻量级且更便宜地挂起或恢复。这就是为什么可以在不产生显著内存或性能开销的情况下创建比线程更多的协程。这种能力有助于解决 [C10k问题](https://en.wikipedia.org/wiki/C10k_problem)，该问题涉及如何高效处理一万个并发连接。

> [!attention] 
> 
> 请注意：协程虽然降低了任务切换的消耗，但是也带来了问题：**您不能在协程中使用阻塞函数调用**，如果您想充分利用异步编程的优势。
> 
> 阻塞调用是同步的，这意味着它在等待数据到达时阻止其他代码运行。相比之下，非阻塞调用可以自愿放弃控制权，等待被通知数据已准备好
> 

在 Python 中，**通过调用异步函数来创建协程对象**，也称为协程函数。这些函数使用 `async def` 语句定义，而不是常规的 `def`。只有在异步函数的体内，您才被允许使用 `await` 关键字，它将暂停协程的执行，直到等待的任务完成

```python
import asyncio

async def main():
    await asyncio.sleep(3.5)  # 让出控制权，让其他任务有机会执行
```

在这个情况下，你将 `main()` 定义为一个异步函数，当调用时隐式地返回一个协程对象。多亏了 `await` 关键字，你的协程使用非阻塞方式调用 `asyncio.sleep()`，模拟了 `3.5` 秒的延迟。当 `main()` 函数等待唤醒事件时，其他任务可以潜在地并发运行。

> [!attention] 
> 
> 注意：要运行上述示例代码，您需要将 `main()` 的调用包裹在 `asyncio.run()` 中或在其 Python 的 `asyncio REPL` 中 `await main()`。
> 

先前我们使用的 `requests` 库是同步的，你必须现在切换到一个非阻塞的替代品，例如为Python 的 `asyncio` 设计的 [`aiohttp`](https://docs.aiohttp.org/en/stable/)

```shell
pip install aihttp
```

现在，我们安装好了 `aiohttp` 这个第三方库。现在，我们将之前的实例代码修改为异步版本的

```python title:io_async.py
import asyncio
import time

import aiohttp


async def download_site(url, session):
    async with session.get(url) as response:
        print(f"Read {len(await response.read())} bytes from {url}")

async def download_all_sites(sites):
    async with aiohttp.ClientSession() as session:
        tasks = [download_site(url, session) for url in sites]
        await asyncio.gather(*tasks, return_exceptions=True)

async def main():
    sites = ["https://www.jython.org", "http://olympus.realpython.org/dice"] * 80
    start_time = time.perf_counter()
    await download_all_sites(sites)
    duration = time.perf_counter() - start_time
    print(f"Downloaded {len(sites)} sites in {duration} seconds")


if __name__ == "__main__":
    asyncio.run(main())
```

使用 `asyncio` 的版本与同步版本非常相识。这是 Python 中的 `asyncio` 的一大优点。然而，它也是一把双刃剑。虽然从理论上讲，它使得并发代码比多线程版本更容易理解，但当涉及到更复杂的情况时，`asyncio` 却并不是容易使用的。

> [!tip] 
> 
> 与同步版本相比，异步版本中的函数使用 `async def` 关键字定义，它将您的常规函数重新定义为**异步函数**。
> 
> 当遇见要阻塞的操作时，使用关键字 `await` 等待其后面表达式的执行，并让出控制权
> 

您可以在所有任务之间共享 `session` ，因此会话在此作为上下文管理器创建。任务可以共享 `session`，因为它们都在同一线程上运行。在 `session` 处于不良状态时，一个任务无法中断另一个任务。

> [!attention] 
> 
> 这里隐藏了一个小但重要的变化。还记得提到的创建线程的最佳数量吗？在多线程示例中，最佳线程数量并不明显。
> 
> 异步IO 的一个酷优点是它的扩展性远优于线程或 `concurrent.futures`。每个任务比线程创建所需资源和时间都要少得多，因此创建和运行更多任务效果很好。这个例子只为每个要下载的网站创建一个单独的任务，效果相当不错。
> 

并且，它真的很快。异步版本是其中最快的，领先优势明显：

```shell
Read 274 bytes from http://olympus.realpython.org/dice
Read 10966 bytes from https://www.jython.org
...
Read 10966 bytes from https://www.jython.org
Read 10966 bytes from https://www.jython.org
Downloaded 160 sites in 2.6161483000032604 seconds
```

相比于多线程版本，异步 IO 的耗时更短。执行时序图看起来和多线程示例中发生的情况非常相似。只是 I/O 请求都是由同一个线程完成的：

![[Pasted image 20250423214745.png]]

> [!summary] 
> 
> 同步版本中，您遍历一个站点列表，并按照确定性顺序下载其内容。
> 
> 在多线程版本中，您将任务调度控制权交给了操作系统，因此最终的顺序看起来是随机的。
> 
> 虽然异步版本可能会显示一些完成情况的聚类，但由于网络条件的变化，它通常是非确定性的。
> 

这是一个常见的论点，认为必须正确地添加 `async` 和 `await` 让异步编程变得比较麻烦。在某种程度上，这是真的。这个论点的另一面是，**它迫使你思考一个给定任务何时会被切换**，这可以帮助你创建更好的设计。

存在一些 `asyncio` 的问题。您需要使用特殊的异步版本库来充分发挥 `asyncio` 的优势。如果您只是使用 `requests` 下载网站，那么将会慢得多，因为 `requests` 并未设计为通知事件循环它已被阻塞。随着时间的推移，这个问题变得越来越不显著，因为越来越多的库开始采用 `asyncio`。

协程的所有优势都会在一项任务不让出控制的情况下被浪费掉。代码中的一点点小错误就可能导致任务运行失控并长时间占用处理器，从而饿死其他需要运行的任务。如果任务没有将控制权交还给事件循环，事件循环就无法打断

> [!tip] 
> 
> 协程不会被操作系统知晓，协程的切换完全有应用程序控制。一旦有一个协程阻塞，那么整个进程都会被阻塞
> 

### 使用进程

由于 Python 存在 GIL(全局解释器锁)和当前 CPython 的设计，**线程和异步IO实现的并发都只能利用 CPU 的一个核心**

> [!tip] 
> 
> Go 语言是一种高度协程化的语言，它可以让协程在不同的线程上执行。这是与 Python 中协程的一大差异
> 

在 Python 中，想要利用 CPU 的多核优势唯一的方法就是使用 **进程**。在 Python 想要创建进程需要使用标准库 `multiprocessing` 和标准库 `concurrent.futures` 中的相应包装器。它们能够使您的代码能在多个 CPU 上运行。从高层次来看，它是通过在每个 CPU 上创建一个新的 Python 解释器实例来实现的，然后将您的程序的一部分分配给它运行

启动独立的 Python 解释器并不像在当前 Python 解释器中启动新线程那样快。这是一个性能消耗较大操作，并伴随着一些限制和困难，但对于正确的问题，它可能带来巨大的差异

不同于此前的方案，使用多进程可以让您充分利用您那酷炫、新电脑的所有CPU。以下是示例代码

```python
import atexit
import multiprocessing
import time
from concurrent.futures import ProcessPoolExecutor

import requests

session: requests.Session

def download_site(url):
    with session.get(url) as response:
        name = multiprocessing.current_process().name
        print(f"{name}:Read {len(response.content)} bytes from {url}")


def download_all_sites(sites):
    with ProcessPoolExecutor(initializer=init_process) as executor:
	     # 让进程在 sites 中的每个项上执行 download_site 函数
        executor.map(download_site, sites) 

def main():
    sites = [
        "https://www.jython.org",
        "http://olympus.realpython.org/dice",
    ] * 80
    start_time = time.perf_counter()
    download_all_sites(sites)
    duration = time.perf_counter() - start_time
    print(f"Downloaded {len(sites)} sites in {duration} seconds")


# init_process 用于初始化进程
def init_process():
    global session
    session = requests.Session()
    atexit.register(session.close)  # 注册进程退出时的操作


if __name__ == "__main__":
    main()
```

这里发生的情况是，`ProcessPoolExecutor` 会创建了一系列独立的 Python 解释器进程，并让每个进程在可迭代对象中的某些项目上运行指定的函数，在你的情况下是网站列表。主进程与其他进程之间的通信由我们自行处理。

> [!attention] 
> 
> 创建池实例的行值得你注意。首先，它没有指定在池中创建多少个进程，尽管这是一个可选参数。
> 
> 默认情况下，它将确定你的计算机中的 CPU 数量，并与之匹配。这通常是最佳答案，也适用于你的情况。
> 

I/O密集型问题中，增加进程数量不会使事情更快。实际上，这会减慢速度，因为创建和回收所有这些进程的成本大于并行执行 I/O 请求的好处

执行上述代码的输出结果为

```
Read 274 bytes from http://olympus.realpython.org/dice
Read 274 bytes from http://olympus.realpython.org/dice
...
Read 10966 bytes from https://www.jython.org
Read 10966 bytes from https://www.jython.org
Downloaded 160 sites in 3.0692823999852408 seconds
```

在配备四个CPU核心的计算机上，它的运行速度大约是同步版本的 $4$ 倍。然而，它比多线程版本略慢，比异步版本慢得多。这段代码的执行时序图看起来是这样的

![[Pasted image 20250424002657.png]]

## 加速 CPU 密集型任务

现在，我们将研究一个 CPU 密集型问题。正如你之前所学的，IO 密集型问题大部分时间都在等待外部操作完成，例如网络调用。相比之下，CPU 密集型问题执行较少的 IO 操作，其总执行时间取决于它处理所需数据有多快

为了这个示例，你将使用一个有点愚蠢的函数来创建一个运行在CPU上需要很长时间的代码片段。此函数使用递归方法计算第 $n$ 个斐波那契数

```python
>>> def fib(n):
...     return n if n < 2 else fib(n - 2) + fib(n - 1)
...
>>> for n in range(1, 11):
...     print(f"fib({n:>2}) = {fib(n):,}")
...
fib( 1) = 1
fib( 2) = 1
fib( 3) = 2
fib( 4) = 3
fib( 5) = 5
fib( 6) = 8
fib( 7) = 13
fib( 8) = 21
fib( 9) = 34
fib(10) = 55
```

注意，随着函数计算更高的斐波那契数，结果值增长得多快。这种实现的递归性质导致对相同数字的多次重复计算，这需要大量的处理时间。这就是为什么这是一个如此方便的CPU密集型任务示例。

首先，我们实现一个非并发的版本

```python
import time

def main():
    start_time = time.perf_counter()
    for _ in range(20):  # 计算 20 次 fib(35)
        fib(35)
    duration = time.perf_counter() - start_time
    print(f"Computed in {duration} seconds")

def fib(n):
    return n if n < 2 else fib(n - 2) + fib(n - 1)

if __name__ == "__main__":
    main()
```

这段代码在循环中调用 `fib(35)` 二十次。由于其实现具有递归性质，函数自身调用了数亿次！它全部在一个单线程、一个进程、一个 CPU 上完成。

与 IO 密集型示例不同，CPU 密集型示例通常在运行时间上比较一致。这个在之前相同的机器上大约需要 $15$ 秒：

```
Computed in 15.171952964000013 seconds
```

显然，我们能做得比这更好。毕竟，所有的东西都在单个 CPU 上运行，没有任何并发。接下来，你将看到你可以如何改进它

### 使用线程

CPU密集型问题，无需等待。CPU正在尽可能快地运转以完成问题。在Python中，线程和异步任务都在同一进程中运行在同一个CPU上。这意味着一个CPU要完成非并发代码的所有工作，还要额外设置线程或任务的工作

> [!attention] 
> 
> 这意味着，在 CPU 密集型任务中使用线程并不会提高程序性能
> 

```python
import time
from concurrent.futures import ThreadPoolExecutor

def main():
    start_time = time.perf_counter()
    with ThreadPoolExecutor(max_workers=5) as executor:
        executor.map(fib, [35] * 20)
    duration = time.perf_counter() - start_time
    print(f"Computed in {duration} seconds")

def fib(n):
    return n if n < 2 else fib(n - 2) + fib(n - 1)

if __name__ == "__main__":
    main()
```

少量代码需要从非并发版本更改。导入 `concurrent.futures` 后，只需将循环遍历数字更改为创建线程池并使用其 `.map()` 方法将单个数字发送到空闲的工作线程。

执行上述代码的输出结果为

```
Computed in 16.114787849000038 seconds
```

注意到：因为 Python 存在 GIL，一个 Python 进程中有且只有一个线程执行。因为线程切换带来额外的性能消耗，从而导致程序性能降低

### 使用异步

实现这个 CPU 密集型问题的异步版本涉及将你的函数重写为协程函数，使用`async def`并等待它们的返回值：

```python
import asyncio
import time

async def main():
    start_time = time.perf_counter()
    tasks = [fib(35) for _ in range(20)]
    await asyncio.gather(*tasks, return_exceptions=True)
    duration = time.perf_counter() - start_time
    print(f"Computed in {duration} seconds")

async def fib(n):
    return n if n < 2 else await fib(n - 2) + await fib(n - 1)

if __name__ == "__main__":
    asyncio.run(main())
```

这里我们创建了二十个任务并将它们传递给 `asyncio.gather()` 以使相应的协程并发运行。然而，实际上它们是按顺序运行的，因为每个任务都会阻塞执行，直到前一个任务完成

  运行时，此代码执行时间比您的原始同步版本多一倍，并且比多线程版本也要长：

```
Computed in 37.484856695000076 seconds
```

> [!important] 
> 
> 异步方法对于CPU密集型问题来说是最慢的，然而对于I/O密集型问题来说却是最快的。由于这里没有涉及I/O操作，因此没有什么需要等待的。事件循环以及每个 `await` 语句处的上下文切换开销显著减慢了总体的执行速度。
> 

### 使用进程

与其他并发模型不同，基于进程的并行计算是专门设计来在多个 CPU 之间共享繁重的 CPU 工作负载的。

```python
import time
from concurrent.futures import ProcessPoolExecutor

def main():
    start_time = time.perf_counter()
    with ProcessPoolExecutor() as executor:
        executor.map(fib, [35] * 20)
    duration = time.perf_counter() - start_time
    print(f"Computed in {duration} seconds")

def fib(n):
    return n if n < 2 else fib(n - 2) + fib(n - 1)

if __name__ == "__main__":
    main()
```

几乎与斐波那契问题的多线程版本完全相同。你实际上只修改了两行代码！你用 `ProcessPoolExecutor` 替换了 `ThreadPoolExecutor`

之前提到，到 `ProcessPoolExecutor` 构造函数的 `max_workers` 可选参数值得注意。您可以使用它来指定您想创建和管理多少个进程。默认情况下，它将确定您的机器中有多少个 CPU，并为每个 CPU 创建一个进程。虽然这对您的简单示例来说效果很好，但在生产环境中，您可能想要更多一点的控制。

这个版本大约需要 $3$ 秒，只使用了最初开始的非并发实现五分之一的时间：

```
Computed in 2.657741920000035 seconds
```

这是比您看到的其他选项要好得多，因此无疑是此类任务的最佳选择。这里显示的是执行时序图：

![[Pasted image 20250424005333.png]]

多进程使用中存在一些缺点，这些缺点在像这样的简单示例中并不明显。例如，**将问题划分为若干部分以便每个处理器可以独立操作有时可能很困难**。

也许多种解决方案需要进程之间进行更多的通信。这可能会给您的解决方案增加一些复杂性，这对于一个非并发程序来说根本不需要处理

## 高并发的处理

考虑一个学校的食堂。到了中午放学时间，学生们都涌向食堂，食堂需要提供为学生们提供午饭服务。一个食堂能够同时服务许多学生，这就是并发。如果学生非常多，那么就高并发

如何才能让食堂更快的服务这些学生呢？下面我们探索几个高并发的处理方案

### 缓冲(队列)

假设食堂只有一个窗口，那么最天然的方式就是 **排队**。让学生在窗口前排成一字形的 **队列**，食堂一个一个的为学生提供服务

> [!tip] 
> 
> 使用 **队列** 将带处理的任务缓冲下来，让程序一个一个的从队列中获取任务并处理任务。
> 
> 这种方式是典型的 **生产者-消费者模型**。
> 

在 Python 中，提供了 $3$ 种队列，它们非常相似只是用于不同的地方

| 队列                         | 描述               |
| :------------------------- | :--------------- |
| `queue.Queue()`            | 用于线程间数据交互        |
| `mutilprocessing.Queue()`  | 用于进程间数据交换        |
| `asyncio.Queue()`          | 用于 `Task` 间的数据交换 |

此外，还有 `LifoQueue()`(后进先出队列) `PriorityQueue()`(优先队列)

### 争抢

假设食堂只有一个窗口，让学生们 **争抢** 这个窗口也不失为一个有效的方案。然而，这种方案存在一些弊端。学生们在不断争抢这一个窗口，有些学生可能争抢不过其他学生，有些学生可能会被其他学生从窗口推开。

> [!tip] 
> 
> 这个过程就像，两个线程同时访问共享资源，从而出现的 **竞争条件**
> 

为了解决这个问题，我们给窗口装上门，并加上一把锁。让学生抢夺这个锁，只有抢夺到锁的学生从而内部将门锁上。当学生离开窗口时，就会释放锁，让其他学生争抢

> [!tip] 
> 
> 上述过程就是 **互斥锁** 的工作原理。只要获得了锁，其他线程就无法进入这个区域执行
> 

### 预处理(缓存)

只要处理的快，排队和争抢并不是问题。就算有 $2$ 万人一起排队，只要能够在短时间内完成也是没有任何问题的。

然而，有时候任务处理的会非常慢。例如，在食堂打饭时，等待时间是比较长的。因为，不同的学生可能会点不同的菜品，并且要现做。现做就是导致食堂打饭等待时间长的主要原因

我们可以 **分析并统计出大多数学生最喜爱的菜品**，将这些菜品 **提前做好**。当学生到食堂时，大多数学生爱吃的菜品是已经准备好的，他们可以直接拿走这些菜品。只有少部分学生会等待

### 并行

当学生太多时，只有一个窗口无法满足所有学生。最简单的版本就是 **多开一些窗口**。这样每时每刻都可以同时服务 $n$ 个学生($n$ 是窗口个数)

> [!tip] 
> 
> 类似于 **多进程** 和 **多线程**。这种多开一个执行流的方式会 **消耗更多的性能**；例如，进程和线程的调度；创建进程或线程；回收进程或线程。这些操作都会浪费性能
> 
> 这种使用多进程和多线程，或者构造更多的服务器解决并发问题的思想是 **水平扩展**
> 

### 提速

通过培训窗口服务员，提高它们的打饭速度；或者在一个窗口安排多个服务员也是一种有效解决方案。

> [!tip] 
> 
> 提高单颗 CPU 性能；或者安装更多的 CPU，从而提高程序性能。这是一种 **垂直扩展** 的思想
> 
> 

## queue 模块

`queue` 模块实现了 **多生产者-多消费者队列**。这特别 **适用于消息必须安全地在多线程间交换** 的线程编程。模块中的 `Queue` 类实现了所有所需的锁定语义。

本模块实现了 **三种类型的队列**，它们的 **区别仅仅是条目的提取顺序**。 在 FIFO 队列中，先添加的任务会先被提取。 在 LIFO 队列中，最近添加的条目会先被提取 (类似于一个栈)。 在优先级队列中，条目将保持已排序状态 (使用 [`heapq`](https://docs.python.org/zh-cn/3.13/library/heapq.html#module-heapq "heapq: Heap queue algorithm (a.k.a. priority queue).") 模块) 并且值最小的条目会先被提取。

下表总结了 `queue` 模块中的 $3$ 种队列

| 队列                      | 描述      |
| :---------------------- | :------ |
| `queue.Queue()`         | FIFO 队列 |
| `queue.LifoQueue()`     | LIFO 队列 |
| `queue.PriorityQueue()` | 优先队列    |

由于这 $3$ 中类型的队列只有获取条目顺序的差异，下面我们介绍 `queue.Queue()` 类的使用即可

### queue.Queue(maxsize=0)

`queue.Queue(maxsize=0)` 构造一个FIFO队列。`maxsize` 是一个整数，用于设置队列中可以放置的项目数量的上限。一旦达到这个大小，插入将阻塞，直到队列项目被消耗

> [!important] 
> 
> 如果 `maxsize` 小于或等于零，则队列大小为无限
> 

此外，`Queue` 对象支持下表列出的方法

| 方法                                          | 描述                           |
| :------------------------------------------ | :--------------------------- |
| `Queue.qsize()`                             | 返回队列的估计大小                    |
| `Queue.empty()`                             | 检查队列是否为空                     |
| `Queue.full()`                              | 检查队列是否满队                     |
|                                             |                              |
| `Queue.put(item, block=True, timeout=None)` | 将 `item` 放入队列                |
| `Queue.put_nowait(itme)`                    | 相当于 `put(item, block=False)` |
| `Queue.get(block=True, timeout=None)`       | 从队列中获取元素                     |
| `Queue.get_nowait()`                        | 相当于 `get(False)`             |
|                                             |                              |
| `Queue.task_done()`                         | 表示前面排队的任务已处理完成               |
| `Queue.join()`                              | 阻塞至队列中所有的元素都被处理              |
|                                             |                              |
| `Queue.shutdown(immediate=False)`           | 关闭队列                         |

> [!tip] 
> 
> `Queue.put(item, block=True, timeout=None)` 会将 `item` 放入队列中。如果队列为满队，则阻塞 `timeout` 秒。如果 `timeout` 秒之后没有插入成功，则抛出 `queue.Full` 异常
> 
> `Queue.get(block=True, timeout=None)` 从队列中获取数据。如果队列为空，则会阻塞 `timeout` 描述。如果 `timeout` 秒之后没有获取成功，则抛出 `queue.Empty` 异常
> 
> `Queue.shutdown(immediate=False)` 会关闭队列。对于关闭后的队列，在调用 `Queue.get()` 和 `Queue.put()` 方法时就会引发 `queue.ShutDown` 异常
> + 默认情况下，`Queue.get()` 只会空队列上引发异常
> + 如果 `immediate=True`，则会将队列中的所有元素标记为已完成。此时，调用 `Queue.get()` 会立即抛出异常。并且，所有 `put()` 和 `get()` 被阻塞的调用方将被撤销阻塞
> 

