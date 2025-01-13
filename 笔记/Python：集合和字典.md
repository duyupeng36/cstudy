# 集合和字典

Python 中的 **集合** 和 **字典** 是 [[散列表]] 的一种实现

## 集合 `set`

Python 还支持 **_集合_** 这种数据类型，它是 **可变的** 容器类型。集合是由 _不重复_ 元素组成的 _无序容器_
- 基本用法 **包括成员检测**、**消除重复元素**、**迭代容器元素**
- 集合对象支持 **合集**、**交集**、**差集**、**对称差分** 等数学运算

> [!important] 集合是不可索引的

创建集合用花括号或 `set()`  函数。注意，**创建空集合只能用 `set()`**，不能用 `{}`，**`{}` 创建的是空字典**

```python
>>> basket = {'apple', 'orange', 'apple', 'pear', 'orange', 'banana'}
>>> basket
{'apple', 'pear', 'orange', 'banana'}
>>> "orange" in basket  # 成员检测
True
>>> "crabgrass" in basket
False
>>> a = set('abracadabra')
>>> b = set('alacazam')
>>> a
{'c', 'd', 'a', 'b', 'r'}
>>> b
{'z', 'c', 'a', 'l', 'm'}
>>> a - b   # 求两个集合的差集
{'d', 'r', 'b'}
>>> a | b   # 按位或，求两个集合的并集
{'z', 'c', 'd', 'a', 'l', 'b', 'm', 'r'}
>>> a & b  # 按位与，求两个集合的交集
{'a', 'c'}
>>> a ^ b  # 异或，两个集合都有的元素被剔除，剩余元素合并为一个集合
{'z', 'l', 'b', 'd', 'm', 'r'}
```

> [!important] 集合有一下几个特性:  
> 1.  **无序性**: 集合中的元素是没有顺序,
> 2.  **唯一性**: 同一个元素在集合中只存在一个
> 3.  集合是**可变数据类型**, 但是集合中的 **元素只能存放不可变(可哈希)的数据.**  如果存放了可变类型数据, 会触发`TypeError`异常

> [!danger] 集合中的元素必须是 **可哈希的**
> - 集合中的元素只要出现了不可哈希对象就会触发 `TypeError`
> 
> Python 的内置函数 `hash()` 可以返回一个对象的 **哈希值**。

```python
>>> {(1,2,[1])}
Traceback (most recent call last):
  File "<stdin>", line 1, in <module>
TypeError: unhashable type: 'list'
```

---
Python >= 3.2.3 对 `str` 和 `bytes` 对象的 哈希值会使用一个不可预测的随机值 _加盐_。虽然它们在一个单独 Python 进程中会保持不变，但是它们的值在重复运行的 Python 之间是不可预测的

> [!info] 原因
> 这是为了**防止**通过精心选择输入来利用字典插入操作在最坏情况下的执行效率即  $O(n^2)$ 复杂度制度的 **拒绝服务攻击**

```shell
dyp@ubuntu:~$ python -c "print(hash('123'))"
-8446606495242098355
dyp@ubuntu:~$ python -c "print(hash('123'))"
-9111418234901541349
dyp@ubuntu:~$ python -c "print(hash('123'))"
-6107098491586316605
```

改变哈希值会影响集合的迭代次序。Python 也从不保证这个次序不会被改变（通常它在 32 位和 64 位构建上是不一致的）

> [!info] 环境变量 `PYTHONHASHSEED` Python 哈希种子：默认为 None
> 如果此变量未设置或设为 `random`，将使用一个随机值作为 `str` 和 `bytes` 对象哈希运算的种子

```shell
dyp@ubuntu:~$ PYTHONHASHSEED=1 python -c "print(hash('123'))"
8874197587556847498
dyp@ubuntu:~$ PYTHONHASHSEED=1 python -c "print(hash('123'))"
8874197587556847498
dyp@ubuntu:~$ PYTHONHASHSEED=1 python -c "print(hash('123'))"
8874197587556847498
```

### 集合之间的运算(数学运算)

#### 求两个集合的交集

交集: `&` 或 `set.intersection(*others)`: 返回一个新集合，其中包含原集合以及 `others` 指定的所有集合中共有的元素

```python
>>> {1,2,3} & {2, 3, 4}
{2, 3}
>>> {1,2,3}.intersection({2, 3, 4})
{2, 3}
```

#### 求两个集合的并集

并集: `|` 或 `set.union(*others)`: 返回一个新集合，其中包含来自原集合以及 `others` 指定的所有集合中的元素

