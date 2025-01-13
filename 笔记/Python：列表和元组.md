# 列表和元组

Python 提供的内建数据结构 **列表** 和 **元组**，这两个是 [[线性表]] 的顺序实现。在 Python 中，列表和元组成为 **序列**。

> [!tip] 
> 
> 所谓的序列，就是其 **元素有序** 的容器。本质上就是线性表
> 

在 Python 中，对象可以分为 **可变对象** 和 **不可变对象** 两种

> [!tip] 可变对象
> 
> 在对象 `id()` 不变的情况下，改变对象的值。例如，**列表**
> 

> [!tip] 不可变对象
> 
> **具有固定值的对象**。不可变对象包括 **数字** **字符串** 和 **元组**。这样的对象的值是不能被改变。
> 
> **如果必须存储一个不同的值，则必须创建新的对象**。它们在需要常量 **哈希值** 的地方起着重要作用
> 

## 列表

列表是 Python 中常用的数据结构，其类型是 `list`。在 Python 中创建 `list` 对象方式非常简单

> [!tip] 字面值：创建列表对象最常用的方式
> 
> Python 中 `list` 对象的字面值形如 `[value1, value2, ...]`
> + `list` 中的每一项由逗号(`,`) 分隔，并被方括号(`[]`) 包围 
> + `value` 的类型可以是不同
> 
> 构造 `list` 最常用的方式就是使用字面值 `[]`，只要 `[]` 出现 Python 解释器就会创建一个全新的 `list` 对象
> 

> [!tip] 构造函数
> 
> 使用构造函数 `list()` 或者 `list(iterable)` 可以创建一个列表
> + 列表中的元素要么为空，要么来自 `iterable`  
> 
> `iterable` 是一种 **可迭代对象**，如果 `iterable` 本身就是 `list`，那么就会返回 `iterable` 的副本
> 

```python
>>> squares = [1, 4, 9, 16, 25]
>>> squares
[1, 4, 9, 16, 25]
>>> data = ["duyupeng", 28]
>>> data
['duyupeng', 28]
```

> [!attention] 如何才能做到保存不同类型的值？
> 
> 我们知道顺序表中存储的是相同类型的元素。Python 中列表也不能违背这个规则，那么它是如何做到保存不同类型的值的呢？
> 
>  对于不同类型的数据，占用的内存空间可能不相同。但是，当数据保存在内存中之后，内存地址就可以标识该数据。对于一个固定的系统，内存地址的长度是固定的，Python 就是在列表中 **存储其对象的内存地址来存储不同类型的对象的**
>  
>  ![[Pasted image 20250113185145.png]]
> 
> 使用内存地址表示对象的这种方式称为引用。不仅仅是在列表这样，Python 中的变量也是只保存一个对象所在的内存地址，而不代表该对象所在的位置
> 
> ![[Pasted image 20250113124805.png]]
> 

### 通用序列操作

大多数序列都支持下面的介绍的操作。可变序列和不可变序列都支持下列操作

#### `in` 和 `not in`

序列作为一种容器，检查某个值是否在容器中是必须被支持的

| 运算           | 结果                                           |
| :----------- | :------------------------------------------- |
| `x in s`     | 如果序列 `s` 中的某一项等于 `x`，则结果为 `True`，否则为 `False` |
| `x not in s` | 如果序列 `s` 中的某一项等于 `x`，则结果为 `False`，否则为 `True` |

```python
>>> squares = [1, 4, 9, 16, 25]
>>> 1 in squares     # 1 在容器 squares 中
True
>>> 1 not in squares
False
>>> 20 in squares   # 20 不在容器 squares 中
False
>>> 20 not in squares
True

```

#### 拼接

| 运算                 | 结果                    |
| :----------------- | :-------------------- |
| `s + t`            | 序列 `s` 与序列 `t` 进行拼接操作 |
| `s * n` 或者 `n * s` | 序列 `s` 与自身拼接 `n` 次    |

```python
>>> squares = [1, 4, 9, 16, 25]
>>> cubes = [1, 8, 27, 64, 125]
>>> squares + cubes   # 拼接两个序列，返回一个全新序列
[1, 4, 9, 16, 25, 1, 8, 27, 64, 125]
>>> squares * 3  # 将序列重复拼接 3 次
[1, 4, 9, 16, 25, 1, 4, 9, 16, 25, 1, 4, 9, 16, 25]
```

