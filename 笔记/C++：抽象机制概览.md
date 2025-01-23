# 抽象机制概览

下面我们将粗略学习 C++ 中的抽象和资源管理。将会学习如何定义 **新类型**(用户自定义类型)，重点介绍 **具体类**、**抽象类** 和 **类层次结构** 有关的基本属性、实现技术和语言特性。**模板** 是一种用其他类型和算法 **对类型和算法进行参数化** 的机制

> [!tip] 
> 
> 用户自定义类型与内置类型的计算表现为函数，有时泛化为 **模板函数** 和 **函数对象**
> 
> 这些特性用于支持 **面向对象编程** 和 **泛型编程** 等编程风格
> 

## 类

C++ 最核心的语言特性就是 **类**。类是一种用户自定义数据类型，用于在程序代码中表示某种**概念**

> [!tip] 
> 
> **类** 是用户自定义类型，在程序中表示 **某种概念**
> 

类含有一系列的 **成员**，可能是 **数据** **函数** 或者 **类型**。这些成员可能 **公开的**(`public`)、**私有的**(`private`)或者 **受保护的**(`protected`)

无论何时，只要我们想为程序设计一个有用的概念、想法和实体，都应该设法把它表示为程序中的一个类，这样我们的想法就能表达成代码

> [!hint] 
> 
> 使用一组精挑细选的类完成任务要比使用内置类型完成带来更好的可读性和正确性
> 

从本质上来说，基础类型、运算符和语句之外的所有语言特性存在的目的就是帮助我们定义更好的类以及更便捷地使用。C++ 提供了三种重要的类的基本支持：**具体类** 、**抽象类**、**类层次中的类**

### 具体类

**具体类** 的基本思想是它们的 **行为就像内置类型一样**。例如，一个复数类型和一个无穷精度整数与内置的 `int` 非常相似，它们有自己的语义和操作集合。`vector` 和 `string` 类也很像内置的数组

**具体类型** 的典型特征就是，它的 **表现形式是其定义的一部分**。所谓表现形式就是数据成员。就像`vector` 类中，其表现形式只不过是一个或几个指向保存在别处的数据的指针，但这种 **表现形式出现在具体类的每个对象中**

> [!tip] 
> 
> 具体类的每个对象都包含了其表现形式，使得实现可以在时空上达到最优，尤其是允许我们
> + 把具体类的对象放在任何可以存储数据的内存区域：栈、堆、数据区等；甚至可以放在其他对象中
> + 直接应用对象，而非仅仅通过指针或引用
> + 创建对象后立即进行完整的初始化
> + 拷贝对象
> 

类的表现形式可以被限定为私有的，只能通过成员函数访问。因此，表现形式发生任何修改都需要重新编译程序

#### 复数类

下面我们介绍一个经典的用户自定义算术类型：**复数类型**

```cpp
class complex {
    double re, im; // 表现形式：两个双精度浮点数
public:
    complex(double r, double i):re{r}, im{i}{}; // 用两个标量构建复数
    complex(double r): re{r}, im{0}{}; // 使用一个标量构建复数
    complex() : re{0}, im{0}{}; // 默认构建复数 {0., 0.}

    double real() const {return re;}  // 获取对象的实部
    void real(double d) {re = d;}
    double imag() const {return im;}  // 获取对象的虚部
    void imag(double d) {im = d;}
    
    complex & operator+=(complex z) {re += z.re, im += z.im; return *this;} // 加到 re 和 im 然后返回
    complex & operator-=(complex z) {re -= z.re, im -= z.im; return *this;}

    complex & operator*=(complex); // 在类外某处进行定义
    complex & operator/=(complex); // 在类外某处进行定义
};

complex &complex::operator*=(complex z) {
    // 复数乘法 (a + bi) * (c + di)
    // (ac - bd) + (ad + bc)i
    *this = {
    re*z.re - im*z.im,
    im*z.re + re*z.im,
    };
    return *this;
}

complex &complex::operator/=(complex z) {
    // 复数除法 (a + bi) / (c+di)
    // (a+bi)(c-di) / (c ** 2 + d ** 2)
    *this = {
        ((*this) * z.conjugate()).real() / (z.real() * z.real() + z.imag() * z.imag()),
        ((*this) * z.conjugate()).imag() / (z.real() * z.real() + z.imag() * z.imag())
    };
    return *this;
}
```

