# 深入文件 IO

## 原子操作和竞争条件

所有系统调用都是以 **原子操作** 方式执行的

> [!tip] 
> 
> 指内核保证了某系统调用中的所有步骤会作为独立操作而一次性加以执行，其间不会为其他进程或线程所中断
> 

> [!tip] 原子操作：**最小的不可分割的操作**
> 
> 系统调用以原子操作执行：不会被其他进行或线程中断
> 

原子性是某些擦着得以圆满成功的关键，它规避的 **竞争状态**，也称 **竞争冒险**

> [!tip] 竞争状态：操作结果取决于进程的执行顺序
> 
> 操作共享资源的两个进程（或线程），其结果取决于一个无法预期的顺序，即这些进程获得 CPU 使用权的先后相对顺序
> 

### 文件 IO 的竞争状态：以独占的方式打开文件

在 [[文件 IO：通用 IO 模型]] 中我们介绍过 `open()` 系统调用的 `O_CREAT` 和 `O_EXCL`，它们两个配合使用时，如果文件已然存在，则 `open()` 将返回一个错误。这种机制 **保证进程是打开文件的创建者**

> [!tip] 
> 
> 对文件的 **存在性检查** 和 **创建文件** 属于同一原子操作
> 

如果我们使用 `O_EXCL`，我们可能会写出下面的 **错误代码**

```c

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

int main(int argc, char *argv[]) {

    if(argc != 2) {
        fprintf(stderr, "Usage: %s filename\n", argv[0]);
        return EXIT_FAILURE;
    }

    int fd = open(argv[1], O_WRONLY);  // 第一次打开：检查文件是否存在

    if(fd != -1) {  // 文件打开成功
        printf("[PID %ld] File: \"%s\" already exists\n", (long)getpid(), argv[1]);
        close(fd);
    } else { // 文件打开失败
        // ENOENT：文件或目录不存在
        if(errno != ENOENT) {
            // 不是因为 文件或目录不存在导致的错误
            fprintf(stderr, "open: %s\n", strerror(errno));
            return EXIT_FAILURE;
        }
        // 文件或目录不存在  
		printf("[PID %ld] File \"%s\" doesn't exist yet\n", (long) getpid(), argv[1]);
        // 第二次打开: 创建文件
        fd = open(argv[1], O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR);
        if(fd == -1) {
            fprintf(stderr, "open: %s\n", strerror(errno));
            return EXIT_FAILURE;
        }
        printf("[PID %ld] file \"%s\" exclusively\n",(long)getpid(), argv[1]);
    }
    return EXIT_SUCCESS;
}
```

上述代码除了要调用 `open()` 两次外，还潜伏着一个 `bug`：**当第一次调用 `open()` 时，希望打开的文件还不存在，而当第二次调用 `open()` 时，其他进程已创建了该文件**

如下图所示，若内核调度器判断出分配给 **进程A** 的时间片已经耗尽，并将 CPU 使用权交给 **进程B**，就可能会出现这个 `bug`。如两个进程在一个多 CPU 系统上同时运行时，也会出现这种情况

![[Pasted image 20241014211252.png]]

> [!tip] 
> 
> 在这一场景下，进程 A 将得出错误的结论：目标文件是由自己创建的。因为无论目标文件存在与否，进程 A 对 `open()` 的第二次调用都会成功
> 

为了模拟上述结果，我们在处理文件不存在之前先暂停 $5$ 秒中，模拟耗时操作

```c
// 文件或目录不存在
printf("[PID %ld] File \"%s\" doesn't exist yet\n", (long) getpid(), argv[1]);
sleep(5);
printf("[PID %ld] Done sleeping\n", (long)getpid());
```

现在，编译运行该程序

```c
$ ./bad_exclusive_open tfile sleep & ./bad_exclusive_open tfile
[1] 1092384
[PID 1092385] File "tfile" doesn't exist yet
[PID 1092385] file "tfile" exclusively
[PID 1092384] File "tfile" doesn't exist yet
[PID 1092384] Done sleeping
[PID 1092384] file "tfile" exclusively

[1]+  Done                    ./bad_exclusive_open tfile sleep
```

