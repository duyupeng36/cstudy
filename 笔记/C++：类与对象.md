# 类与对象

## 面向对象编程思想

在过程模式中，我们把程序看成是操纵 **被动对象** 的 **主动主体**

> [!tip] 被动对象
> 
> **数据** 和 **数据项** 构成类程序世界中的被动对象
> 

> [!tip] 主动主体
> 
> 程序本身就是发起动作的主动主体
> 

在面向过程模式下，被动对象的数据项存储在计算机内存中，程序使用 **过程** 操作被动对象

> [!tip] 过程
> 
> 所谓的过程就是程序发出的动作
> 

考虑一个文件打印的程序。在面向过程模式下，为了能打印，文件需要保持在内存中。因此，文件就是一个被动对象。程序使用一个称为 `print` 的过程，该过程告诉计算机如何打印文件中的每个字节

> [!tip] 
> 
> 程序使用过程的行为称为 **调用**
> 

在面向过程模式下，**对象和过程是完全分开的实体**。对象是一个能接收过程的独立实体

![[Pasted image 20250123184217.png]]


与面向过程模式不同，面向对象模式处理 **活动对象**，而不被动对象。活动对象能执行的动作都包含在该对象中：对象只需要接收合适的外部刺激来执行其中一个动作

> [!tip] 活动对象
> 
> 能够执行的动作都包含在对象中，对象只要接收到合适的刺激就能执行其中的动作
> 

继续考虑文件打印程序。在面向对象模式中，文件能把所有的被文件执行的过程打包在一起。在这种模式中，程序仅仅像对象发送一个相应的请求即可

![[Pasted image 20250123184417.png]]

## 类基础

**C++ 中类是创建新类型的工具**，创建出的类型可以像内置类型一样方便的使用。**派生类** 和模板允许我们 **表达类之间的关系** 并利用这些关系

> [!tip] 类型表达了一种概念
> 
> C++ 的内置类型 `float` 和 运算符 `+, -, *, /` 一起提供了数学概念 **实数** 的近似表示
> 

类是用户自定义类型。如果一个概念没有直接对应的内置类型，我们可以定义一个新类型表示这个概念

> [!tip] 
> 
> 在程序中使用与概念匹配的类型，可以使程序更容易理解，更容易分析，也更容易被修改
> 

定义一个新类型的基本思路：将实现细节与正确使用它的必要属性分离

> [!tip] 
> 
> 换句话说就是，将类型 **对象的数据存储布局** 与 **访问数据的函数的完整列表** 分开
> 

下面是定义一个类的完整语法形式

```cpp
class  Name [: 继承方式 基类,...]
{
访问控制限定符:
	// 构造函数的定义
	Name(形参表): 成员变量{初值}, ... {}// 或者 Name(形参表); 仅仅是声明
	// 析构函数的声明
	~Name();

	// 成员函数的声明
	返回类型 函数名(形参表) [const] [异常说明];
	
	// 表现形式
	数据类型 变量名;
};  // 注意，此处必须要有分号
```

例如，一个类 `X` 的定义如下

```cpp
class X
{
private:  // 类的实现细节，是私有的
    int m; 
public:   // 用户接口是公有的
    X(int i = 0) : m{i} {}; // 构造函数，初始化数据成员 m

    int mf(int i)
    {
        int old = m;
        m = i; // 将数据成员 m 设置为新值
        return old; // 返回旧值
    }
};

X var{7}; // 一个 X 类型的变量，初始化为 7

int user(X var, X* ptr)
{
    int x = var.mf(7); // 使用 var.mf() 成员函数
    int y = ptr->mf(9); // 使用 ptr->mf() 成员函数
    int z = var.m; // 错误：不能访问私有成员
}
```

### 成员函数

考虑用 `struct` 实现日期的概念：定义 `Date` 的表示方式和操纵这种类型变量的一组函数

```cpp
struct Date
{
    int __year;  // 年
    int __month;  // 月
    int __day;  // 日
};

void init_date(Date& d, int, int, int); // 初始化日期
void addDays(Date& d, int n);           // 增加 n 天
void addMonths(Date& d, int n);         // 增加 n 月
void addYears(Date& d, int n);          // 增加 n 年
```

