# 多态

多态是面向对象语言的基本特征之一。所谓的多态，就是同一组接口被不同的类实现可以表现出不同的行为。简而言之，**一个接口，多种实现**

> [!question] 什么是接口？
> 
> 接口就是一组规则。即要求 **类必须实现的成员函数**
> 

> [!tip] 使用多态的好处
> 
> 面向对象的基本特征包括 **封装** **继承** 和 **多态**。其中，**封装** 就是将数据和函数组合起来并限制它们的访问权限，这样可以限制客户端的使用方式，使得代码模块化程度更高。**继承** 可以在不修改原始代码的情形下类的扩展功能。
> 
> **多态的好处就是将代码解耦**，提供程序的扩展性。多态依赖于接口，接口只是规定了对象需要什么行为(成员函数)，只要对象有这些行为(成员函数)，这个对象就可以被使用，无论是哪个类的对象
> 

C++ 中支持两种类型的多态：**运行时多态** 和 **静态多态**。上面我介绍的概念就是运行时多态。C++ 通过虚函数支持运行时多态

## 虚函数

**虚函数** 机制允许程序员在基类中声明函数，然后再每个 **派生类中重新定义这些函数**，从而解决类型域方法的固有问题。编译器和链接器会保证对象和施用于对象之上的函数之间的正确关联

```cpp
class Base {
public:
	Base(long base): _base{base} {}
	void display() const {
		cout << "Base::display()" << endl;
	}
private:
	long _base;
};

class Derived: public Base
{
public:
	Derived(long base, long derived): Base{base}, _derived{derived}
	{}
	
	void display() const {
		cout << "Derived::display()" << endl;
	}
private:
	long _derived;
};

void print(Base * pbase){
    pbase->display();
}

int main()
{
	Base base{10};
	Derived derived{10, 20};
	
	print(&base);
	print(&derived

	cout << "sizeof(Base): " << sizeof(Base) << endl;
	cout << "sizeof(Derived): " << sizeof(Derived) << endl;
}
```

编译上述代码并运行的结果为

```shell hl:4
➜  cppfiles g++ main.cpp
➜  cppfiles ./a.out 
Base::display()
Base::display()
sizeof(Base): 8
sizeof(Derived): 16
```

输出的第二个 `Base::display()` 是因为将派生类的指针赋值给了基类的指针变量，因此它只能操作派生类的基类子对象。

下面我们将基类的 `display()` 成员函数声明为虚函数

```cpp
#include <iostream>

using namespace std;

class Base {
public:
	Base(long base): _base{base} {}
	virtual void display() const {
		cout << "Base::display()" << endl;
	}
private:
	long _base;
};

class Derived: public Base
{
public:
	Derived(long base, long derived): Base{base}, _derived{derived}
	{}
	
	void display() const {
		cout << "Derived::display()" << endl;
	}
private:
	long _derived;
};

void print(Base * pbase){
    pbase->display();
}

int main()
{
	Base base{10};
	Derived derived{10, 20};
	
	print(&base);
	print(&derived);

	cout << "sizeof(Base): " << sizeof(Base) << endl;
	cout << "sizeof(Derived): " << sizeof(Derived) << endl;
}
```

编译运行上述程序的结果

```shell hl:4-6
➜  cppfiles g++ main.cpp
➜  cppfiles ./a.out 
Base::display()
Derived::display()
sizeof(Base): 16
sizeof(Derived): 24
```

> [!attention] 
> 
> 请注意：加入 `virtual` 关键字之后，使用基类的指针执行派生类对象时可以调用派生类的成员函数
> 
> 此外，基类和派生类对象占用的空间大小均发生了改变，说明其内存结构发生了变化
> 

### 虚函数表指针

对于包含虚函数的类，其类对象会在开始位置存储一个指针指向该类的虚函数表，这个指针称为虚函数指针。如下图所示

![Pasted image 20250222201021|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755785718712-204b9a4c0a774874baa2d9a9fafd37c0.png)

当 `Base` 类的 `display` 函数加上了 `virtual` 关键字，变成了一个虚函数，`Base` 对象的存储布局就改变了。在存储的开始位置会多加一个虚函数指针，**该虚函数指针指向一张虚函数表**（简称虚表），其中存放的是虚函数的入口地址

