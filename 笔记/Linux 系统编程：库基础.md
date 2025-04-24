# 库基础

> [!tip] 库是一种 **将函数打包成一个单元使之能够 _在运行时被多个进程共享_ 的技术**

构建程序的一种方式是简单地将每一个源文件编译成目标文件，然后将这些目标文件链接在一起组成一个可执行程序

例如，我们在 `main.c` 中调用了 `add()` 函数。这个 `add` 函数定义在 `add.c` 中

```c title:main.c
#include <stdio.h>

int add(int loperand, int roperand);

int main() {
    printf("add(3, 4) = %d\n", add(3, 4)); 
    return 0;
}
```

```c title:add.c
int add(int loperand, int roperand) {
    return loperand + roperand;
}
```

`main.c`  和 `add.c` 两个源文件都能通过编译，但是在链接阶段会因为找不到符号的定义而失败

![[Pasted image 20241007182354.png]]

但是，将这 `add.o` 和 `main.o` 一起链接就能通过链接，从而形成可执行文件

![[Pasted image 20241007182511.png]]

## 目标库

示例：实现一个计算功能的目标库。既然作为库文件提供给其他调用者使用，那么就需要让调用者知道其中的函数原型。首先，就需要创建一个 `calc.h` 的头文件，并键入如下内容

```c title:cacl.h
#ifndef __CALC_H_
#define __CALC_H_

/**
 * @a: left operand
 * @b: right operand
 * @return: a + b
*/
int add(int a, int b);

/**
 * @a: left operand
 * @b: right operand
 * @return: a - b
*/
int sub(int a, int b);

#endif
```

然后，创建一个 `.c` 文件，实现 `add` 和 `sub` 函数

```c title:calc.c
#include "cacl.h"

int add(int a, int b){
    return a + b;
}


int sub(int a, int b){
    return a - b;
}
```

同时，我们想要显示这个计算等式，创建 `show.h` 函数，并键入如下内容

```c title:show.h
#ifndef _SHOW_H_
#define _SHOW_H_

/**
 * @left: left operand
 * @op: operator
 * @right: right operand
*/
void show(int left, char op, int right);

#endif
```

在 `show.c` 中给出 `show` 函数的实现

```c title:show.c
#include <stdio.h>

#include "show.h"
#include "calc.h"

#define BUFSIZ 1024

void show(int left, char op, int right) {
    char buf[BUFSIZ];
    int result;
    switch(op) {
        case '+':
            result = add(left, right);
            break;
        case '-':
            result = sub(left, right);
            break;
        default:
            fprintf(stderr, "Unknown operator '%c'\n", op);
            return;
    }
    snprintf(buf, BUFSIZ, "%d %c %d = %d\n", left, op, right, result);
    puts(buf);
}
```

现在，我们可以在 `main.c` 中测试 `calc` 库

```c title:main.c
#include "calc.h"
#include "show.h"

int main()
{
    show(1, '+', 2);  // 显示 1 + 2 = 3
    return 0;
}
```

构建程序的方法就是简单地将每一个源文件编译成 **目标文件**，然后将这些目标文件 **链接** 在一起组成一个可执行程序

```shell
gcc -g -c calc.c show.c main.c  # 创建 calc.o show.o main.o 三个目标文件
gcc -g -o main_nolib main.o calc.o show.o  # 将三个目标文件链接在一起
```

执行 `main_nolib` 就可检查这些程序是否能正确运行

![[Pasted image 20241007185009.png]]

**链接** 实际上是由一个单独的 **链接器程序 `ld`** 来完成的。当使用 GCC 链接一个程序时，GCC 会在幕后调用 `ld`

在 Linux 上应该总是通过 GCC 间接地调用链接器，因为 GCC 能够确保使用正确的选项来调用 `ld` 并将程序与正确的库文件链接起来

虽然可以通过 **共享源文件** 的方式让多个程序使用相同的代码，但是这样共享代码的缺陷

> [!question] 共享源文件的缺陷
> 
> 每次都需要编译这些源文件，浪费编译时间
> 
> 源码被泄露
> 

也可以通过 **共享目标文件**，并提供头文件的方式让多个程序使用相同的代码，虽然可以保护源码并节省编译时间，但是也存在下列缺陷

> [!question] 共享目标文件的缺陷
> 
> 在链接的时候仍然需要为所有目标文件命名
> 
> 大量的目标文件会散落在系统上的各个目录中
> 

一种好的解决方法就是将 **一组相关的目标文件** 组织成一个被称为 **对象库** 的单元。对象库分为两种：**静态的** 和 **共享的**。共享库是一种更加现代化的对象库，它比静态库更具优势

## 静态库

**静态库也被称为归档文件**，它是 UNIX 系统提供的第一种库。静态库能带来下列好处

