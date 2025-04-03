# 内置函数

我们已经学习了大部分的内置函数了。首先对这些已经学习的内置函数做个总结

| 函数                                | 描述                       |
| :-------------------------------- | :----------------------- |
| `print(*args, sep=' ', end='\n')` | 输出 `args` 中对象的字符串表示      |
| `pow(x, y, z)`                    | 等价于 `x ** y % z`         |
| `divmod(x, y)`                    | 返回 `(x // y, x % y)`     |
| `round()`                         | 按照 **四舍六入五凑偶** 规则进行舍入    |
| `bin()` `oct()` `hex()`           | 返回整数二进制 八进制 十六进制字符串      |
| `bool()`                          | 返回对象等价的布尔值               |
| `int()`                           | 返回 `int` 类型的对象           |
| `float()`                         | 返回 `float` 类型的对象         |
| `complex()`                       | 返回 `complex` 类型的对象       |
| `str()`                           | 返回 `str` 类型的对象           |
| `type()`                          | 获取对象所属的类型                |
| `id()`                            | 获取对象的表示，CPython 为对象的内存地址 |
| `isinstance(obj, type)`           | 检查 `obj` 是否是类 `type` 的实例 |
| `range()`                         | 返回 `range` 对象            |
| `enumerate(iterable, start=0)`    | 返回 `enumerate` 对象        |
| `list()`                          | 返回 `list` 类型的对象          |
| `tuple()`                         | 返回 `tuple()` 类型的对象       |
| `bytes()`                         | 返回 `bytes` 类型的对象         |
| `bytearray()`                     | 返回 `bytearray` 类型的对象     |
| `chr()`                           | 返回 Unicode 码值对应的字符       |
| `ord()`                           | 返回字符对应的 Unicode 码值       |
| `set()`                           | 返回 `set` 类型的对象           |
| `frozenset()`                     | 返回 `frozenset` 类型的对象     |
| `dict()`                          | 返回 `dict` 类型的对象          |
| `slice()`                         | 返回用于切片的对象                |
| `len()`                           | 获取容器的长度                  |
| `sorted()`                        | 返回一个已排序的列表               |
| `reversed()`                      | 返回一个列表对象，元素是序列的反转        |
| `iter()`                          | 返回一个迭代器对象                |
| `next()`                          | 获取迭代器对象的下一个值             |
| `dir()`                           | 返回对象的属性字典                |

## zip() 函数

内置函数 `zip(*iterables, strict=False)` 返回一个迭代器，在多个可迭代对象上并行迭代，从每个迭代器返回一个数据项组成元组

```python
>>> for key, value in zip(("name", "age", "gender"), ("张三", 19, "male")):
...     print(key, value)
...
name 张三
age 19
gender male
```

`zip()` 返回元组的迭代器，其中第 $i$ 个元组包含的是 `iterables` 中的每个可迭代对象的第 $i$ 个元素

> [!attention] 
> 
> 值得考虑的是，传给 `zip()` 的可迭代对象可能长度不同；有时是有意为之，有时是因为准备这些对象的代码存在错误
> 

Python 提供了三种不同的处理方案用于处理 `iterables` 中的可迭代对象长度不同的情况

> [!tip] 默认情形：迭代完最短可迭代对象后停止
> 
> 默认情况下，`zip()` 在最短的迭代完成后停止。较长可迭代对象中的剩余项将被忽略，结果会裁切至最短可迭代对象的长度
> 

> [!tip] 严格要求：严格要求可迭代对象长度相同
> 
> 指定参数 `strict` 为 `True`，此时，如果 `iterables` 中的任何一个可迭代对象先一步迭代完成，那么将抛出 `ValueError`
> 

```python
>>> for item in zip(range(3), ['fee', 'fi', 'fo', 'fum'], strict=True):
...     print(item)
...
(0, 'fee')
(1, 'fi')
(2, 'fo')
Traceback (most recent call last):
  File "<python-input-3>", line 1, in <module>
    for item in zip(range(3), ['fee', 'fi', 'fo', 'fum'], strict=True):
                ~~~^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
ValueError: zip() argument 2 is longer than argument 1
```

> [!tip] 适应最长：让短的可迭代对象适应长的可迭代对象
> 
> 在标准库 `itertools` 中提供了一个 `zip_longest(*iterables, fillvalue=None)`，对于较短的可迭代对象，会使用 `fillvalue` 进行填充
> 

## globals() 和 locals() 函数

在 [[Python：函数基础#名字空间与作用域]] 中我们介绍了名字空间的概念，Python 提供两个函数 `globals()` 和 `locals()` 用于返回 **模块全局名字空间** 和 **局部名字空间** 的字典

## abs() 函数

函数 `abs()` 返回参数的绝对值。其中参数可以 `int` `float` `complex` 以及任何实现了 `__abs__()` 特殊方法的类实例

## all() 和 any() 函数

内置函数 `all(iterable)` 检查可迭代对象 `iterable` 中的元素的逻辑值是否均为 `True`；而内置函数 `any(iterable)` 检查可迭代对象 `iterable` 中的元素是的逻辑值是否存在 `True`

`all()` 和 `any()` 函数等价于下面的实现

```python
def all(iterable):
    for element in iterable:
        if not element:
            return False
    return True

def any(iterable):
    for element in iterable:
        if element:
            return True
    return False
```

## exec() 和 eval() 函数

函数 `eval(source, /, globals=None, locals=None)` 在给定的环境中执行 `source`

> [!attention] 
> 
> 参数 `source` 必须是 Python 的 **表达式** 或者是由函数 `compile()` 返回  **code object**
> 
> 参数 `globals` 必须是字典类型的对象，默认情况下是 `globals()` 的返回值
> 
> 参数 `locals` 可以是任何映射类型，默认情况下是 `locals()` 的返回值。如果指定了参数  `globals` 但是没有指定参数 `locals` ，则参数 `locals` 为 `globals` 
> 

```python
>>> x = 10
>>> eval("x + 10")  # 默认从 globals() 中解析名字
20

>>> eval("x + 10", {}, {'x': 1})
11

>>> eval("x = x + 10")
Traceback (most recent call last):
  File "<python-input-16>", line 1, in <module>
    eval("x = x + 10")
    ~~~~^^^^^^^^^^^^^^
  File "<string>", line 1
    x = x + 10
      ^
SyntaxError: invalid syntax
```

在 `eval()` 函数中，是不能修改 `globals` 或 `locals` 中的变量的。然而，函数 `exec(source, /, globals=None, locals=None, *, closure=None)` 可以执行任意的 Python 表达式或 **code object**，包括对环境中变量的修改

```python
>>> x = 10
>>> exec("x += 10")
>>> x
20
```
