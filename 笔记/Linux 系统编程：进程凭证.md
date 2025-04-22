# 进程凭证

每个进程都有一套用数字表示的 **用户 ID(UID)** 和 **组 ID(GID)**。有时，也将这些 ID 称之为 **进程凭证**


## 实际用户ID 和 实际组ID

> [!tip] 
> 
> **实际用户 ID** 和 **实际组 ID** 确定了 **进程所属的用户和组**
> 

作为登录过程的步骤之一，登录 shell 从 `/etc/passwd` 文件中读取相应用户密码记录的第三字段和第四字段，置为其 **实际用户 ID** 和 **实际组 ID** 

当创建新进程（比如，shell 执行一程序）时，将从其父进程中继承这些 ID

> [!tip] 实际用户：登录系统的用户
> 

## 有效用户 ID 和 有效组 ID

在大多数 UNIX 实现中当进程尝试执行各种操作（即系统调用）时，将结合 **有效用户 ID**、**有效组 ID**，连同 **辅助组 ID** 一起来 **确定授予进程的权限**

> [!tip] 有效用户 ID 和 有效组 ID：确定进程权限

当进程访问诸如文件、System V 进程间通信对象之类的系统资源时，此类 ID 会决定系统授予进程的权限，而这些 **资源的属主则另由与之相关的 用户 ID 和 组 ID 来决定**

内核还会使用有效用户 ID 来决定一个进程是否能向另一个进程发送信号

> [!NOTE] 特权进程
> **有效用户 ID 为 0**（root 的用户 ID）的进程拥有超级用户的所有权限。这样的进程又称为 **特权级进程**。而某些系统调用只能由特权级进程执行

> [!tip] 
>  
>  一般而言，有效用户 ID 与 实际用户 ID 相同；有效组 ID 与 实际组 ID 相等
>  

## Set-User-ID 和 Set-Group-ID 程序

**`set-user-ID` 程序** 会将进程的有效用户 ID 置为可执行文件的用户 ID（属主），从而获得常规情况下并不具有的权限。**`set-group-ID` 程序** 对进程有效组 ID 实现类似任务

