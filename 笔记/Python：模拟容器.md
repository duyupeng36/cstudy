# 模拟容器

Python 提供了两种类型的容器，分别是 **序列** 和 **哈希表**。其中，序列包括 _列表_  _元组_ _字符串_ 和 _bytes_；哈希表包括 _集合_ 和 _字典_。当然，还有其他类型的序列。

如果我们自定义了容器类型，为了让容器的使用与 Python 内置容器类型保持一致，我们需要定义下表列特殊方法

| 特殊方法                                | 描述                                     |
| :---------------------------------- | :------------------------------------- |
| `container.__len__()`               | 返回 `container` 的长度，即 `len(container)`  |
| `container.___length_hint__()`      | 类似 `__len__()`，只是这是一个估计值               |
| `container.__getitem__(key)`        | 获取 `key` 对应的值，即 `container[key]`       |
| `container.__setitem__(key, value)` | 设置 `key` 对应的值 `container[key]=value`   |
| `container.__delitem__(key)`        | 删除 `key` 对应的值，即 `del container[key]`   |
| `container.__missing__(key)`        | 字典中找不到 `key` 的处理                       |
|                                     |                                        |
| `container.__iter__()`              | 返回 `container` 的迭代器  `iter(container)` |
| `container.__reversed__()`          | 返回返序遍历的迭代器`reversed(container)`        |
| `container.__contains__(item)`      | 检查 `container` 是否包含 `item`             |

上表中，前面 $6$ 个特殊方法被用来 **模拟序列** 或 **模拟映射**；在模拟序列和映射时使用的是同一套方法，因为我们可以认为序列和映射之间的差异就是 `key` 的差异

> [!tip] 
> 
> 序列允许的 `key` 必须是 `int`，并且 `0 <= key < N`，其中 `N` 是整个序列的长度，即 `len(sequence)`
> 
> 映射允许的 `key` 必须是可哈希对象
> 

为了让自定义容器的行为与 Python 内置类型 `dict` 类似，通常应该提供 `keys()` `values()` `items()` `get()` `setdefault()` `pop()` `popitem()` `clear()` `copy()` `update()` 等方法

> [!tip] 
> 
> 在标准库 `collections.abc` 中提供了一个抽象类 `MutableMapping`，它会根据由 `__getitem__()`, `__setitem__()`, `__delitem__()` 和 `keys()` 组成的基本集来创建这些方法
> 

让自定义容器的行为与可变序列类型(类似于 `list`)，则应当额外提供 `append()`, `count()`, `index()`, `extend()`, `insert()`, `pop()`, `remove()`, `reverse()` 和 `sort()` 等方法，就

> [!attention] 
> 
> 最后，序列类型还应当通过定义下文描述的 `__add__()`, `__radd__()`, `__iadd__()`, `__mul__()`, `__rmul__()` 和 `__imul__()` 等方法来实现加法（即拼接）和乘法（即重复）；它们不应定义其他数值运算符。
> 
> 这些数值运算符参考 [[Python：模拟数字]]
> 
 
  此外还建议映射和序列都实现 `__contains__()` 方法以允许高效地使用 `in` 运算符；对于映射，`in` 应当搜索映射的键；对于序列，则应当搜索其中的值。 
  
  另外还建议映射和序列都实现 `__iter__()` 方法以允许高效地迭代容器；对于映射，`__iter__()` 应当迭代对象的键；对于序列，则应当迭代其中的值。
  
## 模拟序列

在 Python 中，`list` 是线性表的顺序实现。下面我们实现一个链表，并让链表的使用与内置类型 `list` 相似

