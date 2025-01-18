# 函数基础

函数简单来说就是 **一连串语句**，这些语句 **被组合在一起**，并被指定了一个 **名字**。但是函数的使用大大 **减少了程序代码的冗余，提高代码的可读性**

## 定义与调用

在 Python 中定义函数需要使用 **关键字 `def`**，紧跟其后的是一个 **空格** 和 **函数名**，以及 **圆括号内的形参列表**。函数体从下一行开始，并且必须具有相同的缩进

如下示例，定义了一个输出 **斐波拉契数列** 前 $n$ 项的函数 `fib()`

```python title:fib.py
def fib(n):
	a, b = 0, 1
	while a < n:
		print(a, end=' ')
		a, b = b, a + b
	print()
```

Python 解释器在遇见 `def` 关键之后，会扫描函数体(不会执行函数体中的代码)并创建一个函数对象，并赋值给函数名

> [!tip] 
> 
> 也就说，Python 中 **函数名本质上就是一个普通的标识符**，它引用了一个函数对象
> 

当需要执行函数体中的代码时，就需要通过 `函数名(实参列表)` 的形式 **调用函数对象**

```python fib.py
fib(100)  # 调用 fib() 函数
```

函数在执行时使用 **函数局部符号表**。**函数体内的变量赋值都存储在局部符号表中**。在函数体内部引用变量时，首先查找 **局部符号表** 中的变量；如果在局部符号表中没有查找到变量，就在 **外层函数局部符号表** 中查找；如果在外层函数局部符号表中没有查找到变量，就在 **全局符号表**；如果还未查找到变量，最后在 **内置名称符号表** 中查找。

> [!tip] 
> 
> 函数体内部的变量赋值时，会存储在局部符号表
> 
> 函数体内部引用变量时，首先查找局部符号表，然后查找外层函数局部符号表，再然后查找全局符号表，最后查找内置名称符号表
> 

### 参数传递

在调用函数时，实际参数被加入到被调函数的局部符号表中；因此，实参是使用 **值传递** 的。然而，**Python 中变量的值始终时对象的引用**

> [!tip] 
> 
> 实参使用按值调用来传递，其中的 **值指的是对象的引用**，而 **不是的值**
> 

下面我们通过 `id()` 函数验证

```python title:fib.py
def fib(n):
    print("id(n) = ", id(n))  # 形参的 id
    a, b = 0, 1
    while a < n:
        print(a, end=' ')
        a, b = b, a + b
    print()


number = 10  # 实参
print("id(number) = ", id(number))  # 实参的 id
fib(number)
```

执行脚本的输出结果是

```
python fib.py
id(number) =  124249497793776
id(n) =  124249497793776
0 1 1 2 3 5 8 
```

> [!tip] 
> 
> 从上述输出结果来看，实参 `id` 和形参 `id` 的输出时相同的，证明他们是引用的同一个对象
> 

### 定义就是名字与函数对象的绑定

函数定义就是在当前符号表中 **把函数名与函数对象关联**

```python
>>> def fib():  
...     pass  
...        
>>> type(fib)  
<class 'function'>
```

> [!tip] 
> 
> 函数名就是一个普通的标识符，引用了一个函数对象
> 

由于函数名是一个标识符，因此可以将函数名复制给另一个变量

```python
>>> def fib(n):
...     a,b=0,1
...     while a < n:
...         print(a, end=' ')
...         a, b = b, a + b
...     print()
...
>>> fib(10)
0 1 1 2 3 5 8
>>> f = fib  # 将函数对象赋值给另一个变量
>>> f(10)
0 1 1 2 3 5 8
```

### return 语句

通过 `return` 语句可以让函数返回一个值

```python title:fib.py
def fib(n):
    result = []
    a, b = 0, 1
    while a < n:
        result.append(b)
        a, b = b, a + b
    return result  # 将 result 返回
```

在调用函数时，可以使用一个变量接收函数的返回值

```python title:fib.py
number = 10

result = fib(number)  # 调用函数，并将返回值赋值给 result

print(result)
```

> [!attention] 
> 
> Python 中的函数没有 `return` 语句也会返回一个特殊的 `None` 值，表示没有任何值
> 

Python 在执行函数体时，如果遇见 `return` 语句，就会从函数返回并从调用处的下一条语句开始执行

## 函数调用详解

函数调用时，需要将实际参数传递给函数。Python 提供了两种参数传递形式：**位置实参** 和 **关键字实参**

现假设我们定义了下面的函数

```python
def ask_ok(prompt, retries, reminder):
	while True:
		reply = input(prompt)
		if reply in {'y', 'ye', 'yes}:
			return True
		if reply in {'n', 'no', 'nop', 'nope'}:
			return False
		retries -= 1
		if retries < 0:
			raise ValueError('Invalied user response')
		print(reminder)
```

### 位置实参

在传递实参时，函数的形式参数使用其在形参列表中的位置与实参列表中对应位置的对象进行绑定。如下方式调用 `ask_ok()` 函数

