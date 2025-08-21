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

![Pasted image 20250123184217|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755785388881-e35be6760d3545be847fd0f170b79e50.png)


与面向过程模式不同，面向对象模式处理 **活动对象**，而不被动对象。活动对象能执行的动作都包含在该对象中：对象只需要接收合适的外部刺激来执行其中一个动作

> [!tip] 活动对象
> 
> 能够执行的动作都包含在对象中，对象只要接收到合适的刺激就能执行其中的动作
> 

继续考虑文件打印程序。在面向对象模式中，文件能把所有的被文件执行的过程打包在一起。在这种模式中，程序仅仅像对象发送一个相应的请求即可

![Pasted image 20250123184417|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755785388881-d005e58484eb4f33bf9c2c97a459b2fe.png)

## 类定义

**C++ 中类是创建新类型的工具**，创建出的类型可以像内置类型一样方便的使用。**派生类** 和 **模板** 允许我们表达类之间的关系并利用这些关系

> [!tip] 类型表达了一种抽象的概念
> 
> C++ 的内置类型 `float` 和 运算符 `+, -, *, /` 一起提供了数学概念 **实数** 的近似表示
> 

类是用户自定义类型。如果一个概念没有直接对应的内置类型，我们可以定义一个新类型表示这个概念

> [!tip] 
> 
> 在程序中使用与概念匹配的类型，可以使程序更容易理解，更容易分析，也更容易被修改
> 

下面是类定义的法形式。关键字 `class` 用于定义一个类

```cpp
class Name [: 继承方式 基类,...]
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

> [!tip] 
> 
> C++ 中类有两部分组成：**数据成员** 和 **成员函数**，在某些资料中统称为成员
> + 数据成员在某些语言中称为 **属性**
> + 成员函数在某些语言中称为 **方法**
> 

与 `struct` 一样，可以先声明后定义

```cpp
class Name;  // 声明 Name 是一个类

// 在合适的位置可以定义类
class Name {};
```

例如，如下代码定义了一个类 `Person`

```cpp title:person.cpp
#include <iostream>
using namespace std;

class Person {

	char *_name; // 指针数据成员
    int _age;    // 普通数据成员

	// 成员函数，也称为方法
    void show() {
		cout << "name: " << _name << ", age: " << _age << endl;
    }
};
```

### 访问控制

现在，我们有一个类 `Person`，我们就可以定义 `Person` 类的对象，并访问对象的成员

```cpp title:person.cpp
int main() {
    Person p;
    p.show();
    return 0;
}
```

使用 `g++` 命令编译 `person.cpp` 文件，`g++` 提示 `p.show()` 不能访问

```shell
➜  cppfiles g++ person.cpp -o person     
person.cpp: In function ‘int main()’:
person.cpp:17:11: 错误：‘void Person::show()’ is private within this context
   17 |     p.show();
      |     ~~~~~~^~
