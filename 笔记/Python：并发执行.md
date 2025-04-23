# 并发执行

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
> 关于 Python 中的异步IO，请参阅 [[Python：协程]]
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




