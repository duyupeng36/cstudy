# 移动语义和默认操作

## 移动语义

将 `a` 的值给 `b` 的传统方法是拷贝。对于计算机内存中的一个整数，这几乎是唯一合理的方式：硬件用单一指令即可完成整数的拷贝。但是，从通用和逻辑角度考虑就不是这样了。考虑下面 `swap` 的实现，它完成两个对象的值交换

```cpp
template<typename T>
void swap(T &a, T &b) {
    T tmp {a};  // 将 a 的副本放入 tmp，此时内存中有两份 a 的值
    a = b;      // 将 b 的值赋给 a，此时内存中有两份 b 的值
    b = tmp;    // 将 tmp 的值赋给 b，此时内存中有两份 tmp 的值
}
```

这个示例代码可能需要做更多的工作。例如，如下方式使用上述的 `swap` 函数

```cpp
void f(string &s1, string &s2, vector<string> &sv1, vector<string> &sv2)
{
    swap(s1, s2);  // 交换 s1 和 s2 的值
    swap(sv1, sv2);  // 交换 sv1 和 sv2 的值
}
```

> [!attention] 
> 
> 当 `s1` 有上千个字符，`sv1` 有上千个元素。拷贝的代价就会非常高。由于 `swap()` 的语义是在两个变量之间 **移动** 值，而不是拷贝 
> 

我们可以这样来看移动这个概念：假如你希望借我的手机使用，我会直接将手机给你，而不是创建一个新的手机

> [!tip] 
> 
> 对于现实物体我们通常说 **送出**、**移交**、**转移所有权**、**移动** 等
> 

计算机中很多对象非常像现实物体而不是整数，例如，锁、套接字、文件句柄、线程、长字符串以及大向量等。C++ 不仅支持拷贝概念，也支持移动的概念。通过移动可以避免拷贝的逻辑和性能问题。我们可以为类定义 **移动构造函数** 和 **移动赋值运算符**

> [!tip] 移动构造函数 `T(T&&)`
> 
> 由于移动之后的对象处于被移出的状态，这个状态的对象是不能被使用的。因此，只能对一个 **右值** 或者 **确定不再使用的对象** 进行移动操作
> 

> [!tip] 移动赋值运算符 `T& operator=(T&&)`
> 
> 和移动构造函数类似。只有当该对象是一个右值或者确定不能使用的对象才会调用移动赋值运算符进行赋值
> 


考虑如下定义的 `Matrix` 代码

```cpp title:matrix.hpp
#ifndef MATRIX_HPP

#include <iostream>
#include <array>

using namespace std;

class Matrix {

// 构造/析构函数
public: 
    // 普通构造函数
    explicit Matrix(int d1, int d2);
    // 拷贝构造函数
    Matrix(const Matrix& m);


    // 析构函数
    ~Matrix() { delete[] elem; }

// 运算符重载
public:
    // 拷贝赋值运算符
    Matrix& operator=(const Matrix& m);


// 常量成员函数
public:
    int size() const;


// 数据成员
private:
    array<int, 2> dim; // 二维 dim[0], dim[1]
    int* elem;  // 指向 dim[0] * dim[1] 个 int 类型的元素的指针
};

#endif // MATRIX_HPP
```

```cpp title:matrix.cpp
#include "matrix.hpp"

// 构造函数
Matrix::Matrix(int d1, int d2) : dim{d1, d2}, elem{new int[d1 * d2]{0}} {
    if(d1 <= 0 || d2 <= 0) {
        throw invalid_argument("Matrix::Matrix(int, int): d1 and d2 should be positive.");
    }
    if(elem == nullptr) {
        // 内存分配失败
        throw bad_alloc();
    }
    cout << "Matrix::Matrix(int, int): constructor called." << endl;
}

// 拷贝构造函数
Matrix::Matrix(const Matrix& m) : dim{m.dim}, elem{new int[m.dim[0] * m.dim[1]]} {
    if(elem == nullptr) {
        // 内存分配失败
        throw bad_alloc();
    }
    // 拷贝
    for(int i = 0; i < m.dim[0] * m.dim[1]; ++i) {
        elem[i] = m.elem[i];
    }
    cout << "Matrix::Matrix(const Matrix&): copy constructor called." << endl;
}

// 析构函数
Matrix::~Matrix() {
    if(elem != nullptr) {
        delete[] elem;
        elem = nullptr;
    }
    cout << "Matrix::~Matrix(): destructor called." << endl;
}

// 拷贝赋值运算符
Matrix& Matrix::operator=(const Matrix& m) {
    if(this == &m) {
        // 自赋值
        return *this;
    }
    // 释放原有内存
    delete[] elem;
    // 分配新内存
    elem = new int[m.dim[0] * m.dim[1]];
    if(elem == nullptr) {
        // 内存分配失败
        throw bad_alloc();
    }
    // 拷贝
    for(int i = 0; i < m.dim[0] * m.dim[1]; ++i) {
        elem[i] = m.elem[i];
    }
    dim = m.dim;
    cout << "Matrix::operator=(const Matrix&): copy assignment operator called." << endl;
    return *this;
}


// 常量成员函数
int Matrix::size() const {
    return dim[0] * dim[1];
}
```

