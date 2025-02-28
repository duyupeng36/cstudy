# 魔术方法

Python 提供许多魔术方法，方便我们控制对象的行为。下面我们介绍与对象基本操作相关的魔术方法

## 对象的生命周期

在 [[Python：类和对象]] 中，我们介绍了一个与对象声明周期有关的魔术方法 `__init__()`。这是用于为对象添加初始化状态的方法；也就是说，当调用 `__init__()` 时，对象就已经存在了

### 对象的创建

在 Python 中，任何一个对象需要创建，首先都调用 `__new__()` 魔术方法。该方法的 **第一个参数代表类**，剩余参数是用于给对象提供初始状态的

```python

class Base:

    def __new__(cls, *args, **kwargs):
        print(cls)
        print(args)
        print(kwargs)


b = Base(1, 2, 3)
print(b)
```

执行上述代码输出的结果是

```
<class '__main__.Base'>  # Base 类对象
(1, 2, 3)                # args
{}                       # kwargs
None                     # b
```

下面我们让 `__new__()` 返回其他对象

```python

class Base:

    def __new__(cls, *args, **kwargs):
        print(cls)
        print(args)
        print(kwargs)
        return 20


b = Base(1, 2, 3)

print(b)
```

结果为

```
<class '__main__.Base'>  # Base 类对象
(1, 2, 3)                # args
{}                       # kwargs
20                       # b
```

> [!tip] 
> 
> `__new__()` 需要返回一个值，这个返回值必须是 `__new__()` 所在类的对象。当然，也可以返回其他对象，这样做可能会造成混乱
> 

下面是一种错误的做法

```python hl:6
class Singleton:
    def __new__(cls, *args, **kwargs):
        print(cls)
        print(args)
        print(kwargs)
        return cls(*args, **kwargs)   # 注意，cls() 会再次调用 __new__() 从而陷入无限递归：RecursionError: maximum recursion depth exceeded


a = Singleton(10, 20)

print(a)
```

> [!tip] 
> 
> 在定义魔术方法的时候，一定小心，避免陷入无限递归
> 

正确的做法是调用父类的 `__new__()` 方法，并返回.

```python

class Singleton:
    def __new__(cls, *args, **kwargs):
        print(cls)
        print(args)
        print(kwargs)
        # return cls(*args, **kwargs)   # 注意，cls() 会再次调用 __new__() 从而陷入无限递归：RecursionError: maximum recursion depth exceeded
        return super(Singleton, cls).__new__(cls, *args, **kwargs)

a = Singleton(10, 20)


print(a)
```

> [!attention] 
> 
> 注意，`__new__()` 是一个静态方法，它不会自动将类传递 `__new__()`。在调用父类的 `__new__()` 时，请注意，一定要将 `cls` 手动传递
> 
> 参数 `cls` 和 `self` 一样，只是约定这样命名，从而提高程序代码的可读性
> 

