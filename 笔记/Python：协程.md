# 协程

**协程** 可以看作是一种 **特殊的函数**，它可以 **在任意时刻暂停或者恢复执行**，而不需要像线程那样频繁地进行上下文切换

> [!NOTE] 维基百科
> 
> 协程（英语：coroutine）是计算机程序的一类组件，推广了 **协作式多任务** 的 **子例程(函数)**，**允许执行被挂起与被恢复**
> 
> 相对函数而言，协程更为一般和灵活，但在实践中使用没有函数那样广泛。协程更适合于用来实现彼此熟悉的程序组件，如 **协作式多任务**、**异常处理**、**事件循环**、**迭代器**、**无限列表** 和 **管道**。

## 协程

我们已经知道 **线程是进程中的执行体**，拥有一个执行入口以及从进程虚拟地址空间中分配的栈(包括内核栈和用户栈)，操作系统会记录线程控制信息，当线程获得 CPU 时间片之后才可以执行

> [!seealso] 
> 
> 参考 [[操作系统：多线程概念]] 和 [[操作系统：进程调度]]
> 

当线程获得 CPU 时间片之后，CPU 中的栈指针寄存器(`%rsp`)、栈基寄存器(`%rbp`) 和 程序计数器(`%rip`) 等都要切换到对应的线程

![[Pasted image 20250707230534.png]]

### 协程的概念

线程也可以创建自己的执行体，它们也拥有自己的执行入口和自己的执行栈。这样线程就可以按需调度自己的执行体了。为了切换这些执行体，线程就需要记录这些执行体的控制信息，包括
+ 执行体标识符
+ 执行栈的位置
+ 执行入口地址
+ 执行s上下文
+ ...

线程可以选择一个执行体来执行，此时 CPU 中的程序计数器(`%rip`) 就会执行这个执行体的执行入口，栈指针寄存器(`%rsp`)和栈基寄存器(`%rbp`) 也会指向线程给它分配的执行栈。

![[Pasted image 20250707232030.png]]

要切换执行体时，需要先保存当前执行体的执行上下文，然后才切换到另一个执行体

![[Pasted image 20250707232310.png]]

通过同样的方式，可以恢复到之前的执行体，从而可以在上次中断的地方继续执行。如下图所示

![[Pasted image 20250707232633.png]]

> [!tip] 
> 
> 这些由线程创建的执行体就是所谓 **协程**。因为用户程序不能操作内核空间，所以只能为协程分配用户栈，而且操作系统对协程一无所知。因此，协程也被称为 **用户态线程**
> 

协程的思想在很早就提出了，并且由不同的实现方式。例如，Windows中的纤程、Lua 中的 coroutine。可是无论被赋予什么样的名字，在创建协程时都要指定执行入口；底层都要分配协程执行栈和控制信息，否则用户态调度无法执行。而让出执行权时，也都要保存当前协程的执行现场，否则就无法恢复

> [!attention] 协程的关键思想
> 
> 协程思想的关键就在于 **主动让出控制流** 和 **恢复到让出时的状态**。每个协程用于自己的执行栈，可以保存自己的执行现场
> 

因此，可以让用户程序按需创建协程；协程在合适的位置主动让出控制流，此时，会保存执行上下文；然后，将控制流切换到其他协程。协程恢复执行时，会根据之前保存的执行上下文恢复到让出控制流时的状态。这样就实现了即 **轻量** 又 **灵活** 的由 **用户态进行调度** 的多任务模型

![[Pasted image 20250707235106.png]]

尽管协程具有轻量灵活的优点，但是因为只要一个协程阻塞，整个线程都阻塞的缺点，导致了协程没有及时得到应用。然而，随着高并发成为主流趋势，瞬间抵达的海量请求会导致多进程模型下内存资源不足，多线程模型下内核态用户态之间快速切换。因此，协程这种灵活且轻量的用户态调度模型又重新回到了我们的视野中，并且此时 **IO 多路复用** 也变得非常成熟了。目前，协程结合 IO 多路复用是一个炙手可热的高并发解决方案

> [!seealso] 
> 
> 关于 IO 多路复用参考 [[Linux 系统编程：IO 多路复用]] 和 [[Linux 系统编程： epoll]]
> 

### IO 多路复用

我们知道通过操作系统记录的进程控制信息可以找到进程的 **打开文件描述符表**

![[Pasted image 20241015143221.png]]

进程打开的文件，创建的 SOCKET 等等资源都会记录在进程文件描述符表中，并且返回一个 **文件描述符** 给应用程序用于识别不同的打开文件或者 SOCKET。文件和SOCKET 的操作都需要操作系统提供，也就是需要通过系统调用来完成。