```python
>>> {1,2,3} | {2, 3, 4}
{1, 2, 3, 4}
>>> {1,2,3}.union({2, 3, 4})
{1, 2, 3, 4}
```

#### 求两个集合的差集

差集(相对补集): `-` 或 `set.difference(*others)`: ​	返回一个新集合，其中包含原集合中在 `others` 指定的其他集合中不存在的元素

```python
>>> {1,2,3} - {2, 3, 4}
{1}
>>> {2, 3, 4} - {1,2,3}
{4}
>>> {2, 3, 4}.difference({1,2,3})
{4}
>>> {1,2,3}.difference({2, 3, 4})
{1}
```

#### 求两个集合的对称差

对称差集: `^` 或 `set.symmetric_difference(other)`: 返回一个新集合，其中的元素或属于原集合或属于 `other` 指定的其他集合，但不能同时属于两者

```python
>>> {1,2,3} ^ {2, 3, 4}
{1, 4}
>>> {1,2,3}.symmetric_difference({2, 3, 4})
{1, 4}
```

#### 判断一个集合是否是另一个集合的子集

判断 `set` 是否是 `other` 的交集: `set.isdijoint(other)`: 如果集合中没有与 `other` 共有的元素则返回 `True`

```python
>>> {1,2, 3}.isdisjoint({4,5,6})
True
>>> {1,2, 3}.isdisjoint({3, 4,5,6})
False
```

判断 `set` 是否是 `other` 的子集:  `set.issubset(other)` 或 `set <= other` 或 `set < other`: 检测集合`set`是否是`other`的子集
```python
>>> {1,2,3} <= {1,2}
False
>>> {1,2,3} <= {1,2,3,4}
True
>>> {1,2,3} < {1,2,3,4}
True
```

#### 判断一个集合是否是另一个集合的超集

判断 `set` 是否是 `other` 的超集:  `set.issuperset(other)` 或 `set >= other` 或 `set > other`: 判断集合`set`是否为`other`的超集
```python
>>> {1,2,3} > {1,2}
True
>>> {1,2,3} >{1,2,4}
False
```

> [!note] 不可变的集合 `frozenset`
> 上述方法也支持 `frozenset`。下面的方法是 `set` 类型独有的方法


### 向集合中添加元素

`set.add(elem)`: 将元素 `elem` 添加到集合中

```python
>>> _set
{4, 5, 7, 8}
>>> _set.add(9)
>>> _set
{4, 5, 7, 8, 9}
```

### 更新集合

 `set.update(*others)` 或 `set |= other | ...`: 更新集合，添加来自 `others` 中的所有元素
 
```python
>>> _set = {1,2,3}
>>> _set |= {1,2,3,4}
>>> _set
{1, 2, 3, 4}
>>> _set.update({1,2,3,4,5,6})
>>> _set
{1, 2, 3, 4, 5, 6}
```

`set.intersection_update(*others)` 或 `set &= other & ...`: 更新集合，只保留其中在所有`others`中也存在的元素

```python
>>> _set
{1, 2, 3, 4, 5, 6, 7, 8, 9, 20}
>>> _set &= {9, 1, 3, 5}
>>> _set
{9, 3, 5, 1}
>>> _set.intersection_update({8, 9, 1, 3})
>>> _set
{9, 3, 1}
```

`set.difference_update(*others)` 或 `set -= other | ...`: 更新集合，移除其中也存在于 `others` 中的元素

```python
>>> _set
{9, 3, 1}
>>> _set -= {1,2,3,4,5}
>>> _set
{9}

>>> _set = {1,2,3,4,5,6}
>>> _set.difference_update({1,2,3})
>>> _set
{4, 5, 6}
```

`set.symmetric_difference_update(other)` 或 `set ^= other`: 更新集合，只保留存在于 `set` 和 `other` 的一方而非共同存在的元素

```python
>>> _set = {4, 5, 6}
>>> _set ^= {6, 7, 8}
>>> _set
{4, 5, 7, 8}
```

### 移除集合元素

`set.remove(elem)`: 从集合中移除元素 `elem`。 如果 `elem` 不存在于集合中则会引发 `KeyError`

```python
>>> _set
{4, 5, 7, 8, 9}
>>> _set.remove(5)
>>> _set
{4, 7, 8, 9}
```

 `set.discard(elem)`: 如果元素 `elem` 存在于集合中则将其移除