> [!tip] 静态库的好处
> 
> 可以将一组经常被用到的目标文件组织进单个库文件，这样就可以使用它来构建多个可执行程序并且在 **构建各个可执行程序的时候无需重新编译原来的源代码文件**
>  
> **链接命令变得更加简单了**。在链接命令行中只需要指定静态库的名称即可，而无需一个个地列出目标文件了。链接器知道如何搜素静态库并将可执行程序需要的对象抽取出来
> 

静态库本质上就一个 **归档文件**：其中 **包含了需要被共享的目标文件**

> [!tip]
> 
> 该归档文件记录了每个目标文件的各种特性：文件权限、用户和组ID，最后修改时间等
> 

### 创建静态库

创建和维护静态库使用 Linux 命令 `ar` 

```shell
ar [选项] [库名] 目标文件列表
```

> [!tip] 选项：执行需要执行的操作
> 
> + `-r` 替换目标文件
> + `-t` 现象归档文件中的目录表
> + `-v` 显示目标文件的其他特性
> + `-d` 删除
> 

> [!tip] 库名: `lib{name}.a`
> 
> 注意 `{name}` 表示需要替换的名字
> 

> [!tip] 目标文件目录：指定需要被归档的目标文件
> 

创建和更新静态库的标准方法就是使用 `ar -r` ，使用下面的命令可以构建一个静态库

```shell
ar -r libcalc.a calc.o show.o  # 将上述 calc 库的目标文件归档为一个静态库
```

![[Pasted image 20241007213305.png]]

### 使用静态库

现在通过静态库使用 `calc` 和 `show` 中的函数。如下 `main` 函数的定义

```c title:main.c
#include "calc.h"
#include "show.h"

int main()
{
    show(1, '+', 2);  // 显示 1 + 2 = 3
    return 0;
}
```

通过下面的 `gcc` 命令编译

```shell
gcc -g -o main_static_lib main.c   
```

将会报错：`undefined reference to show`。这是因为 `gcc` 无法搜索到静态库，无法链接到 `show` 函数

将程序与静态库链接的方法有两种：**直接指定静态库全名** 和 **使用 `-l` 选项指定库名**

> [!tip] 直接指定静态库全名：要求静态库文件位于 GCC 执行的目录下
> 
> ```shell
> gcc -g -o main_static_lib main.c libcalc.a
> ```
> 

> [!tip] 使用 GCC 的 `-l` 选项指定库名：库位于链接器搜索目录中
> 
> 链接器搜索库的目录可以是 `/usr/lib`
> 
> ```shell
> gcc -g -o main_static_lib main.c -lcalc  # 注意，库名去除了 lib 前缀和 .a 后缀
> ```
> 
> 库的文件名去除了 `lib` 前缀和 `.a` 后缀
> 
> 如果库没有位于链接器搜索的目录中，GCC 编译选项 `-L` 可以指定库所在的目录
> 
> ```shell
> gcc -g -o main_static_lib main.c -Llibdir -lcalc  # libdir 为库所在的目录
> ```
> 
> 环境变量 `LIBRARY_PATH` 也可以指定库的搜索路径
> 
> ```shell
> export LIBRARY_PATH=.                        # 指定库搜索路径   
> gcc -g -o main_static_lib main.c -lcalc # 链接库
> ```
> 
> 

### 总结

创建静态库的过程如下图

![[Pasted image 20241007225845.png]]

使用静态库有两种方法：第一种就是将静态库放在 GCC 执行的目录下，使用 GCC 链接时直接指定静态库的全名。第二种就是将静态库放置在链接器的任意一个搜索目录中，并通过 GCC 选项 `-l{name}` 指定静态库名

如果静态库不再链接器的搜索目录中，可以使用 GCC 的 `-L{libdir}` 指定静态库所在的目录，然后使用 `-l{name}` 链接静态库

> [!tip] LIBRARY_PATH 环境变量
> 
> 环境变量 `LIBRARY_PATH` 指定静态库的搜索目录
> 

## 共享库

共享库解决了静态库存在的缺点

> [!warning] 静态库的缺陷
> 
> 由于静态库每一次被链接都会创建一个副本，这样导致代码的冗余，从而 **浪费空间**
> + 空间浪费的 **不仅仅是 磁盘空间**。当使用同一个静态库的多个程序运行时，虚拟内存中会保存这一份目标的多个副本，从而 **提高虚拟内存的整体使用量**，同时 **造成RAM的浪费**
> 
> 此外，由于目标被复制到可执行程序中；当 **库更新后** 可执行程序并不能获得更新，**需要重新链接**
> 

也就说，共享库具有下列优势

