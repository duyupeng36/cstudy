# 描述器

描述器是 Python 一个独特的特性，它可以 **拦截属性访问**。Python 中的 `property` `staticmethod` 和 `classmethod` 都是基于描述器实现的。

在开始介绍描述器之前，我们看如下示例：假想你正在给学校写一个成绩管理系统，并没有太多编码经验的你，可能会这样子写。

```python title:descriptor.py
class Student:

    def __init__(self, name, math, chinese, english):
        self.name = name
        self.math = math
        self.chinese = chinese
        self.english = english

    def __str__(self):
        return f"Student(name={self.name}, math={self.math}, chinese={self.chinese}, english={self.english})"


if __name__ == '__main__':

    student1 = Student("小明", 90, 89, 100)

    print(student1)
```

上述代码看起来一切都很合理，输出结果如下

```
Student(name=小明, math=90, chinese=89, english=100)
```

然而，上述程序存在逻辑漏洞。如果老师在输入成绩时，不小心将成绩输入成负数，程序也不会给出任何提示

```python
>>> from descriptor import Student
>>> Student("小红", -78, 90, 102)
Student(name=小红, math=-78, chinese=90, english=102)
```

为了解决这个问题，我们首先就会想到在 `__init__()` 中添加逻辑判断。然而，在 `__init__()` 中太多的逻辑判断会导致程序的可读性降低。

这时候，我们想到使用 `property` 装饰器。因此，我们写了下面的代码

```python title:descriptor.py
class Student:

    def __init__(self, name, math, chinese, english):
        self.name = name
        self.math = math
        self.chinese = chinese
        self.english = english

    def __str__(self):
        return f"Student(name={self.name}, math={self.math}, chinese={self.chinese}, english={self.english})"

    __repr__ = __str__

    @property
    def math(self):
        return self._math

    @math.setter
    def math(self, value):
        if not isinstance(value, (int, float)):
            raise TypeError(f"{value!r} is not an int or float number")
        if value < 0 or value > 120:
            raise ValueError(f"{value!r} is not between 0 and 120")

        self._math = value

    @property
    def chinese(self):
        return self._chinese

    @chinese.setter
    def chinese(self, value):
        if not isinstance(value, (int, float)):
            raise TypeError(f"{value!r} is not an int or float number")
        if value < 0 or value > 120:
            raise ValueError(f"{value!r} is not between 0 and 120")
        self._chinese = value

    @property
    def english(self):
        return self._english

    @english.setter
    def english(self, value):
        if not isinstance(value, (int, float)):
            raise TypeError(f"{value!r} is not an int or float number")
        if value < 0 or value > 120:
            raise ValueError(f"{value!r} is not between 0 and 120")
        self._english = value


if __name__ == '__main__':

    student1 = Student("小明", 90, 89, 100)

    print(student1)
```

上述代码对于可读性提高了不少，并且也能检查输入的数据是否符合要求

```python
>>> import descriptor
>>> descriptor.Student("小明", -78, 90, 102)
Traceback (most recent call last):
  File "<python-input-1>", line 1, in <module>
    descriptor.Student("小明", -78, 90, 102)
    ~~~~~~~~~~~~~~~~~~^^^^^^^^^^^^^^^^^^^^^^
  File "C:\Users\duyup\Documents\pyfiles\descriptor.py", line 6, in __init__
    self.math = math
    ^^^^^^^^^
  File "C:\Users\duyup\Documents\pyfiles\descriptor.py", line 24, in math
    raise ValueError(f"{value!r} is not between 0 and 120")
ValueError: -78 is not between 0 and 120
```

然而，上述代码存在 **冗余**。对于属性 `math` `chinese` `english` 都存在相同的代码；此外，每增加一种属性，都需要提供额外的两个方法并且这些方法执行的逻辑是相同的

