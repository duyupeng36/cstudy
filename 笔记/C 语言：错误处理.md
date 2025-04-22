# 错误处理

真正商业用途的程序却必须“非常稳健”，即能够从错误中恢复正常而不至于崩溃

> [!tip]
> 为了使程序非常稳健，我们需要能够 **预见程序执行时可能遇到的错误**，包括 **对每个错误进行检测**，并 **提供错误发生时的合适行为**
> 

这里我们介绍 C 语言的两种 **检测错误的方法**：**断言** 和 **errno 全局变量**。最后，探讨 **非局部跳转**

> [!tip] 
> 
> 错误处理还有一种机制是 [[Linux 系统编程：信号基本概念]]。不在这里介绍，后续在系统编程中介绍信号
> 


## 断言


**断言** 来验证在程序实现期间所做的特定假设仍然有效。断言是一个具有布尔值（称为谓词）的函数，表达了关于程序的逻辑命题

C 语言支持两种类型的断言 **静态断言** 和 **运行期断言**

> [!tip] 静态断言：在 C11 标准开始支持静态断言 `_Static_assert` 
> 
> 在编译期间使用 `_Static_assert` 执行静态断言：**_检查_ 和 _诊断_ 工作放在程序 _编译期间_ 进行**
> 
> `_Static_assert` 是 C11 新增的关键字
> 

> [!tip] 运行期断言：
> 
> 运行期使用 `<assert.h>` 提供的 `assert` 宏在运行期执行 _检查_ 和 _诊断_ 工作
> 



### 静态断言

C11 标准新增了一个关键字 `_Static_assert` 用于在编译时检查和诊断假设是否有效。语法格式如下：

```c
_Static_assert(常量表达式, 字符串字面值);
```

> [!tip] 常量表达式：必须是一个整型常量表达式
> 
> 该常量表达式的值如果 **不是 $0$（即，`true`）**，则没有任何效果；如果值 **是 $0$（即，`false`）**，则违反约束条件
>   

C 标准规定 `unsigned int` 类型的对象至少占 $2$ 字节。如果这些字节按照补码编码解释，则取值范围为 $-32767 \sim 32767$ ，然而绝大多数平台支持比这个规定大得多的范围。为了保险，要求 `unsigned int` 能够表示超出上述范围的数值，因此可以使用静态断言决定是否运行程序继续编译

```c
#include <limits.h>

int main()
{
	// 静态检查 unsigned int 是否符合要求
	_Static_assert(UINT_MAX >= 32767, "Not suport this platform.");
	return 0;
}
```

头文件 `<assert.h>` 中提供了如下定义

```c
#define static_assert _Static_assert
```

在 C23 标准中，`static_assert` 是关键字。故 `<assert.h>` 不再提供它

### 运行期断言

`assert` 宏会将运行期诊断测试插入程序中。该宏在头文件 `<assert.h>` 中定义，接受一个标量表达式作为参数

```c
#define assert(scalar-expression) /* 由实现定义 */
```

如果标量表达式为 0，那么宏扩展通常会向标准错误流 `stderr` 写入失败调用的相关信息，信息写入 `stderr` 之后，`assert` 宏会调用 `abort` 函数

> [!attention] 
> 
>  C89 标准要求 `assert` 的参数必须是 `int` 类型。C99 标准 **允许任意标量类型**
> 