> [!tip] 共享库的优势
> 
> **整个程序的大小变得更小了**。某些时候，程序可以完全载入内存，从而能够更快地启动程序
>
> 
> **目标模块没有被复制进可执行文件中**，而是在共享库中集中维护的。也就说，修改目标模块时无需重新链接程序就能够看到变更
> 
> 物理内存中只会加载一份可执行文件，然而，不同程序(进程)映射的虚拟地址可能不同
> 

当然，共享库也带来额外的开销。主要开销如下所述

> [!tip] 共享库带来的额外开销
> 
> 在概念上以及创建共享库和构建使用共享库的程序的实践上，**共享库比静态库更复杂**
> 
> 共享库在编译时必须要使用 **位置独立的代码** `pic`，这在大多数架构上都会带来性能开销，因为它 **需要使用额外的一个寄存器**
> 
> **在运行时必须要执行符号重定位**。在符号重定位期间，需要将对共享库中每个符号（变量或函数）的引用修改成符号在虚拟内存中的实际运行时位置。由于存在这个重定位的过程，与静态链接程序相比，一个使用共享库的程序或多或少需要花费一些时间来执行这个过程
> 

### 创建共享库

创建共享库的第一步和静态库一样，都需要编译生产目标文件

> [!important] 
> 
> 创建共享库需要生成 **位置无关** 的目标文件。使用 GCC 选项 `-fPIC` 或者 `-fpic`
> 

> [!tip] 位置无关
> 
> 所谓的 **位置无关**，会 **改变编译器生成执行 _特定操作_ 的代码的方式**。也就是， _访问静态和外部变量_，_访问字符串字面值_，以及 _获取函数的地址_，这些操作都将 **使用相对寻址进行编码**
> 
> 使用相对地址的好处就是，**共享库可以在运行时被映射到任意的虚拟地址出**
> 

下图展示创建共享库的过程

![[Pasted image 20241007230950.png]]

创建目标共享库，使用如下两条命令

```shell
gcc -g -c -fPIC -Wall calc.c show.c   # 创建目标模块
gcc -g -shared -o libcalc.so calc.o show.o   # 创建 libcalc.so 共享库
```

这两条命令可以合成一条命令

```shell
gcc -g -fPIC -Wall calc.c show.c -shared -o libcalc.so
```

### 使用共享库

使用一个共享库就需要做两件事情：**找出在运行时所需的共享库** 和 **解析嵌入的库名**

> [!tip] 找出在运行时所需的共享库
> 
> 由于共享库没有包含着可执行文件中，链接阶段通过 **嵌入共享库名称到可执行文件中** 这一个简单的机制，使得程序运行是可以找到共享库
> 
> + 在 `ELF` 文件中，库依赖性是记录在可执行文件的 `DT_NEEDED` 标签中的
> 
> + 程序依赖的所有共享库列表被称为程序的动态依赖列表
> 

> [!tip] 解析嵌入的库名
> 
> 在运行时需要某种机制解析嵌入的库名，这个机制非常简单
> 
> + 找出与在可执行文件中指定的名称对应的共享库文件
> 
> + 如果库不在内存中的话就将库加载进内存
> 

通过 GCC 链接动态库和静态库类似：**指定库全名** 或者 **通过 `-l` 选项指定库名**

> [!tip] 指定共享库全名：要求库文件位于 GCC 的执行目录之下
> 
> 下面就是使用库全名链接动态库
> 
> ```shell
> gcc -g -Wall -o main_shared main.c libcalc.so
> ```
> 

> [!tip] 使用 GCC 的 `-l` 选项指定库名：库位于链接器搜索目录中
> 
> 链接器搜索路径通常为 `/usr/lib` 目录。只需要将共享库拷贝到 `/usr/lib` 中即可
> 
> ```c
> gcc -g -Wall -o main_shared main.c -lcalc 
> ```
> 和静态库一样，库名去除了 `lib` 前缀和 `.so` 后缀
> 
> 如果共享库没有位于链接器搜索的目录中，GCC 编译选项 `-L` 可以指定库所在的目录
> 
> ```shell
> gcc -g -Wall -o main_shared  main.c -Llibdir -lcalc  # libdir 为库所在的目录
> ```
> 
> 环境变量 `LIBRARY_PATH` 也可以指定库的搜索路径
> 
> ```shell
> export LIBRARY_PATH=./shared_library                            # 指定库搜索路径   
> gcc -g -o main_shared main.c -lcalc  -I ./ shared_library  #  链接库
> ```
> GCC 选项 `-I {headers-dir}` 指定 GCC 查找头文件的目录
> 

此时，如果运行 `main_share_lib`，就会提示错误：`error while loading shared libraries: libcalc.so: cannot open shared object file: No such file or directory`

