# 基础语法

## 注释

在计算机语言中，**注释** 是计算机语言的一个 **重要组成部分**，用于 **在源代码中解释代码的功用**，可以 **增强程序的可读性，可维护性**，或者 **用于在源代码中处理不需运行的代码段**，来调试程序的功能执行

> [!tip] Python 只提供了：行注释
> 
> 行注释使用 `#` 开头，直到物理行结束
>
 
```python
# 这是一行注释
```

## 数值类型

Python 解释器像一个简单计算器。我们可以输入一个表达式，它会给出表达式的结果

```python
>>> 2 + 2
4
>>> 50 - 5 * 6
20
>>> 8 / 5
1.6
>>>
```

与 C 语言类似，Python 内置了两种数值类型：整数类型和浮点数类型

> [!tip] 
> 
> Python 中只有一种整数类型和浮点数类型
> 

### 整数类型

在上述例子中，形如 `2` `5` `6` `8` `50` 等字面值称为整数，其类型为 `int` 类型。在 Python 中，整数字面值支持 **十进制** **二进制** **八进制** 和 **十六进制**

> [!tip] 
> 
> 与其他语言不同的是，Python 中的整数是没有尺寸限制的，也就是其位模式不限制长度
> 

> [!important] 字面值
> 
> 在程序设计语言中，字面值是一个 **符号**，**它本身就是值**，而不是值的引用
> 
> **字面值指在程序中出现且无法更改的值**。只能从它的值来谈论它
> 

在 Python 中创建 `int` 类型的对象有两种方法：**字面值** 和 **构造函数**

> [!tip] 字面值
> 
> 我们只需要按照整数字面值词法书写字面值，Python 就会构建对应值的整数类型对象
> 

```python
>>> 10, type(10)  # 十进制整数字面值，内建函数 type() 用于获取对象的类型
(10, <class 'int'>)
>>> 0b1010, type(0b1010)  # 二进制整数字面值
(10, <class 'int'>)
>>> 0o12, type(0o12)  # 八进制整数字面值
(10, <class 'int'>)
>>> 0x10, type(0x10)  # 十六进制整数字面值
(16, <class 'int'>)
```

> [!tip] 构造函数：`int(string, /,base=10)` 或者 `int(number=0)`
> 
> 构造函数 `int()` 从一个数字或字符串构建的整数对象，或者如果未给出参数则返回 `0`
> 

```python
>>> int(3.12)
3
>>> int("1023")
1023
>>> int("aaff")
Traceback (most recent call last):
  File "<python-input-10>", line 1, in <module>
    int("aaff")
    ~~~^^^^^^^^
ValueError: invalid literal for int() with base 10: 'aaff'
>>> int("aaff", base=16)
43775
```

> [!attention] 
> 
> 如果 `int()` 的第一个参数是字符串，那么字符串必须符合 `base` 指定的进制
> 
> 一个以 $n$ 为基数的整数字符串包含多个数位，每个数位代表从 $0$ 到 $n-1$ 范围内的值。 $0 \sim 9$  的值可以用任何 Unicode 十进制数码来表示。 $10 \sim 35$ 的值可以用 `a` 到 `z` (或 `A` 到 `Z`) 来表示。 默认的 `base` 为 $10$。
> 

### 浮点数类型

形如 `1.6` `.6` `2e1` 等字面值称为小数，其类型为 `float` 类型。Python 中浮点数的字面值表示为 **十进制小数指定**，并且可以附加字符 `e` 或 `E` 后跟正整数或负整数来指定 **科学计数法**

> [!tip] 
> 
> Python 中的浮点数采用 IEEE 754 双精度浮点数表示。因此，浮点数可以具有的最大值约为 $1.8 \times 10^{308}$
> 
> 对于任何大于  $1.8 \times 10^{308}$ 值的数，Python 使用 `inf` 表示
> 
> ```python
> >>> 1.79e308
> 1.79e+308
> >>> 1.8e308
> inf
> ```
> 
> 非零数最接近零的值约为 $5.0 \times 10^{-324}$ 。任何比该值更接近零的值实际上都是零
> 

