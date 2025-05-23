# 进程间通信简介

在同一个虚拟内存地址，在不同的进程中，会被映射到不同的物理内存区域，因此在多个进程之间通过虚拟地址交换数据是不可能完成的

鉴于进程之间天然的 **内存壁垒**，想要实现多个进程间交换数据，就必须提供一种专门的机制，也就是所谓的 **进程间通信机制(IPC)**

UNIX 系统上各种 **通信** 和 **同步** 工具，并根据功能将它们分成了三类

- **通信**：这些工具 **关注进程之间的数据交换**
- **同步**：这些进程 **关注进程和线程操作之间的同步**
- **信号**：尽管信号的主要作用并不在此，但在特定场景下仍然可以将它作为一种同步技术。更罕见的是信号还可以作为一种通信技术：**信号编号本身是一种形式的信息**，并且 **可以在实时信号上绑定数据（一个整数或指针）**

下图展示了这些 IPC 工具

![[Pasted image 20241106000843.png]]

> [!tip] 
> 
> 尽管其中一些工具关注的是同步，但通用术语进程间通信通常指代所有这些工具
> 

## 通信工具

 通信工具 **允许进程间相互交换数据**。可以将通信工具分成两类：**数据传输工具** 和 **共享内存**

> [!tip] 数据传输工具
> 
> 数据传输工具有多种，区分这些工具的关键因素是 **写入** 和 **读取** 的概念
> 
> 为了进行通信，一个进程将数据写入到 IPC 工具中，另一个进程从中读取数据。这些工具要求在用户内存和内核内存之间进行两次数据传输：
> + 一次传输是在写入的时候从用户内存到内核内存
> + 另一次传输是在读取的时候从内核内存到用户内存
> 
> 管道是数据传输工具的一种，如下图所示
> 
> ![[Pasted image 20241106085728.png]]
> 

> [!tip] 共享内存
> 
> 共享内存允许进程通过将数据放到由 **进程间共享的一块内存** 中以完成信息的交换。内核通过将 **每个进程中的页表条目指向同一个 RAM 分页** 来实现这一功能
> 
> 一个进程可以通过将 **数据放到共享内存块中** 使得其他进程读取这些数据。由于通信 **无需系统调用**，也 **无需用户内存和内核内存之间的数据拷贝**，因此共享内存的速度非常快
> 

### 数据传输

可以进一步将数据传输工具分成 $3$ 个类别：

> [!tip] 字节流：[[Linux 系统编程：PIPE]] [[Linux 系统编程：FIFO]] 和 **流式 Socket** 交换的数据是一个 **无分隔符的字节流**
> 
> 每个读取操作可能会从 IPC 工具中 **读取任意数量的字节**，不管写者写入的块的大小是什么。**这个模型参考了传统的 UNIX _文件是一个字节序列_ 模型**
> 

> [!tip] 消息：[[Linux 系统编程：System V 消息队列]] [[POSIX 消息队列]] 和 **数据报 Socket** 交换的数据是以 **分隔符分隔的消息**
> 
> 每次读取都会将写者写入的 **整条消息完整的读取出来**
>
> 不能只读取消息的一部分；一次读取操作也不能读取多条消息
>

> [!tip] 伪终端
> 
> [[伪终端]] 是一种在特殊情况下使用的通信工具
> 

数据传送工具的 **读取操作具有破坏性**，每次读取都会消耗数据。**其他进程将无法获取所消耗的数据**

> 在 socket 中可以使用 `MSG_PEEK` 标记来执行非破坏性读。socket 允许将一条消息广播或组播到多个接收者处

**读者和写者进程之间的同步是原子的**。如果一个读取者试图从一个当前不包含数据的数据传输工具中读取数据，那么在默认情况下读取操作会被阻塞直至一些进程向该工具写入了数据

> [!summary] 
> 
> + 数据传送工具的 **读取操作具有破坏性**。读取操作会消耗数据，从而导致其他进程无法获取被消耗的数据
> + 读者从没有包含数据的数据传输工具中读取数据时，默认阻塞直到写者写入数据
> 

### 共享内存

大多数现代 UNIX 系统提供了三种形式的共享内存：[[Linux 系统编程：System V 共享内存]] [[POSIX 共享内存]] 以及 [[Linux 系统编程：内存映射]]

