# 推导式

推导式是一种便捷的创建多值对象的方式。在 Python 中，支持 **列表推导式**、**集合推导式**、**字典推导式** 和 **生成器推导式** 四种

> [!attention] 
> 
> 请注意：**没有元组推导式**，因为元组通常用于存储少量的异构数据
> 

## 列表推导式

**列表推导式** 创建列表的方式更简洁。常见的用法为，对序列或可迭代对象中的每个元素应用某种操作，用生成的结果创建新的列表；或用满足特定条件的元素创建子序列

例如，创建平方值的列表

```python
>> squares = []
>>> for x in range(10):
...     squares.append(x ** 2)
... 
>>> squares
[0, 1, 4, 9, 16, 25, 36, 49, 64, 81]
>>> x  # 变量 x 在循环结束后依旧存在
9
```

> [!attention] 
> 
> 这段代码创建（或覆盖）变量 `x`，该变量在循环结束后仍然存在
> 

使用 **列表推导式** 可以无副作用地计算平方列表

```python
>>> squares = [x ** 2 for x in range(10)]
>>> squares
[0, 1, 4, 9, 16, 25, 36, 49, 64, 81]
```

列表推导式的方括号内包含以下内容：一个表达式，随后是一个 `for` 语句，然后，是零个或多个 `for` 或 `if` 子句。结果是由表达式依据 `for` 和 `if` 子句求值计算而得出一个新列表。下面是完整的列表推导式

```python
[表达式 for var in iterable [(for var2 in iterable2) | if condition]* ]
```

例如，以下列表推导式将两个列表中不相等的元素组合起来

```python
>>> [(x, y) for x in [1,2,3] for y in [3, 1, 4] if x != y]
[(1, 3), (1, 4), (2, 3), (2, 1), (2, 4), (3, 1), (3, 4)]
```

> [!attention] 
> 
> 上述示例中的表达式是元组，此时必须加上括号
> 

在列表推导式中，可以使用复杂的表达式和嵌套函数

```python
>>> from math import pi
>>> [str(round(pi, i)) for i in range(1, 6)]
['3.1', '3.14', '3.142', '3.1416', '3.14159']

>>> [i for i in range(20) if i % 2 == 0 or i % 3 == 0]
[0, 2, 3, 4, 6, 8, 9, 10, 12, 14, 15, 16, 18]

>>> result = []  # 上述列表推导式等价于
>>> for i in range(20):
...     if i % 2 == 0 or i % 3 == 0:
...             result.append(i)
...
>>> result
[0, 2, 3, 4, 6, 8, 9, 10, 12, 14, 15, 16, 18]
```

此外，**列表推导式是可以嵌套的**。列表推导式中的初始表达式可以是任何表达式，甚至可以是另一个列表推导式

例如，下面这个 $3 \times 4$ 矩阵，由 $3$ 个长度为 $4$ 的列表组成

```python
>>> matrix = [
...     [1, 2, 3, 4],
...     [5, 6, 7, 8],
...     [9, 10, 11, 12],
... ]
```

当我们要求上述矩阵的转置时，可以使用如下的列表推导式

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

