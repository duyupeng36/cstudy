# 元类

**在 Python 类也是一个对象**，称为类对象。Python 数据模型为每个类定义了很多属性。在之前的学习中，我们介绍对象的了 `__class__` `__name__` `__mro__` 等等属性。此外，下表列出其他一些常用的属性

| 类属性                    | 描述                         |
| :--------------------- | :------------------------- |
| `cls.__bases__`        | 基类元组                       |
| `cls.__qualname__`     | **限定名称**，即从模块的全局作用域到类的点分路径 |
| `cls.__subclasses__()` | 返回包含类的直接子类的列表，其实现使用弱引用     |

既然类是对象，那么类的类是什么呢？之前，我们说过函数 `type()` 就可以获得类的类型！

```python
class Person:
    def __init__(self, name, age):
        self.name = name
        self.age = age


print(type(Person))  # 查看类对象的类型：<class 'type'>
```

> [!tip] 
> 
> 类的类型就是 `type`。换句话说，类是类 `type` 的实例，我们称为 `type` 为元类
> 

## type：内置的类工厂函数

我们通常认为 `type` 是一个函数，会返回对象所属的类，即 `type(obj)` 返回 `object.__class__`

然而，`type` 是一个类，在调用时会传入 $3$ 个参数，创建一个新类。以下面这个简单的类为例。

```python hl:7
class Person:
    def __init__(self, name, age):
        self.name = name
        self.age = age


Student = type("Student", (Person,), {"show": lambda self: print(self.name, self.age)})

s = Student("John", 19)
s.show()
```

Python 读取 `class` 语句时会调用 `type` 构建类对象，传入的参数如下所示。

| 参数      | 描述                                        |
| :------ | ----------------------------------------- |
| `name`  | 这是一个**字符串**，`class` 关键字后的标识符，例如 `Student` |
| `bases` | 类标识符后面圆括号内提供的 **基类元组**                    |
| `dict`  | **属性名称到值的映射**。可调用对象变成方法，其他值变成类属性          |

> [!tip] 
> 
> type 构造函数还接受一些可选的关键字参数，`type` 自身忽略这些参数，但是会原封不动地传给 `__init_subclass__` 方法，该方法必须使用它们
> 

`type` 类是一个创建类的类，称为元类。也就是说，`type` 类的实例还是类。标准库还提供了一些其他元类，不过 `type` 是默认的元类

## 自定义元类

根据 Python 对象模型，**类是对象**，因此 **类肯定是另外某个类的实例**。默认情况下，**Python 中的类是 `type` 的实例**。也就是说，`type` 是大多数内置的类和用户定义的类的元类

```python
>>> str.__class__
<class 'type'>
>>> type.__class__
<class 'type'>
```

> [!tip] 
> 
> 为了避免无限回溯，`type` 的类是其自身
> 

注意，我没有说 `str` 是 `type` 的子类。我的意思是，`str` 是 `type` 的实例。`str` 类是 `object` 的子类。下图描述了这种奇怪的现象

![[Pasted image 20250302221647.png]]

在看一个例子

```Python
>>> from collections.abc import Iterable
>>> Iterable.__class__
<class 'abc.ABCMeta'>
>>> from abc import ABCMeta
>>> ABCMeta.__class__
<class 'type'>
```

`collections.Iterable` 所属的类是 `abc.ABCMeta`。注意，`Iterable` 是抽象类，而 `ABCMeta` 是具体类——然而，`Iterable` 是 `ABCMeta` 的实例。

向上追溯，`ABCMeta` 最终所属的类也是 `type`。所有类都直接或间接地是 `type` 的实例，不过只有元类同时也是 `type` 的子类。如果想理解元类，那么一定要知道这种关系：**元类从 `type` 类继承了构造类的能力**

![[Pasted image 20250302222324.png]]


> [!important] 
> 
> 我们要抓住的重点是，**元类是 type 的子类**，因此可以作为制造类的工厂
> 

