# 函数

函数简单来说就是 **一连串语句**，这些语句 **被组合在一起**，并被指定了一个 **名字**。虽然 “函数” 这个术语来自数学，但是 C 语言的函数不完全等同数学函数

> [!tip]
> 
> 在 C 语言中，函数不一定要有参数，也不一定要计算数值

**函数是 C 程序的构建块**。每个函数本质上是一个 **自带声明和语句的小程序**。可以利用函数把程序划分成小块，这样便于人们理解和修改程序。

> [!tip]
> 
> **函数可以复用**：一个函数最初可能是某个程序的一部分，但可以将其用于其他程序
> 

在前面的内容中，我们已经接触过一些函数了。现在，我们介绍函数的细节知识

## 定义

函数定义的一般格式为

```c
返回类型 函数名(形式参数列表)
	复合语句
```

函数的 **“返回类型”** 是函数 **返回值的类型**

> [!important] 
> + **不能返回数组**，但关于返回类型没有其他限制
> + 指定返回类型是 `void` 类型，说明函数没有返回值
> + 如果省略返回类型，C89 标准中会假定函数返回值的类型是 `int` 类型；C99 标准中是不合法的
> 

**函数名** 后边有一串 **形式参数列表**。需要在每个形式参数的前面说明其类型，**形式参数间用逗号进行分隔**

> [!tip] 
> 
> 如果函数 **没有形式参数**，那么 **圆括号内应该出现 `void`**
> 
> 即使 **几个形式参数具有相同的数据类型**，也 **必须分别说明每个形式参数的类型**
> 

```c
double average(double a, b) // 这是错误的
{
	return (a + b) / 2;
}
```

**复合语句是 _函数体_**，函数体由 **一对花括号**，以及 **内部的 _声明_ 和 _语句_ 组成**

```c
double average(double a, double b) 
{
	double sum; // 声明
	sum = a + b;  // 语句
	return sum / 2; // 语句
}
```

> [!important] 
> 
> **函数体内声明的变量专属于此函数**，其他函数不能对这些变量进行检查或修改
> 
> + C89/C90 要求变量声明必须出现在语句前面；C99 中变量声明和语句可以混在一起，只要 **变量在第一次使用之前进行声明就行**
> 
> 

**对于 _返回类型_ 为 _void 的函数_，称为 _void 函数_**，其函数体可以只是一对花括号

```c
void print(void)
{
	// TODO: 准备要完成的内容
}
```

> [!tip]
> 
> 程序开发过程中留下空函数体是有意义的。因为没有时间完成函数，所以为它预留下空间，以后可以再回来编写它的函数体。
> 

程序开发中，我们可能经常需要使用 **平均值函数**，C 语言库没有这个函数，我们可以自己定义一个

```c
// 返回类型 函数名( 形参列表 )
double average(double a, double b) 
{
	return (a + b) / 2;
}
```

## 调用


**函数调用** 由 **函数名** 和跟随其后的 **实际参数列表** 组成，其中 **实际参数列表用圆括号括起来**

```c
average(x, y); 
average(10, y);
average(10, 20);
```

> [!warning] 
>
> 如果丢失圆括号，那么将无法进行函数调用
> 
> ```c
> average; // 错误的
> ```
> 
> 这样的结果是合法（但没有意义）的表达式语句，虽然看上去正确，但是不起任何作用
> 

对于 **void 函数** 的调用，后面 **始终跟着分号**，使得该调用称为语句

```c
print();
```

**非 void 函数调用会产生一个值**，该值可以存储在变量中，进行测试或用于其他用途

```c
avg = average(x, y);
if (average(x, y) > 10) {
	
}
```

如果不需要函数的返回值也可以 **直接丢弃**

```c
// 丢弃函数的返回值
average(10, 20);
```

一个更为正常的丢弃函数返回值的例子就是 `printf` 函数返回写入标准输出流的字节数

```c
num_chars = printf("hello, world!\n");
```

因为我们可能对显示出的字符数量不感兴趣，所以通常会丢掉 `printf` 函数的返回值

```c
printf("Hello, world!\n");
```

例程：素数判定

