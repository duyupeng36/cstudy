# 继承

类表示现实世界或者应用程序中的一种概念或思想。这些概念或思想都不是孤立存在的，都有与之共存或相关的概念。

> [!example] 
> 
> 例如，当我们尝试解释汽车是什么时，很快就会聊到轮子、引擎、司机、行人等概念
> 

在程序设计中，我们使用类表示概念，因此问题转换为如何用类表示这些概念之间的关系。Python 与 C++ 一样，提供了 **派生类** 的概念和语言机制来表示**类的层次关系**，也就是表达**类之间的共性**

例如，**圆形的概念和三角形的概念它们是相关的，都表示形状**；即，它们具有 **形状** 这一公共概念。因此，我们明确定义类 `Circle` 和 类 `Triangle` 共同有用类 `Shape`。本例中类 `Shape` 称为 **基类**；类 `Circle` 和 `Triangle` 称为 **派生类**。如果在程序中表示 **圆形** 和 **三角形**，而不涉及 **形状** 的概念，那么就会遗漏某些重要的东西


下图展示了继承关系

![Pasted image 20240622145609|600|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755786177753-9f0977c0b6dc45f595d6bf00b74a3720.png)


兔子和羊属于食草动物类，狮子和豹属于食肉动物类。食草动物和食肉动物又是属于动物类。所以继承需要符合的关系是：`is-a`，**父类更通用**，**子类更具体**。

虽然食草动物和食肉动物都是属于动物，但是两者的属性和行为上有差别，所以 **子类会具有父类的一般特性也会具有自身的特性**

## 派生类

与 C++ 一样，Python 中类也是可以被继承的

> [!tip] 
> 
> 继承用于描述概念之间的层次关系。
> 
> ![Pasted image 20250412151429|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755786177754-0a1a07b92dab4674961a5ca82aa39800.png)
> 
> + 第一步，由下到上进行分析, 首先由对象总结抽象出类, 在由类总结抽象出父类
> + 第二步，由分析出的继承关心定义类完成继承
> 

```python
class Animal:  # 动物基类
    pass


class People(Animal):  # 人
    pass


class Pig(Animal):  # 猪
    pass


class Monkey(Animal):  # 猴子
    pass
```

> [!tip] 
> 
> `Student` 类由 `Person` 类派生而来，称为 `Person` 类为 **基类**，而 `Student` 类为 **派生类**
> 

基于上图我们可以看出类与类之间的 **继承指的是什么 `is a`  的关系**（比如人类，猪类，猴类都是动物类）。**子类可以继承父类所有的属性**，因而继承可以用来解决类与类之间的代码重用性问题

```python
class People:
    def __init__(self,name, age, gender):
        self.name = name
        self.age = age
        self.gender = gender


class Student(People):
    def __init__(self, name, age, gender, no):
        People.__init__(self, name, age, gender)  # 调用父类的方法
        self.no = no

    def choose_class(self, class_obj):
        print("学生 %s 选择课程 %s" % (self.name, class_obj))


student = Student("dyy", 18, "男", 1712211212)
student.choose_class("应用化学")


class Teacher(People):
    def __init__(self, name, age, gender, salary, level):
        self.salary = salary
        self.level = level
        People.__init__(self, name, age, gender)  # 调用父类的方法

    def scored(self):
        print("老师 %s 正在为学生打分" % self.name)


teacher = Teacher("ddd", 20, "男", 30000, 10)
teacher.scored()
```

**派生类定义的执行过程与基类相同**。 当构造类对象时，基类会被记住，类对象的属性 `__base__` 记录了该类对象的所有基类。

当一个类派生出另一个类，这个被派生的类会获取其基类的所有属性。然而，继承并只是简单的代码复制

在 Python 中，每个类都有一个 `__base__` 属性，它记录了该类的所有基类。当通过对象查找属性时，就会按照某种属性查找规则在 `__base__` 中记录的基类中查找属性

### 覆盖基类的方法

**派生类可能会重写其基类的方法**。 因为方法在调用同一对象的其他方法时没有特殊权限，所以基类方法在尝试调用调用同一基类中定义的另一方法时，可能实际上调用是该基类的派生类中定义的方法。

