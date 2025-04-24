# 共享库高级特性

## 延迟加载

当一个可执行文件开始运行之后，动态链接器会加载程序的动态依赖列表中的所有共享库，但有些时候 **延迟加载库** 是比较有用的，如只在需要的时候再加载一个插件

> [!tip] 延迟加载 API：这组 API 的原型在 `<dlfcn.h>` 中
> 
> **动态链接器** 的延迟加载功能是通过一组 `API` 来实现的。这组 `API` 通常被称为 `dlopen API`。关于动态加载库的函数原型在 `<dlfcn.h>` 头文件中声明
> 

延迟加载 API 的核心函数有下列几个

```c
#include <dlfcn.h>

void * dlopen(const char *libfilename, int flags);

const char *dlerror(void);

void * dlsym(void *handle, char * symbol);

int dlclose(void *handle);

int dladdr(const void *addr, Dl_info *info);
```

#### 打开共享库

`dlopen()` 函数将名为 `lib{filename}` 的共享库 **加载进调用进程的虚拟地址空间** 并 **增加该库打开的引用计数**。该函数的原型如下

```c
#include <dlfcn.h>

void * dlopen(const char *libfilename, int flags);
```

> [!tip] 参数 `libfilename` 是动态库路径
> 
> + 如果 `libfilename` 包含 `/` ，则 `dlopen` 按照 `libfilename` 指定的目录加载库
> + 否则，按照 [[Linux 系统编程：库基础#运行时加载库]] 中指定的规则搜索共享库
> 

> [!tip] 参数 `flags` 是加载库的方式
> 
> 参数 `flags` 是一个位掩码，通常只取 `RTLD_LAZY` 和 `RTLD_NOW` 
> 
> |flags|描述|
> |:-|:-|
> |`RTLD_LAZY`|懒加载|
> |`RTLD_NOW`|立即加载|
> 
> + 懒加载只适用于函数符号，对于非函数符号会立即被加载
> 
> 除了上述两个参数外，还可能有其他取值
> 
> |flags|描述|
> |:-|:-|
> |`RTLD_GLOBAL`|当前库及其依赖树中的符号均可以通过 `dlsym` 查找|
> |`RTLD_LOCA`|与 `RTLD_CLOBAL` 相反：依赖树中的符号不能被查找|
> |`RTLD_NOLOA`|不加载库：用于检查库是否被加载|
> |`RTLD_DEEPBIND`|解析符号时优先使用当前库的符号|
> 
> `RTLD_NOLOA` 用于检查库是否被加载。检查规则
> + `dlopen()` 返回库的地址，则库被加载
> + `dlopen()` 返回 `NULL`，则库未被加载
> 

> [!tip] 返回值
> 
> 如果库加载成功，返回库加载到内存中的地址；否则，返回 `NULL`
> 

### 错误诊断

如果在 `dlopen API` 的函数调用中得到了一个错误，那么可以使用 `dlerror()` 来获取一个指向表明错误原因的字符串的指针

```c
#include <dlfcn.h>

const char *dlerror(void);
```

### 解析符号地址

`dlsym()` 函数在 `handle` 指向的库以及该库的依赖树中的库中搜索名为 `symbol` 的符号（函数 或变量）。该函数的原型如下

```c
#include <dlfcn.h>

void * dlsym(void *handle, char * symbol);
```

> [!tip] 参数 `handle`：`dlopen` 返回的地址 或者 **伪句柄**
> 
> 下表列出了伪句柄的取值
> 
> |伪句柄|描述|
> |:-|:-|
> |`RTLD_DEFAULT`|按照 _主程序->所有以加载库_ 的顺序|
> |`RTLD_NEXT`|调用其他地方定义的同名函数|
> 

> [!tip] 参数 `symbol`：需要搜索符号字符串

> [!tip] 返回值
> 
> 如果找到 `symbol` 指定的符号，就返回指向该符号的地址；否则，返回 `NULL`
> 
> 返回 `NULL` 不一定是找不到符号，要知道是什么原因请调用 `dlerror()` 查看错误信息
> 

`dlvsym(handle, symbol, version)`与 `dlsym()` 类似，但它是搜索的是 **版本化的符号**
> [!tip] 
> 
> 如果要获得 `dlvsym` 函数的原型，必须要定义 `_GNU_SOURCE` 特性测试宏
> 

### 关闭共享库

`dlclose()` 函数关闭一个库

```c
#include <dlfcn.h>

int dlclose(void *handle);
```

> [!tip] 参数 `handle`：需要关闭库的指针
> 

> [!warning] 
> 
> `dlclose()` 只会减小 `handle` 指向的动态库的引用计数。只有当 `handle` 指向的动态库的引用计数为 `0` 时才会卸载该库
> 

> [!tip] 返回值
> 
> `handle` 成功关闭返回 `0`，否则返回非`0`
> 

### 获取符号的相关信息

`dladdr()` 返回一个包含地址 `addr`（通常通过前面的 `dlsym()` 调用获得）的相关信息的结构

```c
#define __USE_GNU
#include <dlfcn.h>

int dladdr(const void *addr, Dl_info *info);
```

`info` 参数是一个指向由调用者分配的结构的指针，其结构形式如下

```c
typedef struct
{
	const char *dli_fname;  // 共享库的 路径名
	void *dli_fbase;        // 运行时的 基地址
	const char* dli_sname; 
	void *dli_saddr;
} Dl_info;
```

### 例程：延迟加载 libcalc 库

例程：调用在 [[Linux 系统编程：库基础]] 中制作的 `libcalc.so.2.0.1`

```c
#include <stdio.h>
#include <string.h>

// 延迟加载 API
#include <dlfcn.h>

#include "calc.h"


int main(int argc, char *argv[]) 
{
    if (argc != 3) 
    {
        fprintf(stderr, "Usage: %s <libname> <symbol>\n", argv[0]);
        return -1;
    }

    char *libname = argv[1];
    char *symbol = argv[2];

    // 延迟加载 libcalc.so 库
    void *libcalc = dlopen(libname, RTLD_LAZY);  
    
    if(!libcalc)
    {
        // 打开库文件出错
        fprintf(stderr, "dlopen: %s\n", dlerror());
        return -1;
    }

    // 解析符号
    void *name = dlsym(libcalc, symbol);

    if(!name)
    {
        // 解析符号错误
        fprintf(stderr, "dlsym: %s\n", dlerror());
        return -1;
    }

    // 使用符号

    if (strcmp(symbol, "add") == 0) 
    {
        int (*add)(int, int) = (int(*)(int, int)) name;
        printf("add(3, 4) = %d\n", add(3, 4));
        return 0;
    } 
    else if (strcmp(symbol, "sub") == 0)
    {
        int(*sub)(int, int) = (int(*)(int, int)) name;
        printf("sub(4, 3) = %d\n", sub(4, 3));
        return 0;
    } 
    else if(strcmp(symbol, "show") == 0)
    {
        void(*show)(int, char, int) = (void(*)(int, char, int)) name;
        show(3, '+', 4);
    }
    else
    {
        fprintf(stderr, "Unknown symbol: %s\n", symbol);
        return -1;
    }

    // 关闭动态库
    dlclose(libcalc);
    return 0;
}
```

为了生成这个程序，编写了下面的 `Makefile`

```shell title:Makefile
CC := /usr/bin/gcc 
CFLAGS := -Wall

EXECUTABLE_NAME := dlopen

SRCS := $(wildcard *.c)
OBJECTS := $(patsubst %.c, %.o, $(SRCS))

$(EXECUTABLE_NAME): $(OBJECTS)
	$(CC) ${CFLAGS} $^ -o $@

%.o:%.c %.h
	$(CC) -c $(CFLAGS) $< -o $@

.PHONY: clean rebuild run

clean:
	$(RM) $(EXECUTABLE_NAME) $(OBJECTS)

rebuild: clean $(EXECUTABLE_NAME)

run: $(EXECUTABLE_NAME)
	./$(EXECUTABLE_NAME) $(LIBFILENAME) $(SYMBOL)
```

我们使用 `make LIBFILENAME="./libcalc.so.2.0.1" SYMBOL=add run` 运行该程序

```shell
dlopen → make LIBFILENAME="./libcalc.so.2.0.1" SYMBOL=add run
./dlopen ./libcalc.so.2.0.1 add
version 2.0.1
add(3, 4) = 7
```

### 在主程序中访问符号

假设使用 `dlopen()` 动态加载了一个共享库，然后使用 `dlsym()` 获取了共享库中 `x()` 函数的地址，接着调用 `x()`。如果在 `x()` 中调用了函数 `y()`，那么通常会在程序加载的其中一个共享库中搜索 `y()`。

**有些时候需要让 `x()` 调用 _主程序_ 中的 `y()` 实现（类似于回调机制）**。为了达到这个目的就必须要使主程序中的符号（全局作用域）对动态链接器可用，即在链接程序时使用 `--export-dynamic` 链接器选项

```shell
gcc -Wl,--export-dynamic main.c
```

## 符号的可见性

设计共享库时，被公开的符号应该只是构成其声明的接口。否则可能会出现一些问题

> [!warning] 
> 
> 如果导出了未详细说明的接口，那么使用这个库的应用程序的作者可能会选择这些接口。**可能导致将来升级时的兼容性问题**
> + 库开发人员认为可以修改或删除不属于文档记录的 ABI 中的接口，而库用户则希望继续使用名称与他们当前正在使用的接口名称一样的接口
> 
> 在运行时符号解析阶段， **由共享库导出的所有符号可能会优先于其他共享库提供的相关定义**
> 
> 导出非必需的符号会 **增加在运行时需加载的动态符号表的大小**
> 

控制符号可见性的方法有两种：C 关键字 `static` 和 GNU GCC 的 `visibility` 属性

> [!tip] `static` 关键词：使得一个符号私有于一个源代码模块
> 
> 被 `static` 修饰的符号 **无外部链接属性**，从而无法被其他目标文件绑定
> + 同一源文件中对该符号的所有引用会被绑定到该符号的定义上
> 

> [!tip] `visibility` 属性
> 
> `GNU C` 编译器 `gcc` 提供了一个特有的属性声明 `__attribute__ ((visibility("hidden")))` ，它执行与 `static` 关键词类似的任务
> 
> `hidden` 特性 **使得符号对库之外的文件不可见**，库内所有源文件均可见
> 
> ```c
> void __attribute__ ((visibility("hidden"))) func(void) {
> }
> ```
> 

一种更通用的方法是使用 **链接器版本脚本** 文件。可以用来 **精确控制符号的可见性** 以及 **选择将一个引用绑定到符号的哪个版本**

## 链接器版本脚本

版本脚本是一个包含链接器 `ld` 执行的指令的文本文件。要使用版本脚本必须要指定 `--version-script` 链接器选项

```shell
gcc -Wl,--verision-script,myscriptfile.map ...
```

> [!tip] 版本脚本的后缀通常（但不统一）是 `.map`

### 控制符号可见性


版本脚本的一个用途是 **控制那些可能会在无意中变成全局可见（即对与该库进行链接的应用程序可见）的符号的可见性**

举一个简单的例子，假设需要从三个源文件 `vis_comm.c`  `vis_f1.c` 以及 `vis_f2.c` 中构建一个共享库，这三个源文件分别定义了函数 `vis_comm()`、`vis_f1() `以及` vis_f2()`。`vis_comm()`函数由 `vis_f1() `和 `vis_f2()`调用，但不想被与该库进行链接的应用 程序直接使用

> [!tip]
> 
> `vis_comm()` 不想被库外的代买可见
> 

```c title:vis_comm.c
#include "vis_comm.h"

#include <stdio.h>
void vis_comm()
{
    printf("vis_comm\n");
}
```

```c title:vis_f1.c
#include "vis_f1.h"
#include "vis_comm.h"

#include <stdio.h>

void vis_f1()
{
    printf("vis_f1\n");
    vis_comm();
}
```

```c title:vis_f2.c
#include "vis_f2.h"
#include "vis_comm.h"

#include <stdio.h>

void vis_f2()
{
    printf("vis_f2\n");
    vis_comm();
}
```

现在假设使用常规方法构建共享库

```shell
gcc -g -c -fPIC -Wall vis_comm.c vis_f1.c vis_f2.c
gcc -g -shared -o vis.so vis_comm.o vis_f1.o vis_f2.o
```

如果使用下面的 `readelf` 命令来列出该库导出动态符号，那么就会看到下面的输出

```shell
➜ readelf --syms --use-dynamic vis.so | grep vis_
     6: 000000000000114f    32 FUNC    GLOBAL DEFAULT   12 vis_f2
     7: 0000000000001119    22 FUNC    GLOBAL DEFAULT   12 vis_comm
     8: 000000000000112f    32 FUNC    GLOBAL DEFAULT   12 vis_f1
```

这个共享库导出了三个符号：`vis_comm()` `vis_f1()`以及 `vis_f2()`，但这里需要确保这个库只导出 `vis_f1()` 和 `vis_f2()` 符号

> [!tip] 
> 
> 显然，该共享库导出了一个我们不希望被导出的符号 `vis_comm`
> 


可以 **通过版本脚本** 实现对 `vis_comm` 符号的隐藏。如下所示的版本脚本则可以实现

```map
VER_1 {
	global:
		vis_f1;
		vis_f2;
	local:
		*;
};
```

标识符 `VER_1` 是一种版本标签。一个版本脚本可以包含多个版本节点
- **`global` 标记** 出来以分号(`;`) 分隔的对 **库外可见的符号列表**
- **`local` 标记** 出以分号(`;`) 分隔的对 **库外不可见的符号列表**

每个版本节点以花括号(`{}`)组织起来并且在括号前面设置一个唯一的版本标签
- 如果使用版本脚本只是为了控制符号的可见性，那么版本标签是多余的，但老版本的 `ld` 仍然需要用到这个标签
- `ld` 的现代版本允许省略版本标签，如果 **省略了版本标签** 的话就认为版本节点拥有一个 **匿名版本标签** 并且在这个脚本中不 **能存在其他版本节点**

接着可以像下面这样使用版本脚本来构建共享库

```shell
gcc -g -c -fPIC -Wall vis_comm.c vis_f1.c vis_f2.c   
gcc -g -shared -o vis.so vis_comm.o vis_f1.o vis_f2.o -Wl,--version-script,vis.map
```

此时，在使用 `readelf` 就不能查看到 `vis_comm()`了

```shell
➜ readelf --syms --use-dynamic vis.so | grep vis_
     6: 000000000000113f    32 FUNC    GLOBAL DEFAULT   13 vis_f2@@VER_1
     8: 000000000000111f    32 FUNC    GLOBAL DEFAULT   13 vis_f1@@VER_1
```

### 符号版本化

**符号版本化 _允许一个共享库提供同一个函数的多个版本_**。每个程序会使用它与共享库进行链接时函数的当前版本

> [!tip] 符号版本化的作用
> 
> + 这种处理方式的结果是可以对共享库 **进行不兼容的改动而无需提升库的主要版本号**
> + 符号版本化可以 **取代传统的共享库 _主要_ 和 _次要_ 版本化模型**
> 
> `glibc` 从 `2.1` 开始使用了这种符号版本化技术，因此 `glibc 2.0` 以及之前的所有版本都是通过单个主要库版本（`libc.so.6`）来支持的
> 

下面通过一个简单的例子来展示符号版本化的用途。首先使用一个版本脚本来创建共享库的第一个版本

```c title:sv.c
#include <stdio.h>

void xyz(void)
{
    printf("v1 xyz!\n");
}
```

然后，创建一个 `sv_v1.map` 文件

``` title:sv.map
VER_1 {
	global:
        xyz;
	local:
		*;  # 隐藏其他所有符号
};
```

在这个阶段，版本脚本 `sv_v1.map` 只用来控制共享库的符号的可见性，即只导出 `xyz()`， 同时隐藏其他所有符号。接着创建一个程序 `p1` 来使用这个库

```c title:sv_prog.c 
#include <stdlib.h>

int main(int argc, char *argv[])
{
    void xyz(void);
    xyz();
    exit(EXIT_SUCCESS);
}
```

编译并运行，就能看见预期结果

```shell
➜  gcc -g -c -fPIC -Wall sv.c
➜  gcc -g -shared -o libsv.so sv.o -Wl,--version-script,sv.map
➜  gcc -g -o p1 sv_prog.c libsv.so
➜  LD_LIBRARY_PATH=. ./p1      
v1 xyz!
```

现在修改库中 `xyz()` 的定义，但同时仍然需要确保程序 `p1` 继续使用老版本的函数。 为完成这个任务，必须要在库中定义两个版本的 `xyz()`

```c title:sv.c
#include <stdio.h>

__asm__(".symver xyz_old, xyz@VER_1");
__asm__(".symver xyz_new, xyz@@VER_2");

void xyz_old(void)
{
    printf("v1 xyz!\n");
}

void xyz_new(void)
{
    printf("v2 xyz!\n");
}

void pqr(void) {
    printf("v2 pqr\n");
}
```

这里两个版本的 `xyz()` 是通过函数 `xyz_old()` 和  `xyz_new()` 来实现的。`xyz_old()` 函数对应于原来的 `xyz()` 定义，`p1`程序应该继续使用这个函数； `xyz_new()` 函数提供了与库的新版本进行链接的程序所使用的 `xyz()` 的定义

下面两个`.symver` 汇编器指令用于将这两个函数绑定到两个不同的版本标签上

```c
__asm__(".symver xyz_old, xyz@VER_1");
__asm__(".symver xyz_new, xyz@@VER_2");
```

> [!tip] `xyz@@VER_2` 中的 `@@` 表示默认绑定
> 
> 
> 

下面是与修改过之后的库对应的版本脚本: `sv_v2.map`

``` title:sv.map
VER_1 {
	global:
        xyz;
	local:
		*;  # 隐藏其他所有符号
};

VER_2 {
    global: pqr;
} VER_1;  # VER_2 依赖 VER_1
```

这个版本脚本提供了一个新版本标签 `VER_2`，它依赖于标签 `VER_1

> [!tip]
> 
> 版本标记 **依赖表明了相邻两个库版本之间的关系**。从语义上来讲，`Linux` 上的版本标签依赖的唯一效果是 **版本节点可以从它所依赖的版本节点中继承 `global` 和 `local` 规范**
> 
> 依赖可以串联起来，这样就可以定义另一个依赖于 `VER_2` 的版本节点 `VER_3` 并以此类推地定义其他版本节点
> 

**版本标签名本身是没有任何意义的**，它们相互之间的关系是通过制定的版本依赖来确定的
+ 选择名称 `VER_1` 和 `VER_2` 仅仅为了暗示它们之间的关系
+ 为了便于维护，建议在版本标签名中包含 **包名** 和一个 **版本号**。如 `glibc` 会使用名为 `GLIBC_2.0` 和 `GLIBC_2.1` 之类的版本标签名

`VER_2` 版本标签还指定了将库中的 `pqr()` 函数导出并绑定到 `VER_2` 版本标签。如果没有通过这种方式来声明 `pqr()`，那么 `VER_2` 版本标签从 `VER_1` 版本标签继承而来的 `local` 规范将会使 `pqr()` 对外不可见

> [!warning] 
> 
> 还需注意的是如果省略了 `local` 规范，那么库中的 `xyz_old()` 和 `xyz_new()` 符号也会被导出（这通常不是期望发生的事情）
> 

现在按照以往方式构建库的新版本

```shell
gcc -g -c -fPIC -Wall sv.c                                    
gcc -g -shared -o libsv.so sv.o -Wl,--version-script,sv.map
```

现在创建一个新程序 `p2`，它使用了 `xyz()` 的新定义，同时程序 `p1` 使用了旧版的 `xyz()`

```shell
➜  gcc -g -o p2 sv_prog.c libsv.so
➜  LD_LIBRARY_PATH=. ./p2
v2 xyz!
➜  LD_LIBRARY_PATH=. ./p1
v1 xyz!
```

可执行文件的版本标签依赖是在静态链接时进行记录的。使用 `objdump –t` 可以打印出每个可执行文件的符号表，从而能够显示出两个程序中不同的版本标签依赖

```shell
➜  objdump -t p1 | grep xyz
0000000000000000       F *UND*  0000000000000000              xyz@VER_1
➜  objdump -t p2 | grep xyz
0000000000000000       F *UND*  0000000000000000              xyz@VER_2
```

### 初始化和终止函数

**可以定义一个或多个在共享库被加载和卸载时自动执行的函数**。这样在使用共享库时就能够 **完成一些初始化和终止工作** 了

> [!tip] 
> 
> 不管库是自动被加载还是使用 `dlopen` 接口显式加载的，初始化函数和终止函数都会被执行
> 

初始化和终止函数是 **使用 `gcc` 的 `constructor` 和 `destructor` 特性来定义** 的

在库被加载时需要执行的所有函数都应该定义成下面的形式

```c
void __attribute__((constructor)) some_name_loda(void)
{
	// 初始化代码
}
```

类似地，卸载函数的形式如下

```c
void __attribute__((destructor)) some_name_unload(void)
{
	// 终止代码
}
```

> [!hint] 
> 
> 根据需要使用其他名字替换函数名 `some_name_load()` 和 `some_name_unload()`
> 

**使用 `gcc` 的 `constructor` 和 `destructor` 特性还能创建主程序的初始化函数和终止函数
