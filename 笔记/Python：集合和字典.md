# 集合和字典

Python 中的集合和字典是 [[散列表]] 的实现，每个元素通过 **散列函数** 的到一个散列值，并 **基于散列值将元素插入到散列表中**

## 哈希运算

内置函数 `hash(obj)` 获取可哈希对象(`obj`)的散列值。Python 内置的可哈希类型包括
+ 数值类型：`int` `float` `complex` 
+ 布尔类型：`bool`
+ 字符串：`str`
+ 字节序列：`bytes`
+ 元组：`tuple`
+ NoneType：`None`

### 数值类型的哈希运算

对于不同的数字类型 `x` 和 `y`，当要求 `x == y` 时必定有 `hash(x) == hash(y)`。为了便于在各种数字类型上实现并保证效率，Python 对数字类型的哈希运算是基于 **为任意有理数定义统一的数学函数**，因此该运算对 `int` 和 `Fraction` 的全部实例，以及 `float` 和 `Decimal` 的全部有限实例均可用

> [!tip] 
> 
> 本质上，对于数字类型的哈希运算是通过一个固定 **质数** `P` 进行 `P` **降模** 给出
> 
> CPython 目前所用的质数设定，在 C long 为 $32$ 位机器上 `P = 2 ** 31 - 1` 而在 C long 为 $64$ 位机器上 `P = 2 ** 61 -1`
> 

