# System V 消息队列

**消息队列允许进程以消息的形式交换数据**。尽管消息队列在某些方面与管道和 FIFO 类似，但它们之间仍然存在显著的差别

> [!tip] 引用方式不同
> 
> 用来引用消息队列的句柄是一个由 `msgget()` 调用返回的标识符。这些标识符与 UNIX系统上大多数其他形式的 I/O 所使用的文件描述符是不同的
> 

> [!tip] 消息队列面向消息通信，管道面向字节流
> 
> 面向消息通信的特征: **读者接收到由写者写入的整条消息**。意味着，读取一条消息的一部分而让剩余部分遗留在队列中或一次读取多条消息都是不可能的
> 
 
> [!tip] 消息队列中的消息可以使用一个整数来表示类型
> 
> 除了包含数据之外，每条消息还有一个用整数表示的类型。从消息队列中读取消息既 **可以按照先入先出的顺序**，也 **可以根据类型来读取消息**
> 

## 创建或打开消息队列

`msgget()` 系统调用创建一个新消息队列或取得一个既有队列的标识符

```c
#include <sys/types.h>
#include <sys/msg.h>

int msgget(key_t key, int flags);
/* 成功返回消息队列的标识符，失败返回 -1*/
```

> [!tip] 参数 `key`
> 
> 该参数的取值通常是 [[Linux 系统编程：System V IPC 介绍#IPC Key]] 中介绍的 `IPC_PRIVATE` 或 `ftok()` 生成的 key
> 

> [!tip] 参数 `flags`：消息队列的权限或者控制操作
> 
> 权限设置参考 [[Linux 系统编程：文件属性#文件权限#普通文件的权限]] 
> 
> 此外还有两个控制 `msgget()` 行为的位掩码
> + `IPC_CREAT`: 使用 `key` 创建一个消息队列
> + `IPC_EXECL`: 与 `key` 关联的消息队列必须是调用进程创建的，否则调用失败并设置 `errno` 为 `EEXIST`
> 

如下示例程序：允许使用两个命令行参数指定 `msgget()` 的 `key` 和 `flags` 从而创建消息队列

```c title:svmsg/svmsg_create.c
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/msg.h>
#include <unistd.h>  // getopt()

#include "base.h"

/**
 * 提供使用选项
 * @param programName 程序名称
 * @param msg 错误消息
 */
static void usage(const char *programName, const char *msg)
{
    if(msg != nullptr)
    {
        fprintf(stderr, "%s", msg);
    }
    fprintf(stderr, "Usage: %s [-cx] {-f pathname | -k key | -p} [octal-perms]\n", programName);
    fprintf(stderr, "   -c              Use IPC_CREAT flag\n");
    fprintf(stderr, "   -x              Use IPC_EXCL flag\n");
    fprintf(stderr, "   -f pathname     Generate key using ftok()\n");
    fprintf(stderr, "   -p              Use IPC_PRIVATE flag\n");
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
    int numKeyFlags = 0;  // 统计 -f, -k, -p 选项
    int flags = 0, opt;
    long lkey;
    key_t key = 0;

    while ((opt = getopt(argc, argv, "cf:k:px")) != -1)
    {
        switch (opt) {
            case 'c':
                flags |= IPC_CREAT;
                break;
            case 'f':  // -f pathname
                key = ftok(optarg, 1);
                if(key == -1)
                {
                    errExit("ftok error: ");
                }
                numKeyFlags++;
                break;
            case 'k': // -k key(octal, decimal or hexademicmal)
                if(sscanf(optarg, "%li", &lkey) != 1)
                {
                    cmdLineErr("-k option requires a numeric argument\n");
                }
                key = (key_t)lkey;
                numKeyFlags++;
                break;
            case 'p':
                key = IPC_PRIVATE;
                numKeyFlags++;
                break;
            case 'x':
                flags |= IPC_EXCL;
                break;
            default:
                usage(argv[0], "Bad option\n");
        }
    }
    if(numKeyFlags != 1)
    {
        usage(argv[0], "Exactly one of the options -f, -k, or -p must be supplied\n");
    }
    int perms = (optind == argc) ? (S_IRUSR | S_IWUSR) : (int)strtol(argv[optind], nullptr, 8);
    int msqid = msgget(key, flags | perms);
    if(msqid == -1)
    {
        errExit("msgget error: ");
    }
    printf("%d\n", msqid);
    return 0;
}
```

## 交换消息

`msgsnd(msqid, &msgp)` 和 `msgrcv(msqid, &msgp)`系统调用执行消息队列上的 IO 操作
+ 第一个参数是 **消息队列标识符**(`msqid`)：表示操作的消息队列，通常是 `msgget()` 的返回值
+ 第二个参数 `msgp` 是一个由程序员定义的结构的指针，**该结构用于存放被发送或接收的消息**。这个结构的常规形式如下

```c
struct msg
{
	long int mtyp;
	char mtext[]; // 弹性数组成员
}
```

这个定义仅仅简要地说明了消息的第一个部分包含了消息类型，它用一个类型为 `long` 的整数来表示，而消息的剩余部分则是由程序员定义的一个结构，其长度和内容可以是任意的，而无需是一个字符数组。因此 **`mgsp` 参数的类型为 `void *`**，这样就允许传入任意结构的指针了。

**`mtext` 字段长度可以为零**，当对于接收进程来讲所需传递的信息仅通过消息类型就能表示或只需要知道一条消息本身是否存在时，这种做法有时候就变得非常有用了

### 发送消息

`msgsnd()` 系统调用向消息队列中写入一条消息

```c
#include <sys/types.h>
#include <sys/msg.h>

int msgsnd(int msqid, const void *msgp, size_t msgsz, int flags);
/* 成功返回 0； 错误返回 -1*/
```

> [!tip] 参数 `msqid`：消息队列标识符
> 
> 该参数通常是 `msgget()` 的返回值
> 

> [!tip] 参数 `msgp`: 指向消息的指针
> 
> 消息的数据结构通常是如下定义的
>
> ```c
> struct msg
> {
> 	long int mtyp;
> 	char mtext[]; // 弹性数组成员
> }
> ```
> 
> 其中 `msg.mtyp` 必须是一个大于 $0$ 的值；并将所需传递的信息复制到程序员定义的 `msg.mtext`字段中
> 

> [!tip] 参数 `msgsz`: `msg.mtext` 字段的字节数
> 

> [!tip] 参数 `flags`：控制 `msgsend()` 的行为
> 
> 只有一个标志:`IPC_NOWAIT`，即执行 **非阻塞消息发送**
> 
> 默认情况下，**当消息队列满时，`msgsnd()` 会阻塞** 直到队列中有足够的空间来存放这条消息
> 
> 但如果指定了 `IPC_NOWAIT` 标记，那么`msgsnd()` 就会立即返回并设置 `errno` 为 `EAGAIN`
> 

当 `msgsnd()` 调用因队列满而发生阻塞时可能会被 **信号处理器中断**。当发生这种情况时， `msgsnd()` 总是会返回并将 `errno` 设置为 `EINTR`

> [!WARNING] 注意
> 
> 被信号中断的`msgsnd()` 系统调用 **永远不会自动重启**，不管在建立信号处理器时是否设置了 `SA_RESTART` 标记
> 
> 向消息队列写入消息要求具备在该队列上的写权限
> 

下面的例程是像消息队列中发送消息的示例

```c title:svmsg/svmsg_send.c
#include<unistd.h>
#include<sys/msg.h>

#include "base.h"
#define MAX_MTEXT 1024

struct mbuf {
    long mtype;
    char mtext[MAX_MTEXT];
};

static void usage(const char * progName, const char *msg) {
    if(msg != nullptr) {
        fprintf(stderr, "%s", msg);
    }
    fprintf(stderr, "usage: %s [-n] msqid msg-type [msg-text]\n", progName);
    fprintf(stderr, "       -n      Use IPC_NOWAIT flag\n");
    exit(EXIT_FAILURE);
}


int main(int argc, char *argv[]){

    int opt;
    int flags = 0;
    while ((opt = getopt(argc, argv, "n")) != -1) {
        if(opt == 'n') {
            flags |= IPC_NOWAIT;
        } else {
            usage(argv[0], nullptr);
        }
    }
    if(argc < optind + 2 || argc > optind + 3) {
        usage(argv[0], "Wrong number of arguments\n");
    }
    // 从命令行获取 msqid
    int msqid = (int)strtol(argv[optind], nullptr, 10);

    // 获取消息类型和消息内容
    struct mbuf msg;
    msg.mtype = strtol(argv[optind + 1], nullptr, 10);
    size_t msgLen = 0;
    if(argc > optind + 2) {
        msgLen = strlen(argv[optind + 2]) + 1;
        if(msgLen > MAX_MTEXT) {
            cmdLineErr("msg-text too long(max: %d characters)\n", MAX_MTEXT);
        }
        memcpy(msg.mtext, argv[optind + 2], msgLen);
    } else {
        msgLen = 0;
    }

    // 发送消息
    if(msgsnd(msqid, &msg, msgLen, flags) == -1) {
        errExit("msgsnd error: ");
    }
    return 0;
}
```
### 接收消息

**`msgrcv()`** 系统调用从消息队列中读取（以及删除） 一条消息并将其 内容复制进 `msgp` 指向的缓冲区中

```c
#include <sys/types.h>
#include <sys/msg.h>

int msgrcv(int msqid, void *msgp, size_t maxmsgsz, long msgtype,int flags);
/* 成功返回 0； 错误返回 -1*/
```

> [!tip] 参数 `msqid`：消息队列标识符
> 
> 通常是 `msgget()` 的返回值
> 

> [!tip] 参数 `msgp`：指向存储消息的缓冲区的指针
> 
> `msgp` 通常指向如下的结构体
> 
> ```c
> struct msg
> {
> 	long int mtyp;
> 	char mtext[]; // 弹性数组成员
> }
> ```
> 
> `msg.mtyp` 会被先设置，用于选择消息的类型
> 

> [!tip] 参数 `maxmsgsz`：`msg.mtext` 的最大值
> 
> 如果队列中的消息长度超过了 `maxmsgsz` 大小，则 不会从消息队列中删除，并且 `msgrcv()` 立即返回并设置 `errno` 为 `E2BIG`
> 

> [!tip] 参数 `msgtype`：控制 `msgrcv()` 如何筛选消息的过程
> 
> 读取消息的顺序可以与发送消息的顺序不一致。根据 `msg.mtyp` 字段的值选择读取消息的类型。 `msgtype` 参数控制选择消息的过程
>

下表总结了 `msgtype` 不同取值的对消息的筛选过程

| `msgtype` | 描述                                                                                           |
| :-------- | :------------------------------------------------------------------------------------------- |
| $= 0$     | 删除消息队列中第一条消息，并通过 `msgp` 参数返回                                                                 |
| $\gt 0$   | 删除消息队列中消息类型为 `msgtype` 的第一条消息，并通过 `msgp` 参数返回                                                |
| $\lt 0$   | 消息队列当做优先队列(小根堆)使用。删除消息类型 `mtyp` 最小且小于或等于 $\vert \text{msgtype} \vert$ 的第一条消息，并通过 `msgp` 参数返回 |

> [!tip] 参数 `flags`：控制 `msgrcv()` 的行为
> 
> `IPC_NOWAIT` ：执行 **非阻塞的** 消息接收。默认情形下，消息队列中没有匹配 `msgtype` 的消息， `msgrcv()` 将会阻塞直到消息队列中出现匹配消息为止。
> + 指定 `IPC_NOWAIT` 时，`mgsrcv()` 如果确实阻塞了，那么也会立即返回并设置 `errno` 位 `ENOMSG`
>   
> `MSG_NOERROR`：执行 `msgrcv()` 操作时，如果消息队列中的消息长度超过了 `maxmsgsz`(即 `msg.mtext` 字段的最大可用空间)时，则删除该消息并将 **消息截断** 为 `msg.mtext` 的最大可用空间。被截去的数据将会丢失
> 

下面例程从消息队列中接收消息

```c title:svmsg/svmsg_recv.c
#include<unistd.h>  
#include<sys/msg.h>  
  
#include "base.h"  
  
#define MAX_MTEXT 1024  
  
struct mbuf {  
    long mtype;  
    char mtext[MAX_MTEXT];  
};  
  
static void usage(const char * progName, const char *msg) {  
    if(msg != nullptr) {  
        fprintf(stderr, "%s", msg);  
    }  
    fprintf(stderr, "usage: %s [options] msqid [max-bytes]\n", progName);  
    fprintf(stderr, "       -e          use MSG_NOERROR flag\n");  
    fprintf(stderr, "       -t type     select mysage of given type\n");  
    fprintf(stderr, "       -n          select IPC_NOWAIT of given type\n");  
#ifdef MSG_EXCEPT  
    fprintf(stderr, "             -x          use MSG_EXCEPT flag\n");  
#endif  
    exit(EXIT_FAILURE);  
}  
  
  
int main(int argc, char *argv[]){  
    int opt;  
    int flags = 0;  
    int type = 0;  
    while ((opt = getopt(argc, argv, "ent:x")) != -1) {  
        switch (opt) {  
            case 'e': flags |= MSG_NOERROR; break;  
            case 'n': flags |= IPC_NOWAIT; break;  
            case 't': type = (int)strtol(optarg, nullptr, 10); break;  
#ifdef MSG_EXCEPT  
            case 'x':   flags |= MSG_EXCEPT; break;  
#endif  
            default:  
                usage(argv[0], nullptr);  
        }  
    }  
    if(argc < optind + 1 || argc > optind + 2) {  
        usage(argv[0], "Wrong number of arguments");  
    }  
    int msqid = (int)strtol(argv[optind], nullptr, 10);  
    size_t maxBytes = (argc > optind + 1) ? strtol(argv[optind + 1], nullptr, 10) : MAX_MTEXT;  
  
    struct mbuf msg;  
    ssize_t msgLen = msgrcv(msqid, &msg, maxBytes, type, flags);  
    if(msgLen == -1) {  
        errExit("msgrcv error: ");  
    }  
    printf("Received: type=%ld, length=%ld", msg.mtype, msgLen);  
    if(msgLen>0) {  
        printf("; body=%s", msg.mtext);  
    }  
    printf("\n");  
    exit(EXIT_SUCCESS);  
}
```

下面的 shell 会话是 `svmsg_create.c` `svmsg_send.c` `svmsg_recv.c` 三个程序的使用示例

![Pasted image 20241108141258|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755783200012-b6ae1a2c6fec4eeb9d82cb62c2f81ae8.png)

## 消息队列控制操作

`msgctl()` 系统调用在标识符为 `msqid` 的消息队列上执行控制操作

```c
#include <sys/types.h>
#include <sys/msg.h>

int msgctl(int msqid, int cmd, struct msqid_ds *buf);
/*成功返回 0；错误返回 -1*/
```

> [!tip] 参数 `msqid`：消息队列的标识符
> 
> 通常是 `msgget()` 的返回值
> 

> [!tip] 参数 `cmd`：`msgctl` 执行的操作
> 
> + `IPC_RMID`：删除消息队列
> + `IPC_STAT`：获取 `msgid_ds` 结构副本
> + `IPC_SET`：设置 `msgid_ds` 
> 

## 消息队列关联数据结构

每个消息队列都有一个关联的 `msqid_ds` 数据结构，其形式如下

```c
struct msqid_ds
{
  struct ipc_perm msg_perm;	/* 用户和权限 */

  time_t msg_stime;		/* 上次 msgsnd() 时间 */
  time_t msg_rtime;		/* 上次 msgrcv() 时间 */
  time_t msg_ctime;		/* 上次状态改变时间 */

  unsigned long __msg_cbytes; /* 消息队列的空间(字节) */
  msgqnum_t msg_qnum;		/* 消息队列中的消息数量 */
  msglen_t msg_qbytes;		/* 消息队列被占用的空间(字节) */
  pid_t msg_lspid;		/* 上次执行 msgsnd() 进程的 PID */
  pid_t msg_lrpid;		/* 上次执行 msgrcv() 进程的 PID */
};
```

## 消息队列的限制


Linux 会对队列操作施加下列限制

|   限制常量   | 解释                          | 备注                                                                  |
| :------: | :-------------------------- | :------------------------------------------------------------------ |
| `MSGMNI` | 系统中所能创建的消息队列标识符的数量          | 系统级限制(`msgget(),ENOSPC`)                                            |
| `MSGMAX` | 单条消息中最多可写入的字节数(`mtext`的字节数) | 系统级限制(`msgsnd(),EINVAL`)                                            |
| `MSGMNB` | 最多保存的字节数总数(所有`mtext`字节数的和)  | 系统级限制(`msgsnd()`阻塞;  `msgsnd(..., IPC_NOWAIT)` 则立即返回并设置错误码`EAGAIN`) |

一些 UNIX 实现还定义了下列限制

|   限制常量    | 解释                       | 备注    |
| :-------: | :----------------------- | :---- |
| `MSGTQL`  | 系统中所有消息队列所能存放的消息总数       | 系统级限制 |
| `MSGPOOL` | 用来存放系统中所有消息队列中的数据的缓冲池的大小 | 系统级限制 |
