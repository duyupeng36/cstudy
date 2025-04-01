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

## 名字空间与作用域

### 名字空间概念

Python 中的 **namespace （名字空间）** 是 **从名称到对象的映射**。通常使用 Python 的 **字典** 实现（查询名字只需要消耗 $O(1)$ 时间），但除非涉及到性能优化，我们一般不会关注这方面的事情，而且将来也可能会改变这种方式

Python 程序的名字空间是静态创建，但是被动态使用的。任何时刻都至少有 $3$ 个名字空间嵌套：最外层的名字空间是 **内置名字空间**，其次是 **模块全局名字空间**，然后是函数或类内部的 **局部名字空间**。如下图所示，


![[Pasted image 20240330160418.png]]

此外，**对象的属性集合也是命名空间的一种形式**

> [!important] 
> 
> 关于命名空间的一个重要知识点是，**不同命名空间中的名称之间绝对没有关系**；例如，两个不同的模块都可以定义 `maximize` 函数，且不会造成混淆。用户使用函数时必须要在函数名前面加上模块名。
> 

命名空间是在 **不同时刻创建的，且拥有不同的生命周期**
+ **内置名称空间**：在 Python _解释器启动时创建_，并且永远不会删除
+ **模块的全局命名空间**：在 _读取模块定义时创建_，模块的命名空间也会持续到解释器退出
+ 从脚本文件读取或交互式读取的，由解释器顶层调用执行的语句是 `__main__` 模块调用的一部分，也拥有自己的 **全局命名空间**
+ **函数的局部命名空间**：在函数 _被调用时被创建_，并在函数返回或抛出未在函数内被处理的异常时，被删除。当然，每次递归调用都有自己的局部命名空间

### 作用域

一个命名空间的 _作用域_ 是 Python 代码中的 **一段文本区域**，从这个区域 **可直接访问该命名空间**。“可直接访问”的意思是，该文本区域内的 **名称在被非限定引用时，查找名称的范围**，是包括该命名空间在内的。

**作用域** 虽然是被 **静态确定** 的，但会 **被动态使用**。执行期间的任何时刻，都会有 $3$ 或 $4$ 个“命名空间可直接访问”的嵌套作用域，如下图所示

![[Pasted image 20250401230214.png]]

- **最内层作用域**，包含局部名称，并 **首先在其中进行搜索**
- 那些 **外层闭包函数的作用域**，包含 **“非局部、非全局”** 的名称，从最靠内层的那个作用域开始，**逐层向外搜索**
- **倒数第二层作用域**，包含当前模块的 **全局名称**
- **最外层（最后搜索）的作用域**，是 **内置名称的命名空间**


> [!important] 
> 
> 划重点，**作用域是按字面文本确定的**：**模块内定义的函数的全局作用域就是该模块的命名空间，无论该函数从什么地方或以什么别名被调用**
> 
> 另一方面，**实际的名称搜索是在运行时动态完成的**。但是，Python 正在朝着 “编译时静态名称解析”的方向发展，因此不要过于依赖动态名称解析！（局部变量已经是被静态确定了。）

## 函数嵌套定义

在 Python 中，函数支持嵌套定义的：即，在函数内部定义另一个函数。

```python
o = 49
def outer():  # outer 是一个标识符，属于全局名称空间

    o = 65  # o 是一个标识符，属于 outer 的局部名称空间
    
    def inner(): # inner 也是一个标识符，属于 outer 的局部名称空间

        print(f"{o} in inner", chr(o))  # chr 是一个标识符，属于内建名称空间
        # 这里访问的 o 在 inner 的局部名称空间中找不到，所以会去 outer 的局部名称空间中找
        
        # 这里访问的 chr 在 inner 的局部名称空间中找不到，所以会去 outer 的局部名称空间中找，
        # chr 在 outer 的局部名称空间中找不到，所以会去全局名称空间中找，
        # 此时，chr 还没有查询到，所以会去内建名称空间中找，找到了 chr，所以可以正常执行
        # 如果此过程中没有找到 chr，那么会报错 NameError: name 'chr' is not defined

    print(f"{o} in outer", chr(o))
    
    inner()

print(f"{o} in global", chr(o))
outer()  

# 输出: 
# 49 in global 1
# 65 in outer A
# 65 in inner A
```

如下图所示，我们标注了每个嵌套的名字空间及其作用域

![[Pasted image 20240614174355.png]]

> [!tip] 
> 
> 请注意：如果函数有参数，则参数也是属于函数的局部名字空间的
> 

