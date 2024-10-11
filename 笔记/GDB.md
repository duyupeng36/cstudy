# GDB

之前，我们在 CLion 中调试的时候，需要在出现 bug 的大概位置打断点，然后执行 **单步调试**

> [!tip]
> 
> 单步调试每次执行一行 C 代码。编译时需要保证汇编代码与原始 C 代码的结构类似，所以编译时不要开启编译时优化
> 
> 由于在汇编代码中是没有变量名等信息。为了监视变量，因此在编译时需要使用 GCC 的 `-g` 选项补充调试信息
> 

下面的代码将作为演示使用 GDB 的例子

```c title:main.c
#include <stdio.h>

void func(int i)
{
    printf("I am func, i = %d\n", i);
}

int main() {
    int j = 10;
    int arr[3] = {1,2,3};
    int *p;
    arr[2] = 4;
    p = arr;
    func(j);
    for(int i = 0; i < 5; i++)
    {
        puts("Hello, World!");
    }
    return 0;
}
```

要使用 GDB 就需要在编译时关闭优化，并附加调试信息

```shell
gcc -O0 -g main.c -o main
```

启动 GDB 调试 `main` 可执行程序

```shell
$ gdb main  # 启动 GDB
GNU gdb (GDB) 15.2
Copyright (C) 2024 Free Software Foundation, Inc.
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.
Type "show copying" and "show warranty" for details.
This GDB was configured as "x86_64-pc-linux-gnu".
Type "show configuration" for configuration details.
For bug reporting instructions, please see:
<https://www.gnu.org/software/gdb/bugs/>.
Find the GDB manual and other documentation resources online at:
    <http://www.gnu.org/software/gdb/documentation/>.

For help, type "help".
Type "apropos word" to search for commands related to "word"...
Reading symbols from main...
(gdb) 
```

下表列出了 GDB 的基本操作命令