上述 `complex` 类是标准库 `complex` 的简化版本，类定义本身仅包含需要访问其表现形式的操作。它的表现形式非常简单，也是约定俗成的。出于编程实践要求，必须兼容 $50$ 年前 Fortran 语言提供的版本，还需要一些常规的运算符

> [!tip] 
> 除了满足逻辑上的要求外，`complex` 还 **必须足够高效**，否则没有使用价值。这意味着 **应该将简单的操作设置成内联的**。也就是说，在最终生成的机器代码中，一些简单操作不能以函数调用的方式实现
> 
> **定义在类内部的函数默认是内联的**
> 

**无需实参** 就可以调用的构造函数称为 **默认构造函数**。`complex()` 是 `complex` 类的默认构造函数

> [!important] 
> 
> 类的默认构造函数可以防止该类的对象处于未初始化状态
> 

负责返回复数实部和虚部的函数，即 `double real() const;` 和 `double imag() const;` 中的 `const` 说明符表示这两个函数不会修改调用的对象

有很多有用的操作并不需要直接访问 `complex` 的表现形式，因此它们的定义可以与类的定义分离开

```cpp
complex operator+(complex a, complex b) {return a += b;}
complex operator-(complex a, complex b) {return a -= b;}
complex operator-(complex a) {return {-a.real(), -a.imag()};} // 一元负号
complex operator*(complex a, complex b) {return a *= b;}
complex operator/(complex a, complex b) {return a /= b;}
```

> [!tip] 
> 
> 这里利用了 **C++ 实参传递是以值传递** 的方式进行的，它把实参的一份副本传递给函数，因此修改形参不会影响主调函数的实参，并可以将结果作为返回值
> 

相等性比较运算符 `==` 和 `!=` 的定义非常直观了

```cpp
bool operator==(complex a, complex b)
{
    return a.real() == b.real() && a.imag() == b.imag();
}

bool operator!=(complex a, complex b)
{
    return !(a == b);
}
```

完成上述定义后，`complex` 就可以像使用 `int` 那样使用 `complex` 

```cpp
int main() {
    complex a{2.0, 3.0}, b{1/a};

    complex c {a + b * complex{1, 2.3}};

    std::cout  << c.real() << "+" << c.imag() << "i" << std::endl;
    return 0;
}
```

#### 容器类

**容器是一个包含若干元素的对象**。就像标准库中 `vector` 类的对象，每个对象都可容纳若干个元素，因此 `vector` 的每个对象都是容器。所以，称 `vector` 是一种容器类型。例如，我们将标准库中 `vector` 简化设计如下 `Vector` 类

```cpp
class Vector {
	private:
		int sz;    // 向量维度
		int *elem;  // 向量的每个元素
	public:
		// 构造函数
		Vector(int s) {
			sz = s;
			elem = new int[s];
		}

		// 提供下标(索引)访问第 i 个元素
		int&operator[](int i) {
			return elem[i];
		}
		// 获取向量的维度
		int size() {
			return sz;
		}
};

int main() {
    Vector v(6);  // 创建一个 vector 对象，包含 6 个元素
}
```

下图解释了 `Vector` 对象的含义

![[Pasted image 20250122192945.png]]

上述 `Vector` 作为 `double` 类型的容器，完成了基本功能的设计。然而，存在一些问题

##### 越界访问检查

下标访问运算符没有做边界访问的检查，如果发生越界访问可能会造成程序的不稳定。因此，我们应该对 `operator[]` 操作进行限制

```cpp
#include <stdexcept>
using std::out_of_range;

double &Vector::operator[](int i)
{
	// 越界条件
	if (i < 0 || i >= size())
	{
		throw out_of_range("operator[]");
	}
	return elem[i];
}
```

在使用时，主调函数应该尝试捕获 `out_of_range` 异常类

```cpp
void f(Vector &v)
{
    //...
    try { // 此处开始出现的异常将被后面定义的处理模块处理
        v[v.size()] = 7; // 试图访问 v 末尾之后的位置

    } catch(out_of_range){
        // 在此处处理越界错误
    }
}
```

##### 建立正确的不变式

>[!tip] 前置条件
>
> `Vector` 对象作为容器，进行索引的时候假设索引值为 `[0, v.size())`，这是 **前置条件**。在前置条件未得到满足时，函数不应该执行
> 

