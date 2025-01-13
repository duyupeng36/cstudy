# 函数基础

函数的使用大大 **减少了程序代码的复杂度**， **减少了代码冗余**。对于一个程序中要使用到相同或相似的代码，通常将重复代码封装成一个函数，由参数来控制函数的功能差异

## 定义与调用函数

**_定义_** 函数使用关键字 **`def`**，后跟 **函数名** 与 **括号内的形参列表**。函数语句从下一行开始，并且必须缩进。例如，下面的函数为一个可以输出限定数值内的斐波拉契数列的函数

```python
def fib(n):
    """Print a Fibonacci series up to n.
    
    :param n: end of the Fibonacci series
    :return: None
    """
    a, b = 0, 1
    while a < n:
        print(a, end=" ")
        a, b = b, a + b
    print()

fib(2000)  # 调用刚才定义的函数
```

使用 `python` 执行该脚本的结果为：

```shell
➜ python fib.py                       
0 1 1 2 3 5 8 13 21 34 55 89 144 233 377 610 987 1597
```

**函数内的第一条语句是字符串** 时，该字符串就是 **文档字符串**，也称为 **_docstring_**
- 利用文档字符串可以自动生成在线文档或打印版文档
- 还可以让开发者在浏览代码时直接查阅文档

> [!tip]  Python 开发者最好养成在代码中加入文档字符串的好习惯

函数在 **_执行_** 时使用 **函数局部变量符号表**，所有函数变量赋值都存在 **局部符号表** 中

**在引用变量时**，首先，在 **局部符号表里查找变量**，然后，是 **外层函数局部符号表**，再是 **全局符号表**，最后是 **内置名称符号表**。因此，尽管可以引用全局变量和外层函数的变量，但最好不要在函数内直接赋值（除非是 `global` 语句定义的全局变量，或 `nonlocal` 语句定义的外层函数变量）

### 传递给函数的实参是什么？

在调用函数时会将 **实际参数（实参）** 引入到被调用函数的 **局部符号表** 中；因此，实参是使用 **按值调用** 来传递的（其中的 **值** 始终是 **对象的 _引用_ 而不是对象的值**）。 当一个函数调用另外一个函数时，会为该调用创建一个新的局部符号表
> [!NOTE] 实参的传递
> 实参是使用 **按值调用** 来传递的，其中的 **值 始终是 对象的引用**，而不是对象的值
> 
>实际上，**_对象引用调用_ 这种说法更好**，因为，传递的是可变对象时，调用者能发现被调者做出的任何更改（插入列表的元素）。

下面的代码将使用 `id()` 查看传递给函数的值是什么

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

> [!tip] 
> 
> 我们发现在函数内层和外层输出的 `id()` 的值是一样的。证明了 **函数在传实参是传递的是对象的引用**

### 函数定义是名字与函数对象的绑定

函数定义在当前符号表中把函数名与 **函数对象（被 Python 解释器放在堆中）** 关联在一起

```python
>>> def fib():
...     pass
...
>>> type(fib)
<class 'function'>
```

> [!check] 函数名是一个标识符，它引用了一个函数对象

解释器 **把函数名指向的对象作为用户自定义函数**。还可以 使用其他名称指向同一个函数对象，并访问访该函数

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
>>> f = fib
>>> f(100)
0 1 1 2 3 5 8 13 21 34 55 89 
```

> [!summary] 
> 
> 函数对象是 **可调用对象**
> 

### `return` 语句

我们定义的两个函数 `fib` 和 `arg_param`  不返回值，因此，其他语言不把它当作函数，而是当作过程

**使用 `return` 语句可以为函数返回值**。不过 **即使没有 `return` 语句**，函数 **也会返回一个值**，这个值比较是 `None` （是一个内置名称）

一般来说，解释器不会输出单独的返回值 `None` ，如需查看该值，可以使用 `print()`：

```
>>> fib(0)
>>> print(fib(0))
None
```

编写不直接输出斐波那契数列运算结果，而是返回运算结果列表的函数也非常简单：

```python title:fib2.py
def fib2(n):
    """Print a Fibonacci series up to n.
    
    :param n: end of the Fibonacci series
    :return: None
    """
    result = []
    a, b = 0, 1
    while a < n:
        result.append(a)
        a, b = b, a + b
    return result


