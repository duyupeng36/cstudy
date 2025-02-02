# 构造与清理

我们已经接触过构造函数和析构函数了。其中 **构造函数用于指出一个类对象应该如何初始化**。与构造函数对应，**析构函数用来确保对象销毁时进行恰当的清理操作**

> [!tip] 
> 
> 创建一个对象时，会自动调用构造函数。当销毁一个对象时，会自动调用析构函数
> 

C++ 中有效的资源管理技术都依赖于构造函数和析构函数这对操作。下面介绍构造/析构函数的更多内容

## 构造函数和析构函数
### 构造函数与不变式

> [!tip] 构造函数
> 
> 与类名相同的成员函数称为构造函数。其特点
> + 与类名相同
> + 没有指出返回类型
> + 参数列表后可以跟随初始化列表
> 
> 除了上述 $3$ 个特点外，其余与普通函数是一样的
> 

> [!warning] 请注意：与类名相同的只能是构造函数
> 
> 数据成员、普通成员函数以及成员类型都不允许与类名相同
> 

下面的示例代码给出了错误提示

```cpp
struct S
{
	S();                 // OK：构造函数
	void S(int);         // 错误：构造函数没有返回类型
	int S;               // 错误：类名只能表示构造函数
	enum S{foo, bar};    // 错误：类名只能表示构造函数
}
```

构造函数的任务就是初始化类对象。一般而言初始化操作必须建立一个 **类不变式**

> [!tip] 不变式
> 
> 所谓类不变式就是通过对象调用成员函数时必须保持不变的条件
> 

比如，如果我们需要创建一个向量类。表现形式至少包含两个：**指向元素的指针** 和 **元素的个数**。该向量类的不变式为：**指向元素的指针必须指向一片受进程管理的内存空间**；**元素个数必须是非负的**

```cpp
class Vector
{
public:
    Vector(int s);
    double& operator[](int i) { return elem[i]; }
    int size() { return sz; } 
private:
    double* elem;  // 指向一个存放 sz 个 double 元素的数组
    int sz;  // sz 非负
};
```

> [!tip] 
> 
> 这里我们通过注释陈述了类的不变式。构造函数必须保证这些不变式成立
> 

为了使上述类的不变式成立，则 `Vector` 的构造函数定义如下

```cpp
Vector::Vector(int s)
{
    if (s < 0)  // 不变式 sz 非负无法建立
		// 抛出异常
        throw std::length_error{"Vector constructor: negative size"};
    elem = new double[s];
    sz = s;
}
```

> [!attention] 
> 
> 如果构造函数无法建立不变式，对象就不应该被创建。因此，构造函数应该抛出异常
> 

如果构造函数 **无法建立不变式**，则 **不应该创建对象** 且必须 **确保没有资源泄漏**。确保不存在资源泄漏的情况就需要提供个在合适的时候回收资源的函数，称为 **析构函数**

> [!tip] 资源
> 
> 需要获取并在用完后最终归还或释放的任何东西都是 **资源**
> 
> + 例如，内存、锁、文件句柄、线程句柄等
> 

### 析构函数与资源

构造函数初始化对象。它创建供成员函数进行操作的环境。创建环境时需要获取资源，如文件、锁、内存等，这些资源在使用后必须释放

> [!tip] 
> 
> 有限资源使用完成后必须释放，否则可能导致程序异常
> 

为了确保类释放这些资源，需要一个可以自动触发的函数，保证在对象销毁时被调用，这样的函数称为类的 **析构函数**。析构函数与类的构造函数做相反的操作，因此析构函数的名字就是取反符号(`~`)后面跟上类名即可。

> [!tip] 
> 
> 析构函数与构造函数做的事情时相反的。析构函数名就是取反符号(`~`)后面紧跟类名
> 

```cpp
class Vector
{
public:
    Vector(int s):elem{new double[s]}, sz{s}{} // 构造函数：获取资源
    ~Vector() {delete[] delem;}                // 析构函数：释放资源
    // ...
private:
    double* elem;  // 指向一个存放 sz 个 double 元素的数组
    int sz;  // sz 非负
};
```

例如下面的代码