根据 `Vector` 的定义，`operator[]()` 函数作用于 `Vector` 类型的对象且只有在 `Vector` 的 **成员有合理的值** 时它才有意义。特别的，`elem` 是一个指向一个含有 `sz` 个 `double` 的数组。但是，这只是注释中说明而已。

> [!tip] 不变式
> 
> 对于类来说，**假定某件事情为真的声明** 称为类的不变式。建立类的不变式是构造函数的任务
> 
> + 确保成员变量被初始化为正确的值，从而使得成员函数可以依赖该不变式
> + 确保成员函数退出时，不变式仍然成立
> 

我们的 `Vector` 类的构造函数只履行了部分职责：正确初始化了成员变量，但是没有检查传入的实参是否有效。如果传入的实参是负数，例如

```cpp
Vector v(-27); // 这条语句带来混乱
```

可以是标准异常库中的 `length_error` 报告元素的数目必须为非负数

```cpp
Vector::Vector(int s)
{
    if (s < 0) 
    {
	    throw std::length_error{"the lenght of Vector must be a nonnegative number"};
    }
    elem = new double[s];
    sz = s;
}
```

在使用 `Vector` 时，主调函数应该捕获 `length_error`。由于 `new` 运算符在分配内存时，如果没有足够的空间会抛出 `bad_alloc` 异常，也应该捕获

```cpp
void test(){
    try 
    {
        Vector v(-27);
    } 
    catch(std::length_error e)
    {
        // 处理长度为负值问题
    }
    catch(std::bad_alloc e)
    {
        // 处理内存分配失败问题
    }
}
```

> [!tip] 
> 
> 通常情况下，当遇到异常后函数就无法继续完成工作了。此时，处理异常的含义仅仅是做一些简单的局部资源清理，然后重新抛出异常
> 

##### 析构函数

现在我们的 `Vector` 还有一个致命的缺陷：使用 `new` 分配了内存但是没有释放这些内存

> [!attention] 
> 
> 虽然 C++ 定义了垃圾回收接口，可以将未使用的内存提供给新对象，但是 C++ 并不保证垃圾收集器总是可用的
> 

因此，我们迫切需要一种机制能够确保构造函数分配的内存一定会被销毁，这种机制称为 **析构函数**

```cpp
#include <iostream>
#include <stdexcept>

using std::out_of_range;

class Vector{
private:
	double *elem; // 指向元素的指针
	int sz; // 元素的数量
public:
	Vector(int s);  // Vector 的构造函数，用于构建一个 Vector 创建类的不变式
	
	~Vector();  // 析构函数，释放资源
};

Vector::Vector(int s)
{
    if (s < 0) 
    {
	    throw std::length_error{"the lenght of Vector must be a nonnegative number"};
    }
    elem = new double[s];
    sz = s;
}

Vector::~Vector()
{
    delete[] elem;
}
```

> [!tip] 
> 
> 析构函数的命名规则就是在在类名前面加上一个按位取反运算符 `~`，语义上它是构造函数的补充
> 

`Vector` 的构造函数使用 `new` 运算符从堆分配一些内存空间，析构函数则使用 `delete` 运算符释放该空间以达到清理资源的目的。这些都 **无需使用者干预**，它们只需要像使用普通内置类型一样使用 `Vector` 对象即可

```cpp

void fct(int n)
{
    Vector v(n);
    // .. 使用 v
    {
        Vector v2(2 * n);
        // 使用 v 和 v2
    } // 此处 v2 自动销毁
    // 使用 v
}  // 此处 v 自动销毁
```

>[!tip] 
>
> 构造函数和析构函数的机制是资源管理技术的基础
>
> + 构造函数申请资源
> + 析构函数释放资源
>
>这就是所谓的 **数据句柄模型**，常用来管理对象生命周期中大小会发生变化的数据。在构造函数中请求资源，在析构函数中释放它们的技术称为 **资源获取即初始化(RAII)**
>

##### 初始化容器

容器的作用就是保存元素，因此我们可能需要一种便利的方式将元素存入容器中。一种方法就先用若干元素创建一个 `Vector`，然后再次为这些元素赋值。但是，这不是一个好办法，C++ 中有更好的途径：

> [!tip] 初始化器列表构造函数
> 
> 接收参数类型为 `std::initialize_list` 构造函数称为初始化器列表构造函数，可以使用一个初始化器列表对容器进行初始化
> 
> 

> [!tip] 末尾最佳的方式初始化
> 
> 实现一个 `push_back()` 方法，用于在序列的末尾添加一个新元素
> 