```c title:prime.c
#include <math.h>
#include <stdio.h>


bool is_prime(int n) {
    for (int i = 2; i <= sqrtl(n); i ++) {
        if (n % i == 0) {
            return false;
        }
    }
    return true;
}


int main() {

    int number;
    printf("Enter a number: ");
    scanf("%d", &number);


    if(is_prime(number)) {
        printf("prime number\n");
    } else {
        printf("not prime number\n");
    }

    return 0;
}
```

## 声明

在上面的例程中，函数的定义总是放置在调用点的上面。**C 标准并 _没有_ 要求函数的定义必须放置在调用点之前**。我们重新 `prime.c` 文件中的代码

```c title:prime.c
#include <math.h>
#include <stdio.h>



int main() {

    int number;
    printf("Enter a number: ");
    scanf("%d", &number);


    if(is_prime(number)) {
        printf("prime number\n");
    } else {
        printf("not prime number\n");
    }

    return 0;
}

bool is_prime(int n) {
    for (int i = 2; i <= sqrtl(n); i ++) {
        if (n % i == 0) {
            return false;
        }
    }
    return true;
}
```

在 `main` 函数中遇到第一个 `is_prime` 函数调用时，编译器没有任何关于 `is_prime` 函数的信息

> [!tip]
> 
> 编译器不知道 `is_prime` 的返回值类型，形式参数类型和个数

此时，编译器不会给出出错消息，而是假设 `is_prime` 函数返回 `int` 型的值

> [!tip] 隐式声明
> 
> 这里，编译为 `is_prime` 函数创建了一个 **隐式声明**
> 
> 编译器无法检查传递给 `is_prime` 的实参个数和实参类型，只能进行 **默认实参提升** 并期待最好的情况发生

当编译器在后面遇到 `average` 的定义时，它会发现函数的返回类型实际上是`bool` 而不是 `int`。从而，编译器给出错误提示


C 语言允许 **在调用前声明每个函数**。函数声明使得编译器可以先对函数进行概要浏览，而函数的完整定义以后再给出。**函数声明类似于函数定义的第一行**，不同之处是在其 **结尾处有分号**

```c
返回类型 函数名(形参列表);
```

> [!important] 函数的声明必须与函数的定义一致
> 

为 `is_prime` 添加函数声明后的代码如下

```c title:prime.c
#include <math.h>
#include <stdio.h>

bool is_prime(int n);


int main() {

    int number;
    printf("Enter a number: ");
    scanf("%d", &number);


    if(is_prime(number)) {
        printf("prime number\n");
    } else {
        printf("not prime number\n");
    }

    return 0;
}

bool is_prime(int n) {
    for (int i = 2; i <= sqrtl(n); i ++) {
        if (n % i == 0) {
            return false;
        }
    }
    return true;
}
```

> [!help] 函数原型
> 
> 为了区别旧时函数声明风格，这里我们将函数的声明称为 **函数原型**
> 
> 函数原型本质就是函数的声明。原型为如何调用函数提供了完整的描述：提供了多少实际参数、这些参数应该是什么类型，以及返回的结果是什么类型
> 

> [!tip]  **数原型不需要说明函数形式参数的名字**，只要显示它们的类型就可以了
> 
> 通常 **最好是不要省略形式参数的名字**，因为这些名字可以说明每个形式参数的目的，并且提醒程序员在函数调用时实际参数的出现次序
> 

> [!tip] 
> 
> C99 遵循这样的规则：在调用一个函数之前，必须先对其进行声明或定义。调用函数时，如果此前编译器未见到该函数的声明或定义，会导致 **隐式声明错误**

## 实际参数

这里我们有必要理解 **实际参数(argument)** 和 **形式参数(parameter)** 的差异

> [!tip] 
> 
> **形式参数(parameter)** 出现 **在函数定义中的变量声明**，是 _预先声明的变量_，其值在函数被调用时获得
> 
> **实际参数(argument)** 是出现 **在函数调用中的表达式**
> 

### 值传递

在 C 语言中，**实际参数是值传递的**：调用函数时，计算出每个实际参数的值并且把它赋给相应的形式参数