```cpp
Vector * f(int s)
{
    std::cout << "f() start" << std::endl;
    Vector v(s);  // 创建一个 Vector 对象，并在返回时销毁
    return new Vector(s + s);  // 在堆上创建一个 Vector 对象，并返回
}

int main()
{   std::cout << "main() start" << std::endl;
    Vector * pv = f(5);  // pv 指向一个 Vector 对象
    std::cout << "f() end" << std::endl;
    delete pv;  // 释放 pv 指向的对象
    std::cout << "main() end" << std::endl;
    return 0;
}
```

注意，我们在 `Vector` 的构造函数和析构函数中都添加了额外输出。该例程允许的输出如下

```cpp
main() start
f() start
Vector::Vector()  // f() 在栈上创建 Vector 对象时调用
Vector::Vector()  // f() 在堆上创建 Vector 对象时调用（new）
Vector::~Vector() // f() 返回，清除栈空间内容时 Vector 对象被清除时调用
f() end
Vector::~Vector()  // main() 中使用 delete 清除 Vector 对象时调用
main() end
```

> [!tip] 这种基于构造函数/析构函数的资源管理风格称为 **资源获取即初始化**(`RAII`)
> 
> 栈空间上创建对象和使用 `new` 在堆空间上创建对象的构造函数都会自动触发执行
> 
> 栈空间上的对象被自动清除和使用 `delete` 归还堆空间内存时对象的析构函数都会自动触发执行
> 

### 基类和成员析构函数

