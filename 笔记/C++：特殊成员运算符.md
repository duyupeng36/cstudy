# 特殊成员运算符

下列运算符

```
[]    ()    ->    ++    --    new    delete
```

与 `+, <, ~` 等传统的一元和二元运算符相比有其特殊之处，主要是从这些运算符在代码中的使用到程序员给出的定义和映射与传统运算符有细微的差别。其中 `[]` 和 `()` 是两种最重要的用户自定义运算符

## 下标运算符 `[]` 重载

我们可以用 `operator[]()` 函数对类对象的下标赋予某种新的含义。`operator[]()` 函数的第二个参数是下标，可以是任意类型的。因此，它常被用于定义 `vector`、关联数组等类型

> [!important] 
> 
> 请注意：`operator[]()` 运算符 **必须以成员函数的形式进行重载**
> 

下面我们实现一个 `Vector` 类，为了方便使用 `int` 类型作为元素的类型。为了了解 `[]` 运算符的重载，我们只实现构造函数和 `operator[]()` 函数

```cpp title:vector.hpp hl:29
#ifndef VECTOR_HPP
#include <initializer_list>

class Vector {
public:
    // 构造函数
    Vector();                               // 默认构造函数
    Vector(std::initializer_list<int> il);  // 列表初始化构造函数
    Vector(int n, int val);                 // 构造函数

    Vector(const Vector &v);               // 拷贝构造函数
    Vector(Vector &&v);                    // 移动构造函数

    // 析构函数
    ~Vector();

public:
    // 非修改性函数
    int size() const;                      // 返回元素个数
    int capacity() const;                  // 返回容量
    bool empty() const;                    // 判断是否为空
    bool full() const;                     // 判断是否满

public:
    // 运算符重载
    Vector &operator=(const Vector &v);    // 拷贝赋值运算符
    Vector &operator=(Vector &&v);         // 移动赋值运算符

    int &operator[](int index);            // 下标运算符：必须以成员函数的形式重载

private:
    int *_start;
    int *_finish;
    int *_end_of_storage;
};

#endif // VECTOR_HPP
```

> [!tip] 
> 
> 注意，我们让 `operator[]()` 返回了一个容器元素类型的引用，这样可以对指定容器元素进行赋值
> 

```cpp title:vector.cpp hl:117-122
#include <stdexcept>
#include "vector.hpp"

Vector::Vector()
: _start{nullptr}
, _finish{nullptr}
, _end_of_storage{nullptr}
{}

Vector::Vector(const std::initializer_list<int> &il)
: _start{new int[il.size()]}
, _finish{_start + il.size()}
, _end_of_storage{_finish} 
{
    if(!_start) {
        throw std::bad_alloc();
    }
    int *p = _start;
    for(auto e: il) {
        *p++ = e;
    }
}

Vector::Vector(int n, int val)
:_start{new int[n]{val}}
, _finish{_start + n}
, _end_of_storage{_start + n} 
{
    if(!_start) {
        throw std::bad_alloc();
    }
}

Vector::Vector(const Vector &v)
: _start{new int[v.size()]}
, _finish{_start + v.size()}
, _end_of_storage{_finish}
{
    if(!_start) {
        throw std::bad_alloc();
    }
    int *p = _start;
    for(int i = 0; i < v.size(); ++i) {
        *p++ = v._start[i];
    }
}

Vector::Vector(Vector &&v) noexcept
: _start{v._start}
, _finish{v._finish}
, _end_of_storage{v._end_of_storage}
{
    v._start = v._finish = v._end_of_storage = nullptr;
}

Vector::~Vector() {
    if(_start) {
        delete[] _start;
        _start = _finish = _end_of_storage = nullptr;
    }
}

// 非修改性函数
int Vector::size() const {
    return _finish - _start;
}

int Vector::capacity() const {
    return _end_of_storage - _start;
}

bool Vector::empty() const {
    return _start == _finish;
}

bool Vector::full() const {
    return _finish == _end_of_storage;
}

// 运算符重载
Vector &Vector::operator=(const Vector &v) {
    if(this == &v) {
        return *this;
    }
    if(_start) {
        delete[] _start;
    }
    _start = new int[v.size()];
    if(!_start) {
        throw std::bad_alloc();
    }
    _finish = _start + v.size();
    _end_of_storage = _finish;

    int *p = _start;
    for(int i = 0; i < v.size(); ++i) {
        *p++ = v._start[i];
    }
    return *this;
}

Vector &Vector::operator=(Vector &&v) noexcept{
    if(this == &v) {
        return *this;
    }
    if(_start) {
        delete[] _start;
    }
    _start = v._start;
    _finish = v._finish;
    _end_of_storage = v._end_of_storage;
    v._start = v._finish = v._end_of_storage = nullptr;
    return *this;
}


int &Vector::operator[](int index) {
    if(index < 0 || index >= size()) {
        throw std::out_of_range("index out of range");
    }
    return _start[index];
}
```

