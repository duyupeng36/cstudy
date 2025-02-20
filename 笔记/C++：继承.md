# 继承

类表示的是现实世界或应用程序世界中的一种概念，或思想。任何一个概念都不是孤立存在的，都有与之共存的相关的概念，而且其强大能力中的大部分都源于与其他概念的关联。例如，当我们尝试解释汽车是什么时，很快就会聊到轮子、引擎、司机、行人等概念。

> [!tip] 
> 
> 由于我们使用类表示概念，问题就变为如何表示概念之间的关系。
> 

C++ 提供了 **派生类** 的概念及相关的语言机制来 **表达层次关系**，也就是 **表达类之间的共性**。例如，**圆形的概念和三角形的概念它们是相关的，它们都表示形状**；即，它们具有形状这一公共概念。因此，我们明确定义类 `Circle` 和 类 `Triangle` 共同有用类 `Shape`。本例中类 `Shape` 称为 **基类**；类 `Circle` 和 `Triangle` 称为 **派生类**。在程序中表示 **圆形** 和 **三角形**，而不涉及 **形状** 的概念，就会遗漏某些重要的东西。C++ 语言特性支持从已有的类构建类

> [!tip] 实现继承
> 
> 通过共享基类所提供的特性来减少实现工作量
> 

> [!tip] 接口继承
> 
> 通过一个公共基类提供的接口允许不同派生类互换使用
> 

接口继承被称为 **运行时多态**。在 [[C++：模板]] 所提供的类的通用性与继承无关，被称为 **编译时多态**。下面我们介绍实现继承。接口继承将在 [[C++：多态]] 中介绍

C++ 的继承语法如下

```cpp
class 派生类: 继承方式 基类1, 继承方式 基类2
{};
```

> [!tip] 
> 
> 继承可以在派生类中访问基类的 `public` 和 `protected` 成员，
> 
> 派生类可以使用不同的继承方式继承基类的成员，所谓的继承方式就是限制继承下来的成员的访问权限
> 

## 派生类

考虑设计一个程序，管理公司的雇员。这个程序可能要有如下所示的数据结构

```cpp
struct Employee
{
	string first_name, family_name;  // 名字 姓氏
	char middle_initial;             // 中间名的首字母
	Date hiring_date;                // 雇佣时间
	short department;                // 所属部门
};

struct Manager
{
	Employee emp;                    // 经理也是雇员，存储他的信息
	list<Employee*> group;           // 经理管理的雇员
	short level;                     // 等级
};
```

> [!tip] 经理也是一个雇员
> 
> 在上述例子中，经理的信息存储在 `Manager` 对象的 `emp` 成员中。但是，这样的代码未向编译器或者其他工具表达出 "一个 `Manager` 也是一个 `Employee` 的概念"
> 
> 换句话说，`Manager *` 不是 `Employee *`，因此在需要两者之一的地方，不能简单的使用另一个
> 
> 当然，我我们可以将 `Manager *` 显示类型转换为 `Employee *` 。然而，这样做会导致程序混乱
> 

为了显示表示  "一个 `Manager` 也是一个 `Employee` 的概念"，可以使用派生类

```cpp
struct Manager: public Employee 
{
	list<Employee*> group;           // 经理管理的雇员
	short level;                     // 等级
};
```

上述代码中，`Manager` 派生自 `Employee`，也就是 `Employee` 是 `Manager` 的基类

> [!tip] 
> 
> 派生类除了自己的成员外，还拥有基类的成员。请注意，派生类中不允许访问基类的 `private` 成员
> 

通常，我们称一个派生类继承了来自基类的成员，因此这种关系也称为 **继承**。派生类概念的一种流行且高效的实现就是将派生类对象表示为基类对象，再加上哪些专属于派生类的信息放在末尾

![[Pasted image 20250215222922.png]]

> [!attention] 
> 
> 派生类对象没有任何内存额外开销，所需内存就是成员所需的空间
> 

