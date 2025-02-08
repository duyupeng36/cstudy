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

函数对象使得我们可以接受某些特殊操作为参数。在很多情况下，**函数对象必须保存执行其操作需要的数据**。例如，我们定义一个含有 `operator()()` 的类，它负责把一个预存的值加到它的参数上

```cpp
#include <string>
#include <iostream>

using namespace std;

class Add 
{
    int val;
public:
    Add(int v): val{v} {}

    void operator()(int &v) const{
        v += val;
    }
};


int main()
{
    int a = 0;
    Add add{10};  // 创建一个对象 add
    add(a);       // 将对象当做函数是使用
    cout << a << endl;
}
```

如果希望为一个容器中的每个元素都加上一个固定的值。我们可以定义如下函数

```cpp
void for_each(int *start, int *end, Add & add) {
    while (start != end)
    {
        add(*start++);
    }   
}
```

`for_each()` 函数的第 $3$ 个参数需要一个 `Add` 类对象的引用。从 `start` 到 `end` 依次遍历，并对每个元素都调用 `Add` 类的对象 。如下示例

```cpp
int main()
{
    int a = 0;
    Add add{10};

    int data[10]{1,2,3,4,5,6,7,8,9, 10};

    for_each(data, data + 10, add);
    for(int *p =data; p != data + 10; p++) {
        cout << *p << ' ';
    }
    cout << endl;
}
```

## 解引用运算符 `->` 和 `*` 重载

解引用运算符 `->` 也称为箭头运算符，该运算符是用于通过指针获取类的成员。解引用运算符 `*` 用于获取指针所指向的对象

例如

```cpp
#include <string>
#include <iostream>

using namespace std;

struct Person
{
    string name;
    int age;
};

int main()
{
    Person *p = new Person{"dyp", 20};
    cout << p->name << " " << p->age << endl;
    Person person = *p;  // 发生一次复制
    cout << person.name << " " << person.age << endl;
}
```

上述代码使用了一个 **裸指针** 用于管理分配在堆空间中的对象。在 C++中，这通常不是一个很好的用法，因为我们可能会忘记释放堆空间的内存，从而导致内存泄漏

> [!tip] 
> 
> 使用裸指针操作堆空间的对象 **非常容易导致内存泄漏**。我们希望有这么一种指针类型，在我们不需要使用它指向的堆空间时自动释放堆内存
> 

显然，我们需要使用一个资源管理器来管理堆内存。并且让它表现出与指针一样的特性，我们需要重载 `->` 和 `*` 运算符。假设我们定义了一个类 `Ptr` 用于表示指针的概念

```cpp
template <class T>
class Ptr
{

public:
    Ptr(T *ptr) : _ptr{ptr} {}
    ~Ptr()
    {
        cout << "~Ptr()" << endl;
        if (_ptr)
        {
            delete _ptr;
            _ptr = nullptr;
        }
    }

public:
    // 运算符重载
    T *operator->()
    {
        return _ptr;
    }
    T &operator*()
    {
        return *_ptr;
    }

private:
    T *_ptr;
};
```

类 `Ptr` 的对象可以用于访问类 `T` 的成员，其用法与指针非常相似。例如

```cpp
void f(Ptr<Person> p)
{
    cout << p->name << " " << p->age << endl;
    // p->name 等价于 (p.operator->())->name
    cout << (*p).name << " " << (*p).age << endl;
    // (*p).name 等价于 (p.operator*()).name
}
```

下图展示了函数 `f()` 栈帧的内存图

![[Pasted image 20250208091530.png]]

通过 `Ptr` 类的对象访问 `Person` 类的对象的成员，需要让 `Ptr` 类的对象通过某种方法返回 `Person` 类对象的指针。

> [!tip] 
> 
> 对象 `p` 到指针 `p.operator->()` 的转换与其所指的成员 `name` 和 `age` 没有任何关系。这正是 `operator->()` 是一元后置运算符的意义
> 
> 注意：这里并没有引入任何新的语法，所以 `->` 之后依旧需要一个成员的名字
> 

> [!tip] 重载 `->` 的目的就是为了实现 **智能指针**
> 
> 所谓的智能指针就是 **行为与指针类似的对象**，并且当用其访问对象时执行某些操作。其核心思想就是 **通过对象的生命周期管理资源**
> 
> 上面定义的 `Ptr` 类就一种智能指针，但是它还存在一些严重的 `bug`，请忽视它
> 

