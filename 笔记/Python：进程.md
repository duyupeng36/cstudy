# 进程

早期受计算机硬件性能限制，计算机一次只能执行一个程序，它完全控制系统，并且访问所有资源。然而，随着计算机硬件性能的提高，再让一个程序完全控制系统显然会浪费硬件的性能。为了更充分的利用计算机硬件性能，现代操作系统允许 **将多个程序载入内存**，以便于 **并发** 执行

为了实现程序的并发执行，需要对程序提供更严格的控制和更好的划分。这些需求促使 **进程** 概念的产生，即 **进程是执行的程序**。然而，在操作系统眼中，进程并不是只包含程序，还包含了其他内核数据结构

> [!seealso] 
> 
> 关于进程的详细概念，参考 [[操作系统：进程概念]]
> 

## 进程状态

进程在执行时会改变 **状态**。进程状态，部分取决于进程的当前活动。每个进程可能处于以下状态:

| 状态             | 描述            |
| :------------- | :------------ |
| 新的(new)        | 进程正在被创建       |
| 运行(running)    | 指令真正被执行       |
| 等待(waiting)    | 进程等待事件的发生     |
| 就行(ready)      | 进程等待被分配 CPU   |
| 终止(terminated) | 进程指令执行完毕或者被终止 |

这些状态名称比较随意，而且随着操作系统的不同而有所不同。不过，它们表示的状态在所有系统上都会出现。有的系统对进程状态定义的更细。重要的是要认识到: 一次只有一个进程可在一个处理器上 **运行**；但是许多进程可处于 **就绪** 或 **等待** 状态。下图显示了一个进程的状态图

![[Pasted image 20241022001020.png]]

> [!tip] 
> 
> 关于进程调度，请参考 [[操作系统：进程调度]] [[Linux 系统编程：优先级和调度]]
> 

## 进程属性

在内核眼中，**进程是由内核定义的抽象的实体**，并为该实体分配用以执行程序的各项系统资源。换句话说，进程由 **用户内存空间** 和 **一系列内核数据结构** 组成。

> [!tip] 
> 
> 用户内存空间 包含了 **程序代码** 及 **代码所使用的变量**
> 
> **内核数据结构**则用于 **维护进程状态信息**：记录在内核数据结构中的信息包括
> + **与进程相关的标识号(IDs)**
> + 虚拟内存表
> + 打开文件的描述符表
> + **信号传递**及处理的有关信息
> + 进程资源使用及限制
> + **当前工作目录**
> + ....
> 

在 [[Linux 系统编程：进程属性]] 中介绍了操作系统提供的用于获取进程属性的系统函数。在 Python 中，我们可以使用标准库 `os` 来调用这些接口

> [!tip] 
> 
> 标准库 `os` 封装了操作系统接口，我们可以使用标准库 `os` 中的函数调用操作系统接口。
> 

### 进程号与父进程号

**每个进程都有一个进程号(PID)**，进程号是 **一个正数**，用以 **唯一标识系统中的某个进程**。对各种系统调用而言，进程号有时可以作为传入参数，有时可以作为返回值

在 Python 中，使用 `os.getpid()` 获取当前进程的进程号。也就是那个进程调用该接口，就返回那个进程的进程号

每个进程都有一个创建自己的 **父进程**。使用调用 `os.getppid()` 可以检索到 **父进程的进程号**

> [!attention] 
> 
> 当父进程已经结束，在 Unix 中返回的 父进程ID 是初始进程的 PID 中的一个，在 Windows 中仍然是同一个进程 ID，该进程 ID 有可能已经被进行进程所占用。
> 

```python
>>> import os
>>> os.getpid()
39368
>>> os.getppid()
51308
```

### 环境变量

每一个进程都有与其相关的称之为 **环境列表** 的字符串数组，或简称为 **环境**。其中每个字符串都以 **名称=值（name=value）** 形式定义。常将列表中的 **名称** 称为 **环境变量**

> [!tip] 
> 
> 新进程在创建之时，会继承其父进程的环境副本，也就是说，**环境变量 提供了将信息从父进程传递给子进程的方法**
> 

请注意：**子进程只有在创建时才能获得其父进程的环境 _副本_**，所以这一信息传递是单向的、一次性的；子进程创建后，父、子进程均可更改各自的环境变量，且这些变更对对方而言不再可见

