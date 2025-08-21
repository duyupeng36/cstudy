# System V 共享内存

在 [[操作系统：虚拟内存]] 中我们介绍过，不同进程的 **虚拟内存页 _映射_ 到不同的物理页帧上**，即便是相同的虚拟内存页也会映射到不同的物理内存页帧上。因此，进程之间的信息交换因为 **内存壁垒** 的存在不得不使用额外的机制来完成。在我们介绍了 [[Linux 系统编程：PIPE]] 和 [[Linux 系统编程：FIFO]] ，现在我们介绍 System V 共享内存

> [!tip] 
> 
> **共享内存** 就是一些物理内存页帧，这些页帧被不同的进程共享，即 **不同进程的某些虚拟内存页映射到共享物理内存页帧**
> 
> 换句话说，共享内存允许两个或多个进程共享物理内存的同一块区域（通常被称为段）
> 

![Pasted image 20241106232904|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755783133848-698053d738ec426ea57d0e7a7329d3f6.png)

由于一个 **共享内存段** 会成为一个 **进程用户空间内存的一部分**，因此这种 IPC 机制 **无需内核介入**

> [!important] 
> 
> 一个进程将数据拷贝到共享内存段中后，共享该内存段的所有进程对该数据均可见
> 

[[Linux 系统编程：PIPE]] 或者 [[Linux 系统编程：System V 消息队列]] 要求发送进程将数据从用户空间的缓冲区复制进内核内存和接收进程将数据从内核内存复制进用户空间的缓冲区。共享内存与之相比，是最快的 IPC 机制

> [!summary] 
> 
> 共享内存是最快的 IPC 机制，没有比共享内存更快的 IPC 机制了
> 

共享内存不由内核控制多个进程对共享内存访问可能出现如下 **数据竞争** 的情形

> [!example] 
> 
> + 两个进程同时执行更新操作
> 
> + 一个进程在从共享内存中获取数据的同时另一个进程正在更新这些数据
> 

为了避免上述数据竞争，需要使用额外的同步方法来避免上述情形。[[Linux 系统编程：System V 信号量]] 就是用于进程同步的方式

为使用一个共享内存段通常需要执行下面的步骤

+ 调用 `shmget()` 创建一个新共享内存段或取得一个既有共享内存段的标识符（即由其他进程创建的共享内存段）。这个调用将返回后续调用中需要用到的共享内存标识符
+ 使用 `shmat()` 来附上共享内存段，即使该段成为调用进程的虚拟内存的一部分
+ 使用由 `shmat()` 调用返回的 `addr` 值引用该共享内存，它是一个指向进程的虚拟地址空间中该共享内存段的起点的指针
+ 调用 `shmdt()` 来分离共享内存段。在这个调用之后，进程就无法再引用这块共享内存了。这一步是可选的，并且在进程终止时会自动完成这一步
+ 调用 `shmctl()` 来删除共享内存段。只有当当前所有附加内存段的进程都与之分离之后内存段才会被销毁。只有一个进程需要执行这一步

## 创建或打开一个共享内存段

`shmget()` 系统调用用于创建一个新共享内存段或者获取一个既有共享内存段的标识符。新创建的内存段中的内容会被初始化为 $0$ 

```c
#include <sys/types.h>
#include <sys/shm.h>

int shmget(key_t key, size_t size, int flgs);
/* 成功返回共享内存段标识符；失败返回 -1 */
```