对于普通的指针来说，使用 `->` 和使用 `*` 以及 `[]` 行为类似。已知在类 `Y` 中 `->` `*` 和 `[]` 均有其默认含义。假设 `p` 的类型是 `Y*`，则有

```
p->m 等价于 (*p).m 等价于 p[0].m;
```

> [!tip] 
> 
> 对于用户自定义类型，这个规则是无效的，如果需要自定义类对象满足上述规则，则需要自行重载上述运算符从而实现相同的效果
> 

> [!attention] 
> 
> 请注意：运算符 `->` 必须是 **非 static 成员函数**。并且，返回值类型必须是 **指针** 或者 **类的对象**，我们需要把 `->` 作用于它们
> 
> `->` 和 `.` 非常相似，但是我们不能重载点运算符(`.`)
> 

## 自增运算符 `++` 和自减运算符 `--` 重载

继续上述 `Ptr` 类的实现。指针支持使用 `++` 和 `--` 运算符将指针指向下一个位置，现在我们的 `Ptr` 模拟的指针还不能做到这一点。下面我们通过重载 `++` 和 `--` 实现这个操作

> [!tip] 前置 `++` 和 `--`
> 
> 当 `++` 和 `--` 运算符作为前置运算符时，返回的是修改后的对象的值
> 
> 为了让 **类指针对象** 模拟内置指针类型的行为，应该让 `operator++()` 和 `operator--()` 返回对象自身的引用。例如，在我们定义的 `Ptr` 类中，让 `operator++()` 和 `operator--()` 返回 `Ptr &`
> 
> ```cpp
> Ptr & operator++();
> Ptr & operator--()
> ```
> 

> [!tip] 后置 `++` 和 `--`
> 
> 当 `++` 和 `--` 运算符作为后置运算符时，返回的是修改之前的对象的值
> 
> 通用，为了模拟内置指针类型的行为，应该让 `operator++()` 和 `operator--()` 返回一个新对象。为了区分前置和后缀运算符函数，C++ 规则必须为后置的运算符函数提供一个额外的 `int` 类型的参数，这个参数仅仅只是占位，为了函数重载的正确进行
> 
> ```cpp
> Ptr operator++(int);
> Ptr operator--(int)
> ```
> 

下面代码展示了为 `Ptr` 类实现 `operator++()` 和 `operator++(int)` 成员函数。由于指针可以作为函数参数也可以被复制，因此我们需要定义 `Ptr` 的拷贝操作，通过使用引用计数控制是否释放 `Ptr` 管理的对象

```cpp hl:26
template <class T>
class Ptr
{

public:
    // 管理的是单个对象
    Ptr(T *ptr) : _ptr{ptr}, _data{nullptr}, _size{0}, _reference{new size_t{1}} {}
    // 管理对象数组
    Ptr(T *data, size_t size) : _ptr{data}, _data{data}, _size{size}, _reference{new size_t{1}} {}

    // 拷贝构造
    Ptr(const Ptr &p) : _ptr{p._ptr}, _data{p._data}, _size{p._size}, _reference{p._reference}
    {
        ++*p._reference; // 让引用计数 +1
    }

    // 拷贝赋值
    Ptr &operator=(const Ptr &rhs)
    {
        if (this == &rhs)
        {
            return *this;
        }

        // 引用计数+1;
        ++*rhs._reference;

        // 调用 this 的析构函数，让 this 的引用计数减少 1
        this->~Ptr();

        // 拷贝
        this->_ptr = rhs._ptr;
        this->_data = rhs._data;
        this->_size = rhs._size;
        this->_reference = rhs._reference;

        return *this;
    }

    ~Ptr()
    {
        --*_reference;
        // 引用计数未减少到 0，此时还有对象才有这些资源
        if (*_reference)
        {
            return;
        }
        cout << "~Ptr()" << endl;
        // 如果管理的是单个对象：_ptr 指向对象并且_data 为 nullptr 并且 _size 为 0
        if (_ptr && !_data && !_size)
        {
            cout << "管理单个对象，释放对象" << endl;
            delete _ptr;
            _ptr = nullptr;
        }
        else if (_data && _size)
        {
            cout << "管理是数组，释放数组" << endl;
            delete[] _data;
            _data = nullptr;
            _size = 0;
        }
		// 释放 _referenc 指向的对象
        if(_reference)
        {
            delete _reference;
            _reference = nullptr;
        }
    }

public:
    // 运算符重载
    T *operator->()
    {
        return _ptr;
    }

    T &operator*()
    {
        return *_ptr;
    }

    Ptr &operator++() // 前置 ++
    {
        // 管理的数组
        if (_data && _size)
        {
            // 如果超出范围
            if (_ptr == _data + _size)
            {
                throw std::out_of_range("Outside the array range");
            }
            ++_ptr;
        }
        else
        {
            throw std::runtime_error("The pointer does not point to an element in the array.");
        }
        return *this;
    }

    Ptr operator++(int) // 后置 ++
    {
        Ptr tmp{*this}; // 调用拷贝构造，创建一个副本
        // 管理数组
        if (_data && _size)
        {
            // 超出范围
            if (_ptr == _data + _size)
            {
                throw std::out_of_range("Outside the array range");
            }
            ++_ptr;
        }
        else
        {
            // 单个对象
            throw std::runtime_error("The pointer does not point to an element in the array.");
        }
        return tmp;
    }

private:
    T *_ptr;      // 指向当前指针指向的数组元素
    T *_data;     // 指向数组
    size_t _size; // 表示元素个数

    size_t *_reference; // 引用计数：被所有对象共享
};
```

