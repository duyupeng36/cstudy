# 类和对象

类提供了把数据和操作数据的函数绑定在一起的方法。创建新类时创建了新的对象 **类型**，从而能够创建该类型的新 **实例**。实例具有能维持自身状态的 **属性**，还具有能修改自身状态的 **方法**（由其所属的类来定义）。

> [!tip] 类型表达了一种抽象的概念
> 
> 例如 `float` 类型和运算符 `+, -, *, /` 一起提供了数学概念 **实数** 的近似表示
> 

如果用 C++ 术语描述 Python 中类，类成员(包括数据成员和成员函数)通常是 `public` ^[[[C++：类与对象]]]，所有的成员函数都是 `virtual` ^[[[C++：多态]]]

> [!attention] 
> 
> 请注意：Python 也可以定义私有变量
> 

与 C++ 不同的是，在 Python 中没有用于对象的方法引用本对象成员的简写形式：**方法在声明时，显示使用第一个参数代表调用该方法的对象**。在 C++ 中，不用在成员函数中显示声明一个参数，用于代表调用该成员函数的对象

> [!important] 
> 
> 在 Python 中，**类本身也是一个对象**，为导入和重命名提供了语义支持
> 
> 与 C++ 不同的是，Python 的内置类型也可以作为基类，供用户扩展。
> 
> Python 和 C++ 一样，具有特殊语法的内置运算符（算术运算符、下标等）都可以为类实例重新定义
> 

Python 与 C++ 中的术语对应关系

| Python 术语 | C++ 术语 |
| :-------- | ------ |
| 属性        | 数据成员   |
| 方法        | 成员函数   |

## 名字和对象

在 Python 中，对象之间相互独立，每个对象至少有 $1$ 名字；也就是说，**多个名字(甚至是不同作用域内的名字) 可以绑定到同一个对象**