#### 索引和切片

| 运算         | 结果                                 |
| :--------- | :--------------------------------- |
| `s[i]`     | 索引取值，即序列 `s` 中第 `i`  项             |
| `s[i:j]`   | 切片，即序列 `s` 从 `i` 到 `j` 的切片         |
| `s[i:j:k]` | 切片，即序列 `s` 从 `i` 到 `j`，步长为 `k` 的切片 |


```python
>>> squares = [1,4,9,16,25]
>>> squares
[1, 4, 9, 16, 25]
>>> squares[0]  # 索引操作将返回条目
1
>>> squares[-1] # 支持负索引
25
>>> squares[-3:]  # 最后三个元素切片
[9, 16, 25]
```

> [!tip] 负索引
> 
> 下图展示了正索引与负索引之间的关系：$正索引=len(序列)+负索引$
> 
> ![[Pasted image 20250113132630.png]]
> 
> - **正索引**：从 $0$ 开始编号，到 `len(list) - 1` 为止。`len(list)` 表示列表中的元素个数
> - **负索引**：从 $-1$ 开始编号，到 `-len(list)` 为止

索引操作，必须满足给定的范围，对于越界的索引，会触发 `IndexError` 异常

```python
>>> squares[4]
25
>>> squares[5]
Traceback (most recent call last):
  File "<stdin>", line 1, in <module>
IndexError: list index out of range
```

#### 操作序列的内建函数和方法

| 运算                   | 结果                                           |
| :------------------- | :------------------------------------------- |
| `len(s)`             | 获取序列 `s` 的长度                                 |
| `min(s)`             | 获取序列 `s` 中的最小值                               |
| `max(s)`             | 获取序列 `s` 中的最大值                               |
| `s.index(x[,i[,j]])` | 元素 `x` 在序列 `s` 中首次出现的位置。在索引 `i` 之后，索引 `j` 之前 |
| `s.count(x)`         | 统计元素 `x` 在序列中出现的次数                           |

### 列表方法

#### 修改元素

与 `immutable` 字符串不同，列表是 `mutable` 类型，其内容是可以改变的

```python
>>> cubes = [1, 8, 27, 65, 125]  # 这里有点问题
>>> 4 ** 3  # 4 的立方是 64，不是 65！
64
>>> cubes[3] = 64  # 替换错误的值
>>> cubes
[1, 8, 27, 64, 125]
```

> [!tip] mutable：可变对象
> 
> 可变对象可以在其 `id()` 保持固定的情况下改变其取值
> 

如果需要修改多个值，可以采用 **切片赋值** 的方式批量修改列表中元素

```python
>>> a = list(range(10))
>>> t = []
>>> t[:] = a
>>> t == a
True
>>> t is a
False
>>> t
[0, 1, 2, 3, 4, 5, 6, 7, 8, 9]
>>> t[1:3]=5  # 必须使用可迭代对象
Traceback (most recent call last):
  File "<stdin>", line 1, in <module>
TypeError: can only assign an iterable
>>> t[1:3]=[5]  # 将序列的 1:3 索引的内容替换为 [5]
>>> t
[0, 5, 3, 4, 5, 6, 7, 8, 9]
>>> t[1:3] = []  # 删除 1:3 索引内的元素
>>> t
[0, 4, 5, 6, 7, 8, 9]
>>> t[1:] = range(10,20)  # 覆盖 + 扩展
>>> t
[0, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19]
```

> [!attention] 
> 
> 在执行切片赋值时，`=` 右边必须是可迭代对象
> 

#### 添加元素

还可以通过使用 `list.append()` 方法在列表的末尾添加新条目

```python
>>> cubes.append(216)    # 添加 6 的立方
>>> cubes.append(7 ** 3) # 添加 7 的立方
>>> cubes
[1, 8, 27, 64, 125, 216, 343]
```

> [!tip] 
> 
> `list.append()` 方法如果不触发列表的 **扩容机制**，则其时间复杂度为 $O(1)$
> 