共享内存可以在多个进程之间共享同一个数据结构。然而，当 **一个进程正在更新共享内存** 中的一个数据结构时，**另一个进程就不应该试图读取** 这个数据结构

> [!tip] 
> 
> 尽管共享内存的通信速度更快，但速度上的优势是用来弥补需要对在共享内存上发生的操作进行同步的不足的。在共享内存中，**信号量** 通常用来作为同步方法
> 

**放入共享内存中的数据对所有共享这块内存的进程可见**


## 同步工具

同步工具可以 **协调进程的操作**。通过同步可以防止多个进程同时更新同一个内存块中的同一个数据结构

> [!tip] 
> 
> 例如，当两个进程同时更新一块共享内存；或者同时更新文件的同一个数据块
> 

如果没有同步，那么这种同时更新的操作可能会导致应用程序产生错误的结果

UNIX 系统提供了同步工具有：**信号量** **文件锁** 以及 **互斥量** 和 **条件变量**

> [!tip] 信号量：由内核维护的整数，其值永远不会小于 $0$。一个进程可以 **增加** 或 **减小** 一个信号量的值
> 
> 如果一个进程试图将信号量的值 **减小到小于  $0$**，那么内核会 **阻塞该操作** 直至信号量的值增长到允许执行该操作的程度
> 
> 进程可以要求执行一个 **非阻塞操作**，那么就不会发生阻塞，内核会让该操作 **立即返回** 并返回一个 **标示无法立即执行该操作的错误**
> 
> 信号量的含义由程序确定。最常用的信号量是 **二值信号量**，信号量的值只能取 $0$ 或 $1$，这个种信号量是为了 **预约对共享资源的独占访问**
> 
> 如果同种共享资源有多个，那么信号量的最大值就应该与资源数量相等
> 
> 参考 [[Linux 系统编程：System V 信号量]] [[POSIX 信号量]] 
> 

> [!tip] 文件锁：用于协调多个进程同时操作同一个文件
> 
> 文件锁分为两类：**读（共享）锁** 和 **写（互斥）锁**
> 
> 任意进程都可以持有同一文件（或一个文件的某段区域）的读锁，但当一个进程持有了一个文件（或文件区域）的写锁之后，其他进程将无法获取该文件（或文件区域）上的读锁和写锁
> + 读锁：任意进程均可以持有
> + 写锁：只能被一个进程持有。当一个进程持有写锁之后，其他进程不能持有读锁和写锁
> 
> 参考 [[Linux 系统编程：文件锁]]
> 

> [!tip] 互斥量和条件变量：通常用于线程同步

## IPC 工具比较

要访问一个 IPC 对象，进程必须要通过某种方式来标识出该对象，一旦将对象“打开”之后，进程必须要使用某种句柄来引用该打开着的对象

下表展示了各种 IPC 工具的标识符和句柄

![[Pasted image 20241106102444.png]]

各种 IPC 工具在功能上是存在差异的，因此在确定使用何种工具时需要考虑这些差异。下面首先对数据传输工具盒共享内存之间的差异进行总结

> [!tip] 数据传送工具
> 
> 数据传送工具提供了读取和写入操作，**传输的数据只供一个读者进程消耗**
> 
> 内核会自动处理读者和写者之间的同步
> 

> [!tip] 共享内存
> 
> 通过共享内存能够使数据对共享同一内存区域的所有进程可见
> 
> 通信操作非常简单，就像访问虚拟地址那样访问共享内存。但是，同步处理会增加共享内存设计的复杂性
> 

IPC 工具重要特性：**持久性** 和 **权限模型** 

> [!tip] 权限模型
> 
> 控制着哪些进程能够访问的 IPC 对象
> 

> [!tip] 持久性
> 
> IPC 工具的声明周期。持久性有 $3$ 类
> + 进程持久性：只要有一个进程持有 IPC 对象，那么 IPC 对象就不会被销毁。例如，[[Linux 系统编程：PIPE]] 和 [[Linux 系统编程：FIFO]]
> + 内核持久性：当显示调用删除 IPC 对象的系统调用时，IPC 对象才会被销毁
> + 文件系统持久性：保持在文件系统中，需要操作文件系统才能销毁 IPC 对象
> 

下表终结了 IPC 对象的权限模型和持久性

![[Pasted image 20241106103543.png]]