```cpp
#include <iostream>
#include <stdexcept>

using namespace std;

class Vector{
private:
	double *elem; // 指向元素的指针
	int sz; // 元素的数量
public:
	// ....
    
    Vector(initializer_list<double>); // 列表初始化构造函数
    // ...
    
    void push_back(double); // 末尾添加一个元素，容器长度加1
};
```

### 抽象类

我们定义的 `complex` 和 `Vector` 等类型之所以被称为 **具体类型**，是因为它们的表现形式属于类定义的一部分，可以像使用内置类型那样使用具体类。相反，**抽象类** 则将使用者与类的实现细节完全隔离开

> [!tip] 
> 
> 为了做到将类的使用者与类的实现细节分离开，我们将 **分离接口和表现形式**。由于我们对抽象类的表现形式一无所知，甚至对它的大小也不了解，所以必须放弃纯局部变量，转而 **使用自由存储为对象分配空间**，然后通过引用或者指针访问对象
> 

为了规范容器的实现，我们定义了一个 `Container` 的抽象类，它规定了容器应该实现哪些接口

```cpp

class Container {
public:
    virtual double& operator[](int i) = 0;  // 纯虚函数，抽象方法。子类必须实现该方法
    virtual int size() const = 0; // 抽象方法，子类必须实现盖房
    virtual ~Container() {};  // 虚析构函数。子类可以重写也可以不重写
};
```

对于后续定义的特定容器来说，`Container` 类是一个纯粹的接口。关键字 `virtual` 的意思就是：可以在 `Container` 的派生类中重新定义。`Container` 的派生类负责实现 `Container` 接口

> [!tip] 虚函数
> 
> 使用 `virtual` 关键字声明的成员函数称为 **虚函数**
> 

> [!tip] 纯虚函数
> 
> 虚函数后面加上的 `= 0` 说明该虚函数是 **纯虚函数**，意味着 `Container` 的派生类 **必须** 定义这个函数
> 

由于我们不知道 `Container` 的表现形式，因此我们不能直接定义一个 `Container` 对象，`Container` 只是作为接口，它的派生类负责实现 `operator[]()` 和 `size()`

> [!tip] 抽象类
> 
> 含有纯虚函数的类称为抽象类
> 

`Container` 类通常用作函数的参数

```cpp
void use(Container& container) {
    const int sz = container.size();
    for (int i = 0; i < sz; i++) {
        container[i] *= 2;
    }
}
```

函数 `use()` 可以完全忽视实现细节的情况下使用 `Container` 接口。在 `use()` 中，并不知道使用的是哪个类型提供的 `size()` 和 `operator[]()`

> [!tip] 多态类型
> 
> 如果一个类负责为其他一些类提供接口，那么我们把负责提供接口的类称为多态类型
> 

`Container` 作为抽象类是没有构造函数，毕竟它没有数据需要初始化。`Container` 含有一个析构函数，而且该析构函数是 `virtual` 的。

> [!tip] 
> 
> 由于抽象类需要通过指针或引用操纵，而当我们试图通过一个指针销毁 `Container` 时，我们并不清楚它的实现部分到底拥有那些资源
> 

我们可以使用一个具体类实现抽象类 `Container` 接口所需的函数

```cpp
class Vector : public Container {  
    double *elem;  
    int sz;  
public:  
    Vector(const int size) {  
        if (size <= 0) {  
            throw std::length_error("size must be greater than zero");  
        }  
        elem = new double[size];  
        sz = size;  
    }  
    ~Vector() {  
        std::cout << "Vector deleted" << std::endl;  
        delete[] elem;  
    }  
  
    double& operator[](const int i) {  
        if (i < 0 || size() <= i) {  
            throw std::out_of_range("Vector::operator[]");  
        }  
        return elem[i];  
    }  
    int size() const override {return sz;};  
};
```

`:public` 表示 `Vector` 派生自 `Container`。我们可以称 `Container` 是 `Vector` 基类；`Vector` 是 `Container` 的子类

> [!tip] 
> 
> 派生类从基类中继承成员，所以我们把基类和派生类的这种关系称为 **继承**
> 

`Vector` 的成员 `operator[]()` 和 `size()` **覆盖(override)** 了基类中的 `operator[]()` 和 `size()`。析构函数 `~Vector()` 覆盖了基类的析构函数 `~Container()`

