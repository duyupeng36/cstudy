# 程序执行

在 [[Linux 系统编程：进程创建]] [[Linux 系统编程：进程终止]] 和 [[Linux 系统编程：监控子进程]] 中探讨了如何创建进程、终止进程和回收进程。下面我们将学习 `exec()` 函数族，该函数可以使调用进程 **用全新的程序替换当前运行的程序**

## 执行新程序

系统调用 `execve()` 可以将新程序加载到某一进程的内存空间。在这一操作过程中，将丢 **弃旧有程序**，而 **进程的栈、数据以及堆段会被新程序的相应部件所替换**。

在执行了各种 C 语言函数库的运行时启动代码以及程序的初始化代码后，新程序会从 `main()` 函数处开始执行。

> [!tip] 
> 
> 由 `fork()` 生成的子进程对 `execve()` 的调用最为频繁，不以 `fork()` 调用为先导而单独调用 execve()的做法在应用中实属罕见
> 

基于系统调用 `execve()`，还提供了一系列冠以 `exec` 来命名的上层库函数，虽然接口方式各异，但功能相同。

> [!tip] 
> 
> 调用这些函数加载一个新程序的过程称作 `exec` 操作
> 

现在，我们首先看 `execvec()` 系统调用

```c
#include <unistd.h>

int execvec(const char *pathname, char * const argv[], char *const envp[]);
/* 成功调用则不返回；失败返回 -1 */
```

> [!tip] 参数 `pathname`：准备载入当前进程空间的 **新程序的路径名**
> 
> + 可以是绝对路径，也可以相对当前工作目录的相对路径
> 

> [!tip] 参数 `argv`：字符指针常量数组，指定传递给新程序的 **命令行参数**
> 
> 该数组对应于 C 语言 `main()` 函数的第 $2$ 个参数，并且格式也是相同的
> 
> 是由字符串指针所组成的列表，以 `NULL` 结束。`argv[0]` 是程序名，通常是 `pathname` 中的 `basename` 部分（路径名的最后一部分）
> 

> [!tip] 参数 `envp`：字符指针常量数组，指定新程序的环境变量
> 
> 对应于新程序的 `environ` 数组(环境变量)，也是由字符串指针组成的列表，以 `NULL` 结束，所指向的字符串格式为 `name=value`
> 

> [!tip] 返回值
> 
> `execvec()` 通常是不返回。如果返回一定是 $-1$，代表出现错误。可能出现的错误如下表
> 

| 错误码       | 描述                                    |
| :-------- | :------------------------------------ |
| `EACCES`  | `pathname` 不是常规文件，未对该文件赋予执行权限，有目录不可搜索 |
| `ENOENT`  | `pathname` 所指代的文件并不存在                 |
| `ENOEXEC` | `pathname` 具有可执行权限，但是系统却无法识别其文件格式     |
| `ETXTBSY` | 存在进程以写入方式打开 `pathname` 所指代的文件         |
| `E2BIG`   | 参数列表和环境列表所需空间总和超出了允许的最大值              |

下面的例程展示 `execve()` 的用法。该程序首先为新程序创建参数列表和环境列表，接着调用 `execve()` 来执行由命令行参数(`argv[1]`)所指定的程序路径名

```c title:procexec/t_execve.c
#include <unistd.h>

#include "base.h"

int main(int argc, char *argv[]) {
    if(argc != 2 || strcasecmp(argv[1], "--help") == 0) {
        usageErr("%s <executable-file>\n", argv[0]);
    }

    char *argVec[10];
    char *envVec[] = {"GREET=salut", "BYE=adieu", nullptr};

    argVec[0] = strrchr(argv[1], '/');  // strrchr(s, c) 字符中定位字符：返回一个指针，指向字符串 s 中最后一个出现的字符 c
    if(argv[0] != nullptr) {
        argVec[0]++;
    } else {
        argVec[0] = argv[1];
    }
    argVec[1] = "hello world";
    argVec[2] = "good bye";
    argVec[3] = nullptr;

    execve(argv[1], argVec, envVec);
    errExit("execve");
}
```

为了执行 `t_execve.c` 编译出了的程序，我们设计了如下输出环境变量和参数列表的程序

```c title:procexec/envargs.c

#include "base.h"

// 显示参数列表和环境列表
int main(int argc, char *argv[], char *env[]) {
    for(int i = 0; i < argc; i++) {
        printf("argv[%d] = %s\n", i, argv[i]);
    }

    for(char **p = env; *p != nullptr; p++) {
        printf("environ: %s\n", *p);
    }
    return 0;
}
```

如下 shell 会话演示了对上述两个程序的使用（指定执行程序时使用的是相对路径名）

```shell
➜  build ./t_execve ./envargs
argv[0] = envargs
argv[1] = hello world
argv[2] = good bye
environ: GREET=salut
environ: BYE=adieu
```