> [!tip] 
> 
> 新版的 `Ptr` 类通过引用计数的方式让多个 `Ptr` 对象持有相同的资源。每当发生一次拷贝时，就让引用计数 `+1`。每当发生一次析构时，就让引用计数 `-1`。只有当引用计数减少到 $0$ 时，才让对象持有的资源被销毁

> [!tip] 
> 
> 这里在拷贝赋值运算符中我们调用了当前对象的析构函数。因为，检查当前对象是否是最后一个持有资源的对象本身就是析构函数做的事情
> 

下面的示例中演示了新版 `Ptr` 类的使用。通过使用 `Ptr` 对象管理资源，可以保证

```cpp
void f(Ptr<int> p) {
    for(int i = 0; i < 10; i++) {
        cout << *p++ << ' ';
    }
    cout << endl;
}

int main()
{
    Ptr<int> arr{new int[10]{10, 20, 30, 40, 50, 60, 70, 80, 90, 100}, 10}; 
    f(arr);  // 发生一次拷贝

    return 0;
}
```

## 分配和释放堆内存

在 [[C++：单例模式]] 中介绍过重载 `operator new()` 函数用于拦截对象的内存分配过程。下面我们详细介绍运算符 `new` 和 `delete` 

运算符 `new` 通过调用 `operator new()` 分配内存。相应的，运算符 `delete` 通过调用 `operator delete()` 释放内存。用户可以重定义全局的 `operator new()` 和 `operator delete()`，也可以为特定的类型定义 `operator new()` 和 `operator delete()`

> [!warning] 
> 
> 警告，不建议改写全局的 `operator new()` 和 `operator delete()`
> 

更好的做法是为特定的类型提供这些操作。其中，这个特定的类可以作为很多派生类的基类

```cpp
class Employee
{
public:
	// ...
	void* operator new(size_t);
	void operator delete(void *, size_t);

	void* operator new[](size_t);
	void operator delete[](void *, size_t);
}
```

> [!tip] 
> 
> `operator new()` 和 `operator delete[]()` 是隐式 `static` 成员函数。因此，它们无法使用 `this` 指针
> 

## 成员指针

**成员指针** 是一种 **类似偏移量** 的语言构造，允许程序员使用运算符 `->*` 和 `.*` 间接引用类成员

> [!tip] 
> 
> 运算符 `->*` 和 `.*` 是最特殊也是最少使用的 C++ 运算符
> + 运算符 `->` 用于访问一个类成员，例如 `m * p -> m`
> + 运算符 `->*` 用于访问一个类成员，其名字保存在成员指针中，例如 `ptom::p->*ptom`
> 
> 运算符 `->` 和 `->*` 均允许我们通过函数传递来的成员名访问类成员。在两者情况下，`p` 都必须是指向恰当类的对象的指针
> 