> [!tip] 
> 
> 如果成员包含其他类的对象，`~Vector()` 会隐式调用成员对象的析构函数
> 

现在我们使用标准库中的`list` 实现 `Container` 接口

```cpp
class List : public Container
{
    std::list<double> ld;  // 一个 double 类型的标准库 list
public:
    List() {}  // 空列表
    List(std::initializer_list<double> lis) :ld{lis}{}
    ~List(){}

    double & operator[](int i);
    int size() const { return ld.size();}
};

double & List::operator[](int i)
{
    for(auto &x :ld)
    {
        if(i == 0) return x;
        i--;
    }
    throw std::out_of_range("List container");
}
```

我们可以创建一个 `List`，然后让 `use()` 使用它。下面代码显示了让 `use()` 使用了两个类型的对象

```cpp
int main() {
    Vector v(6);
    use(v);
    List l {1,2,3,4,5,6};
    use(l);
}
```

这段代码的关键在于 `use(Container &)` 并不清楚它的实参是 `Vector` 还是 `List`。这样，无论 `List` 的实现发生了什么改变还是使用了 `Container` 的另一个派生类，都不需要重新编译 `use(Container &)`

> [!tip] 
> 
> 我们只能通过指针或引用操作对象
> 

### 虚函数

继续考虑 `Container` 的用法

```cpp
#include <iostream>

void use(Container &c)
{
    const int sz = c.size();

    for(int i = 0; i != sz; i++)
    {
        std::cout << c[i] << '\n';
    }
}
```

> [!question] 
> 
> `use()` 中的 `c[i]` 是如何解析都正确的 `operator[]()`？
> + `use(v)`：必须调用 `Vector` 的 `operator[]()`
> + `use(l)`：必须调用 `List` 的 `operator[]()`
> 

为了解决上述问题，`Container` 对象就必须包含一些有助于它在运行时选择正确函数的信息。**编译器常见的解决方式时将 _虚函数的名字_ 转换成 _函数指针表_ 中对应的 _索引值_**，这张表称为 **虚函数表**，简称 `vtbl`。每个虚函数的类都有它自己的 `vtbl` 用于辨识寻函数，其工作机制如下

![[Pasted image 20250123134457.png]]

即使主调函数不清楚对象的大小和数据布局，`vtbl` 中的函数也能确保对象正确使用。主调函数的实现只需要知道 `Container` 中 `vtbl` 的指针的位置以及每个虚函数对应的索引就可以了

这样的虚函数调用机制与普通的函数调用机制的效率非常接近，而它的空间开销包括两部分：如果类包含虚函数，则该类的每个对象需要一个额外的指针；另外每个这样的类都需要一个 `vtbl`

### 类层次

`Container` 是一个简单的类层次的例子，所谓 **类层次** 是指 **通过派生创建一组类，在框架中有序排列**

**类层次表示具有层次关系的概念**。例如，消防车是卡车的一种，卡车是车辆的一种；笑脸是一个圆，圆是一个形状

![[Pasted image 20250123134855.png]]

上图中的箭头表示继承关系，由子类指向父类。例如，`Circle` 类派生自 `Shape` 类

上面的这个简单图列可以写成代码。首先，需要说明一个类，令其定义所有这些形状的公共属性


```cpp
struct Point
{
    int x, y;
};

class Shape
{
public:
    virtual Point center() const = 0; // 虚函数
    virtual void move(Point to) = 0;

    virtual void draw() const = 0; // 在当前画布上绘制
    virtual void rotate(int angle) = 0;

    virtual ~Shape(){}  // 析构函数
};
```

这个接口显然是一个抽象类：对于每种 `Shape` 来说，它们的表现形式基本上各不相同。这样就可以编写函数令其操纵由形状指针组成的向量

```cpp
void rotate_all(vector<Shape *> & v, int angle)  // 将 v 的元素按照自定角度选择
{
    for(auto p : v)
        p->rotate(angle);
}
```

要定义一种具体的形状，首先必须指明它是一个 `Shape`，然后在规定其特有的属性，包括虚函数

```cpp
class Circle: public Shape
{
private:
    Point x;  // 圆心
    int r;  // 半径
public:
    Circle(Point point, int radius); // 构造函数
    
    Point center() const {return x;} // 返回圆心
    void move(Point to) {x = to;} 

    void draw() const;

    void rotate(int){}
}
```

继续构造 `Smiley` 类