按照上述方式从 `Employee` 派生 `Manager`，使得 `Manager` 成为 `Employee` 的一个子类型，从而在任何接受 `Employee` 的地方都可以使用 `Manager`

```cpp
void f(Manager &m1, Employee &e1)
{
	list<Employee*> elist {&m1, &e1}
}
```

> [!important] 
> 
> 一个 `Manager` 也是一个 `Employee`，因此 `Manager *` 可以隐式转换为 `Employee *`，`Manager &` 可以隐式转换为 `Employee &`
> 
> 请注意：一个 `Employee` 不一定是 `Manager`，因此 `Employee *` 隐式转换为 `Manager *`，因为 `Manager *` 管理的空间要比 `Employee` 管理的空间要大，编译器认为这是不安全的行为。如果需要，必须使用强制类型转换将 `Employee *`  转换为 `Manager *` 
> 

> [!attention] 将一个类作为基类，等价于定义一个该类的无名对象。因此，该类必须定义后才能作为基类
> 

```cpp
class Employee;  // 声明

class Manager : public Employee  // 错误：Employee 未定义
{

};
```

## 继承方式

继承方式和基类成员的访问方式共同决定基类成员在派生类和派生类对象中是否可访问。下面我们探究不同的继承方式的特点

C++ 提供 $3$ 中继承方式。如下表给出了 $3$ 中继承方式的成员访问权限

| 继承方式        | 基类成员的访问方式   | 派生类中的访问权限   | 派生类对象对基类成员的访问权限 |
| :---------- | ----------- | ----------- | --------------- |
| `public`    | `public`    | `public`    | 可以访问            |
| `public`    | `protected` | `protected` | 不可访问            |
| `public`    | `private`   | 不可访问        | 不可访问            |
|             |             |             |                 |
| `protected` | `public`    | `protected` | 不可访问            |
| `protected` | `protected` | `protected` | 不可访问            |
| `protected` | `private`   | 不可访问        | 不可访问            |
|             |             |             |                 |
| `private`   | `public`    | `private`   | 不可访问            |
| `private`   | `protected` | `private`   | 不可访问            |
| `private`   | `private`   | 不可访问        | 不可访问            |

> [!tip] 
> 
> 对于 `class`，继承方式`private` 是默认的；对于 `struct`，继承方式 `public` 是默认的
> 

> [!summary] 
> 
> + 派生类内部不能访问基类的 `private` 成员，其他成员可以访问
> 
> + 派生类对象在类作用域外只有 `public` 继承方式继承的基类的 `public` 成员才能被访问
> 
> 换句话说：私有成员无法在其声明的类作用域外访问。继承方式与基类成员访问权限做交集
> 

```cpp
class A
{
public:
	int a;
};

class B : private A   // B 私有继承 A。私有继承限制了后面的派生类对上层基类的成员访问权限
{};

class C : private B  // C 私有继承 B。由于 B 的基类 A 是
{
	void func(){
		a;            //error，无法访问a
	}
};
```

## 成员函数

`Employee` 和 `Manager` 这样的简单数据结构没什么特别的用处。若想提供一个真正的类型，就要为其定义一组恰当的操作，而且不能依赖于特定的表现形式

```cpp
class Employee
{
public:
	void print() const;
	string full_name() const;
private:
	string first_name, family_name;  // 名字 姓氏
	char middle_initial;             // 中间名的首字母
	Date hiring_date;                // 雇佣时间
	short department;                // 所属部门
};

class Manager: public Employee       // 继承 Employee 的成员
{
public:
	void print() const;

private:
	list<Employee*> group;           // 经理管理的雇员
	short level;                     // 等级	
}
```

> [!tip] 
> 
> 派生类作用域中可以访问基类的 `public` 和 `protect` 成员，就好像它们声明在派生类中一样
> 

```cpp
void Manager::print() const
{
	cout << "name is " << full_name() << "\n";
}
```