与其他文件一样，**可执行文件的 用户 ID 和 组 ID** 决定了该文件的所有权。可执行文件还拥有两个 **特别的权限位** **`set-user-ID` 位和 `set-group-ID` 位** ^[[[文件属性#文件权限]]]

> [!tip] 
> 
> **可使用 `chmod` 命令来设置这些权限位**。非特权用户能够对其拥有的文件进行设置，而特权级用户（`CAP_FOWNER`）能够对任何文件进行设置
> 

如果为程序设置了 `set-user-ID` 权限位和 `set-group-ID` 权限位，那么通常用来表示文件可执行权限的 `x` 标识会被 `s` 标识所替换

```shell
$ ls -al check_password
-rwxrwxr-x 1 ecs-user ecs-user 16712 Oct 25 16:40 check_password

$ chmod u+s check_password  # 设置 set-user-ID 权限位

$ ls -al check_password
-rwsrwxr-x 1 ecs-user ecs-user 16712 Oct 25 16:40 check_password
```

当运行 `set-user-ID` 程序，即通过调用 `exec()` 将 `set-user-ID` 程序载入进程的内存中时，**内核会将进程的有效用户 ID 设置为可执行文件的用户 ID**

> [!tip] 
> 
> **运行带有 `set-user-ID` 权限位的可执行文件时**，**内核将进程的有效 ID 设置为 可执行文件的用户 ID**
> 

通过这种方法修改进程的有效用户 ID 或者组 ID，能够使进程获得常规情况下所不具有的权限

> [!important]  重要
> - **set-user-ID 程序**：带有 set-user-ID 权限位的程序
> - **set-group-ID 程序**：带有 set-group-ID 权限位的程序
> 

利用程序的 `set-user-ID` 和 `set-group-ID` 机制，将进程的有效 ID 修改为 root 之外的其他用户。例如，为提供对一个受保护文件（或其他系统资源）的访问， 采用如下方案就绰绰有余
- 创建一个具有对该文件访问权限的专用用户（组）ID
- 然后再创建一个 `set-user-ID`（`set-group-ID`）程序，将进程有效用户（组）ID 变更为这个专用 ID
- 这样，无需拥有超级用户的所有权限，程序就能访问该文件

**术语 `set-user-ID-root` 来表示 root 用户所拥有的 `set-user-ID` 程序**，以示与由其他用户所拥有的 set-user-ID 程序有所区别，后者仅为进程提供其属主所具有的权限

我们的 `check_password` 程序 ^[[[作业23：用户和组]]] 需要以 root 用户身份运行，以便于获取 `/etc/shadow` 文件的访问权限。想要使该程序可以为任意用户执行，必须设置为 `set-user-ID-root` 程序

```shell
# 修改属主 和 属组
$ sudo chown root check_password && sudo chgrp root check_password

$ ls -al check_password
-rwxrwxr-x 1 root root 16712 Oct 25 16:40 check_password

# 设置 set-user-ID 和 set-group-ID 权限位
$ sudo chmod u+s check_password && sudo chmod g+s check_password

$ ls -al check_password
-rwsrwsr-x 1 root root 16712 Oct 25 16:40 check_password
```

以 dyp 身份运行它：

```shell
# 不需要添加 sudo 也可以访问 /etc/shadow 文件
$ ./check_password
username: ecs-user
Password:
successfully authenticated: UID=1000
```

现在，不是 root 身份也能访问 `/etc/shadow` 文件了

## 保存 set-user-ID 和保存 set-group-ID

设计 **保存 set-user-ID（saved set-user-ID）** 和 **保存 set-group-ID (saved set-group-ID)**，意在与 `set-user-ID` 和 `set-group-ID` 程序结合使用。当执行程序时，将会（依次）发生如下事件（在诸多事件之中）
- **若可执行文件的 `set-user-ID` (`set-group-ID`)权限位已开启，则将进程的有效用户（组）ID置为可执行文件的属主**。若未设置 set-user-ID (set-group-ID)权限位，则进程的有效用户（组）ID 将保持不变
-  **保存 set-user-ID 和保存 set-group-ID 的值由对应的有效 ID 复制而来**。无论正在执行的文件是否设置了 `set-user-ID` 或 `set-group-ID` 权限位，这一复制都将进行

假设某进程的 **实际用户 ID**、**有效用户 ID** 和 **保存 set-user-ID** 均为 `1000`，当其执行了 `root` 用户（用户 ID 为 0）拥有的 `set-user-ID` 程序后，进程的用户 ID 将发生如下变化: `real=100 effective=0 saved=0`

## 文件系统用户 ID 和组 ID

在 Linux 系统中，要进行诸如打开文件、改变文件属主、修改文件权限之类的 **文件系统操作**时，决定其操作 **权限的是文件系统用户 ID 和组 ID（结合辅助组 ID）**，而非有效用户 ID 和组 ID

> [!WARNING] 注意
> 有效用户 ID 和组 ID 仍在使用

通常，**文件系统用户 ID 和组 ID 的值 等同于 相应的有效用户 ID 和组 ID**。此外，**只要有效用户或组 ID 发生了变化**，无论是通过系统调用，还是通过执行 set-user-ID 或者 set-group-ID 程序，则相应的 **文件系统 ID 也将随之改变为同一值**
- 由于 **文件系统 ID 对有效 ID 如此的“亦步亦趋”**，这意味着在特权和权限检查方面，Linux 实际上跟其他 UNIX 实现非常类似
- 只有当使用 Linux 特有的两个系统调用（`setfsuid()` 和 `setfsgid()`）时，才可以刻意制造出文件系统 ID 与相应有效 ID 的不同，因而 Linux 也不同于其他的 UNIX 实现

> [!NOTE] 提供文件系统用户ID 和 组ID 的原因
> 文件系统 ID 始见于 Linux 1.2 版本。在该版本的内核中，如果 **进程甲** 的 *有效用户 ID* 等同于 **进程乙** 的 *实际用户 ID 或者有效用户 ID*，那么**甲进程 就可以向 乙进程 发送信号**
> 
> 这在当时影响到了不少程序，比如 `Linux NFS`（网络文件系统）服务器程序，在访问文件时就 **好像拥有着相应 客户进程的有效 ID**
> 
> 然而，如果 `NFS` 服务器真地修改了自身的有效用户 ID，面对非特权用户进程的信号攻击，又将不堪一击
> 
> 为了防范这一风险，文件系统用户 ID 和组 ID 应运而生。**`NFS` 服务器将有效 ID 保持不变**，而是通过 **修改文件系统 ID 伪装成另一用户，这样既达到了访问文件的目的，又避免了遭受信号攻击**

**自内核 2.0 起，Linux 开始在信号发送权限方面遵循 SUSv3 所强制规定的规则**，且这些规则 **不再涉及目标进程的有效用户 ID** 。因此，从严格意义上来讲，保留文件系统 ID 特性已无必要（如今，进程可以根据需要，审慎而明智地利用稍后介绍的系统调用，使以非特权值对有效用户 ID 的赋值来去自由，以实现预期结果），但为了与现有软件保持兼容，这一功能得以保留了下来

## 辅助组 ID

**辅助组 ID 用于标识进程所属的若干附加的组**。**新进程从其父进程处继承这些 ID**，登录
`shell` 从系统组文件中获取其辅助的组 ID。将这些 ID 与有效 ID 以及文件系统 ID 相结合，就能决定对文件、System V IPC 对象和其他系统资源的访问权限

## 获取和修改进程凭证


### 获取实际和有效 ID

系统调用 `getuid() `和 `getgid()` 分别返回调用进程的实际用户 ID 和组 ID

系统调用 `geteuid()` 和 `getegid()` 则对进程的有效 ID 实现类似功能。对 **这些系统函数的调用总会成功**

```c
#include <unistd.h>

uid_t getuid(void);  // 获取实际用户 ID (RUID)
uid_t geteuid(void); // 获取有效用户 ID (EUID)

gid_t getgid(void);  // 获取实际组 ID (RGID)
gid_t getegid(void); // 获取有效组 ID (EGID)
```

### 修改有效 ID

`setuid()` 系统调用以给定的 `uid` 参数值来 **修改调用进程的有效用户 ID**，也可能修改实际用户 ID 和保存 `set-user-ID`。系统调用 `setgid()` 则对相应组 I 实现了类似功能

```c
#include <unistd.h>

int setuid(uid_t uid);
int setgid(git_t git);
/* 成功返回 0；错误返回 -1 */
```

进程使用 `setuid()` 和 `setgid()` 系统调用能对其凭证做哪些修改呢？其规则 **取决于进程是否拥有特权（即有效用户 ID 为 0）**。适用于 `setuid()` 系统调用的规则如下
- **当非特权进程调用 `setuid()` 时，仅能修改进程的有效用户 ID**
	- 仅能将有效用户 ID修改成相应的实际用户 ID 或保存 set-user-ID
	- **对于非特权用户而言，仅当执行 `set-user-ID` 程序时，setuid()系统调用才起作用**，因为在执行普通程序时，进程的实际用户 ID、有效用户 ID 和保存 set-user-ID 三者之值均相等
- **当特权进程以一个非 0 参数调用 `setuid()` 时，其实际用户 ID、有效用户 ID 和保存 set-user-ID均被置为 uid 参数所指定的值**
	- **该操作是单向的**，一旦特权进程以此方式修改了其 ID，那么所有特权都将丢失，且之后也不能再使用 `setuid()` 调用将有效用户 ID 重置为 0

使用 `setgid()` 系统调用修改组 ID 的规则与之相类似，仅需要把 `setuid() `替换为 `setgid()`，把用户替换为组。然而，对于 **对组 ID 的修改不会引起进程特权的丢失**（拥有特权与否由有效用户 ID 决定），特权级程序可以使用 `setgid()` 对组 ID进行任意修改

**对 set-user-ID-root 程序（即其有效用户 ID 的当前值为 0）而言**，以 **不可逆方式放弃进程所有特权** 的首选方法是使用下面的系统调用（以实际用户 ID 值来设置有效用户 ID 和保存set-user-ID）
```c
if(setuid(getuid()) == -1)
{
	// 错误处理
}
```

**set-user-ID 程序** 的属主如果不是 root 用户，可使用 `setuid()` 将 **有效用户 ID** 在 **实际用户 ID** 和 **保存 `set-user-ID`** 之间来回切换。然而，使用 `seteuid()` 来达成这个目的则更为可取，因为无论 `set-user-ID` 程序是否属于 `root` 用户，`seteuid()` 都能够实现同样的功能

进程能够使用 `seteuid()` 来修改其有效用户 ID（改为参数 `euid` 所指定的值），还能使用`setegid()来` 修改其有效组 ID（改为参数 `egid` 所指定的值）

```c
#include <unistd.h>

int seteuid(uid_t euid);
int setegid(git_t egid);
```

进程使用 `seteuid()` 和 `setegid()` 来修改其有效 ID 时，会遵循以下规则
- **非特权级进程仅能将其有效 ID 修改为相应的实际 ID 或者保存设置 ID**
	- 对于非特权进程，`seteuid()` 和 `setegid()` 分别等效于 `setuid()` 和 `setgid()`
- **特权级进程能够将其有效 ID 修改为任意值**。若特权进程使用 `seteuid()` 将其有效用户 ID 修改为非 0 值，那么此进程将不再具有特权（但可以根据上述 来恢复特权）

对于需要 **对特权“收放自如”** 的 `set-user-ID` 和 `set-group-ID` 程序，更推荐使用` seteuid()`，示例如下

```c
euid = geteuid();  // 保存有效用户ID

if(seteuid(getuid()) == -1)  // 丢失特权
{
	// 错误处理
}
// ....
if(seteuid(euid) == -1)  // 恢复特权
{
	// 错误处理
}
```
### 修改实际 ID 和有效 ID

`setreuid()` 系统调用允许调用进程独立 **修改其 实际 和 有效用户 ID**。`setregid()`系统调用对实际和有效组 ID 实现了类似功能

```c
#include <unistd.h>

int setreuid(uid_t ruid, uid_t euid);

int setregid(gid_t rgid, gid_t egid);
```

### 获取实际、有效和保存设置 ID

在大多数 UNIX 实现中，**进程不能直接获取（或修改）其保存 `set-user-ID` 和保存 `set-group-ID` 的值**。然而，Linux 提供了两个（非标准的）系统调用来实现此项功能：`getresuid()`  和 `getresgid()`。

```c
#define _GNU_SOURCE
#include <unistd.h>

int getresuid(uid_t *ruid, uid_t *euid, uid_t *suid);
int getresgid(gid_t *rgid, gid_t *egid, gid_t *sgid);
```

### 修改实际、有效和保存设置 ID

`setresuid()` 系统调用允许调用进程独立修改其 3 个用户 ID 的值。每个用户 ID 的新值由系
统调用的 3 个参数给定。`setresgid()`系统调用对相应的组 ID 实现了类似功能

```c
#define _GNU_SOURCE
#include <unistd.h>

int setresuid(uid_t ruid, uid_t euid, uid_t suid);
int setresgid(gid_t rgid, gid_t egid, gid_t sgid);
```
### 获取和修改文件系统 ID

**所有修改进程有效用户 ID 或组 ID 的系统调用总是会修改相应的文件系统 ID**。要想独立于有效 ID 而修改文件系统 ID，必须使用 Linux 特有的系统调用：`setfsuid()` 和 `setfsgid()` 

```c
#include <sys/fsuid.h>

int setfsuid(uid_t fsuid);

int setfsgid(gid_t fsgid);
```

### 获取和修改辅助组 ID

`getgroups()` 系统调用会将当前进程所属组的集合返回至由参数 `grouplist` 指向的数组中

```c
#include <unistd.h>

int getgroups(int  gidsetsize, git_t grouplist[]);
```
**调用程序必须负责为 `grouplist` 数组分配存储空间，并在 `gidsetsize` 参数中指定其长度**

**特权级进程**能够使用 `setgroups()`和 `initgroups()`来修改其辅助组 ID 集合
```c
#define _BSD_SOURCE
#include <grp.h>

int setgroups(size_t gidsetsize, const gid_t *grouplist);
int initgroups(const char *user, gid_t group);
```
- `setgroups()`系统调用用 `grouplist` 数组所指定的集合来替换调用进程的辅助组 ID。参数 `gidsetsize` 指定了置于参数 `grouplist` 数组中的组 ID 数量
- `initgroups()`函数将扫描 `/etc/groups `文件，为 user 创建属组列表，以此来初始化调用进程的辅助组 ID

## 修改进程凭证的系统调用总结

下表对修改进程凭证的各种系统调用及库函数的效果进行了总结

![[Pasted image 20241025170634.png]]


下图为表中信息的概括图示

![[Pasted image 20241025170608.png]]

## 示例：显示进程凭证

```c
#define _GNU_SOURCE  // #define __USE_XOPEN  // 开启 uid_t 和 gid_t 的定义

#include <unistd.h>
#include <sys/fsuid.h>
#include <limits.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pwd.h>
#include <grp.h>
#include <shadow.h>

#define SG_SIZE (NGROUPS_MAX + 1)


/**
 * @uid: 用户id
 * @return: 用户名
*/
char *uid2username(uid_t uid)
{
    struct passwd *pwd;
    errno = 0;
    pwd = getpwuid(uid);
    // 错误检查
    if(pwd == NULL)
    {
        // 没有找到用户
        if(errno == 0)
        {
            fprintf(stderr, "uid2username: getpwuid: no such user with uid %d\n", uid);
            return NULL;
        }
        else  // 出错
        {
            fprintf(stderr, "uid2username: getpwuid failed: %s\n", strerror(errno));
            return NULL;
        }
    }
    return pwd->pw_name;  // 返回用户名
}

/**
 * @gid: 组id
 * @return: 组名
*/
char *gid2groupname(gid_t gid)
{
    struct group *grp;
    errno = 0;
    grp = getgrgid(gid);
    // 错误检查
    if(grp == NULL)
    {
        // 没有找到组
        if(errno == 0)
        {
            fprintf(stderr, "gid2groupname: getgrgid: no such group with gid %d\n", gid);
            return NULL;
        }
        else  // 出错
        {
            fprintf(stderr, "gid2groupname: getgrgid failed: %s\n", strerror(errno));
            return NULL;
        }
    }
    return grp->gr_name;  // 返回组名
}   


int main(int argc, char *argv[])
{
    uid_t ruid, euid, suid, fsuid;
    gid_t rgid, egid, sgid, fsgid;
    gid_t suppGroups[SG_SIZE];
    int numGroups, j;
    char *p;
    if(getresuid(&ruid, &euid, &suid) == -1)
    {
        perror("getresuid");
        return 1;
    }
    if(getresgid(&rgid, &egid, &sgid) == -1)
    {
        perror("getresgid");
        return 1;
    }
    fsuid = setfsuid(0);
    fsgid = setfsgid(0);
    printf("UID: ");
    p = uid2username(ruid);  // 真实用户 ID
    printf("real=%s(%ld); ", p == NULL ? "???" : p, (long)ruid);
    printf("\n");
    p = uid2username(euid);  // 有效用户 ID
    printf("eff=%s(%ld); ", p == NULL ? "???" : p, (long)euid);
    printf("\n");
    p = uid2username(suid);  // save-set-user-id ID
    printf("saved=%s(%ld); ", p == NULL ? "???" : p, (long)suid);
    printf("\n");
    p = uid2username(fsuid);  // 文件系统用户 ID
    printf("fs=%s(%ld); ", p == NULL ? "???" : p, (long)fsuid);

    printf("\nGID: ");
    p = gid2groupname(rgid);  // 真实组 ID
    printf("real=%s(%ld); ", p == NULL ? "???" : p, (long)rgid);
    printf("\n");
    p = gid2groupname(egid);  // 有效组 ID
    printf("eff=%s(%ld); ", p == NULL ? "???" : p, (long)egid);
    printf("\n");
    p = gid2groupname(sgid);  // save-set-group-id ID
    printf("saved=%s(%ld); ", p == NULL ? "???" : p, (long)sgid);
    printf("\n");
    p = gid2groupname(fsgid);  // 文件系统组 ID
    printf("fs=%s(%ld); ", p == NULL ? "???" : p, (long)fsgid);
    printf("\n");

    numGroups = getgroups(SG_SIZE, suppGroups);
    if(numGroups == -1)
    {
        perror("getgroups");
        return 1;
    }
    printf("Supplementary groups (%d): ", numGroups);
    for(j = 0; j < numGroups; j++)
    {
        p = gid2groupname(suppGroups[j]);
        printf("%s(%ld) ", p == NULL ? "???" : p, (long)suppGroups[j]);
    }
    printf("\n");
    return 0;
}
```