```python
ask_ok("Ok to overwrite the file?", 2, "Come on, only yes or no!")
```

实参 `"Ok to overwrite the file?"` 在实参列表中的位置为 $0$，则绑定在形成列表中位置为 $0$ 的变量 `prompt` 上

实参 `2` 在实参列表中的位置为 $1$，则绑定在形成列表中位置为 $1$ 的变量 `retries` 上。实参 `"Come on, only yes or no!"` 在实参列表中的位置为 $2$，则绑定在形参列表中位置为 $2$ 的变量 `reminder` 上

### 关键字实参

在传递实参时，**可以指定绑定实参的形参变量名**。如下方式调用 `ask_ok()` 函数

```python
ask_ok(retries=2, reminder="Come on, only yes or no!", prompt="Ok to overwrite the file?")
```

> [!tip] 
> 
> 由于指定了接受实参的形参变量名，因此实参列表中可以按任意顺序排列
> 

### 混用

在传递实参时，位置实参和关键字实参可以一起使用。但是，**关键字实参必须位于位置实参之后**

```python
def sum3(a, b, c):
	return a + b + c
```

上面定义的函数需要 $3$ 个实参，可以通过位置实参或关键字实参进行传递

```python
sum3(1, 2, 3)
sum3(b=2, c=3, a=1)
```

甚至，我们可以混用两种形式

```python
sum3(1, c=3, b=2)
```

> [!attention] 
> 
> 再次强调，关键实参必须位于所有位置实参之后
> 

混用时，还需要注意 **不要为同一个形参多次赋值**。如下调用 `sum3()` 将会失败

```python
sum3(1, a=2,c=3, b=1)  # 会触发 TypeError
```

```
Traceback (most recent call last):
  File "<python-input-15>", line 1, in <module>
    sum3(2, a=1,b=3,c=2)
    ~~~~^^^^^^^^^^^^^^^^
TypeError: sum3() got multiple values for argument 'a'
```

### 解包实参

调用函数时，要求独立的 **位置实参**。如果实参在列表或元祖中，在调用函数时可以使用 `*` 从序列中把实参解包出来

```python
>>> args = [3,6]
>>> list(range(args))  # 需要列表中的元素，而不是列表
Traceback (most recent call last):
  File "<python-input-20>", line 1, in <module>
    list(range(args))
         ~~~~~^^^^^^
TypeError: 'list' object cannot be interpreted as an integer
>>> list(range(*args))  # 从列表中解包出实参
[3, 4, 5]
```

如果实参在字典中，并且字典的 `key` 与形参变量相同，可以使用 `**` 从字典中将实参解包出来

```python
>>> def parrot(voltage, state='a stiff', action='voom'):
...     print("-- This parrot wouldn't", action, end=' ')
...     print("if you put", voltage, "volts through it.", end=' ')
...     print("E's", state, "!")
...     
>>> d = {"voltage": "four million", "state": "bleedin' demised", "action": "VOOM"}
>>> parrot(**d)  # 从字典中解包实参
-- This parrot wouldn't VOOM if you put four million volts through it. E's bleedin' demised !
```

> [!summary] 
> 
> `*` 解包可迭代对象，按位置传参
> 
> `**` 解包字典，按关键字传参
> 

## 函数定义详解

### 形参默认值

在定义函数时，我们可以为函数形参指定默认值。为形参指定默认值通常非常有用。在调用函数时，可以使用比定义时更少的参数。例如，下列函数 `ask_ok()` 的定义，它为 `retries` 和 `reminde` 提供了默认值

```python
def ask_ok(prompt, retries=4, reminde="Please try again!"):
	while True:
		reply = input(prompt)
		if reply in {'y', 'ye', 'yes}:
			return True
		if reply in {'n', 'no', 'nop', 'nope'}:
			return False
		retries -= 1
		if retries < 0:
			raise ValueError('Invalied user response')
		print(reminder)
```

在调用上述函数时，针对形参 `retries` 和 `reminde` 可以提供也可以提供其对应的实参

```python
ask_ok('Do you really want to quit?')  # 只给出必要的实参
ask_ok('OK to overwrite the file?', 2)  # 给出一个可选实参
ask_ok('OK to overwrite the file?', 2, 'Come on, only yes or no!') # 给出所有实参
```

> [!tip] 
> 
> 如果形参没有对应的实参，则使用其默认值
> 

> [!attention] 
> 
> 默认值在定义时的作用域内求值，并且只会计算一次
> 

```python
i = 5

def f(arg=i):  # 此时，该作用域内的 i 的值为 5
	print(arg) # 所以，形参 arg 的默认值是 5

i = 6
f()
```

上述脚本的执行结果为

```
5
```

由于形参默认值只会在定义时求值一次，当形参默认值时列表等可变对象时，其结果可能不同

```python
def f(a, L=[]):
    L.append(a)
    return L

print(f(1))
print(f(2))
print(f(3))
```