> [!question] 
> 
> 三个变量的合法性逻辑都是一样的，只要大于 $0$，小于 $120$ 就可以，代码重复率太高了，这里三个成绩还好，但假设还有地理、生物、历史、化学等十几门的成绩呢，这代码简直没法忍
> 

Python 的描述器就可以用于解决这个问题。下面我们介绍 Python 中的描述器

## 实现描述器

如果一个类定义了下面几个方法中的任何一个，那么该类就是一个 **描述器类**

| 方法                                           | 描述            |
| :------------------------------------------- | :------------ |
| `object.__set__(self, instance, value)`      | 设置描述器对象时调用的方法 |
| `object.__get__(self, instance, owner=None)` | 获取描述器对象时调用的方法 |
| `object.__delete__(self, instance)`          | 删除描述器对象时调用的方法 |

当 **描述器类的实例** 出现在一个 **所有者类** 之中的时候描述器才会起作用，即该描述器类的实例必须在 **所有者类** 或它的 **某个上级类** 的 **类字典** 中

> [!tip] 
> 
> 换句话说就是：只有描述器类的实例是类属性时，描述器协议才会起作用
> 

下面代码展示了描述器类的使用。

```python hl:15-18
class Score:

    def __get__(self, instance, owner):
        print("__get__ called: ", instance, owner)

    def __set__(self, instance, value):
        print("__set__ called", instance, value)

    def __delete__(self, instance):
        print("__delete__ called", instance)


class Student:

    # 描述器的实例必须是类属性才能起作用
    math = Score()
    chinese = Score()
    english = Score()
```

很明显，描述器的使用涉及两个类：**描述器类** 和 **所有者类**

> [!tip] 
> 
> 描述器类：实现了描述器协议的类
> 
> 所有者类：持有描述器类实例的类
> 

在上述例子中，`Score` 类就是描述器类；而 `Student` 类就是所有者类

> [!tip] 
> 
> 描述器将属性访问拦截为一个函数调用，该属性的值就是函数的返回值
> 
> 描述器可以 **增强属性**，该属性值可以通过复杂计算得到
> 

### `__set__()`

当通过 **所有者类** 或 **所有者类的实例** 设置描述器属性时，Python 会调用描述器的 `__set__()` 方法。在该方法中，我们必须完成属性的设置工作。

```python hl:5
class Score:

    def __set__(self, instance, value):
        print("__set__ called", instance, value)
        self.__dict__["math"] = value  # 这遇见了第一个问题
```

现在，我们要设置属性。但是，遇见了第一个问题：属性名是什么？因此，在创建描述器实例时，我们应该提供一个描述器的名字。在 Python 3.6 开始，支持使用特殊方法 `__set_name__()` 的方式给描述器添加名字，这个名字就是 **描述器实例的变量名**

```python
class Score:
    def __init__(self):
        print("__init__ called")
        
    def __set__(self, instance, value):
        print("__set__ called", instance, value)
        self.__dict__["math"] = value  # 这遇见了第一个问题

    def __set_name__(self, owner, name):
        print("__set_name__ called", owner, name)

class Student:

    # 描述器的实例必须是类属性才能起作用
    math = Score()
    chinese = Score()
    english = Score()

    def __init__(self, name, math, chinese, english):
        self.name = name
        self.math = math
        self.chinese = chinese
        self.english = english
```

执行上述代码，会输出如下结果

```
__init__ called
__init__ called
__init__ called
__set_name__ called <class '__main__.Student'> math
__set_name__ called <class '__main__.Student'> chinese
__set_name__ called <class '__main__.Student'> english
```

> [!tip] 
> 
> 显然，`__set_name__()` 会在所有的描述器实例初始化完成后在统一被调用。这样，我们就可以在 `__set_name__()` 中记录下当前描述器实例管理的属性
> 

因此，在 `__set__()` 方法中，我们就可以保存属性了

