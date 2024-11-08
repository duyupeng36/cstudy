# System V 信号量

System V 信号量是 **进程同步** 技术。所谓进程同步就是**控制进程对 _共享资源_ 的访问顺序**

> [!tip] 
> 
> + 共享内存，参考 [[System V 共享内存]]
> + 共享文件映射，参考 [[内存映射#文件映射#共享文件映射]]
> + 共享匿名映射，参考 [[内存映射#匿名映射]]
> 

信号量的一个常见用途是 **同步对一块共享内存的访问** 以防止出现一个进程在访问共享内存的同时另一个进程更新这块内存的情况。

**信号量是由内核维护的整数**，其值被限制为大于或等于 $0$。在一个信号量上可以执行各种操作

+ 将信号量设置成一个绝对值
+ 在信号量当前值的基础上加上一个数量
+ 在信号量当前值的基础上减去一个数量
+ 等待信号量的值等于 $0$

> [!important] 
> 
> 最后两个操作可能会导致调用进程阻塞
> 
> + 当减小一个信号量的值时，内核会将所有试图将信号量值降低到  $0$ 之下的操作阻塞
> + 如果信号量的当前值不为 ，那么等待信号量的值等于 $0$  的调用进程将会发生阻塞
> 

调用进程被阻塞时就会保持直到其他一些进程将信号量的值修改为一个允许这些操作继续向前的值，在那个时刻内核会唤醒被阻塞的进程。下图显示了使用一个信号量来同步两个交替将信号量的值在 $0$ 和 $1$ 之间切换的进程的动作

![[Pasted image 20241107173735.png]]

> [!important] 
> 
> 在控制进程的动作方面，**信号量本身并没有任何意义**，它的意义 **仅由使用信号量的进程赋予其的关联关系来确定**
> 
> 一般来讲，**进程之间会达成协议将一个信号量与一种共享资源关联起来**，如一块共享内存区域
> 
> 从语义上讲，**增加信号量值** 表示其 **关联的共享资源变得可用**，其他进程可以使用该共享资源。**减小信号量值** 表示其 **关联共享资源被预先占用**，其他进程需要等待该共享资源被释放
> 
> 信号量还有其他用途，如在 `fork()`之后同步父进程和子进程
> 

使用 System V 信号量的常规步骤如下
+ 使用 `semget()` 创建或打开一个信号量集
+ 使用 `semctl()` `SETVAL` 或 `SETALL` 操作初始化集合中的信号量。注意，该操作只有一个进程执行
+ 使用 `semop()` 操作信号量值。使用信号量的进程通常会使用这些操作来表示一种共享资源的获取和释放
+ 当所有进程都不再需要使用信号量集之后使用` semctl()` `IPC_RMID` 操作删除这个集合。注意，该操作只有一个进程执行

> [!tip] 
> 
> System V 信号量比较复杂，因为它们的分配是以备称为 **信号量集** 的组为单位进行的
> 
> 在使用 `semget()` 系统调用创建集合的时候需要指定集合中的 **信号量数量**。
> 
> 虽然 **在同一时刻通常只操作一个信号量**，但通过 `semop()` 系统调用可以原子地在同一个集合中的多个信号量之上执行一组操作
> 

由于 System V 信号量的创建和初始化是在不同的步骤之后完成的，因此当两个进程同时都试图执行这两个步骤时就会出现竞争条件

## 创建或打开一个信号量集

`semget()` 系统调用创建一个新信号量集或获取一个既有集合的标识符

```c
#include <sys/types.h>
#include <sys/sem.h>

int semget(key_t key, int nsems, int flags);
/* 成功返回信号量标识符；失败返回 -1 */
```

> [!tip] 参数 `key`：IPC key
> 
> 使用 [[System V IPC 介绍]] 中描述的 `IPC_PRIVATE` 或者 `ftok()` 生成
> 

> [!tip] 参数 `nsems`: 信号量集中信号量的数量
> 
> 创建信号量集时，`nsems` 必须大于 $0$
> 
> 获取既有信号量集时，`nsems` 必须小于或等于既有信号量集的大小，否则设置 `errno` 位 `EINVAL`。**无法修改一个既有集中的信号量数量**
> 

> [!tip] 参数 `flags`：指定权限，或控制 `semget()` 的行为
> 
> 权限参考 [[文件属性#文件权限#普通文件的权限]]
> 
> 控制 `semget()` 行为的位掩码有两个
> + `IPC_CREAT`：创建一个 IPC 对象与 `key` 关联
> + `IPC_EXECL`：必须不存在与 `key` 关联的 IPC 对象
> 

> [!tip] 返回值
> 
> `semget()` 在成功时会返回 **信号量集的标识符**，无论是新建的还是既有的
> 

## 信号量控制操作

`semctl()` 系统调用在一个 **信号量集** 或集合中的 **单个信号量** 上执行各种控制操作

```c
#include <sys/types.h>
#include <sys/sem.h>

int semctl(int semid, int semnum, int cmd, .../* union semun arg */);
/* 成功返回非负值;失败返回 -1 */
```

> [!tip] 参数 `semid`：指定需要操作信号量集的标识符
> 
> 该参数通常是 `semget()` 的返回值
> 

> [!tip] 参数 `semnum`：信号量集中第 `semnum` 个信号量
> 
> 在 **单个信号量** 上执行控制操作时，`semnum` 标识出了集合中的具体信号量
> 
> 其他操作则忽略该参数，并且可以设置为 $0$
> 

> [!tip] 参数 `cmd`：指定具体的控制操作；参数 `arg`：某些操作需要的额外参数
> 
> 下表列出 `cmd` 支持的具体操作。某些操作需要使用第 $4$ 个参数 `arg`，该参数需要我们自行定义如下
> 
> ```c
> #include <sys/types.h>
> #include <sys/sem.h>
> union semun
> {
> 	int  val;
> 	struct semid_ds *buf;
> 	unsigned short *array;
> 	#if defined(__linux__)
> 	struct seminfo * __buf;
> 	#endif
> };
> ```

| `cmd`      | 描述                                                 |
| :--------- | :------------------------------------------------- |
| 常规操作       | System V IPC 对象均支持。`semnum` 参数被忽略                  |
| `IPC_RMID` | 删除信号量集及其关联的数据结构.<br>所有等待该信号量集的进程都会被唤醒              |
| `IPC_STAT` | 获取 `semid_ds` 结构体副本                                |
| `IPC_SET`  | 修改 `semid_ds` 结构体中的值                               |
|            |                                                    |
| 获取和初始化信号量值 | 获取或初始化一个集合中的单个或所有信号量的值                             |
| `GETVAL`   | 获取信号量集中的第 `semnum` 个信号量值                           |
| `SETVAL`   | 初始化信号量集中的第 `semnum` 个信号量为 `arg.val`                |
| `GETALL`   | 获取信号量集中的所有信号量值，存放在 `arg.array` 中                   |
| `SETALL`   | 使用 `arg.array` 初始化信号量集                             |
|            |                                                    |
| 获取单个信号量的信息 | 返回信号量集中第 `semnum` 个信号量的信息。无需 `arg` 参数              |
| `GETPID`   | 上一个在第 `semnum` 个信号量上执行 `semop()` 进程的 PID(`sempid`) |
| `GETNCNT`  | 当前等待该信号量的值增长的进程数(`semncnt`)                        |
| `GETZCNT`  | 当前等待该信号量的值变成 $0$ 的进程数(`senzcnt`)                   |

如果存在一个进程正在等待在由 `SETVAL` 或 `SETALL` 操作所修改的信号量上执行一个操作并且对信号量所做的变更将允许该操作继续向前执行，那么内核就会唤醒该进程

使用或 `SETALL` 修改一个信号量的值会在所有进程中清除该 **信号量的撤销条目**

> [!tip] **检查时** 和 **使用时** 的竞争条件：依赖操作返回值不变
> 
> `GETVAL` 和 `GETALL` 返回的信息在调用进程使用它们时可能已经过期了
> 
> `GETPID` `GETNCNT` `GETZCNT` 操作的返回信息在调用进程使用它们时已过期
> 

## 信号量关联数据结构

每个信号量集都有一个关联的 `semid_ds` 数据结构，其形式如下

```c
struct semid_ds {
	struct ipc_perm sem_perm;  /* 用户和权限 */
	time_t          sem_otime; /* 上次执行 semop() 的时间 */
	time_t          sem_ctime; /* 上次状态改变事件 */
	unsigned long   sen_nsems; /* 集合中的信号量数量 */
};
```

## 信号量初始化细节

SUSv3 并未要求实现对 `semget()` 创建的信号量集中的信号量值进行初始化。因此，必须使用 `semctl()` 显示初始化信号量

> 在 Linux 中，`semget()` 返回的信号量集中的信号量初始化为 $0$，但是为了可移植性不能依赖具体实现

> [!attention] 
> 
> 由于信号量的创建和初始化必须要通过单独的系统调用而不是单个原子步骤来完成的事实可能会导致在初始化一个信号量时出现竞争条件
> 

假设一个应用程序由多个 **地位平等的进程构成**，这些进程使用一个 **信号量来协调相互之间的动作**。由于 **无法保证哪个进程会首先使用信号量**（这就是地位平等的含义），因此 **每个进程都必须要做好在信号量不存在时创建和初始化信号量的准备**

下面的代码片段是一个 **错误的** 初始化 System V 信号量的方法

```c

semid = semget(key, 1, IPC_CREAT | IPC_EXECL | perms);  // 第一次调用 semget

if(semid != -1) {  // 信号量集创建成功
	union semun arg;
	/* XXXX：时间片到 */
	arg.val = 0;  // 初始信号量
	if (semctl(semid, 0, SETVAL, arg) == -1) {
		// 初始化错误
	}
} else {
	// 信号量集创建失败
	if(errno != EEXIST) {
		// 不是因为信号量集已存在的错误
	}
	
	// 信号量集存在
	semid = semget(key, 1, perms);  // 第二次调用 semget
	if(semid == -1) {
		// 获取信号量集失败
	}
}

// 对信号量进行操作
sops[0].sem_op = 1; // 信号量 +1
sops[0].sem_num = 0; // 编号为 0 的信号量
sops[0].sem_flg = 0; // 不指定标记

if(semop(semid, sops, 1) == -1) {
	// 操作错误
}
```

如果两个进程同时执行，如果第一个进程的时间片在代码中标记为 `/* XXXX */`  处期满(时间片到)，那么就会切换到第二个进程开始执行。执行顺序可能如下图

![[Pasted image 20241107211605.png]]

> [!question] 这个执行顺序的问题如下
> 
> + 进程 B 在一个未初始化的信号量上执行了 `semop()` 操作
> 
> + 进程 A 中 `semctl()` 调用覆盖了进程 B 做出的变更
> 

这个问题的解决方案依赖于一个现已成为标准的特性，即与这个信号量集相关联的 `semid_ds` 数据结构中的 `sem_otime` 字段的初始化

> [!tip] 特性：信号量集首次被创建时，`semid_ds.sem_otime` 字段被初始化为 $0$，并且只有后续 `semop()` 调用才会修改该字段的值
> 

因此可以利用这个特性来消除上面描述的竞争条件，即只需要插入额外的代码来强制第二个进程（即没有创建信号量的那个进程）等待直到第一个进程既初始化了信号量又执行了一个更新 `sem_otime` 字段但不修改信号量的值的 `semop()` 调用为止

```c title:svsem/svsem_good_init.c

semid = semget(key, 1, IPC_CREAT | IPC_EXECL | perms);

if(semid != -1) {  // 成功创建
	union semun arg;
	struct sembuf sop;
	
	arg.val = 0; // 信号初始值为 0
	if(semctl(semid, 0, SETVAL, arg) == -1) {
		// 初始化错误
	}
	// 执行一个空操作，改变 semid_ds.sem_otime 字段，让其他进程看到我们已经初始化信号集
	sop.sem_num = 0; // 操作编号为 0 的信号量
	sop.sem_op = 0;  // 检查信号量是否等于 0
	sop.sem_flg = 0;
	if(semop(semid, &sop, 1) == -1) {
		// 操作错误
	}
} else {
	// 创建失败
	const int MAX_TRIES = 10;
	int j;
	union semun arg;
	struct semid_ds ds;
	
	if(errno != EEXIST) {
		// 非 EEXIST 错误
	}
	semid = semget(key, 1, perms); // 获取已存在的信号量集标识符
	if(semid == -1) {
		// 获取失败
	}
	
	// 等待其他进程调用 semop(): 尝试 MAX_TRIES 次，每秒尝试 1 次
	arg.buf = &ds;
	for(j = 0; j < MAX_TRIES; j++) {
		if(semctl(semid, 0, IPC_STAT, arg) == -1) {
			// 获取信号编号为 0 的信号量错误
		}
		if(ds.sem_otime != 0) {
			break;
		}
		sleep(1);
	}
	
	if(ds.sem_otime == 0) {
		// 10 s 内都没有其他进程初始化完成信号量集
	}
}
// 其他操作
```

## 信号量操作

`semop()` 系统调用在 `semid` 标识的信号量集中的信号量上执行一个或多个操作

```c
#include <sys/types.h>
#include <sys/sem.h>

int semop(int semid, struct sembuf *sops, unsigned int nsops);
/*成功返回 0；失败返回 -1*/
```

> [!tip] 参数 `semid`: 指定信号量所在的信号量集
> 


> [!tip] 参数 `nsops`: `sembuf` 结构体数组的大小
> 
> `sembuf` 结构体数组至少包含 $1$ 个元素
> 

> [!tip] 参数 `sops`：指向一个 `sembuf` 结构体数组的指针
> 
> `sembuf` 结构体指定执行的操作。`sembuf` 结构体定义如下
>
> ```c
> struct sembuf
> {
> 	unsigned short  sem_num;   // 信号量编号
> 	short           sem_op;    // 要执行的操作
> 	short           sem_flg;   // 操作flags(IPC_NOWAIT 和 SEM_UNDO)
> };
> ```
> `sembuf.sem_num` 字段：信号集中信号编号 
>
> `sembuf.sem_op` 字段：对信号量的操作
> + `> 0` 表示将 `sembuf.sem_op` 加到信号量值上。操作完成时，其他等待信号量值减小的进程可能被唤醒
> + `= 0` 表示检查信号量值是否为 $0$
> 	+ 是：立即结束
> 	+ 否：阻塞等待
> + `< 0` 表示在信号量值上减去 `sembuf.sem_op` 
> 	+ 信号量值如果大于 `|sembuf.sem_op|` 立即结束；否则阻塞等待
> 
> `sembuf.sem_flg` 字段：控制该操作
> + `IPC_NOWAIT`：在编号为 `sem_num` 的信号量上执行非阻塞操作。如果该操作会阻塞，则立即返回并设置 `errno` 为 `EAGAIN`
> 
> 操作将会按照在数组中的顺序以 **原子的** 方式被执行
> 

> [!attention] 当 `semop()` 调用阻塞时，进程会保持阻塞直到发生下列某种情况为止
> 
> + 另一个进程修改了信号量值使得待执行的操作能够继续向前
> 
> + 一个信号中断了 `semop()` 调用。此时 `semop()` 立即返回并设置 `errno` 为 `EINTR`
> 	+ `semop()` 调用被信号中断后不会自动启动
> 
> + 另一个进程删除了 `semid` 引用的信号量。发生这种情况时 `semop()`会立即返回并设置 `errno` 为 `EIDRM`
> 

在特定信号量上执行一个操作时可以通过在相应的 `sem_flg` 字段中指定 `IPC_NOWAIT` 标记来防止 `semop()` 阻塞。此时，如果 `semop()` 本来要发生阻塞的话就会立即返回并将 `errno` 设置为 `EAGAIN`

> [!important] `semop()` 调用时原子的，如果本次调用操作多个信号量，要么都成功，要么都不成功 
> 
> 尽管通常一次只会操作一个信号量，但也可以通过一个 `semop()` 调用在一个集合中的多个信号量上执行操作。这里需要指出的关键一点是这组操作的执行是原子的，即 `semop()` 要么立即执行所有操作，要么就阻塞直到能够同时执行所有操作
> 

如下示例代码片段：使用 `semop()` 在一个集合中的 $3$ 个信号量上执行操作

```c
struct sembuf sops[3];

memset(sops, 0, sizeof(sops));

sops[0].sem_num = 0;
sops[0].sem_op = -1;
// 操作信号量 0 被阻塞，整个操作被阻塞

sops[1].sem_num = 1;
sops[1].sem_op = 2;
// 操作信号量 1 被阻塞，整个操作被阻塞

sops[2].sem_num = 2;
sops[2].sem_op = 0;
sops[2].sem_flg = IPC_NOWAIT;
// 操作信号量 2 被阻塞，其他操作也不会执行，并且立即返回并设置 errno 为 EAGAIN

if(semop(semid, sops, 3) == -1) {
	if(errno == EAGAIN) {
		// 信号量 2 被阻塞
	} else {
		// 其他错误
	}
}
```

### 设置超时

`semtimedop()` 系统调用与 `semop()` 执行的任务一样，但它多了一个 `timeout` 参数，通过这个参数可以指定调用所阻塞的时间上限。

```c
#define _GNE_SOURCE
#include <sys/types.h>
#include <sys/sem.h>

int semtimedop(int semid, struct sembuf *sops, unsigned int nsops, struct timespec *timeout);
```

如果信号量操作完成之间所等待时间已超过了规定的时间间隔，那么 `setimedop()` 会返回，并设置 `errno` 为 `EAGAIN`

### 示例程序

该程序为 `semop()` 系统调用提供了一个命令行界面。这个程序接收的 **第一个参数是操作所施加的信号量集的 _标识符_**

剩余的命令行参数指定了在单个 `semop()` 调用中需要执行的一组信号量操作。单个命令行参数中的操作使用逗号分隔。每个操作的形式为下面中的一个
+ `semnum+value`：将 `value` 加到第 `semnum` 个信号量上
+ `semnum-value`：从第 `semnum` 个信号量上减去 `value`
+ `semnum=0`：测试第 `semnum` 信号量以确定它是否等于 $0$

在每个操作的最后可以可选地包含一个 `n`, 一个`u`  或同时包含两者
+ 字母 `n` 表示在这个操作的 `sem_flg` 值中包含 `IPC_NOWAIT`
+ 字母 `u` 表示在 `sem_flg` 中包含 `SEM_UNDO`

下面的命令行在标识符为 $0$ 的信号量集上执行了两个 `semop()` 调用

```shell
$ ./svsem_op 0 0=0 0-1,1-2n
# ./svsem_op <sem_id> semnum=0[n|u] semnum-value[n|u],semnum+value[n|u]
```

+ 第一个命令行参数规定 `semop()` 调用等待直到第一个信号量值等于 $0$ 为止

+ 第二个参数规定 `semop()` 调用从信号量 $0$ 中减去 $1$ 以及从信号量 $1$ 中减去 $2$。信号量 $0$ 上的操作的 `sem_flg` 为 $0$，信号量 $1$ 上的操作的 `sem_flg` 是 `IPC_NOWAIT`

```c title:svsem/svsem_op.c

#include <ctype.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <unistd.h>

#include "base.h"
#include "current_time.h"

#define MAX_SEMOPS 1000 /* 单次 semop() 调用操作信号量的最大值 */

static int parseOps(char *arg, struct sembuf *ops) {
    char *comma, *sign, *remaining, *flags;
    int numOps;
    for(numOps = 0, remaining = arg; ; numOps ++) {
        if(numOps >= MAX_SEMOPS) {
            cmdLineErr("Too many operations(maximum = %d): \"%s\"\n", MAX_SEMOPS, arg);
        }

        // <sem_num><+|-|=><value>[n][u],<sem_num><+|-|=><value>[n][u],
        if(*remaining == '\0') {
            fatal("Trailing comma or empty argument: \"%s\"", arg);
        }
        if(!isdigit((unsigned char) *remaining)) {
            cmdLineErr("Expected initial digit: \"%s\"\n", arg);
        }

        ops[numOps].sem_num = strtol(remaining, &sign, 10);  // 获取信号量编号
        // 检查剩余字符串
        if(*sign == '\0' || strchr("+-=", *sign) == NULL) {
            cmdLineErr("Expected '+', '-' or '=' in \"%s\"\n", arg);
        }
        if(!isdigit((unsigned char) *(sign + 1))) {
            cmdLineErr("Expected digit after '%c' in \"%s\"\n",*sign, arg);
        }
        ops[numOps].sem_op = (short)strtol(sign + 1, &flags, 10);
        if(*sign == '-') {
            ops[numOps].sem_op = (short)-ops[numOps].sem_op;
        }
        else if(*sign == '=') {
            if(ops[numOps].sem_op != 0) {
                cmdLineErr("Expected \"=0\" in \"%s\"\n", arg);
            }
        }
        ops[numOps].sem_flg = 0;
        for(;;flags++) {
            if(*flags == 'n') {
                ops[numOps].sem_flg |= IPC_NOWAIT;
            } else if(*flags == 'u') {
                ops[numOps].sem_flg |= SEM_UNDO;
            } else {
                break;
            }
        }
        if(*flags != ',' && *flags != '\0') {
            cmdLineErr("Bad trailing charactor (%c) in \"%s\"\n",*flags, arg);
        }
        comma = strchr(remaining, ',');
        if(comma == NULL) {
            break;
        }
        remaining = comma + 1;
    }
    return numOps + 1;
}

int main(int argc, char *argv[]) {

    if(argc < 2 || strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "Usage: %s semid op[,op...] ...\n", argv[0]);
        fprintf(stderr, "'op' is either: <sem#>{+|-}<value>[n][u]\n");
        fprintf(stderr, "            or: <sem#>=0[n][u]\n");
        fprintf(stderr, "       \"n\" means include IPC_NOWAIT in 'op'\n");
        fprintf(stderr, "       \"u\" means include SEM_UNDO in 'op'\n\n");
        fprintf(stderr, "The operations in each argument are performed in a signal semop() call\n\n");
        fprintf(stderr, "e.g. %s 12345 0+1,1-2un\n", argv[0]);
        fprintf(stderr, "     %s 12345 0=0n 1+1,2-1u 1=0\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    struct sembuf sops[MAX_SEMOPS];

    for(int ind = 2; argv[ind] != NULL; ind ++) {
        memset(sops, 0, sizeof(sops));
        int nsops = parseOps(argv[ind], sops);
        printf("%5ld, %s: about to semop() [%s]\n", (long) getpid(), current_time(),argv[ind]);
        int semid = (int)strtol(argv[1], nullptr, 10);
        if(semop(semid, sops, nsops) == -1) {
            errExit("semop (PID %ld) error: ", (long) getpid());
        }
        printf("%5ld, %s: semop() completed [%s]\n", (long) getpid(), current_time(), argv[ind]);
    }
    exit(EXIT_SUCCESS);
}
```

## 多个阻塞信号量操作的处理

如果有 **多个进程对同一个信号量 _减去相同值_ 而阻塞**，那么 **条件允许时这些被阻塞的进程那个先执行是不确定**，因为哪个进程执行依赖于内核的调度算法

如果有 **多个进程对同一个信号量 _减去不相同值_ 而阻塞**，那么会按照 **先满足条件先服务** 的顺序来进行。这种情形下可能会造成进程饿死

> [!tip] 饿死情形 $1$：一个进程 **因信号量的状态无法满足所请求的操作** 继续往前执行的条件而永远保持阻塞
> 
> 假设一个信号量的当值为 $0$，进程 A 请求将信号量值减去 $2$，然后进程 B 请求将信号量值减去 $1$。如果第三个进程将信号量值加上了 $1$，那么进程B 首先会被解除阻塞并执行它的操作，即使进程 A 首先请求在该信号量上执行操作也一样
> 

> [!tip] 饿死情形 $2$: 当一个进程因 **试图在多个信号量上执行操作** 而发生阻塞时也可能会出现饿死的情况
> 
> 两个信号量的初始值都为 $0$
> + 进程 A 请求将信号量 $0$ 和 信号量 $1$ 的值减去 $1$（阻塞）
> + 进程 B 请求将信号量 $0$ 的值减去 1（阻塞）
> + 进程 C 将信号量 $0$ 的值加上 $1$
> 
> 此刻，进程 B 解除阻塞并完成了它的请求，即使它发出请求的时间要晚于进程 A。同样，也可以设计出一个让进程 A 饿死的同时让其他进程调整和阻塞于单个信号量值的场景
> 

##  信号量撤销值

假设一个进程在 **调整完一个信号量值（如减小信号量值使之等于 $0$）之后终止** 了，不管是有意终止还是意外终止。**在默认情况下，信号量值将不会发生变化**

> [!question] 调整完信号量值之后终止带来的问题：其他进程可能因为该进程的终止一直被信号量阻塞
> 
> 这样就可能会给其他使用这个信号量的进程带来问题，因为它们可能因等待这个信号量而被阻塞着，即 **等待已经被终止的进程撤销对信号量所做的变更**
> 

为避免上述问题的发生，在通过 `semop()` 修改一个信号量值时可以使用 `SEM_UNDO` 标记。当指定这个标记时，内核会记录信号量操作的效果，然后在进程终止时撤销这个操作。不管进程是正常终止还是非正常终止，撤销操作都会发生

> [!tip] `sembuf.sem_flg = SEM_UNDO` 标记：进程终止时撤销本次信号量操作
> 
> 注意，内核记录的并不是所有的 `SEM_UNDO` 操作。而是记录一个 `SEM_UNDO` 操作的总和 `semadj`(称为，信号量调整值)
> 
> 进程终止时，需要做的就是从当前信号量值上减去 `semadj`
> 

> [!important] `semctl()` 的 `SETVAL` 或 `SETALL` 操作设置信号量值：破坏了进程 `semadj` 
> 
> 当使用 `semctl()` `SETVAL` 或 `SETALL` 操作设置一个信号量值时，所有使用这个信号量的进程中相应的 `semadj` 会被清空（即设置为 `0`）

> [!important] `fork()` 子进程不会继承父进程的 `semadj` 值
> 
> 通过 `fork()` 创建的子进程 _不会继承_ 其父进程的 `semadj` 值，因为对于子进程来讲撤销其父进程的信号量操作毫无意义
> 

> [!tip] `exec()` 操作不会破坏 `semadj` 值
> 
> `semadj` 值会在 `exec()` 中得到保留。这样就能在使用 `SEM_UNDO` 调整一个信号量值之后通过 `exec()` 执行一个不操作该信号量的程序，同时在进程终止时原子地调整该信号量
> 

### SEM_UNDO 无法执行的情形

由于 **修改一个信号量通常对应于请求或释放一些共享资源**，因此仅仅使用 `SEM_UNDO` 可能不足以允许一个多进程应用程序在一个进程异常终止时恢复。除非进程终止会原子地将共享资源的状态返回到一个一致的状态（在很多情况下是不可能的），否则撤销一个信号量操作可能不足以允许应用程序恢复

> [!example]
> 
> **当进程终止时无法对信号量进行调整**。考虑下面应用于一个初始值为 0 的信号量上的操作
> + 进程 A 将信号量值增加 $2$，并为该操作指定了 `SEM_UNDO` 标记
> + 进程 B 将信号量值减去 $1$，因此信号量的值将变成 $1$
> + 进程 A 终止
> 
> 此时就无法完全撤销进程 A 在第一步中的操作中所产生的效果，因为信号量的值太小了
>   

解决上述问题常用的方法有 $3$ 种
+ 强制阻塞进程 A 直到能够完成信号量的调整
+ 尽可能的将信号量值减小并退出，这是 Linux 常用做法
+ 直接退出，不执行任何信号量调整操作

## 信号量限制

大多数 UNIX 实现都对 System V 信号量的操作进行了各种各样的限制。下面列出了 Linux 上信号量的限制。括号中给出了当限制达到时会受影响的系统调用及其所返回的错误

|    限制    | 解释                          | 备注                                                              |
| :------: | :-------------------------- | :-------------------------------------------------------------- |
| `SEMAEM` | 在 `semadj` 总和中能够记录的最大值      | 与 `SEMVMX` 的值是一样的。(`semop()`, `ERANGE`)                         |
| `SEMMNI` | 信号量标识符的数量                   | 系统级限制。（`semget()`, `ENOSPC`）                                    |
| `SEMMSL` | 信号量集中能分配的信号量的最大数量           | （`semget(),` `EINVAL`）                                          |
| `SEMMNS` | 信号量集中的信号量数量                 | 系统级限制。系统上信号量的数量还受 `SEMMNI` 和 `SEMMSL` 的限制（`semget()`, `ENOSPC`） |
| `SEMOPM` | 每个 `semop()` 调用能够执行的操作的最大数量 | （`semop()`, `E2BIG`）                                            |
| `SEMVMX` | 信号量能取的最大值                   | （`semop()`, `ERANGE`）                                           |

一些 UNIX 实现（不包括 Linux）在 **信号量撤销操作** 方面还定义了下面的限制

|    限制    | 解释                  | 备注    |
| :------: | :------------------ | :---- |
| `SEMMNU` | 限制了信号量撤销结构的总数量      | 系统级限制 |
| `SEMUME` | 每个信号量撤销结构中撤销条目的最大数量 |       |

## 示例

### 创建和操作信号量

下面的程序演示了各种信号量系统调用的用法。这个程序可以在两种模式下运行
+ 创建一个只包含 $1$ 个信号量的信号量集，初始值由命令行参数指定
+ 修改信号量集中的信号量值。信号量集 ID 和修改的值有命令行参数指定

```c title:svsem/svsem_demo.c
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/sem.h>

#include "base.h"
#include "semun.h"
#include "current_time.h"


int main(int argc, char *argv[]) {
    if(argc < 2 || argc > 3 || strcmp(argv[1], "--help") == 0) {
        usageErr("%s init-value\nor %s semid operation\n", argv[0], argv[0]);
    }

    int semid;
    // 创建或初始化
    if(argc == 2) {
        union semun arg;

        semid = semget(IPC_PRIVATE, 1, S_IRUSR |S_IWUSR);
        if(semid == -1) {
            errExit("semget error: ");
        }
        arg.val = (int)strtol(argv[1], nullptr, 10);
        if(semctl(semid, 0, SETALL, &arg) == -1) {
            errExit("semctl error: ");
        }
        printf("Semaphore ID = %d\n", semid);
    } else {
      // 操作信号量
        struct sembuf sop;

        semid = (int)strtol(argv[1], nullptr, 10);
        sop.sem_num = 0;  // 信号量编号
        sop.sem_flg = 0;  // 操作的 flags
        sop.sem_op = (short)strtol(argv[2], nullptr, 10);

        printf("%ld: about to semop() at %s\n", (long)getpid(), current_time());
        if(semop(semid, &sop, 1) == -1) {
            errExit("semop error: ");
        }
        printf("%ld: semop() completed at %s\n", (long)getpid(), current_time());
    }
    return EXIT_SUCCESS;
}
```

下面是使用该程序的 shell 示例

```shell
➜  build ./svsem_demo 0  # 创建一个信号量集，并将信号量初始化为 0
Semaphore ID = 1
➜  build ./svsem_demo 1 -2 &  # 对信号量集ID为 1 中的信号量减少 2，由于信号量不能小于 0，所以该命令阻塞
[1] 175355
175355: about to semop() at 2024-11-08 09:45:45
➜  build ./svsem_demo 1 +3  # 对信号量集 ID 为 1 中的信号量增加 3，导致上述命令中信号量减少得以满足，从而导致上述命令继续执行
175384: about to semop() at 2024-11-08 09:46:02
175384: semop() completed at 2024-11-08 09:46:02
175355: semop() completed at 2024-11-08 09:46:02
[1]  + 175355 done       ./svsem_demo 1 -2
```

### 监控信号量集

使用了各种 `semctl()` 操作来显示标识符为命令行参数值的既有信号量集的信息。这个程序首先显示了 `semid_ds` 数据结构中的时间字段，然后显示了集合中各个信号量的当前值及其 `sempid` `semncnt` 和 `semzcnt` 值

```c title:svsem/svsem_mon.c
#include <sys/types.h>
#include <sys/sem.h>

#include "base.h"
#include "semun.h"
#include "current_time.h"

int main(int argc, char *argv[]) {
    if(argc != 2 || strcmp(argv[1], "--help") == 0) {
        usageErr("%s semid\n", argv[0]);
    }

    int semid = (int)strtol(argv[1], nullptr, 10);

    struct semid_ds ds;
    union semun arg, dummy;
    arg.buf = &ds;
    if(semctl(semid, 0, IPC_STAT, &arg) == -1) {
        errExit("semctl error: ");
    }
    printf("Semaphore changed: %s", ctime(&ds.sem_ctime));
    printf("Last semop():      %s", ctime(&ds.sem_otime));

    // 显示每个信号量的信息
    arg.array = calloc(ds.sem_nsems, sizeof(arg.array[0]));
    if(arg.array == nullptr) {
        errExit("calloc error: ");
    }
    if(semctl(semid, 0, GETALL, &arg) == -1) {
        errExit("semctl-GETALL error: ");
    }
    printf("Sem # Value SEMPID SEMNCNT SEMZCNT\n");
    for(int i = 0; i < ds.sem_nsems; i++) {
        printf("%3d %5d %5d %5d %5d\n", i, arg.array[i],
            semctl(semid, i, GETPID, &dummy),
            semctl(semid, i, GETNCNT, &dummy),
            semctl(semid, i, GETZCNT, &dummy));
    }
    return 0;
}
```