> [!tip]
> 
> **形式参数的值时实际参数的值的副本**。也就是说，**函数内部对形式参数的改变并不会影响实际参数的值**
> 
> 从效果上来说，每个形式参数的行为好像是把变量初始化成与之匹配的实际参数的值
> 

下面是交换两个变量的函数的初次尝试

```c
#include <math.h>
#include <stdio.h>


void swap(int a, int b) {
    const int temp = a;
    a = b;
    b = temp;
}



int main() {

    int a = 10, b = 20;
    printf("交换之前：a = %d, b = %d\n", a, b);  // a = 10, b = 20
    swap(a, b);
    printf("交换之后：a = %d, b = %d\n", a, b);  // a = 10, b = 20

    return 0;
}
```

运行结果为

```
交换之前：a = 10, b = 20
交换之后：a = 10, b = 20
```

如下图所示，显示了函数调用的过程的 **栈帧变化**

![Pasted image 20240925220821|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755701770881-565068f6793640d58eac9a31d2744858.png)

> [!tip]
> 
> 当函数返回时，属于该函数的栈帧就立即被销毁
> 

### 实际参数的转换

C 语言 _允许在实际参数的类型与形式参数的类型不匹配_ 的情况下进行函数调用

> [!tip] **函数调用转换**
> 
> **编译器在调用前遇到原型**。**就像使用赋值一样**，每个实际参数的值被隐式地转换成相应形式参数的类型
> 
> **编译器在调用前没有遇到原型**。编译器 **执行默认实参提升**
> + 把 `float` 类型的实际参数转换成 `double` 类型
> + 执行 **整数提升**
> 

### 数组型实参

数组经常被用作实际参数。形式参数是一维数组时，通常情况下不说明数组的长度

```c
int f(int a[])
{
	....
}
```

> [!tip]
> 
> 实际参数可以是元素类型正确的任何一维数组
> 

> [!question] **函数如何知道数组是多长呢？**
> 

C 语言没有为函数提供任何简便的方法来确定传递给它的数组的长度；如果函数需要，我们必须把长度作为额外的参数提供出来。