person.cpp:10:10: 附注：declared private here
   10 |     void show() {
      |          ^~~~
```

这是因为 C++ 中的类成员被限制访问的。C++ 的类提供了 $3$ 种访问控制：**私有的(private)** **公有的(public)** 和 **被保护的(protected)**

> [!tip] private：默认的访问控制，禁止成员在类外访问
> 
> `class` 关键字限制定义在其中的成员是**私有的(private)**。对于类私有成员，目前来说只能在类定义中访问，离开类定义将无法访问
> 

> [!tip] public：公开的，允许成员在类外被访问
> 
> 在类外，可以通过对象直接访问
> 

> [!tip] protected：被保护的，仅类内和派生类内可访问
> 
> `protected` 访问控制在派生类中会详细介绍，这里忽略
> 

为了让成员函数在类外可访问，我们需要使用 `public` 规定成员的访问属性

```cpp title:person.cpp
#include <iostream>

using namespace std;

class Person {

    char *_name;
    int _age;
public:
    void show() {
		cout << "name: " << _name << ", age: " << _age << endl;
    }
};

int main() {
    Person p;
    p.show();
    return 0;
}
```

编译允许的结果如下

```shell
➜  cppfiles g++ person.cpp -o person
➜  cppfiles ./person    
name: , age: -1301635008
```

> [!attention] 
> 
> 私有成员的保护依赖于对类成员名的使用限制。通过地址或显示类型转换可以绕过 `private` 的保护
> 
> C++ 只能防止意外而无法防止故意规避。只有硬件才能完美防止语言的恶意使用，而实际系统中很难实现
> 

### class 和 struct 对比

在 C++ 中，`struct` 本质也是 `class`，与 `class` 的唯一差异就是成员的默认访问控制不同。

> [!tip] `struct` 的默认访问控制为  `public`
> 
> 在 C++ 中，`struct` 是成员的默认访问控制为 `public` 的类
> 

```cpp title:person_struct.cpp
#include <iostream>

using namespace std;

struct Person {

    char *_name;
    int _age;
    void show() {
	    cout << "name: " << _name << ", age: " << _age << endl;
    }
};

int main() {
    Person p;
    p.show();
    return 0;
}
```

编译并运行的结果如下

```shell
➜  cppfiles g++ person_struct.cpp -o person_struct    
➜  cppfiles ./person_struct 
name: , age: -307584960
```

### 成员函数的定义

在 `Person` 类定义的示例中，成员函数 `show()` 是定义在类中。通常，我们 **应该将成员函数的定义从类定义中分离出来**。这样做的可以 **降低类定义的复杂性**

> [!tip] 降低类定义的复杂性
> 
> 降低类定义的复杂性。一个类中的成员函数通常会很多，如果成员函数的定义也写在类中，会导致代码复杂
> 

```cpp title:person.cpp
#include <iostream>

using namespace std;

class Person {

    char *_name;
    int _age;

public:
    void show();  // 显示对象的信息
};

// 在类定义外合适的为主定义成员函数
// 注意，定义成员函数时，需要通过显示限定的方式说明该函数属于哪个类
void Person::show(){
    cout << "name: " << _name << ", age: " << _age << endl;
}

int main() {
    Person p;
    p.show();
    return 0;
}
```

当我们将成员函数的定义移到类外时，**不应该在头文件中定义这些成员函数**。这是因为在多文件编程中，可能导致重复定义的问题

> [!attention] 
> 
> 当头文件在多个文件中被包含时。虽然单个文件可以编译通过，但是在链接时会因为同一个名字可以找到多个实现，从而导致重复定义的问题
> 

当将成员函数从类定义中移出到类外定义时，成员函数的定义一般会放置实现文件中，而不是放在头文件中。

```cpp title:person.hpp
#ifndef PERSON_HPP
#define PERSON_HPP

class Person {

    char *_name;
    int _age;

public:
    void show();
};
#endif // PERSON_HPP
```

```cpp title:person.cpp
#include <iostream>
using namespace std;

#include "person.hpp"

// 在类定义外合适的为主定义成员函数
// 注意，定义成员函数时，需要通过显示限定的方式说明该函数属于哪个类
void Person::show(){
    cout << "name: " << _name << ", age: " << _age << endl;
}
```

## 对象的创建

通过 `Person` 类创建的对象在调用 `show()` 时输出的 `name` 和 `age` 非常奇怪，这是因为没有对数据成员 `name` 和 `age`  进行初始化

由于 `name` 和 `age` 的访问控制为 `private`，因此需要提供一个初始化成员函数，通过调用该函数达到初始化 `name` 和 `age` 的目的

```cpp title:person.hpp
class Person {

    char *_name;
    int _age;

public:
    void init(const char *name, int age);  // 初始化成员函数
    void show();
};
```

```cpp title:person.cpp
#include <iostream>
#include <cstring>
using namespace std;

#include "person.hpp"


void Person::init(const char *name, int age){
    _name = new char[strlen(name) + 1];
    strcpy(_name, name);
    _age = age;
}
```

在使用 `Person` 类的对象之前，我们应该首先调用 `init()` 成员函数

```cpp title:person.cpp
int main() {
    Person p;
    p.init("zhangsan", 20);  // 调用 init 成员函数
    p.show();
    return 0;
}
```

编译 `person.cpp` 并运行的结果如下

```shell
➜  cppfiles g++ person.cpp -o person
➜  cppfiles ./person
name: zhangsan, age: 20
```

> [!question] 
> 
> **类的成员函数知道哪个对象调用它**。它是怎么知道的呢？其原因放在后面介绍
> 

### 构造函数

使用 `init()` 这样的函数为类对象提供初始化功能不优雅也容易出错

> [!attention] 
> 
> + 我们可能忘记调用 `init()`
> + 或者调用两次 `init()`。这会导致对象被初始化两次，往往会带来灾难性后果
> 

更好的方式就是允许我们定义一个成员函数，该成员函数是专门完成对象初始化任务的。这种函数的本质是 **构造一个给定类型对象的值**，因此被称为 **构造函数**

> [!tip] 构造函数
> 
> 专门完成对象的初始化任务。它最显著特征就是 **与类具有相同的名字** 并且 **没有返回类型**；此外，与普通成员函数并无区别
> 

```cpp title:person.hpp
class Person {

    char *_name;
    int _age;

public:
    Person(const char *name, int age) {
        _name = new char[strlen(name) + 1];
        strcpy(_name, name);
        _age = age;
    }
    void show();
};
```

在创建对象时，就需要提供对应的初始化值，从而完成对象的创建

```cpp title:person.cpp
int main() {
    Person p{"张三", 20};
    p.show();
    return 0;
}
```
 
编译 `person.cpp` 并运行的结构如下

```shell
➜  cppfiles g++ person.cpp -o person
➜  cppfiles ./person                
name: 张三, age: 20
```

> [!important] 
> 
> 构造函数会在对象创建时自动调用
> + **全局对象**：程序启动之后 `main()` 函数调用之前，会自动调用构造函数
> + **静态对象**：程序控制到达静态对象的定义位置时，会自动调用构造函数，并且只会调用一次
> + **局部对象**：程序控制到达局部对象的定义位置时，自动调用构造函数
> 

请注意：即使我们不定义构造函数，编译器也会为自动为我们添加一个构造函数，称为**默认构造函数**

> [!tip] 默认构造函数
> 
> 默认构造函数是 **由编译器自动生成的无参构造函数**。默认构造函数不会初始化对象的数据成员
> 
> 当我们提供了构造函数之后，编译器就不再插入默认构造函数
> 

```cpp
class Person {

    char *_name;
    int _age;

public:
    Person() {}  // 编译器生成的默认构造函数，其实默认构造函数什么也不会做
    void show();
};
```

> [!tip] 构造函数是可以重载的
> 
> 通过重载构造函数，可以为类提供多重初始化对象的方式。从而提高对象创建的灵活性
> 

假设我们的 `Person` 类中还有一个成员 `_phone` 用于存储电话号码，由于数据成员 `_phone` 并不是必选的。因此，可以为 `_phone` 提供初始值，也可以不提供。通过重载构造函数，可以很清晰的表达这个思想

```cpp
class Person {

    char *_name;              // 姓名
    int _age;                 // 年龄
    unsigned short _phone[11];  // 电话号码

public:
	// 不提供成员 _phone 的初始值
    Person(const char *name, int age) {
        _name = new char[strlen(name) + 1];
        strcpy(_name, name);
        _age = age;
        for (int i = 0; i < sizeof(_phone); i++) {
            _phone[i] = 0;
        }
    }
    // 提供成员 _phone 的初始值
    Person(const char *name, int age, const unsigned short *phone) {
        _name = new char[strlen(name) + 1];
        strcpy(_name, name);
        _age = age;
        for (int i = 0; i < sizeof(_phone); i++) {
            _phone[i] = phone[i];
        }
    }
    void show();
};
```

观察上面代码中的两个构造函数，它们视乎是相关联的。通过使用 **默认参数** 可以消除这这种关联情形

> [!tip] 使用默认参数消除关联的构造函数，从而减少构造函数的数量
> 
> C++ 的函数支持默认参数，通过默认参数可以使用更少的实参调用函数。因此，使用默认参数的构造函数可以匹配多重调用，从而减少构造函数的数量
> 

```cpp title:person.hpp
class Person {

    char *_name;              // 姓名
    int _age;                 // 年龄
    unsigned short _phone[11];  // 电话号码

public:

    Person(const char *name, int age, const unsigned short *phone=nullptr) {
        _name = new char[strlen(name) + 1];
        strcpy(_name, name);
        _age = age;
        
        // 如果 phone 为空，则将 _phone 数组清零
        if(!phone) {
            memset(_phone, 0, sizeof(_phone));
            return;
        }
        // 否则，将 phone 数组的内容拷贝到 _phone 数组
        memcpy(_phone, phone, sizeof(_phone));
    }

    void show();
};
```

```cpp title:person.cpp
#include <iostream>
#include <cstring>
using namespace std;

#include "person.hpp"

void Person::show(){
    cout << "name: " << _name 
    << ", age: " << _age << ", phone: ";

    for(int i = 0; i < sizeof(_phone)/sizeof(_phone[0]); i++) {
        cout << _phone[i];
    }
    cout << endl;
}


int main() {
    Person p{"张三", 20};
    p.show();
    Person p2{"李四", 21, (unsigned short []){1,3,5,2,1,6,6,9,7,2,1}};
    p2.show();
    return 0;
}
```

编译运行 `person.cpp` 的结果为

```shell
➜  cppfiles g++ -g person.cpp -o person
➜  cppfiles ./person
name: 张三, age: 20, phone: 00000000000
name: 李四, age: 21, phone: 13521669721
```

### 类内初始化器

类 `Person` 的构造函数 `Person()` 中是对数据成员的赋值而不是初始化。在 C++ 中，对类对象的初始化是使用 **初始化列表** 完成的

> [!tip] 初始化列表
> 
> 位于构造函数的参数列表之后，函数体之前。用冒号(`:`) 开始，如果需要初始化多个数据成员使用逗号(`,`)分隔
> 
> 初始值采用 `{}` 或者 `()` 形式给出
> 

```cpp title:person.hpp
class Person {

    char *_name;              // 姓名
    int _age;                 // 年龄
    unsigned short _phone[11];  // 电话号码

public:

    Person(const char *name, int age, const unsigned short *phone=nullptr)
    : _name{new char[strlen(name) + 1]}, _age{age}, _phone{0}
     {
        
        strcpy(_name, name);
        
        // 如果 phone 为空，则将 _phone 数组清零
        if(!phone) {
            memset(_phone, 0, sizeof(_phone));
            return;
        }
        // 否则，将 phone 数组的内容拷贝到 _phone 数组
        memcpy(_phone, phone, sizeof(_phone));
    }
    void show();
};
```

由于构造函数可以重载，这个初始化列表可能需要多次书写。因此，我们可以使用 **类内初始化器**

```cpp title:person.hpp
class Person {

    char *_name {nullptr};              // 姓名
    int _age {0};                 // 年龄
    unsigned short _phone[11]{0};  // 电话号码

public:

    Person(const char *name, int age, const unsigned short *phone=nullptr);
    void show();
};
```

现在，类 `Person` 对象的每个数据成员都有初始值了。当然，也可以在构造函数中自定的初始化列表

```cpp title:person.cpp
Person::Person(const char *name, int age, const unsigned short *phone) 
:_name{new char[strlen(name)+1]}, _age{age}
{

    strcpy(_name, name);
    
    // 如果 phone 为空，则将 _phone 数组清零
    if(!phone) {
        memset(_phone, 0, sizeof(_phone));
        return;
    }
    // 否则，将 phone 数组的内容拷贝到 _phone 数组
    memcpy(_phone, phone, sizeof(_phone));
}
```

这段代码等价于

```cpp
Person::Person(const char *name, int age, const unsigned short *phone) 
:_name{new char[strlen(name)+1]}, _age{age}, _phone{0}
{

    strcpy(_name, name);
    
    // 如果 phone 为空，则将 _phone 数组清零
    if(!phone) {
        memset(_phone, 0, sizeof(_phone));
        return;
    }
    // 否则，将 phone 数组的内容拷贝到 _phone 数组
    memcpy(_phone, phone, sizeof(_phone));
}
```

### 对象占用的内存空间

C++ 中 `class` 与 `struct` 本质上是一样的。因此一个类对象占用的空间取决于数据成员的大小及其声明顺序和内存对齐

> [!tip] 
> 
> 数据成员的大小参考 [[C 语言：基本概念#基本类型]]
> 
> 内存对齐参考 [[C 语言：结构体#内存对齐]]
> 

类 `Person` 的尺寸由数据成员 `_name` `_age` 和 `_phone` 以及对齐方式确定

```cpp
class Person {

    char *_name {nullptr};              // 姓名
    int _age {0};                 // 年龄
    unsigned short _phone[11]{0};  // 电话号码
    // ...
}; 
```

+ 成员 `_name` 是一个指针，在 $64$ 位平台上占 $8$ 字节；该成员会放在地址为 $8$ 的倍数的位置
+ 成员 `_age` 是 `int` 类型，在 $64$ 位平台上占 $4$ 字节；该成员会放在地址为 $4$ 的倍数的位置
+ 成员 `_phone` 是 `short` 类型的数组，一个 `short` 在 $64$ 位平台上占 $2$ 字节；该成员会放在地址为 $2$ 的倍数的位置

由于 `Person` 类最大对齐位置是 $8$，因此 `Person` 对象会放在地址为 $8$ 的倍数的位置。下图展示了 `Person` 对象的内存布局

![Pasted image 20250127004946|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755785388881-2512541643f14130952578c0e7fb5bd2.png)

因此 `Person` 类的对象占用 $40$ 字节。下面的代码验证了该结果是否正确

```cpp title:person.cpp
int main() {
    Person p{"张三", 20};
    p.show();
    cout << "sizeof(p): " << sizeof(p) << endl;
    return 0;
}
```

编译运行的结果为

```shell
➜  cppfiles g++ -g person.cpp -o person
➜  cppfiles ./person
name: 李四, age: 21, phone: 13521669721
sizeof(p): 40
```

> [!question] 空对象占用多少字节呢？
> 
> 没有任何数据成员的类的对象占用 $1$ 字节的内存空间。这个一个字节中的数据是没有任何用处的数据，只是为了实现对类对象的引用。
> 
> 如果对象不占用的任何空间，该对象的引用将无法创建
> 

```cpp title:test.cpp

#include <iostream>
using namespace std;


class A {};

int main() {
    cout << "sizeof(A): " << sizeof(A) << endl;
    return 0;
}
```

这段代码的输出结果为

```shell
➜  cppfiles g++ test.cpp -o test       
➜  cppfiles ./test  
sizeof(A): 1
```

## 对象的销毁

我们定义的 `Person` 类有一个指针成员，并且在构造函数中为指针成员申请了内存。当 `Person` 类的对象不再使用时，申请的内存应该需要被释放。然而，我们并没有执行任何 `delete` 语句

> [!tip]
> 
> 一个类的对象如果持有堆空间，当对象不能被访问时就应该释放对象持有的堆空间，否则就会造成 **资源泄漏**
> 

### valgrind 工具箱

`valgrind` 是一种开源工具集，它提供了一系列用于调试和分析程序的工具。其中最为常用和强大的工具就是 **memcheck**。它是 valgrind 中的一个内存错误检查器，它能够对 C/C++ 程序进行内存泄漏检测、非法内存访问检测等工作。

```shell
sudo pacman -Syu valgrind
```

安装完成后，我们使用 `memcheck` 工具检查我们的 `Person` 类对象在销毁时是否有残留的未释放的内存

```shell
valgrind --tool=memcheck ./person
```

如果想要更详细的泄漏情况，如造成泄漏的代码定位，编译时加上 `-g`

```shell
valgrind --tool=memcheck --leak-check=full ./person
```

如果需要查看静态区的情况，还需要

```shell
valgrind --tool=memcheck --leak-check=full --show-reachable=yes ./person
```

> [!tip] 但是这么长的指令使用起来不方便，每查一次就得输入一次
> 
> 为了解决这个问题，我们可以在 `.zshrc` 或者 `.profile` 中添加别名
> 
>  ```shell
> alias memcheck='valgrind --tool=memcheck --leak-check=full --show-reachable=yes'
> ```
> 

下面我们使用 `memcheck` 工具查看 `Person` 类是否发生内存泄漏

```shell hl:12,13,25-30
➜  cppfiles memcheck ./person 
==58917== Memcheck, a memory error detector
==58917== Copyright (C) 2002-2024, and GNU GPL'd, by Julian Seward et al.
==58917== Using Valgrind-3.24.0 and LibVEX; rerun with -h for copyright info
==58917== Command: ./person
==58917== 
name: 张三, age: 20, phone: 00000000000
name: 李四, age: 21, phone: 13521669721
sizeof(p): 40
==58917== 
==58917== HEAP SUMMARY:
==58917==     in use at exit: 14 bytes in 2 blocks
==58917==   total heap usage: 4 allocs, 2 frees, 74,766 bytes allocated
==58917== 
==58917== 7 bytes in 1 blocks are definitely lost in loss record 1 of 2
==58917==    at 0x4846613: operator new[](unsigned long) (vg_replace_malloc.c:729)
==58917==    by 0x109208: Person::Person(char const*, int, unsigned short const*) (in /home/dyp/文档/cppfiles/person)
==58917==    by 0x109388: main (in /home/dyp/文档/cppfiles/person)
==58917== 
==58917== 7 bytes in 1 blocks are definitely lost in loss record 2 of 2
==58917==    at 0x4846613: operator new[](unsigned long) (vg_replace_malloc.c:729)
==58917==    by 0x109208: Person::Person(char const*, int, unsigned short const*) (in /home/dyp/文档/cppfiles/person)
==58917==    by 0x1093F5: main (in /home/dyp/文档/cppfiles/person)
==58917== 
==58917== LEAK SUMMARY:
==58917==    definitely lost: 14 bytes in 2 blocks
==58917==    indirectly lost: 0 bytes in 0 blocks
==58917==      possibly lost: 0 bytes in 0 blocks
==58917==    still reachable: 0 bytes in 0 blocks
==58917==         suppressed: 0 bytes in 0 blocks
==58917== 
==58917== For lists of detected and suppressed errors, rerun with: -s
==58917== ERROR SUMMARY: 2 errors from 2 contexts (suppressed: 0 from 0)
```

> [!tip] 
> 
> 由于 `Person` 类的对象没有释放其持有的堆空间，造成内存泄漏
> 

那么如何进行妥善的内存回收呢？这需要交给 **析构函数** 来完成。

### 析构函数

构造函数初始化对象。它创建供成员函数进行操作的环境。创建环境时需要获取资源，如文件、锁、内存等，这些资源在使用后必须释放

为了确保类的对象释放这些资源，需要一个可以 **自动触发的函数**，**保证在对象销毁时被调用**，这样的函数称为类的 **析构函数**。析构函数与类的构造函数做相反的操作，因此析构函数的名字就是取反符号(`~`)后面跟上类名即可。

> [!tip] 
> 
> 析构函数与构造函数做的事情时相反的。析构函数名就是取反符号(`~`)后面紧跟类名
> 

以 `Person` 类为例，在对象初始化时在堆空间中申请了内存。因此，当对象不再被访问时，就应该自动调用析构函数释放内存

```cpp title:person.hpp
class Person {

    char *_name {nullptr};              // 姓名
    int _age {0};                 // 年龄
    unsigned short _phone[11]{0};  // 电话号码

public:
    // 构造函数
    Person(const char *name, int age, const unsigned short *phone=nullptr);
    // 析构函数
    ~Person();
    void show();
};
```

```cpp title:person.cpp
Person::~Person() {
    if (_name) {
        delete [] _name; 
    }   
}
```

现在，我们再一次使用 `memcheck` 工具检查 `person` 对象是否有内存泄漏

```shell
➜  cppfiles memcheck ./person
==62315== Memcheck, a memory error detector
==62315== Copyright (C) 2002-2024, and GNU GPL'd, by Julian Seward et al.
==62315== Using Valgrind-3.24.0 and LibVEX; rerun with -h for copyright info
==62315== Command: ./person
==62315== 
name: 张三, age: 20, phone: 00000000000
name: 李四, age: 21, phone: 13521669721
==62315== 
==62315== HEAP SUMMARY:
==62315==     in use at exit: 0 bytes in 0 blocks
==62315==   total heap usage: 4 allocs, 4 frees, 74,766 bytes allocated
==62315== 
==62315== All heap blocks were freed -- no leaks are possible
==62315== 
==62315== For lists of detected and suppressed errors, rerun with: -s
==62315== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
```

显然，在使用析构函数之后，对象持有的堆空间中的内存就被释放了，所以就没有内存泄漏了

### 构造和析构函数的调用时机


> [!tip] 全局对象
> 
> 程序开始运行时，在 `main()` 函数被调用前就会调用全局对象的构造函数
> 
> 在程序结束运行时，自动调用全局对象的析构函数
> 

> [!tip] 局部对象
> 
> 程序控制到达局部对象的定义处时，自动调用构造函数
> 
> 程序的控制离开局部对象的作用域时，自动调用析构函数
> 

> [!tip] static 静态对象
> 
> 程序控制到达 static 静态对象的定义位置时，自动调用构造函数
> 
> 程序结束运行时，自动调用所有的 static 静态对象的析构函数
> 

> [!tip] 堆空间中的对象
> 
> 程序的控制从 `new` 返回时自动调用构造函数
> 
> 程序控制执行到 `delete` 时自动调用析构函数
> 

## 默认拷贝

**默认情况下，对象是可以拷贝的**。特别是，一个类对象可以用同类的另一个对象的副本进行初始化

```cpp
Person p1{"张三", 20, (unsigned short[]){1,3,5,2,7,0,8,1,9,4,4}}
Person p2 = p1;
```

> [!tip] 默认情况下，**一个类对象的副本是逐成员拷贝得到**
> 

上述代码中 `p1` 和 `p2` 的关系如下

![Pasted image 20250130000424|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755785388881-11617f57a2f64107a8c1caa15398178d.png)

显然，这种默认复制并不是我们期望的行为。我们会在后续介绍如何提供更恰当的行为。参考 [[C++：拷贝语义]]

## 特殊成员

### explicit 构造函数

默认情况下，用单一参数调用一个构造函数，其行为类似从参数的类型到类自身类型的类型转换。这种类型转换有时非常有用，比如，我们在 [[C++：抽象机制概览]] 中接触的 `complex` 类

```cpp
complex<double> d{1};  // 默认
```

> [!tip] 
> 
> 对于复数类型，忽略虚部可以得到实数轴上的一个复数。这是符合数据定义的
> 

在大多数情况下，这种默认转换可能是混乱和错误的主要根源。假设我们定义了如下类 `Date` 用于表示日期的概念

```cpp
class Date {
public:
    Date(int y=0, int m=0, int d=0);
private:
    int year;   // 年
    int month;  // 月
    int day;    // 日
};
```

构造函数 `Date()` 可以接受 $0$ 至 $3$ 个参数，当使用一个参数调用构造函数 `Date()` 时会默认进行类型转换

```cpp
void fact(Date d);

void f() 
{
	Date d{15};  // d 的值为 {15, 0, 0}
	// ...
	fact(15);   // 含义不清，可能造成混乱
	d = 15;     // 含义不清，可能造成混乱
}
```

> [!tip] 
> 
> 上述代码中第 $7$ 行和第 $8$ 行含义不清，究其原因就是 $15$ 与 `Date` 类没有明显的清晰的逻辑关系
> 

幸运的是，我们可以 **指明构造函数不能作为隐式类型转换**。如果构造函数的声明带有关键字 `explicit`，则它只能用于初始化和显式类型转化

> [!tip] 
> 
> `explicit` 表示明确这个构造函数只能用于初始化和显式类型转换
> 

```cpp
class Date {
public:
    explicit Date(int y=0, int m=0, int d=0);  // 明确只能用于初始化和显式类型转换
private:
    int year;   // 年
    int month;  // 月
    int day;    // 日
};
```

下面是使用类 `Date` 的示例，正确和错误的使用方式均给出了注释

```cpp
Date d1{15};        // 正确：直接初始化
Date d2 = Date{15}; // 正确：拷贝初始化
Date d3 = {15};     // 错误：不允许隐式类型转换
Date d4 = 15;       // 错误：不允许隐式类型转换

void f()
{
    my_fct(15);       // 错误：不允许隐式类型转换
    my_fct({15});     // 错误：不允许隐式类型转换
    my_fct(Date{15}); // 正确：显式类型转换
}
```

> [!tip] **直接初始化** 和 **拷贝初始化**
> 
> 使用 `=` 进行初始化可看做 **拷贝初始化**。初始化器的副本会被放入待初始化的对象。对于不使用 `=` 的初始化称为 **直接初始化**
> 
> 请注意：在拷贝初始化中，如果初始化器是一个右值，拷贝操作可能会被编译器优化为移动操作
> 

> [!important] 
> 
> 默认情况下，请将单参数的构造函数声明为 `explicilt`。除非有很好的理由，否则必须采用默认的方式。
> 
> 谨记：如果定义隐式类型转换构造函数，请在注释中注明原因。
> 

### static 成员

现在继续关注 `Date` 类的设计。我们希望在构造 `Date` 对象的时候可以获取默认值，我们可以设置一个全局静态变量用于保存 `Date` 的默认值

```cpp
static  Date default_date{1970, 01, 01};

Date::Date(const int y, const int m, const int d): year{y}, month{m}, day {d}
{
    year = y ? y : default_date.year;
    month = m ? m : default_date.month;
    day = d ? d : default_date.day;
}
```

> [!attention] 
> 
> 由于 `Date` 依赖于全局变量，因此 `Date` 只能用于定义和正确使用 `default_date` 的上下文
> 

幸运的是，我们可以通过 **静态成员** 获得这种便利性其实不需要承担使用公开访问的全局变量的负担。

> [!tip] 静态成员：属于类但不属于类对象的成员称为静态成员
>
> C++ 静态成员包括：**静态数据成员** 和 **静态成员函数**
> 
> **静态数据成员**： 只有唯一副本，而不是像普通数据成员那样每个对象都有其副本
> 
> **静态成员函数**： 需要访问类成员但又不需要通过特定对象调用的函数
>

将 `default_date` 设计为 `Date` 的静态成员，并提供 `set_default()` 静态成员用于修改 `default_date` 的值

```cpp
class Date {  
public:  
    explicit Date(int y=0, int m=0, int d=0);  
  
    static void set_default(int y, int m, int d);  // 静态成员函数  
private:  
    int year;   // 年  
    int month;  // 月  
    int day;    // 日  
    static  Date default_date;  // 仅仅只是声明  
};
```

使用 `default_date` 可以定义如下 `Date()` 构造函数

```cpp
Date::Date(const int y, const int m, const int d): year{y}, month{m}, day {d}
{
    year = y ? y : default_date.year;
    month = m ? m : default_date.month;
    day = d ? d : default_date.day;
}

```

在合适的时候使用 `Date::set_default()` 修改默认值。注意，引用 `static` 成员，需要通过 `类名::`，而不是具体的对象

```cpp
void f() {
	Date::set_default(2025, 1, 29);
}
```

> [!attention] 请注意
> 
> **在类内部的静态成员仅仅只是声明**，需要在某处定义它们。在定义时不在需要指定 `static` 关键字
> 

```cpp
Date Date::default_date{1970, 1, 1};

void Date::set_default(int y, int m, int d) {
    default_date = Date{y, m, d};
}
```

### const 成员

在 C++ 中，成员可以是 `const` 的，包括数据成员和成员函数。对于 `const` 数据成员 **只能在构造函数的初始化列表中初始化**，一旦初始化完成，`const` 数据成员就具有只读属性

> [!tip] 
> 
> `const` 数据成员 **只能在构造函数的初始化列表中初始化**。C++ 11 标准之后可以使用类内初始化器进行初始化。
> 
> `const` 数据成员一旦完成初始化就具有只读属性，此时之后的操作就不能对其进行修改
> 

定义了一个 `Point` 类用于表示笛卡尔坐标系中的一个点。我们希望创建出一个点之后，这个点的位置就不能改变。

```cpp
class Point {
public:
	explicit Point(int ix=0, int iy=0);
private:
	const int _ix;
	const int _iy;
};
```

> [!tip] 
> 
> `Point` 类的数据成员 `_ix` 和 `_iy` 在初始化之后就不允许修改了
> 

由于数据成员 `_ix` 和 `_iy` 是 `const` 的，因此只能在初始化列表中进行初始化。在构造函数体内部是无法对 `_ix` 和 `_iy` 进行赋值的

```cpp
Point::Point(int ix, int iy): _ix{ix}, _iy{ix} {}
```

C++ 还支持 `const` 成员函数，用于表示该函数的调用不会修改对象的任何数据成员。编译器会捕捉 `const` 成员函数体中视图修改数据成员的操作

我们在 [[#类定义]] 中定义的 `Person` 类中有一个成员函数 `show()`，它不会修改任何数据成员，也不应该修改任何数据成员。因此，我们可以将其声明为 `const` 成员函数

```cpp title:person.hpp
class Person {

    char *_name {nullptr};              // 姓名
    int _age {0};                 // 年龄
    unsigned short _phone[11]{0};  // 电话号码

public:
	// ...
    void show() const;  // const 成员函数
};
```

> [!tip] 
> 
> 在定义时，`const` 关键字必须跟随。因为 `cosnt` 是函数类型的一部分，而不是声明
> 

```cpp title:person.cpp
void Person::show() const {  // 这个 cosnt 不能丢失，否则会出现编译错误
    cout << "name: " << _name 
    << ", age: " << _age << ", phone: ";

    for(int i = 0; i < sizeof(_phone)/sizeof(_phone[0]); i++) {
        cout << _phone[i];
    }
    cout << endl;
}
```

> [!tip] 
> 
> `const` 成员函数可以被 `const` 对象和非 `const` 对象调用。然而，普通成员函数只能被非 `const` 对象调用
> 

```cpp
#include "person.hpp"

int main() {
    const Person p1{"张三", 20, (const unsigned short[]){1, 3, 5, 7, 9, 0, 2, 4, 6, 8, 0}};
    p1.show();  // const 对象
    
    Person p2 = p1;
    p2.show();  // 非 const 对象
}
```

> [!summary] 
> 
> 如果一个成员函数中确定不会修改数据成员，就把它定义为 `const` 成员函数
> 

注意同名的成员函数的 `const` 版本和非 `const` 版本是重载关系。哪怕他们具有相同的参数列表，也是重载关系。因为编译器在参数列表中添加了一个参数.

> [!tip] 
> 
> `const` 版本的成员函数编译器会将 `const T *const this` 放在参数列表的第一个位置
> 
> 非 `const` 版本的成员函数编译会将 `T *const this` 放在参数列表的第一个位置
> 

例如，类 `X` 的 `print()` 成员函数

```cpp
class X {
public:
	void print() const;  // void print(const X * const this)
	void print();        // void print(X *const this)
}
```

`const` 成员函数中不允许修改数据成员，`const` 数据成员初始化后不允许修改，其效果是否相同？

> [!attention] 
> 
> 在 `const` 成员函数中，`this` 指针的类型是 `const T * const this`，也就是说 `this` 指针指向的对象的值不能修改
> 
> 如果数据成员包含一个普通的指针成员，在 `const` 成员函数中，可以修改指针指向对象的值，但是不能修改指针成员的指向 
> 

```cpp
class B {
    int *p;
public:
    B(int i) : p(new int{i}) {}
    void f() const {
        *p = 10;        // OK：由于成员 p 的值没有被修改，只是修改了 *p 的值 
        delete p;
        p = new int{20};  // 错误: 修改了 const B * this 中的成员的值，意味着修改了常量对象的值
    }
};
```

> [!attention] 
> 
> 在非 `const` 成员函数中，`this` 指针的类型是 `T *const this`，也就是说 `this` 指针指向对象的值可以修改
> 
> 如果数据成员包含一个指针成员是 `const` 成员，在非 `const` 成员函数中，可以修改指针的指向，但是不能修改指针指向的对象
> 

```cpp
class C {
    const int *q;
public:
    C(int i) : q{new int{i}}{}

    void g() {
        *q = 10; // 错误：修改了 const 对象
        delete q;
        q = new int{10};  // 正确： *q 是 const 的，但是 q 不是 const 的 
    }
};
```
### mutable 数据成员

某些成员函数在逻辑上是 `const`，但它仍然需要修改成员的值。即对一个用户而言，成员函数看起来不会改变对象的状态，但是它更新了用户不能直接观察的某些细节

> [!tip] 逻辑常量性
> 
> 某些 `const` 成员函数会在背后修改一些用户不能直接观察的细节，这通常被称为 **逻辑常量性**
> 

例如，类 `Date` 可能会有一个返回字符串表示的函数。由于构造字符串通常是非常耗时的，因此，在对象中保存一个拷贝，在反复要求字符串表示时可以简单的返回此拷贝即可(除非 `Date` 的值已经被修改)

```cpp
#include <string>
class Date {
public:
    explicit Date(int y=0, int m=0, int d=0);

    // 获取 Date 的字符串表示
    std::string toString() const;
private:
    int year;   // 年
    int month;  // 月
    int day;    // 日
    
    bool isCached;
    std::string cache;
    void computeCache() const;
};

Date::Date(int y, int m, int d) : year{y}, month{m}, day{d} {}

std::string Date::toString() const {
    if (!isCached) {
        computeCache();
    }
    return cache;
}

void Date::computeCache() const {
    cache = std::to_string(year) + "-" + std::to_string(month) + "-" + std::to_string(day);
    isCached = true;
}
```

> [!warning] 
> 
> 上述代码中的第 $27 \sim 30$ 用于计算 `Date` 的字符串表示。由于 `computeCache()` 在 `const` 成员函数中被调用，它不应该修改普通的数据成员
> 

为了让 `const` 成员函数可以修改数据成员，只需要将数据成员指定为 `mutalbe`(可变的)

```cpp hl:13,14
#include <string>
class Date {
public:
    explicit Date(int y=0, int m=0, int d=0);

    // 获取 Date 的字符串表示
    std::string toString() const;
private:
    int year;   // 年
    int month;  // 月
    int day;    // 日
    
    mutable bool isCached;      // mutable 成员
    mutable std::string cache;  // mutable 成员
    void computeCache() const;
};
```

> [!tip] 
> 
> `mutable` 成员即使实在 `const` 对象中也是可以被修改的
> 

`mutable` 成员只适用于小对象的表现形式的一小部分。对于更复杂的情形，一般是将需要改变的数据放在一个独立对象中，然后间接访问它们

```cpp hl:3-6,27-32,34-37
#include <string>

struct cache {
    bool isCached;
    std::string cache;
};

class Date {
public:
    explicit Date(int y=0, int m=0, int d=0);

    // 获取 Date 的字符串表示
    std::string toString() const;
private:
    int year;   // 年
    int month;  // 月
    int day;    // 日
    
    cache *c;
    void computeCache() const;
};

Date::Date(int y, int m, int d) : year{y}, month{m}, day{d}, c{new cache} {
    c->isCached = false;
}

std::string Date::toString() const {
    if (!c->isCached) {
        computeCache();
    }
    return c->cache;
}

void Date::computeCache() const {
    c->cache = std::to_string(year) + "-" + std::to_string(month) + "-" + std::to_string(day);
    c->isCached = true;
}
```

> [!tip] 
> 
> 这种支持缓存的编程技术可以推广到各种形式的懒惰求值中
> 

### 成员类型

类型和类型别名也可以作为类成员。例如，假设我们正在实现 [[数据结构：树与二叉树#二叉树]] 。我们可以将 `Node` 类作为 `Tree` 类的成员类

```cpp

template <typename T>
class Tree {
    using value_type = T;            // 成员别名
    enum Policy {rb, splay, treeps}; // 成员枚举
    class Node {
        Node *right;
        Node *left;
        value_type value;  // 可访问所属类的类型或类型别名
    public:
        void f(Tree *t);
    };
    Node *root;
public:
    void g(const T &);
	// ...
};
```

**成员类** 也称为 **嵌套类**，可以引用其所属类的类型和 `static` 成员

> [!attention] 
> 
> 当给定所属类的一个对象时，只能引用非 `static` 成员
> 

嵌套类可以访问其所属类的成员，甚至是 `private` 成员，但是它没有当前类对象的概念

```cpp
template <typename T>
void Tree<T>::Node::f(Tree *t) {
    // 这里没有 this 指针指向 Tree 类型的对象
    // 此时的 this 指向的是 Tree::Node 类型的对象
    root = right;     // 错误：没有指定类型为 Tree 的对象. root 不是静态的，必须属于某个 Tree 的对象
    
    t->root = right;  // 正确
    value_type v = left->value; // 正确：value_type 不与某个对象关联
}
```

> [!tip] 
> 
> 嵌套类可以访问其所属类的所有成员，但是它没有所属类的对象的概念
> 

一个类没有任何特殊权限可以访问其嵌入类的私有成员

```cpp
template <typename T>
void Tree<T>::g(Tree::Node *p) {
    value_type val = right->value;    // 错误：没有 Tree::Node 类型的对象
    value_type val = p->right->value; // 错误：Node::right 是私有的
    p->f(this);                       // 正确
}
```

> [!attention] 
> 
> 一个类没有权限访问其嵌入类的私有成员
> 

## 自引用

继续 `Date` 类的设计。下面我们为 `Date` 类定义几个修改对象状态的成员函数

```cpp
class Date {
public:
	//...
    // 增加 n 年
    void addYear(int n);
    // 增加 n 月
    void addMonth(int n);
    // 增加 n 日
    void addDay(int n);
private:
    int _year;   // 年
    int _month;  // 月
    int _day;    // 日
    //...
};
```

成员函数 `addYear()` `addMonth()` 和 `addDay()` 一组相关的更新函数，一种很游泳的技术就是返回已更新对象的引用，这样就可以实现 **链式调用**

```cpp
void f(Date &d)
{
	d.addYear(1).addMonth(2).addDay(10);  // 链式调用
}
```

因此，我们需要修改这 $3$ 个函数的定义，让它们返回调用对象的引用

```cpp
class Date {
public:
	//...
    // 增加 n 年
    Date& addYear(int n);
    // 增加 n 月
    Date& addMonth(int n);
    // 增加 n 日
    Date& addDay(int n);
private:
    int _year;   // 年
    int _month;  // 月
    int _day;    // 日
    //...
};
```

每个非 `static` 成员函数都知道是那个对象调用的它，并且能够显式引用这个对象。

```cpp
Date& Date::addYear(int n) {
    _year += n;
    if(_day == 29 && _month == 2 && !isLeapYear()) {  // 增加 n 年后，如果是闰年，2 月 29 日变为 3 月 1 日
        _day = 1;
        _month = 3;
    }
    c->isCached = false;
    return *this;
}
```

> [!tip] `this` 指针
> 
> `this` 指针是编译器帮我们传递到成员函数内部的调用该函数的对象的指针。`this` 是一个指针常量，其类型就是 `T * const this`，**不能被修改**
> 
> 在成员函数中，可以直接引用数据成员，因为编译器会自动帮我们通过 `this` 找到对应对象的数据成员
> 

每次通过对象调用成员函数，编译器都会将对象的指针传递给成员函数，这就是成员函数知道是谁调用它的原因

> [!attention] 
> 
> 编译器将 `this` 指针作为成员函数的第一个参数传入的
> 

> [!tip] `this` 的生命周期
> 
> 由于 `this` 是由编译器传递的参数，因此 `this` 只在调用成员函数时才会存在，函数返回时就自动销毁了
> 
> 也就说是，`this` 指针的生命周期与对象的生命周期无关。`this` 指针本身只在成员函数执行期间存在，但它指向的对象可能在成员函数执行前就已经存在，并且在成员函数执行后继续存在
> 
