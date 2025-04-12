# 类与对象

在 [[C++：类与对象]] 中介绍面向对象的基本概念。与 C++ 一样，Python 也是支持面向对象编程的，**类** 也是创建新类型的工具。不像 C++ 那样在类中引入了许多复杂的语法规则，Python 中的类并没有引入过多复杂的语言

> [!tip] 
> 
> **类描述了一种概念**。例如，Python 中的 `float` 类与运算符 `+ - * /` 一起描述了 **实数** 这一数学概念，但是这并不是真正意义上的实数
> 
> 也就是说，类是用于描述现实世界中的各自概念的
> 

**类** 用于描述一种概念，编程语言中会默认提供一些概念，然而编程语言不可能也无法完全提供现实世界中的所有概念。因此，现代编程语言提供了一种让程序员自定义概念的手段就是 **类**

## 类定义

在 Python 中，定义类使用关键字 `class`，随后是 **类名**。例如

```python
class ClassName:
    <语句-1>
    .
    .
    .
    <语句-N>
```

与函数定义 (`def` 语句) 一样，**类定义必须先执行才能生效**。在实践中，**类定义内的语句通常都是函数定义**，但也可以是其他语句。这部分内容稍后再讨论。**类里的函数定义一般是特殊的参数列表**，这是由方法调用的约定规范所指明的 --- 同样，稍后再解释。

> [!important] 
> 
> 当进入类定义时，**将创建一个新的命名空间**，并将其用作局部作用域 --- 因此，所有对局部变量的赋值都是在这个新命名空间之内。 特别的，**类内部函数定义的函数名会绑定到类的局部名字空间中**
> 

> [!tip] 
> 
> 当 (从结尾处) 正常离开类定义时，将创建一个 **类对象**。 这基本上是一个围绕类定义所创建的命名空间的包装器。 
> 
> 原始的 (在进入类定义之前有效的) 作用域将重新生效，类对象将在这里与类定义头所给出的类名称进行绑定 (在这个示例中为 `ClassName`)。
> 

换句话说就是：`class` 关键字创建了一个类对象，并使用 `class` 后的标识符引用该类对象

我们在定义类时，类名采用 **大驼峰** 命名方式进行命名。当 Python 解释器读取到关键字 `class` 时，Python 就会创建一个 **类对象** ，然后在 `class` 所在的名字空间中创建一个名字（即类名）并引用该类对象

如下定义了一个 `Person` 类

```python
class Person:
    """An example class"""
    x = "abc"

    def show(self):
        return Person.__name__

# 此处，离开类定义，原始的作用域重新生效，并且在当前名字空间的名字 Person 引用 class 创建的类对象
```

> [!tip] 
> 
> 在类定义中的变量定义和函数定义分别称为类对象的 **数据属性** 和 **函数属性**，它们都 **类属性**
> 
> 与 C++ 不同的是，类属性保存在类对象的名字空间中，被该类的所有实例共享。也就是说，通过类实例可以访问类属性。当类属性发生变化时，所有的实例都能观察到这个变化
> 
> 函数属性类似于 C++ 中的成员函数。与 C++ 不同的是，Python 要求在类中的定义的函数的形参列表的第一个参数名命名为 `self`，它代表了调用这个函数的实例，类似于 C++ 中的 `this` 指针
> 

## 类对象

在 Python 中，类也是对象，通常被称为 **类对象**。Python 中类对象支持两种操作：**属性引用** 和 **实例化**

### 属性引用

**属性引用** 使用 Python 中所有属性引用所使用的标准语法: `obj.name`。 **有效的属性名称** 是类对象被创建时存在于 **类命名空间中的所有名称**。 因此，如果类定义是这样的:

```python
class MyClass:
    """一个简单的示例类"""
    i = 12345

    def f(self):
        return 'hello world'
```

那么 `MyClass.i` 和 `MyClass.f` 就是有效的属性引用，将分别返回一个整数和一个函数对象。

类属性也可以被赋值，因此可以通过赋值来改变 `MyClass.i` 的值。

```python
MyClass.i = 6789
```