操作系统在创建 TCP SOCKET 时，会在内核空间中同步创建一个 **读缓冲区** 和 一个 **写缓冲区**。应用程序接收数据时就需要从 TCP 读缓冲区拷贝数据用户态缓冲区中；下图展示了应用程序接收数据的过程

![[Pasted image 20250708223734.png]]

发送数据时需将用户态缓冲区中的数据拷贝到 TCP 写缓冲区中。等待操作系统通过 TCP SOCKET 发送出去

![[Pasted image 20250708224149.png]]

现在，问题出现了：当应用程序需要读取数据时，读缓冲区中可能就没有数据可读；当应用程序需要发送数据时，写缓冲区中可能没有空间用于写入。应该如何解决这个问题呢？

> [!tip] 阻塞 IO
> 
> 第一种方法就是 **乖乖的让出 CPU**。进入等待队列中，当 IO 就绪后，再次获得时间片就可以继续执行了。
> 
> 缺点：
> + 要处理一个 SOCKET 就需要占用一个进程或者线程，等到该 SOCKET 处理完成后才能接手下一个
> + 在高并发场景下会加剧调度开销
> 

> [!tip]  非阻塞 IO
> 
> 第二种方法就是不让出 CPU，但是需要 **轮询 SOCKET 是否就绪**。这是一种忙等待的方式.
> 
> 缺点：
> + 轮询的间隔时间无法掌控，容易造成 CPU 资源浪费
> 

目前最常用的解决方案就是 **IO 多路复用**，由操作系统提供支持。通过将需要等待的文件描述符加入到监听集合中，这样可以通过一次系统调用同时监听多个文件描述。每当由文件描述符就绪，就可以逐个处理就绪的文件描述符了。

> [!tip] IO 多路复用
> 
> 由操作系统提供支持，应用程序将需要监听的文件描述符告知操作系统，当操作系统监听到文件描述符就绪时，就会通知应用程序。此时，应用程序就可以逐个处理的文件描述符了
> 
> 优点：
> + 应用程序不会应用等待某个文件描述符而阻塞，也不会陷入忙等待
> 
> 

Linux 提供了 $3$ 种 IO 多路复用的实现，参考 [[Linux 系统编程：IO 多路复用]] 和 [[Linux 系统编程： epoll]]。下面我们在 Python 种演示

```python hl:36,50,22-25
import select
import socket
import argparse


class Server:
    def __init__(self):
        self.__args = self.__parse_command_line()

        self.__server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM) # Create a TCP socket
        self.__server_socket.setblocking(False)  # Set the socket to non-blocking mode
        self.__server_socket.bind((self.__args.host, self.__args.port)) # Bind the socket to the specified host and port
        self.__server_socket.listen(socket.SOMAXCONN)  # Listen for incoming connections
        self.__epoll = select.epoll()  # Create an epoll object
        self.__register(self.__server_socket.fileno(), select.EPOLLIN)  # Register the server socket with the epoll object
        self.__connections = {} # Dictionary to keep track of connected clients

    def start(self):
        """Start the server and listen for incoming connections."""
        while True:
            # Wait for events on the registered file descriptors
            events = self.__epoll.poll()
            for (fd, event) in events:
                # 处理文件描述符的就绪事件
                self.__handle_event(fd, event)

    def __register(self, fd, events):
        """Register a file descriptor with the epoll object."""
        print(f"Registering fd {fd} with events {events}")
        self.__epoll.register(fd, events)
    
    def __unregister(self, fd):
        """Unregister a file descriptor from the epoll object."""
        self.__epoll.unregister(fd)
    
    def __handle_event(self, fd, event):
        if event & select.EPOLLHUP:
            # Handle client disconnection
            print(f"Client {fd} disconnected")
            self.__unregister(fd)
        elif event & select.EPOLLIN:
            print(f"Event EPOLLIN on fd {fd}")
            if fd == self.__server_socket.fileno():
                # Accept a new client connection
                self.__accept()
            else:
                # Handle data from an existing client
                self.__handle_client(fd)

    def __handle_client(self, fd):
        """Handle communication with a connected client."""
        client = self.__connections.get(fd)
        try:
            data = client.recv(1024)  # Receive data from the client
            if not data:
                print(f"Client {fd} closed the connection")
                self.__unregister(fd)  # Unregister the client if no data is received
                client.close()
                self.__connections.pop(fd, None)  # Remove the client from the connections dictionary
            else:
                print(f"Received data from {fd}: {data.decode()}")
                # Here you can process the received data and send a response if needed
                # For example, echo the data back to the client
                client.send(data)  # Echo the received data back to the client
        except socket.error as e:
            print(f"Socket error on {fd}: {e}")
            self.__unregister(fd)
            client.close()

    def __accept(self):
        """Accept a new client connection."""
        client, addr = self.__server_socket.accept()
        print(f"Accepted connection from {addr}")
        client.setblocking(False)
        self.__register(client.fileno(), select.EPOLLIN | select.EPOLLHUP | select.EPOLLET)
        self.__connections[client.fileno()] = client
        
    
    def __parse_command_line(self):
        parser = argparse.ArgumentParser(description="Chat server")
        parser.add_argument("--host", type=str, default="localhost", help="Host to bind the server")
        parser.add_argument("--port", type=int, default=12345, help="Port to bind the server")
        return parser.parse_args()


if __name__ == "__main__":
    server = Server()
    server.start()
```