f100 = fib2(100)  # 调用它
print(f100)
```

使用 `python` 执行该脚本的结果为

```shell
➜ python fib2.py 
[0, 1, 1, 2, 3, 5, 8, 13, 21, 34, 55, 89]
```

## 函数调用详解

函数调用时, 要传递到函数内部的参数称为 **实际参数**。实际参数在函数调用前就已经存在与内存中

函数调用时, **参数的传递形式** 取决于函数的定义。但是，参数传递形式只有两种。假设，我们由如下定义方式

```python
def ask_ok(prompt, retries, reminder):
	"""测试输入是否成功"""
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

### 位置实参

在传递实参时，函数的形式参数采用其位置与实参列表中的对象进行绑定。例如，如下形式调用 `ask_ok`

```python
ask_ok('OK to overwrite the file?', 2, 'Come on, only yes or no!')
```
实参 `'OK to overwrite the file?'` 在实参列表中的位置为 $0$，则绑定在形参列表中位置为 $0$ 的变量 `prompt` 上

实参 `2` 在实参列表的位置为 $1$，则绑定在形参列表中位置为 $1$ 的 `retries` 上

实参 `'Come on, only yes or no!'` 在实参列表中的位置为 $2$，则绑定在形参列表中位置为 $2% 的 `reminder` 上

### 关键字实参

函数调用时，指定形式参数的名字传递实参。例如，如下形式调用 `ask_ok`

```python
ask_ok(retries=2, reminder='Come on, only yes or no!', prompt='OK to overwrite the file?')
```

通过关键字可以在形参列表中按任意顺序排列

### 混用位置实参和关键字实参

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

> [!success] 函数调用时，_关键字参数_ 必须跟  _在位置参数后面_

### 不要对同一个参数多次赋值

所有传递的关键字参数都必须匹配一个函数接受的参数，**关键字参数的顺序并不重要**。**不能对同一个参数多次赋值**，下面就是一个因此限制而失败的例子

```python
>>> def function(a):
...     pass
... 
>>> function(0, a=10)
Traceback (most recent call last):
  File "<stdin>", line 1, in <module>
TypeError: function() got multiple values for argument 'a'
```

### 解包实参列表

**函数调用要求独立的位置参数，但实参在列表或元组里时**，例如，内置的 `range()` 函数要求独立的 `start `和 `stop `实参

如果这些参数不是独立的，则要 在调用函数时，用 **`*` 操作符把实参从列表或元组解包出来**

```python
>>> list(range(3, 6))
[3, 4, 5]
>>> args = [3, 6]
>>> list(range(*args))  # 解包位置实参
[3, 4, 5]
```

> [!important] 字典中的键必须与形参名相同
> +  `**` 操作符把实参从字典中解包

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

> [!tip] `*` 解构可迭代对象，按位置传参。`**` 解构字典，按关键字传参

## 函数定义详解

###  形参缺省值

缺省值也称默认值，为形参指定默认值是非常有用的方式。调用函数时，可以使用比定义时更少的参数

```python
def ask_ok(prompt, retries=4, reminder='Please try again!'):
    while True:
        reply = input(prompt)
        if reply in {'y', 'ye', 'yes'}:  # 关键字 in 用于确认可迭代中是否包含某个值
            return True
        if reply in {'n', 'no', 'nop', 'nope'}:
            return False
        retries = retries - 1
        if retries < 0:
            raise ValueError('invalid user response')
        print(reminder)
```

该函数可以用以下方式调用
- 只给出必选实参：`ask_ok('Do you really want to quit?')`
- 给出一个可选实参：`ask_ok('OK to overwrite the file?', 2)`
- 给出所有实参：`ask_ok('OK to overwrite the file?', 2, 'Come on, only yes or no!')`

---

