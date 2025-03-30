# 内建数据结构

Python 解释器内置实现了常用的数据结构。包括 **序列** **集合** 和 **字典**，下表列出了 Python 内建的所有数据结构，这些数据结构使用非常平凡。我们应该对它了如指掌

| 数据结构        | 描述          |
| :---------- | ----------- |
| `str`       | 文本字符序列      |
| `bytes`     | 字节序列        |
| `bytearray` | 类似于 `bytes` |
| `list`      | 列表          |
| `tuple`     | 元组          |
|             |             |
| `set`       | 集合          |
| `frozenset` | 冻结集合        |
|             |             |
| `dict`      | 字典          |

## 序列

Python 中的序列都是 [[线性表]] 的实现。下面我们就开始学习 Python 中序列

### List

列表是我们学习的第一个序列，它是 [[线性表]] 的顺序实现。在 Python 中，列表的类型是 `list`，提供非常多的方法用于操作 `list` 对象

在 Python 中创建列表对象可以使用 **列表字面值** 或者 **构造函数 `list()`**。使用字面值创建一个列表对象是 Python 中最常用的方式。

```python
>>> numbers = []  # 空列表
>>> numbers
[]
>>> squares = [1, 2, 4, 9, 16, 25, 36]
>>> squares
[1, 2, 4, 9, 16, 25, 36]
```

假设我们已经有一个可迭代对象，如果需要将可迭代对象中的元素放在列表中，可以使用 `list()`

```python
>>> numbers = list(range(20))
>>> numbers
[0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19]
```

> [!tip] 
> 
> `list(iterable)` 将可迭代对象 `iterable` 中的所有元素依次取出，并放入一个新建的 `list` 对象中
> 

> [!attention] 
> 
> 请注意：在 Python 中，`list` 对象可以保存任意类型的对象，甚至是另一个列表。然而，通常我们使用 `list` 保存同一个类型的对象
> 
> 通常，`list` 对象会在 `for` 循环中遍历其中的每个元素，并对每个元素作相同的操作
> 

[[线性表]] 是保存同种类型的元素的表。然而，在 Python 中，我们可以在 `list` 中保存不同的数据类型，这是怎么做到呢？

> [!tip] 
> 
> 由于线性表要求元素占用相同的内存空间，在 C/C++ 中，我们知道对于不同的类型，其指针占用的空间是相同的。
>  
> 换句话说就是 Python 中的 `list` 保存的是对象的引用(指向对象的指针)，而不是对象本身
> 

#### 索引

对于序列而言，其中的元素是有序的。所以每个元素都可以对应一个编号，这个编号称为 **索引**。在 Python （大部分语言）中，索引都是从 $0$ 开始，到元素个数减 $1$ 为止

```python
>>> numbers = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19]
>>> numbers[0]  # 索引从 0 开始
0
>>> numbers[19] # 索引最大值为：列表长度 - 1
19
>>> numbers[20] # 超过最大值会引发 IndexError
Traceback (most recent call last):
  File "<python-input-3>", line 1, in <module>
    numbers[20] # 超过最大值会引发 IndexError
    ~~~~~~~^^^^
IndexError: list index out of range
>>> 
>>> numbers[-1] # Python 支持负索引
19
>>> numbers[-20] # 负索引最小值为：-列表长度
0
>>> numbers[-21] # 小于最小值会引发 IndexError
Traceback (most recent call last):
  File "<python-input-6>", line 1, in <module>
    numbers[-21] # Python
    ~~~~~~~^^^^^
IndexError: list index out of range
>>>
```

> [!attention] 
> 
> 请注意：在其他语言中，可能不支持负索引。但是正索引与负索引存在相互转换的关系：`负索引 = 正索引 - 列表的长度`
> 

下面展示 Python 中正索引与负索引的关系

```python
 +-----+-----+-----+-----+-----+-----+
 | 'P' | 'y' | 't' | 'h' | 'o' | 'n' |
 +-----+-----+-----+-----+-----+-----+
 0     1     2     3     4     5     6
-6    -5    -4    -3    -2    -1
```

> [!tip] 
> 
> 索引操作本质上就是使用 **第一个元素所在的地址加上偏移** 定义列表中的元素。这种元素查找方式非常高效，它可以直接定位一个元素；其时间复杂度为 $O(1)$，称为常量时间复杂度
> 

#### 切片

Python 中的切片是从一个序列中拷贝一个子序列出来的操作

```python
>>> li = [-1, 1, 2, 3, 4, 'h', 'e', 'l', 'l', 'o']
>>> li[1: 3]  # 选取索引在 [1,3) 范围内的元素形成一个子列表
[1, 2]
>>> z = li[1: 3]
>>> id(z),id(li)
(1655037648512, 1655068749312)
```

切片操作可以在 `[]` 内指定三个数，分别是 **起始索引** **结束索引** 和 **步长**，即 `li[start: end: step]`

请注意：这三个数可以任意任意省略，此时它们取默认值

+ `start` 默认是 $0$
+ `end` 默认是序列长度 (`len(squence)`)
+ `step` 默认是 $1$


> [!attention] 
> 
> **结束索引不包含在切片内**。换句话说，索引为 `end` 的元素不在子切片中
>  

```python
>>> li[0:3]  # [0,3)
[-1, 1, 2]
>>> li[:3]  # [0,3)
[-1, 1, 2]
>>> li[3:]  # [3, len(li))
[3, 4, 'h', 'e', 'l', 'l', 'o']
>>> li[3:len(li)]  # [3, len(li))
[3, 4, 'h', 'e', 'l', 'l', 'o']
```

> [!attention] 
> 
> 切片 `[start: end: step]` 本质上就是去序列中选择索引为 $i = start + j * step$ 的元素组成一个新切片，其中 $start \le i \lt end, j=0,1,\cdots$
> 

由于 Python 支持负索引，因此在切片的时候也可以使用负索引

```python
>>> li = [-1, 1, 2, 3, 4, 'h', 'e', 'l', 'l', 'o']
>>> li[-6:]  # 等价于 li[4:]
[4, 'h', 'e', 'l', 'l', 'o']
>>> li[::-1] # 等价于 li[0:len(li):-1]
['o', 'l', 'l', 'e', 'h', 4, 3, 2, 1, -1]
```

##### 内置函数 slice

内置函数 `slice()` 用于返回一个 `slice` 对象

```python
 slice(stop)
 slice(start, stop[, step])
```

`slice` 对象用于序列切片

```python
>>> li = [-1, 1, 2, 3, 4, 'h', 'e', 'l', 'l', 'o']
>>> li[slice(3)]  # 等价于 li[:9]
[-1, 1, 2]
>>> li[slice(1,3)] # 等价于 li[1:3]
[1, 2]
>>> li[slice(0, len(li), 2)] # 等价于 li[1::2]
[-1, 2, 4, 'e', 'l']
```

#### 查找

Python 中的 `list` 对象提供了两个方法用于查找 `list` 中是否存在一个元素 `value`

| 方法                                   | 描述                      |
| :----------------------------------- | ----------------------- |
| `list.index(value[, start [, end]])` | 在指定的范围内查找元素 `value` 的索引 |
| `list.count(value)`                  | 统计列表中 `value` 出现的次数     |

> [!attention] 
> 
> 方法 `index()` 可以指定搜索区间为 `[start, end)`，这样就只会在索引为 `[start, end)` 的范围内查找元素 `value`。如果找到相同的元素，则返回元素的索引；如果没找到，则抛出 `ValueError` 异常
> 

```python
>>> numbers = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19]
>>> numbers.index(2)  # 在整个列表中查找
2
>>> numbers.index(11, 9, 12)  # 在索引为 [9, 12) 的区间内查找
11
>>> numbers.index(4, 9, 12)  # 在 [9, 12) 区间未找到元素 4，抛出 ValueError
Traceback (most recent call last):
  File "<python-input-10>", line 1, in <module>
    numbers.index(4, 9, 12)  # 返回索引 11
    ~~~~~~~~~~~~~^^^^^^^^^^
ValueError: 4 is not in list
```

显然，在一个序列中查找一个元素，通常需要 **遍历序列** 中的所有元素，然后一个一个对比的。当一个序列中的元素越多，查找花费的时间就越大。例如，一个列表中的元素个数为 $10$，从列表中找到一个元素在最坏的情况下需要比较 $10$ 次。

> [!tip] 
> 
> 这种随着元素规模(个数)线性增长的时间复杂度为 $O(n)$，称为线性时间复杂度
> 

#### 序列的长度

在 Python 中可以使用内置函数 `len()` 返回一个容器中元素的个数。在 CPython 的底层实现中，使用了一个成员记录容器中元素的个数。因此，使用 `len()` 获取元素的个数时并不会遍历容器

```python
>>> numbers = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19]
>>> len(numbers)  # 获取序列 numbers 的元素个数
20
```

#### 修改

列表是可变类型，其对象是可变的。因此，我们可以修改列表对象的值。

在 Python 想要修改列表中第 $i$ 个元素的值，需要使用 `list[i] = value` 的方式给列表第 $i$ 个元素赋值即可

```python
>>> numbers = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19]
>>> numbers[0] = 100
>>> numbers
[100, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19]
>>> numbers[-1] = 200
>>> numbers
[100, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 200]
```

#### 增加

下表列出了 Python 提供了各种给列表对象增加元素的方法

| 方法                          | 描述                             |
| :-------------------------- | ------------------------------ |
| `list.append(value)`        | 在列表末尾添加一个元素 `value`            |
| `list.extend(iterable)`     | 将可迭代对象 `iterable` 中的元素追加在列表末尾  |
| `list.insert(index, value)` | 在列表索引为 `index` 的位置插入元素 `value` |

列表对象在创建时会预先分配好一部分内存空间，`append()` 方法会将元素 `value` 添加到列表元素的尾部。如下图

![[Pasted image 20250323214353.png]]

```python
>>> numbers = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
>>> numbers.append(11)
>>> numbers
[0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11]
```

`insert(index, value)` 方法会将元素 `value` 插入到列表中索引位 `index` 的位置。也就说，在插入 `value` 之前，需要将 `index` 及其之后的元素整体向后移动一位，将索引为 `index` 的位置空出来之后才能完成插入。如下图所示

![[Pasted image 20250323215403.png]]

```python
>>> numbers.insert(10, 20) # 在索引位置为 10 处插入元素 20
>>> numbers
[0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 20, 10, 11]
>>> numbers.insert(100, 20)  # 超出最大索引范围：默认插入在最后列表末尾，等价于 append()
>>> numbers
[0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 20, 10, 11, 20]
>>> numbers.insert(-100, 100)
>>> numbers
[100, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 20, 10, 11, 20]
```

