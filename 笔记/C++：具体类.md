# 具体类

在 [[C++：类与对象]] 中介绍了类定义的相关语法特性。现在，我们以 `Date` 类的设计为例，讨论如何设计一个简单高效的 `Date` 类并且展示语言特性如何支持这个设计

程序中会经常使用大量较小的抽象。例如：整数、浮点数、复数、指针等。每个程序都使用若干这种小的抽象，其中一些简单的具体类型经常被大量使用。

> [!tip] 
> 
> C++ 将一部分抽象作为内置类型。但是，大多数抽象并不支持，任何通用语言也很难做到支持所有的抽象。因此，语言必须为用户提供定义具体类的机制
> 

如果一个 **类的表现形式是类定义的一部分**，我们称这样的类是具体的，或者简单称为 **具体类**。具体类易于推断，编译器可以为它生成优化的代码。因此，我们更倾向于对频繁使用且性能攸关的小类型使用具体类。例如，复数、智能指针、容器

> [!tip] 定义中明确类的表现形式我们能
> + 将具体类的对象置于栈、静态内存或其他对象中
> + 拷贝和移动
> + 直接引用具名对象
> 

下面就给出 `Date` 类的设计

```cpp title:date.hpp
#ifndef DATE_HPP
#define DATE_HPP

#include <string>
using namespace std;

namespace date {
    // 月份的枚举类型
    enum class Month:int {
        illegal = 0, January=1, February, March, April, May, June, July, August, September, October, November, December
    };
    // 日期类
    class Date {
    public:
        // 成员类型：表示 Date 错误
        class Bad_date {};
        // 构造函数
        explicit Date(int = {}, int = {}, int = {});  // 防止隐式转换
        // 查看成员：非修改性函数
        int year() const;
        Month month() const;
        int day() const;
        // 修改成员：修改性函数
        Date& addYear(int);
        Date& addMonth(int);
        Date& addDay(int);
        // 日期转换为字符串
        string toString() const;
    private:
        int _year;  // 年份 year + 1970
        Month _month;        // 月份 1-12
        int _day;   // 日期 1-31
        // 检查日期是否合法
        bool is_valid() const;
    };

    // 辅助函数
    bool is_date(int, int, int);  // 检查 date 是否合法
    bool is_leapyear(int);  // 检查是否是闰年
    const Date& default_date();  // 返回默认日期

    // 运算符重载
    bool operator==(const Date&, const Date&);
    bool operator!=(const Date&, const Date&);
    bool operator<<(ostream&, const Date&);
    bool operator>>(istream&, Date&);
}  // namespace date

#endif // DATE_HPP
```

这组操作对一个用户自定义类型而言非常典型
- 构造函数指出此类型地对象/变量如何初始化
- 允许用户检查 `Date` 的函数。这些函数标记为 `const`，表面它们不会修改调用它的对象/变量的状态
- 允许用户无须了解表示细节也无须摆弄复杂语法即可修改 `Date` 的函数
- 隐式定义操作，允许 `Date` 自由拷贝
- 类 `Bad_date` 用来报告错误，抛出异常
- 辅助函数，不是类成员，不能直接访问 `Date` 的表现形式，但是我们认为它们与名字空间 `date` 的使用是相关的

## 成员函数

对于每个成员函数必须在某处给出它的实现。例如，我们在 `date.cpp` 中实现 `Date` 的构造函数

```cpp title:date.cpp
#include "date.hpp"

// 名字空间名::类名::成员函数名
date::Date::Date(int y, int m, int d)
    : _year{y}, _month{m}, _day{d}
{
    if(_year == 0) _year = default_date().year();
    if(_month == Month::illegal) _month = default_date().month();
    if(_day == 0) _day = default_date().day();
    if(!is_valid()) throw Bad_date{};
}
```

> [!tip] 
> 这个构造函数检查提供的数据是否是一个合法的 `Date`。如果不合法，则抛出异常并终止对象的建立；如果合法，则进行正确的初始化
> 
> 这个构造函数提供了一个典型的初始化实现。一旦 `Date` 对象创建成功，即可以使用和拷贝而无需再次检查。换句话说，就是 **构造函数建立了类的不变式**。**其他成员函数依赖于不变式且必须保持不变式**。这样的设计简化其他成员函数的代码
> 

构造函数 `Date()` 依赖于 `is_valid()`  成员函数用于检查 `{_year, _month, _day}` 是否合法。下面给出这个两个函数的定义

```cpp

bool Date::Date::is_valid() const {
    is_date(_year, _month, _day);
}
```

关于 `Date` 的非修改性函数可以简单定义如下

```cpp
int date::Date::year() const {
    return _year;
}

date::Month date::Date::month() const {
    return _month;
}

int date::Date::day() const {
    return _day;
}
```

这里我们采用的是复合人类阅读习惯的表现形式，然而对计算机非常不友好。这样的表现形式会造成成员函数 `addYear()` `addMonth()` 和 `addDay()` 实现起来相对复杂

