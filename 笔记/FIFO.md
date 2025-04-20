# FIFO

在 [[PIPE]] 中介绍了 **无名管道**，和管道的一些基本特征。现在，我们介绍 FIFO，也称为有名管道，原因在于 FIFO 在文件系统中提供了一个名字，可以供任何进程打开

## FIFO

FIFO 也叫有名管道，是一种特殊的文件，它的路径名存在于文件系统中

> [!tip] 
>
> 由于 FIFO 的名字在文件系统中，因此任何进程都可以使用 `open()` 打开 FIFO。即使毫无关系的进程，也可以通过管道文件通信
> 
> 管道文件在磁盘上 **只有 $\text{i-node}$ 没有数据块**，也不保存数据
> 

打开 FIFO 方式与打开一个普通文件是一样的。这样就 **能够将 FIFO 用于非相关进程之间的通信**

下图展示了利用 FIFO 进行进程间通信的逻辑的通信逻辑

![[Pasted image 20241030104048.png]]

使用 `open()` 通过 FIFO 文件名打开 FIFO 后，就能在使用在 [[文件 IO：通用 IO 模型]] 中介绍的 `read()` `write()` `close()` 系统调用操作 FIFO。FIFO 的有 **读端** 和 **写端**，先写入 FIFO 的数据会被先读出来。FIFO 的名称由此而来：先入先出(First In First Out，FIFO)

> [!tip] 
> 
> 当所有引用 FIFO 的所有描述符都被关闭之后，所有未被读取的数据会被丢弃
> 

### 创建 FIFO 文件

创建 FIFO 文件可以使用 shell 命令 `mkfifo`

```shell
mkfifo [-m mode] pathname
```