在 Python 中可以使用标准库 `os` 中的全局变量 `os.environ` 或者 `os.environb` 方位当前进程的所有环境变量

> [!attention] 
> 
> `os.environ` 和 `os.environb` 的区别就是 `os.environb` 采用 `bytes` 存储环境变量
> 

全局变量 `os.environ` 和 `os.environb` 是一个 **可变映射**。获取或修改进程的环境变量可以直接使用映射的方法

```python
import os

print(os.environ["SHELL"])
print(os.environ.get("SHELL"))
os.environ["PATH"] = "/usr/local/bin:/usr/bin"
print(os.environ.get("PATH"))
```

此外，Python 在标准库 `os` 中还提供下表列出的函数用于获取和修改进程的环境变量

| 函数                         | 描述                            |
| :------------------------- | ----------------------------- |
| `os.getenv(key, default)`  | 获取环境变量 `key` 的值               |
| `os.getenvb(key, default)` | 类似于 `os.getenv(key, default)` |
| `os.unsetenv(key)`         | 删除环境变量 `key`                  |

### 进程凭证

每个进程都有一套用数字表示的 **用户 ID(UID)** 和 **组 ID(GID)**。有时，也将这些 ID 称之为 **进程凭证**

在 Unix/Linux 操作系统中，进程凭证被分为了几个种类，它们用于在进程访问系统资源时确定进程是否具有相应的权限。下表列出了进程凭证的分类

| 用户凭证    | 描述                  |
| :------ | :------------------ |
| 实际用户 ID | 确定进程所属的用户           |
| 实际组 ID  | 确定进程所属的组。即，实际用户所在的组 |
|         |                     |
| 有效用户 ID | 确定进程权限              |
| 有效组 ID  | 确定进程权限              |
| 辅助组 ID  | 确定进程的权限             |

登录操作系统时，会启动一个进程 shell。此时，系统会要求我们输入用户名和密码。当用户名和密码鉴权通过后，登录过程结束。此时，进程 shell 的实际用户就是当前登录的用户

> [!tip] 
> 
> 换句话说，就是 **实际用户就是登录 shell 的用户**
> 

但是，进程在访问系统资源时进行鉴权操作使用的却不是实际用户ID，而是使用 **有效用户 ID** 确定进程是否有权访问系统资源。

> [!important] 
> 
> 通常情况下，有效用户 ID 与实际用户 ID 相同；有效组 ID 与实际组 ID 相同。
> 
> **有效用户 ID 为 0**（root 的用户 ID）的进程拥有超级用户的所有权限。这样的进程又称为 **特权级进程**。而某些系统调用只能由特权级进程执行
> 
 
> [!seealso] 
> 
> 请注意，参考 [[Linux 系统编程：进程凭证]] 获取详细内容
> 

Python 在标准库 `os` 中提供在 [[Linux 系统编程：进程凭证]] 中介绍的获取进程凭证的系统调用。

当我们需要 **查看进程凭证** 时，我们可以使用下表列出的标准库 `os` 中的函数

| 函数                            | 描述                 |
| :---------------------------- | :----------------- |
| `os.getuid()` `os.getgid()`   | 获取实际用户 ID 和 实际组 ID |
| `os.geteuid()` `os.getegid()` | 获取有效用户 ID 和 有效组 ID |

当我们需要 **修改进程凭证** 时，我们可以使用下表列出的标准库 `os` 中的函数

| 函数                                                  | 描述                |
| :-------------------------------------------------- | :---------------- |
| `os.setuid(euid)` `os.setgid(egid)`                 | 设置有效用户 ID 和有效组 ID |
| `os.setreuid(ruid, euid)` `os.setregid(rgid, egid)` | 设置实际和有效ID(用户和组)   |
| `os.setresuid(ruid, euid, suid)`                    | 设置实际 有效和保存用户 ID   |
| `os.setresgid(rgid, egid, sgid)`                    | 设置实际 有效和保存组 ID    |

## 进程创建

在 Python 中，我们可以调用标准库 `os` 中的 `os.fork()` 函数创建进程，该函数封装的是 [[Linux系统编程：进程创建]] 中系统调用 `fork()`

