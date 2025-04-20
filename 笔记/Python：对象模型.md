# 对象模型

## tracemalloc：跟踪内存分配

`tracemalloc` 模块是一个用于对 Python 已申请的内存块进行调试的工具。它能提供以下信息:

- 回溯对象分配内存的位置
- 按文件、按行统计 Python 的内存块分配情况: 内存块总大小、数量以及块平均大小
- 对比两个内存快照的差异，以便排查内存泄漏

下面我们来看看如何跟踪内存

```python
import tracemalloc


# 开始跟踪内存分配
tracemalloc.start()

# 创建 1000000 个字典，存储在一个列表中
d = [dict(zip('xy', (5, 6))) for _ in range(1000000)]
# 创建 1000000 个元组，存储在一个列表中
t = [tuple(zip('xy', (5, 6))) for _ in range(1000000)]

snapshot = tracemalloc.take_snapshot()    # 获取当前内存快照
top_stats = snapshot.statistics('lineno')  # 统计快照对象

for stat in top_stats:
    print(stat)
```

执行上述代码的输出结果如下

```
./test.py:7: size=184 MiB, count=1999983, average=96 B
./test.py:8: size=168 MiB, count=3000004, average=59 B
```

输出结果表面字典占用的内存要比元组占用的内存大。这是因为 **字典会提前分配一些额外内存**；而 **元组只会分配它需要的内存**

关于快照的统计选项可以有下面几种

| 统计            | 描述        |
| ------------- | --------- |
| `'filename'`  | 按文件名统计    |
| `'lineno'`    | 按文件名和行号统计 |
| `'traceback'` | 回溯内存分配    |

## `__slots__` 

我们知道，对象的属性是保存在对象的属性字典中，并且每个对象都有一个独立的属性字典。当进程中存在很多对象时，就会占用大量的内存空间。

```python
import tracemalloc


class A:

    def __init__(self, a, b):
        self.a = a
        self.b = b


tracemalloc.start()

d = [A(0, 1) for _ in range(1000000)]

snapshot = tracemalloc.take_snapshot()

for line in snapshot.statistics("lineno"):
    print(line)
```

执行上述代码的输出结果为

```
./test.py:13: size=92.0 MiB, count=1000001, average=96 B
```

可以发现创建 $100$ 万个对象占用了 $92 \text{MB}$ 的内存。

> [!tip] 
> 
> 对象的属性比较少的，使用字典存储属性会浪费许多空间。因为实例属性字典占用的额外内存没有被使用
> 

Python 提供了一个特殊属性 `__slots__`，这个特殊属性会将对象的属性字典替换掉为占用内存较小的元组对象。

```python
import tracemalloc


class A:

    __slots__ = ("a", "b")  # 指定实例的属性名

    def __init__(self, a, b):
        self.a = a
        self.b = b


tracemalloc.start()

d = [A(0, 1) for _ in range(1000000)]

snapshot = tracemalloc.take_snapshot()

for line in snapshot.statistics("lineno"):
    print(line)
```

在替换实例的属性字典为元组之后，创建 $100$ 万个对象占用的内存降低为了 $53.8 \text{MB}$

```
./test.py:15: size=53.8 MiB, count=1000001, average=56 B
```

> [!attention] 
> 
> 请注意，`__slots__` 是类属性，它会替代该类的实例的属性字典。换句话说，该类的实例的 `__dict__` 属性就不会存在了
> 

```python
class A:

    __slots__ = ("a", "b")  # 指定实例的属性名

    def __init__(self, a, b):
        self.a = a
        self.b = b


a = A(1, 2)

# AttributeError：属性字典被替换了，此时该属性就不会存在
print(a.__dict__)
```

> [!attention] 
> 
> 由于实例的 `__dict__` 属性不存在了，那么设置实例属性时就必须设置实例已经存在的属性。这是因为实例内部采用元组保存属性值，它与 `__slots__` 中的属性名按位置一一对应。因为元组是不可变的，显然不能添加新属性
> 

```python
class A:

    __slots__ = ("a", "b")  # 指定实例的属性名

    def __init__(self, a, b):
        self.a = a
        self.b = b


a = A(1, 2)

a.a = 10
a.b = 20

a.c = 30  # AttributeError：实例 a 没有属性 c
```

请注意：`__slots__` 不会替换派生类对象的属性字典。也就是说：`__slots__` 属性只会替换它所在类的实例的属性字段

```python
class A:

    __slots__ = ("a", "b")  # 指定实例的属性名

    def __init__(self, a, b):
        self.a = a
        self.b = b


class B(A):
    pass

b = B(1, 2)
b.c = 200
print(b.__dict__)  # {'c': 200}：派生类拥有自己的属性字典
```

## 对象模型

**对象** 是 Python 中对数据的抽象。 Python 程序中的所有数据都是由对象或对象间关系来表示的。 从某种意义上说，按照冯·诺依曼的“存储程序计算机”模型^[[[计算机组成：计算机模型]]]，**代码本身也是由对象来表示的**。