**参数默认值在函数定义时求值**，所以：

```python
i = 5

def f(arg=i):  # 对默认值求值，arg == 5
    print(arg)

i = 6
f()
```

运行该脚本的结果为

```shell
➜  python 默认值求值.py 
5
```

> [!ERROR] 重要警告
> 默认值会被函数对象保存且 **默认值只会在函数定义时计算一次**。默认值为列表、字典或类实例等可变对象时，会产生与该规则不同的结果

```python
def f(a, L=[]):  # 在定义函数对象时，会在 __defaults__ 属性上保存形参的缺省值
    L.append(a)
    return L

print(f.__defaults__)  # ([],)

print(f(1))  # [1]
print(f(2))  # [1, 2]
print(f(3))  # [1, 2, 3]
```

> [!check] 函数参数的缺省值保存在函数对象的 `__defaults__` 属性上。这个对象是一个元祖

运行该脚本的结果为

```shell
➜ python 默认值求值.py
([],)
[1]
[1, 2]
[1, 2, 3]
```

**不想在后续调用之间共享默认值时，应以如下方式编写函数**

```python
def f(a, L=None):
	if L is None:
		L = []
	L.append(a)
	return L

print(f(1))  # [1]
print(f(2))  # [2]
print(f(3))  # [3]
```

### 限制实参的传递方式

默认情况下，实参可以 **按位置** 或 **显式关键字** 传递给 Python 函数。为了让代码易读、高效，**最好限制参数的传递方式**，这样，开发者只需查看函数定义，即可确定参数项是 _仅按位置_、_按位置或关键字_，还是 _仅按关键字_ 传递

函数定义如下

```python
def f(pos1, pos2, /, pos_or_kwd, *args, kwd1, kwd2, **kwargs):
      -----------    ----------------   -------------------
        |             |                  |
        |        Positional or keyword   |
        |                                |-- Keyword only
        |-- Positional only
```

` /` 和 `*` 是可选的。这些符号表明形参如何把参数值传递给函数：**位置**、**位置或关键字**、**关键字（_命名形参_）**

#### 位置和关键字参数

函数定义中 **未使用 `/` 和 `*` 时**，参数可以按位置或关键字传递给函数。或者 **在 `/` 和 `*` 之间** 的参数也可以按位置或关键字传递给函数

```python
def standard_arg(arg1,arg2):  # 可以位置传递也可以关键字传递
    print(arg1, arg2)


standard_arg(10, 20)  # 10 20
standard_arg(arg1=20, arg2=30)  # 20 30
```

#### 仅限位置参数

特定形参可以标记为 **_仅限位置_**。**_仅限位置_** 时，形参的 **顺序** 很重要，且这些形参 **不能用关键字传递**

**仅限位置形参应放在 `/` （正斜杠）前**。`/` 用于在逻辑上分割仅限位置形参与其它形参。如果函数定义中没有 `/`，则表示没有仅限位置形参

`/` 后可以是 **_位置或关键字_** 或 **_仅限关键字_** 形参

```python
def pos_only_arg(pos_arg, /):  # 仅可以通过位置传递
    print(pos_arg)


pos_only_arg(10)
```

> [!warning] Python 版本必须在 3.8 之后

#### 仅限关键字参数

把形参标记为 **_仅限关键字_**，表明 **必须以关键字参数形式传递该形参**，应在参数列表中第一个 **_仅限关键字_** 形参前添加 `*`。

```python
def kwd_only_arg(*, arg):  # 仅可以通过关键字传递
    print(arg)


kwd_only_arg(arg=20)
```

### 接收任意数量的实参

调用函数时，使用 **任意数量的位置实参** 是最少见的选项。为了接收这些位置实参，在定义函数时，可以在最后一个位置参数使用 `*args` 作为参数名，这表明将多余的位置实参打包到一个元组对象中，通过 `args` 引用该元组对象。如下函数定义

```python
def write_multiple_items(file, separator, *args):
    file.write(separator.join(args))
```