> [!tip] 
> 
> 数据类型 `Date`  和这些函数之间并没有显示关联
> 

我们可以通过将函数声明为成员来建立这种关联

```cpp
struct Date
{
    int __year;  // 年
    int __month;  // 月
    int __day;  // 日
    void init(int, int, int);  // 初始化日期
    void addDays(int n);           // 增加 n 天
    void addMonths(int n);           // 增加 n 月
    void addYears(int n);           // 增加 n 年
};
```

**声明(定义)于类定义内的函数 称为 _成员函数_**，对恰当类型的特定变量使用成员访问语法才能调用这种函数

> [!tip] 
> 
> 结构体也是一种类
> 

```cpp
Date birthday; 
void f()
{
    Date today;

    today.init(2007, 12, 24);// 初始化日期为 2007 年 12 月 24 日
    birthday.init(1963, 5, 19);// 初始化日期为 1963 年 5 月 19 日

    Date tomorrow = today; // 将 tomorrow 初始化为 today
    tomorrow.addDay(1);       // 增加 1 天
    // ...
}
```

由于不同的结构可能有同名的成员函数，在定义成员函数时必须指定结构名(类名)

```cpp
void Date::init(int yy, int mm, int dd)
{
    __year = yy;
    __month = mm;
    __day = dd;
}
```

在成员函数中，不必显示引用对象即可以使用成员的名字。名字所引用的是调用该函数的对象的成员。例如，当 `today` 调用 `Date::init()` 时，`__month = mm;` 是对 `today.__month` 赋值

> [!tip] 
> 
> **类的成员函数知道哪个对象调用它**。它是怎么知道的呢？其原因放在后面介绍
> 

### 默认拷贝

**默认情况下，对象是可以拷贝的**。特别是，一个类对象可以用同类的另一个对象的副本进行初始化

```cpp
Date d1 = birthday; // 使用 birthday 的副本初始化 d1
Date d2{birthday};  // 使用 birthday 的副本初始化 d2
```

> [!tip] 
> 
> 默认情况下，**一个类对象的副本是逐成员拷贝得到**。如果类 `X` 的默认拷贝行为不是我们希望的，可以提供更恰当的行为
> 

类似的，类对象默认也是通过赋值操作拷贝

```cpp
void f(Date &d)
{
	d = birthday;
}
```

> [!attention] 
> 
> **默认拷贝语义是逐成员复制**。如果对于类 `X` 是不正确的选择，用户可以定义一个恰当的赋值运算符
> 

### 访问控制

前面我们提到的 `Date` 声明提供了一组处理 `Date` 对象的函数，但并未指定是否只有这些函数依赖于 `Date` 的表现形式以及是否只有它们直接访问类 `Date` 对象。这种约束可以通过用 `class` 关键字替代 `struc` 来表达

```cpp
class Date
{
    int __year;  // 年
    int __month;  // 月
    int __day;  // 日
    
public:
    void init(int, int, int); // 初始化日期
    void addDays(int n);           // 增加 n 天
    void addMonths(int n);         // 增加 n 月
    void addYears(int n);          // 增加 n 年
};
```

> [!tip]
> 
> 标签 `public` 将类的主体分为两部分：第一部分中的名字是 **类私有的**(`private`)，它们只能被类中的成员函数使用。第二部中的名字是 **公开的**(`public`)，构成类对象的公共接口
> 

> [!attention] 
> 
> `struct` 是一个成员默认为 `public` 的 `class`，成员函数的声明和使用是一样的
> 

```cpp
void Date::init(int yy, int mm, int dd)
{
    __year = yy;
    __month = mm;
    __day = dd;
}

void Date::addDays(int n)
{
    __day += n;
}
```

> [!warning] 
> 
> 但是，**非成员函数禁止使用私有成员**
> 

```cpp
void timewarp(Date& d)
{
    d.__year -= 200; // 错误: 试图访问类 Date 私有的成员
}
```