此外，Python 创建类对象时还会附加一些其他的属性。例如，[`__doc__`](https://docs.python.org/zh-cn/3.13/reference/datamodel.html#type.__doc__ "type.__doc__") 也是一个有效的属性，将返回所属类的文档字符串: `"一个简单的示例类"`。

```python
print(MyClass.__doc__)  # 输出：一个简单的示例类
```

此外，还有一个十分重要的类属性 [`__dict__`](https://docs.python.org/zh-cn/3.13/reference/datamodel.html#type.__dict__)， 这是一个提供类的命名空间的 **只读** 视图的 **映射代理**。

```python
print(MyClass.__dict__)  # 类名字空间的只读字典视图
print(type(MyClass.__dict__))  # <class 'mappingproxy'>
```

> [!tip] 
> 
> `mappingproxy` 类是一个 **映射的只读代理**。 它提供了对映射条目的动态视图，这意味着当映射发生改变时，视图会反映这些改变。
> 

### 实例化

**类的 _实例化_ 使用函数表示法**。 可以把类对象视为是返回该类的一个新实例的不带参数的函数。 举例来说（假设使用上述的类）:

```python
x = MyClass()
```

上述语句会创建类 `MyClass` 的新 _实例_ 并将此实例分配给局部变量 `x`。

> [!important] 
> 
> 实例化操作 (“调用”类对象) 会创建一个 **空对象**。 类中的数据属性不会出现在类的实例中。
>

```python
print(x.__dict__)  # 输出：{}
```

实例对象也有一个属性 [`__dict__`](https://docs.python.org/zh-cn/3.13/reference/datamodel.html#object.__dict__)，一个用于 **存储对象的属性的字典** 或其他映射对象。 并非所有实例都具有 `__dict__` 属性

> [!important] 
> 
> 类属性一定不会出现在实例的属性字典(`__dict__`)中
> 

许多类都希望创建的对象实例是根据特定初始状态定制的。 因此一个类可能会定义名为 [`__init__()`](https://docs.python.org/zh-cn/3.13/reference/datamodel.html#object.__init__ "object.__init__") 的特殊方法，就像这样:

```python
class MyClass:
    """一个简单的示例类"""
    i = 12345

    def __init__(self):
        self.data = []

    def f(self):
        return 'hello world'
```

当一个类定义了 [`__init__()`](https://docs.python.org/zh-cn/3.13/reference/datamodel.html#object.__init__ "object.__init__") 方法时，类的实例化会自动为新创建的类实例唤起 `__init__()`。 因此在这个例子中，可以通过以下语句获得一个已初始化的新实例:

```python
x = MyClass()
print(x.__dict__)  # 输出：{"data": []}
```

当然，[`__init__()`](https://docs.python.org/zh-cn/3.13/reference/datamodel.html#object.__init__ "object.__init__") 方法还有一些参数用于实现更高的灵活性。 在这种情况下，提供给类实例化运算符的参数将被传递给 `__init__()`。 例如，

```python
class Complex:
    def __init__(self, real, image):
        self.real = real
        self.image = image


x = Complex(3.0, -4.5)
print(x.real, x.image)  # 3.0 -4.5
```

## 实例对象

现在我们能用实例对象做什么？ **实例对象所能理解的唯一操作是属性引用**。 有两种有效的属性名称：**数据属性** 和 **方法**。

### 数据属性

**数据属性** 对应于 C++ 中的 _数据成员_，在 Python 中也称为 **实例变量**。 数据属性不需要声明；**就像局部变量一样，它们将在首次被赋值时产生**

举例来说，如果 `x` 是上面创建的 `MyClass` 的实例，则以下代码将打印数值 `16`，且不保留任何追踪信息:

```python
x = MyClass()

x.counter = 1
while x.counter < 10:
    x.counter = x.counter * 2

print(x.counter)
del x.counter
```

另一种实例属性引用称为 **方法**。 方法是 **“从属于”** 对象的函数。实例对象的有效方法名称依赖于其所属的类。 根据定义，一个类中所有是函数对象的属性都是定义了其实例的相应方法。 因此在我们的示例中，`x.f` 是有效的方法引用，因为 `MyClass.f` 是一个函数，而 `x.i` 不是方法，因为 `MyClass.i` 不是函数。 但是 `x.f` 与 `MyClass.f` 并不是一回事，`x.f` 是 **方法对象**，而 `MyClass.f` 是函数对象

### 方法对象

方法对象的创建过程：当通过实例 `x` 查找属性 `f` （即 `x.f`）时，Python 会首先在 `x.__dict__` 中查找名字 `f`。如果没找到，则会在 `type(x).__dict__` 中查找。如果在 `type(x).__dict__` 中查找到了名字 `f`，并且名字 `f` 时一个函数对象，Python 就会将实例 `x` 与函数对象 `f` 进行绑定，从而创建方法对象

通常情况下，方法在绑定后立即被调用:

```python
x.f()  # 方法绑定实例后立即调用
```

在 `MyClass` 示例中，这将返回字符串 `'hello world'`。

```python
class MyClass:
    """一个简单的示例类"""
    i = 12345

    def __init__(self):
        self.data = []

    def f(self):
        return 'hello world'


x = MyClass()
res = x.f()
print(res)  # 输出：hello, world
```

但是，方法并不是必须立即调用: `x.f` 是一个方法对象，它可以被保存起来以后再调用。 例如:

```python
xf = x.f
while True:
    print(xf())  # 持续输出 hello, world
```

当一个方法被调用时究竟会发生什么？ 你可能已经注意到尽管 `f()` 的函数定义指定了一个参数，但上面调用 `x.f()` 时却没有带参数。 这个参数发生了什么事？ 当一个需要参数的函数在不附带任何参数的情况下被调用时 Python 肯定会引发异常 --- 即使参数实际上没有被使用

实际上，你可能已经猜到了答案：**方法的特殊之处就在于实例对象会作为函数的第一个参数被传入**。 

> [!attention] 
> 
> 在我们的示例中，调用 `x.f()` 其实就相当于 `MyClass.f(x)`
> 

总之，调用一个具有 $n$ 个参数的方法就相当于调用再多一个参数的对应函数，这个参数值为方法所属实例对象，位置在其他参数之前。

> [!summary] 
> 
> 总而言之，方法的运作方式如下。 当一个实例的非数据属性被引用时，将搜索该实例所属的类。 如果名称表示一个属于函数对象的有效类属性，则指向实例对象和函数对象的引用将被打包为一个方法对象。 当传入一个参数列表调用该方法对象时，将基于实例对象和参数列表构造一个新的参数列表，并传入这个新参数列表调用相应的函数对象
> 

## 实例变量 vs 类变量

一般来说，实例变量用于每个实例的唯一数据，而类变量用于类的所有实例共享的属性和方法

```python
>>> class Dog:
...     kind = "canine"            # 类变量：该类的实例都可以访问
...     def __init__(self, name):
...         self.name = name       # 
...
>>> d = Dog("Fido")
>>> e = Dog("Buddy")
>>> d.kind  # 访问类变量 kind
'canine'
>>> e.kind
'canine'
>>> d.name  # 访问实例变量 name
'Fido'
>>> e.name
'Buddy'
```

> [!tip] 
> 
> 当一个实例 `obj` 访问属性 `attr` 时，会首先在 `obj.__dict__` 中查找，如果找到对应的属性就结束查找，否则就在 `type(obj).__dict__` 中进行查找
> 

在上述代码中，Python 在执行属性访问语句 `d.kind` 时，首先在 `d.__dict__` 中查找 `kind`，由于 `d.__dict__` 中没有 `kind`，所以 Python 会去查找 `type(d).__dict__`，此时找到了属性 `kind`。同理，`e.kind` 会在 `type(e).__dict__` 中查找到。由于 `d` 和 `e` 都是类 `Dog` 的实例，所以 `d.kind` 和 `e.kind` 时同一个对象

> [!warning] 
> 
> 在 [[Python：函数基础#默认参数]] 我们介绍了可变默认参数带来的奇怪语义，这是由于可变对象在多次调用中共享带来的问题
> 
> 在这里，如果类变量是可变类型的对象，那么通过实例修改了类变量，那么其他实例也会发现这个修改
> 

```python
>>> class Dog:
...     tricks = []  # 可变类变量
...     def __init__(self, name):
...         self.name = name
...     def add_trick(self, trick):
...         self.tricks.append(trick)
...
>>> d = Dog("Fido")
>>> e = Dog("Buddy")
>>> d.add_trick("roll over")  # 通过实例 d 修改了类变量
>>> e.tricks   # 实例 e 也会看见这个修改
['roll over']
```

正确的类设计应该使用 **实例变量**，因为每只狗狗拥有的名字和技能是不同的，它们之间不能共享

```python
>>> class Dog:
...     def __init__(self, name):
...         self.name = name
...         self.tricks = []
...
...     def add_trick(self, trick):
...         self.tricks.append(trick)
...
>>> d = Dog("Fido")
>>> e = Dog("Buddy")
>>> d.add_trick("roll over")  # 实例实例 d 添加技能
>>> e.tricks  # 实例 e 不受影响
[]
>>> d.tricks
['roll over']
>>> e.add_trick("play dead")  # 给实例 e 添加技能
>>> d.tricks  # 实例 d 不受影响
['roll over']
>>> e.tricks
['play dead']
```

> [!summary] 
> 
> 实例会优先查找自己属性字典(`__dict__`)中的属性，然后才会查找实例所属类的属性。也就是说，当类属性和实例属性同名时，通过实例查找属性时，会优先使用实例属性
> 

## 补充说明

### 访问控制约定

在 C++ 中，限制成员的访问有明确的关键字，而在 Pyhon 中没有任何关键字用于限制属性的访问，而是通过 **命名约定** 限制属性访问的

> [!tip] 
> 
> **保护属性** 的命名约定：以 **单下划线(`_`)** 开头。这样属性是被保护的，只能在类定义以及其派生类定义时可以直接访问。通过实例无法不能访问。对于 Python 而言，这样的约定只是为了让 Python 的 IDE 给出友好提示，而非无法访问。


如下图，定义了一个 `Dog` 类，有两个保护成员 `_name` 和 `_age`。当我们通过实例访问这两个属性时，Pycharm 等 IDE 工具就会提示我们访问了保护变量

![[Pasted image 20250410233229.png]]

> [!tip] 
> 
> **私有属性** 的命名约定：以 **双下划线** 开头并且 **不能以双下划线结尾** 命名，这样的属性时私有的，即只能在类定义中访问，离开类定义后就无法访问了。
> 
> 请注意，Python 是通过 **名字替换** 的方式实现的私有属性。对于符合私有属性命名约定的属性，Python 会在属性名前面添加 `_ClassName` 的方式隐藏属性，即被替换后的属性名为 `_ClassName__xxx`

如下图，定义了一个 `Dog` 类，有两个私有属性 `__name` 和 `__age`。我们查看实例的 `__dict__`，发现实例的属性变为了 `_Dog__name` 和 `_Dog__age`

![[Pasted image 20250410234030.png]]

> [!summary] 
> 
> 实际上，**在 Python 中没有任何东西能强制隐藏数据** --- 它是完全基于约定的。
> 
> 我们应当避免地直接使用数据属性，因为 **通过直接操作数据属性的方式可能会破坏由方法所维护的固定变量，即破坏 _不变式_**
> 
> 请注意：我们可以随时向一个实例对象添加他们自己的数据属性而不会影响方法的可用性，只要保证避免名称冲突 --- 再次提醒，**使用命名约定可以省去许多令人头痛的麻烦**
> 

#### property 装饰器

对于隐藏属性，不建议使用者直接修改，而是使用类提供的 **数据属性访问接口** 进行访问和修改

如下示例，表示一个平面上的点。我们不希望直接修改这点的位置，而是通过类提供的方法进行修改

```python
class Point:
    def __init__(self, x, y):
        self.__x = x
        self.__y = y


point = Point(10, 20)
print(point.__x, point.__y) # 错误：无法访问 __x 和 __y
```

> [!warning] 
> 
> 上述脚本执行时会抛出 `AttributeError`，告知我们没有属性 `__x` 和 `__y`
> 

为了能够访问和修改这个两个属性，就需要提供一个 `x()` 和 `y()` 两个方法

```python
class Point:
    def __init__(self, x, y):
        self.__x = x
        self.__y = y

    def x(self):
        return self.__x

    def y(self):
        return self.__y


point = Point(10, 20)
print(point.x(), point.y())  # 通过函数访问
```

然而，`x` 和 `y` 在概念上应该是数据属性，此处表现出来的是方法属性。Python 提供的一个装饰器 `property`，它可以将这种读取值的方式标记为数据属性。当通过数据属性方式访问方法时，就会自动执行该方法的调用

```python
class Point:
    def __init__(self, x, y):
        self.__x = x
        self.__y = y

    @property
    def x(self):
        print("通过数据属性的访问方式我被调用了")
        return self.__x

    @property
    def y(self):
        print("通过数据属性的访问方式我被调用了")
        return self.__y


point = Point(10, 20)
print(point.x, point.y)  # 通过函数访问
```

为了修改值，往往使用 `obj.name = value` 的方式，此时我们还无法做到修改值

```python
point = Point(10, 20)
point.x = 30
```

这段代码执行时，也会抛出 `AttributeError`，异常信息为 `property 'x' of 'Point' object has no setter`，这个异常信息提示我们属性 `x` 没有 `setter` 方法

为了能够通过 `obj.name = value` 的方式修改属性值，只需要重新定义一个 `name` 函数，并标记上 `@name.setter` 即可

```python
class Point:
    def __init__(self, x, y):
        self.__x = x
        self.__y = y

    @property
    def x(self):   # getter
        return self.__x

    @x.setter
    def x(self, x):  # setter
        if not isinstance(x, (int, float)):
            raise TypeError("x must be int or float")
        self.__x = x

    @property
    def y(self):  # getter
        return self.__y

    @y.setter
    def y(self, y): # setter
        if not isinstance(y, int| float):
            raise TypeError("y must be int or float")
        self.__y = y

point = Point(10, 20)
print(point.x, point.y)  # 通过函数访问
point.x = 30
point.y = 100
print(point.x, point.y)  # 通过函数访问
```

### 方法调用约定

在方法内部引用数据属性或其他方法时，并没有简便方式。 我发现这实际上提升了方法的可读性：当浏览一个方法代码时，不会存在混淆局部变量和实例变量的机会。

**方法的第一个参数常常被命名为 self**。 这也不过就是一个约定: `self` 这一名称在 Python 中绝对没有特殊含义。 但是要注意，不遵循此约定会使得你的代码对其他 Python 程序员来说缺乏可读性，而且也可以想像一个 _类浏览器_ 程序的编写可能会依赖于这样的约定

**任何一个作为类属性的函数都为该类的实例定义了一个相应方法**。 函数定义的文本并非必须包含于类定义之内：将一个函数对象赋值给一个局部变量也是可以的。

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


方法可以通过使用 `self` 参数的方法属性调用其他方法

```python
class Bag:
    def __init__(self):
        self.data = []

    def add(self, x):
        self.data.append(x)

    def addtwice(self, x):
        self.add(x)
        self.add(x)
```

每个值都是一个对象，因此具有 _类_ （也称为 _类型_），并存储为 `object.__class__` 。

```python
class Dog:

    def __init__(self, name, age):
        self.__name = name
        self.__age = age


d = Dog("Buddy", 3)

print(d.__class__)  # <class '__main__.Dog'>

print(Dog.__class__)  # <class 'type'>
```

> [!attention] 
> 
> 再次强调：在 Python 中类也是一个对象，它是 `type` 类的对象。换句话说，Python 中的任何一个类都是 `type` 类的实例
> 

> [!summary] 
> 
> 方法在调用时，会将调用该方法的实例传递给方法的第一个参数，这个参数按照约定命名为 `self`，从而可以提高 Python 方法的可读性
> 
> 只要一个类属性是函数对象，那么都为该类的实例定义了一个方法。因此，我们可以在类外定义函数，然后将这个函数对象赋值给类的局部变量
> 
> 每个对象都有一个 `__class__` 属性，它是该对象的类
> 

在类中定义的函数，默认情况下都是给该类实例调用的方法，即 **绑定实例的方法**。然而，有时候我们可能需要再没有实例的时候调用某些方法。

在 Python 中，有两类方法在调用时可以不用于绑定实例：**静态方法** 和 **类方法**

#### staticmethod 装饰器

我们在定义 **三角形类** 时，需要提供三条边的边长度用于构造三角形，并提供计算三角形周长和面积的方法。然而，传入的 $3$ 条边并不一定能够构造三角形，因此需要验证这 $3$ 三边是否能够构成三角形

```python
class Trangle:

	def __init__(self, a, b, c):
		if not (a + b > c and a + c > b and b + c > a):
			raise ValueError(f"{a} {b} {c} 不能构成三角形")
		self.a = a
		self.b = b
		self.c = c

	def circumference(self):
		pass

	def area(self):
		pass
```

验证三条边长是否能够构成三角形的代码最好不要出现在`__init__()` 方法中，我们要保证 `__init__()` 方法中的代码比较简单。因此，我们需要将这段代码抽取出来编写成一个验证方法。由于此时尚未有三角形的实例，因此这个验证方法不需要知道是哪个实例调用的它

```python
import math


class Triangle:

	def __init__(self, a, b, c):
		if not self.valid_triangle(a, b, c):
			raise ValueError(f'the three edge {a} {b} {c} cannot be triangulated')
			
		self.a = a
		self.b = b
		self.c = c

	def circumference(self):
		return self.a + self.b + self.c

	def area(self):
		p = self.circumference() / 2
		return math.sqrt(p * (p - self.a) * (p - self.b) * (p - self.c))

	@staticmethod
	def valid_triangle(a, b, c):
		return a + b > c and a + c > b and b + c > a
```

因为在调用这个方法时三角形对象尚未创建出来（因为都不知道三条边能不能构成三角形），所以这个方法是 **属于三角形类而并不属于三角形对象的**。在 Python 中，我们可以使用装饰器 `staticmethod` 标记一个在类中定义的函数为静态方法。

> [!attention] 
> 
> 请注意：`staticmethod` 装饰器只是拦截了方法的调用。此时，不在给方法传递实例
> 
> 关于 `staticmethod` 的原理，我们会在 [[Python：描述器]] 中介绍
> 

#### classmethod 装饰器

在 Python 中，类本身也是对象，它也可以有方法，这个方法被称为 **类方法**。想要为类定义方法可以在类定义中使用装饰器 `classmethod` 标记一个函数，并让其成为类方法。

> [!tip] 
> 
> 在类定义时，被 `classmethod` 装饰的函数会变成类方法，通过类调用该方法时，会将类作为第一个参数传递给该方法
> 

**类方法通常用于创建该类的实例**。比如，内置类型 `int` 就有一个类方法 `int.from_bytes()` 它从字节序列中返回一个 `int` 类的实例。

```python
class Dog:

    @classmethod
    def from_name(cls):
        print("类方法", cls)


print(Dog.from_name)  # <bound method Dog.from_name of <class '__main__.Dog'>>
print(Dog().from_name)  # <bound method Dog.from_name of <class '__main__.Dog'>>

Dog.from_name()  # 类方法 <class '__main__.Dog'>
Dog().from_name()  # 类方法 <class '__main__.Dog'>
```

> [!attention] 
> 
> 请注意：无论是通过类对象还是通过其实例对象，最终传递给类方法的第一个参数始终是该类对象
> 

## 构造与析构

通常，在类实例化时我们希望给实例初始化一些属性。就像 C++ 中那样给对象的数据成员初始化一样。

在 Python 中，对象的构造函数是 Python 的特殊方法 `__init__(self)`。任何类都有默认的 `__init__(self)`，即使我们不定义 `__init__(self)` 方法，Python 解释器也会帮我们初始化对象，只它初始化的对象是一个 **空对象**，即，没有任何属性的对象

> [!tip] 
> 
> 在 Python 中，任何对象在被刚创建出来的时候都是空的，它没有携带任何属性。只有被 `__init__(self)` 初始化之后，对象才会被设置属性
> 

在 Python 中，给对象添加属性就像定义变量那样直接赋值即可

```python
class Dog:

    def __init__(self, name, age):
        self.name = name  # 给对象添加属性 name
        self.age = age    # 给对象添加属性 age
```

> [!tip] 
> 
> 和 C++ 一样，**构造函数不能有返回值**。在 Python 中，构造函数不能有除了 `None` 之外的任何其他的返回值
> 

对象可能持有一些资源，例如 **打开的文件**、**锁** 等，当对象被销毁时，这些资源也应该被释放。当对象要被销毁时，Python 解释器会调用特殊方法 `__del__(self)` 

```python
class Dog:

    def __init__(self, name, age):
        self.name = name
        self.age = age

    def __del__(self):
        print("Dog was deleted")
```

然而，**对于 Python 而言，对象的销毁完全由解释器的垃圾回收机制管理**。也就是说，对象的销毁可能不及时，从而导致进程中的 **文件关闭不及时** 或者 **锁释放不及时**。
