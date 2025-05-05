# asyncio

**异步IO** 是一种在 Python 中受到专门支持的并发编程设计，从 Python 3.4 到 3.7 快速发展，可能还超出了这个范围。**协程**（特殊生成器函数）是 Python 中异步 IO 的核心。我们稍好介绍

**并发** 与 **并行** 是多任务编程中研究最为广泛的课题。我们在引入异步 IO之前回顾一下这两个概念。

**并行** 是指 **同时执行多个操作**。**多进程是实现并行的手段**，它涉及到将任务分散到计算机的CPU 核心。

> [!tip] 
> 
> 多进程非常适合于 CPU 密集型任务：紧密绑定的循环和数学计算通常属于这一类
> 

**并发** 是一个比并行更广泛的概念。它意味着 **多个任务能够以重叠的方式运行**。（有句话说并发不意味着并行。）

> [!tip] 
> 
> 线程是一种并发执行模型，其中多个线程轮流执行任务。一个进程可以包含多个线程
> 

关于线程，重要的是要知道它更适合 IO 密集型的任务。虽然 CPU 密集型的任务的特点是计算机的内核从头到尾不断努力工作，但 **IO 密集型的任务主要是等待输入/输出完成**。

> [!summary] 并发包括多进程和多线程
> 
> 回顾上述内容，并发包括多进程（适用于 CPU 密集型任务）和多线程（适用于 IO 密集型任务）
> 
> 多进程是并行的一种形式，而并行是并发的一个特定类型（子集）。Python标准库通过其`multiprocessing`、`threading` 和 `concurrent.futures` 等标准库长期以来为这两者提供了支持
> 

## 异步 IO 的引入

目前，一个独立设计的已经被更全面地集成到 CPython 中：**异步 IO**，通过标准库 `asyncio` 和新的 `async` 和 `await` 关键字来实现。

> [!attention] 
> 
> 为了清楚起见，异步 IO 不是一个新发明的概念，它已经存在于其他语言和运行时环境中。其中最有代表的语言就是 Golang
> 

在 Python 官方文档中，标准库 `asyncio` 被分在并发代码的库。然而，异步 IO 既不是多线程，也不是多进程。它不是建立在任何一个之上的。

实际上，**异步I/O是一种单线程、单进程的设计**：它使用 **协作多任务**。换句话说，异步I/O 即使在单个进程中使用单个线程也能给人一种并发的感觉。**协程** 是异步I/O的核心特性，**可以并发调度**，但它们本身并不是并发的

下面我们解释 **异步** 这一概念。这不是一个严谨的定义，但在这里，我可以想到两个特性：

+ 异步例程能够在等待最终结果时“暂停”，同时允许其他例程在此期间运行
+ 异步代码，通过上述机制，促进并发执行。换句话说，异步代码提供了并发的外观和感觉。

### 异步 IO 例子

异步I/O 一开始可能看起来有些反直觉且矛盾。一种促进并发代码使用的东西如何只使用单线程和单核心 CPU 呢？我们以 Miguel Grinberg 在 2017 年 PyCon 演讲中的一个例子，他解释得非常漂亮

> [!quote] 
> 
> 国际象棋大师 _尤迪特·波尔加_ 举办了一场国际象棋展览，在展览中她与多位业余选手对弈。她有两种进行展览的方式：**同步** 和 **异步**
> 
> 我们做如下假设
> + 尤迪特·波尔加有 $24$ 个对手
> + 尤迪特·波尔加 每走一步棋耗费 $5$ 秒
> + 尤迪特·波尔加的对手 每周一步棋耗费 $55$ 秒
> + 游戏平均 $30$ 个回合（总共 $60$ 步）
> 
> **同步版本**：尤迪特·波尔加一次玩一局游戏，永远不会同时玩两局，直到游戏完成。每局游戏需要 $(55 + 5) \times 30 = 1800$ 秒，即 $30$ 分钟。整个展览需要 $24 \times 30 = 720$ 分钟，即 $12$ 小时
> 
> **异步版本**：尤迪特·波尔加从一张桌子走到另一张桌子，每张桌子移动一次。她离开桌子，在等待时间内让对方走下一步。在所有 $24$ 场比赛中，尤迪特·波尔每场需要 $24 \times 5 = 120$ 秒，即 $2$ 分钟。整个展览现在缩短到 $120 * 30 = 3600$ 秒，即刚好 $1$ 小时
> 