## global 语句

执行如下示例代码，最终输出的结果为 $10$

```python hl:5
a = 10

def wapper():
    b = a + 10
    print(b)
    
wapper()

# 输出
# 20
```

下面，我们对上述代码进行简单的修改：在 `print(b)` 后面加上一条赋值语句。此时，再执行此代码时，Python 解释器抛出了 `UnboundLocalError`

```python
a = 10

def wapper():
    b = a + 10 
    print(b)
    a = 20 
    
wapper()

# 错误
# UnboundLocalError: cannot access local variable 'a' where it is not associated with a value
```

这错误信息说明了我们在使用局部变量 `a` 之前没有给它绑定值。因为，**Python 在扫描函数定义的代码时，会在函数的局部符号表中创建好出现在赋值运算符左侧名字**。当代码执行到 `b = a + 10` 时，此时变量 `a` 会在局部符号表中被查找到，但是此时变量 `a` 尚未绑定对象，因此抛出 `UnboundLocalError`

下面的另一个示例代码会抛出同样的错误

```python
x = 10

def wapper():
    x += 2  # x +=1：同样是赋值语句，x 就出现在了 wapper 的局部名字空间中。
		    # x += 1 等价于 x = x + 1，这里会引用 x，但是 x 尚未引用对象
    print(x)
   
wapper()
```

在某些场景下，我们必须让函数可以修改全局名称空间中名字的绑定。Python 提供了一个**关键 `global` 用于声明特定变量在全局作用域中，并应在全局作用域中重新绑定**

```python
x = 10

def wapper():
    global x  # 声明 x 在全局名称空间中, 之后对 x 的赋值会在全局作用域中重新绑定
    x += 2
    print("In wapper: ", x)
   
wapper()
print("In global: ", x)

# 输出
# In wapper:  12
# In global:  12
```

请注意：如果 `global` 声明了全局名字空间中没有的名字，那么之后的赋值语句就会将在全局空间中绑定。然而，下列代码会抛出 `NameError`，因为在创建变量 `x` 之前使用了`x`

```python
del x

def wapper():
    global x # 声明 x 在全局名称空间中，然而 x 在全局名称空间中并不存在
    x += 20  # 这条语句出发 NameError 异常
    print("In wapper: ", x)

wapper()

print("In global: ", x)
```

如果声明了名字 `x` 定义在全局名字空间中，但是在使用是全局名字空间不存在名字  `x`，此时抛出 `NameError`。因此，我们在使用前应该首先进行复制操作

```python
del x

def wapper():
    global x # 声明 x zai 全局名称空间中，然而 x 在全局名称空间中并不存在
    x = 100  # 将 x 在全局名字空间中重新绑定
    x += 20  # 也会绑定在全局名称空间中
    print("In wapper: ", x)

wapper()

print("In global: ", x)

# 输出
# In wapper:  120
# In global:  120
```

## 闭包

在计算机科学中，**闭包** 也称为 **词法闭包** 或 **函数闭包**，是一种词法绑定技术。在实现闭包时，通常采用结构体实现，这个结构体会保存一个 **函数** 和 **函数的关联环境**

> [!tip] 
> 
> 函数的环境里是若干对符号和值的对应关系，它既要包括 **约束变量**（该函数内部绑定的符号），也要包括 **自由变量**（在函数外部定义但在函数内被引用），有些函数也可能没有自由变量
> 

在 Python 中，如果函数 `f` 内定义了函数 `g`，那么如果函数 `g` 引用了自由变量，且这些自由变量没有被解释器优化掉，那么将产生闭包。

> [!tip] 
> 
> 换句话说，在 Python 中产生闭包的必要条件：**函数嵌套** 并且 **内层函数引用了外层函数中定义的变量**
> 
> 对于内层函数而言，外层函数中的变量是 **非局部非全局** 的
> 

如下示例，`outer` 中定义的变量 `non`，对于内层函数 `inner` 而言是非局部非全局变量

```python
o = 49
def outer(): 
    non = 65  # 对 inner 函数而言，non 是非局部非全局的
    def inner():
        o = 97 
        print(f"{o} in inner", chr(o))  
    inner()
```

如下所示，如果在内层函数中引用了外层函数的局部变量，即非局部非全局的变量，那么就会形成闭包

