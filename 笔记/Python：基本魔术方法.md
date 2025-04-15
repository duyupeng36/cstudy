# 基本魔术方法

Python 提供了许多特殊方法，这些特殊方法可以定制对象的行为，从而极大的方便我们使用对象

下表列出了这些魔术方法的分类，本节我们介绍前 $4$ 类魔术方法

| 类别      | 方法                                                          |
| :------ | :---------------------------------------------------------- |
| 实例化     | `__new__()` `__init__()` `__del__()`                        |
| 可视化     | `__str__()` `__repr__()` `__bytes__()`                      |
| 可哈希     | `__hash__()`                                                |
| 逻辑值     | `__bool__()` `__len__()`                                    |
| 运算符     | 算术运算符：`__add__()` 比较运算符：`__eq__()`                          |
| 容器      | `__len__()` `__getitem__()` `__setitem__()` `__delitem__()` |
| 可调用对象   | `__call__()`                                                |
| 上下文管理   | `__enter__()` `__exit__()`                                  |
| 自定义属性访问 | `__getattr__()` `__setattr__()` `__delattr__()`             |
| 描述器     | `__get__()` `__set__()` `__delete__()`                      |

## 实例化

在 Python 中，类对象实例化会涉及 $2$ 个特殊方法：`__new__()` 和 `__init__()`。当类对象实例化时，Python 首先调用类的 `__new__()` 方法，并在 `__new__()` 返回后调用 `__init__()` 方法。

下列的代码中，我们定义了一个 `Person` 类，它同时定义了 `__new__()` 和 `__init__()` 两个方法

```python
class Person:

	def __init__(self, name, age):
		self.__name = name
		self.__age = age

		print("__init__ called")


	def __new__(cls, *args, **kwargs):
		print("__new__ called")
		return 123


p = Person("John", 36)
print(p)
```

在上述代码中，我们在 `__new__()` 中返回了一个整数 `123`。执行上述代码时，输出的结果为

```
__new__ called
123
```

> [!tip] 
> 
> 显然，`__new__()` 方法被调用了，变量 `p` 引用的就是 `__new__()` 返回的对象。
> 
> 请注意：只有当 `__new__()` 返回的是当前类的实例时，`__init__()` 才会被 Python 解释器调用
> 

```python
class Person:

	def __init__(self, name, age):
		self.__name = name
		self.__age = age

		print("__init__ called")


	def __new__(cls, *args, **kwargs):
		print("__new__ called")
		print(cls)  # <class '__main__.Person'>
		return cls(*args, **kwargs)  # BUG: 会再次调用 __new__ 陷入无限递归


p = Person("John", 36)
print(p)
```

> [!bug] 
> 
> 上述代码存在严重的 bug。请注意：`__new__()` 的第一个参数 `cls` 本质上就是当前类对象。当类对象实例化时，Python 解释器会首先调用 `__new__()`。然而，我们在 `__new__()` 中，有使用了类对象实例化的语法，从而会导致 `__new__()` 方法被无限递归调用
> 
> 由于 Python 会限制递归深度，当递归超过限制时会抛出 `RecursionError`
> 

由于我们不知道 `__new__()` 的具体实现，因此，我们需要调用父类的 `__new__()` 方法

> [!attention] 
> 
> 请注意：Python3 中，任何类都隐式继承了 `object` 类
> 

因此，在这里，我们需要调用 `object` 类的 `__new__()` 方法

```python hl:12
class Person:

	def __init__(self, name, age):
		self.__name = name
		self.__age = age

		print("__init__ called")


	def __new__(cls, *args, **kwargs):
		print("__new__ called")
		return object.__new__(cls)


p = Person("John", 36)
print(p)
```

上述代码执行时的输出结果为

```
__new__ called
__init__ called
<__main__.Person object at 0x000001EECE8216A0>
```

由于 `object` 类是 `Person` 类的基类，因此我们可以使用 `super()` 方式调用

```python hl:12
class Person:

	def __init__(self, name, age):
		self.__name = name
		self.__age = age

		print("__init__ called")


	def __new__(cls, *args, **kwargs):
		print("__new__ called")
		return super().__new__(cls)


p = Person("John", 36)
print(p)
```

`__new__(cls, *args, **kwargs)` 方法还可以接受额外的参数，这些参数就是传递给 `__init__()` 的参数

```python
class Person:

	def __init__(self, name, age):
		self.__name = name
		self.__age = age

		print("__init__ called", f"{name}, {age}")


	def __new__(cls, *args, **kwargs):
		print("__new__ called", f"{args}, {kwargs}")
		return super().__new__(cls)


p = Person("John", 36)
print(p)
```

上述脚本执行时输出的结果为

```
__new__ called ('John', 36), {}  # 展示了 args 和 kwargs 的值
__init__ called John, 36
<__main__.Person object at 0x0000019C4C0216A0>
```

> [!summary] 
> 
> `__new__(cls, *args, **kwargs)` 是一个 `object` 的 **静态方法**。使用 `cls` 作为模板在内存空间中开辟一块存储空间用于存储对象的各种属性
> 
> + `cls` 代表着当前被实例化的类
> + `args` 位置传参数的初始化参数
> + `kwargs` 关键字传参的初始化参数
> 
> 请注意：不要在 `__new__(cls)` 中使用 `cls()` 创建实例，这样会陷入无限递归
> 

## 可视化

我们在打印内建类的对象时，Python 会输出一个符合自觉的内容。例如

```python
>>> numbers = [1,2,3,4]
>>> print(numbers)  # 输出的内容是符合列表的只管印象
[1, 2, 3, 4]
```