> [!attention] 
> 
> 请注意：参数 `index`  超出索引范围时，`insert()` 方法并不会出错，而是会将 `value` 插入到列表末尾或者开始位置
> 

##### 扩容策略

列表元素在插入过程中，可能会因为预先分配的内存空间被耗尽，从而导致列表 **扩容** 策略的执行。

Python 列表扩容策略在 `cpython/Objects/listobject.c` 中的函数 `list_resize` 给出了列表的扩展策略

```c
static int
list_resize(PyListObject *self, Py_ssize_t newsize)
/*
	self 就操作的列表对象，newsize 元素添加之后的 ob_size
*/
{

    size_t new_allocated, target_bytes;
    Py_ssize_t allocated = self->allocated;  // 已分配的内存容量

	// 如果 newsize 在当前分配容量的一半以上且小于当前容量，
	// 意味着内存容量足够，不需要重新分配，只需更新大小即可。
    if (allocated >= newsize && newsize >= (allocated >> 1)) {
        assert(self->ob_item != NULL || newsize == 0);
        Py_SET_SIZE(self, newsize);
        return 0;
    }

    /* new_allocated 计算方式为当前大小的 1.125 倍再加上 6，然后确保分配大小是 4 的倍数。这样做是为了减少频繁的内存分配和拷贝，提高性能。
     */
    new_allocated = ((size_t)newsize + (newsize >> 3) + 6) & ~(size_t)3;
    /* 检查是否过度分配：如果分配的内存大于实际需要的太多，则重新计算分配大小。
     */
    if (newsize - Py_SIZE(self) > (Py_ssize_t)(new_allocated - newsize))
        new_allocated = ((size_t)newsize + 3) & ~(size_t)3;

    if (newsize == 0)
        new_allocated = 0;

#ifdef Py_GIL_DISABLED  // Python >= 3.13 中如果禁用了全局解释器锁
    _PyListArray *array = list_allocate_array(new_allocated);
    if (array == NULL) {
        PyErr_NoMemory();
        return -1;
    }
    PyObject **old_items = self->ob_item;
    if (self->ob_item) {
        if (new_allocated < (size_t)allocated) {
            target_bytes = new_allocated * sizeof(PyObject*);
        }
        else {
            target_bytes = allocated * sizeof(PyObject*);
        }
        memcpy(array->ob_item, self->ob_item, target_bytes);
    }
    if (new_allocated > (size_t)allocated) {
        memset(array->ob_item + allocated, 0, sizeof(PyObject *) * (new_allocated - allocated));
    }
     _Py_atomic_store_ptr_release(&self->ob_item, &array->ob_item);
    self->allocated = new_allocated;
    Py_SET_SIZE(self, newsize);
    if (old_items != NULL) {
        free_list_items(old_items, _PyObject_GC_IS_SHARED(self));
    }
#else // 没有禁用全局解释器锁的处理
    PyObject **items;
    if (new_allocated <= (size_t)PY_SSIZE_T_MAX / sizeof(PyObject *)) {
        target_bytes = new_allocated * sizeof(PyObject *);
        items = (PyObject **)PyMem_Realloc(self->ob_item, target_bytes);
    }
    else {
        // integer overflow
        items = NULL;
    }
    if (items == NULL) {
        PyErr_NoMemory();
        return -1;
    }
    self->ob_item = items;
    Py_SET_SIZE(self, newsize);
    self->allocated = new_allocated;
#endif
    return 0;
}
```

> [!summary] 
> 
> 列表的扩容策略
> 
> + 如果 `newsize` 占当前分配容量的一半以上且小于当前容量，意味着内存容量足够，不需要重新分配，只需更新长度即可
> 
> + 如果需要重新分配，按照 `(newsize * 1.125 + 6) & ~3` 分配。即，将扩容为 `newsize` 的 $1.125$ 倍加 $6$，并调整大小为 $4$ 的倍数。这样做是为了减少频繁的内存分配和拷贝，提高性能。
> 

#### 删除

Python 中列表是线性表的顺序实现，这意味着元素在内存中紧邻存储。当我们删除列表中的一个元素时，就必须将该元素后面的所有元素都向前移动一位。

> [!tip] 
> 
> 由于删除操作伴随着元素的移动。最好的情况就是在列表末尾删除，这样就不要移动任何元素。最坏的情况就是删除列表的首元素，这唯意味着要移动剩下的 $n-1$ 个元素。
> 
> 换句话说，列表删除操作的时间复杂度为 $O(n)$
> 

下表列出了 Python 的列表对象提供了 $3$ 个删除元素的方法

| 方法                   | 描述                               |
| :------------------- | :------------------------------- |
| `list.remove(value)` | 从列表 `list` 中移除找到的第一个元素 `value`   |
| `list.pop(index=-1)` | 从列表 `list` 中移除索引为 `index` 的元素并返回 |
| `list.clear()`       | 清空列表元素                           |

> [!attention] 
> 
> 请注意：`list.remove(value)` 如果在列表 `list` 中没有找到元素 `value`，则抛出 `ValueError`
> 
> `list.pop(index=-1)` 通过索引 `index` 删除元素，默认删除列表的最后一个元素，并返回。如果 `index` 超出了列表索引范围，则抛出 `IndexError`
> 
> `list.clear()` 并不会删除列表 `list` 中的所有元素，而是仅仅将列表的长度置为 $0$ 即可，然后将列表元素的引用计数减少 $1$
> 

```python
>>> numbers = [1, 2, 3, 4, 5, 6, 7, 8, 9]
>>> numbers.remove(1)
>>> numbers
[2, 3, 4, 5, 6, 7, 8, 9]
>>> numbers.remove(10)  # 没找到元素抛出 ValueError
Traceback (most recent call last):
  File "<python-input-49>", line 1, in <module>
    numbers.remove(10)
    ~~~~~~~~~~~~~~^^^^
ValueError: list.remove(x): x not in list

>>> numbers.pop() # 删除最后一个元素，并返回
9
>>> numbers
[2, 3, 4, 5, 6, 7, 8]
>>> numbers.pop(0)
2
>>> numbers.pop(7)  # 超出索引范围
Traceback (most recent call last):
  File "<python-input-66>", line 1, in <module>
    numbers.pop(5)
    ~~~~~~~~~~~^^^
IndexError: pop index out of range

>>> numbers.clear()
>>> numbers
[]
```

##### del 关键字

关键字 `del` 用于删除对象的引用。在 Python 中，变量或者容器存储的都是对象的引用，因此可以使用关键字 `del` 删除列表中的元素。

对列表的元素执行 `del`  语句，其行为与 `list.pop()` 类似

```python
>>> numbers = [1, 2, 3, 4, 5]
>>> numbers[1]
2
>>> del numbers[1]  # 删除列表 numbers 中索引为 1 的元素
>>> del numbers[6]  # 超出索引范围
Traceback (most recent call last):
  File "<python-input-64>", line 1, in <module>
    del numbers[6]  # 删除列表 numbers 中索引为 1 的元素
        ~~~~~~~^^^
IndexError: list assignment index out of range
```


#### 排序和反转

列表提供了下面两个方法用于 **原地排序** 和 **原地反转** 列表

| 方法                                     | 描述                     |
| :------------------------------------- | :--------------------- |
| `list.sort(*,key=None, reverse=False)` | 对列表 `list` 进行原地从小到大的排序 |
| `list.reverse()`                       | 将列表 `list` 进行原地反转      |


> [!attention] 
> 
> 请注意：方法 `list.sort()` 有两个参数：`key` 和 `reverse`
> 
> 方法 `list.sort()` 使用了运算符 `<`，列表可以存储有不同的类型的对象，不同类型的对象之间可能不支持运算符 `<`。参数 `key` 指定了在比较之前执行的操作，通常是类型转换
> 
> 方法 `list.sort()` 默认进行从小到大的排序。参数  `reverse` 指定是否返序
> 

```python
>>> li
[-1, 1, 2, 3, 4, 'h', 'e', 'l', 'l', 'o']
>>> li.sort(key=str, reverse=True)  # 降序排列
>>> li
['o', 'l', 'l', 'h', 'e', 4, 3, 2, 1, -1]
>>> li.sort(key=str)  # 升序排列
>>> li
[-1, 1, 2, 3, 4, 'e', 'h', 'l', 'l', 'o']
>>> li.reverse()  # 反转
>>> li
['o', 'l', 'l', 'h', 'e', 4, 3, 2, 1, -1]
```

> [!tip] 
> `list.sort()` 方法采用 [[LIPIcs.ESA.2018.4.pdf|TimeSort]] 实现，于 2002 年由  Tim Peters 设计。该算法从 **归并排序** 和 **插入排序** 改进而来，具有如下特性
> + 稳定：相同元素在排序后的位置关系不变
> + 快速：最坏时间复杂度为 $O(n\log n)$，最好时间复杂度为 $O(n)$
> + 空间消耗：空间复杂度为 $O(n)$
> 

##### 内置函数 `sorted` 和 `reversed`

下表列出了 Python 提供了排序和反转容器的两个内置函数

| 函数                                                | 描述                               |
| :------------------------------------------------ | -------------------------------- |
| `sorted(iterable, /, *, key=None, reverse=False)` | 返回一个已排序的新列表，元素来自可迭代对象 `iterable` |
| `reversed(sequence)`                              | 返回一个可迭代对象，用于返序遍历序列 `sequence`    |

```python
>>> li = [-1, 1, 2, 3, 4, 'h', 'e', 'l', 'l', 'o']
>>> sorted(li, key=str, reverse=True)
['o', 'l', 'l', 'h', 'e', 4, 3, 2, 1, -1]

>>> li = [-1, 1, 2, 3, 4, 'h', 'e', 'l', 'l', 'o']
>>> reversed(li)
<list_reverseiterator object at 0x0000018157ED6CE0>
>>> list(reversed(li))
['o', 'l', 'l', 'e', 'h', 4, 3, 2, 1, -1]
```

#### 成员检查 in

关键字 `in` 可以用于检查对象是否在容器中。本质上就是去遍历容器，如果找到了指定的元素，那么就返回 `True`；否则，返回 `False`

```python
>>> numbers = [1, 2, 3, 4, 5]
>>> 1 in numbers
True
>>> 10 in numbers
False
```

#### 复制

请注意：在 Python 中，**赋值只是简单的拷贝对象的引用**，而不是复制对象