实际应用中，最好用 [内置函数](https://docs.python.org/zh-cn/3/library/functions.html) 替代复杂的流程语句。此时，`zip()` 函数更好用

```python
>>> list(zip(*matrix))  # 等价于 list(zip(matrix[0], matrix[1], matrix[2]))
[(1, 5, 9), (2, 6, 10), (3, 7, 11), (4, 8, 12)]
```

## 集合推导式

集合也可以使用集合推导式创建，其语法与列表推导式相同，只是将中括号换成花括号即可

```python
>>> a = {x for x in 'abracadabra' if x not in 'abc'} 
>>> a
{'d', 'r'}

>>> {(x, y) for x in "abcd" for y in range(3)}
{('b', 2), ('c', 0), ('a', 0), ('d', 0), ('d', 1), ('b', 1), ('c', 2), ('a', 2), ('d', 2), ('b', 0), ('a', 1), ('c', 1)}
>>> {(a,) for a in "abcabc"}
{('c',), ('a',), ('b',)}
>>> {(a) for a in "abcabc"}
{'c', 'a', 'b'}
```

## 字典推导式

**字典推导式** 可以用任意键值表达式创建字典

```python
{key: value for x in iterable [...]}
```

> [!warning] 
> 
> 字典推导式会创建字典对象，因此 `key` 必须是可哈希对象
> 

```python
>>> {x: x ** 2 for x in (2, 4, 6)}
{2: 4, 4: 16, 6: 36}

>>> {str(0x61 + i): (i+1)**2 for i in range(1, 4)}  # {"98":4, ....}
{'98': 4, '99': 9, '100': 16}

>>> {chr(0x61 + i): (i+1)**2 for i in range(1, 4)}  # {"b":4, ....}
{'b': 4, 'c': 9, 'd': 16}

>>> {x % 3 : x ** 2 for x in range(10)}  # {0:81, 1:49, 2:64}
{0: 81, 1: 49, 2: 64}
```

## 可迭代对象

在 Python 中，可以使用 `for` 循环进行遍历的对象称为可迭代对象。Python 内置的容器都是可迭代对象

> [!important] 
> 
> **Python 中容器对象都是可迭代对象**
> + `list`
> + `tuple`
> + `set`
> + `dict`
> + `string`
> + `bytes`
> + `bytearry`
> 

从本质上讲，可迭代对象是其所属类中定义了 `__iter__()` 特殊方法，我们会在 [[Python：迭代器协议]] 中详细介绍

### 迭代器

迭代器是专门用于迭代取值的对象，迭代器对象 **只能从头到尾迭代一遍**。它具有可迭代对象的特征，即迭代器对象所属类定义了 `__iter__()` 方法。此外，迭代器对象还具有返回下一个值的方法，即其所属类定义了 `__next__()` 方法

> [!seealso] 
> 
> 详细内容留在 [[Python：面向对象]] 之后介绍
> 

下面我们介绍两个内置函数 `iter()` 和 `next()`：它们用于创建一个迭代器对象，和从迭代器对象中返回下一个值

| 函数               | 描述                      |
| :--------------- | ----------------------- |
| `iter(iterable)` | 使用 `iterable` 创建一个迭代器对象 |
| `next(iterator)` | 返回迭代器 `iterator` 的下一个值  |

```python
>>> numbers = [1, 2, 3, 4, 5, 6, 7, 8, 9]
>>> iterator = iter(numbers)
>>> type(iterator)
<class 'list_iterator'>
>>> next(iterator)
1
>>> next(iterator)
2
>>> next(iterator)
3

>>> iterator[0]   # 迭代器只能通过 `next()` 或者 `for` 循环取值
Traceback (most recent call last):
  File "<python-input-3>", line 1, in <module>
    iterator[0]
    ~~~~~~~~^^^
TypeError: 'list_iterator' object is not subscriptable
```

当迭代器中的所有元素都已经被返回，此时再次使用 `next(iterator)` 就会抛出 `StopIteration` 异常

```python
...
>>> next(iterator)
8
>>> next(iterator)
9
>>> next(iterator)
Traceback (most recent call last):
  File "<python-input-19>", line 1, in <module>
    next(iterator)
    ~~~~^^^^^^^^^^
StopIteration

>>> len(iterator)
Traceback (most recent call last):
  File "<python-input-21>", line 1, in <module>
    len(iterator)
    ~~~^^^^^^^^^^
TypeError: object of type 'list_iterator' has no len()
```

> [!tip] 
> 
> 迭代器对象不是一个容器对象，不能使用 `len()` 函数
> 

### 生成器

生成器对象是一种特殊迭代器对象，由 **生成器函数所创建的对象** 称为生成器迭代器。关于生成器函数，我们将在 [[Python：函数高级]] 中介绍

Python 为我们提供了一个简单的创建生成器对象的表达式称为 **生成器推导式**。其语法与列表推导式类似，只需要将方括号换成圆括号即可

```python
(表达式 for var in iterable [(for var2 in iterable2) | if condition]* )
```

> [!attention] 
> 
> 使用 `(item for item in iterable)` 创建的是一个生成器对象，而非元组。
> 

```python
>>> g = (i ** 2 for i in range(1, 10))
>>> g
<generator object <genexpr> at 0x0000022FF8EAA670>
>>> len(g)  # 生成器不是容器，不支持 len() 函数
Traceback (most recent call last):
  File "<python-input-9>", line 1, in <module>
    len(g)  # 生成器不是容器，不支持 len() 函数
    ~~~^^^
TypeError: object of type 'generator' has no len()
```

如果需要从生成器对象中获取值，需要使用内置函数 `next()` 函数，或者使用 `for` 循环迭代

```python
>>> next(g)  # 使用 next() 返回生成器的下一个值
1

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

