# 推导式

## 列表推导式

**列表生成式** 创建列表的方式更简洁。常见的用法为
- 对序列或可迭代对象中的每个元素应用某种操作，用生成的结果创建新的列表（**对容器中的元素执行某个操作，结果作为一个新列表对象**）
- **创建某个容器中元素的子序列**，子序列中的元素需要满足特定条件

例如，创建平方值的列表

```python
>> squares = []
>>> for x in range(10):
...     squares.append(x ** 2)
... 
>>> squares
[0, 1, 4, 9, 16, 25, 36, 49, 64, 81]
>>> x
9
```
> [!question] 注意
> 这段代码创建（或覆盖）变量 `x`，该变量在循环结束后仍然存在

使用 **列表生成式** 可以无副作用地计算平方列表

```python
>>> squares = [x ** 2 for x in range(10)]
>>> squares
[0, 1, 4, 9, 16, 25, 36, 49, 64, 81]
```
> [!success] 遍历 `x` 在生成式外面就不存在了

列表推导式的方括号内包含以下内容：一个表达式，后面为一个 `for` 子句，然后，是零个或多个 `for` 或 `if` 子句。结果是由表达式依据 `for` 和 `if` 子句求值计算而得出一个新列表。下面是完整的列表推导式
```python
[表达式 for var in iterable \[(for var2 in iterable2) | if condition\]* ]
```

例如，以下列表推导式将两个列表中不相等的元素组合起来
```python
>>> [(x, y) for x in [1,2,3] for y in [3, 1, 4] if x != y]
[(1, 3), (1, 4), (2, 3), (2, 1), (2, 4), (3, 1), (3, 4)]
```
- 表达式是元组（例如上例的 `(x, y)`）时，必须加上括号

**列表推导式可以使用复杂的表达式和嵌套函数**

```python
>>> from math import pi
>>> [str(round(pi, i)) for i in range(1, 6)]
['3.1', '3.14', '3.142', '3.1416', '3.14159']
```

```
>>> [i for i in range(20) if i % 2 == 0 or i % 3 == 0]
[0, 2, 3, 4, 6, 8, 9, 10, 12, 14, 15, 16, 18]
>>> for i in range(20):
...     if i % 2 == 0 or i % 3 == 0:
...             result.append(i)
...
>>> result
[0, 2, 3, 4, 6, 8, 9, 10, 12, 14, 15, 16, 18]
```

### 嵌套的列表推导式

**列表推导式中的初始表达式可以是任何表达式，甚至可以是另一个列表推导式**

下面这个 $3 \times 4$ 矩阵，由 $3$ 个长度为 $4$ 的列表组成

```python
>>> matrix = [
...     [1, 2, 3, 4],
...     [5, 6, 7, 8],
...     [9, 10, 11, 12],
... ]
```

下面的列表推导式可以转置行列

```python
>>> [[row[i] for row in matrix] for i in range(4)]
[[1, 5, 9], [2, 6, 10], [3, 7, 11], [4, 8, 12]]
```

内部的列表推导式是在它之后的 `for` 的上下文中被求值的，所以这个例子等价于

```python
>>> transposed = []
>>> for i in range(4):
...     transposed.append([row[i] for row in matrix])
... 
>>> transposed
[[1, 5, 9], [2, 6, 10], [3, 7, 11], [4, 8,
```