构造函数和析构函数可以很好的与 [[C++：抽象机制概览#类层次]]] 配合。构造函数会 **自顶向下地创建一个对象** 

> [!tip] 创建对象的过程
> + 首先，构造函数调用其基类的构造函数
> + 然后，调用成员的构造函数
> + 最后，执行自身的函数体
> 

析构函数则 **按相反的顺序拆除一个对象**

> [!tip] 销毁对象的过程
> +  首先，析构函数执行自身地函数体
> + 然后，调用成员的析构函数 
> + 最后，调用基类的析构函数
> 

下面的例程展示了对象的创建和销毁的过程

```cpp
#include <iostream>

using namespace std;

class Book {
    public:
        Book() {
            cout << "Book()" << endl;
        }
        ~Book() {
            cout << "~Book()" << endl;
        }
};

class Person {
    public:
        Person() {
            cout << "Person()" << endl;
        }
        ~Person() {
            cout << "~Person()" << endl;
        }
};


class Student: public Person {
    public:
        Student() {
            cout << "Student()" << endl;
        }
        ~Student() {
            cout << "~Student()" << endl;
        }
    private:
        Book book;
};

int main() {
    cout << "main() start" << endl;
    Student *s = new Student();
    delete s;
    cout << "main() end" << endl;
    return 0;
}
```

编译运行的结果如下

```
main() start
Person()   # 基类构造函数
Book()     # 成员构造函数
Student()  # 本类的构造函数
~Student() # 本类的析构函数
~Book()    # 成员的析构函数
~Person()  # 基类的析构函数
main() end
```

> [!attention] 
> 特别是，一个 `virtual` 基类必须在任何可能使用它的基类之前构造，并在它们之后销毁。这种顺序保证了一个基类或者一个成员不会在初始化完成之前或已销毁之后使用
> 

构造函数 **按声明顺序** 执行成员和基类的构造函数：如果两个构造函数使用了不同的顺序，析构函数不能保证按构造的相反顺序进行销毁

如果一个类的使用方式要求有默认构造函数，或者类没有其他构造函数，则编译器会尝试生成一个默认构造函数

```cpp
struct S {
	string s;
};

S x;  // OK:x.s 初始化为 ""
```

类似的，如果需要初始化器，可以使用逐成员初始化

```cpp
struct X{
	X(int);
};
struct S {
	X x;
};

S x1;  // 错误：没有为 x1.x 提供值
S x2{1}; // OK: x2.x 用 1 进行初始化
```

### 手动调用构造函数和析构函数

在 [[C++：类与对象#析构函数]] 中我们介绍过，当对象退出作用域或者被 `delete` 的时候，析构函数会被隐式调用

> [!warning] 
> 
> 显示调用析构函数通常是不必要的，而且会导致严重的错误
> 

在极少数的情形下，我们可能需要显示调用析构函数。考虑一个容器（例如，`std::vector`）维护一个可增长和缩减(例如，使用`push_back()` 和 `pop_back()`)的内存池。当我们添加一个元素时，容器必须对一个特定地址调用构造函数

```cpp
void C::push_back(const X& a)
{
	// ...
	new(p) X{a}; // 在地址 p 用值 a 拷贝构造一个 X，称为 放置式 new
}
```

相反地，当我们删除一个元素时，容器需要调用其析构函数

```cpp
void C::pop_back()
{
	//...
	p->~X();  // 销毁地址 p 中的 x
}
```

> [!attention] 
> 
> `p->~X()` 对 `*p` 调用 `X` 的析构函数。对正常方式(离开作用域或用 `delete`释放)销毁的对象绝不能使用这种方法
> 

如果为类 `X` 声明一个析构函数，当 `X` 的对象离开作用域或者被 `delete` 时，析构函数会自动调用。如果我们想手动管理并且避免手动调用析构函数，我们可以将析构函数声明为 `=delete` 或者声明为 `private`

```cpp
class Student: public Person {
    public:
        Student() {
            cout << "Student()" << endl;
        }
        void destroy() {
            this->~Student();
        }
    private:
        Book book;
        ~Student() {
            cout << "~Student()" << endl;
        }
};
```

这样就离开作用域和使用 `delete` 均不能析构一个对象，必须使用 `destroy()` 成员函数进行析构

> [!tip] 
> 
> 将析构函数声明为 `private` 会更灵活、更通用
> 

### virtual 析构函数

析构函数可以声明为 `virtual`，而且对于含有虚函数的类通常就应该这么做

```cpp
class Shape
{
public:
    // ...
    virtual void draw() = 0;
    virtual ~Shape() ;
};

class Circle : public Shape
{
public:
    // ...
    void draw();
    ~Circle();  // 覆盖了 ~Shape()
};
```

我们需要一个 `virtual` 析构函数的原因是，如果通常是 **通过基类提供的接口来操纵一个对象，那么通常也应该通过此接口来 `delete` 它**

```cpp
void user(Shape * p)
{
	p->draw();
	// ...
	delete p; // 调用恰当的析构函数
}
```

**假如 `Shape` 的析构函数不是 `virtual` 的，则 `delete` 在尝试调用恰当的派生类的析构函数时就会失败**。这种失败会导致被释放对象拥有的资源泄漏

## 类对象初始化

### 不使用构造函数

我们不能为内置类型定义构造函数，但能用一个恰当类型的值初始化内置类型对象

```cpp
int a{1};
char *p {nullptr};
```

> [!tip] 初始化 **无构造函数** 的类对象的方法
> 
> 逐成员初始化
> 
> 拷贝初始化：编译器会默认提供一个逐成员拷贝构造函数
> 
> 默认初始化：编译器会默认提供一个构造函数
> 

如下示例

```cpp
#include <iostream>

using namespace std;

struct Work
{
    string author;
    string name;
    int year;
};


int main() {
    Work s9{"William Shakespeare", 
		    "Romeo and Juliet", 1597};   // 逐成员初始化
    Work currently_playing {s9};         // 拷贝初始化
    Work none {};                        // 默认初始化
    return 0;
}
```

`currently_playing` 的 $3$ 个成员分别是 `s9` 的 $3$ 个成员的副本。使用 `{}` 进行默认初始化的效果是使用 `{}` 对每个成员进行初始化。因此 `none` 被初始化为 `{{}, {}, {}}`，即 `{"", "", 0}`

> [!tip] 
> 
> 如果没有声明可接受参数的构造函数，也可以完全省去初始化器
> 

```cpp
Work alpha;  // 全局变量：静态分配的，初始化与使用 {} 完全一样 

void f()
{
	Work beta;  // 局部变量：类对象成员进行默认初始化，内置类型不执行初始化
	// ...
}
```

上述代码的规则不像我们希望的那么清晰。我们在注释中给出了全局变量和局部变量的初始化规则

> [!tip] 
> 
> **静态分配的对象**，这种初始化方式与使用 `{}` 完全一样，因此 `alpha` 的值是 `{"", "", 0}`
> 
> **对局部变量和自由存储空间对象**，只对类对象成员进行默认初始化，内置类型的成员是不进行初始化，因此 `beta` 的值是 `{"", "", unknow}`
> 

造成上述复杂性的原因是为了提高极少数关键情况下的性能。例如

```cpp
struct Buf
{
    int count;
    char buf[16 * 1024];
};
```

定义一个 `Buf` 类型的局部变量，将其作为输入操作的目标，在进行输入操作前不对它进行初始化。大多数局部变量初始化的性能不是关键，而未初始化局部变量是主要的错误来源之一。如果我们希望保证局部变量被初始化或者只是不希望意外结果发生，可以提供一个初始化器

```cpp
Buf buf0;  // 静态全局变量：默认初始化， count 和 buf 全部初始化为 0

void f() 
{
	Buf buf1;   // 元素未初始化
	Buf buf{};  // count 和 buf 全部初始化为 0
	
	int *p1 = new int;   // *p1 未初始化
	int *p2 = new int{}; // *p2 初始化 0
	int *p3 = new int{7} // *p3 初始化 7
}
```

> [!important] 
> 
> **显然，只有当我们能访问成员时，逐成员初始化才能被执行**。如果一个类有私有的非 `static` 数据成员，就需要一个构造函数来进行初始化
> 

### 使用构造函数

当逐成员拷贝不能满足需求时，我们可以定义构造函数来初始化对象。特别是，构造函数常用来建立类的不变式并获取必要的资源

如果我们为类声明了构造函数，每个对象就会使用某些构造函数。如果在创建对象时没有提供构造函数所要求的恰当的初始化器，就会导致错误

```cpp
struct X
{
    X(int);
};

X x0;         // 错误： 无初始化器
X x1{};       // 错误：初始化器为空
X x2{42};     // 正确
X x3 {"two"}; // 错误：错误的初始化器类型
X x4 {1, 2};  // 错误：初始化器数目错误
X x5 {x4};    // 正确：拷贝构造函数是隐式定义的
```

> [!attention] 
> 
> 当定义了一个可接受参数的构造函数之后，默认构造函数就不存在了。但是，拷贝构造函数不会消失，因为 **默认假设对象是可拷贝的**
> 

如果假设导致错误的情况下，可以明确地禁止拷贝或者声明拷贝构造函数为 `private`

> [!seealso] 
> 
> 如何禁止拷贝参考 [[C++：拷贝语义]] 
> 

我们使用 `{}` 语法表明正在进行初始化而不是赋值。只要是构造对象的位置，都可以使用 `{}` 初始化语法为构造函数提供参数

```cpp
struct Y : X
{
    X m {0};                // 数据成员 m；{0} 类内初始化器，为 .m 提供初始值
    
    Y(int a):X{a}, m{a} {}; // 构造函数；初始化基类 X 和数据成员 .m
    Y():X{0} {};            // 默认构造函数；初始化基类 x
};

X g{1};  // 初始化全局变量

void f()
{
    X de1 {};            // 错误：没有提供默认值
    Y de2 {};            // 正确：使用默认构造函数
    X *p {nullptr}; 
    X var {2};           // 初始化局部对象
    p = new X{4};        // 初始自由空间中的对象
    X a[] {1,2,3};       // 初始化数组
    vector<X> v {1,2,3}; // 初始化向量
}
```

> [!tip] 
> 
> `{}` 初始化是通用初始化：可以使用在任何需要初始化的位置。并且 `{}` 初始化是一致的：无论在哪里用语法 `{v}` 将类型 `X` 的对象初始化为值 `v`，都会创建相同的值 `X{v}`
> 

C++ 还支持使用 `=` 和 `()` 初始化语法，但是这两个语法不是通用的

```cpp
struct Y : X
{
    X m;
    Y(int a) : X(a), m=a { } // 错误：不能用 = 进行成员初始化
};

X g(1); // 初始化全局变量

void f(int a)
{
    X def();                 // 函数返回一个 X
    X *p {nullptr};
    X var = 2;               // 初始化局部变量
    p = new X = 4;           // 错误：= 不能用于 new
    X a[](1, 2, 3);          // 错误：不能用 () 初始化数组
    vector<X> v(1, 2, 3, 4); // 错误：不能用 () 初始化列表元素
}
```

初始化语法 `()` 可以确保在初始化过程中使用一个构造函数。即，对一个类，可以保证用构造函数初始化，而不会进行 `{}` 语法提供的逐成员初始化或者 `initializer_list` 初始化列表初始化

```cpp
struct S1
{
	int a, b;  // 无构造函数
}

struct S2
{
	int a, b;
	S2(int aa = 0, int bb = 0): a{aa}, b{bb} {}; // 构造函数
}

S1 x11(1, 2); // 错误：没有构造函数
S1 x12 {1, 2}; // 逐成员初始化

S1 x13(1); // 错误：没有构造函数
S1 x14{1}; // 正确: {1, 0}

S2 x21(1, 2); // 正确：使用构造函数
S2 x22{1, 2}; // 正确：使用构造函数

S2 x23(1); // 正确：使用构造函数，其中一个参数使用默认值
S2 x24{1}; // 正确：使用构造函数，其中一个参数使用默认值
```

初始化语法 `{}` 从 C++11 标准开始支持。 使用 `()` 个初始化的场景非常少，除非需要区分用一个 **元素列表初始化** 与使用 **构造函数参数列表初始化**。例如标准库 `vector`

```cpp
vector<int> v1{77}; // 用值初始化一个元素
vector<int> v2(77); // 将 77 个元素初始为 0
```

### 默认构造函数

> [!important] 
> 
> 默认构造函数是：**无参数的构造函数**。如果没有提供任何一个构造函数，编译器就会自动生成一个默认构造函数
> 

```cpp
class Vector
{
public:
	Vector(); // 默认构造函数，无参数
}
```

如果在构造对象时，**未指定参数** 或提供了一个 **空初始化列表**，则会调用默认构造函数

```cpp
Vector v1;   // 未提供参数，调用默认构造函数
Vector v2{}; // 空初始化列表，调用默认构造函数
```

> [!attention] 
> 
> 如果 **只接受一个参数的构造函数提供了一个默认参数**，那么该构造函数就成为 **默认构造函数**
> 

```cpp
class String
{
public:
	String(const char * = ""); // 只接受一个参数的构造函数并且提供了默认值
}

String s1;   // 未提供参数，调用默认构造函数，初始化为空字符串
String s2{}; // 提供空初始化列表，调用默认构造函数，初始化为空字符串
```

内置类型被认为具有默认构造函数和拷贝构造函数。内置类型的非 `static` 局部变量的默认构造函数不会被调用

> [!tip] 
> 
> 内置整数类型默认初始化为 $0$
> 
> 内置浮点数类型默认初始化为 $0.0$
> 
> 指针类型默认初始化为 `nullptr`
> 

**引用和 `const` 必须被初始化，一个包含这些成员的类不能默认构造**，除非程序员提供类内成员初始化器或定义默认构造函数来初始化它们

```cpp
int glob{9};

struct X
{
	const int a1{7};   // 正确
	const int a2;      // 需要用户自定义构造函数
	const int &r {9};  // 正确
	int & r1{glob};    // 正确
	int &r2;           // 需要用户自定义构造函数
}
```

声明数组、标准库 `vector` 以及类似的容器时可以分配一组默认初始化元素。在此情况下，被用作 `vector` 或者数组元素类型的类显然需要一个默认构造函数

> [!tip] 
> 
> 类 `X` 的对象作为容器元素且容器需要分配一组默认初始化元素时，类 `X` 需要一个默认构造函数
> 

### 初始化器列表构造函数

接受单一 `std::initilizer_list` 参数的构造函数被称为 **初始化器列表构造函数**：一个初始化器列表构造函数使用 `{}` 列表作为器初始化值来构造对象

> [!tip] 
> 
> 标准库容器都有初始化器列表构造函数、初始化器列表赋值运算符等成员
> 

```cpp
vector<double> v = {1, 2, 3.456, 99.99};

list<pair<string, string>> languages = {
	{"Nygaard", "Simula"},
	{"Richards", "BCPL"},
	{"Stroustrup", "C++"}
};

map<vector<string>, vector<int>> years = {
	{{"Maurice", "Vincent", "Wikes"}, {1913, 1945, 1951, 1967, 2000}},
	{{"Martin", "Richards"}, {1968, 1979, 1982}},
	{{"David", "John", "Wheeler"}, {1927, 1947, 1951, 2004}}
};
```

想要使用接受一个 `{}` 列表进行初始化的机制，就要定义一个接受 `std::initilizer_list` 类型参数的函数，通常是一个构造函数

```cpp
void f(initializer_list<int>);
f({1, 2, 3, 4, 5});
f({});
f{1, 2}; // 错误，缺少函数调用运算符()
```

> [!tip] 
> 
> 一个初始化器列表的 **长度可以是任意的**，但元素 **必须是同构的**。即，所有的元素类型都必须是模板参数类型 `T`，或可以隐式转换为 `T`
> 

当存在初始化器列表构造函、默认构造函数和普通构造函数，在使用 `{}` 初始化语法时如何调用？

```cpp
struct X
{
    X(initializer_list<int>){
	    cout << "X::X(initializer_list)" << endl;
	};                                     // 初始化器列表构造函数
	
    X(){cout << "X::X()" << endl;};        // 默认构造函数
    
    X(int){cout << "X::X(int)" << endl;};  // 普通构造函数
};

int main()
{
    X x0{};  // 选择默认构造还是初始化器列表构造？（默认构造）
    X x1{1}; // 选择 X(int) 还是 X(initiliezr_list) 构造？（初始化器列表构造）
    return 0;
}
```

> [!important] 使用 `{}` 初始化语法时构造函数的调用规则：默认构造函数 > 初始化器列表构造函数 > 普通构造函数
> 
> 如果默认构造函数或初始化器列表构造函数都匹配，优先选择默认构造函数
> + 只要有可能，就选择最简单的构造函数
> + 如果定义的初始化器列表构造函数在接受空列表时做的事情与默认构造函数不一样，那么我们可能犯了设计错误
> 
> 如果初始化列表构造函数和一个普通的构造函数都匹配，优先选择初始化器列表构造函数
> 

如果我们真的希望调用接受一个或两个整型参数的构造函数，就必须使用 `()` 语法

```cpp
vector <int> v1(1);    // 构造一个元素，具有默认值 0
vector <int> v2(1, 2); // 构造一个值为 2 的元素
```

现在，还存在一个问题：如何访问 `initilizer_list` 中的元素？C++ 为 `initilizer_list<T>` 提供了 `begin()` `end()` 和 `size()` 成员函数用于访问 `initilizer_list` 中的元素

```cpp
void f(initializer_list<int> arg)
{
    for(int i = 0; i < arg.size(); i++)
    {
        cout << arg.begin()[i] << endl;  
        // 因为 initializer_list 不支持下标运算符
        // begin() 返回指向列表中第一个元素的指针，将其作为数组使用
    }
}
```

> [!tip] `initilizer_list<T>` 是一个指向元素类型为 `T` 的数组的句柄
> 
> `initilizer_list<T>` 是以传值的方式传递的。这是重载解析规则要求的，而且不会带来额外开销，因为 `initilizer_list<T>` 对象是一个小句柄，指向一个元素类型为 `T` 的数组
> 

`begin()` 返回指向  `initilizer_list<T>` 中第一个元素的指针，可以按照指针的方式使用 `begin()` 的返回值

```cpp
void f2(initializer_list<int> arg)
{
    for(auto p = arg.begin(); p != arg.end(); p++)
    {
        cout << *p << endl;
    }
}

void f3(initializer_list<int> arg)
{
    for(auto v : arg)
    {
        cout << v << endl;
    }
}
```

> [!attention] 
> 
> 请注意：`initilizer_list<T>` 中而原始是 `const T` 的，我们不能通过指针修改
> 

```cpp 
void f2(initializer_list<int> arg){ 
	*arg.begin() = 99;  // 错误：initializer_list 中的元素是常量
}
``` 

假如 `f({1,2,3})` 调用成功，那么看起来就像 `{1,2,3}` 中的 `1` 的值会改变。这严重破坏我们的基本概念(字面值常量)

> [!attention] 
> 
> 由于 `initilizer_list` 元素是不可变的，我们不能对其使用移动构造函数
> 

一个容器可能像下面代码这样实现初始化器列表构造函数

```cpp
class Vector
{
public:
	Vector(initilizer_list<int>);
private:
	int sz;
	int *elem;
}

Vector::Vector(std::initilizer_list<int> lst):sz{lst.size()}
{
	reserve(sz); // 获取足够的内存
	uninitializer_copy(lst.begin(), lst.end(), elem);
}
```

## 成员和基类的初始化

构造函数可以建立不变式并获取资源。一般而言，构造函数是通过初始化类成员和基类完成这些工作的

### 成员初始化

考虑下面这个类，它用来保存一个小组织的信息

```cpp
#include <string>
#include <vector>

using namespace std;

class Club
{
private:
    string name; // 组织名称
    vector<string> members; // 组织成员
    vector<string> officers; // 组织干部

    Date founded; // 组织成立日期
public:
    Club(const string &, Date); // 构造函数
};
```

`Club` 的构造函数接受两个参数，分别是 `Club` 的名字和成立日期。在构造函数的定义中，通过 **成员初始化器列表** 给出成员的构造函数的参数

```cpp
Club::Club(const string &nm, Date fd)
: name{nm}, members{}, officers{}, founded{fd}  // 成员初始化器列表
{
	// ...
}
```

类自身的构造函数在其函数体执行之前先调用成员的构造函数。成员的构造函数按成员在类中的声明顺序调用，而不是按成员在初始化器列表中的顺序

> [!tip] 
> 
> 为了避免混淆，请按照成员声明的顺序组织成员初始化器列表
> 

如果一个成员的构造函数不需要参数，就不必在成员初始化器列表中提及此成员

```cpp
Club::Club(const string &nm, Date fd): name{nm}, founded{fd}
{
	// ...
}
```

> [!important] 
> 
> 尽量显示初始化成员，因为对于内置类型而言，隐式初始化的成员是为初始化的
> 

> [!warning] 
> 
> **构造函数只能初始化其成员或基类**，不会初始化其成员的成员或基类的基类
> 

```cpp
struct B
{
    B(int);
};

struct BB : B
{
};

struct BBB : BB
{
    BBB(int) : B(i){};  // 错误： B(i) 基类 BB 的基类
}; // 错误，尝试初始化基类的基类
```

> [!attention] 
> 
> 成员的初始化与成员的赋值含义是不同的。有些成员只能初始化
> + 引用和 `const` 成员必须初始化
> + 对于其他类型的成员，可以选择初始化或赋值。我们更倾向于使用初始化，它能明确表示我们正在进行初始化
> 

```cpp
class X
{
	const int i; // 必须初始化
	Club cl;
	Club &rc;  // 必须初始化
	// ...
	X(int ii, const string &n, Date d):i{ii}, cl{n, d}, rc{c}{}
}
```

### 基类初始化

**派生类的基类的初始化方式与非数据成员相同**。即，如果基类要求一个初始化器，就必须在构造函数中提供相应的基类初始化器。如果我们希望进行默认构造函数，可以显示指出

```cpp
class B1{B1();};  // 具有默认构造函数
class B2{B2(int);}; // 无默认构造函数

struct D1:B1, B2 {
	D1(int i): B1{}, B2{i}{}
};

struct D2:B1,B2
{
	D2(int i): B2{i}{}  // 隐式使用 B1{}
};

struct D3:B1, B2
{
	D3(int i) {}  // 错误：B2要求一个 int 初始化器
}
```

与成员初始化类似，**基类按声明顺序进行初始化**，建议按此顺序指定基类的初始化器。

### 委托构造函数

如果你希望两个构造函数做相同的操作，**可以重复代码**，也可以定义一个 **`init()` 函数**来执行两者相同的操作。但是，这两个方法带来冗长的代码会**影响可读性**，重复代码则**容易出错**，两者都**妨碍了可维护性**

我们可以**使用一个成员风格的初始化器**，但是使用类自身的名字，即构造函数，它会调用另一个构造函数，作为这个构造过程的一部分。这样的构造函数称为 **委托构造函数**，也称**转发构造函数**

```cpp
class X
{
	int a;
public:
	X(int x){if (0 < x && x <= max) a = x; else throw Bad_x{x};}
	X():X{42}{}  // 委托构造函数
	X(string s):X{to<int>(s)}{}
	//...
}
```

> [!attention] 
> 
> 注意，**不能同时显示和委托初始化一个成员**
> 

```cpp
class X
{
	int a;
public:
	X(int x){if (0 < x && x <= max) a = x; else throw Bad_x{x};}
	X():X{42}, a{56}{}  // 错误：不能同时显示和委托初始化一个成员
}
```

在基类和成员初始化器列表中调用其他构造函数实现委托初始化，与在构造函数体中显示调用其他构造函数是不同的。

```cpp
class X
{
	int a;
public:
	X(int x){if (0 < x && x <= max) a = x; else throw Bad_x{x};}
	X(){
		X{42};  // 简单创建了一个对象 X{42} 然后丢弃掉：显然不会初始化自己
	}  // 极大概率出错
}
```

> [!attention] 
> 
> `X{42}` 简单创建一个新的无名对象，对它不做任何处理。这种做法多半是错误的
> 