假设已经有一个可迭代对象，如果需要将这个可迭代对象中的元素追加到列表末尾，可以使用 `list.extend(iterable)` 方法。该方法将 `iterable` 中的每个元素按顺序追加到列表末尾

```python
>>> squares
[]
>>> squares.extend([1, 4, 9, 16, 25, 36, 49])
>>> squares
[1, 4, 9, 16, 25, 36, 49]
```

> [!tip] 
> 
> `list.extend(iterable)` 方法如果不触发列表的扩容机制，则其时间复杂度为 $O(n)$
> 

`list.insert(index, value)` 在指定 `index` 中插入 `value`

```python
>>> li = [1,2,3]
>>> li
[1, 2, 3]
>>> li.insert(0, -1)
>>> li
[-1, 1, 2, 3]
>>> li.insert(-1, 5)  # 这不是 append
>>> li
[-1, 1, 2, 5, 3]
>>> li.insert(len(li) + 3, 5)
>>> li
[-1, 1, 2, 5, 3, 5]
```

> [!tip] 
> 
> 如果指定的 `index` 超过列表的长度(`index >= len(list)`)，则插入在 `list` 的末尾 
> 

#### 删除元素

删除列表中的元素也是允许的。使用 `del` 语句可以删除列表中的某个元素

```python
>>> cubes
[1, 8, 27, 64, 125, 216, 343]
>>> del cubes[-1]  # 删除最有一个元素
>>> cubes
[1, 8, 27, 64, 125, 216]
```

列表对象提供了如下几种方法用于删除列表中的元素

`list.pop(index=-1)`: 指定`index` 索引删除元素, 默认删除列表末尾。 并返回删除的元素。当列表中无元素, 抛出 `IndexError`

> [!note] 实现细节
> 将 `index` 之后的元素向前移动一位。平均时间复杂度为 $O(n)$
> - 最好情形是删除列表最后一个元素，最好时间复杂度为 $O(1)$

```python
>>> cubes
[1, 8, 27, 64, 125, 216]
>>> cubes.pop()  # 删除最后一个元素
216
>>> cubes.pop(1) # 删除索引为 1 的元素
8
>>> cubes
[1, 27, 64, 125]
```

`list.remove(value)`: 指定 `value` 值删除. 如果没有这样的元素，则抛出 `ValueError` 异常

> [!note] 实现细节
> 先找到 `value`，将 `value` 之后的元素向前移动一位。平均时间复杂度为 $O(n)$

```python
>>> squares
[1, 4, 9, 16, 25]
>>> squares.remove(9)  # 删除元素 9
>>> squares
[1, 4, 16, 25]
```

`list.clear()`: 清空列表中的元素

> [!note]  实现细节
> 会将 `list` 对象的长度设置为 $0$，`list` 中的每个元素的引用计数减 $1$
> 

```python
>>> squares
[1, 4, 16, 25]
>>> squares.clear()
>>> squares
[]
```

#### 排序和翻转

列表提供了下面两个方法用于 **原地排序** 和 **原地反转** 列表

`list.sort(*,key=None, reverse=False)`: 排序, `key`用于指定元素在排序前的操作；`reverse`用于控制升序或降序。默认为升序排列

> [!tip] 
> 
> `sort` 方法采用 [[LIPIcs.ESA.2018.4.pdf]]  中的排序算法，于 2002 年由  Tim Peters 设计。该算法从 **归并排序** 和 **插入排序** 改进而来，具有如下特性
> - 稳定：相同元素在排序后的位置关系不变
> - 快速：最坏时间复杂度为 $O(n\log n)$，最好时间复杂度为 $O(n)$
> - 空间消耗：空间复杂度为 $O(n)$
> 

`list.reverse()` : 原地反转列表

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

### 列表扩容机制

向容器中增加元素后的元素个数大于容器容量时，会触发容器的扩容策略。Python 列表扩容策略在 `cpython/Objects/listobject.c` 中的函数 `list_resize` 给出了列表的扩展策略

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

> [!summary] 列表的扩容策略
> - 如果 `newsize` 在当前分配容量的一半以上且小于当前容量，意味着内存容量足够，不需要重新分配，只需更新长度即可
> - 如果需要重新分配，按照 `(newsize * 1.125 + 6) & ~3` 分配。即，将扩容为 `newsize` 的 $1.125$ 倍加 $6$，并调整大小为 $4$ 的倍数。这样做是为了减少频繁的内存分配和拷贝，提高性能。
> 