- 如果 `x = m / n` 是一个非负的有理数且 `n` 不可被 `P` 整除，则定义 `hash(x)` 为 `m * invmod(n, P) % P`，其中 `invmod(n, P)` 是对 `n` 模 `P` 取反。
- 如果 `x = m / n` 是一个非负的有理数且 `n` 可被 `P` 整除（但 `m` 不能）则 `n` 不能对 `P` 降模，以上规则不适用；在此情况下则定义 `hash(x)` 为常数值 `syshash_info.inf`。
- 如果 `x = m / n` 是一个负的有理数则定义 `hash(x)` 为 `-hash(-x)`。 如果结果哈希值为 `-1` 则将其替换为 `-2`。
- 特殊值 `sys.hash_info.inf` 和 `-sys.hash_info.inf` 分别用于正无穷或负无穷的哈希值
- 对于一个 [`complex`](https://docs.python.org/zh-cn/3.13/library/functions.html#complex "complex") 值 `z`，会通过计算 `hash(z.real) + sys.hash_info.imag * hash(z.imag)` 将实部和虚部的哈希值结合起来，并进行降模 `2**sys.hash_info.width` 以使其处于 `range(-2**(sys.hash_info.width - 1), 2**(sys.hash_info.width - 1))` 范围之内。 同样地，如果结果为 `-1` 则将其替换为 `-2`。

### 字符串的哈希运算

Python >= 3.2.3 对 `str` 和 `bytes` 对象的 哈希值会使用一个不可预测的随机值 **加盐**。虽然它们在一个单独 Python 进程中会保持不变，但是它们的值 **在重复运行的 Python 之间是不可预测的**

> [!tip] 
> 这是为了**防止**通过精心选择输入来利用字典插入操作在最坏情况下的执行效率即  $O(n^2)$ 复杂度制度的 **拒绝服务攻击**
> 

```shell
➜ ~ python -c "print(hash('123'))"
4624436169594881861
➜ ~ python -c "print(hash('123'))"
3976487518757174138
➜ ~ python -c "print(hash('123'))"
3344723484242486126
➜ ~ python -c "print(hash('123'))"
-8718639665451025911
```

> [!tip] 环境变量 `PYTHONHASHSEED` Python 哈希种子：默认为 None
> 
> 如果此变量未设置或设为 `random`，将使用一个随机值作为 `str` 和 `bytes` 对象哈希运算的种子
> 

```python
➜  ~ PYTHONHASHSEED=1 python -c "print(hash('123'))"
2322596916611965022
➜  ~ PYTHONHASHSEED=1 python -c "print(hash('123'))"
2322596916611965022
➜  ~ PYTHONHASHSEED=1 python -c "print(hash('123'))"
2322596916611965022
```

## 集合

集合是一张散列表，其中的每个元素只会出现一次。根据元素的 **哈希值** 确定元素是否存放在散列表中。Python 中的集合通常是用于：成员检测、序列去重、数学中的集合计算

> [!attention] 
> 
> + 由于需要计算元素的哈希值，因此集合存储的元素必须是 **可哈希的**
> + 集合中的元素是 **无序的**：原因在于元素存储进哈希表时依赖的元素的哈希值，不同元素的哈希值可能相差非常巨大
> + 集合中的元素是 **唯一的**：如果两个元素的 **哈希值相同** 但是其 **值不同**，称为哈希冲突。集合中的元素的唯一性并 **不是值哈希值唯一**，而是指元素的 **值唯一**
> 

> [!tip] 可哈希的
> 
> 一个对象如果具有在其 **生命期内绝不改变的哈希值**，并 **可以同其他对象进行比较**就被称为 **可哈希** 对象。 可哈希对象必须具有相同的哈希值比较结果才会相等
> 
> 可哈希性使得对象能够作为 **字典键** 或 **集合成员** 使用，因为这些数据结构要在内部使用哈希值
> 

Python 解释器内建了两种集合：`set` 和 `frozenset`。`set` 类型是可变的，其内容可以被修改。`frozenset` 类型是不可变并且是可哈希的，其内容在创建后不能再改变

> [!tip] 
> 
> 与 `set` 相比，`frozenset` 是不可变的且一定是可哈希的。它们都支持
> + `x in set`
> + `len(set)`
> + `for x in set`
> 

Python 中创建 `set` 对象有两种基本的方法：**字面值** 和 **构造函数**

> [!tip] `set` 对象的字面值
> 
> `set` 对象的字面值由花括号 `{...}` 包围的单值。例如 `{'jack', (1, 2, 3)}`
> 
> 请 **注意**：字面值`{}` 并不是构建的空 `set` 对象，而是字典对象。如果需要构建空 `set` 对象请使用构造函数
> 

```python
>>> fruits = {'apple', 'orange', 'apple', 'pear', 'orange', 'banana'}
>>> fruits   # 重复值只保留一个
{'banana', 'apple', 'orange', 'pear'}
>>> for item in fruits:
...     print(item)
...
banana
apple
orange
pear
>>> 'apple' in fruits
True
>>> a = {}
>>> type(a)
<class 'dict'>
>>> {1,2,3, [1]}  # [1] 是列表，是不可哈希的
Traceback (most recent call last):
  File "<python-input-15>", line 1, in <module>
    {1,2,3, [1]}
TypeError: unhashable type: 'list'
```

> [!tip] `set` 对象的构造函数
> 
> 函数 `set(iterable)` 使用 `iterable` 中的值构建一个 `set` 对象，并且`iterable` 中的元素必须是可哈希的
> 

```python
>>> fruits = set(['apple', 'orange', 'apple', 'pear', 'orange', 'banana'])
>>> fruits
{'banana', 'apple', 'orange', 'pear'}
>>> 'orange' in fruits
True
>>> for item in fruits:
...     print(item)
...
banana
apple
orange
pear
>>> a = set()
>>> type(a)
<class 'set'>
```


> [!attention] 集合也支持推导式
> 
> 类似 [[Python：列表和元组#列表推导式]]，集合同样支持推导式
> 
> 集合生成式中，**最后放入集合的元素必须是可哈希的**
> 

```python
>>> a = {x for x in 'abracadabra' if x not in 'abc'} 
>>> a
{'d', 'r'}
>>> {(x, y) for x in "abcd" for y in range(3)}  # 12 项 元祖的集合
{('b', 2), ('c', 0), ('a', 0), ('d', 0), ('d', 1), ('b', 1), ('c', 2), ('a', 2), ('d', 2), ('b', 0), ('a', 1), ('c', 1)}
>>> {(a,) for a in "abcabc"} # 3 项 元祖的集合
{('c',), ('a',), ('b',)}
>>> {(a) for a in "abcabc"}  # 3 项 str 类型的集合
{'c', 'a', 'b'}

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


### 数学运算

`set` 和 `frozenset` 均支持下列操作，这些操作用于执行两个集合之间的数学运算

| 运算符     | 方法                                | 描述                       |
| :------ | --------------------------------- | ------------------------ |
| `&`     | `set.intersection(*others)`       | 求两个集合的交集                 |
| &#124   | `set.union(*others)`              | 求两个集合的并集                 |
| `-`     | `set.difference(*others)`         | 求两个集合的差集                 |
| `^`     | `set.symmetric_difference(other)` | 求两个集合的对称差                |
|         | `set.isdijoint(other)`            | 判断两个集合是否存在交集             |
| `<, <=` | `set.issubset(other)`             | 判断 `set` 是否是 `other` 的子集 |
| `>, >=` | `set.issuperset(other)`           | 判断 `set` 是否是 `other` 的超集 |

```python
# 求交集
>>> {1,2,3} & {2, 3, 4}
{2, 3}
>>> {1,2,3}.intersection({2, 3, 4})
{2, 3}

# 求并集
>>> {1,2,3} | {2, 3, 4}
{1, 2, 3, 4}
>>> {1,2,3}.union({2, 3, 4})
{1, 2, 3, 4}

# 求差集
>>> {1,2,3} - {2, 3, 4}
{1}
>>> {2, 3, 4} - {1,2,3}
{4}
>>> {2, 3, 4}.difference({1,2,3})
{4}
>>> {1,2,3}.difference({2, 3, 4})
{1}

# 求对称差
>>> {1,2,3} ^ {2, 3, 4}
{1, 4}
>>> {1,2,3}.symmetric_difference({2, 3, 4})
{1, 4}
```

### 更新集合

`set` 对象提供了下列方法用于修改对象

| 方法                                       | 描述                              |
| :--------------------------------------- | :------------------------------ |
| `set.add(value)`                         | 将 `value` 插入到 `set` 中           |
| `set.update(*others)`                    | 将 `others` 中的元素插入到 `set` 中      |
| `set.intersection_update(*others)`       | 保留 `set` 中在 `others` 中均存在的元素    |
| `set.difference_update(*others)`         | 移除 `set` 中在 `others` 中存在的元素     |
| `set.symmetric_difference_update(other)` | 保留 `set` 和 `other` 的一方而非共同存在的元素 |

### 移除集合中的元素

`set` 对象提供下列方法用于移除集合中的元素

| 方法                  | 描述                                            |
| :------------------ | --------------------------------------------- |
| `set.remove(elem)`  | 从 `set` 中移除 `elem`，如果 `elem` 不存在触发 `KeyError` |
| `set.discard(elem)` | 与 `set.remove(elem)` 类似。只是不会触发 `KeyError`     |
| `set.pop()`         | 从 `set` 中随机移除并返回一个元素                          |
| `set.clear()`       | 清空 `set`                                      |

## 字典

另一个非常有用的 Python 内置数据类型是 **字典**，它是一种 **映射类型**。 字典是以 **键** 进行索引的，**键可以是任何不可变类型**；**字符串** 和 **数字** 总是可以作为键

> [!tip] 映射
> 
> **映射对象** 会将 **可哈希对象** 映射到 **任意对象**。 **映射属于可变对象**。 目前仅有一种标准映射类型 **字典(`dict`)**
> 

> [!attention] 
> 
> 注意，当使用元组作为键时，元组中的所有元素都必须是可哈希的。只要元组中的任意一个元素是不可哈希的，则该元组对象就是不可哈希的
> 

可以把字典理解为 **_键值对_ 的集合**，并且 **键必须是唯一的**。Python 中创建字典使用 **字面值** 和 **构造函数**

> [!tip] `dict` 对象的字面值
> 
> Python 中 `dict` 对象通过花括号 `{...}` 标识，其中的每个元素是逗号分隔 `key:value` 组成。例如 `{'name': '杜宇鹏', 'age': 28}`
> 

```python
>>> person = {"name": "杜宇鹏", "age": 28}
>>> person
{'name': '杜宇鹏', 'age': 28}
```

> [!tip] `dict` 对象的构造函数
> 
> 构造函数 `dict(iterable)` 或者 `dict(key=value,...)` 用于创建一个全新的字典。其中 `iterable` 是 `key:value` 的可迭代对象
> 

```python
>>> dict(zip(['one', 'two', 'three'], [1, 2, 3]))
{'one': 1, 'two': 2, 'three': 3}
>>> dict([('two', 2), ('one', 1), ('three', 3)])
{'two': 2, 'one': 1, 'three': 3}
>>> dict({'one': 1, 'three': 3}, two=2)
{'one': 1, 'three': 3, 'two': 2}
>>> dict(one=1, two=2, three=3)
{'one': 1, 'two': 2, 'three': 3}
```

> [!attention] 字典推导式
> 
> **字典推导式** 可以用任意键值表达式创建字典
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

### 获取值

字典的一个常规操作就是通过 `key` 从字典中获取值。Python 中的 `dict` 提供了下面几种方法获取值

| 方法                               | 描述                                                        |
| :------------------------------- | --------------------------------------------------------- |
| `dict[key]`                      | 从 `dict` 中获取 `key` 对应的值。`key` 不存在触发 `KeyError`            |
| `dict.get(key,defalue=None)`     | 从 `dict` 中获取 `key` 对应的值。`key` 不存在返回 `default`             |
| `dict.setdefault(key[,defalut])` | 从 `dict` 中获取 `key` 对应的值。 `key` 不存在添加 `key` 值由 `defalt` 指定 |
| `dict.values()`                  | 返回由 `dict` 中的 `value` 构成的字典视图对象                           |

### 获取键

知道字典中存储的 `key` 是非常有必要的。Python 提供下列方法获取字典中的所有 `key`

| 方法            | 描述                           |
| :------------ | ---------------------------- |
| `list(dict)`  | 返回由 `dict` 中 `key` 构成的列表     |
| `tuple(dict)` | 返回由 `dict` 中 `key` 构成的元组     |
| `dict.keys()` | 返回由 `dict` 中 `key` 构成的字典视图对象 |

> [!tip] 
> 
> 当同时需要 `key` 和 `value` 时，方法 `dict.items()` 会返回 `(key, value)` 形式的字典视图对象

### 删除键

Python 提供了 $3$ 个方法从 `dict` 中删除 `key`

| 方法                        | 描述                                                                                       |
| :------------------------ | ---------------------------------------------------------------------------------------- |
| `del dict[key]`           | 从 `dict` 中删除 `key`。`key` 不存在则触发 `KeyError`                                               |
| `dict.pop(key[,default])` | 从 `dict` 中删除 `key` 并返回 `value`。`key` 不存在时，返回 `default`；`default` 为 `None` 则触发 `KeyError` |
| `dict.popitem()`          | 从 `dict` 中随机删除一个 `key:value` 并返回。Python 3.7 以插入顺序采用 `LIFO` 顺序删除                          |

```python
>>> dict1 = {"name":"dyp"}
>>> dict1 |= {"age":18}
>>> dict1 |= {"sex":1}
>>> dict1 |= {"hobby":"看电视"}
>>> dict1.popitem()
('hobby', '看电视')
>>> dict1.popitem()
('sex', 1)
>>> dict1.popitem()
('age', 18)
>>> dict1.popitem()
('name', 'dyp')
```

### 字典视图对象

由 `dict.keys()`,` dict.values()` 和 `dict.items()` 所返回的对象是 **视图对象**。 该对象提供 **字典条目的一个动态视图**

> [!tip] 动态视图：当字典改变时，视图也会相应改变
> 

```python
>>> dict1 = {"one":1, "two":2, "three":3}
>>> keys = dict1.keys()
>>> keys
dict_keys(['one', 'two', 'three'])
>>> dict1 |= {"four": 4}  # 添加 "four"
>>> keys  # keys 中增加了 "four"
dict_keys(['one', 'two', 'three', 'four'])
```

字典视图 **可以被迭代** 以产生与其对应的数据，并 **支持成员检测**。下表列出了字典视图的操作

| 操作                   | 描述                                                              |
| :------------------- | :-------------------------------------------------------------- |
| `len(dictview)`      | 返回字典中的条目数                                                       |
| `iter(dictview)`     | 返回字典中的键、值或项（以 `(键, 值)` 为元素的元组表示）的迭代器                            |
| `x in dictview`      | 如果 `x` 是对应字典中存在的键、值或项（在最后一种情况下 `x` 应为一个 `(键, 值)` 元组） 则返回 `True` |
| `reversed(dictview)` | 返回一个逆序获取字典键、值或项的迭代器。 视图将按与插入时相反的顺序进行迭代                          |
| `dictview.mapping`   | 返回 `types.MappingProxyType` 对象，封装了字典视图指向的原始字典                   |

> [!important] 
> 
> **字典是无序的**。但是，从 Python3.6 开始，它 **记录了 `key` 的插入顺序**，迭代时采用插入顺序进行迭代 
> 

### 更新字典

将一个键值对插入 `dict` 可以使用两个方法

| 方法                   | 描述                                  |
| :------------------- | ----------------------------------- |
| `dict[key] = value`  | 将 `key:value` 插入到 `dict` 中          |
| `dict.update(other)` | 使用 `other` 中的 `key:value` 更新 `dict` |

```python
>>> dict1 = {"one":1, "two":2, "three":3}
>>> dict1.update({"four":4})
>>> dict1
{'one': 1, 'two': 2, 'three': 3, 'four': 4}
```

### defaultdict

在 `collections` 包中提供了一个 `defaultdict` 类型，它是 `dict` 的子类，通过调用用户指定的 **工厂函数**，**为键提供默认值**

```python
>>> from collections import defaultdict
>>> d = defaultdict(list)  # list 是一个函数，当查找键不存在时自动调用
>>> d['a'].append(10)
>>> d
defaultdict(<class 'list'>, {'a': [10]})
>>> d['b']  # 自动调用 list 创建一个空列表
[]
>>> d
defaultdict(<class 'list'>, {'a': [10], 'b': []})
```