> [!summary] 
> 
> 由于第一个进程在检查文件是否存在和创建文件之间发生了中断，造成两个进程都声称自己是文件的创建者。结合 `O_CREAT` 和 `O_EXCL` 标志来一次性地调用 `open()` 可以防止这种情况，因为这确保了检查文件和创建文件的步骤属于一个单一的原子操作
> 

### 文件 IO 的竞争状态：向文件尾部追加数据

多个进程同时向同一个文件尾部添加数据（例如，全局日志文件）。为了达到这一目的，也许可以考虑在每个写进程中使用如下代码

```c
if (lseek(fd, 0, SEEK_END) == -1)
{
	// 错误处理
}

if(write(fd, buf, len) != len)
{
	// 错误处理
}
```

如果第一个进程执行到 `lseek()` 和 `write() `之间，被执行相同代码的第二个进程所中断，那么这两个进程会在写入数据前，将文件偏移量设为相同位置，而当第一个进程再次获得调度时，会覆盖第二个进程已写入的数据。此时再次出现了竞争状态，因为执行的结果依赖于内核对两个进程的调度顺序

> [!tip] 
> 
> 要规避这一问题，需要将 **文件偏移量的移动** 与 **数据写操作** 纳入同一原子操作。在打开文件时加入 `O_APPEND` 标志就可以保证这一点
> 

## fcntl

`fcntl()` 系统调用对一个打开的文件描述符执行一系列控制操作

```c
#include <fcntl.h>

int fcntl(int fd, int cmd, ...);
/* 成功返回 cmd，错误返回 -1 */
```

`cmd` 参数所支持的操作范围很广；将在后续介绍

`fcntl()` 的第三个参数以省略号来表示，这意味着可以将其设置为不同的类型，或者加以省略

> [!tip] 
> 
> 内核会依据 `cmd` 参数的值来确定该参数的数据类型
> 

下面我们会介绍 $3$ 个 `cmd` 

| `cmd`     | 描述         |
| :-------- | ---------- |
| `F_GETFL` | 获取 `flags` |
| `F_SETFL` | 设置 `flags` |
| `F_DUPFD` | 复制文件描述符    |

## 打开文件的状态标志

`fcntl()` 的用途之一是针对一个打开的文件，获取或修改其 **访问模式** 和 **状态标志**

> [!tip] 
> 
> 访问模式和状态标志是通过指定 `open()` 调用的 `flag` 参数来设置的
> 

> [!important] 
> + 将 `fcntl()` 的 `cmd` 参数设置为 `F_GETFL`，用于 **获取** 访问模式 和 状态标志
> 
> + 将 `fcntl()` 的 `cmd` 参数设置为 `F_SETFL`，用于 **设置**  访问模式 和 状态标志
> 

> [!tldr] 
> 针对一个打开的文件，只有通过 `open()` 或后续` fcntl()` 的 `F_SETFL` 操作，才能对该文件的状态标志进行设置
> 
> Linux 实现与标准有所偏离：如果一个程序编译时采用了 **打开大文件技术**，那么当使用 `F_GETFL` 命令获取文件状态标志时，**标志中将总是包含 `O_LARGEFILE` 标志**
> 

### 获取打开文件的状态标志

下面的代码用于 检查文件是否以 _同步方式_(`O_SYNC` 状态) 打开

```c
#include <stdio.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s file\n", argv[0]);
        return 1;
    }

    int fd;

    if (argc > 2) {
        // 以同步方式打开
        fd = open(argv[1], O_RDWR | O_SYNC);
    } else {
        fd = open(argv[1],O_RDWR);
    }

    if (fd == -1) {
        perror("open");
        return 1;
    }

    printf("Open file success\n");

    int flags;
    // 获取 flags
    flags = fcntl(fd, F_GETFL);
    if (flags == -1) {
        perror("fcntl");
        return 1;
    }

    // 判断是否以 O_SYNC 打开文件 argv[1]
    if (flags & O_SYNC) {
        printf("file: %s O_SYNC is on\n", argv[1]);
    } else {
        printf("file: %s O_SYNC is off\n", argv[1]);
    }
    close(fd);
    return 0;
}
```