在需要禁止 `operator[]()` 赋值的情形下，我们需要将返回值设置为 `const` 引用，同时标记 `operator[]()` 为 `const` 函数

## 重载运算符 `>>` 和 `<<` 为格式输入和输出

当我们定义一个具体类之后，希望它的对象可以像内置类型那样进行输入输出时，就需要重载 `>>` 和 `<<` 运算符。由于 `operator>>()` 和 `operator<<()` 用于输入和输出时，需要访问类的私有成员，因此我们 **通过友元函数的形式进行重载**

我们期望能够通过标准输入(`cin`) 和标准输出(`cout`) 的方式操作上面定义的 `Vector` 对象，我们需要为期重载 `operator>>()` 和 `operator<<()`

```cpp title:vector.hpp hl:15-16
#ifndef VECTOR_HPP
#define VECTOR_HPP

#include <iostream> 
#include <initializer_list>

class Vector {
	// ...
private:
    int *_start;
    int *_finish;
    int *_end_of_storage;

    // 友元函数
    friend std::ostream &operator<<(std::ostream &os, const Vector &v);
    friend std::istream &operator>>(std::istream &is, Vector &v);
};

#endif // VECTOR_HPP
```

> [!tip] 
> 
> 友元可以是 `private` 也可以是 `public`，这两者没有任何差别
> 

```cpp title:vector.cpp
std::ostream &operator<<(std::ostream &os, const Vector &v) {
    os << "[";
    for(int i = 0; i < v.size(); ++i) {
        i == v.size() - 1 ? std::cout << v[i] : std::cout << v[i] << ", ";
    }
    os << "]";
    return os;
}

std::istream &operator>>(std::istream &is, Vector &v) {
    std::string line;        // 以 [1, 2, 3, 4] 的形式输入
    std::getline(is, line);
    // 跳过前导空白符
    auto pos = line.find_first_of("[");
    // 检查输入格式是否正确
    if(pos == std::string::npos) {
        throw std::invalid_argument("invalid input");
    }
    line = line.substr(pos+1);  // 1, 2, 3, 4]
    pos = line.find_last_of("]");
    if(pos == std::string::npos) {
        throw std::invalid_argument("invalid input");
    }
    line = line.substr(0, pos); // 1, 2, 3, 4

    std::vector<std::string> nums;  // 存储按照 , 分割出来的子字符串


    while ((pos=line.find(',')) != std::string::npos) {
        std::string num = line.substr(0, pos);
        nums.push_back(num);
        line = line.substr(pos+1);  
    }
    nums.push_back(line);

    v = Vector(nums.size(), 0);

    int i = 0;
    for(auto num: nums) {
        v[i++] = std::stoi(num);
    }

    return is;
}
```

## 解引用运算符 `->` 和 `*` 重载





## 函数调用运算符 `()` 重载

函数调用可以看作一个二元运算符: `expression(expression-list)`，它的左侧运算对象是 `expression` 右侧运算符对象是 `expression-list`。调用运算符 `()` 可以像其他运算符一样被重载

```cpp
#include <utility>
using namespace std;

struct Action
{
    int operator()(int); 
    pair<int, int> operator()(int, int);
    double operator()(double);    
};

void f(Action act)
{
    int x = act(2);      // 调用 Action::operator()(int)
    auto y = act(3, 4);  // 调用 Action::operator()(int, int)
    double z = act(2.3); // 调用 Action::operator()(double)
}
```

`operator()()` 的参数列表按照常规的参数传递规则进行求值和检查。重载函数调用运算符对于只包含一个操作的数据类型以及有一个主导操作的数据类型来说尤其有用。**调用运算符** 又称 **应用运算符**

> [!tip] 函数对象
> 
> 运算符 `operator()()` 最直接也是最重要的目标就是为某些 **行为类似函数的对象** 提供函数调用语法
> 
> 行为类似函数的对象称为 **类函数对象**，或者简单称为 **函数对象**
> 

函数对象使得我们可以接受某些特殊操作为参数。在很多情况下，**函数对象必须保存执行其操作需要的数据**