现在 `init()` 就非常重要了，因为数据设定为私有迫使我们提供一组初始化成员的方法

```cpp
Date dx;

dx.__month = 3;  // 错误： 试图访问 Date 的私有成员
dx.init(2011, 3, 25); // 正确
```

> [!important] 限制一组显示声明的函数才能访问数据结构可以带来多方面的好处
> + 任何导致对象保存非法数据的错误都必然是由成员函数中的代码引起的
> + 如果我们改变了一个类的表示方式，就只能修改成员函数来利用新的表示方式。用户代码依赖于公共接口，因此，无效重写
> + 聚焦于设计一个好的接口能产生更好的代码，因为我们可以对调试投入更多的思考和时间
> 

**私有数据的包含依赖于对类成员名的使用限制**。因此，通过地址操作和显示类型转换可以绕过私有保护。**C++ 只能防止意外而无法防止故意规避**。只有硬件才能完美防止对通用语言的恶意使用，而在实际系统中其实很难实现

```cpp
Date dx;
Date * pd = &dx;
int *pi = reinterpret_cast<int*>(pd);  // 将 dx 的指针转换为 int *
*pi = 2023; // 试图修改 dx 的年份
*(pi + 1) = 12; // 试图修改 dx 的月份
*(pi + 2) = 7; // 试图修改 dx 的日期
dx.print(); // 2023-12-7
```

### 构造函数

> [!warning] 
> 
> 使用 `init()` 这样的函数为类对象提供初始化功能不优雅也容易出错
> 
> 这种方式没有规定一个对象必须进行初始，程序员 **可能忘记初始化**，或者 **初始化两次**(初始化两次往往会带来灾难性后果) 
> 

一种更好的方法是允许程序员声明一个函数，它显示表明自己是专门完成对象初始化任务的。这种函数的本质是 **构造一个给定类型对象的值**，因此被称为 **构造函数**。**它最显著特征就是与类具有相同的名字**

> [!tip] 构造函数：初始化一个给定类型对象的值
> 
> 构造函数的显著特征是与类具有相同的名字
> 

```cpp
class Date
{
public:
    // void init(int, int, int); // 初始化日期：容易出错
    Date(int, int, int); // 构造函数
    // ....

private:
    int __year;  // 年
    int __month;  // 月
    int __day;  // 日
};
```

> [!tip] 
> 
> **如果一个类有构造函数，其所有对象都会通过调用构造函数完成初始化**。如果构造函数需要参数，在初始化就要提供这些参数
> 

```cpp
Date today = Date(2023, 12, 7); // 使用 Date 的构造函数初始化 today
Date xmas(2023, 12, 25); // 简写形式

Date birthday; // 错误：缺少初始化器
Date realease1_0(2023, 12);  // 错误：漏掉第三个参数
```

构造函数定义了类的初始化方式，我们可以使用花括号 `{}` 初始化器

```cpp
Date today = Date{2023, 12, 7}; // 使用 Date 的构造函数初始化 today

Date xmas {2023, 12, 25}; // 简写形式

Date realease1_0{2023, 12};  // 错误：漏掉第三个参数
```

> [!tip] 构造函数可以重载
> 
> 通过提供多个构造函数，可以为某些类型的对象提供多种不同的初始化方法
> 

```cpp
class Date
{
public:
    
    Date(int, int, int); // 构造函数：年 - 月 - 日
    Date(int, int);      // 构造函数：日 - 月 - 当前年
    Date(int);           // 构造函数：日 - 当前月 - 当前年
    Date();              // 构造函数：当前日期
    Date(const char *);  // 构造函数：字符串表示的日期
    // ....

private:
    int __year;  // 年
    int __month;  // 月
    int __day;  // 日
};
```

构造函数的重载解析规则与普通函数的重载解析规则相同，只要构造函数的参数类型明显不同，编译器就能选择正确的版本使用