```python hl:4
class Score:

    def __set__(self, instance, value):
        self.__dict__[self.__property_name] = value  # 设置属性

    def __set_name__(self, instance, name):
        self.__property_name = name  # 设置当前描述器管理的属性名

class Student:

    # 描述器的实例必须是类属性才能起作用
    math = Score()
    chinese = Score()
    english = Score()

    def __init__(self, name, math, chinese, english):
        self.name = name
        self.math = math
        self.chinese = chinese
        self.english = english


if __name__ == '__main__':
    stu1 = Student("小明", 102, 110, 90)
    print(stu1.__dict__)  # {'name': '小明'}
```

> [!attention] 
> 
> 请注意，上述代码中，我们将属性值保存在了描述器实例中。由于描述器实例是类属性，被该类的所有实例共享，这些显然是不可以的
> 

注意到，`__set__()` 有一个参数 `instance`，它代表的就是访问这个描述符的对象(可能是类对象，也可能是类实例)。因此，我们需要将属性值放在 `instance.__dict__` 中

```python

class Score:

    def __set__(self, instance, value):
        # self.__dict__[self.__property_name] = value  # 设置属性
        if not isinstance(value, (int, float)):
            raise TypeError('Expected int or float')
        if value < 0 or value > 120:
            raise ValueError('Value must be between 0 and 120')

        instance.__dict__[self.__property_name] = value

    def __set_name__(self, instance, name):
        self.__property_name = name  # 设置当前描述器管理的属性名

class Student:

    # 描述器的实例必须是类属性才能起作用
    math = Score()
    chinese = Score()
    english = Score()

    def __init__(self, name, math, chinese, english):
        self.name = name
        self.math = math
        self.chinese = chinese
        self.english = english


if __name__ == '__main__':
    stu1 = Student("小明", 102, 110, 90)
    
    # {'name': '小明', 'math': 102, 'chinese': 110, 'english': 90}
    print(stu1.__dict__) 
```

### `__get__()`

调用 `__get__(self, instance, owner=None)` 方法以获取 **所有者类的属性**（类属性访问）或 **该类的实例的属性**（实例属性访问）。 

> [!tip] 
> 
> 参数 `owner` 代表所有者类；而参数 `instance` 代表所有者类的实例。
> 
> 

```python

class Score:

    def __set__(self, instance, value):
        # self.__dict__[self.__property_name] = value  # 设置属性
        if not isinstance(value, (int, float)):
            raise TypeError('Expected int or float')
        if value < 0 or value > 120:
            raise ValueError('Value must be between 0 and 120')

        instance.__dict__[self.__property_name] = value

    def __get__(self, instance, owner):
        print("__get__ called")
        return instance.__dict__[self.__property_name]

    def __set_name__(self, instance, name):
        self.__property_name = name  # 设置当前描述器管理的属性名

class Student:

    # 描述器的实例必须是类属性才能起作用
    math = Score()
    chinese = Score()
    english = Score()

    def __init__(self, name, math, chinese, english):
        self.name = name
        self.math = math
        self.chinese = chinese
        self.english = english


if __name__ == '__main__':
    stu1 = Student("小明", 102, 110, 90)
    # {'name': '小明', 'math': 102, 'chinese': 110, 'english': 90}
    print(stu1.__dict__)  
    print(stu1.math)  # 102
```

上述代码执行后的输出结果为

```
{'name': '小明', 'math': 102, 'chinese': 110, 'english': 90}
__get__ called
102
```

从第一行输出可以看出，实例中存在属性 `math`。但是，`stu1.math` 并没有直接访问 `stu1` 的实例属性，而是调用了描述器的 `__get__()` 方法。显然，描述器的优先级要比正常的属性访问要高


> [!attention] 
> 
> 此方法应当返回计算得到的属性值或是引发 `AttributeError` 异常
> 

### `__delete__()` 

当我们需要描述器管理的属性时，Python 会调用描述器的 `__delete__()` 方法

