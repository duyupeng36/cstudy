# 描述器

仅当一个包含以下方法之一的 **类**（称为 **_描述器_ 类**）的 _实例_ 出现于另一个 **类(所有者)** 中的时候才会起作用（该描述器必须在所有者类或其某个上级类的字典中）。在以下示例中，**“属性” 指的是名称为所有者类 `__dict__` 中的特征属性的键名的属性**

| 描述器方法                                        |
| :------------------------------------------- |
| `object.__get__(self, instance, owner=None)` |
| `object.__set__(self, instance, value)`      |
| `object.__delete__(self, instance)`          |

> [!tip]
> 
> 描述器类：定义 `__get__` `__set__` `__delete__` 三者之一的类
> 
> 描述器：**描述器类的实例是另一个类的类属性**
> 

> [!tip] `self` `instance` `owner` 三种代表什么？
> 
> `self` 代表了描述器类的实例
> 
> `instance` 代表了描述器所有者类的实例
> 
> `owner` 代表了描述器的所有者类
> 

很明显，**描述器涉及两个类**，思考下面程序的执行流程

```python
class A:  # 描述器类
    def __init__(self):
        print("A __init__")
        self.a1 = "a1"

    def __get__(self, instance, owner):
        print(f"A.__get__({self} {instance} {owner})")
        # owner 属主类
        return self


class B: # 属主类
    x = A()  # 描述器实例
    # 通过 B 或者 B() 访问 x，转而调用了 A().__get__() 方法
    def __init__(self):
        print("B __init__")


print("-" * 30)

B.x

print("=" * 30)

b = B()
b.x
```

上述代码执行时的输出结果为

```
A __init__
------------------------------
A.__get__(<__main__.A object at 0x7f92e2ca9a90> None <class '__main__.B'>)   # B.x 
==============================
B __init__
A.__get__(<__main__.A object at 0x7f92e2ca9a90> <__main__.B object at 0x7f92e2ca9ac0> <class '__main__.B'>)  # b.x
```

> [!important] 
> 
> 描述器将属性访问行为拦截了下来，转而调用一个函数。该属性的值就是这个函数的返回值
> 
> 也就是说：描述器可以 **增强属性**，是的该属性值可以通过复杂计算得到
> 

> [!attention] **描述器类的实例** 必须是 **类属性** 才可以使用描述器协议
> 
> 只有 **类属性** 是 **描述器类的实例** 才可以触发描述器协议
> 

总的说来，**描述器就是具有 “绑定行为” 的 _类属性_**，其属性访问已被描述器协议中的 `__get__()`, `__set__()` 和 `__delete__()` 方法所重载:

> [!tip] 描述器
>  如果一个类中定义了 `__get__()` `__set__()` 和 `__delete__()` 中的任意一个方法，这个类就是一个描述器

描述器分为 **非数据描述器** 和 **数据描述器**，它们之间的区别只是描述器协议实现的差异

> [!tip] 非数据描述器
> 
> 只实现了 `__get__()` 方法

> [!tip] 数据描述器
> 
> 实现了 `__get__()` 并且还实现了 `__set__()` 或 `__delete__()` 
> 

## 属性访问

> [!tip] 属性访问的默认行为
> 
> 从一个对象的 `__dict__` 中获取、设置或删除属性

例如，`a.x`  的查找顺序会从 `a.__dict__['x']` 开始，然后是 `type(a).__dict__['x']`，接下来依次查找 `type(a)` 的上级基类，**_不包括元类_**

> [!tip] 查找到属性的值是 _描述器对象_ 时，Python **会重载默认行为并转而 _发起描述器方法的调用_**
> 
> 这具体发生在属性查询链的哪个环节，则要根据所 _定义的描述器方法_ 及其 _被调用的方式_ 来决定

描述器发起调用的 _开始点_ 是一个绑定 `a.x`。参数的组合方式依 `a` 而定

> [!tip] 直接调用
> 
> 最简单但最不常见的调用方式是 _用户代码直接发起调用一个描述器方法_: `x.__get__(a)`
> 

> [!tip] 实例绑定
> 
> 如果绑定到一个对象实例，`a.x` 会被转换为调用: `type(a).__dict__['x'].__get__(a, type(a))`

> [!tip] 类绑定
> 
> 如果绑定到一个类，`A.x` 会被转换为调用: `A.__dict__['x'].__get__(None, A)`。
> 

> [!tip] 超绑定
> 
> 类似 `super(A, a).x` 这样的带点号查找将在 `a.__class__.__mro__` 中搜索紧接在 `A` 之后的基类 `B` 并返回 `B.__dict__['x'].__get__(a, A)`。 如果 `x` 不是描述器，则不加改变地返回它。
> 