```shell
$ ./main_shared
./main_shared: error while loading shared libraries: libcalc.so: cannot open shared object file: No such file or directory
```

出现上述问题的原因在与程序被加载进内存时，**动态链接器** 就开始动态链接。由于动态链接器只会按照 **预先定义的加载规则** 在文件系统上搜索库文件。由于共享库 `libcalc.so` 不在这个运行定义的规则中，因此无法动态加载，从而导致加载动态库错误

> [!tip] 动态链接
> 
> 所谓的动态链接，就是 **动态链接器** 在程序被加载到内存中时，就开始 **解析潜入的库名**
> 
> **动态链接器本身也是一个共享库**，其名称为 `/lib/ld-linux.so.2`，所有使用共享库的 `ELF` 可执行文件都会用到这个共享库
> 

> [!tip] 预先定义的加载规则
> 
> 规则指定了一组存放共享库的标准目录。通常共享库存放在 `/usr/lib` 目录中
> 

为了通知动态链接器一个共享库位于一个非标准目录中的一种方法就是使用 `LD_LIBRARY_PATH` 环境变量。如果定义了环境变量 `LD_LIBRARY_PATH`，那么动态链接器在查找标准目录之前就会先查找 `LD_LIBRARY_PATH` 指定的目录

> [!important] LD_LIBRARY_PATH 指定动态链接器搜索共享库的目录
> 
> 对于标准动态库加载目录，环境变量 `LD_LIBRARY_PATH`可以指定这些非标准目录，并且动态链接器会主动在 `LD_LIBRARY_PATH` 指定的目录中寻找
> 

```shell
~/cfiles  LD_LIBRARY_PATH=./shared_library ./main_shared
1 + 2 = 3
```

### 分析工具

分析共享库，可执行文件以及编译过的目标文件（`.o`）的一组有用工具

#### ldd 命令

`ldd` 命令会解析出每个库引用（使用的搜索方式与动态链接器一样）

```shell
$ ldd main_shared      2024年10月07日 星期一 23时58分11秒
	linux-vdso.so.1 (0x00007d6966253000)
	libcalc.so => not found   # 我们自己制作的共享库
	libc.so.6 => /usr/lib/libc.so.6 (0x00007d6966030000)
	/lib64/ld-linux-x86-64.so.2 => /usr/lib64/ld-linux-x86-64.so.2 (0x00007d6966255000)
```

#### objdump 和 readelf 命令

`objdump` 命令能够用来获取各类信息

> [!tip] 反汇编
>
> 从一个可执行 文件、编译过的目标以及共享库中反汇编出二进制机器码
>

> [!tip] 显示 ELF 文件的头部信息
> 
> 还能够用来显示这些文件中各个 `ELF` 节的头部信息
> 
> 当这样使用 `objdump` 时它就类似于 `readelf`，`readelf` 能显示类似的信息，但显示格式不同
> 

![[Pasted image 20241008001052.png]]

#### nm 命令

`nm` 命令会列出目标库或可执行程序中定义的一组符号。`nm` 命令的一大用途就是 _找出哪些库定义了一个符号_。如要找出哪个库定义了 `crypt()` 函数则可以像下面这样做

```shell
 nm -A /usr/lib/lib*.so 2>/dev/null| grep " crypt$"
```

### 别名(soname)

之前嵌入到可执行文件中的库名以及运行时动态链接器搜索的名字是共享库的 **实际名称**，也称为 **真实名称**

> [!tip] 库的实际名字是嵌入在 `ELF` 中的 `DT_NEEDED` 标签中

然而，还可以使用 **别名** 来创建共享库，这种别名称为 `soname`

> [!tip] 库的别名潜入在 `ELF` 中的 `DT_SONAME` 标签中


使用 `soname` 的第一步是在创建共享库时指定 `soname`

> [!tip]
> 
> 指定共享库的 `soname`  需要使用 GCC 选项 `-Wl,-soname,别名`，`-WL,<选项>` 是传递给链接器的选项
> 

```shell
gcc -g -c -fPIC -Wall calc.c show.c 
gcc -g -shared -Wl,-soname,libcl.so -o libcalc.so calc.o show.o 
```

使用 `readelf` 指定 `-d` 选项可以用于确定一个共享库是否开启了 `soname`

```shell
$ readelf -d libcalc.so | grep SONAME
 0x000000000000000e (SONAME)             Library soname: [libcl.so]
```

在使用 `soname` 创建了一个共享库之后就可以照常创建可执行文件了

```shell
$ gcc -g -Wall -o main_shared main.c -Lshared_library -lcalc -I ./shared_library
```

但这次链接器检查到库 `libmath.so` 包含了 `soname libm.so`，于是将这个 `soname` 嵌入到了可执行文件中。现在当运行这个程序时就会看到下面的输出