输出结果如下：

```
[1]
[1, 2]
[1, 2, 3]
```

如果不希望在后续调用之间共享默认值时，应以如下方式编写函数

```python
def f(a, L=None):
    if L is None:
        L = []
    L.append(a)
    return L
```

### 限制参数传递方式

**默认情况下，参数可以按位置或显式关键字传递给 Python 函数**。为了让代码易读、高效，**最好限制参数的传递方式**，这样，开发者只需查看函数定义，即可确定参数项是仅按位置、按位置或关键字，还是仅按关键字传递

函数定义如下：

```python
def f(pos1, pos2, /, pos_or_kwd, *, kwd1, kwd2):
      -----------    ----------     ----------
        |             |                  |
        |        位置或关键字   |
        |                                - 仅限关键字
         -- 仅限位置
```

`/` 和 `*` 是可选的。这些符号表明形参如何把参数值传递给函数：**位置**、**位置或关键字**、**关键字**。关键字形参也叫作命名形参。

> [!tip] 位置或关键字参数
> 
> 形参声明时为使用 `/` 和 `*`，或者形参声明在 `/` 和 `*` 之间，则形参可以采用位置实参或关键字实参进行传递 
> 

> [!tip] 仅位置参数
> 
> 形参声明在符号 `/` 之前，则形参仅采用位置实参进行传递
> 

> [!tip] 仅关键字参数
> 
> 形参声明在符号 `*` 之后，则形参仅采用关键字实参进行传递
> 

### 接受可变数量的实参

调用函数时，使用任意数量的实参是最少见的选项。这些实参包含在元组中。在可变数量的实参之前，可能有若干个普通参数：

```python
def write_multiple_items(file, separator, *args):
    file.write(separator.join(args
```

**可变数量参数** 用于 **采集传递给函数的所有剩余参数**，因此，它们 **通常在形参列表的末尾**。`*args` 形参后的任何形式参数只能是仅限关键字参数，即只能用作关键字参数，不能用作位置参数：

```python
>>> def concat(*args, sep="/"):
...     return sep.join(args)
... 
>>> concat("earth", "mars", "venus")
'earth/mars/venus'
>>> concat("earth", "mars", "venus", sep=".")
'earth.mars.venus'
```

如果调用函数时提供了任意数量的关键字实参。这些关键字实参包含在一个字典中。在可变数量的关键字实参前，可能有若干个关键字实参

```python
def function_name(a, b, *arg, **kwarg):
	pass
```

`**kwargs` 形参在定义时必须位于形参列表的最后，用于采集传递给函数的所有剩余关键字参数

### 函数注解

函数注解 是可选的用户自定义函数类型的元数据完整信息。**标注** 以字典的形式存放在函数的 `__annotations__` 属性中而 **对函数的其他部分没有影响**。

> [!tip] 形参注解：提示形参需要的类型
> 
> 形参标注的定义方式是在**形参名后加冒号**，后面 **跟一个会被求值为标注的值的表达式**。

> [!tip] 返回值注解：提供函数的返回值类型
> 返回值标注的定义方式是加组合符号 `->`，后面跟一个表达式，这样的校注位于形参列表和表示 `def` 语句结束的冒号。
> 

下面的示例有一个必须的参数、一个可选的关键字参数以及返回值都带有相应的标注:

```python
>>> def f(ham: str, eggs: str = 'eggs') -> str:
...     print("Annotations:", f.__annotations__)
...     print("Arguments:", ham, eggs)
...     return ham + ' and ' + eggs
...
>>> f('spam')
Annotations: {'ham': <class 'str'>, 'return': <class 'str'>, 'eggs': <class 'str'>}
Arguments: spam eggs
'spam and eggs'
```

## 匿名函数

`lambda`  关键字用于创建小巧的匿名函数。`lambda a, b: a+b` 函数返回两个参数的和

**匿名函数可用于任何需要函数对象的地方**。在语法上，**匿名函数只能是单个表达式**。在语义上，它只是**常规函数定义的语法糖**。与嵌套函数定义一样，`lambda` 函数可以引用包含作用域中的变量：

```python
>>> def make_incrementor(n):
...     return lambda x: x + n
...
>>> f = make_incrementor(42)
>>> f(0)
42
>>> f(1)
43
```

上例用 `lambda` 表达式返回函数。还可以把匿名函数用作传递的实参

```python
>>> pairs = [(1, 'one'), (2, 'two'), (3, 'three'), (4, 'four')]
>>> pairs.sort(key=lambda pair: pair[1])
>>> pairs
[(4, 'four'), (1, 'one'), (3, 'three'), (2, 'two')]
```

## 嵌套函数

Python 函数中可以再定义另一个函数，从而形参函数的嵌套

```python hl:2-3
def mkinc(a):
	def inc():
		return a + 1
	return inc 
```

上面高亮部分就在函数内部定义的函数。
