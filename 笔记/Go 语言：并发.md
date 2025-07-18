# 并发

早期的计算机一次只能执行一个程序。这种程序完全控制系统，并且访问所有资源。而现代计算机允许加载过个程序到内存中，以便于 **并发** 执行

> [!tip] 并发 Concurrency
> 
> **一段时间内** 发生了很多事情；在这段时间的 **某个时刻**，可能只有一件事情发生
> 

为了实现程序的并发执行，需要对程序提供 **更严格的控制** 和 **更好的划分**。这些需求促使 **进程** 概念的产生，即 **进程为执行的程序**

## 进程

我们使用编程语言编写的代码被编译器编译后成对应平台的机器指令后写入 **可执行文件** 中，这个可执行文件就是 **程序**。当我们想要执行这个程序时，操作系统将可执行文件从磁盘加载到内存中，从而创建 **进程**。操作系统会创建与这个程序相关的内核数据结构，用于管理这个进程

在操作系统内核中，**进程是由内核定义的抽象的实体**，并为该实体分配用以执行程序的各项系统资源。从内核角度看，进程由 **用户内存空间** 和 **一系列内核数据结构** 组成

现代操作系统使用 [[操作系统：虚拟内存]] 管理进程使用的内存资源。进程执行时除了加载的程序代码还有其他数据，这些数据按照不同的类别存储在虚拟内存的不同位置。下图给出了进程虚拟内存的结构

![[Pasted image 20241013130418.png]]

内核数据结构用于维护 **进程状态信息**，称为 **进程控制块**(Process Control Block, **PCB**)，通常包含了下表中的几个数据

| 内核数据     | 描述                         |
| :------- | :------------------------- |
| PID      | 进程标识符，操作系统用于唯一识别进程的一个无符号整数 |
| 虚拟内存表    | 用于管理进程使用的内存信息。即页表等数据结构     |
| 进程资源     | 进程资源的使用及其限制信息              |
| 当前工作目录   | 进程工作的目录，使用相对路径查找文件时的起始目录   |
| 信号       | 进程可以接收的信号机器处理程序            |
| 程序计数器    | 下一条要执行指令的地址                |
| CPU 寄存器  | 进程使用的寄存器中的值                |
| CPU 调度信息 | 进程的状态。包括进程的优先级、调度队列等信息     |
| 进程的记账信息  | 包括 CPU 时间、实际时间、进程数量等信息     |
| IO 状态    | 分配给进程的 IO 设备列表，打开文件列表      |
| ...      | ....                       |

> [!tip] 程序和进程：**_程序本身不是进程_**
> 
> **程序** 只是 **被动(passive)实体**，如存储在磁盘上包含一系列指令的 **文件**(经常称为可执行文件( executable file))
> 
> **进程** 是 **活动( active)实体**，具有一个 **程序计数器** 用于表示下一条要执行命令和 **一组相关资源**
> 

### 进程的状态

进程在执行时会改变 **状态**。进程状态，部分取决于进程的当前活动。每个进程可能处于以下 $5$ 种状态

| 进程的状态 | 描述                    |
| :---- | :-------------------- |
| 新的    | 进程正在创建中，创建完成立即进入就行状态  |
| 就绪    | 进程正在等待分配 CPU          |
| 运行    | 进程的指令正在执行             |
| 等待    | 进程正在等待某个事件发生，例如 IO 完成 |
| 终止    | 进程已经执行完毕              |

这些状态名称比较随意，而且随着操作系统的不同而有所不同。不过，它们表示的状态在所有系统上都会出现。有的系统对进程状态定义的更细。重要的是要认识到: 一次只有一个进程可在一个处理器上 **运行**；但是许多进程可处于 **就绪** 或 **等待** 状态。下图显示了一个进程的状态图

![[Pasted image 20241022001020.png]]

下面我们介绍进程是如何在上述 $5$ 种状态种进行转变的，也就是进程的调度
### 进程的调度

程序想要运行，首先需要输入设备将程序和数据装入计算机内存中。然后，等待内核调度并运行完成。最后，在通过输出设备输出结果。整个过程中，程序从输入到输出在所有环节都是 **独占资源**