换句话说，对象的创建行为我们可以通过重定义 `__new__()` 方法进行拦截，从而控制对象的创建。这与 [[C++：特殊运算符#分配和释放堆内存]] 非常类似

> [!tip] 
> 
> 实现单例模式就可以通过拦截 `__new__()` 的方式，让对象只创建一次
> 

```python
class Singleton:
    __instance = None
    def __new__(cls, *args, **kwargs):
        if cls.__instance:
            return cls.__instance
        else:
            self = super(Singleton, cls).__new__(cls, *args, **kwargs)
            cls.__instance = self
            return self


a = Singleton()
b = Singleton()
c = Singleton()

print([a, b, c])
```

### 对象的初始化

对象的内存分配完成后，下一步就是为这个片内存提供初始值。在 Python 中，给对象提供初始值的方法就是 `__init__()` 方法

> [!tip] 
> 
> 方法 `__init__()` 会在 `__new__()` 返回后立即调用。注意，这是由 Python 帮我们调用的，我们无需手动调用 `__init__()` 方法
> 
> 在 [[Python：元类]] 中，我们会介绍对象是如何创建和初始化的
> 

下面的示例代码中定义了 `__init__()` 和 `__new__()` 方法

```python

class Person:
    def __init__(self, name, age):
        print("__init__")
        print(f"{name}, {age}")
        self.name = name
        self.age = age

    def __new__(cls, *args, **kwargs):
        print("__new__")
        print(args, "  ", kwargs)
        return super().__new__(cls)


p = Person("John", 36)
```

执行上述代码，输出的结果为

``` hl:2
/usr/bin/python3 /mnt/c/Users/duyup/Documents/pyfiles/test.py
__new__
('John', 36)    {}
__init__
John, 36
```

分析上述输出结果，我们知道 _创建对象时最先调用的方法是 `__new__()`_，对象需要的初始化参数也传递给了 `__new__()` 方法。_当 `__new__()` 返回后，立即调用 `__init__()` 方法用于给对象提供初始数据属性_

### 对象的销毁

同 C++ 一样，当对象不在被使用时，如果对象持有资源，那么这些资源就应该被释放。在 Python 对象被销毁时会自动调用 `__del__()`

> [!tip] 
> 
> 注意，Python 的所有对象的回收都是有 Python 自己的垃圾回收机制完成的。参考 [[Python：垃圾回收]]
> 

也就是说，我们通过 `del` 语句去删除一个对象时，`__del__()` 方法不一定会被触发。只有当最后一个引用对象的变量被删除时，对象才会真的被销毁，此时才会调用 `__del__()` 函数

```python

class Person:
    def __init__(self, name, age):
        print("__init__")
        print(f"{name}, {age}")
        self.name = name
        self.age = age

    def __new__(cls, *args, **kwargs):
        print("__new__")
        print(args, "  ", kwargs)
        return super().__new__(cls)

    def __del__(self):
        print("__del__")


p = Person("John", 36)

b = p

del p

while True:
    pass
```

上述代码执行完 `del p` 语句后进行一个死循环，此时我们并不能看见输出的 `"__del__"`。因为，此时 `b` 还在引用 `person` 对象，从而导致 `person` 对象不会被销毁

如果我们把 `while` 语句去掉，等程序结束时，Python 会清理所有的对象，此时 `person` 对象被销毁，从而触发 `__del__()` 方法的调用

```python
class Person:
    def __init__(self, name, age):
        print("__init__")
        print(f"{name}, {age}")
        self.name = name
        self.age = age

    def __new__(cls, *args, **kwargs):
        print("__new__")
        print(args, "  ", kwargs)
        return super().__new__(cls)

    def __del__(self):
        print("__del__")


p = Person("John", 36)
```

上述代码执行时输出的结果为

``` hl:5
__new__
('John', 36)    {}
__init__
John, 36
__del__ 
```

## 对象的内容展示

对于我们自定义类的实例对象，当我们使用 `print()` 函数去输出它时，得到的只会看见该对象的内存信息，而看不见对象中的属性值。

```python

class Person:
    def __init__(self, name, age):
        self.name = name
        self.age = age


p = Person("John", 36)

print(p)  # <__main__.Person object at 0x7f8cee5d2660>
```

有时候，我们想要查看对象中的内容，因此，为对象提供一个字符串形式是很有必要的。Python 中提供了 $3$ 个魔术方法用于提供对象的字符串形式

| 方法            | 描述                                |
| :------------ | :-------------------------------- |
| `__str__()`   | 返回对象的字符串形式                        |
| `__repr__()`  | 返回对象的字符串表达。这是一个符合 Python 规范的对象字面值 |
| `__bytes__()` | 返回一个 `bytes` 对象，用于表示              |

> [!tip] `__str__()` 方法的调用时机
> 
> 当我们使用 `print()` `str()` `format()` 等需要字符串的环境，Python 就会触发对象的 `__str__()` 方法，从而获得对象的字符串形式
> 
> 也就是说，**`__str__()` 的调用时机就是对象处于需要字符串的环境**
> 

> [!tip] `__repr__()` 方法的调用时机
> 
> `__repr__()` 方法是用于定制对象的字符串表达形式。通常使用 `__repr__()` 定制对象的 Python 字符串字面值。既然 `__repr__()` 也是定义 Python 对象的字符串形式，那么在需要字符串环境的情形下，`__repr__()` 也是会被自动调用的
> 

当一个类同时定义了 `__str__()` 和 `__repr__()`，那么那个方法被优先调用呢？

```python
class Person:

    def __init__(self, name, age=18):
        self.name = name
        self.age = age

    def __str__(self):
        print("__str__被调用")
        return f"Person(name={self.name}, age={self.age})"

    def __repr__(self):
        print("__repr__被调用")
        return f"<__main__.Person(name={self.name}, age={self.age})>"


if __name__ == "__main__":
    p1 = Person("Tom")
    print(1, p1)  # Person(name=Tom, age=18)

    p2 = Person("Jerry", 8)
    print(2, p2)  # Person(name=Jerry, age=8)

    p3 = str(p2)  # 也会触发 __str__ 方法
    print(3, p3)

    print(4, f"{p2}")          # 格式化，也会触发 __str__ 方法
    print(5, "{}".format(p2))  # 字符串的 format 方法也会除法 __str__ 方法
	
    print(6, [p1, p2])  # 触发 __repr__ 方法
    print(7, repr(p2))  # 触发 __repr__ 方法
```

> [!important] 
> 
> `__repr__()` 和 `__str__()` 同时存在时，需要字符串的环境优先调用 `__str__()`，对于需要对象的环境，则会优先调用 `__repr__()`。参考下面列出的两种情形
> + `print()` 和 `str()`  和 `format` 系列会优先调用 `__str__` 方法
> + 当对象作为容器的元素时，如果打印容器，那么在输出容器中的每个元素都会调用 `__repr__` 方法
> 

`__bytes__` 方法通常只有在使用 `bytes()` 进行强制类型转换时才会被使用

```python
class Person:

    def __init__(self, name, age=18):
        self.name: str = name
        self.age: int = age

    def __bytes__(self):
        return self.name.encode() + self.age.to_bytes(byteorder="little", signed=False)


if __name__ == "__main__":
    p1 = Person("Tom")
    print(bytes(p1))
```

## 可哈希对象

下面我们来探讨 Python 中的可哈希对象。在此之前，我们知道可哈希对象一定是不可变对象。然而，真实情况并不是这样的。Python 中的可哈希对象时只定义了 `__hash__()` 方法的对象，该方法返回一个整数，这个整数就是对象的哈希值

> [!tip] 
> 
> 简单来讲，**如果类定义了 `__hash__()` 方法**，那该类是 **可哈希的**，类实例就是可哈希对象；如果类没有定义 `__hash__()` 方法，那么该类就是不可哈希的
> 
> `__hash__()` 方法在需要哈希值的上下文中就会自动被调用。例如，`hash()` 函数，将对象 **作为集合的元素** 或者 **字典的键**
> 

```python hl:22
class Person:

    def __init__(self, name, age=18):
        self.name: str = name
        self.age: int = age

    def __repr__(self):
        return self.name

    def __hash__(self):
        return 100


if __name__ == "__main__":

    p1 = Person("Tom")
    p2 = Person("Tom")

    print([p1, p2])  # [Tom, Tom]
    print((p1, p2))  # (Tom, Tom)
    print({p1, p2})  # {Tom, Tom}
```

上述代码中第 $22$ 行输出的内容是 `{Tom, Tom}`，因为 `__hash__()` 返回的值只是代表了对象的哈希值，如果两个对象的哈希值相同，意味着出现 **哈希冲突**。请注意，**两个对象的哈希值相同，并不代表它们的值也相等**。集合中要求的是存储 _值不相等_ 的对象

本例的 `p1` 和 `p2` 的类中没有定义 `__eq__()` 方法，默认比较两个对象的内存地址。所以，仅仅只是哈希值相等的两个对象放入集合中是不会去重的，还要确保两个对象的值相等

## 逻辑值检测

在 Python 中，**任何对象都可以进行逻辑值检测**。如果需要控制逻辑值检测的行为可以定义 `__bool__()` 方法，该方法的返回值就是对象的逻辑值。对于自定义类的实例而言，默认情况下其逻辑值为 `True`

当一个 **容器对象** 在进行逻辑值检测的时，它会根据容器中是否有元素进行逻辑值检测。只要容器中有元素，其逻辑值为 `True`。获取容器对象的长度是通过 `__len__()` 方法，也就是说，Python 也可以根据 `__len__()` 方法的返回值进行逻辑值检测

> [!tip] Python 中逻辑值检测规则
> 
> + 如果类定义了 `__bool__()`，则该方法的返回值就是该类的实例的逻辑值
> + 否则，如果类定义了 `__len__()`，则会根据 `__len__()` 的返回值计算该类实例的逻辑值
> 	+ 如果 `__len__()` 返回非零，则该类实例的逻辑值为 `True`
> 	+ 否则，该类实例的逻辑值为 `False`
> + 如果 `__bool__()` 和 `__len__()` 都没有定义，则该类的逻辑值默认为 `True`
> 

```python
class Person:

    def __init__(self, name, age=18):
        self.name: str = name
        self.age: int = age

    def __repr__(self):
        return self.name

    def __len__(self):
        print("__len__")
        return 1


if __name__ == "__main__":
    p = Person("Tom")
    print(bool(p))

# Out:
# __len__
# True
```