```python
>>> _set
{4, 7, 8, 9}
>>> _set.discard(10)  # 不存在也不会触发异常
>>> _set.discard(90)
>>> _set.discard(9)
>>> _set
{4, 7, 8}
```

`set.pop()`: 从集合中移除并返回任意一个元素。 如果集合为空则会引发 `KeyError`

```python
>>> _set
{4, 7, 8}
>>> _set.pop()
4
>>> _set
{7, 8}
```

`set.clear()`: 从集合中移除所有元素

```python
>>> _set
{7, 8}
>>> _set.clear()
>>> _set
set()
```

## 可哈希

Python 中，如下类型的对象时可哈希的
+ 数值型 `int`、`float`、`complex`
+ 布尔型 `True`、`False`
+ 字符串 `string`、`bytes`
+ `tuple`
+ `None`

> [!important] **不可变类型** 都是可哈希类型

## 字典 `dict`


另一个非常有用的 Python 内置数据类型是 **_字典_**，它是一种 **映射类型**。 字典是以 **键** 进行索引的，**键可以是任何不可变类型**；**字符串** 和 **数字** 总是可以作为键

> [!note] mapping
> `mapping` 对象会将 `hashable` 值映射到任意对象。 **映射属于可变对象**。 目前仅有一种标准映射类型 **字典**

> [!NOTE] 注意
> **如果一个元组只包含字符串、数字或元组则也可以作为键**
> 
> 如果一个 **元组直接或间接地包含了任何可变对象**，则 **不能作为键**
> 
> **列表不能作为键**，因为列表可以使用索引赋值、切片赋值或者 `append()` 和 `extend()` 等方法进行原地修改列表

**可以把字典理解为 _键值对_ 的集合**，但字典的 **键必须是唯一的**。**花括号 `{}` 用于创建空字典**。另一种初始化字典的方式是，**在花括号里输入逗号分隔的键值对**，这也是字典的输出方式

**字典的主要用途是通过关键字存储、提取值**。用 **`del` 可以删除键值对**。用已存在的关键字存储值，与该关键字关联的旧值会被取代。**通过不存在的键提取值，则会报错**

**对字典执行 `list(d)` 操作，返回该字典中所有键的列表**，**按插入次序排列**（如需排序，请使用 `sorted(d)`）。**检查字典里是否存在某个键，使用关键字 `in`**

```python
>>> tel = {'jack': 4098, 'sape': 4139}
>>> tel['guido'] = 4127   # 添加新的键值对
>>> tel
{'jack': 4098, 'sape': 4139, 'guido': 4127}
>>> del tel['sape']  # 删除已存在的键值对
>>> tel["irv"] = 4127  # 添加新的键值对
>>> tel
{'jack': 4098, 'guido': 4127, 'irv': 4127}
>>> list(tel)   # 将字典转换为列表
['jack', 'guido', 'irv']
>>> sorted(tel)  # 对键进行排序
['guido', 'irv', 'jack']
>>> 'guido' in tel  # 测试 键 是否存在
True
>>> 'jack' not in tel  # 测试 键 是否不存在
False
```

### 构造字典的其他方式
```python
>>> a = dict(one=1, two=2, three=3)
>>> b = {'one': 1, 'two': 2, 'three': 3}
>>> c = dict(zip(['one', 'two', 'three'], [1, 2, 3]))
>>> d = dict([('two', 2), ('one', 1), ('three', 3)])
>>> e = dict({'three': 3, 'one': 1, 'two': 2})
>>> f = dict({'one': 1, 'three': 3}, two=2)
>>> a == b == c == d == e == f
True
```

`dict()` 构造函数可以直接用键值对序列创建字典

```python
>>> dict([('sape', 4139), ('guido', 4127), ('jack', 4098)])
{'sape': 4139, 'guido': 4127, 'jack': 4098}
```

关键字是比较简单的字符串时，**直接用关键字参数指定键值对更便捷**：

```python
>>> dict(sape=4139, guido=4127, jack=4098)
{'sape': 4139, 'guido': 4127, 'jack': 4098}
>>> dict({"name": "dyp", "age": 19})
{'name': 'dyp', 'age': 19}
```

### 字典所支持的操作

如果需要自定义映射类型，下面的这些操作也应该支持，假设 `d` 是一个字典类型对象