```cpp 
class Smiley:public Circle  // 使用 圆 作为一个笑脸的基类
{
private:
    vector <Shape *> eyes; // 眼睛, 通常包含两只眼睛
    Shape * mouth; 

public:
    Smiley(Point point, int radius): Circle{point, radius}, mouth{nullptr} {}

    ~Smiley()
    {
        delete mouth;
        for(auto p :eyes) delete p; // 调用 Shape::~Shape()
    }

    void move(Point to);
    void draw() const;
    void rotate(int);

    void add_eye(Shape *s) {eyes.push_back(s);}

    void set_mouth(Shape *s); 
    virtual void wink(int i); // 眨眼数 i
    // ...
};
```

接下来，通过调用 `Smiley` 的基类 `Circle` 的 `draw()` 和 `Smiley` 的成员 `eye` 的 `draw()` 来定义 `Smiley::draw()`

```cpp
void Smiley::draw() const
{
    Circle::draw();
    for(auto p : eyes)
        p->draw();
    mouth->draw();
}
```

> [!attention] 
> 
> 注意，`Smiley` 把它的 `eyes` 放在标准库 `vector` 中，然后在析构函数里把它们释放掉了
> 
> `Shape` 的析构函数是虚函数，`Smiley` 的析构函数覆盖了它
> 

对于抽象类来说，因为其派生类的对象通常是通过抽象基类的接口操纵的，所以基类中必须含有一个虚析构函数。当使用基类的指针释放派生类对象时，虚函数调用机制能够确保我们调用正确的析构函数，然后析构函数再隐式调用其他基类的析构函数和成员的析构函数

> [!tip] 
> 
> 抽象类的派生类是通过抽象类的指针或引用操纵的，因此抽象类中必须含有一个虚析构函数
> 

通过派生类的方式定义新类时，可以向其中添加数据成员或者新的操作，或者都添加。总的来说类层次提供了两变量

> [!tip] 接口继承
> 
> 派生类对象可以在任何需要基类对象的地方使用。基类看起来就是派生类的接口
> 

> [!tip] 实现继承
> 
> 基类负责提供简化派生类实现的函数或数据
> 

具体类其行为非常类似于内置类型：将其定义为局部变量，通过名字访问它们，随意拷贝它们，等等。类层次中的类则不同：更倾向于使用 `new` 在自由存储中为其分配空间，然后通过指针或者引用访问它们

例如，我们设计如下的一个函数，它首先冲输入流中读入描述形状的数据，然后构造对应的 `Shape` 对象

```cpp
enum class Kind {circle, triangle, smiley};

Shape * read_shape(istream& is)
{
    
    Kind k;
    // 从 is 中读取形状描述信息，找到形状的种类 k
    switch (k)
    {
    case Kind::circle:
        Point p;
        int r;
        // 读入 circle 数据 {Point, int} 到 p 和 r
        return new Circle{p, r};
    case Kind::triangle:
        Point p1, p2, p3;
        // 读入 triangle 数据 {Point, Point, Point} 到 p1, p2, p3
        return new Circle{p, r};
    case Kind::smiley:
        Point p;
        int r;
        Shape * e1, *e2, *m;
        // 读入 smiley 数据 {Point, int, Shape, Shape, Shape} 到 p, r, e1, e2, m
        Smiley *ps = new Smiley{p, r};
        ps->add_eye(e1);
        ps->add_eye(e2);
        ps->set_mouth(m);
        return ps;
    }
}
```

程序可以以下面的方式使用该函数

```cpp
void draw_all(vector<Shape *> &v)
{
    for(auto p : v)
        p->draw();
}

void user()
{
	std::vector<Shape *> v;
    while (cin)
    {
       v.push_back(read_shape(cin));
    }
    draw_all(v);    // 对每个元素调用 draw()
    rotate_all(v, 45);  // 对每个元素调用 rotate(45)
    for(auto p : v) delete p;  // 最好删除所有元素
}
```

`user()` 并不知道它操纵的具体是哪种类型。`user()` 的代码也只需要编译一次就可以使用随后添加到程序中的新 `Shape`

在 `user()` 外没有任何指针指向这些形状，因此 `user()` 需要负责释放掉它们。这些工作由 `delete` 运算符完成并且完全依赖于 `Shape` 的虚析构函数

> [!tip] 
> 
> **因为是虚析构函数，所以 `delete` 会调用最终派生类的析构函数**。这一点非常关键：派生类可能有很多种需要释放的资源
> 