> [!attention] 
> 
> 成员指针不能赋予 `void*` 或任何其他普通指针。空指针 `nullptr` 可赋予成员指针，表示无成员
> 

```cpp hl:23-24,27-28,31-32
#include <string>
#include <iostream>
using namespace std;

class Person 
{
public:
    string name;
    int age;

    void print() {
        cout << "name: " << name << endl;
        cout << "age: " << age << endl;
    }
};


int main() {
    Person p {"杜宇鹏", 29};

    Person * pp = &p;

    string Person:: * pname = &Person::name;  // 数据成员指针
    void (Person:: *pfun)() = &Person::print; // 成员函数指针

    // 通过对象
    cout << p.*pname << endl;
    (p.*pfun)();

    // 通过对象的指针
    cout << pp->*pname << endl;
    (pp->*pfun)();
}
```

> [!tip] 
> 
> 初始化成员指针的时候，必须要使用 `&类名::成员名`
> 
> + **数据成员指针** 可以理解为 **成员在对象中的偏移量**
> + **成员函数指针** 可以理解为 **成员函数在代码区的地址**
> 

>[!important] 成员指针的意义
>
>+ **回调函数**：将成员函数指针作为参数传递给其他函数，使其他函数能够在特定条件下调用该成员函数
>+ **事件处理**：将成员函数指针存储事件处理程序中，以便在特定事件发生时调用相应的成员函数
>+ **多态性**：通过将成员函数指针存储在基类指针中，可以实现多态性，在运行时能够去调用相应的成员函数。
>

## 类型转换函数

以前我们认识了普通变量的类型转换，比如说 `int` 型转换为 `long` 型， `double` 型转换为 `int` 型，接下来我们要讨论下类对象与其他类型的转换