`Derived` 继承了 `Base` 类，那么创建一个 `Derived` 对象，依然会创建出一个 `Base` 类的基类子对象

![Pasted image 20250222201214|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755785718712-30cede2970ee45b5910c13088a0a62b7.png)

在 `Derived` 类中又定义了 `display` 函数，发生了 **覆盖** 的机制(`override`)，**覆盖的是虚函数表中虚函数的入口地址**

![Pasted image 20250222201312|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755785718712-32dd2dd697c74b968b1277d1e704139c.png)

使用 `Base* p` 去指向 `Derived` 对象，**依然只能访问到基类的部分**。用指针 `p` 去调用 `display` 函数，发现是一个虚函数，那么会 **通过 `vfptr` 找到虚函数表**，此时虚函数表中存放的是 `Derived::display` 的入口地址，所以调用到 `Derived` 的 `display` 函数。

### 虚函数的覆盖

如果一个基类的成员函数被生命为虚函数，那么它的所有派生类中也保持为虚函数，即使在派生类中省略了 `virtual` 关键字，也仍然是虚函数。虚函数一般用于灵活拓展，所以需要 **派生类中对此虚函数进行覆盖** 。覆盖需要满足一定的要求：

> [!tip] 发生虚函数覆盖的条件
> 
> 与基类的虚函数名相同
> 
> 与基类的虚函数的参数列表相同(参数类型 参数个数 参数顺序)
> 
> 与基类的虚函数具有相同的返回类型
> 

上述三个条件必须完全满足，否则不会形成虚函数的覆盖，而且编译器也不会有任何提示。为了保证虚函数覆盖的发生，可以使用关键字 `override` 

> [!tip] 
> 
> 在虚函数的函数参数列表之后，函数体的大括号之前，加上 `override` 关键字，告诉编译器此处定义的函数是要对基类的虚函数进行覆盖。
> 

```cpp hl:21
#include <iostream>

using namespace std;

class Base {
public:
	Base(long base): _base{base} {}
	virtual void display() const {
		cout << "Base::display()" << endl;
	}
private:
	long _base;
};

class Derived: public Base
{
public:
	Derived(long base, long derived): Base{base}, _derived{derived}
	{}
	
	void display() const override  // 关键字 override 表示需要覆盖基类的虚函数，如果没有覆盖成功，编译器会报错
	{
		cout << "Derived::display()" << endl;
	}
private:
	long _derived;
};

void print(Base * pbase){
    pbase->display();
}

int main()
{
	Base base{10};
	Derived derived{10, 20};
	
	print(&base);
	print(&derived);

	cout << "sizeof(Base): " << sizeof(Base) << endl;
	cout << "sizeof(Derived): " << sizeof(Derived) << endl;
}
```

> [!summary] 
> 
>覆盖是在虚函数之间的概念，需要派生类中定义的虚函数与基类中定义的虚函数的形式完全相同
>
>当基类中定义了虚函数时，派生类去进行覆盖，即使在派生类的同名的成员函数前不加`virtual`，依然是虚函数
>
>发生在基类派生类之间，基类与派生类中同时定义相同的虚函数 **覆盖的是虚函数表中的入口地址**，并不是覆盖函数本身
>

### 动态多态发生的条件

虚函数机制被触发的条件是非常严格的。需要同时满足下列 $4$ 个条件才能激活虚函数机制表现出多态性

> [!tip] 条件一：基类必须定义虚函数
>
>基类中必须要有成员函数被声明为 `virtual` 的
>

> [!tip] 条件二：派生类中要覆盖虚函数
> 
> 派生类中必须重新定义与基类中的 `virtual` 成员函数完全相同的成员函数
> 

> [!tip] 条件三：通过基类的指针或引用操作派生类的对象
> 

> [!tip] 条件四：通过基类的指针或引用调用虚函数

### 虚函数表

我们知道当一个类声明了 `virtual` 的成员函数时，类对象就会多出一个指针指向虚函数表。下面我们探究 `virtual` 关键在虚函数机制中的含义