|               操作               | 描述                                                                           | 备注                                                                          |
| :----------------------------: | :--------------------------------------------------------------------------- | --------------------------------------------------------------------------- |
|           `list(d)`            | 返回字典 `d` 中使用的所有键的列表                                                          | 按添加键的顺序                                                                     |
|            `len(d)`            | 返回字典 `d` 中的项数                                                                |                                                                             |
|            `d[key]`            | 返回 `d` 中以 `key` 为键的项                                                         | 没有 `key` 触发 `KeyError`                                                      |
|        `d[key] = value`        | 将 `d[key]` 设为 `value`                                                        |                                                                             |
|          `del d[key]`          | 将 `d[key]` 从 `d` 中移除                                                         | 没有 `key` 触发 `KeyError`                                                      |
|           `key in d`           | 如果 `d` 中存在键 `key` 则返回 `True`，否则返回 `False`                                    |                                                                             |
|         `key not in d`         | 等价于 `not key in d`                                                           |                                                                             |
|           `iter(d)`            | 返回以字典的键为元素的迭代器                                                               | 这是 `iter(d.keys())` 的快捷方式                                                   |
|          `d.clear()`           | 移除字典中的所有元素                                                                   |                                                                             |
|           `d.copy()`           | 返回原字典的浅拷贝                                                                    |                                                                             |
|  `fromkey(iterable[, value])`  | 使用来自 `iterable` 的键创建一个新字典，并将键值设为 `value`，默认值为 `None`                         | 返回新字典的 **类方法**                                                              |
|    `d.get(key[, default])`     | 获取 `key` 的值                                                                  | 没有 `key` 返回 `default`。默认为 `None`                                            |
|          `d.items()`           | 返回又字典项 `(key, value)` 形成的新视图                                                 |                                                                             |
|           `d.keys()`           | 返回由字典键组成的一个新视图                                                               |                                                                             |
|     `d.pop(key[,default])`     | 如果 `key` 存在于字典中则将其 **移除并返回其值**，否则返回 `default`                                | `default` 未给出且 `key` 不存在于字典中，则会引发 `KeyError`                                |
|         `d.popitem()`          | 从字典中移除并返回一个`(key, value)`对                                                   | **在 3.7 版本更改为**: 键值对会按 `LIFO` 的顺序被返回                                        |
|         `reversed(d)`          | 返回一个逆序获取字典键的迭代器                                                              | 这是 `reversed(d.keys())` 的快捷方式。在 3.8 版本添加                                    |
| `d.setdefault(key[, default])` | 如果字典存在键 `key` ，返回它的值。如果不存在，插入值为 `default` 的键 `key` ，并返回 `default`，默认为 `None` |                                                                             |
|      `d.update([other])`       | 使用来自 `other` 的 `(key, value)` 更新字典，覆盖原有的键                                    | 可以接受字典，二元元组，关键字参数                                                           |
|          `d.values()`          | 返回由字典值组成的一个新视图                                                               | 两个 `dict.values()` 视图之间的相等性比较将总是返回 `False`。 这在 `dict.values()` 与其自身比较时也同样适用 |
|         d $\mid$ other         | 合并 `d` 和 `other` 中的键和值来创建一个**新的字典**，两者必须都是字典                                 | 当 `d` 和 `other` 有相同键时， `other` 的值优先。_在 3.9 版本添加_                            |
|        d $\mid$= other         | 用 `other` 的键和值 **更新字典 `d`**                                                  | 类似于 `d.update(other)`                                                       |

```python
>>> map = {'one': 1, 'two': 2, 'three': 3}
>>> len(map)  # 字典中的项数
3
>>> list(map)  # 返回字典key的列表
['one', 'two', 'three']
>>> map['one']  # 获取字典中间对应的值，字典中没有触发 KeyError
1
>>> map['one'] = "一"  # 设置键的值
>>> map
{'one': '一', 'two': 2, 'three': 3}
```

### 查看

`dict[key]`：返回 `d` 中以 `key` 为键的项，没有 `key` 触发 `KeyError`

`dict.get(key[, default])`: 获取字典`key`对应的`value`值. key不存在时，返回`default`默认为`None`

`dict.setdefault(key[, default])`: 返回对应 `key` 的 `value` 值；如果`key`不存在则添加 `key` 且值为 `default`，默认为None，同时会返回 `default` 的指针

```python
>>> dict2
{'name': 'dyp', 'age': 19}
>>> dict2.get("name")
'dyp'
>>> dict2.setdefault("school", "sit")
'sit'
>>> dict2
{'name': 'dyp', 'age': 19, 'school': 'sit'}
```

下面几个方法通常用于遍历

`dict.keys()`: 返回字典`keys`构成的 **字典视图对象**

