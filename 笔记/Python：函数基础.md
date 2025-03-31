# 函数基础

## 函数的定义与调用

下列代码创建了一个计算斐波拉契数列的函数

```python
>>> def fib(n):  # 定义一个函数
...     """Print a Fibonacci series less than n."""
...     a, b = 0, 1
...     while a < n:
...         print(a, end=" ")
...         a, b = b, a + b
...     print()
...
>>> fib(2000)  # 调用函数
0 1 1 2 3 5 8 13 21 34 55 89 144 233 377 610 987 1597
```

在 Python 中，定义函数必须使用关键字 `def`，随后跟着的是 **函数名** 与括号内的 **形参列表**，函数语句从下一行开始，并且 **必须缩进**

函数内的第一条语句是字符串时，该字符串就是 **文档字符串**。利用文档字符串可以自动生成在线文档或打印版文档，还可以让开发者在浏览代码时直接查阅文档；Python 开发者最好养成在代码中加入文档字符串的好习惯。

当函数被调用后，函数开始执行。此时，函数使用它自己的 **局部符号表**，在函数内部的任何赋值语句都会在函数的局部符号表中创建名字。

> [!important] 
> 
> 在引用变量时，符号查找顺序如下
> + 首先，在 局部符号表里查找变量
> + 其次，是外层函数局部符号表(Python 中的函数允许嵌套)
> + 然后，是全局符号表，
> + 最后是 内置名称符号表
> 
> 请注意：与其他语言不同的是，Python 可以引用全局变量和外层函数的变量，但是无法在函数内是无法直接给全局变量或外层函数的局部变量赋值。因为在 Python 中定义变量是通过赋值实现的，如果在函数内部赋值，那么就会在函数的局部符号表中创建一个新名字
> 

### 传递给函数的实参是什么？

调用函数是需要提供一个实参，回忆一下，Python 中的任何符号都只是一个对象的引用，当我们将一个符号作为函数的实参时，本质上就是使用实参对函数形参进行赋值；然而，在 Python 中，赋值仅仅是拷贝对象的引用而不是拷贝对象

> [!tip] 
> 
> 实参传递本质上是对函数形参列表的赋值，在 Python 中，赋值仅仅只是拷贝对象的引用，而不是拷贝对象
> 

在正式验证上面的描述之前，我们要介绍一个内置函数 `id()`，该函数用于返回对象的标识，在 CPython 中，`id` 返回的就是对象的虚拟内存地址

```python
>>> a = 10
>>> id(a)
140728978900168
>>> b = a  # 赋值，仅仅只是将对象的引用拷贝给新变量 b
>>> id(b)
140728978900168
>>> a is b
True
```

下面我们使用 `id()` 函数验证在 Python 传递实参仅仅只是传递对象的引用

```python title:arg_param.py
def arg_param(param):
    print(f"函数内部形式参数 param 的 id: {id(param):#0x}")


arg = 2000
print(f"函数外部的实际参数 real 的 id: {id(arg):#0x}")
arg_param(arg)
```

执行上述脚本的结果为：

```shell
➜ python arg_param.py
函数外部的实际参数 real 的 id: 0x756e4ee4f030
函数内部形式参数 param 的 id: 0x756e4ee4f030
```

我们发现在函数内层和外层输出的 `id()` 的值是一样的。证明了 **函数在传实参是传递的是对象的引用**

### 函数名仅仅只是一个标识符

在 Python 中，函数名和普通标识符没有任何区别，它只是函数对象的引用。我们可以让函数名引用其他类型的对象。例如，我们之前定义的 `fib()` 函数，名字 `fib` 就仅仅只是一个普通的标识符罢了

```python
>>> fib
<function fib at 0x000001E40B873E20>
>>> fib = 20  # 让 fib 引用其他对象
>>> fib
20
```

Python 解释器在读取到关键字 `def` 时，就会在堆空间中创建一个函数对象，并让 `def` 后的名字引用该对象。

```python
>>> def fib(n):
...     """
...     Print a Fibonacci series up to n.
...
...     :param n: end of the Fibonacci series
...     :return: None
...     """
...     a, b = 0, 1
...     while a < n:
...         print(a, end=" ")
...         a, b = b, a + b
...     print()
... 
>>> fib
<function fib at 0x7214ff5b6f20>
>>> f = fib  # 让另一个变量也引用函数对象
>>> f(100)
0 1 1 2 3 5 8 13 21 34 55 89 
```

## 返回值

函数可以使用 `return` 关键字返回值。不过，对于 Python 而言，即使没有 `return` 语句也会返回一个值，这个值是 Python 内建常量 `None`，其类型是 `NoneType`，表示没有值的概念

> [!tip] 
> 
> `None` 表示没有值，其类型是 `NoneType`
> 

