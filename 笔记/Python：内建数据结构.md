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

在 Python 中创建列表对象可以使用 **列表字面值** 或者 **类型转换函数 `list()`**。使用字面值创建一个列表对象是 Python 中最常用的方式。

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

下面我们介绍如何创建元组对象。Python 提供了 $2$ 种创建元组对象的方法：**字面值** 和 **类型转换函数 `tuple()`**

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
> 使用类型转换函数 `tuple(iterable)` 时，需要提供一个可迭代对象作为参数，该函数会使用可迭代对象中元素创建一个全新的元组对象并返回
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
format % value
```

将 `value` 值插入到 `format` 字符串中 转换标记(`%[可选项]转换符`)  的位置上。这是继承自 C 语言 `printf` 函数的格式化方式

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

**键值映射格式化**在 `format` 方法中也是可用的，只需要在花括号中指定名字 `{somename}` 并在 `str.format()` 方法中给出对应的名字和值即可

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



### Bytearray




## 集合





## 字典