```python
class Score:

    def __set__(self, instance, value):
        # self.__dict__[self.__property_name] = value  # 设置属性
        if not isinstance(value, (int, float)):
            raise TypeError('Expected int or float')
        if value < 0 or value > 120:
            raise ValueError('Value must be between 0 and 120')

        instance.__dict__[self.__property_name] = value

    def __get__(self, instance, owner):
        print("__get__ called")
        if self.__property_name not in instance.__dict__:
            raise AttributeError('No such attribute')
        return instance.__dict__[self.__property_name]

    def __delete__(self, instance):
        print("__delete__ called")
        del instance.__dict__[self.__property_name]

    def __set_name__(self, instance, name):
        self.__property_name = name  # 设置当前描述器管理的属性名

class Student:

    # 描述器的实例必须是类属性才能起作用
    math = Score()
    chinese = Score()
    english = Score()

    def __init__(self, name, math, chinese, english):
        self.name = name
        self.math = math
        self.chinese = chinese
        self.english = english


if __name__ == '__main__':
    stu1 = Student("小明", 102, 110, 90)
    # {'name': '小明', 'math': 102, 'chinese': 110, 'english': 90}
    print(stu1.__dict__)  
    
    del stu1.math
    # {'name': '小明', 'chinese': 110, 'english': 90}
    print(stu1.__dict__)  
```

## 调用描述器

简单讲，**描述器就是具有 “绑定行为” 的 _类属性_**，其属性访问已被描述器协议中的 `__get__()`, `__set__()` 和 `__delete__()` 方法所重载。如果一个对象定义了以上方法中的任意一个，它就被称为描述器。

**属性访问的默认行为是从一个 _对象的字典_ 中获取、设置或删除属性**。例如，Python 在执行 `a.x` 时，会按照如下顺序查找属性 `x`

> [!tip] 
> 
> 属性的默认查找顺序： `a.__dict__['x']` => `type(a).__dict__['x']` => 基类的 `__dict__['x']`
> 
> 注意：实例属性查找范围是不包括元类的
> 

但是，如果找到的值是定义了某个描述器方法的对象，则 Python 可能会重载默认行为并转而唤起描述器方法。这具体发生在优先级链的哪个环节则要根据所 **定义的描述器方法** 及其 **被调用的方式** 来决定。

### 被调用方式

> [!attention] 
> 
> **直接调用**：最简单但最不常见的调用方式是 _用户代码直接发起调用一个描述器方法_: `x.__get__(a)`
> 

直接调用时通过描述器对象调用其描述器协议规定的方式。这种调用描述器的方式通常是不使用的。**最常用的调用方式是 _实例绑定_**

> [!attention] 
> 
> **实例绑定**：如果绑定到一个对象实例，`a.x` 会被转换为调用: `type(a).__dict__['x'].__get__(a, type(a))`
> 

另外还有两种调用方式分别是：**类绑定** 和 **超类绑定**。这两种方式也不常用

> [!tip] 
> 
> 类绑定：如果绑定到一个类，`A.x` 会被转换为调用: `A.__dict__['x'].__get__(None, A)`。
> 

> [!tip] 
> 
> 超绑定：类似 `super(A, a).x` 这样的带点号查找将在 `a.__class__.__mro__` 中搜索紧接在 `A` 之后的基类 `B` 并返回 `B.__dict__['x'].__get__(a, A)`。 如果 `x` 不是描述器，则不加改变地返回它。
> 

### 描述器分类

一个描述器可以定义 `__get__(`)，`__set__()` 和 `__delete__()` 的任意组合；根据不同的组合，描述器被分为两类：**数据描述器** 和 **非数据描述器**

> [!important] 
> 
> 数据描述器：定义了 `__get__()` 和 `__set__()` (或 `__delete__()`) 的描述器就是数据描述器
> 

> [!important] 
> 
> 非数据描述器：自定义了 `__get__()` 的描述器就是非数据描述器
> 