这里有两个漏洞：使用者可能未能 `delete` 掉 `read_shape()` 返回的指针；`Shape` 指针容器的拥有者可能没有 `delete` 指针所指的对象。因此，**函数返回一个指向自由存储上的对象的指针是非常危险的**。一种解决方案是不要返回一个裸指针，而是返回标准库 `unique_ptr` 并把 `unique_ptr` 放在容器中

```cpp
#include <memory>
using namespace std;

enum class Kind {circle, triangle, smiley};

unique_ptr<Shape> read_shape(istream& is)  // 从输入流 is 中读取形状的描述信息
{
    
    Kind k;
    // 从 is 中读取形状描述信息，找到形状的种类 k
    switch (k)
    {
    case Kind::circle:
        Point p;
        int r;
        // 读入 circle 数据 {Point, int} 到 p 和 r
        return unique_ptr<Shape>{new Circle{p, r}};
    case Kind::triangle:
        Point p1, p2, p3;
        // 读入 triangle 数据 {Point, Point, Point} 到 p1, p2, p3
        return unique_ptr<Shape>{new Circle{p, r}};
    case Kind::smiley:
        Point p;
        int r;
        Shape * e1, *e2, *m;
        // 读入 smiley 数据 {Point, int, Shape, Shape, Shape} 到 p, r, e1, e2, m
        Smiley *ps = new Smiley{p, r};
        ps->add_eye(e1);
        ps->add_eye(e2);
        ps->set_mouth(m);
        return unique_ptr<Shape>{ps};
    }
}


// ################################################################

void rotate_all(vector<unique_ptr<Shape>> & v, int angle)  // 将 v 的元素按照自定角度选择
{
    for(auto &p:v)  // unique_ptr 不能被复制，所以必须使用引用
        p->rotate(angle);
}

void draw_all(vector<unique_ptr<Shape>> &v)
{
    for(auto &p : v)
        p->draw();
}

void user()
{
	vector<unique_ptr<Shape>> v;
    while (cin)
    {
       v.push_back(read_shape(cin));
    }
    draw_all(v);    // 对每个元素调用 draw()
    rotate_all(v, 45);  // 对每个元素调用 rotate(45)
}// 所有形状被隐式销毁
```

## 模板

显然，我们并不一定总是使用 `double` 向量。向量是一个通用的概念，不应该局限于浮点数。因此，向量的元素类型应该独立表示。

一个 **模板** 就是一个类或一个函数，但需要我们 **用一组类型** 或者值 **对其进行参数化**。使用模板表示那些通用的概念，然后通过指定实参生成待定的类型或函数

对于我们之前使用的 `double` 向量，只要将其改为 `template` 并用一个形参替代特定类型 `double`，就能将其泛化成任意类型的向量了

```cpp
template<typename T>
class Vector
{
private:
    T *elem;  // elem 指向含有 sz 个 T 类型的元素的数组
    int sz;
public:
    Vector(int s);  // 构造函数，用于建立不变式，请求资源
    ~Vector(){delete[] elem;}  // 析构函数，释放资源

    Vector(const Vector& a); // 拷贝构造函数
    Vector & operator=(const Vector& a); // 拷贝赋值运算符

    Vector(Vector&& a); // 移动构造函数
    Vector & operator=(Vector&& a); // 移动赋值运算符

    T& operator[](int i);
    const T& operator[](int i) const;
    int size() const {return sz;}
}
```

类前缀 `template<typename T>` 指明 `T` 是该声明的形参，它是数学上对所有 `T` 或对所有类型  `T` 的 C++ 表达

成员函数的定义方式与之类型

```cpp
template<typename T>
Vector<T>::Vector(int s)
{
    if (s < 0) throw Negative_size{};
    elem = new T[s];
    sz = s;
}

template<typename T>
const T& Vector<T>::operator[](int i) const
{
    if (i < 0 || size() <= i) throw out_of_range{"Vector::operator[]"};
    return elem[i];
}
```

这样，我们定义 `Vector` 对象的方式变为

```c
Vector<char> vc(200);  // 定义含 200 个字符的向量
Vector<string> vs(17); // 定义含 17 个字符串的向量
Vector<list<int>> vli(45); // 定义含 45 个整数列表的向量
```

> [!tip] 
> 
> `Vector<list<int>>` 表示嵌套模板实参的结束，并不是输入运算符被放错的地方
> 