在 [[C++：类与对象#特殊成员#explicit 构造函数]] 只能我们介绍了其他类默认可以向自定义类型进行转换，可以使用 `explicit` 关键字紧张这种转换

在 [[C++：友元与运算符重载#复数类型]] 中简单的实现了一个类型转换 `operator bool()`。现在，我们详细讨论类型转换函数

> [!tip] 类型转换函数
> 
> 我们可以为类声明一个 **成员函数**，其形式是固定为 `operator T()`，其中 `T` 是目标类型。它具有下列特征
> + 没有参数
> + 没有返回类型
> + 函数体必须返回目标类型的值
> + 必须是一个成员函数
> 

## C++ 运算符的优先级和结合性

在 [[C 语言：基本概念#表达式#运算符]] 中学习了 C 语言运算符的优先级和结合性。在后续 C++ 的学习过程中，C++ 的运算符学习的差不多了。下面我们总结 C++ 运算符的优先级和结合性

下表内容来之 [C++ 内置运算符的优先级和结合性](https://learn.microsoft.com/zh-cn/cpp/cpp/cpp-built-in-operators-precedence-and-associativity?view=msvc-170)

| 运算符说明                                                                                                                             | 运算符                                                                                                                               | 替代项          |
| --------------------------------------------------------------------------------------------------------------------------------- | --------------------------------------------------------------------------------------------------------------------------------- | ------------ |
| 第 1 组优先级，无关联性                                                                                                                     |                                                                                                                                   |              |
| [范围解析](https://learn.microsoft.com/zh-cn/cpp/cpp/scope-resolution-operator?view=msvc-170)                                         | [`::`](https://learn.microsoft.com/zh-cn/cpp/cpp/scope-resolution-operator?view=msvc-170)                                         |              |
| 第 2 组优先级，从左到右关联                                                                                                                   |                                                                                                                                   |              |
| [成员选择（对象或指针）](https://learn.microsoft.com/zh-cn/cpp/cpp/member-access-operators-dot-and?view=msvc-170)                            | [`.` 或 `->`](https://learn.microsoft.com/zh-cn/cpp/cpp/member-access-operators-dot-and?view=msvc-170)                             |              |
| [数组下标](https://learn.microsoft.com/zh-cn/cpp/cpp/subscript-operator?view=msvc-170)                                                | [`[]`](https://learn.microsoft.com/zh-cn/cpp/cpp/subscript-operator?view=msvc-170)                                                |              |
| [函数调用](https://learn.microsoft.com/zh-cn/cpp/cpp/function-call-operator-parens?view=msvc-170)                                     | [`()`](https://learn.microsoft.com/zh-cn/cpp/cpp/function-call-operator-parens?view=msvc-170)                                     |              |
| [后缀递增](https://learn.microsoft.com/zh-cn/cpp/cpp/postfix-increment-and-decrement-operators-increment-and-decrement?view=msvc-170) | [`++`](https://learn.microsoft.com/zh-cn/cpp/cpp/postfix-increment-and-decrement-operators-increment-and-decrement?view=msvc-170) |              |
| [后缀递减](https://learn.microsoft.com/zh-cn/cpp/cpp/postfix-increment-and-decrement-operators-increment-and-decrement?view=msvc-170) | [`--`](https://learn.microsoft.com/zh-cn/cpp/cpp/postfix-increment-and-decrement-operators-increment-and-decrement?view=msvc-170) |              |
| [类型名称](https://learn.microsoft.com/zh-cn/cpp/cpp/typeid-operator?view=msvc-170)                                                   | [`typeid`](https://learn.microsoft.com/zh-cn/cpp/cpp/typeid-operator?view=msvc-170)                                               |              |
| [常量类型转换](https://learn.microsoft.com/zh-cn/cpp/cpp/const-cast-operator?view=msvc-170)                                             | [`const_cast`](https://learn.microsoft.com/zh-cn/cpp/cpp/const-cast-operator?view=msvc-170)                                       |              |
| [动态类型转换](https://learn.microsoft.com/zh-cn/cpp/cpp/dynamic-cast-operator?view=msvc-170)                                           | [`dynamic_cast`](https://learn.microsoft.com/zh-cn/cpp/cpp/dynamic-cast-operator?view=msvc-170)                                   |              |
| [重新解释的类型转换](https://learn.microsoft.com/zh-cn/cpp/cpp/reinterpret-cast-operator?view=msvc-170)                                    | [`reinterpret_cast`](https://learn.microsoft.com/zh-cn/cpp/cpp/reinterpret-cast-operator?view=msvc-170)                           |              |
| [静态类型转换](https://learn.microsoft.com/zh-cn/cpp/cpp/static-cast-operator?view=msvc-170)                                            | [`static_cast`](https://learn.microsoft.com/zh-cn/cpp/cpp/static-cast-operator?view=msvc-170)                                     |              |
| 第 3 组优先级，从右到左关联                                                                                                                   |                                                                                                                                   |              |
| [对象或类型的大小](https://learn.microsoft.com/zh-cn/cpp/cpp/sizeof-operator?view=msvc-170)                                               | [`sizeof`](https://learn.microsoft.com/zh-cn/cpp/cpp/sizeof-operator?view=msvc-170)                                               |              |
| [前缀递增](https://learn.microsoft.com/zh-cn/cpp/cpp/prefix-increment-and-decrement-operators-increment-and-decrement?view=msvc-170)  | [`++`](https://learn.microsoft.com/zh-cn/cpp/cpp/prefix-increment-and-decrement-operators-increment-and-decrement?view=msvc-170)  |              |
| [前缀递减](https://learn.microsoft.com/zh-cn/cpp/cpp/prefix-increment-and-decrement-operators-increment-and-decrement?view=msvc-170)  | [`--`](https://learn.microsoft.com/zh-cn/cpp/cpp/prefix-increment-and-decrement-operators-increment-and-decrement?view=msvc-170)  |              |
| [二进制反码](https://learn.microsoft.com/zh-cn/cpp/cpp/one-s-complement-operator-tilde?view=msvc-170)                                  | [`~`](https://learn.microsoft.com/zh-cn/cpp/cpp/one-s-complement-operator-tilde?view=msvc-170)                                    | **`compl`**  |
| [逻辑“非”](https://learn.microsoft.com/zh-cn/cpp/cpp/logical-negation-operator-exclpt?view=msvc-170)                                 | [`!`](https://learn.microsoft.com/zh-cn/cpp/cpp/logical-negation-operator-exclpt?view=msvc-170)                                   | **`not`**    |
| [一元求反](https://learn.microsoft.com/zh-cn/cpp/cpp/unary-plus-and-negation-operators-plus-and?view=msvc-170)                        | [`-`](https://learn.microsoft.com/zh-cn/cpp/cpp/unary-plus-and-negation-operators-plus-and?view=msvc-170)                         |              |
| [一元加](https://learn.microsoft.com/zh-cn/cpp/cpp/unary-plus-and-negation-operators-plus-and?view=msvc-170)                         | [`+`](https://learn.microsoft.com/zh-cn/cpp/cpp/unary-plus-and-negation-operators-plus-and?view=msvc-170)                         |              |
| [Address-of](https://learn.microsoft.com/zh-cn/cpp/cpp/address-of-operator-amp?view=msvc-170)                                     | [`&`](https://learn.microsoft.com/zh-cn/cpp/cpp/address-of-operator-amp?view=msvc-170)                                            |              |
| [间接寻址](https://learn.microsoft.com/zh-cn/cpp/cpp/indirection-operator-star?view=msvc-170)                                         | [`*`](https://learn.microsoft.com/zh-cn/cpp/cpp/indirection-operator-star?view=msvc-170)                                          |              |
| [创建对象](https://learn.microsoft.com/zh-cn/cpp/cpp/new-operator-cpp?view=msvc-170)                                                  | [`new`](https://learn.microsoft.com/zh-cn/cpp/cpp/new-operator-cpp?view=msvc-170)                                                 |              |
| [销毁对象](https://learn.microsoft.com/zh-cn/cpp/cpp/delete-operator-cpp?view=msvc-170)                                               | [`delete`](https://learn.microsoft.com/zh-cn/cpp/cpp/delete-operator-cpp?view=msvc-170)                                           |              |
| [强制转换](https://learn.microsoft.com/zh-cn/cpp/cpp/cast-operator-parens?view=msvc-170)                                              | [`()`](https://learn.microsoft.com/zh-cn/cpp/cpp/cast-operator-parens?view=msvc-170)                                              |              |
| 第 4 组优先级，从左到右关联                                                                                                                   |                                                                                                                                   |              |
| [指向成员的指针（对象或指针）](https://learn.microsoft.com/zh-cn/cpp/cpp/pointer-to-member-operators-dot-star-and-star?view=msvc-170)           | [`.*` 或 `->*`](https://learn.microsoft.com/zh-cn/cpp/cpp/pointer-to-member-operators-dot-star-and-star?view=msvc-170)             |              |
| 第 5 组优先级，从左到右关联                                                                                                                   |                                                                                                                                   |              |
| [乘法](https://learn.microsoft.com/zh-cn/cpp/cpp/multiplicative-operators-and-the-modulus-operator?view=msvc-170)                   | [`*`](https://learn.microsoft.com/zh-cn/cpp/cpp/multiplicative-operators-and-the-modulus-operator?view=msvc-170)                  |              |
| [除法](https://learn.microsoft.com/zh-cn/cpp/cpp/multiplicative-operators-and-the-modulus-operator?view=msvc-170)                   | [`/`](https://learn.microsoft.com/zh-cn/cpp/cpp/multiplicative-operators-and-the-modulus-operator?view=msvc-170)                  |              |
| [取模](https://learn.microsoft.com/zh-cn/cpp/cpp/multiplicative-operators-and-the-modulus-operator?view=msvc-170)                   | [`%`](https://learn.microsoft.com/zh-cn/cpp/cpp/multiplicative-operators-and-the-modulus-operator?view=msvc-170)                  |              |
| 第 6 组优先级，从左到右关联                                                                                                                   |                                                                                                                                   |              |
| [加法](https://learn.microsoft.com/zh-cn/cpp/cpp/additive-operators-plus-and?view=msvc-170)                                         | [`+`](https://learn.microsoft.com/zh-cn/cpp/cpp/additive-operators-plus-and?view=msvc-170)                                        |              |
| [减法](https://learn.microsoft.com/zh-cn/cpp/cpp/additive-operators-plus-and?view=msvc-170)                                         | [`-`](https://learn.microsoft.com/zh-cn/cpp/cpp/additive-operators-plus-and?view=msvc-170)                                        |              |
| 第 7 组优先级，从左到右关联                                                                                                                   |                                                                                                                                   |              |
| [左移](https://learn.microsoft.com/zh-cn/cpp/cpp/left-shift-and-right-shift-operators-input-and-output?view=msvc-170)               | [`<<`](https://learn.microsoft.com/zh-cn/cpp/cpp/left-shift-and-right-shift-operators-input-and-output?view=msvc-170)             |              |
| [右移](https://learn.microsoft.com/zh-cn/cpp/cpp/left-shift-and-right-shift-operators-input-and-output?view=msvc-170)               | [`>>`](https://learn.microsoft.com/zh-cn/cpp/cpp/left-shift-and-right-shift-operators-input-and-output?view=msvc-170)             |              |
| 第 8 组优先级，从左到右关联                                                                                                                   |                                                                                                                                   |              |
| [小于](https://learn.microsoft.com/zh-cn/cpp/cpp/relational-operators-equal-and-equal?view=msvc-170)                                | [`<`](https://learn.microsoft.com/zh-cn/cpp/cpp/relational-operators-equal-and-equal?view=msvc-170)                               |              |
| [大于](https://learn.microsoft.com/zh-cn/cpp/cpp/relational-operators-equal-and-equal?view=msvc-170)                                | [`>`](https://learn.microsoft.com/zh-cn/cpp/cpp/relational-operators-equal-and-equal?view=msvc-170)                               |              |
| 小于或等于                                                                                                                             | [`<=`](https://learn.microsoft.com/zh-cn/cpp/cpp/relational-operators-equal-and-equal?view=msvc-170)                              |              |
| 大于或等于                                                                                                                             | [`>=`](https://learn.microsoft.com/zh-cn/cpp/cpp/relational-operators-equal-and-equal?view=msvc-170)                              |              |
| 第 9 组优先级，从左到右关联                                                                                                                   |                                                                                                                                   |              |
| [等式](https://learn.microsoft.com/zh-cn/cpp/cpp/equality-operators-equal-equal-and-exclpt-equal?view=msvc-170)                     | [`==`](https://learn.microsoft.com/zh-cn/cpp/cpp/equality-operators-equal-equal-and-exclpt-equal?view=msvc-170)                   |              |
| [不相等](https://learn.microsoft.com/zh-cn/cpp/cpp/equality-operators-equal-equal-and-exclpt-equal?view=msvc-170)                    | [`!=`](https://learn.microsoft.com/zh-cn/cpp/cpp/equality-operators-equal-equal-and-exclpt-equal?view=msvc-170)                   | **`not_eq`** |
| 第 10 组优先级，从左到右关联                                                                                                                  |                                                                                                                                   |              |
| [按位“与”](https://learn.microsoft.com/zh-cn/cpp/cpp/bitwise-and-operator-amp?view=msvc-170)                                         | [`&`](https://learn.microsoft.com/zh-cn/cpp/cpp/bitwise-and-operator-amp?view=msvc-170)                                           | **`bitand`** |
| 第 11 组优先级，从左到右关联                                                                                                                  |                                                                                                                                   |              |
| [按位“异或”](https://learn.microsoft.com/zh-cn/cpp/cpp/bitwise-exclusive-or-operator-hat?view=msvc-170)                               | [`^`](https://learn.microsoft.com/zh-cn/cpp/cpp/bitwise-exclusive-or-operator-hat?view=msvc-170)                                  | **`xor`**    |
| 第 12 组优先级，从左到右关联                                                                                                                  |                                                                                                                                   |              |
| [按位“与或”](https://learn.microsoft.com/zh-cn/cpp/cpp/bitwise-inclusive-or-operator-pipe?view=msvc-170)                              | [`\|`](https://learn.microsoft.com/zh-cn/cpp/cpp/bitwise-inclusive-or-operator-pipe?view=msvc-170)                                | **`bitor`**  |
| 第 13 组优先级，从左到右关联                                                                                                                  |                                                                                                                                   |              |
| [逻辑与](https://learn.microsoft.com/zh-cn/cpp/cpp/logical-and-operator-amp-amp?view=msvc-170)                                       | [`&&`](https://learn.microsoft.com/zh-cn/cpp/cpp/logical-and-operator-amp-amp?view=msvc-170)                                      | **`and`**    |
| 第 14 组优先级，从左到右关联                                                                                                                  |                                                                                                                                   |              |
| [逻辑或](https://learn.microsoft.com/zh-cn/cpp/cpp/logical-or-operator-pipe-pipe?view=msvc-170)                                      | [`\|`](https://learn.microsoft.com/zh-cn/cpp/cpp/logical-or-operator-pipe-pipe?view=msvc-170)                                     | **`or`**     |
| 第 15 组优先级，从右到左关联                                                                                                                  |                                                                                                                                   |              |
| [条件](https://learn.microsoft.com/zh-cn/cpp/cpp/conditional-operator-q?view=msvc-170)                                              | [`? :`](https://learn.microsoft.com/zh-cn/cpp/cpp/conditional-operator-q?view=msvc-170)                                           |              |
| [转让](https://learn.microsoft.com/zh-cn/cpp/cpp/assignment-operators?view=msvc-170)                                                | [`=`](https://learn.microsoft.com/zh-cn/cpp/cpp/assignment-operators?view=msvc-170)                                               |              |
| [乘法赋值](https://learn.microsoft.com/zh-cn/cpp/cpp/assignment-operators?view=msvc-170)                                              | [`*=`](https://learn.microsoft.com/zh-cn/cpp/cpp/assignment-operators?view=msvc-170)                                              |              |
| [除法赋值](https://learn.microsoft.com/zh-cn/cpp/cpp/assignment-operators?view=msvc-170)                                              | [`/=`](https://learn.microsoft.com/zh-cn/cpp/cpp/assignment-operators?view=msvc-170)                                              |              |
| [取模赋值](https://learn.microsoft.com/zh-cn/cpp/cpp/assignment-operators?view=msvc-170)                                              | [`%=`](https://learn.microsoft.com/zh-cn/cpp/cpp/assignment-operators?view=msvc-170)                                              |              |
| [加法赋值](https://learn.microsoft.com/zh-cn/cpp/cpp/assignment-operators?view=msvc-170)                                              | [`+=`](https://learn.microsoft.com/zh-cn/cpp/cpp/assignment-operators?view=msvc-170)                                              |              |
| [减法赋值](https://learn.microsoft.com/zh-cn/cpp/cpp/assignment-operators?view=msvc-170)                                              | [`-=`](https://learn.microsoft.com/zh-cn/cpp/cpp/assignment-operators?view=msvc-170)                                              |              |
| [左移赋值](https://learn.microsoft.com/zh-cn/cpp/cpp/assignment-operators?view=msvc-170)                                              | [`<<=`](https://learn.microsoft.com/zh-cn/cpp/cpp/assignment-operators?view=msvc-170)                                             |              |
| [右移赋值](https://learn.microsoft.com/zh-cn/cpp/cpp/assignment-operators?view=msvc-170)                                              | [`>>=`](https://learn.microsoft.com/zh-cn/cpp/cpp/assignment-operators?view=msvc-170)                                             |              |
| [按位“与”赋值](https://learn.microsoft.com/zh-cn/cpp/cpp/assignment-operators?view=msvc-170)                                           | [`&=`](https://learn.microsoft.com/zh-cn/cpp/cpp/assignment-operators?view=msvc-170)                                              | **`and_eq`** |
| [按位“与或”赋值](https://learn.microsoft.com/zh-cn/cpp/cpp/assignment-operators?view=msvc-170)                                          | [`\|=`](https://learn.microsoft.com/zh-cn/cpp/cpp/assignment-operators?view=msvc-170)                                             | **`or_eq`**  |
| [按位“异或”赋值](https://learn.microsoft.com/zh-cn/cpp/cpp/assignment-operators?view=msvc-170)                                          | [`^=`](https://learn.microsoft.com/zh-cn/cpp/cpp/assignment-operators?view=msvc-170)                                              | **`xor_eq`** |
| [引发表达式](https://learn.microsoft.com/zh-cn/cpp/cpp/try-throw-and-catch-statements-cpp?view=msvc-170)                               | [`throw`](https://learn.microsoft.com/zh-cn/cpp/cpp/try-throw-and-catch-statements-cpp?view=msvc-170)                             |              |
| 第 16 组优先级，从左到右关联                                                                                                                  |                                                                                                                                   |              |
| [Comma](https://learn.microsoft.com/zh-cn/cpp/cpp/comma-operator?view=msvc-170)                                                   | [`，`](https://learn.microsoft.com/zh-cn/cpp/cpp/comma-operator?view=msvc-170)                                                     |              |