通过 **实例绑定** 调用描述器时，发起描述器调用的优先级取决于定义了哪些描述器方法。如果它没有定义 `__get__()`，则访问属性将返回描述器对象自身，除非对象的实例字典中有相应的属性值。

> [!tip] 
> 
> 如果描述器没有定义 `__get__()`，那么当访问描述器时就会返回描述器自身。如果实例字典中有响应的属性，则返回实例字典中的值
> 

根据描述器的分类，如果描述器定义了 `__set__()` 和/或 `__delete__()`，则它是一个数据描述器；如果两者均未定义，则它是一个非数据描述器。 通常，数据描述器会同时定义 `__get__()` 和 `__set__()`，而非数据描述器则只有 `__get__()` 方法。 定义了 `__get__()` 和 `__set__()` (和/或 `__delete__()`) 的数据描述器总是会重载实例字典中的定义。 与之相对地，非数据描述器则可被实例所重载。

> [!summary] 
> 
> 简单来讲，实例对象进行属性访问时，会按照如下顺序进行属性访问：**数据描述器** => `__dict__` => **非数据描述器** 
> 
> 对于 `a.x` 这样的属性访问。
> 
> + 如果属性查找到 `x` 的值是一个数据描述器，则调用数据描述器方法
> 	+ 如果是获取属性值，则调用 `type(a).__dict__['x'].__get__()`
> 	+ 如果是设置属性值，则调用 `type(a).__dict__['x'].__set__()`
> 	+ 如果是删除属性，则调用 `type(a).__dict__[x].__delete__()`
> 
> + 如果属性查找到 `x` 是属性，即在 `a.__dict__` 中，则 `x` 的值就是 `a.__dict__['x']` 
> + 最后，如果属性查找到 `x` 是非数据描述器，则调用 `type(a).__dict__['x'].__get__()` 方法 
> 

## 描述器应用

下面我们介绍描述器的应用，这里我们将实现 Python 中常用的 $3$ 个装饰器

### staticmethod 原理

`@staticmethod` 本质上是一个非数据描述器，当我们访问被 `@staticmethod` 装饰的函数属性时，会调用描述器的 `__get__()` 方法，该方法只是简单的返回被它装饰的函数

```python
class StaticMethod:

    def __init__(self, func):
        self.__func = func

    def __get__(self, instance, owner):
        return self.__func

class A:

    @StaticMethod
    def foo():  # foo <=> StaticMethod(foo)
        print("static method")

A.foo()
a = A()
a.foo()
```

经过 `StaticMethod` 装饰后的 `foo`  就是 `StaticMethod` 类的实例，并且 `foo` 是类属性；因此符合描述器的要求

通过 `A()` 或 `A` 访问 `foo` 属性时，Python 发现 `foo` 是一个描述器，转而调用 `foo.__get__()` 方法。

由于 `foo.__get__()` 只是简单的返回被 `StaticMethod` 装饰的函数，所以 `A.foo` 或 `A().foo` 本质上就是原来的函数。显然，通过 `A().foo()` 调用时，不会将调用 `foo()` 的实例传递给 `foo` 

### classmethod 原理