一般来说，解释器不会输出单独的返回值 `None` ，如需查看该值，可以使用 `print()`：

```python
>>> def fib(n):
...     """Print a Fibonacci series up to n"""
...     a, b = 0, 1
...     while a < n:
...         print(a, end=" ")
...         a, b = b, a + b
...     print()
...
>>> fib(1)
0            # 这个输出是函数内部的 print 输出的结果
>>> result = fib(1)
0
>>> print(result)
None
```

下面我们修改 `fib()` 函数，让它返回一个列表，元素是小于参数 `n` 的斐波拉契数列

```python
>>> def fib(n):
...     """Calculate a Fibonacci series up to n"""
...     a, b = 0, 1
...     result = []
...     while a < n:
...         result.append(a)
...         a, b = b, a + b
...     return result  # 返回列表的引用
...
>>> fib(10)
[0, 1, 1, 2, 3, 5, 8]
```

## 函数调用

我们已经知道怎样定义和调用函数，下面我们详细介绍 Python 调用函数的语法

在 Python 调用函数最基本的语法是 `函数名(实参列表)`。但是，在 Python 中可以有两种方法给函数提供实参：**位置实参** 和 **关键字实参**

我们使用如下定义的函数来介绍提供实参的两种方式

```python
>>> def ask_ok(prompt, retries, reminder):
...     """测试输入是否成功"""
...     while True:
...         reply = input(prompt)
...         if reply in {'y', 'ye', "yes"}:
...             return True
...         if reply in {'n', 'no', "nop", "nope"}:
...             return False
...         retries -= 1
...         if retries < 0:
...             raise ValueError("invalid user response")
...         print(reminder)
...
```

### 位置实参

函数调用时，函数的形参列表按位置与实参列表进行绑定。例如，如下方式调用 `ask_ok()` 函数

```python
>>> ask_ok('OK to overwrite the file?', 2, 'Come on, only yes or no!')
OK to overwrite the file?
Come on, only yes or no!
OK to overwrite the file?yes
True
```

实参 `'OK to overwrite the file?'` 在实参列表中的位置为 $0$，则绑定在形参列表中位置为 $0$ 的变量 `prompt` 上

实参 `2` 在实参列表的位置为 $1$，则绑定在形参列表中位置为 $1$ 的 `retries` 上

实参 `'Come on, only yes or no!'` 在实参列表中的位置为 $2$，则绑定在形参列表中位置为 $2$ 的 `reminder` 上

### 关键字实参


函数调用时，可以指定接收实参的形参名。例如，如下形式调用 `ask_ok`

```python
ask_ok(retries=2, reminder='Come on, only yes or no!', prompt='OK to overwrite the file?')
```

> [!tip] 
> 
> 由于指定了接收实参的形参名，可以不用在意实参的顺序了
> 

### 位置实参和关键字实参混用

如下函数定义示例

```python
def sum3(x, y, z):
	return x + y + z
```

该函数的调用需要 $3$ 个实参，可以通过位置实参、关键字实参传递

```python
# 位置实参
sum3(1, 2, 3)

# 关键字实参
sum3(y=1, z=2, x=3)
```

甚至可以混用位置实参和关键字实参

```python
# 混用位置实参和关键字实参
sum3(1, z=2, y=3)
```

> [!attention] 
> 
> 函数调用时，_关键字参数_ 必须跟  _在位置参数后面_
> 

请注意：不要对同一个参数多次赋值，所有传递的关键字参数都必须匹配一个函数接受的参数，**关键字参数的顺序并不重要**。**不能对同一个形式参数多次赋值**，下面就是一个因此限制而失败的例子

```python
>>> def function(a):
...     pass
... 
>>> function(0, a=10)
Traceback (most recent call last):
  File "<stdin>", line 1, in <module>
TypeError: function() got multiple values for argument 'a'
```

### 解包实参

当我们用于调用函数的实参在一个容器中时，我们可以从容器中将实参取出来之后才调用函数。这样做显得比较冗余，**Python 支持在函数调用时使用解包操作**

> [!tip] 
> 
> 函数调用要求独立的位置参数，但实参在列表或元组里时，例如，内置的 `range()` 函数求独立的 `start `和 `stop `实参
> 

如果这些参数不是独立的，则要在调用函数时，用 **操作符 `*` 把实参从列表或元组解包出来**

```python
>>> list(range(3, 6))
[3, 4, 5]
>>> args = [3, 6]
>>> list(range(args))  # args 是一个列表，不能解释为整数
Traceback (most recent call last):
  File "<python-input-22>", line 1, in <module>
    list(range(args))  #
         ~~~~~^^^^^^
TypeError: 'list' object cannot be interpreted as an integer
>>> list(range(*args))  #
[3, 4, 5]
```