实际应用中，**最好用 [内置函数](https://docs.python.org/zh-cn/3/library/functions.html) 替代复杂的流程语句**。此时，**`zip()` 函数更好用**

```python
>>> list(zip(*matrix))  # 等价于 list(zip(matrix[0], matrix[1], matrix[2]))
[(1, 5, 9), (2, 6, 10), (3, 7, 11), (4, 8, 12)]
```

---
列表生成式练习
```python
>>> [i ** 2for i in range(1,11)]
[1, 4, 9, 16, 25, 36, 49, 64, 81, 100]
>>> li = [1,4,9,2,5,10,15]
>>> [li[i] + li[i+1] for i in range(len(li)-1)]  # 相邻两项的和
[5, 13, 11, 7, 15, 25]
>>> import random
>>> import string
>>> [ f"{i:04d}.{''.join(random.choices(string.ascii_letters, k=10))}" for i in range(1,10)]  # ID 生成器
['0001.loXRiSvheG', '0002.qVGOpSzacY', '0003.FAVEWSNjWD', '0004.kFWMIShyoL', '0005.MpixdOAOaQ', '0006.CRMvuiTZvs', '0007.AjIHRZcetJ', '0008.ovSywuscNw', '0009.rgnnznjJMN']
```

## 生成器推导式

类似于列表生成式，当我们将两边的方括号变为圆括号之后，得到的表达式的值不是元祖，而是 **生成器** 
```python
(i for i in iterabel [if condition xxx])
```

> [!seealso] 生成器表达式
> 生成器表达式是产生构建生成器对象的快速方法。它返回一个可以 **惰性求值**、**延迟计算** 的对象，在需要的时候才会在内存中创建元素
>
> 关于生成器的详细内容，令见 [[函数高级#生成器函数]]
> 
```python
>>>[](函数高级.md#生成器函数)函数)函数)ange(1,10))  # 返回一个生成器对象
<generator object <genexpr> at 0x7037be4e7df0>
```

> [!note] 生成器表达式构建的生成器不能使用 `len` 函数

```python
>>> len((i for i in range(1,10)))  # 生成器对象没有 len()
Traceback (most recent call last):
  File "<stdin>", line 1, in <module>
TypeError: object of type 'generator' has no len()
```

如果需要从生成器对象中获取值，需要使用内置函数 `next()` 函数，或者使用 `for` 循环迭代
```python
>>> square = (i ** 2 for i in range(5))  # 创建一个生成器对象
>>> square
<generator object <genexpr> at 0x7037bdabbe60>
>>> next(square)  # 获取生成器的一个值
0
>>> next(square)
1
>>> for i in square:  # 迭代获取
...     print(i)
...
4
9
16
>>> next(square)  # 当一个生成器中的元素被取完了，再次获取就会触发 StopIteration 异常
Traceback (most recent call last):
  File "<stdin>", line 1, in <module>
StopIteration
```

> [!summary] **生成器对象** 是一种 **惰性可迭代对象**，类似于 `range` 对象。可以使用使用 `next()` 函数获取值，也可以使用 `for` 遍历
> 
> - 生成器对象与 `list, tuple, range` 的不同之处在于生成器对象可以使用 `next()` 获取值，而 `list, tuple, range` 都不可以
> - **生成器对象只能被迭代一次**，而 `list, tuple, range` 可以被反复迭代 

```python
>>> square = (i ** 2 for i in range(5))
>>> next(square)
0
>>> next(square)
1
>>> next(square)
4
>>>
>>> square = [i ** 2 for i in range(5)]  # 这是一个列表，列表生成式，会在内存中立即创建列表对象，包括其中的所有值
>>> next(square)
Traceback (most recent call last):
  File "<stdin>", line 1, in <module>
TypeError: 'list' object is not an iterator
```

> [!question] 什么时候适合生成器？
> - 通常使用在只需要遍历一次的场景
> - 内存紧张的场景。生成器只有再被取值的时候才会将值计算出来

> [!question] 什么时候不适用生成器
> - **在繁忙系统中，不适用生成器！！！**，尤其是生成器产生值是耗时操作的时候

## 集合推导式

与 [[#列表推导式]] 类似，**集合也支持推导式**

```python
>>> a = {x for x in 'abracadabra' if x not in 'abc'} 
>>> a
{'d', 'r'}
```

```python
>>> {(x, y) for x in "abcd" for y in range(3)}  # 12 项 元祖的集合
{('b', 2), ('c', 0), ('a', 0), ('d', 0), ('d', 1), ('b', 1), ('c', 2), ('a', 2), ('d', 2), ('b', 0), ('a', 1), ('c', 1)}
>>> {(a,) for a in "abcabc"} # 3 项 元祖的集合
{('c',), ('a',), ('b',)}
>>> {(a) for a in "abcabc"}  # 3 项 str 类型的集合
{'c', 'a', 'b'}
```

> [!error] 集合生成式中，最后放入集合的元素必须是可哈希的

```python
>>> {[x, y] for x in "abcd" for y in range(3)}  # 报错
Traceback (most recent call last):
  File "<stdin>", line 1, in <module>
  File "<stdin>", line 1, in <setcomp>
TypeError: unhashable type: 'list'
>>> {{x, y} for x in "abcd" for y in range(3)}  # 报错
Traceback (most recent call last):
  File "<stdin>", line 1, in <module>
  File "<stdin>", line 1, in <setcomp>
TypeError: unhashable type: 'set'
>>> {(x, y) for x in "abcabc" for y in [[1]]}  # 元祖中出现了可变的列表
Traceback (most recent call last):
  File "<stdin>", line 1, in <module>
  File "<stdin>", line 1, in <setcomp>
TypeError: unhashable type: 'list'
```

## 字典推导式

**字典推导式** 可以用任意键值表达式创建字典

```python
{key: value for x in iterable [...]}
```

> [!warning] `key` 必须是可哈希对象

```python
>>> {x: x ** 2 for x in (2, 4, 6)}
{2: 4, 4: 16, 6: 36}
```

```python
>>> {str(0x61 + i): (i+1)**2 for i in range(1, 4)}  # {"98":4, ....}
{'98': 4, '99': 9, '100': 16}

>>> {chr(0x61 + i): (i+1)**2 for i in range(1, 4)}  # {"b":4, ....}
{'b': 4, 'c': 9, 'd': 16}

>>> {x % 3 : x ** 2 for x in range(10)}  # {0:81, 1:49, 2:64}
{0: 81, 1: 49, 2: 64}
```


## 可迭代对象

**可以进行迭代取值的对象**

- 或者说, 可以被`for`循环遍历的对象;
- 或者说, 有`__iter__`方法的对象称为可迭代对象

> [!important] 可迭代对象的本质
> Python 中可迭代对象的本质就是实现了 `__iter__()` 方法的对象

> [!seealso] 另见
> - [[魔术方法：迭代器协议]]

### 容器都是可迭代对象

> [!important] **Python 中容器对象都是可迭代对象**
> - list
> - tuple
> - set
> - dict
> - string
> - bytes
> - bytearry

> [!summary] 总结
> 能够通过迭代一次次返回不同的元素的对象。
> - 所谓相同，不是指值是否相同，而是元素在容器中是否是同一个，例如列表中值可以重复的，`['a'，'a"]`，虽然这个列表有 $2$ 个元素，值一样，但是两个 `'a'` 是不同的元素

### 迭代器对象

**迭代器特殊的可迭代对象**，具备可迭代对象的特征

- 通过 `iter` 方法把一个可迭代对象封装成迭代器
- 通过 `next` 方法，迭代 迭代器对象

> [!important] 迭代器对象只能从头到尾迭代一遍

**迭代器** 是 **访问容器元素** 的一种方式。它可以 **记住遍历的位置**。

> [!summary] 迭代器对象从容器的第一个元素开始访问，直到所有的元素被访问完结束。迭代器 **只能往前不会后退**。当通过迭代器访问完容器的所有元素后，再次尝试通过迭代器访问容器中的元素就会触发 `StopIteration` 异常

> [!seealso] 迭代器令见
迭代器对象必须定义两个魔术方法 `__iter__()` 和 `__next__()`。详细参考 [[魔术方法：迭代器协议]]

### 生成器对象

生成器对象是一种迭代器对象，由 **生成器函数所创建的对象** 称为生成器迭代器

> [!seealso] 令见 [[函数高级#生成器函数]]