```cpp
// Date 类的修改性函数
date::Date& date::Date::addYear(int n) {
    _year += n;
    if(_month == Month::February && _day == 29 && !is_leapyear(_year)) {  // 增加 n 后，如果是闰年，2 月 29 日，否则变为 3 月 1 日
        _day = 1;
        _month = Month::March;
    }
    return *this;
}

date::Date& date::Date::addMonth(int n) {
    if(n > 0) {
        int delta_year = n / 12;  // 计算年份增加的数量
        int mm = static_cast<int>(_month) + n % 12;  // 计算月份
        if(mm > 12) {
            ++delta_year;
            mm -= 12;
        }
        _year += delta_year;
        _month = static_cast<Month>(mm);
        if(_month == Month::February && (_day > 28 || _day > 29)) {
            _day -= is_leapyear(_year) ? 29 : 28;
            _month = Month::March;
        }
        if(_year < 0) throw Bad_date{};  // 超出 year 的范围
        if(_year == 0) _year = 1;
        return *this;
    }
    // n <= 0
    return *this;
}

date::Date& date::Date::addDay(int n) {
    static constexpr int days_in_month[][13] = {
        {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},  // 平年
        {0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}   // 闰年
    };
    if(n <= 0) return *this;

    if(n + _day <= days_in_month[is_leapyear(_year)][static_cast<int>(_month)]) {
        _day += n;
        return *this;
    }
    n -= days_in_month[is_leapyear(_year)][static_cast<int>(_month)] - _day;
    if(static_cast<int>(_month) == 12) {
        _month = Month::January;
        ++_year;
    } else {
        _month = static_cast<Month>(static_cast<int>(_month) + 1);
    }
    
    while (n > days_in_month[is_leapyear(_year)][static_cast<int>(_month)])
    {
        n -=  days_in_month[is_leapyear(_year)][static_cast<int>(_month)];
        if(static_cast<int>(_month) == 12) {
            _month = Month::January;
            ++_year;
        } else {
            _month = static_cast<Month>(static_cast<int>(_month) + 1);
        }
    }
    _day = n;
    return *this;
}
```

> [!tip] 
> 
> 如果需要更简单的实现，则需要修改其表现形式，例如使用零日(如，1970-1-1)起的天数
> 

目前，我们的 `Date` 类的成员函数已经完成实现了。请注意，`Date` 默认提供赋值和拷贝。而且，`Date` 的对象不持有任何资源，无需要析构函数

> [!tip] 
> 
> 如果类不显示管理资源，就不应该定义特殊的成员函数。编译器会自动生成，并且保证行为正确
> + 析构函数
> + 移动构造函数
> + 移动赋值运算符
> + 拷贝构造函数(默认逐成员拷贝)
> + 拷贝赋值函数(默认逐成员拷贝)
> 

## 辅助函数

一个类都会有一些无需定义在类内的关联函数，它们不需要直接访问类表现形式。这种函数如果定义在类中，会增加接口的复杂性，同时还是增加修改表现形式后带来的成员函数的检查数目。**通过将它们和类放在同一个名字空间中表示它们之间的关联**。自然地，我们必须在某处定义辅助函数

例如，类 `Date` 的辅助函数 `default_date()` 返回一个静态分配的局部 `Date` 变量的引用用于提供默认值

```cpp
Date& default_date() {
    static Date dd{1970, Month::January, 1};  // 静态局部变量，只初始化一次，直到程序结束时才销毁
    return dd;
}
```

辅助函数 `is_date()` 用于检查提供的值是否是一个有效的日期

```cpp
bool date::is_date(int y, int m, int d) {
    Month mm = static_cast<Month>(m);

    if(mm == Month::illegal) return false;  // 检查月份是否合法
    if(d < 1 || d > 31) return false;      // 检查日期是否合法
    // 检查二月份日期是否合法
    if(mm == Month::February) {
        return is_leapyear(y) ? d <= 29 : d <= 28;
    }
    // 检查 1, 3, 5, 7, 8, 10, 12 月份日期是否合法
    if(mm == Month::January || mm == Month::March || mm == Month::May || mm == Month::July || mm == Month::August || mm == Month::October || mm == Month::December) {
        return d <= 31;
    }
    // 检查 4, 6, 9, 11 月份日期是否合法
    return d <= 30;
}
```

辅助函数 `is_leapyear()` 用于检查当前年份是否是闰年

```cpp
bool date::is_leapyear(int y) {
    return (y % 4 == 0 && y % 100 != 0) || y % 400 == 0;
}
```

## 运算符重载

添加一些函数使得用户自定义类型能使用人们习惯的符号通常是很有用的。比如，如果需让 `Date` 的对象可以执行 `==` 和 `!=` 两个运算符，就需要为 `Date` 类定义运算符函数 `==` 和 `!=`

```cpp
bool date::operator==(const date::Date& lhs, const date::Date& rhs) {
    return lhs.year() == rhs.year() && lhs.month() == rhs.month() && lhs.day() == rhs.day();
}

bool date::operator!=(const date::Date& lhs, const date::Date& rhs) {
    return !(lhs == rhs);
}
```

当然还可能包含其他运算符

```cpp
bool operator<(Date, Date);  // 小于
bool operator>(Date, Date);  // 大于

Date& operator++(Date&);      // +1 天
Date& operator--(Date&);      // -1 天

Date& operator+=(Date&, int);  // += n 天
Date& operator-=(Date&, int);  // -= n 天

Date operator+(Date, int);  // +n 天
Date operator-(Date, int);  // -n 天

ostream& operator<<(ostream&, Date);  // 输出
istream& operator>>(istream&, Date&);  // 输入
```

这些运算符都与 `Date` 一起声明在 `Chrono` 名字空间中，以 **避免重载问题** 以及 **从参数依赖查找收益**

> [!tip] 
> 
> 对于 `Date` 类而言，这些运算符只是提供了一种便利机制。但是，对很多类型而言，例如复数、向量、类函数对象，常规运算符地使用深入人心，以至于定义这些运算符是强制地。更详细地内容在 [[C++：友元与运算符重载]] 中介绍
> 

这里我们没有提供拷贝运算符，因为默认拷贝的语义对于 `Date` 类而言完全足够
