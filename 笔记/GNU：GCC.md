# GCC

在 [[C 语言：程序的结构]] 中，我们知道一个 C 程序有 **源文件(`.c` 文件)** 和 **头文件(`.h` 文件)** 组成。在学习了基本的汇编指令之后，我们来看看 GCC 是如何生成程序的

![[Pasted image 20241007102703.png]]

> [!tip]
> 
> 前三个阶段：预处理，编译和汇编统一称为 **编译阶段**
> 

预处理阶段我们在 C 基础阶段已经完成学习了。现在，我们从编译阶段开始研究程序的生成过程

## 编译阶段

编译阶段，我们分析下面的 C 代码编译后的 汇编代码，并绘制执行过程中的栈帧变化

```c title:main.c
#include <stdio.h>

void func(int i)
{
    printf("Iam func, i = %d\n", i);
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

使用 `gcc -S main.c` 得到的汇编代码如下

```c title:main.s
	.file	"main.c"
	.text
	.section	.rodata
.LC0:
	.string	"Iam func, i = %d\n"
	.text
	.globl	func
	.type	func, @function
func:
.LFB0:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %eax
	movl	%eax, %esi
	leaq	.LC0(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	func, .-func
	.section	.rodata
.LC1:
	.string	"Hello, World!"
	.text
	.globl	main
	.type	main, @function
main:
.LFB1:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$48, %rsp
	movq	%fs:40, %rax
	movq	%rax, -8(%rbp)
	xorl	%eax, %eax
	movl	$10, -36(%rbp)
	movl	$1, -20(%rbp)
	movl	$2, -16(%rbp)
	movl	$3, -12(%rbp)
	movl	$4, -12(%rbp)
	leaq	-20(%rbp), %rax
	movq	%rax, -32(%rbp)
	movl	-36(%rbp), %eax
	movl	%eax, %edi
	call	func
	movl	$0, -40(%rbp)
	jmp	.L3
.L4:
	leaq	.LC1(%rip), %rax
	movq	%rax, %rdi
	call	puts@PLT
	addl	$1, -40(%rbp)
.L3:
	cmpl	$4, -40(%rbp)
	jle	.L4
	movl	$0, %eax
	movq	-8(%rbp), %rdx
	subq	%fs:40, %rdx
	je	.L6
	call	__stack_chk_fail@PLT
.L6:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE1:
	.size	main, .-main
	.ident	"GCC: (GNU) 14.2.1 20240910"
	.section	.note.GNU-stack,"",@progbits
```

我们从 `main` 函数的汇编代码开始，然后分析栈帧中的内容。这部分代码完成 `main` 函数栈帧的分配任务

```c
main:
.LFB1:
	.cfi_startproc
	pushq	%rbp              // 保存上一个函数的栈基地址
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp       // 当前栈顶地址作为 main 函数的栈基地址
	.cfi_def_cfa_register 6
	subq	$48, %rsp        // 为 main 函数分配栈帧
	movq	%fs:40, %rax     // 将 %fs:40 保存在 %rax 中
	movq	%rax, -8(%rbp)   // 将 %fs:40 存储在 main 栈帧开始位置，防止栈溢出
	xorl	%eax, %eax       // 将 %rax 的低 32 位清零
```

上述汇编代码执行后，`main` 函数的栈空间如下

![[Pasted image 20241007143922.png]]

继续分析 `main` 函数的汇编代码：这段汇编代码完成数组初始化，即是 `int arr[3] = {1,2,3};`

```c
	movl	$10, -36(%rbp)  // int j = 10
	movl	$1, -20(%rbp)   // arr[0]
	movl	$2, -16(%rbp)   // arr[1]
	movl	$3, -12(%rbp)   // arr[2]
```

此时，`main` 函数的栈帧如下图所示
![[Pasted image 20241007145741.png]]

继续分析 `main` 函数的汇编代码：这段代码获取数组首元素的指针

```c
leaq    -20(%rbp), %rax  // 将数组首元素的地址放入 %rax 中
movq    %rax, -32(%rbp)  // 将 %rax 保存在内存地址为 R[%rbp] - 32 的位置
```

此时， `main` 函数的栈帧如下图所示

![[Pasted image 20241007150508.png]]

继续分析 `main` 函数的汇编代码：这段代码准备 `func` 函数的参数，并调用 `func` 函数

```c
	movl	-36(%rbp), %eax  // 将 j 的值复制到 %eax
	movl	%eax, %edi       // 将 %eax 复制到 %edi
	call	func             // 调用 func
```

此时， `main` 函数的栈帧如下图所示

![[Pasted image 20241007151007.png]]

> [!tip] 寄存器 `%edi` 中的值为 `10`

由于这里调用了 `func` 函数，我们转而分析 `func` 函数的汇编代码

```c
func:
.LFB0:
	.cfi_startproc
	pushq	%rbp                 // %rbp 入栈
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp          // 当前的栈指针(%rsp) 作为 func 的栈基指针(%rbp) 
	.cfi_def_cfa_register 6
	subq	$16, %rsp           // 为 func 函数分配 16 字节的空间
```

此时，栈帧中保存了 `main` 和 `func` 两个函数的栈帧

![[Pasted image 20241007154552.png]]


继续分析 `func` 函数的汇编代码：将 `func` 的参数入栈，然后复制到 `%eax` 中 

```c
	movl    %edi, -4(%rbp)    // 参数入栈
	movl    -4(%rbp), %eax    // 参数复制到 %eax
```

![[Pasted image 20241007160123.png]]

继续分析 `func` 函数的汇编代码：准备调用 `printf` 函数的参数，然后调用 `printf` 函数

```c
	movl    %eax, %esi          // 准备 printf 的第二个参数
	leaq    .LC0(%rip), %rax    // 加载字符串字面值的地址
	movq    %rax, %rdi          // 准备 printf d的低一个参数
	movl    $0, %eax            // %eax 清零
	call    printf@PLT          // 调用 printf 函数
```

![[Pasted image 20241007160255.png]]

`.LC0` 是字符串字面值保存的位置

```c
	.section	.rodata
.LC0:
	.string	"Iam func, i = %d\n"
	.text
	.globl	func
	.type	func, @function
```

> [!tip] 字符串字面字保存在 **read only data** 区域

函数 `func` 的最后几条指令用于返回

```c
	nop                 // 空指令
	leave               // 离开
	.cfi_def_cfa 7, 8  
	ret                 // 返回
	.cfi_endproc
```

此时，栈帧又变回到调用 `func` 之前的位置
![[Pasted image 20241007160708.png]]

现在，控制又回到了 `main` 函数中，继续分析 `main` 函数的汇编代码

```c
	movl	$0, -40(%rbp)     // 将地址为 %rbp - 40 的内存清零

```

此时，`main` 函数的栈帧为

![[Pasted image 20241007161200.png]]

继续分析 `.L3` 标号的代码：实现 `for` 循环（跳转到中间的方式实现）

```c
	jmp	.L3                   // 跳转到 .L3 位置继续执行 
.L4:
	leaq	.LC1(%rip), %rax  // 加载字符串字面值地址
	movq	%rax, %rdi        // 准备 printf 的低一个参数
	call	puts@PLT          // -40(%rbp) + 1
	addl	$1, -40(%rbp)
.L3:
	cmpl	$4, -40(%rbp)    // 比较 -40(%rbp) 和 $4
	jle	.L4                  // 当 -40(%rbp) <= 4 跳转到 .L4
```

![[Pasted image 20241007161745.png]]

循环结束后，检查 `main` 函数的栈是否溢出。栈未溢出，`main` 函数调用结束，栈帧被销毁

```c
	movl	$0, %eax         // 将 %eax 清零
	movq	-8(%rbp), %rdx   // 读取 -8(%rbp) 中的值道 %rdx
	subq	%fs:40, %rdx     // %rdx - %fs:40
	je	.L6                  // 为零时跳转到 .L6（栈未溢出）
	call	__stack_chk_fail@PLT  // 栈溢出了
.L6:
	leave                     // 离开
	.cfi_def_cfa 7, 8
	ret                       // main 函数返回
	.cfi_endproc
```

## 汇编阶段

编译阶段做的就是一一对应，将汇编指令的字符串映射为字节编码

Linux 命令 `as` 用于将汇编代码映射为字节编码
> [!tip] AS 命令
> ```
>AS - the portable GNU assembler. // 可移植的 GNU 汇编器
> ```

将 `main.s` 汇编，生成目标文件 `main.o`

```shell
$ as main.s -o main.o 
```

此时，目标文件是可重定位的文件，此时 `main.o` 中还没有起始地址，使用的库函数地址也不知道。Linux 命令 `nm` 可以获取目标文件中的符号表
> [!tip] nm 命令
> 
> ```
>  nm - list symbols from object file
> ```
> 

```shell
 ~/cfiles  nm main.o                                                  31.5s  2024年10月07日 星期一 17时15分42秒
0000000000000000 T func
0000000000000027 T main
                 U printf              // U 表示 unknow，也就是 printf 函数没有入口地址
                 U puts                // U 表示 unknow 也就是 puts 函数没有入口地址
                 U __stack_chk_fail
```

> [!tip] 
> 前 $3$ 个阶段统称为编译阶段。GCC 选项 `-c` 可以直接编译得到目标文件
> 

```shell
 gcc -c main.c -o main.o
```

 Linux 命令 `objdump` 可以执行 **返汇编**，获得目标文件的字节编码对应的指令字符串
 
```shell
objdump -d main.o > main.s   // 反汇编获得字节编码对应的指令字符串
```

```c
main.o：     文件格式 elf64-x86-64  
  
  
Disassembly of section .text:  
  
0000000000000000 <func>:  
   0:   55                     push   %rbp  
   1:   48 89 e5               mov    %rsp,%rbp  
   4:   48 83 ec 10            sub    $0x10,%rsp  
   8:   89 7d fc               mov    %edi,-0x4(%rbp)  
   b:   8b 45 fc               mov    -0x4(%rbp),%eax  
   e:   89 c6                  mov    %eax,%esi  
  10:   48 8d 05 00 00 00 00   lea    0x0(%rip),%rax        # 17 <func+0x17>  
  17:   48 89 c7               mov    %rax,%rdi  
  1a:   b8 00 00 00 00         mov    $0x0,%eax  
  1f:   e8 00 00 00 00         call   24 <func+0x24>  
  24:   90                     nop  
  25:   c9                     leave  
  26:   c3                     ret  
  
0000000000000027 <main>:  
  27:   55                     push   %rbp  
  28:   48 89 e5               mov    %rsp,%rbp  
  2b:   48 83 ec 30            sub    $0x30,%rsp  
  2f:   64 48 8b 04 25 28 00   mov    %fs:0x28,%rax  
  36:   00 00   
  38:   48 89 45 f8            mov    %rax,-0x8(%rbp)  
  3c:   31 c0                  xor    %eax,%eax  
  3e:   c7 45 dc 0a 00 00 00   movl   $0xa,-0x24(%rbp)  
  45:   c7 45 ec 01 00 00 00   movl   $0x1,-0x14(%rbp)  
  4c:   c7 45 f0 02 00 00 00   movl   $0x2,-0x10(%rbp)  
  53:   c7 45 f4 03 00 00 00   movl   $0x3,-0xc(%rbp)  
  5a:   c7 45 f4 04 00 00 00   movl   $0x4,-0xc(%rbp)  
  61:   48 8d 45 ec            lea    -0x14(%rbp),%rax  
  65:   48 89 45 e0            mov    %rax,-0x20(%rbp)  
  69:   8b 45 dc               mov    -0x24(%rbp),%eax  
  6c:   89 c7                  mov    %eax,%edi  
  6e:   e8 00 00 00 00         call   73 <main+0x4c>  
  73:   c7 45 d8 00 00 00 00   movl   $0x0,-0x28(%rbp)  
  7a:   eb 13                  jmp    8f <main+0x68>  
  7c:   48 8d 05 00 00 00 00   lea    0x0(%rip),%rax        # 83 <main+0x5c>  
  83:   48 89 c7               mov    %rax,%rdi  
  86:   e8 00 00 00 00         call   8b <main+0x64>  
  8b:   83 45 d8 01            addl   $0x1,-0x28(%rbp)  
  8f:   83 7d d8 04            cmpl   $0x4,-0x28(%rbp)  
  93:   7e e7                  jle    7c <main+0x55>  
  95:   b8 00 00 00 00         mov    $0x0,%eax  
  9a:   48 8b 55 f8            mov    -0x8(%rbp),%rdx  
  9e:   64 48 2b 14 25 28 00   sub    %fs:0x28,%rdx  
  a5:   00 00   
  a7:   74 05                  je     ae <main+0x87>  
  a9:   e8 00 00 00 00         call   ae <main+0x87>  
  ae:   c9                     leave  
  af:   c3                     ret
```

## 链接阶段

**链接阶段就是将调用的函数转换为函数的入口地址**。链接时使用的工具为 `ld` 命令。不过，我们不会手动调用 `ld` 命令，因为依赖的库文件还可能依赖其他库

> [!tip] 
> 
> 直接使用 GCC 让 GCC 帮我们调用 `ld` 命令完成连接
> 

```c
gcc main.o -o main.out  // 链接
```

###  链接期间的错误

**一些在编译期间无法发现的错误会在链接期间被发现**。尤其是如果程序中 **丢失了函数定义或变量定义**，那么 **链接器将无法解析外部引用**，从而导致出现类似 `"undefined symbol"` 或 `"undefined reference"` 的消息

链接器检查到的错误通常很容易修改。下面是一些最常见的错误起因

> [!tip]
> 
> **拼写错误**。如果变量名或函数名拼写错误，那么链接器将进行缺失报告
> 
> **缺失文件**。如果链接器不能找到文件 `foo.c` 中的函数，那么它可能不知道存在此文件
> 
> **缺失库**。链接器不可能找到程序中用到的全部库函数
> 

## 库文件

库文件就是由别人提供的函数定义，然后编译为 `.o` 文件，这些 `.o` 文件以一种 **特殊的形式** 提供给其他程序使用

> [!tip] 
> 
> 库文件：就是 **由几个 `.o` 文件**，以某种形式 **组合** 起来的特殊文件成为库文件

Linux 中的库文件提供两种形式：**静态库** 和 **共享库**。

![[Pasted image 20241007174639.png]]

> [!tip] 静态库：**也称 _归档文件_**
> 
> 静态库就是几个 `.o` 文件形成的打包文件。这样做有下面两个好处
> 
> + 可以 **将一组经常被用到的目标文件组织进单个库文件**，这样就可以使用它来构建多个可执行程序并且在 **构建各个可执行程序的时候 _无需重新编译原来的源代码文件_**
> 
> + **链接命令变得更加简单了**。在 _链接命令行中只需要指定静态库的名称即可_，而无需一个个地列出目标文件了。链接器知道如何搜素静态库并将可执行程序需要的对象抽取出来
>
> **链接静态库** 就是将静态库中 **被使用的模块复制到可执行程序文件中**。就好象我们购买的汽车，轮子就是在汽车上的
> 
> ![[Pasted image 20241007180409.png]]

> [!warning] 静态库的缺陷
> 
> 由于静态库每一次被链接都会创建一个副本，这样导致代码的冗余，从而 **浪费空间**
> + 空间浪费的 **不仅仅是 磁盘空间**。当使用同一个静态库的多个程序运行时，虚拟内存中会保存这一份目标的多个副本，从而 **提高虚拟内存的整体使用量**，同时 **造成RAM的浪费**
> 
> 此外，由于目标被复制到可执行程序中；当 **库更新后** 可执行程序并不能获得更新，**需要重新链接**
> 

为了解决静态库的缺陷，Linux 提供了 **共享库**

> [!tip] 共享库
> 
> 关键思想是目标模块的 **_单个副本_ 由所有需要这些模块的程序共享**。**目标模块 _不会被复制_ 到链接过的可执行文件中**
> 
> 也就是说，程序加载到内存中之后，才会链接对应的库文件中的目标模块。就好比赛车，在需要的时候按照合适的轮子
> 
> ![[Pasted image 20241007180847.png]]
> 

当然，共享库也带来额外的开销。主要开销如下所述

> [!tip] 
> 在概念上以及创建共享库和构建使用共享库的程序的实践上，**共享库比静态库更复杂**
>
> 共享库在编译时必须要使用 **位置独立的代码** `pic`，这在大多数架构上都会带来性能开销，因为它**需要使用额外的一个寄存器**
> 
> **在运行时必须要执行符号重定位**。在符号重定位期间，需要将对共享库中每个符号（变量或函数）的引用修改成符号在虚拟内存中的实际运行时位置。由于存在这个重定位的过程，与静态链接程序相比，一个使用共享库的程序或多或少需要花费一些时间来执行这个过程
> 

### 制作库

制作库我们在 [[Linux 系统编程：库基础]] 中介绍如何制作库

## GCC 的选项

| 选项                  | 描述        |
| :------------------ | :-------- |
| `gcc -D {宏名}`       | 编译时宏定义    |
| `gcc -I {头文件目录}`    | 增加头文件搜索目录 |
| `gcc -O{0,g,1,2,3}` | 指定优化等级    |
| `gcc -Wall`         | 输出全部警告    |
| `gcc -g`            | 添加调试信息    |