回顾一下系统调用 `fork()`：调用 `fork()` 后将存在两个进程，每个进程都会从 `fork()` 的返回位置继续执行。在父进程中，`fork()` 会返回子进程的 PID；而在子进程中，`fork()` 返回 $0$

> [!attention] 
> 
> `fork()` 采用 **复制** 的方式将父进程拷贝一份从而创建子进程。所以，两个进程拥有 **相同的文本段**。
> 
> 但是，其他虚拟内存区域确是不同的，不过子进程的栈区，数据段，堆区都是从父进程中完全拷贝而来的。
> 
> 关于 `fork()` 系统调用请参考 [[Linux系统编程：进程创建]]
> 

也就是说，`fork()` 返回后，父子进程均可以修改各自的数据，并且不会相互影响

```python
import os
import sys
import time

gdata = [111]


def main():
    ldata = [222]
    pid = os.fork()  # 创建进程
    match pid:
        case 0:
            # 子进程执行代码
            gdata[0] *= 3
            ldata[0] *= 3
        case -1:
            print("创建子进程失败", file=sys.stderr)
            sys.exit(-1)
        case _:
            time.sleep(1) # 父进程等待子进程结束

    # 父子进程都会执行
    print(f"{'child' if pid == 0 else 'parent'}: gdata={gdata} and id(gdata)={id(gdata):#x}; ldata={ldata} and id(ldata)={id(ldata):#x}")
    return 0

if __name__ == "__main__":
    main()
```

上述代码执行的结果如下

```
child: gdata=[333] and id(gdata)=0x7fef4ee24b40; ldata=[666] and id(ldata)=0x7fef4ee25200
parent: gdata=[111] and id(gdata)=0x7fef4ee24b40; ldata=[222] and id(ldata)=0x7fef4ee25200
```

> [!warning] 
> 
> 由于 `os.fork()` 只能在 Unix/Linux 平台上使用；因此，在 Python 我们不使用 `os.fork()` 创建进程
> 

在 Python 中，想要创建进程请使用标准库 `multiprocessing`。其中， Python 将进程抽象并封装成了 **进程类(`Process`)**。下面我们开始介绍 `multiprocessing` 模块的使用

### Process 类