`args` 参数用于采集传递给函数的所有剩余参数，因此，它们 **通常在形参列表的末尾**。

在 **`*args` 形参后** 的任何形式参数 **只能是仅限关键字参数**，即 只能用作关键字参数，不能用作位置参数：
```python
def concat(*args, sep="/"):
	return sep.join(args)

concat("earth", "mars", "venus")  # 返回字符串: 'earth/mars/venus'
concat("earth", "mars", "venus", sep=".")  # 返回字符串：'earth.mars.venus'
```

调用函数时，也可能使用 **任意数量的关键字实参**。为了接收这些关键字实参，在定义函数时，必须在形参列表的末尾使用 `**kwargs` 表明剩余的关键字参数封包在一个字典对象中，通过 `kwargs` 引用该字典对象上

```python
def function_name(*args, **kwargs):
    pass
```
1. `*`: 将接收按位置参数传过来的的变量, 并将他们打包成一个元组赋值给`args`
2. `**`: 将接收按默认参数形式(关键字参数)传递过来的变量, 并将他们打包成一个字典赋值给`kwargs`

### 形参总结

注意 `/` 和 `*`

```python
def combined_example(pos_only, /, standard1, standard2=2, *args,kwd_only, **kwargs):  
	# / 前仅可以通过位置传递; * 后仅可以通过关键字传递
    print(pos_only, standard, args, kwd_only, kwargs)

combined_example(10, 20, kwd_only=30)
```

> [!hint] 注意，形参缺省值需要写在 `/` 之后的, `*` 之前的最后一个位置上

![[Pasted image 20240614104741.png]]

---
> [!warning] 注意潜在的重复传参
> 下面的函数定义中，`kwds` 把 `name` 当作键，因此，可能与位置参数 `name` 产生潜在冲突
> ```python
> def foo(name, **kwds):
>     return 'name' in kwds
>
> foo(1, **{"name": 2})  # name 将获取两次值
>```

> [!check] 仅位置参数与仅关键字参数是不同的参数类型 
> 加上 `/` （仅限位置参数）后，就可以了。此时，函数定义把 `name` 当作位置参数，`'name'` 也可以作为关键字参数的键：
> ```python
> def foo(name, /,**kwds):
>     return 'name' in kwds
>
> foo(1, **{"name": 2})
> ```

**仅限位置形参的名称可以在 `**kwds` 中使用，而不产生歧义**

### 文档字符串

第一行应为对象用途的简短摘要。为保持简洁，不要在这里显式说明对象名或类型，因为可通过其他方式获取这些信息（除非该名称碰巧是描述函数操作的动词）。这一行应 **以大写字母开头**，**以句点结尾**

文档字符串为多行时，**第二行应为空白行**，在视觉上将摘要与其余描述分开。后面的行可包含若干段落，**描述对象的调用约定、副作用等**

**Python 解析器不会删除 Python 中多行字符串字面值的缩进**，因此，文档处理工具应在必要时删除缩进。这项操作遵循以下约定：文档字符串第一行 **_之后_** 的第一个非空行决定了整个文档字符串的缩进量（第一行通常与字符串开头的引号相邻，其缩进在字符串中并不明显，因此，不能用第一行的缩进），然后，删除字符串中所有行开头处与此缩进“等价”的空白符。不能有比此缩进更少的行，但如果出现了缩进更少的行，应删除这些行的所有前导空白符。转化制表符后（通常为 8 个空格），应测试空白符的等效性

```python
def my_function():
    """Do nothing, but document it.

    No, really, it doesn't do anything.
    """
    pass


print(my_function.__doc__)
```
运行该脚本的结果为
```shell
➜ python 文档字符串.py 
Do nothing, but document it.

    No, really, it doesn't do anything.
```

### 函数注解