> [!warning] 
> 
> 对 C++ 程序员的提示：**Python 中所有的方法实际上都是 `virtual` 方法**，参考 [[C++：多态#虚函数]]。
> 
> 所以，在基类中定义的方法调用另一个方法时，可能调用到其派生类中定义的方法
> 

下列代码就说明了上述警告

```python
class Person:

    def __init__(self, name, age):
	    print(type(self))  # <class '__main__.Student'>
        self.name = name
        self.age = age
        self.show()  # 如果实例化派生类对象，这将会调用派生类的 show() 方法

    def show(self):
        print(self.name, self.age)


class Student(Person):
    def show(self):  # 覆盖了基类的 show() 方法
        print("I am a student!", "my name is " + self.name, "my age is " + str(self.age))


student = Student("John", 22)  # 1.派生类实例化时，会调用基类的 __init__ 方法
```

> [!tip] 
> 
> 派生类实例化时，会调用基类的 `__init__()` 方法，此时第一个参数传递的是派生类对象的引用
> 

为了不让派生类覆盖基类的方法，我们可以将基类的方法定义为 `__method()`。例如，

```python
class Person:

    def __init__(self, name, age):
        self.name = name
        self.age = age
        self.__show()
        print(type(self))  # <class '__main__.Student'>

    def __show(self):  # 在 Person 中，被替换为 _Person__show
        print(self.name, self.age)


class Student(Person):
    def __show(self): # 在 Student 中，被替换为 _Student__show
        print("I am a student!", "my name is " + self.name, "my age is " + str(self.age))

student = Student("John", 22)
```

> [!tip] 
> 
> 根据 Python 对 `__name` 的改写规则，在 `Person` 中的 `__name` 被替换为 `_Person__name`。而在 `Student` 中的 `__name` 被替换为 `_Student__name`
> 

### supper()：调用基类的方法

如果需要 **在基类的方法上做扩展**，往往需要 **在派生类的方法中调用基类的方法**。可以 **使用基类类名** 直接调用的方式可以调用基类的方法，这种方法通常是不建议的

```python hl:10,25
class People:
    def __init__(self,name, age, gender):
        self.name = name
        self.age = age
        self.gender = gender


class Student(People):
    def __init__(self, name, age, gender, no):
        People.__init__(self, name, age, gender)  # 调用父类的方法
        self.no = no

    def choose_class(self, class_obj):
        print("学生 %s 选择课程 %s" % (self.name, class_obj))


student = Student("dyy", 18, "男", 1712211212)
student.choose_class("应用化学")


class Teacher(People):
    def __init__(self, name, age, gender, salary, level):
        self.salary = salary
        self.level = level
        People.__init__(self, name, age, gender)  # 调用父类的方法

    def scored(self):
        print("老师 %s 正在为学生打分" % self.name)


teacher = Teacher("ddd", 20, "男", 30000, 10)
teacher.scored()
```

> [!warning] 
> 
> 不建议直接使用 **基类名** 的方式调用基类中的方法。通常，我们应该使用 `supper()` 进行代理。
> 

内置类 `super(type, object_or_type=None)` 返回一个代理对象，它会将方法调用委托给 `type` 的父类或兄弟类。 **这对于访问已在类中被重写的继承方法很有用**。

```python
class People:
    def __init__(self,name, age, gender):
        self.name = name
        self.age = age
        self.gender = gender


class Student(People):
    def __init__(self, name, age, gender, no):
        self.no = no
        # super().__init__(name, age, gender)
        super(Student, self).__init__(name, age, gender)
        # 调用 People 类的基类的 __init__() 方法，
        # 并自动将 self 传递给 __init__() 的第一个参数

    def choose_class(self, class_obj):
        print("学生 %s 选择课程 %s" % (self.name, class_obj))


student = Student("dyy", 18, "男", 1712211212)
student.choose_class("应用化学")


class Teacher(People):
    def __init__(self, name, age, gender, salary, level):
        self.salary = salary
        self.level = level
        super().__init__(name, age, gender)
        # 这是默认的情况，调用基类的 __init__() 方法，同时自动传递 self
        
        # super(Teacher, self).__init__(name, age, gender)
        
    def scored(self):
        print("老师 %s 正在为学生打分" % self.name)


teacher = Teacher("ddd", 20, "男", 30000, 10)
teacher.scored()
```

## 多继承

同 C++ 一样，Python 也支持多继承

```python
class DerivedClassName(Base1, Base2, Base3):
	pass
```

对于多数目的来说，在最简单的情况下，你可以认为搜索从父类所继承属性的操作是深度优先、从左到右的，当层次结构存在重叠时不会在同一个类中搜索两次。 因此，如果某个属性在 `DerivedClassName` 中找不到，就会在 `Base1` 中搜索它，然后（递归地）在 `Base1` 的基类中搜索，如果在那里也找不到，就将在 `Base2` 中搜索，依此类推。

真实情况比这个更复杂一些；方法解析顺序会动态改变以支持对 `super()` 的协同调用。 这种方式在某些其他多重继承型语言中被称为后续方法调用，它比单继承型语言中的 `super` 调用更强大

动态调整顺序是有必要的，因为所有多重继承的情况都会显示出一个或更多的 **菱形** 关联（即至少有一个上级类可通过多条路径被最底层的类所访问）。 例如，所有类都是继承自 `object` 类，因此任何多重继承的情况都提供了一条以上的路径可以通向 `object` 

为了确保基类不会被访问一次以上，**动态算法会用一种特殊方式将搜索顺序线性化**，保留每个类所指定的从左至右的顺序，只调用每个上级类一次，并且保持单调性（即一个类可以被子类化而不影响其父类的优先顺序）。 总而言之，这些特性使得设计具有多重继承的可靠且可扩展的类成为可能。

> [!seealso] 
> 
> 要了解更多细节，请参阅 [Python 2.3 方法解析顺序](https://docs.python.org/zh-cn/3.13/howto/mro.html#python-2-3-mro)
> 

### C3 算法

在正式介绍 C3 算法之前，我们分析下面的继承顺序，判断类 `C` 能否被成功创建？

```python
O = object


class X(O):
    pass


class Y(O):
    pass


class A(X, Y):
    pass


class B(Y, X):
    pass


class C(A, B):  # 该类不能被产生
    pass
```

它可以用以下继承图来表示，其中我用 `O` 来标记 `object` 类，它是任何新式类层级结构的起点

```
 -----------
|           |
|    O      |
|  /   \    |
 - X    Y  /
   |  / | /
   | /  |/
   A    B
   \   /
     ?
```

由于在 `A` 中 `X` 先于 `Y`；在 `B` 中 `Y` 先于 `X`，因此在 `C` 中方法解析顺序将出现歧义。执行上述代码，Python 会给出下列错误提示

```
TypeError: Cannot create a consistent method resolution
order (MRO) for bases X, Y
```

让我们引入一些适用于接下来的讨论的简单标记法。 我会使用这样的快捷标记:

```
C1 C2 ... CN
```

表示类列表 `[C1, C2, ... , CN]`。定义 `head` 为类列表中的第一个元素，即 `C1`；`tail` 表示类列表的剩余元素 `[C2, ..., CN]`。我还将使用这样的标记 `C + (C1 C2 ... CN) = C C1 C2 ... CN` 表示列表 `[C] + [C1，C2，...，CN]` 的总和

考虑多重继承层级结构中的类 `C`，`C` 继承自基类 `B1, B2, ... , BN`。 计算类 `C` 的线性化 `L[C]` 规则为：**C 的线性化就是 C 加上父类的线性化和父类列表的执行合并的总和**。该规则标记为

```
L[C(B1 ... BN)] = C + merge(L[B1] ... L[BN], B1 ... BN)
```

特别地，如果 C 为 `object` 类，它是没有父类的，其线性化很简单:

```
L[object] = object
```

> [!tip] 预设的合并规则
> 
> + 取第一个列表的 `head`，即 `L[B1][0]`；如果这个 `head` 不在任何其他列表的 `tail` 内，则将其添加到 `C` 的线性化中，并在合并结果中将其从列表中移除，否则如果下一个列表的 `head` 是好的 `head` 则使用它。 
> + 然后重复上述操作直到所有类都被移除或是无法找到好的 `head`。 在这种情况下将无法构造合并结果，Python 将拒绝创建类 C 并将引发 异常。
> 

这一预设规则可以确保合并操作 _保留_ 顺序，如果顺序能被保留的话。 在另一方面，如果顺序无法被保留（如上文讨论的顺序严重不一致的例子）则无法计算合并结果。

> [!example] 只有一个父类的情形
> 
> 单继承下的 `MRO`
> 
> ```
> L[C(B)] = C + merge(L[B],B) 
> 	    = C + merage(BO,B)
> 	    = C+ B + O
> ```

下面我们来看一个复杂的例子

```python
O = object  # L[O] = O


class F(O): 
	# L[F] = F O
    pass


class E(O): 
	# L[E] = E O
    pass


class D(O): 
	# L[D] = D O
    pass


class C(D, F): 
	# L[C] = C mearge(L[D], L[F], D, F) = C mearge(DO, FO, D, F)
	#      = C D mearge(O, FO, F) = C D F mearge(O, O) = C D F O
    pass


class B(D, E):
	# L[B] = B mearge(L[D], L[E], D, E) = B mearge(DO, EO, D, E)
	#      = B D mearge(O, EO, E) = B D E mearge(O, O) = B D E O
    pass


class A(B, C):
	# L[A] = A mearge(L[B], L[C], B, C) = A mearge(BDEO, CDFO, B, C)
	#      = A B mearge(DEO, CDFO, C) = A B C mearge(DEO, DFO)
	#      = A B C D mearge(EO, FO) = A B C D E mearge(O, FO) 
	#      = A B C D E F mearge(O, O) = A B C D E F O
    pass
```

在这种情况下继承图可以绘制为

```
                          6
                         ---
Level 3                 | O |                  (more general)
                      /  ---  \
                     /    |    \                      |
                    /     |     \                     |
                   /      |      \                    |
                  ---    ---    ---                   |
Level 2        3 | D | 4| E |  | F | 5                |
                  ---    ---    ---                   |
                   \  \ _ /       |                   |
                    \    / \ _    |                   |
                     \  /      \  |                   |
                      ---      ---                    |
Level 1            1 | B |    | C | 2                 |
                      ---      ---                    |
                        \      /                      |
                         \    /                      \ /
                           ---
Level 0                 0 | A |                (more specialized)
```

下面我们将 `B(D, E)` 修改为 `B(E, D)`，计算该列的线性化

```python
O = object  # L[O] = O


class F(O): 
	# L[F] = F O
    pass


class E(O): 
	# L[E] = E O
    pass


class D(O): 
	# L[D] = D O
    pass


class C(D, F): 
	# L[C] = C mearge(L[D], L[F], D, F) = C mearge(DO, FO, D, F)
	#      = C D mearge(O, FO, F) = C D F mearge(O, O) = C D F O
    pass


class B(E, D):
	# L[B] = B mearge(L[E], L[D], E, D) = B mearge(EO, DO, E, D)
	#      = B E mearge(O, DO, D) = B E D mearge(O, O) = B E D O
    pass


class A(B, C):
	# L[A] = A mearge(L[B], L[C], B, C) = A mearge(BEDO, CDFO, B, C)
	#      = A B mearge(EDO, CDFO, C) = A B E mearge(DO, CDFO)
	#      = A B E C mearge(DO, DFO) = A B E C D mearge(O, FO) 
	#      = A B E C D F mearge(O, O) = A B E C D F O
    pass
```

这样一个小的改动，完全改变了类的层次结构

```
                           6
                          ---
Level 3                  | O |
                       /  ---  \
                      /    |    \
                     /     |     \
                    /      |      \
                  ---     ---    ---
Level 2        2 | E | 4 | D |  | F | 5
                  ---     ---    ---
                   \      / \     /
                    \    /   \   /
                     \  /     \ /
                      ---     ---
Level 1            1 | B |   | C | 3
                      ---     ---
                       \       /
                        \     /
                          ---
Level 0                0 | A |
```

下面我们尅直接调用类方法 `mro()` 就可以直接获取方法解析顺序了

```python
print(A.mro())
# [<class '__main__.A'>, <class '__main__.B'>, <class '__main__.E'>, <class '__main__.C'>, <class '__main__.D'>, <class '__main__.F'>, <class 'object'>]
```

## 混合类 Mixins

继承通常表示 `is a` 的关系。然而，多继承往往会破外这种关系，导致程序失去可读性。在 Python 中，通过 `Mixins` 机制可以极大的提高程序的可读性

例如，下面的代码使用类表示交通工具。对于飞机来说，可能存在很多种飞机，如果为每个飞机都实现一个 `fly()` 方法，往往是不可取的。通过新建一个 `FlyableMixin` 类，让不同的飞机继承该混合类即可

```python
class Vehicle:  # 交通工具
    pass


class FlyableMixin:
    def fly(self):
        '''
        飞行功能相应的代码
        '''
        print("%s I am flying" % self.__class__)


class CivilAircraft(Vehicle, FlyableMixin):  # 民航飞机
    pass


class Helicopter(Vehicle, FlyableMixin):  # 直升飞机
    pass


class Car(Vehicle): 
    pass


h = Helicopter()
h.fly()  # <class '__main__.Helicopter'> I am flying

c = CivilAircraft()
c.fly()  # <class '__main__.CivilAircraft'> I am flying
```

> [!attention] 使用 Minxins 的注意事项
> 
> + `Mixins`机制下的类用于混合功能到子类中. 类名一般以`Mixin`结尾
> + `Mixins`类的职责必须单一.
> + `Mixins`类可以继承多个, 标识身份的类只能继承一个
> + `Mixins`类独立于子类, 不依赖子类的实现. 子类如果没有继承`Mixins`类也可以工作
> 

## 属性查找

在 [[Python：类与对象]] 中介绍了一个实例查找属性的顺序是：首先，在实例自身的`obj.__dict__` => `type(obj).__dict__`。下面我们来看继承背景下的属性查找顺序。

**类对象的基类信息将被用来解析属性引用**：如果请求的属性在类中找不到，搜索将转往基类中进行查找。 如果基类本身也派生自其他某个类，则此规则将被递归地应用

> [!tip] 
> 
> 类对象的属性查找顺序：**类对象 => 基类 => 基类的基类 => object 类**
> 
> 通过类对象查找属性时，首先查找当前类中的属性；如果没找到，就从基类中进行查找
> 

派生类的实例化没有任何特殊之处: `Student()` 会创建该类的一个新实例。 属性引用将按以下方式解析：搜索相应的类属性，如有必要将按基类继承链逐步向下查找，如果产生了一个函数对象则方法引用就生效。

> [!tip] 
> 
> 派生类的实例对象的属性查找顺序：**实例 => 实例所属类 => 基类 => 基类的基类 => object 类**
> 
> 通过派生类的实例对象查找属性时，首先在实例本身查找；如果没找到，就从其所属类中查找；如果还没找到，就从类的基类中查找
> 

> [!attention] 
> 
> 请注意：Python 中的所有类都是继承自 `object` 类
> 

## 多态

在 Python 中，多态不像 C++ 那样麻烦。**只需要保证不同类定义了相同的方法即可**。

```python
class Animal:
    def talk(self):  # 动物共有的方法, 交流
        print("%s 发出叫声" % self.__class__, end=": ")

class Cat(Animal): #动物的形态之一:猫
    def talk(self):
        super(Cat, self).talk()
        print('喵喵喵。。。')


class Dog(Animal): #动物的形态之二:狗
    def talk(self):
        super(Dog, self).talk()
        print("汪汪汪。。。")


class Pig(Animal): #动物的形态之三:猪
    def talk(self):
        super(Pig, self).talk()
        print('哼哼哼。。。')


class People(Animal):
    def talk(self):
        super(People, self).talk()
        print('嘤嘤嘤。。。')



animal1 = People()
animal2 = Dog()
animal3 = Pig()
animal4 = Cat()


def talk(animal: Animal):  # 统一接口定义.只要定义了 talk() 方法即可
    animal.talk()

talk(animal1)
talk(animal2)
talk(animal3)
talk(animal4)
```

### 鸭子类型

Python 崇尚的 **鸭子类型**（`duck typing`）：“如果看起来像、叫声像而且走起路来像鸭子，那么它就是鸭子”。比起继承的方式，鸭子类型在某种程度上实现了程序的松耦合度

```python
# 二者看起来都像文件,因而就可以当文件一样去用，然而它们并没有直接的关系

class Txt: # Txt 类有两个与文件类型同名的方法，即 read 和 write
    def read(self):
        pass
        
    def write(self):
        pass

class Disk:  # Disk 类也有两个与文件类型同名的方法：read 和 write
    def read(self):
        pass
        
    def write(self):
        pass
```

> [!tip] 
> 
> 不同类定义了相同的方法，那么这个两个类就可以视为同一种类
> 

### 抽象基类

虽然鸭子类型只要求实现相同的方法即可。然而，这样做往往是错误的根源。因为，程序员有可能将方法名写错，从而导致错误。**为了确保不同类实现相同的方法**，让类继承一个公共的基类，并且基类要求派生类必须实现某些方法。

> [!tip] 
> 
> 这种限制派生类行为的类称为抽象类。参考 [[C++：多态#抽象类]]
> 

在 Python 中，抽象类的定义需要依赖标准模块 `abc`

```python
import abc

# 指定 metaclass 属性将类设置为抽象类，抽象类本身只是用来约束子类的，不能被实例化
class Animal(metaclass=abc.ABCMeta): 

    @abc.abstractmethod # 该装饰器限制子类必须定义有一个名为 talk 的方法
    def talk(self): # 抽象方法中无需实现具体的功能
        pass

class Cat(Animal): # 但凡继承 Animal 的子类都必须遵循 Animal 规定的标准
    def talk(self):
        pass

class Dog(Animal):
	def talk(self):
		pass

cat = Cat() # 若子类中没有一个名为 talk 的方法则会抛出异常 TypeError，无法实例化
```

> [!tip] 
> 
> 在介绍 [[Python：元类]] 的时候，我们会介绍 `ABCMeta` 的实现原理
> 