下面我们将正式介绍如何自定义元类。再次之前，再次回顾一下 `__new__()` 方法。创建实例第一步就是调用 `__new__()` 方法。元类也是类，其实例还是类，因此通用的逻辑也适用于元类

```python
class Klass(BaseKlass, metaclass=MetaKlass): 
    x = 42 
    def __init__(self, y): 
        self.y = y
```

为了处理上述 `class` 语句，Python 调用元类 `MetaKlass` 的  `__new__(mcls, cls_name, bases, cls_dict)`(元类 `MetaKlass` 的 `__new__` 方法)，传入以下参数

| 参数         | 描述                     |
| :--------- | ---------------------- |
| `mcls`     | 元类自身，本例中即是 `MetaKlass` |
| `cls_name` | 新建类的类名，这是一个字符串         |
| `bases`    | 基类的元组                  |
| `cls_dict` | 新建类的属性                 |

在实现 `MetaKlass` 的 `__new__()` 方法时，可以对参数进行审查和修改，然后调用 `super().__new__(mcls, cls_name, bases, cls_dict)`，最终调用 `type._new__()` 创建新的类对象。

`super().__new__()` 返回之后，还可以进一步处理新创建的类，返回给 Python。随后，Python 调用 `BaseKlass.__init_subclass__`，传入新创建的类，如果有类装饰器的话，还会应用类装饰器。最后，Python 把类对象绑定给所在命名空间中的名称

> [!tip] 
> 
> `class` 语句是顶层语句时，所在的命名空间通常是模块全局命名空间
> 

> [!summary] `class` 语句创建类对象的步骤
> 
> + 调用元类的 `__new__()` 方法，需要提供四个参数 `mcls` `cls_name` `bases` `cls_dict`
> + 当 `__new__()` 返回后，Python 会调用基类的 `__init_subclass__()`，需要提供新创建的类作为参数
> + 如果类由装饰器，还会执行装饰器
> + 最后，Python 将对象绑定给 `class` 语句所在的名字空间中的名称
> 

> [!attention] 
> 
> 如果自定义了元类，需要 `__new__()` 方法中调用 `super().__new__()`。即调用 `type.__new__()` 方法。
> 
> 在 `super().__new__()` 返回后，还可以进一步的定制新创建的类
> 

在元类的 `__new__` 方法中，最常执行的操作是向 `cls_dict` 中添加项，或者替换其中的项。`cls_dict` 是一个映射，表示待构造的类的命名空间

> [!example] 
> 
> 例如，调用 `super().__new__` 之前，可以向 `cls_dict` 中添加函数，为待构造的类注入方法。
> 

> [!attention] 
> 
> 请注意，方法也可以在构建类之后添加，不然 `__init_subclass__` 和类装饰器就失去存在意义了
> 
> 有一个属性必须在运行 `type.__new__` 之前添加到 `cls_dict` 中，即 `__slots__`。关于 `__slots__` 我们稍后介绍
> 

> [!summary] 
> 
> 如何自定义一个元类？
> + 第一步：继承 `type` 类
> + 第二步：重写 `__new__()` 方法。这一步是可选的
> + 第三步：重写 `__init__()` 方法。这一步也是可选的，新建的类后可以通过它定制类属性
> + 第四步：重写 `__call__()` 方法，注意这是必须的，因为元类的实例必须是一个可调用对象。这是必须的
> 

