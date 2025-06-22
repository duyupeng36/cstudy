# string 的底层实现

在 [[C++：string类和vector类]] 中我们介绍了 `string` 类的基本使用。下面我们探索 `string` 的底层实现

`string` 类的实现基本上有 $3$ 种方式

> [!tip] Eager Copy：深拷贝
> 
> 每次复制都会拷贝完整的数据
> 

> [!tip] Copy-On-Write：写时复制
> 
> 将拷贝延迟到对象进行修改时
> 

> [!tip] Short String Optimization：短字符串优化
> 
> 对于短字符串就保持在对象中，而不是在堆空间中
> 

上述 $3$ 种方式最简单的就是 **深拷贝**。即无论在任何情形下，只要发送对象拷贝就重新分配资源空间，然后复制对象持有的资源

## 写时复制

每次都发生拷贝时会重新申请空间然后复制内容，这样做的效率及其低下。为了提升性能，演化出了 **写时复制技术**

> [!tip] 
> 
> 在 [[Linux 系统编程：进程创建]] 中我们介绍过系统调用 `fork()` 就是采用写时复制技术实现的
> 

### 引用计数

下面我们探究写时复制的原理。写时复制的关键思想就是 **在修改对象持有的资源时才会发生拷贝操作**。也就是说，**一个对象持有的资源可能被多个对象共享**。现在，就出现了第一个问题：_**什么时候才释放一个对象持有的资源呢？**_

> [!important] 
> 
> 由于资源被多个对象共享，因此资源不能随意的回收，**应该在确定资源没有对象持有的时候才进行回收**。
> 

显然，需要使用一个计数器记录资源被次有的次数。只要计数器不为 $0$ 就证明资源目前被对象持有，此时不能回收资源。这个计数器就称为 **引用计数(reference count)**。

> [!tip] 引用计数
> 
> + 当发送一次拷贝时，就将引用计数 $+1$
> + 当发送一次析构时，就将引用计数 $-1$
> 
> 

现在，引入了引用计数，随之而来的第二个问题也出现了：_**引用计数存在哪个地方？**_

> [!error] 错误存储方式：存在对象中！！！
> 
> 注意，这是错误的。引用计数在具体的对象中，引用计数是独立的。如下图
> 
>![[Pasted image 20250209170522.png]]
> 

> [!error] 错误存储方式：静态数据成员
> 
> 由于静态数据成员被所有对象共享，然而有时候需要多个引用计数。
> 
> ![[Pasted image 20250209170943.png]]
> 

