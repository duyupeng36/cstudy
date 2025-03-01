# 运算符重载

Python 通过对象的魔术方法支持定义运算符的行为。

## 比较运算符

Python 的比较为对象提供比较运算的逻辑，只需要在对象所属类中定义下表列出的方法即可

| 比较运算符                 | 描述                        |
| :-------------------- | ------------------------- |
| `__eq__(self, other)` | 判断 `self` 与 `other` 是否相等  |
| `__ne__(self, other)` | 判断 `self` 与 `other` 是否不相等 |
|                       |                           |
| `__lt__(self, other)` | 判断 `slef` 是否小于 `other`    |
| `__gt__(self, other)` | 判断 `self` 是否大于 `other`    |
|                       |                           |
| `__le__(self, other)` | 判断 `self` 是否小于或等于 `other` |
| `__ge__(self, other)` | 判断 `self` 是否大于或等于 `other` |

请注意，比较运算符被分为了 $3$ 个组。在实现时无需定义上表中的 $6$ 个方法，而只需要定义每个组中的一个即可获得对象的全序

> [!tip] `__eq__()` 和 `__ne__()`
> 
> 两个对象在进行相等性判断时，如果 `__eq__()` 方法（`==`）返回 `False`，那么就一定意味着 `__ne__()` 方法 （`!=`） 返回 `True`
> 

> [!tip] `__lt__()` 和 `__gt__()`
> 
> 两个对象在进行大小性判断时，如果 `__lt__()` 方法（`<`） 返回 `False`，那么就一定意味着 `__gt__()` 方法 (`>`) 返回 `True`
> 

> [!tip] `__le__()` 和 `__ge__()`
> 
> 两个对象在进行大小性判断时，如果 `__le__()` 方法（`<=`） 返回 `False`，那么就一定意味着 `__ge__()` 方法 (`>=`) 返回 `True`
> 

换句话说，只需定义 $3$ 个比较运算符的方法就可以完成全序比较

```python
class Person:
    def __init__(self, name, age):
        self.name = name
        self.age = age

    def __lt__(self, other):           # 隐式推断 __gt__()
        print("__lt__")
        return self.age < other.age

    def __eq__(self, other):           # 隐式推断 __ne__()
        print("__eq__")
        return self.age == other.age

    def __le__(self, other):           # 隐式推断 __ge__()
        print("__le__")
        return self.age <= other.age



p1 = Person("张三", 18)
p2 = Person("李四", 20)

print(p1 > p2)   # 调用 __lt__
print(p1 >= p2)  # 调用 __le__
print(p1 != p2)  # 调用 __eq__
```

上述代码的输出结果为

```
__lt__
False    # print(p1 > p2)
--------------------
__le__
False    # print(p1 >= p2)
--------------------
__eq__
True     # print(p1 != p2)
```

> [!attention] 
> 
> 在 Python 中，`==` 的逻辑并不是直接比较两个对象的值，而是会 **首先比较两个对象是不是同一个对象**
> + 如果是同一个对象，那么直接返回 `True`
> + 如果不是同一个对象，才会比较对象的值
> 

下面代码展示了对象的相等性比较逻辑

```python hl:12
class Person:
    def __init__(self, name, age):
        self.name = name
        self.age = age

    def __lt__(self, other):           # 隐式推断 __ge__()
        print("__lt__")
        return self.age < other.age

    def __eq__(self, other):           # 隐式推断 __ne__()
        print("__eq__")
        return  self is other or self.age == other.age

    def __le__(self, other):           # 隐式推断 __gt__()
        print("__le__")
        return self.age <= other.age



p1 = Person("张三", 18)
p2 = Person("李四", 20)

print(p1 > p2)   # 调用 __lt__
print(p1 >= p2)  # 调用 __le__
print(p1 != p2)  # 调用 __eq__
```

Python 在 `functools` 模块中提供了一个装饰 `total_ordering`，该装饰器根据 `__lt__()` `__le__()` `__gt__()` 和 `__ge__()` 方法中的一个生成其他比较方法