## exec() 库函数

下面介绍构建在 `execve()` 系统调用上的库函数。这是一个函数族，它们执行相同的功能，只是在为新程序指定程序名、参数列表以及环境变量的方式上有所不同

```c
#include <unistd.h>

int execle(const char *pathname, const char *arg,...
		   /*, (char *)NULL, char *const envp[]*/);

int execlp(const char *filename, const char *arg, ...
		   /*, (char *)NULL */);

int execvp(const char *fliename, char * const argv[]);

int execv(const char *pathname, char * const argv[]);

int execl(const char *pathname, char * const arg, ... 
		  /*, (char *)NULL*/);

/* 成功不返回，失败返回 -1*/
```

上述 `exec()` 函数族的名字的最后一个字母区分了这些函数。下表总结了这些函数的差异

![Pasted image 20241103233055|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755707580927-a56dc11c7b0e4ecf934739f08e7b872c.png)

> [!summary] 
> 
> + `p`：表示程序文件通过 `PATH` 环境变量寻找
> + `v`：命令行参数以向量(vector)方式提供
> + `l`：命令行参数以列表(list) 方式提供
> + `e`：表示新程序的环境变量通过 **显示指定**。否则使用调用者的环境变量
> 

### 环境变量 PATH

函数 `execvp()` 和 `execlp()` 允许调用者只提供欲执行程序的文件名。二者均使用环境变量 `PATH` 来搜索文件。`PATH` 的值是一个以 **冒号** 分隔，由多个目录名，也将其称为 **路径前缀** 组成的字符串。下例中的 `PATH` 包含 $9$ 个目录

```shell
➜  build echo $PATH
/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:/snap/bin
```

对于一个登录 shell 而言，其 `PATH` 值将由系统级和特定用户的 shell 启动脚本来设置。由于子进程继承其父进程的环境变量，shell 执行每个命令时所创建的进程也就继承了 shell 的 `PATH`

> [!tip] 
> 
> 在 `PATH` 中，包含一个长度为 $0$ 的前缀，也可以用来指定当前工作目录。表示方式有：连续的冒号，起始冒号或尾部冒号；例如， `/usr/bin:/bin: `
> 
> SUSv3 废止了上述技术，**当前工作目录只能使用 `.`(点) 显示指定**
>

如果没有定义变量 `PATH`，那么 `execvp()` 和 `execlp()` 会采用默认的路径列表：`.:/usr/bin:/bin`

> [!important] 
> 
> 出于安全考虑，当前工作目录排除在 `root` 用户的 `PATH` 之外。为了防止
> + 执行当前工作目录下与标准命令同名的程序，该程序可能由恶意用户提前放置
> + 将常用命令拼写错误而执行了当前工作目录下的其他程序
> 

函数 `execvp()` 和 `execlp()` 会在 `PATH` 包含的每个目录中搜索文件，从列表开头的目录开始，直至成功执行了既定文件

> [!tip] 
> 
> 通常，`execvp()` 和 `execlp()` 用在 **不清楚可执行程序的具体位置**，或者 **不想硬编码程序名** 的情形下
> 


避免在设置了 `set-user-ID` 或 `set-group-ID` 的程序中调用 `execvp()` 和 `execlp()`，至少应当慎用。需要特别谨慎地控制 `PATH` 环境变量，以防运行恶意程序。

下面的例程展示了 `execlp()` 的使用示例

```c title:procexec/t_execlp.c

#include <unistd.h>

#include "base.h"

int main(int argc, char *argv[]) {
    if(argc != 2 || strcmp(argv[1], "--help") == 0 ) {
        usageErr("%s [filename]\n", argv[0]);
    }
    execlp(argv[1], argv[1], "hello, world", nullptr);
    errExit("execlp ");
    return 0;
}
```

下面的 shell 会话演示该程序的执行结果

```shell
➜  build echo $PATH
/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:/snap/bin
➜  build ./t_execlp envargs  # envargs 不在 PATH 环境变量指定的所有目录中
error: execlp No such file or directory
➜  build ./t_execlp echo
hello, world
```

### 程序参数指定为列表

如果在编程时 **已知执行某个程序的参数个数**，调用 `execle()` `execlp()` 或者 `execl()` 时就可以 **将参数作为列表传入**。较之于将参数装配于一个 `argv` 向量中，代码要少一些，便于使用。

```c title:procexec/t_execle.c

#include <unistd.h>

#include "base.h"

int main(int argc, char *argv[]) {
    if(argc != 2 || strcasecmp(argv[1], "--help") == 0) {
        usageErr("%s <executable-file>\n", argv[0]);
    }

    char *envVec[] = {"GREET=salut", "BYE=adieu", nullptr};
    char *filename = strrchr(argv[1], '/');

    if(filename != nullptr) {
        filename++;
    } else {
        filename = argv[1];
    }
    execle(argv[1], filename, "hello, world", NULL, envVec);  // 以 null 分隔 argv 和 env
    errExit("execve");
    return 0;
}
```

