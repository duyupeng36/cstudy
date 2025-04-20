# PIPE

**管道(PIPE) 是 UNIX 系统上最古老的 _进程间通信(IPC)_ 方法**，它在 20 世纪 70 年代早期 UNIX 的第三个版本上就出现了。管道为一个常见需求提供了一个优雅的解决方案：给定两个运行不同程序的进程，在 shell 中如何让一个进程的输出作为另一个进程的输入呢？

UNIX/Linux 提供两种类型管道：**PIPE** 和 **Named PIPE**

> [!tip] PIPE：**无名管道**，也称为管道
> 
> 管道可以用来在 **相关进程之间传递数据**。例如，父子进程之间
> 

对于不相关的进程，即独立允许的两个进程，它们之间想要通信，可以借助 Named PIPE

> [!tip] Named PIPE：**有名管道**，也称为 **FIFO**
> 
> FIFO 是 PIPE 概念的一个变体，它们之间的一个重要差别在于 **FIFO 可以用于任意进程间的通信**
> 
> 借助文件系统，FIFO 可以被任何进程使用
> 

## 概念

每个 shell 用户都对在命令中使用 **管道** 比较熟悉，如下面这个 **统计一个目录中文件的数目的命令** 所示

```shell
ls | wc -l
```

为执行上面的命令，shell 会创建了两个进程来分别执行 `ls` 和 `wc`

> [!tip] 
> 
> 创建进程来指向程序是通过使用 `fork()` 和 `exec()` 来完成的。详细参考 [[进程创建]] 和 [[程序执行]]
> 

下图展示了这两个进程是如何使用管道的

![[Pasted image 20241029232742.png]]