```shell
$ LD_LIBRARY_PATH=./shared_library ./main_shared  
./main_shared: error while loading shared libraries: libcl.so: cannot open shared object file: No such file or directory
```

这里的问题是动态链接器无法找到名为 `libcl.so` 共享库

> [!tip] 
> 
> 使用 `soname` 时需要创建一个符号链接将 `soname` 指向库的真实名称，并且要将这个符号链接放在动态加载器的搜索目录中
> 

```shell
$ ln -s libcalc.so libcl.so  # 创建符号链接
$ LD_LIBRARY_PATH=./shared_library ./main_shared 
1 + 2 = 3
```

下图给出了在使用一个内嵌的 `soname`，将程序与共享库链接起来，以及创建运行程序所需要的 `soname` 符号链接涉及到的编译链接事项

![[Pasted image 20241008110247.png]]

下图给出了创建的程序被加载进内存以备执行时发生的事情

![[Pasted image 20241008111554.png]]

### 版本和命名规则

我们可以创建共享库的多种版本，通过 **版本号** 进行区别。**如果一个共享库相互连续的两个版本是相互兼容的**。意味着每个模块中的函数 **对外呈现出来的调用接口是一致的**，并且函数的语义是等价的

> [!tip] 次要版本
> 
> 这种版本号不同但相互兼容的版本称为共享库的次要版本

但有时候需要创建创建一个库的新 **主版本**：即 **与上一个版本不兼容的版本**。同时，**必须要确保使用老版本的库的程序仍然能够运行**

> [!tip] 主要版本
> 
> 版本号不同且互不兼容的版本成为共享库的主要版本
> 

为了满足上述版本化的要求，共享库的 **_真实名称_ 和 _soname_ 必须要使用一种标准的命名规范**

#### 标准规范

> [!tip] 真实名称命名规范：`lib{realName}.so.{major-id}.{minor-id}`
>  
>  `{major-id}` 主要版本号：不兼容的版本通过唯一的主要版本号区分。通常 **由递增的数字构成**
>  
>  `{minor-id}` 次要版本号：区分共享库主要版本兼容的次要版本。次要版本标识符可以是 **任意字符串**
>  

下面列出了一些共享库真实名称的示例

```shell
libdemo.so.1.0.1  # major-id 是 1；minor-id 是 0.1
libdemo.so.1.0.2
libdemo.so.2.0.1
```

> [!tip] soname 命名规范：`lib{soname}.so.{major-id}`
> 
> 共享库的 soname 只需要 `major-id` 即可
> 
> 通常，将 `soname` 创建为包含真实名称的目录中的一个 **相对符号链接**
> 

对于共享库的某个特定的主要版本来讲，可能存在几个库文件，这些库文件是通过不同的次要版本标识符来区分的。通常，**每个库的主要版本的 soname 会指向在主要版本中最新的次要版本**

除了真实名称和 `soname` 之外，通常还会为每个共享库定义第三个名称：**链接器名称**，将可执行文件与共享库链接起来时会用到这个名称

> [!tip] 链接器名称命名规范：`lib{name}.so`
> 
> 共享库的链接器名称没有主要版本号和次要版本号
> 

有了链接器名称之后就可以构建能够自动使用共享库的正确版本的独立于版本的链接命令了。一般来讲，**链接器名称与它所引用的文件位于同一个目录中**

> [!important] 
> 
> 链接器名称最好指向 `soname` 的链接，因此对 `soname` 所做的变更会自动反应到链接器名称上
> 

| 名称       | 格式                                   |
| :------- | :----------------------------------- |
| 真实名称     | `lib{name}.so.{major-id}.{minor-id}` |
| `soname` | `lib{name}.so.{major-id}`            |
| 链接器名称    | `lib{name}.so`                       |

下图展示共享库三种名称之间的关系

![[Pasted image 20241008115758.png]]

#### 使用标准规范

使用标准规范创建目标库

```shell
# 第一步：生成位置无关的 .o 目标文件
gcc -g -c -fPIC -Wall calc.c  show.c  
# 第二步：使用标准规范创建共享库，并指定 soname
gcc -g -shared -Wl,-soname,libcl.so.1 -o libcalc.so.1.0.0 calc.o show.o
                           ----------    ----------------
                           soname        real name
# 第三步：创建 soname 链接
ln -s libcalc.so.1.0.0 libcl.so.1
# 第四步：创建 链接器名字 链接
ln -s libcl.so.1 libcl.so
```

经过上述个步骤，可以创建出了符合标准的共享库。使用上述共享库