> [!tip] 正确的方式：存储在堆空间中
> 
> 将引用计数存放在堆空间中，每个对象都持有引用计数的指针，从而可以让多个对象共享，也可以让不同的对象持有不同的引用计数
> 
> ![[Pasted image 20250209171203.png]]
> 
> 在 [[C++：特殊运算符#自增运算符 `++` 和自减运算符 `--` 重载]] 中我们实现 `Ptr` 类时使用了该方案
> 

引用计数正确的存储方式就是将其存储在堆空间中，然而这样会涉及两次 `new`。因为，涉及堆空间的申请一定会有系统调用的存在，每次系统调用都会有性能损失。我们可以将其优化为一次 `new`。

![[Pasted image 20250209172741.png]]

了解写时复制的原理之后，下面我们以写时复制的方式实现 `string` 类

```cpp title:string.hpp
#ifndef STRING_HPP

#include <cstddef>
#include <iostream>

namespace cow {
    class string;
    std::ostream & operator<<(std::ostream& os, const string & s);
    string operator+(string lhs, string rhs);
    string operator+(string lhs, const char *rhs);
    string operator+(const char * lhf, string rhs);
    string operator*(int lhs, string rhs);

    class string
    {

    public:

        const char * c_str() const {
            return _pstr;
        }

        size_t size() const {
            return _size;
        }

    public:
        /**
         * @brief 默认构造函数 
         * 
         * 将数据成员初始化为 nullptr。因为有内类初始化器，会将 _pstr 初始化为 nullptr
         */
        string();

        /**
         * @brief 使用 C 风格字符串初始化
         * 
         * 允许隐式类型转换，将一个 C 风格字符串转换为 string 类型的对象是非常符合逻辑的
         * 
         * @param s 指向 C 风格字符串的指针
         */
        string(const char * s); 

        /**
         * @brief 拷贝构造函数
         * 
         * @param s string 类对象的引用
         */
        string(const string & s);
    
        /**
         * @brief 析构函数
         * 
         */
        ~string();

    public:
        /**
         * @brief 拷贝赋值运算符
         * 
         * @param rhs 
         * @return string& 
         */
        string & operator=(const string & rhs);

        /**
         * @brief 原地拼接字符串
         * 
         * 检查 *_prefcont 是否大于 1。
         * + 如果是，触发 Copy-On-Write
         * + 如果不是，原地修改
         * 
         * @param rhs 右操作数 
         * @return string& 返回当前对象的引用
         */
        string & operator+=(const string &rhs);
        /**
         * @brief 原地拼接 C 风格字符串
         * 
         * 检查是否需要触发 Copy-On-Write
         * 
         * @param rhs C 风格字符串
         * @return * string& 返回当前对象的引用
         */
        string & operator+=(const char * rhs);

        /**
         * @brief 将字符串重复拼接 n 次
         * 
         * @param n 重复次数
         * @return string& 返回拼接后字符串
         */
        string & operator*=(int n);

    private:
	
        char *malloc(const char *s = nullptr);
        void release();
        
    private:
        friend std::ostream &operator<<(std::ostream& os, const string & s);
        friend string operator+(string lhs, string rhs);
        friend string operator+(string lhs, const char *rhs);
        friend string operator+(const char * lhf, string rhs);
        friend string operator*(string lhs, int rhs);
        friend string operator*(int lhs, string rhs);

    private:
        
        /*
         * _prefcont 
         * ↓
         * ==============================
         * | size_t 8 字节 | 存储字符串的内容
         * ==============================
         *                 ↑
         *                 _pstr
         */
        // _pstr 存储指向字符串起始位置的地址
        char * _pstr{nullptr};
        // _prefcont 存储指向引用计数的地址
        size_t * _prefcont{nullptr};
        using refcont_t = size_t;
        refcont_t _size{0};  // 字符串的长度
    };

} // end namespace cow 
#endif // STRING_HPP
```

上述 `string.hpp` 头文件给出了 `string` 类的数据成员和需要实现的接口。下面我们讨论 `string` 的构造函数和析构函数的实现

```cpp title:string.cpp
#include <stdexcept>
#include <cstring>

#include "string.hpp"

cow::string::string()
: _pstr{malloc()}, _prefcont{reinterpret_cast<refcont_t *>(_pstr - sizeof(refcont_t))}, _size{0}
{
    if (!_pstr)
    {
        throw std::bad_alloc();
    }
    *_prefcont = 1; // 引用计数初始化为 1
    std::cout << "cow::string::string()" << std::endl;
}

cow::string::string(const char *s)
: _pstr{malloc(s)}, _prefcont{reinterpret_cast<refcont_t *>(_pstr - sizeof(refcont_t))}, _size{strlen(s)}
{
    if (!_pstr)
    {
        throw std::bad_alloc{};
    }
    *_prefcont = 1; // 引用计数初始化为 1
    // 拷贝字符串到 _pstr 指向的位置
    // _size + 1 确保拷贝 '\0'
    for (int i = 0; i < _size + 1; i++)
    {
        *(_pstr + i) = *(s + i);
    }
    std::cout << "cow::string::string(const char *)" << std::endl;
}

cow::string::string(const string &s) : _pstr{s._pstr}, _prefcont{s._prefcont}, _size{s._size}
{
    std::cout << "cow::string::string(const string &)" << std::endl;
    // 引用计数 +1
    ++*s._prefcont;
}

cow::string::~string()
{
    release();
    std::cout<<"cow::string::~string()" << std::endl;
    if(_prefcont == nullptr) {
	    std::cout<<"delete heap" << std::endl;
    }
}

cow::string &cow::string::operator=(const cow::string &s)
{
    if (this == &s)
    {
        return *this;
    }

    // 引用计数减少 1，如果 引用计数为 0 释放对象持有的资源
    release(); 
    // 让 s._prefcont 增加 1
    ++*s._prefcont;

    _prefcont = s._prefcont;
    _pstr = s._pstr;
    _size = s._size;

    return *this;
}

char * cow::string::malloc(const char *s) {
    if(s == nullptr) {
        return new char[sizeof(refcont_t) + 1]{0} + sizeof(refcont_t);
    }
    return new char[sizeof(refcont_t) + strlen(s) + 1]{0} + sizeof(refcont_t);
}

void cow::string::release()
{
    --*_prefcont; // 引用计数减少 1
    if (*_prefcont)
    {
        return;
    }

    if (_pstr)
    {
        delete[] (_pstr - sizeof(refcont_t));
        _pstr = nullptr;
        _prefcont = nullptr;
    }
}
```

为了方便测试，我们首先实现 `operator<<()` 运算符

```cpp
std::ostream & cow::operator<<(std::ostream & os, const cow::string & s) {
    for(int i = 0; i < s._size; i++) {
        os << s._pstr[i];
    }
    return os;
}
```

下面是我们初步测试的代码：总共会调用 $3$ 次析构函数，但是只有最后一次才会释放堆内存。

```cpp
#include <iostream>

#include "string.hpp"

void f(cow::string s)
{
    std::cout << s << std::endl;
}

int main() {

    cow::string s{"hello"}; 

    f(s);  
    cow::string s2;
    s2 = s;  
    return 0;
}
```

上述代码编译运行的结果如下

```shell
➜  cppfiles g++ main.cpp string.cpp
➜  cppfiles ./a.out 
cow::string::string(const char *)
cow::string::string(const string &)
hello
cow::string::~string()
cow::string::string()
cow::string::operator=(const string &s)
cow::string::~string()
cow::string::~string()
delete heap  # 只有一次释放堆内存
```

### 写时复制实现

准备工作已经完成，下面我没开始实现写时复制的实现。首先，考虑为 `cow::string` 实现 `operator[]()` 运算符

```cpp title:string.hpp
#ifndef STRING_HPP

#include <cstddef>
#include <iostream>

namespace cow {
	// ...
    class string
    {
    // ...
    public:
        /**
         * @brief 下标取值
         * 
         * @param i 第 i 个字符
         * @return char& 返回字符的引用
         */
        char & operator[](int i);

	// ...
    };
} // end namespace cow 
#endif // STRING_HPP
```

```cpp
char & cow::string::operator[](int i)
{
    if(i < 0 || i > size())
    {
        throw std::out_of_range("out of range");
    }
    return _pstr[i];
}
```

下面是 `operator[]()` 的测试代码

```cpp
#include <iostream>

#include "string.hpp"

int main() {

    cow::string s{"hello"}; // 普通构造
    cow::string s2 {s};    // 拷贝构造

    std::cout << "s: " << s << std::endl;
    std::cout << "s2: " << s2 << std::endl;

    s[0] = 'H';

    std::cout << "s: " << s << std::endl;
    std::cout << "s2: " << s2 << std::endl;

    return 0;
}
```

编译运行的结果为

```shell
➜  cppfiles g++ main.cpp string.cpp
➜  cppfiles ./a.out 
cow::string::string(const char *)
cow::string::string(const string &)
s: hello
s2: hello
s: Hello
s2: Hello
cow::string::~string()
cow::string::~string()
delete heap
```

显然，在修改后 `s` 和 `s2` 均发生了改变。因为这里进行了写操作，出现写操作时就应该去复制资源，而不是继续在原来的资源上进行修改

> [!question] 由于 `char` 是内置类型，我们不能对内置类型进行运算符重载。应该如何解决这个问题呢？
> 
> 显然，我们需要一个与 `char` 等价的自定义类型，在这个类型上重载 `operator=()` 
> 
> 由于这个类型只在 `cow::string` 中使用，我们将其实现为 `cow::string` 的嵌套类
> 

```cpp hl:25 title:string.hpp
#ifndef STRING_HPP

#include <cstddef>
#include <iostream>

namespace cow {
    class string
    {
    private:
        class CharPoxy
        {
        public:
            CharPoxy(string & s, int idx);

            CharPoxy & operator=(char ch);  // 拦截赋值运算符
            operator char();  // 类型转换

        private:
            string &_this;
            int _idx;
        };

    private:
        // 写的时候调用该函数复制一份
        void copy();

    public:

        /**
         * @brief 下标取值
         * 
         * @param i 第 i 个字符
         * @return char& 返回字符的引用
         */
        CharPoxy operator[](int i);
    };

} // end namespace cow 
#endif // STRING_HPP
```

我们为 `cow::string` 类提供了一个 `private` 的 `copy()` 成员函数，当有修改操作时就调用该函数，从而将资源复制出来

```cpp title:string.cpp
void cow::string::copy() {

    if(*_prefcont == 1) {
        // 没有与其他对象共享
        return;
    }
    // 与其他对象共享，发生写操作。进行复制
    char * newpstr = malloc(_pstr);
    for(int i = 0; i < size() + 1; i++) {
        *(newpstr + i) = *(_pstr + i);
    }
    // 当前对象引用的资源的引用计数 -1,
    // 此时，如果引用计数为 0 则释放资源
    release();
    _pstr = newpstr;
    
    // 新资源的引用计数
    _prefcont = reinterpret_cast<refcont_t *>(newpstr - sizeof(refcont_t));
    *_prefcont = 1;
}
```

为了拦截 `cow::string` 通过下标 `operator[]()` 的修改对象的值，我们定义了一个内部类 `CharPorxy` ，让 `opertor[]()` 返回内部类的对象，内部类拦截拦截了 `operator=()`，在其中执行写操作

```cpp title:charporxy.cpp
#include "string.hpp"


cow::string::CharPoxy::CharPoxy(cow::string &s, int idx):_this{s}, _idx{idx} {}

// 在需要 char 类型的环境就是会执行该类型转换
cow::string::CharPoxy::operator char() {
    return _this._pstr[_idx];
}

cow::string::CharPoxy& cow::string::CharPoxy::operator=(char ch) {

    _this.copy();  // 复制
    // 修改
    _this._pstr[_idx] = ch;
    return *this;
}
```

> [!tip] 
> 
> 上述实现就是所谓的 **代理模式**。
> 

## 短字符串优化

当字符串的字符数小于等于 $15$ 时， `buffer` 直接存放整个字符串；当字符串的字符数大于 $15$ 时， `buffer` 存放的就是一个指针，指向堆空间的区域。这样做的好处是，当字符串较小时，直接拷贝字符串，放在 `string` 内部，不用获取堆空间，开销小。

![[Pasted image 20250210151422.png]]

```cpp
class string {
	union Buffer{
		char * _pointer;
		char _local[16];
	};
	size_t _size;
	size_t _capacity;
	Buffer _buffer;
};
```