> [!warning] 
> 
> 派生类作用域中无法访问基类的 `private` 成员。如果让派生类的成员函数可以访问基类的私有成员，这会令私有成员的概念变得毫无意义。
> 
> 只需要派生一个类，就能获得基类的私有成员的访问权限是不合适。这样一旦发生错误，我们不止需要检查成员函数和友元，还需要检查该类的所有派生类
> 

```cpp
void Manager::print() const
{
	cout << "name is " << family_name << "\n";  // 错误：family_name 是基类的 private 成员，派生类不允许访问
}
```

> [!tip] 
> 
> 对于派生类而言，最简单的方式就是只使用基类的 `public` 成员
> 

```cpp
void Manager::print() const
{
	Employee::print();  // 打印 Employee 信息
	cout << level;      // 打印 Manager 特有的信息
}
```

> [!warning] 
> 
> 请注意：调用基类的 `print()` 必须使用 `Employee::print()`，因为 `print()` 在 `Manager` 中已经被 **重新定义** 。
> 

如果在 `Manager::print()` 中调用的是 `print()` 就会陷入递归，直到栈空间耗尽而导致程序崩溃

```cpp
void Manager::print() const
{
	print();   // 递归
	// ...
}
```

## 继承的局限性

继承可以复用代码，提高代码的可重用性。但是，C++ 中继承也存在局限性

> [!tip] 派生类可能会持有额外的资源，因此派生类的构造和析构函数必须重新定义
> 
> + 构造和析构函数不能被继承
> + 拷贝构造和拷贝赋值运算符不能被继承
> + `operator new` 和 `operator delete` 不能被继承
> 
> 由于友元破坏了类的封装，C++ 禁止了友元的继承
>   

## 单继承

现在，我们学习在单继承在下派生类对象的构造与析构
### 派生类对象的构造

派生类对象占用的内存被分配出来时，会自动调用派生类的构造函数。派生类的构造函数会首先调用基类的构造函数，然后调用类对象成员的构造函数，最后才会执行派生类的构造函数体

> [!tip] 基类构造函数的调用机制
> + 派生类没有显式调用基类的构造函数时，会默认调用基类的无参构造
> + 如果基类没有无参构造函数，则派生类必须显示调用基类的构造函数

下面的代码演示了在创建派生类对象时，编译器会在构造时默认调用基类的无参构造。

```cpp
#include <iostream>

using namespace std;

class A 
{
public:
    A() {
        cout << "A()" << endl;
    }
};

class Base
{
public:
    Base(): _base{10} {
        cout << "Base()" << endl;
    }
private:
    long _base;
};


class Derived : public Base
{
public:
    Derived(long derived)
    /*:Base() 这是隐式调用的*/
    : _derived {derived}  
    {
        cout << "Derived(long derived)" << endl;
    }
private:
    long _derived;
    A _a;
};

int main() 
{
    Derived d{10};
}
```

编译运行的结果为

```shell
➜  cppfiles g++ main.cpp 
➜  cppfiles ./a.out 
Base()                    # 基类的构造函数
A()                       # 对象成员的构造函数
Derived(long derived)     # 派生类的构造函数
```

创建派生类对象时，在对象的内存布局开始位置会存在一个基类子对象，在基类子对象之后存储派生类对象自己的数据成员

![[Pasted image 20250216144240.png]]

下面的代码演示了基类没有无参构造函数时，派生类必须显式调用基类的构造函数，否则无法创建派生类对象

```cpp
#include <iostream>

using namespace std;

class A 
{
public:
    A() {
        cout << "A()" << endl;
    }
};

class Base
{
public:
    Base(long base): _base{base} {
        cout << "Base()" << endl;
    }
private:
    long _base;
};


class Derived : public Base
{
public:
    Derived(long derived)
    : _derived {derived}  
    {
        cout << "Derived(long derived)" << endl;
    }
private:
    long _derived;
    A a;
};

int main() 
{
    Derived d{10};
}
```