下面的 shell 会话展示了上述例程的运行结果

```shell
➜  build ./t_execle ./envargs
argv[0] = envargs
argv[1] = hello, world
environ: GREET=salut
environ: BYE=adieu
```

### 将调用者的环境遍历传递给新程序

函数 `execlp()` `execvp()` `execl()` 和 `execv()` 不允许开发者显式指定环境列表，新程序的环境继承自调用进程

> [!tip] 
> 
> 于安全方面的考虑，有时希望确保程序在一个已知（安全）的环境列表下运行
> 

```c title:proexec/t_execl.c
#include <unistd.h>

#include "base.h"

int main() {
    printf("Initial value of USER: %s\n", getenv("USER"));
    if(putenv("USER=britta") != 0) {
        errExit("putenv ");
    }

    execl("/usr/bin/printenv", "printevn", "USER", "SHELL", nullptr);
    errExit("execl ");
    return 0;
}
```

下面的 shell 会话展示了上述例程的执行结果

```shell
➜  build ./t_execl
Initial value of USER: ecs-user
britta
/usr/bin/zsh
```

### 执行打开文件

glibc >= 2.3.2 开始提供了 `fexecve()`，其行为与 `execve()` 类似，只是指定将要执行的程序是通过打开文件描述符 `fd` 指定的。

> [!example] 
> 
> 有些应用程序需要打开某个程序文件，通过执行 **校验和** 来验证文件内容，然后再运行该程序
> 

对于需要 **校验后才执行的程序**，`fexecve()` 函数就有了用武之地

```c
#include <unistd.h>

int fexecve(int fd, char *const argv[], char *const envp[]);
```

当然，即便没有 `fexecve()` 函数，也可以调用 `open()` 来打开文件，读取并验证其内容，并最终运行。然而，在打开与执行文件之间，存在将该文件替换的可能性（持有打开文件描述符并不能阻止创建同名新文件），最终造成验证者并非执行者的情况

## 解释器脚本

所谓 **解释器**，就是能够 **读取并执行文本格式命令的程序**

UNIX 内核运行解释器脚本的方式与二进制程序无异，前提是脚本必须满足下面两点要求：首先，**必须赋予脚本文件可执行权限**；其次，文件的 **起始行必须指定运行脚本解释器的路径名**。格式如下

```
#! 解释器路径 [参数]
```

> [!important] 
> 
> **字符 `#!` 必须置于该行起始处**，这两个字符串与解释器路径名之间可以以空格分隔
> 
> **解释器路径** 不会使用环境变量 `path`，通常 **使用绝对路径**
> 
> 解释器脚本文件首行中的 **可选参数不应包含空格**，因为空格此处所起的作用完全取决于实现
> 

因为脚本并不包含二进制机器码，所以当调用 `execve()` 来运行脚本时，显然发生了一些不同寻常的事件。`execve()` 如果检测到传入的文件以两字节序列 `"#!"` 开始，就会析取该行的剩余部分（路径名以及参数），然后按如下图的方式来执行解释器程序

![Pasted image 20241105150947|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755707580928-80e4efdbd6144e24bf47c994a181afb3.png)

## 文件描述符与 exec()

> [!important] 
> 
> 默认情况下，由 `exec()` 的调用程序所打开的所有文件描述符在 `exec()` 的执行过程中会保持打开状态，且在新程序中依然有效
> 

保存打开文件描述符往往是很实用的，因为 **调用程序可能会以特定的描述符来打开文件**，而在 **新程序中这些文件将自动有效**，无需再去了解文件名或是把它们重新打开

shell 利用这一特性为其所执行的程序处理 IO 重定向。例如，假设键入如下的 shell 命令

```shell
$ ls /tmp > dir.txt
```

shell 运行该命令时，执行了以下步骤

+ 调用 `fork()` 创建子进程，子进程会也运行 shell 的一份拷贝（因此命令行也有一份拷贝）
+ 子 shell 以 $\text{fd 1}$ 打开文件 `dir.txt` 用于输出
	+ shell 打开文件 `dir.txt`，然后使用 `dup2()` 强制标准输出复制为新描述符服务
+ 子 shell 执行程序 `ls`。`ls` 将其结果输出到标准输出，亦即文件 `dir.txt` 中

### 执行时关闭标志 FD_CLOEXEC

在执行 `exec()` 之前，程序有时需要确保关闭某些特定的文件描述符

> [!warning] 执行 `exec()` 前关闭特定文件描述符的原因
> 
> 在特权进程中调用 `exec()` 来 **启动一个未知程序** 时（并非自己编写）
> 
> **启动程序并不需要使用这些已打开的文件描述符** 时
> 

