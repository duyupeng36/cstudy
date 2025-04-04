# 数组

## 基本概念

数组是 **一片连续的内存区域** 并且这片内存区域通过 _数据类型_ 将其 **划分 $n$ 个大小相等的小的内存区域**。也就是说，数组只能 **存储同一种 _数据类型_ 的对象**。

当 **_同种类型_** 的一组数据存储在一片连续的内存区域时，只要知道这片连续内存区域的 **_基地址_**，就能 **_随机访问_** 其中的任何一个元素

> [!tip] 随机访问：访问一个元素的时间复杂度为 $O(1)$，常数时间复杂度
> 

在大多数语言中，包括 C 语言，**数组的索引都是从 $0$ 开始的**。就其原因在于索引从 $0$ 开始使得 **元素地址的计算效率更高**。例如，要计算第 $i$ 个元素的地址($\text{Addr}_{i}$) 

$$
\text{Addr}_{i} = \text{Addr}_{\text{base}} + i \times \text{sizeof}(\text{type})
$$

其中 $\text{Addr}_{\text{base}}$  表示数组的 **基地址**，$\text{sizeof}(\text{type})$ 表示 `type`  类型的 **对象占用的内存空间**(字节)

如果索引从 $1$ 开始，则第 $i$ 个元素的地址为

$$
\text{Addr}_{i} = \text{Addr}_{\text{base}} + (i-1) \times \text{sizeof}(\text{type})
$$
这样，_每次寻址都会多一次减法运算_

> [!summary] 
> 
> **数组** 是是用于 **存储多个相同类型数据** 的 **数据结构**  。数组中的 _值称为元素_，可以 _根据其在数组中的位置 **直接访问**_
>

## 一维数组

声明一个数组的语法为 `<元素类型> 数组名[数组长度]`

> [!tip] 
> 
> **数组元素可以是任何类型**
> 
> **数组长度** 可以用任何 **整型常量表达式** 指定。因为程序以后改变时可能需要调整数组的长度，所以 **较好的方法是用宏来定义数组的长度**
> 

例如，如下声明语句

```c
int a[10]
```

定义一个长度为 $10$ 的数组 `a`

![[Pasted image 20240925002338.png]]

换句话说它 **定义了一个由 $10$ 个对象组成的集合**，它们的 **名字分别为 `a[0], a[1], a[2], ..., a[9]`**

因为程序以后改变时可能需要调整数组的长度，所以较好的方法是用宏来定义数组的长度

```c
#define N 10

int a[N];
```

### 初始化

像其他变量一样，数组也可以在声明时获得一个初始值。**数组初始化器** 是用一个 _花括号_ 括起来的 **常量表达式列表**，常量表达式之间使用逗号进行分隔

```c
int a[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
```

如果 **初始化器比数组短**，那么数组中 **剩余的元素赋值为0**

```c
int a[10] = {1,2,3,4,5,6};
```

利用这一特性，可以很容易地 **把数组初始化为全 0**

```c
int a[0] = {0};
```

> [!warning] 
> 
> **初始化器完全为空是非法的**，所以要在花括号内放上一个 0
> 
> **初始化器比数组长也是非法的**
> 

> [!tip] 如果给定了初始化器，也可以省略数组的长度
> 声明数组时，给定了初始化器可以省略数组长度。数组的长度 **由编译器从初始化器中推断**
> ```c
> int a[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10}
>```
>
> 省略数组长度，和显示指定长度一样数组仍然有 **固定数量的元素**，
>

#### 指示器

数组中只有 **相对较少的元素需要进行显式的初始化**，而其他元素可以进行默认赋值

假设有一个长度为 $15$ 的数组 `a`，我们希望 `a[2] = 29, a[9] = 7, a[14] = 48`，其余元素全为 $0$ 。在 C89/C90 标准的语法中，我们只能如下进行初始化

```c
int a[15] = {0, 0, 29, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 48};
```