> [!tip] 参数 `key`：获取共享内存段的键
> 
> 使用 [[Linux 系统编程：System V IPC 介绍#IPC Key]] 中介绍的 `IPC_PRIVATE` 或者由 `ftok()` 生成
> 

> [!tip] 参数 `size`：共享内存段的大小，字节为单位
> 
> 由于内核以 **系统分页大小的整数倍** 来分配共享内存。因此，`size` 会被提升到最近的系统分页大小的整数倍
> 
> 如果使用 `shmget()` 获取既有共享内存段，该参数不会有任何效果。但是，它必须要小于或等于段的大小
> 

> [!tip] 参数 `flgs`：指定共享内存段的权限，或者待检查的权限
> 
> 权限的指定参考 [[Linux 系统编程：文件属性#文件权限]]。此外，还有几个位掩码控制 `shmget()` 的行为
> + `IPC_CREAT`：创建一个新的共享内存段。如果已存在，则忽略
> + `IPC_EXECL`：与 `IPC_CREAT` 一起使用，表示必须是调用进程创建共享内存段。如果已存在，则返回 `EEXIST`
> 

Linux 还允许使用下列非标准标记

| flags 标记        | 描述                                                        |
| :-------------- | --------------------------------------------------------- |
| `SHM_HUGETLB`   | 创建一个使用 **巨页** ^[巨页是很多现代硬件架构提供的一项特性用来管理使用超大分页尺寸的内存] 的共享内存段 |
| `SHM_NORESERVE` | 与 `MAP_NORESERVE` 标记在 `mmap()` 中所起的作用一样                   |

> [!tip] 返回值
> 
> `shmget()` 在成功时返回新或既有共享内存段的 **标识符**
> 

## 使用共享内存

### 附加共享内存段

`shmat()` 系统调用将 `shmid` 标识的共享内存段附加到调用进程的虚拟地址空间中

```c
#include <sys/types.h>
#include <sys/shm.h>

void * shmat(int shmid, const void *addr, int flags);
/* 成功返回共享内存在进程虚拟地址空间中的首地址; 失败返回 (void *) -1 */
```

> [!tip] 参数 `flags`：控制 `shmat()` 的附近共享内存段行为
> 
> 参数 `flags` 是下列位掩码的位或
> 
> + `SHM_RDONLY`：附近只读段。试图更新会导致段错误，内核会向进程发送 `SIGSEGV` 信号
> + `SHM_REMAP`：替换参数 `addr` 处的任意既有映射
> + `SHM_RND`：将 `addr` 四舍五入到 `SHMLBA(hared memory low boundary address)` 字节的倍数
> 

> [!tip] 参数 `addr` 指定共享内存段附加的虚拟地址
> 
> 通常取 `NULL`。对于非 `NULL` 取值是不建议的
> 

> [!tip] 参数 `shmid`：共享内存段标识符
> 
> 该值是 `shmget()` 的返回值
> 

参数 `addr` 和 `flags` 控制着共享内存段时如何附加到调用进程的虚拟地址空间中的。下表进行了总结

| 参数 `addr` | 参数 `flags`     | 描述                                  |
| :-------- | :------------- | ----------------------------------- |
| `NULL`    | `SHM_RND`      | 内核选择合适的虚拟地址并附近共享内存段                 |
| `NULL`    | $0$            | 内核选择合适的虚拟地址并附近共享内存段                 |
| 非 `NULL`  | 没有包含 `SHM_RND` | 共享内存段被附加到 `addr` 指定的地址处             |
| 非 `NULL`  | `SHM_RND`      | 共享内存段被附近到 `addr` 舍入为 `SHMLBA`最近处    |
| 非 `NULL`  | `SHM_REMAP`    | 替换 `addr` 处的既有映射。`addr` 不允许为 `NULL` |
|           | `SHM_RDONLY`   | 共享内存段以供 **只读访问**                    |

> [!tip] 返回值
> 
> `shmat()` 返回附加共享内存段的虚拟地址。该地址可以 **像对待普通 C 指针那样使用**，共享内存段与进程的虚拟内存的其他部分看起来毫无差异

> [!important] 
> 
> 通过 `fork()` 创建的子进程会继承其父进程附加的共享内存段。因此，共享内存为父进程和子进程之间的通信提供了一种简单的 IPC 方法
> 

### 分离共享内存段

当一个进程不再需要访问一个共享内存段时就可以调用 `shmdt()` 来讲该段分离出其虚拟地址空间了。`addr` 参数标识出了待分离的段，它应该是由之前的 `shmat()` 调用返回的一个值

```c
#include <sys/types.h>
#include <sys/shm.h>

int shmdt(const void *addr);
/* 成功返回 0; 失败返回 -1 */
```

> [!tip] 分离共享内存段的其他方式
> 
> + 在一个 `exec()` 中，所有附加的共享内存段都会被分离
> + 在进程终止之后共享内存段也会自动被分离
> 

---

下面例程是使用共享内存传输数据的一种示例。这个例程由两个程序构成：**写者** 和 **读者**。

> [!tip] 写者 
> 
> 写者从标准输入中读取数据块并将数据复制（“写”）到一个共享内存段中
> 

> [!tip] 读者
> 
> 读者将共享内存段中的数据块复制（“读”）到标准输出中
> 

这里需要解决一个问题：写者和读者同时访问共享内存造成数据竞争问题。最好的方法就是使用 [[Linux 系统编程：System V 信号量]]，不过目前尚未学习。这里我们使用 [[Linux 系统编程：FIFO]] 作为同步工具。读者需要再写者写入之后才能读取共享内存

```c title:svshm/shm_writer.c
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "base.h"


int main(int argc, char *argv[]) {

    // shm_writer pathname proj
    if(argc != 3 || strcmp(argv[1], "--help") == 0) {
        usageErr("%s <pathname> <proj>\n", argv[0]);
    }
    struct stat st;
    if(stat(argv[1], &st) == -1) {
        errExit("%s stat error: ", argv[1]);
    }
    char *end;
    int proj = (int)strtol(argv[2], &end, 10);
    if(strcmp(argv[2], end) == 0) {
        errExit("%s is not a valid proj\n", argv[2]);
    }

    // 写者关闭 stdout 缓冲
    setvbuf(stdout, nullptr, _IONBF, 0);
    // 创建 FIFO 用于同步
    if(mkfifo("./sync.fifo", 0644) == -1) {
        errExit("mkfifo error: ");
    }
    printf("Writer[PID %ld] create sync fifo success\n", (long)getpid());

    // 写者：通过 FIFO 通知读者进程。因此，写者以 O_WRONLY 打开
    int sync = open("./sync.fifo", O_WRONLY);
    if(sync == -1) {
        errExit("open error: ");
    }
    printf("Writer[PID %ld] open sync fifo write side success\n", (long)getpid());

    // 创建共享内存的 key
    int shm_key = ftok(argv[1], proj);
    if(shm_key == -1) {
        errExit("ftok error: ");
    }

    // 创建共享内存
    int shm_id = shmget(shm_key, 4096, IPC_CREAT | S_IRUSR | S_IWUSR);
    if(shm_id == -1) {
        errExit("shmget error: ");
    }
    printf("Writer[PID %ld] create share memory success\n", (long)getpid());
    // 附加共享内存
    char *addr = shmat(shm_id, nullptr, SHM_RND);
    if(addr == (void *)-1) {
        errExit("shmat error: ");
    }
    printf("Writer[PID %ld] attach share memory success\n", (long)getpid());

    // 从标准输入读取并写入大共享内存
    char buffer[3072];
    printf("Writer[PID %ld] start read from stdin\n", (long)getpid());
    while (true) {
        printf("Writer[PID %ld] read: ", (long)getpid());
        memset(buffer, 0, sizeof(buffer));
        fgets(buffer, 3072, stdin);
        buffer[strlen(buffer) - 1] = '\0';
        if(strcmp(buffer, "exit") == 0 || strcmp(buffer, "quit") == 0 || strcmp(buffer, "q") == 0 || strcmp(buffer, "Q") == 0) {
            break;
        }
        snprintf(addr, 4096,"key=%#x, shm_id=%#x: %s\n", shm_key, shm_id, buffer);
        if(write(sync, " ", 1) != 1) {
            errExit("write error: ");
        }
    }
    printf("Writer[PID %ld] write success\n", (long)getpid());

    // 分离共享内存
    if(shmdt(addr) == -1) {
        errExit("shmdt error: ");
    }
    printf("Writer[PID %ld] detach share memory success\n", (long)getpid());
    // 关闭 FIFO
    if(close(sync) == -1) {
        errExit("close error: ");
    }
    printf("Writer[PID %ld] close sync fifo success\n", (long)getpid());

    // 删除共享内存
    if(shmctl(shm_id, IPC_RMID, nullptr) == -1) {
        errExit("shmctl error: ");
    }
    // 删除 FIFO
    if(unlink("./sync.fifo") == -1) {
        errExit("unlink error: ");
    }
    printf("Writer[PID %ld] delete sync fifo success\n", (long)getpid());
    return 0;
}
```

```c title:svshm/shm_reader.c
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "base.h"


int main(int argc, char *argv[]) {

    // shm_reader pathname proj
    if(argc != 3 || strcmp(argv[1], "--help") == 0) {
        usageErr("%s <pathname> <proj>\n", argv[0]);
    }
    struct stat st;
    if(stat(argv[1], &st) == -1) {
        errExit("%s stat error: ", argv[1]);
    }
    char *end;
    int proj = (int)strtol(argv[2], &end, 10);
    if(strcmp(argv[2], end) == 0) {
        errExit("%s is not a valid proj\n", argv[2]);
    }

    // 写者关闭 stdout 缓冲
    setvbuf(stdout, nullptr, _IONBF, 0);

    // 读者：接收写者同 FIFO 传递的消息
    int sync = open("./sync.fifo", O_RDONLY);
    if(sync == -1) {
        errExit("open error: ");
    }
    printf("Reader[PID %ld] open sync fifo read side success\n", (long)getpid());

    // 创建共享内存的 key
    int shm_key = ftok(argv[1], proj);
    if(shm_key == -1) {
        errExit("ftok error: ");
    }

    // 创建共享内存
    int shm_id = shmget(shm_key, 4096, IPC_CREAT | S_IRUSR | S_IWUSR);
    if(shm_id == -1) {
        errExit("shmget error: ");
    }
    printf("Reader[PID %ld] get share memory success\n", (long)getpid());
    // 附加共享内存
    char *addr = shmat(shm_id, nullptr, SHM_RND);
    if(addr == (void *)-1) {
        errExit("shmat error: ");
    }
    printf("Reader[PID %ld] attach share memory success\n", (long)getpid());

    // 从标准输入读取并写入大共享内存
    printf("Reader[PID %ld] start read from share memory\n", (long)getpid());
    while (true) {
        if(read(sync, (char[]){0}, 1) == 0) {
            break;
        }
        printf("%s",  addr);
    }
    printf("Reader[PID %ld] read success\n", (long)getpid());

    // 分离共享内存
    if(shmdt(addr) == -1) {
        errExit("shmdt error: ");
    }
    printf("Reader[PID %ld] detach share memory success\n", (long)getpid());
    // 关闭 FIFO
    if(close(sync) == -1) {
        errExit("close error: ");
    }
    printf("Reader[PID %ld] close sync fifo success\n", (long)getpid());
    return 0;
}
```

## 共享内存在虚拟内存中的位置

在 [[Linux 系统编程：进程属性]] 和 [[操作系统：虚拟内存|操作系统：虚拟内存]] 中介绍了一个进程的各个部分在虚拟内存中的布局。如果遵循所推荐的方法，即 **允许内核选择在何处附加共享内存段**，那么（在 x86-32 架构上）内存布局就会像下图中所示的那样，共享内存段被附加在向上增长的堆和向下增长的栈之间未被分配的空间中。为给堆和栈的增长腾出空间，附加共享内存段的虚拟地址从`0x40000000` 开始

![Pasted image 20241107153620|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755783133849-356b8a9c2d344e48b901a30a85c0a4f1.png)

> [!tip] 
> 
> **共享内存段映射在堆和栈之间**，为给堆和栈的增长腾出空间，附加共享内存段的虚拟地址从`0x40000000` 开始
> 
> **[[Linux 系统编程：内存映射]] 和 [[Linux 系统编程：库基础#共享库]] 也是被放置在这个区域中的**
> 
> **内存映射** 和 **共享内存段** 默认被放置的位置可能会有些不同，这依赖于内核版本和进程的 `RLIMIT_STACK` 资源限制的设置
> 

> 地址 `0x40000000` 被定义成了内核常量 `TASK_UNMAPPED_BASE`。通过将这个常量定义成一个不同的值并且重建内核可以修改这个地址的值

## 在共享内存中存储指针

每个进程都可能会用到不同的共享库和内存映射，并且可能会附加不同的共享内存段集。因此如果遵循推荐的做法，**让内核来选择将共享内存段附加到何处**，那么 **一个段在各个进程中可能会被附加到不同的地址上**

正因为这个原因，**在共享内存段中存储指向段中其他地址的引用时应该使用（相对）偏移量，而不是（绝对）指针**

假设一个共享内存段的起始地址为 `baseaddr`（即 `baseaddr` 的值为 `shmat()` 的返回值）。再假设需要在 `p` 指向的位置处存储一个指针，该指针指向的位置与 `target` 指向的位置相同。如下图所示

![Pasted image 20241107154049|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755783133850-dbad58eb744843c6921bc304de1451cf.png)

如果在段中构建一个链表或二叉树，那么这种操作就是非常典型的一种操作。在 C 中设置`*p` 的传统做法如下所示

```c
*p = target
```

上面这段代码存在的问题是 **当共享内存段被附加到另一个进程中时 `target` 指向的位置可能会位于一个不同的虚拟地址处**，这意味着在那个进程中那个策划中存储在 `*p` 中的值是是无意义的。**正确的做法是在 `*p` 中存储一个偏移量**，如下所示

```c
*p = (target - baseaddr);
```

在解引用这种指针时需要颠倒上面的步骤

```c
target = baseaddr + *p;
```

**这里假设在各个进程中 `baseaddr` 指向共享内存段的起始位置**（即各个进程中` shmat()` 的返回值）。给定这种假设，**那么就能正确地对偏移量值进行解释**，不管共享内存段被附加在进程的虚拟地址空间中的何处。

## 共享内存控制

`shmctl()`系统调用在 `shmid` 标识的共享内存段上执行一组控制操作

```c
#include <sys/types.h>
#include <sys/shm.h>

int shmctl(int shmid, int cmd, struct shmid_ds *buf);
/* 成功返回 0; 失败返回 -1 */
```

> [!tip] 参数 `cmd`：控制 `shmctl()` 的行为
> 

下表对 `shmctl` 的 `cmd` 参数进行了总结

|     cmd      | 描述                             | 备注                            |
| :----------: | :----------------------------- | ----------------------------- |
|  `IPC_RMID`  | 标记共享内存段及其关联 `shmid_ds` 数据结构被删除 | 只有当前没有进程附加该段，才会执行删除操作         |
|  `IPC_STAT`  | 获取共享内存关联 `shmid_ds` 数据结构       |                               |
|  `IPC_SET`   | 重新设置共享内存关联的 `shmid_ds` 数据结构    | 需要第三个参数 `buf`                 |
|  `SHM_LOCK`  | 将一个共享内存段锁进内存，不允许缺页处理程序将其换出内存   | 加锁后，如果被换出的共享内存页再次换入，也不会被锁在内存中 |
| `SHM_UNLOCK` | 允许缺页处理程序将其换出内存                 |                               |

上表中的前 $3$ 个 `cmd` 是常规控制操作，与 [[Linux 系统编程：System V 信号量]] [[Linux 系统编程：System V 消息队列]] 上的操作完全一样
+ `IPC_RMID`：删除 System V IPC 对象
+ `IPC_STAT`：获取 `shmid_ds` 结构
+ `IPC_SET`：设置 `shmid_ds` 结构

最后 $2$ 个 `cmd` 是用于将共享内存段锁定在 RAM 中，这样它就永远不会被交换出去了
+ `SHM_LOCK`：将共享内存段锁定到 RAM 中
+ `SHM_UNLOCK`：解除共享内存段的锁定

## 共享内存关联数据结构

每个共享内存段都有一个关联的 `shmid_ds` 数据结构，其形式如下

```c
struct shmid_ds {
	struct ipc_perm shm_perm; /* 用户和权限 */
	size_t shm_segsz;         /* 内存段大小，字节为单位 */
	time_t shm_atime;         /* 上次 shmat() 时间 */
	time_t shm_dtime;         /* 上次 shmdt() 时间 */
	time_t shm_ctime;         /* 上次状态改变时间 */
	pid_t  shm_cpid;          /* 创建者 PID */
	pid_t shm_lpid;           /* 上次 shmat()/shmdt() PIC */
	shmatt_t shm_nattch;      /* 当前附加进程的数量 */
};
```

## 共享内存的限制

 Linux 共享内存的限制列表。括号中列出了当限制达到时 **受影响的系统调用** 及其 **返回的错误**

| 限制       | 描述                                           |
| :------- | :------------------------------------------- |
| `SHMMNI` | 共享内存标识符的数量。`shmget()` 会设置 `errno` 为 `ENOSPC` |
| `SHMMIN` | 共享内存段的最小大小。`shmget()` 会设置 `errno` 为 `EINVAL` |
| `SHMMAX` | 共享内存段的最大大小。`shmget()` 会设置 `errno` 为 `EINVAL` |
| `SHMALL` | 共享内存中的分页总数。`shmget()` 会设置 `errno` 为 `ENOSPC` |
| `SHMSEG` | 进程能附加的共享内存段数量                                |