> [!tip] 失败调用的相关信息包括：
> 
> 包括 **参数文本**、**源文件名** `__FILE__`、源文件中的 **行号** `__LINE__` 和 **外围函数名** `__func__`
> 
> 不同实现可能输出的信息并不相同。C99 标准要求 `assert` 显示器所在的函数名。C99 的建议消息格式为: `Assertion failed: expression, function abc, file xyz, line nnn
> 


下面程序片段，用于检测数组下标是否越界

```c
int a[10];
... // 得到一个数组下标 i
assert(0 <= i && i < 10); // 检查下标 i 是否在规定范围内
a[i] = 0; // i 在规定范围内。否则，向 stderr 中写入消息，然后调用 abort 终止程序
```

输出的消息由实现定义。可能是: `Assertion failed: 0 <= i && i < 10, file demo.c, line 16`


`assert` 引入例如额外的检查，会增加程序运行时间。通常，**在测试中使用 `assert`，测试完成后禁止 `assert`**

```c
#define NDEBUG // 禁用 assert
#include <assert.h>
```

`NDEBUG` 宏的值不重要，只要定义了 `NDEBUG` 宏即可

> [!tip] 
> 
> 在编译时，可以使用编译器选项 `-D` 在编译时定义宏
> 

## errno 错误码

标准库中的一些函数通过向 `<errno.h>` 中声明的 `int` 类型 `errno` 变量存储一个错误码（正整数）来表示有错误发生。`errno` **可能实际上是个宏**。如果确实是宏，C 标准要求它表示左值，以便像变量一样使用

> [!attention] 
> 
> 当使用 `errno` 来检测库函数调用中的错误时，在函数调用前将 `errno` 置零非常重要
> 
> ```c
> errno = 0; 
> y = sqrt(x);  
> f (errno != 0 ) { 
> 	fprintf(stderr, "sqrt error; program terminated.\n"); 
> 	exit(EXIT_FAILURE); 
> } 
> ```
> 

标准库 `<stdio.h>` 和 `<string.h>` 提供了两个函数，用于获取 `errno` 对应的标准错误信息

```c
void perror(const char *s);  // 来自 <stdio.h> 
char *strerror(int errnum);  // 来自 <string.h>
```

`perror` 函数在 `<stdio.h>` 中定义，调用 `perror(msg_head)` 根据 `errno` 的值生成一条错误信息: `"msg_head: <errno_msg>\n"`，其中 `errno_msg` 是由 `errno` 决定的消息

`strerror` 函数属于 `<string.h>`。当以错误码为参数用 `strerror` 时，函数会返回一个指针，它指向一个描述这个错误的字符串
## 非局部跳转

通常，函数会返回到它被调用的位置。`goto` 语句只能在同一个函数的作用域内使用。**但是 `<setjmp.h>` 可以使一个函数直接跳转到另一个函数，不需要返回**。`<setjmp.h>` 中声明了两个函数，它们的原型如下

```c
int setjmp(jmp_buf env);
_Noreturn void longjmp(jmp_buf _Buf,int _Value);
```

调用 `setjmp` 需要一个 `jmp_buf` 类型的变量。`setjmp` 被调用时，会将当前环境保存在 `env` 中，然后返回 `0`

> [!tip] 
> 
> `setjmp` 保存的环境信息回在调用 `longjmp` 时使用到
> 

调用 `longjmp` 跳转到 `setjmp` 设置的 `env` 环境中，然后 `setjmp` 返回 `longjmp` 的第二个参数 `_Value`

> [!attention] 
> 
> 如果 `_Value` 的值为`0`，那么 `setjmp` 宏会返回 `1`
> 

> [!important] 
> 
> 要确保在调用 `longjmp` 时，`_Buf` 的实际参数是被 `setjmp` 宏初始化的
> 

总而言之，**`setjmp` 会在第一次调用时返回  `0`**；随后，`longjmp` 将控制权重新转给最初的 `setjmp` 宏调用，而 **`setjmp` 在这次调用时会返回一个非零值**

下面的例程用于测试 `setjmp` 和 `longjmp`

```c
/*
测试 setjmp 和 longjmp
*/

#include <setjmp.h>
#include <stdio.h>

jmp_buf env;  // 定义一个全局变量，用于存储环境位置（包括setjmp自身位置的指针）

void f1(void);
void f2(void);

int main() {
	// 初始化，第一次调用返回 0；第二次调用返回 longjmp 指定的值
    if(setjmp(env) == 0) {
        printf("setjmp return 0\n");
    } else {
        printf("Program terminates: longjmp called\n");
        return 0;
    }
    f1();
    printf("Program terminates normally\n");
    return 0;
}

void f1(void) {
    printf("f1 begins\n");
    f2();
    printf("f1 returns\n");
}

void f2(void) {
    printf("f2 begins\n");
    longjmp(env, 1);  // 将控制直接转移到 main 中的setjmp 位置
    printf("f2 returns\n");
}
```