在 Python 中可以构造 `float` 类型对象的方式有 $2$ 种：**字面值** 和 **构造函数**

使用字面值构造 `float` 类型对象的

```python
>>> 1.0, type(1.0)
(1.0, <class 'float'>)
>>> .1, type(.1)
(0.1, <class 'float'>)
>>> 1., type(1.)
(1.0, <class 'float'>)
>>> 2e1, type(2e1)
(20.0, <class 'float'>)
```

> [!tip] 构造函数: `float(number=0.0)` 或 `float(string)`
> 
> 构造函数 `float()` 基于一个数字或字符串构建的浮点数
> 

```python
>>> float('+1.23')
1.23
>>> float('   -12345\n')
-12345.0
>>> float('1e-003')
0.001
>>> float('+1E6')
1000000.0
>>> float('-Infinity')
-inf
```

> [!attention] 
> 
> 如果 `float()` 的参数是字符串时，则字符串必须符合的字符串字面值
> 

### 布尔类型

布尔类型表示逻辑值的类型。在 Python 中，布尔类型是整数类型的子类型，但是布尔类型只有 `True` 和 `False` 两个值

> [!tip] 
> 
> `True` 和 `False` 是 Python 的内置常量，其类型是 `bool` 类型
> 
> + `True` 默认的 `int` 类型值为 $1$
> + `False` 默认的 `int` 类型值为 $0$ 
> 

在 Python 中，通过构造函数 `bool()` 可以使用任何类型的对象创建一个 `bool` 类型的对象

```python
>>> bool(0)
False
>>> bool(0.0)
False
>>> bool("")
False
>>> bool(209)
True
>>> bool(0.01)
True
>>> bool("a")
True
```

### 复数类型

Python 内建复数类型(`complex` 类型)，只是该类型不常用。Python 中复数类型的字面值为 `<real part> + <imaginary part>j`

在 Python 中可以使用字面值构建 `complex` 类型的对象，也可以使用构造 `complex(real, imag)` 

```python
>>> 2 + 3j, type(2 + 3j)
((2+3j), <class 'complex'>)
>>> complex(2, 3)
(2+3j)
```

## 变量

与 C 语言不同的是，**Python 中的变量在 _首次赋值的时定义_**，也就是说，Python 中的变量通过赋值定义的

```python
>>> name = "杜宇鹏"  # 定义变量 name
>>> age = 28  # 定义变量 age
```

> [!tip] 
> 
> 由 `"..."` 包围的字面值是字符串，其类型为 `str`
> 

Python 中的变量本质上是一个引用，类似与 C 语言中的指针变量

![[Pasted image 20250108181852.png]]

> [!tip] 
> 
> Python 中的变量可以理解为一个标签，可以贴在任何对象上。也就是说，Python 中的变量是没有类型的，而 **变量引用的对象才具有类型**
> 

Python 中的一个 **变量名** 是用来指代一个对象，但是 **它不保存对象值**，而是 **存储对象的引用**(即存储对象的地址)，变量名需要满足一些 **命名规则

> [!attention] 命名规则
> 
> + **必须** 以一个 **字母(Unicode字母)** 或 **下划线** 开头。后面可以跟任意数量的字母、数字或下划线
> + **区分大小写**：大写字母和小写字母是不同的。例如，`heapSort` 和 `Heapsort` 是两个不同的名字
> + **不要使用 Python 的关键字**
> + 尽量 **不要使用预定义名字(内置函数，内置类等)**
> 

```python
>>> name = "杜宇鹏"
>>> age = 28
>>> _heap = "heap"
>>> 1ab = 2  # 数字开头的标识符是错误的
  File "<python-input-28>", line 1
    1ab = 2
    ^
SyntaxError: invalid decimal literal
>>> def = 3  # def 是 Python 的关键字
  File "<python-input-29>", line 1
    def = 3
        ^
SyntaxError: invalid syntax
>>> int = 19  # 内置名称虽然可以被使用，但是可能会造成程序混乱
>>> int("10")
Traceback (most recent call last):
  File "<stdin>", line 1, in <module>
TypeError: 'int' object is not callable
```

## 常量

