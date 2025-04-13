# 运算符重载

Python 提供了丰富的运算符，这些运算符可以极大的简化我们的代码。对于我们的自定义类型，如果能够支持 Python 的内建运算符，在使用自定义类型的实例时就会非常方便。下面我们详细介绍 Python 中的运算符重载

> [!seealso] 
> 
> 关于运算符重载的概念，参考 [[C++：友元与运算符重载]] 
> 

## 比较运算符

下表列出的方法被称为 **富比较** 方法，运算符与方法名称的对应关系如下

| 特殊方法          | 描述       |
| :------------ | :------- |
| `x.__eq__(y)` | `x == y` |
| `x.__ne__(y)` | `x != y` |
|               |          |
| `x.__lt__(y)` | `x < y`  |
| `x.__le__(y)` | `x <= y` |
|               |          |
| `x.__gt__(y)` | `x > y`  |
| `x.__ge__(y)` | `x >= y` |

> [!tip] 
> 
> 对于自定义的对象，它们在执行比较运算时就会调用对应的特殊方法
> 

```python
import math


class Point:
    def __init__(self, x, y):
        self.x = x
        self.y = y

    def __repr__(self):
        return f'Point({self.x}, {self.y})'

class Line:

    def __init__(self, p1, p2):
        self.p1 = p1
        self.p2 = p2

    def __repr__(self):
        return f'Line({self.p1}, {self.p2})'

    def length(self):
        return math.sqrt((self.p1.x - self.p2.x)**2 + (self.p1.y - self.p2.y)**2)


    def __eq__(self, other):
        return self.length() == other.length()


line1 = Line(Point(1,2), Point(3,4))
line2 = Line(Point(4,6), Point(7,8))

print(line1 == line2)
```

上述代码只实现了 `__eq__()` 方法，如果想要实现对象的全序我们就要实现上述六个方法。然而，比较运算符可以被分为三组。只需实现三个函数即可即可实现对象的全序比较

> [!tip] 
>  `__lt__`: `<`，可以隐式推断 `>`
>  
>  `__le__`: `<=`，可以隐式推断 `>=`
>  
>  `__eq__`: `==`，可以隐式推断 `!=`
>  

```python
import math


class Point:
    def __init__(self, x, y):
        self.x = x
        self.y = y

    def __repr__(self):
        return f'Point({self.x}, {self.y})'

class Line:

    def __init__(self, p1, p2):
        self.p1 = p1
        self.p2 = p2

    def __repr__(self):
        return f'Line({self.p1}, {self.p2})'

    def length(self):
        return math.sqrt((self.p1.x - self.p2.x)**2 + (self.p1.y - self.p2.y)**2)


    def __eq__(self, other):
        return self.length() == other.length()

    def __le__(self, other):
        return self.length() < other.length()

    def __lt__(self, other):
        return self.length() > other.length()


line1 = Line(Point(1,2), Point(3,4))
line2 = Line(Point(4,6), Point(7,8))

print(line1 != line2)
print(line1 < line2)
print(line1 > line2)
```

根据比较运算符的隐式推断规则，Python 提供了一个装饰器 `total_ordering` 用于构建对象的全序，它定义在标准库 `functools` 中。 该装饰器可以根据 `__lt__()` `__le__()` `__gt__()` `__ge__()` 中的一个方法生成所有的比较方法

```python
import functools
import math


class Point:
    def __init__(self, x, y):
        self.x = x
        self.y = y

    def __repr__(self):
        return f'Point({self.x}, {self.y})'

@functools.total_ordering  # 自动生成全序比较方法
class Line:

    def __init__(self, p1, p2):
        self.p1 = p1
        self.p2 = p2

    def __repr__(self):
        return f'Line({self.p1}, {self.p2})'

    def length(self):
        return math.sqrt((self.p1.x - self.p2.x)**2 + (self.p1.y - self.p2.y)**2)

    def __lt__(self, other):
        return self.length() > other.length()


line1 = Line(Point(1,2), Point(3,4))
line2 = Line(Point(4,6), Point(7,8))

print(line1 == line2)
print(line1 != line2)
print(line1 < line2)
print(line1 <= line2)
print(line1 > line2)
print(line1 >= line2)
```