> [!tip] `virtual` 的成员函数并不意味着成员函数就消失了，它依旧存在内存中的代码段
> 
> 换句话说，虚函数依旧存在
> 

> [!tip] 一个类存在 `virtual` 成员函数时，该类对象会多一个指针指向虚函数表
> 
> 这意味着，通过类对象调用虚函数时，是 **间接调用** 的
> 

> [!tip] 基类的指针指向派生类时，通过该指针调用的虚函数时，会调用派生类的虚函数
> 

如果没有虚函数，当通过基类的指针去调用一个普通的成员函数，那么就不会通过虚函数表查找虚函数指针，而是直接到程序代码区中找到该函数。

在了解了虚函数和虚函数表之后，下面我们通过几个问题深入学习

> [!question] 虚函数表存在哪里？
> 
> 程序编译完成时，虚函数表就已经存在；在使用过程中，**虚函数表不应该被修改**。虚函数表应该存储在只读的内存区域
> 
> 不同的实现可能存储的位置不同，但是均应该保证在执行过程中不能修改虚函数表中的内容
> 

> [!question] 一个类有几张虚函数表？
> 
> 虚函数表可以理解为一个数组，存放的都是虚函数的指针
> 
> 可以没有虚函数表：意味着，没有虚函数
> 
> 可以有一张虚函数表，该类至少声明了一个虚函数
> 
> ![Pasted image 20250222222214|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755785718713-872e409dd1064b848e15cbf4e0bc805a.png)
> 
>可以有多张虚函数表，该类继承了多个有虚函数的基类
>
> ![Pasted image 20250222222319|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755785718713-4dca6abb9d164ba3a7fa869dce97a587.png)
>
> 也就是说，一个类至少有 $0$ 张虚函数表
>

> [!question] 虚函数的底层实现是怎样的？
> 
> 虚函数使用虚函数表实现的。当类中声明了 `virtual` 的成员函数时，就会在该类对象的开始位置添加一个虚函数表指针，该指针指向一张虚函数表，虚函数表中存放的就是虚函数的入口地址
> 

到目前为止，我们学习了 $3$ 个易于混淆的概念：**重载(overload)** **隐藏(oversea)** 和 **覆盖(override)**。下面我们详细区分这三种概念

> [!tip] 重载：发生在同一个作用域中，只有当函数名相同，参数列表不同时才会发生形成重载关系
> 
> 编译器会根据参数的调用函数的实参类型选择合适的函数进行调用
> 

> [!tip] 隐藏：发生在基类和派生类之间的同名成员
> 
> 当派生类定义了与基类同名的成员时，在派生类中就无法直接访问基类的成员了
> 

> [!tip] 覆盖：发生在基类和派生类之间，派生类定义了与基类相同的虚函数时，派生类的虚函数表中的虚函数入口地址就会被覆盖 
> 
> 注意：覆盖的是虚函数表中的虚函数入口地址，而不是虚函数本身
> 

### 虚函数的限制

C++ 的虚函数机制提供了灵活的用法，但是也受到了一些约束

> [!tip] 约束一：构造函数不能是虚构函数
> 
> 构造函数的作用是完成对象数据成员的初始化，而虚函数机制被激活的条件之一就是要先创建对象
> 
> 只有有了对象才能表现出运行时多态。如果构造函数设为虚函数，那么此时构造未执行完成，对象还未被初始化，存在矛盾
> 

> [!tip] 约束二：静态成员函数不能是虚函数
> 
> 虚函数需要通过 `this` 指针找到 `vtptr` 才能被调用，而 `static` 成员函数没有 `this` 指针
> 

> [!tip] 约束三：内联函数不能是虚函数
> 
> 内敛函数在编译器间完成替换，而在编译期间无法展现运行时多态机制。这样存在冲突，内联会失效
> 

> [!tip] 约束三：普通函数不能设为虚函数
> 
> 虚函数的目的是实现对象的多态，与普通函数无关
> 

### 虚函数的各种访问情形

虚函数机制的触发条件中规定了要 **使用基类指针（或引用）来调用虚函数**，那么其他的调用方式会是什么情况呢？

