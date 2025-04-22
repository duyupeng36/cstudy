# 用户和组

每个用户都拥有唯一的 **用户名** 和一个与之相关的数值型 **用户标识符 UID**

每个组也都拥有唯一的 **组名称**  和一个与之相关的数值型 **组标识符 GID**

> [!tip] 
> 
> 一个用户可以隶属于一个或多个组
> 

UID 和 GID 的主要用途有 $2$ 个：**确定系统资源的所有权** 和 **赋予进程访问资源的权限**

> [!example] 
> 
> 每个文件都属于某个特定的用户和组，而每个进程也拥有相应的 UID 和 GID 属性，这就决定了进程的所有者，以及进程访问文件时所拥有的权限
> 
> 关于文件的访问权限，参考 [[Linux 系统编程：文件属性#文件权限]]
> 

Unix/Linux 采用系统文件 `/etc/passwd` 和 `/etc/shadow` 管理用户和用户密码；采用系统文件 `/etc/group` 管理用户组。下面我们介绍这些文件中的内容

## 密码文件


针对系统的 **每个用户账号**，系统密码文件 `/etc/passwd` 会 **专列一行** 进行描述。每行都包含 $7$ 个字段，之间用 **冒号分隔**

```shell
# username:password:uid:gid:comments:home-dir:login-shell
ecs-user:x:1000:1000::/home/ecs-user:/bin/bash
```

每个字段分别是 **登录名 : 加密后的密码 : 用户ID(UID) : 组ID(GID) : 注释 : 主目录 : 登录 shell**

> [!tip] 登录名
> 
> 登录名，也称 **用户名**，是登录系统时，用户所必须输入的唯一名
> 
> 人类可读的符号，与数字用户标识符相对应
> 

> [!tip] 加密后的密码：长度为 $13$ 个字符
> 
> 当字符串长度超过 $13$ 个字符时，将禁止此账户登录，原因是此类字符串不能代表一个经过加密的有效密码
> 
> 现代 Unix/Linux 启用了 `shadow`，系统不会解释该字段，而经过加密处理的密码实际上却存储到 `shadow` 密码文件中。这时，`/etc/passwd` 中密码字段使用字符 `"x"` 表示 
> 
> 若 `/etc/passwd` 中密码字段为空，则该账户登录时无需密码。即便启用了 `shadow` 密码，也是如此
> 

> [!tip] UID：用户的数值型 ID
> 
> 如果该字段的值为 0，那么相应账户即具有 **特权级权限**。这种账号一般只有一个，称为 **特权用户**，其登录名为 `root`
> 
>  Linux 2.4 及其以后的版本则以 $32$ 位值来存储 UID，因此能够支持更多的用户数
> 

> [!tip] GID：组的数值型 ID
> 
> 用户属组中 **首选属组的数值型 ID**。这部分在组文件中详细介绍
> 

> [!tip] 注释：对用户的描述性文字
> 
> `finger` 之类的程序会显示此信息
> 

> [!tip] 主目录：用户登录后所处的初始化路径
> 
> 会以该字段内容来设置 `HOME` 环境变量
> 

> [!tip] 登录 shell：一旦用户登录，便交由该程序控制
> 
> 该程序为 `shell` 的一种，但也可以是其他任何程序
> 
> 如果该字段为空，那么登录 `shell` 默认为 `/bin/sh`
> 
> 会以该字段值来设置 `SHELL` 环境变量
> 

## 影子文件

由于许多非特权应用需要读取 `/etc/passwd` 中的其他文件信息，该文件不得不开发读权限，给密码破解工具带来了可乘之机。

作为防范此类攻击的手段之一，`shadow` 密码文件 `/etc/shadow` 应运而生。其理念是用户的所有 **非敏感** 信息存放于 _人人可读_ 的密码文件中，而经过加密处理的 **密码则由 `shadow` 密码文件单独维护**，仅供具有特权的程序读取

> [!tip] 
> 
> `/etc/shadow` 文件只允许特权进程读取
> 

`shadow` 密码文件包含有 **登录名**、经过加密的 **密码**，以及 **其他若干与安全性相关的字段**。每个字段也用冒号`:` 分隔

```shell
ecs-user:$6$u3YrqPwX$TFelEmqJX03cc0HMxfXC1INXglHy1c63fI7pomgJ/nltk1TDT7e/bsh2iFEtweaMNeKmMylQFtx8M9K5EFNTO1:19987:0:9999
```

## 组文件

出于各种管理方面的考虑，尤其是要控制对文件和其他系统资源的访问，对用户进行编组极具实用价值

对用户所属各组信息的定义由两部分组成: **密码文件中记录 GID** 和 **组文件中列出的用户所属的各组**


> [!tip] 
> 
> **在早期 UNIX 实现中，一个用户同时只能从属于一个组**。登录时，用户最初的属组关系由密码文件的组 ID 字段决定，在此之后，可使用 newgrp(1)命令去改变用户属组，但需要用户提供组密码（若该组处于密码的保护之下）
> 
> **4.2 BSD 引入了并发多属组的概念**，**POSIX.1-1990 随后对其进行了标准化**。采用这种方案，**组文件会列出每个用户所属的其他属组**
> 
> `groups(1)` 命令会显示当前 `shell` 进程所属各组的信息，如果将一个或多个用户名作为其命令行参数，那么该命令将显示相应用户所属各组的信息
> 

系统中的每个组在组文件 `/etc/group ` 中都对应着一条记录。每条记录包含 $4$ 个字段，之间以冒号分隔

```shell
# group-name:password:gid:user-list
adm:x:4:syslog,ecs-user
```

每个字段表示: **组名 : 经过加密处理的密码 : 组 ID(GID) :  用户列表**

> [!tip] 组名：组的名称
> 
> 与数值型组标识符相对应的人类可读符号
> 

> [!tip] 加密后的密码：非强制
> 
> 组密码属于 **非强制特性**，对应于该字段。随着多属组的出现，当今的 UNIX 系统已经很少使用组密码
> 
> 
> 
> 

> [!tip] GID：该组的数值型 ID
> 
> 正常情况下，对应于 GID 为 $0$ 的是特权组，只定义一个名为 `root` 的组
> 
> Linux 2.4 以后的版本则以 $32$ 位值来存储组 ID
> 

> [!tip] 用户列表：属于该组的用户名列表，之间以逗号分隔。
> 

## 获取用户和组的信息

### 从密码文件获取记录


函数 `getpwnam()` 和 `getpwuid()` 的作用是从密码文件 `/etc/passwd` 中获取记录


```c
#include <pwd.h>

struct passwd * getpwnam(const char *name);
struct passwd * getpwuid(uid_t uid);
/* 成功返回指针，指向存储记录的结构；错误返回 NULL*/
```

`getpwnam()` 函数提供一个参数 `name`，就会返回一个指针，指向 `passwd` 结构，其中包含了与密码记录相对应的信息

```c
/* /etc/passwd 中的记录 */
struct passwd
{

  char *pw_name;     /* 用户名.  */
  char *pw_passwd;      /* 散列后的密码：shadow 密码未启用时  */
  uid_t pw_uid;    /* UID.  */
  gid_t pw_gid;    /* GID.  */
  char *pw_gecos;    /* 真实名字  */
  char *pw_dir;         /* 家目录 */
  char *pw_shell;    /* 登录shell  */
};
```

`getpwuid()` 函数的返回结果与 `getpwnam()` 完全一致，但会使用提供给 `uid` 参数的数值型 UID 作为查询条件

> [!attention] 
> 
> `passwd` 结构也是由 **静态分配** 而成，所以每次调用都会改写该数据结构
> 
> 由于 `getpwnam()` 和 `getpwuid()` 返回的指针指向由静态分配而成的内存，故而二者都是 **不可重入** 的
> 

如果在 `/etc/passwd` 文件中未发现匹配记录，那么 `getpwnam()` 和 `getpwuid()` 将返回 `NULL`，且不会改变 `errno`。这意味着，可以使用如下代码，对 **出错** 和 **“未发现匹配记录”** 这两种情况加以区分

```c
struct passwd * pwd;

errno = 0;
pwd = getpwnam(name);
if(pwd == NULL) {
	if(errno == 0) {
		// 未发现匹配记录
	} else {
		// 出错
	}
}
```

> [!tip] 
> 
> `glibc` 从 2.7 版本开始遵循上述规范
> 
> 在 2.7 版本之前，如果未能在 `/ect/passwd` 文件中发现一条匹配记录，那么两个函数均会返回 `NULL`，并将 `errno` 设置为 `ENOENT`。
> 

#### 遍历密码文件

函数 `setpwent()` `getpwent()` 和 `endpwent()` 的作用是按顺序扫描密码文件 `/etc/passwd` 中的记录

```c
#include <pwd.h>

struct passwd *getpwent(void);
/* 成功返回指针，指向存储记录的结构；错误返回 NULL*/

void setpwent(void);
void getpwent(void);
```

函数  `getpwent()` 能够从密码文件 `/etc/passwd` 中逐条返回记录，当不再有记录或出错时，该函数返回 `NULL`

> [!tip] 
> 
> `getpwent()` 一经调用，会自动打开密码文件
> 
> 当密码文件处理完毕后，可调用 `endpwent()` 将其关闭
> 
> 如果对该文件处理到中途时，还可以调用 `setpwent()` 函数重返文件起始处
> 

### 从组文件获取记录

函数 `getgrnam()` 和 `getgrgid()` 的作用是从组文件 `/etc/group` 中获取记录

```c
#include <grp.h>

struct group *getgrnam(const char *name);
struct group *getgrgid(gid_t gid);

/* 成功返回指针，指向存储记录的结构；错误返回 NULL*/
```

函数 `getgrnam()` 和 `getgrgid()` 分别通过 **组名** 和 **GID** 来查找属组信息。两个函数都会返回一个指针，指向如下类型结构：

```c
/* The group structure.	 */
struct group
{
	char *gr_name;		/* Group name.	*/
	char *gr_passwd;		/* Password.	*/
	gid_t gr_gid;		/* Group ID.	*/
	char **gr_mem;		/* Member list.	*/
};
```

> [!attention] 
> 
> 与前述密码相关函数一样，对这两个函数的任何一次调用都会改写该结构的内容
> 

> [!tip] 未找到记录的行为：和 `getpwnam()` 和 `getpwuid()` 相同
> 
> 如果在 `/etc/group` 文件中未发现匹配记录，那么 `getgrnam()` 和 `getgrgid()` 将返回 `NULL`，且不会改变 `errno`
> 

#### 遍历组文件

函数 `setgrent()` `getgrent()` 和 `endgrent()` 的作用是按顺序扫描密码文件 `/etc/group` 中的记录

```c
#include <pwd.h>

struct passwd *getgrent(void);
/* 成功返回指针，指向存储记录的结构；错误返回 NULL*/

void setgrent(void);
void endgrent(void);
```

函数  `getgrent()` 能够从组文件 `/etc/group` 中逐条返回记录，当不再有记录或出错时，该函数返回 `NULL`

> [!tip] 
> 
> `getgrent()` 一经调用，会自动打开密码文件
> 
> 当密码文件处理完毕后，可调用 `endgrent()` 将其关闭
> 
> 如果对该文件处理到中途时，还可以调用 `setgrent()` 函数重返文件起始处
> 

### 从影子密码文件中获取记录

下列函数的作用包括从 `/etc/shadow` 影子密码文件中获取个别记录，以及扫描该文件中的所有记录

```c
#include <shadow.h>

struct spwd *getspnam(const char *name);  // 通过名字获取 /etc/shadow 中的一条记录

/* 下面三个函数用于遍历 /etc/shadow 文件 */
struct spwd *getspent(void); 
void setspent(void);
void endspent(void);
```

函数 `getspnam()` 和 `getspent()` 会返回指向 `spwd` 类型结构的指针。该结构的形式如下

```c
/* /etc/shadow 文件中的一条记录 */
struct spwd
{
	char *sp_namp;		/* 登录名  */
	char *sp_pwdp;		/* 散列后的密码  */
	long int sp_lstchg;		/* 上次更改的日期  */
	long int sp_min;		/* 密码变更之间的最小天数  */
	long int sp_max;		/* 密码变更之间的最大天数.  */
	long int sp_warn;		/* 警告用户更改密码的天数  */
	long int sp_inact;		/* 帐户可能处于非活动状态的天数  */
	long int sp_expire;		/* 从 1970 年 1 月 1 日到帐户到期的天数 */
	unsigned long int sp_flag;	/* Reserved.  */
};
```

## 密码加密和用户认证

某些应用程序会 **要求用户对自身进行认证**，通常会采取 **用户名/密码** 的认证方式。出于这一目的，应用程序可能会维护其自有的用户名和密码数据库。然而，或许是由于势所必然，或许是为了方便起见，有时需要让用户输入标准的用户名/密码：定义于 `/etc/passwd` 和 `/etc/shadow` 之中

> [!tip] 
> 
> 需要 **登录到远程系统** 的网络应用程序，诸如 `ssh` 和 `ftp`，就是此类程序的典范，必须按标准的 `login` 程序那样，对用户名和密码加以验证
> 

由于安全方面的原因，UNIX 系统 **采用 _单向加密_ 算法对密码进行加密**，这意味着由密码的加密形式将无法还原出原始密码

> [!hint] 
> 
>验证候选密码的唯一方法是使用同一算法对其进行加密，并将加密结果与存储于 `/etc/shadow` 中的密码进行匹配
> 

### 加密密码

加密算法封装于 `crypt()`函数之中

```c
#define __USE_MISC  /* Since glibc 2.28*/
#include <unistd.h>

char *crypt(const char *key, const char *salt);
```

> [!tip] 参数 `key`：密码
> 
> 最长可达 $8$ 字符的密钥，并施以数据加密算法 `DES` 的一种变体
> 

> [!tip] 参数 `salt`：指向一个 **两字符** 的字符串，用来扰动 `DES` 算法
> 

> [!tip] 返回值
> 
> 该函数会返回一个指针，指向长度为 $13$ 个字符的字符串，该字符串为 **静态分配** 而成，内容即为经过加密处理的密码
> 

`salt` 参数和经过加密的密码，其组成成员均取自同一字符集合，范围在 `[a-zA-Z0-9/.]`之间，共计 $64$ 个字符

> [!tip] 
> 
> 两个字符的 `salt` 参数可使加密算法产生 $4096=64 \times 64$ 种不同变化
> 
> 预先对整部字典进行加密，再以其中的每个单词与经过加密处理的密码进行比对的做法并不可行，破解程序需要对照字典的 $4096$ 种加密版本来检查密码
> 

由 `crypt()` 所返回的经过加密的密码中，**头两个字符是对原始 `salt` 值的拷贝**。也就是说，加密候选密码时，能够从已加密密码（存储于`/etc/shadow` 内）中获取 `salt` 值

> [!tip]
>  
> 加密新密码时，shell命令 `passwd` 会生成一个随机 `salt` 值
> 
> 事实上，在 `salt` 字符串中，只有前两个字符对 `crypt()` 函数有意义。因此，可以直接将已加密密码指定为 `salt` 参数
> 

> [!attention] 
> 
> 要想在 Linux 中使用 `crypt()`，在编译程序时需开启 `–lcrypt` 选项，以便程序链接  `crypt` 库
> 

### 获取密码

`getpass()` 函数从标准输入中读取用户输入的密码

```c
#define _DEFAULT_SOURCE /* glibc >= 2.19: _DEFAULT_SOURCE */
#include <unistd.h>

char *getpass(const char *prompt)
```

> [!tip] 参数 `prompt`：提示字符串
>

`getpass()`函数首先会 **屏蔽回显功能**，并停止对终端特殊字符的处理。然后，该函数会打印出 `prompt` 所指向的字符串，并从标准输入中读取一行输入。反回以 `NULL` 结尾的输入字符串作为函数结果

> [!tip] 
> 
> 返回结果之前，`getpass()` 会将终端设置还原
> 
> 注意，返回的字符串中字符 `\n` 被剥离
> 
