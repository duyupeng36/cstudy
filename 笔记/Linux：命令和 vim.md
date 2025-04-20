# Linux 命令和 vim 操作

## Linux 命令
### 用户管理

Linux 系统中有两种用户：**root** 和 **普通用户**

> [!tip] root 用户
> 
> root 用户即超级用户：具有最高的权限，可以执行任何命令，删除任何文件，创建和删除用户，**不受任何限制**

> [!tip] 普通用户
> 
> 普通用户能执行的命令有限的，限制只能删除属于自己的文件，不能创建和删除用户。普通用户能执行的操作是 **受限制** 的

#### 切换到指定用户

如果想切换到指定用户，可以使用命令 `su [用户名]`。例如，从普通用户切换到 `root` 用户，使用 `su root` 命令，此时，需要输入 `root` 用户的密码

```shell
dyp@dyp-AliYun:~$ su root
Password:
```

> [!tip] sudo 命令
> 
> 普通用户以 root 的身份去做某些事情，使用 `sudo` 命令
> 

#### 修改密码

使用 `su root` 切换用户时，提示输入 `root` 的密码。目前尚未给 `root` 用户设置密码。为了给 `root` 设置密码使用 `passwd [用户名]` 命令

```shell
dyp@dyp-AliYun:~$ sudo passwd root
[sudo] password for dyp:  # 当前用户的密码
New password:  # root 用户的新密码
Retype new password:  # 重复输入一次
passwd: password updated successfully
```

> [!tip] 以普通用户执行 `passwd` 命令修改其他用户的密码时，请使用 `sudo` 提权

#### 添加用户

如果想要在 Linux 系统中增加一个用户，请使用 `useradd [用户名]` 命令。使用 `useradd` 命令添加用户需要指定几个选项

> [!tip] 必选项
> 
> + `-m` 或者 `--create-home`：创建 **家目录**
> +  `-G` 或 `--groups GROUPS`：指定将新建用户添加到的用户组
> + `-s` 或 `--shell SHELL`：指定登录 `shell` 程序

> [!tip] 创建用户需要 `root` 权限才能执行

 例如，新建一个 `alice` 并添加到 `sudo` 用户组

```shell
dyp@dyp-AliYun:~$ sudo useradd -m -G sudo alice  # 创建用户名为 alice 的用户
[sudo] password for dyp:
dyp@dyp-AliYun:~$ sudo cat /etc/passwd | grep alice  # 为指定登录shell时默认使用 /bin/sh
alice:x:1002:1002::/home/alice:/bin/sh
```

> [!hint] 
> 
> 登录 `shell` 如果为指定会默认使用 `/bin/sh`

> [!tip] 文件 `/etc/passwd` 保存了用户的基本信息
> 
> 文件 `/etc/passwd` 文件中保存了 Linux 系统中所有用户的基本信息。每一行都是一个用户的基本信息，从左到右每一项为
> 
> `username:x:uid:gid:comment:home-dir:login-shell`
> 

> [!tip] 文件 `/etc/shadow` 保持了用户加密后的密码
> 
> 由于许多非特权应用需要读取 `/etc/passwd` 中的其他文件信息，该文件不得不开发读权限，给密码破解工具带来了可乘之机
> 
> 作为防范此类攻击的手段之一，`shadow` 密码文件 `/etc/shadow` 应运而生。其理念是用户的所有非敏感信息存放于“人人可读”的密码文件中，而 **经过加密处理的密码则由 shadow 密码文件单独维护，仅供具有特权的程序读取**
>

#### 删除用户

如果需要删除用户，请使用 `userdel [用户名]`。通常需要指定 `-r` 或 `--remove` 选项同步删除家目录

```shell
dyp@dyp-AliYun:~$ sudo userdel -r alice
userdel: alice mail spool (/var/mail/alice) not found
```

### 用户组

出于各种管理方面的考虑，尤其是要控制对文件和其他系统资源的访问，对用户进行编组极具实用价值

> [!tip] 用户组信息保存在 `/etc/group` 中
> 
> `/etc/group` 中的每一行，从左到右为
> 
> `group-name:group-passwd:gid:user-list`
> 
> + `group-passwd` 是非强制
>
 
#### 添加用户组

添加用户时，会默认以该用户成组。如果需要添加一个全新的用户组，可以使用 `groupadd [组名]`

