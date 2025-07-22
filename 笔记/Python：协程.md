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

**线程也可以创建自己的执行体**，它们也拥有自己的 **执行入口** 和 **自己的执行栈**。这样线程就可以按需调度自己的执行体了。为了切换这些执行体，线程就需要记录这些执行体的控制信息，包括
+ 执行体标识符
+ 执行栈的位置
+ 执行入口地址
+ 执行s上下文
+ ...

线程可以选择一个执行体来执行，此时 CPU 中的程序计数器(`%rip`) 就会指向这个执行体的执行入口，栈指针寄存器(`%rsp`)和栈基寄存器(`%rbp`) 也会指向线程给它分配的执行栈。

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

## 生成器协程

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

在 [[操作系统：进程调度#进程执行(进程行为)]] 我们知道进程执行的任务分为两种：**CPU/GPU 密集型任务** 和 **IO 密集型任务**。下表列出了常见的任务分类

| CPU/GPU bound | I/O bound |
| :------------ | :-------- |
| 哈希计算          | 网络        |
| 加密            | 操作数据库     |
| 解密            | 读写磁盘      |
| 压缩            |           |
| 语义分析          |           |
| 音频渲染          |           |
| ...           |           |

我们知道计算机执行指令是顺序执行的。下图展示了 $3$ 个函数的调用，如果 `func2()` 需要消耗 $3$ 小时，那么 `func3()` 只有在 `func2()` 返回后才能被调用。

![[Pasted image 20250716195347.png]]


> [!attention] 
> 
> 无论是 CPU/GPU 密集型任务还是 I/O 密集性任务，对于函数而言，本质上都是阻塞的
> 

如下示例代码，它演示了函数顺序调用的过程

```python
def fib(n):
    if n <= 2:
        return 1
    return fib(n-1) + fib(n-2)


def main():
    res = fib(43)
    print("hello, world!")


if __name__ == "__main__":
    main()
```

![[all-function-blocking.gif]]

执行上述代码时，需要等待很长一段时间才会输出 `"hello, world!"`。程序实际上是在等待函数 `fib(43)` 的返回。在这个例子中，我们在进行斐波拉契数列的计算，这是一个 CPU 密集型任务。同样的，即使我们在执行从远端服务器获取结果的任务也会等待。

> [!attention] 
> 
> 所有的函数调用本质上都是阻塞的，需要等待函数的返回
> 

例如，我们请求一个服务器响应结果，但是服务器并不会立即响应。因为，它可能在处理其他任务，或者正在查询数据库并且需要消耗一定的时间才能返回我们需要的结果。

> [!attention] 
> 
> 就像之前介绍的那样，我们可以 **阻塞等待**，也可以 **不停的轮询(非阻塞)**。更优雅的解决方式就是让操作系统通知我们结果是否到达(IO多路复用)
> 

**异步 IO** 的出现就是为了解决 **IO 密集型** 任务中的输入输出之间的平衡问题。在一个线程中，当一个任务执行时遇见了 IO 操作，如果 IO 操作会导致线程阻塞时，线程并不等待 IO 操作完成，而是切换到另一个任务继续执行。

> [!attention] 
> 
> 这种切换就是我们之前介绍的协程。在 Python 中，我们称为异步IO
> 

从 Python3.4 开始，Python 引入了 `asyncio` 标准库，并在 Python3.5 版本开始引入 `async/await` 两个关键字。关键字 `async` 用于定义异步函数，或者称为 **协程函数**。而关键字 `await` 用于调用协程函数。所有的协程函数在一个 **事件循环(event loop)** 中执行

> [!tip] 
> 
> 事件循环本质上就是任务执行的管理器，或者称为任务调度器。当协程使用 `await` 等待另一个协程函数执行时，此时当前协程就会暂停执行，并将控制流交还给事件循环。事件循环获得控制流之后，就会挑选一个协程并将控制流交给它
> 

### 协程：async/await

从 Python3.5 版本开始，我们使用 `async def` 定义协程，使用 `await` 等待一个协程执行完毕。例如 

```python
import asyncio

async def counter():
    print("one")
    await asyncio.sleep(1)
    print("two")


if __name__ == "__main__":
    print("type(counter): ", type(counter))    # type(counter):  <class 'function'>
    print("type(counter()): ",type(counter())) # <class 'coroutine'>
```

执行上述代码时会抛出 `RuntimeWarning:  coroutine 'counter' was never awaited` 警告，提示我们协程 `counter()` 没有被等待

> [!attention] 
> 
> 执行上述代码的输出表明，使用 `async def` 定义是一个函数，当该函数调用时不会立即执行，而是返回一个对象，其类型为 `coroutine`
> 
> 换句话说，调用由 `async def` 定义的函数会返回一个协程对象
> 

调用协程函数仅仅只是返回一个协程对象，如何才能让协程函数执行呢？我们之前就已经提过，**协程仅仅在事件循环中执行**。为此，在 `asyncio` 标准库中提供了一些简便的执行协程对象的函数。例如 `asyncio.run()`

```python
import asyncio

async def counter():
    print("one")
    await asyncio.sleep(1)
    print("two")


if __name__ == "__main__":
    c = counter()
    asyncio.run(c)
```

函数 `asyncio.run(counter())` 会在当前创建一个事件循环，并将控制流交给协程 `counter()`，这样协程就可以在事件循环中执行了

> [!attention] 
> 
> 请注意：每个线程只能有一个事件循环
> 

关键字 `await` 可以用于在协程中调用其他的协程函数。关键字 `await` 需要做两件事情。首先，**将协程放入事件循环中**；然后，**暂停使用它的协程**。显然，这会导致使用 `await` 的协程等待，直到 `await` 后面的协程执行完毕。

下面的示例代码演示了 `await` 暂停使用它的协程的例子

```python
import asyncio


async def one():
    return 1


async def green(timeout):
    await asyncio.sleep(timeout)
    return "hello, world!"


async def main():
	res_one = await one()
	res_green1 = await green(2)  # 等 2 秒
	res_green2 = await green(3)  # 等 3 描述
    print(res_one)
    print(res_green1)
    print(res_green2)


if __name__ == "__main__":
    asyncio.run(main())
```

可以遇见的是，上述代码执行时最少需要 $5$ 秒。显然，我们的异步函数并没有异步执行

```shell
➜  pystudy time python3.14 coroutine.py 
1
hello, world!
hello, world!

real    0m6.711s
user    0m0.053s
sys     0m0.013s
```

### 任务：Task

上一节中最后一个例子我们观察到我们的协程并没有异步执行。想要异步的执行协程，我们需要使用 **任务(tasks)**。任务类(task class)是对协程的一层包装，它安排协程在事件循环中执行，同时也运行取消任务

> [!hint] 
> 
> 协程是不能被取消的。
> 

下面代码展示了一个协程对象具有的属性

```python
>>> async def coroutine():
...     return 1
... 
>>> dir(coroutine())
<stdin>:1: RuntimeWarning: coroutine 'coroutine' was never awaited
RuntimeWarning: Enable tracemalloc to get the object allocation traceback
['__await__', '__class__', '__del__', '__delattr__', '__dir__', '__doc__', '__eq__', '__format__', '__ge__', '__getattribute__', '__getstate__', '__gt__', '__hash__', '__init__', '__init_subclass__', '__le__', '__lt__', '__name__', '__ne__', '__new__', '__qualname__', '__reduce__', '__reduce_ex__', '__repr__', '__setattr__', '__sizeof__', '__str__', '__subclasshook__', 'close', 'cr_await', 'cr_code', 'cr_frame', 'cr_origin', 'cr_running', 'cr_suspended', 'send', 'throw']
```

> [!hint] 
> 
> 我们看见了几个熟悉的属性： `send` 和 `close` 和 `throw`。生成器对象也具有这些属性
> 

下面我们来看一下 `task` 对象具有的属性

```python
>>> import asyncio
>>> task = asyncio.create_task(coroutine())
Traceback (most recent call last):
  File "<python-input-7>", line 1, in <module>
    task = asyncio.create_task(coroutine())
  File "/usr/lib/python3.13/asyncio/tasks.py", line 407, in create_task
    loop = events.get_running_loop()
RuntimeError: no running event loop
```

> [!attention] 
> 
> `task` 不仅仅只是一个 `task` 对象，它会安排协程在事件循环上执行。由于这里我们还没有创建事件循环，所有 Python 抛出 `RuntimeError` 异常
> 

```python
>>> async def main():
...     task = asyncio.create_task(coroutine())
...     print(dir(task))
...
>>> asyncio.run(main())
['__await__', '__class__', '__class_getitem__', '__del__', '__delattr__', '__dir__', '__doc__', '__eq__', '__format__', '__ge__', '__getattribute__', '__getstate__', '__gt__', '__hash__', '__init__', '__init_subclass__', '__iter__', '__le__', '__lt__', '__module__', '__ne__', '__new__', '__reduce__', '__reduce_ex__', '__repr__', '__setattr__', '__sizeof__', '__str__', '__subclasshook__', '_asyncio_future_blocking', '_callbacks', '_cancel_message', '_coro', '_exception', '_fut_waiter', '_log_destroy_pending', '_log_traceback', '_loop', '_make_cancelled_error', '_must_cancel', '_result', '_source_traceback', '_state', 'add_done_callback', 'cancel', 'cancelled', 'cancelling', 'done', 'exception', 'get_context', 'get_coro', 'get_loop', 'get_name', 'get_stack', 'print_stack', 'remove_done_callback', 'result', 'set_exception', 'set_name', 'set_result', 'uncancel']
```

很显然，`task` 对象具有更多的属性。这些属性在后续的学习中我们都会介绍。下面我们来看如何让上一小节的两个任务并发执行

```python
import asyncio


async def one():
    return 1


async def green(timeout):
    await asyncio.sleep(timeout)
    return "hello, world!"


async def main():
    one_task = asyncio.create_task(one())
    green_task1 = asyncio.create_task(green(2))
    green_task2 = asyncio.create_task(green(3))

    print(await one_task)
    print(await green_task1)
    print(await green_task2)

if __name__ == "__main__":
    asyncio.run(main())
```

这里我们将 `await` 一个协程替换为 `await()` 一个 `Task` 对象，由于在创建 `Ttask` 对象时，协程就已经被放入事件循环了，而不是在 `await` 时才放入事件循环。因此，所有的任务在 `main coroutine` 暂停时都会得到执行。因此，上述代码最终执行事件时耗时最长的任务

```shell
➜  pystudy time python3.14 coroutine.py 
1
hello, world!
hello, world!

real    0m3.098s
user    0m0.046s
sys     0m0.023s
```

请注意：不要像下面的方式创建并等待 `Task` 对象完成。这样与直接等待协程对象并没有区别

```python
import asyncio


async def one():
    return 1


async def green(timeout):
    await asyncio.sleep(timeout)
    return "hello, world!"


async def main():

    print(await asyncio.create_task(one()))
    print(await asyncio.create_task(green(2)))
    print(await asyncio.create_task(green(3)))

if __name__ == "__main__":
    asyncio.run(main())
```

> [!summary] 
> 
> 函数 `asyncio.create_task(coroutine)` 将一个协程 `coroutine` 包装成一个任务并将 `coroutine` 放入当前线程的事件循环中执行，然后该函数会返回一个可等待的 `task` 对象
> 
> 当我们使用 `await` 关键字等待 `Task` 对象时，就可以获取 `Task` 对象关联的协程的返回值
> 

#### 任务状态

`Task` 对象提供了一些获取其状态的方法。例如，检查任务是否完成可以使用方法 `task.done()`；检查任务是否取消可以使用方法 `task.cancelled()`。下表列出了几个获取 `Task` 对象状态的方法

| 方法                 | 描述                        |
| :----------------- | :------------------------ |
| `task.done()`      | `Task` 对象处于完成状态时返回 `True` |
| `task.canclled()`  | `Task` 对象被取消了返回 `True`    |
| `task.canclling()` | `Task` 对象在正在被取消时返回 `True` |

#### 任务取消

大多数的异步任务涉及网络 IO。由于网络环境的原因，任务什么时候完成时不可预知的，可能会持续很久的时间。因此，当任务等待过长的时间时，我们应该能够让任务取消。

`Task` 对象提供了一个 `task.canclle()` 方法用于取消任务。当我们等待一个被取消的任务时会抛出 `asyncio.CancelledError` 异常，我们必须处理这个异常

```python
import asyncio


async def green(timeout):
    await asyncio.sleep(timeout)
    return "hello, world!"


async def main():
    green_task = asyncio.create_task(green(20))

    seconds = 0
    while not green_task.done():
        await asyncio.sleep(1)
        seconds += 1
        print("任务执行 %d 秒" % seconds)
        # 5 秒后取消任务
        if seconds == 5:
            # 取消任务
            green_task.cancel()

    try:
        res = await green_task
    except asyncio.CancelledError as e:
        print(e)
    
    print(res)


if __name__ == "__main__":
    asyncio.run(main())
```

让任务等待固定的时间是常见的，因此 `asyncio` 为我们提供了封装好的函数 `asyncio.wait_for()` 用于设置任务的等待时间，当超时时会将任务取消

```python
import asyncio


async def green(timeout):
    await asyncio.sleep(timeout)
    return "hello, world!"


async def main():
    green_task = asyncio.create_task(green(20))

    try:
        res = await asyncio.wait_for(green_task, timeout=5)
        print(res)
    except asyncio.exceptions.TimeoutError as e:
        print("timeout: task has canclled")


if __name__ == "__main__":
    asyncio.run(main())
```

> [!attention] 
> 
> `asyncio.wait_for()` 将等待一个任务 `timeout` 秒，如果超时后任务没有结束，就会将任务取消。超时会抛出 `asyncio.execptions.TimeoutError` 异常
> 

有时候任务超时并不意味着要取消，引起超时的原因可能是技术原因。在 `asyncio` 标准库中提供了一个函数 `asyncio.shield(task)`，它会屏蔽 `task` 的取消操作

```python
import asyncio


async def green(timeout):
    await asyncio.sleep(timeout)
    return "hello, world!"


async def main():
    green_task = asyncio.create_task(green(20))

    try:
        res = await asyncio.wait_for(asyncio.shield(green_task), timeout=5)
        print(res)
    except asyncio.exceptions.TimeoutError as e:
        print("timeout with other reasion")
	    res = await green_task
	    print(res)

if __name__ == "__main__":
    asyncio.run(main())
```

### async with 语句

在 [[Python：文件 IO#上下文管理器]] 中我们介绍了 `with` 语句并且在 [[Python：上下文管理器]] 中介绍 `with` 语句的原理。

异步上下文管理器也是非常有用的。例如，当我们在进行数据库操作时，我们必须执行 `commit` 才能将数据真正的存储到数据库中。对于像数据库中 IO 操作，非常适合使用异步IO，因此有必要支持异步上下文管理器

用于支持异步上下文管理的魔术方法有两个

| 魔术方法           | 描述                  |
| :------------- | :------------------ |
| `__aenter__()` | `__enter__()` 的异步版本 |
| `__aexit__()`  | `__exit__()` 的异步版本  |

```python
import asyncio
import aiohttp


class AsyncSession:
    def __init__(self, url):
        self.url = url

    async def __aenter__(self):
        self.session = aiohttp.ClientSession()
        response = await self.session.get(self.url)
        return response

    async def __aexit__(self, excep_type, execp_vale, traceback):
        await self.session.close()
    

async def chech(url):
    async with AsyncSession(url) as response:
        html = await response.text()
        print(f"{url}: {html[:20]}")


async def main():
    chech1 = asyncio.create_task(chech("https://www.baidu.com"))
    chech2 = asyncio.create_task(chech("https://duoke360.com"))
    chech3 = asyncio.create_task(chech("https://liwenzhou.com"))

    await chech1
    await chech2
    await chech3

if __name__ == "__main__":
    asyncio.run(main())
```

### asyncio.gather()

上述示例中，我们创建了 $3$ 个任务，为了获得这些任务的结果，我们需要 `await` $3$ 次。标准库 `asyncio` 中提供了一个函数 `asyncio.gather()` 它可以收集 `coroutine` 并将它们放入事件循环中执行。当所有任务执行完毕后，任务的结果以列表形式返回

```python
import asyncio
import aiohttp


class AsyncSession:
    def __init__(self, url):
        self.url = url

    async def __aenter__(self):
        self.session = aiohttp.ClientSession()
        response = await self.session.get(self.url)
        return response

    async def __aexit__(self, excep_type, execp_vale, traceback):
        await self.session.close()
    

async def chech(url):
    async with AsyncSession(url) as response:
        html = await response.text()
    return f"url: {url}, html: {html[:16].strip()}"


async def main():
    results = await asyncio.gather(chech("https://www.baidu.com"), chech("https://duoke360.com"), chech("https://liwenzhou.com"))

    for res in results:
        print(res)


if __name__ == "__main__":
    asyncio.run(main())
```

#### 异常

如果任务执行过程中出现了异常，那么将导致程序崩溃。在 `gather()` 中有一个参数 `return_exceptions` 默认为 `False`，我们可以将其设置为 `True`，这样就可以允许将任务的异常加入结果列表中

```python
import asyncio
import aiohttp


class AsyncSession:
    def __init__(self, url):
        self.url = url

    async def __aenter__(self):
        self.session = aiohttp.ClientSession()
        response = await self.session.get(self.url)
        return response

    async def __aexit__(self, excep_type, execp_vale, traceback):
        await self.session.close()
    

async def chech(url):
    async with AsyncSession(url) as response:
        html = await response.text()
    return f"url: {url}, html: {html[:16].strip()}"

async def raise_exceptions():
    raise ValueError("测试错误")


async def main():
    results = await asyncio.gather(
        chech("https://www.baidu.com"), 
        chech("https://duoke360.com"), 
        chech("https://liwenzhou.com"), 
        raise_exceptions(),  # 抛出一个 ValueError
        return_exceptions=True # 允许任务返回异常。异常会被放入结果列表中
        )

    for res in results:
        print(res)


if __name__ == "__main__":
    asyncio.run(main())
```

### asyncio.as_completed()

> [!attention] 
> 
> 请注意，使用 `asyncio.gather()` 时只有在所有任务结束之后才能开始处理结果。如果我们想要任务完成后即使处理任务结果 `gather()` 是无法做到的

在标准库 `asyncio` 中提供了 `asyncio.as_completed(*tasks)` 函数，它将返回一个生成器，当任务完成后会将任务从生成器中返回

```python
import asyncio
import aiohttp


class AsyncSession:
    def __init__(self, url):
        self.url = url

    async def __aenter__(self):
        self.session = aiohttp.ClientSession()
        response = await self.session.get(self.url)
        return response

    async def __aexit__(self, excep_type, execp_vale, traceback):
        await self.session.close()
    

async def chech(url):
    async with AsyncSession(url) as response:
        html = await response.text()
    return f"url: {url}, html: {html[:16].strip()}"


async def main():
    tasks = [chech("https://www.baidu.com"), 
             chech("https://duoke360.com"), 
             chech("https://liwenzhou.com")
            ]
            
    for task in asyncio.as_completed(tasks):
        res = await task
        print(res)


if __name__ == "__main__":
    asyncio.run(main())
```

### 问题：await 不会切换到其他 coroutine

下面将演示一个不明显的问题！当我们使用 `await` 等待一个立即返回的 `coroutine` 时，并不会导致协程的切换

```python
import asyncio
import time


async def nothing():
    print("Busy")


async def busy_loop():
    for i in range(10):
        await nothing()  # 等待一个立即返回的 coroutine
		time.sleep(1)


async def normal():
    for i in range(10):
        print("Normal coroutine")

async def main():
    await asyncio.gather(busy_loop(), normal())


if __name__ == "__main__":
    asyncio.run(main())
```

下图展示了上述代码的执行流程。在 `busy_loop()` 协程中的 `await` 并没有通知事件循环将控制流切换到 `normal()` 协程

![[await-issue.gif]]

为了解决这种占用控制流的问题，我们可以使用 `asyncio.sleep(0)`，当我们使用 `await` 等待 `asyncio.sleep()` 时会发生控制流的切换。同时，通知事件循环等待 $0$ 秒，因此在形式上不会阻塞

```python
import asyncio
import time


async def nothing():
    await asyncio.sleep(0)
    print("Busy")


async def busy_loop():
    for i in range(10):
        await nothing()  # 等待一个立即返回的 coroutine


async def normal():
    for i in range(10):
        await asyncio.sleep(0)
        print("Normal coroutine")

async def main():
    await asyncio.gather(busy_loop(), normal())


if __name__ == "__main__":
    asyncio.run(main())
```

### 任务组：TaskGroup

从 Python3.11 版本开始，推荐使用 `TaskGroup` 收集多个任务，`TaskGroup` 是一个异步上下文管理器类

```python
import asyncio
import aiohttp
import time


class AsyncSession:
    def __init__(self, url):
        self.url = url

    async def __aenter__(self):
        self.session = aiohttp.ClientSession()
        response = await self.session.get(self.url)
        return response

    async def __aexit__(self, excep_type, execp_vale, traceback):
        await self.session.close()
    

async def chech(url):
    async with AsyncSession(url) as response:
        html = await response.text()
    return f"url: {url}, html: {html[:16].strip()}"



async def main():
    async with asyncio.TaskGroup() as tg:
        # create_task() 会将任务放入事件循环中进行调度
        chech1 = tg.create_task(chech("https://www.baidu.com"))
        chech2 = tg.create_task(chech("https://duoke360.com"))
        chech3 = tg.create_task(chech("https://www.python.org/"))
    # 退出异步上下文管理区域时，所有的任务都将被等待
    
    # 使用 Task 对象 result() 方法获取任务的结果
    print(chech1.result())
    print(chech2.result())
    print(chech3.result())


if __name__ == "__main__":
    asyncio.run(main())
```

#### 异常

下面的示例代码演示了在任务组中的任务抛出了多个异常，Python 会将这些异常打包成一个 **异常组**

```python
import asyncio


async def coro_normal():
    return "Hello, world!"

async def coro_value_error():
    raise ValueError

async def coro_type_error():
    raise TypeError


async def main():
    try:  
        async with asyncio.TaskGroup() as tg:
            # create_task() 会将任务放入事件循环中进行调度
            chech1 = tg.create_task(coro_normal())
            chech2 = tg.create_task(coro_value_error())
            chech3 = tg.create_task(coro_type_error())
    except ExceptionGroup as eg:
        print(f"{eg=}")  
        # eg=ExceptionGroup('unhandled errors in a TaskGroup', [ValueError(), TypeError()])


if __name__ == "__main__":
    asyncio.run(main())
```

> [!attention] 
> 
> 对于异常组概念，参考 [[Python：异常处理#引发和处理多个不相关的异常]]
> 

为了处理异常组中特定的异常，我们可以使用 `except*` 语句

```python
import asyncio


async def coro_normal():
    return "Hello, world!"

async def coro_value_error():
    raise ValueError

async def coro_type_error():
    raise TypeError


async def main():
    try:  
        async with asyncio.TaskGroup() as tg:
            # create_task() 会将任务放入事件循环中进行调度
            chech1 = tg.create_task(coro_normal())
            chech2 = tg.create_task(coro_value_error())
            chech3 = tg.create_task(coro_type_error())

        results = (chech1.result(), chech2.result(), chech3.result())
    except* ValueError as ve:
        print(f"{ve=}")
    except* TypeError as te:
        print(f"{te=}")
    else:
        print(f"{results=}")

if __name__ == "__main__":
    asyncio.run(main())
```

## 异步迭代器

在 [[Python：模拟容器]] 中我们介绍了迭代器的含义及其实现方式。现在，我们语句了一个问题：如果 `__next__()` 方法或其内部私用的是协程；换句话说，使用 `async def` 定义 `__next__()` 方法，将允许使用 `await` 调用其他协程。例如，我们在数据库中读取数据。

因此，Python 引入了 **异步迭代器** 的概念，这个概念使用两个魔术方法: `__aiter__()` 和 `__anext__()` 实现

```python
import asyncio

from redis import asyncio as aioredis

class RedisReader:

    def __init__(self, redis, keys):
        self.redis = redis
        self.keys = keys

    def __aiter__(self):
        self.ikeys = iter(self.keys)
        return self
    
    async def __anext__(self):
        try:
            key = next(self.ikeys)
        except StopIteration:
            raise StopAsyncIteration
        
        async with self.redis.client() as connection:
            value = await connection.get(key)
        return value


async def main():
    redis = await aioredis.from_url("redis://localhost")
    keys = ["name", "age", "gender"]
    
    async for value in RedisReader(redis=redis, keys=keys):
        print(value.decode("utf-8"))


if __name__ == "__main__":
	asyncio.run(main())
```

## 异步生成器

和 [[Python：函数高级#生成器函数]] 一样，定义异步生成器函数需要使用 `async def` 和 `yield` 关键字

```python
import asyncio


async def async_range(start=0, end=0, step=1):
    if start != 0 and end == 0:
        start, end = 0, start
    
    for i in range(start, end, step):
        yield i

async def pow(x, y):
    return x ** y


async def main():
    async for i in async_range(0, 10, 2):
        res = await pow(i, 2)
        print(res)

asyncio.run(main())
```

## asyncio.Queue

标准库 `asyncio`  中提供了一个用于 `coroutine` 之间交换数据的类 `Queue`，它提供了与 [[Python：进程#队列]] 相似的接口

```python
import asyncio
import random

async def producer(queue, name):
    timeout = random.randint(1, 5)
    await queue.put(timeout)
    print(f"Producer-{name} put{timeout} into th queue", queue)


async def consumer(queue):
    while not queue.empty():
        timeout = await queue.get()
        await asyncio.sleep(timeout)
        print(f"Consumer ate {timeout}", queue)


async def main():
    queue = asyncio.Queue()
    producers = []
    for i in range(10):
        task = asyncio.create_task(producer(queue, i))
        producers.append(task)
    
    await asyncio.gather(*producers, consumer(queue))

asyncio.run(main())
```

下面是执行上述代码的输出

```shell
➜  ((py3.12) ) pystudy python coroutine.py 
Producer-0 put2 into th queue <Queue maxsize=0 _queue=[2] tasks=1>
Producer-1 put5 into th queue <Queue maxsize=0 _queue=[2, 5] tasks=2>
Producer-2 put4 into th queue <Queue maxsize=0 _queue=[2, 5, 4] tasks=3>
Producer-3 put2 into th queue <Queue maxsize=0 _queue=[2, 5, 4, 2] tasks=4>
Producer-4 put1 into th queue <Queue maxsize=0 _queue=[2, 5, 4, 2, 1] tasks=5>
Producer-5 put1 into th queue <Queue maxsize=0 _queue=[2, 5, 4, 2, 1, 1] tasks=6>
Producer-6 put3 into th queue <Queue maxsize=0 _queue=[2, 5, 4, 2, 1, 1, 3] tasks=7>
Producer-7 put5 into th queue <Queue maxsize=0 _queue=[2, 5, 4, 2, 1, 1, 3, 5] tasks=8>
Producer-8 put4 into th queue <Queue maxsize=0 _queue=[2, 5, 4, 2, 1, 1, 3, 5, 4] tasks=9>
Producer-9 put2 into th queue <Queue maxsize=0 _queue=[2, 5, 4, 2, 1, 1, 3, 5, 4, 2] tasks=10>
Consumer ate 2 <Queue maxsize=0 _queue=[5, 4, 2, 1, 1, 3, 5, 4, 2] tasks=10>
Consumer ate 5 <Queue maxsize=0 _queue=[4, 2, 1, 1, 3, 5, 4, 2] tasks=10>
Consumer ate 4 <Queue maxsize=0 _queue=[2, 1, 1, 3, 5, 4, 2] tasks=10>
Consumer ate 2 <Queue maxsize=0 _queue=[1, 1, 3, 5, 4, 2] tasks=10>
Consumer ate 1 <Queue maxsize=0 _queue=[1, 3, 5, 4, 2] tasks=10>
Consumer ate 1 <Queue maxsize=0 _queue=[3, 5, 4, 2] tasks=10>
Consumer ate 3 <Queue maxsize=0 _queue=[5, 4, 2] tasks=10>
Consumer ate 5 <Queue maxsize=0 _queue=[4, 2] tasks=10>
Consumer ate 4 <Queue maxsize=0 _queue=[2] tasks=10>
Consumer ate 2 <Queue maxsize=0 tasks=10>
```

我们观察到 `await queue.put(value)` 并不会与事件循环交互，这是我们之前介绍的问题。因为，这里我们使用的是无限长的队列，`queue.put()` 并不会阻塞并且 `queue.put()` 没有返回值，这就是它不与事件循环交换的原因。

> [!attention] 
> 
> 请注意：在使用 `asyncio.Queue()` 时最好指定 `queue` 的大小
> 

```python
import asyncio
import random

async def producer(queue, name):
    timeout = random.randint(1, 5)
    await queue.put(timeout)
    print(f"Producer-{name} put{timeout} into th queue", queue)


async def consumer(queue, name):
    while not queue.empty():
        timeout = await queue.get()
        await asyncio.sleep(timeout)
        print(f"Consumer-{name} ate {timeout}", queue)


async def main():
    queue = asyncio.Queue(3)
    producers = []
    for i in range(20):
        task = asyncio.create_task(producer(queue, i))
        producers.append(task)
    
    consumers = []
    for i in range(2):
        task = asyncio.create_task(consumer(queue, i))
        consumers.append(task)

    await asyncio.gather(*producers, *consumers)

asyncio.run(main())
```