> [!tip] 方式一：通过派生类对象调用虚函数
> 
> 通过派生类对象直接调用虚函数并不会触发动态多态。此时，只要 `Derived` 中定义了 `display()` 成员函数就会对 `Base` 中的 `display()` 函数隐藏
> 

> [!tip] 方式二：在构造函数和析构函数中调用虚函数
> 

```cpp
class Grandpa
{
public:
    Grandpa(){ cout << "Grandpa()" << endl; }
    ~Grandpa(){ cout << "~Grandpa()" << endl; }

    virtual void func1() {
        cout << "Grandpa::func1()" << endl;
    }

    virtual void func2(){
        cout << "Grandpa::func2()" << endl;
    }
};

class Parent
: public Grandpa
{
public:
    Parent(){
        cout << "Parent()" << endl;
        func1();  //构造函数中调用虚函数
        // 在执行 Parent 类的构造函数体时，Grandpa 类对象已经构造完成
        // 由于 Parent 没有覆盖 Grandpa 的虚函数，此时调用的一定是 Grandpa 的虚函数
    }

    ~Parent(){
        cout << "Parent()" << endl;
        func2();  //析构函数中调用虚函数
    }
};

class Son
: public Parent
{
public:
    Son() { cout << "Son()" << endl; }
    ~Son() { cout << "~Son()" << endl; }

    virtual void func1() override {
        cout << "Son::func1()" << endl;
    }

    virtual void func2() override{
        cout << "Son::func2()" << endl;
    }
};

void test0(){
    Son ss;
    Grandpa * p = &ss;
    p->func1();
    p->func2();
}
```

> [!summary] 
> 
> **如果在Parent类的构造和析构函数中调用虚函数**
> + 在 `Parent` 的构造函数执行时，并不知道是在构造 `Son` 的对象，在此过程中，**只能看到本层及以上的部分**（因为 `Grandpa` 类的基类子对象已经创建完毕，虚表中记录了 `Grandpa::func1` 和 `Grandpa::func2` 的地址）
> 
> + 在 `Parent` 的析构函数执行时，此时 `Son` 的析构函数已经执行完了，**也只能看到本层及以上的部分**。
> 
> 如果 `Parent` 类中也覆盖了 `func1` 和 `func2`，那么会调用 `Parent` 本层的虚函数。
> 

> [!tip] 方式三：在普通成员函数中调用虚函数
> 

```cpp hl:49
class Base{
public:
    Base(long x)
    : _base(x)
    {}

    virtual void display() const{
        cout << "Base::display()" << endl;
    }

    void func1(){
        display();  // 这里需要具体分析
        cout << _base << endl;
    }

    void func2(){
        Base::display();  // 毫无疑问会调用 Base 的 display 成员函数
    }
private:
    long _base = 10;
};


class Derived
: public Base
{
public:
    Derived(long base,long derived)
    : Base(base)
    , _derived(derived)
    {}

	// Derived 覆盖了 Base 的的 display() 虚函数
    void display() const override{
        cout << "Derived::display()" << endl;
    }
    
private:
    long _derived;
};

void test0(){
    Base base(10);
    Derived derived(1,2);

    base.func1();  // 基类对象，func1() 中调用 display() 一定是 Base 的display()
    base.func2(); 

    derived.func1(); // 需要分析.
    // 上述调用的是基类的 func1()，此时 func1() 中的 this 指针(Base *) 指向了派生类对象。
    // 在 func1() 调用虚函数时就会触发动态多态机制，调用的虚函数就是派生类中的虚函数
    derived.func2(); 
}
```

通过 `Derived` 类对象调用 `func1()` 时，因为 `Derived` 类没有重新定义自己的 `func1()`，所以会调用基类的 `func1()` 函数。此时，传递给 `func1()` 的 `this` 指针指向的就是派生类对象 `derived`(即，派生类对象的指针被转换为了基类对象的指针)。此时 `this` 指向的是派生类对象，这种情况下符合动态多态出发的机制。在 `func1()` 中调用了虚函数 `display()` 一定是派生类中的 `display()`

## 抽象类