对于 **_大数组_** ，如果使用这种方式赋值，将是 **冗长** 和 **容易出错**的

C99 标准提供了一种称为 **指示器** 的语法，用于解决上述问题。**数组的指示器** 语法是在初始化器中，使用 `[i] = value` 的方式指定数组的第 $i$ 个元素的值。因此，上面的例子可以用指示器写为
+ `[i]` 称为指示符

```c
int a[15] = {[2] = 29, [9] = 7, [14] = 48};
```

> [!attention] 
> 
> 在指示器语法中 `[]` 中的 `i` 必须是 **常量表达式**
> 
> 如果被初始化的数组长度为 $n$，则 `[]` 中常量表达式值的范围为 $[0, n-1]$

在声明时，如果数组的长度省略，指示符可以指定任意非负整数；数组的长度由编译器 _根据指示符的最大值推断_

 ```c
int b[] = {[5] = 10, [23] = 13, [11] = 36, [15] = 29};
```

上述声明中，指示符的最大值为 $23$，因此数组的长度为 $24$

> [!tip] 初始化器中可以同时使用 _逐元素初始化_ 和 _指示器_

```c
int c[10] = {5, 1, 9, [4] = 3, 7, 2, [8] = 6};
```

> [!tip] 指示器 `[i]` 后面的元素初始化第 `i+1` 个元素

### 对数组使用 sizeof 运算符

目前我们回避了 **数组的类型** 这一概念。声明数组时使用的类型不是数组的类型，而是数组元素的类型。

> [!tip] 对象的类型决定了对象所占的内存空间大小

因此，数组的类型就决定了数组对象所占内存空间的大小。如下声明中

```c
int a[10];
```

变量名 `a` 是一个数组对象，其类型为 `int[10]`。运算符 `sizeof` 可以计算一个对象占据的内存大小。因此，`sizeof(a)` 的值等于 `10 * sizeof(int)`

还可以用 `sizeof(a[0])` 来计算数组元素的大小。用数组大小除以数组元素的大小可以得到数组的长度

```c
sizeof(a) / sizeof(a[0])
```

当需要数组长度时，一些程序员采用上述表达式。例如，数组 `a` 的清零操作可以写成如下形式

```c
for(int i = 0; i < sizeof(a) / sizeof(a[0]); i++) {
	a[i] = 0;
}
```

**即使数组长度在日后需要改变，也不需要改变循环**

上述代码在某些编译器中会给出一条警告信息，因为表达式 `sizeof(a) / sizeof(a[0])` 的值的类型是 `size_t`，这是一个无符号类型

> [!tip]
> 
> 把有符号整数与无符号整数相比较是很危险的。尽管上例这样做没有任何问题，因为 `i` 和 `sizeof(a) / sizeof(a[0])` 都是非负值
> 

为了避免出现这一警告，可以将 `i` 的类型改成 `size_t`，或者将 `sizeof(a) / sizeof(a[0])` 强制转换为有符号整数

```c
for(int i = 0; i < (int)(sizeof(a) / sizeof(a[0])); i++) {
	a[i] = 0;
}
```

表达式  `(int) (sizeof(a) / sizeof(a[0]))` 写起来不太方便，定义一个函数式宏来计算数组的长度

```c
#define SIZE(A) ((int)(sizeof(A)/sizeof(A[0])))

for(int i = 0; i < SIZE(a); i++) {
	a[i] = 0;
}
```

## 多维数组

数组可以有任意维数。例如，下面的声明产生一个二维数组(数学上称为矩阵)

```c
int m[5][10];
```

数组 `m` 有 $5$ 行 $10$ 列。如下所示，数组的行和列下标都从 $0$ 开始索引

![[Pasted image 20240925111411.png]]

为了访问 $i$ 行 $j$ 列的元素，表达式需要写成 $m[i][j]$。表达式 $m[i]$ 指明了数组 `m` 的第 $i$ 行，而 $m[i][j]$ 则选择了此行中的第 $j$ 个元素