```python
def counter():
	c = [0]
	def inc():
		c[0] += 1  # 3. inc() 中引用了 c，c 引用的对象会被 
				   # inc 指向的函数对象记录了下来，从而形成了闭包
		# 注意 c[0] += 1 等价于 c[0] = c[0] + 1；c[0] 不是 inc() 的局部变量
		return c[0]
	return inc  # 1. conter() 调用时创建一个函数对象，
				# 然后 inc() 绑定该对象，并将该对象的引用地址返回

foo = counter()  # 2. foo 记住了 inc 引用的函数对象，
				# 该函数对象在 counter 函数返回后就不会被垃圾回收

print(foo())  # 1  3. 调用 foo 就等于调用在 counter 中创建的函数对象
print(foo())  # 2
print(foo())  # 3
```

> [!tip] 
> 
> `c[0] += 1` 是不会报错的。`c[0]` 是访问列表，而不是引用 `inc()` 的局部变量
> 

如果上述示例中的变量 `c` 不是列表，而是单纯的使用一个整数，此时执行时就会抛出 `UnboundLocalError`

```python
def counter():
	c = 0
	def inc():
		c += 1  # 4. 由于赋值语句的存在，在函数调用时 inc() 的局部名字空间中
				  # 就会有名字 c，但是尚未绑定对象。此时c += 1又会引用变量 c，
				  # 从而触发 UnboundLocalError 异常 
		return c
	return inc  # 1. conter 调用时创建一个函数对象，然后 inc 绑定该对象，
				  # 并将该对象的引用地址返回

foo = counter()  # 2. foo 记住了 inc 引用的函数对象，该函数对象在 
				   # counter 函数返回后就不会被垃圾回收

print(foo())  # 3. 调用 foo 就等于调用在 counter 中创建的函数对象
```

### nonlocal

在上述示例中，我们需要让 `inc()` 中对非全局非局部变量 `c`  的修改绑定到它原来的作用域中。请注意：这里我们不能使用 `global` 关键字，因为变量 `c` 不是全局变量。

在之前，我们就已经介绍过了变量 `c` 对于内层函数 `inc()` 而是是非局部非全局的，Python 使用 `nonlocal` 关键字来声明变量绑定在非局部非全局的作用域中

```python
def counter():
	c = 0
	def inc():
		nonlocal c  # 声明 c 位非局部非全局变量
		c += 1  # c 在它的上层函数找到了，从而引用了上层函数中的局部变量形成闭包
		return c
	return inc  # 1. conter 调用时创建一个函数对象，然后 inc 绑定该对象，
			      # 并将该对象的引用地址返回

foo = counter()  # 2. foo 记住了 inc 引用的函数对象，
                   # 该函数对象在 counter 函数返回后就不会被垃圾回收

print(foo())  # 3. 调用 foo 就等于调用在 counter 中创建的函数对象
```

> [!note] 
> 
> Python 有一个特殊规定：**如果不存在生效的 `global` 或 `nonlocal` 语句，则对名称的赋值总是会进入最内层作用域**
> + 赋值不会复制数据，只是将名称绑定到对象
> + 删除也是如此：语句 `del x `从局部作用域引用的命名空间中移除对 `x` 的绑定
> + 所有引入新名称的操作都是使用局部作用域：尤其是 `import` 语句和函数定义会在局部作用域中绑定模块或函数名称
> + `nonlocal` 声明的变量绝对不不能绑定到全局名字空间中
> 

在 Python 函数也是以对象的形式出现的，函数对象存在一个属性 `function.__closure__`，这个属性要么是 `None`；要么是一个元组，记录了内层函数引用的自由变量 

```python
def counter():
    c = 0
    print(f"c:{id(c):#X}")

    def inc():
        nonlocal c  # 声明 c 位非局部非全局变量
        c += 1  # c 在它的上层函数找到了，从而引用了上层函数中的局部变量形成闭包
        return c

    print(inc.__closure__) # (<cell at 0x0000021CC0022710: int object at 0x00007FFAE003F998>,)

    return inc  # 1. conter 调用时创建一个函数对象，然后 inc 绑定该对象，并将该对象的引用地址返回


foo = counter()  # 2. foo 记住了 inc 引用的函数对象，该函数对象在 counter 函数返回后就不会被垃圾回收

print(foo.__closure__)  # (<cell at 0x0000021CC0022710: int object at 0x00007FFAE003F998>,)

print(foo())  # 3. 调用 foo 就等于调用在 counter 中创建的函数对象

# 这时候，foo被调用了一次，由于 int 是不可变量类型，所以新创建一个对象
print(foo.__closure__)  # (<cell at 0x0000022D703427A0: int object at 0x00007FFAE003F9B8>,)
```