| 命令                          | 效果                        |
| :-------------------------- | :------------------------ |
| 开始和停止                       |                           |
| `quit`                      | 退出 GOB                    |
| `run`                       | 运行程序到第 $1$ 个断点处           |
| `kill`                      | 停止程序                      |
|                             |                           |
| 命令行参数                       |                           |
| `set 程序名 命令行参数列表`           | 设置命令行参数                   |
| `show 程序名`                  | 查看命令行参数                   |
|                             |                           |
| 断点                          |                           |
| `break [文件名:] 行号 or 函数名`    | 在函数的入口地址处设置断点             |
| `break * 地址`                | 在地址处设置断点                  |
| `delete 1`                  | 删除 $1$ 号断点                |
| `delete`                    | 删除所有断点                    |
| `ignore 1 2`                | 忽略 $1$ 号断点 $2$ 次          |
|                             |                           |
| 执行                          |                           |
| `stepi`                     | 执行 $1$ 条指令                |
| `stepi 4`                   | 执行 $4$ 条指令                |
| `nexti`                     | 类似于 `stepi`，但是以函数调用为单位    |
| `continue`                  | 继续执行到下一个断点                |
| `finish`                    | 运行到当前函数返回                 |
|                             |                           |
| 检查代码                        |                           |
| `disas`                     | 反汇编当前函数                   |
| `disas 函数名`                 | 反汇编函数                     |
| `disas 地址`                  | 反汇编位于指定地址附近的函数            |
| `disas 地址1,地址2`             | 反汇编指定地址范围内的代码             |
|                             |                           |
| 检查数据                        |                           |
| `print /x $rip`             | 以十六进制输出 `PC` 中的值          |
| `print $rax`                | 以 十进制输出`％rax` 的内容         |
| `print /x $rax`             | 以 十六进制输出`％rax` 的内容        |
| `print /t $rax`             | 以 二进制输出 `％rax` 的内容        |
| `print 0x1OO`               | 以十进制输出立即数                 |
| `print /x 555`              | 以十六进制输出立即数                |
| `print /x ($rsp + 8)`       | 以 十 六进制输出`％rsp` 的内容加上 `8` |
| `print *(long *) 地址`        | 输出位于指定地址处的长整型数据           |
| `print *(long *) ($rsp+ 8)` | 输出位于地址 `%rsp+ 8` 处的长整型数据  |
| `display 表达式`               | 单步调试时先生表达式的值              |
|                             |                           |
| 查看内存                        |                           |
| `x/2g 地址`                   | 检查从指定地址开始的双字(8 字节）        |
| `x/20b 函数名`                 | 检查函数的前 $20$ 个字节           |
|                             |                           |
| 其他有用信息                      |                           |
| `info frame`                | 有关当前栈帧的信息                 |
| `info registers`            | 所有寄存器的值                   |
| `info break`                | 查看断点信息                    |
| `info display`              | 查看 `display` 信息           |
| `help`                      | 获取有关 G DB 的信息             |

## 查看内存

查看内存是非常复杂的，我们详细解释这部分内容。解释内存使用 GDB 命令 `x`

```shell
(gdb) help x  
Examine memory: x/FMT ADDRESS.  
```

> [!tip] ADDRESS：要检查的内存地址的表达式
> 

> [!tip] FMT 有三个部分组成：**计数数量**,  **格式字符**, **尺寸字符**
> + 计数数量：这是一个数字
> + 格式字符：指定内存中的值如何显示
> + 尺寸字符：指定单位计数数量的单位

| 格式字符 | 描述       |
| :--- | -------- |
| `o`  | 八进制      |
| `x`  | 十六进制     |
| `d`  | 十进制      |
| `u`  | 无符号十进制   |
| `t`  | 二进制      |
| `f`  | 浮点       |
| `a`  | 地址       |
| `i`  | 指令       |
| `c`  | 字符       |
| `s`  | 字符串      |
| `z`  | 十进制，左侧补零 |
 
| 尺寸字符 | 描述   |
| :--- | :--- |
| `b`  | 字节   |
| `h`  | 半字   |
| `w`  | 字    |
| `g`  | 8 字节 |

```shell
(gdb) x/3tw arr
0x7fffffffe07c: 00000000000000000000000000000001        00000000000000000000000000000010        00000000000000000000000000000011
(gdb) x/4tb arr
0x7fffffffe07c: 00000001        00000000        00000000        00000000
```

## 检查崩溃的程序

程序崩溃时，会将内存堆栈信息存储到核心转储文件(`core` 文件)

> [!tip] 
> 
> 核心转储是进程意外终止时包含进程地址空间（内存）的文件
> 
> 生成 coredump 文件的位置，默认位置与可执行程序在同一目录下，文件名是 `core.***`，其中 `***` 是一个数字代表进程的 `PID`
>  

为了产生核心转储文件，我们使用如下代码进行模拟

```c
#include <stddef.h>

int main() 
{
        int *p=NULL;
        *p = 1;  // 悬空指针问题
        return 0;
}
```

编译并运行该程序

```shell
$ gcc -g -O0 error.c -o error
$ ./error
段错误（核心已转储）
```

查看当前目录中并没有发现类似 `core` 文件名的文件。这可能是由于系统资源限制了核心转储文件的生成

```shell
$ ulimit -a
...
core file size              (blocks, -c) unlimited
...
```

系统并未限制核心转储文件的大小。因此，使用 `man core` 查看相关说明

```shell
$ man core
....
/proc/sys/kernel/core_pattern  is  empty  and /proc/sys/kernel/core_uses_pid contains the value 0.  (These
files are described below.)  Note  that  if  /proc/sys/kernel/core_pattern  is  empty  and  /proc/sys/ker‐
nel/core_uses_pid  contains  the value 1, core dump files will have names of the form .pid, and such files
are hidden unless one uses the ls(1) -a option
....
```

我们需要对文件 `/proc/sys/kernel/core_pattern` 进行修改，该文件是 `coredump` 文件名的模式

```shell
echo core > /proc/sys/kernel/core_pattern 
```

再次执行示例程序。此时，在当前目录就产生了 `core` 文件

```shell
$ ./error
段错误（核心已转储）
$ ls -l
-rw------- 1 dyp dyp 290816 10月 8日 21:05 core.558741  # 这就是产生的 core 文件
-rwxr-xr-x 1 dyp dyp  15256 10月 8日 20:55 error
-rw-r--r-- 1 dyp dyp     96 10月 8日 18:42 error.c
```

使用 GDB 加载 `core` 文件

```shell
$ gdb error core.***
...
Core was generated by `./error'.
Program terminated with signal SIGSEGV, Segmentation fault.
#0  0x0000611127a63129 in main () at error.c:7
7               *p = 1;  // 悬空指针问题
```