```cpp
Date today {4}; // 4 日，当前月，当前年： 调用 Date::Date(int)
Date july4 {"July 4, 1983"}; // 1983年7月4日: 调用 Date::Date(const char*)
Date guy{5, 11};  // 5 日，11月，当前年: 调用 Date::Date(int, int)
Date now; // 默认初始化为今天: 调用 Date::Date()
Date start{}; // 默认初始化为今天: 调用 Date::Date()
```

`Date` 的前 $4$ 个构造函数是相关联的，只是参数的数量不同。通过 默认参数可以有效减少这种相关联的函数

```cpp
class Date
{
public:
    
    Date(int = 0, int = 0, int = 0); // 构造函数：年 - 月 - 日
    // Date(int, int);      // 构造函数：日 - 月 - 当前年
    // Date(int);           // 构造函数：日 - 当前月 - 当前年
    // Date();              // 构造函数：当前日期
    Date(const char *);  // 构造函数：字符串表示的日期
    
    // ...

private:
    int __year;  // 年
    int __month;  // 月
    int __day;  // 日
};

Date :: Date(int dd, int mm, int yy)
{
    __day = dd ? dd : today.__day; // 如果 dd 为 0，使用 today.d
    __month = mm ? mm : today.__day; // 如果 mm 为 0，使用 today.m
    __year = yy ? yy : today.__day; // 如果 yy 为 0，使用 today.y

	// 检测 Date 是否合法
} 
```

### explicit 构造函数

默认情况下，用单一参数调用一个构造函数，其行为是从参数类型到类自身的隐式类型转换。对于像 **复数** 这样的隐式类型转换会非常有用；忽略虚部，就会得到实数轴上的一个复数，这是符合数学定义的

```cpp
complex<double> x {1};  // complex<double>(1.0, 0.0)
```

> [!tip]
> 
> 对于多数的其他类型，这种隐式类型转换往往是混乱和错误的根源
> 

比如，下列代码使用了我们定义的 `Date` 类，第 $7$ 行和第 $8$ 行可读性非常差，因为数值 $15$ 和 `Date` 之间并无明显清晰的逻辑关联

```cpp hl:7,8
void my_fct(Date d);

void f()
{
	Date d{15};  // d 变为 {15, today.m, today.y}
	// ...
	my_fct(15); // 混乱的
	d = 15;     // 混乱的
}
```

幸运的是，**C++ 支持我们指明构造函数不能作为隐式类型转换**。如果构造函数的声明带有关键字 `explicit`，则它只能用于初始化和显示类型转换

> [!tip] 
> 
> 关键字 `explicit` 清楚的说明了构造函数仅仅用于构造对象。即，被限定为 `explicit` 的构造函数只能用于初始化和显示类型转换
> 

```cpp hl:16
class Date
{
public:
    
    explicit Date(int = 0, int = 0, int = 0); // 构造函数：年 - 月 - 日
    explicit Date(const char *);  // 构造函数：字符串表示的日期
    // ...

private:
    int __year;  // 年
    int __month;  // 月
    int __day;  // 日
};

Date d1{15}; // 正确：直接初始化
Date d2 = Date{15}; // 正确：拷贝初始化
Date d3 = {15}; // 错误：不允许隐式类型转换
Date d4 = 15; // 错误：不允许隐式类型转换

void f()
{
    my_fct(15); // 错误：不允许隐式类型转换
    my_fct({15}); // 错误：不允许隐式类型转换
    my_fct(Date{15}); // 正确：显示类型转换
}
```

上面代码中的第 $16$ 行使用等号(`=`) 的方式初始化可以看做 **拷贝初始化**

> [!tip] 拷贝初始化
> 
> 初始化器的副本被放入待初始化的对象中
> 
> 注意：如果初始化器是一个右值，这种拷贝可能会被编译器优化掉，而采用移动的方式进行初始化
> 

> [!tip] 直接初始化：省略拷贝初始化中的 `=`
> 
> 省略 `=` 会将初始化变为 **显示初始化**，也称 **直接初始化**
> 

默认情况下，应该始终将单参构造函数声明为 `explicit`。除非有很好的理由，否则的话就应该按默认方式

> [!hint] 
> 
> 如果定义隐式类型转换，最好注明原因，否则代码维护者可能怀疑你疏忽了或则不懂这一原则
> 

