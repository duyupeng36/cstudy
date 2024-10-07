# GCC

在 [[程序的结构]] 中，我们知道一个 C 程序有 **源文件(`.c` 文件)** 和 **头文件(`.h` 文件)** 组成。在学习了基本的汇编指令之后，我们来看看 GCC 是如何生成程序的

![[Pasted image 20241007102703.png]]

> [!tip]
> 
> 前三个阶段：预处理，编译和汇编统一称为 **编译阶段**
> 

预处理阶段我们在 C 基础阶段已经完成学习了。现在，我们从编译阶段开始研究程序的生成过程

## 编译

参考下面的 C 代码

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

使用 `gcc -Og -S main.c` 得到的汇编代码如下

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