```python
def _gt_from_lt(self, other):
    'Return a > b.  Computed by @total_ordering from (not a < b) and (a != b).'
    op_result = type(self).__lt__(self, other)
    if op_result is NotImplemented:
        return op_result
    return not op_result and self != other

def _le_from_lt(self, other):
    'Return a <= b.  Computed by @total_ordering from (a < b) or (a == b).'
    op_result = type(self).__lt__(self, other)
    if op_result is NotImplemented:
        return op_result
    return op_result or self == other

def _ge_from_lt(self, other):
    'Return a >= b.  Computed by @total_ordering from (not a < b).'
    op_result = type(self).__lt__(self, other)
    if op_result is NotImplemented:
        return op_result
    return not op_result

def _ge_from_le(self, other):
    'Return a >= b.  Computed by @total_ordering from (not a <= b) or (a == b).'
    op_result = type(self).__le__(self, other)
    if op_result is NotImplemented:
        return op_result
    return not op_result or self == other

def _lt_from_le(self, other):
    'Return a < b.  Computed by @total_ordering from (a <= b) and (a != b).'
    op_result = type(self).__le__(self, other)
    if op_result is NotImplemented:
        return op_result
    return op_result and self != other

def _gt_from_le(self, other):
    'Return a > b.  Computed by @total_ordering from (not a <= b).'
    op_result = type(self).__le__(self, other)
    if op_result is NotImplemented:
        return op_result
    return not op_result

def _lt_from_gt(self, other):
    'Return a < b.  Computed by @total_ordering from (not a > b) and (a != b).'
    op_result = type(self).__gt__(self, other)
    if op_result is NotImplemented:
        return op_result
    return not op_result and self != other

def _ge_from_gt(self, other):
    'Return a >= b.  Computed by @total_ordering from (a > b) or (a == b).'
    op_result = type(self).__gt__(self, other)
    if op_result is NotImplemented:
        return op_result
    return op_result or self == other

def _le_from_gt(self, other):
    'Return a <= b.  Computed by @total_ordering from (not a > b).'
    op_result = type(self).__gt__(self, other)
    if op_result is NotImplemented:
        return op_result
    return not op_result

def _le_from_ge(self, other):
    'Return a <= b.  Computed by @total_ordering from (not a >= b) or (a == b).'
    op_result = type(self).__ge__(self, other)
    if op_result is NotImplemented:
        return op_result
    return not op_result or self == other

def _gt_from_ge(self, other):
    'Return a > b.  Computed by @total_ordering from (a >= b) and (a != b).'
    op_result = type(self).__ge__(self, other)
    if op_result is NotImplemented:
        return op_result
    return op_result and self != other

def _lt_from_ge(self, other):
    'Return a < b.  Computed by @total_ordering from (not a >= b).'
    op_result = type(self).__ge__(self, other)
    if op_result is NotImplemented:
        return op_result
    return not op_result

_convert = {
    '__lt__': [('__gt__', _gt_from_lt),
               ('__le__', _le_from_lt),
               ('__ge__', _ge_from_lt)],
    '__le__': [('__ge__', _ge_from_le),
               ('__lt__', _lt_from_le),
               ('__gt__', _gt_from_le)],
    '__gt__': [('__lt__', _lt_from_gt),
               ('__ge__', _ge_from_gt),
               ('__le__', _le_from_gt)],
    '__ge__': [('__le__', _le_from_ge),
               ('__gt__', _gt_from_ge),
               ('__lt__', _lt_from_ge)]
}

def total_ordering(cls):
    """Class decorator that fills in missing ordering methods"""
    # Find user-defined comparisons (not those inherited from object).
    roots = {op for op in _convert if getattr(cls, op, None) is not getattr(object, op, None)}
    if not roots:
        raise ValueError('must define at least one ordering operation: < > <= >=')
    root = max(roots)       # prefer __lt__ to __le__ to __gt__ to __ge__
    for opname, opfunc in _convert[root]:
        if opname not in roots:
            opfunc.__name__ = opname
            setattr(cls, opname, opfunc)
    return cls
```

上面代码给出了 `total_ordering` 装饰器的实现逻辑。也就是说，只要定义`__lt__()` `__le__()` `__gt__()` 和 `__ge__()` 方法中的任意一个方法，那么 `total_ordering` 就能生成完整的全序比较逻辑

> [!tip] 
> 
> Python 默认提供了 `__eq__()` 的实现，只是简单的比较两个对象的地址，只要两个对象的地址不相等，Python 就认为这个两个对象不相等
> 
> 仅仅比较地址是不合理的，我们应该覆盖 `__eq__()` 的默认行为
> 

根据上述分析，我们只需要实现 `__eq__()` 以及 `__lt__()` `__le__()` `__gt__()` 和 `__ge__()` 中的一个方法就能通过 `total_ordering` 装饰器生成完整的全序比较逻辑

```python
import functools


@functools.total_ordering
class Person:

    def __init__(self, name, age=18):
        self.name: str = name
        self.age: int = age

    def __repr__(self):
        return f"Person(name={self.name}, age={self.age})"

    def __sub__(self, other):  # 创建一个新对象
        print("__sub__")
        return Person(self.name, self.age - other.age)

    def __isub__(self, other):  # 就地修改，不会创建一个新对象
        print("__isub__")
        self.age -= other.age
        return self

    def __eq__(self, other):
        return self.age == other.age

    def __lt__(self, other):
        return self.age < other.age


if __name__ == '__main__':

    tom = Person("Tom")
    jerry = Person("Jerry", age=10)

    print(tom >= jerry)
```