如果实参在字典中时，也可以使用 操作符 `**` 把字典中的 `key:value` 解包出来，此时就相当于使用关键字参数进行传参

```python
>>> def parrot(voltage, state='a stiff', action='voom', type='Norwegian Blue'):
...     print("-- This parrot wouldn't", action, end=' ')
...     print("if you put", voltage, "volts through it.")
...     print("-- Lovely plumage, the", type)
...     print("-- It's", state, "!")
... 
>>> d = {"voltage": "four million", "state": "bleedin' demised", "action": "VOOM"}
>>> parrot(**d)  # 解包字典实参
-- This parrot wouldn't VOOM if you put four million volts through it.
-- Lovely plumage, the Norwegian Blue
-- It's bleedin' demised !
```

> [!summary] 
> 
> + 操作符 `*` 解构序列对象，按位置传参
> 
> + 操作符 `**` 解构字典，按关键字传参
> 

## 函数定义

函数定义支持可变数量的参数。这里列出三种可以组合使用的形式。

### 默认参数

为参数指定默认值是非常有用的方式。调用函数时，可以使用比定义时更少的参数，例如：

```python
def ask_ok(prompt, retries=4, reminder='Please try again!'):
    while True:
        reply = input(prompt)
        if reply in {'y', 'ye', 'yes'}: 
            return True
        if reply in {'n', 'no', 'nop', 'nope'}:
            return False
        retries = retries - 1
        if retries < 0:
            raise ValueError('invalid user response')
        print(reminder)
```

函数 `ask_ok()` 的调用形式有如下几种

```python
# 只给出必要的实参
ask_ok('Do you really want to quit?')

# 给出一个可选实参
ask_ok('OK to overwrite the file?', 2)

# 给出所有参数
ask_ok('OK to overwrite the file?', 2, 'Come on, only yes or no!')
```

> [!attention] 
> 
> 参数的默认值在定义函数的作用域中求值
> 

```python
>>> i = 5
>>> def f(arg=i):
...     print(arg)
...
>>> i = 6
>>> f()
5
```

> [!warning] 
> 
> 请注意：默认值在函数定义时求值且只求值一次
> 
> 默认值为列表、字典或类实例等可变对象时，会产生与该规则不同的结果
> 

例如，下面的函数会累积后续调用时传递的参数：

```python
>>> def f(a, L=[]):  # 注意，默认值只求值一次
...     L.append(a)
...     return L
...
>>> f(1)
[1]
>>> f(2)
[1, 2]
>>> f(3)
[1, 2, 3]
```

不想在后续调用之间共享默认值时，应以如下方式编写函数

```python
>>> def f(a, L=None):  # 注意，默认值只求值一次
...     if not L:
...         L = []
...     L.append(a)
...     return L
...
>>> f(1)
[1]
>>> f(2)
[2]
>>> f(3)
[3]
```

### 限制实参的传递方式

默认情况下，实参可以使用 **位置实参** 或 **关键字实参** 传递给 Python 函数。为了让代码易读、高效，可以 **限制参数的传递方式**，这样，开发者只需查看函数定义，即可确定参数项是 _仅按位置_、_按位置或关键字_，还是 _仅按关键字_ 传递

函数按如下方式定义

```python
def f(pos1, pos2, /, pos_or_kwd, *, kwd1, kwd2):
      -----------    ----------     ----------
        |             |                  |
        |          位置或关键字            |
        |                                - 仅限关键字
        -- 仅限位置
```

> [!tip] 
> 
> 在形参列表中，如果使用斜线(`/`) 分隔形参，那么在斜线之前的参数就只能通过位置参数传递；这种只能通过位置传递的参数称为 **仅位置参数**
> 
> 在形参列表中，如果使用星号(`*`) 分隔形参，那么在星号之后的参数就只能通过关键字参数传递；这种只能通过关键字传递的参数称为 **仅关键字参数**
> 
> 对于那些介于斜线和星号之间的形参，它们既可以通过位置参数也可以通过关键字参数传递，这些参数称为 **位置或关键字参数**
> 

#### 位置或关键字参数

函数定义中 **没有使用 `/` 和 `*` 时**，或者 **在 `/` 和 `*` 之间** 的参数都可以按位置或关键字传递给函数。

```python
def standard_arg(arg1, arg2):  # 可以位置传递也可以关键字传递
    print(arg1, arg2)


standard_arg(10, 20)  # 10 20
standard_arg(arg1=20, arg2=30)  # 20 30
```

#### 仅位置参数

特定形参可以标记为 **_仅限位置_**。**_仅限位置_** 时，形参的 **顺序** 很重要，且这些形参 **不能用关键字传递**

