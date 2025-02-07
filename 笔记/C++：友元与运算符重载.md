# 友元与运算符重载

## 友元

一个普通的成员函数声明语句在逻辑上包含相互独立的 $3$ 层含义

> [!tip] 第一层：该函数有权访问类的私有成员

> [!tip] 第二层：该函数位于类的作用域中
> 

> [!tip] 第三层：必须用一个含有 `this` 指针的对象调用该函数

在 [[C++：类与对象#static 成员]] 中我们学习的 `static` 成员函数限制它只有前 $2$ 层含义。如果想要一个函数的声明只具有第一层含义，我们可以使用关键字 `friend` 去声明它

例如，我们可以定义一个计算 `Matrix` 与 `Vector` 乘积的运算符。`Vector` 和 `Matrix` 会隐藏它们各自的表现形式，并向外提供一组可操作对象的函数。然而，我们的乘法运算不应使用它们之中任何一个成员，而且我们也不希望用户通过底层操作访问 `Vector` 和 `Matrix` 的完整表现形式。为了避免这样，我们把 `operator*()` 声明成这两个类的 `friend`

```cpp
constexpr rc_max {4};  // 行列尺寸

class Vector
{
	float v[rc_max];
	friend Vector operator*(const Maxtrix&, const Vector&);
};

class Matrix
{
	Vector v[rc_max];
	friend Vector operator*(const Maxtrix&, const Vector&);
};
```

此时 `operator*()` 就可以访问 `Vector` 和 `Matrix` 的表示部分了。下面是一个简单的实现版本

```cpp
Vector operator*(const Matrix& m, const Vector v)
{
	Vector r;
	for(int i = 0; i!= rc_max; i++)
	{
		r.v[i] = 0;
		for(int j = 0; j != rc_max; j++)
		{
			r.v[i]+=m.v[i].v[j] * v.v[j];
		}
	}
	return r;
}
```

> [!tip] 
> 
> `friend` 声明即可以位于类的私有部分，也可以位于公有部分，二者没有什么差别。就像一般的成员函数一样，**友元函数应该显式地声明在类地内部**，它们共同构成了该类地完整接口
> 

一个类的成员函数也可以是另一个类的友元。例如，我们为 `List` 的迭代器 `List_iterator`

```cpp
class List_iterator
{
	int *next();
};

class List{
	friend int * List_iterator::next();
};
```

如果想要一个类的所有成员函数成为另一个类的友元，可以简单的写为

```cpp
class List
{
	friend class List_iterator;  // j将类 List_iterator 声明为 List 的友元类 
};
```

为类声明 `friend` 可以授权访问该类的所有函数。这意味着我们其实不了解函数的细节，只有深入友元类的内部专门查看后才能知道到底那些函数被赋予访问权限。在这一点上友元类声明与成员函数和友元函数声明有所区别。显然，**必须慎用友元类**，我们 **应该只用它表示那些确实有紧密关系的概念**

可以把模板参数设置为 `friend`

```cpp
template<typename T>
class X
{
	friend T;
	friend class T; // class 是多余的
};
```

**友元必须在类的 _外层作用域提前声明_，或者定义在 _直接外层非类作用域中_**。对于在最内层嵌套名字空间作用域内首次声明成 `friend` 的名字来说，它的友元性到了更外层的作用域就失效了

```cpp

class C1{}; // 将成为 N::C 的友元
void f1(); // 将成为 N::C  的友元

namespace N {
    class C2{}; // 将成为 C 的友元
    void f2(); // 将成为 C 的友元

    class C {
        int x;
    public:
        friend class C1;  // ok，已预先定义
        friend void f1();  // ok，已预先声明

        friend class C3; // Ok, 在外层作用域中定义
        friend void f3(); // Ok, 在外层作用域中定义

        friend class C4; // 首次出现在名字空间 N 内，因此，友元关系只存在于此
        friend void f4(); 
    };
    /* 定义 */
    class C3{}; // 将成为 C 的友元
    void f3(); // 将成为 C 的友元
}

class C4{}; // 不是 N::C 的友元
void f4(){N::C x; x.x = 1;}; // 错误：x 是私有变量并且 f4() 不是 N::C 的友元
```

> [!tip] 
> 
> + 友元可以访问类的私有成员，**破坏了类的封装**
> 
> + **友元是单向的**：A 是 B 的友元，A 可以访问 B 的私有属性。但并不代表 B 是 A 的友元，除非 B 也声明 A 是其友元
> 
> + **由于不具有传递性**：A 是 B 的友元，B 是 C 的友元，不能推出 A 是 C 的友元
> 
> + **友元不能被继承**
> 
> 总结：也就是说，**友元必须被声明**
> 

## 运算符重载

C++ 预定义中的运算符的操作对象只局限于基本的内置数据类型，但是对于自定义的类型是没有办法操作的。当然我们可以定义一些函数来实现这些操作，但考虑到用运算符表达含义的方式很简洁易懂，**当定义了自定义类型时，也希望这些运算符能被自定义类类型使用，以此提高开发效率，增加代码的可复用性**。为了实现这个需求，C++提供了运算符重载。其指导思想是：**希望自定义类类型在操作时与内置类型保持一致**。

我们可以声明一些新的函数，令其表示下面的运算符

```
+     -     *     /     %      ^        & 
|     ~     !     =     >      <        +=
-=    *=    /=    %=    ^=     &=       |=
<<    >>    >>=   <<=   ==     !=       <=
>=    &&    ||    ++    --     ->*      ,
->    []    ()    new   new[]  delelte  delelte[]
```

> [!attention] 请注意：下列运算符不能被重载
> 
> 下列 $3$ 个运算符接受一个名字作为第二个操作数，主要的作用是指向或引用成员
> + `::` 作用域限定符
> + `.` 成员运算符
> + `.*` 通过指向成员的指针访问成员
> 
> 下列 $3$ 个运算符负责报告运算对象的基本信息，也不能重载
> + `sizeof`: 对象尺寸
> + `alignof`: 对象的对齐方式
> + `typeid`: 对象的 `type_info`
> 
> 还有一个三元条件表达式也不能重载
> + `?:` 条件表达式
> 

C++ 还允许我们自定义字面值，通过 `operator""{delim}()` 的方式自定义字面值常量。甚至，C++ 还允许我们自定义类型转换，通过 `operator T()` 的方式。

> [!tip] 
> 
> `operator""` 并没有影响 `""` 运算符，只是语法上的借鉴。同理，`operator T()` 也没有影响类型 `T` 
> 

> [!warning] 警告：C++ 不允许定义新的运算符

运算符函数的名字按照 `operator@()` 的规则进行命名，其中 `@` 就是需要重载的运算符

> [!tip] 
> 
> + 运算符重载只是改变了运算符的计算逻辑，而不会改变运算符的优先级和结合性
> 
> + `&&` 和 `||` 被重载后就不具备短路求值特性
> 

### 二元和一元运算符

重载二元运算符的方式有两种方式：**接收一个参数的非 static 成员函数** 和 **接收两个参数的非成员函数(友元函数)**

```cpp
class X
{
public:
	X operator@(X); // X operator(this, x)
};

X operator@(X, X);
```

重载一元运算符的方式也有两种：**不接收参数的非 static 成员函数** 和 **接收一个参数的非成员函数(友元函数)**

```cpp
class X
{
public:
	X * operator&();     // 一元 & 取地址
	X operator&(X);      // 二元 & 按位与
	X operator++(int);   // 后置递增运算符：参数列表中的 int 仅仅只是与前置 ++ 进行区别
	X operator++();      // 前置递增运算符
	X operator&(X,X);    // 错误：三元运算符了
	X operator/();       // 错误: 没有一元 /
};

X operator-(X);          // 前置一元负号
X operator-(X, X);       // 二元减法
X operator--(X&, int);   // 后置递减运算符
X operator--();          // 前置递减运算符
X operator-();           // 错误：缺少运算对象
X operator-(X, X, X);    // 错误：三元的 -
X operator%(X);          // 错误：一元的 %
```

> [!attention] 
> 
> 运算符 `&&, ||` 和 逗号(`,`) 的默认含义中包含顺序信息。但是这一特殊规则不适用于自定义的 `&&, ||` 和 (`,`)
> 

### 运算符的预置含义

某些内置运算符的含义与其他接受相同参数的运算符组合的含义相同。例如，如果 `a` 是一个 `int`，则 `++a` 与 `a += 1` 以及 `a = a + 1` 等价。但是，除非有专门的说明，否则这一现象对于用户自定义的运算符不适用。

> [!tip] 
> 
> 编译器不会根据 `Z::operator+()` 和 `Z::operator=()` 的定义生成 `Z::operator+=()` 的定义
> 

当用作于类的对象时，运算符 `=`(赋值)、`&`(取地址)和 `,` 自带预置含义。可以使用 `=delete` 删除这些预置含义

```cpp
class X
{
public:
	// ...
	void operator=(const X &) = delete;
	void operator&() = delete;
	void operator,() = delete;
	// ...
};
```

### 运算符函数的参数与返回类型

重载运算符时，通常希望提供一种比较便捷的运算符，比如 `a = b + c`。因此，关于向运算符函数传递参数以及返回结果的问题，可供选择的方式比较有限

> [!tip] 
> + 不能请求一个指针类型的参数并且指希望程序员使用取地址符号
> + 也不能让运算符返回指针并希望解引用它
> 
> 换句话说，`*a = &b + &c` 是不能被接受的
> 

关于运算符函数的参数，主要有两种选择: **值传递** 和 **引用传递**

> [!tip] 值传递
> 
> 对于小对象(大小在 $1 \sim 4$ 字长)，采用值传递方式通常是最好的选择，得到性能也是最好的
> 

> [!tip] 引用传递
> 
> 对于较大对象，一般采用引用传递的方式
> 

通常情况下，一个运算符返回一个结果。向一个新创建的对象返回指针或引用基本上是一种比较糟糕的选择。**最好的方式是用传值的方式返回对象**：对于大对象来说，我们 **应该定义移动操作**

## 复数类型

在 [[C++：抽象机制概览]] 中我们定义了一个 `complex` 类，下面我们逐步完善这个类。这里我们依旧使用 `double` 作为基础类型。下面给出了 `complex` 类的详细设计

```cpp title:complex.hpp
#ifndef COMPLEX_HPP
#include <iostream>

namespace numbers
{
    class complex
    {
    public:
        /* 这样可能显得比较冗长，使用默认参数减少构造函数的数量
        complex():_real{0}, _imaginary{0}{}                                          // 默认构造函数
        complex(double real, double imaginary):_real{real}, _imaginary{imaginary}{}  // 普通构造函数
        complex(double real):_real{real}, _imaginary{0}{}                            // 重载构造函数
        */
        /**
         * @brief 构造函数：初始化实部和虚部
         * 
         * @param real 实部
         * @param imaginary 虚部
         */
        constexpr complex(double real = 0, double imaginary = 0) : _real{real}, _imaginary{imaginary} {}
        // 由于 complex 没有显显式持有资源，因此不需要特殊的成员函数
        // 编译器会自动生成默认：拷贝构造函数 拷贝赋值运算符 析构函数 移动构造函数 移动赋值运算符

    public:
        /**
         * @brief 复数的加法赋值运算符重载
         * 
         * @param other 另一个复数
         * @return complex& 返回当前对象的引用
         */
        complex &operator+=(const complex &other);    
        /**
         * @brief 复数的减法赋值运算符重载
         * 
         * @param other 另一个复数
         * @return complex& 返回当前对象的引用
         */
        complex &operator-=(const complex &other);    
        /**
         * @brief 复数的乘法赋值运算符重载
         * 
         * @param other 另一个复数
         * @return complex& 返回当前对象的引用
         */
        complex &operator*=(const complex &other);    
        /**
         * @brief 复数的除法赋值运算符重载
         * 
         * @param other 另一个复数
         * @return complex& 返回当前对象的引用
         */
        complex &operator/=(const complex &other);    
        
        /**
         * @brief 对复数取负运算符重载
         * -(a + bi) = -a - bi
         * @return complex 返回一个新的 complex 对象
         */
        complex operator-() const;
        /**
         * @brief 判断两个复数是否相等
         * 
         * @param other 另一个复数
         * @return true 实部和虚部都相等
         * @return false 实部和虚部有一个不相等
         */
        bool operator==(const complex &other) const;   
        /**
         * @brief 判断两个复数是否不相等
         * 
         * @param other 另一个复数
         * @return true 实部和虚部有一个不相等
         * @return false 实部和虚部都相等
         */
        bool operator!=(const complex &other) const;   
        
        // 自定义类型转换
        /**
         * @brief 从 complex 到 double 的转换
         * 
         * @return double 返回实部
         */
        explicit operator double() const; // 从 complex 到 double 的转换
        /**
         * @brief 在 bool 环境中使用 complex 对象
         * 
         * @return true {1, 0} || {0, 1} || {1, 1}
         * @return false {0, 0}
         */
        explicit operator bool() const;   // 从 complex 到 bool 的转换

        /**
         * @brief 重载 ! 运算符
         * 
         * @return true {0, 0}
         * @return false {1, 0} || {0, 1} || {1, 1}
         */
        bool operator!() const;


    public:
        /**
         * @brief 获取实部
         * 
         * @return double 返回实部
         */
        double real() const;
        /**
         * @brief 获取虚部
         * 
         * @return double 返回虚部
         */
        double imaginary() const;
        /**
         * @brief 获取共轭复数
         * 
         * @return complex 返回一个新的 complex 对象
         */
        complex conjugate() const;
    private:
        double _real;
        double _imaginary;
    };

    /* complex 的四则运算符 */
    /**
     * @brief 复数的加法运算符重载
     * (a + bi) + (c + di) = (a + c) + (b + d)i
     * @param  lhs 左操作数
     * @param  rhs 右操作数
     * @return complex 返回一个新的 complex 对象
     */
    complex operator+(complex lhs, complex rhs);

    /* 允许隐式类型转换，不需要重载 */
    // complex operator+(const complex& lhs, const complex &rhs);
    
    /**
     * @brief 复数的减法运算符重载
     * (a + bi) - (c + di) = (a - c) + (b - d)i
     * @param other 另一个复数
     * @return complex 返回一个新的 complex 对象
     */
    complex operator-(complex lhs, complex rhs);

    /**
     * @brief 复数的乘法运算符重载
     *  (a + bi) * (c + di) = (ac - bd) + (ad + bc)i
     * @param other 另一个复数
     * @return complex 返回一个新的 complex 对象
     */
    complex operator*(complex lhs, complex rhs);

    /**
     * @brief 复数的除法运算符重载
     * (a + bi) / (c + di) = (ac + bd) / (c^2 + d^2) + (bc - ad) / (c^2 + d^2)i
     * @param other 另一个复数
     * @return complex 返回一个新的 complex 对象
     */
    complex operator/(complex lhs, complex rhs);

    /* 为 complex 提供字面值 */
    complex operator""_i(long double imaginary); // 自定义字面量运算符，用于创建虚数

    /**
     * @brief 重载输出运算符
     * 
     * @param os ostream 对象
     * @param c complex 对象
     * @return std::ostream& 返回 ostream 对象
     */
    std::ostream &operator<<(std::ostream &os, const complex &c);

    /**
     * @brief 重载输入运算符
     * 
     * @param is istream 对象
     * @param c complex 对象的引用
     * @return std::istream& 
     */
    std::istream &operator>>(std::istream &is, complex &c);

}

#endif // COMPLEX_HPP
```

```cpp title:complex.cpp
#include <string>
#include <sstream>
#include "complex.hpp"


/* 成员函数 */

numbers::complex &numbers::complex::operator+=(const numbers::complex &other)
{
    _real += other._real;
    _imaginary += other._imaginary;
    return *this;
}

numbers::complex &numbers::complex::operator-=(const numbers::complex &other)
{
    _real -= other._real;
    _imaginary -= other._imaginary;
    return *this;
}

numbers::complex &numbers::complex::operator*=(const numbers::complex &other)
{
    double real = _real * other._real - _imaginary * other._imaginary;
    double imaginary = _real * other._imaginary + _imaginary * other._real;
    _real = real;
    _imaginary = imaginary;
    return *this;
}

numbers::complex &numbers::complex::operator/=(const numbers::complex &other)
{
    double denominator = other._real * other._real + other._imaginary * other._imaginary;
    double real = (_real * other._real + _imaginary * other._imaginary) / denominator;
    double imaginary = (_imaginary * other._real - _real * other._imaginary) / denominator;
    _real = real;
    _imaginary = imaginary;
    return *this;
}

numbers::complex numbers::complex::operator-() const
{
    return numbers::complex{-_real, -_imaginary};
}

bool numbers::complex::operator==(const numbers::complex &other) const
{
    return _real == other._real && _imaginary == other._imaginary;
}

bool numbers::complex::operator!=(const numbers::complex &other) const
{
    return !(*this == other);
}

numbers::complex::operator double() const
{
    return _real;
}

numbers::complex::operator bool() const
{
    return _real != 0 || _imaginary != 0;
}

bool numbers::complex::operator!() const
{
    return _real == 0 && _imaginary == 0;
}

double numbers::complex::real() const
{
    return _real;
}

double numbers::complex::imaginary() const
{
    return _imaginary;
}

numbers::complex numbers::complex::conjugate() const
{
    return numbers::complex{_real, -_imaginary};
}

/* 辅助函数 */

numbers::complex numbers::operator+(numbers::complex lhs, numbers::complex rhs)
{
    return lhs += rhs;
}

numbers::complex numbers::operator-(numbers::complex lhs, numbers::complex rhs)
{
    return lhs -= rhs;
}

numbers::complex numbers::operator*(numbers::complex lhs, numbers::complex rhs)
{
    return lhs *= rhs;
}

numbers::complex numbers::operator/(numbers::complex lhs, numbers::complex rhs)
{
    return lhs /= rhs;
}


// 字面值

numbers::complex numbers::operator""_i(long double imaginary)
{
    return numbers::complex{0, static_cast<double>(imaginary)};
}

// 重载 << 运算符，用于输出 complex 对象
std::ostream &numbers::operator<<(std::ostream &os, const numbers::complex &c)
{
    os << c.real();
    if(c.imaginary() >= 0) {
        os << " + " << c.imaginary() << "_i";
    } else {
        os << " - " << -c.imaginary() << "_i";
    }
    return os;
}

// 重载 >> 运算符，用于输入 complex 对象
std::istream &numbers::operator>>(std::istream &is, numbers::complex &c)
{
    // 要求输入格式为: a+bi
    double real, imaginary;
    char plus, i;
    is >> real >> plus >> imaginary >> i;
    if(plus != '+' || i != 'i') {
        is.setstate(std::ios_base::failbit);
        return is;
    }
    c = numbers::complex{real, imaginary};
    return is;
}
```
