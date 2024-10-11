# 文件 IO：通用 IO 模型

[[文件系统]] 和 [[文件系统的实现]] 实现中，我们介绍来文件系统的功能，以及文件数据是如何存储的。现在，我们开始介绍与文件系统相关的系统调用。现在，在这里只介绍文件 IO

> [!seealso] 
> 
> 关于文件 IO，在 [[标准 IO]] 只能了解标准库输入输出库函数
> 

在 Unix/Linux 的文件系统中，大部分资源被都当作文件处理。进程执行 IO 通过 **文件描述符** 指代以打开的文件

> [!tip] 
> 
> 在 [[文件系统的实现#文件系统实现]] 我们介绍过，在内存中，内核维护了一个打开文件表，进程也维护了一个打开文件表。进程级打开文件表的表项指向了内核级打开文件表中的某一表项
> 
> Unix/Linux 使用进程级打开文件表的索引作为 **文件描述符**。也就说，**文件描述符是一个小整数**，代表了进程打开的文件
>

Unix/Linux 中，文件描述符表示所有类型的打开文件。**针对每个进程，文件描述符都自成一套**

> [!tip] Unix/Linux 文件类型
> + 普通文件
> + 目录文件
> + 设备文件
> 	+ 字符设备文件
> 	+ 块设备文件
> + 套接字文件
> + 管道文件
> 	+ PIPE
> 	+ FIFO
> + 终端文件
>

大多数程序都希望使用下面 $3$ 种标准的文件描述符

| 文件描述符 | 用途     | POSIX 名称        | stdio 宏  |
| :---- | ------ | --------------- | -------- |
| $0$   | 标准输入   | `STDIN_FILENO`  | `stdin`  |
| $1$   | 标准输出   | `STDOUT_FILENO` | `stdout` |
| $2$   | 标准错误输出 | `STDERR_FILENO` | `stderr` |

程序开始运行之前，$\text{shell}$ 代表程序打开这 $3$ 个文件描述符。**程序继承了 $\text{shell}$  文件描述符的副本**

> [!tip]
> 
> 这 $3$ 个标准文件描述符始终是 $\text{shell}$  打开的
> 

在程序中可以直接使用这 $3$ 个标准文件描述符：可以直接使用数字，也可以使用 POSIX 名称

> [!tip]
> 
> POSIX 名称是在 `<unistd.h>` 头中定义的宏
> 
> ```c
> /* 标准文件描述符.  */
> #define	STDIN_FILENO	0	/* 标准输入 */
> #define	STDOUT_FILENO	1	/* 标准输出  */
> #define	STDERR_FILENO	2	/* 标准错误输出.  */
> ```

文件系统的 IO 操作主要的系统调用有 $4$ 个

| 系统调用      | 头文件          | 描述   |
| :-------- | ------------ | ---- |
| `open()`  | `<fcntl.h>`  | 打开文件 |
| `read()`  | `<unistd.h>` | 读取数据 |
| `write()` | `<unistd.h>` | 写入数据 |
| `close()` | `<unistd.h>` | 关闭文件 |

> [!attention] 
> 
> UNIX IO 模型是通用的，意味着上述 $4$ 个函数可以对所有类型的文件执行 IO 操作
> 

## 打开文件：`open()`

系统调用 `open()` 或者 **打开** 已经存在的文件，或者 **创建并打开** 新文件

```c
#include <fcntl.h>

int open(const char *pathname, int flags, /* mode_t mode */ ...);
/* 该函数成功返回文件描述符，失败返回 -1*/
```

`open()` 以 `flags` 指定的方式打开 `pathname` 指定的文件。如果成功打开，则返回打开文件描述符；否则返回 $-1$，并将 `errno` 置为相应的错误标志

> [!tip] pathname：要打开的 **文件路径**
> 
> 如果 pathname 是符号链接，会跟踪它链接的文件
> 

> [!tip] flags：位掩码，指定打开文件后的 **访问模式**
> 
> 下表列出了所有的 flags 参数取值的宏。这些宏定义在 `<fcntl.h>` 头中
>
>| 标志            | 用途                             |
> | :------------ | :----------------------------- |
> | `O_RDONLY`    | 以只读方式打开                        |
> | `O_WRONLY`    | 以只写方式打开                        |
> | `O_RDWR`      | 以读写方式打开                        |
> |               |                                |
> | `O_CLOEXEC`   | 设置 `close-on-exec` 标志          |
> | `O_CREAT`     | 若文件不存在则创建                      |
> | `O_EXCL`      | 结合 `O_CREAT` 参数使用，专门用于创建文件     |
> | `O_DIRECT`    | 无缓冲的输入/输出                      |
> | `O_DIRECTORY` | 如果 `pathname` 不是目录，则失败         |
> | `O_LARGEFILE` | 在 $32$ 位系统中使用此标志打开大文件          |
> | `O_NOATIME`   | 调用 `read()` 时，不修改文件最近访问时间      |
> | `O_NOCTTY`    | 不要让 `pathname`（所指向的终端设备）成为控制终端 |
> | `O_NOFOLLOW`  | 对符号链接不予解引用                     |
> | `O_TRUNC`     | 截断已有文件，使其长度为零                  |
> |               |                                |
> | `O_APPEND`    | 总在文件尾部追加数据                     |
> | `O_ASYNC`     | 当 I/O 操作可行时，产生信号 `signal` 通知进程   |
> |  `O_DSYNC`     | 提供同步的 I/O 数据完整性                |
> | `O_NONBLOCK`  | 以非阻塞方式打开                       |
> | `O_SYNC`      | 以同步方式写入文件                      |
> 
> 这些常量被分为 $3$ 组
> + 文件访问模式标志：包含 `O_RDONLY` `O_WRONLY` `O_RDWR` 共$3$ 个常量，调用 `open()` 时，只能指定其中之一。调用 `fcntl()` 通过 `F_GETFL` 可以检索文件的访问模式
> + 文件创建标志：上表中的第二部分，其控制范围不拘于 `open()` 调用行为的方方面面，还涉及后续 IO 操作。这些标志不能检索，也无法修改
> + 已打开文件的状态标志：使用 `fcntl()` 通过 `F_GETFL` 和 `F_SETFL` 操作可以检索和修改此类标志
> 

> [!tip] mode：位掩码，指定文件的 **访问权限**
> 
> 只有当调用 `open` 创建文件时才需要指定该参数。权限的设置参考 [[Linux 命令和 vim 操作#类型和权限]] 
> 
> 

> [!tip] 返回值
> 
> + **成功**：返回文件描述符，文件描述符表中最小未使用的位置
> + **失败**：返回 $-1$，并设置 `errno` 
> 

### open() 函数的错误


若打开文件时发生错误，`open()` 将返回 $−1$，错误号 `errno` 标识错误原因。`open()` 可能发生的错误有如下几个

| 错误号       | 描述                                                                                                           |
| :-------- | :----------------------------------------------------------------------------------------------------------- |
| `EACCES`  | 文件权限不允许调用进程以 `flags` 参数指定的方式打开文件。                                                                            |
| `EISDIR`  | 所指定的文件属于目录，而调用者企图打开该文件进行写操作                                                                                  |
| `EMFILE`  | 进程已打开的文件描述符数量达到了进程资源限制所设定的上限                                                                                 |
| `ENFILE`  | 文件打开数量已经达到系统允许的上限。                                                                                           |
| `ENOENT`  | 要么文件不存在且未指定 `O_CREAT` 标志<br>要么指定了 `O_CREAT` 标志，但是 `pathname` 指定目录之一不存在<br>要么 `pathname` 参数为符号连接，而该链接指向的文件不存在 |
| `EROFS`   | 所指定的文件隶属于只读文件系统，而调用者企图以写方式打开文件                                                                               |
| `ETXTBSY` | 所指定的文件为可执行文件（程序），且正在运行                                                                                       |

### 创建文件

在早期的 UNIX 实现中，`open()` 只有两个参数，无法创建新文件，而是使用  `creat()` 系统调用来创建并打开一个新文件

```c
#include <fcntl.h>

int creat(const char *pathname, mode_t mode);
/* 成功创建文件返回打开文件描述符，失败返回 -1 并设置 erron */
```

系统调用 `creat()` 等价于 `open()` 调用

```c
fd = open(pathname, O_WRONLY | O_CREATE | O_TRUNC, mode);
```

> [!tip] 
> 
> 请忽略该系统调用，使用 `open()` 可以对打开文件方式提供更多控制
> 

## 关闭文件：`close()`

`close()` 系统调用 **关闭一个打开的文件描述符**，并将其释放回调用进程，供该进程继续使用

```c
#include <unistd.h>

int close(int fd);
/* 成功返回 0；错误返回 -1*/
```

> [!tip] 参数 `fd` ：需要关闭文件的文件描述符
> 
> `open()` 调用返回的值
> 

> [!attention] 
> 
> 当一进程终止时，将自动关闭其已打开的所有文件描述符
> 

## 顺序访问

### 读取文件内容：`read()`

系统调用 `read()` 用于 **读取文件内容**

```c
#include <unistd.h>

ssize_t read(int fd, void *buffer, size_t count);
/* 返回读取的字节数；EOF 返回 0；错误返回 -1*/
```

`read()` 从文件描述符 `fd` 所指代的打开文件中读取 `count` 字节，并将读取的内容放入 `buffer` 指定的内存区域

> [!tip] 参数 `fd`：文件描述符
> 
> 通常是调用 `open()` 返回的值。也可以是 `STDIN_FILENO` `STDOUT_FILENO` `STDERR_FILENO`
> 
 
> [!tip] 参数 `buffer`：缓冲区
> 
> 存放数据的内存块的首地址
> 

 > [!tip] 参数 `count`: 要读取的字节数
 > 
 > 通常为缓冲区 `buffer` 的大小
 > 

### 内容写入文件：`write()`

系统调用 `write()`  **将数据写入一个已打开的文件中**

```c
#include <unistd.h>

ssize_t write(int fd, void *buffer, size_t count);
/* 返回成功写入的字节数；错误返回 -1*/
```

`write()` 将缓冲区 `buffer` 中长度为 `count` 字节的内容写入到 `fd` 指定的打开文件。写入成功，返回写入的字节数，通常是 `count`；写入失败，返回 `-1` 并设置 `errno`

> [!tip] 参数 `fd`：文件描述符
> 
> 通常是调用 `open()` 返回的值。也可以是 `STDOUT_FILENO` `STDERR_FILENO`
> 

> [!tip] 参数 `buffer`：指向待写入内容的缓冲区
> 

> [!tip] 参数 `count`：待写入的字节数
> 
> 要写入的字节数，通常为 `buffer` 的大小
> 


## 直接访问

### 改变文件偏移量：`lseek()`

针对文件描述符 `fd` 参数所指代的已打开文件，`lseek()` 系统调用依照 `offset` 和 `whence` 参数值调整该文件的偏移量

```c
#include <unistd.h>

off_t lseek(int fd, off_t offset, int whence);
/* 成功返回新的文件偏移量，失败返回-1*/
```

`lseek()` 函数将 `fd` 的文件偏移量设置为 `whence` 为基点偏移 `offset` 字节。设置成功就返回原始文件偏移量；失败返回 $-1$，并设置 `errno`

> [!tip] 参数 `fd`：需要调整当前文件偏移量的文件描述符
> 
> 通常是系统调用 `open()` 的返回值
> 

> [!tip] 参数 `offset`：新的偏移量
> 
> 单位是字节，可以是正数，也可以是负数
> 
> + 正数：在基点处，向后偏移 `offset` 字节
> + 负数：在基点处，向前偏移 `offset` 字节
> 

> [!tip] 参数 `whence`：新的偏移的参照基点
> 
> 参照哪个基点解释参数 `offset`。在 `<fcntl.h>` 中定义了参照基点的常量
> 
> |基点|值|描述|
> |:-|:-|:-|
> |`SEEK_SET`| 0 |文件开头位置作为基点|
> |`SEEK_CUR`| 1 |文件当前偏移位置作为基点|
> |`SEEK_END`| 2 |文件末尾作为基点|
> 
> 下图解释了参数 `whence` 的含义
> 
> ![[Pasted image 20241011180619.png]]
> 
> 参数 `whence` 的值可以决定 `offset` 的取值
> + `whence` 参数的取值为 `SEEK_CUR` 和 `SEEK_END`，`offset` 参数可以为正数，也可以为负数
> + `whence` 参数的取值 `SEEK_SET` 时，`offset` 参数只能为正数
> 
> 

> [!tip] 返回值
> 
> + **成功**：返回调整前的文件偏移量
> + **失败**：返回 $-1$，并设置 `errno`
> 

### 文件空洞

如果程序的 **文件偏移量已然跨越了文件结尾**，然后再执行 I/O 操作，将会发生什么情况？`read()` 调用将返回 $0$，表示文件结尾。然而，令人惊讶的是，`write()` 调用，可以写入文件结尾后的任意位置

> [!tip] 文件空洞：从 _文件结尾_ 后到 _新写入数据_ 间的这段空间
> 
>从编程的角度看，文件空洞中是存在字节的，读取空洞将返回 $0$(空字节) 填充缓冲区
> 

在 [[文件系统的实现]] 中，介绍的 EXT2 文件系统，包括大多数 Unix 文件系统，都支持文件空洞这样概念。并且 **文件空洞是不占用任何磁盘空间的**

> [!tip]
> 
> 如果后续某个时间点在文件空洞位置写入了数据，文件系统才会分配数据块
> 

文件空洞的主要优势在于，与为实际需要的空字节分配磁盘块相比，稀疏填充的文件会占用较少的磁盘空间


## 例程

在编写程序之前，需要做一些准备工作。就是对错误的处理，我们不期望每次都编写错误处理的代码，而是调用某个函数完成错误处理

```c title:errors.h
#ifndef ERRORS_H
#define ERRORS_H

#include "base.h"

#define usageError(argc, target, format, ...)     \
    do                                            \
    {                                             \
        if ((argc) != (target))                   \
        {                                         \
            fprintf(stderr, format, __VA_ARGS__); \
            exit(EXIT_FAILURE);                   \
        }                                         \
    } while (0)

#define catchError(value, target, format, ...)    \
    do                                            \
    {                                             \
        if ((value) == (target))                  \
        {                                         \
            fprintf(stderr, format, __VA_ARGS__); \
            exit(EXIT_FAILURE);                   \
        }                                         \
    } while (0)

#endif // ERRORS_H
```

某些需要固定包含的头文件间，可以提前写在一个头文件中，每次都包含我们的头文件即可

```c title:base.h
#ifndef BASE_H
#define BASE_H

// 常用头文件
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

// 文件系统相关头文件
#include <fcntl.h>
#include <sys/stat.h>

// unix 标准头文件
#include <unistd.h>

#endif //BASE_H
```

### copy

Unix/Linux 系统命令 `cp` 用于将文件复制一份。实现一个简单版本的 `cp` 命令，只需要将一个文件复制到另一个文件

```c title:copy.c

#include "base.h"
#include "errors.h"

#ifndef BUFFER_SIZE  // 支持 gcc -D
#define BUFFER_SIZE 1024
#endif


int main(int argc, char *argv[]) {

    usageError(argc, 3, "Usage: %s <old-file> <new-file>\n", argv[0]);

    int flags = O_RDONLY;
    int src_fd = open(argv[1], O_RDONLY);
    catchError(src_fd, -1, "Open: %s\n", strerror(errno));

    flags = O_WRONLY | O_CREAT | O_TRUNC;                       // 创建文件用于写入，如果文件存在则截断
    const mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;  // rw-r--r--
    int dst_fd = open( argv[2], flags, mode);
    catchError(dst_fd, -1, "Open: %s\n", strerror(errno));

    // 循环读取
    char buffer[BUFFER_SIZE];
    ssize_t num_read;
    while ((num_read = read(src_fd, buffer, BUFFER_SIZE)) > 0) {
        if(write(dst_fd, buffer, num_read) != num_read) {
            fprintf(stderr, "Write: %s\n", strerror(errno));
            return -1;
        }
    }

    catchError(num_read, -1, "Read: %s\n", strerror(errno));

    // 关闭文件
    catchError(close(src_fd), -1, "Close: %s\n", strerror(errno));
    catchError(close(dst_fd), -1, "Close: %s\n", strerror(errno));
    return 0;
}
```

### lseek

```shell

```