### 描述器的优先级

对于 **实例绑定**，发起描述器调用的优先级 **取决于定义了哪些描述器方法**

> [!tip] 
> 
> 一个描述器类可以定义 `__get__()`, `__set__()` 和 `__delete__()` 的任意组合
> 

> [!tip] 描述器类没有定义 `__get__()`，访问属性将 _返回描述器对象自身_
> 
> 如果它没有定义 `__get__()` ，则访问属性将返回描述器对象自身，_除非对象的实例字典中有相应的属性值_
> 

> [!tip] 数据描述器会重载实例 `__dict__` 中的定义
> 
>  定义了 `__get__()` 和 `__set__()` (或 `__delete__()`) 的 **数据描述器总是会重载实例字典中的定义**。 与之相对地，非数据描述器则可被实例所重载
> 

> [!tip] 也就是说：数据描述器 > `__dict__` > 非数据描述器 。对于 `a.x`
> 
> 如果属性查找到 `x` 的值是一个数据描述器，则调用描述器方法
> 
> 如果属性查找到 `x` 是属性，即在 `a.__dict__` 中，则 `x` 的值就是 `a.__dict__['x']` 
> 
>最后，如果属性查找到 `x` 是非数据描述器，则调用 `type(a).__dict__['x'].__get__()` 方法
> 

## staticmethod 原理

`staticmethod` 核心就是一个非数据描述器，当我们访问被 `staticmethod` 装饰的属性时，会触发 `__get__` 方法，返回它装饰的函数即可

```python
class StaticMethod:

    def __init__(self, fn):
        self.__fn = fn

    def __get__(self, instance, owner):
        return self.__fn

class A:

    @StaticMethod
    def foo():  # foo = StaticMethod(fn)
        print("static method: ......")
```

> [!tip] `A.foo` 现在就是 `StaticMethod` 类的实例，它是类属性；因此符合描述器的要求
> 
> 同通过 `A()` 或 `A` 访问 `foo` 属性时，Python 发现 `foo` 是一个描述器，转而调用 `foo.__get__` 方法，`A.foo` 或者 `A().foo` 的值就是 `foo.__get__()` 的返回值
> 

## classmethod 原理

`classmethod` 和 `staticmethod` 一样都是描述器，只不过 `classmethod` 将被装饰的函数的第一个参数固定为它所属的类。

```python
class ClassMethod:
    def __init__(self, fn):
        self.__fn = fn

    def __call__(self, *args, **kwargs):
        return self.__fn(self.__owner, *args, **kwargs)

    def __get__(self, instance, owner):
        self.__owner = owner
        print("get:......")
        # return partial(self.__fn, owner)  # 返回一个新函数，固定了 self.__fn 的第一参数为 owner
        return self

class A:    
  
    @ClassMethod  
    def bar(cls):  # bar = ClassMethod(fn)  
        print("clas method: ......", cls)
```

> [!tip] `A.bar` 现在就是 `ClassMethod` 类的实例，它是类属性；因此符合描述器的要求
> 
> 通过 `A()` 或者 `A` 访问 `bar` 属性时，Python 发现 `bar` 是一个描述器，转而调用 `bar.__get__` 方法，该方法返回来 `ClassMethod` 实例自身
> 
> 由于 `ClassMethod` 实例的类中自身定义了 `__call__` 方法，因此它是一个可调用对象。所以 `bar()` 等价于 `ClassMehthod(bar).__call__()`，在 `__call__` 中，我们将 `A` 类注入到了原始 `bar` 函数的第一个参数

## 示例

假设现在我们正在做一个 **成绩管理系统**，刚刚学习完面向对象的我们很自然的编写出来如下代码

```python
class Student(object):
    def __init__(self, name, math, chinese, english):
        self.name = name
        self.math = math
        self.chinese = chinese
        self.english = english

    def __repr__(self):
        return f"<Student {self.name}: math={self.math}, chinese={self.chinese},eng={self.english}>"
```

然而，该代码没有对数据有效性进行任何验证，导致数据错误风险

```python
stu = Student("小米", 100, 120, 100)
stu.math = -20
print(stu)  # <Student 小米: math=-20, chinese=120,eng=100>
```

很明显，**成绩是不能为负数的**。因此，自然而然地想到给每个成绩加上数据合法性检查

```python
class Student(object):
    def __init__(self, name, math, chinese, english):
        self.name = name

        if 0 <= math <= 100:
            self.math = math
        else:
            raise ValueError('Math must be between 0 and 100')

        if 0 <= chinese <= 100:
            self.chinese = chinese
        else:
            raise ValueError('Chinese must be between 0 and 100')

        if 0 <= english <= 100:
            self.english = english
        else:
            raise ValueError('English must be between 0 and 100')

    def __repr__(self):
        return f"<Student {self.name}: math={self.math}, chinese={self.chinese},eng={self.english}>"
```