只有一个 尤迪特·波尔加，她只有两只手，每次只能自己移动一步。但异步比赛将展览时间从 $12$ 小时缩短到 $1$ 小时。因此，合作多任务处理只是说程序的事件循环与多个任务通信，让每个任务在最佳时间轮流运行。

异步IO 在等待期间占用较长时间，这些期间函数本应阻塞，并允许其他函数在等待期间运行。（一个阻塞的函数实际上禁止其他函数从它开始运行到它返回的时间运行。)

### 异步 IO 并不简单

我听人说过，“能使用异步IO就使用异步IO；必要时使用线程。” 事实是，构建持久的多线程代码可能很难，而且容易出错。异步IO 避免了线程设计中可能遇到的一些潜在的速度障碍

但这并不是说 Python 中的异步IO 很容易。请注意：当深入底层编程时，异步编程也可能很困难！ Python 的异步模型围绕着诸如 **callbacks**，**events**，**transports**，**protocols** 和 **futures** 等概念建立，而术语可能令人生畏。它的 API 一直在不断变化的事实使其并不容易。

幸运的是，`asyncio` 已经成熟到其大多数功能不再是试验性的，而其文档已经进行了巨大的大修，并且有关该主题的一些优质资源也开始出现。

## 协程

异步IO 的核心是协程。**协程是 Python 特殊生成器函数**。让我们从一个基本定义开始，然后在此基础上进行构建：**协程是一个可以在返回之前暂停执行的函数**，它可以在一段时间内间接地 **将控制传递给另一个协程**。

> [!attention] 
> 
> 请注意：实现协程的关键就是 **允许函数在执行过程中暂停**，并且 **将控制传递给另一个函数**
> 

> [!NOTE] 维基百科
> 协程（英语：coroutine）是计算机程序的一类组件，推广了 **协作式多任务** 的 **子例程(函数)**，**允许执行被挂起与被恢复**
> 
> 相对函数而言，协程更为一般和灵活，但在实践中使用没有函数那样广泛。协程更适合于用来实现彼此熟悉的程序组件，如 **协作式多任务**、**异常处理**、**事件循环**、**迭代器**、**无限列表** 和 **管道**。

### 协程的本质