C++ 中提供了两种形式定义 **抽象类**：**定义了纯虚函数** 和 **只定义了 `protected` 构造函数**

> [!tip] 抽象类
> 
> 抽象类是描述其派生类的行为，不需要完成类的特定实现。在其他语言中，抽象类也称为接口
> 
> **抽象类不能创建对象**，只能使用抽象类的指针指向派生类的对象或者给抽象类的引用绑定派生类的对象

### 纯虚函数

**纯虚函数**是一种特殊的虚函数，在许多情况下，在基类中不能对虚函数给出有意义的实现，而把它声明为纯虚函数，**它的实现留给该基类的派生类去做**。这就是纯虚函数的作用。纯虚函数的格式如下：

```cpp
class 类名 {
public:
	virtual 返回类型 函数名(参数 ...) = 0;
};
```

在基类中声明纯虚函数就是在告诉子类的设计者必须提供一个纯虚函数的实现，但基类不知道派生类会怎样实现它。

> [!important] 
> 
> 多个派生类可以对纯虚函数进行多种不同的实现，但是都需要遵循基类给出的接口（纯虚函数的声明）
> 

下面是纯虚函数的使用示例

```cpp
class A
{
public:
    virtual void print() = 0;
    virtual void display() = 0;
};

class B
: public A
{
public:
    virtual void print() override{
        cout << "B::print()" << endl;
    }
};

class C
: public B
{
public:
    virtual void display() override{
        cout << "C::display()" << endl;
    }
};

int main(){
    //A类定义了纯虚函数，A类是抽象类
    //抽象类无法创建对象
    //A a;//error
  
    //B b;//error
    C c;
    A * pa2 = &c;
    pa2->print();
    pa2->display();
    
    return 0;
}
```

在 `A` 类中声明纯虚函数，`A` 类就是抽象类，无法创建对象；

在 `B` 类中去覆盖 `A` 类的纯虚函数，如果把所有的纯虚函数都覆盖了（都实现了），`B` 类可以创建对象；只要还有一个纯虚函数没有实现，`B` 类也会是抽象类，也无法创建对象；

再往下派生 `C` 类，完成所有的纯虚函数的实现，`C` 类才能够创建对象。

最顶层的基类（定义纯虚函数的类）虽然无法创建对象，但是可以定义此类型的指针，指向派生类对象，去调用实现好的纯虚函数。

纯虚函数的使用案例：实现一个图形库，获取图形名称，获取图形之后获取它的面积

```cpp
#define PI 3.141592653
class Figure{
public:
    virtual string getName() const = 0;
    virtual double getArea() const = 0;
};

class Rectangle//矩形
: public Figure
{
public:
    Rectangle(double len,double wid)
    : _length(len)
    , _width(wid)
    {}

    string getName() const override
    {
        return "矩形";
    }
    double getArea() const override
    {
        return _length * _width;
    }
private:
    double _length;
    double _width;
};

class Circle
: public Figure
{
public:
    Circle(double r)
    : _radius(r)
    {}

    string getName() const override
    {
        return "圆形";
    }
    double getArea() const override
    {
        return PI * _radius * _radius;
    }
private:
    double _radius;
};

class Triangle
: public Figure
{
public:
    Triangle(double a,double b,double c)
    : _a(a)
    , _b(b)
    , _c(c)
    {}
  
    string getName() const override
    {
        return "三角形";
    }
    double getArea() const override
    {
        double p = (_a + _b + _c)/2;
        return sqrt(p * (p -_a) * (p - _b)* (p - _c));
    }
private:
    double _a,_b,_c;
};

```

## 虚析构函数

虽然构造函数不能被定义成虚函数，但 **析构函数可以定义为虚函数**，一般来说，**如果类中定义了虚函数，析构函数也应被定义为虚析构函数**，尤其是类对象持有资源时，需要清理和释放的资源的时候。

下面我们以持有堆内存空间为例，分析虚析构函数的必要性