我们希望 `Matrix` 的 `swap()` 也避免严重的性能问题，我们就需要给 `Matrix` 提供移动构造函数和移动赋值运算符函数。如果没有移动语义，那么将两个 `Matrix` 对象交换就是使用拷贝语义。在 `Matrix` 对象持有较大资源时，此时会出现性能问题

```cpp title:matrix.hpp
#ifndef MATRIX_HPP

#include <iostream>
#include <array>

using namespace std;

class Matrix {

// 构造/析构函数
public: 
	// ...
    // 移动构造函数
    Matrix(Matrix &&m);
	// ...

// 运算符重载
public:
	// ...
    // 移动赋值运算符
    Matrix& operator=(Matrix&& m);
// ...
};

#endif // MATRIX_HPP
```

```cpp title:matrix.cpp
// 移动构造函数
Matrix::Matrix(Matrix &&m) : dim{m.dim}, elem{m.elem} {
    m.dim = {0, 0};
    m.elem = nullptr;
    cout << "Matrix::Matrix(Matrix&&): move constructor called." << endl;
}

// 移动赋值运算符
Matrix& Matrix::operator=(Matrix&& m) {
    if(this == &m) {
        // 自赋值
        return *this;
    }
    // 释放原有内存
    if(elem != nullptr) {
        delete[] elem;
        elem = nullptr;
    }
    // 移动
    elem = m.elem;
    dim = m.dim;
    // 将 m 置为移出状态
    m.elem = nullptr;
    m.dim = {0, 0};
    cout << "Matrix::operator=(Matrix&&): move assignment operator called." << endl;
    return *this;
}
```

> [!important] 移动操作必须总是将参数置于一种析构函数可处理的状态 
> 
> 移动构造函数和移动赋值运算符接受非 `const` 引用参数：它们可以修改实参，且通常也确实是这么做的，写入实参。
> 

我们定义如下用于交换 `Matrix` 的 `swap()` 函数，并调用

```cpp title:main.cpp
#include "matrix.hpp"

void swap(Matrix& m1, Matrix& m2) {
    cout << "swap(Matrix&, Matrix&): called." << endl;
    Matrix tmp{m1};
    m1 = m2;
    m2 = tmp;
}

int main() {
    Matrix m1{2, 3};
    Matrix m2{3, 4};
    swap(m1, m2);
    return 0;    
}
```

编译运行上述代码得到的结果是

```shell
➜  cppfiles g++ -std=c++11 -fno-elide-constructors main.cpp matrix.cpp
➜  cppfiles ./a.out
Matrix::Matrix(int, int): constructor called.
Matrix::Matrix(int, int): constructor called.
swap(Matrix&, Matrix&): called.
Matrix::Matrix(const Matrix&): copy constructor called.
Matrix::operator=(const Matrix&): copy assignment operator called.
Matrix::operator=(const Matrix&): copy assignment operator called.
Matrix::~Matrix(): destructor called.
Matrix::~Matrix(): destructor called.
Matrix::~Matrix(): destructor called.
```

我们发现编译器并没有调用移动构造函数和移动赋值运算符。编译器如何知道什么时候可以使用移动操作而不是拷贝操作呢？

> [!tip] 使用右值
> 
> 我们 **必须通过右值引用实参** 才能告知编译器使用移动操作
> 

> [!tip] 返回局部对象
> 
> 从函数中返回局部对象时，语言规则指出可以使用移动操作。因为，下一步操作就是销毁局部对象
> 

下面代码用于测试何时使用移动操作

```cpp
#include "matrix.hpp"

void f()
{
    cout << "f(): called." << endl;
    Matrix m1{2, 3}; // Matrix::Matrix(int, int): constructor called.
    cout << hex << "m1.elem: " << m1.ptr() << endl;
    Matrix m2{m1}; // Matrix::Matrix(const Matrix&): copy constructor called.
    cout << hex << "m2.elem: " << m2.ptr() << endl;

    Matrix m3{static_cast<Matrix &&>(m2)}; // Matrix::Matrix(Matrix&&): move constructor called.
    cout << hex << "m2.elem: " << m2.ptr() << endl;
    cout << hex << "m3.elem: " << m3.ptr() << endl;

    m2 = static_cast<Matrix &&>(m1); // Matrix::operator=(Matrix&&): move assignment called.
    cout << hex << "m1.elem: " << m1.ptr() << endl;
    cout << hex << "m2.elem: " << m2.ptr() << endl;
    cout << "f(): end." << endl;
}

int main()
{
    f();
    return 0;
}
```