在 Python 中，我们没有任何办法定义常量。只能采用约定：标识符全部大写指示不能修改该标识符引用的对象

```python
>>> RED = 0xFF0000
>>> GREEN = 0x00FF00
>>> BLUE = 0x0000FF
```

Python 中真正意义上的常量只有 _字面值_ 和内置的 `True` `False` `None`

## 字符串

除了数值外，Python 还可以处理文本，即 **字符串**。字符串字面值用成对的 **单引号 (`'...'`)** 或 **双引号 (`"..."`)** 来表示

> [!attention] 
> 
> 请注意，Python 中没有字符类型。在 Python 中，`'a'` 是长度为 $1$ 的字符串
> 

```python
>>> 'spam eggs' # 单引号
'spam eggs'
>>> "Paris rabbits got your back :)! Yay!" # 双引号
'Paris rabbits got your back :)! Yay!'
>>> '1975' # 带引号的数字也是字符串
'1975'
```

如果希望字符串中包含字符 `'` 和 `"`，则可以使用 **转义符(`\`)** 或者 **使用不同的类型的引号**

```python
>>> 'doesn\'t'  # 使用 \' 转义单引号
"doesn't"
>>> "doesn't"  # 使用不同类型的引号
"doesn't"
```

> [!tip] 
> 
> 使用不同类型的引号可以是字符串字面值的表示更加清晰
> 

在 Python shell 中，字符串定义和输出字符串看起来可能不同。 `print()`函数会略去标示用的引号，并打印经过转义的特殊字符，产生更为易读的输出:

```python
>>> s = 'First line.\nSecond line.'  # \n 表示 newline 即换行
>>> s
'First line.\nSecond line.'
>>> print(s)
First line.
Second line.
```

### 转义字符序列

Python 支持下表列出转义序列。

|      转义字符      |                  解释                   |
| :------------: | :-----------------------------------: |
|   **字符转义序列**   |                                       |
|     `'\a'`     |          ASCII `BEL` 字符(响铃)           |
|     `'\b'`     |           ASCII `BS` 字符(退格)           |
|     `'\f'`     |           ASCII `FF` 字符(换页)           |
|     `'\n'`     |           ASCII `LF` 字符(换行)           |
|     `'\r'`     |          ASCII `CR` 字符(回车符)           |
|     `'\t'`     |         ASCII `TAB` 字符(水平制表符)         |
|     `'\v'`     |         ASCII `VT` 字符(垂直制表符)          |
|      `\\`      |               表示反斜杠`\`                |
|                |                                       |
|   **数字转义序列**   |                                       |
|   `'\uxxxx'`   |   Unicode $16$ 位编码为十六进制数 `xxxx` 的字符   |
| `'\Uxxxxxxxx'` | Unicode $32$ 位编码为十六进制数 `xxxxxxxx` 的字符 |
|    `'\ooo'`    |            编码为八进制值`ooo`的字符            |
|    `'\xhh'`    |            编码为十六进制`hh` 的字符            |

### 原始字符串

有时候不希望 `\` 将的字符转义成特殊的字符，处理可以在 `\` 前面再加上 `\` 取消转义行为。此外，还可以使用原始字符串，即 `r"..."`

```python
>>> "c:\windows\nt"  # 这里 \n 表示换行
<stdin>:1: SyntaxWarning: invalid escape sequence '\w'
'c:\\windows\nt'  # \\ 让 \ 不在具有转义的含义
>>> print("c:\windows\nt")
<stdin>:1: SyntaxWarning: invalid escape sequence '\w'
c:\windows
t
>>> print(r'c:\windows\nt')  # r前缀将使得字符串中的转义字符失去转义效果
c:\windows\nt
```

> [!attention] 
> 
> 原始字符串还有一个微妙的限制：**一个原始字符串不能以奇数个 `\` 字符结束**
> 

### 跨行字符串

**字符串字面值可以包含多行**。 一种实现方式是使用 **三重引号**：`"""..."""` 或 `'''...'''`。 字符串中 **将自动包括行结束符**，但也可以在换行的地方添加一个 **续行符**(\\) 来避免此情况。 参见以下示例：

```python
>>> print("""\
... Usage: thingy [OPTIONS]
...      -h                        Display this usage message
...      -H hostname               Hostname to connect to
... """)
Usage: thingy [OPTIONS]
     -h                        Display this usage message
     -H hostname               Hostname to connect to
>>> print("""  # 这个换行符被包括在字符串内
... Usage: thingy [OPTIONS]
...      -h                        Display this usage message
...      -H hostname               Hostname to connect to
... """)

Usage: thingy [OPTIONS]
     -h                        Display this usage message
     -H hostname               Hostname to connect to
```

### 字符串拼接

Python 中字符串可以使用运算符 `+` 和 `*` 进行拼接

> [!tip] 
> 
> 运算符 `+` 将两个字符串拼接起来
> 
> 运算符 `*` 将字符串重复拼接 $n$ 次
>  

```python
>>> 3 * "un" + "ium"  # un 出现三次后，出现 ium
'unununium'
```

对于相邻的字符串字面值，Python 解释器会自动将其合并为一个字符串

```python
>>> "Py" "thon"
'Python'
>>> text = ('Put several strings within parentheses '
...         'to have them joined together.')
>>> text
'Put several strings within parentheses to have them joined together.'
```

> [!attention] 
> 
> 请注意：只有相邻的两个字符串字面值才会被 Python 解释器自动拼接
> 

合并多个变量，或合并变量与字面值，要用 `+`

```python
>>> prefix + 'thon'
'Python'
```

> [!important] 
> 
> 字符串是不可变对象。运算符 `+` 和 `*` 操作字符串是，不会修改原始字符串对象，而是会产生一个全新的字符串对象
> 

### 从标准输入中读取字符串

为了从标准输入中获取数据，Python 提供了 `input(prompt)` 完成此项功能

> [!tip] 
> 
> 如果调用 `input()` 函数时提供了参数 `prompt`，则 `input()` 函数会先向标准输出写入 `prompt`，然后再从标准输入中读取字符
> 

```python
>>> s = input("-->")
--> 你好
>>> s
' 你好'
>>> type(s)
<class 'str'>
```

> [!attention] 
> 
> 函数 `input()` 返回的是从标准输入中读取都的字符串。如果需要
> 

## 运算符

### 算术运算符

Python 中的算术运算符用于执行常规算术运算符。下表列出了 Python 中所有的算术运算符

| 运算符      | 描述                   |
| :------- | :------------------- |
| `x + y`  | 返回 `x` 与 `y` 的和      |
| `x - y`  | 返回 `x` 与 `y` 的差      |
| `x * y`  | 返回 `x` 与 `y` 的积      |
| `x / y`  | 返回 `x` 与 `y` 的商      |
| `x // y` | 返回 `x` 与 `y` 的商的整数部分 |
| `x % y`  | 返回 `x - (x // y)`    |
| `x ** y` | 返回 `x` 的 `y` 次幂      |

> [!attention] 
> 
> 运算符 `//` 也称为整数除法。其返回值的类型不一定是 `int`，随操作数的类型确定
> 

### 比较运算符

在 Python 中有 **十种比较运算符**。 它们的 **优先级相同**（比逻辑运算符的优先级高）

| 运算符          | 描述                                             |
| :----------- | :--------------------------------------------- |
| `x < y`      | 如果 `x` 的值小于 `y` 的值，则返回 `True`。否则，返回 `False`    |
| `x <= y`     | 如果 `x` 的值小于或等于 `y` 的值，则返回 `True`。否则，返回 `False` |
| `x > y`      | 如果 `x` 的值大于 `y` 的值，则返回 `True`。否则，返回 `False`    |
| `x >= y`     | 如果 `x` 的值大于或等于 `y` 的值，则返回 `True`。否则，返回 `False` |
| `x == y`     | 如果 `x` 的值等于 `y` 的值，则返回 `True`。否则，返回 `False`    |
| `x is y`     | 返回 `id(x) == id(y)`                            |
| `x is not y` | 返回 `id(x) != id(y)`                            |
| `x in y`     | 如果 `x` 在容器 `y` 中，则返回 `True`。否则，返回 `False`      |
| `x not in y` | 如果 `x` 不在容器 `y` 中，则返回 `True`。否则，返回 `False`     |

### 逻辑运算符

Python 提供下表列出 $3$ 种逻辑运算符。这些运算符的优先级从高到低排列

| 运算符               | 描述                                              |
| :---------------- | :---------------------------------------------- |
| `not expr`        | 如果 `expr` 的值为 `True`，则返回 `False`；否则返回 `True`    |
| `expr1 and expr2` | 如果 `expr1` 的值为 `False`，则返回 `expr1`；否则返回 `expr2` |
| `expr1 or expr2`  | 如果 `expr1` 的值为 `True`，则返回 `expr1`；否则返回 `expr2`  |

> [!tip] `and` 和 `or` 的短路特性
> 
> 按 **从左到右** 的顺序，依次 **测试表达式的值**。只要 **当前测试的表达式** 能够确定整个逻辑表达式的值，则 **该表达式的值作为整个表达式的结果**
> 

```python
>>> not True
False
>>> not False
True
>>> "a" and 10
10
>>> 0 and 10
0
>>> "a" and False
False
>>> "a" or False  # "a" 的逻辑值为　True。因此，确定了表达式 "a" or False 的逻辑值 True, 从而该表达式的值为 "a" 
'a'
```

### 位运算符

Python 提供了与 C 语言相同的位运算符

| 运算符    | 含义         |
| ------ | ---------- |
| `<<`   | 左移         |
| `>>`   | 右移         |
|        |            |
| $\sim$ | 按位取反 `NOT` |
| `&`    | 按位与 `AND`  |
| `^`    | 按位异或 `XOR` |
| &#124  | 按位或 `OR`   |

> [!tip] 
> 
> 上述 $6$ 个运算符只能用于整数类型
> 

位运算符的优先级全都 **低于数字运算**，但又 **高于比较运算**；一元运算 `~` 具有与其他一元算术运算 (`+` 和 `-`) 相同的优先级

按位或`|`: 两个位，只要有一位为 $1$，那么结果就是 $1$

```python
a = 0b10001
b = 0b01010

print(a | b)  # 0b11011 ==> 27
```

 按位与 `&` : 两个位都为 $1$，那么结果才是 $1$
 
```python
a = 0b10001
b = 0b01010

print(a & b)  # 0b00000 ==> 0
```

 按位异或 `^` : 两个位相同，那么结果就是 $0$

```python
a = 0b10001
b = 0b01010

print(a ^ b)  # 0b11011 ==> 27
```

按位取反 `~` : $0$ 变 $1$，$1$ 变 $0$

```python
a = 0b10001
"""
python 中整数的长度是无限的，并采用补码表示

a = 0b10001 ==> 0000..000|10001 
~a = 1111..111|01110 ==> -18
"""
print(~a)  # 101110 ==> -18
```

 左移 `<<` : 将一个数的二进制码向左移动指定的位数。`x << n` 等价 `x * 2 ** n`

```python
a = 0b10001
"""
a = 0b10001 => 2^5 + 1 = 32 + 1 = 33
a << 2 = 0b1000100 => 2^6 + 2^2 = 64 + 4 = 68
"""
print(a << 2)  # 0b1000100 ==> 40
```

右移 `>>` : 将一个数的二进制码向右移动指定的位数。`x >> n` 等价于 `x / 2 ** n`

```python
a = 0b10001
"""
a = 0b10001 => 2^5 + 1 = 32 + 1 = 33
a >> 2 = 0b00100 => 2^2 = 4
"""
print(a >> 2)  # 0b00100 ==> 4
```

### 增强赋值运算符

将赋值运算符和算术运算符以及位运算符组合之后得到的增强赋值运算符

|    运算符     |         结果         |
| :--------: | :----------------: |
|  `x += y`  |  等价于 `x = x + y`   |
|  `x -= y`  |  等价于 `x = x - y`   |
|  `x *= y`  |  等价于 `x = x * y`   |
|  `x /= y`  |  等价于 `x = x / y`   |
| `x //= y`  |  等价于 `x = x // y`  |
| `x **= y`  |  等价于 `x = x ** y`  |
| x &#124= y | 等价于 x = x  &#124 y |
|  `x &= y`  |  等价于 `x = x & y`   |
|  `x ^= y`  |  等价于 `x = x ^ y`   |
| `x <<= n`  |  等价于 `x = x << n`  |
| `x >>= n`  |  等价于 `x = x >> n`  |

### 运算符的优先级

下表对 Python 中运算符的优先顺序进行了总结，从最高优先级（最先绑定）到最低优先级（最后绑定）。 相同单元格内的运算符具有相同优先级。 除非语法显式地指明，否则运算符均为双目运算符。 相同单元格内的运算符从左至右组合的（只有 **幂运算符是从右至左组合的**）。

|                                 运算符                                  | 描述                         |
| :------------------------------------------------------------------: | :------------------------- |
| `(expressions...),[expressions...],{key: value...},{expressions...}` | 绑定或加圆括号的表达式，列表显示，字典显示，集合显示 |
|    `x[index]`, `x[index:index]`, `x(arguments...)`, `x.attribute`    | 索引，切片，调用，属性引用              |
|                              `await x`                               | `await` 表达式                |
|                                 `**`                                 | 乘方                         |
|                             `+x, -x, ~x`                             | 正，负，按位非                    |
|                           `*, @, /, //, %`                           | 乘，矩阵乘，除，整除，取余              |
|                                `+, -`                                | 加和减                        |
|                               `<<, >>`                               | 移位                         |
|                                 `&`                                  | 按位与                        |
|                                 `^`                                  | 按位异或                       |
|                                &#124                                 | 按位或                        |
|            `in, not in, is, is not, <, <=, >, >=, !=, ==`            | 比较运算，包括成员检测和标识号检测          |
|                                `not`                                 | 布尔逻辑非                      |
|                                `and`                                 | 布尔逻辑与                      |
|                                 `or`                                 | 布尔逻辑或                      |
|                             `if -- else`                             | 条件表达式                      |
|                               `lambda`                               | `lambda` 表达式               |
|                                 `:=`                                 | 赋值表达式                      |

## 控制流程

### if 语句

`if` 语句通过对一个表达式进行逻辑值检测，根据表达式的逻辑值选择一组语句执行

```python
>>> x = int(input("--> "))
--> 42
>>> if x < 0:
...     x = 0
...     print("负数被改变为 0")
... elif x == 0:
...     print("零")
... elif x == 1:
...     print("一")
... else:
...     print("其他")
...
其他
```

`if` 语句的完整语法如下

```python
if <condition expression>:
	<statement>
elif <condition expression>:
	<statement>
...
else:
	<statement>
```

> [!attention] 
> 
> `elif` 和 `else` 子句都是可选子句。其中 `elif` 子句是 `else if` 的缩写，用于避免过多的缩进
> 
> `if ... elif ... elif ... else` 相当于其他语言中的 `switch ... case` 语句
> 

> [!tip] 
> 
> Pyhon 使用 `<Tab>` 对语句进行分组。**连续的具有相同缩进的语句称为一个程序块**
> 

例如，在 Python 中，如果一个分支需要执行多条语句，就需要使这些语句保持相同的 **缩进**

```python hl:2-5
if <condition expression>:
    <statement>
    <statement>
    ...
    <statement>
<following statement>
```

高亮部分的语句缩进相同，因此这些语句被设为同一块。当 `<condition expression>` 的逻辑值为 `True` 时，则会执行整个块。否则，跳过整个块。无论如何，都会执行 `<following statement>`

#### 条件表达式

Python 支持一种额外的决策实体，称为条件表达式。最简单的条件表达式的语法如下：

```python
<expr1> if <condition> else <expr2>
```


首先对 `<condition>` 进行逻辑值检测 。如果为 `True`，则表达式的计算结果为 `<expr1>` 。如果为 `False`，则表达式的计算结果为 `<expr2>` 。即，**首先计算中间的表达式，然后根据该结果返回两端的表达式之一**

条件表达式本质上是  `if-else` 语句的简化

```python
if <condition>:
	<expr1>
else:
	<expr2>
```

**条件表达式的行为在语法上类似于表达式**。它 **可以用作较长表达式的一部分**。条件表达式的优先级实际上低于所有其他运算符，因此需要括号将其单独分组

```python
>>> x = y = 40
>>> z = 1 + x if x > y else y + 2
>>> z
42
>>> m = (1 + x) if x > y else (y + 2)
>>> m
42
```

如果您希望首先计算条件表达式，则需要用分组括号将其括起来

```python
>>> z = 1 + (x if x > y else y) + 2
>>> z
43
```

#### 占位语句 pass

**由于 Python 使用缩进而不是分隔符，因此不可能指定空块**。如果您用 `if <condition>:` 引入 `if` 语句，则必须在其后面添加一些内容，可以在同一行上，也可以在下一行缩进

```python title:foo.py
if True:
	
print("foo")
```

如果尝试运行该脚本 ，你会得到

```shell
➜ python foo.py
  File "foo.py", line 3
    print('foo')
        ^
IndentationError: expected an indented block
```

Python `pass` 语句解决了这个问题。它根本不会改变程序的行为。它被用作占位符，以使解释器在语法上需要语句的任何情况下都满意，但你实际上不想做任何事情：

```python title:foo.py
if True:
	pass
print("foo")
```

### while 语句

**while 语句用于在表达式保持为真的情况下重复地执行**。例如，我们可以像下面这样写出 **斐波那契数列** 初始部分的子序列

> [!tip] 斐波拉契数列
> 
> 斐波拉契数列：第$0$ 项为 $0$，第 $1$ 项为 $1$，从第 $2$ 项开始为前两项的和
> $$ f(n) = \begin{cases} 0 & i=0\\ 1 & i = 1 \\ f(n-2) + f(n-1) & i \ge 2 \end{cases} $$
> 

```python title:fib.py
a = 0
b = 1
while a < 10:
    print(b)
    a, b = b, a + b
```

执行该脚本的输出结果为

```shell
➜ python fib.py
1
1
2
3
5
8
13
```

> [!summary] 
> 
> `while` 循环只要条件（这里是 `a < 10`）为真就会一直执行
> 
> 注意语句 `a, b = b, a + b` 是一个多重赋值语句。等号右边的所有表达式的值，都是在这一语句对任何变量赋新值之前求出来的
> 

`while` 例程：一张纸厚道为 $0.01$ mm，对折多少次可以超过珠穆朗玛峰 $8884.84$ m

```python title:while.py
"""
一张纸厚道为 0.01 mm，对折多少次可以超过珠穆朗玛峰 8884.84 m
"""

HEIGHT = 8884.84E3  # 珠穆朗玛峰的高度，单位以转换为毫米

count = 0  # 统计折叠次数

thickness = 0.01
while thickness < HEIGHT:
    thickness *= 2
    count += 1
    print(f"第 {count} 次折叠后纸的厚度为 {thickness}")

print(f"对折 {count} 次后，纸的厚度为 {thickness} 超过珠穆朗玛峰的高度{HEIGHT}")
```


执行该脚本的结果为：

```shell
➜ python while.py
第 1 次折叠后纸的厚度为 0.02
第 2 次折叠后纸的厚度为 0.04
第 3 次折叠后纸的厚度为 0.08
第 4 次折叠后纸的厚度为 0.16
第 5 次折叠后纸的厚度为 0.32
第 6 次折叠后纸的厚度为 0.64
第 7 次折叠后纸的厚度为 1.28
第 8 次折叠后纸的厚度为 2.56
第 9 次折叠后纸的厚度为 5.12
第 10 次折叠后纸的厚度为 10.24
第 11 次折叠后纸的厚度为 20.48
第 12 次折叠后纸的厚度为 40.96
第 13 次折叠后纸的厚度为 81.92
第 14 次折叠后纸的厚度为 163.84
第 15 次折叠后纸的厚度为 327.68
第 16 次折叠后纸的厚度为 655.36
第 17 次折叠后纸的厚度为 1310.72
第 18 次折叠后纸的厚度为 2621.44
第 19 次折叠后纸的厚度为 5242.88
第 20 次折叠后纸的厚度为 10485.76
第 21 次折叠后纸的厚度为 20971.52
第 22 次折叠后纸的厚度为 41943.04
第 23 次折叠后纸的厚度为 83886.08
第 24 次折叠后纸的厚度为 167772.16
第 25 次折叠后纸的厚度为 335544.32
第 26 次折叠后纸的厚度为 671088.64
第 27 次折叠后纸的厚度为 1342177.28
第 28 次折叠后纸的厚度为 2684354.56
第 29 次折叠后纸的厚度为 5368709.12
第 30 次折叠后纸的厚度为 10737418.24
对折 30 次后，纸的厚度为 10737418.24 超过珠穆朗玛峰的高度8884840.0
```

### for 语句

Python 的 `for` 语句不给予用户定义迭代步骤和结束条件的能力，而是 **在 _可迭代对象_ 上，按它们在序列中出现的顺序迭代**。目前，我们学习过的可迭代对象就只有 字符串(`str`) 一种

```python
>>> for char in "hello, world!":
...     print(char)
...
h
e
l
l
o
,

w
o
r
l
d
!
```

#### range 对象

内置函数 `range([start:int, ]end:int, [step:int])` 将产生一个 `range` 对象，它是一个可迭代对象，该对象中会生成根据 `start, end, step` 生成一个首项为 `start`，公差为 `step` 等差序列

> [!tip] 参数 `start` 和 `step` 可以省略
> 
> + `start` 的默认值是 $0$
> + `step` 的默认值是 $1$
> 

```python
>>> for i in range(1, 21, 2):
...     print(i**2)
...
1
9
25
49
81
121
169
225
289
361
```

#### enumerate 对象

内置函数 `enumerate(iterable, start=0)` 将返回一个可迭代对象。 `iterable` 中的每一项都将一个整数值关联。对 `enumerate` 对象迭代取值时，将返回一个元组 `(i, iterable[j])`

```python
>>> for i in enumerate("hello, world!"):
...     print(i)
...
(0, 'h')
(1, 'e')
(2, 'l')
(3, 'l')
(4, 'o')
(5, ',')
(6, ' ')
(7, 'w')
(8, 'o')
(9, 'r')
(10, 'l')
(11, 'd')
(12, '!')
```

### break 和 continue 语句

**break 语句将跳出 _最近的一层_ for  或 while 循环**

```python
>>> for n in range(2, 10):
...     for x in range(2, n):
...         if n % x == 0:
...             print(f"{n} equals {x} * {n//x}")
...             break
...
4 equals 2 * 2
6 equals 2 * 3
8 equals 2 * 4
9 equals 3 * 3
```

**continue 语句将继续执行循环的下一次迭代**

```python
>>> for num in range(2, 10):
...     if num % 2 == 0:
...         print(f"Found an even number {num}")
...         continue
...     print(f"Found an odd number {num}")
...
Found an even number 2
Found an odd number 3
Found an even number 4
Found an odd number 5
Found an even number 6
Found an odd number 7
Found an even number 8
Found an odd number 9
```

### 循环的 `else` 子句

在 `for` 或 `while` 循环中 `break` 语句可能对应一个 `else` 子句。 如果循环在未执行 `break` 的情况下结束，`else` 子句将会执行。

```python
>>> for n in range(2, 10):
...     for x in range(2, n):
...         if n % x == 0:
...             print(n, 'equals', x, '*', n//x)
...             break
...     else:
...         # 循环到底未找到一个因数
...         print(n, 'is a prime number')
...
2 is a prime number
3 is a prime number
4 equals 2 * 2
5 is a prime number
6 equals 2 * 3
7 is a prime number
8 equals 2 * 4
9 equals 3 * 3
```

> [!tip] 
> 
> 在 `for` 循环中，`else` 子句会在循环结束其他最后一次迭代之后，即未执行 `break` 的情况下被执行
> 
> 在 `while` 循环中，它会在循环条件变为假值后执行
> 
> 无论 `for` 循环还是 `while` 循环，当在循环被 `break` 终结时 `else` 子句 **不会** 被执行
> 