```cpp
#include <iostream>

using namespace std;

class Base
{
public:
	Base(int data): _base{new int{data}} {
		cout << "Base(int)" << endl;
	}

	~Base() {
		cout << "~Base()" << endl;
		delete _base;
	}

private:
	int * _base;
};

class Derived : public Base
{
public:
	Derived(int base, int derived): Base{base}, _derived{new int{derived}} {
		cout << "Derived(int, int)" << endl;
	}

	~Derived() {
		cout << "~Derived()" << endl;
		delete _derived;
	}
private:
	int * _derived;
};

int main() {
	Base * pbase = new Derived{10, 20};
	delete pbase;
}
```

编译上述代码并运行的结果为

```shell
➜  cppfiles g++ main.cpp
➜  cppfiles ./a.out 
Base(int)
Derived(int, int)
~Base()
```

> [!tip] 
> 
> 当需要使用基类的指针去释放派生类对象时，如果析构函数不是虚析构函数的情况下，只能调用基类的析构函数。此时，如果派生类持有资源就会造成资源泄漏的问题
> 
> ![Pasted image 20250223202425|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755785718713-34ac9c742e2d4187a05b17dde260d3a9.png)
> 

下面我们将基类的析构函数声明为虚析构函数

```cpp
#include <iostream>

using namespace std;

class Base
{
public:
	Base(int data): _base{new int{data}} {
		cout << "Base(int)" << endl;
	}

	virtual ~Base() {
		cout << "~Base()" << endl;
		delete _base;
	}

private:
	int * _base;
};

class Derived : public Base
{
public:
	Derived(int base, int derived): Base{base}, _derived{new int{derived}} {
		cout << "Derived(int, int)" << endl;
	}
	
	// 派生类覆盖了基类的析构函数
	~Derived() override {
		cout << "~Derived()" << endl;
		delete _derived;
	}
private:
	int * _derived;
};

int main() {
	Base * pbase = new Derived{10, 20};
	delete pbase;
}
```

此时编译运行的结果为

```shell
➜  cppfiles g++ main.cpp
➜  cppfiles ./a.out 
Base(int)
Derived(int, int)
~Derived()
~Base()
```

> [!tip] 
> 
> 由于析构函数是虚函数，**基类指针指向了派生类的对象**。通过基类指针销毁对象时，**调用了虚析构函数**。因此，**触发动态多态的机制**，从而间接调用到派生类的析构函数。
> 
> ![Pasted image 20250223202528|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755785718713-74e27d60922b4d8692cdac8f95d5bbd8.png)
> 
> 在派生类的析构函数体执行完成之后，会自动调用成员和基类的析构函数
> 

> [!summary] 
> 
> **在实际的使用中，如果有通过基类指针回收派生类对象的需求，都要将基类指针的析构函数设为虚函数。**
> 

## 访问虚函数表

从前面的知识讲解，我们已经知道虚函数表的存在，但之前都是理论的说法，我们是否可以通过程序来验证呢？——当然可以

下面的代码展示了如何获得虚函数表

```cpp
#include <iostream>
using namespace std;


class Base
{
public:
	virtual void print()
	{
		cout << "Base::print()" << endl;
	}
	virtual void display()
	{
		cout << "Base::display()" << endl;
	}
	virtual void show()
	{
		cout << "Base::show()" << endl;
	}

private:
	long _base = 10;
};

class Derived
	: public Base
{
public:
	virtual void print()
	{
		cout << "Derived::print()" << endl;
	}
	virtual void display()
	{
		cout << "Derived::display()" << endl;
	}
	virtual void show()
	{
		cout << "Derived::show()" << endl;
	}

private:
	long _derived = 100;
};

int main()
{
	Derived d;
	// d 的首地址位置存储的就是该对象的虚函数表的地址。虚函数表中存储的是虚函数的地址
	// 在 64 位系统中指针的大小是 8 字节。我们可以将它视为 long 类型的值
	
	long * pd = reinterpret_cast<long *>(&d);

	cout << pd[0] << endl;  // 虚函数表的指针：指向虚函数表的第一个元素
	cout << pd[1] << endl;  // _base
	cout << pd[2] << endl;  // _derived

	// pd[0] 指向的就是虚函数表中的第一个元素
	// 由于虚函数表存储的也是指针，在 64 位系统中占 8 字节
	// 我们可以将虚函数表中的元素当中 long 类型的值
	long *pVtable = reinterpret_cast<long *>(pd[0]);

	void(*f)() = reinterpret_cast<void(*)()>(pVtable[0]);
	f();
	f = reinterpret_cast<void(*)()>(pVtable[1]);
	f();
	f = reinterpret_cast<void(*)()>(pVtable[2]);
	f();
}
```