> [!tip] 输入和输出阶段 CPU 没有执行任何计算任务，浪费了 CPU 时间
> 实际上输入、输出环节非常慢，而且此时 CPU 没有计算任务，处于 **忙等待**
> 
> 如果下一个程序需要运行，整个过程将再走一遍，这是一个接一个运行的 **串行** 方式

计算机处理 IO 时，会让 CPU 处于忙等待，能否设计一套控制程序运行的技术，来充分利用 CPU 资源？完成这个任务的这就是 **多道处理程序**

多道程序的目标就是 **始终允许某个进程运行** 以最大化 CPU 利用率。这种想法比较简单。**一个进程执行直到它应等待为止**，通常等待某个I/O请求的完成

> [!tip] 多道处理程序 
> 
> 对于简单的计算机系统，当计算机在处理 IO 时，CPU 就处于闲置状态。所有这些等待时间就会浪费，没有完成任何有用工作
> 
> 采用多道程序，试图有效利用这些等待时间。多个进程同时处于内存，当其中一个进程等待时，多道处理程序就从该进程接管 CPU 控制，并将 CPU 交给另一进程。这种方式不断重复。**当一个进程必须等待时，另一进程接管CPU使用权**
>   

进程的调度取决于 $3$ 个方面的因素：**进程的行为**、**调度时机** 和 **调度算法**

#### 进程执行(进程行为)

CPU 的调度取决于进程的行为。程序执行包括 **周期** 进行的 **CPU 执行** 和 **IO 执行**。进程在这两种行为之间不断交替

> [!tip] 进程执行
> 进程执行从 CPU 执行开始，然后是 IO 执行；接着是另一个 CPU 执行，接着另一个 IO 执行；如此往复，直到最后一个 CPU 执行通过向操作系统请求结束，以便于终止进程
> 
> 下图展示了 CPU 执行和 IO 执行交替的序列
> 
> ![[Pasted image 20250706222704.png]]
> 

上图中，进程 A 花费了绝大多数时间在 CPU 执行上，而进程 B 则在 IO 执行上花费了较多时间

> [!tip] 计算密集型任务和 IO 密集型任务
> 
> 对于像进程 A 那样花费绝大多数时间在 CPU 执行上的进程，称为  **计算密集型进程**
> + 计算密集型进程的特征就是：较 **长时间** 处于 CPU 执行 和 **较小频率** 的 IO 执行
> 
> 对于像进程 B 那样在 IO执行上花费绝大多数时间的进程，称为 **IO密集型进程**
> + IO 密集型进程的特征就是：较 **短时间** 处于 CPU 执行 和 **高频率** 的 IO 执行
> 

现代 CPU 变得越来越快，更多的任务倾向为 IO 密集型。由于 CPU 的改进比磁盘(或网络)改进快得多，从而导致未来对 IO 密集型进程的调度处理更加重要。所以，进程调度的基本思想：如果需要运行 IO 密集型进程，那么就应该让它尽快运行，以便于发出 IO请求(磁盘请求或网络请求)，从而保持磁盘或网络始终忙绿

#### 调度时机

调度处理的第一个关键问题是 **何时进行调度决策**。存在需要调度处理的 $4$ 种情形

> [!tip] 调度需要处理的 $4$ 种情形
> 
> 第一：在 **创建一个新进程之后**，需要决定运行父进程还是子进程。父进程 `fork()` 一个子进程之后，这两个进程都处于就绪状态，所以这是一个正常的调度决策，可以任意决定。也就是说，**调度程序可以合法的选择先运行父进程还是先运行子进程**
> 
> 第二：在一个 **进程退出时** 必须做出调度决策。一个进程不再运行(因为它不存在了)，所以必须从就绪进程中选择另外某个进程（**进程结束时，必须调度一个就绪进程**）。如果没有就绪进程，通常会运行一个系统提供的空闲进程
> 
> 第三：当一个运行的进程 **被阻塞时** (可能由于：IO、信号量、其他原因)，必须 **调度一个就绪进程** 运行。阻塞原因也会成为调度进程的因素
> + 例如，进程 A 是一个重要进程，需要等待进程 B 退出临界区域。让进程 B 运行就会使得进程 B 退出临界区域，从而使得进程 A 可以执行
> 
> 第四：**当 IO 中断发生时，必须调度一个就绪进程**。如果中断来自 IO 设备，而该设备现在完成了工作，某些被阻塞的等待该 IO 的进程就可能成为就绪进程，是否让新就绪的进程运行，就取决于调度器
> 