上面的代码编译时会提示下列错误

```shell
➜  cppfiles g++ main.cpp
main.cpp: 在构造函数‘Derived::Derived(long int)’中:
main.cpp:28:24: 错误：对‘Base::Base()’的调用没有匹配的函数
   28 |     : _derived {derived}
      |                        ^
main.cpp:16:5: 附注：备选: ‘Base::Base(long int)’
   16 |     Base(long base): _base{base} {
      |     ^~~~
main.cpp:16:5: 附注： 备选需要 1 个实参，但提供了 0 个
```

> [!tip] 由于基类的构造函数需要一个参数，因此在其派生类的构造函数中需要显示调用

```cpp hl:28
#include <iostream>

using namespace std;

class A 
{
public:
    A() {
        cout << "A()" << endl;
    }
};

class Base
{
public:
    Base(long base): _base{base} {
        cout << "Base()" << endl;
    }
private:
    long _base;
};


class Derived : public Base
{
public:
    Derived(long derived)
    : Base{20},   // 必须显示调用基类的构造函数：基类没有无参构造函数
    _derived {derived}  
    {
        cout << "Derived(long derived)" << endl;
    }
private:
    long _derived;
    A a;
};

int main() 
{
    Derived d{10};
}
```

编译运行的结果为

```shell
➜  cppfiles g++ main.cpp
➜  cppfiles ./a.out 
Base()                # 基类的构造函数
A()                   # 成员的构造函数
Derived(long derived) # 派生类的构造函数的函数体
```

> [!summary] 
> 
> 派生类对象在创建时，首先会调用派生类的构造函数，在执行构造函数的函数体之前。在执行构造函数的函数体之前，首先调用基类的构造函数，然后调用成员的构造函数，最后执行派生类构造函数的函数体
> + 基类的构造函数 -> 成员的构造函数 -> 派生类构造函数的函数体
>   
> 派生类的构造函数的初始化列表中，如果不显示调用基类的构造函数，则调用基类的默认构造函数
> 
> 如果基类没有默认构造函数，派生类的初始化列表必须中必须显示调用基类的构造函数
> 

### 派生类对象的析构

当派生类对象不能够被访问或者调用 `delete` 销毁对象时，立即调用派生类的析构函数并立即执行析构函数的函数体，然后调用派生类对象成员的析构函数，最后调用基类的析构函数

```cpp
#include <iostream>

using namespace std;

class A 
{
public:
    ~A() {
        cout << "~A()" << endl;
    }
};

class Base
{
public:
    ~Base() {
        cout << "~Base()" << endl;
    }
private:
    long _base;
};


class Derived : public Base
{
public:
    ~Derived()
    {
        cout << "~Derived()" << endl;
    }
private:
    long _derived;
    A a;
};

int main() 
{
    Derived d;
}
```

编译上述代码并执行的结果为

```shell
➜  cppfiles g++ main.cpp
➜  cppfiles ./a.out 
~Derived()   # 首先，执行派生类的析构函数
~A()         # 然后，执行成员的析构函数
~Base()      # 最后，执行基类的析构函数
```

### 隐藏基类的成员

在派生类中如果定义了和基类同名的成员，此时，在派生类中就无法直接访问基类中的该同名成员。我们称派生类中的成员 **隐藏** 了基类中的成员

```cpp hl:18,12,35,28
#include <iostream>

using namespace std;

class Base
{
public:
    Base(long base) : _base{base}
    {
    }

    void print() const
    {
        cout << "Base::_base = " << _base << endl;
    }

private:
    long _base;
};

class Derived : public Base
{
public:
    Derived(long derived) : Base{20}, _base{derived}
    {
    }

    void print() const // 隐藏了 Base 的成员函数 print()
    {
        Base::print();                                // 调用基类的 print() 成员函数。本质上是 this->Base::print()
        cout << "Derived::_base = " << _base << endl; // 注意，这里的 _base 是派生类的数据成员
    }

private:
    long _base; // 隐藏了基类的数据成员 _base
};

int main()
{
    Derived d{10};
    d.print();
}
```