回忆一下，在 [[Python：函数高级#生成器函数]] 中，我们介绍了关键字 `yield`。这个关键字能够暂停函数的执行并让出控制流。

**暂停** 函数的执行是一种非常重要的能力。通常情况下，函数正常要执行到 `return` 后才会交出控制，但是，使用 `yield` 关键字就可以 **由开发者决定函数何时将控制交出**

下面的代码展示了使用在 Python 中使用生成器推进两个函数执行的的过程

```python
import string


def count():
    c = 1
    for i in range(5):
        print("count:", c)
        yield c
        c += 1


def char():
    s = string.ascii_letters
    for c in s:
        print("char:", c)
        yield c


t1 = count()
t2 = char()

next(t1)
next(t2)
next(t1)
next(t2)
print("----------------")

# Output
# count: 1
# char: a
# count: 2
# char: b
# ----------------
```

可以看出代码在 `yield` 处暂停并将控制交换，然后 `next()` 可以驱动另一个函数执行。这样，可以由程序员在合适的地方通过 `yield` 来暂停一个函数执行，并驱动另外一个函数执行

> [!tip] 
> 
> 这里我们在 **单线程** 模式下实现了 **多个任务之间的并发执行**
> 
> 这种 **相互协作** 的让程序中的 **多个任务** 并发执行被称为 **协作式多任务程序**，通常简称为 **协程**
> 

> [!tip] 
> 
> 本质上讲，**协程就是让正在执行的函数让出控制并执行另一个函数**。通常情况下，函数交出控制权的位置都是在发生 IO 操作时
> 

线程时间片耗尽后导致的函数切换对开发人员来说是不可控的，而且 **线程控制能力是内核的功能**，是在内核态完成的，而 **协程的控制是在用户态完成的**

### 事件循环

如何才能让 **多个任务反复交替执行** 呢？我需要构建一个 **循环**，称为 **_事件循环_**，让多个任务在事件循环中交替执行；因此，我们还需要一个列表用于保存正在执行的任务，这个列表称为 **任务列表**

```python
import string
import time


def count():
    c = 1
    for i in range(5):
        print("count:", c)
        yield c
        time.sleep(0.5)
        c += 1


def char():
    s = string.ascii_letters
    for c in s:
        print("char:", c)
        yield c
        time.sleep(0.5)


t1 = count()
t2 = char()

tasks = [t1, t2]

# 开启事件循环
while True:
    pops = []
    # 在时间循环中执行任务
    for i, task in enumerate(tasks):
        # 给定一个默认值，这个默认值是任务永远不会返回的
        if next(task, None) is None:
            print(f"task {task} finish")
            # tasks.remove(task)  # 移出任务  bug：一个遍历的在列表不能随意删除其中的元素
            # 我们需要一个额外的列表存储已经被删除的任务的索引
            pops.append(i)
    # 遍历待删除的任务索引，然后删除。 bug: 如果删除该任务后，一个索引比该任务大的索引将发生变化。因此，需要反序删除
    # for i in pops:
    #     tasks.pop(i)
    for i in sorted(pops, reverse=True):
        tasks.pop(i)

    # 所有任务都以执行完毕
    if len(tasks) == 0:
        break
```

> [!tip]
> 可以通过上面的代码看到 $2$ 个任务交替进行，而这个函数的交替，完全是靠程序员的代码实现的，而不是靠多线程的时间片用完操作系统强行切换，而且这种切换是在同一个线程中完成的
> 
> 最重要的是，**协程的切换是在用户态完成**，而不是像线程那样在内核态完成。所以，Coroutine 是可以在用户态通过控制在适当的时机 **让出控制** 的多任务切换技术

> [!attention] 协程的核心：同一个线程中，在适当的时候 **将 _控制_ 从当前任务转移到其他任务**
> 
> 只要是代码就要在线程中执行，协程也不例外。即使使用了协程，依旧会出现线程的切换
> 

下面的示例展示了基于协程实现的并发服务器

```python
import socket

server = socket.socket(socket.AF_INET, type=socket.SOCK_STREAM, proto=0)
server.setblocking(False)  # 非常关键，不然整个线程就会被阻塞
server.bind(("127.0.0.1", 8080))
server.listen(1024)


def accept(server: socket.socket):
    while True:
        try:
            conn, address = server.accept()
        except BlockingIOError as e:
            yield -1  # 没有客户端请求连接。让出控制权，去检查其他的连接
        else:  # 与客户端建立了连接就会走到这一步 
            print(f"客户端 {address} 建立连接")
            conn.setblocking(False)  # 将通信套接字也设置为非阻塞套接字
            yield conn  # 将通信套接字返回


def recv(conn: socket.socket):
    while True:
        try:
            data = conn.recv(1024)
        except (BlockingIOError, ConnectionResetError) as e:
            if isinstance(e, ConnectionResetError):  # 客户端强行结束连接
                conn.close()
                return  # 此时任务结束
            else:
                print(f"客户端 {conn.getpeername()} 尚未发来消息")
                yield -1  # 让出控制权，去检查其他的连接或监听套接字
        else:
            if not data:
                print(f"客户端 {conn.getpeername()} 断开连接")
                conn.close()
                return  # 此时任务结束
            print(f"客户端 {conn.getpeername()} 发来消息: {data.decode()}")
            conn.send(data.upper())


tasks = [accept(server)]  # 任务列表。最开始只有 accept 任务

while True:
    pops = []
    for i, task in enumerate(tasks.copy()):
        if (result := next(task, None)) is None:
            print(f"任务: {task} 完成")
            pops.append(i)
        elif isinstance(result, socket.socket):
            tasks.append(recv(result))

    for i in sorted(pops, reverse=True):
        tasks.pop(i)

    if len(tasks) == 0:
        break
```

> [!warning] 协程的弊端
> 
> **协程一旦阻塞，那么就阻塞当前所在线程**，该线程代码被阻塞不能向下继续执行了
> 
> **协程必须主动让出**，才能轮到该线程中另外一个协程运行

> [!tip] 
> 
> 关于 Python 中的 `socket` 编程，请参考 [[Python：网络编程]]
> 

## 原生协程

现在，我们已经知道协程这一概念了，下面我们来看从 Python3.5 开始引入两个关键字 `async` 和 `await` 两个关键字，并原生支持协程

让我们采用沉浸式方法编写一些异步IO 代码。这个简短程序是异步IO的 `Hello World`，但有助于阐明其核心功能

```python
import asyncio  
  
  
async def count():  
    print("One")  
    await asyncio.sleep(1)  
    print("Two")  
  
async def main():  
    await asyncio.gather(count(), count(), count())  
  
  
if __name__ == '__main__':  
    import time  
    s = time.perf_counter()  
    asyncio.run(main())  
    elapsed = time.perf_counter() - s  
    print(f"{__file__} executed in {elapsed:0.2f} seconds.")
```

执行上述程序控制台输出的结果为

```shell
$python3 coroutine.py 
One
One
One
Two
Two
Two
countasync.py executed in 1.02 seconds.
```

输出顺序是异步IO的核心。与每个 `count()` 调用进行交流的是一个 **单独的事件循环** 或协调器。当每个任务达到 `await asyncio.sleep(1)` 时，函数向上通知到事件循环，并交回控制权，说：“我将要睡眠 $1$ 秒。在此期间，请允许其他有意义的事情继续进行。”

> [!tip] 
> 
> `await` 用于向上通知事件循环，并且让出控制交换给事件循环，事件循环会选择另一个协程执行
> 

对比上述代码的同步版本

```python
import time

def count():
    print("One")
    time.sleep(1)
    print("Two")

def main():
    for _ in range(3):
        count()

if __name__ == "__main__":
    s = time.perf_counter()
    main()
    elapsed = time.perf_counter() - s
    print(f"{__file__} executed in {elapsed:0.2f} seconds.")
```

在执行上述代码时，输出结果的顺序和执行时间会发生轻微但关键的变化

```shell
$ python3 countsync.py
One
Two
One
Two
One
Two
countsync.py executed in 3.01 seconds.
```

### async/await 使用规则

至此，`async`、`await` 和它们创建的协程函数的正式定义已经就绪。这一节有点难懂，但是掌握 `async/await` 是很有用的，所以如果您需要，请回到这一节:

> [!tip] 
> 
> 关键字 `async` 引入了 **原生协程** 或 **异步生成器**。表达式 `async with` 和 `async for` 也是有效的，你将在后面的内容中看到它们。
> 

> [!tip] 
> 
> 关键字 `await` 将函数控制传递回事件循环。与此同时，它暂停周围协程的执行。
> 
> 如果 Python 在 `g()` 的范围内遇到 `await f()` 表达式，这就是 `await` 告诉事件循环的方式，“暂停 `g()` 的执行，直到我所等待的东西—— `f()` 的结果——被返回。与此同时，让其他事情去运行。”
> 

```python
async def g():
    r = await f()  # 在这里暂停，直到 f() 返回
    return r
```

关于何时以及如何使用 `async/await` 有一套严格的规则。无论你是还在学习语法还是已经接触过使用 `async/await`，这些都很方便：

> [!attention] 
> 
> 使用 `async def` 定义协程函数。协程函数内部可以包含 `await` `return` 和 `yield` 关键字，但是这些都是可选的。这意味着声明 `async def noop(): pass` 是有效的
>  
>  请注意：`yield from` 是不能出现在协程函数中。此外，`await` 也不能用在非 `async def` 的函数中 
>  

这里有一些简短的例子，旨在总结上述几条规则

```python
async def f(x):
    y = await z(x)  # OK - `await` and `return` 允许出现在协程函数中
    return y

async def g(x):
    yield x  # OK - this is an async generator

async def m(x):
    yield from gen(x)  # No - SyntaxError

def m(x):
    y = await z(x)  # Still no - SyntaxError (no `async def` here)
    return y
```

当我们调用 `async def` 定义的函数时，Python 会返回一个 `croutine` 对象，并且抛出 `RuntimeWarning` 警告 `RuntimeWarning: Enable tracemalloc to get the object allocation traceback`

```python
async def foo():
    pass


print(foo())  # <coroutine object foo at 0x7f3dfd385dd0>
# RuntimeWarning: coroutine 'foo' was never awaited
```

> [!warning] 
> 
> 警告，单纯的调用协程函数是不会让协程函数执行的，想要让协程对象执行就必须将协程对象放在一个 **事件循环** 或者 `await` 关键字之后
> 

请注意：当您使用 `await f()` 时，`f()` 必须是一个 **可等待的对象**。

> [!tip] 
> 
> 满足下列条件的对象就是可等待对象
> +另一个协程
> 定义了 `__await__()` 特殊方法的对象，该方法返回一个迭代器
> 
> 对于绝大多数的目的，我们只需要关注协程即可
> 

下面是一个异步IO减少等待时间的例子：给定一个生成随机整数的协程 `makerandom()`，它在 `[0, 10]` 范围内持续产生随机整数，直到其中一个超过阈值。你希望多个对这个协程的调用不需要依次等待完成

```python
import asyncio

import random

c = (
    "\033[0m",
    "\033[36m",
    "\033[91m",
    "\033[35m",
)

async def make_random(idx: int, threshold: int = 6) -> int:
    print(c[idx + 1] + f"Initiated make_random({idx}).")
    i = random.randint(0, 10)
    while i <= threshold:
        print(c[idx + 1] + f"make_random({idx}) == {i} too low; retrying.")
        await asyncio.sleep(1)
        i = random.randint(0, 10)
    print(c[idx + 1] + f"---> Finished: make_random({idx}) == {i}." + c[0])
    return i

async def main():
    res = await asyncio.gather(*(make_random(i, 10 - i -1) for i in range(3)))
    return res


if __name__ == "__main__":
    random.seed(444)
    r1, r2, r3 = asyncio.run(main())
    print()
    print(f"r1: {r1}, r2: {r2}, r3: {r3}")
```

上述代码执行的输出结果为

![[Pasted image 20250504145212.png]]

该示例程序使用了一个 `main` 协程和一个 `make_random` 协程；并在 $3$ 个不同的输入中并发运行 `make_random()`

> [!tip] 
> 
> 大多数程序都将包含小的、模块化的协程和一个包装函数，这个包装函数用于将每个较小的协程链接在一起。然后，`main()` 通过映射中央协程到某个 **可迭代对象** 或者 **池** 来 **收集** 任务 
>  

在这个小例子中，池是 `range(3)`。在后面给出的一个更完整的例子中，它是一组需要被同时请求、解析和处理的 URL，并且 `main()` 封装了每个 URL 的整个例程

> [!attention] 
> 
> 请注意，在异步编程中使用 CPU 密集型任务可能不是最合适。但是，我们在示例中使用了 `asyncio.sleep()` 模拟某些 IO 密集型任务
> 

### 协程链

协程的关键特性是它们可以串联起来。（记住，协程对象是可等待的，因此另一个协程可以等待它。）这允许你将程序分解为更小、更易于管理和可回收的协程

```python
#!/usr/bin/env python3
# chained.py

import asyncio
import random
import time


# 第一个协程
async def part1(n: int) -> str:
    i = random.randint(0, 10)
    print(f"part1({n}) sleeping for {i} seconds.")
    await asyncio.sleep(i)
    result = f"result{n}-1"
    print(f"Returning part1({n}) == {result}.")
    return result


# 第二个协程
async def part2(n: int, arg: str) -> str:
    i = random.randint(0, 10)
    print(f"part2{n, arg} sleeping for {i} seconds.")
    await asyncio.sleep(i)
    result = f"result{n}-2 derived from {arg}"
    print(f"Returning part2{n, arg} == {result}.")
    return result


# 协程链
async def chain(n: int) -> None:
    start = time.perf_counter()
    p1 = await part1(n)
    p2 = await part2(n, p1)
    end = time.perf_counter() - start
    print(f"-->Chained result{n} => {p2} (took {end:0.2f} seconds).")


# main 协程
async def main(*args):
    await asyncio.gather(*(chain(n) for n in args))


if __name__ == "__main__":
    import sys
    random.seed(444)
    args = [1, 2, 3] if len(sys.argv) == 1 else map(int, sys.argv[1:])
    start = time.perf_counter()
    asyncio.run(main(*args))
    end = time.perf_counter() - start
    print(f"Program finished in {end:0.2f} seconds.")
```

执行上述代码，请注意观察输出结果

![[chained.gif]]

从上述输出结果看，`part1()` 协程执行时会暂停随机时间后，`part2()` 协程才会开始处理结果。在这个设置中，**`main()` 的运行时将等于它收集和调度的任务的最大运行时间**，即 `part1()` 和 `part2()` 的最大执行时间之和

### asyncio.Queue

Python 在 `asyncio` 中提供了一个使用与协程间传递信息的队列 `asyncio.Queue()`。它与标准库 `queue.Queue()` 相似。

> [!attention] 
> 
> 请注意，`asyncio.Queue()` 不是线程安全，它是被设计专用于 `async/await` 代码。并且，`asyncio.Queue()` 没有 `timeout` 形参；请使用 [`asyncio.wait_for()`](https://docs.python.org/zh-cn/3.13/library/asyncio-task.html#asyncio.wait_for "asyncio.wait_for") 函数为队列添加超时操作。
> 

在我们之前的例子中，我们并没有真正需要队列结构。在 `chained.py` 中，每个任务(`future`)由一组协程组成，**这些协程明确地互相等待并通过单个输入链传递**。

还有一种替代结构也可以用于异步IO：许多 **相互不关联的生产者将项目添加到队列中**。每个生产者可以在交错的、随机的、未通知的时间将多个项目添加到队列中。一群 **消费者贪婪地从排队的队伍中拉出商品，而不等待任何其他信号**

> [!tip] 
> 
> 在这种设计中，没有任何单个消费者与生产者之间的链接。消费者事先不知道生产者的数量，甚至不知道将要添加到队列中的商品的累计数量。
> 

单个生产者需要耗费未知的时间才能向队列提交数据；而消费者每隔一定时间就会从队列中获取数据。这样，队列就作为了与生产者和消费者进行信息交换的通道，从而避免了生产者与消费者间直接进行信息交换。

使用异步IO 来完成这个任务的困难在于需要向消费者发送一个信号，表明生产已完成。否则，`await q.get()` 将会无限期地挂起，因为队列已经被完全处理，但消费者没有任何意识到生产已经完成。如下示例代码给出了 `asyncio.Queue()` 的使用

```python title:asyncq.py
import asyncio
import itertools
import os
import random
import time


# 辅助函数：创建一个数据
async def makeitem(size: int = 5) -> str:
    return os.urandom(size).hex()


# 辅助函数：随机睡眠 n 秒
async def randsleep(caller=None) -> None:

    i = random.randint(0, 10)
    if caller:
        print(f"{caller} sleeping for {i} seconds.")
    await asyncio.sleep(i)


# 生产者
async def produce(name: int, q: asyncio.Queue) -> None:
    n = random.randint(0, 10)
    for _ in itertools.repeat(None, n):  # 生产者随机生产 n 个数据
        await randsleep(caller=f"Producer {name}")  # 生产者随机等待 t 秒后开始生产数据
        i = await makeitem()  # 等待数据生成完成
        t = time.perf_counter() 
        await q.put((i, t))  # 将数据放入队列
        print(f"Producer {name} added <{i}> to queue.")


# 消费者
async def consume(name: int, q: asyncio.Queue) -> None:

    while True:
        await randsleep(caller=f"Consumer {name}") # 消费者随机等待 t 秒后开始读取数据
        i, t = await q.get()  # 等待获取队列中的数据
        now = time.perf_counter()
        print(f"Consumer {name} got element <{i}> in {now - t:0.5f} seconds. ")
        q.task_done()  # 任务处理完成，让队列计数器减少 1



async def main(nprod: int, ncon: int):
    q = asyncio.Queue()
    producers = [asyncio.create_task(produce(n, q)) for n in range(nprod)]  # nprod 个生产者
    consumers = [asyncio.create_task(consume(n, q)) for n in range(ncon)]   # ncon 个消费者

    await asyncio.gather(*producers)  # 等待生产者完成
    await q.join()  # 等待消费者消费完队列中的数据，隐含等待消费者

    for c in consumers:
        c.cancel()  # 取消协程


if __name__ == "__main__":
    import argparse
    random.seed(444)
    parser = argparse.ArgumentParser()
    parser.add_argument("-p", "--nprod", type=int, default=5)
    parser.add_argument("-c", "--ncon", type=int, default=10)

    ns = parser.parse_args()
    start = time.perf_counter()
    asyncio.run(main(**ns.__dict__))
    elapsed = time.perf_counter() - start
    print(f"Program completed inn {elapsed:0.5f} seconds.")
```

前两个协程函数 `makeitem()` 和 `randsleep()` 是辅助函数，分别用于生产数据项和暂停随机时间。

生产者协程函数 `produce()` 将 $1$ 到 $5$ 个数据项放入队列。每个项目是一个 `(i, t)` 元组，其中 `i` 是随机字符串，`t` 是生产者尝试将元组放入队列的时间

当消费者协程 `consume()` 取出一个数据项时，它只是简单地计算该商品在队列中等待的时间，使用的是该商品放入队列时的时间戳。

> [!attention] 
> 
> 请注意，我们使用 `asyncio.sleep()` 用于模拟一些其他更复杂的协程，如果它是一个常规的阻塞函数，则会消耗时间并阻塞所有其他执行
> 

我们使用 $2$ 个生产者和 $5$ 个消费者执行上述协程

```shell
➜ python asyncq.py -p2 -c5 
Producer 0 sleeping for 4 seconds.
Consumer 0 sleeping for 4 seconds.
Consumer 1 sleeping for 7 seconds.
Consumer 2 sleeping for 4 seconds.
Consumer 3 sleeping for 4 seconds.
Consumer 4 sleeping for 8 seconds.
Producer 0 added <da243873c2> to queue.
Producer 0 sleeping for 10 seconds.
Consumer 0 got element <da243873c2> in 0.00058 seconds. 
Consumer 0 sleeping for 7 seconds.
Producer 0 added <5f22d04eb8> to queue.
Producer 0 sleeping for 8 seconds.
Consumer 2 got element <5f22d04eb8> in 0.00031 seconds. 
Consumer 2 sleeping for 4 seconds.
Producer 0 added <1f977b64b7> to queue.
Producer 0 sleeping for 7 seconds.
Consumer 3 got element <1f977b64b7> in 0.00009 seconds. 
Consumer 3 sleeping for 1 seconds.
Producer 0 added <aa3e55c99c> to queue.
Consumer 1 got element <aa3e55c99c> in 0.00008 seconds. 
Consumer 1 sleeping for 6 seconds.
Program completed inn 29.12731 seconds.
```

从上述输出中，我们发现处理每个数据耗时非常少，然而最终程序却耗时严重

> [!tip] 
> 
> 这种延迟可能是由两个原因导致的
> 
> + 大部分不可避免的开销
> + 当数据出现在队列中时，所有消费者都在睡觉
> 