```python
>>> a = [1,2,3]
>>> b = a
>>> a is b  # True：a 和 b 是同一个对象
True
```

如果需要复制对象，那么需要使用对象提供的拷贝方法。或者使用 `copy` 模块提供的拷贝函数。下面我们介绍列表对象的复制

在 Python 中，拷贝列表可以使用下表列出的操作方式

| 操作                    | 描述                   |
| :-------------------- | :------------------- |
| `list.copy()`         | 使用列表对象提供的拷贝方法拷贝列表    |
| `list[:]`             | 切片的方式拷贝列表            |
| `copy.copy(list)`     | 使用 `copy` 模块提供的拷贝函数  |
| `copy.deepcopy(list)` | 使用 `copy` 模块提供的深拷贝函数 |

> [!tip] 
> 
> 上述表格中的前三个方法都是对列表进行 **浅复制**。即是对列表对象进行逐元素复制，并返回一个全新的列表对象
> 
> 换句话说就是：**浅复制只是拷贝了列表元素的引用**。如果列表中的元素是另一个列表，那么拷贝产生的列表中的元素与原列表中的元素是同一个对象
> 

```python
>>> li = [1, 2, 3, 4, [1, 2, 3]]
>>> new_li = li.copy()
>>> li == new_li  # 判断两个列表的值是否相等：即判断列表中对的所有元素是否相等
True
>>> li is new_li
False
>>> li[4] is new_li[4]  # 判断索引为 4 的元素是否是同一个元素
True
>>> new_li[4][0] = 100  # 显然修改 new_li[4] 时，li[4] 也会发送变化
>>> li
[1, 2, 3, 4, [100, 2, 3]]
>>> new_li
[1, 2, 3, 4, [100, 2, 3]]
```

显然，上述例子中列表的第 $4$ 号元素指向同一个对象，如下图所示

![[Pasted image 20250325222427.png]]


> [!tip]
>  
> 上述例子展示了浅复制存在的问题：相互依赖。为了解决这个问题，Python 在 `copy` 模块中提供了一个 `deepcopy()` 函数用于执行 **深复制**
> 
> 所谓的深复制就是在遇见对象的引用时，就会创建一个全新的对象，而不是仅仅拷贝一个引用
> 
> 请注意：Python 中操作的都是对象的引用，然而 Python 将对象划分为两种：**可变对象** 和 **不可变对象**。**对于可变对象而言，执行深复制时才创建一个全新的对象**，而对于不可变对象，执行深复制时也只会拷贝其引用。
> 

```python
>>> li = [1, 2, 3, 4, [1, 2, 3]]
>>> import copy
>>> new_li = copy.deepcopy(li)
>>> li == new_li
True
>>> li is new_li
False
>>> li[0] is new_li[0]  # 第 0 号元素是同一个对象
True
>>> li[4] is new_li[4] # 第 4 号元素不是同一个对象
False
>>> li[4][0] is new_li[4][0]
True
```


显然，上述例子中列表的第 $4$ 号元素指向不同的对象，如下图所示

![[Pasted image 20250325224230.png]]

#### 运算符 + 和 *

Python 为列表重定义了运算符 `+` 和 `*` 的含义，下面我们介绍列表对象上如何执行运算符 `+` 和 `*` 的

> [!tip] 
> 
> 运算符 `+` 用于两个列表对象时，称为 **列表拼接** 运算符
> 
> 运算符 `*` 用于列表对象和 `int` 对象时，称为 **列表自拼接** 运算符
> 

当两个列表对象执行列表拼接运算符(`+`) 时，会返回一个全新的列表对象，其中的元素复制于运算符(`+`) 的两个操作数。例如

```python
>>> numbers = [1, 2, 3, 4, 5]
>>> squares = [1, 4, 9, 16, 25]
>>> li = numbers + squares
>>> li
[1, 2, 3, 4, 5, 1, 4, 9, 16, 25]
>>> id(numbers), id(squares), id(li)
(1655037569600, 1655037648320, 1655037649344)
```

> [!tip] 
> 
> 内置函数 `id(obj)` 用于返回对象 `obj` 的标识，在 CPython 中，这个值就是对象 `obj` 所在的内存地址
> 

当一个列表对象和一个整数对象执行列表自拼接运算符(`*`) 时，返回一个全新的列表对象，其中的元素是从列表对象中复制。

```python
>>> numbers = [1,2,3,4,5]
>>> numbers * 3
[1, 2, 3, 4, 5, 1, 2, 3, 4, 5, 1, 2, 3, 4, 5]
>>> 4 * numbers
[1, 2, 3, 4, 5, 1, 2, 3, 4, 5, 1, 2, 3, 4, 5, 1, 2, 3, 4, 5]
```

 当一个列表对象 `list` 与一个整数对象 `n` 执行运算符 `*` 时，会创建一个全新的列表对象，其中的元素是将 `list` 中的元素复制 $n$ 次 

> [!attention] 
> 
> 请注意：列表存储的本质上是元素的引用，当发生元素复制的时候，复制是对象的引用。当列表元素是可变对象时，请小心复制的奇怪表现
> 

```python
>>> li = [[1]] * 3
>>> li
[[1], [1], [1]]
>>> li[0].append(2)
>>> li
[[1, 2], [1, 2], [1, 2]]
```

> [!tip] 
> 
> Python 中的任何数据都以对象的形式存在，通过对象的引用操作对象。也就是说，变量本质上就是对象的引用，列表中的元素也是对象的引用。
> 

下图展示 `[[1]] * 3` 表达式执行的内存结构

![[Pasted image 20250324205610.png]]

> [!attention] 
> 
> 也就是说：变量 `li` 引用一个列表，该列表有 $3$ 个元素，这个些元素都是同一个列表的引用
> 

### Tuple

在 Python 中，元组和列表一样也是线性表的顺序实现。只是 **Python 限制我们对元组元素进行修改**

> [!tip] 
> 
> 和列表一样，元组的元素也是一个个对象的引用，Python 禁止我们修改元组的元素。下图说明了元组的不可变并不是值不可变
> 
> ![[Pasted image 20250326194118.png]]
> 
> Python 将这些不能修改的对象称为 **不可变对象**
> 

> [!summary] 
> 
> 在 Python 中，不可变对象有许多。我们已经学过的 **数字**、**字符串** 和 **元组** 都是不可变对象
> 
> 任何对不可变对象的修改操作，需要创建一个全新的不可变对象。这种情况，不可变对象表现出来的特性和值一样。
> 
> 然而，由于可变对象可修改其内容，这种情况下，表现出来特性和引用一样
> 

下面我们介绍如何创建元组对象。Python 提供了 $2$ 种创建元组对象的方法：**字面值** 和 **构造函数 `tuple()`**

> [!tip] 
> 
> 元组的字面值词法：使用 **逗号(`,`)** 分隔的多个表达式。例如，`a, b` 就是一个元组。然而，为了提高可读性，通常我们将这些由逗号分隔的表达式放在一个圆括号中
> 
> 注意：元组字面值最重要的逗号，如果没有逗号，Python 就不会将其解释为元组。例如，`(1)` 就是整数 `1`；只有 `(1,)` 才是元组
> 

```python
>>> 1,2,3
(1, 2, 3)
>>> (1)  # 注意：这是整数 1。因为缺失 ,
1
>>> ()  # 这是空元组
()
```

> [!tip] 
> 
> 使用构造函数 `tuple(iterable)` 时，需要提供一个可迭代对象作为参数，该函数会使用可迭代对象中元素创建一个全新的元组对象并返回
> 

```python
>>> tuple()  # 空元组
()
>>> tuple([1, 2, 3, 'a', 'b'])
(1, 2, 3, 'a', 'b')
>>> tuple("hello")
('h', 'e', 'l', 'l', 'o')
>>> tuple(range(5))
(0, 1, 2, 3, 4)
```

#### 索引和切片

既然元组与列表都是 Python 对线性表的顺序实现，那么元组通用支持 **切片** 和 **索引** 操作

```python
>>> tu = (1, "hello", 2, "alpha")
>>> tu[0]
1
>>> tu[-1]
'alpha'
>>> tu[-3:]
('hello', 2, 'alpha')
>>> tu[3:]
('alpha',)
```

#### 查找

对于元组而言，Python 同样提供了下面两个方法用于查找元组中的元素

| 方法                                    | 描述                      |
| :------------------------------------ | ----------------------- |
| `tuple.index(value[, start [, end]])` | 在指定的范围内查找元素 `value` 的索引 |
| `tuple.count(value)`                  | 统计列表中 `value` 出现的次数     |

```python
>>> tu.index(1)
0

>>> tu.index(3)   # 没找到，抛出 ValueError
Traceback (most recent call last):
  File "<python-input-30>", line 1, in <module>
    tu.index(3)
    ~~~~~~~~^^^
ValueError: tuple.index(x): x not in tuple

>>> tu.count('hello')
1
```

#### 元组打包与序列解包

语句 `t = 12345, 54321, 'hello!'` 是 **_元组打包_** 的例子：值 `12345`, `54321` 和 `'hello!'` 一起被打包进元组。

```python
>>> t = 12345, 54321, 'hello!'
>>> t
(12345, 54321, 'hello!')
```

> [!tip] 
> 
> 所谓的元组打包本质上就是创建一个新元组对象的字面值
> 

对元组而言，逆操作也可以

```python
x,y,z = t
```