#### 调度算法

进程调度使用了硬件时钟提供了周期性中断，在每个时钟中断或者每 $k$ 个时钟中断时做出调度决策。根据如何处理时钟中断，可以把调度分为两类: **非抢占式** 和 **抢占式**

> [!tip] 非抢占式和抢占式调度
> 非抢占式调取：挑选一个进程后，让该进程运行直到或者被阻塞、或者自动释放 CPU。即使该进程需要运行若干个小时，它也不会被强迫挂起
> + 时钟中断时不会进行调度
>   
> + 在处理完时钟中断之后，如果没有更高优先级的就绪进程到来时，则被中断的进程会继续执行
> 
>抢占式调度：挑选一个进程，并且让该进程运行某个固定时间片。如果时间片到时该进程没有结束，则它被挂起，而调度程序会挑选另一个就绪进程运行。
>+ 进程进行抢占式调度处理，需要在时间片的末端发生时间中断，以便将 CPU 控制返回给调度程序
>  
>+ 如果没有可用的时钟，那么非抢占式调度就是唯一的选择

在不同的环境需要不同的调度策略。出现这样的情形是由于不同的应用领域有不同的目标。也就是说，不同的系统中，调度程序的优化是不同的。根据应用领域不同，可以将环境划分为三种：**批处理**、**交互式** 和 **实时**

> [!tip] 三种系统环境：批处理、交互式、实时
> 
> **批处理系统** 中，不会有用户不耐烦地在终端旁等待一个短请求的快捷响应。因此，**非抢占式调度**，或者对每个进程都有 **长时间周期的抢占式调度**，通常都是可接受的
> 
> 在 **交互式用户环境** 中，**为了避免一个进程霸占 CPU 拒绝为其他进程服务，抢占是必须得**。即便没有进程想要永远运行，但是，某个进程由于一个进程错误也可能无限期的排斥其他进程。为了避免这样的情况发生，抢占也是必须得
> 
> 在实时系统中，抢占有时是不需要的，因为进程了解它们可能会长时间的得不到运行，所以通常会很快完成工作并阻塞
> 

不同环境中的调度算法考虑的方面有所不同，但是它们有共同的特点。下面列出来各自系统需要考虑的问题

| 调度原因      | 描述              |
| :-------- | :-------------- |
| **所有系统**  |                 |
| 公平        | 给每个进程公平的 CPU 份额 |
| 策略强制执行    | 保证规定的策略被执行      |
| 平衡        | 保持系统的所有部分都忙碌起来  |
|           |                 |
| **批处理系统** |                 |
| 吞吐量       | 每小时任务数量         |
| 周转时间      | 从提交任务到终止的时间最小   |
| CPU 利用率   | 保持 CPU 始终忙碌     |
|           |                 |
| **交互式系统** |                 |
| 响应时间      | 快速响应请求          |
| 均衡性       | 满足用户的期望         |
|           |                 |
| **实时系统**  |                 |
| 满足截止时间    | 避免丢失数据          |
| 可预测性      | 在多媒体系统种避免品质降低   |

我们在 [[操作系统：进程调度]] 中已经介绍了关于批处理和交互式系统的调度算法，这里我们不再介绍

### 多进程演示

在 Go 语言中，并没有给我们提供创建进程的接口，因此下面的示例我们使用 Python 进行演示。

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

### 进程间通信

因为操作系统必须保证进程不能修改其他进程使用的内存。因此，进程之间存在天然的内存壁垒。如下图所示，它展示使用系统调用 `fork()` 创建的进程之间的关系

![[Pasted image 20241102210610.png]]

当使用 `fork()` 创建子进程后，子进程完全复制父进程的所有内容。之后它们就会互不干扰的继续执行