> [!important] 
> 
> **隐藏并代表基类中的同名成员不存在了**，只是在访问该成员时会优先被访问到
> 
> 如果需要访问基类中的成员，就必须加上作用域限定符，但是这种写法不符合面向对象的原则，不推荐使用
> 

## 多继承

C++ 除了支持单继承外，**还支持多重继承**。那为什么要引入多重继承呢？其实是因为在客观现实世界中，我们经常碰到 **一个人可以身兼数职** 的情况，如在学校里，一个同学可能既是一个班的班长，又是学生中某个部门的部长；在创业公司中，某人既是软件研发部的 CTO ，又是财务部的 CFO ；一个人既是程序员，又是段子手。诸如此类的情况出现时，单一继承解决不了问题，就可以采用 **多继承** 了。

> [!tip] 
> 
> 在前面的学习中，我们知道继承表达的是 `IS A` 的关系。例如， `Manager` 是一个 `Employee`
> 

![[Pasted image 20250216163009.png]]

### 派生类的构造和析构

采用多继承方式派生的类的构造和析构与单继承的情况下类似

```cpp
#include <iostream>

using namespace std;

class A
{
public:
    A() {
        cout << "A()" << endl;
    }
    ~A() 
    {
        cout << "~A()" << endl;
    }
};

class B
{
public:
    B() {
        cout << "B()" << endl;
    }
    ~B() 
    {
        cout << "~B()" << endl;
    }
};

class C
{
public:
    C() {
        cout << "C()" << endl;
    }
    ~C() 
    {
        cout << "~C()" << endl;
    }
};

class D: public A,public B, public C
{
public:
    D() {
        cout << "D()" << endl;
    }
    ~D() 
    {
        cout << "~D()" << endl;
    }
};

int main()
{
    D d;
}
```

编译运行上述代码，得到的结果如下

```shell
➜  cppfiles g++ main.cpp
➜  cppfiles ./a.out 
A()                      # 基类 A
B()                      # 基类 B
C()                      # 基类 C 
D()                      # 派生类 D
~D()                     # 派生类 D 的析构
~C()                     # 基类 C 的析构
~B()                     # 基类 B 的析构
~A()                     # 基类 A 的析构
```

下面我们修改基类的继承顺序，然后编译运行

```cpp
class D: public A, public C ,public B
{
public:
    D() {
        cout << "D()" << endl;
    }
    ~D() 
    {
        cout << "~D()" << endl;
    }
};

int main()
{
    D d;
}
```

上述代码的执行结果如下

```shell
➜  cppfiles g++ main.cpp
➜  cppfiles ./a.out 
A()                       # 基类 A
C()                       # 基类 C
B()                       # 基类 B
D()                       # 派生类 D
~D()                      # 派生类 D 的析构
~B()                      # 基类 B 的析构
~C()                      # 基类 C 的析构
~A()                      # 基类 A 的析构
```

> [!summary] 构造函数
> 
> 创建派生类对象时，首先调用派生类的构造函数。在执行派生类构造函数的函数体之前， 首先 **会按照继承顺序调用基类的构造函数**，然后调用成员的构造函数，最后执行派生类的构造函数的函数体
> 

> [!tip] 析构函数
> 
> 派生类对象被销毁时，首先调用派生类的析构函数。在执行完派生类的析构函数之后，会调用派生类成员的析构函数，最后 **按照继承顺序的相反顺序调用基类的析构函数**
> 

下图展示了多重继承的派生类对象的内存布局图

![[Pasted image 20250216165231.png]]

### 成员名访问的二义性

发生多继承时，不同的基类可能提供了同名的成员。派生类如果没有定义同名的成员，通过派生类对象访问时，应该选择哪个基类提供的同名成员呢？