```python hl:15-26

class MetaSingleton(type):

    def __new__(mcls, name, bases, attrs):
        """ 创建类对象
        :param name: 类对象的名字
        :param bases: 类对象的基类对象
        :param attrs: 类对象的属性字典
        """
        # 这里我们可以拦截类对象的创建过程
        # 1. 增加或减少类对象的属性
        # 2. 修改类名
        return super().__new__(mcls, name, bases, attrs)

    def __call__(cls, *args, **kwargs):
        """ 类对象实例化时调用元类中定义的 __call__ 方法
        :param args: cls 圆括号中的位置参数
        :param kwargs: cls 圆括号中的关键字参数
        :return: 返回 cls 的一个实例对象
        """
        if not hasattr(cls, "_instance"):
            # 调用 cls 中定义的  __new__() 方法创建一个实例
            cls._instance = object.__new__(cls)
        # 调用 cls 中定义的 __init__() 方法初始化实例
        cls._instance.__init__(*args, **kwargs)
        return cls._instance

class Singleton(metaclass=MetaSingleton):

    def __init__(self, x, y):
        self.x = x
        self.y = y

    def __new__(cls, *args, **kwargs):

        return super().__new__(cls, *args, **kwargs)


s1 = Singleton(1, 2)
s2 = Singleton(2, 3)

print(s1, s2)

print(s1.x, s1.y)
print(s2.x, s2.y)
```

> [!attention] 
> 
> 当 `__new__()` 返回后自动调用 `__init__()` 方法就是通过元类的 `__call__()` 方法控制的。
> 

如果不清楚类创建和初始化的过程，我们也可以直接使用 `type` 类的 `__call__()` 方法

```python

class MetaSingleton(type):

    def __new__(mcls, name, bases, attrs):
        """ 创建类对象
        :param name: 类对象的名字
        :param bases: 类对象的基类对象
        :param attrs: 类对象的属性字典
        """
        # 这里我们可以拦截类对象的创建过程
        # 1. 增加或减少类对象的属性
        # 2. 修改类名
        return super().__new__(mcls, name, bases, attrs)

    def __call__(cls, *args, **kwargs):
        """ 类对象实例化时调用元类中定义的 __call__ 方法
        :param args: cls 圆括号中的位置参数
        :param kwargs: cls 圆括号中的关键字参数
        :return: 返回 cls 的一个实例对象
        """
        print(args, kwargs)
        if not hasattr(cls, "_instance"):
            cls._instance = super().__call__(*args, **kwargs)  # 调用 type 的 __call__() 方法
        return cls._instance


class Singleton(metaclass=MetaSingleton):

    def __init__(self, x, y):
        self.x = x
        self.y = y


s1 = Singleton(1, 2)
s2 = Singleton(2, 3)

print(s1, s2)

print(s1.x, s1.y)
print(s2.x, s2.y)
```

## 补充 `__slots__` 属性

类还可以定义一个特殊的属性 `__slots__`。这个属性是用于替换类的名字空间的。

> [!question] 
> 
> 默认情况下，Python 中的类的名字空间采用字典实现的。也就是我们经常使用的 `__dict__` 属性
> 
> 由于 `__dict__` 属性时一个字典，字典默认会提前分配一片内存空间。当一个进程中存在很多的实例对象时，可能会对内存效率带来巨大的开销
> 

为了解决上述问题，Python 引入了 `__slots__` 属性通常被定义为元组，它限制了实例对象只能添加 `__slots__` 中指定的名字

```python
class Person:
    __slots__ = ("name", "age")  # 限制类的实例对象只能有两个属性
    def __init__(self, name, age):
        self.name = name
        self.age = age

    def __repr__(self):
        return f"<Person({self.name}, {self.age})>"


p = Person("John",  28)

p.gender = "male"  # AttributeError: 'Person' object has no attribute 'gender'
```

> [!attention] 
> 
> 请注意：使用 `__slots__` 属性后，类实例对象就不存在 `__dict__` 属性了
> 

```python hl:13-14
class Person:
    __slots__ = ("name", "age")
    def __init__(self, name, age):
        self.name = name
        self.age = age

    def __repr__(self):
        return f"<Person({self.name}, {self.age})>"


p = Person("John",  28)

print(p.__dict__)
# AttributeError: 'Person' object has no attribute '__dict__'
```