> [!attention] 注意
> 
> 不要把 `m[i][j]` 写成 `m[i, j]`。C 语言会把逗号看作逗号运算符，因此 `m[i,j]` 就等同于 `m[j]`
> 

虽然我们以表格形式显示二维数组，但是实际上它们在计算机内存中的存储方式和一维数组相同。C 语言是按照 **行主序** 存储数组的，也就是从第 $0$ 行开始，接着是第 $1$ 行，以此类推。例如，下面显示了数组 `m` 的存储

![[Pasted image 20240925111050.png]]

> [!tip]
> 
> C 语言中，多维数组本质上也是一维数组
> 
> **二维数组** 本质上是 **元素是一维数组的数组**
> 
> **N 维数组** 就是 **元素是 N - 1 维数组的数组**
> 

### 初始化

通过 **嵌套一维初始化器** 的方法可以产生 **二维数组的初始化器**

```c
int m[5][9] = {{1, 1, 1, 1, 1, 0, 1, 1, 1}, 
               {0, 1, 0, 1, 0, 1, 0, 1, 0}, 
               {0, 1, 0, 1, 1, 0, 0, 1, 0},
               {1, 1, 0, 1, 0, 0, 0, 1, 0}, 
               {1, 1, 0, 1, 0, 0, 1, 1, 1}};
```

每一个内部初始化器提供了矩阵中一行的值。为高维数组构造初始化器可采用类似的方法。

如果初始化器没有大到足以填满整个多维数组，那么把数组中剩余的元素赋值为 `0`

```c
int m[5][9] = {{1, 1, 1, 1, 1, 0, 1, 1, 1}, 
               {0, 1, 0, 1, 0, 1, 0, 1, 0}, 
               {0, 1, 0, 1, 1, 0, 0, 1, 0}};
```

如果内层的列表没有大到足以填满数组的一行，那么把此行剩余的元素初始化为 0

```c
int m[5][9] = {{1, 1, 1, 1, 1, 0, 1, 1, 1}, 
               {0, 1, 0, 1, 0, 1, 0, 1}, 
               {0, 1, 0, 1, 1, 0, 0, 1}, 
               {1, 1, 0, 1, 0, 0, 0, 1}, 
               {1, 1, 0, 1, 0, 0, 1, 1, 1}};
```

甚至可以省略内层的花括号

```c
int m[5][9] = {1, 1, 1, 1, 1, 0, 1, 1, 1, 
			   0, 1, 0, 1, 0, 1, 0, 1, 0, 
			   0, 1, 0, 1, 1, 0, 0, 1, 0, 
			   1, 1, 0, 1, 0, 0, 0, 1, 0, 
			   1, 1, 0, 1, 0, 0, 1, 1, 1};
```

C99 的指示器对多维数组也有效。有指定值的元素都默认值为 `0`

```c
double ident[2][2] = {[0][0] = 1.0, [1][1] = 1.0};
```

### 常量数组

常量数组就是存放一组不能发生修改的值。例如，定义十六进制字符

```c
const char hex_chars[] = 
  {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 
   'A', 'B', 'C', 'D', 'E', 'F'};
```

程序不应该对声明为 `const` 的数组进行修改，编译器能够检测到直接修改某个元素的意图

## 变长数组

我们强调过，声明数组时，数组的长度必须使用常量表达式。然而，在 C99 标准开始支持 **变长数组**。因此，**数组的长度可以使用非常量表达式**

> [!tip] 
> 
> **变长数组的长度** 不一定要用变量来指定，**任意表达式**（可以含有运算符）都可以
> 

```c
int a[3*i+5]; 
int b[j+k];
```

变长数组常见于除 `main` 函数以外的其他函数。对于函数 `f` 而言，变长数组的最大优势就是每次调用 `f` 时长度可以不同