![[Pasted image 20250216170805.png]]

```cpp hl:68
#include <iostream>

using namespace std;

class A
{
public:
    A() {
        cout << "A()" << endl;
    }
    ~A() 
    {
        cout << "~A()" << endl;
    }
    void print() const
    {
        cout << "A::print()" << endl;
    }
};

class B
{
public:
    B() {
        cout << "B()" << endl;
    }
    ~B() 
    {
        cout << "~B()" << endl;
    }
    void print() const
    {
        cout << "B::print()" << endl;
    }
};

class C
{
public:
    C() {
        cout << "C()" << endl;
    }
    ~C() 
    {
        cout << "~C()" << endl;
    }
    void print() const
    {
        cout << "C::print()" << endl;
    }
};

class D: public A, public C ,public B
{
public:
    D() {
        cout << "D()" << endl;
    }
    ~D() 
    {
        cout << "~D()" << endl;
    }
};

int main()
{
    D d;
    d.print();  // 不明确的
}
```

上述代码编译时，编译器会提供如下报错信：编译器提示我们可供选择的 `print()` 成员函数有多个

```shell
➜  cppfiles g++ main.cpp
main.cpp: 在函数‘int main()’中:
main.cpp:68:7: 错误：对成员‘print’的请求有歧义
   68 |     d.print();  // 不明确的
      |       ^~~~~
main.cpp:31:10: 附注：备选为: ‘void B::print() const’
   31 |     void print() const
      |          ^~~~~
main.cpp:47:10: 附注：        ‘void C::print() const’
   47 |     void print() const
      |          ^~~~~
main.cpp:15:10: 附注：        ‘void A::print() const’
   15 |     void print() const
      |          ^~~~~
```

> [!tip] 
> 
> 可以使用作用域限定，选择使用哪个 `print()` 成员函数
> 

```cpp
int main()
{
    D d;
    d.A::print();  // 明确使用 A 类的 print() 成员
}
```

### 菱形继承的问题

菱形继承是多个继承中的特定方案，其中一个类是从两个或多个类得出的类，这些类又是从公共基类得出的。

```cpp
#include <iostream>

using namespace std;

class A
{
public:
    void print() const 
    {
        cout << "A::print()" << endl;
    }
private:
    double _a;
};

// B 继承 A
class B : public A 
{

private:
    double _b;

};

// C 继承 A
class C : public A
{
private:
    double _c;
};

// D 继承 B 和 C
class D: public B, public C
{
private:
    double _d;
};

int main() {
    D d;
    d.print(); // 不明确的
}
```

![[Pasted image 20250216192337.png]]

> [!attention] 
> 
> 它产生了一种歧义，这是由于复制公共基类而产生的，这在调用重复的成员时导致了模棱两可的行为。
> 
> ![[Pasted image 20250216195108.png]]
> 

除了成员访问的二义性外，还存在存储方面的二义性。由于 `D` 类继承了 `B` 和 `C`，而 `B` 和 `C` 又继承自 `A`。因此，对于类 `D` 对象在内存中存储如下下图

![[Pasted image 20250216195905.png]]

> [!warning] 
> 
> 创建 `D` 类的对象时，需要创建其基类 `B` 和 `C` 的子对象。由于 `B` 和 `C` 继承自 `A`，因此在创建 `B` 和 `C` 的对象时，需要创建基类 `A` 的子对象
> 
> 所以，`D` 类对象的内存布局中包含了 $2$ 个类 `A` 的子对象。访问继承自 `A` 的成员时，就会出现二义性问题。因为编译器不知道使用哪个基类子对象去调用成员函数
> 

解决菱形继承带来的二义性问题的方式就是：**中间层的基类采用 _虚继承_ 方式解决菱形继承带来的二义性问题**