考虑使用递归计算斐波拉契数列的第 $n$ 项这样一个任务。在 [[Python：函数高级#递归]] 我们就已经介绍过来这个任务是非常耗时的；当执行这样一个任务时，控制台中没有任何输出的情况下，用户可能任务程序被卡死了，从而使用 `ctrl+c` 强制结束任务。

例如，如下例程，计算 `fib(42)` 在我使用的电脑上需要执行 $20$ 秒左右的时间。在长时间的等待下，用户可能会不耐烦，从而强制结束任务。

```python
import time


def fib(n):
    if n < 0:
        raise ValueError('n cannot be negative')

    if n < 2:
        return 1
    return fib(n - 1) + fib(n - 2)


def main():
    start = time.perf_counter()
    result = fib(42)
    end = time.perf_counter()
    print(f"fib(42) = {result} use {end - start} seconds")


if __name__ == '__main__':
    main()
```

因此，添加一个动画用于提示用户计算正在进行，从而避免用户主动结束进程。因此，我们定义一个 `spinner()` 函数，用于执行动画。然后，我们在 `main()` 函数中调用它

```python
import itertools
import time


def fib(n):
    if n < 0:
        raise ValueError('n cannot be negative')

    if n < 2:
        return 1
    return fib(n - 1) + fib(n - 2)


def spinner(delay=0.05):
    for r in itertools.cycle(r"-\|/"):
        print(r, end='')
        time.sleep(delay)


def main():
    start = time.perf_counter()
    result = fib(42)
    spinner()  # 开启动画
    end = time.perf_counter()
    print(f"fib(42) = {result} use {end - start} seconds")


if __name__ == '__main__':
    main()
```

用于代码时顺序执行的，因此 `spinner()` 函数要在 `fib(42)` 返回之后才会执行。显然，这不是我们期望的行为。我们期望的是 `fib(42)` 和 `spinner()` 并发执行。

为了使 `fib(42)` 和 `spinner()` 并发执行，我们因该将其放在两个执行流中。这里，我们应该将其放在两个进程中执行。下面我们将 `spinner()` 放在子进程中执行，而计算 `fib(42)` 的任务就放在父进程中执行


```python hl:25,27,32
import itertools
import time

from multiprocessing import Process


def fib(n):
    if n < 0:
        raise ValueError('n cannot be negative')

    if n < 2:
        return 1
    return fib(n - 1) + fib(n - 2)


def spinner(delay=0.05):
    for r in itertools.cycle(r"-\|/"):
        print(f"\rresult = {r}" , end="")
        time.sleep(delay)


def main():
    start = time.perf_counter()
    # 创建一个进程对象
	spinner_process = Process(target=spinner, args=(0.2, ))  
    # 调用进程对象的 start() 方法启动进程
    spinner_process.start()
    
    # 父进执行计算任务
    result = fib(42)
    # # 父进程计算任务完成，通知子进程结束
    spinner_process.terminate()  
    end = time.perf_counter()
    print("\rresult = ", result)
    print("calculation time: ", end - start, "seconds")


if __name__ == '__main__':
    main()
```

下图展示了上述代码执行的结果

![[spinner-process.gif]]

上述代码介绍 `Process` 类的基本用法。下面我们来看 `Process` 类的更多内容

> [!important] 
> 
> `Process` 实例表示在单独进程中执行的任务
> 

在创建 `Process` 类的实例时，我们可以为它提供下表列出的参数

| 参数       | 描述                      |
| :------- | :---------------------- |
| `group`  | 始终为 `None`              |
| `target` | 进程执行的函数                 |
| `name`   | 进程名字                    |
| `args`   | `target` 需要的位置参数，必须是元组  |
| `kwargs` | `target` 需要的关键字参数，必须是字典 |
| `daemon` | `bool` 值代表进程是否为守护进程     |

> [!attention] 
> 
> 请注意：这些参数应该 **总是使用关键字参数传入**。并且，`group` 应当始终为 `None`；它的存在仅仅时为了与 `threading.Thread` 兼容
> 

此外，`Process` 对象还有下表列出的几个 **方法** 或 **数据属性** 用于操作 `Process` 对象

| 属性                | 描述                                          |
| :---------------- | :------------------------------------------ |
| `run()`           | 表示进程活动的方法，及进程执行的任务                          |
| `start()`         | 创建进程，并在该进程中执行 `run()`。每个 `Process` 对象只能调用一次 |
| `join([timeout])` | 阻塞调用进程，等待进程 `process` 结束                    |
| `name`            | 进程的名称。仅用于标识                                 |
| `is_alive()`      | 检查进程是否存活                                    |
| `daemon`          | 进程的守护标志                                     |
| `pid`             | 进程号，只有在 `start()` 之后才存在，否则为 `None`          |
| `exitcode`        | 子进程的退出代码                                    |
| `terminate()`     | 终止进程。向进程发送 `SIGTERM `                       |
| `kill()`          | 终止进程。向进程发送 `SIGKILL`                        |
| `close()`         | 释放进程持有的所有资源                                 |

> [!tip] 
> 
> `exitcode` 只有在子进程退出后才会存在，否则一定是 `None`。该属性的值依赖于进程的退出方式
> + `run()` 正常返回，`exitcode` 的值为 $0$
> + `run()` 中抛出异常，`exitcode` 的值为 $1$
> +  调用 `sys.exit(N)`，`exitcode` 的值为 `N`
> + 由信号 `N` 终止，则 `exitcode` 的值为 `-N`
> 

> [!tip] 
> 
> 我们可以继承 `Process` 类，在派生类中，我们可以重写 `run()` 方法。标准 `run()` 方法调用传递给对象构造函数的可调用对象作为目标参数（如果有），分别从 `args` 和 `kwargs` 参数中获取位置和关键字参数。
> 

## 进程终止

进程终止有两种方式：**异常终止** 和 **正常终止**

> [!tip] 
> 
> 通常，异常终止时接收到了某种信号，参考 [[Linux 系统编程：信号基本概念]]；而，正常终止是进程 **正常结束** 或者 **调用退出函数**
> 

### 终止函数

下面我们介绍进程的正常终止。在 Python 中，进程退出可以调用 `os._exit(n)`，参数 `n` 是退出状态码。

> [!tip] 
> 
> `os._exit(n)` 是系统调用 `_exit(n)` 的封装。该系统调用请参考 [[Linux 系统编程：进程终止#_exit() 和 exit()]]
> 

请注意：调用 `os._exit()` 的程序总会成功终止，即 `os._exit()` 是不返回的

> [!warning] 
> 
> 警告，虽然可以将 $0 \sim 255$ 中的任何一个值传递给 `os._exit()`，并传递给父进程，但是取值大于 $128$ 时将在 shell 脚本中引发混乱
> 
> 当以信号终止一个命令时，shell 会将变量 `$?` 置为 $128$ 与信号值之和，以表征这一事实。如果这与进程调用 `_exit()` 时所使用的 `status` 相同，将令 shell 无法区分
> 

然而，程序通常不会直接调用 `os._exit()`，而是调用标准库 `sys` 中的 `sys.exit(n)` 函数，该函数只是简单抛出 `SystemExit` 异常，表示解释器将要退出。

> [!tip] 
> 
> 函数 `sys.exit(n)` 的参数 `n` 表示 **退出状态码**，默认情况下，该参数是 $0$。shell 会将 $0$ 视为“成功终止”，非零值视为“异常终止”
> 
> 同 `os._exit()` 的要求一样，参数 `n` 通常在 $0 \sim 127$ 之间取值
> 

Python 还提供了一个内建函数 `exit(n)`，它本质上就是 `sys.exit()`。

### 退出处理程序

进程退出之前可能需要执行一些清理操作。在 Python 中使用标准库 `atexit` 中提供的 `atexit.register(func, *args, **kwargs)` 函数注册一个清理函数。这个函数类似与 [[Linux 系统编程：进程终止#退出处理程序]] 中介绍 `on_exit()` 函数

 被注册的函数会在解释器正常终止时执行. `atexit` 会按照注册顺序的 **逆序** 执行; 如果你注册了 `A`, `B` 和 `C`, 那么在解释器终止时会依序执行 `C`, `B`, `A`.

> [!warning] 
> 
> 注意: 通过该模块注册的函数, 在程序被未被 Python 捕获的信号杀死时并不会执行, 在检测到 Python 内部致命错误以及调用了 `os._exit()` 时也不会执行.
> 
> 简单说就是，进程因为 **信号** 或者 `os._exit()` 退出时不会执行。只有通过 `sys.exit()` 或者进程执行完毕时退出才会执行
> 

请注意：不要在退出处理程序中调用 `exit()` 或者 `sys.exit()` 等退出函数

## 监控子进程

父进程有时候需要监视子进程的状态：**子进程终止** 或因 **收到信号而停止**。无论子进程是因为什么原因停止的，父进程都应该立即回收子进程占用的系统资源

> [!tip] 
> 
> 这样已经结束的进程依旧会占用系统资源(进程的内核数据结构)，因为 内核将终止的子进程转换 **僵尸进程**(zombie)，从而允许在子进程结束后，其父进程可以在某一个时刻执行查看子进程是如何退出的
> 

监控子进程涉及的系统调用请参考 [[Linux 系统编程：监控子进程]]。在 Python 中，我们可以使用标准库 `os` 提供的操作系统接口

### os.wait()

函数 `os.wait()` 类似于系统调用 `wait()`，与之不同的是 `os.wait()` 会返回一个元组 `(pid, status)` 并且没有可等待的子进程时，`os.wait()` 会抛出 `ChildProcessError`

> [!warning] 
> 
> 请注意：`wait()` 每次返回只会等待一个子进程，如果调用 `wait()` 的进程有多个子进程，应该在循环中调用 `wait()`
> 

下面的例程演示了 `wait()` 的用法。该程序 **创建多个子进程**，每个子进程对应一个命令行参数(整数)，代表该子进程应该 `time.sleep()` 的秒数，然后直接退出。在创建所有的子进程之后，**父进程循环调用 `os.wait()` 来监控这些子进程的终止**。而直到 `os.wait()` 抛出 `ChildProcessError` 后结束循环

```python
import sys
import os
import time

from multiprocessing import Process

def routine(name, n: int):
    print(f"[{time.strftime('%Y-%m-%d %H:%M:%S', time.localtime())}] child {name} started with PID {os.getpid()}, sleep {n} seconds")
    time.sleep(n)
    exit(0)


def main():
    for i, n in enumerate(sys.argv[1:], start=1):
        Process(target=routine, args=(i, int(n))).start()

    num_dead = 0
    while True:
        try:
            pid, status = os.wait()
        except ChildProcessError:
            print("No more child processes - bye!")
            break
        num_dead += 1
        print(f"[{time.strftime('%Y-%m-%d %H:%M:%S', time.localtime())}] wait() returned child PID {pid}(num_dead = {num_dead})")


if __name__ == "__main__":
    main()
```

### os.waitpid()

`os.wait()` 存在诸多限制。例如
- `wait()` **无法等待某个特定的子进程完成**，只能按顺序等待下一个子进程终止
- `wait()` 会一直 **阻塞直到有子进程退出**
- `wait()` 只能发现那些已经 **终止** 的子进程。子进程因为某个信号 **停止**(例如，`SIGSTOP` 或 `SIGTTIN`)，或者已经停止子进程收到 `SIGCONT` 信号恢复执行后 `wait()` 就无法监视了

我们可以使用 `os.waitpid(pid, options)` 函数监视特定子进程，与系统调用 `waitpid()` 类似。在 Python 中，函数 `os.waitpid()` 返回一个元组 `(pid, status)`；这与 `os.wait()` 的含义相同

参数 `pid` 根据不同的取值指定了 `os.waitpid()` 等待的子进程。下表列出了 `pid` 的取值

| 参数 `pid`   | 描述                                        |
| :--------- | :---------------------------------------- |
| `pid > 0`  | 等待进程号为 `pid` 的进程                          |
| `pid = 0`  | 等待父进程所在进程组中的任意进程                          |
| `pid < -1` | 等待 **进程组标识符(`PGID`)** 与 `pid` 绝对值相等的所有子进程 |
| `pid = -1` | 等待任意子进程                                   |

参数 `options` 是一个位掩码，用于指定 `waitpid` 的行为。取值如下表

| 参数 `options`                        | 描述                         |
| :---------------------------------- | -------------------------- |
| `os.WUNTRACED`：`(Wait Untraced)`    | 等待终止的或者因信号停止的子进程           |
| `os.WCONTINUED`： `(Wait Continued)` | 等待因 `SIGCONT` 而恢复执行的已停止子进程 |
| `os.WNOHANG`：`(Wait No Hang)`       | 非阻塞等待                      |

> [!attention] 
> 
> 如果指定 `options` 中的位掩码包含了 `WNOHANG`，那么通过参数 `pid` 指定的子进程状态未发生变化，`waitpid()` 会立即返回
> 
> 也就是，`WNOHANG` 告诉 `waitpid()` 执行非阻塞等待。如果被等待的子进程正在执行，`waitpid` 返回 。如果指定的 `pid` 不存在子进程，则返回 `(0, 0)`
> 
> 在其他情况下，如果没有匹配的子进程可以被等待，则将引发 `ChildProcessError`
> 


下面的示例程序创建了一个子进程，该子进程会循环调用 `signal.pause()`（在此期间可以向子进程发送信号），但如果在命令行中指定了整型参数，则子进程会立即退出，并以该整型值作为退出状态。同时，父进程通过 `os.waitpid()` 监控子进程，打印子进程返回的状态值并将其作为参数传递给 `print_wait_status()`。一旦发现子进程已正常退出，亦或因某一信号而终止，父进程会随即退出

```python

import os
import sys
import time
import signal

from multiprocessing import Process

  
def print_wait_status(status):  
    # 正常终止  
    if os.WIFEXITED(status):  
        print("child exited, status=%d" % os.WEXITSTATUS(status))  
    # 因信号终止  
    elif os.WIFSIGNALED(status):  
        print("child killed by signal %d(%s)" % (os.WTERMSIG(status), signal.strsignal(os.WTERMSIG(status))))  
    # 因核心转储  
    elif os.WCOREDUMP(status):  
        print("core dumped")  
    # 因信号停止  
    elif os.WIFSTOPPED(status):  
        print("child stopped by signal %d(%s)" % (os.WSTOPSIG(status), signal.strsignal(os.WSTOPSIG(status))))  
    # 因信号重启  
    elif os.WIFCONTINUED(status):  
        print("child continued")  
    else:  
        print("what happened to this child?(status = %x)" % status)


def routine():
    print(f"Child started with PID = {os.getpid()}")
    if len(sys.argv) > 1:
        os._exit(int(sys.argv[1]))
    while True:
        signal.pause()  # 暂停程序

def main():

    process = Process(target=routine)
    process.start()

    while True:
        try:
            # 等待并且等待因为信号停止和恢复的子进程
            pid, status = os.waitpid(-1, os.WUNTRACED | os.WCONTINUED)
        except ChildProcessError as e:
            print(f"waitpid() failed: {e}")
            break

        print(f"waitpid() returned: PID = {pid}; status = {status:#04x}({status >> 8}, {status&0xff})")
        print_wait_status(status)

        # 子进程正常终止或者因为信号而终止
        if os.WIFEXITED(status) or os.WIFSIGNALED(status):
            exit(0)


if __name__ == "__main__":
    main()

```

下面是上述程序运行的结果

```shell
$ python waitpid.py 9
Child started with PID = 164837
waitpid() returned: PID = 164837; status = 0x900(9, 0)
child exited, status=9

$ python waitpid.py 
Child started with PID = 164869
waitpid() returned: PID = 164869; status = 0x86(0, 134)
child killed by signal 6(Aborted)
```

### 等待状态

由 `os.wait()` 和 `os.waitpid()` 返回的 `status` 的值，可用来区分以下子进程事件

- 子进程调用`os._exit()`或 `sys.exit()` 而终止，并指定一个整型值作为退出状态
- 子进程收到未处理信号而终止
- 子进程因为信号而停止，并且父进程以 `WUNTRACED` 标志调用 `waitpid()`
- 子进程因收到信号 `SIGCONT` 而恢复，并且父进程以 `WCONTINUED` 标志调用 `waitpid()`

> [!tip] 
> 
> 术语 **等待状态** 来涵盖上述所有情况，而使用 **终止状态** 的称谓来指代前两种情况
> 

变量 `status` 是一个整数对象，并且我们只使用最低的 $2$ 个字节。对这 $2$ 个字节的填充方式取决于子进程所发生的具体事件。如下图，给出了这两个字节的填充方式

![[Pasted image 20241103160400.png]]

> [!tip] 
> 
> + 正常终止：填充高字节，低字节填充为 $0$
> + 被信号终止：填充低字节
> + 被信号停止：高字节填充停止信号，低字节填充 `0x7f`
> + 被信号回复：填充为 `0xffff`
> 

在标准库 `os` 中提供了一些函数用于提取解析等待状态值。下表列出了这些函数

| 函数                                             | 描述                    |
| :--------------------------------------------- | :-------------------- |
| `os.WIFEXITED(status)`: `With IF EXITED`       | 子进程是否正常退出             |
| `os.WEXITSTATUS(status)`                       | 获取子进程的退出状态            |
|                                                |                       |
| `os.WIFSIGNALED(status)`： `With IF SIGNALED`   | 子进程是否被信号终止            |
| `os.WTERMSIG(status)`                          | 获取终止子进程的信号编号          |
| `os.WCOREDUMP(status)`                         | 子进程是否产生核心转储文件         |
|                                                |                       |
| `os.WIFSTOPPED(status)`： `With IF STOPPED`     | 子进程是否被信号停止            |
| `os.WSTOPSIG(status)`                          | 获取停止子进程的信号变化          |
|                                                |                       |
| `os.WIFCONTINUED(status)`： `With IF CONTINUED` | 子进程是否被 `SIGCONT` 信号恢复 |

> [!tip] 
> 
> 更多内容请参考 [[Linux 系统编程：监控子进程]] 和 标准库 [`os`](https://docs.python.org/zh-cn/3.13/library/os.html)
> 

## 执行程序

当我们创建出进程之后，可以使用 `os.exec()` 系列函数可以将新程序加载到某一进程的内存空间。在这一操作过程中，将丢 **弃旧有程序**，而 **进程的栈、数据以及堆段会被新程序的相应部件所替换**。

Python 在标准库 `os` 中提供的函数与 [[Linux 系统编程：程序执行]] 中介绍函数相似，这里就不在介绍。它们的差异参考标准库 [`os`](https://docs.python.org/zh-cn/3.13/library/os.html)

## 进程间通信

我们之前严重过，进程之间的数据是隔离的。想要实现多个进程间交换数据，就必须提供一种专门的机制，也就是所谓的 **进程间通信机制(IPC)**

现代操作系统提供了许多 IPC 机制，主要分为两类：**通信** 和 **同步**

- **通信**：这些工具 **关注进程之间的数据交换**
- **同步**：这些工具 **关注进程操作之间的同步**

使用多进程时，一般使用 **消息机制** 实现进程间通信，尽可能避免使用同步工具。现代操作系统通过的进程间通信工具主要有两种：**数据传输** 和 **共享内存**

> [!tip] 
> 
> 数据传输：基于 **读取** 和 **写入** 这两个概念。为了进行通信，一个进程将数据写入到 IPC 工具中，另一个进程从 IPC 工具中读取数据。

很显然，这些工具要求 **在用户内存和内核内存之间进行两次数据拷贝操作**：
 + 一次传输是在写入的时候从用户内存到内核内存
 + 另一次传输是在读取的时候从内核内存到用户内存

管道是数据传输工具的一种，如下图所示
 
 ![[Pasted image 20241106085728.png]]

> [!tip] 
> 
> 共享内存允许进程通过将数据放到由 **进程间共享的一块内存** 中以完成信息的交换。内核通过将 **每个进程中的页表条目指向同一个 RAM 分页** 来实现这一功能


一个进程可以通过将 **数据放到共享内存块中** 使得其他进程读取这些数据。由于通信 **无需系统调用**，也 **无需用户内存和内核内存之间的数据拷贝**，因此共享内存的速度非常快

共享内存可以在多个进程之间共享同一个数据结构。然而，当 **一个进程正在更新共享内存** 中的一个数据结构时，**另一个进程就不应该试图读取** 这个数据结构

> [!warning] 
> 
> 尽管共享内存的通信速度更快，但速度上的优势是用来弥补需要对在共享内存上发生的操作进行同步的不足的。在共享内存中，**信号量** 通常用来作为同步方法
> 

> [!summary] 
> 
> 进程之间进行数据交换必须通过内核完成，无论是数据传输还是共享内存
> 

`multiprocessing` 支持进程之间的两种通信通道：**队列** 和 **管道**

### 队列

在标准库 `multiprocessing` 中提供了一个类 `Queue` 它类似于 **消息队列**；任何放入 `multiprocessing.Queue` 中的对象都将被序列化

> [!tip] 
> 
> 关于 `Queue` 的细节请参阅 [管道和队列](https://docs.python.org/zh-cn/3.13/library/multiprocessing.html#pipes-and-queues)
> 

```python
from multiprocessing import Process, Queue

def f(q):
    q.put([42, None, 'hello'])

if __name__ == '__main__':
    q = Queue()
    p = Process(target=f, args=(q,))
    p.start()
    print(q.get())    # 打印 "[42, None, 'hello']"
    p.join()
```

### 管道

使用 `multiprocessing.Pipe()` 函数会返回一个由管道连接的 **连接对象**，默认情况下是双工（双向）。例如

```python
from multiprocessing import Process, Pipe

def f(conn):
    conn.send([42, None, 'hello'])
    conn.close()

if __name__ == '__main__':
    parent_conn, child_conn = Pipe()
    p = Process(target=f, args=(child_conn,))
    p.start()
    print(parent_conn.recv())   # 打印 "[42, None, 'hello']"
    p.join()
```

返回的两个连接对象 `Pipe()` 表示管道的两端。每个连接对象都有 `send()` 和 `recv()` 方法（相互之间的）。请注意，如果两个进程（或线程）同时尝试读取或写入管道的 **同一** 端，则管道中的数据可能会损坏。当然，在不同进程中同时使用管道的不同端的情况下不存在损坏的风险。

> [!tip] 
> 
> `send()` 方法将序列化对象而 `recv()` 将重新创建对象
> 