`dict.values()`: 返回字典`value`值构成的 **字典视图对象**

`dict.items()`: 返回(`(键, 值)` 对) 组成的一个 **字典视图对象**

```python
>>> dict2
{'name': 'dyp', 'age': 19}
>>> dict2.keys()
dict_keys(['name', 'age'])
>>> dict2.values()
dict_values(['dyp', 19])
>>> dict2.items()
dict_items([('name', 'dyp'), ('age', 19)])
```


有时候，我需要删除字典中满足某些条件的 `item`，在遍历过程中一定不能移除字典中的 `item`

> [!warning] 不要在遍历过程中改变字典的长度
> 遍历过程中修改容器的长度是非常危险的操作

```python
>>> for k in dict1.keys():
...     dict1.pop(k)  # 在遍历过程中删除字典中的元素，会触发 RuntimeError
...
1
Traceback (most recent call last):
  File "<stdin>", line 1, in <module>
RuntimeError: dictionary changed size during iteration
```

> [!success] 记录待删除的 `key`，字典变量完成后，在执行删除

##### 字典视图对象

由 `dict.keys()`,` dict.values()` 和 `dict.items()` 所返回的对象是 视图对象。 该对象提供 **字典条目的一个动态视图**

> [!important] 当字典改变时，视图也会相应改变

```python
>>> dict1 = {"one":1, "two":2, "three":3}
>>> keys = dict1.keys()
>>> keys
dict_keys(['one', 'two', 'three'])
>>> dict1 |= {"four": 4}  # 添加 "four"
>>> keys  # keys 中增加了 "four"
dict_keys(['one', 'two', 'three', 'four'])
```

> [!tip] 字典视图 **可以被迭代** 以产生与其对应的数据，并 **支持成员检测**

|          操作          | 描述                                                              | 备注                                    |
| :------------------: | :-------------------------------------------------------------- | ------------------------------------- |
|   `len(dictview)`    | 返回字典中的条目数                                                       |                                       |
|   `iter(dictview)`   | 返回字典中的键、值或项（以 `(键, 值)` 为元素的元组表示）的迭代器                            | 键和值是 _按插入时的顺序进行迭代_ 的。**在 3.7 版本发生变更** |
|   `x in dictview`    | 如果 `x` 是对应字典中存在的键、值或项（在最后一种情况下 `x` 应为一个 `(键, 值)` 元组） 则返回 `True` |                                       |
| `reversed(dictview)` | 返回一个逆序获取字典键、值或项的迭代器。 视图将按与插入时相反的顺序进行迭代                          | **在 3.8 版本发生变更**: 字典视图现在是可逆的。         |
|  `dictview.mapping`  | 返回 `types.MappingProxyType` 对象，封装了字典视图指向的原始字典                   | **在 3.10 版本添加**                       |

**`key` 视图与集合类似** 因为其条目是唯一的并且为可哈希的。 `item` 视图也有类似集合的操作因为 `(键, 值)` 对是唯一的并且键是可哈希的。 如果 `item` 视图中的所有值也都是可哈希的，那么 `item` 视图就可以与其他集合执行互操作

> [!info] **字典是无序的**。但是，从 Python3.6 开始，它 **记录了 `key` 的插入顺序**，迭代时采用插入顺序进行迭代 

###  添加 和 修改

`dict[key] = value`

`dict.update(other_dict)`: 将 `other_dict` 中的值添加到字典中，如果 `key` 相同则覆盖

```python
>>> dict2.update({"sex": 1})
>>> dict2
{'name': 'dyp', 'age': 19, 'sex': 1}
```

### 移除元素

`del dict[key]`

`dict.pop(key[,default])`: 如果 `key` 存在于字典中则将其移除并返回其值，否则返回 `default`。如果 `default` 未给出且 `key` 不存在于字典中 触发异常`KeyError`

`dict.popitem()` 从字典中移除并返回一个 `(键, 值)` 对。 在 3.7 发生变更：现在会确保采用 `LIFO` 顺序

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


## `defaultdict`

在 `collections` 包中提供了一个 `defaultdict` 类型，它是 `dict` 的子类，通过调用用户指定的 **工厂函数**，**为键提供默认值**

```python
>>> from collections import defaultdict
>>> d = defaultdict(list)  
>>> d['a'].append(10)
>>> d
defaultdict(<class 'list'>, {'a': [10]})
>>> d['b']
[]
>>> d
defaultdict(<class 'list'>, {'a': [10], 'b': []})
```