## 一元运算符

只接受一个操作数的运算符称为一元运算符，Python 中的一元运算符比较少，只有下面几个

| 特殊方法               | 描述      | 备注   |
| :----------------- | :------ | :--- |
| `__neg__(self)`    | `-`     | 负数   |
| `__pos__(self)`    | `+`     | 正数   |
| `__abs__(self)`    | `abs()` | 内置函数 |
| `__invert__(self)` | `~`     | 按位取反 |

```python
class Point:
    def __init__(self, x, y):
        self.x = x
        self.y = y

    def __neg__(self):  # 负号 -
        return Point(-self.x, -self.y)

    def __pos__(self):  # 正号 +
        return Point(self.x, self.y)

    def __invert__(self):  # 取反 ~
        return Point(~self.x, ~self.y)

    def __repr__(self):
        return f'Point({self.x}, {self.y})'


p = Point(1, 2)

print(-p)
print(+p)
print(~p)
```

## 二元运算符

Python 中的二元运算符有很多，这些二元运算符都有对应的特殊方法。为类定义这些方法可以简化实例的使用。下表列出了 Python 中的二元运算符与特殊方法的对应表

| 特殊方法                | 描述                                  |          |
| :------------------ | :---------------------------------- | -------- |
| `x.__add__(y)`      | `x + y`                             | 加法       |
| `x.__sub__(y)`      | `x - y`                             | 减法       |
| `x.__mul__(y)`      | `x * y`                             | 乘法       |
| `x.__matmul__(y)`   | `x @ y`                             | 矩阵乘法     |
| `x.__truediv__(y)`  | `x / y`                             | 真实除法     |
| `x.__floordiv__(y)` | `x // y`                            | 地板除法     |
| `x.__mod__(y)`      | `x % y`                             | 模运算      |
| `x.__divmod__(y)`   | `divmod(x, y)`                      | `divmod` |
| `x.__pow__(y, [z])` | `pow(x, y)` `x ** y` `pow(x, y, z)` | 幂运算      |
| `x.__lshift__(y)`   | `x << y`                            | 左移       |
| `x.__rshift__(y)`   | `x >> y`                            | 右移       |
| `x.__and__(y)`      | `x & y`                             | 位与       |
| `x.__xor__(y)`      | `x ^ y`                             | 异或       |
| `x.__or__(y)`       | x \| y                              | 位或       |

下面的代码展示了一个 `Complex` 类的实现代码：这里我们实现了复数的四则运算。

```python
class Complex:

    def __init__(self, real, imaginary):
        self.__real = real
        self.__imaginary = imaginary


    @property
    def real(self):
        return self.__real

    @property
    def imaginary(self):
        return self.__imaginary

    def __repr__(self):
        return f"Complex({self.real}, {self.imaginary})"

    def __add__(self, other):
        """复数加法
        (a + bi) + (c + di) = (a + c) + (b + d)i
        """
        if isinstance(other, (int, float)):
            other = Complex(real=other, imaginary=0)

        return Complex(self.real + other.real, self.imaginary + other.imaginary)

    def __sub__(self, other):
        """复数减法
        (a + bi) - (c + di) = (a - c) - (b + di)
        """
        if isinstance(other, (int, float)):
            other = Complex(real=other, imaginary=0)

        return Complex(self.real - other.real, self.imaginary - other.imaginary)

    def __mul__(self, other):
        """复数乘法
        (a + bi) * (c + di) = (ac - bd) + (ad + bc)i
        """
        if isinstance(other, (int, float)):
            other = Complex(real=other, imaginary=0)

        real = self.real * other.real - self.imaginary * other.imaginary
        imaginary = self.real * other.imaginary + self.imaginary * other.real
        return Complex(real, imaginary)

    def __truediv__(self, other):
        """复数除法
        (a + bi) / (c + di) = ((a + bi)*(c - di)) / ((c + di) * (c - di))
                            = ((ac + bd) + (bc - ad)i) / (c ** 2 + d ** 2)
                            = ac + bc / (c ** 2 + d ** 2) + (bc - ad)i / (c ** 2 + d ** 2)
        """
        if isinstance(other, (int, float)):
            other = Complex(real=other, imaginary=0)

        real = (self.real * other.real + self.imaginary * other.imaginary) / (other.real ** 2 + other.imaginary ** 2)
        imaginary = (self.imaginary * other.real - self.real * other.imaginary) / (other.real ** 2 + other.imaginary ** 2)

        return Complex(real, imaginary)


    def __invert__(self):
        """获取共轭复数"""
        return Complex(self.real, -self.imaginary)


c1 = Complex(1, 2)
c2 = Complex(2, 3)

print(c1 + c2)
print(c1 - c2)
print(c1 * c2)
print(c1 / c2)

print(c1 + 2)
print(c1 - 2)
print(c1 * 2)
print(c1 / 2)
```