```c
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>    // 提供 pid_t 的定义
#include <unistd.h>  // 提供 fork() 的函数原型


static int idata = 111;  // 在进程的初始化数据段

int main() {

    int istack = 222;  // 在进程的 main 函数的栈帧中

    pid_t child_pid;

    switch (child_pid = fork())
    {
    case -1: // 创建子进程失败
        fprintf(stderr, "fork failed: %s\n", strerror(errno));
        break;
    
    case 0:  // fork 在子进程中返回 0
        idata *= 3;
        istack *= 3;
        break;

    default: // 在父进程中返回 子进程的 pid
        sleep(3);
        break;
    }
    // 父子进程都会执行此处的代码
    printf("PID=%ld %s idata=%d istack=%d\n", (long)getpid(), (child_pid == 0) ? "(childe)": "(parent)", idata, istack);
    return 0;
}
```

使用 GCC 编译运行的结果为

```shll
$ gcc process.c 
$ ./a.out 
PID=78492 (childe) idata=333 istack=666
PID=78491 (parent) idata=111 istack=222
```

> [!attention] 
> 
> 注意到：子进程中对全局遍历和局部变量的修改并没有影响到父进程中的全局变量和局部变量
> 

同一个虚拟内存地址，在不同的进程中，会被映射到不同的物理内存区域，因此在多个进程之间通过虚拟地址交换数据是不可能完成的。鉴于进程之间天然的内存壁垒，想要实现多个进程间交换数据，就必须提供一种专门的机制，也就是所谓的 **进程间通信机制**

Linux 系统上提供了较为完整的 _通信_ 和 _同步_ 工具，他们被划分为了三类。如下图所示

![[Pasted image 20241106000843.png]]

> [!attention] 
> 
> 这些内容请参考
> + [[Linux 系统编程：PIPE]]
> + [[Linux 系统编程：FIFO]]
> + [[Linux 系统编程：System V 消息队列]]
> + [[Linux 系统编程：System V 共享内存]]
> 
> 

## 线程

线程是允许应用程序 **并发执行任务** 的机制。一个进程中可以包含多个线程，它们均会 **独立执行**，且 **共享**  虚拟内存中的 **数据段**，**代码段** 和 **堆内存段**。线程都有自己 **独立的栈内存段**。如下图是同时执行 $4$ 个线程的进程

![[Pasted image 20241110230539.png]]

多线程编程具有如下四大类的优点

> [!tip] 响应性
> 
> 如果一个交互程序采用多线程，那么即使部分阻塞或者执行冗长操作，它仍可以继续执行，从而增加对用户的响应程度。这对于用户界面设计尤其有用
> 
> + 例如，当用户触发一个耗时操作时，如果让耗时操作在一个线程上执行，服务线程已经可以继续为用户提供服务
> 

> [!tip] 资源共享
> 
> 进程间通信需要额外的 IPC 机制。然而，**线程本来就是共享 _堆内存_ 和 _数据段_ 的**，无需提供额外的通信机制
> 

> [!tip] 经济
> 
> 为一个进程分配资源是非常昂贵的。例如，每个进程都需要创建页表，打开文件描述符表等待数据结构
> 
> 由于线程能够共享它们所属进程的资源，所以创建和切换线程更加经济
> 

> [!tip] 可伸缩性
> 
> 对于 **多处理器体系结构**，多线程的优点更大，因为 **线程可在多处理核上并行运行**
> 
> 不管有多少可用 CPU, 单线程进程只能运行在一个 CPU 上
> 

### 多核编程

早期计算机设计为了响应更多的计算性能需求，单处理器系统逐步发展为多处理系统(计算核心放在多个芯片上)。现代 CPU 的设计趋势是 **将多个计算核心放在单个芯片上**。但是，无论是将多个计算核心放在多颗芯片还单颗芯片上，都称为 **多核处理器** 或 **多处理**。多线程可以更轻松的利用 CPU 的多个计算核心

> [!tip] 
> 
> 对于多核 CPU，线程可以更方便的利用 CPU 的多个核心
> 

考虑一个有 $4$ 个线程的应用程序，讨论单核系统和多核系统上线程的执行。在单核系统上，**CPU 只能同一时间执行单个线程**。因此，**并发** 仅仅意味着 **线程随着时间推移交错执行**。如下图，显示 $4$ 个线程在单核系统上的执行顺序