**仅限位置形参应放在 `/` （正斜杠）前**。`/` 用于在逻辑上分割仅限位置形参与其它形参。如果函数定义中没有 `/`，则表示没有仅限位置形参。`/` 后可以是 **_位置或关键字_** 或 **_仅限关键字_** 形参

```python
def pos_only_arg(pos_arg, /):  # 仅可以通过位置传递
    print(pos_arg)


pos_only_arg(10)
```

> [!attention] 
> 
> 仅位置参数是从 Python 3.8 才开始支持
> 

#### 仅限关键字参数

把形参标记为 **_仅限关键字_**，表明 **必须以关键字参数形式传递该形参**，应在参数列表中第一个 **_仅限关键字_** 形参前添加 `*`。

```python
def kwd_only_arg(*, arg):  # 仅可以通过关键字传递
    print(arg)


kwd_only_arg(arg=20)
```

### 任意实参列表

允许函数接收任意数量的实参是很有用的，就像内置函数 `print()` 一样，它可以接收任意数据的参数

在定义函数时，如果需要让函数接收任意数量的实参，那么可以在最后一个位置参数后面使用 `*args` 的方式作为形参名，这个参数会将额外的位置参数打包为一个元组并通过 `args` 引用这个元组对象

```python
def write_multiple_items(file, separator, *args):
    file.write(separator.join(args))
```

> [!tip] 
> 
> `args` 参数用于采集传递给函数的所有剩余参数，因此， **通常在形参列表中位置参数的末尾**
> 
> 也就是说，如果 `*args` 后面还有其他形参，在调用函数时就只能使用关键字参数进行传递了
> 

```python
def concat(*args, sep="/"):
	return sep.join(args)

# 返回字符串: 'earth/mars/venus'
concat("earth", "mars", "venus")  

# 返回字符串：'earth.mars.venus'
concat("earth", "mars", "venus", sep=".") 
```

此外，**Python 也支持接收任意数量的关键字参数**。通常，在参数列表的末尾使用 `**kwargs` 收集额外的关键字实参，并将这些关键字实参打包为一个字典，通过 `kwargs` 引用这个字典

```python
def function_name(*args, **kwargs):
    pass
```

> [!tip] 
> 
> `*args`:  将接收按位置参数传过来的实参, 并将他们打包成一个元组赋值给`args`
>  
> `**kwargs`: 将接收按默认参数形式(关键字参数)传递过来的变量, 并将他们打包成一个字典赋值给`kwargs`
> 
> 注意：`args` 和 `kwargs` 是约定的名字，这样可以提高程序的可读性
> 

### 文档字符串

函数体的一行如果是一个字符串，那么这个字符串会被 Python 解释器当作 **文档字符串**。文档字符串通常用于对 **函数功能的解释** 和 **参数的说明**

> [!important] 
> 
> 文档字符串如果是多行时，
> + 第一行 **以大写字母开头并以句点结尾**，这是对函数功能的简单描述(摘要)，与字符串开头的引号相邻
> + **第二行应为空白行**，在视觉上将摘要与其余描述分开。
> + 后面的行可包含若干段落，**描述对象的调用约定、副作用等**
> 

```python
def my_function():
    """Do nothing, but document it.

    No, really, it doesn't do anything.
    """
    pass


print(my_function.__doc__)
```

Python 解释器不会删除文档字符串中的缩进。因此，文档处理工具应在必要时删除缩进。这项操作遵循以下约定：**文档字符串第一行之后的第一个非空行决定了整个文档字符串的缩进量**（第一行通常与字符串开头的引号相邻，其缩进在字符串中并不明显，因此，不能用第一行的缩进）。然后，**删除字符串中所有行开头处与此缩进“等价”的空白符**。不能有比此缩进更少的行，但如果出现了缩进更少的行，应删除这些行的所有前导空白符。转化制表符后（通常为 8 个空格），应测试空白符的等效性

### 函数注解

**函数注解** 是可选的 **用户自定义函数类型的元数据完整信息**。详细参考 [函数注释](https://peps.python.org/pep-3107/) 和 [类型提示](https://peps.python.org/pep-0484/)

注解以字典的形式存放在函数的 `__annotations__` 属性中，注解仅仅只是提示函数的使用者，不会影响函数的其他部分

**形参注解** 的定义方式是在 **形参名后加冒号**，后面跟一个会被求值为注解的值的表达式。**返回值注解** 的定义方式是使用组合符号 `->`，后面跟一个表达式，这样的注解位于形参列表和表示 `def` 语句结束的冒号

下面的示例有一个必选参数、一个可选的关键字参数以及返回值都带有相应的标注:

```python
def f(ham: str, eggs: str = 'eggs') -> str:
    print("Annotations:", f.__annotations__)
    print("Arguments:", ham, eggs)
    return ham + ' and ' + eggs
```

## 作用域