```shell
dyp@dyp-AliYun:~$ sudo groupadd test
dyp@dyp-AliYun:~$ cat /etc/group | grep test
test:x:1002:
```

#### 删除用户组

删除用户组时，请使用 `groupdel [组名]`

```shell
dyp@dyp-AliYun:~$ sudo groupdel test
dyp@dyp-AliYun:~$ cat /etc/group | grep test
dyp@dyp-AliYun:~$
```

#### 将用户添加到用户组

除了在创建用户时指定用户所在的用户组，还可以使用 `usermod -G 组名 username` 命令为用户指定一个全新的组列表

使用 `usermod -a 组名 username` 将用户添加到新的组

### 目录和文件操作

#### 目录

##### 查看目录中的内容

命令 `ls [选项] [目录]` 会列出目录中的文件和子目录

>[!tip] ls` 命令有 $3$ 个选项可选
>
>+ `-l` 以列表形式
>+ `-h` 人类可读形式
>+ `-a` 列出目录中的所有文件和目录，包括隐藏的文件和目录
>


```shell
dyp@dyp-AliYun:~$ ls
go  python-study  roadmap-projects  snap  vblog
dyp@dyp-AliYun:~$ ls -l
total 20
drwxrwxr-x 4 dyp dyp 4096 Aug 18 19:22 go
drwxrwxr-x 2 dyp dyp 4096 Aug 27 22:56 python-study
drwxrwxr-x 6 dyp dyp 4096 Aug 20 15:26 roadmap-projects
drwx------ 3 dyp dyp 4096 Aug 18 18:19 snap
drwxrwxr-x 4 dyp dyp 4096 Aug 18 19:15 vblog
dyp@dyp-AliYun:~$ ls -lh
total 20K
drwxrwxr-x 4 dyp dyp 4.0K Aug 18 19:22 go
drwxrwxr-x 2 dyp dyp 4.0K Aug 27 22:56 python-study
drwxrwxr-x 6 dyp dyp 4.0K Aug 20 15:26 roadmap-projects
drwx------ 3 dyp dyp 4.0K Aug 18 18:19 snap
drwxrwxr-x 4 dyp dyp 4.0K Aug 18 19:15 vblog
dyp@dyp-AliYun:~$ ls -lha
total 100K
drwxr-x--- 14 dyp  dyp  4.0K Aug 28 18:30 .
drwxr-xr-x  3 root root 4.0K Aug 28 22:16 ..
-rw-------  1 dyp  dyp  9.1K Aug 29 01:02 .bash_history
-rw-r--r--  1 dyp  dyp   220 Aug 18 17:00 .bash_logout
-rw-r--r--  1 dyp  dyp  3.7K Aug 18 17:00 .bashrc
drwx------  7 dyp  dyp  4.0K Aug 18 20:01 .cache
drwxrwxr-x  3 dyp  dyp  4.0K Aug 18 18:19 .config
drwxrwxr-x  3 dyp  dyp  4.0K Aug 18 17:05 .dotnet
-rw-rw-r--  1 dyp  dyp   239 Aug 18 17:07 .gitconfig
drwxrwxr-x  4 dyp  dyp  4.0K Aug 18 19:22 go
-rw-------  1 dyp  dyp    20 Aug 28 18:15 .lesshst
drwx------  3 dyp  dyp  4.0K Aug 18 17:07 .local
-rw-r--r--  1 dyp  dyp   807 Aug 18 17:00 .profile
-rw-------  1 dyp  dyp     3 Aug 28 18:30 .psql_history
drwxrwxr-x 19 dyp  dyp  4.0K Aug 27 22:52 .pycharm_helpers
-rw-------  1 dyp  dyp    21 Aug 27 23:59 .python_history
drwxrwxr-x  2 dyp  dyp  4.0K Aug 27 22:56 python-study
drwxrwxr-x  6 dyp  dyp  4.0K Aug 20 15:26 roadmap-projects
drwx------  3 dyp  dyp  4.0K Aug 18 18:19 snap
drwx------  2 dyp  dyp  4.0K Aug 18 17:15 .ssh
-rw-r--r--  1 dyp  dyp     0 Aug 18 22:43 .sudo_as_admin_successful
drwxrwxr-x  4 dyp  dyp  4.0K Aug 18 19:15 vblog
drwxrwxr-x  5 dyp  dyp  4.0K Aug 25 10:56 .vscode-server
-rw-rw-r--  1 dyp  dyp   183 Aug 18 17:05 .wget-hsts
```

> [!tip] 默认显示当前目录下的文件和子目录
> 
> 使用 `ls -l` 查看目录中的文件时，左边第一组代表了文件的类型和权限
> 
> `d rwx rwx rwx`：第一个字母代表文件类型，第二组代表文件所有者权限，第三组代表文件属组权限，第四组代表其他用户的权限
> 
> + `-`：普通文件
> + `d`：目录文件
> + `l`：符号链接文件
> + `b`：块设备文件
> + `c`：字符设备文件
> + `p`：命名管道文件(FIFO)
> + `s`：socket 文件
> 

> [!tip] 隐藏文件和目录
> 
> `Linux` 中隐藏文件和目录是以 `.` 开头命名的

##### 切换工作目录

切换工作目录使用 `cd [目录]` 命令

> [!tip] 几个常用的快捷命令
> 
> + 回到用户的家目录
> 	+ `cd` 命令默认回到当前用户的家目录
> 	+ `cd ~`
> + 回到上一次目录
> 	+ `cd -`
> 

##### 查看当前工作目录

命令 `pwd` 可以显示当前工作目录

```shell
dyp@dyp-AliYun:~$ pwd
/home/dyp
```

##### 创建目录

创建目录使用 `mkdir [选项] 目录` 命令

> [!tip] 选项
> 
> `mkdir` 有几个选项可以比较常用
> + `-p` ：递归创建
> 

```shell
dyp@dyp-AliYun:~$ mkdir a/b/c
mkdir: cannot create directory ‘a/b/c’: No such file or directory
dyp@dyp-AliYun:~$ mkdir -p a/b/c  # -p 递归创建目录
dyp@dyp-AliYun:~$ ls
a  go  python-study  roadmap-projects  snap  vblog
dyp@dyp-AliYun:~$ ls -l a
```

##### 删除目录

删除目录使用 `rmdir [选项] 目录` 命令。**注意：目录必须是空目录**

> [!tip] 选项
> 
> `rmdir` 有几个选项可以比较常用
> + `-p` ：递归删除
> 

```shell
dyp@dyp-AliYun:~$ rmdir -p a/b/c
dyp@dyp-AliYun:~$
```

##### 显示目录的树形结构

命令 `tree [选项] [目录]` 会将目录中的所有文件显示为树形结构

> [!tip]
> 
> 该命令可能会没有，如果提示 `Command 'tree' not found` 请使用 `sudo apt install tree` 安装即可 
> 

```shell
dyp@dyp-AliYun:~$ tree roadmap-projects/
roadmap-projects/
├── expense-tracker
│   ├── fs
│   │   └── fs.go
│   └── go.mod
├── github-user-activity
│   ├── activity
│   │   └── activity.go
│   ├── events
│   │   └── events.go
│   ├── events.json
│   ├── go.mod
│   ├── main.go
│   ├── README.md
│   └── request.go
├── go.work
├── go.work.sum
├── README.md
└── task-tracker-cli
    ├── app
    │   ├── app.go
    │   └── functions.go
    ├── cmd
    │   ├── create.go
    │   ├── delete.go
    │   ├── list.go
    │   ├── root.go
    │   └── update.go
    ├── fs
    │   └── fs.go
    ├── go.mod
    ├── go.sum
    ├── main.go
    ├── README.md
    ├── task
    │   ├── status.go
    │   └── task.go
    └── tasks.json