**函数注解** 是可选的 **用户自定义函数类型的元数据完整信息**。详细参考 [函数注释](https://peps.python.org/pep-3107/) 和 [类型提示](https://peps.python.org/pep-0484/)

**标注 以字典的形式存放在函数的 `__annotations__` 属性** 中而 **对函数的其他部分没有影响**。

**形参标注** 的定义方式是在 **形参名后加冒号**，后面跟一个会被求值为标注的值的表达式。

**返回值标注** 的定义方式是加组合符号 `->`，后面跟一个表达式，这样的校注位于形参列表和表示 `def` 语句结束的冒号。

下面的示例有一个必须的参数、一个可选的关键字参数以及返回值都带有相应的标注:

```python
def f(ham: str, eggs: str = 'eggs') -> str:
    print("Annotations:", f.__annotations__)
    print("Arguments:", ham, eggs)
    return ham + ' and ' + eggs
```

## 编码风格

现在你将要写更长，更复杂的 Python 代码，是时候讨论一下 **代码风格** 了。 大多数语言都能以不同的风格被编写（或更准确地说，被格式化）；有些比其他的更具有可读性。 能让其他人轻松阅读你的代码总是一个好主意，采用一种好的编码风格对此有很大帮助。

Python 项目大多都遵循 [**PEP 8**](https://peps.python.org/pep-0008/) 的风格指南；它推行的编码风格易于阅读、赏心悦目。Python 开发者均应抽时间悉心研读；以下是该提案中的核心要点

- **缩进，用 4 个空格**，不要用制表符。

    4 个空格是小缩进（更深嵌套）和大缩进（更易阅读）之间的折中方案。制表符会引起混乱，最好别用
    
- **换行，一行不超过 79 个字符**。
    
    这样换行的小屏阅读体验更好，还便于在大屏显示器上并排阅读多个代码文件
    
- **用空行分隔函数和类**，及函数内较大的代码块。
    
- **最好把注释放到单独一行**。
    
- **使用文档字符串**。
    
- **运算符前后、逗号后要用空格**，但不要直接在括号内使用： `a = f(1, 2) + g(3, 4)`。
    
- **类和函数的命名要一致**；按惯例，命名类用 `UpperCamelCase`，命名函数与方法用 `lowercase_with_underscores`。命名方法中第一个参数总是用 `self` (类和方法详见 [[面向对象]] ）
    
- 编写用于国际多语环境的代码时，**不要用生僻的编码**。Python 默认的 UTF-8 或纯 ASCII 可以胜任各种情况。
    
- 同理，就算多语阅读、维护代码的可能再小，也 **不要在标识符中使用非 ASCII 字符**。

## 名字空间与作用域

### 名字空间
**namespace （命名空间）** 是 **从名称到对象的映射**。通常使用 Python 的 **字典** 实现（查询名字只需要消耗 $O(1)$ 时间），但除非涉及到性能优化，我们一般不会关注这方面的事情，而且将来也可能会改变这种方式。命名空间的例子有：

- **内置名称集合**（包括 `abs()` 函数以及内置异常的名称等）
- **模块的全局名称**
- **函数调用中的局部名称**
- **对象的属性集合也是命名空间的一种形式**

关于命名空间的一个重要知识点是，**不同命名空间中的名称之间绝对没有关系**；例如，两个不同的模块都可以定义 `maximize` 函数，且不会造成混淆。用户使用函数时必须要在函数名前面加上模块名。

Python 有三种名字空间，如下图所示

![[Pasted image 20240330160418.png]]

命名空间是在 **不同时刻创建的，且拥有不同的生命周期**
+ **内置名称空间**：在 Python _解释器启动时创建_，并且永远不会删除
+ **模块的全局命名空间**：在 _读取模块定义时创建_，模块的命名空间也会持续到解释器退出
+ 从脚本文件读取或交互式读取的，由解释器顶层调用执行的语句是 `__main__` 模块调用的一部分，也拥有自己的 **全局命名空间**
+ **函数的局部命名空间**：在函数 _被调用时被创建_，并在函数返回或抛出未在函数内被处理的异常时，被删除。当然，每次递归调用都有自己的局部命名空间

### 作用域

一个命名空间的 _作用域_ 是 Python 代码中的 **一段文本区域**，从这个区域 **可直接访问该命名空间**。“可直接访问”的意思是，该文本区域内的 **名称在被非限定引用时，查找名称的范围**，是包括该命名空间在内的。

**作用域** 虽然是被 **静态确定** 的，但会 **被动态使用**。执行期间的任何时刻，都会有 $3$ 或 $4$ 个“命名空间可直接访问”的嵌套作用域：
- **最内层作用域**，包含局部名称，并 **首先在其中进行搜索**
- 那些 **外层闭包函数的作用域**，包含 **“非局部、非全局”** 的名称，从最靠内层的那个作用域开始，**逐层向外搜索**
- **倒数第二层作用域**，包含当前模块的 **全局名称**
- **最外层（最后搜索）的作用域**，是 **内置名称的命名空间**

如下图展示了作用域

![[Pasted image 20240330161646.png]]

如果一个名称被声明为 **全局**，则所有引用和赋值都将直接指向 “倒数第二层作用域”，即包含模块的全局名称的作用域。 要重新绑定在 **最内层作用域以外** 找到的变量，可以使用 `nonlocal` 语句；如果未使用 `nonlocal` 声明，这些变量将为只读（尝试写入这样的变量将在最内层作用域中创建一个 新的 局部变量，而使得同名的外部变量保持不变）。

![[Pasted image 20240614174355.png]]

通常，**当前局部作用域将（按字面文本）引用当前函数的局部名称**。在函数之外，局部作用域引用与全局作用域一致的命名空间：模块的命名空间。 **类定义在局部命名空间内再放置另一个命名空间**。

> [!important] 重要的
> 划重点，**作用域是按字面文本确定的**：**模块内定义的函数的全局作用域就是该模块的命名空间，无论该函数从什么地方或以什么别名被调用**
> 
> 另一方面，**实际的名称搜索是在运行时动态完成的**。但是，Python 正在朝着 “编译时静态名称解析”的方向发展，因此不要过于依赖动态名称解析！（局部变量已经是被静态确定了。）

## 函数嵌套

在函数中定义另一个函数，形成了函数的嵌套

```python hl:1,4
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

![[Pasted image 20240614174355.png]]

上图标注了各种名称空间，并划定了其作用域的范围。注意，**函数的形式参数属于函数的局部名称空间**

如下，我们重新在 `inner` 函数中重新定义一个变量 `o = 97`

```python hl:1,3,7
def outer():  # outer 是一个标识符，属于全局名称空间

    o = 65  # o 是一个标识符，在 outer 的局部名称空间首次被定义
    
    def inner(): # inner 也是一个标识符，属于 outer 的局部名称空间
        
        o = 97  # o 是一个标识符，在 inner 的局部名称空间首次被定义

        print(f"{o} in inner", chr(o))  # chr 是一个标识符，属于内建名称空间
        # 这里访问的 o 在 inner 的局部名称空间中直接找到了，所以直接使用
        #  
        # 这里访问的 chr 在 inner 的局部名称空间中找不到，所以会去 outer 的局部名称空间中找，
        # chr 在 outer 的局部名称空间中找不到，所以会去全局名称空间中找，
        # 此时，chr 还没有查询到，所以会去内建名称空间中找，找到了 chr，所以可以正常执行
        # 如果此过程中没有找到 chr，那么会报错 NameError: name 'chr' is not defined

    print(f"{o} in outer", chr(o))
    # 这里访问的 o 在 outer 的局部名称空间中找到了，所以直接使用
    # 它和 inner 中的 o 属于不同的名字空间，是两个不同的标识符
    
    inner()
    
    print(f"{o} in outer", chr(o))  # 同名标识符在内层作用域中的赋值不会影响外层作用域的标识符

outer()  

# 输出: 
# 65 in outer A 
# 97 in inner a
# 65 in outer A
```
 
## `global`

下面是另一个示例

```python hl:5
a = 10

def wapper():
    b = a + 10
    print(b)
    
wapper()

# 输出
# 20
```

现在，我们在第 $5$ 行 `print(b)` 之后，加上一条语句
```python hl:4,6
a = 10

def wapper():
    b = a + 10 # 2. 这的 a 引用的是 wapper 中的局部名称空间中的 a，此时 a 尚未引用对象
    print(b)
    a = 20 # 1. 这条赋值语句在函数被调用那一刻开始，a 就在 wapper 的局部名称空间中
    
wapper()

# 错误
# UnboundLocalError: cannot access local variable 'a' where it is not associated with a value
```

这段代码的运行将会在第 $4$ 行触发 `UnboundLocalError`

> [!important] 出错的原因
> 我们先回顾一下名字空间：**函数的名字空间在函数被调用时创建**
> - 第 $8$ 行调用了 `wapper` 函数，此时 `wapper` 函数的名字空间被创建，其中的名字包括
> 	- `wapper` 的形式参数
> 	- 赋值语句定义的名字（Python 中的变量在第一次赋值时被定义）
> - 也就是说，此时 `wapper` 在调用那一刻，名字空间中存在的名字有 `a, b` 但是，`a` 和 `b` 绑定的对象还是未知的 (`{"a":None, "b": None}`)
> - 当执行到第 $4$ 行语句 `b = a + 10`，此时的 `a` 是局部名称，并且尚未绑定对象，触发 `UnboundLocalError` 

下面的代码

```python
x = 10

def wapper():
    x += 2  # x +=1：同样是赋值语句，x 就出现在了 wapper 的局部名字空间中。
		    # x += 1 等价于 x = x + 1，这里会引用 x，但是 x 尚未引用对象
    print(x)
   
wapper()
```

同样会触发 `UboundLocalError`。在某些场景下，我们必须让函数可以修改全局名称空间中名字的绑定。Python 提供了一个关键 `global` 用于 **表明特定变量在全局作用域里**，并应在全局作用域中重新绑定

```python
x = 10

def wapper():
    global x  # 声明 名字 x 在全局名称空间中, 该条语句之后的重新绑定也应该在全局名称空间中
    x += 2
    print("In wapper: ", x)
   
wapper()
print("In global: ", x)

# 输出
# In wapper:  12
# In global:  12
```

分析下面代码

```python
del x

def wapper():
    global x # 声明 x zai 全局名称空间中，然而 x 在全局名称空间中并不存在
    x += 20  # 这条语句出发 NameError 异常：NameError: name 'x' is not defined
    print("In wapper: ", x)

wapper()

print("In global: ", x)
```

上面的代码会出错，因为全局名字空间中没有 `x`

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

## **闭包**

想一想，在函数嵌套中，在 **内层函数中未定义** 的变量，在它的 **外层函数中定义**。这个变量对于内存函数而言称为 **非局部非全局变量**

如下示例，`outer` 中定义的变量 `non`，对于内层函数 `inner` 而言，称为非局部非全局的变量

```python
o = 49
def outer(): 
    non = 65  # 对 inner 函数而言，non 是非局部非全局的
    def inner():
        o = 97 
        print(f"{o} in inner", chr(o))  
    inner()
```

指在函数 _嵌套_ 定义时, 内部函数 _引用_ 了外部函数的变量, 就会形成闭包。例如，下面的代码

```python
def counter():
	c = [0]
	def inc():
		c[0] += 1  # 3. inc 中引用了 c，c 引用的对象会被 inc 指向的函数对象记录了下来，从而形成了闭包
		return c[0]
	return inc  # # 1. conter 调用时创建一个函数对象，然后 inc 绑定该对象，并将该对象的引用地址返回

foo = counter()  # 2. foo 记住了 inc 引用的函数对象，该函数对象在 counter 函数返回后就不会被垃圾回收

print(foo())  # 1  3. 调用 foo 就等于调用在 counter 中创建的函数对象
print(foo())  # 2
print(foo())  # 3
```

> [!important]  `c[0] += 1` 是不会报错的。`c[0]` 是访问列表，而不是引用 `inc` 的局部变量

下图站立上述代码的分析过程
![[Pasted image 20240614222704.png]]

我们将 `c[0]` 替换为 `c`

```python
def counter():
	c = 0
	def inc():
		c += 1  # 4. 由于赋值语句的存在，在函数调用时 inc 的局部名字空间中就会有名字 c，但是尚未绑定对象。此时 c += 1 又会引用变量 c，从而触发 UnboundLocalError 异常 
		return c
	return inc  # 1. conter 调用时创建一个函数对象，然后 inc 绑定该对象，并将该对象的引用地址返回

foo = counter()  # 2. foo 记住了 inc 引用的函数对象，该函数对象在 counter 函数返回后就不会被垃圾回收

print(foo())  # 3. 调用 foo 就等于调用在 counter 中创建的函数对象
```

> [!important] `c+=1` 会触发 `UnboundLocalError`

> [!error] 这里的并没有闭包


### `nonlocal`

为了上述代码形成闭包，我们需要在 `inc` 函数中引用外层函数的变量。Python 提供的关键字 `nonlocal`  **表明特定变量在非全局非局部作用域中**，并应在非全局非局部作用域中重新绑定
```python
def counter():
	c = 0
	def inc():
		nonlocal c  # 声明 c 位非局部非全局变量
		c += 1  # c 在它的上层函数找到了，从而引用了上层函数中的局部变量形成闭包
		return c
	return inc  # 1. conter 调用时创建一个函数对象，然后 inc 绑定该对象，并将该对象的引用地址返回

foo = counter()  # 2. foo 记住了 inc 引用的函数对象，该函数对象在 counter 函数返回后就不会被垃圾回收

print(foo())  # 3. 调用 foo 就等于调用在 counter 中创建的函数对象
```

> [!note] Python 有一个特殊规定
> 
> **如果不存在生效的 `global` 或 `nonlocal` 语句，则对名称的赋值总是会进入最内层作用域**
> - 赋值不会复制数据，只是将名称绑定到对象
> - 删除也是如此：语句 `del x `从局部作用域引用的命名空间中移除对 `x` 的绑定
> - 所有引入新名称的操作都是使用局部作用域：尤其是 `import` 语句和函数定义会在局部作用域中绑定模块或函数名称
> - `nonlocal` 声明的变量绝对不不能绑定到全局名字空间中

> [!important] 函数对象的属性 `function.__closure__` 要么是 None；要么是一个元组，记录了内层函数对外层函数自由变量的引用

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

## 练习

### 编写一个函数，能够至少接受 $2$ 个参数，返回最大值和最小值

首先想到的是

```python
def max_min(x, y, *args):
    MAX, *_, MIN = sorted([x, y, *args])  # sorted 采用的 TimeSort 算法，时间复杂度为 O(nlogn)
    return MAX, MIN
```

> 随然使用的优秀的排序算法，但是排序依旧很耗时。排序

然后，使用内置函数 `max` 和 `min` 

```python
def max_min(x, y, *args):
    return max(x, y, *args), min(x, y, *args)  # 两次遍历，时间复杂度为 O(2n)。解构两次 O(2n)  ==> O(n)
```

这个版本遍历了两次。并且解构参数执行了两次

```python
def max_min(x, y, *args):
    MAX = max(args)  # 遍历一次 args O(N)
    MIN = min(args)  # 遍历一次 args O(N)
    
    MAX = max(x, y, MAX)  # (x if x > y else y) if x > MAX else (y if y > MAX else MAX)

    MIN = min(x, y, MIN)  # (x if x < y else y) if x < MIN else (y if y < MIN else MIN)

    return MAX, MIN 
```

先区分 `x` 和 `y` 的大小，然后一次遍历

```python
def max_min(x, y, *args):
    # MAX = x if x > y else y
    # MIN = x if x < y else y

    MAX, MIN = (x, y) if x > y else (y, x)
    
    for item in args:  # 依次遍历 args 中的元素 时间复杂度为 O(n)
        if MAX < item:
            MAX = item
        elif MIN > item:  # item 比最大值大了，就不会比最小值小。互斥的
            MIN = item
    return MAX, MIN
```