上述代码编译运行的结果如下

```shell
➜  cppfiles g++ -std=c++11 -fno-elide-constructors main.cpp matrix.cpp
➜  cppfiles ./a.out 
f(): called.
Matrix::Matrix(int, int): constructor called.
m1.elem: 0x649006e276c0
Matrix::Matrix(const Matrix&): copy constructor called.
m2.elem: 0x649006e276e0
Matrix::Matrix(Matrix&&): move constructor called.
m2.elem: 0
m3.elem: 0x649006e276e0
Matrix::operator=(Matrix&&): move assignment operator called.
m1.elem: 0
m2.elem: 0x649006e276c0
f(): end.
Matrix::~Matrix(): destructor called.
Matrix::~Matrix(): destructor called.
Matrix::~Matrix(): destructor called.
```

> [!important] 
> 
> 必须要通过传递右值引用参数告知编译器，才能执行移动操作
> 

所有，我们需要将用于移动 `Matrix` 的 `swap()` 函数进行如下修改

```cpp
#include "matrix.hpp"

void swap(Matrix &m1, Matrix &m2)
{
    cout << "swap(Matrix&, Matrix&): called." << endl;
    Matrix tmp{static_cast<Matrix &&>(m1)};
    m1 = static_cast<Matrix &&>(m2);
    m2 = static_cast<Matrix &&>(tmp);
}

int main()
{
    Matrix m1{2, 3}; // Matrix::Matrix(int, int): constructor called.
    Matrix m2{3, 2}; // Matrix::Matrix(int, int): constructor called.
    cout << hex << "m1.elem: " << m1.ptr() << endl;
    cout << hex << "m2.elem: " << m2.ptr() << endl;
    swap(m1, m2);
    cout << hex << "m1.elem: " << m1.ptr() << endl;
    cout << hex << "m2.elem: " << m2.ptr() << endl;
    return 0;
}

```

上述代码编译运行的结果如下

```shell hl:5,6,12,13
➜  cppfiles g++ -std=c++11 -fno-elide-constructors main.cpp matrix.cpp
➜  cppfiles ./a.out 
Matrix::Matrix(int, int): constructor called.
Matrix::Matrix(int, int): constructor called.
m1.elem: 0x5d574f80d2b0
m2.elem: 0x5d574f80d6e0
swap(Matrix&, Matrix&): called.
Matrix::Matrix(Matrix&&): move constructor called.
Matrix::operator=(Matrix&&): move assignment operator called.
Matrix::operator=(Matrix&&): move assignment operator called.
Matrix::~Matrix(): destructor called.
m1.elem: 0x5d574f80d6e0
m2.elem: 0x5d574f80d2b0
Matrix::~Matrix(): destructor called.
Matrix::~Matrix(): destructor called.
```

## 默认操作

编写拷贝操作、析构函数这样的常规操作会很繁琐，也会很乏味且容易出错，因此编译器可以为我们生成这些操作。默认情况下，编译器会为一个类 `T` 自动生成
- 默认构造函数: `T()`
- 拷贝构造函数: `T(const T&)`
- 拷贝赋值运算符: `T& operator=(const T&)`
- 移动构造函数: `T(T&&)`
- 移动赋值运算符: `T& operator=(T&&)`
- 析构函数: `~T()`

默认情况下，如果程序使用了这些操作，编译器就会自动生成默认的版本。但是，如果我们自定义了其中一个或多个操作，那么对应的操作就不会自动生成

> [!tip] 自动生成操作的规则
> 
> 如果程序员为一个类 **声明了任意构造函数**，那么编译器就 **不会为该类生成默认构造函数**
> 
>  如果程序员为一个类 **声明了拷贝操作、移动操作或析构函数** ，则编译器 **不会为该类生成默认的拷贝操作、移动操作或析构函数**
> 
> 请注意：处于向后兼容性的需求，**即使程序员定义了析构函数，编译器还是会生成拷贝构造函数和拷贝赋值运算符**。这一特性在 ISO 标准中已经弃用了
> 

如果需要，我们可以显示指出希望编译器生成哪些函数(`=default`)以及不希望生成哪些函数(`=delete`)