```cpp hl:17,26
#include <iostream>

using namespace std;

class A
{
public:
    void print() const 
    {
        cout << "A::print()" << endl;
    }
private:
    double _a;
};

// B 继承 A
class B : virtual public A 
{

private:
    double _b;

};

// C 继承 A
class C : virtual public A
{
private:
    double _c;
};

// D 继承 B 和 C
class D: public B, public C
{
private:
    double _d;
};


int main() {
    D d;
    d.print();   
}
```

通过 `virtual` 继承处理菱形继承的问题，实际上改变的是派生类对象的内存布局。类 `B` 和 `C` 的对象的内存布局中多出了一个 **虚基类指针**，指向基类对象所在的内存地址。同时，类 `D` 的对象中只会有一份基类`A` 的子对象。**不同的编译器对多继承派生类对象的内存组织可能不同**

下图展示了的 **msvc 编译器** 对多继承类对象的内存布局的组织。

![[Pasted image 20250216201206.png]]

使用 **g++ 编译器**，然后使用 GDB 查看对象派生类 `D` 的对象(`d`) 的内存布局

```shell
➜  cppfiles g++ -g main.cpp
➜  cppfiles gdb a.out 
....
(gdb) set print object on
(gdb) set print vtbl on
(gdb) set print pretty on
(gdb) b 42
(gdb) r
....
(gdb) p d  # 输出对象 d
$1 = (D) {
  <B> = {
    <A> = {
      _a = 6.9533490038348635e-310
    }, 
    members of B:
    _vptr.B = 0x555555557ca8 <vtable for D+24>,
    _b = 3.6426471936583425e-319
  }, 
  <C> = {
    members of C:
    _vptr.C = 0x555555557cc0 <VTT for D>,
    _c = 2.1165311283480734e+214
  }, 
  members of D:
  _d = 6.9533558073678158e-310
}
```

## 基类与派生类对象之间的转换

一般情况下，**基类占据的空间要比派生类占据的空间小**。为了内存访问的安全，**编译器 _不允许_ 将基类对象赋值给派生类对象**。但是，**派生类对象可以赋值给基类对象**

> [!tip] 
> 
> 一般情况下，**派生类对象占据的空间要比基类对象占据的空间大**，将派生类对象转换为基类对象，不可能超过可访问的内存空间
> + 派生类对象赋值给基类对象，可以让基类数据成员全部得到数据，因此是安全的
> + 基类对象赋值给派生类对象，派生类对象中的数据成员可能没有得到值。换句话说，就是派生类对象部分初始化
> 

```cpp
Base base;
Derived d1;

/* 派生类对象占据的空间大于或等于基类对象占据的空间，因此派生类对象可以赋值给基类对象，并且只会拷贝派生类对象中的基类子对象 */
base = d1;        //ok
/* 派生类对象占据的空间大于或等于基类对象占据的空间，因此将基类对象赋值给派生类对象时，会导致派生类对象数据不完整 */
d1 = base;        //error
```

同理，**派生类对象的指针可以隐式转换为基类对象的指针**。因为，基类的指针能够访问的内存空间的大小一定小于或等于派生类对象占据的内存空间。反过来就不行了，因为派生类的指针可访问的内存空间大小可能大于基类对象占据的内存空间，从而出现越界访问的问题

> [!attention] 向上转换：派生类的指针或引用向基类的指针或引用进行转换。这是可以隐式进行的
> 
> 向上转换属于是访问空间缩小的转换，这是符合安全策略的。
> 

```cpp
Base * pbase = &d1;         // ok：基类的指针不可能访问超过派生类对象占据的内存空间
Derived * pderived = &base  // error：派生类的指针可能访问超过基类对象占据的空间，这会导致越界访问的问题

Base & rbase = d1;         // ok
Derived & rderived = base; // error
```

下图说明了为什么派生类的指针能转换为基类的指针，而基类的指针不能转换为派生类的指针

![[Pasted image 20250216233527.png]]

