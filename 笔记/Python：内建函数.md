# 内建函数
## 简单的内建函数

下目标列出一些简单内建函数

|                         函数                         | 作用                                                                                      |
| :------------------------------------------------: | :-------------------------------------------------------------------------------------- |
|                       `id()`                       | 返回对象的标识                                                                                 |
|                      `hash()`                      | 返回一个对象的哈希值                                                                              |
|                      `type()`                      | 返回对象的类                                                                                  |
|           `float()` `int()` `complex()`            | 构建数值                                                                                    |
|              `bin()` `hex()` `oct()`               | 整数的二进制 十六进制 八进制形式的字符串                                                                   |
| `list()` `tuple()` `str()` `bytes()` `bytearray()` | 构建序列容器                                                                                  |
|          `set()`  `frozenset()` `dict()`           | 构建散列表                                                                                   |
|                      `ord()`                       | 返回字符的位模式(编码，整数表示)                                                                       |
|                      `chr()`                       | 将编码转换为字符                                                                                |
|                 `input([prompt])`                  | 从标准输入读取字符串                                                                              |
|                     `print()`                      | 向标准输出写入对象的字符表示形式                                                                        |
|                      `len()`                       | 返回容器的元素个数                                                                               |
|         `isinstance(obj, class_or_tuple)`          | 判断 `obj` 是否为某些类型                                                                        |
|         `issubcleass(cls, class_or_tuple)`         | 判断 `cls` 是否为某些类的子类                                                                      |
|                      `abs()`                       | 返回数值的绝对值                                                                                |
|                  `max()` `min()`                   | 返回容器中的最大值和最小值                                                                           |
|                `round(x, ndigits)`                 | 返回 `x` 的近似值（保留 `ndigits` 位小数），输入规则：舍入至最接近的 10 的负 `ndigits` 次幂的倍数，如果与两个倍数同样接近，则 **选用偶数** |
|                    `pow(x,y,m)`                    | 返回 $x^y \mod {m}$                                                                       |
|                     `range()`                      | 返一个 `range` 对象                                                                          |
|                   `divmod(x, y)`                   | 返回 `(x//y, x % y)`                                                                      |
|              `sum(iterable,start=0)`               | 返回容器中的元素的和                                                                              |
|          `slice(start, stop, step=None)`           | 返回一个表示由 `range(start, stop, step)` 指定的索引集的 `slice` 对象。`slice` 参考 [[线性数据结构#切片]]          |

## sorted 和 reversed

|                        函数                         | 作用                              |
| :-----------------------------------------------: | :------------------------------ |
| `sorted(iterable, /, *, key=None, reverse=False)` | 根据 `iterable` 中的项返回一个新的已排序列表    |
|                  `reversed(seq)`                  | 返回一个反向的 `iterator`。通过索引方向取值的迭代器 |
> [!note] `seq` 的需要满足如下条件
>  `seq` 必须是一个 **具有 `__reversed__()` 方法** 或是支持 **序列协议**（具有` __len__()` 方法和从 $0$ 开始的整数参数的 `__getitem__()` 方法）的对象。参见 [[魔术方法：序列协议]]

```python
>>> list(reversed("13579"))
['9', '7', '5', '3', '1']
>>> {reversed("13579")}
{<reversed object at 0x7[](魔术方法：容器协议.md)
```

```python
#下面两句输出一样吗?
print(sorted({1,9,5},reverse=True))  # 输出列表字符串形式
print(reversed(sorted({1,9，5})))  # 输出一个对象的字符串形式
```

## `enumerate(iterable, start=0)` 

返回一个 **枚举对象**，这是一个 _迭代器对象_。`iterable` 必须是一个序列，或 `iterator`，或其他支持迭代的对象

`enumerate()` 返回的迭代器的 `__next__() `方法返回一个元组，里面包含一个计数值（从 `start` 开始，默认为 0）和通过迭代 `iterable` 获得的值

```python
>>> li = [1,2,3,4]
>>> enumerate(li)
<enumerate object at 0x72b69bac88c0>
>>> next(enumerate(li))
(0, 1)
>>> enum = enumerate(li)
>>> next(enum)  # 返回一个元组，右 enumerate 的 __next__() 方法返回
(0, 1)
>>> next(enum)
(1, 2)
>>> next(enum)
(2, 3)
>>> next(enum)
(3, 4)
>>> next(enum)
Traceback (most recent call last):
  File "<stdin>", line 1, in <module>
StopIteration
```

等价于

```python
def enumerate(iterable, start=0):
    n = start
    for elem in iterable:
        yield n, elem
        n += 1
```

> [!seealso] 另加 [[函数高级#生成器函数]]

## `iter` 和 `next`

|                   函数                    | 作用          |
| :-------------------------------------: | :---------- |
| `iter(object)` `iter(object, sentinel)` | 返回一个 迭代器对象  |
|                `next()`                 | 获取迭代器对象的下一项 |

> [!check] `iter()` 的第二个参数影响 `iter()` 的行为
>  - 如果 _没有第二个参数_，`object` 必须是一个**支持 `iterable` 协议** (有 `__iter__()` 方法) 的多项集对象，或者必须**支持序列协议** (有 `__getitem__()` 方法并使用从 0 开始的整数参数)。 如果它不支持这些协议，则会引发 `TypeError`
>  - 如果 _给出了第二个参数 `sentinel`_，则 `object` 必须是一个**可调用对**象。 在这种情况下创建的迭代器将针对每次调用其 `__next__()` 方法不带参数地调用 `object`；如果返回的值等于 `sentinel`，则会引发 `StopIteration`，否则将返回该值

> [!seealso] 另请参阅 [[魔术方法：迭代器协议]]

```python
>> list_iterable = [1, 2, 3, 4, 5, 6, 7, 8]
>>> list_iter = iter(list_iterable)  # 创建迭代器
>>> next(list_iter)
1
>>> next(list_iter)
2
>>> next(list_iter)
3
>>> next(list_iter)
4
>>> next(list_iter)
5
>>> next(list_iter)
6
>>> next(list_iter)
7
>>> next(list_iter)
8
>>> next(list_iter)
Traceback (most recent call last):
  File "<stdin>", line 1, in <module>
StopIteration

>>> dict_iterable = {"name": "dyy", "age": 21}
>>> dict_iter = iter(dict_iterable)  # 字典可迭代对象
>>> next(dict_iter)
'name'
>>> next(dict_iter)
'age'
```

## `zip(*iterables, strict=False)`

在多个迭代器上并行迭代，从每个迭代器返回一个数据项组成元组。

```python
>>> for item in zip([1, 2, 3], ['sugar', 'spice', 'everything nice']):
...     print(item)
...
(1, 'sugar')
(2, 'spice')
(3, 'everything nice')
```

> [!note] 更正确的说法
>  **`zip()` 返回元组的迭代器**，其中第 `i` 个元组包含的是每个参数迭代器的第 `i` 个元素。
>