现在，可以解释在 [[Python：基本概念#while 语句]] 的例子中使用的 **赋值语句  `a, b = b, a + b`  其实就是元组的打包与解包**

称之为 **_序列解包_** 也是妥妥的，**_适用于右侧的任何序列_**。序列解包时，左侧变量与右侧序列元素的数量应相等。有时，序列太长，且又需要解包操作时，我们可以在变量前面加上 `*` 表示将剩余元封包在一个列表中

```python
>>> letters = ('a', 'b', 'c', 'd', 'e', 'f', 'g')
>>> a,b, *c = letters  # 只需要前两个元素，剩余元素重新封包
>>> a
'a'
>>> b
'b'
>>> c
['c', 'd', 'e', 'f', 'g']
>>> e, *f, g = c
>>> e
'c'
>>> f
['d', 'e', 'f']
>>> g
'g'
```

#### 运算符 + 和 *

元组也支持运算符 `+` 和 `*`，执行与列表相同的操作，即复制给定的元组中的元素，产生一个全新的元组对象

```python
>>> tu = (1,) * 3
>>> tu
(1, 1, 1)
>>> tu[0]
1
>>> tu[0] = 2
Traceback (most recent call last):
  File "<python-input-61>", line 1, in <module>
    tu[0] = 2
    ~~^^^
TypeError: 'tuple' object does not support item assignment

>>> tu = ([1], ) * 3
>>> tu[0][0] = 100
>>> tu
([100], [100], [100])
```

### Str

我们在 [[Python：基本概念#字符串]] 简单介绍了 Python 中的字符串，下面我们更详细的介绍

在 Python 字符串是 **字符序列**，和元组一样，它是 **不可变类型**；也就是说，我们不能修改一个字符串中的字符

```python
>>> s = "hello"
>>> s[0] = 'H'
Traceback (most recent call last):
  File "<python-input-37>", line 1, in <module>
    s[0] = 'H'
    ~^^^
TypeError: 'str' object does not support item assignment
```

#### 字符串的结构

在 Python 中，字符串是如何存储的呢？要了解字符串在 Python 内部是如何实现的，就需要翻阅 Python 源码

```c title:Include/cpython/unicodeobject.h
typedef struct {
    PyObject_HEAD
    Py_ssize_t length;  // 字符串的码点数。注意不是字节数
    Py_hash_t hash;     // 字符串的哈希值
    struct {
        uint16_t interned;
        unsigned short kind:3;
        unsigned short compact:1;
        unsigned short ascii:1;
        unsigned short statically_allocated:1;
        unsigned short :10;
    } state;
} PyASCIIObject;

typedef struct {
    PyASCIIObject _base;
    Py_ssize_t utf8_length; // utf-8 编码的字节长度
    char *utf8;
} PyCompactUnicodeObject;

typedef struct {
    PyCompactUnicodeObject _base;
    union {
        void *any;
        Py_UCS1 *latin1;// 字符串中所有的字符只占 1 字节
        Py_UCS2 *ucs2;  // 字符串中有字符需要占 2 字节
        Py_UCS4 *ucs4;  // 字符串中有字符串需要占 4 字节
    } data; 
} PyUnicodeObject;
```

在 Python 字符串存储的是 Unicode 码值。但是一定的优化

> [!tip] 
> 
> 如果字符串中所有字符都在 ASCII 码范围内，那么就可以用占用 $1$ 个字节的 Latin-1 编码进行存储
> 
> 如果字符串中存在了需要占用 $2$ 字节（比如中文字符），那么整个字符串就将采用占用 2 个字节 UCS-2 编码进行存储
> 
> 如果字符串中存在了需要占用 $4$ 字节的字符，那么整个字符串就采用占用 $4$ 字节的 UCS-4 编码进行存储
> 
> 换句话说：Python 内部 **采用了定长编码** 的方式存储字符串，虽然带来了内存消耗，但是带来了字符串访问性能的提高
> 

> [!attention] 
> 
> Python 为什么没有采用 UTF-8 编码方式存储字符串呢？
> 
> Python 在存储字符串时，不采用的 UTF-8 编码的原因是 UTF-8 是变长编码。这会导致我们在通过索引查找字符串中的单个字符失效。例如 `string[n]` 只能索引到第 $n$ 个字节的值，而无法得到正确的字符。
> 
> 如果采用 UTF-8 编码，想要实现 `string[n]` 查找第 $n$ 个字符，那么就需要统计前 $n$ 个字符占用的字节数，从而导致 $O(1)$ 的时间复杂度升级为 $O(n)$
> 

#### 字符串缓存

Python 中的字符串时不允许修改的，对于一些 **简单的字符串**，我们可以提前缓存下来，当再次创建这些字符串时，只需要简单的返回已缓存字符串对象的引用即可

所谓简单的字符串时指：符合标识符命名规范的且长度不超过 $20$ 的字符，这些字符串只会被创建一次。

> [!tip] 
> 
> 短字符串缓存可以降低 Python 中字符串对内存的消耗。
> 
> 同样的策略 Python 还应用到了整数。Python 缓存了取值范围位于 $[-5, 256]$ 之间的小整数，每当我们需要这个范围内的整数时，Python 也只会返回这些对象的引用
> 

此外，**在同一行** 创建的值相同的任意字符串也只会创建一次

```python
>>> s1 = "中问"
>>> s2 = "中问"
>>> s1 is s2
False
>>> s3,s4 = "函数", "函数"  # 同一行创建的字符串
>>> s3 is s4  # s3 和 s4 是同一个对象
True
```

#### 索引和切片

字符串时 Unicode 码值的序列，因此我们可以对字符串进行索引和切片

```python
>>> s1 = "hello, 世界"
>>> s1[-1]
'界'
>>> s1[0]
'h'
>>> s1[0:5]
'hello'
>>> s1[7:]
'世界'
```

> [!attention] 
> 
> 请注意：对字符串索引得到的也就是字符串。因此，Python 中没有处理单个字符的能力
> 
> 在其他语言中，索引得到的可能是字符的编码值。在 Python 中，如果需要得到字符的 Unicode 码值，可以使用内置函数 `ord(char)` 将 **单字符字符串** 转换为 Unicode 码值(整数)。
> 
> 此外，内置函数 `chr(value)` 将 Unicode 码值 `value` 转换为单字符字符串 
> 

```python
>>> ord(s1[0])
104
>>> ord(s1[-1])
30028
>>> chr(104)
'h'
>>> chr(97)
'a'
```

#### 拼接

想要拼接两个字符串，可以使用运算符 `+` 和 `*`。

> [!summary] 
> 
> Python 中，序列都可以使用 `+` 和 `*` 两个运算符，从而产生一个全新的序列，其元素来自运算符的序列类型的操作数的拷贝
> 

```python
>>> 3 * 'un' + 'ium'
'unununium'
```

对于相邻的两个字符串字面值，Python 解释器会自动将其拼接为一个字符串对象

```python
>>> "Py" "thon"
'Python'

>>> s = "Py"
>>> s "thon"
  File "<python-input-66>", line 1
    s "thon"
      ^^^^^^
SyntaxError: invalid syntax
```

> [!WARNING] 
> 
> 只能是两个字面值才会自动拼接，如果其中一个是变量，将抛出 `SyntaxError`
> 

上述拼接操作在只有少量的字符串对象时使用是比较方便的。但是，如果我们有一个系列的字符串对象存储在一个可迭代对象中，那么使用运算符 `+` 拼接将及其低效

Python 的字符串对象提供了一个方法 `str.join(iterable)`：该方法使用 `str` 作为间隔符，并将 `iterable` 中的字符串拼接成为一个全新的字符串对象

```python
>>> "|".join(["h", "e", "l", "l", "o"])
'h|e|l|l|o'
>>> "|".join("hello")
'h|e|l|l|o'
```

> [!attention] 
> 
> 请注意：`iterable` 中的元素必须是 `str` 类型的对象
> 

#### 查找

由于字符串是字符序列，可以预见的是字符串对象一定提供了 `str.index()` 和 `str.count()` 这两个方法，此外还有一个额外的方法 `str.find()` 用于查找子串的索引

| 方法                               | 描述                             |
| :------------------------------- | :----------------------------- |
| `str.index(sub[,start[,end]])`   | 从字符串 `str`  查找子字符串 `sub` 的起始位置 |
| `str.count(sub[,start[,end]])`   | 统计 `str` 中字符串 `sub` 出现的次数      |
| `str.find(sub[, start[, end]])`  | 与 `str.index()` 相同，没找到返回 $-$   |
| `str.rfind(sub[, start[, end]])` | 与 `str.find()` 类似，只是从右相左查找     |

```python
>>> s = "hello, world!"

>>> s.index("ll")
2
>>> s.index("q")
Traceback (most recent call last):
  File "<python-input-2>", line 1, in <module>
    s.index("q")
    ~~~~~~~^^^^^
ValueError: substring not found

>>> s.find("ll")
2
>>> s.find("q")
-1
>>> s.rfind("l")
10

>>> s.count('l')
3
```


> [!attention] 
> 
> `str.index()` 方法在没有找到子串时会抛出 `ValueError` 异常，通常我们不是这种抛出异常的方式，而是使用 `str.find()` 
> 

#### 分割

Python 为字符串提供了两种形式的分割：**split** 和 **partition**；它们的行为存在较大的差异。下面我们首先介绍 split，它是最常使用的分割方式

| 方法                                  | 描述                                  |
| :---------------------------------- | :---------------------------------- |
| `str.split(sep=None, maxsplit=-1)`  | 将字符串 `str` 在 `sep` 处分割 `maxsplit` 次 |
| `str.rsplit(sep=None, maxsplit=-1)` | 与 `str.split()` 类似，只是从右向左分割         |
| `str.splitlines(keepends=False)`    | 在 **行边界** 进行分割                      |

> [!tip] 
> 
> 对于 `str.split()` 方法，默认情况下会在 **空白符** 位置处分割，直到字符串 `str` 中没有空白符为止。换句话说，就是 `maxsplit=-1` 时表示尽可能的在所有 `sub` 处分割
> 
> 常见的空白符包括：空格(`" "`)，转义字符(`"\n"` `"\r"` `"\t"` 等)
> 

> [!tip] 
> 
> `str.splitlines()` 在行边界处分割字符串，默认情况下，最终结果是不包含行边界的，除非指定 `keepends` 为 `True` 
> 

下表列出来作为行边界的转义字符

| 行边界           | 描述                 |
| ------------- | ------------------ |
| `\n`          | 换行                 |
| `\r`          | 回车                 |
| `\r\n`        | 回车 + 换行            |
| `\v` 或 `\x0b` | 行制表符，Python >=3.12 |
| `\f` 或 `\x0c` | 换表单，Python >=3.12  |
| `\x1c`        | 文件分隔符              |
| `\x1d`        | 组分隔符               |
| `\x1e`        | 记录分隔符              |
| `\x85`        | 下一行 (C1 控制码)       |
| `\u2028`      | 行分隔符               |
| `\u2029`      | 段分隔符               |

```python
>>> "hello world".split()  # 默认在空白字符串处分割
['hello', 'world']
>>> "hello world".split("l", 1) # 分割一次
['he', 'lo world']
>>> "hello world".split("l")  # 
['he', '', 'o wor', 'd']
>>> "\n\ta\r\n ab\tc\n".split()
['a', 'ab', 'c']
>>> "\n\ta\r\n ab\tc\n".split('\n')
['', '\ta\r', ' ab\tc', '']

>>> "hello world".rsplit("l", 1)
['hello wor', 'd']
>>> "\n\ta\r\n ab\tc\n".splitlines()
['', '\ta', ' ab\tc']
```

> [!attention] 
> 
> 请注意，**split** 的分割方式会立即返回一个列表，**分割符不会包含在结果中**
> 

下面我们介绍 partition 分割方式：partition 分割方式只会在 **首次** 出现分割符的位置分割字符，并返回一个  $3$  元组。

> [!tip] 
>  
> 注意，这个 $3$ 元组的内容为：分隔符之前的部分、分隔符本身，以及分隔符之后的部分
>
> 如果分隔符未找到，则返回的 $3$ 元组中包含字符本身以及两个空字符串
> 

| 方法                    | 描述                                       |
| :-------------------- | :--------------------------------------- |
| `str.partition(sep)`  | 将字符串 `str` 在首次出现 `sep` 的处分割，并返回一个 $3$ 元组 |
| `str.rpartition(sep)` | 同 `str.partition(sep)`，只是从右到左查找 `sep`    |

```python
>>> " hello world ".partition("o") # 只在第一次出现子串的位置分割，并保留分割子串
(' hell', 'o', ' world ')

>>> " hello world ".partition("a") # 没找到，返回字符串本身和两个空字符串
(' hello world ', '', '')
```

#### 替换

`str.replace(old, new, [count])` 方法会使用子串 `new` 替换字符串中的子串 `old`，从左到右替换 `count` 次。默认情况下，全部替换

> [!tip] 
> 
> 请注意：该方法返回一个全新的字符串，并不会修改原始字符串对象 `str` 
> 
> 原因在于：字符串是一个不可变对象
> 

```python
>>> " hello world ".replace("o", "", 1)
' hell world '
>>> " hello world ".replace("o", "")
' hell wrld '
>>> "c***b***c***c".replace("**", "*")  # 注意：只会前进，不会后退退
'c**b**c**c'
```

#### 移除

有时候，字符串的首尾可能会出现一些我们不期望出现的字符，此时就必须 **修剪** 掉这些字符。在 Python 中，字符串对象提供了一个 `str.strip([chars])` 方法用于剥离字符串首尾出现在 `chars` 中的字符

| 方法                    | 描述                                      |
| :-------------------- | :-------------------------------------- |
| `str.strip([chars])`  | 剥离字符串 `str` 首尾出现在 `chars` 中的字符          |
| `str.lstrip([chars])` | 类似于 `str.strip()`，但是只剥离字符串开始出现的 `chars` |
| `str.rstrip([chars])` | 类似于 `str.strip()`，但是只剥离字符串末尾出现的 `chars` |

> [!attention] 
> 
>  `str.strip()` 方式在  **默认情况下会剥离字符串中的空白字符**
> 

```python
>>> "a\nb\t".strip()   # 默认剥离空白字符
'a\nb'
>>> "a\nb\t".strip("\tba")  # 剥离指定的字符
'\n'
```

在 Python3.9 版本额外提供了两个方法，它们会剥离指定的字符串前缀

| 方法                            | 描述                       |
| :---------------------------- | ------------------------ |
| `str.removeprefix(prefix, /)` | 剥离字符串 `str` 的前缀 `prefix` |
| `str.removesuffix(suffix, /)` | 剥离字符串 `str` 的后缀 `suffix` |

> [!tip] 
> 
> `str.removeprefix(prefix, /)` 方法类似于切片 `str[len(prefix): ]`，只是该方法会检查 `str` 的前缀是否是 `prefix`
> 
> `str.removesuffix(suffix, /)` 方法类似于切片 `str[:-len(suffix)]`，只是该方法会检查 `str` 的后缀是否是 `suffix`
> 
> 

```python
>>> 'MiscTests'.removesuffix('Tests')
'Misc'
>>> 'TmpDirMixin'.removesuffix('Tests')  # 后缀不是 'Tests'，就不会做任何操作
'TmpDirMixin'
```

#### 检查

Python 的字符串提供了一些列方法用于检查字符串是否满足给定的条件

| 方法                                       | 描述                  |
| :--------------------------------------- | :------------------ |
| `str.isalnum()`                          | 判断字符串是否全为字母或数字      |
| `str.isalpha()`                          | 判断字符串中是否全为字母        |
| `str.isisdigit()`                        | 判断字符串是否全为数字         |
| `str.isdecimal()`                        | 判断字符串是否全为十进制数字      |
| `str. isnumeric()`                       | 判断字符是否为数字字符         |
| `str.isidentifier()`                     | 判断字符串是否为标识符         |
| `str.isascii()`                          | 判断字符串是否全为 ASCII 字符, |
| `str.islower()`                          | 判断字符串是否全为小写字符       |
| `str.isupper()`                          | 判断字符串是否全为大写字符       |
| `str.isprintable()`                      | 判断字符串是否为可打印字符       |
| `str.isspace()`                          | 判断字符串中是否全为空白字符      |
| `str.istitle()`                          | 判断字符串是否为标题字符串       |
| `str.startswith(prefix[, start[, end]])` | 判断字符是否为`prefix`开头,  |
| `str.endswith(prefix[, start[, end]])`   | 判断字符串是否为`prefix`结尾, |

```python
>>> "abcd1234".isalnum()  # 判断是否为数字或字母
True
>>> "abcd1234 ".isalnum()  # 判断是否全为数字
False

>>> "abcd".isalpha()  # 判断是否全为字母
True
>>> "abc d".isalpha()
False

>>> "3.12".isdigit()  # 判断是否全为数字, "." 不是数字字符 
False
>>> "3".isdigit()
True

>>> "3.14".isdecimal()  # 判断是否为十进制数字, "." 不是数字
False
>>> "3".isdecimal()
True

>>> "一二三".isnumeric()  # 判断是否全为数字字符，Unicode 码
True
>>> "一二三12".isnumeric()
True
>>> "一二三12Ⅰ".isnumeric()  
True

>>> "else".isidentifier()  # 判断是否为有效的标识符
True

>>> "hello world".islower()  # 判断是否全为小写字符
True
>>> "hello World".islower()
False

>>> "ABCD".isupper()
True

>>> "\n".isprintable()  # 判断字符串是否为可打印字符
False
>>> "10".isprintable()
True

>>> "Title V".istitle()  # 判断字符串是否为标题化字符串
True

>>> "hello world".startswith("he")  # 判断字符串是否为'he'开头
True
>>> "hello world".startswith("he", 0, 3)
True

>>> "hello world".endswith("th")  # 判断字符串是否为'th'结尾
False
>>> "hello world".endswith("ld")
True
```

#### 大小写转换


Python 提供了一系列方法用于清除字符的大小写差异。下表给出了常用的方法

| 方法                 | 描述                  |
| :----------------- | ------------------- |
| `str.title()`      | 标题化字符串, 即是每个单词首字母大写 |
| `str.upper()`      | 大写所有字母              |
| `str.lower()`      | 小写所有字母              |
| `str.swapcase()`   | 字符串大小写互换            |
| `str.capitalize()` | 大写首字母, 其余部分小写       |
| `str.casefold()`   | 消除大小写               |

```python
>>> string = "hello world"
>>> string.title()  # 标题化
'Hello World'
>>> string.upper()  # 大写字母
'HELLO WORLD'
>>> string.swapcase()  # 大小写互换
'HELLO WORLD'
>>> string.capitalize()  # 大写首字母
'Hello world'
>>> string.casefold()  # 消除大小写
'hello world'
>>> "HELLO WORLD".casefold() == string.casefold()
True
```

#### 格式化

Python 为字符串格式化提供了几种方法，其中有两个简单的方法：`str.center()` 和 `str.zfill()`

| 方法                              | 描述                                |
| :------------------------------ | :-------------------------------- |
| `str.center(width, [fillchar])` | 将字符串 `str` 放在宽度为 `width` 的新字符串中间  |
| `str.zfill(width)`              | 将字符串 `str` 使用 `"0"` 填充到宽度 `width` |

```python
>>> "world".center(20) # 默认使用空格填充
'       world        '
>>> "world".center(20, "*")  
'*******world********'

>>> "world".zfill(20)
'000000000000000world'
```


> [!tip] 
> 
> 所谓的格式化就是将其中类型的对象使用对应的格式动词转换为字符串的过程。
> 
> Python 提供三种方式进行字符串格式化：`c-style` 风格，`format` 风格 和 `f-string` 风格
> 

##### `c-style`格式化

字符串具有一种特殊的内置操作：使用 `%` (取模) 运算符。 这也被称为字符串的 __格式化__ 或 __插值__ 运算符。例如，

```python
format % values
```

将 `values` 值插入到 `format` 字符串中 **转换标记**(`%[可选项]转换符`)  的位置上。这是继承自 C 语言 `printf` 函数的格式化方式

**转换标记** 按照从左到右顺序包括如下几个部分

| 转换标记部分     | 描述                      |
| :--------- | :---------------------- |
| `%`        | 转换标记的开始字符，必选的           |
| `(soname)` | 转换标记的名字，可选的             |
| `flag`     | 指定格式，可选的                |
| `m`        | 最小宽度，可选                 |
| `.n`       | 精度，可选                   |
| `length`   | 长度修饰符，可选。Python 会忽略这修饰符 |
| `转换符`      | 格式动词，必选                 |

> [!tip] 
> 
> 最小宽度 `m` 可以指定为字符 `*`，此时需要再 `values` 元组中指出最小宽度的值
> 
> 精度 `.n` 可做指定为 `.*`，也需要在 `values` 元素中支持精度的取值
> 


下表列出了 Python 中支持的所有格式动词，更详细内容可以参见  [[C 语言：基本概念#格式化输入和输出]]  和  [[C 语言：标准 IO 流#格式化输入输出]]

| 转换符   | 含义                               |
| :---- | :------------------------------- |
| `"d"` | 有符号十进制整数                         |
| `"i`  | 有符号十进制整数                         |
| `"o"` | 有符号八进制                           |
| `"u"` | 过时类型，等价于`"d"`                    |
| `"x"` | 有符号十六进制整数；字母小写                   |
| `"X"` | 有符号十六进制整数；字母大写                   |
| `"e"` | 浮点数指数形式；字母`e` 小写                 |
| `"E"` | 浮点数指数形式；字母 `E` 大写                |
| `"f"` | 浮点十进制格式                          |
| `"F"` | 浮点十进制格式                          |
| `"g"` | 浮点数格式                            |
| `"G"` | 浮点数格式                            |
| `"c"` | 单个字符                             |
| `"r"` | 字符串，使用 `repr()` 转换的任何 Python 对象  |
| `"s"` | 字符串，使用 `str()` 转换的任何 Python 对象   |
| `"a"` | 字符串，使用 `ascii()` 转换的任何 Python 对象 |
| `"%"` | 不转换参数，直接输出 `%`                   |

> [!attention] 
> 
> 注意：由于 **Python 字符串显式指明长度**，`%s` 转换不会将 `'\0'` 视为字符串的结束
> 
> 可以给出长度修饰符 (`h`, `l` 或 `L`)，但会被忽略，因为对 Python 来说没有必要，所以 `%ld` 等价于 `%d`
> 

下表列出来 `flag` 标记的取值及其函数，参考  [[C 语言：基本概念#格式化输入和输出]] 

| `flag` | 含义                           |
| :----- | :--------------------------- |
| `"#"`  | 值的转换将使用替代形式                  |
| `"0"`  | 转换将为数字填充字符`"0"`              |
| `"-"`  | 转换值将靠左对齐                     |
| `" "`  | 空格，符号位转换产生整数将留出一个空格          |
| `"+"`  | 符号字符(`"+"`或`"-"`)将显示转换为结果的开头 |

两个示例：

```python
>>> print('%(language)s has %(number)03d quote types.'%{'language': "Python", "number": 2})
Python has 002 quote types.
>>> print('%s has %03d quote types.' % ("Python", 2))
Python has 002 quote types.
```

> [!WARNING]
> 
注意：在键值映射格式化过程中，不能出现 `*` 标记符

##### `format` 方法

可以调用 `format` 字符串的 `str.format()` 方法产生一个 `format` 的副本，其中 `{}` 的位置被替换为 `format` 中指定的参数

下面的例子是使用 `str.foramt()` 方法的最简单的形式，按照 `{}` 在 `format` 字符串中的位置与 `str.format()` 中给出的参数位置一一对应即可，称为 **位置映射**

```python
>>> print('{} has {} quote types.'.format("Python", 2))
Python has 2 quote types.
```

位置映射还可以是这样的形式`{i}` ，其中 `i` 表示 `str.format()` 给出的参数索引

```python
>>> print("{1} has {0} quote types.".format(2, "Python"))
Python has 2 quote types.
```

可以和 `c-style` 风格一样的方式指定格式化的形式，只需要在 花括号中添加冒号(`:`) 并在冒号后自定格式化参数。其中，格式化参数和 `c-style` 转换类型和转换 `flag` 一致

```python
>>> print('{} has {:>04d} quote types.'.format("Python", 2))  # 右对齐
Python has 0002 quote types.
>>> print('{} has {:<4d} quote types.'.format("Python", 2))  # 左对齐
Python has 2    quote types.
```

注意，此时如果想要格式化后的值靠左对齐需要使用 `<` 标志

**键值映射格式化**在 `format` 方法中也是可用的，只需要在花括号中指定名字 `{soname}` 并在 `str.format()` 方法中给出对应的名字和值即可

```python
>>> print('{language} has {number} quote types.'.format(language="Python", number=2))
Python has 2 quote types.
```

##### `f-string` 方式

注意，`f-string` 要求 Python $\ge$ 3.6 才可以使用 `f-string` 方式进行格式化。只需要在 `str.format()` 方式的 `format` 字符串前加上 `f` 即可直接在 `{}` 中书写 Python 表达式

```python
first_name = "ada" 
last_name = "lovelace" 
full_name = f"{first_name} {last_name}"
print(full_name)


name = "Fred"
print(f"He said his name is {name!r}")  # !r 等价于 repr()

width = 10
precision = 4
value = decimal.Decimal("3.1415926")
print(f"result: {value:{width}.{precision}}")  # 嵌套的字段

now = datetime.datetime.now()
print(f"now:{now: %Y-%m-%d %H:%M:%S}")  # 使用日期格式说明符

number = 1024
print(f"{number:#0x}")  # 使用整数格式说明符

foo = "bar"
print(f"{foo = }")  # 保留空格

line = "The mill's closed"
print(f"{ line = }")
print(f"{line = :20}")
print(f"{line = !r:20}")
```

### Bytes

> [!seealso] 
> 
> 我们在 [[数据存储#存储文本]] 中详细介绍了计算机是如何存储字符，这里就不在重复
> 

Python 内部的字符串存储为 `U+000 ~ U+10FFFF` 范围内的 Unicode 码值 ^[[[数据存储#存储文本]]]。由于一个字符占用了多个字节，一旦字符串对象需要再 CPU 和内存以外使用，**字节序** ^[[[信息存储#寻址和字节序]]] 就会成为一个重要的影响因素

> [!tip] 
> 
> 例如，将字符串对象通过网络发送给其他计算机；或者，将字符串对象存储到文件中
> 

因此，我们需要将字符串转为与字节序无关的 **字节序列**，我们称这样的过程为 **编码**。而从字节序列重建字符串被称为 **解码**。 

> [!tip] 
> 
> 存在许多不同的文本序列化编解码器，它们被统称为 **文本编码格式**。
> 

在 Python 中，字节序列使用类型 `bytes` 表示，它是不可变类型，即 `bytes` 对象是不可变对象。`bytes` 类型的对象处理的通常是与类型无关的数据。换句话说，**bytes对象持有的数据是无类型的**

Python 提供了 $2$ 种创建 `bytes` 类型的对象的方法：**字面值** 和 **构造函数 `bytes()`**。老规矩，我们还是先看字面值。注意到，`bytes` 是一个字节序列，也就是说，这个序列的每个元素都只占一个字节。简单来讲，在使用字面值创建 `bytes` 对象时，提供的字符必须是 ASCII 编码的字符

> [!attention] 
> 
> `bytes` 的字面值是基于基于 ASCII 字符，但 **`bytes` 对象的行为实际上更像是不可变的整数序列**，序列中的每个值的大小被限制为 `0 <= x < 256`
> 
> 无论将 `.py` 文件声明的是何种编码，`bytes` 的字面值中只能包含 ASCII 字符。任何超出 $127$ 的二进制值必须使用相应的转义序列形式加入 `bytes` 字面值
> 

```python
>>> b = b"hello, world"  # 本质上就是字符串 "hello, world" 的 ASCII 编码值的序列
>>> for i in b:
...     print(i, type(i))
...
104 <class 'int'>
101 <class 'int'>
108 <class 'int'>
108 <class 'int'>
111 <class 'int'>
44 <class 'int'>
32 <class 'int'>
119 <class 'int'>
111 <class 'int'>
114 <class 'int'>
108 <class 'int'>
100 <class 'int'>
```

表示 `bytes` 字面值的语法与字符串字面值的大致相同，只是添加了一个 `b` 前缀：
- 单引号: `b'同样允许嵌入 "双" 引号'`。
- 双引号: `b"仍然允许嵌入 '单' 引号"`
- 三重引号: `b'''三重单引号'''`, `b"""三重双引号"""

请注意，此处仅仅为了说明 `bytes` 字面值语法，并不是说 `bytes` 字面值中可以使用 ASCII 字符以外的字符

下面我们介绍 `bytes` 对象的构造函数 `bytes()`。首先，我们可以使用一个字符串或者一个字面值，如下示例

```python
class bytes(source=b'')  # 创建一个空字节序列
class bytes(source, encoding)  # source 是字符串，encoding 指定编码方式
class bytes(source, encoding, errors) # errors 指定编码错误的处理程序
```

此外，`bytes` 对象还可以通过其他几种方式来创建：
- 指定长度的以零值填充的 bytes 对象: `bytes(10)`
- 通过由整数组成的可迭代对象: `bytes(range(20))`
- 通过缓冲区协议复制现有的二进制数据: `bytes(obj)`

```python
>>> bytes("中文", "utf-8")
b'\xe4\xb8\xad\xe6\x96\x87'
>>> bytes(10)  # 长度为 10 的零字节序列
b'\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00'
>>> bytes(range(97, 108))  # 使用整数序列
b'abcdefghijk'

>>> import array
>>> a = array.array('b', range(65,76))
>>> a
array('b', [65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75])
>>> bytes(a)  # 使用实现缓冲区协议的对象
b'ABCDEFGHIJK' 
```

> [!tip] 
> 
> `bytes` 类型和 `str` 类型非常相似。`str` 类型的方法，`bytes` 类型都有其对应的字节版本
> 

##### 字符串与 bytes 之间的转换

我们说过在 Python 字符串被存储为 Unicode 码值。当字符串需要存储到磁盘或者通过网络发送时，将字符串需要 Unicode 码值重新编码，将其转换为 `bytes` 类型

> [!important] 
> 
> 字符串对象提供了 `str.encode([codes])` 用于将字符串编码为指定编码方式 `codes` 的字节序列。默认情况下采用 `UTF-8` 编码
> 

此外，也可以使用 `bytes()` 构造函数将一个字符串编码为指定字符编码方式的字节序列

```python
>>> "中文".encode()
b'\xe4\xb8\xad\xe6\x96\x87'
>>> "中文".encode("GB2312")
b'\xd6\xd0\xce\xc4'
```

`bytes` 对象提供了 `bytes.decode([codes])` 方法用于将字节对象转换为 `str` 对象

```python
>>> b'\xd6\xd0\xce\xc4'.decode()  # 注意使用编码时对应的字符集进行解码
Traceback (most recent call last):
  File "<python-input-2>", line 1, in <module>
    b'\xd6\xd0\xce\xc4'.decode()  # 注意使用编码时对应的字符集进行解码
    ~~~~~~~~~~~~~~~~~~~~~~~~~~^^
UnicodeDecodeError: 'utf-8' codec can't decode byte 0xd6 in position 0: invalid continuation byte
>>> b'\xd6\xd0\xce\xc4'.decode("GB2312")
'中文'
```

##### int 与 bytes 之间的转换

我们知道在 Python 中，整数类型(`int`)可以占用任意字节，本质上就是字节序列，只是我们将这个字节序列当做了整数处理

> [!attention] 
> 
> 字节序列可以表示任何东西，它与数据类型无关。因此，当需要向计算机外部传递数据时，通常需要传递与类型无关的字节序列
> 

当我们需要按字节处理整数时，就可以使用 `int` 对象的方法 `int.to_bytes(with, byteorder="big")` 的方式将整数转换为长度为 `with` 的字节序列，整数字节按照大端序排列

```python
>>> (0xeaff).to_bytes()  # 默认是 1 字节按大端序。由于1字节不足以存放 0xeaff，所谓抛出异常
Traceback (most recent call last):
  File "<python-input-4>", line 1, in <module>
    (0xeaff).to_bytes() 
    ~~~~~~~~~~~~~~~~~^^
OverflowError: int too big to convert
>>> (0xeaff).to_bytes(2)  # 默认大端序
b'\xea\xff'
>>> (0xeaff).to_bytes(2, byteorder="big")
b'\xea\xff'
>>> (0xeaff).to_bytes(2, byteorder="little")  # 小端序
b'\xff\xea'
```

当我们有一个字节序列对象时，想要转换为整数，就需要使用 `int` 类型的类方法 `int.from_bytes(bytes, byteorder="big")`，该方法会按照大端序字节序将字节序列转换为一个整数

```python
>>> int.from_bytes(b'\xea\xff')  # 默认大端序
60159
>>> 0xeaff
60159
>>> int.from_bytes(b'\xea\xff', byteorder="little")  # 按小端序解释字节序列
65514
>>> 0xffea
65514
```

### Bytearray

`bytearry` 与 `bytes` 类似，只是 `bytes` 的可变版本。但是，**`bytearray` 对象没有专属的字面值语法**，它们总是通过调用构造器来创建：
- 创建一个空实例: `bytearray()`
- 创建一个指定长度的以零值填充的实例: `bytearray(10)`
- 通过由整数组成的可迭代对象: `bytearray(range(20))`
- 通过缓冲区协议复制现有的二进制数据: `bytearray(b'Hi!')`

> [!attention] 
> 
> 由于 `bytearray` 对象是可变的，该对象除了 `bytes` 和 `bytearray` 操作 中所描述的 `bytes` 和 `bytearray` 共有操作之外，**还支持 可变 序列操作**
> 

> [!important] 
> 
> `bytes` 和 `bytearry` 的元素是 $1$ 字节的无符号整数，即 $[0, 255]$
> 

## 散列

**散列表** 是只不过是一个 **具有固定大小的顺序表**，其中的元素包含一个 **关键字**，和一个 **值**，我们将其记为 `(key, value)`。将数组得大小称为散列表的大小记为 `TableSize`。当我们需要存储一个键值对(`key, value`)时，将 `key` 使用 **散列函数** 处理得到一个 **散列值**，然后使用散列值从 $0$ 到 $\text{TableSize} - 1$ 范围中的某个索引，它指明了`(key, value)` 存储在数组中的位置。

有两种常用的方法将散列值映射到 $0 \sim \text{TableSize}-1$ 范围内的数：**取模法** 和 **位与**

![[Pasted image 20250329161618.png]]


> [!attention] 
> 
> 请注意，当使用位与选择索引时，必须要要求 `TableSize` 的值是 $2^n$。详细参考 [[栈和队列#^2f9461|使用位与代替模运算]]
> 

由于表中存储单元数目有限，但是关键字实际上是无穷无尽的，所以不可能保证任何两个关键字映射到不同的存储单元。**当两个关键字的散列值相同**，称为 **散列冲突**。当出现散列冲突时，通常有两种解决方法：**开放地址法** 和 **链接表法**

所谓的开放地址法是指当出现散列冲突时，就从冲突位置向后选择没有被存储数据的地址；而链接表法则是在出现散列冲突后，就在后面链接一个表即可。如下图所示：假设 `k1` 和 `k2` 的散列值都选择了编号为 $2$ 的位置，由于 `k1` 先占用了位置 $2$。开放地址法就在 `k1` 的后面寻找可插入位置；而链接表法就直接在 `k1` 的后面连一张表即可

![[Pasted image 20250329184710.png]]

Python 中散列表的实现有两类：**集合(`set`)** 和 **字典(`dict`)**。下面我们介绍这两种数据结构

### Set & Frozenset

集合(`set`)是 **可变的** 容器类型；而 `frozenset` 是不可变容器类型 。集合是由 _不重复_ 元素组成的 _无序容器_。既然是容器，那么容器支持的操作(成员检查、迭代容器中的元素)，集合也支持

> [!tip] 
> 
> 集合中存储的对象必须是可以计算哈希值的 **可哈希对象**。由于对象是根据其哈希值选择的存放位置，因此是元素在容器中是无序的
> 

> [!important] 
> 
> Python 中的可哈希对象
> + `int` `float` `complex` 类型的对象
> + `True` `False`
> + 字符串对象
> + 字节序列 `bytes`
> + 元组对象 `tuple`
> + 空值对象 `None`
> 

在 Python 中，创建一个集合对象有两种方式：**集合字面值** 和 **构造函数 `set()`**。从字面值开始，因为字面值是我们最常使用的方式。在 Python 中，集合的字面值词法使用一对 **大括号** 包围一些列的值组成

```python
>>> s = {1,2,"hello", "world", (1,2,3), 3.14}
>>> s
{1, 2, 3.14, 'hello', (1, 2, 3), 'world'}
>>> not_set = {}  # 注意，这样的字面值不是集合，而是字典
>>> type(not_set)
<class 'dict'>
```

> [!attention] 
> 
> 请注意：仅仅使用 `{}` 表示是创建一个空字典，而非空集合
> 

我们可使用 `set(iterable)` 函数创建集合，其中的元素来自可迭代对象 `iterable`，如果其中的有重复元素，在集合中只会出现一次

```python
>>> s = set([1,2,3,4,3,2,1])
>>> s
{1, 2, 3, 4}
>>> s = set()  # 空集合：只能使用构造函数创建空集合
>>> s
set()
```

> [!important]
> 
> 集合有一下几个特性:  
> 1.  **无序性**: 集合中的元素是没有顺序
> 2.  **唯一性**: 相同值的元素在集合中只存在一个
> 3.  集合是**可变数据类型**, 但是集合中的 **元素只能存放不可变(可哈希)的数据.**  如果存放了可变类型数据, 会触发`TypeError`异常
> 

Python >= 3.2.3 对 `str` 和 `bytes` 对象的哈希值会使用一个不可预测的随机值 _加盐_。虽然它们在一个单独 Python 进程中会保持不变，但是它们的值在重复运行的 Python 之间是不可预测的

> [!info] 
> 
> 这是为了**防止**通过精心选择输入来利用字典插入操作在最坏情况下的执行效率即  $O(n^2)$ 复杂度制度的 **拒绝服务攻击**
> 

```shell
dyp@ubuntu:~$ python -c "print(hash('123'))"
-8446606495242098355
dyp@ubuntu:~$ python -c "print(hash('123'))"
-9111418234901541349
dyp@ubuntu:~$ python -c "print(hash('123'))"
-6107098491586316605
```

改变哈希值会影响集合的迭代次序。Python 也从不保证这个次序不会被改变（通常它在 32 位和 64 位构建上是不一致的）

> [!info] 
> 
> 环境变量 `PYTHONHASHSEED` Python 哈希种子：默认为 None。如果此变量未设置或设为 `random`，将使用一个随机值作为 `str` 和 `bytes` 对象哈希运算的种子
> 

```shell
dyp@ubuntu:~$ PYTHONHASHSEED=1 python -c "print(hash('123'))"
8874197587556847498
dyp@ubuntu:~$ PYTHONHASHSEED=1 python -c "print(hash('123'))"
8874197587556847498
dyp@ubuntu:~$ PYTHONHASHSEED=1 python -c "print(hash('123'))"
8874197587556847498
```

已经知道了集合的原理和如何创建集合对象，下面我们介绍集合对象支持的方法和运算符

#### 数学运算

Python 提供的集合类型是模拟数学上的集合概念，因此，Python 必须提供集合的数学运算：**交集** **并集** **差集** **对称差** 等，下表总结了 Python 目前提供的集合运算

| 运算                                       | 描述                       |
| :--------------------------------------- | :----------------------- |
| `&` 或 `set.intersection(*others)`        | 返回 **交集**                |
| $\mid$ 或 `set.union(*others)`            | 返回 **并集**                |
| `-` 或 `set.difference(*others)`          | 返回 **差集**                |
| `^` 或 `set.symmetric_difference(other)`  | 返回 **对称差集**              |
| `set.isdijoint(other)`                   | 检查 `set` 与 `other` 是否不相交 |
| `set.issubset(other)` 或者 `set <= other`  | 检查 `set` 是否是 `other` 的子集 |
| `set.issuperset(other)` 或者 `set>= other` | 检查 `set` 是否包含 `other`    |

> [!tip] 
> 
> 交集：例如 `a & b` 返回一个全新的集合对象，元素是集合 `a` 和 `b` 中共同出现的元素
> 
> 并集：例如 `a | b` 返回一个全新的集合对象，元素是集合 `a` 和 `b` 中都出现的元素
> 
> 差集：例如 `a - b` 返回一个全新的集合对象，元素是集合 `a` 中的元素并且这些元素没有在 `b` 中出现
> 
> 对称差集：例如 `a ^ b` 返回一个全新的集合对象，元素是 `(a - b) | (b - a)`
> 

```python
# 交集
>>> {1,2,3} & {2, 3, 4}  # 交集
{2, 3}
>>> {1,2,3}.intersection({2, 3, 4})
{2, 3}

# 并集
>>> {1,2,3} | {2, 3, 4} # 并集
{1, 2, 3, 4}
>>> {1,2,3}.union({2, 3, 4})
{1, 2, 3, 4}

# 差集
>>> {1,2,3} - {2, 3, 4}
{1}
>>> {1,2,3}.difference({2, 3, 4})
{1}
>>> {2, 3, 4} - {1,2,3}
{4}
>>> {2, 3, 4}.difference({1,2,3})
{4}


# 对称差
>>> {1,2,3} ^ {2, 3, 4}
{1, 4}
>>> {1,2,3}.symmetric_difference({2, 3, 4})
{1, 4}
>>> {1,2,3} - {2,3,4} | {2,3,4} - {1,2,3}
{1, 4}

# 检查是否存在交集
>>> {1,2, 3}.isdisjoint({4,5,6})
True
>>> {1,2, 3}.isdisjoint({3, 4,5,6})
False

# 检查是否是子集
>>> {1,2,3} <= {1,2}
False
>>> {1,2,3} <= {1,2,3,4}
True
>>> {1,2,3} < {1,2,3,4}
True

# 检查是否是超集
>>> {1,2,3} > {1,2}
True
>>> {1,2,3} >{1,2,4}
False
```

> [!attention] 
> 
> 不可变的集合 `frozenset` 也支持上述已经介绍的方法 。下面的方法是 `set` 类型独有的方法
> 

#### 增加

`set` 对象时可变的容器，那么我们可以向其中添加元素。下表列出了 `set` 对象支持的元素添加方法

| 方法              | 描述                   |
| :-------------- | :------------------- |
| `set.add(elem)` | 向集合 `set` 中元素 `elem` |

```python
>>> s = {4, 5, 7, 8}
>>> s.add(9)
>>> s
{4, 5, 7, 8, 9}
```

#### 更新集合

使用其他集合对象更新当前集合中的元素：可以添加元素，也可以移除元素

| 方法                                       | 描述                                   |
| :--------------------------------------- | ------------------------------------ |
| `set.update(*others)`                    | 向 `set` 中添加 `others` 并集中的元素          |
| `set.difference_update(*others)`         | 从 `set` 中移除 `others` 并集中的元素          |
| `set.intersection_update(*others)`       | 从 `set` 中移除元素，只保留 `others` 交集中存在的元素  |
| `set.symmetric_difference_update(other)` | 更新 `set`，只保留 `set` 和 `other` 中均存在的元素 |

方法 `set.update(others)` 等价于 `set |= others | ...`

```python
>>> s = {4, 5, 7, 8, 9}
>>> s.update({1, 2, 4}, {5, 7}, {9, 10})  # 向 s 中添加 others 并集中的元素
>>> s
{1, 2, 4, 5, 7, 8, 9, 10}

>>> s |= {3, 0} | {11, 20}  # 等价于 s.update(others)
>>> s
{0, 1, 2, 3, 4, 5, 7, 8, 9, 10, 11, 20}
```

方法 `set.difference_update(*others)` 等价于 `set -= others | ...`

```python
>>> s = {0, 1, 2, 3, 4, 5, 7, 8, 9, 10, 11, 20}
>>> s.difference_update({3,5,7}, {0, 3, 9}, {11, 20})
>>> s
{1, 2, 4, 8, 10}

>>> s = {0, 1, 2, 3, 4, 5, 7, 8, 9, 10, 11, 20}
>>> s -= {3,5,7} | {0, 3, 9} | {11, 20}
>>> s
{1, 2, 4, 8, 10}
```

方法 `set.intersection_update(*others)` 等价于 `set &= other & ...`

```python
>>> s = {0, 1, 2, 3, 4, 5, 7, 8, 9, 10, 11, 20}
>>> s.intersection_update({3,5,7}, {0, 3,5,7,9})
>>> s
{3, 5, 7}

>>> s = {0, 1, 2, 3, 4, 5, 7, 8, 9, 10, 11, 20}
>>> s &= {3, 5, 7} & {0, 3, 5, 7, 9}
>>> s
{3, 5, 7}
```

方法 `set.symmetric_difference_update(other)` 等价于 `set ^= other`

```python
>>> s = {0, 1, 2, 3, 4, 5, 7, 8, 9, 10, 11, 20}
>>> s.symmetric_difference_update({90, 27, 11, 20, 3, 5, 7})
>>> s
{0, 1, 2, 4, 8, 9, 10, 90, 27}
```

#### 删除

将一个元素从集合中移除也是非常有用的操作。Python 的 `set` 对象提供的删除元素的方法在下表列出

| 方法                  | 描述                                        |
| :------------------ | :---------------------------------------- |
| `set.remove(elem)`  | 从 `set` 中移除元素 `elem`，不存在则抛出 `KeyError`    |
| `set.discard(elem)` | 类似于 `set.remove()`，只是它不抛出异常               |
| `set.pop()`         | 从 `set` 中删除一个元素并返回，`set` 为空则抛出 `KeyError` |
| `set.clear()`       | 清空 `set`                                  |

```python
>>> s = {0, 1, 2, 4, 8, 9, 10, 90, 27}
>>> s.remove(0)
>>> s
{1, 2, 4, 8, 9, 10, 90, 27}
>>> s.remove(20)   # 不存在抛出 keyError
Traceback (most recent call last):
  File "<python-input-69>", line 1, in <module>
    s.remove(20)
    ~~~~~~~~^

>>> s.discard(10)
>>> s
{1, 2, 4, 8, 9, 90, 27}
>>> s.discard(20)  # 没有 20 也不会抛出 KeyError

>>> s.pop()
1
>>> s.pop()
2
>>> s.pop()
4

>>> s.clear()
>>> s
set()
```

### Dict

Python 中的字典(`dict`) 就是真正的散列表了，它是是可变的键值对容器。由于要对 `key`  求哈希值，因此 `key` 必须是可哈希对象，至于 `value` 的类型没有任何要求

> [!tip] 
> 
> 在 Python 中，通常使用 **字符串** 的对象作为字典的键，此外还可以使用 **数字**、**只包含不可变对象的元组**
> 
> 请注意：如果一个元组包含了列表，由于列表是可变的，从而会导致元组的值发生变化，影响到元组的哈希值
> 

我们 **可以把字典理解为 _键值对_ 的集合**，但是要求 **键必须是唯一的**。在 Python 中，创建字典可以使用 **字典字面值** 或 **构造函数 `dict()`**

字典字面值使用 **花括号(`{}`)** 包围的形如 `key:value` 的集合，例如

```python
>>> person = {"name": "小明", "age": 20, "gender": "male"}
>>> person
{'name': '小明', 'age': 20, 'gender': 'male'}
```

使用构造函数 `dict()` 创建字典对象的形式就比较多了，下面列表了常见的用法

```python
>>> p = dict(person)  # 返回一个字典对象的拷贝
>>> p
{'name': '小明', 'age': 20, 'gender': 'male'}

>>> p = dict((("name", "小明"), ("age", 20), ("gender", "male")))
>>> p
{'name': '小明', 'age': 20, 'gender': 'male'}

>>> p = dict([["name", "小明"], ["age", 20], ["gender", "male"]])
>>> p
{'name': '小明', 'age': 20, 'gender': 'male'}

>>> p = dict(zip(["name", "age", "gender"], ["小明", 20, "male"]))
>>> p
{'name': '小明', 'age': 20, 'gender': 'male'}

>>> p = dict(name="小明", age=20, gender="male")
>>> p
{'name': '小明', 'age': 20, 'gender': 'male'}
```

> [!tip] 
> 
> 上述这些方式可以混合使用，但是我们建议只单纯的使用一种方式
> 

Python 为操作字典对象提供了许多方法，下表列出了字典常用操作

| 方法                | 描述                                              |
| :---------------- | ----------------------------------------------- |
| `list(dict)`      | 从 `dict` 中获取 `key`，并返回 `key` 形成的列表              |
| `len(dict)`       | 获取 `dict` 的长度                                   |
| `dict[key]`       | 从 `dict` 中返回 `key` 对应的 `value`。没有则抛出 `KeyError` |
| `dict[key]=value` | 设置 `key:value` 键值对                              |
| `del dict[key]`   | 从 `dict` 中删除 `key`。没有则抛出 `KeyError              |
| `key in dict`     | 检查 `key` 是否在 `dict` 中                           |
| `iter(d)`         | 返回 `dict` 中 `key` 形成的迭代器                        |

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

#### 获取值

通过 `key` 获取 `value` 除了使用 `dict[key]` 以外，Python 还提供了额外的方法

| 方法                                   | 描述                                |
| :----------------------------------- | :-------------------------------- |
| `dict.get(key, default=None)`        | 返回 `key` 对应的 `value`              |
| `dict.setdefault(key, default=None)` | 返回 `key` 对应的 `value`              |
| `dict.values()`                      | 返回 `dict` 中所有 `values` 组成的迭代器     |
| `dict.items()`                       | 以 `(key, value)` 形式返回 `dict` 中的元素 |

> [!tip] 
> 
> `dict.get()` 和 `dict.setdefault()` 都可以用于获取 `key` 对应的 `value`，它们之间的区别就是当不存在 `key` 是 `dict.get()` 会返回参数 `default` 指定的值，默认为 `None`
> 
> 然而，`dict.setdefault()` 则将 `key:default` 添加到 `dict` 中，并返回参数 `default` 的值
> 

```python
>>> p = {'name': '小明', 'age': 20, 'gender': 'male'}
>>> p.get("gender")
'male'
>>> p.setdefault("name")
'小明'
>>> p
{'name': '小明', 'age': 20, 'gender': 'male'}
>>> p.setdefault("math")
>>> p
{'name': '小明', 'age': 20, 'gender': 'male', 'math': None}
>>> p.values()
dict_values(['小明', 20, 'male', None])
>>> p.items()
dict_items([('name', '小明'), ('age', 20), ('gender', 'male'), ('math', None)])
```

#### 修改

除了通过表达式 `dict[key] = value` 的方式添加或修改 `dict` 中的键值对外，我们还可以使用下表列出来的方法

| 方法                               | 描述                        |
| :------------------------------- | :------------------------ |
| `dict.update(other)`             | 使用 `other` 中的键值对更新 `dict` |
| $\text{dict} \mid= \text{other}$ | 使用 `other` 中的键值对更新 `dict` |

```python
>>> p
{'name': '小明', 'age': 20, 'gender': 'male', 'math': None}
>>> p.update({"math": 90, "Chinese": 120})
>>> p
{'name': '小明', 'age': 20, 'gender': 'male', 'math': 90, 'Chinese': 120}
```

#### 删除

通过 `del dict[key]` 删除字典中的键存在风险的，因为 `key` 可能不存在会抛出 `KeyError`。因此，Python 提供了下表列出的方法用于删除 `dict` 中的键

| 方法                        | 描述                              |
| :------------------------ | :------------------------------ |
| `dict.pop(key[,default])` | 删除 `dict` 中的 `key`，并返回其 `value` |
| `dict.popitem()`          | 从 `dict` 中移除并返回 `(key, value)`  |

> [!tip] 
> 
> 请注意，方法 `dict.pip()` 如果没有提供参数 `default`，则当 `key` 不存在时，也会抛出 `KeyError`
> 
> `dict.popitem()` 在 Python 3.7 之前是随机删除，而在 Python3.7 之后，会确保采用 `LIFO` 顺序
> 

```python
>>> p
{'name': '小明', 'age': 20, 'gender': 'male', 'math': 90, 'Chinese': 120}
>>> p.pop("math")
90
>>> p.popitem()
('Chinese', 120)
```

#### 字典视图对象

由 `dict.keys()`,` dict.values()` 和 `dict.items()` 所返回的对象是 **视图对象**。 该对象提供 **字典元素的动态视图**

> [!important] 
> 
> 当字典改变时，视图也会相应改变
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

字典视图 **可以被迭代** 以产生与其对应的数据，并 **支持成员检测**
