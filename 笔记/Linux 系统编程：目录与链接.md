# 目录与链接

每个进程都有两个目录相关属性 **根目录** 及 **当前工作目录**，分别用于为解释绝对路径名和相对路径名提供参照点

## 目录与硬链接

在文件系统中，目录的存储方式类似于普通文件。目录与普通文件的区别有二

> [!tip] 
> 
> + 在其 $\text{i-node}$ 条目中，会将目录标记为一种不同的文件类型
> 
> + 目录是经特殊组织而成的文件。本质上说就是一个表格，包含文件名和 $\text{i-node}$ 编号
> 
> 在大多数原生 Linux 文件系统上，文件名长度可达 $255$ 个字符
> 

下图展示了文件 `/etc/passwd` 锁维护的文件系统 $\text{i-node}$ 表已经相关目录文件的部分内容，展示了目录与 $\text{i-node}$ 的关系

![[Pasted image 20241021192526.png]]

> [!attention] 
> 
> 虽然一个进程能够打开一个目录，但却不能使用 `read()` 去读取目录的内容。为了检索目录内容，进程必须使用后续讨论的系统调用和库函数
> 
> 进程同样也不能使用 `write()` 来改变一个目录的内容，仅能借助于诸如 `open()`（创建一个新文件） `link()` `mkdir()` `symlink()` `unlink()` 及 `rmdir()` 之类的系统调用
> 

> [!tip] 
> 
> $\text{i-node}$ 表的编号始于 $1$，而非 $0$，因为若目录条目的 $\text{i-node}$ 字段值为 $0$，则表明该条目尚未使用
> 
> $\text{i-node}\ 1$ 用来记录文件系统的坏块。文件系统根目录(`/`)总是存储在 $\text{i-node}$ 条目 $2$ 中，所以内核在解析路径名时就知道该从哪里着手
> 