## 算术运算符

对于算术运算符，Python 同样提供了魔术方法让程序员可以自定义对象的算术运算符行为

| 算术运算符                            | 描述                                                 |
| :------------------------------- | :------------------------------------------------- |
| `__add__(self, other)`           | 返回 `self + other`                                  |
| `__sub__(self, other)`           | 返回 `self - other`                                  |
| `__mul__(self, other)`           | 返回 `self * other`                                  |
| `__matmul__(self, other)`        | 返回 `self @ other`，这是矩阵乘法                           |
| `__truediv__(self, other)`       | 返回 `self / other`                                  |
| `__floordir__(self, other)`      | 返回 `self // other`                                 |
| `__mod__(self, other)`           | 返回 `self % other`                                  |
| `__divmod__(self, other)`        | 返回 `divmod(self, other)`                           |
| `__pow__(self, other[, modulo])` | 返回 `pow(self, other[, modulo])`<br>`self ** other` |

## 位运算符

对于位操作符，Python 也支持我们重定义

| 位运算符                      | 描述                 |
| :------------------------ | ------------------ |
| `__lshift__(self, other)` | 返回 `self << other` |
| `__rshift__(self, other)` | 返回 `self >> other` |
| `__and__(self, other)`    | 返回 `self & other`  |
| `__xor__(self, other)`    | 返回 `self ^ other`  |
| `__or__(self, other)`     | 返回 `self \| other` |
| `__invert__(self)`        | 返回 `~self`         |

> [!attention] 
> 
> 对于逻辑运算符 `and, or` 和 `not` Python 不支持修改它们的行为
> 

## 增强赋值运算符

对于基本的赋值运算符，Python 不支持修改其行为。但是，增强赋值运算符的行为是支持重定义的

|               特殊方法                | 描述    | 备注                          |
| :-------------------------------: | :---- | :-------------------------- |
|     `__iadd__(_self, other)`      | `+=`  | `x +=y` 等价于 `x.__iadd__(y)` |
|     `__isub__(_self, other)`      | `-=`  |                             |
|      `__imul__(self, other)`      | `*=`  |                             |
|    `__imatmul__(self, other)`     | `@=`  | 矩阵乘法                        |
|    `__itruediv__(self, other)`    | `/=`  |                             |
|   `__ifloordiv__(self, other)`    | `//=` |                             |
|      `__imod__(self, other)`      | `%=`  |                             |
| `__ipow__(self, other[, modulo])` | `**=` |                             |
|    `__ilshift__(self, other)`     | `<<=` | 左移                          |
|    `__irshift__(self, other)`     | `>>=` | 右移                          |
|      `__iand__(self, other)`      | `&=`  | 按位与                         |
|      `__ixor__(self, other)`      | `^=`  | 按位异或                        |
|      `__ior__(self, other)`       | \|=   | 按位或                         |

> [!attention] 
> 
> 请注意：对于这些增强运算符，如果未定义其对应的方法，就会使用其对应的运算符函数。例如，如果 `__isub__(self, other)` 未定义，则 `self -= other` 就会去调用 `__sub__(self, other)`

## 反向二元运算符

对于某些二元运算符，例如 `__add__(self, other)`。当执行 `a + b` 时，如果 `a` 没有定义 `__add__(self, other)`，此时 Python 会转而调用 `b` 的 `__radd__(self, other)` 方法进行尝试

```python

class Person(object):
    def __init__(self, name, age):
        self.name = name
        self.age = age


    def __radd__(self, other):
        return Person(self.name, self.age + other)

    def __str__(self):
        return f'Person({self.name}, {self.age})'



p = Person("John", 18)

print(p)  # Person(John, 18)

p = 2 + p  # 2.__add__(p)：整数显然不支持，此时 Python 就会转而调用 p.__radd__(2) 进行尝试

print(p)  # Person(John, 20)
```

## 一元运算符

Python 中支持重载的一元运算符只有 `+` `-` `~` 和 `abs()` 

|       一元运算符        | 描述             |
| :----------------: | :------------- |
|  `__neg__(self)`   | 返回 `-self`     |
|  `__pos__(self)`   | 返回 `+self`     |
|  `__abs__(self)`   | 返回 `abs(self)` |
| `__invert__(self)` | 返回 `~self`     |