实例初始的逻辑就变得非常复杂，很影响代码的可读性。立马我们想起了 `propety` 装饰器。因此，我们将代码重构为

```python
class Student:
    def __init__(self, name, math, chinese, english):
        self.name = name
        self._math = math
        self._chinese = chinese
        self._english = english

    @property
    def math(self):
        return self._math

    @math.setter
    def math(self, value):
        if 0 <= value <= 100:
            self._math = value
        else:
            raise ValueError("math must be in [0, 100]")

    @property
    def chinese(self):
        return self._chinese

    @chinese.setter
    def chinese(self, value):
        if 0 <= value <= 100:
            self._chinese = value
        else:
            raise ValueError("chinese must be in [0, 100]")

    @property
    def english(self):
        return self._english

    @english.setter
    def english(self, value):
        if 0 <= value <= 100:
            self._english = value
        else:
            raise ValueError("english must be in [0, 100]")

    def __repr__(self):
        return f"<Student: {self.name}, math: {self.math}, chinese: {self.chinese}, english: {self.english}>"


if __name__ == "__main__":
    stu = Student("哈哈哈", -20, 100, 200)
    print(stu)
```

这样依旧可以完成检验，并且代码的可读性得到了增强。然而，`math, chinese, english` 的检查逻辑是重复的，而且没增加一门课程序，就需要重复编写这样的逻辑，导致代码冗余。因此，这里我们可以使用先前学习的 **描述器**，抽象出一个 **分数类**

```python
class Score:
    def __init__(self, default=0):
        self._score = default
    
    def __set__(self, instance, value):
        if not isinstance(value, (int, float)):
            raise TypeError("Score must be integer or decimal")
        
        if not 0 <= value <= 100:
            raise ValueError("Score must be in range(0, 100)")
        
        self._score = value
    
    def __get__(self, instance, owner):
        return self._score
    
    def __delete__(self, instance):
        del self._score


class Student:
    
    math = Score()
    chinese = Score()
    english = Score()
    
    def __init__(self, name, math, chinese, english):
        self.name = name
        self.math = math
        self.chinese = chinese
        self.english = english

    def __repr__(self):
        return f"<Student: {self.name}, math: {self.math}, chinese: {self.chinese}, english: {self.english}>"


if __name__ == "__main__":
    stu = Student("哈哈哈", 20, 100, 100)
    print(stu)
```

这样，所有的相同的判断逻辑不用重复在写一次了

就是描述符给我们带来的编码上的便利，它在实现 **保护属性不受修改和属性类型检查** 的基本功能，同时有大大 **提高代码的复用率**

### 描述器作为属性访问的代理

上述示例最后版本的代码使用了描述器。但是，这个版本存在一个 `bug`

```python
stu1 = Student("stu1", 20, 100, 100)
print("stu1: ", stu1)
stu2 = Student("stu2", 100, 98, 100)
print("stu2: ", stu2)
print("-----------------------------------------------------------------")
print("stu1: ", stu1)
print("stu2: ", stu2)
```

这段代码的输出结果为

```
@duyup ➜ pycode python .\test.py
stu1:  <Student: stu1, math: 20, chinese: 100, english: 100>
stu2:  <Student: stu2, math: 100, chinese: 98, english: 100>
-----------------------------------------------------------------
stu1:  <Student: stu1, math: 100, chinese: 98, english: 100>
stu2:  <Student: stu2, math: 100, chinese: 98, english: 100>
```

> [!bug] 
> 
> 导致上述 BUG 的原因：原因在于 `math, chinese, english` 是 `Student` 类中的 **类属性**，导致 `std2` 和 `std1` 在访问 `math`，`chinese`，`english` 这三个变量时，其实都是访问类变量
 
**描述器的机制**，只是 **抢占了访问顺序，而具体的逻辑却要因地制宜，视情况而定**。如果需要把 `math, chinese, english` 这三个类属性变为实例属性，使之相互隔离，就需要通过 `__set__` 和 `__get__` 的 `instance` 参数，它代表的访问描述器的实例