### 复制

在介绍复制之前，先了解一个内置函数 `id()`。它返回对象的一个编号，对于 CPython 而言，返回的是对象的 **内存地址**

```python
>>> name = "dyp"
>>> age = 26
>>> id(name)
1565296096768
>>> id(age)
140734678632136
```

#### 赋值不是复制

在 Python 中，对变量的赋值并不是将对象的数据拷贝到变量中，而是将对象的 `id` 拷贝到变量中

```python
>>> a = 65535
>>> b = a
>>> a is b
True
>>> id(a), id(b)
(1565295922832, 1565295922832)
>>> li1 = [1,2,3]
>>> li2 = li1
>>> id(li1), id(li2)
(1565296207040, 1565296207040)
```

将一个变量赋值给另一个变量后的内存结果

![[Pasted image 20250113140752.png]]

**Python 中的简单赋值绝不会复制数据**。 当你将一个列表赋值给一个变量时，该变量将引用 现有的列表。你通过一个变量对列表所做的任何更改都会被引用它的所有其他变量看到。

```python
>>> rgb = ["Red", "Green", "Blue"]
>>> rgba = rgb
>>> id(rgb) == id(rgba)  # 它们指向同一个对象
True
>>> rgba.append("Alph")
>>> rgb
['Red', 'Green', 'Blue', 'Alph']
```

> [!important] 
> 
> Python 中的**赋值**一定不是对象的复制，**只是将对象的地址给新的变量**，然后对象的 **引用计数加 $1$**
> 

#### 拷贝列表

> [!question] 如何才能拷贝出一个新的列表？
> + 切片可以复制一个列表
> + `list.copy()` 也可以复制一个列表
>

列表的切片本质上就是对列表中部分元素的拷贝。因此，我们可以通过切片的方式拷贝整个列表

```python
>>> rgb
['Red', 'Green', 'Blue']
>>> rgba = rgb[:]  # 返回 rgb 的拷贝
>>> rgba
['Red', 'Green', 'Blue']
>>> id(rgb) == id(rgba)  # rgb 与 rgba 引用不同的对象
False
>>> rgba.append("Alph")
>>> rgb
['Red', 'Green', 'Blue']
>>> rgba
['Red', 'Green', 'Blue', 'Alph']
```

`list.copy()` 方法就可以使用 `list` 中的元素创建一个新的列表

```python
>>> rgb
['Red', 'Green', 'Blue']
>>> rgba = rgb.copy()  # 返回 rgb 列表的拷贝
>>> rgba
['Red', 'Green', 'Blue']
>>> id(rgb) == id(rgba)  # 它们指向不同的对象
False
>>> rgba.append('Alph')
>>> rgb
['Red', 'Green', 'Blue']
>>> rgba
['Red', 'Green', 'Blue', 'Alph']
```

### 浅复制 vs 深复制

#### 浅复制

切片和 `list.copy()` 执行的都是浅复制。什么是浅复制呢？

> [!tip] 
> 
> 在 Python 中复制对象时，默认执行浅复制。也就说，在复制对象的内容时，如果某些内容是其对象的引用，**浅复制只会拷贝该引用**，而 **不会拷贝该引用的对象**
> 

下图展示了浅复制原理。红色的是复制出来的对象

![[Pasted image 20250113142338.png]]

```python
>>> a = [1,2,[3,4]]  # a 中的最后一个元素引用了一个列表
>>> b = a[:]  # 执行浅复制
>>> a == b  # a 和 b 的内容相同
True
>>> a
[1, 2, [3, 4]]
>>> b
[1, 2, [3, 4]]
>>> a[2]
[3, 4]
>>> a[2][0] = 100  # 修改 a 的最有一个元素中的值
>>> a
[1, 2, [100, 4]]
>>> b
[1, 2, [100, 4]]
```

#### 深复制

所谓的 **深复制**，就是在拷贝对象时，需要拷贝对象引用的对象。

> [!tip] 
> 
> 在复制对象的内容时，如果某些内容是其他对象的引用，**深复制会复制该引用的对象**，而不是仅仅复制引用