请注意：如果一个构造函数声明为 `explicit` 且定义在类外，则定义构造函数是不需要重复 `explicit`

```cpp
class Date {
	// ... 表现形式
public:
	explicit Date(int dd);
};

Date::Date(int dd) {/* ... */} // 正确的
explicit Date:Date(int dd) {/* ... */} // 错误
```

> [!tip] 
> 
> `explicit` 也可以用于无参或多参构造函数，只是大多数 `explicit` 起重要作用的都是接受单一参数的构造函数
> 

### 类内初始化器

当使用多个构造函数时，成员初始化可以是重复的

```cpp
class Date
{
public:
    
    Date(int = 0, int = 0, int = 0); // 构造函数：年 - 月 - 日
    Date(int, int);      // 构造函数：日 - 月 - 当前年
    Date(int);           // 构造函数：日 - 当前月 - 当前年
    Date();              // 构造函数：默认 Date: today
    Date(const char *);  // 构造函数：字符串表示的日期
    // ...

private:
    int __year;  // 年
    int __month;  // 月
    int __day;  // 日
};
```

通过引入默认参数，可以减少构造函数的数量来解决重复初始化的问题。另一种方法就 **为数据成员添加初始化器**

```cpp
class Date
{
public:
    
    Date(int = 0, int = 0, int = 0); // 构造函数：年 - 月 - 日
    Date(int, int);      // 构造函数：日 - 月 - 当前年
    Date(int);           // 构造函数：日 - 当前月 - 当前年
    Date();              // 构造函数：默认 Date: today
    Date(const char *);  // 构造函数：字符串表示的日期
    // ...

private:
    int __year{today.__year};  // 年：类内初始化器
    int __month{today.__month};  // 月
    int __day{today.__day};  // 日
};
```

> [!tip] 类内初始化器
> 
> 在类内部直接给成员变量添加的初始化器
> 

通过类内初始化器，现在每个构造函数都已经初始化了类的表现形式。当然，构造函数也可以携带 **初始化列表**，从而完成对类数据成员的初始化

> [!tip] 构造函数初始化列表
> 
> 构造函数的初始化列表位于构造函数的形式参数列表之后，函数体之前。用冒号(`:`)开始，如果有多个数据成员，则用逗号分隔
> 

```cpp
Date :: Date(int dd, int mm, int yy): d{dd} // 类内初始化器
{
   // 检测 Date 是否合法
} 
```

这段代码等价于

```cpp
Date :: Date(int dd, int mm, int yy): d{dd}, m{today.m}, y{today.y}
{
   // 检测 Date 是否合法
} 
```

### 类内函数定义