```python
import io
from abc import ABCMeta, abstractmethod

class ABCNode(metaclass=ABCMeta):

    @abstractmethod
    def __init__(self, value):
        self.__value = value
        self.__previous = None
        self.__next = None

    def __str__(self):
        return f"<Node object at {id(self):#x}>"

    def __repr__(self):
        return f"Node(value={self.__value}, previous={self.__previous} ,next={self.__next})"

    def __bool__(self):
        return bool(self.value)

    @property
    def value(self):
        return self.__value

    @value.setter
    def value(self, value):
        self.__value = value

    @property
    def previous(self):
        return self.__previous

    @previous.setter
    def previous(self, previous):
        self.__previous = previous

    @property
    def next(self):
        return self.__next

    @next.setter
    def next(self, next_):
        self.__next = next_


class Empty(ABCNode):
    def __init__(self, value):
        self.value = value
        self.previous = None
        self.next = None


empty = Empty(None)


class Node(ABCNode):
    def __init__(self, value, /):
        self.value = value
        self.previous: ABCNode = empty
        self.next: ABCNode = empty


class List:
    """实现链表"""

    def __init__(self):
        self.head: ABCNode = empty
        self.tail: ABCNode = empty
        self.size: int = 0

    def __repr__(self):
        return f"List(head={self.head}, tail={self.tail}, size={self.size})"

    def __str__(self):
        result = io.StringIO()
        result.write("[")

        current = self.head

        while current and current.next:
            result.write(str(current.value))
            result.write(", ")
            current = current.next

        result.write(str(current.value))
        result.write("]")
        return result.getvalue()

    def __len__(self) -> int:
        """实现 len(self)"""
        return self.size

    def __convert_and_check_index(self, index: int):
        """处理负索引并检查索引是否越界"""
        sign = 1 if index >= 0 else -1
        # 负索引 = 正索引 - 列表长度
        if sign < 0:  # 负索引
            index = self.size + index
        if index >= self.size or index < 0:
            raise IndexError(f"{index if sign > 0 else index - self.size} is out of range")
        return index

    def __getitem__(self, index):
        """实现 self[index] 和 self[start:end:step]"""

        if isinstance(index, int):
            index = self.__convert_and_check_index(index)
            current = self.head
            for _ in range(index):
                current = current.next
            return current.value
        elif isinstance(index, slice):
            raise NotImplementedError("Slice is not yet implemented.")
        else:
            raise TypeError('index must be int or slice')

    def __setitem__(self, index, value):
        """实现 self[index] = value"""
        if isinstance(index, int):
            index = self.__convert_and_check_index(index)
            current = self.head
            for _ in range(index):
                current = current.next
            current.value = value

        elif isinstance(index, slice):
            raise NotImplementedError("Slice assignment has not been implemented.")
        else:
            raise TypeError('index must be int or slice')

    def __delitem__(self, index):
        """实现 del self[index]"""
        if isinstance(index, int):
            index = self.__convert_and_check_index(index)

            current = self.head
            for _ in range(index - 1):
                current = current.next

            if current is self.head:  # 删除第一个节点
                self.head = current.next
                current.next.previous = empty
            else:
                current.next.next.previous = current
                current.next = current.next.next

            self.size -= 1
        elif isinstance(index, slice):
            raise NotImplementedError("Deleting slices has not been implemented.")
        else:
            raise TypeError('index must be int or slice')

    def __iter__(self):
        """实现 iter(self)"""

        self.__current = self.head
        return self

    def __next__(self):
        """实现 next(self)"""
        if not self.__current:
            raise StopIteration
        result = self.__current.value
        self.__current = self.__current.next
        return result

    def __reversed__(self):
        """实现 reversed(self)"""
        current = self.tail
        while current:
            yield current.value
            current = current.previous

    def append(self, value):
        """尾部添加"""
        node = Node(value)
        if not self.head and not self.tail:
            self.head = self.tail = node
        else:
            self.tail.next = node
            node.previous = self.tail
            self.tail = node
        self.size += 1

    def insert(self, index, value):
        if not isinstance(index, int):
            raise TypeError('index must be int')

        node = Node(value)

        if index <= 0:

            node.next = self.head
            self.head = node

            if self.head:  # 非空链表头部插入，注意
                self.head.next.previous = node

            if not self.tail:  # 空链表时，tail 也指向该节点
                self.tail = node

        elif index >= self.size:
            self.append(value)
            return
        else:
            current = self.head
            for _ in range(index - 1):
                current = current.next

            node.next = current.next
            current.next.previous = node
            current.next = node
            node.previous = current

        self.size += 1

    def clear(self):
        """清空列表：对象的释放交给垃圾回收"""
        self.head = self.tail = Empty
        self.size = 0
```