下图展示了深复制的原理。绿色的部分就是深复制处理的全新对象

![[Pasted image 20250113143252.png]]

浅复制仅仅拷贝引用，深复制则需要重新创建一个可变类型对象，它的值和原来的那个可变类型对象一样

对于不可变类型，无论深复制还是浅复制，复制的都是对象的引用，只是该对象是不能通过引用修改对象的值

```python
>>> import copy
>>> b = copy.deepcopy(a)
>>> b
[1, 2, [100, 4]]
>>> a
[1, 2, [100, 4]]
>>> id(a[0])
140734678631336
>>> id(b[0])
140734678631336
>>> id(a[2])
1565296206464
>>> id(b[2])
1565296209536
>>>
```

显然，`a[2]` 与 `b[2]` 不再引用同一个对象

### 列表推导式

**列表推导式创建列表的方式更简洁**。常见的用法为，对序列或可迭代对象中的每个元素应用某种操作，用生成的结果创建新的列表；或用满足特定条件的元素创建子序列

例如，创建平方值的列表

```python
>>> squares = []
>>> for i in range(1, 11):
...     squares.append(i ** 2)
...
>>> squares
[1, 4, 9, 16, 25, 36, 49, 64, 81, 100]
```

注意，上述代码会产生一个副作用：创建或覆盖了变量 `i`，该变量在循环结束后仍然存在

```python
>>> squares = []
>>> for i in range(1, 10):
...     squares.append(i ** 2)
...
>>> squares
[1, 4, 9, 16, 25, 36, 49, 64, 81]
>>> i  # 循环结束后，变量 i 依旧存在
9
```

如果需要无副作用地计算平方列表，可以使用列表推导式

```python
>>> squares = [ i ** 2 for i in range(1,10)]
>>> squares
[1, 4, 9, 16, 25, 36, 49, 64, 81]
```

> [!tip] 
> 
> 列表推导式的方括号内包含以下内容：表达式，必须的 `for` 语句，零个或多个 `for` 或 `if` 子句
> 
> 结果是由表达式依据 `for` 和 `if` 子句求值计算而得出一个新列表
> 

```python
>>> combs = [(x, y) for x in [1,2,3] for y in [3, 1, 4] if x != y]
>>> combs
[(1, 3), (1, 4), (2, 3), (2, 1), (2, 4), (3, 1), (3, 4)]
```

上述列表推导式等价于

```python
>>> combs = []
>>> for x in [1,2,3]:
...     for y in [3,1,4]:
...         if x != y:
...             combs.append((x, y))
...
>>> combs
[(1, 3), (1, 4), (2, 3), (2, 1), (2, 4), (3, 1), (3, 4)]
```

列表推导式中的表达式可以是任意表达式，甚至可以是另一个列表推导式

```python
>>> matrix = [
...     [1, 2, 3, 4],
...     [5, 6, 7, 8],
...     [9, 10, 11, 12],
... ]
>>> matrix
[[1, 2, 3, 4], [5, 6, 7, 8], [9, 10, 11, 12]]
```

下面的列表推导式可以转置行

```python
>>> [[row[i] for row in matrix ] for i in range(4)]
[[1, 5, 9], [2, 6, 10], [3, 7, 11], [4, 8, 12]]
```

这个列表推导式等价于

```python
>>> transposed = []
>>> for i in range(4):
...     transposed.append([row[i] for row in matrix])
...
>>> transposed
[[1, 5, 9], [2, 6, 10], [3, 7, 11], [4, 8, 12]]
```

实践中，最好使用内置函数替代复杂的流程语句。这里，我们可以使用 `zip()` 函数

```python
>>> list(zip(*matrix))
[(1, 5, 9), (2, 6, 10), (3, 7, 11), (4, 8, 12)]
```

## 元组

元组(`tuple`)与列表有(`list`)许多相似之处。其中最主要的就是，它们都是线性表的顺序实现。不同的是，**元组根据需要分配固定的内存**，而列表可以动态扩展

> [!tip] 
> 
> + 元组只分配固定大小的内存
> + 列表需要多分配一些内存，从而避免插入数据数据时触发扩容机制带来的性能损耗
> 

