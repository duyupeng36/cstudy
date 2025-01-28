# 构造 析构

我们已经接触过构造函数和析构函数了。其中 **构造函数用于指出一个类对象应该如何初始化**。与构造函数对应，**析构函数用来确保对象销毁时进行恰当的清理操作**

> [!tip] 
> 
> 创建一个对象时，会自动调用构造函数。当销毁一个对象时，会自动调用析构函数
> 

C++ 中有效的资源管理技术都依赖于构造函数和析构函数这对操作。下面我们探索一下构造和析构函数的执行

```cpp
/**
 * 探究 构造/析构 函数的调用顺序
*/

#include <iostream>
#include <vector>

using namespace std;

struct Tracer
{
    string mess;
    Tracer(const string& s) : mess{s} { clog << mess; }
    ~Tracer() { clog << "~" <<mess; }
};

void f(const vector<int> & v)
{
    Tracer tr{"in f()\n"};  // 进入函数时，构造一个 Tracer 对象
    for(auto x : v)
    {
        // 每次循环开始，构造一个 Tracer 对象
        Tracer tr{string{"v loop "} + to_string(x) + "\n"};
        // 每次循环结束，析构一个 Tracer 对象 
    }

    // 函数结束时，析构一个 Tracer 对象
}

int main()
{
    f({2, 3, 5});
    return 0;
}

// Output:
/*
in f()
v loop 2
~v loop 2
v loop 3
~v loop 3
v loop 5
~v loop 5
~in f()
*/
```

> [!tip] 
> 
> 构造函数会 **在对象创建完成后** 自动调用用于初始化对象
> 
> 析构函数会 **在离开对象的作用域** 时自动调用，用于释放必要的资源
>

## 构造函数与不变式

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

> [!tip] 
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

## 析构函数与资源

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