10 directories, 27 files
```

> [!tip] 选项
> 
> + `-a` 显示所有文件(包含隐藏文件)，不包含 `.` 和 `..`
> + `-d` 只显示目录，不显示文件
> + `-u` 显示文件的属主
> + `-h` 以人类已读方式显示
> 

#### 文件

##### 创建文件

命令 `touch 文件名` 可以创建一个 **空文件**

```shell
dyp@dyp-AliYun:~$ ls | grep test.txt
dyp@dyp-AliYun:~$ touch test.txt
dyp@dyp-AliYun:~$ ls | grep test.txt
test.txt
```

##### IO 重定向：输出重定向

`shell` 会代表程序打开三个标准IO

| 文件描述符 | 用途   |
| :---- | :--- |
| `0`   | 标准输入 |
| `1`   | 标准输出 |
| `2`   | 标准错误 |

例如，命令 `echo ...` 可以在标准输出上写入内容

```shell
dyp@dyp-AliYun:~$ echo hello
hello
```

为了将输出重定向到 `test.txt` 中，可以使用 **输出重定向** `>` 或者 `>>`

```shell
dyp@dyp-AliYun:~$ echo hello > test.txt
dyp@dyp-AliYun:~$ cat test.txt
hello
```

命令 `cat 文件` 可以查看文件的内容

> [!tip] `>` 和 `>>` 的区别
> + `>` 覆盖文件的内容
> + `>>` 最佳内容到文件末尾

##### IO 重定向：输入重定向

**输入重定向** `<`。例如，利用 IO重定向，使用 `cat` 命令复制文件

```shell
dyp@dyp-AliYun:~$ cat > test2.txt < test.txt
dyp@dyp-AliYun:~$ ls | grep test
test2.txt
test.txt
dyp@dyp-AliYun:~$ cat test.txt
hello
hello, word
dyp@dyp-AliYun:~$ cat test2.txt
hello
hello, word
```

##### 复制文件

复制文件使用 `cp [选项] 源 目标` 

```shell
dyp@dyp-AliYun:~$ ls
go  python-study  roadmap-projects  snap  test.txt  vblog
dyp@dyp-AliYun:~$ cp test.txt test2.txt  # 复制到文件
dyp@dyp-AliYun:~$ ls
go  python-study  roadmap-projects  snap  test2.txt  test.txt  vblog
dyp@dyp-AliYun:~$ cp test.txt vblog/  # 复制到一个目录
dyp@dyp-AliYun:~$ ls vblog/ | grep test
test.txt
```

> [!tip] `cp` 默认是不能拷贝目录文件的，如果需要拷贝目录文件需要指定 `-r` 选项

> [!tip] 选项
> + ` -R, -r, --recursive` ：拷贝目录时递归拷贝目录下的所有文件 
> + `-f`：拷贝时，同名文件强制覆盖，不提醒
> + `-i`：拷贝时，同名文件进行提醒

##### 移动文件

移动文件时使用命令 `mv [选项] 源 目标` 将 **源(目录或文件)** 移动到 **目标(目录或文件)**

```shell
dyp@dyp-AliYun:~$ ls
go  python-study  roadmap-projects  snap  test2.txt  test.txt  vblog
dyp@dyp-AliYun:~$ mv test2.txt test3.txt  # 重命名
dyp@dyp-AliYun:~$ ls | grep test
test3.txt
test.txt
```

> [!tip] 选项
>
> + `-f`：移动时，强制覆盖同名文件
> + `-i`：移动时，如果出现同名文件进行提示

##### 删除文件

删除文件使用 `rm [选项] 目标` 删除目标指定的文件

```shell
dyp@dyp-AliYun:~$ ls
go  python-study  roadmap-projects  snap  test3.txt  test.txt  vblog
dyp@dyp-AliYun:~$ rm test3.txt # 删除普通文件
dyp@dyp-AliYun:~$ ls
go  python-study  roadmap-projects  snap  test.txt  vblog
```

如果目标是目录，需要使用 `-r` 选项递归删除目录中的所有文件

```shell
dyp@dyp-AliYun:~$ ls
a  go  python-study  roadmap-projects  snap  test.txt  vblog
dyp@dyp-AliYun:~$ rm a
rm: cannot remove 'a': Is a directory
dyp@dyp-AliYun:~$ rm -r a
dyp@dyp-AliYun:~$ ls
go  python-study  roadmap-projects  snap  test.txt  vblog
```

> [!tip] 选项
> + `-f` 强制删除文件。删除文件不属于自己时，系统会提醒。指定 `-f` 时，就不会提醒

##### 查找文件

如果需要查找文件请使用 `find [path ...] [expression]`

> [!tip] `path`：搜索文件的起始路径，默认为当前目录

> [!tip] `expression` 包含 **运算符**, **选项** , **测试条件** 和 **动作**
> 
> 运算符
> + 取反：`!EXPR` 或者 `-not EXPR` 
> + 逻辑或: `EXPR1 -o EXPR2` 或者 `EXPR1 -or EXPR2`
> + 逻辑与：`EXPR1 -a EXPR2` 或 `EXPR1 -and EXPR2`
>
>测试条件
>+ `-amin n`：查找在 `n` 分钟内被访问过的文件
>+ `-atime n`：查找在 `n*24` 小时内被访问过的文件
>+ `-cmin n`：查找在 `n` 分钟内状态发生变化的文件（例如权限）
>+ `-ctime n`：查找在 `n*24` 小时内状态发生变化的文件（例如权限）
>+ `-mmin n`：查找在 `n` 分钟内被修改过的文件。
>+ `-mtime n`：查找在 `n*24` 小时内被修改过的文件
>- `-name pattern`：按文件名查找，支持使用通配符 `*` 和 `?`
>- `-type type`：按文件类型查找，可以是 `f`（普通文件）、`d`（目录）、`l`（符号链接）等
>- `-size [+-]size[cwbkMG]`：按文件大小查找，支持使用 `+` 或 `-` 表示大于或小于指定大小，单位可以是 `c`（字节）、`w`（字数）、`b`（块数）、`k`（KB）、`M`（MB）或 `G`（GB）
>- `-user username`：按文件所有者查找
>- `-group groupname`：按文件所属组查找
>
>动作
>+ `-print` 默认动作打印
>+ `-delete` 删除
>+ `-exec COMAND {}\;` 执行某个命令
>

#### 文件查看

##### 查看文件内容

命令 `cat` 可以查看文件内容，也可以当做向文件写入数据的工具

```shell
dyp@dyp-AliYun:~$ cat test.txt  # 查看文件内容
hello
hello, word
ahag
你好
dyp@dyp-AliYun:~$ cat >> test.txt << eof  # 从标准输入中读取内容，并写入到 test.txt 中，遇到eof 结束
> ahag
> 你好
> eof
dyp@dyp-AliYun:~$ cat test.txt
hello
hello, word
ahag
你好
```

##### 查看文件前 $n$ 行

命令 `head [选项] 文件名` 查看文件开头几行

```shell
dyp@dyp-AliYun:~$ head -n 2 test.txt
hello
hello, word
```

##### 查看文件后 $n$ 行

命令 `tail [选项] 文件名` 查看文件尾部几行

```shell
dyp@dyp-AliYun:~$ tail -n 2 test.txt
ahag
你好
```

##### 内容排序

命令 `sort file` 对文件内容按行排序

```shell
dyp@dyp-AliYun:~$ sort test.txt
ahag
hello
hello, word
你好
```

##### 内容去重

命令 `uniq 文件` 可以对相邻行进行去重

```shell
dyp@dyp-AliYun:~$ sort test.txt
hello
hello
hello, word
dyp@dyp-AliYun:~$ sort test.txt | uniq
hello
hello, word
```

##### 内容统计

命令 `wc [选项] 文件` 统计文件的行数，单词数，字符个数

```shell
dyp@dyp-AliYun:~$ wc test.txt
 3  4 24 test.txt