```shell
# 指定链接 calc 共享库
gcc main.c -o main_static -lcalc -Lstatic_library -I ./static_library 
# 指定共享库加载目录并运行
LD_LIBRARY_PATH=./shared_library ./main_shared 
```

如果指定共享库加载目录，`ld` 会给出下面错误消息

```
./main_shared: error while loading shared libraries: libcl.so: cannot open shared object file: No such file or directory        
                                                     --------
                                                     soname
```

### 安装共享库

目前为止介绍的例子中都是将共享库创建在用户私有的目录中，然后使用  `LD_LIBRARY_PATH` 环境变量来确保动态链接器会搜到该目录

> [!warning] 
> 
> 特权用户和非特权用户都可以使用这种技术，但在生产应用程序中不应该采用这种技术
> 

一般来讲，**共享库及其关联的符号链接会被安装在其中一个标准库目录中**，标准库目录包括

> [!tip] 动态链接器加载库文件的标准目录
> 
> `/usr/lib`：**大多数标准库安装的目录**
> 
> `/lib`：应该将 **系统启动时用到的库** 安装在这个目录中。在系统启动时可能还没有挂载 `/usr/lib`
> 
> `/usr/local/lib`：应该将 **非标准或实验性的库安装** 在这个目录中（对于 `/usr/lib` 是一个由多个系统共享的网络挂载但需要只在本机安装一个库的情况则可以将库放在这个目录中）
>  
> 在 `/etc/ld.so.conf` 中列出的目录
> 
> 在大多数情况下，将文件复制到这些目录中需要具备超级用户的权限
> 

安装完之后就必须要创建 `soname` 和链接器名称的符号链接了，通常它们是作为相对符号链接与库文件位于同一个目录中

例如，我们将目标库安装在 `/usr/local/lib` 中

```shell
# 拷贝 libcalc.so.1.0.0 到 /usr/local/lib 中
sudo cp libcalc.so.1.0.0 /usr/local/lib

# 创建 soname 符号链接
sudo ln -s libcalc.so.1.0.0 libcl.so.1 

# 创建 链接器名称 符号链接
sudo ln -s libcl.so.1 libcl.so
```

这样，我们就链接器和动态链接器就可以自动搜索到共享库了

```shell
# 编译：使用 soname 链接 libcalc.so.1.0.0
gcc main.c -o main_share -I ./shared_library -lcl
# 执行：动态链接器可以自动搜索
./main_share
```

#### ldconfig

共享库存在两个问题

> [!question] 
> 
> + **搜索共享库会非常缓慢**：**共享库可以位于各种目录中**，如果动态链接器需要通过搜索所有这些目录来找出一个库并加载这个库，那么整个过程将非常慢
>   
> + **soname 符号链接可能不是最新的**： 当安装了新版本的库或者删除了旧版本的库，那么 `soname` 符号链接就不是最新的
>

`ldconfig` 程序通过执行两个任务来解决这些问题

> [!tip] 任务一
> 搜索标准目录，然后创建或更新 /etc/ld.so.cache 缓存文件。这个缓存文件包含了搜索到的库的主要版本列表
> 
> + `ldconfig` 会搜索在 `/etc/ld.so.conf` 中指定的目录，然后搜索 `/lib` 和 `/usr/lib`
>  
> +  `/etc/ld.so.conf` 文件由一个**目录路径名**（应该是 **绝对路径名**）列表构成，其中路径名之间用换行、空格、制表符、逗号或冒号分隔
>  
> + 命令 `ldconfig –p` 会显示 `/etc/ld.so.cache` 的当前内容
> 

> [!tip] 任务二
> 
> 它检查每个库的 **各个主要版本的最新次要版本**（即具有最大的次要版本号的版本）以找出嵌入的 `soname`，然后在同一目录中 **为每个 `soname` 创建（或更新）相对符号链接**
> 

为了能够正确执行这些动作，`ldconfig` 要求 **库的名称根据标准规范来命名**（即库的真实名称包含主要和次要标识符，它们随着库的版本的更新而恰当的增长）

在默认情况下，`ldconfig` 会执行上面两个任务，但可以使用命令行选项来指定它执行其中 一个动作

+ `-N` 防止缓存的重建
+ `-X` 选项会阻止 `soname` 符号链接的创建
+ `-v` 选项会使得 `ldconfig` 输出描述其所执行的动作的信息

每当安装了一个新的库，更新或删除了一个既有库，以及 `/etc/ld.so.conf`  中的目录列表被修改之后，都应该运行 `ldconfig`

如果创建和使用的是一个私有库（即没有安装在上述标准目录中的库），那么可以通过 使用`-n` 选项让 `ldconfig` 创建 `soname` 符号链接

```shell
# 创建 soname 符号链接
ldconfig -nv . 
```

### 升级共享库