> [!tip] 
> 
> `-m mode` 用于指定 FIFO 文件的权限为 `mode`，指定方式类似于 `chmod` 命令，参考 [[Linux：命令和 vim#类型和权限#修改权限]]
> 

系统调用 `mkfifo()` 也可以用于创建 FIFO 文件

```c
#include <sys/stat.h>

int mkfifo(const char *pathname, mode_t mode);
/* 成功返回 0，失败返回 -1*/
```

> [!tip] 参数 `pathname`：指定创建 FIFO 文件路径

> [!tip] 参数 `mode`：指定 FIFO 文件的权限
> 
> `mode` 参数的设置参考 [[文件属性#文件权限]]。FIFO 文件的权限也会被 `umask` 值进行掩码操作
> 

**一旦 `FIFO` 被创建，任何进程都能够打开它**，只要它能够通过常规的 [[文件属性#检查文件的访问权限|文件权限检测]]

### 打开 FIFO 文件

使用 `open()` 打开一个 FIFO 文件具备一些不寻常的语义

一般来讲，使用 `FIFO` 时唯一明智的做法是在 **两端** 分别设置一个 **读取进程** 和一个 **写入进程**。在默认情况下，打开一个 FIFO 以便读取数据(`open(fifopath, O_RDONLY)`) 将会 **阻塞** 直到另一个进程打开 FIFO 以写入数据(`open(fifopath, O_WRONLY)`) 为止。相应地，打开一个 FIFO 以便写入数据将会 **阻塞** 直到另一个进程打开 FIFO 以便读取数据为止

> [!tip] 
> 换句话说，**打开一个 FIFO 会同步读取进程和写入进程**。如果一个 FIFO 的另一端已经打开（可能是因为一对进程已经打开了 FIFO 的两端） ，那么 `open()` 调用会立即成功
> 

> [!attention] 
> 
> 在大多数 UNIX 实现（包括 Linux）上，**当打开一个 FIFO 时可以通过指定 `O_RDWR` 标记来绕过打开 FIFO 时的阻塞行为**。这样，`open()` 就会立即返回，但无法使用返回的文件描述符在 `FIFO` 上读取和写入数据
> 
> 这种做法破坏了 `FIFO` 的 `I/O` 模型，SUSv3 明确指出 **以 `O_RDWR` 标记打开一个 `FIFO` 的结果是未知的**，因此出于可移植性的原因，开发人员不应该使用这项技术
> 
> 对于那些 **需要避免在打开 FIFO 时发生阻塞的需求，`O_NONBLOCK` 标记提供了一种标准化的方法来完成这个任务**
> 

一旦打开了 FIFO 文件，就可以使用 `read()` `write()` 从 FIFO 中读取或写入数据

> [!tip] 
> 
> `read()` FIFO 时，如果 FIFO 中没有数据，就会阻塞，直到另一个进程向 FIFO 中写入至少 $1$ 字节数据
> 
> `write()` FIFO 时，如果写入的字节数小于或等于 `PIPE_BUF`，能够保证一次性写入 FIFO 中。否则，内核就会对数据进行分段。如果 FIFO 的缓冲区被填满，那么 `write()` 就会阻塞，直到另一个进程从 FIFO 中读取了数据 
> 

下面的示例演示了两个进程时如何通过 FIFO 进行通信的

```c title:fifo/producer.c
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "base.h"

int main(const int argc, char *argv[]) {
    if(argc != 2 || strcmp(argv[1], "--help") == 0) {
        usageErr("%s <fifo pathname>\n", argv[0]);
    }

    if(mkfifo(argv[1], 0644) == -1) {
        errExit("mkfifo %s error: ", argv[1]);
    }

    // 写入进程，以 O_WRONLY 方式打开 FIFO 文件
    const int fifo = open(argv[1], O_WRONLY);  // 阻塞，等待读端打开 FIFO
    if(fifo == -1) {
        errExit("open fifo %s error: ", argv[1]);
    }

    // 不断从标准输入中读取数据，并写入 FIFO 中
    while (true) {
        char buffer[BUFSIZ];
        // 从 STDIN 中读取数据
        const ssize_t bytes = read(STDIN_FILENO, buffer, BUFSIZ);
        if(bytes == -1 || bytes == 0) {
            break;
        }

        // 写入 FIFO
        if(write(fifo, buffer, bytes) < 0) {
            errExit("write fifo %s error: ", argv[1]);
        }
    }

    // 关闭管道
    if(close(fifo) == -1) {
        errExit("close fifo %s error: ", argv[1]);
    }
    // 删除 FIFO 文件
    if(unlink(argv[1]) == -1) {
        errExit("unlink fifo %s error: ", argv[1]);
    }

    return 0;
}
```

```c title:fifo/consumer.c
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "base.h"


int main(int argc, char *argv[]) {
    if(argc != 2 || strcmp(argv[1], "--help") == 0) {
        usageErr("%s <fifo pathname>\n", argv[0]);
    }

    // 读进程，以 O_RDONLY 打开 FIFO
    int fifo = open(argv[1], O_RDONLY);  // 阻塞，等待写端打开 FIFO
    if(fifo == -1) {
        errExit("open fifo %s error: ", argv[1]);
    }

    while (true) {
        char buf[BUFSIZ];
        // 从 FIFO 中读取数据
        const ssize_t bytes = read(fifo, buf, BUFSIZ);
        if(bytes == -1 || bytes == 0) {
          break;
        }
        // 写入标准输出
        if(write(STDOUT_FILENO, buf, bytes) < 0) {
            errExit("write to stdout error: ");
        }
    }

    // 关闭管道
    if(close(fifo) == -1) {
        errExit("close fifo %s error: ", argv[1]);
    }

    return 0;
}
```

### 使用 FIFO 实现全双工通信

![[Pasted image 20241030124434.png]]

进程 A 以 `O_WRONLY` 打开 `A2B FIFO`，进程 B 以 `O_RDONLY` 打开 `A2B FIFO`。这样进程 A 可以通过 `A2B FIFO` 向进程 B 发生消息

进程 A 和 以 `O_RDONLY` 打开 `B2A FIFO`，进程 B 以 `O_WRONLY` 打开 `B2A FIFO`。这样进程 B 可以通过 `B2A FIFO` 向进程 A 发生消息

> [!tip] 
> 
> 通过两个 FIFO 可以实现 A 端和 B 端的全双工通信
> 

但是，在编写代码的时候需要注意几个要点

> [!tip] 防止死锁
> 
> 进程 A 先以 `O_WRONLY` 方式打开 `A2B FIFO`，后以 `O_RDONLY` 的方式打开 `B2A FIFO` ；则进程 B 就必须先以 `O_RDONLY` 打开 `A2B FIFO`，然后再以 `O_WRONLY` 打开 `B2A FIFO`。否则，就会引发死锁
> 
> + 进程 A 先调用 `open("A2B FIFO", O_WRONLY)`，就会阻塞等待进程 B 调用 `open("A2B FIFO", O_RDONLY)`
> + 进程 B 先调用 `open("B2A FIFO", O_WRONLY)`，就会阻塞等待进程 A 调用 `open("B2A FIFO", O_RDONLU)`
> 
> 这样两个进程都在等待，从而导致 **死锁** 现象
> 

```c title:fifo/programA.c

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "base.h"

int main(int argc, char *argv[]) {
    // programA A2B_FIFO B2A_FIFO
    if (argc != 3 || strcmp(argv[1], "--help") == 0) {
        usageErr("%d A2B_FIFO B2A_FIFO", argv[0]);
    }

    // A2B_FIFO 以 O_WRONLY 打开
    int fd_a2b = open(argv[1], O_WRONLY);
    if (fd_a2b == - 1) {
        errExit("open %s error: ", argv[1]);
    }

    printf("%s opened\n", argv[1]);

    // B2A_FIFO 以 O_RDONLY 打开
    int fd_b2a = open(argv[2], O_RDONLY);
    if (fd_b2a == - 1) {
        errExit("open %s error: ", argv[2]);
    }
    printf("%s opened\n", argv[2]);

    close(fd_b2a);
    close(fd_a2b);
}
```

```c title:fifo/programB.c

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "base.h"

int main(int argc, char *argv[]) {
    // programB A2B_FIFO B2A_FIFO
    if (argc != 3 || strcmp(argv[1], "--help") == 0) {
        usageErr("%d A2B_FIFO B2A_FIFO", argv[0]);
    }

    // A2B_FIFO 以 O_RDONLY 打开
    int fd_a2b = open(argv[1], O_RDONLY);
    if (fd_a2b == - 1) {
        errExit("open %s error: ", argv[1]);
    }
    printf("%s opened\n", argv[1]);

    // B2A_FIFO 以 O_WRONLY 打开
    int fd_b2a = open(argv[2], O_WRONLY);
    if (fd_b2a == - 1) {
        errExit("open %s error: ", argv[2]);
    }
    printf("%s opened\n", argv[2]);

    close(fd_b2a);
    close(fd_a2b);
}
```

> [!attention] 
> 
> 如果在 `programA.c` 中先以 `O_WRONLY` 打开 `A2B_FIFO`，同时在 `programB.c` 中先以 `O_WRONLY` 打开 `B2A_FIFO`，就会造成死锁，因为
> 
> + `programA` 等待 `programB` 打开 `A2B_FIFO` 用于读
> + `programB` 等待 `programA` 打开 `B2A_FIFO` 用于读
> 
> `programA` 和 `programB` 都在相互等待，从而造成死锁
> 

之后，我们就可以让 `programA` 和 `programB` 分别从标准输入中读取信息，并传递给对方。这里，我们假设 `programA` 先向 `programB` 传递消息

```c fifo/programA.c

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "base.h"

int main(int argc, char *argv[]) {
    // programA A2B_FIFO B2A_FIFO
    if (argc != 3 || strcmp(argv[1], "--help") == 0) {
        usageErr("%d A2B_FIFO B2A_FIFO", argv[0]);
    }

    // A2B_FIFO 以 O_WRONLY 打开
    const int a2b = open(argv[1], O_WRONLY);
    if (a2b == - 1) {
        errExit("open %s error: ", argv[1]);
    }
    printf("%s opened\n", argv[1]);

    // B2A_FIFO 以 O_RDONLY 打开
    const int b2a = open(argv[2], O_RDONLY);
    if (b2a == - 1) {
        errExit("open %s error: ", argv[2]);
    }
    printf("%s opened\n", argv[2]);

    // programA 可以从 标准输入 中读取消息，并通过 A2B_FIFO 发送给 programB
    // programA 可以从 B2A_FIFO 中读取 programB 返回的消息，并写入标准输出
    while (true) {
        char buf[BUFSIZ] = {};
        // 假设 programA 先向 programB 发送消息
        ssize_t size = read(STDIN_FILENO, buf, BUFSIZ);
        if(size == - 1 || size == 0) {
            break;
        }
        if(write(a2b, buf, size) < 0) {
            errExit("write error");
        }

        // 接收 programB 发来的消息
        memset(buf, 0, BUFSIZ);
        size = read(b2a, buf, BUFSIZ);
        if(size == -1 || size == 0) {
            break;
        }
        if(write(STDOUT_FILENO, buf, size) < 0) {
            errExit("write error");
        }
    }
    close(b2a);
    close(a2b);
}
```

```c title:fifo/programB.c

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "base.h"

int main(int argc, char *argv[]) {
    // programB A2B_FIFO B2A_FIFO
    if (argc != 3 || strcmp(argv[1], "--help") == 0) {
        usageErr("%d A2B_FIFO B2A_FIFO", argv[0]);
    }

    // A2B_FIFO 以 O_RDONLY 打开
    const int a2b = open(argv[1], O_RDONLY);
    if (a2b == - 1) {
        errExit("open %s error: ", argv[1]);
    }
    printf("%s opened\n", argv[1]);

    // B2A_FIFO 以 O_WRONLY 打开
    const int b2a = open(argv[2], O_WRONLY);
    if (b2a == - 1) {
        errExit("open %s error: ", argv[2]);
    }
    printf("%s opened\n", argv[2]);
    // programB 可以从 A2B_FIFO 中读取 programA 发送来的消息，并写入标准输出
    // programB 可以从 标准输入 中读取消息，并通过 B2A_FIFO 发送给 programA
    while (true) {
        char buf[BUFSIZ] = {};
        // 假设 programA 先向 programB 发送消息
        // 接收 programA 发来的消息
        memset(buf, 0, BUFSIZ);
        ssize_t size = read(a2b, buf, BUFSIZ);
        if(size == -1 || size == 0) {
            break;
        }
        if(write(STDOUT_FILENO, buf, size) < 0) {
            errExit("write error");
        }

        size = read(STDIN_FILENO, buf, BUFSIZ);
        if(size == - 1 || size == 0) {
            break;
        }
        if(write(b2a, buf, size) < 0) {
            errExit("write error");
        }
    }

    close(b2a);
    close(a2b);
    return 0;
}
```

虽然，这个版本可以做到相互通信。但是，还存在一个问题；如果 `programA` 连续给 `programB` 发送多条消息，`programB` 只能接收第一条消息，就阻塞等待从标准输入中读取数据了。

![[fifo-problem.gif]]

现在，我们分析 `programB` 为什么只读取到 `programA` 发送过来的第一条消息。下图展示了 `programB` 中循环部分左的工作

![[Pasted image 20241030154021.png]]

> [!example] 
> 
> + 首先，`programB` 读取 `programA` 发送过来的一条消息，并写入 `STDOUT_FILENO`
> + 然后，`programB` 等待 `STDIN_FILENO` 的输入，从而阻塞
> 

因为读 `STDIN_FILENO` 时会阻塞，从而导致 `programB` 不能读取 `a2b` 中的数据。有两种方法来解决这样的问题。第一种使用 **非阻塞 IO**，马上就会介绍；第二中使用 **IO 多路复用**，监控这些文件描述符，那些文件描述符就绪后就立即处理

> [!seealso] 关于监控文件描述符，另见
> 
> [[IO 多路复用]] 
> 

## 非阻塞 IO

当一个进程打开一个 FIFO 的一端时，如果 FIFO 的另一端还没有被打开，那么该进程就会被阻塞。如果不希望打开 FIFO 被阻塞，可以在调用 `open()` 时指定 `O_NONBLOCK` 标记

```c
fd = open("fifopath", O_RDONLY | O_NONBLOCK);
if(fd == -1) {
	// 错误处理
}
```

如果 FIFO  **的另一端已经被打开**，那么 `O_NONBLOCK` 对 `open()` 调用 **不会产生任何影响**

只有当 FIFO 的 **另一端还没有被打开** 的时候 `O_NONBLOCK`  标记才会起作用，而具体产生的影响则依赖于打开 FIFO 是用于读取还是用于写入的

> [!tip] 
> 
> + 如果打开 FIFO 是为了 **读取**，并且 FIFO 的写入端当前未被打开，那么 `open()` 调用会立即成功。就像 FIFO 的另一端已经被打开一样
> + 如果打开 FIFO 是为了 **写入**，并且还 **没有打开 FIFO 的另一端来读取数据**，那么 `open()` 调用会失败，并将 `errno` 设置为 `ENXIO`
> 

也就是说，当 FIFO 的另一个端 **没有写者** 时 **打开一个 FIFO 以便读取数据** 是没有问题的，因为任何试图从 FIFO 读取数据的操作都 **不会返回任何数据**。但当试图**向 _没有读者_ 的 FIFO 中写入数据** 时将会导致 `SIGPIPE` 信号的产生以及 `write()` 返回 `EPIPE` 错误

下表总结了打开 FIFO 的目的和指定 `O_NONBLOCK` 标记的语义

![[Pasted image 20241030163715.png]]

> [!tip] 打开 FIFO 时使用 `O_NONBLOCK` 标记存在两个目的
> 
> + 允许单个进程打开一个 FIFO 的两端。进程首先打开 FIFO 的读端，然后打开 FIFO 的写端。打开 FIFO 的任何一端都需要指定 `O_NONBLOCK`
> 
> + 防止打开两个 FIFO 的进程之间产生死锁
> 

关于打开两个 FIFO 的进程之间参数死锁在 [[#FIFO#使用 FIFO 实现全双工通信]] 中以及介绍过了

> [!tip] 死锁
> 
> 当两个或多个进程中每个进程都因等待对方完成某个动作而阻塞时会产生死锁。如下图
> 
> ![[Pasted image 20241030164221.png]]
> + 进程 X 首先打开了 `FIFO A` 准备读数据，它需要等待进程 Y 打开 `FIFO A` 写入数据
> + 进程 Y 首先打开了 `FIFO B` 准备读数据，它需要等待进程 X 打开 `FIFO B` 写入数据
>   
> 进程 X 和进程 Y 都在等待对方完成打开 FIFO 的操作，从而产生死锁。解决这样的死锁，可以 **交换其中一个进程的打开 FIFO 的顺序**；此外，还可以指定 `O_NONBLOCK` 标记首先打开 FIFO 用于读取
> 

### 非阻塞 read 和 write

`O_NONBLOCK` 标记不仅影响了 `open()` 的语义，还会影响后续调用 `read()` 和 `write()` 的语义

下表对 FIFO 上的 `read()` 操作进行了总结，包括 `O_NONBLOC` 标记的作用。从一个包含 $p$ 字节的 FIFO 中读取 $n$ 字节的语义

![[Pasted image 20241030164801.png]]

> [!tip] 
> 
> 只有当 **没有数据** 并且 **写入端被打开** 时阻塞和非阻塞读取之间才存在差别。此时，普通的 `read()` 会阻塞，非阻塞 `read()` 会失败并设置 `errno` 为 `EAGAIN`
> 

当 `O_NONBLOCK` 标记与 `PIPE_BUF` 限制共同起作用时 `O_NONBLOCK` 标记 FIFO 写入数据的影响会变得复杂。下表对 `write()` 的行为进行了总结，向 FIFO 写入 $n$ 字节的语义

![[Pasted image 20241030165221.png]]

当据无法立即被传输时 `O_NONBLOCK` 标记会导致在一个管道或 `FIFO` 上的 `write()` 失败（错误是 `EAGAIN`）。这意味着当写入了 `PIPE_BUF` 字节之后，如果在管道或 `FIFO` 中没有足够的空间了，那么 `write()` 会失败，因为内核无法立即完成这个操作并且无法执行部分写入，否则就会破坏不超过 `PIPE_BUF` 字节的写入操作的原子性的要求数

当一次写入的数据量超过 `PIPE_BUF` 字节时，该写入操作无需是原子的。因此，`write()`会尽可能多地传输字节以充满管道或 `FIFO`，即，执行部分写

> [!tip] 执行部分写的情形下
> 
> `write()` 返回的值是实际传输的字节数，并且 **调用者随后必须要进行重试以写入剩余的字节**
>   

如果 FIFO 已经满了，从而导致哪怕连一个字节都无法传输了，那么 `write()` 会失败并返回 `-1` 并设置 `errno` 为`EAGAIN`

使用 **非阻塞 IO** 修改 [[#FIFO#使用 FIFO 实现全双工通信]]，让 `programA` 和 `programB` 任意一个都可以先发送消息

```c title:fifo/programA_nonblock.c

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "base.h"

int main(int argc, char *argv[]) {
    // programA A2B_FIFO B2A_FIFO
    if (argc != 3 || strcmp(argv[1], "--help") == 0) {
        usageErr("%d A2B_FIFO B2A_FIFO", argv[0]);
    }

    // B2A_FIFO 以 O_RDONLY 打开
    const int b2a = open(argv[2], O_RDONLY | O_NONBLOCK);
    if (b2a == - 1) {
        errExit("open %s error: ", argv[2]);
    }
    printf("%s read side opened\n", argv[2]);

    // A2B_FIFO 以 O_WRONLY 打开
    int a2b;
    while (true) {
        a2b = open(argv[1], O_WRONLY | O_NONBLOCK);
        if (a2b == - 1) {
            if(errno == ENXIO) {
                continue;
            }
            errExit("open %s error: ", argv[1]);
        }
        break;
    }
    printf("%s write side opened\n", argv[1]);

    // 将 STDIN_FILENO 设置为非阻塞
    int flags = fcntl(STDIN_FILENO, F_GETFL);
    flags |= O_NONBLOCK;
    fcntl(STDIN_FILENO, F_SETFL, flags);

    // programA 可以从 标准输入 中读取消息，并通过 A2B_FIFO 发送给 programB
    // programA 可以从 B2A_FIFO 中读取 programB 返回的消息，并写入标准输出
    char buf[BUFSIZ] = {};
    while (true) {
        // 两端可以随便那个先发送消息
        memset(buf, 0, BUFSIZ);
        // STDIN_FILENO 非阻塞的，没有数据将返回 -1。遇见 EOF 返回 0
        ssize_t size = read(STDIN_FILENO, buf, BUFSIZ);
        if(size == 0) {
            break;
        }
        
        if(size > 0 && write(a2b, "programA: ", sizeof("programA: ")) < 0) {
            errExit("write %s error: ", argv[1]);
        }
        if(size > 0 && write(a2b, buf, size) < 0) {
            errExit("write %s error: ", argv[1]);
        }

        // 接收 programB 发来的消息
        memset(buf, 0, BUFSIZ);
        // B2A_FIFO 非阻塞的。当 B2F_FIFO 被关闭并且没有数据，read 返回  0
        size = read(b2a, buf, BUFSIZ);
        if(size == 0) {
            break;
        }
        if(size > 0 && write(STDOUT_FILENO, buf, size) < 0) {
            errExit("write error");
        }
    }
    close(a2b);
    close(b2a);

    return 0;
}
```

```c title:fifo/programB_nonblock.c

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "base.h"

int main(int argc, char *argv[]) {
    // programB A2B_FIFO B2A_FIFO
    if (argc != 3 || strcmp(argv[1], "--help") == 0) {
        usageErr("%d A2B_FIFO B2A_FIFO", argv[0]);
    }

    // B2A_FIFO 以 O_WRONLY 打开
    int b2a;
    while (true) {
        b2a = open(argv[2], O_WRONLY | O_NONBLOCK);
        if (b2a == - 1) {
            if(errno == ENXIO) {
                continue;
            }
            errExit("open %s error: ", argv[2]);
        }
        break;
    }
    printf("%s write side opened\n", argv[2]);

    // A2B_FIFO 以 O_RDONLY 打开
    const int a2b = open(argv[1], O_RDONLY | O_NONBLOCK);
    if (a2b == - 1) {
        errExit("open %s error: ", argv[1]);
    }
    printf("%s read side opened\n", argv[1]);

    // 将标准输入设置为非阻塞
    int flags = fcntl(STDIN_FILENO, F_GETFL);
    flags |= O_NONBLOCK;
    fcntl(STDIN_FILENO, F_SETFL, flags);
    // programB 可以从 A2B_FIFO 中读取 programA 发送来的消息，并写入标准输出
    // programB 可以从 标准输入 中读取消息，并通过 B2A_FIFO 发送给 programA
    while (true) {
        char buf[BUFSIZ] = {};
        // 两端可以随便那个先发送消息
        ssize_t size = read(STDIN_FILENO, buf, BUFSIZ);
        if(size == 0) {
            break;
        }
        if(size > 0 && write(b2a, "programB: ", sizeof("programA: ")) < 0) {
            errExit("write %s error: ", argv[2]);
        }
        if(size > 0 && write(b2a, buf, size) < 0) {
            errExit("write error");
        }
        memset(buf, 0, BUFSIZ);
        size = read(a2b, buf, BUFSIZ);
        if(size == 0) {
            break;
        }
        if(size > 0 && write(STDOUT_FILENO, buf, size) < 0) {
            errExit("write error");
        }
    }

    close(b2a);
    close(a2b);
    return 0;
}
```

## 自定义协议


由于 FIFO 中的数据是字节流，**消息之间是没有边界**。这意味着当多条消息被递送到一个进程中时，发送者和接收者必须要 **约定某种规则来分隔消息**

> [!tip] 
> 
> 最简单的莫过于给 **每条消息后面都附加一个 _分隔符_**。该方法的缺点
> + 分隔符不能出现在消息中
> + 读进程需要从 FIFO 中逐字节扫描数据，已确定分隔字符
> 
> 下图展示了该方式在 FIFO 中的消息
> 
> ![[Pasted image 20241101095748.png]]
> 

> [!tip] 
> 
> 另一种方式就是对消息附加一个 **固定大小的头**，头中包含消息体的长度。这种形式要求读进程首先从 FIFO 中读取头，然后使用头中保存的消息体长度读取剩余消息
> 
> 下图展示了该方式在 FIFO 中的数据
> 
> ![[Pasted image 20241101100225.png]]
> 

> [!tip] 
> 
> 一种更简单的方式就是使用 **固定长度的消息**。该方法的优势就简单，但是存在下面缺陷
> + 浪费 FIFO 容量，因为短消息需要填充到固定的长度
> + 写入端写入的消息长度超过固定长度的消息，会导致读取端读取数据出现异常
> 
> 下图展示了该方式在 FIFO 中的消息结构
> 
> ![[Pasted image 20241101100616.png]]
> 