### 获取打开文件的访问标志


判定文件的访问模式有一点复杂，这是因为 `O_RDONLY(0)`、`O_WRONLY(1)` 和 `O_RDWR(2)` 这 $3$ 个常量并不与打开文件状态标志中的单个比特位对应

> [!tip]
> 
> 在 `<fcntl.h>` 头文件中，这 $3$ 常量定义如下
> 
> ```c
> #define O_RDONLY	     00
> #define O_WRONLY	     01
> #define O_RDWR		     02
> ```
> 
> **读** 和 **写** 这两种访问模式都有 $2$ 个访问标志
> 

因此，要判定访问模式，需使用掩码 `O_ACCMODE`

```c
#define O_ACCMODE	   0003
```

将 `O_ACCMODE` 与 `flag` 相与，将结果与 $3$ 个常量进行比对。如下示例代码

```c
accessMode = flags & O_ACCMODE;
if(accessMode == O_WRONLY || accessMode == O_RDWR) {
	printf("file is writeable")
}
```

### 修改状态标志

可以使用 `fcntl()` 的 `F_SETFL` 命令来修改打开文件的某些状态标志

> [!important] 
> 
> 允许更改的标志有 `O_APPEND` `O_NONBLOCK` `O_NOATIME` `O_ASYNC` 和 `O_DIRECT`。系统将忽略对其他标志的修改操作
> 
> 有些其他的 UNIX 实现允许 `fcntl()` 修改其他标志，如 `O_SYNC`
> 

使用 `fcntl()` 修改文件状态标志，尤其适用于下面两个场景

> [!hint] 
> 
>  **文件不是由调用程序打开的**，所以程序也无法使用 `open()` 调用来控制文件的状态标志。例如，文件是 $3$ 个标准输入输出描述符中的一员，这些描述符在程序启动之前就被打开
> 
> **文件描述符的获取是通过 `open()` 之外的系统调用**。比如 `pipe()` 调用，该调用创建一个管道，并返回两个文件描述符分别对应管道的两端。再比如 `socket()` 调用，该调用创建一个套接字并返回指向该套接字的文件描述符
> 

为了修改打开文件的状态标志，可以使用 `fcntl()` 的 `F_GETFL` 命令来获取当前标志的副本，然后修改需要变更的比特位，最后再次调用 `fcntl()` 函数的 `F_SETFL` 命令来更新此状态标志。因此，为了添加 `O_APPEND` 标志，可以编写如下代码

```c
int flags;

flags = fcntl(fd, F_GETFL);
if (flags == -1)
{
	// 错误处理
}

flags |= O_APPEND;  // 添加 O_APPEND

if(fcntl(fd, F_SETFL, flags) == -1)
{
	// 错误处理
}
```

## 文件描述符和打开文件的关系

目前，我们接触的代码中，文件描述符和文件似乎是一一对应的关系。然而，在 [[文件系统的实现]] 中。我们说过，**进程级打开文件表** 中的条目指向的是 **系统级打开文件表**中 条目

> [!tip] 
> 
> 进程级打开文件表，也称为 **打开文件描述符表**。而进程使用的文件描述符就是文件描述符表的索引
> 
> 也就说，进程使用的文件描述符是一个小整数
> 

Unix/Linux 系统内核  维护的 $3$ 个数据结构：进程级打开文件表，系统级打开文件表 和 文件系统的 $\text{i-node}$ 表

> [!tip] 进程级文件描述符表
> 针对每个进程，内核为其维护 **打开文件的描述符表**。该表的每一条目都记录进程打开文件的文件描述符的相关信息。这些信息包括
> + 控制文件描述符操作的一组标志
> + 对系统级打开文件表中一项的引用
> 

> [!tip] 系统级打开文件表
> 
> 内核对所有打开的文件维护有一个 **系统级的打开文件表**，其中的一项称为 **打开文件句柄**，每个句柄都存储了一个打开文件相关的全部信息
> + 当前文件偏移量
> + 打开文件时所使用的状态标志
> + 文件访问模式
> + 与信号驱动 I/O 相关的设置
> + 对该文件 $\text{i-node}$ 对象的引用
> 