```

> [!tip] `wc` 的输出内容: **行数** **单词数** **字符数** 

##### 内容搜索

命令 `grep [查找模式] [文件名 ....]` 搜索文件中的内容

> [!tip] 查找模式
> 
> + `-E` 使用扩展正则
> + `-G` 使用基本正则
> + `-e, --regexp=PATTERNS` 使用正则匹配
> + `-i` 忽略大小写
> + `-w` 匹配完整单词
>

#### 类型和权限

Linux 系统中，文件的类型和权限采用 $16$ 位二进制表示，高 $4$ 位表示文件类型，低 $12$ 位表示文件权限

![[Pasted image 20241003220239.png]]


> [!tip] 每个位置为 $1$ 代表具有对应的权限
> 
> + `R` 代表读权限
> + `W` 代表写权限
> + `X` 代表执行权限
>   
>   针对可执行文件
>   
> + `U` 代表 Set-User-ID：如果是可执行程序，程序执行时，**进程的有效 ID 就是文件的所有者 ID**
> + `G` 代表 Set-Group-ID：如果是可执行程序，程序执行时，**进程的有效组 ID 就是文件属组 ID**
>   
>   针对目录文件
>   
> + `T` 代表粘滞位：当⽬录被设置了粘滞位权限以后，即便⽤户对该⽬录有写⼊权限，也不能删除该⽬录中其他⽤户的⽂件数据，⽽是 **只有该⽂件的所有者和 root ⽤户才有权将其删除**

##### 查看文件类型

命令 `file 文件` 可以查看文件的类型

```shell
dyp@dyp-AliYun:~$ file /usr/bin/ls
/usr/bin/ls: ELF 64-bit LSB pie executable, x86-64, version 1 (SYSV), dynamically linked, interpreter /lib64/ld-linux-x86-64.so.2, BuildID[sha1]=36b86f957a1be53733633d184c3a3354f3fc7b12, for GNU/Linux 3.2.0, stripped
dyp@dyp-AliYun:~$ file test.txt
test.txt: Unicode text, UTF-8 text
```

##### 修改权限

如果需要修改文件权限，请使用 `chmod` 命令

`chmod` 命令有两种使用方式：**字母权限** 和 **数字权限**

> [!tip] 字母权限
>
>使用 `u, g, o` 分别代表文件的 **属主(u)** **属组(g)** 和 **其他用户(o)**
>
>使用 `+, -, =` 分别代表 **添加权限**，**移除权限**，**设置权限** 操作
>
>使用 `s, t, r, w, x` 分别代表 
>+ **设置用户/组ID(`s`)** 
>+ **粘滞位(`t`)** 
>+ **读(`r`)** 
>+ **写(`w`)** 
>+ **执行(`x`)** 
>

```shell
dyp@dyp-AliYun:~$ ls -al | grep go
drwxrwxr-x  4 dyp  dyp  4096 Aug 18 19:22 go
dyp@dyp-AliYun:~$ chmod u-x go  # 移除属主的执行权限
dyp@dyp-AliYun:~$ ls -al | grep go
drw-rwxr-x  4 dyp  dyp  4096 Aug 18 19:22 go
```

> [!tip] 八进制数字权限
> 
> 使用 $4$ 个八进制数字分别代表
> 
> 第一个八进制数是下面几个数的和
> + `4` 代表 `Set-User-ID` 
> + `2` 代表 `Set-Group-ID` 
> + `1` 代表 `sticky`
>   
>  第二至四个八进制数也是下面几个数的和 
>  + `4` 代表 **读**
>  + `2` 代表 **写**
>  + `1` 代表 **执行**

```shell
dyp@dyp-AliYun:~$ ls -l | grep go
drwxrwxr-x 4 dyp dyp 4096 Aug 18 19:22 go
dyp@dyp-AliYun:~$ chmod 665 go  # 移除属主和属组的执行权限
dyp@dyp-AliYun:~$ ls -l | grep go
drw-rw-r-x 4 dyp dyp 4096 Aug 18 19:22 go
```

##### 修改属主

如果需要修改文件的属主，可以使用 `chown [选项] 用户名 文件` 命令。将 `文件` 的属主修改为 `用户名` 指定的用户

> [!tip] 选项
> + `-R` 递归：子目录也同步修改

##### 修改属组

如果需要修改文件的属组，可以使用 `chgrp [选项] 组名 文件` 命令。将 `文件` 的属组修改为 `组名` 指定的组

> [!tip] 选项
> + `-R` 递归：子目录也同步修改

### 磁盘
#### 查磁盘的整体使用情况

命令 `df` 可以查看文件系统的整体使用状况

```shell
dyp@dyp-AliYun:~$ df  -h
Filesystem      Size  Used Avail Use% Mounted on
tmpfs           169M  1.1M  168M   1% /run
/dev/vda3        40G  5.1G   33G  14% /
tmpfs           843M  1.1M  842M   1% /dev/shm
tmpfs           5.0M     0  5.0M   0% /run/lock
/dev/vda2       197M  6.1M  191M   4% /boot/efi
tmpfs           169M  4.0K  169M   1% /run/user/1001
```

#### 查看文件和目录占用的磁盘大小

命令 `du` 可以查看某个文件或目录占用的磁盘大小

```shell
dyp@dyp-AliYun:~$ df  -h
Filesystem      Size  Used Avail Use% Mounted on
tmpfs           169M  1.1M  168M   1% /run
/dev/vda3        40G  5.1G   33G  14% /
tmpfs           843M  1.1M  842M   1% /dev/shm
tmpfs           5.0M     0  5.0M   0% /run/lock
/dev/vda2       197M  6.1M  191M   4% /boot/efi
tmpfs           169M  4.0K  169M   1% /run/user/1001
```

### 归档和压缩

归档命令 `tar [选项]` 可以创建归档文件或解除归档

#### 归档

创建归档文件使用: `tar -cvf 归档文件名.tar [文件....]`

> [!tip] 选项
> + `-c` 创建归档文件
> + `-v` 显示归档过程
> + `-f` 使用归档文件

#### 提取归档文件

从归档文件中提取文件使用：`tar -xvf 归档文件名.tar [-C 目标目录]` 

> [!tip] 选项
> + `-x` 解除归档 

#### 归档并压缩

使用 `gzip [选项] 源文件 目标文件` 压缩或解压文件

> [!tip] 选项
> + `-r` 压缩文件
> + `-d` 解压

归档并压缩(使用`gzip`)： `tar -cvzf 归档文件名.tar.gz [文件....]` 

归档并压缩(使用`bzip2`)：`tar -cvjf 归档文件名.tar.bz2 [文件....]`

## vim 操作

`vim` 是一个命令行的文本编辑器。`vim` 有三种模式：**命令模式**  **编辑模式** 和 **末行模式**

当使用 `vim` 打开文件时，`vim` 默认处于命令模式

> [!tip] 从命令模式进入编辑模式
> + `i` 在光标处插入
> + `I` 在光标行首插入
> + `a` 在光标后插入
> + `A` 在光标行尾插入
> + `o` 在光标下一行插入
> + `O` 在光标上一行插入

> [!tip] 从编辑模式进入命令模式
> + `esc` 从编辑模式进入命令模式

### 命令模式

在命令模式下，支持操作有很多

#### 删除与复制

| 命令      | 描述                  |
| :------ | :------------------ |
| `x`     | 删除光标处的字符            |
| `dd`    | 删除光标所在的整行           |
| `[n]dd` | 从光标所在位置开始删除连续 $n$ 行 |
| `D`     | 删除光标至行尾             |
| `yy`    | 复制光标所在行             |
| `[n]yy` | 从光标所在行开始复制连续的 $n$ 行 |
| `p`     | 在光标处粘贴              |
| `u`     | 撤销上次操作              |

#### 移动光标


| 命令       | 描述           |
| :------- | :----------- |
| `^`      | 光标移动到行首      |
| `$`      | 光标移动到行尾      |
| `ctrl+d` | 向下翻半页        |
| `ctrl+f` | 向下翻一页        |
| `ctrl+u` | 向上翻半页        |
| `ctrl+b` | 向上翻一页        |
| `gg`     | 光标定位到文档开头    |
| `G`      | 光标定位到文档末尾    |
| `H`      | 光标定位到当前页首    |
| `L`      | 光标定位到当前页尾    |
| `w`      | 光标向后移动一个单词   |
| `b`      | 光标向前移动一个单词   |
| `[n]+`   | 光标向后移动 $n$ 行 |
| `[n]`-   | 光标向前移动 $n$ 行 |

#### 查找

命令模式输入 `/search-string` 可以全文查找指定的字符串。按回车后，光标定位到第一个 `search-string` 的开头位置
+ `n` 光标移动到下一个匹配位置
+ `N` 光标移动到上一个匹配位置

#### 块操作

命令模式中的 `v` 命令，可以启用块选择。

命令模式 `ctrl+v` 命令，可以按列选择

### 末行模式

想要进入末行模式，首先 `vim` 必须先处于命令模式，然后按 `:` 进入末行模式

#### 保存和退出

编辑完成之后，如果需要退出 `vim` 可以使用下面几个末行命令

> [!tip] 末行命令
> + `q` 退出，未修改
> + `q!` 强制退出，放弃修改内容
> + `w` 写入修改
> + `wq` 保存并退出
> + `wq!` 保持并强制退出
>  
>  这些命令必须在末行模式下
>  

#### 替换

在末行模式中，可以执行替换操作。替换的末行命令为： `:start,end/[src]/[dst]/ig`

> [!tip]
> 
> `start` 起始行号
> 
> `end` 结束行号
> 
> `/[src]` 原始文本
> 
> `/[dst]` 目标文本
> 
> `/i` 忽略大小写，`/g` 全部替换

如果需要执行全文替换，末行命令为：`:1,$/[src]/[dst]/ig` 或者 `:%s/[src]/[dst]/ig`

#### 以十六进制显示文件内容

末行命令 `:%!xxd` 以十六进制显示文件内容。`:%!xdd -r` 返回文本模式