> [!tip] 
> 
> 在其他语言中，一个对象拥有的其他名称成为别名。例如，在 C++ 中，引用 ^[[[C++：与 C 的差异#引用]]] 就是对象的别名
> 
> 与 C++ 不同的是，**Python 中的名字可以重新绑定给任意的其他对象**。C++ 中的引用一旦绑定就不能重新绑定
> 

> [!attention] 
> 
> 在 Python 中，使用别名可能会带来意想不到的结果。幸运的是，在处理数字、字符串、元组等 **不可变基本类型** 时，可以不必理会。然而，当涉及 **可变对象**（如列表、字典，以及大多数其他类型）的 Python 代码的语义，别名可能会产生意料之外的效果
> 

**别名表现出来的特性就像 C++ 中的引用**，本质上就是一个指针。传递对象时的代价会非常小，因为实现只传递了一个指针。如果函数修改了作为参数传递的对象，调用者就可以看到更改

> [!important] 
> 
> Python 中将对象分为可变对象和不可变对象。对于不可变对象，一旦涉及到修改，Python 都会新创建一个对象。由于这个机制的存在，Python 中参数传递都是传递的对象的引用
> 
> 不用像 C++ 那样用两种参数传递机制
> 

## 作用域与名字空间

在学习类之前，在重复一次 Python 的作用域。类定义对命名空间有一些巧妙的技巧，了解作用域和命名空间的工作机制有利于加强对类的理解。

在 Python 中，**名字空间是名字到对象的映射**。Python 中的名字空间使用字典实现，除非涉及性能优化，否则我们不关注这方面的问题。

> [!example] 名字空间例子
> 
> + 内置名字空间：包含了 Python 内建的名字，例如 `abs()` 函数以及内置类型等
> + 模块的全局名字
> + 函数调用中的局部名字
> + 对象的属性集合
> 

不同的名字空间中的名字是没有任何关系。例如，两个不同的模块都可以定义 `maximize()` 函数，且不会造成混淆。用户使用函数时必须要在函数名前面加上模块名。

> [!tip] 
> 
> 点号之后的名称是 **属性**。例如，表达式 `z.real` 中，`real` 是对象 `z` 的属性。严格来说，对模块中名称的引用是属性引用：表达式 `modname.funcname` 中，`modname` 是模块对象，`funcname` 是模块的属性。**模块属性和模块中定义的全局名称之间存在直接的映射：它们共享相同的命名空间！**
> 

属性可以是只读的或者可写的。**可写属性可以对其进行赋值**。模块属性是可写的：你可以写入， 也可以使用 `del` 语句删除可写属性

```python
modname.the_answer = 42 # 将名为 modname 对象中的属性 the_answer 赋值为 42
del modname.the_answer # 将从名为 modname 对象中移除属性 the_answer
```

> [!important] 名字空间生命周期：命名空间是在不同时刻创建的，且拥有不同的生命周期
> 
> 内置名字空间：在 Python 解释器启动的时候创建，直到 Python 解释器退出
> 
> 模块的全局名字空间：在读取模块定义时创建，直到 Python 解释器退出
> 
> 函数的局部名字空间：在函数调用时创建，并在函数返回或抛出异常时删除
> 

名字空间的作用域是 Python 代码中一段文本区域，这个区域 **可以直接访问** 该名字空间中的所以名字。

> [!tip] 
> 
> 所谓的可直接访问的意思是：该文本区域内的名字在被 **非限定引用** 时，查找名字的范围，是包括该名字空间在内的
> 

在 Python 中，名字空间被静态确定，动态使用。Python 程序执行期间，任何时刻都会有 $3$ 到 $4$ 个名字空间可直接访问的嵌套作用域。参考 [[Python：函数基础#作用域规则]]

## 类定义

在 Python 定义类使用关键字 `class`，最简单的类定义形式如下

```python
class Person:
	pass
```

> [!important] 
> 
> 请注意：类定义必须先执行才能生效。就像定义函数哪样
> 
> 如果把类定义语句放在 `if` 内部或者函数内部，只要没执行就不会生成类
> 

在实践中，**类定义内的语句通常都是函数定义**，也可以是其他语句(后面会详细解释)。类里的函数定义一般是特殊的参数列表，这是由方法调用约定规范所指明的

**当进入类定义时，会立即创建一个新的名字空间，并将其用作局部作用域**。因此，所有对局部变量的赋值都是在这个新的名字空间之内。特别的，**函数定义会绑定到这里的新函数名称**

当 (从结尾处) 正常离开类定义时，将创建一个 **类对象**。 这基本上是一个围绕类定义所创建的命名空间的包装器。 原始的 (在进入类定义之前有效的) 作用域将重新生效，类对象将在这里与类定义头所给出的类名称进行绑定 (在这个示例中为 `Person`)

> [!tip] 
> 
> 在 Python 中，类也是一个对象，我们简称为 **类对象**
> 

### 类对象

在 Python 中，类对象支持两种操作：**属性引用** 和 **实例化**

> [!tip] 类对象的 **属性引用** 使用 Python 中所有属性引用的标准语法：**对象.属性名**
> 
> 有效的属性名称是类对象被创建时存在于类命名空间中的所有名称
> 

例如，如果类定义如下

```python
class MyClass:
	""""一个简单的示例类"""
	i = 12345

	def f(self):
		return "hello, world!\n"
```

#### 属性引用

属性引用 `MyClass.i` 和 `MyClass.f` 都是有效的属性引用，它们分别返回一个 `int` 类型的对象和一个函数对象

> [!attention] 
> 
> 类属性可以被赋值。因此，`MyClass.i = 54321` 就可以改变 `MyClass.i` 的值
> 

类对象还有一些特殊的属性。例如，`__doc__` 就是一个有效的属性，它是所属类的文档字符串：本例中，返会的就是 `"一个简单的示例类"`。此外，还有一个特殊属性 `__dict__`，这代表的是所属类对象的名字空间，它是一个 `mappingproxy` 对象，代理访问类对象的所有属性

> [!tip] 
> 
> 不能通过 `类.__dict__[属性名] = value` 的方式修改类属性的值。也就是说，`类.__dict__` 是只读的
> 

```python
MyClass.__dict__['i'] = 20
# TypeError: 'mappingproxy' object does not support item assignment

MyClass.i = 20  # OK
```

#### 实例化

**Python 中类的实例化使用的是函数调用语法**。可以把类对象视为是返回该类的一个新实例的函数

```python
x = MyClass()  # 创建一个实例对象，并绑定到局部名称 x
```

> [!attention] 
> 
> 请注意，**实例化创建的对象是一个空对象**，即没有包含任何数据成员。
> 
> 如果我们希望为创建的对象实例提供一些初始化状态，就需要在类中定义一个 **构造函数** 
> 

### 构造函数

在 Python 中，构造函数是一个有特殊约定命名的方法。所有类的构造函数都是名为 `__init__()` 的特殊方法

```python
class Person:
    def __init__(self, name, age):
        self.name = name
        self.age = age
```

当一个类定义了 `__init__()` 方法时，类的实例化会自动为新创建的类实例调用 `__init__()`。当 `__init__()` 要求额外的参数时，在实例化时就应该传递对应的参数

```python
person = Person("张三", 48)
```

### 实例对象

现在我们能用实例对象做什么？ **实例对象所能理解的唯一操作是属性引用**。 有两种有效的属性名称：**数据属性** 和 **方法**

在 Python 数据属性对应于 C++ 中的数据成员。与 C++ 不同的是，**数据属性不需要声明**；就像局部变量一样，它们将 **在首次被赋值时产生**。也就是说，我们可以给实例对象添加任意的数据属性。例如，如果 `x` 是之前创建的 `MyClass` 类的实例对象，则下面代码将打印数值 $16$，且不保留任何追踪信息

```python
x = MyClass()

x.counter = 1  # 添加数据属性 counter
while x.counter < 10:
	x.counter *= 2
print(x.counter)
del x.counter  # 删除对象的数据属性
```

> [!tip] 
> 
> 在 C++ 中，对象的数据成员是确定的，我们不能添加也不能删除对象的数据成员
> 

实例对象的另一种属性引用称为 **方法**，**它是从属于对象的函数**。实例对象的有效方法依赖于其所属的类。

> [!important] 
> 
> 一个类中所有是函数对象的属性都是定义了其实例的相应方法
> 

在我们的示例中，`x.f` 是有效的方法引用，因为 `MyClass.f` 是一个函数；而 `x.i` 不是方法，因为 `MyClass.i` 不是函数

> [!attention] 
> 
> 请注意：`x.f` 与 `MyClass.f` 不是同一回事。`x.f` 返回的是方法对象，不是函数对象
> 
> 也就是说，实例对象 `x` 执行属性引用时，如果发现属性是所属类的函数，则会将该对象与函数绑定生成一个方法对象 
> 

### 方法对象

通常，方法在绑定对象后立即被调用

```python
x.f()  # 方法绑定对象后立即被调用
```

在 `MyClass` 示例中，这将返回字符串 `'hello world'`。 但是，**方法并不是必须立即调用**: `x.f` 是一个方法对象，**它可以被保存起来以后再调用**。 例如:

```python
xf = x.f
while True:
	print(xf())
```

> [!question] 当一个方法被调用时发生了什么？
> 
> 请注意，在定义函数 `f()` 是指定了一个形式参数 `self`，但是调用时并没有未其提供参数
> 
> 当一个需要参数的函数在不附带任何参数的情况下被调用时 Python 肯定会引发异常，即使参数实际上没有被使用。那么一定是在某个时候，Python 为我们自动传入了该参数。
> 

方法的特殊之处在于它知道是那个对象调用的它，因为 Python 会将实例对象会作为函数的第一个参数被传入。在上述示例中，调用 `x.f()` 就相当于调用 `MyClass.f(x)`

> [!summary] 
> 
> 当一个实例的**非数据属性被引用**时，将搜索该实例所属的类。 如果 **名称表示一个属于函数对象** 的有效类属性，则 **指向实例对象和函数对象的引用将被打包为一个方法对象**。 
> 
> 当传入一个参数列表调用该方法对象时，将基于实例对象和参数列表构造一个新的参数列表，并传入这个新参数列表调用相应的函数对象。
> 

### 类变量和实例变量

在 Python 中，实例对象拥有自己的名字空间，因此 **实例中的属性是每个实例独有的**。类同样是一个对象，**类中的属性对该类的所有实例共享**

```python
class Dog:
    kind = "canine"  # 类属性： Dog 类的所有实例共享这个变量

    def __init__(self, name):
        self.name = name  # 实例属性：独属于指定的实例

d = Dog("Fido")
e = Dog("Buddy")

print(d.kind, e.kind)  # 被 Dog 类的所有实例共享
print(d.name, e.name)  # 每个实例都拥有自己的属性
```

上述代码运行输出的结果为

```shell
canine canine
Fido Buddy
```

由于类变量被该类的所有实例共享，当共享数据是一个可变对象时会导致令人惊讶的结果。

```python
class Dog:
    kind = "canine"  # 类属性： Dog 类的所有实例共享这个变量
    tricks = []      # 类变量：可变对象
    def __init__(self, name):
        self.name = name  # 实例属性：独属于指定的实例

    def add_trick(self, trick):
        self.tricks.append(trick)


d = Dog("Fido")
d.add_trick('roll over')
e = Dog("Buddy")
e.add_trick('play dead')

print(d.tricks)  # error：得到错误的结果 ['roll over', 'play dead']
```

> [!tip] 
> 
> 由于 `Dog.tricks` 是列表，被 `Dog` 的所有实例共享。因此，每个实例都可以看见 `Dog.tricks` 的修改。这是不符合实际的类设计
> 

正确的类设计应该使用实例变量

```python
class Dog:
    kind = "canine"  # 类属性： Dog 类的所有实例共享这个变量
    # tricks = []      # 类变量：可变对象

    def __init__(self, name):
        self.name = name  # 实例属性：独属于指定的实例
        self.tricks = []  # 实例变量：独属于指定的实例

    def add_trick(self, trick):
        self.tricks.append(trick)


d = Dog("Fido")
d.add_trick('roll over')
e = Dog("Buddy")
e.add_trick('play dead')

print(d.tricks)  # ['roll over']
```

## 补充说明

实例及其所属类拥有同名属性时，通过实例查找属性时优先使用实例的属性。

```python
class Warehouse:
	purpose = "storage"
	region = "west"

w1 = Warehouse()
print(w1.purpose, w1.region)  # 结果：storage west

w1.region = "east"   # 给实例 w1 创建了一个新属性 region
print(w1.purpose, w1.region)  # 结果：storage east
```

在 Python 中，类不能用于实现纯抽象数据类型。因为数据属性可以被除了方法外的普通用户访问。

> [!tip] 
> 
> **在 Python 中没有任何东西能够强制隐藏数据**，它完全是基于约定的。
> 

对象的普通用户应该谨慎的使用数据属性，因为这会破坏由方法维护的固定变量。

> [!attention] 
> 
> 用 C++ 的术语来说，直接操作数据成员会破坏类的不变式
> 

此外，在保证避免名字冲突的情况下，我们可以向一个实例对象添加数据属性而且不会影响方法的可用性

> [!attention] 
> 
> 请注意：在 Python 的方法内部，**须通过方法的第一个参数引用实例的数据属性或则其他方法**，没有任何简便方式
> 
> 方法的第一个参数通常约定命名为 `self`，没有任何其他特殊含义。不遵循这样的约定可能会导致代码可读性变差。
> 

**任何一个作为类属性的函数都为该类的实例定义了一个相应的方法**。函数定义的文本并非必须包含于类定义之内：将一个函数对象赋值给一个局部变量也是可以的

```python
# 在类之外定义的函数
def f1(self, x, y):
    return min(x, x+y)

class C:
    f = f1

    def g(self):
        return 'hello world'

    h = g
```

在 Python 中，每个值都是一个对象，因此具有 _类_ （也称为 _类型_），并存储为 `object.__class__`

```python
class Dog:
    kind = "canine"  # 类属性： Dog 类的所有实例共享这个变量
    # tricks = []      # 类变量：可变对象

    def __init__(self, name):
        self.name = name  # 实例属性：独属于指定的实例
        self.tricks = []  # 实例变量：独属于指定的实例

    def add_trick(self, trick):
        self.tricks.append(trick)


d = Dog("Fido")
print(d.__class__) # __main__.Dog
```

## 保护属性和私有属性

虽然在 Python 中不像 C++ 那样可以限制属性的访问权限，但是 Python 通过约定提供了类似的访问权限控制

> [!tip] 保护属性：Python 约定采用 **单下划线** 开始的名字用作保护属性。
> 
> 请注意：名字末尾没有下划线
> 

```python
class Person:
    
    def __init__(self, name, age):
        self._name = name
        self._age = age
        
p = Person("张三", 48)
```

这样 IDE 就不会自动提示属性 `_name` 和 `_age`，除非显式访问

![[Pasted image 20250226213902.png]]

> [!tip] 私有属性：Python 中约定采用 **双下划线** 开始的名字被用作私有属性
> 
> 注意：名字结尾没有双下划线
> 

```python
class Person:

    def __init__(self, name, age):
        self.__name = name
        self.__age = age

p = Person("张三", 48)
```

实例 `p` 中不会存在 `__name` 和 `__age` 这两个属性。我们通过 `__dict__` 查看实例对象的属性

```python
print(p.__dict__)  # {'_Person__name': '张三', '_Person__age': 48}
```

观察输出结果，`__name` 和 `__age` 分别变成了 `_Person__name` 和 `_Person__age`。也就是说，Python  `__name` 的属性替换为了 `_classname__name` 的形式

## property 装饰器

当希望查看私有数据属性的值时，我们不能直接通过换名规则去访问私有属性，而是应该提供访问私有属性的方法。就像 C++ 中提供一个 `const` 成员函数一样

```python
class Person:

    def __init__(self, name, age):
        self.__name = name
        self.__age = age
    
    def name(self):
        return self.__name

    def age(self):
        return self.__age

p = Person("张三", 48)

print(p.name())  # 张三
print(p.age())   # 48
```

通过函数访问数据属性并不优雅，而且不符合 Python 的使用习惯。Python 提供了一个装饰器 `property`，它的作用就是将一个方法当作数据属性使用。原理将在 [[Python：描述器]] 中介绍

```python
class Person:

    def __init__(self, name, age):
        self.__name = name
        self.__age = age

    @property
    def name(self):
        return self.__name

    @property
    def age(self):
        return self.__age

p = Person("张三", 48)

print(p.name)  # 张三
print(p.age)   # 48
```

> [!attention] 
> 
> 请注意：这里的属性时不能赋值的。下面的代码给出了错误提示，说明属性 `name` 没有 `setter` 函数
> 

```python
class Person:

    def __init__(self, name, age):
        self.__name = name
        self.__age = age

    @property
    def name(self):   # name <==> property(name)
        return self.__name

    @property
    def age(self):
        return self.__age

p = Person("张三", 48)

print(p.name)
print(p.age)

p.name = "李斯"  # AttributeError: property 'name' of 'Person' object has no setter
```

如果需要给属性 `name` 和 `age` 赋值，就需要提供一个 `setter`。此时，`name` 和 `age` 都是 `property` 类的实例，该实例提供了一个 `setter` 方法返回 `property` 对象的一个副本

```python
class Person:

    def __init__(self, name, age):
        self.__name = name
        self.__age = age

    @property
    def name(self):
        return self.__name

    @name.setter
    def name(self, name):
        if not isinstance(name, str):
            raise ValueError("Name must be a string.")
        self.__name = name

    @property
    def age(self):
        return self.__age

    @age.setter
    def age(self, age):
        if not isinstance(age, int):
            raise ValueError("Age must be an integer.")
        if age < 0:
            raise ValueError("Age cannot be negative.")

        self.__age = age


p = Person("张三", 48)

print(p.name)
print(p.age)

p.name = "李斯" 
p.age = 5000

print(p.name)
print(p.age)
```

## staticmethod 装饰器

在 Python 中，可以为类提供一个 **静态方法**：属于类但不属于实例。换句话说，**静态方法不会绑定实例对象**

例如，在实现一个代表三角形的类时，我们需要检查提供的三条边的长度能否构成一个三角形。用于检查三条边能否构成三角形的方法不依赖于三角形对象，因此将其实现为静态方法

```python
class Triangle:
    def __init__(self, a, b, c):
        self.a = a
        self.b = b
        self.c = c

    @staticmethod
    def is_triangle(a, b, c):
        if a <= 0 or b <= 0 or c <= 0:
            raise ValueError("三角形的三条边必须是正整数.")

        if a + b > c and a + c > b and a + c > b:
            return True

        return False

Triangle.is_triangle(1, 2, 3)
```

## classmethod 装饰器

在类中定义的函数默认是绑定给类的实例使用的。有时候，我们想要类中定义的函数绑定给类使用，这样的函数称为 **类方法**

在 Python 中，定义类方法需要使用 `classmethod` 装饰器，该装饰器就将当前类当中第一个参数传递。如下示例，展示了在 Python 中实现单例模式的方式之一。类方法 `get_instance()` 用于返回一个类的实例，因此该方法需要将当前类作为参数

```python
class Singleton:

    @classmethod
    def get_instance(cls):
        if hasattr(cls, "_instance"):
            return getattr(cls, "_instance")
        else:
            cls._instance = cls()
            return cls._instance

a = Singleton.get_instance()
b = Singleton.get_instance()
c = Singleton.get_instance()
```

> [!attention] 
> 
> 请注意：上述实现存在一个 bug。`_instance` 类属性是可以被访问的。
> 

```python
a._instance  # 这里可以访问实例对象。这不是关键的问题
Singleton._instance = Singleton()  # bug，我们可以修改类的单例
```

建议使用 `__instance`，可以进一步的隐藏，虽然依旧可能被修改，但是增加了修改成本

```python
class Singleton:

    @classmethod
    def get_instance(cls):
        if hasattr(cls, f"_{cls.__name__}__instance"):
            return getattr(cls, f"_{cls.__name__}__instance")
        else:
            cls.__instance = cls()
            return cls.__instance

a = Singleton.get_instance()
b = Singleton.get_instance()
c = Singleton.get_instance()
```