创建一个 `Derived` 类对象 `d`，这个对象的内存结构是由三个内容构成的，开始位置是虚函数指针，第二个位置是 `long` 型数据 `_base`, 第三个位置是 `long` 型数据 `_derived`.

第一次强转将这个 `Derived` 类对象视为了存放三个 `long` 型元素的数组，打印这个数组中的三个元素，后两个本身就是 `long` 型数据，输出其值，第一个本身是指针（地址），打印出来的结果是编译器以 `long` 型数据来看待这个地址的值。

这个虚函数指针指向虚函数表，虚函数表中存放三个虚函数的入口地址（`3 * 8` 字节），那么再将虚函数表视为存放三个 `long` 型元素的数组，第二次强转，得到的结果是编译器以 `long` 型数据来看待这三个函数地址的值。

虚函数表中的三个元素本身是函数指针，那么再将这个三个元素强转成相应类型的函数指针，就可以通过函数指针进行调用了。

通过上述代码，我们验证了 **虚函数表中存放虚函数的顺序，是按照虚函数的声明顺序去存放的**。

![Pasted image 20250223215012|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755785718714-464f274ca7434ffbbaf5d4299ad0a27b.png)

## 带虚函数的多继承

描述：先是 `Base1` `Base2` `Base3` 都拥有虚函数 `f` `g` `h`；`Derived` 公有继承以上三个类，在 `Derived` 中覆盖了虚函数 `f`，还有一个普通的成员函数 `g1`，四个类各有一个 `double` 成员。

```cpp
class Base1
{
public:
	Base1() 
	: _iBase1(10) 
	{ cout << "Base1()" << endl; }
	
	virtual void f()
	{
		cout << "Base1::f()" << endl;
	}

	virtual void g()
	{
		cout << "Base1::g()" << endl;
	}

	virtual void h()
	{
		cout << "Base1::h()" << endl;
	}

	virtual ~Base1() {}
private:
	double _iBase1;
};

class Base2
{
	//...
private:
	double _iBase2;
};

class Base3
{
public:
	//...
private:
	double _iBase3;
};

class Derived 
	: public Base1
	, public Base2
	, public Base3
{
public:
	Derived()
	: _iDerived(10000) 
	{ cout << "Derived()" << endl; }

	void f()
	{
		cout << "Derived::f()" << endl;
	}

	void g1()
	{
		cout << "Derived::g1()" << endl;
	}
private:
	double _iDerived;
};

int main(void)
{
	cout << sizeof(Derived) << endl;

	Derived d;
	Base1* pBase1 = &d;
	Base2* pBase2 = &d;
	Base3* pBase3 = &d;

	cout << "&Derived = " << &d << endl; 
	cout << "pBase1 = " << pBase1 << endl; 
	cout << "pBase2 = " << pBase2 << endl;
	cout << "pBase3 = " << pBase3 << endl;

	return 0;
}
```

编译运行上述代码的结果为

```cpp
➜  cppfiles g++ main.cpp
➜  cppfiles ./a.out 
56
Base1()
Base2()
Base3()
Derived()
&Derived = 0x7fff886b7ad0
pBase1 = 0x7fff886b7ad0
pBase2 = 0x7fff886b7ae0
pBase3 = 0x7fff886b7af0
```

三种不同的基类类型指针指向派生类对象时，**实际指向的位置是基类子对象的位置**

![Pasted image 20250223220907|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755785718714-bd12df9c7cd14b42adaf691c90a3b1fd.png)

下图展示了 MSVC 编译器中的内存布局与虚函数表存放的内容

![Pasted image 20250223221100|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755785718714-9527c7eefd774af38bdbe9bbe97a9243.png)
### 对象内存布局

> [!tip] 每个基类子对象都有自己的虚函数表
> 
> 类中只要定义了虚函数，虚函数表就一定会存在，不依赖于对象
> 