> [!warning] 警告：不要使用 `sizeof` 运算符计算数组形参的长度
> 
> 我们会在 [[C 语言：指针#指针与数组]] 中解释原因
> 

例程，给出具有 `int` 类型值的数组 `a` 时， `sum_array` 函数返回数组 `a` 中元素的和

```c
int sum_array(int a[], int n)
{
	int i, sum = 0;
	for(i = 0; i < n; i++) 
	{
		sum += a[i];
	}
	return sum;
}
```

> [!tip]
> 
> `sum_array` 函数需要知道数组的长度，因此必须把长度作为第二个参数提供
>

在调用 `sum_array` 函数时，第一个参数是数组的名字，第二个参数是这个数组的长度

```c
sum_array(b, LEN); // LEN 是数组 b 中元素的个数
```

> [!tip] 
> 
> **函数无法检测传入的数组长度的正确性**

可以利用这一点，让函数只计算数组的前几项的值(**告诉函数数组的长度比实际长度小**)

```c
total = sum_array(b, 50); // 只计算数组前 50 个元素的和。计算数组 b 中有 100 个元素
```

> [!warning] 
> 
> 注意不要告诉函数，数组型实际参数比实际情况大
> 
> ```c
> total = sum_array(b, 150);   // WRONG
> ```
> `sum_array` 函数将超出数组的末尾，从而导致未定义的行为

> [!important] 
> 
> 函数改变数组形参的元素，可以在数组实参中发现
> 

下面的函数通过在每个数组元素中存储 `0` 来修改数组

```c
void set_zeros(int a[], int n) 
{
	for(int i = 0; i < n; i++)
	{
		a[i] = 0;
	}
}
```

### 可变长参数

类似于 `printf` 这样的函数 **允许任意数量的参数**， `<stdarg.h>` 头提供的工具使我们能够自己编写带有变长参数列表的函数。

`<stdarg.h>` 中声明了一种类型并定义了几个宏函数

| 类型/宏                 | 描述                                                   |
| :------------------- | ---------------------------------------------------- |
| `va_list ap`         | 指针类型，指向可变参数列表开始元素                                    |
| `va_start(ap, n)`    | 将 `ap` 指向可变参数的开始位置，里从 `n` 后边开始                       |
| `va_arg(ap, type)`   | 将 `ap` 指向的可变参数转换为 `type` 指定的类型，<br>然后 `ap` 指向下一个可变参数 |
| `va_end(ap)`         | 清理可变参数                                               |
| `va_copy(dest, src)` | 将 `src` 复制到 `dest` 中                                 |

> [!attention] 
> 
> `va_copy` 宏把 `src` 复制到 `dest` 中。`va_copy` 之所以能起作用，是因为在把 `src` 复制到 `dest` 之前可能已经多次用 `src` 来调用 `va_arg` 了。调用 `va_copy` 可以使函数记住在参数列表中的位置，从而以后可以回到同一位置继续处理相应的参数（及其后面的参数）
> 

> [!tip]
> 
> 每次调用 `va_start` 或 `va_copy` 时都必须与 `va_end` 成对使用，而且这些成对的调用必须在同一个函数中
> 
> 所有的 `va_arg` 调用必须出现在 `va_start`（或 `va_copy`）与配对的 `va_end` 调用之间
> 

```c
int max(int n, ...)
{
    va_list ap;
    int i, current, largest;
    
    va_start(ap, n);

    largest = va_arg(ap, int);
    for(i = 1; i < n; i++) 
    {
        current = va_arg(ap, int);
        if(current > largest)
            largest = current;
    }
    va_end(ap);
    return largest;
}
```

> [!tip]
> 
> 可变参数只能是函数的最后一个参数。并且，在可变参数前必须有其他参数
> 

在 C23 标准起，`<stdarg.h>` 中用于实现可变函数参数的宏 `va_start` 不再强制需要第二个参数，拥有可变参数的函数也不再需要至少一个有名参数

```c
int max(...)  // 不需要命名参数
{
    va_list ap;
    int current, largest;
    
    va_start(ap); // 不需要第二个参数

    int count = va_arg(ap, int);
    for(int i = 0; i < count; i++) 
    {
        current = va_arg(ap, int);
        if(current > largest)
            largest = current;
    }
    va_end(ap);
    return largest;
}
```

### C99 新特性

#### 变长数组形参

如果使用变长数组形式参数，我们可以显式地说明数组 `a` 的长度就是 `n`

```c
int sum_array(int n, int a[n])
{
	int i, sum = 0;
	for(i = 0; i < n; i++) 
	{
		sum += a[i];
	}
	return sum;
}
```

> [!tldr] 第一个参数(`n`)的值确定了第二个参数(`a`)的长度

> [!warning] 变长数组的形参的长度一定要在数组形参之前声明

#### 数组参数中的 static

C99 允许在数组参数声明中使用关键字`static`。在下面的例子中，将 `static` 放在数字 `3` 之前，表明数组 `a` 的长度至少可以保证是 `3`

```c
int sum_array(int a[static 3], int n)
{
	....
}
```

> [!warning] 如果数组参数是多维的，**static 仅可用于第一维**

#### 复合字面量

对于 `sum_array` 函数，当调用 `sum_array` 函数时，第一个参数通常时一个数组名

```c
int b[] = {3, 0, 3, 4, 1};
total = sum_array(b, 5)
```

如果 `b` 不做其他用途，那么把 `b` 作为一个变量声明就行显得有点浪费

在 C99 中，可以使用 **复合字面量** 来避免该问题，复合字面量是通过 **指定其包含的元素而 _创建的没有名字的数组_**

```c
total = sum_array((int []){3, 0, 3, 4, 1}, 5);
```

> [!tip] 复合字面量为左值(可寻址的)，所以其元素的值可以改变

## return 语句

**非 void 的函数必须使用 return 语句来指定将要返回的值**。`return` 语句有如下格式：

```c
return 表达式;
```

> [!tip] **函数返回转换**
> 
> 如果 `return` 语句中表达式的类型和函数的返回类型不匹配，那么编译器会把表达式的类型隐式地转换成返回类型
> 

如果没有给出表达式，`return` 语句可以出现在返回类型为 `void` 的函数中

```c
return ; // 返回空
```

## 程序终止

既然 `main` 是函数，那么它必须有返回类型。正常情况下，`main` 函数的返回类型是 `int` 类型

```c
int main() 
{
	...
}
```

**`main` 函数返回的值是状态码**，在某些操作系统中程序终止时可以检测到状态码

> [!tip] 状态码
> 
> 如果程序 **正常终止**，`main` 函数应该返回 `0`
> 
> 如果程序 **异常终止**，`main` 函数应该返回非 `0` 的值
> 

### exit 函数

在 `main` 函数中执行 `return` 语句是终止程序的一种方法，另一种方法是调用 `exit` 函数，此函数属于 `<stdlib.h>` 头。传递给 `exit` 函数的实际参数和 `main` 函数的返回值具有相同的含义：两者都说明程序终止时的状态

```c
exit(0); // 表示正常终止
exit(1); // 表示异常终止
```

直接使用 `0` 和 `1` 导致程序可读性差，可以使用`EXIT_SUCCESS` 和 `EXIT_FAILURE` 宏替代 `0` 和 `1` ，它们都是定义在 `<stdlib.h>` 中的宏

> [!tip] 
> 
> `EXIT_SUCCESS` 和 `EXIT_FAILURE` 的值都是由实现定义的，通常分别是 `0` 和 `1`
> 

## 递归

简单来说，**递归函数就是函数 _自己调用自己_**。有两种实现方式，一种是 **_直接_ 在自己函数中调用自己**，一种是 **_间接_ 在自己函数中调用的其他函数中调用了自己**。

> [!important] 递归需要 **有边界条件**、**递归前进段**、**递归返回段**
> + **递归一定要有边界条件**，当边界条件不满足的时候，递归前进；当边界条件满足的时候，递归返回
> 

### 斐波拉契数列

例如，使用递归实现 **斐波拉契数列**

$$
fib(n) = \begin{cases} 0 & n=0 \\ 1 & n=1 \\ fib(n-1) + fib(n-2) & n \ge 2 \end{cases}
$$

```c
#include <math.h>
#include <stdio.h>


unsigned int fib(const unsigned int n) {

    if (n <= 2) {
        return 1;
    }
    return fib(n - 1) + fib(n - 2);
}
```

这个实现存在性能问题，**每次计算的 `fib(n)` 结果在函数返回后就被丢弃了**，当第二次计算 `fib(n)` 时，又会再次执行递归

![Pasted image 20240926104051|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755701790320-62d1482b3bd5455c90b1e0fa30f27ec3.png)

**左分支的计算包含了右分支的值**，我只需要记录左分支的结果即可加速这个算法

```c
unsigned int fib(const unsigned int n, unsigned int *result) {

    // 没有提供记录的位置
    if (result == nullptr) {
        result = (unsigned int *)calloc(n+1, sizeof(unsigned int));
    }

    if (n <= 2) {
        return 1;
    }

    // result[n] 为 表示没有记录
    if (result[n] == 0) {
        result[n] = fib(n-1, result) + fib(n-2, result);
    }

    return result[n];
}
```

根据  **斐波拉契数列** 的递归定义，可以采用 **动态规划** 的思想进行顺序计算，编写出下面的代码

```c
unsigned int fib(unsigned int n) {  
    unsigned int prev = 0, next = 1;  
    unsigned int temp;  
    while (n > 0) {  
        temp = prev + next;  
        prev = next;  
        next = temp;  
        n--;  
    }  
    return prev;  
}
```

> [!tip] 
> 
> **递归也可以用于模拟循环**
> 

```c
#include <math.h>
#include <stdio.h>
#include <stdlib.h>


unsigned int fib(unsigned int n, unsigned int pre, unsigned int next) {
    if (n < 3) {  // 这个条件依赖于起始值
        return next;
    }
    return fib(n-1, next, pre+next);
}


int main() {
    printf("%u\n", fib(10, 1, 1));
    return 0;
}
```

计算过程如下图

![Pasted image 20241004004337|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755701803856-5ceb1b41fbd8492da915f39a7ccc810c.png)

利用线性代数的知识，斐波拉契数列可以使用矩阵该表示

$$
\begin{cases}
F_{n+1} &= F_{n} + F_{n-1} \\
F_{n} &= F_{n}
\end{cases} 
$$

上述线性方程组可以使用矩阵表示为

$$
\begin{pmatrix}
F_{n+1}\\ F_{n}
\end{pmatrix} = 
\begin{pmatrix}
1 & 1 \\ 1 & 0
\end{pmatrix} \begin{pmatrix}
F_{n}\\ F_{n-1}
\end{pmatrix}
$$
因此我们有

$$
\begin{pmatrix}
F_{n+1}\\ F_{n}
\end{pmatrix} = 
\begin{pmatrix}
1 & 1 \\ 1 & 0
\end{pmatrix} ^{n} \begin{pmatrix}
F_{1}\\ F_{0}
\end{pmatrix} = \begin{pmatrix}
1 & 1 \\ 1 & 0
\end{pmatrix} ^{n} \begin{pmatrix}
1\\ 0
\end{pmatrix}
$$

### 汉诺塔

汉诺塔是一个经典的递归问题，如下图所示

![Pasted image 20240926135946|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755701810528-5b74fe69444a47d2ab22db1829b9d849.png)

将 A 柱上的 $n$ 个盘片移动到 C 柱上，在移动过程中需要遵循以下规则：

1. 一次只能移动一个盘子
2. 任何时候较大的盘子不能放在较小的盘子上
3. 可以使用一个辅助柱子(B 柱)

问移动 $n$ 个盘片需要多少步？怎么移动？我们将盘片从上到下编号为 $1,2,3\cdots,n$

> [!tip] $n=1$ 时
> 
> 此时，只有 $1$ 个盘片，只需要 $1$ 步即可移动到 C 柱上

> [!tip] $n=2$ 时
> 
> 此时，有 $2$ 个盘片。首先将 $1$ 号盘片移动到 B 柱上，再将 $2$ 号盘片移动到 C 柱上，最后将 $1$ 号盘片移动到 C 柱上
> 

> [!tip] $n=3$ 时
> 
> 此时，有 $3$ 个盘片。首先要将前 $2$ 个盘片移动到 B 柱上，再将 $3$ 号盘片移动到 C 柱上，最后将前 $2$ 个盘片移动到 C 柱上
> 

如果需要移动 $n$ 个盘片，就需要将前 $n-1$ 个盘片移动到 B 柱上，再将 $n$ 号盘片移动到 C 柱上，最后将 B 柱上的 $n-1$ 个盘片移动到 C 柱上。

> [!hint] 
> 
> 显然，这是一个递归过程
> 

移动 $n$ 个盘片需要的步骤的递推公式为

$$
F(n) = 2F(n-1) + 1 = 2^{n}-1
$$

下面的视频展示了汉诺塔的移动过程

<iframe width="560" height="315" src="https://www.youtube-nocookie.com/embed/KlY-A8gk5sA" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

```c
void hanoi(int n, const char source, const char target, const char auxiliary) {
    if (n == 1) {
        printf("Move disk 1 from %c to %c\n", source, target);
        return;
    }

    // 将 n - 1 个盘片从 A 柱 移动到 B 柱上
    hanoi(n - 1, source, auxiliary, target);
    // 将 n 号盘片从 A 柱移动到 C 柱上
    printf("Move disk %d from %c to %c\n", n, source, target);
    // 将 n - 1 个盘片从 B 柱子移动到 C 柱上
    hanoi(n - 1, auxiliary, target, source);
}
```

## 程序结构

**局部变量** 和 **全局变量** 是程序包含多个函数时所产生问题

> [!tip] 变量的作用域与存储期限
> 
> **作用域**：作用域是 _编译时_ 概念。即，**可以引用该变量的那一部分程序文本**
> 
> **存储期**：存储期是 _运行时_ 概念，即能够确保 **变量的存储空间必定存在的那一部分时间**
> 

### 局部变量

我们把 **在函数体内声明的变量** 称为该函数的 **局部变量**

```c
int sum_digits(int n) 
{
	int sum = 0; // sum 就是局部变量
	while (n > 0)
	{
		sum += n % 10;
		n /= 10;
	}
	return sum;
}
```

默认情况下，局部变两具有 **块作用域** 和 **自动存储期**

> [!tip] 块作用域
> 
> 从变量声明的点开始一直到所在函数体的末尾
> 
> 因为局部变量的作用域不能延伸到其所属函数之外，所以其他函数可以把同名变量用于别的用途
> 

```c
int sum_digits(int n) 
{
	int sum = 0;          ----
	while (n > 0)            |
	{                        |
		sum += n % 10;       |-- sum 的作用域
		n /= 10;             |
	}                        |
	return sum;              |
}                         ----
```

> [!tip] 自动存储期
> 
> 局部变量的存储空间是在包含该变量的 **函数被调用时“自动”分配的**，**函数返回时收回分配**，所以称这种变量具有自动存储期
> 
> + 具有动态存储期的局部变量分配在进程虚拟内存中的 **栈区域**。函数调用时分配栈空间，函数返回时 **回收栈空间**，导致存储在栈中的变量不可用
> 

> [!hint] 形式参数和局部变量的性质一样
> 
> 形式参数拥有和局部变量一样的性质，即自动存储期和块作用域
> 
> 形式参数和局部变量的唯一区别是 **函数的每次调用都会对形式参数进行初始化**
> 
#### 静态局部变量

在局部变量声明中使用关键字 **static** 可以使变量具有 **静态存储期** 而不再是自动存储期。因为具有静态存储期的变量拥有 **永久的存储单元**，所以在 **整个程序执行期间都会保留变量的值**

> [!tip] 静态存储期
> 
> 具有静态存储期的变量，在 **程序启动是会被初始化** 并且 **只会被初始化一次**。在程序的整个执行期间都会保留变量的值
> 
> + 具有静态存储期的变量分配在进程虚拟内存中的 **数据段**。处于数据段的变量不会被销毁的
> 

### 外部变量

我们把 **在函数体外部声明的变量** 称为该函数的 **外部变量**，有时称为 **全局变量**

外部变量具有 **文件作用域** 和 **静态存储期**

> [!tip] 文件作用域
> 
> **从变量声明的点开始一直到所在文件的末尾**
> 
> 变量声明点之后定义的函数都能直接访问这个外部变量
> 

### 程序块

一个复合语句也是一个 **块**，但块并非只有复合语句这一种形式。块也叫 **程序块**

- 整个 `if` 语句是一个程序块；`if` 语句的每一个子句也是程序块
- `while, do-while, for` 语句也都是一个程序块
- 函数体是程序块
- **程序块允许嵌套**

默认情况下，**声明在程序块中的变量的存储期是自动的**：进入程序块时为变量分配存储单元，退出程序块时收回分配的空间。变**量具有块作用域，也就是说，不能在程序块外引用**

> [!tip] 程序块的作用
> 
> + 避免函数体起始位置的声明与只是临时使用的变量混淆
> + 减少名字冲突
> 

C99 允许在程序块的任何地方声明变量，就像允许在函数体内的任何地方声明变量一样

### 作用域规则

C 语言的 **作用域规则** 使得程序源和编译器可以确定程序中同名标识符的含义。C 语言中的作用域规则如下
- 一个程序块就是一个作用域。当程序块嵌套时，会出现作用域嵌套的情况
- 内层作用域声明的标识符会覆盖上层作用域出现的同名标识符
- 当离开内层作用域时，原来的标识符恢复

下面的程序片段是作用域规则的一个示例

```c
int i; // 全局作用域

void f(int i) // i 是函数块作用域
{
	i = 1; // 访问并修改函数块作用域内的变量i
}

void g(void)
{
	int i = 2; // 函数块作用域
	if(i > 0) 
	{
		int i;  // if 块作用域 
		i = 3;  // 访问并修改 if 块作用域中的变量
	}
	i = 4; // 访问并修改 函数块作用域中的变量
}

void h(void)
{
	i = 5; // 访问并修改全局作用域的变量
}
```