`@classmethod` 和 `@staticmethod` 一样都是非数据描述器，只不过 `@classmethod` 将被装饰的函数的第一个参数固定为它所属的类。要固定一个函数的参数，就需要使用 [[Python：装饰器与偏函数#偏函数]]

```python
import functools


class ClassMethod:

    def __init__(self, func):
        self.__func = func

    def __get__(self, instance, owner):
        return functools.partial(self.__func, owner)


class A:



    @ClassMethod
    def bar(cls): # bar <=> ClassMethod(bar)
        print("class method: ", cls)


A.bar()
```

经过 `ClassMethod` 装饰后的 `bar` 现在就是 `ClassMethod` 类的实例，它是类 `A` 属性；因此符合描述器的要求

通过 `A()` 或者 `A` 访问 `bar` 属性时，Python 发现 `bar` 是一个非数据描述器，转而调用 `bar.__get__()` 方法，该方法返回一个偏函数对象，并将 `bar()` 的第一个参数固定为 `bar` 所属的类 

如果不使用偏函数，我们也可以为 `ClassMethod` 定义一个 `__call__()` 方法，并让 `bar.__get__()` 返回 `ClassMethod` 类的实例

```python
class ClassMethod:

    def __init__(self, func):
        self.__func = func

    def __call__(self, *args, **kwargs):
        # 调用被装饰函数，第一个参数传递类
        return self.__func(self.__owner, *args, **kwargs)

    def __get__(self, instance, owner):
        self.__owner = owner  # 记录当前所属的类
        return self
```

### property 原理

`@property` 装饰器本质上是一个数据描述器。因此，需要提供获取属性，设置属性和删除属性的函数。然后，在描述器方法中调用这些函数

```python
import functools


class Property:

    def __init__(self, fget=None, fset=None, fdel=None, doc=None):
        self.fget = fget
        self.fset = fset
        self.fdel = fdel
        self.doc = doc

    def __get__(self, instance, owner):
        print("__get__ called")
        if instance is None:
            return self
        if self.fget is None:
            raise AttributeError("fget must be set")

        return self.fget(instance)

    def __set__(self, instance, value):
        print("__set__ called")
        if self.fset is None:
            raise AttributeError("fset must be set")
        self.fset(instance, value)

    def __delete__(self, instance):
        print("__delete__ called")
        if self.fdel is None:
            raise AttributeError("fdel must be set")
        self.fdel(instance)

    def getter(self, fget):
        return type(self)(fget, self.fset, self.fdel, self.doc)

    def setter(self, fset):
        return type(self)(self.fget, fset, self.fdel, self.doc)

    def deleter(self, fdel):
        return type(self)(self.fget, self.fset, fdel, self.doc)


class A:

    def __init__(self, value):
        self.__value = value

    @Property
    def property(self):
        return self.__value

    @property.setter
    def property(self, value):
        self.__value = value

a = A(5)

print(a.property)
a.property = 10
print(a.property)
```

## 自动生成描述器

回到我们在实现描述器中的例子。当我们需要给 `Student` 类增加一门课程时，就需要在在类中实例化一个描述器对象；于此同时，在 `__set__()` 方法中，硬编码了参数需要的类型。

```python hl:14,31
class Score:

    def __set_name__(self, owner, name):
        self.__property_name = name

    def __get__(self, instance, owner):
        if instance is None:
            return self
        if self.__property_name not in instance.__dict__:
            raise AttributeError(f"{instance.__class__.__name__!r} object does't have property {self.__property_name!r}")
        return instance.__dict__[self.__property_name]

    def __set__(self, instance, value):
        if not isinstance(value, (int, float)):
            raise TypeError('Expected int or float')
        if value < 0 or value > 120:
            raise ValueError('Value must be between 0 and 120')
        instance.__dict__[self.__property_name] = value

    def __delete__(self, instance):
        if self.__property_name not in instance.__dict__:
            raise AttributeError(f"{instance.__class__.__name__!r} object does't have property {self.__property_name!r}")
        del instance.__dict__[self.__property_name]


class Student:

    math = Score()
    chinese = Score()
    english = Score()
    biography = Score()  # 新增的

    def __init__(self, name, age, math, chinese, english, biology):
        self.name = name
        self.age = age
        self.math = math
        self.chinese = chinese
        self.english = english
        self.biology = biology  # 新增的

    def __repr__(self):
        return f"Student(name={self.name}, age={self.age}, math={self.math}, chinese={self.chinese}, english={self.english}, biology={self.biology})"


if __name__ == '__main__':

    student = Student("小明", 28, 97, 102, 98, 76)
    print(student)
``` 

为了解决参数类型硬编码的问题，我们需要修改描述器类的定义

```python hl:17,31-34
class Score:

    def __init__(self,  type_) -> None:
        self.__type = type_

    def __set_name__(self, owner, name):
        self.__property_name = name

    def __get__(self, instance, owner):
        if instance is None:
            return self
        if self.__property_name not in instance.__dict__:
            raise AttributeError(f"{instance.__class__.__name__!r} object does't have property {self.__property_name!r}")
        return instance.__dict__[self.__property_name]

    def __set__(self, instance, value):
        if not isinstance(value, self.__type):
            raise TypeError('Expected int or float')
        if value < 0 or value > 120:
            raise ValueError('Value must be between 0 and 120')
        instance.__dict__[self.__property_name] = value

    def __delete__(self, instance):
        if self.__property_name not in instance.__dict__:
            raise AttributeError(f"{instance.__class__.__name__!r} object does't have property {self.__property_name!r}")
        del instance.__dict__[self.__property_name]


class Student:

    math = Score((int, float))
    chinese = Score((int, float))
    english = Score((int, float))
    biography = Score((int, float))  # 新增的

    def __init__(self, name, age, math, chinese, english, biology):
        self.name = name
        self.age = age
        self.math = math
        self.chinese = chinese
        self.english = english
        self.biology = biology  # 新增的

    def __repr__(self):
        return f"Student(name={self.name}, age={self.age}, math={self.math}, chinese={self.chinese}, english={self.english}, biology={self.biology})"


if __name__ == '__main__':

    student = Student("小明", 28, 97, 102, 98, 76)
    print(student)
```

现在，参数的类型又硬编码在 `Student` 类中了。我们期望可以根据 `Student` 中 `__init__()` 参数的名字和类型注解自动生成描述器对象

想要读取类的 `__init__()` 方法，就需要使用 [[Python：装饰器与偏函数#`inspect` 模块]] 中介绍的方法。

由于此时的描述器对象时在类对象创建完成后才添加的，`__set_name__()` 不在起作用。因此，我们需要再初始化描述器实例的时候提供属性名

下面的代码给出了自动添加描述器的实现

```python
import inspect


class Score:

    def __init__(self,  name, type_) -> None:
        self.__property_name = name
        self.__type = type_

    def __get__(self, instance, owner):
        if instance is None:
            return self
        if self.__property_name not in instance.__dict__:
            raise AttributeError(f"{instance.__class__.__name__!r} object does't have property {self.__property_name!r}")
        return instance.__dict__[self.__property_name]

    def __set__(self, instance, value):
        if not isinstance(value, self.__type):
            raise TypeError('Expected int or float')
        if value < 0 or value > 120:
            raise ValueError('Value must be between 0 and 120')
        instance.__dict__[self.__property_name] = value

    def __delete__(self, instance):
        if self.__property_name not in instance.__dict__:
            raise AttributeError(f"{instance.__class__.__name__!r} object does't have property {self.__property_name!r}")
        del instance.__dict__[self.__property_name]


def generate_score(cls):
    sig = inspect.signature(cls)

    for name, parameter in sig.parameters.items():
        if parameter.annotation is not inspect.Parameter.empty:
            setattr(cls, name, Score(name, parameter.annotation))

    return cls


@generate_score
class Student:  # Student=generate_score(Student)

    def __init__(self, name, age, math:int | float, chinese: int | float, english: int | float, biology: int | float, chemistry: int | float):
        self.name = name
        self.age = age
        self.math = math
        self.chinese = chinese
        self.english = english
        self.biology = biology  # 新增的
        self.chemistry = chemistry

    def __repr__(self):
        return f"Student(name={self.name}, age={self.age}, math={self.math}, chinese={self.chinese}, english={self.english}, biology={self.biology})"


if __name__ == '__main__':
    student = Student("小明", 19, 78, 90, 102, 99, 100)
    print(student)
```