> [!tip] 派生类如果有自己的虚函数，会加入到第一个虚函数表之中
> 
> 派生类定义了自己的虚函数时，并不会新创建一张虚函数表，而是放入第一张虚函数表中
> 

> [!tip] 内存布局中，其基类的布局按照基类被声明时的顺序进行排列
> 
> 有虚函数的基类会往上放，因为希望尽快访问到虚函数
> 

> [!tip] 派生类会覆盖基类的虚函数
> 
> **只有第一个虚函数表中存放的是真实的被覆盖的函数的地址**
> 
> **其它的虚函数表中对应位置存放** 的并不是真实的对应的虚函数的地址，而**是一条跳转指令**
> 

### 带虚函数的多重继承的二义性

```cpp
class A{
public:
    virtual void a(){ cout << "A::a()" << endl; } 
    virtual void b(){ cout << "A::b()" << endl; } 
    virtual void c(){ cout << "A::c()" << endl; } 
};

class B{
public:
    virtual void a(){ cout << "B::a()" << endl; } 
    virtual void b(){ cout << "B::b()" << endl; } 
    void c(){ cout << "B::c()" << endl; } 
    void d(){ cout << "B::d()" << endl; } 
};

class C
: public A
, public B
{
public:
    virtual void a(){ cout << "C::a()" << endl; } 
    void c(){ cout << "C::c()" << endl; } 
    void d(){ cout << "C::d()" << endl; } 
};
```

上述代码各个类的对象的内存布局如下图所示

![Pasted image 20250223225606|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755785718714-9d48d810b7a6415196322f053304ed1f.png)

**请分析以下各种调用情况的结果**

```cpp
int main()
{
    C c;
    c.a();     // C 类隐藏了基类中 a，因此输出结果为 "C::a()"
    // c.b();  // error：冲突，基类 A 和 B 都有 b 函数
    c.c();     // C 类隐藏了基类的成员函数 c()，因此输出结果为 "C::c()"
    c.d();     // C 类隐藏了基类 B 的成员函数 d()，输出结果为 "C::d()"
    cout << endl;
    
	A* pa = &c;  // 基类 A 的指针指向了派生类 C 的对象
    pa->a();     // 动态多态，输出结果为 "C::a()"
    pa->b();     // C 没有覆盖虚函数 b()，输出结果为 "A::b()"
    pa->c();     // C 覆盖了基类虚函数 c()，因此输出结果为 "C::c()"
    // pa->d();  // error: 基类中没有成员函数 d()，通过基类的指针无法访问
	cout << endl;
	
	B* pb = &c;
    pb->a();      // C 类覆盖了基类 B 中的虚函数 a()，输出结果为 "C::a()"
    pb->b();      // C 类没有覆盖基类 B 中的虚函数 b()，输出结果为 "B::b()"
    pb->c();      // 在 B 类中，c() 不是虚函数。输出结果为 "B::c()"
    pb->d();      // 在 B 类中，d() 也不是虚函数，输出结果为 "B::d()"
	cout << endl;
	
	C * pc = &c; // 派生类 C 的指针指向了派生类 C 的对象
    pc->a();     // C 类隐藏了基类中 a()，因此输出结果为 "C::a()"
    // pc->b();  // Cerror：冲突，基类 A 和 B 都有 b 函数
    pc->c();     // C 类隐藏了基类中的成员函数 c() ，输出结果为 "C::c()"
    pc->d();     // C 类隐藏了基类 B 中的成员函数 d()，输出结果为 "C::d()"
	return 0;
}
```

下面验证 `C` 类中的成员函数 `c()` 是否是虚函数？

```cpp
class D: public C
{
public:
	void c() { cout << "D::c()" << endl; }
};

int main(void)
{

	C *pc = new D;
	pc->c();   // 输出 D::c()
	return 0;
}
```

编译运行的结果是

```shell
➜  cppfiles g++ main.cpp
➜  cppfiles ./a.out 
D::c()
```

> [!attention] 
> 
> 上述运行的结果明显看出 `C` 类中的 `c()` 成员函数是虚函数
> 