> [!tip] $\text{i-node}$ 表
> 
> 每个文件系统都会为驻留其上的所有文件建立一个 $\text{i-node}$ 表。每个文件的 $\text{i-node}$ 信息，具体如下
> 
> + 文件类型
> + 访问权限
> + 文件锁列表指针
> + 文件的各种属性
> 

现在请忽略 $\text{i-node}$ 在磁盘和内存中的表示差异。**磁盘上的 $\text{i-node}$** 记录了文件的固有属性，诸如：文件类型、访问权限和时间戳。访问一个文件时，会在 **内存中为 $\text{i-node}$ 创建一个副本**，其中记录了引用该 $\text{i-node}$ 的打开文件句柄数量以及该 $\text{i-node}$ 所在设备的主、从设备号，还包括一些打开文件时与文件相关的临时属性

下图展示了两个进程的 **进程级文件描述符表** **系统级打开文件表** **内存中 $\text{i-node}$ 的副本** 的关系

![[Pasted image 20241015143221.png]]


进程 A 的 `fd 1` 和 `fd 20` 指向同一个系统级打开文件表项 $23$，这可能是通过 **复制文件描述符** 形成的
 
进程 A 的 `fd 2` 和 进程 B 的 `fd 2` 指向同一个系统打开文件表项 $73$， 这种情形可能在调用 `fork()` 后出现：**进程 A 与进程 B 之间是父子关系**
 
进程 A 的 `fd 0` 和进程 B 的 `fd 3` 分别指向不同的打开文件句柄，但这些句柄均指向 **i-node** 表中的相同条目，换言之，**指向同一文件**。发生这种情况是因为每个 **进程各自对同一文件发起了 `open()` 调用**

> [!summary] 
> 
>  **两个不同的文件描述符，若指向同一打开文件句柄**，将共享同一文件偏移量。因此，如果通过 **其中一个文件描述符来修改文件偏移量**（由调用 `read()`、`write()` 或 `lseek()` 所致），那么 **从另一文件描述符中也会观察到这一变化**。无论这两个文件描述符分属于不同进程，还是同属于一个进程，情况都是如此
> 
> 要获取和修改打开的文件标志（例如，`O_APPEND`、`O_NONBLOCK` 和 `O_ASYNC`），可执行 `fcntl()` 的 `F_GETFL` 和 `F_SETFL` 操作，其对作用域的约束与上一条颇为类似
> 
> **文件描述符状态标志**（亦即，`close-on-exec` 标志）**为进程和文件描述符所私有**。对这一标志的修改将不会影响同一进程或不同进程中的其他文件描述符
> 

## 复制文件描述符

Bash shell 的 IO 重定向语法 `2>&1`，意在通知 `shell` 把标准错误(`fd 2`)重定向到标准输出(`fd 1`)。因此，下列命令将把（因为 `shell` 按从左至右的顺序处理 IO 重定向语句）标准输出和标准错误写入 `result.log` 文件：

```shell
./fcntl tfile.txt > result.log 2>&1
```

`shell` 通过将 **`fd 1` 复制到 `fd2` 实现了标准错误的重定向操作**，**因此 `fd 2` 与 `fd 1` 指向同一个打开文件句柄**。实现文件描述符的接口有 $4$ 个

### dup

`dup()` 调用复制一个打开文件描述符 `oldfd`，并返回 `newfd`。`oldfd` 和 `newfd` 均指向同一个系统打开文件表项

> [!tip] 
> 
> `oldfd` 和 `newfd` 引用同一个文件句柄。系统会保证 **新描述符一定是编号值最低的未用文件描述**
> 

```c
#include <unistd.h>

int dup(int oldfd);
```

假如发起如下调用：

```c
newfd = dup(1);
```

在正常情况下，`shell` 已经代表程序打开了 $\text{fd 0}, \text{fd 1}, \text{fd 2}$，且没有其他描述符在用，`dup()` 调用会创建 $\text{fd 1}$ 的副本，返回 $\text{fd 3}$

如果希望返回 $\text{fd 2}$，首先关闭 $\text{fd 2}$，然后再调用 `dup()` 进行复制

```c
close(2);
newfd = dup(1);
```