> [!seealso] 
> 
> 在 Python 种使用 epoll 请参考 [select](https://docs.python.org/zh-cn/3.13/library/select.html)
> 

虽然 IO 多路复用解决了线程应为等待 IO 而阻塞的或忙等待的问题，但是并非没有问题！当在文件描述符可读时，此时如果消息没有读完，那么就需要保存当前文件描述符的状态。当文件描述符再次可读时，需要恢复状态。因此，在 IO 多路复用中实现业务逻辑时，我们需要随着事件的等待和就绪而平凡的保存文件描述符的状态和恢复文件描述符的状态

> [!attention] 
> 
> 这显然不符合我们的常规开发习惯。对应复杂的业务逻辑，这会增加程序的复杂性
> 

如下图所示，在文件描述符就绪时需要恢复文件描述符的状态，并且在需要等待时保存文件描述符的状态。这显然和协程调度的逻辑非常相似，因此非常适合使用协程。

![[Pasted image 20250709081227.png]]

在上述示例代码中的第 $22$ 至第 $25$ 行的事件循环依旧存在，但是我们不直接处理围绕在文件描述符上的业务逻辑，而是面向协程调度

> [!tip] 
> 
> 如果就绪是用于监听网络连接的文件描述符，就接收连接并创建一个套接字，将这个套接字交给一个协程处理。协程的入口地址就指向业务处理函数的入口地址。当业务处理函数需要等待 IO 事件时，就注册事件，并让出控制。
> 
> 如果是其他文件描述符就绪时，就只需要恢复其关联的协程即可
> 

## 生成器

我们知道 **让出和恢复控制流** 是实现协程的核心思想。在 Python 中，我们可以使用 `yield` 关键字让出控制流，并且使用内置函数 `next()` 或者生成器对象的 `send()` 方法恢复控制。因此，在 Python 我们可以使用 `yield` 和 `next()` 来实现协程。如下示例，展示了两个函数通过 `yield` 和 `next()` 交替执行的例子

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

如何才能让 **多个任务反复交替执行** 呢？我需要构建一个 **循环**，让多个任务在事件循环中交替执行；因此，我们还需要一个列表用于保存正在执行的任务，这个列表称为 **任务列表**

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
> 只要是代码就要在线程中执行，协程也不例外。即使使用了协程，依旧会出现线程的切换

## 原生协程

**异步 IO** 是一种并发编程设计，在 Python 中获得了专门的支持，从 Python 3.4 到 3.7 迅速发展。我们之前介绍协程的概念时提到，将协程与某个文件描述符绑定，当文件描述符就绪时恢复协程，当需要等待文件描述符就绪时就挂起协程。这种处理 IO 的方式称为 **异步** 处理，简称为 **异步 IO**。这也是 Python 中提供了一个 `asyncio` 的标准库用于处理异步IO原因

您可能会害怕地想，“并发、并行、多线程、多进程“，这已经有很多东西需要掌握了。 **异步 IO 适合在哪里？**


> [!attention] 
> 
> 请注意：在正式学习异步 IO 之前，我们需要将安装 Python3.7 及其之上的解释器版本
> 
> 

这里，我们将再次回顾并发与并行的概念。虽然现在我们重点关注的是 **异步IO** 及其在 Python 中的实现，但有必要花点时间将异步IO与其他同类进行比较，以便了解异步IO如何适应更大的、有时令人眼花缭乱的难题。

> [!tip] 
> 
> 并行(**Parallelism**)包括 **同时执行** 多个动作。在 Python 中使用 `multiprocessing` 标准库实现并行。它需要将任务分散到计算机的 CPU 上。 它非常适合 CPU 密集型任务：紧密绑定的 for 循环和数学计算通常属于这一类
> 

> [!tip] 
> 
> 并发(**Concurrency**) 多个任务能够以重叠的方式运行。并发的概念比并行更宽泛。线程是一种并发执行模型，其中多个线程轮流执行任务。 一个进程可以包含多个线程。
> 

在过去的几年里，一个独立的设计已经被更全面地内置到 CPython 中：**异步IO**，通过标准库 `asyncio` 和新的 `async` 和 `await` 关键字实现

> [!hint] 
> 
> 需要明确的是，异步IO 并不是一个新发明的概念，它已经存在或正在被构建到其他语言和运行时环境中，如 Go、C# 或 Scala
> 

异步 IO 是一种单线程、单进程设计：它使用 **协作式多任务处理**。 换句话说，尽管在单个进程中使用单个线程，但异步 IO 仍会给人一种并发的感觉。异步 IO 的调度单元是 **协程**，它可以并发调度

> [!tip] 
> 
> 协程可以在需要等待 IO 时候让出执行控制流，事件循环将控制流交给其他的协程
> 

异步意味着什么？ 这不是一个严格的定义，但出于我们在这里的目的，我可以想到两个属性：
+ 异步例程能够在等待其最终结果时 “暂停” ，并让其他例程同时运行
+ 异步代码通过上述机制促进了并发执行。 换句话说，**异步代码提供了并发的外观和感觉**

下图将并发与并行放在了一起。 白色术语表示概念，绿色术语表示概念的实现或影响方式：

![[Pasted image 20250711000333.png]]

### async/await

**异步IO的核心是协程**。协程是 Python 生成器函数的专用版本。让我们从一个基线定义开始，然后在此基础上进行构建：**协程是一个可以在返回之前暂停执行的函数**，它可以在一段时间内 **间接地将控制传递给另一个协程**。

在 Python 中，使用关键字 `async def` 定义一个协程，一个协程想要运行这个协程必须在关键字 `await` 或者事件循环中。

```python
import asyncio

async def count():
    print("One")
    await asyncio.sleep(1)
    print("Two")


async def main():
    await asyncio.gather(count(), count(), count())


if __name__ == "__main__":
    import time
    start = time.perf_counter()
    asyncio.run(main())
    elapsed = time.perf_counter() - start
    print(f"executed in {elapsed:0.2f} seconds.")
```

上述代码中，我们总共执行了 $4$ 个协程，其中一个 `main()` 协程和 $3$ 个 `count()` 协程。`main()` 协程的只要作用就是将 $3$ 个 `count()` 协程执行起来。如下给出了上述代码执行时的输出

```shell
➜  pystudy python coroutine.py
One
One
One
Two
Two
Two
executed in 1.00 seconds.
```

> [!tip] 
> 
> 输出顺序是异步I/O的核心。与每个 `count()` 调用的交互是一个独立 **事件循环**。当每个任务达到 `await asyncio.sleep(1)` 时，`count()` 会将控制交换给事件循环 ，并告知事件循环我需要等待。在等待期间，事件循环调度其他协程执行
> 

使用 `async def` 定义的协程函数中，可以使用 `await` `yield` 和 `return` 关键字

```python
# 在协程函数中，允许 await 和 return 关键字
async def f(x):
    y = await z(x)  # OK - `await` and `return` allowed in coroutines
    return y

# 在协程函数中，允许 yield 关键字：这是一个异步生成器
async def g(x):
    yield x  # OK - this is an async generator

# 在协程函数中，禁止使用 yield from
async def m(x):
    yield from gen(x)  # No - SyntaxError

# await 后面的必须是 async def 定义的协程函数
def m(x):
    y = await z(x)  # Still no - SyntaxError (no `async def` here)
    return y
```

下面是异步IO如何减少等待时间的一个示例：给定一个协程 `makerandom()`，它不断生成 `[0,10]` 范围内的随机整数，直到其中一个超过阈值，您希望该协程的多个调用不需要等待彼此连续完成。您可以大致遵循上述两个脚本的模式，并进行轻微更改：

```python
import asyncio
import random
import logging

logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s', datefmt='%Y-%m-%d %H:%M:%S')

c = (
    "\033[0m",
    "\033[36m",  # cyan
    "\033[32m",  # green
    "\033[33m",  # yellow
)

async def make_random(idx: int, threshold: int = 6) -> int:
    print(c[idx+1] + f"initiated make_random({idx})")
    i = random.randint(0, 10)
    while i <= threshold:
        print(c[idx+1] + f"make_random({idx}) == {i} too low, retrying...")
        await asyncio.sleep(idx + 1)
        i = random.randint(0, 10)
    print(c[idx+1] + f"make_random({idx}) == {i} done" + c[0])
    return i


async def main():
    res = await asyncio.gather(*(make_random(i, 10 - i - 1) for i in range(3)))
    return res


if __name__ == "__main__":
    random.seed(444)  # For reproducibility
    r1, r2, r3 = asyncio.run(main())
    print()
    print(f"Results: {r1}, {r2}, {r3}")
```

该程序使用一个主协程函数 `make_random()`，并在 $3$ 个不同输入上并发运行。大多数程序通常由小型模块化协程构成，并通过一个包装函数将这些小型协程串联起来。程序入口函数 `main()` 则负责通过将核心协程映射到某个可迭代对象或池中，来收集任务（futures）

![[asyncio.gif]]

