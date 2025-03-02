# 模拟容器

Python 中的标准容器 `str` `list`  `tuple` `set` `dict` 支持一些操作 `len()` `in` `container[index_or_key]`。让程序自定义这些行为是非常有必要的

下表列出来 Python 中提供的用于模拟容器行为的魔术方法

| 容器方法                            | 描述                                   |
| :------------------------------ | ------------------------------------ |
| `__len__(self)`                 | 返回 `len(self)`                       |
| `__length_hint__(self)`         | 类似 `len(self)`，只是这是一个估计值             |
| `__iter__(self)`                | 返回当前容器的迭代器，即 `iter(self)`            |
| `__reversed__(self)`            | 返回当前容器的逆序迭代器，即 `reversed(self)`      |
| `__cntains__(self, item)`       | 判断 `item` 是否在容器中，即成员检测运算符 `in`       |
| `__getitem__(self, key)`        | 实现 `self[key]` 的行为                   |
| `__setitem__(self, key, value)` | 实心 `self[key] = value` 的行为           |
| `__delitem__(self, key)`        | 实现 `del self[key]` 的行为               |
| `__missing__(self, key)`        | 当 `dict` 子类调用 `__getitem__()` 失败时的行为 |

> [!important] 
> 
> `__getitem__(self, key)` 方法、`__setitem__(self, key, value)` 方法和 `__delitem__(self, key)` 方法中 `key` 可以是 `slice`，也可以是一个整数索引。对于 `dict`，`key` 必须是 `hashable` 的
> 
> + 如果 `key` 不存在，则引发 `KeyError` 异常
> + 如果 `key` 是整数索引，则引发 `IndexError`
> 
> 
> `item in container` 检查 `itme` 是否在 `container` 中时，调用 `__contains__`，如果未定义 `__contains__`，就调用 `__iter__()` 方法获取一个迭代器，然后遍历进行查找
> 

下面我们以实现 `Vector` 类为例练习模拟容器的魔术方法

```python

class Vector:
    def __init__(self):
        """ 初始化向量对象 """
        self.__size = 0
        self.__data = []

    def __len__(self):
        return self.__size

    def __iter__(self):
        yield from self.__data

    def __contains__(self, item):
        for item in self.__data:
            if item == item:
                return True
        return False

    def __getitem__(self, key):
        return self.__data[key]

    def __setitem__(self, key, value):
        self.__data[key] = value

    def __delitem__(self, key):
        del self.__data[key]

    def append(self, item):
        self.__data.append(item)
        self.__size += 1

v = Vector()

v.append(1)
v.append(2)
v.append(3)
v.append(4)
v.append(5)


for iin range(len(v)):
    v[i] *= 2

for item in v:
    print(item)
```
 