如果一个函数不仅在类中声明，还在内中定义，那么该函数被当做内联函数^[[[C++：与 C 的差异#inline 函数]]]

> [!tip] 
> 
> 只有那些很小并且不经常修改的函数才适合在类中定义
> 

类中定义的函数默认被当做内联函数处理。这样可以在多个编译单元中使用 `#inlcude` 重复包含类内定义的成员函数，无论在哪里使用 `#include`，其含义都是保持一致的

> [!important] 
> 
> 类成员可以访问同类的其他成员，不管成员在哪里定义。所以，**函数和数据成员的访问不依赖与声明的顺序**
> 
> 这种风格常用来 **保持类定义更为简单易读**。它还 **实现了类接口和类实现在文本上的分离**
> 

```cpp
// 在类中定义函数
class Date
{
public:
	//....
    void addMonths(int n) {__month += n;};  // 在类中定义

private:
    int __year;  // 年
    int __month;  // 月
    int __day;  // 日
};

// 在类外定义函数
class Date
{
public:
	//....
    void addMonths(int n);         // 增加 n 月

private:
    int __year;  // 年
    int __month;  // 月
    int __day;  // 日
};

inline void Date::addMonths(int n)
{
	__month += n; 
}
```

### 可变性

一个名字既可以是一个保存 **不可变值** 的对象，也可以是一个保存 **可变值** 的对象

> [!tip] 不可变值
> 
> 某些变量被 `const` 限制为只读，称为 `const` 变量
> 

系统地使用不可变对象有利于产生更容易理解的代码，有利于尽早发现错误，而且有时会提高性能

> [!tip]
> 
> 不可变性在多线程编程中是一个非常有用的特性
> 

为了使不可变性不局限于内置类型的简单常量定义，必须能定义可操作用户自定义 `const` 对象的函数。对于独立的函数而言，这意味着函数可接受 `const T&` 参数。而对类而言，这意味着我们必须定义能操作 `const` 对象的成员函数

#### 常量成员函数

到目前为止，我们所定义的 `Date` 提供了一些能为 `Date` 赋值的函数。不幸的是，我们没有提供检查 `Date` 值的方法。通过增加读取天、月和年

```cpp hl:6,9,12
class Date {
    int __year;   // 年
    int __month;  // 月
    int __day;    // 日
public:
    int year() const {
        return __year;
    }
    int month() const {
        return __month;
    }
    int day() const {
        return __day;
    }
    
    // 增加 y 年
    void addYears(int y) {
        __year += y;
    }
    // ...
};
```

在上述代码高亮部分的函数声明中，参数列表后的 `const` 指明这些函数不会修改 `Date` 的状态

> [!tip] 编译器试图检查修改 `Date` 对象的代码
> 
> 编译器会试图捕获违反此承诺的代码
> 

```cpp
int Date::year() const 
{
    return ++y; // 错误：试图在 const 函数中改变成员值
}
```

> [!tip] 
> 
> `const` 成员函数的定义在类外时，必须使用 `cosnt` 后缀。因为  `const` 是常量成员函数类型的一部分
> 

```cpp
int Date::year() //  // 错误：遗漏了 cosnt 声明后缀
{
    return y;
}
```

`const` 和非 `const` 对象都可以调用 `cosnt` 成员函数。非 `const` 成员函数只能被非 `const` 对象调用

```cpp
void f(Date& d, const Date & cd)
{
    int i = d.year();  // OK
    d.addDays(10); // OK
    
    int j = cd.day(); // OK
    cd.addYears(1); // 错误：不能改变 cont Date 对象的值
}
```

#### 逻辑常量性

> [!tip] **逻辑常量性**：`const` 成员函数修改了用户不能直接观察的某些细节
> 
> 一个成员函数逻辑上是 `const`，但它仍需要改变成员的值。换句话说就是，对一个用户而言，函数看起来不会改变其对象的状态，但它更新了用户不能直接观察的某些细节
> 

例如，类 `Date` 可能有一个返回字符串表示的函数。构造这个字符串表示非常耗时，因此，保存一个拷贝，在反复要求获取字符串表示时可以简单的返回此拷贝，除非 `Date` 的值已经被改变。

```cpp
#include <string>

using namespace std;

class Date
{
public:
	// ...
    string date2str() const; // 字符表示的 Date
    
private:
	// ...
    bool cache_valid;
    string cache;
    string _date2str(); // 计算并填入缓存
};
```

从用户角度来看，`date2str` 并未改变其 `Date` 的状态，因此它显示应该是一个 `const` 成员函数。然而，有时候必须改变成员 `cache` 和 `cache_valid`，这种设计才能起作用

#### mutable

我们可以将一个类成员定义为 `nutable`，表示即使在 `const` 对象中，也可以修改此成员

```cpp
class Date
{
public:
	// ...
    string date2str() const;  // 字符串时间

private:
	// ...
    mutable bool ccache_valid;  // 可变成员:即使在 const 中，也可以被修改
    mutable string cache;  // 可变成员
    string _strdate(); // 计算并填入可变的缓存
};
```

现在可以定义 `date2str()` 

```cpp
string Date::date2str() const {
    if(!cache_valid)
    {
        _data2str();
        cache_valid = true;
    }
    return cache;
}
```

> [!tip]
> 
> `date2str()` 即可用于 `const` 对象，也可用于非 `const` 对象 
>  

#### 间接访问实现可变性

对于小对象的表现形式只有一小部分允许改变的情形，将成员声明为 `mutable` 是最合适的。但是，通常更好的方式是 **将需要改变的数据存分在一个独立的对象** 中，**间接访问它们**

```cpp

struct cache {
	bool valid;
	string rep;
};

class Date {
public:
	// ...
	string date2str() const

private:
	cache * c;
	void compute_cache_value() const;
	// ....
};

string Date::date2str() const {
    if(!c->valid)
    {
        compute_cache_value();  // 计算并填入缓存
        c->valid = true;
    }
    return c->rep;
}
```

> [!tip] 
> 
> 支持缓存的编程技术可以推广到各种形式的懒惰求值
> 

注意，`const` 不能应用到通过指针或在引用访问的对象。编译器不能将这种指针或引用与其他指针或引用区分开来。即，**成员指针没有任何与其他指针不同的特殊语义**

### 自引用

我们定义的状态更新函数 `addYears(), addMonths(), addDays()` 是不返回值的。对于这样一组的更新函数，一种通常很有用的技术是令它们返回已更新对象的引用，这样这些操作可以串联起来

```cpp
void f(Date &d)
{
	//...
	d.addYears(1).addMonths(1).addDays(1);
	//...
}
```

为了实现这种链式调用，必须将每个函数都声明为返回一个 `Date` 引用

```cpp
class Date
{
public:
    //...
    Date& addDays(int n);           // 增加 n 天
    Date& addMonths(int n);         // 增加 n 月
    Date& addYears(int n);          // 增加 n 年
    //...
};
```

非 `static` 成员函数都知道哪个对象调用它，并能显示引用这个对象

```cpp
Date& Date::addYears(int yy) {
    if(__day == 29 && __month == 2 && !leapyear(y+yy))  // 小心 2 月 29
    {
        __day = 1;
        __month = 3;
    }
    __year += yy;
    return  *this;
}
```

> [!tip] `this` 指针
> 
> `this` 是一个指针，指向调用该成员函数的对象
> 
> 在类 `X` 的非 `const` 成员函数中，`this` 的类型是 `X *`。`this` 是一个右值，无法获得 `this` 的地址，或给 `this` 赋值
> 
> 在类 `X` 的 `const` 成员函数中，`this` 的类型是 `const X *`，防止意外修改对象的状态
> 

`this` 的使用大多数是隐式的。每当我们引用类内的一个非 `static` 成员时，都依赖于一次 `this` 的隐式使用来获得恰当对象的该成员。例如，成员函数 `addYears` 可以如下定义

```cpp
```cpp
Date& Date::addYears(int yy) {
    if(this->__day == 29 && this->__month == 2 && !leapyear(this->__year + yy))  // 小心 2 月 29
    {
        this->__day = 1;
        this->__month = 3;
    }
    this->__year += yy;
    return  *this;
}
```

### static 成员

为 `Date` 设定默认值的确非常方便，但是会带来潜在问题。因为 `Date` 类会依赖全局变量 `today`。这样 `Date` 类只能用于定义和正确使用 `today` 的上下文中。

> [!attention] 
> 
> 这就限制一个类只有在最初编写它的上下文才有用。尝试使用这种上下文依赖的类会给用户带来很多意料之外的不快，代码维护也变得很混乱
> 

幸运的是，我们可以是使用 `static` 成员从而获得这种便利性其实不需要承担使用公开访问的全局变量的负担。


> [!tip] `static` 成员：属于类但不属于类的任何对象
> 
> 是类的一部分但不是某个类对象的一部分的成员，静态(`static`)成员
> +  **static 变量**： 只有唯一副本，而不是像普通非 `static` 成员那样每个对象都有其副本
> + **static 函数**： 需要访问类成员但又不需要通过特定对象调用的函数
> 

下面是重新设计的版本，它保留了 `Date` 默认构造函数的语义，又没有依赖全局变量

```cpp
#ifndef DATE_H
#define DATE_H

class Date {
public:
	// ...
    static void setDefault(int year, int month, int day);  // 静态成员函数
private:
    int __year;   // 年
    int __month;  // 月
    int __day;    // 日

    static Date default_date;  // 静态数据成员
    // ...
};

#endif //DATE_H
```

使用 `default_date` 的 `Date` 构造函数如下

```cpp
Date::Date(int dd, int mm, int yy)
{
    __day = dd ? dd : default_date.__day;
    __month = mm ? mm : default_date.__month;
    __year = yy ? yy : default_date.__year;
    // 检查 Date 是否合法
}
```

使用 `set_default()`，在恰当的时候改变默认值。可以x像引用任何其他成员一样引用 `static` 成员。此外，不必提及任何对象即可引用 `static` 成员，方法使用 `类名::`

```cpp
void f()
{
	Date::set_default(4, 5, 1945);
}
```

**如果使用了 static 函数或数据成员，就必须在某处定义它们**。在 `static` 成员的定义中不要重复关键字 `static`

```cpp
Date Date::default_date{16, 12, 1770}; // Date::default_date 的定义

void Date::set_default(int dd, int mm, int yy) { //  Date::set_default 的定义
    default_date = {dd, mm, yy};
}
```

> [!attention] 
> 
> 注意，写在 `class` 作用域内仅仅是声明，不会为其分配内存空间
> 

注意，`Date{}` 表示 `Date::default_date` 的值。因此，我们不需要一个独立的函数来读取默认值。而且，目标类型为 `Date` 无疑时，更简单的 `{}` 就足够了

下面是关于 `static` 数据成员与 `static` 成员函数的基本使用方法

```cpp
#include <iostream>
using namespace std;
class Date
{
private:
    int d, m, y;
    static Date default_date;  // 静态成员，属于类但不属于类对象中

public:
    Date(int = 0, int = 0, int = 0);  // 构造函数

    // 非修改性函数，用于查询 Date
    int year() const {return y;}
    int month() const {return m;}
    int day() const {return d;}

    static void set_default(int dd, int mm, int yy);  // 静态成函数员
};


Date::Date(int dd, int mm, int yy) {
    d = dd ? dd : default_date.day();
    m = mm ? mm : default_date.month();
    y = yy ? yy : default_date.year();
}

// 注意，在定义静态函数时，不需要重复关键字 static
void Date::set_default(int dd, int mm, int yy) {
    default_date = {dd, mm, yy};
}


Date Date::default_date {16, 12, 1770};  // 使用前需要先定义

int main()
{
    Date::set_default(4, 5, 1945);  // 修改默认值
    Date date;
    cout << date.day() << '/' << date.month() << '/' << date.year() << endl;
}
```


### 成员类型

> [!tip] 
> 
> 类型和类型别名也可以作为类的成员
> 

```cpp
template <typename T>
class Tree {
    using value_type = T;
    enum Policy {rb, splay, treeps};
    class Node 
    {
        value_type value;
        Node* left;
        Node* right;
    public:
        void f(Tree *);
    };
    Node * top;
public:
    void g(const T&);
};
```

> [!tip] 成员类
> 
> 常称为嵌套类，**可以引用其所属类的类型和 `static` 成员**
> 
> 注意：当给定所属类的一个对象时，只能引用非 `static` 成员
> 

**嵌套类可以访问其所属类的成员**，甚至是 `privite` 成员，与成员函数类似。但是，它 **没有当前类对象的概念**

```cpp
template <typename T>
void Tree<T>::Node::f(Tree * p) {
    top = right; // 错误：未指定类型为 Tree 的对象
    p->top = right; //
    value_type v = left->value; // 正确：value_type 不予某个对象关联
}
```

相反，一个类没有任何特殊权限能访问其嵌入类的成员

```cpp
template <typename T>
void Tree<T>::g(Tree::Node * p) {
    value_type val = right->value; // 错误: 没有 Tree::Node 类型的对象
    value_type v = p->right->value; // 错误: Node::right 是私有的
    p->f(this); // 正确：this 的类型是 Tree<T> *
}
```

> [!tip] 
> 
> **成员类更多是提供了一种符号表示上的便利**，而非一种重要的语言特性
> 