回顾 [[操作系统：文件系统的实现#i-node]] 中存储的信息列表，会发现其中并未包含文件名，而仅通过目录列表内的一个映射来定义文件名称

> [!tip] 通过目录列表映射定义文件名称的优点
> 
> 能够在相同或者不同目录中创建多个名称，每个均指向相同的 $\text{i-node}$ 节点。也将这些名称称为 **链接**，有时也称之为 **硬链接**
> 

shell 命令 `ln` 可以为已经存在的文件创建新的硬链接

```shell
NAME
       ln - make links between files

SYNOPSIS
       ln [OPTION]... [-T] TARGET LINK_NAME
       ln [OPTION]... TARGET
       ln [OPTION]... TARGET... DIRECTORY
       ln [OPTION]... -t DIRECTORY TARGET...
```

`ln` 命令默认创建硬链接。下面 shell 会话日志展示了如何使用 `ln` 创建硬链接

```shell
$ ls -li | grep test.txt
918168 -rw-r--r-- 2 ecs-user ecs-user    12 Oct 20 19:11 hard_link_test.txt
918168 -rw-r--r-- 2 ecs-user ecs-user    12 Oct 20 19:11 test.txt
```

经过 `ls –li` 命令所示 $\text{i-node}$ 编码是相同的，因此均指向相同文件。`ls –li` 命令所示内容的第三列为对 $\text{i-node}$ 链接的计数；`test.txt` 锁指向的 $\text{i-node}$ 的链接数上升至 $2$，因为现在指向该文件的有两个名字

若移除其中一个文件名，另一文件名以及文件本身将继续存在

```shell
$ rm -rf hard_link_test.txt
$ ls -li | grep test.txt
918168 -rw-r--r-- 1 ecs-user ecs-user    12 Oct 20 19:11 test.txt
```

仅当 $\text{i-node}$ 的链接计数降为 $0$ 时，也就是移除了文件的所有名字时，才会删除文件的 $\text{i-node}$ 记录和数据块

> [!summary] 
> 
> `rm` 命令从目录列表中删除一文件名，将相应 $\text{i-node}$ 的链接计数减 $1$，若链接计数因此而降为 $0$，则将释放该文件名所指代的 $\text{i-node}$ 和数据块
> 

> [!tip]
> 
> 同一文件的所有名字（链接）地位平等。在移除与文件相关的一个名称后，物理文件继续存在，但只能通过另一文件名来访问其内容
> 


> [!warning] 硬链接的缺陷，均可用符号链接来加以规避
> 
> + 因为目录条目（硬链接）对文件的指代采用了 $\text{i-node}$ 编号，而 $\text{i-node}$ 编号的唯一性仅在一个文件系统之内才能得到保障，所以 **硬链接必须与其指代的文件驻留在同一文件系统中**
> 
> + 不能为目录创建硬链接，从而避免出现令诸多系统程序陷于混乱的链接环路
> 

## 符号链接

符号链接，有时也称为 **软链接**，是一种特殊的文件类型，**其数据是另一文件的名称**。下图展示软连接与硬链接的区别：两个硬链接 `/home/erena/this` 和 `/home/allyn/that` 指向同一个文件，而符号链接 `/home/kiran/other`，则指代文件名 `/home/erena/this`

![[Pasted image 20241021210752.png]]

shell 命令 `ln` 指定 `-s` 选项即可创建符号链接。`ls` 指定 `-F` 选项的输出结果会在符号链接尾部附加 `@`

> [!tip] 
> 
> 符号链接的内容既可以是绝对路径，也可以是相对路径。解释相对符号链接时以链接本身的位置作为参照点
> 

如果 **移除了符号链接所指向的文件名**，**符号链接本身还将继续存在**，尽管无法再对其进行解引用操作，也将此类链接称之为 **悬空链接**

> [!important] 
> 
> 因为符号链接指代一个文件名，而非 $\text{i-node}$ 编号，所以可以用其来链接不同文件系统中的一个文件
> 

符号链接之间可能会形成链路。当在各个文件相关的系统调用中指定了符号链接时，内核会对一系列链接层层解去引用，直抵最终文件

> [!tip] 
> 
> SUSv3 规定，针对路径名中的每个符号链接部件，系统实现应允许对其实施至少 `_POSIX_SYMLOOP_MAX` 次解除引用操作。`_POSIX_SYMLOOP_MAX` 的规定值为 $8$
> 

## 创建和移除硬链接

`link()` 和 `unlink()` 系统调用分别创建和移除硬链接

系统调用 `link()` 以 `newpath` 参数指定的路径名创建一个新链接，该链接与 `oldpath` 参数指定的 $\text{i-node}$ 编号相同

```c
#include <unistd.h>

int link(const char * oldpath, const char * newpath);
/* 成功返回 0；错误返回 -1 */
```

> [!tip] 参数 `oldpath`：已存在的文件的路径名
> 

> [!tip] 参数 `newpath`：硬链接的路径名
> 
> 如果 `newpath` 已存在，则 `link()` 立即返回，并设置错误码为 `EEXIST`
> 

> [!attention] 
> 
> 在 Linux 中，`link()` 系统调用 **不会对符号链接进行解引用操作**。若 `oldpath` 属于符号链接，则会将 `newpath` 创建为指向相同符号链接文件的全新硬链接
> 

系统调用 `ulink()` 移除 `pathname` 指定的链接，且如果此链接是指向文件的最后一个链接，那么还将移除文件本身

```c
#include <unistd.h>

int unlink(const char *pathname);
/* 成功返回 0；错误返回 -1 */
```

> [!attention] 
> 
> 若 `pathname` 中指定的链接不存在，则 `unlink()` 调用失败，并将 `errno` 置为 `ENOENT`
> 
> `unlink()` 不能移除一个目录，完成这一任务需要使用 `rmdir()` 或 `remove()`
> 
> `unlink()` 不会对符号链接进行解引用操作
> 

## 使用符号链接

系统调用 `symlink()` 使用 `pathname` 所指定的路径名创建一个符号链接 `linkpath`

```c
#include <unistd.h>

int symlink(const char *pathname, const char * linkpath);
/*成功返回 0；错误返回 -1*/
```

> [!tip] 
> 
> 移除符号链接请使用 `unlink()` 系统调用
> 

> [!attention] 
> 
> 如果 `linkpath` 中给定的路径名已然存在，则调用失败，并将 `errno` 置为 `EEXIST`
> 
> `pathname` 指定的名字可以不存在。这是，`linkpath` 成为悬空链接，其他系统调用试图对其进行解引用操作都将出错，通常错误号为 `ENOENT`
> 

如果指定一符号链接作为 `open()` 调用的 `pathname` 参数，那么将打开链接指向的文件。如果需要 **读取符号链接本身的内容**，系统调用 `readlink()` 将完成该工作

```c
#include <unistd.h>

ssize_t readlink(const char *pathname, char *buffer, size_t bufsiz);
/* 成功返回读取到 buffer 中的字节数，失败返回 -1 */
```

## 创建和移除目录

系统调用 `mkdir()` 用于创建一个新目录

```c
#include <sys/stat.h>

int mkdir(const char *pathname, mode_t mode);
/* 成功返回 0；失败返回 -1 */
```

> [!tip] 参数 `mode`：参考 [[Linux 系统编程：文件属性#文件权限]] 和  [[Linux 系统编程：文件属性#目录权限]]

> [!attention] 
> 
> 若 `pathname` 指定的路径名的文件已经存在，则调用失败并将 `errno` 置为 `EEXIST`
> 
> `mkdir()` 系统调用所创建的仅仅是 `pathname` 路径名中的最后一部分
> 

系统调用 `rmdir()` 移除由 `pathname` 指定的目录

```c
#include <unistd.h>

int rmdir(const char *pathname);
/* 成功返回 0；失败返回 -1 */
```

> [!attention] 
> 
> 要使 `rmdir()` 调用成功，则要删除的目录必须为空
> 
> 如果 `pathname` 的最后一部分为符号链接，那么 `rmdir()` 调用将不对其进行解引用操作，并返回错误，同时将 `errno` 置为 `ENOTDIR`
> 

## 修改文件名

系统调用 `rename()` 用于重命名文件，或者将文件移动到同一个文件系统下的另一个目录

```c
#include <stdio.h>

int rename(const char *oldpath, const char * newpath);
/* 成功返回 0；失败返回 -1 */
```

调用会将现有的一个路径名 `oldpath` 重命名为 `newpath` 参数所指定的路径名

> [!attention] 
> 
> `rename()` 调用仅操作目录条目，而不移动文件数据：不会影响打开文件描述符，也不会影响指向该文件的其他硬链接
> 

## 删除目录或文件

库函数 `remove()` 移除文件或空目录

```c
#include <stdio.h>

int remove(const char *pathname);
/* 成功返回 0；失败返回 -1 */
```

如果 `pathname` 是一文件，那么 `remove()` 去调用 `unlink()`；如果 `pathname` 为一目录，那么 `remove()` 去调用 `rmdir()`

## 读目录
### 打开目录

`opendir()` 库函数打开一个目录，并返回指向该目录的句柄，供后续调用使用

```c
#include <dirent.h>

DIR *opendir(const char *dirpath);
/* 成功返回目录流；失败返回 NULL */
```

> [!tip] 
> 
> `opendir()` 函数打开由 `dirpath` 指定的目录，并返回指向 `DIR` 类型结构的指针
> 
> 一旦从 `opendir()` 返回，则将目录流指向目录列表的首条记录
> 

`fdopendir()` 库函数从打开文件描述符指代的目录

```c
#include <dirent.h>

DIR *fdopendir(int fd);
/* 成功返回目录流；失败返回 NULL */
```

### 读目录项

`readdir()` 库函数从一个目录流中读取连续的条目

```c
#include <dirent.h>

struct dirent *readdir(DIR *dirp);
/* 成功返回指向静态分配的 dirent 结构；失败返回 NULL */
```

每调用 `readdir()` 一次，就会从 `dirp` 所指代的目录流中读取下一目录条目，并返回一枚指针，指向经静态分配而得的 `dirent` 类型结构，内含与该条目相关的如下信息

```c
struct dirent {
	ino_t d_ino;  /* 文件 i-node 编号 */
	char d_name[]; /* 以 NULL 结尾的文件名 */
};
```

> [!attention] 
> 
> 每次调用 `readdir()` 都会覆盖该结构
> 

### 移动目录项指针

`rewinddir()` 函数可将目录流回移到起点，以便对 `readdir()` 的下一次调用将从目录的第一个文件开始。

```c
#include <dirent.h>

void rewinddir(DIR *dirp);
```

`telldir()` 函数用户获取当前目录流偏移量

```c
#include <dirent.h>

long telldir(DIR *dirp);
```

`seekdir()` 函数将目录流的当前偏移量设置为指定值

```c
#include <dirent.h>

void seekdir(DIR *dirp, long loc);
```

> [!tip] 参数 `loc`：通常是 `telldir()` 的返回值

### 关闭目录流

`closedir()` 函数将由 `dirp` 指代、处于打开状态的目录流关闭，同时释放流所使用的资源

```c
#include <dirent.h>

int close(DIR *dirp);
```

### 获取目录流的文件描述符

`dirfd()` 函数返回与 `dirp` 目录流相关联的文件描述符

```c
#include <dirent.h>

int dirfd(DIR *dirp);
```

## 遍历目录

`nftw()`函数允许对整个目录子树进行递归遍历，并为子树中的每个文件执行某些操作

`nftw()` 函数遍历由 `dirpath` 指定的目录树，并为目录树中的每个文件调用一次由程序员定义的 `func` 函数

```c
#define _XOPEN_SOURCE 500
#include <ftw.h>

int nftw(const char *dirpath, 
		 int(*func)(const char * pathname, const struct stat *statbuf, int typeflag, struct FTW *ftwbuf),
		 int nopenfd, int flags);
/* 成功遍历目录树之后返回 0；错误返回 -1 或调用 func 第一个返回非零值 */
```

> [!attention] 
> 
> 默认情况下，`nftw()` 会针对给定的树执行未排序的前序遍历，即对各目录的处理要先于各目录下的文件和子目录
> 

> [!tip] 参数 `nopenfd` 指定了 `nftw()` 可使用文件描述符数量的最大值
> 
> 当 `nftw()` 遍历目录树时，最多会为树的每一层级打开一个文件描述符
> 
> 如果目录树深度超过这一最大值，那么 `nftw()` 会在做好记录的前提下，关闭并重新打开描述符，从而避免同时持有的描述符数目突破上限 `nopenfd`
> 
> 

`nftw()` 的 `flags` 参数由 $0$ 个或多个下列常量 **位或** 组成，这些常量可对函数的操作做出修正

| 标志          | 描述                            |
| :---------- | :---------------------------- |
| `FTW_CHDIR` | 在处理目录内容之前先调用 `chdir()` 进入每个目录 |
| `FTW_DEPTH` | 对目录树执行后序遍历                    |
| `FTW_MOUNT` | 不会越界进入另一文件系统                  |
| `FTW_PHYS`  | 不要对符号链接进行解引用。默认情况下，会对符号链接解引用  |
| `FTW_D`     | 这是一个目录                        |
| `FTW_DNR`   | 不能读取目录                        |
| `FTW_DP`    | 正在对一个目录进行后序遍历                 |
| `FTW_F`     | 该文件的类型是除目录和符号链接以外的任何类型        |
| `FTW_NS`    | 对该文件调用 `stat()` 失败，可能是因为权限限制  |
| `FTW_SL`    | 是一个符号链接                       |
| `FTW_SLN`   | 这是一个悬空的符号链接                   |

## 进程的当前工作目录

一个进程的 **当前工作目录**（Current Working Directory，CWD）定义了该 **进程解析相对路径名的起点**。新进程的当前工作目录继承自其父进程

### 获取当前工作目录

进程可以使用 `getcwd()` 来获取当前工作目录

```c
#include <unistd.h>

char *getcwd(char *cwdbuf, size_t size);
/* 成功返回 cwdbuf; 失败返回 NULL */
```

`getcwd()` 函数将内含当前工作目录绝对路径的字符串（包括结尾空字符）置于 `cwdbuf` 指向的已分配缓冲区中

> [!attention] 
> 
> 调用者必须为 `cwdbuf` 缓冲区分配至少 `size` 个字节的空间，通常 `cwdbuf` 的大小与 `PATH_MAX` 常量相当
> 

> [!tip]
> 
> 参数 `cwdbuf` 如果为 `NULL`，则 `getcwd()` 使用 `malloc()` 在堆上申请内存
> 
> 主调函数应该调用 `free()` 释放堆上申请的空间
> 

### 修改当前工作目录

`chdir()` 系统调用将调用进程的当前工作目录改变为由 `pathname` 指定的相对或绝对路径名（如属于符号链接，还会对其解除引用）。

```c
#include <unistd.h>

int chdir(const char *pathname);
/* 成功返回 0；失败返回 -1 */
```

`fchdir()` 系统调用与 `chdir()` 作用相同，只是在指定目录时使用了文件描述符，而该描述符是之前调用 `open()` 打开相应目录时获得的

```c
#define _XOPEN_SOURCE 500

#include <unistd.h>

int fchdir(int fd);
/* 成功返回 0；失败返回 -1 */
```

## 改变进程的根目录

每个进程都有一个根目录，该目录是解释绝对路径（即那些以 `/` 开始的目录）时的起点。默认情况下，这是文件系统的真实根目录

特权级（CAP_SYS_CHROOT）进程通过 `chroot()` 系统调用可以改变进程的根目录

```c
#define _BSD_SOURCE

#include <unistd.h>

int chroot(const char *pathname);
```

## 解析路径名

`realpath()` 库函数对 `pathname`（以空字符结尾的字符串）中的所有符号链接一一解除引用，并解析其中所有对 `/.` 和 `/..` 的引用，从而生成一个以空字符结尾的字符串，内含相应的绝对路径名

```c
#include <stdlib.h>

char *realpath(const char *pathname, char *resolved_path);
```

## 解析路径名字符串

`dirname()` 和 `basename()` 函数将一个路径名字符串分解成目录和文件名两部分

```c
#include <libgen.h>

char *dirname(char *pathname);
char *basename(char *pathname);
```

> [!example] 
> 
> 给定路径名为 `/home/britta/prog.c`，`dirname()` 将返回 `/home/britta`，而  `basename()` 将返回 `prog.c`
> 