> [!tip] 
> 
> `Base` 类的指针指向 `Derived` 类的对象，`d1` 对象中存在一个 `Base` 类的基类子对象，这个 `Base` 类指针所能操纵只有继承自 `Base` 类的部分
> 
> `Derived` 类的指针指向 `Base` 对象，除了操纵 `Base` 对象的空间，还需要操纵一片空间，这是非法空间，所以会报错。
> 

当 `Base` 类的指针指向的就是 `Derived` 类的对象时，可以将 `Base` 类的指针转换为 `Derived` 的指针。这是合理的，需要使用 `dynamic_cast` 强制类型转换进行

![[Pasted image 20250216233903.png]]

```cpp hl:5
Derived d1;
Base * pbase = &d1;  // 基类的指针指向的就是派生类的对象

//向下转型
Derived * pd = dynamic_cast<Derived*>(pbase);

if(pd){
	cout << "转换成功" << endl;
	pd->display();
}else{
	cout << "转换失败" << endl;
}
```

> [!attention] 向下转换：基类的指针或引用转换为派生类的指针或引用。这通常是不允许的
> 
> 请注意，只有当基类的指针指向的是派生类对象才能执行 **向下转换**
> 

> [!summary] 
> 
> 可以用 **派生类对象赋值给基类对象**，可以用 **基类指针指向派生类对象**，可以用 **基类引用绑定派生类对象**。
> 
> 反过来均不可以
> 

## 派生类对象的拷贝

需要拷贝派生类对象时，需要遵循一个原则：**基类子对象和派生类部分需要单独处理**

> [!tip] 
> 
> 当派生类中没有显式定义拷贝操作的函数时，就会自动完成基类部分的拷贝操作
> 
> 注意：拷贝操作函数包括 **拷贝构造函数** 和 **拷贝赋值运算符函数**
> 

> [!tip] 
> 
> 当派生类中有显式定义拷贝操作函数时，不会再自动完成基类部分的拷贝操作，需要显式地调用
> 

从拷贝的目的来看，**一个基类等价于一个成员**：为了拷贝派生类的一个对象，必须拷贝其基类子对象

![[Pasted image 20250217123040.png]]

> [!important] 
> 
> 如果 **派生类没有持有资源**，就 **不需要显式定义拷贝构造函数和拷贝赋值运算符函数**，编译器会自动完成并且没有任何错误
> 

但是，如果在派生类中显式定义了拷贝操作的函数，此时就需要显式调用基类的拷贝操作

```cpp hl:27,35
#include <iostream>

using namespace std;

class Base
{
public:
    Base(long base): _base{base} {
    }
    void print() const {
        cout << "_base: " << _base << endl;
    }
protected:
    long _base{10};
};

class Derived: public Base
{
public:
    Derived(long base, long derived)
    : Base{base}  // 初始化基类子对象
    , _derived {derived} {
    }

    /* 拷贝构造函数 */
    Derived(const Derived & d)
    : Base{d},  // 拷贝基类子对象
    _derived{d._derived}
    {
        cout << "Derived(const Derived & d)" << endl;
    }

    /* 拷贝赋值运算符 */
    Derived & operator=(const Derived &rhs){
        Base::operator=(rhs);  // 调用基类的拷贝赋值运算符函数
        _derived = rhs._derived;

        cout << "Derived& operator=(const Derived &)" << endl;
        return *this;
    }

    void print() const {
        cout << "_base: " << _base << endl;
        cout << "_derived: " << _derived << endl;
    }

private:
    long _derived{12};
};

int main() {
    Derived derived1{12, 20};
    Derived derived2{0, 0};
    derived2 = derived1;
    derived2.print();
}
```

> [!summary] 
> 
> + 如果基类的数据成员持有资源，则基类必须显式定义拷贝操作
> 
> + 派生类自身的数据成员没有持有资源，则不用显式定义拷贝操作
> 
> 换句话而言：哪个类持有资源，哪个类就需要显式定义拷贝操作和析构操作
> 