从安全编程的角度出发，应当在加载新程序之前确保关闭那些不必要的文件描述符。对所有此类描述符施以 `close()` 调用就可达到这一目的

> [!attention] 调用 `close()` 关闭 `exec()` 不需要的文件描述符存在的缺陷
> + 某些描述符可能是由库函数打开的。但库函数无法使主程序在执行 `exec()` 之前关闭相应的文件描述符。
> + 如果 `exec()` 因某种原因而调用失败，可能还需要使描述符保持打开状态。

为此，内核为每个文件描述符提供了 **执行时关闭标志**。如果设置了这一标志，那么在成功执行 `exec()` 时，会自动关闭该文件描述符，如果调用 `exec()`失败，文件描述符则会保持打开状态

> [!tip] 设置了执行时关闭标志的文件描述
> + 当 `exec()` 执行成功，自动关闭该文件描述符
> + 当 `exec()` 执行失败，文件描述符保存打开
> 

可以通过系统调用 `fcntl()` 来访问执行时关闭标志。`fcntl()` 的 `F_GETFD` 操作可以获取文件描述符标志的一份拷贝

```c
int flags;

flags = fcntl(fd, F_GETFD)
```

获取这些标志后，可以对 `FD_CLOEXEC` 位进行修改，再调用 `fcntl()` 的 `F_SETFD` 操作令其生效

```c
flags |= FD_CLOEXEC

fcntl(fd, F_SETFD, flags);
```

> [!tip] 
> 
> 实际上 `FD_CLOEXEC` 是文件描述符标志中 **唯一可以操作的一位**。存储在进程级打开文件表的表项中。
> 

当使用 `dup()` `dup2()` 或 `fcntl()` 为一文件描述符创建副本时，总是会清除副本描述符的执行时关闭标志

## 执行 shell 命令：system()

程序可通过调用 `system() `函数来执行任意的 `shell` 命令

```c
#include <stdlib.h>

int system(const char *command);
```

函数 `system()` 创建一个子进程来运行 `shell`，并以之执行命令 `command`。`system()` 的主要优点在于简便

> [!tip] 使用 `system()` 的好处
> 
> + 无需处理对 `fork()`、`exec()`、`wait()` 和 `exit()` 的调用细节
> + `system()` 会代为处理错误和信号
> + 因为 `system()` 使用 `shell` 来执行命令，所以会在执行 `command` 之前对其进行所有的常规 `shell` 处理、替换以及重定向操作
> 

**但这些优点是以低效率为代价的**。使用 `system()` 运行命令 **需要创建至少两个进程**。一个用于运行 `shell`，另外一个或多个则用于 `shell` 所执行的命令，执行每个命令都会调用一次 `exec()`。如果对效率或者速度有所要求，最好还是直接调用 `fork()` 和 `exec()` 来执行既定程序

> [!tip] 返回值
> `system()` 的返回值如下
> 
> 当 `command` 为 `NULL` 指针时，如果 `shell` 可用则 `system()` 返回非 `0 `值，若不可用则返回 `0`
> 
> 若 `command` 不为 `NULL`，则 `system()` 的返回值由本列表中的余下规则决定
> +  如果 **无法创建子进程** 或是 **无法获取其终止状态**，那么 `system()` **返回 $-1$**
> + 若 **子进程不能执行 `shell`**，则 `system()` 的返回值会与子 `shell` 调用 `_exit(127)` 终止时一样
> + 如果所有的系统调用都成功，`system()` 会返回执行 `command` 的子 `shell` 的终止状态
> 	+ `shell` 的终止状态是其执行最后一条命令时的退出状态；如果命令为信号所杀，大多数 `shell` 会以值 `128+n` 退出，其中 `n` 为信号编号

如下例程是使用 `system()` 执行 shell 命令的示例

```c title:procexec/t_system.c
#include <sys/wait.h>

#include "base.h"

#define MAX_CMD_LEN 256

int main() {
    char command[MAX_CMD_LEN];
    int status;

    for(;;) {
        printf("Command:");
        fflush(stdout);
        if(fgets(command, MAX_CMD_LEN, stdin) == NULL) {
            break;  // 读取到 eof
        }
        status = system(command);

        printf("system() returned: status=0x%04x (%d, %d)\n", status, status>>8, status & 0xff);
        if(status == -1) {
            errExit("system");
        }
        if(WIFEXITED(status) && WEXITSTATUS(status) == 127) {
            printf("(Probably) could not invoke shell\n");
        } else {
            print_wait_status(nullptr, status);
        }
    }
    return 0;
}
```

> [!seealso] 
> 
> 函数 `print_wait_status()` 函数参考 [[Linux 系统编程：监控子进程#等待状态]]
> 