在 Python 创建元组对象，也有两种方式：**字面值** 和 **构造函数**

> [!tip] 字面值
> 
> 形如 `(value1,value2,...)` 称为元组字面值。注意，标注元组的并不是圆括号，而是其中的逗号(`,`)
> 

```python
>>> t = 12345,54321,'hello'
>>> t
(12345, 54321, 'hello')
>>> u = t,(1,2,3,4,5)  # 嵌套元组
>>> u
((12345, 54321, 'hello'), (1, 2, 3, 4, 5))
>>> 1,
(1,)
>>> (1)  # 仅仅只是整数 1
1
>>> ()  # 空元组
()
```

> [!tip] 构造函数
> 
> 元组构造函数 `tuple(iterable)`，使用 `iterable` 中元素创建一个元组对象
> 

```python
>>> t = tuple([12345,54321,'hello'])
>>> t
(12345, 54321, 'hello')
>>> empty = tuple()
>>> empty
()
>>> one = tuple([1])
>>> one
(1,)
```

> [!attention] 
> 
> 在输出元组时，元组采用圆括号标注，这样才能正确解释嵌套元组
> 
> 在输入元组时，圆括号可有可无，但是通常是必须的
> 

元组也序列类型，因此在 [[#通用序列操作]] 中的所有操作也适用于元组。

```python
>>> squares = (1, 4, 9, 16, 25)
>>> squares[0]  # 元组的第 0 个元素
1
>>> squares[-1]
25
>>> squares[-3:]
(9, 16, 25)
>>> 9 in squares  # 9 是否在 squares 中
True
>>> 20 not in squares # 20 是否不在 squares 中
True
>>> cubes = (1, 8, 27, 64, 125)
>>> squares + cubes  # 拼接两个元组
(1, 4, 9, 16, 25, 1, 8, 27, 64, 125)
>>> squares * 3
(1, 4, 9, 16, 25, 1, 4, 9, 16, 25, 1, 4, 9, 16, 25)
```

> [!attention] 元组是 immutable
> 
> 元组中的元素的引用是不能被修改的。元组的不可变是指元组保存的引用不可变，而不是元组的值不可变
> 
> ![[Pasted image 20240611224305.png]]
> 
>  元组中的引用不可删除、不可替换。倘若引用的是可变对象，改动对象之后，元组的值也会随之变化
> 

```python
>>> t = (10, 'alpha', [1, 2])
>>> t[0] = 100
Traceback (most recent call last):
  File "<python-input-25>", line 1, in <module>
    t[0] = 100
    ~^^^
TypeError: 'tuple' object does not support item assignment
>>> t[2][0] = 10  # 修改元组中保存的可变对象的值，会导致元组的值发生改变
>>> t
(10, 'alpha', [10, 2])
```

### 元组 vs 列表

虽然，元组与列表很像，但使用场景不同，用途也不同。元组是 `immutable` （不可变的），一般可包含异质元素序列，通过 **解包** 或索引访问（如果是 `namedtuples`，可以属性访问）。列表是 `mutable` （可变的），列表元素一般为同质类型，可迭代访问。

> [!tip] 
> 
> + 元组一般用于存储异质元素。通过解包或索引访问
> 
> + 列表一般用于存储同质元素，通过迭代访问
> 

### 打包 vs 解包

语句 `t = 12345, 54321, 'hello!'` 是 **元组打包** 的例子：值 `12345`, `54321` 和 `'hello!'` 一起被打包进元组。逆操作也可以

```python
x,y,z = t
```

在 [[Python：基础语法#while 语句]] 中使用的赋值语句 `a, b = b, a + b` 就是元组的打包与解包的使用例子

通过元组的打包和解包，可以方便的交换两个变量的值

```python
>>> a = 10
>>> b = 20
>>> a, b = b, a
>>> a
20
>>> b
10
```

> [!tip] 序列打包与解包
> 
> 称之为 **_序列解包_** 也是妥妥的，**_适用于右侧的任何序列_**。序列解包时，左侧变量与右侧序列元素的数量应相等。有时，序列太长，且又需要解包操作时，我们可以在变量前面加上 `*` 表示将剩余元封包在一个列表中
> 

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