```python hl:3-6
class Score:
    
    def __set_name__(self, owner, name):
	    # owner：宿主类
	    # name: Score 实例在宿主类中属性名
        self.property_name = name
    
    def __set__(self, instance, value):
        if not isinstance(value, (int, float)):
            raise TypeError("Score must be integer or decimal")
        
        if not 0 <= value <= 100:
            raise ValueError("Score must be in range(0, 100)")
        
        # 通过描述器，将属性值放在实例字典中
        instance.__dict__[self.property_name] = value
        
    def __get__(self, instance, owner):
        # 通过描述器，从实例字典中读取属性值
        return  instance.__dict__[self.property_name]
    
    def __delete__(self, instance):
        # 删除
        instance.__dict__.pop(self.property_name)

class Student:
    
    math = Score()
    chinese = Score()
    english = Score()
    
    def __init__(self, name, math, chinese, english):
        self.name = name
        self.math = math
        self.chinese = chinese
        self.english = english

    def __repr__(self):
        return f"<Student: {self.name}, math: {self.math}, chinese: {self.chinese}, english: {self.english}>"


if __name__ == "__main__":
    stu1 = Student("stu1", 20, 100, 100)
    print("stu1: ", stu1)
    stu2 = Student("stu2", 100, 98, 100)
    print("stu2: ", stu2)
    print("-----------------------------------------------------------------")
    print("stu1: ", stu1)
    print("stu2: ", stu2)
```

修改后的脚本运行就不再冲突了

```
@duyup ➜ pycode python .\test.py
stu1:  <Student: stu1, math: 20, chinese: 100, english: 100>
stu2:  <Student: stu2, math: 100, chinese: 98, english: 100>
-----------------------------------------------------------------
stu1:  <Student: stu1, math: 20, chinese: 100, english: 100>
stu2:  <Student: stu2, math: 100, chinese: 98, english: 100>
```

> [!attention] 
> 
> 在上述代码的第 $3 \sim 4$ 行中，我引入了一个新的方法 `__set_name__(self, owner, name)` 
> 
> 这个方法是 Python 3.6 版本加入的，参数 `owner` 是宿主类，而参数 `name` 就是该描述器实例在数组类中的变量名
> 

## 参数类型检查

回到我们示例中，在 `Student` 类中，如果每增加一门课程，就需要在在类中实例化一个描述器对象。比如，新增了一门生物课程

```python
class Student:
    math = Score()
    chinese = Score()
    english = Score()
    biology = Score()  # 新增

    def __init__(self, name, math, chinese, english, biology):
        self.name = name
        self.math = math
        self.chinese = chinese
        self.english = english
        self.biology = biology

    def __repr__(self):
        return f"<Student: {self.name}, math: {self.math}, chinese: {self.chinese}, english: {self.english}>"
```

同时，在 `Score` 类中，我们对参数类型的要求直接硬编码在 `__set__` 中，现在我们期望可以执行指定，参数类型，例如

```python
class Student:
    math = Score("math", (float,  int))
    chinese = Score("chinese", (int, float))
    english = Score("english", (int, float))
    biology = Score("biology", (int, float)) 

    def __init__(self, name, math:int|float, chinese:int|float, english:int|float, biology:int|float):
        self.name = name
        self.math = math
        self.chinese = chinese
        self.english = english
        self.biology = biology

    def __repr__(self):
        return f"<Student: {self.name}, math: {self.math}, chinese: {self.chinese}, english: {self.english}, biology: {self.biology}>"
```

这样硬编码在了 `Student` 中，我们期望可以根据 `Student` 中 `__init__()` 参数的名字和类型注解自动生成

```python
import inspect


class TypeCheck:

    def __init__(self, property_name, typ):
        self.property_name = property_name
        self.typ = typ

    def __set__(self, instance, value):
        if not isinstance(value, self.typ):
            raise TypeError(f"score type must be {self.typ}, not {type(value)}")

        if not 0 <= value <= 100:
            raise ValueError(f"score value must be between 0 and 100")

        # 通过描述器，将属性值放在实例字典中
        instance.__dict__[self.property_name] = value

    def __get__(self, instance, owner):
        # 通过描述器，从实例字典中读取属性值
        return instance.__dict__[self.property_name]

    def __delete__(self, instance):
        # 删除
        instance.__dict__.pop(self.property_name)


def generate_score(cls):
    sig = inspect.signature(cls)

    for name, param in sig.parameters.items():
        if param.annotation is not param.empty:
            setattr(cls, name, TypeCheck(name, param.annotation))

    return cls


@generate_score
class Student:

    def __init__(self, name, math:int|float, chinese:int|float, english:int|float, biology:int|float):
        self.name = name
        self.math = math
        self.chinese = chinese
        self.english = english
        self.biology = biology

    def __repr__(self):
        return f"<Student: {self.name}, math: {self.math}, chinese: {self.chinese}, english: {self.english}, biology: {self.biology}>"


if __name__ == "__main__":
    stu1 = Student("stu1", 12, 100, 100, 20)
    print("stu1: ", stu1)
    stu2 = Student("stu2", 100, 98, 100, 20)
    print("stu2: ", stu2)
    print("-----------------------------------------------------------------")
    print("stu1: ", stu1)
    print("stu2: ", stu2)
```
