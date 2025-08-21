# 文件属性

## 获取文件属性：`stat()`

系统调用 `stat()` `lstat()` 和 `fstat()` 可获取与文件有关的信息，其中大部分信息都是来自文件的 $\text{i-node}$

```c
#include <sys/stat.h>

int stat(const char *pathname, struct stat *statbuf);

int lstat(const char *pathname, struct stat *statbuf);

int fstat(int fd, struct stat *statbuf);
/* 成功返回 0；失败返回 -1 */
```

上述三个系统调用都是获取文件元数据信息，只是对文件的描述方式不同

| 系统调用      | 描述                             |
| :-------- | :----------------------------- |
| `stat()`  | 获取 `pathname` 指定的文件元数据         |
| `lstat()` | 如果 `pathname` 是链接文件，则获取链接本身的信息 |
| `fstat()` | 获取打开文件描述符指代文件的元数据              |

所有系统调用都会在缓冲区中返回一个由 `statbuf` 指向的 `stat` 结构，其格式如下

```c
struct stat {
	dev_t st_dev; // 设备 ID 号
	ino_t st_ino; // i-node 号
	mode_t st_mode;  // 文件类型和权限
	nlink_t st_nlink; // 硬链接数
	uid_t st_uid; // 属主 ID
	gid_t st_gid; // 属组 ID
	dev_t st_rdev;  // 特殊设备 ID
	off_t st_size;  // 总字节数
	blksize_t st_blksize; // io 块字节数
	blkcnt_t st_blocks;  // 存储块数
	time_t st_atime; // 最后访问时间
	time_t st_mtime; // 最后修改时间
	time_t st_ctime; // 最后状态改变时间
};
```

### 设备 ID 和 $\text{i-node}$ 号

`st_dev` 字段标识文件所驻留的设备 ID。`st_ino` 字段则包含了文件的 $\text{i-node}$ 号。利用设备号和 $\text{i-node}$ 号可以唯一标识一个文件

> [!tip] 
> 
> `dev_t` 类型记录了设备的 **主 ID** 和 **辅 ID**
> 

如果是针对 **设备文件的 $\text{i-node}$**，那么 `st_rdev` 字段则包含设备的 **主 ID** 和 **辅 ID**

可以利用宏 `major()` 和 `minor()`，可提取 `dev_t` 值的 **主 ID** 和 **辅 ID**

> [!tip] 
> 
> 获取对两个宏声明的头文件则随 UNIX 实现而各异
> 
> 在 Linux 系统上，若定义了 `_BSD_SOURCE` 宏，则两个宏定义于 `<sys/types.h>` 中
> 
> 在 Ubuntu 上，这两个宏定义于 `<sys/sysmacros.h>`
> 

由 `major()` 和 `minor()` 所返回的整形值大小也随 UNIX 实现的不同而各不相同。为保证可移植性，打印时应总是将返回值强制转换为 `long`

### 文件所有权

`st_uid` 和 `st_gid` 字段分别标识文件的 **属主 ID**(`uid`) 和 **属组 ID**(`gid`)

### 链接数

`st_nlink` 字段包含了指向文件的硬链接数，在 [[Linux 系统编程：目录与链接]] 找那个详细介绍

### 文件类型及权限

`st_mode` 字段内含有位掩码，起标识 **文件类型** 和指定 **文件权限** 的双重作用。下图为该字段所含各位的布局情况

![Pasted image 20241019223642|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755705702433-b3dbd90588ba4c1094715c65aeaa6f15.png)

#### 文件类型

想要获取文件类型，可以将 `st_mode` 与常量 `S_IFMT` 相与，即 `st_mode & S_IFMT`

> [!tip]
> 
> Linux 采用 `st_mode` 字段中的 $4$ 位来表示文件类型位。但是，SUSv3 中并未对文件类型位的表示做出任何规定，因此具体实现可能不同
> 

表达式 `st_mode & S_IFMT` 只是获得表示文件类型的 $4$ 个位。想要确定文件类型，需要与下表的常量进行比较；或者使用测试宏函数

| 常量         | 测试宏          | 文件类型    |
| :--------- | ------------ | ------- |
| `S_IFREG`  | `S_ISREG()`  | 常规文件    |
| `S_IFDIR`  | `S_ISDIR()`  | 目录      |
| `S_IFCHR`  | `S_ISCHR()`  | 字符设备    |
| `S_IFBLK`  | `S_ISBLK()`  | 块设备     |
| `S_IFIFO`  | `S_ISFIFO()` | FIFO或管道 |
| `S_IFSOCK` | `S_ISSOCK()` | 套接字     |
| `S_IFLNK`  | `S_ISLNK()`  | 符号链接    |