注意：只有当 $\text{fd 0}$ 已经打开时，这段代码才可以工作。如果想总是可以获得期望的结构可以使用 `dup2()` 系统调用

### dup2

`dup2()` 系统调用会为 `oldfd` 参数所指定的文件描述符创建副本，其编号由 `newfd` 参数指定

```c
#include <unistd.h>

int dup2(int oldfd, int newfd);
/* 成功返回newfd，失败返回 -1*/
```

如果由 `newfd` 参数所指定编号的文件描述符之前已经打开，那么 `dup2()` 会首先将其关闭

`dup2()` 调用会 **默然忽略 `newfd` 关闭期间出现的任何错误**。故此，编码时更为安全的做法是：在调用 `dup2()` 之前，若 `newfd` 已经打开，则应显式调用 `close()` 将其关闭

如果 `oldfd` 并非有效的文件描述符，那么 `dup2()` 调用将失败并返回 `-1`，并设置错误码 `EBADF`，且不关闭 `newfd`

如果 `oldfd` 有效，且与 `newfd` 值相等，那么 `dup2()` 将什么也不做，不关闭 `newfd`，并将其作为调用结果返回

### fcntl(fd, F_DUPFD, startfd)

`fcntl()` 的 `F_DUPFD` 操作是复制文件描述符的另一接口，更具灵活性

```c
newfd = fcntl(oldfd, F_DUPFD, startfd)
```

该调用为 `oldfd` 创建一个副本，且将 **使用大于等于 `startfd` 的最小未用值作为描述符编号**
 
### dup3

文件描述符和其副本之间共享同一打开文件句柄所含的 **文件偏移量** 和 **状态标志**。然而，新文件描述符有其自己的一套文件描述符标志，且其 `close-on-exec` 标志（`FD_CLOEXEC`）总是处于关闭状态

```c
#define __USE_GNU
#include <unistd.h>

int dup3(int oldfd, int newfd, int flags);
/* 成功返回newfd，失败返回 -1*/
```

`dup3()`只支持一个标志 `O_CLOEXEC`，这将促使内核为新文件描述符设置 `close-on-exec` 标志（`FD_CLOEXEC`）

> [!tip] 
> `dup3()` 系统调用始见于 Linux 2.6.27，为 `Linux` 所特有
> 

## 创建临时文件


有些程序需要创建一些 **临时文件**，仅供其在运行期间使用，**程序终止后即行删除**。例如，编译器程序会在编译过程中创建临时文件

C 语言函数库提供了一系列的库函数用于创建临时文件。在 [[标准 IO]] 中我们介绍了 `tmpfile()` 库函数，该函数位于 `<stdio.h>` 头文件中。现在，我们介绍另一个库函数 `mkstemp()` 

`mkstemp()` 函数基于调用者提供的模板 生成一个唯一文件名并打开该文件，返回一个可用于 IO 调用的文件描述符

```c
#include <stdlib.h>

int mkstemp(char *template);
/* 根据 template 创建一个临时文件，并返回打开文件描述符 */
```

根据 `template` 创建一个临时文件，并返回打开文件描述符。其中 `tmplate` 采用路径名格式，并且最后 $6$ 个字符必须是 `XXXXXX`。这 $6$ 个字符将被替换，保证文件名的唯一性，修改后的字符串将通过 `template` 参数返回

> [!important] 
> 
> 由于  `mkstemp` 会修改 `template` 指向字符串的值。因此，**tmplate 必须指定为字符数组**，而非字符串常量
> 
> 文件拥有者对 `mkstemp()` 函数建立的文件拥有读写权限，并且其他用户则没有任何操作权限）
> 
> 打开文件时使用了 `O_EXCL` 标志，以保证调用者以独占方式访问文件
> 

程序如果需要删除零食文件，可以调用 `unlink` 系统调 ^[[[目录与链接]]] 用将其删除

```c
int fd;
char template[] = "/tmp/{name}-XXXXXX";

fd = mkstemp(template);
if(fd == -1) {
	// 创建临时文件失败，进行错误处理
}
// ... 
// 操作临时文件：write read

unlink(template);  // 删除临时文件
```

## 