![[Pasted image 20241111001916.png]]

在多核系统上，CPU 可以执行多个线程。因此，**并发** 意味着 **线程能并行执行**。如下图，显示 $4$ 个线程在双核系统上的执行顺序

![[Pasted image 20241111002027.png]]

这里再次接触到 **并发性** 和 **并行性** 两个概念

> [!attention] 
> 
> 并行性：**同时执行多个任务**
> 
> + 如果食堂有 $3$ 个打饭窗口，可以同时执行 $3$ 个相同的任务，服务不同的食客
> 
 
> [!attention] 
> 
> 并发性：允许多个任务可以同时向前推进
> 

在  **多核架构** 出现之前，大多数计算机系统只有单个处理器。CPU 调度器通过 **快速切换系统内的进程**，以便 **允许每个进程取得进展**，从而提供并行假象。这些进程并发运行，而非并行运行

#### 并行类型

有两种类型的并行: **数据并行** 和 **任务并行**

> [!tip] 
> 
> 数据并行： 数据分布于多个计算核上，并在每个核上执行相同操作
> 
> 考虑一下对大小为 $n$ 的数组的内容进行求和
> + 对于单核系统，单线程只能从第 $0$ 个元素加到第 $n-1$ 个元素
> + 对于多核系统，假设有 $2$ 个线程，线程 A 从第 $0$ 个元素加到第 $\frac{n}{2}-1$ 个元素；线程 B 从第 $\frac{n}{2}$ 个元素加到第 $n-1$ 个元素
> 

> [!tip] 
> 
> 任务并行：多个任务分布在多个计算核上，每个任务执行独立操作
> 
> 不同线程可以操作相同的数据，或者也可以操作不同的数据
> 

### 线程模型

**线程是一个独立的执行流**。实现线程包的方案有两种：**在用户空间中** 和 **在内核中**。在用户空间中实现的线程包称为 **用户线程**；在内核中实现的线程称为 **内核线程**

现代操作系统支持内核线程，并且操作系统只知道内核线程。换句话说，用户线程想要执行就必须绑定一个内核线程

> [!tip] 
> 
> 在 [[操作系统：多线程概念#线程模型]] 中介绍用户线程和内核线程的区别。这里我们不再重复介绍

### 多线程示例

还是使用 [[Python：进程]] 中为计算第 $n$ 为斐波拉契数列添加动画的例子，这里我们替换为线程的版本

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

在大多数语言中，线程是可以并行执行的；然而，由于 CPython 实现是采用了**全局解释器锁**(**GIL**)：**只允许一个 Python 解释器进程中只有一个线程执行**

这意味着在 Python 中使用线程并不能提供所有任务的效率。就行在 [[Python：并发模型]] 中介绍的 CPU 密集型任务就不适合使用线程处理

>[!important] 
>
>在 Python 中，对于那些需要花费大量时间等待外部事件的任务，使用线程加速就非常合适
>

### 线程同步

由于线程是同一个进程的不同执行流，因此线程共享整个进程的虚拟内存空间。多个线程争抢同一个资源时会导致得到的结果可能会出现错误

> [!attention] 
> 
> 相关概念参考 [[Linux 系统编程：互斥量]]
> 

## 协程

我们已经知道 **线程是进程中的执行体**，拥有一个执行入口以及从进程虚拟地址空间中分配的栈(包括内核栈和用户栈)，操作系统会记录线程控制信息，当线程获得 CPU 时间片之后才可以执行

> [!seealso] 
> 
> 参考 [[操作系统：多线程概念]] 和 [[操作系统：进程调度]]
> 

当线程获得 CPU 时间片之后，CPU 中的栈指针寄存器(`%rsp`)、栈基寄存器(`%rbp`) 和 程序计数器(`%rip`) 等都要切换到对应的线程

![[Pasted image 20250707230534.png]]

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

> [!important] 
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

Go 对 Coroutine 做了非常多的优化，提出了 Goroutine。Goroution 可以被自由移动到不同的线程中执行