> [!tip] 
> 
> 管道与 [[数据结构：栈和队列#队列]] 的概念非常相似，一端用于写入，另一端用于读取。可以将管道看成是一个 _铅管_，**它允许数据从一个进程流向另一个进程**
> 

两个进程都连接到了管道上，这样写入进程(`ls`) 就将其标准输出(`fd 1`) 连接到了管道的写入端，读取进程(`wc`) 就将其标准输(`fd 0`) 连接到管道的读取端。使用 [[深入文件 IO#复制文件描述符]] 操作完成

> [!tip] 
> 
> 这两个进程并不知道管道的存在，它们只是从标准文件描述符中读取数据和写入数据
> 

### 通信方式

> [!tip] 单工：消息只能单向传输
> 
> 如果消息只能从 $A$ 端发送到 $B$ 端，而 $B$ 端不能发送到 $A$ 端
> 

> [!tip] 半双工：消息单向传输，但是可以反向
> 
> 消息或者从 $A$ 端发送到 $B$ 端，或者从 $B$ 端发送到 $A$ 端，它们不能同时进行
> 

> [!tip] 全双工：消息可以双向传输
> 
> 如果消息可以从 $A$ 端发送到 $B$ 端，同时也可以从 $B$ 端发送到 $A$ 端
> 

### 管道特征

PIPE 和 FIFO 具有相似的特征

> [!tip] 管道是一个 **字节流**
> 
> 字节流意味着管道中的消息不存在 **边界** 的概念。因此，读取数据的进程可以读取任意大小的数据块，而不管写入进程写入管道的数据块的大小是什么
> 
> **管道传递的数据是 _顺序的_**。读进程读取消息的顺序和写进程写入消息的顺序是一样的
> 
> 在管道中无法使用 `lseek()` 进行随机访问
> 
> 如果需要在管道中实现 **离散消息** 的概念，就不行 **自行定义数据传输协议**。如果确实要发送离散消息，可以选择其他的 IPC 机制，例如 [[System V 消息队列]] 等
> 

> [!tip] 从管道中读取数据
> 
> 如果 **读取管道中没有数据**，那么读进程会被 **阻塞** 直到写进程写入至少 $1$ 字节为止
> 
> 如果 **写入进程关闭了管道**，读进程在读取完管道中剩余字节后就会看到 `EOF`，即 `read()` 返回 $0$
> 

> [!tip] 写入不超过 `PIPE_BUF` 字节的操作是原子的
> 
> 如果 **多个进程写入同一个管道**，那么如果它们在一个时刻写入的数据量不超过  `PIPE_BUF` 字节，那么就可以确保写入的数据不会发生相互混合的情况
> 
> SUSv3 要求 `PIPE_BUF` 至少是 `_POSIX_PIPE_BUF`($512$)。实现应该定义 `PIPE_BUF`，通常在 `<limits.h>` 或者允许调用 `fpathconf(fd, _PC_PIPE_BUF)` 获取。不同的实现 `PIPE_BUF` 的值不同
> + FreeBSD 6.0 中，`PIPE_BUF` 的值为 $512$
> + Tru64 5.1 中，`PIPE_BUF` 的值为 $4096$
> + Solaris 8 中，`PIPE_BUF` 的值为 $5120$
> 
> **在 Linux 上，`PIPE_BUF` 的值为 $4096$**
> 
> 当写入管道的数据块的大小超过了 `PIPE_BUF` 字节，那么内核可能会将数据 **分割成几个较小的片段** 来传输，在读者从管道中消耗数据时再附加上后续的数据
> 
> + `write()` 调用会 **阻塞** 直到管道可以被写入为止
> 
> 如果 **有多个写入进程**，那么大数据块的写入可能会被分解成任意大小的段（可能会小于 `PIPE_BUF` 字节），并且可能会出现与其他进程写入的 **数据交叉** 的现象
> 
> 如果 **读进程关闭了通道**，此时 **写进程向管道写入数据** 时，内核会向写进程发送 `SIGPIPE` 信号从而导致写进程异常退出
> 

> [!tip] 管道是单向的
> 
> 管道中数据传递方向是单向的(**单工**)。管道的一段用于写入，另一端则用于读取
> 
> FIFO 是 **半双工** 的，在打开 FIFO 时确定消息传递的方向
> 
> 某些 UNIX/Linux 实现上，管道是双向的，但是这不符合 UNIX 任何标准
> 

> [!tip] 管道容量是有限的
> 
> **管道其实是一个在内核内存中维护的缓冲区**，这个缓冲区的存储能力是有限的。一旦被填满之后，后续向管道写入的操作就会被阻塞直到读进程从管道中移除数据
> 
> SUSv3 并没有规定管道的存储能力。在 Linux < 2.6.11 前，管道的存储能力与系统分页大小一样(x86-32 上是 $4096$ 字节)。**在 Linux >= 2.6.11，管道的存储能力是 $65536$ 字节** 
> 


> [!attention] 总结
> 管道是一个字节流，管道的消息没有边界
> 
> ---
> 默认情况下，从管道中读取数据时，如果管道中没有数据，`read()` 操作阻塞
> + 如果管道 **写入端已关闭**，`read()` 将管道中剩余数据读取完毕后， 立即返回 $0$
>  
> ---
> 写入管道时，如果写入数据小于或等于 `PIPE_BUF` 字节，那么 `write()` 操作是原子的。在 Linux 上，`PIPE_BUF` 为 $4096$
> + 如果一次写入超过 `PIPE_BUF` 字节，内核会分割写入的字节。如果 **管道文件的缓冲区** 中有数据，`write` 阻塞 
> + 如果 **读取端已关闭**，那么写入端调用 `write()` 写入数据时内核会向写入端发送 `SIGPIPE` 信号，从而导致进程异常退出 
> 
> ---
>
> 管道容量是有限的。Linux >= 2.6.11 管道的容量是 $65536$ 字节
> + **管道被填满之后，`write()` 会阻塞**，直到读进程将管道中的数据读走 
> 

## PIPE

`pipe()` 系统调用创建一个新管道

```c
#include <unistd.h>

int pipe(int filedes[2]);
/* 成功返回 0， 失败返回 -1*/
```

成功的 `pipe()`调用会在数组 `filedes` 中返回两个打开的文件描述符，分别表示 PIPE 的两端

> [!tip] 
> + `filedes[0]` 代表读取端，只读取数据
> + `filedes[1]` 代表写入端，只写入数据

> [!tip] 
> 
> 与所有文件描述符一样，可以使用 `read()` 和 `write()` 系统调用来在管道上执行 I/O 操作。一旦向管道的写入端写入数据之后立即就能从管道的读取端读取数据
> 

管道上的 `read()` 调用会读取的数据量为所请求的字节数与管道中当前存在的字节数两者之间较小的那个

> [!important] 
> 
> 管道中没有数据时调用 `read()` 会阻塞，直到另一个进程在管道的写入端写入数据
> 

也可以在管道上使用 `stdio` 函数(`printf()`、`scanf()`等)，只需要首先使用 `fdopen()` 获取一个与 `filedes` 中的某个描述符对应的文件流即可

> [!attention] 
> 
> 此时，需要注意 `stdio` 缓冲问题，参考 [[进程终止#fork() stdio 缓冲区和 _exit() 的交互]]
> 

下图描述了使用 `pipe()` 创建完管道之后的情况，其中调用进程通过文件描述符引用了管道的两端

![[Pasted image 20241104181048.png]]

在单个进程中管道的用途不多。一般来讲都是 **使用管道让两个进程进行通信**。为了让两个进程通过管道进行连接，在调用完 `pipe()` 之后可以调用 `fork()`。在 `fork()` 期间，**子进程会继承父进程的文件描述符的副本**

> [!important] 关闭未使用的文件描述符
> 
> 调用 `fork()` 之后，其中 **一个进程应该立即关闭管道的写入端** 的描述符，**另一个则应该关闭读取端** 的描述符
> 
> 如果父进程需要向子进程传输数据，那么它就会关闭管道的读取端的描述符  `filedes[0]`，而子进程就会关闭管道的写入端的描述符 `filedes[1]`
> 

如下图展示了使用管道来将数据从父进程传输到子进程

![[Pasted image 20241104181703.png]]

> [!error] 父子进程从同一个管道中读取数据会造成数据竞争
> 
> 造成数据竞争的主要原因还是 **无法得知父子进程的执行顺序**。如果父子进程同时读取同一个管道，此时就会出现数据竞争。如果管道中没有数据，还会出现死锁现象
> 
> 为了解决这种数据竞争，可以采用 **进程同步** 技术；或者 **创建两个管道**，在两个进程之间发送数据的两个方向上各使用一个
> 

### PIPE 允许相关进程间的通信

**PIPE** 可以用于任意两个或更多 **_相关进程_ 之间的通信**，只要在创建子进程的系列  `fork()` 调用之前 **通过一个共同的祖先进程创建 PIPE 即可**

> [!tip] 
> 
> **PIPE 可用于一个 _进程和其孙子进程_ 之间的通信**。第一个进程创建管道，然后创建子进程，接着子进程再创建第一个进程的孙子进程
> 
> **管道通常用于两个 _兄弟进程_ 之间的通信**，它们的 **父进程创建了管道**，然后 **创建两个子进程**
> 

### 关闭未使用 PIPE 文件描述符

关闭未使用 PIPE 文件描述符不仅仅是为了 **确保进程不会耗尽其文件描述符的限制**。此外，还有另外两个重要的原因：**让读者看到 EOF** 和 **让写者知道没有读者进程了**

#### 关闭读进程持有 PIPE 的写端

> [!tip] 为了让读者看到文件结束 EOF
> 
> 从 PIPE 中读取数据的进程会关闭其持有的 PIPE 的写入描述符，这样当其他进程完成输出并关闭其写入描述符之后，**读者就能够看到文件结束，即 EOF**
> 

如果读取进程 **没有关闭 PIPE 的写入端**，那么在其他进程关闭了写入描述符之后，读者也不会看到文件结束，即使它读完了管道中的所有数据。此时，如果在读就会发生阻塞

> [!summary] 
> 
> 由于 **读取进程没有关闭管道的写入端**，此时读取进程调用 `read()` 将会阻塞以等待数据，这是因为 **内核知道至少还存在一个 PIPE 的写入描述符打开着**，即 **读取进程自己打开了这个描述符(PIPE 的写入端)**
> 

#### 关闭写进程持有 PIPE 的读端

写入进程关闭其持有的 PIPE 的读取描述符是出于不同的原因。当一个进程试图向一个 PIPE 中写入数据但没有任何进程拥有该 PIPE 的打开着的读取描述符时，内核会向写入进程发送一个 `SIGPIPE` 信号

> [!tip] 
> 
> PIPE 没有打开的读取文件描述符，当进程向该 PIPE 写入数据时，内核会向进程发送 `SIGPIPE` 信号
> 

在默认情况下，这个信号会杀死一个进程。但进程可以捕获或忽略该信号，这样就会导致 PIPE 上的 `write()` 操作因 `EPIPE` 错误（已损坏的管道）而失败。收到 `SIGPIPE` 信号或得到 `EPIPE` 错误对于标示出 PIPE 的状态是有用的，这就是为何需要关闭 PIPE 的未使用读取描述符的原因

> [!warning] 关闭 PIPE 的未使用读取描述符的原因
> 
> 如果 **写入进程没有关闭 PIPE 的读取端**，那么即使在 **其他进程已经关闭了 PIPE 的读取端** 之后，**写入进程仍然能够向 PIPE 写入数据**，最后写入进程会将数据充满整个管道，后续的写入请求会被永远阻塞
> 

关闭 PIPE 未使用文件描述符的最后一个原因是 **只有当所有进程中 _所有_ 引用一个 PIPE 的文件描述符被 _关闭_ 之后才会销毁该管道以及释放该 PIPE 占用的资源** 以供其他进程复用。此时，管道中所有未读取的数据都会丢失。

> [!summary] 
> 
> PIPE 具有进程持久性，当所有进程中关于 PIPE 的文件描述符被关闭时，PIPE 对象就会被释放
> 

下面是使用 PIPE 的一个示例程序：在该示例中，父进程在一个操作中写入数据，子进程一小块一小块地从管道中读取数据

```c title:pipes/simple_pipe.c
#include <sys/types.h>
#include <sys/wait.h>

#include <unistd.h>

#include "base.h"

int main(int argc, char *argv[]) {
    if(argc < 2 || strcmp(argv[1], "--hlep") == 0) {
        usageErr("%s <message> ...", argv[0]);
    }

    int pfds[2];
    char buffer[BUFSIZ];
    ssize_t numRead;

    // 第一步：打开管道。其中，读端描述符 pfds[0]；写端描述符 pfds[1]
    if(pipe(pfds) == -1) {
        errExit("pipe: ");
    }
    // 第二步: 创建子进程
    switch (fork()) {
        case -1:
            errExit("fork: ");
        case 0: // 子进程
            // 子进程读取父进程传递过来的消息
            // 首先，关闭子进程持有 PIPE 的写端
            if(close(pfds[1]) == -1) {
                errExit("child: close write side: ");
            }
            // 然后，循环从管道读取数据
            for (;;) {
                numRead = read(pfds[0], buffer, BUFSIZ);
                if(numRead == -1) {
                    errExit("read: ");
                }
                if(numRead == 0) {
                    // 看到了 EOF
                    break;
                }
                // 处理部分写问题
                ssize_t totalWritten = 0;
                while (totalWritten < numRead) {
                    ssize_t numWritten = 0;
                    numWritten = write(STDOUT_FILENO, (char *)buffer + totalWritten, numRead - totalWritten);
                    if(numWritten == -1) {
                        errExit("child - failed write: ");
                    }
                    totalWritten += numWritten;
                }
            }
            write(STDOUT_FILENO, "\n", 1);
            if(close(pfds[0]) == -1) {
                errExit("child: close read side: ");
            }
            _exit(EXIT_SUCCESS);
        default: // 父进程
            // 关闭管道的读取段
            if(close(pfds[0]) == -1) {
                errExit("parent: close read side: ");
            }
            // 循环将命令行参数写入管道
            for(int i = 1; i < argc; i++) {
                // 处理部分写问题
                ssize_t totalWritten = 0;
                while (totalWritten < strlen(argv[i])) {
                    ssize_t numWritten = 0;
                    numWritten = write(pfds[1], (char *)argv[i] + totalWritten, strlen(argv[i]) - totalWritten);
                    if(numWritten == -1) {
                        errExit("parent - failed write: ");
                    }
                    totalWritten += numWritten;
                }
            }
            if(close(pfds[1]) == -1) {
                errExit("parent: close write side: ");
            }
            _exit(EXIT_SUCCESS);
    }
}
```

## PIPE 作为进程同步的方法

读进程在读取 PIPE 时，默认情况下，如果 PIPE 中没有数据，读进程将阻塞，直到写进程写入数据。根据 PIPE 的特性，可以使用 PIPE 来做相关进程之间的同步

为了执行同步，父进程在创建子进程之前构建一个PIPE。每个子进程会继承 PIPE 的写入端的文件描述符并在完成动作之后关闭这些描述符

当所有子进程都关闭了 PIPE 的写入端的文件描述符之后，父进程在 PIPE 上的 `read()` 就会返回 $0$。这时，父进程就可以做其他工作了

> [!important] 
> 
> 为了保证在所有子进程关闭 PIPE 的写入端之后，父进程可以从 `read()` 中返回 $0$。那么父进程关闭 PIPE 写入端就非常重要了。
> 

## 使用管道连接程序

当管道被创建之后，为管道的两端分配的文件描述符是可用描述符中数值最小的两个。由于在通常情况下，进程已经使用了 $\text{fd 0} \text{fd 1}, \text{fd 2}$ ，因此会为管道分配一些数值更大的描述符

那么如何才能使用管道连接两个程序（即从 `stdin` 读取和写入到 `stdout` 的程序）使得一个程序的 **标准输出被定向到管道中**，而另一个程序的 **标准输入则从管道中读取**？特别是如何在 **不修改程序本身的代码** 的情况下完成这项工作呢？

> [!tip] 
> 
> 完成上述任务的是 [[深入文件 IO#复制文件描述符]] 
> 

一般来说，会使用下面的代码来获得预期结果

```c
int pfds[2];

pipe(pfds);

/* 创建子进程 */

/* 父进程 */
close(STDOUT_FILENO);  // 关闭标准输出
dup(pfds[1]);  // 管道的写入端绑定在标准输出上

/* 子进程 */
close(STDIN_FILENO); // 关闭标准输入
dup(pfds[0]); // 管道的读取段绑定在标准输入上
```

这些调用的最终结果是进程的 **标准输出被绑定到了管道的写入端**。而对应的一组调用可以用来 **将进程的标准输入绑定到管道的读取端** 上

上述代码依赖于  $\text{fd 0}, \ \text{fd 1},\ \text{fd 2}$ 这三个文件描述符已经打开。如果执行上述代码时 $\text{fd 0}$ 已关闭，那么 `dup(pfds[1])` 将绑定到标准输入上，那么就会错误地将进程的标准输入绑定到管道的写入端上。为了避免这样的情况，使用 `dup2()` 替代

```c
dup2(pfds[1], STDOUT_FILENO); // 关闭 fd 1, 然后将 pfds[1] 复制到 fd 1 的位置
```

由于未使用的管道文件描述符应该被关闭，因此在 `dup2()` 调用之后需要关闭多余的描述符

```c
close(pfds[1]);
```

前面给出的代码依赖于标准输出在之前已经被打开这个事实。假设在 `pipe()` 调用之前，标准输入和标准输出都被关闭了。那么在这种情况下，`pipe()` 就会给管道分配这两个描述符，即 `pfds[0]` 的值可能为 $0$，`pfds[1]` 的值可能为 $1$。其结果是前面的 `dup2()` 和 `close()` 调用将下面的代码等价

```c
dup2(1, 1);  // dup2(pfds[1], STDOUT_FILENO)
close(1);  // close(pfds[1])
```

因此按照防御性编程实践的要求最好将这些调用放在一个 `if` 语句中，如下所示

```c
if(pfds[1] != STDOUT_FILENO) {
	dup2(pfds[1], STDOUT_FILENO);
	close(pfds[1]);
}
```

如下示例程序：使用管道连接 `ls` 和 `wc`

```c title:pipes/pipe_ls_wc.c

#include <base.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {
    int pfds[2];
    if(pipe(pfds) == -1) {
        errExit("pipe error: ");
    }

    switch (fork()) {
        case -1:
            errExit("fork error: ");
        case 0: // 子进程，执行 exec("ls", "ls", nullptr)。将 ls 的输入写入管道
            // 第一步：关闭管道的读取端
            if (close(pfds[0]) == -1) {
                errExit("close error: ");
            }

            // 将 ls 的标准输出连接到 pipe 的写入端 pfds[1]
            if(pfds[1] != STDOUT_FILENO) {
                if(dup2(pfds[1], STDOUT_FILENO) ==-1) {
                    errExit("dup2 error: ");
                }
                if(close(pfds[1]) == -1) {
                    errExit("close error: ");
                }
            }
            execlp("ls", "ls", nullptr);
            errExit("execlp error: ");
        default: // 父进程
            break;
    }
    // 父进程代码。创建另一个子进程，用于执行 exec("wc", "wc", nullptr)
    switch (fork()) {
        case -1:
            errExit("fork error: ");
        case 0: // 另一个子进程，执行 exec("wc", "wc", nullptr)
            // 第一步。关闭管道的写入端
            if(close(pfds[1]) == -1) {
                errExit("close error: ");
            }
            //第二步：将 wc 的标准输入连接到管道读取端
            if(pfds[0] != STDIN_FILENO) {
                if(dup2(pfds[0], STDIN_FILENO) ==-1) {
                    errExit("dup2 error: ");
                }
                if(close(pfds[0]) == -1) {
                    errExit("close error: ");
                }
            }
            execlp("wc", "wc", "-l", nullptr);
            errExit("execlp error: ");
        default:
            break;
    }
    // 父进程

    // IMPORTANT: 父进程需要关闭不需要的文件描述符
    if(close(pfds[0]) == -1) {
        errExit("close error: ");
    }
    if(close(pfds[1]) == -1) {
        errExit("close error: ");
    }

    // 等待子进程结束
    pid_t childPid;
    int status;
    for(;;) {
        childPid = wait(&status);
        if(childPid == -1) {
            if(errno == ECHILD) {
                break;
            }
            errExit("wait error: ");
        }
        if(WIFEXITED(childPid)) {
            printf("child[PID %ld] exit normal: %d\n", (long)childPid, WEXITSTATUS(status));
        } else if(WIFSIGNALED(status)) {
            printf("child[PID %ld] killed by signal %d(%s)\n", (long)childPid, WTERMSIG(status), strsignal(WTERMSIG(status)));
        } else {
            printf("what happend to this child? (status=%x)\n", (unsigned int)status);
        }
    }
    return 0;
}
```

上述代码运行的结果为

```shell
➜  build ls | wc -l
56
➜  build ./pipe_ls_wc
56
what happend to this child? (status=0)
what happend to this child? (status=0)
```

## 通过 PIPE 与 shell 命令进行通信

管道的一个常见用途是执行 shell 命令并读取其输出或向其发送一些输入。标准库函数 `popen()` 和 `pclose()` 简化了上述任务

```c
#include <stdio.h>

FILE *popen(const char * command, const char *mode);
/* 成功返回一个流；失败返回 NULL */

int pclose(FILE *stream);
/* 成功返回子进程退出状态; 失败返回 -1 */
```

`popen()` 函数创建了一个管道，然后创建了一个子进程来执行 shell，而 shell 又创建了一个子进程来执行 `command` 字符串

参数 `mode` 确定调用进程是从管道中读取数据(`mode` 是 `r`) 还是将数据写入到管道中(`mode` 是 `w`)。`mode` 的取值确定了所执行的 **命令的标准输出** 是连接到管道的 **写入端** 还是将其标准输入连接到管道的 **读取端**

![[Pasted image 20241106142605.png]]

> [!tip] 参数 `mode`：指定调用进程对管道的操作
> + `"w"`：调用进程持有管道的写端，子进程持有管道的读端
> + `"r"`：调用进程持有管道的读端，子进程持有管道的写端