`st_mode` 字段的低 $12$ 位定义了文件权限；其中最低 $9$ 位分别用来表示文件属主、属组以及其他用户的读、写、执行权限

### 文件大小

`st_size` 字段用于表示文件的大小。但是，对于不同文件类型大小的含义不同

| 文件类型   | `st_size` 含义                 |
| :----- | :--------------------------- |
| 常规文件   | `st_size` 表示文件的字节数           |
| 符号链接   | `st_size` 表示链接所指路径名的长度，字节为单位 |
| 共享内存对象 | `st_size` 表示对象的大小            |

### 已分配块

**`st_blocks` 字段表示分配给文件的总块数**，块大小有不同的文件系统决定。对于 EXT2 文件系统，其逻辑块大小为 $1024$ $2048$ 还是 $4096$ 字节，`st_blocks` 的取值将总是 $2$ $4$ $8$ 的倍数

**`st_blocks` 字段记录了实际分配给文件的磁盘块数量**。如果文件内含空洞，该值将小于从相应文件字节数字段 `st_size` 的值

### 最优 I/O 块大小

`st_blksize` 字段指的是针对文件系统上文件进行 **I/O 操作时的最优块大小**，以字节为单位。**若 I/O 所采用的块大小小于该值，则被视为低效** ^[[[Linux 系统编程：文件 IO 缓冲#内核缓冲区高速缓存]]] 。一般而言，`st_blksize` 的返回值为 $4096$

### 文件时间戳

文件时间戳由下面 $3$ 个字段记录

| 字段名        | 描述       |
| :--------- | -------- |
| `st_atime` | 上次访问时间   |
| `st_mtime` | 上次修改时间   |
| `st_ctime` | 上次状态改变时间 |

这 $3$ 个字段的类型均属 `time_t`，是标准的 UNIX 时间格式，记录了自 Epoch 以来的秒数 ^[[[Linux 系统编程：时间]]]

> [!tip] Epoch：1970 年 01 月 01 日 00:00:00

## 文件时间戳

`stat` 结构的 `st_atime` `st_mtime` 和 `st_ctime` 字段所含为文件时间戳；对时间戳的记录形式为自 $1970$ 年 $01$ 月 $01$ 日 $0$ 时 $0$ 分 $0$ 秒以来所历经的秒数

大多数原生 Linux 和 UNIX 文件系统都支持上述所有的时间戳字段，但某些非 UNIX 文件系统则未必如此

对于 `stat` 结构所含的 $3$ 个时间戳字段，**Linux 从 2.6 版本将其精度提升至 _纳秒级_**。纳秒级分辨率将提高某些程序的精度，因为此类程序需要根据文件时间戳的先后顺序来作决定

### utime 和 utimes

系统调用 `utime()` 和 `utimes()` 可以显示改变存储于文件 $\text{i-node}$ 中存储的 **上次访问时间** 和 **上次修改时间**

```c
#include <utime.h>

int utime(const char *pathname, const struct utimbuf *buf);
/* 成功返回 0； 错误返回 -1*/
```

> [!tip] 参数 `pathname` 
> 
> 用来标识欲修改时间的文件。若该参数为符号链接，则会进一步解除引用
> 

> [!tip] 参数 `buf`
> 
> 存储上次访问时间和上次修改时间的结构体指针
> 
> ```c
> struct utimbuf
> {
>	time_t actime;  // 访问时间
> 	time_t modtime; // 修改时间
> };
> ```
> 
> 如果 `buf` 为 `NULL`，那么会将文件的上次访问和修改时间同时 **置为当前时间**
> 
> 若将 `buf` 指定为指向 `utimbuf` 结构的指针，则会使用该结构的相应字段去更新文件的上次访问和修改时间
> 

```c
#include <sys/time.h>

int utimes(const char *pathname, const struct timeval tv[2]);

```

`utimes()` 系统调用可以以 **微秒级精度** 来指定时间值。参数 `tv` 是一个长度为 $2$ 的 `timeval` 结构 ^[[[Linux 系统编程：时间]]] 的数组

`futimes()` 和 `lutimes()` 库函数的功能与 `utimes()`大同小异。前两者与后者之间的差异在于，用来指定要更改时间戳文件的参数不同

```c
#include <sys/time.h>

int futimes(int fd, const struct timeval tv[2]);
int lutimes(const char *pathname, const struct timeval tv[2]);
```

### utimensat 和 futimens 

`utimensat()` 系统调用 和 `futimens()` 库函数为设置对文件的 **上次访问和修改时间戳** 提供了扩展功能

```c
#define -XOPEN_SOURCE 700 /* #define _POSIX_C_SOURCE >= 200809 */
#include <sys/stat.h>

int utimenasat(int dirfd, const char *pathname, const struct timespace times[2], int flags);
/* 成功返回 0；失败返回 -1*/

int futimens(int fd, const struct timespec times[2]);
```

## 文件属主

每个文件都有一个与之关联的 **用户 ID**（UID）和 **组 ID**（GID），通过 UID 和 GID 可以判定文件的属主和属组

文件创建时，其 **用户 ID 是进程的有效用户 ID** ^[[[Linux 系统编程：进程凭证]]] 。而新建文件的 **组 ID 是进程的有效组 ID**（等同于 `System V` 系统的默认行为），或 **父目录的组 ID**（`BSD` 系统的行为）

装配 EXT2 文件系统时，`mount` 命令若指定了 `-o grpid`  选项，那么新建文件总是继承其父目录的组 ID。若指定了 `-o nogrpid` 选项，那么在默认情况下，新建文件的组 ID 则取自进程的有效组 ID。不过，如果已将目录的 `set-group-ID` 位置位（通过 `chmod g+s` 命令），那么文件的组 `ID` 又将从其父目录处继承

> [!tip] 
> 
> 一旦将某一目录的 set-group-ID 位置位后，该目录下所有子目录的 set-group-ID 位也将被置位
> 

下表总结了新建文件组所有权的规则

| mount 选项                       | 父目录的 Set-Group-ID | 新建文件的组所有权 |
| :----------------------------- | ----------------- | --------- |
| `-o grpid` 或 `-o bsdgroups`    | 忽略                | 父目录组 ID   |
| `-o nogripd` 或 `-o sysvgroups` | 无                 | 父目录组 ID   |
| `-o nogripd` 或 `-o sysvgroups` | 有                 | 父目录组 ID   |

系统调用  `chown()` `lchown()` 和 `fchown()` 可用来改变文件的属主（用户 ID）和属组（组ID）

```c
#include <unistd.h>

int chown(const char *pathname, uid_t owner, gid_t group);

#define _XOPEN_SOURCE 500  /* 或者 #define _BSD_SOURCE */
#include <unistd.h>

int lchown(const char *pathname, uid_t owner, gid_t group); // 不解析链接文件
int fchown(int fd, uid_t owner, gid_t group);  // 通过文件描述符修改
```

>[!tip] 
>
>参数 `owner` 和 `group` 分别为文件指定新的用户 ID 和组 ID
>
>**若只打算改变其中之一，只需将另一参数置为 `−1`**，即可令与之相关的 ID 保持不变
>

只有 **特权级进程**(`CAP_CHOWN`)才能使用 `chown()` **改变文件的用户 ID**。非特权级进程可使用 `chown()` 将自己所拥有文件的组 ID 改为其所从属的任一属组的 ID，前提是进程的有效用户 ID 与文件的用户 ID 相匹配

## 文件权限

现在介绍应用于文件和目录的权限方案。尽管此处所讨论的权限主要是针对普通文件和目录，但其规则可适用于所有文件类型，包括设备文件、FIFO 以及 UNIX 域套接字等

### 普通文件的权限

`stat` 结构中 `st_mod` 字段的低 $12$ 位定义了文件权限

![Pasted image 20241019223642|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755705702433-b3dbd90588ba4c1094715c65aeaa6f15.png)

> [!tip] 前 $3$ 位为专用位：分别是 `set-user-ID` `set-group-ID` 和 `sticky` 
> 
> + `Set-User-ID` 被标注为 `U`
> + `Set-Group-ID` 被标注为 `G`
> + `Sticky` 被标注为 `T`
> 
>  文件权限的最高三位是针对可执行文件和目录的
>   
>  针对可执行文件
>   
> + `U` 代表 Set-User-ID ^[[[Linux 系统编程：进程凭证]]]：如果是可执行程序，程序执行时，**进程的有效 ID 就是文件的所有者 ID**
> + `G` 代表 Set-Group-ID：如果是可执行程序，程序执行时，**进程的有效组 ID 就是文件属组 ID**
>   
>  针对目录文件
>   
> + `T` 代表粘滞位：当⽬录被设置了粘滞位权限以后，即便⽤户对该⽬录有写⼊权限，也不能删除该⽬录中其他⽤户的⽂件数据，⽽是 **只有该⽂件的所有者和 root ⽤户才有权将其删除**
> 

> [!tip] 其余 9 位为权限的掩码：分别授予访问文件的各类 **用户**
> 
> 将访问文件的用户分为了 $3$ 类
> + **Owner**，也称为 **User**：文件属主
> + **Group**：文件属组，根据创建文件的 **进程有效用户 ID** 或文件的 **父目录 ID**
> + **Other**：其他用户
> 
> 每一类用户授予的权限有下面 $3$ 种
> + **Read**：可以读取文件的内容
> + **Write**：可以像文件写入内容
> + **Execute**：可以执行文件；即，文件是程序或脚本。要执行脚本文件，需同时具备读权限和执行权限
> 
> 针对文件的读写执行三种访问权限，上图的给出了标注
> 
> + `R` 代表读权限
> + `W` 代表写权限
> + `X` 代表执行权限
>   


头文件 `<sys/stat.h>` 定义了可与 `stat` 结构中 `st_mode` 相与的常量，用于检查特定权限位置位与否

| 常量        | 数值      | 权限位                        |
| :-------- | :------ | -------------------------- |
| 特殊权限位     |         |                            |
| `S_ISUID` | `04000` | `Set-User-ID`              |
| `S_ISGID` | `02000` | `Set-Group-ID`             |
| `S_ISVTX` | `01000` | `Sticky`（已过时）              |
|           |         |                            |
| 属主权限位     |         |                            |
| `S_IRUSR` | `0400`  | `User-Read`                |
| `S_IWUSR` | `0200`  | `User-Write`               |
| `S_IXUSR` | `0100`  | `User-Execute`             |
| `S_IRWXU` | `0700`  | `User-Read-Write-Execute`  |
|           |         |                            |
| 属组权限位     |         |                            |
| `S_IRGRP` | `040`   | `Group-Read`               |
| `S_IWGRP` | `020`   | `Group-Write`              |
| `S_IXGRP` | `010`   | `Group-Execute`            |
| `S_IRWXG` | `070`   | `Group-Read-Write-Execute` |
|           |         |                            |
| 其他用户权限    |         |                            |
| `S_IROTH` | `04`    | `Other-Read`               |
| `S_IWOTH` | `02`    | `Other-Write`              |
| `S_IXOTH` | `01`    | `Other-Exectue`            |
| `S_IRWXO` | `07`    | `Other-Read-Write-Execute` |

### 目录权限

> [!tip] 目录与文件拥有相同的权限方案
> 
> 目录对 $3$ 种权限的含义与普通文件并不相同
> 
> + **Read**：可以列出目录中的内容
> + **Write**：可以在目录内创建和删除文件
> + **Execute**：可以访问目录文件
> 

访问文件时，需要拥有对路径名所列所有目录的执行权限

> [!example] 
> 想读取文件 `/home/mtk/x`，则需拥有对目录 `/` `/home` 以及 `/home/mtk` 的执行权限。同时，还有对 `x` 有读权限
> 

下表总结了对目录操作时需要具备的权限

| 操作         | 需要的权限  |
| :--------- | :----- |
| 查看目录中的文件列表 | 读权限    |
| 访问目录内的文件   | 执行权限   |
| 添加或删除文件    | 执行和写权限 |

> [!tip] 
> 
> 如果对目录只有执行权限，而没有读权限；那么只要知道文件名，也是可以读取文件内容的。只是，不能列出目录中的文件列表

### 检查文件的访问权限

只要在 **访问文件或目录的系统调用** 中指定了路径名称，内核就会 **检查相应文件的权限**。如果赋予系统调用的路径名还包含 **目录前缀** 时，那么内核除去会检查对文件本身所需的权限以外，还会 **检查前缀所含每个目录的可执行权限**。内核会使用进程的 **有效用户 ID** **有效组 ID** 以及 **辅助组 ID** ^[[[Linux 系统编程：进程凭证]]] ，来执行权限检查

> [!tip] 
> 
> Linux 内核会使用文件系统用户 ID 和组 ID，而非相应的有效用户 ID 和组 ID，来进行文件权限检查
> 

 调用 `open()` 函数打开了文件，针对返回描述符的后续系统调用将不再进行任何权限检查

> [!tip] 检查文件权限时，内核遵循下列规则
> + 对于 **特权级进程**，授予其所有访问权限
> + 若进程的 **有效用户 ID 与文件的用户 ID 相同**，内核会 **根据文件的属主权限**，授予进程相应的访问权限
> + 若进程的 **有效组 ID 或任一附属组 ID 与文件的组 ID 相匹配**，内核会 **根据文件的属组权限**，授予进程对文件的相应访问权限
> + 若以上三点皆不满足，内核会 **根据文件的其他用户权限**，授予进程相应权限
> 

当然，对于程序来说，根据进程的 真实用户 ID 和组 ID 来检查对文件的访问权限，也并非没有可能

系统调用 `access()` 就是根据进程的真实用户 ID 和组 ID 以及附属组 ID，去检查对`pathname` 参数所指定文件的访问权限

```c
#include <unistd.h>

int access(const char *pathname, int mode);
/* 所有权限都支持返回 0，否则返回 -1*/
```

>[!tip] 参数 `mode`：位掩码
>
> 由下表常量的位或组成
>

| 常量     | 值   | 描述    |
| :----- | :-- | ----- |
| `F_OK` | 0   | 是文件吗？ |
| `R_OK` | 4   | 可读吗？  |
| `W_OK` | 2   | 可写吗？  |
| `X_OK` | 1   | 可执行吗？ |
 
### Set-User-ID Set-Group-ID 和 Sticky 

文件权限掩码还另设有 $3$ 个附加位，分别为 `set-user-ID` `set-group-ID` 和 `sticky` 位

> [!tip] set-user-ID 和 set-group-ID 用于创建特权级进程 
> 
> 另见 [[Linux 系统编程：进程凭证]]
> 

`set-group-ID` 位还有两种其他用途：对于在以 `nogrpid` 选项装配的目录下所新建的文件，**控制其群组从属关系**；可用于 **强制锁定文件**

在老的 UNIX 实现中，提供 `sticky` 位的目的在于让常用程序的运行速度更快。若对某程序文件设置了 `sticky` 位，则首次执行程序时，**系统会将其文本拷贝保存于交换区中**，即“粘”（stick）在交换区内，故而能 **提高后续执行的加载速度**

> [!tip] 
> 
> 现代 UNIX 实现对内存管理更为精准，这个用法以及过时了
> 
>  `Sticky` 权限位的常量名称： `S_ISVTX` 源于对 `sticky` 位的别称：`saved-text` 位
>  

> [!important] 现代 Unix/Linux 对 `sticky` 位的用法
> 作用于目录：限制删除
> 
> + 目录下的文件只能被特权用户或者文件的所有者删除
> 
> 此机制可以用来创建为多个用户共享的一个目录，各个用户可在其下创建或删除属于自己的文件，但不能删除隶属于其他用户的文件
> 

### 进程的文件权限掩码

创建文件或目录时指定的权限会被 **进程创建文件的权限掩码** 修改，

> [!tip] 
> 
> 进程创建文件的权限掩码称为 `umask`：这是一种 **进程属性**，当进程新建文件或目录时，该属性用于 **指明应屏蔽哪些权限位**
> 

进程的 `umask` 通常继承自其父 `shell`，其结果往往正如人们所期望的那样：用户可以使用 `shell` 的内置命令 `umask` 来改变 `shell` 进程的 `umask`，从而控制在 `shell` 下运行程序的 `umask`

大多数 `shell` 的初始化文件会将`umask` 默认置为八进制值 `022 `(`--- -w- -w-`)。其含义为对于同组或其他用户，应总是屏蔽写权限

> [!example] 
> 
> 假定 `open()` 调用中的 `mode` 参数为 `0666`（即令所有用户享有读、写权限，通常如此），那么对新建文件来说，其 **属主拥有读、写权限，所有其他用户只具有读权限**
> 

系统调用 `umask()` 将进程的 `umask` 改变为 `mask` 参数所指定的值

```c
#include <sys/stat.h>

mode_t umask(mode_t mask);
/* 总是成功的，返回以前的 mask*/
```

### 更改文件权限

可利用系统调用 `chmod()` 和 `fchmod()`去修改文件权限

```c
#include <sys/stat.h>

int chmod(const char *pathname, mode_t mode);

#define _XOPEN_SOURCE 500  /* #define _BSD_SOURCE*/
#include <sys/stat.h>

int fchmod(int fd, mode_t mode);

/* 成功返回 0； 错误返回 -1*/
```

系统调用 `chmod()` 更改由 `pathname` 参数所指定文件的权限。若该参数所指为符号链接，调用 `chmod()` 会改变符号链接所指代文件的访问权限，而非对符号链接自身的访问权限

> [!tip] 
> 
> **符号链接自创建起，其所有权限便为所有用户共享，且这些权限也不得更改**。对符号链接解引用时，将忽略所有这些权限
> 