请注意：浮点数或整数与 `Complex` 进行算术运算时，只能作为右操作数。例如

```python
print(2 + c1)  # TypeError: 不支持 int 和 Complex 之间的 +
```

> [!attention] 
> 
> 在 Python 中，`2 + c1` 等价于 `(2).__add__(c1)`。由于内置类型 `int` 不支持与自定义类型 `Complex` 进行加法计算，从而抛出 `TypeError`
> 

在 Python 中，为了运算符重载只能在类定义中进行，因此运算符从在函数的第一个参数一定是类实例本身。为了让 `c1 + 2` 与 `2 + c1` 都成立，即实现算术运算符的 **交换**；Python 提供了另外一些特殊方法。这些方法被调用是有前提

> [!important] 
> 
> 这里，我们以 `__add__()` 为例。如果 `x.__add__(y)` 未实现，即返回 [`NotImplemented`](https://docs.python.org/zh-cn/3.13/library/constants.html#NotImplemented)，Python 会尝试调用 `y.__radd__(x)` 方法。如果 `y.__radd__(x)` 也未实现，则抛出 `TypeError`
> 
> `NotImplemented` 是 Python 中的常量，其类型是 `NotImplementedType`，这个常量只能用于二元运算符方法的返回值，表用该方法未实现
> 

综上所述，当上述运算符未实现时，Python 会转而调用下表列出的运算符函数

| 特殊方法                       | 描述                                   |
| :------------------------- | :----------------------------------- |
| `y.__radd__(x)`            | 执行 `x + y`，在 `x` 未实现 `__add__()` 时调用 |
| `y.__rsub__(x)`            | 执行 `x - y`，在 `x` 未实现 `__sub__()` 时调用 |
| `y.__rmul__(x)`            | `x * y`                              |
| `y.__rmatmul__(x)`         | `x @ y`                              |
| `y.__rtruediv__(x)`        | `x / y`                              |
| `y.__rfloordiv__(x)`       | `x // y`                             |
| `y.__rmod__(x)`            | `x % y`                              |
| `y.__rdivmod__(x)`         | `divmod(x, y)`                       |
| `y.__rpow__(self, x[, z])` | `pow(x, y)` `x ** y` `pow(x, y, z)`  |
| `y.__rlshift__(x)`         | `x << y`                             |
| `y.__rrshift__(x)`         | `x >> y`                             |
| `y.__rand__(x)`            | `x & y`                              |
| `y.__rxor__(x)`            | `x ^ y`                              |
| `y.__ror__(x)`             | x \| y                               |

```python
class Complex:

    def __init__(self, real, imaginary):
        self.__real = real
        self.__imaginary = imaginary


    @property
    def real(self):
        return self.__real

    @property
    def imaginary(self):
        return self.__imaginary

    def __repr__(self):
        return f"Complex({self.real}, {self.imaginary})"

    def __add__(self, other):
        """复数加法
        (a + bi) + (c + di) = (a + c) + (b + d)i
        """
        if isinstance(other, (int, float)):
            other = Complex(real=other, imaginary=0)

        return Complex(self.real + other.real, self.imaginary + other.imaginary)

    def __radd__(self, other):  # 交换律
        return self + other


    def __sub__(self, other):
        """复数减法
        (a + bi) - (c + di) = (a - c) - (b + di)
        """
        if isinstance(other, (int, float)):
            other = Complex(real=other, imaginary=0)

        return Complex(self.real - other.real, self.imaginary - other.imaginary)

    def __rsub__(self, other):
        """浮点数/整数 - 复数
        """
        return Complex(other - self.real, -self.imaginary)

    def __mul__(self, other):
        """复数乘法
        (a + bi) * (c + di) = (ac - bd) + (ad + bc)i
        """
        if isinstance(other, (int, float)):
            other = Complex(real=other, imaginary=0)

        real = self.real * other.real - self.imaginary * other.imaginary
        imaginary = self.real * other.imaginary + self.imaginary * other.real
        return Complex(real, imaginary)

    def __rmul__(self, other):
        return self * other

    def __truediv__(self, other):
        """复数除法
        (a + bi) / (c + di) = ((a + bi)*(c - di)) / ((c + di) * (c - di))
                            = ((ac + bd) + (bc - ad)i) / (c ** 2 + d ** 2)
                            = ac + bc / (c ** 2 + d ** 2) + (bc - ad)i / (c ** 2 + d ** 2)
        """
        if isinstance(other, (int, float)):
            other = Complex(real=other, imaginary=0)

        real = (self.real * other.real + self.imaginary * other.imaginary) / (other.real ** 2 + other.imaginary ** 2)
        imaginary = (self.imaginary * other.real - self.real * other.imaginary) / (other.real ** 2 + other.imaginary ** 2)

        return Complex(real, imaginary)

    def __rtruediv__(self, other):
        """ 复数除法
        a / (b + ci) = a(b - ci) / (b ** 2 + c ** 2)
                    = ab / (b ** 2 + c ** 2) + （-aci） / (b ** 2 + c ** 2)
        """

        return Complex(self.real * other / (self.real ** 2 + self.imaginary ** 2), -other * self.imaginary / (self.real ** 2 + self.imaginary ** 2))

    def __invert__(self):
        """获取共轭复数"""
        return Complex(self.real, -self.imaginary)


c1 = Complex(1, 2)
c2 = Complex(2, 3)

print(c1 + c2)
print(c1 - c2)
print(c1 * c2)
print(c1 / c2)

print(c1 + 2)
print(c1 - 2)
print(c1 * 2)
print(c1 / 2)

print(2 + c1)
print(2 - c1)
print(2 * c1)
print(2 / c1)
```

对于 `+`、 `-`、`*`、`@`、`/`、`//`、 `%`、 `** ` 等可以和赋值运算符组合使用的运算符，Python 也提供了对应的特殊方法

| 特殊方法                              | 描述    | 备注                          |
| :-------------------------------- | :---- | :-------------------------- |
| `__iadd__(_self, other)`          | `+=`  | `x +=y` 等价于 `x.__iadd__(y)` |
| `__isub__(_self, other)`          | `-=`  |                             |
| `__imul__(self, other)`           | `*=`  |                             |
| `__imatmul__(self, other)`        | `@=`  | 矩阵乘法                        |
| `__itruediv__(self, other)`       | `/=`  |                             |
| `__ifloordiv__(self, other)`      | `//=` |                             |
| `__imod__(self, other)`           | `%=`  |                             |
| `__ipow__(self, other[, modulo])` | `**=` |                             |
| `__ilshift__(self, other)`        | `<<=` | 左移                          |
| `__irshift__(self, other)`        | `>>=` | 右移                          |
| `__iand__(self, other)`           | `&=`  | 按位与                         |
| `__ixor__(self, other)`           | `^=`  | 按位异或                        |
| `__ior__(self, other)`            | \|=   | 按位或                         |

> [!attention] 
> 
> 对于这些增强运算符，如果未定义其对应的方法，就会使用其对应的运算符函数。例如，`__isub__` 未定义时，`-=` 就会触发 `__sub__`
> 