共享库的优点之一是当一个运行着的程序正在使用共享库的一个既有版本时也能够安装库的新主要版本或次要版本。在安装的过程中需要做的事情包括创建新的库版本、将其安装在恰当的目录中以及根据需要更新 `soname` 和链接器名称符号链接（或通常让 `ldconfig` 来完成 这部分工作）

#### 升级次要版本

如要创建共享库 `/usr/lib/libcalc.1.0.1` 的一个新次要版本。下面的是新的次要版本

```c title:calc.c
#include "calc.h"

int add(int a, int b){
    printf("version 1.0.2");
    return a + b;
}


int sub(int a, int b){
    printf("version 1.0.2");
    return a - b;
}
```

按照下面的步骤升级共享库

```shell
# 第一步：创建新的目标文件
gcc -g -c -fPIC -Wall calc.c  show.c
# 第二步：创建新的库文件
gcc -g -shared -Wl,-soname,libcl.so.1 -o libcalc.so.1.0.2 calc.o show.o
                            ---------    ----------------
                            soname         real name 
# 第三步：复制到 /usr/lib 中
sudo cp libcalc.so.1.0.2 /usr/lib 
# 第四步：更新 soname 链接
sudo ldconfig -v | grep libcalc
（来自 <builtin>:0 和 <builtin>:0）
        libcl.so.1 -> libcalc.so.1.0.2（已改变）
```

假设已经正确地配置了链接器名称（即指向库的 `soname`），那么就无需修改链接器名称了

已经运行着的程序会继续使用共享库的上一个次要版本，只有当它们终止或重启之后才 会使用共享库的新次要版本。重新运行 `main_share` 即可发现，它已经可以使用新的次要版本了

```shell
$ ./main_share
version 1.0.21 + 2 = 3
```

#### 升级主要版本

如要创建共享库 `/usr/lib/libcalc.1.0.2` 的一个新主要版本。下面的是新的主要版本

```c
#include "calc.h"

int add(int a, int b){
    printf("version 2.0.1\n");
    return a + b;
}


int sub(int a, int b){
    printf("version 2.0.1\n");
    return a - b;
}
```

按照下面的步骤升级共享库

```shell
# 第一步：创建新的目标文件
gcc -g -c -fPIC -Wall calc.c  show.c
# 第二步：创建新的共享库
gcc -g -shared -Wl,-soname,libcl.so.2 -o libcalc.so.2.0.1 calc.o show.o
# 第三步：复制到 /usr/lib 目录中
sudo cp libcalc.so.2.0.1 /usr/lib/
# 第四步：更新 soname 符号链接
sudo ldconfig -v | grep libcalc
（来自 <builtin>:0 和 <builtin>:0）
        libcl.so.2 -> libcalc.so.2.0.1（已改变）
        libcl.so.1 -> libcalc.so.1.0.2
# 第五步：更新 链接器名字 符号链接
sudo ln -sf libcl.so.2 libcl.so
```

> [!tip] 
> `ldconfig` 自动为新主要版本创建了一个 `soname` 符号链接，但从 最后一条命令可以看出，必须要手工更新链接器名称的符号链接
> 

### 链接器选项 `-rpath`

通知动态链接器共享库的位置的方法：**LD_LIBRARY_PATH 环境变量** 和 **标准搜索目录或 /etc/ld.so.config 指定的目录** 。这两种方式已经在之前学习过了

此外，还有另一种通知动态链接器共享库位置的方式：使用链接器的 `-rpath` 选项。这样在 **编译阶段** 就可以在可执行文件中 **插入一个在运行时搜索共享库的目录列表**

> [!tip] 
> 
>  **库位于一个固定位置** 但 **不属于动态链接器搜索的标准位置** 时是非常有用的
> 

使用 GCC 的 `-Wl` 选项将其后跟随的选项传递给链接器：`-Wl,-rpath,{libdir}`

```shell
gcc -Wl,-rpath,./shared_library -lcl -Lshared_library  -I ./shared_library main.c -o main_share
    ---------------------------
    指定动态链接器加载的目录
```

#### LD_RUN_PATH 环境变量

**`-rpath` 选项的一个替代方案是 `LD_RUN_PATH` 环境变量**。可以将一个由分号分隔开来的 目录的字符串赋给该变量，当构建可执行文件时可以将这个变量作为 `rpath` 列表来使用

> [!tip] 
> 
> **只有当构建可执行文件时不指定 `-rpath` 选项时才会使用 `LD_RUN_PATH` 变量**

#### `-rpath` 中的 `$ORIGIN`

假设需要发布一个应用程序，这个应用程序使用了自身的共享库，但同时不希望强制要求用户将这些库安装在其中一个标准目录中，相反，需要允许用户将应用程序解压到任意异目录中，然后能够立即运行这个应用程序