然而，我们输出自定义类型时，由于 Python 不知道我们自定义类型应该输出声明样式，此时 Python 就会输出该对象的类型和其所在的内存地址

```python
>>> class A:pass
...
>>> a = A()
>>> print(a)
<__main__.A object at 0x000002BDA8464C20>
```

为了让自定义类的对象与内建类型有相似的输出行为，Python 提供了 $3$ 个特殊方法用于控制对象的输出行为。我们首先要学习的两个特殊方法是 `__str__()` 和 `__repr__()`。

### `__str__()` 和 `__repr__()`

`__str__()` 方法用于定制对象的字符串表示，这个方法类似于 C++ 中的自定义类型转换。当在需要字符串的环境中，Python 就会尝试调用 `__str__()` 方法。也就是说，使用 `print()`、 `str()`、 `str.format()` 或者 `f-string` 中 Python 会优先调用 `__str__()` 方法

`__repr__()` 方法也是用于定制对象的字符串表示，但是不同的是这是符合 Python 风格的对象字符串。简单来说，`__repr__()` 返回的字符串表示的该类的实例。也就是说，在需要对象的环境中进行输出时，Python 会优先调用 `__repr__()` 

```python
class Person:

    def __init__(self, name, age):
        self.name = name
        self.age = age

    def __str__(self):
        return "My name is {} and my age is {}".format(self.name, self.age)

    def __repr__(self):
        return f"Person(name='{self.name}', age={self.age})"


p = Person("John", 36)

print(p)  # 调用 __str__()

print("%s" % p)  # 调用 __str__()

print(f"{p}")  # 调用 __str__()

print([p])  # 调用 __repr__()
```

> [!important] 
> `__repr__` 和 `__str__` 同时存在时
> 
> + 需要字符串的上下文：`print()` 和 `str()`  和 `str.format()` 会优先调用 `__str__` 方法
> + 需要实例的上下文：对象作为容器的元素时，如果打印容器，那么在输出容器中的每个元素都会调用 `__repr__` 方法
> 

### `__bytes__()`

对象在内存中可以认为是一个字节序列，在 Python 中可以通过 `bytes()` 返回对象的字节序列。在 Python 中，`bytes()` 会调用对象的 `__bytes__()` 方法

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

## 可哈希

当一个类定义了 `__hash__()` 方法，那么这个类的实例是可哈希的。当需要对象的哈希值时，Python 就会调用 `__hash__()` 方法获取对象的哈希值

```python
class Person:

    def __init__(self, name, age):
        self.name = name
        self.age = age

    def __repr__(self):
        return f"Person(name='{self.name}', age={self.age})"

    def __bytes__(self):
        return bytes(self.name, 'utf-8') + self.age.to_bytes(1)

    def __hash__(self):
        print("__hash__ called")
        return hash(self.__bytes__())


p1 = Person("John", 36)

p2 = Person("John", 36)

print(hash(p1), hash(p2))  
# 1384214028416065044 1384214028416065044
print({p1, p2})  
# {Person(name='John', age=36), Person(name='John', age=36)}
```

`__hash__()` 的返回值决定对象的哈希值。但是，**两个哈希值相同的对象不一定有相同的值**。本例的 `p1` 和 `p2` 的类中没有定义 `__eq__()` 方法，默认比较两个对象的内存地址，显然它们的内容地址是不相同的。所以，**仅仅只是哈希值相等的两个对象放入集合中是不会去重的**，还要确保两个对象的值相等

> [!tip] 
> 
> 关于 `__eq__()` 方法，参考 [[Python：模拟数字]]
> 

下面我们为 `Person` 类定义 `__eq__()` 方法

```python
class Person:

    def __init__(self, name, age):
        self.name = name
        self.age = age

    def __repr__(self):
        return f"Person(name='{self.name}', age={self.age})"

    def __bytes__(self):
        return bytes(self.name, 'utf-8') + self.age.to_bytes(1)

    def __hash__(self):
        print("__hash__ called")
        return hash(self.__bytes__())

    def __eq__(self, other):
        return self.name == other.name and self.age == other.age


p1 = Person("John", 36)

p2 = Person("John", 36)

print(hash(p1), hash(p2)) 
# 1384214028416065044 1384214028416065044
print({p1, p2})
# {Person(name='John', age=36)}
```

此时，集合中的元素就只有一个了

## 逻辑值

在 [[Python：基本概念#逻辑值检测]] 中介绍了 Python 内置类型与逻辑的等价关系。然而，对于自定义类型的实例，其逻辑值始终为 `True`。

```python
class Person:

    def __init__(self, name, age):
        self.name = name
        self.age = age

    def __repr__(self):
        return f"Person(name='{self.name}', age={self.age})"


p1 = Person("John", 36)

print(bool(p1))  # True
```

Python 中提供的特殊方法`__bool__()` 可以控制对象的逻辑值。

```python
class Person:

    def __init__(self, name, age):
        self.name = name
        self.age = age

    def __repr__(self):
        return f"Person(name='{self.name}', age={self.age})"

    def __bool__(self):
        print("__bool__ called")
        return bool(self.name and self.age)


p1 = Person("John", 36)

p2 = Person("", 0)

print(bool(p1))  # p1.__bool__() 返回 True
print(bool(p2))  # p2.__bool__() 返回 False

if p1:  # 调用 p1.__bool__() 
    print(p2)

if not p2:  # 调用 p2.__bool__() 
    print(p1)
```

> [!tip] 
> 
> `__bool__()` 方法会在需要逻辑值的语义中被调用
> 