> [!question] 
> 
> 这里存在的问题是 **应用程序无法确定存放共享库的位置**，除非要求用户设置 `LD_LIBRARY_PATH` 或者要求用户运行某种能够标识出所需的目录的安装脚本，但这两种方法都不是令人满意的方法
> 

为解决这个问题，在构建时候增加了 `rpath` 规范中特殊字符串`$ORIGIN`（或等价的`${ORIGIN}`）的支持。动态链接器将这个字符串解释成 **“包含应用程序的目录”**。这意味着可以使用下面的命令来构建应用程序

```shell
gcc -Wl,-rpath,"$ORIGIN"/lib ...
```

#### `DT_RPATH` 和 `DT_RUNPATH` 条目

在第一版 `ELF` 规范中，只有一种 `rpath` 列表能够被嵌入到可执行文件或共享库中，它对应 于 `ELF` 文件中的 `DT_RPATH` 标签

后续的 `ELF` 规范舍弃了 `DT_RPATH`，同时引入了一种新标签 `DT_RUNPATH` 来表示 `rpath` 列表

这两种 `rpath` 列表之间的差别在于当动态链接器在运行时搜索共享库时它们相对于 `LD_LIBRARY_PATH` 环境变量的优先级：`DT_RPATH` 的优先级更高，而 `DT_RUNPATH` 的优先级则更低

> [!tip] 
> 
> **在默认情况下，链接器会将 `rpath` 列表创建为 `DT_RPATH` 标签**
> 
> 为了让链接器将 `rpath` 列表创建为 `DT_RUNPATH` 条目必须要额外使用 `––enable–new–dtags`(启用新动态标签)链接器选项
> 

### 运行时加载库

动态链接器加载库的规则如下

> [!tip] 首先，在可执行文件中的 `DT_RPATH` 指定的目录列表中搜索库
> 
> 注意：可执行文件必须不包含 `DT_RUNPATH` 列表
> 

> [!tip] 其次，在环境变量 `LD_LIBRARY_PATH` 指定的目录列表中查找
> 
> 注意，如果可执行文件是 `set-user-ID` 或者 `set-group-ID` 程序就不会加载 `LD_LIBRARY_PATH` 列表
> 

> [!tip] 然后，在可执行文件中的 `DT_RUNPATH` 指定的目录列表中搜索库
> 

> [!tip] 再然后，检查 `/etc/ld.so.cache` 文件以确认它是否包含了与库相关的条目
> 

> [!tip] 最后，搜索 `/lib` 和 `/usr/lib` 目录

### 运行时符号解析

假设在多个地方定义了一个全局符号（即函数或变量），如在一个可执行文件和一个共享库中或在多个共享库中。那么如何解析指向这个符号的引用呢？

假设现在有一个主程序和一个共享库，它们两个都定义了一个全局函数 `xyz()`，并且共享库中的另一个函数调用了 `xyz()`

```c title:prog.c

void xyz()
{
	printf("main-xyz\n");
}

int main(void)
{
	func();  // 链接到 libfoo.so 中的 func 函数
}
```

```c title:libfoo.so
void xyz()
{
	printf("foo-xyz\n");
}

void func()
{
	xyz();
}
```

在构建共享库和可执行程序并运行这个程序之后能够看到下面的输出

```shell
➜  symbol git:(master) ✗ gcc -g -shared -o libfoo.so foo.o 
➜  symbol git:(master) ✗ gcc -g -o prog prog.c libfoo.so        
➜  symbol git:(master) ✗ LD_LIBRARY_PATH=. ./prog             
max-xyz!
```

> [!tip] 
> 
> 主程序中的 `xyz()` 定义覆盖（优先）了共享库中的 定义
> 

> [!important] 重要语义规则
> 
> + 主程序中全局符号的定义覆盖库中相应的定义
>   
> + 如果一个全局符号在多个库中进行了定义，那么对该符号的引用会被绑定到在扫描库时找到的第一个定义，其中扫描顺序是按照这些库在静态链接命令行中列出时从左至右的顺序
> 

如果想要确保在共享库中对 `xyz()` 的调用确实调用了库中定义的相应函数，那么在构建共享库的时候就需要使用 `–Bsymbolic` 链接器选项

```c
➜  symbol git:(master) ✗ gcc -g -c -fPIC -Wall -c foo.c 
➜  symbol git:(master) ✗ gcc -g -shared -Wl,-Bsymbolic -o libfoo.so foo.o 
➜  symbol git:(master) ✗ gcc -g -o prog prog.c libfoo.so                 
➜  symbol git:(master) ✗ LD_LIBRARY_PATH=. ./prog
foo-xyz!
```
