# 与 C 的差异

C++ 包含了 C 的大部分的特性，只有少部分没有包括，并且 C++ 提供了大量的 C 没有的特性。下面我们将介绍一些与 C 的差异

学习 C++ 我们同样从`"hello, world!\n"` 程序说起。我们需要建立一个 `.cc` 或 `.cpp`的文本文件并在其中写入如下内容

```cpp title:hello.cpp
#include <iostream>

using namespace std;

int main(int argc, char *argv[]) {
    cout << "hello, world!" << endl;
    return 0;
}
```

与 C 一样，我们需要将 C++ 源文件编译并链接为可执行文件。同样可以使用 GCC 编译器

```shell
➜ gcc hello.cpp -lstdc++ -o hello
```

> [!tip] 
> 
> 选项 `-lstdc++` 表示需要链接标准 C++ 库
> 

编译 C++ 源文件可以使用 `g++` 编译器进行编译，这样就会自动链接标准 C++ 库而不需要指定

```shell
➜ g++ hello.cpp -o hello
```

## 名字空间

### 组合问题

任何实际文件都是由若干个独立部分组成的。函数和类提供了相对细粒度的分离，而源文件则提供了粗粒度的分离

> [!tip] 
> 
> 最理想的方式就是 **模块化**，即独立的事物保持分离，只允许通过良好定义的接口访问模块
> 

C++ 通过 **函数**、**类**、**名字空间** 等语法特性的组合和源码的组织来表达模块化

当模块化设计失败时可能会引发一些问题。例如，一个图像库可能提供可能提供不同种类的图形化 `Shape` 及函数

```cpp title:graph.h
class Shape {/* ... */};
class Line: public Shape {/* ... */};
class Poly_line: public Shape {/* ... */}; // 相连的 Line 序列
class Text: public Shape{/* ... */}; // 文本标签

Shape operator+(const Shape&, const Shape&); // 图像组合

Graph_reader open(const char *); // 打开 Shape 文件
``` 

还有另一个库提供文本处理特性

```cpp title:text.h
class Glyph {/* ... */};
class Word {/* ... */};  // Glyph 序列
class Line {/* ... */}; // Word 序列
class Text: public Shape{/* ... */}; // Line 序列

FILE * open(const char *); // 打开 文本文件
Word operator+(const Line&, const Line&); // 连接
```

下面考虑这两个模块在一个程序中一起使用时带来的问题。假设我们使用 `#include` 将两个头文件包含到项目中

```cpp
#include "graph.h"
#include "text.h"
// ...
```

预处理器处理玩这两个 `#include` 指令后，就会形成如下定义

```cpp hl:2,4,12,13,6
class Shape {/* ... */};
class Line: public Shape {/* ... */};
class Poly_line: public Shape {/* ... */}; // 相连的 Line 序列
class Text: public Shape{/* ... */} // 文本标签

Shape operator+(const Shape&, const Shape&); // 图像组合

Graph_reader open(const char *); // 打开 Shape 文件

class Glyph {/* ... */};
class Word {/* ... */};  // Glyph 序列
class Line {/* ... */}; // Word 序列
class Text: public Shape{/* ... */}; // Line 序列

FILE * open(const char *); // 打开 文本文件
Word operator+(const Line&, const Line&); // 连接
```

这样导致了 `Line` `Text` `open` 定义了两次，编译器无法消除这样的歧义

### 名字空间

**名字空间(namespace)** 的概念用来直接表示本属一体的一组特性。名字空间的成员都位于相同的作用域中，可以直接访问；而从名字空间外访问它们就需要显示限定符号

> [!tip] 
> 
> 通过名字空间的划分，可以有效的避免名字冲突
> 

因此，我们可以将图形库命名为 `Graph`

```cpp title:graph.h
namespace Graph {
	class Shape {/* ... */};
	class Line: public Shape {/* ... */};
	class Poly_line: public Shape {/* ... */}; // 相连的 Line 序列
	class Text: public Shape{/* ... */}; // 文本标签
	
	Shape operator+(const Shape&, const Shape&); // 图像组合
	
	Graph_reader open(const char *); // 打开 Shape 文件
}
```

类似的，我们的文本库也可以命名为 `Text`

```cpp
#include <iostream>
namespace Text {
	class Glyph {/* ... */};
	class Word {/* ... */};  // Glyph 序列
	class Line {/* ... */}; // Word 序列
	class Text {/* ... */}; // Line 序列
	
	FILE * open(const char *); // 打开 文本文件
	Word operator+(const Line&, const Line&); // 连接
}

```

通过独特的名字空间就可以避免不同库代码中的名字发生冲突

> [!tip] 名字空间表达某种逻辑结构
> 
> 名字空间中的声明应该一起提供一些特性，使得在用户看来它们是一个整体，而且能反映一组共同设计策略
> 
> 实际上，在一个名字空间中声明的实体是作为名字空间的成员被引用的
> 

**一个名字空间形成了一个具名作用域**。在名字空间中可以引用之前定义的成员，而在名字空间外就不能引用名字空间中的成员，除非使用特殊方式

**名字空间是开放的**；即，可以从多个分离的名字空间声明中向一个名字空间添加名字。换句话说，**名字空间可以多次定义**

```cpp
namespace A {
	int f(); // 现在名字空间 A 包含成员 f()
}

namespace A {
	int g();  // 现在名字空间 A 包含成员 f() 和 g()
}
```

> [!tip] 
> 
> 名字空间的成员可以分散在多个文件中，而不需要连续放置在单一的文件中
> 

### 引用名字空间中的成员

#### 显示限定

为了引用名字空间的成员，可以使用完整的限定名字。也就是说，我们可以在名字空间的定义中声明一个成员，稍后使用 `名字空间名::成员名` 的语法定义它

例如：

```cpp
namespace Parser {
	double expr(bool);  // 声明
	double term(bool);  // 声明
	double prim(bool);  // 声明
}

double var = Parser::expr(true);  // 使用

double Parser::expr(bool get) {/* ... */} // 定义
```

> [!attention] 
> 
> 注意，不能使用 `名字空间名::成员名` 在名字空间的定义外为其声明一个新成员
> 
> 限制在名字空间的定义外声明新成员是 **为了捕获拼写错误** 和 **类型不匹配之类** 的错误
> 

```cpp
void Parser::logical(bool);  // 错误：Parser 中没有 logicl() 的声明
double Parser::trem(bool);   // 错误：Parser 中没有 trem() 拼写错误
double Parser::prim(int);    // 错误：Parse::prim() 接受 bool 类型参数
```

> [!tip] 
> 
> 名字空间形成了一个作用域，通常作用域规则也适用于名字空间。**全局作用域也是一个名字空间**，可以显式的用 `::成员名` 引用全局作用域中的成员 
> 

```cpp
int f();     // 全局函数

int g() {
	int f;   // 局部变量：屏蔽了全局函数 f
	f();     // 错误：f 是一个 int 类型的变量，而不是函数
	::f();   // 引用全局函数 f
}
```

#### using 声明

当我们需要在名字空间外频繁使用其名字时，反复使用 `名字空间名::成员名` 的方式显得非常繁琐

```cpp
#include <string>
#include <vector>
#include <sstream>

std::vector<std::string> split(const std::string&s) {
	std::vector<std::string> result;
	std::istringstream iss(s);
	for (std::string buf; iss >> buf;) {
		result.push_back(buf);
	}
	return result;
}
```

这里反复使用 `std::` 进行限定非常冗长乏味。为了缓解这样的问题，我们可以使用 `using` 声明来指出这段代码中 `string` 表示 `std::string`

```cpp
#include <string>
#include <vector>
#include <sstream>

// using 声明：指示 string 代表 std::string
using std::string;
// using 声明：指示 vector 代表 std::vector
using std::vector;

vector<string> split(const string&s) {
	vector<string> result;
	std::istringstream iss(s);
	for (string buf; iss >> buf;) {
		result.push_back(buf);
	}
	return result;
}
```

> [!tip] 
> 
> `using` 声明将一个代用名引入了 `using` 声明当前所在的作用域，最好尽量保持代用名的局部性避免发生混淆
> 

#### using 指示

使用 `using` 声明每次只会将指定的代用名引入当前作用域。如果需要多个代用名，就需要多次使用 `using` 声明。这样也显得比较乏味，而且每当向当前名字空间中加入新名字或删除名字都需要修改 `using` 声明。

为了解决上述问题，可以使用 `using` 指示。`using` 指示要求编译器允许我们在所在作用域中无需使用限定即可访问某个名字空间中的名字

```cpp
#include <string>
#include <vector>
#include <sstream>

// using 指示：指示编译器使用命名空间 std 中的所有名称。
using namespace std;

vector<string> split(const string&s) {
	vector<string> result;
	istringstream iss(s);
	for (string buf; iss >> buf;) {
		result.push_back(buf);
	}
	return result;
}
```

> [!tip] 
> 
> 使用 `using` 指示后，来自其他名字空间中的名字就好像是声明在名字空间外一样
> 

> [!attention] 
> 
> 过度的使用 `using` 指示可能会导致名字冲突
> 

#### 参数依赖查找

**接受用户自定义类型的参数的函数通常与用户自定义类型在相同的名字空间中**。因此，如果在使用函数的上下文中找不到函数定义，就可以在参数的名字空间中查找它，称为 **参数依赖查找(ADL)**。

> [!tip] ADL 的规则：基于参数类型关联名字空间
> 
> + 参数是类成员：关联类本身和包含类的名字空间
> + 参数是名字空间的成员：关联该名字空间
> + 参数是内置类型：没有关联名字空间
> 
> 

> [!attention] 
> 
> 注意：**名字空间本身必须处于使用函数的作用域中**，且函数声明必须在函数查找和使用之前
> 


```cpp
#include <string>

namespace Chronto {
	class Date {/* */};
	bool operator==(const Date&, const std::string&);
	std::string format(const Date&);
}

void f(Chronto::Date d, int i) {
	std::string s = format(d);  // 通过 ADL 查找 Chronto::format
	std::string t = format(i);  // 错误： int 类型没有关联名字空间
}
```

### 组合使用名字空间

在大规模程序中，可能使用很多名字空间。下面介绍用名字空间构造代码的技术

#### 便利性与安全性

`using` 声明将名字添加到局部作用域；而 `using` 指示则不会，它只是简单地令名字在其所在作用域中可访问

```cpp hl:9,12
namespace X {
	int i, j, k;
}

int k;                 // 全局名字 k

void f1() {
	int i = 0;         // 局部名字 i
	using namespace X; // 令名字空间 X 中的名字可访问
	i++;               // 访问局部名字 i
	j++;               // 访问 X::j
	k++;               // 错误：无法确定访问而的是全局名字 k，还是 X::k
	::k++;             // 访问全局名字 k
	X::k++;            // 访问名字空间 X 总的名字 X::k
}

void f2() {
	int i = 0;
	using X::i;  // 错误：i 在 f2() 中 中声明了两次
	using X::j;
	using X::k;  // 隐藏了全局 k
	
	i++;
	j++;  // X::j
	k++;  // X::k
}
```

> [!tip] 
> 
> 局部声明的名字，包括普通声明或 `using` 声明，都会隐藏同名的非局部变量，而且在声明点上该名字任何不合法的重载都会被检测出来
> 

> [!attention] 
> 
> `using` 指示将名字空间中的名字变成全局作用域可访问，在名字解析时它们与全局名字是平等的
> 
> `using` 指示会令库中的声明的很多名字可被访问，**未使用的名字冲突不认为是错误**
> 

#### 名字空间别名

名字空间的名字也有可能冲突，为此可能会为名字空间命名一个比较长的名字。长名字不利于在实际代码中使用。通过为名字空间的长名字起一个别名，就可以解决上述问题

```cpp
namespace American_Telephone_and_Telegraph {
	// ...
}

using ATT = American_Telephone_and_Telegraph;  // 别名

ATT::String s3 = "Grieg";  // 使用别名就和使用正常名字一样
```

#### 组合名字空间

通常需要组合已有接口来构造新接口。假设我们需要使用已有的 `HerString` 名字空间中的接口和 `HisVector` 名字空间中的接口

```cpp
namespace HerString {
	class String {/* ... */};
	String operator+(const String&, const String&);
	String operator+(const String&, const char *);
	void fill(char);
	// ...
}

namespace HisVector {
	template<class T> 
	class Vector{/* ... */};
	//...
}

namespace My_lib {
	using namespace HerString;  // 组合
	using namespace HisVector;
	void myFct(String&);
}
```

这样就可以使用 `My_lib` 来编写程序

```cpp
void f()
{
	My_lib::String s = "Byron";  // My_lib::String 寻找 My_lib::HerString::String
}
```

> [!tip] 
> 
> `名字空间名::标识符` 会在名字空间名指定的名字空间及其组合名字空间中寻找 `标识符`
> 

当需要定义名字空间的中的实体时，就需要了解真正的名字空间

```cpp
void My_lib::fill(char c)  // 错误：My_lib 中并没有声明 fill(char)
{
	// ...
}

void HerString::fill(char c)  // 正确
{
	// ...
}

void My_lib::my_fact(String &s) // 正确：String 表示 My_lib::String，会寻找到 HerString::String
{
	// ...
}
```

组合多个名字空间时可能会引起名字冲突，此时需要选择使用那个那个名字空间中的名字。例如

```cpp
namespace HisLib {
	class String{/* ... */};
}

namespace HerLib {
	class String{/* ... */};
}

namespace MyLib {
	using namespace HisLib; // 组合 HisLib 名字空间
	using namespace HerLib; // 组合 HerLib 名字空间
	
	// 使用 String 时就会出现冲突
	using HisLib::String;  // 选择使用 HisLib 名字空间中的 String
}
```

### 无名名字空间

有时候将一组声明封装在一个名字空间中只是为了防止名字冲突，即，保持代码的局部性而非为用户提供一个接口

```cpp
namespace Mine {
	// ...
}
```

由于不希望在局部环境之外的代码中看见名字 `Mine`，我们可以简单地不为名字空间命名即可

```cpp
namespace {
	// ...
}
```

显然，必须提供某种方法实现从无名名字空间之外访问其成员。因此，每个无名名字空间都有一个隐含的 `using` 指示

```cpp
namespace $$$ {
	// ...
}
using namespace $$$;
```

> [!tip] 
> 
> `$$$` 是名字空间所在作用域中的独一无二的名字。**不同编译单元中的无名名字空间是不同的**
> 

## const 和 constexpr

在 [[C 语言：基本概念#常量与常量表达式]] 中介绍过 `const` 和 `constexpr`。其中 `const` 用于声明变量是只读的，初始化后程序不能再修改该变量的值。而 `constexpr` 表示常量表达式，指示编译器在编译时求值

> [!tip] 只读变量
> 
> `const` 声明的变量在其作用域内不允许改变。通常用于在函数声明中规定函数不允许修改参数的值
> 

> [!tip] 编译时求值 
> 
> `constexpr` 的作用是启用并确保编译时求值。
> 

C++ 中的 `cosntexpr` 可以用于声明函数，**建议编译器在编译时求函数的返回值**。注意：**函数要非常简单编译器才会在编译时求值**

```cpp
#include <iostream>

constexpr int fib(int n) {
    if (n <= 2) {
        return 1;
    }
    return fib(n - 1) + fib(n - 2);
}

int main() {
    int rsult = fib(5);
    return 0;
}
```

编译上述代码， `g++` 生成的汇编代码如下

``` hl:4
0000000000001119 <main>:
    1119:	55                   	push   %rbp
    111a:	48 89 e5             	mov    %rsp,%rbp
    111d:	c7 45 fc 05 00 00 00 	movl   $0x5,-0x4(%rbp)  # 直接保存了 5, 而不是调用 fib() 函数
    1124:	b8 00 00 00 00       	mov    $0x0,%eax
    1129:	5d                   	pop    %rbp
    112a:	c3                   	ret
```

> [!tip]
> 
> 编译器直接对 `fib(5)` 进行了求值，而不是调用 `fib(5)`
> 

如果我们调用 `fib(100)`，由于递归计算 `fib(100)` 不再是简单的任务，所以 `g++` 不会直接生成 `fib(100)` 的结果。`g++` 会生成 `fib(100)` 函数的调用代码

``` hl:6
0000000000001119 <main>:
    1119:	55                   	push   %rbp
    111a:	48 89 e5             	mov    %rsp,%rbp
    111d:	48 83 ec 10          	sub    $0x10,%rsp
    1121:	bf 64 00 00 00       	mov    $0x64,%edi
    1126:	e8 0a 00 00 00       	call   1135 <_Z3fibi>
    112b:	89 45 fc             	mov    %eax,-0x4(%rbp)
    112e:	b8 00 00 00 00       	mov    $0x0,%eax
    1133:	c9                   	leave
    1134:	c3                   	ret
```

## 动态内存分配

在 C++ 中在堆上分配内存需要使用 `new` 关键字

```cpp
#include <iostream>
using namespace std;

int main() {
    int *p = new int; // 默认初始化为 int 类型的零值

    cout << *p << endl;    
    return 0;
}
```

如果给需要指定初始化，可以使用 `{}` 初始化列表

```cpp
#include <iostream>
using namespace std;

int main() {
    int *p = new int{10}; // c初始化为 10
    
    cout << *p << endl;    
    return 0;
}
```

对于一个使用 `new` 创建的对象来所，必须使用 `delete` 显式销毁它，否则在进程的生命周期内将会一直存在

> [!tip] 
> 
> 如果这些使用 `new` 对象没有被 `delete` 销毁，同时也无法访问，那么这些对象占用的内存就会变成垃圾，从而造成内存泄漏的问题
> 

```cpp
#include <iostream>
using namespace std;

int main() {
    int *p = new int{10}; // c初始化为 10

    cout << *p << endl;    

    delete p;
    cout << *p << endl; // 释放内存后，p指向的内存已经被释放，其中的内容变得不可测
    return 0;
}
```

> [!attention] 
> 
> 使用 `delete` 销毁指针 `p` 指向的对象后，指针 `p` 就变成了 **悬空指针**
> 

`new` 还可以用来创建对象的数组。通过 `new type[len]` 的方式分配 `sizeof(type) * len` 个字节的空间。释放 `new` 分配的数组需要使用 `delete[]`。

> [!tip] 
> 
> 使用 `delete` 和 `delete[]` 释放通过 `new` 分配的内存时需要知道内存空间的大小，参考 [[作业22：内存分配#实现 malloc() 和 free()]] 中的处理，实际分配的内存可能比请求的内存要大一点
> 

```cpp
#include <iostream>
using namespace std;

int main() {
    int *p = new int[10]; // 在堆上分配 sizeof(int) * 10 字节的内存，p 指向这块内存的首地址
    for (int i = 0; i < 10; i++) {
        // 使用 p 指向的内存
        p[i] = i;
    }

    delete[] p;  // 释放 p 指向的内存
    return 0;
}
```

> [!tip] 
> 
> `malloc/free` 与 `new/delete` 的区别
> + `malloc` 申请的内存空间无法自动初始化，而 `new` 可以为内存空间提供初始值
> + `malloc/free` 是库函数，而 `new/delete` 是 C++ 中的运算符
> 

### 内存管理

使用动态内存主要的问题包括

+ 对象泄漏：使用 `new` 创建对象，但是忘记使用 `delete` 销毁
+ 提前释放：在尚有指针指向该对象并且后续仍会使用该对象的情况下过早的 `delete`
+ 重复释放：同一对象被释放两次

对象泄漏可能会导致进程占用大量的内存资源，从而引发进程崩溃。如下代码就是内存泄漏的例子

```cpp
void create_memory_leak()
{
	int *p = new int[100]; // 分配了 sizeof(int) * 100 字节的内存
	// ...
	// 在返回之前，没有调用 delete[] p; 释放这个内存，导致内存泄漏
}
```

与之相比，**提起释放造成的问题更严重**，可能导致指向该对象的指针引用的对象无效或者引用其他对象

```cpp
int *p1 = new int{99};
int *p2 = p1;  // p1 和 p2 指向同一个对象
delete p1;     // p2 指向的对象同样被释放了
p1 = nullpter;  // 只是覆盖了 p1 的值，p2 还是指向原来的内存区域，从而造成了代码安全的错觉
char *p3 = new char{'x'};  // 此时 p3 和 p2 可能指向同一个片内存区域
*p3 = 99;  // 这行代码可能造成错误
```

通常我们不建议使用裸 `new` 和 `delete`。而是采用下列方法

> [!tip] 使用局部变量
> 
> 尽量不要将对象放在堆内存中，而是使用局部存储
> 

> [!tip] 使用管理器对象
> 
> 如果必须要在堆内存上存储对象时，把它的指针放在一个 **管理器对象** 中，让管理器对象作为作用域内的变量出现。由于管理器对象含有一个析构函数，可以确保释放资源
> 
> 这种方式称为 **资源获取即初始化**，是一项避免资源泄漏的基本技术
> 


C++ 提供了许多管理器包括 **智能指针** 以及各自 **容器**。这里我们首先介绍智能指针，不过在此之前，我们首先介绍 **指针的所有权**

**任何资源都必须先分配后释放**。使用 `new` 分配内存，必须使用 `delete` 释放内存；用 `fopen()` 打开的文件，必须使用 `fclose()` 关闭文件。**指针是最常用的资源句柄**，然而在程序中作为资源句柄和不作为资源句柄的指针似乎没有任何区别

```cpp hl:12,13
void confused(int *p) 
{
	// 释放 p? delete p
}

void f()
{
	X *pn = new int{7};
	int i {7};
	int q = &i;
	confused(pn);
	confused(q);
	confused(&global);
}
```

第 $12$ 行和第 $13$ 行不能使用 `delete p` 释放 `p` 所指向的内存。因为此时这里的指针 `p` 不在是由 `new` 分配的对象，无权 `delete` 它。如果 `confused()` 没有 `delete p` 又会造成资源泄露。很明显，需要在函数 `f()` 内部管理存储在堆内存上的对象的生命周期。然而，在大规模程序中需要使用某种简单有效的策略追踪和维护那些需要 `delete` 的资源

> [!tip] 
> 
> 一种好的策略就是 **把表示某种所有权的指针全部置于资源管理器对象中**。此时，我们就假定索 **所有不在资源管理器对象的指针都不负责管理资源**
> 
> C++ 提供了许多管理器对象，例如 `unique_ptr` `vector` `string` 等
> 


## 左值与右值

在描述左值和右值的概念之前，我们首先考虑对象的两种属性

| 属性  | 描述                     |
| :-- | :--------------------- |
| 有身份 | 该对象有名字；有该对象的指针；有该对象的引用 |
| 可移动 | 能否把对象的内容移动出来           |

上面两个属性共有 $4$ 中组合形式，有 $3$ 中形式在 C++ 语法规则精确描述，其中 既没有身份又不能移动的对象不重要，也不需要描述它。下图描述了左值和右值的概念

![[Pasted image 20250119164624.png]]

其中 `i` 表示有身份；`m` 表示可移动。一个经典的 **左值是 有身份但不能移动**，一个经典的**右值 是允许执行移出操作的对象**

## 引用

通过使用指针，我们能够以很低的代价在一个范围内传递大量数据，与直接拷贝所有数据不同，我们只需要传递执行这些数据的指针的值即可。指针的类型决定了我们能对指针所指的对象进行那些操作

使用指针与使用对象名存在以下差别：
+ 语法形式不同：`*p` 和 `p->m` 取代了 `obj` 和 `obj.m`
+ 同一个指针可以在不同时刻指向不同的对象
+ 使用指针需要坚持指针值是否为 `nullptr`，还需要考虑指针是否指向了我们需要的对象

使用指针与使用对象名的差异非常烦人。可能需要我们花费大量精力去管理变化多端的指针变量，而且还得时时防范指针取 `nullptr` 的情况

C++ 为了解决上述问题提供语言层面的机制 **引用**。和指针类似，**引用作为对象的别名存放的也是对象所在的内存地址**。与指针相比，引用不会带来额外的开销。同时引用避免了指针存在的缺陷
+ 访问引用与访问对象本身从语法形式上看是一样的
+ 引用所引的永远是一开始初始化的对象
+ 不存在空引用。引用始终引用一个对象

为了体现 **左值/右值** 以及 **const和非const** 的区别，C++ 提供 $3$ 种引用形式

| 引用           | 描述             |
| :----------- | :------------- |
| 左值引用         | 引用我们希望改变值的对象   |
| `const` 左值引用 | 引用那些我们不希望改变的对象 |
| 右值引用         | 引用那些使用后就不需要的对象 |

### 左值引用

声明语法中，声明符前使用 `&` 表示声明符是一个左值引用。例如，`int &r` 表示引用 `r` 是一个左值引用，用于绑定了一个 `int` 类型的对象

```cpp
void f()
{
	int var = 1;
	int &r {var}; // r 和 var 对应同一个 int 对象
	int x = r;  // x 的值为 1
	
	r = 2; // r 引用的 int 对象变为了 2，也就是说，var 为 2
}
```

> [!attention] 
> 
> 为了确保引用对应某个对象，必须对引用进行初始化
> 

```cpp
int var =1;
int &r1 {var};  // OK: r1 初始化为 var 的引用
int &r2;        // 错误：r2 缺少初始化器
extern int &r3; // OK: r3 在其他位置初始化
```

C++ 没有专门针对引用的运算符。作用于引用的运算符都会操作引用绑定的对象

```cpp
void g()
{
	int var = 0;
	int &rr {var};
	++rr; // var 的值 +1
	int *pp = &rr; // 获取的是 var 的地址
}
```

> [!tip] 
> 
> 引用本身的值一旦经过初始化就不能再改变；永远指向一开始指定的对象
> 

对引用使用取地址运算符(`&`)得到的是引用关联对象的指针。我们既不能令某个指针指向引用，也不能定义引用的数组，从这个意义上来说，**引用不是对象**。显然，**引用的实现方式类似于常量指针**，每次使用引用实际上是对该指针执行解引用操作

> [!attention] 
> 需要谨记：指针是一个对象，而引用不是对象
> 
> ![[Pasted image 20250119180122.png]]
> 

`T&` 引用的初始化只能使用 `T` 类型的左值。与之不同的是，`const T&` 引用的初始值不一定非得是左值，甚至可以不是 `T` 类型的

```cpp
double &dr = 1;  // 错误：double& 引用的初始化需要一个 double 类型的左值。1 不是左值
const double &cdr {1}; // OK 
```

语句 `const double &cdr {1};` 的初始化过程如下

```cpp
double temp = double{1};  // 执行目标类型为 double 的隐式类型转换并存储在一个临时变量中
const double &cdr {temp}; // 使用临时变量初始化 const double& 引用
```

> [!tip] 
> 
> 该临时变量的是生命周期从创建开始，直到它的引用作用域结束
> 

**引用可以作为函数的形参类型**，这样函数就能修改传入其中的对象的值

```cpp
void incr(int &a)
{
	++a;
}

void f()
{
	int x = 1;
	incr(x);  // x = 2。会使用实参对象 x 初始化引用 a
}
```

实参传递在本质上与初始化过程非常相似。当调用 `incr()` 时，形参 `a` 就变成了实参`x` 的别名

**引用还可以作为函数的返回值**。注意，不要返回局部变量的引用。函数返回后，函数的栈帧随之销毁，此时引用的对象不复存在

```cpp hl:5
#include <iostream>

int& g() {
    int a = 10;
    return a; // 返回 a 的引用
}


int main() {
    int& b = g();  // a 对象被销毁
    std::cout << b << std::endl;    
    return 0;
}
```

### 右值引用

右值引用对应一个临时变量，可以修改这个对象，并认为这个对象以后不再会被用到。如果引用所引的是临时对象，就能使用比较廉价的移动操作代替昂贵拷贝操作了

> [!tip] 
> 
> 对于像 `vector` `string` 这样的对象来说，它们本身所含的信息量非常大，用于指向这些信息的句柄可能非常小。
> 
> 如果确定以后不再会使用到该对象，执行廉价的移动操作是最好的选择。例如，函数中的局部变量在函数返回后就不再会被用到了
> 

**右值引用只能绑定右值，不能绑定左值**。正好与左值引用相反

```cpp
string var {"Cambridge"};
string f();

string &r1 {var};         // 左值引用：r1 绑定到了 var 上
string &r2 {f()};         // 左值引用：错误的初始化，f() 是右值
string &r3 {"Princeton"}  // 左值引用：错误，字面值是右值

string &&rr1 {f()};       // 右值引用：OK，rr1 绑定到了一个右值
string &&rr2 {var};       // 右值引用：错误，var 是左值
string &&rr1 {"Oxford"};  // 右值引用：字面值是右值

const string &cr1 {"Harvard"};  // 常量左值引用。创建一个临时变量，然后绑定到 cr1
```

声明符 `&&` 表示 **右值引用**。注意，不使用 `const` 右值引用，因为右值引用大多数的用法都是建立在能够修改所引对象的基础上的。虽然 `const` 左值引用和右值引用都能绑定右值，但是它们实现目的不同

> [!tip] `const` 左值引用
> 
> `const` 左值引用是保护参数不被修改
> 

> [!tip] 右值引用
> 
> 右值引用实现了一种 **破坏性读取**，某些数据本来需要拷贝，使用右值引用可以优化其性能
> 

右值引用所引对象的使用方式与左值引用所引对象以及普通变量没什么区别

```cpp
string f(string &&s) {
	if(s.size())
		s[0] = toupper(s[0])
	return s
}
```

## 类型推断

`auto` 关键字在 C 语言中用于声明对象的存储期。由于局部变量默认的存储期就是 `auto`，因此很少使用。C++ 对 `auto` 关键字进行了重定义，它用于表示 **从初始化器中推断变量的类型**

```cpp
auto a1 = 123; // a1 的类型是 int

auto v1{12345}; // v1 的类型也是 int

auto v2{f()};  // v2 的类型是函数 f() 返回类型
```

`decltype(expr)` 用于从一个表达式中推断类型，注意这里仅仅只是获取 `expr` 的类型，而不会像 `auto` 那样定义一个变量

```cpp
decltype(a1) b1;  // b1 的类型与 a1 的类型相同
```

## 类型别名

给类型起一个新名字是有必要的。当原来的类型名过于冗长或复杂时，为其命名一个全新的名字是很有必要的。有时还可以通过类型别名提供必要的信息，提高程序的可读性

C 语言提供的类型别名使用 `typedef` 关键字，这在 C++ 中依旧可以使用。然而 C++ 提供了根方便的的类型别名定义方法

```cpp
using Pchar_t = char *;  // 字符指针
using Pfun_t = int(*)(double); // 函数指针
```

> [!attention] 
> 
> **类型别名只是一种已有类型的同义词**，并不代表一种新类型
> 

## 类型转换

### 隐式类型转换

和 C 语言一样，C++ 中浮点数和整数可以在赋值语句以及表达式中自由混合使用。C++ 的编译器会自动将值的类型转换为相同的类型。

> [!tip] 
> 
> 由编译器执行的类型转换称为隐式类型转换
> 

不幸的是，隐式类型转换可能带来值的损失。如果某个转换导致了值丢失，则称这种类型转换为 **窄化类型转换**

> [!tip] 
> 
> **值丢失** ：如果我们转换了某个值的类型，然后再转换会原始类型时导致了值与原始值不相等
> 
> **值保护**：如果我们转换了某个值的类型，然后再转换会原始类型后与原始值保持一致
> 

#### 提升

与 C 语言一样，值保护的隐式类型转换通常称为 **提升**。在执行算术运算之前，通常把较短的整数类型通过 **整型提升** 为 `int` 类型

> [!attention] 
> 
> 整型提升的结果一般不会是 `long`，除非原始对象的值超过 `int` 类型能存储的范围
> 

整型提升规则与 [[C 语言：基本概念#隐式转换#常规算术转换]] 中介绍的规则类似

#### 赋值转换

在编写源码时，应该注意不要使用较大尺寸类型的值给较小尺寸类型的值进行赋值。因为此时的隐式类型转换会发生窄化类型转换，从而在不知不觉中丢失信息。

```cpp
void f(double d)
{
	char c = d;  // 注意：double 类型向 char 类型转换。目标类型的尺寸小于原始类型，从而导致窄化转换的发生
}
```

使用 `{}` 列表能防止窄化转换

```cpp
void f(double d)
{
	char c {d};  // 错误：编译器发现窄化转换的发生，并禁止此行为
}
```

#### 整数类型转换

整数能被转换成其他整数类型。**普通的枚举类型** 也能转换成整数类型

如果目标类型是 `unsigned` 的，则结果值是原始值的位模式按照目标类型进行解释

> [!tip] 
> 
> 换句话说，结果值是 $原始值 \mod{2 ^n}$，$n$ 是目标类型所占的位数
> 

```cpp
unsigned char uc = 1023;  // 1023 的位模式是：0000 0011 1111 1111；uc 只会存储 1023 的最低 8 位 1023 % 2^8
```

如果目标类型是 `signed` 的，当原始值能用目标类型表示时，不发生改变；如果不能，结果依赖于实现

```c
signed char sc = 1023;  // sc 的值依赖于实现
```

#### 指针和引用类型转换

任何对象的指针都能隐式转换为 `void *`。指向派生类的指针或引用能隐式转换成指向其可访问的且无二义性的基类的指针或引用

> [!attention] 
> 
> 指向函数的指针和指向成员的指针不能转换为 `void *`
> 
> 求值结果为 $0$ 的表达式能转成任意指针类型的空指针
> 

#### 浮点数向整数类型转换

浮点数转换为整数类型时，浮点值的小数部分被忽略掉。换句话说，从浮点数向整数类型转换会导致截断

> [!attention] 
> 
> 如果截断后的值不能用目标类型表示，则该行为是未定义行为
> 

### 显式类型转换

C 风格的显示类型转换在 C++ 中依旧可以使用。由于 C 风格的类型转换不会进行任何正确性的检查，从而会导致一些列的错误情况的发生

#### 构造转换

使用 `{}` 符号提供对新值类型安全的构造。表达式 `T{e}`  使用值 `e` 构造一个类型为 `T` 的值

```cpp
auto d1 = double{2};
double d2 {double{2}/4}; 
```

表达式 `T{e}` 只会执行类型安全的转换。如果转换发生信息丢失，则不执行。换句话说，就是 **禁止窄化类型转换**

```cpp
auto i = int{2.1};  // 错误，发生窄化转换
```

#### 命名转换

命名转换可以让类型转换地含义更明显，并且让程序员有机会表达它们地真实意图。C++ 提供了 $4$ 种命名转换

| 类型转换符              | 描述                          |
| :----------------- | :-------------------------- |
| `static_cast`      | 执行不同的数据类型之间或者在指针类型之间进行转换    |
| `const_cast`       | 移除或添加 `cosnt` 属性            |
| `reinterpret_cast` | 任意类型之间的转换。从新解释位模式的表达形式      |
| `dynamic_cast`     | 专门用于 **基类和派生类之间安全地转换指针和引用** |

##### `static_cast`

`static_cast` 是 C++ 中的强制转换运算符之一，它允许 **在不同的数据类型之间** 或 **在指针类型之间** 进行转换。这种类型的强制转换 **执行编译时检查**，如果 **给定类型之间没有有效的转换，则给出错误**。`static_cast` 通常比 C 样式强制转换更安全，因为它不会对数据执行不安全的重新解释，并且允许更好的类型检查。

```cpp
int i = 42;
float f = static_cast<float>(i); // Converts integer i to float f
```

```cpp
enum Color { RED, GREEN, BLUE };
int value = 1;
Color color = static_cast<Color>(value); // Converts integer value to corresponding Color enumeration
```

##### `const_cast`

`const_cast` 是一种 C++ 中的强制转换，允许您 **删除变量或向变量添加 `const`**。换句话说，它使您能够修改 `const` 或 `volatile` 对象，或者更改指针或对 `const` 或 `volatile` 类型的引用。在某些情况下，当您需要将 `const` 变量作为参数传递，或者当函数参数需要非 `const` 类型，但您希望确保变量在整个代码中保持不变时，这非常有用

请记住，使用 `const_cast` 修改真正的 `const` 变量可能会导致未定义的行为，因此最好仅在绝对必要时使用此功能。

```cpp
#include <cassert>
#include <iostream>

void modify(int * pvalue) {
    *pvalue =42;
}

int main() {
	const int value = 10;

	int * pvalue = const_cast<int *>(&value);
	std::cout << "value: " << value << std::endl;
	std::cout << "befor modify: *pvalue is " << *pvalue << std::endl;
	modify(pvalue);
	std::cout << "after modify: *pvalue is " << *pvalue << std::endl; 
	assert(pvalue == &value);
    std::cout << "after modify: value is " << value << std::endl;
}
```

运行结果为

```
value: 10
befor modify: *pvalue is 10
after modify: *pvalue is 42
after modify: value is 10
```

> [!attention] 
> 
> 函数 `modify(pvalue)` 的确是修改了 `value` 的值。为什么最后一行输出的还是 $10$？
> 
> 对于 `g++` 编译器来说，`value` 的值的确是被修改了。由于 `value` 是只读的，因此在 `modify()` 后续使用 `value`，`g++` 编译器直接使用的是 `value` 初始化的字面值 
> 

```cpp
#include <cassert>
#include <iostream>

void modify(int * pvalue) {
    *pvalue =42;
}

int main() {
	const int value = 10;

	int * pvalue = const_cast<int *>(&value);
	std::cout << "value: " << value << std::endl;
	std::cout << "befor modify: *pvalue is " << *pvalue << std::endl;
	modify(pvalue);
	std::cout << "after modify: *pvalue is " << *pvalue << std::endl; 
	assert(pvalue == &value);
    std::cout << "after modify: value is " << value << std::endl;
	
    int *pvalue2 = const_cast<int *>(&value);
    std::cout << "*pvalue2: " << *pvalue2 << std::endl;
}
```

输出结果为

```
value: 10
befor modify: *pvalue is 10
after modify: *pvalue is 42
after modify: value is 10
*pvalue2: 42
```

## 函数

C++ 函数声明与 [[C 语言：函数]] 相同，只是 C++ 额外提供了一些新特性


### 后置返回类型

在 C++ 中函数的返回类型可以放在函数的后面。使用 `auto` 关键字表示函数的类型放在参数列表后面。后置返回类型由符号 `->` 引导。如下所示，下面两种声明是等价的

```cpp
string to_string(int a);
auto to_string(int a) -> string;
```

后置返回类型在 **函数模板** 声明中非常有用，因为器返回类型依赖于参数。如下函数模板的声明

```cpp
template<class T, class U>
auto product(const vector<T> &x, const vector<U> & y) -> decltype(x * y);
```


### inline 函数

在 [[C 语言：声明#内联函数]] 中我们介绍过 `inline` 函数了

> [!tip] 
> 
> 所谓的 `inline` 函数，就是 **指示编译器把函数的每次调用都使用对应的机器指令替代**
> 
> + `inline` 函数会使得编译后的程序体积增大，但是省略函数栈帧的创建和销毁时间
> 

例如，下列 $n$ 的阶乘函数可以被定义为 `inline` 的

```cpp
inline int fac(int n) {
	return (n < 2) ? 1: n * fac(n-1);
}
```

`inline` 高速编译器应该尝试为 `fac()` 的调用生成内联代码，而非先为 `fac()` 函数构建代码再通过常规的调用机制调用它。一个较为智能的编译器会为 `fac(6)` 直接生成常量 $720$

> [!tip] 内联函数面临许多复杂情况
> 
> + 内联函数之间相互递归调用
> + 单个内联的递归函数
> + 函数与输入无关
> 
> 很难确保内联函数的每一次调用都是真的内联的
> 

如果希望在编译时求值，最好把函数声明成 `constexpr`，并确保求值过程中用到所有的函数都是 `constexpr`

> [!important] 
> 
> 如果内联函数的定义出现在多个编译单元中，通常是因为该内联函数被定义在头文件中
> 

### constexpr 函数

通常情况下，函数无法在编译时求值，因此不能再常量表达式中被调用。但是通过将函数指定为 `constexpr`，就告知编译器当给定了常量表达式作为实参时，函数应该能被用在常量表达式中

```cpp
constexpr int fac(int n)
{
	return (n > 1) ? n * fac(n-1) : 1;
}

constexpr int f = fac(9);  // 必须在编译时求值
```

> [!tip] 
> 
> 函数必须足够简单才能在编译时求值
> + `constexpr` 必须包含一条独立的 `return` 语句
> + 没有循环
> + 没有局部变量
> + `constexpr` 不能有副作用
> 
> 也就是说：`constexpr` 函数是一个纯函数
> + `constexpr` 函数允许递归和条件表达式。
> 

下列几个函数都是错误 `constexpr` 函数的示例

```cpp
int glob;

constexpr void bad1(int a) // 错误：constexpr 函数必须要有返回值
{
	glob = a;  // 错误：constexpr 函数中不能有副作用
}

constexpr int bad2(int a)
{
	if (a >= 0) return a; else return -a; // 错误：constexpr 函数中不能有 if 语句
}

constexpr int bad3(int a)
{
	sum = 0;
	for(int i = 0; i < a; i++) {  // 错误：constexpr 函数中不能有 for 循环
		sum += fac(i);  // 错误：constexpr 函数中不能有副作用
	}
	return sum;
}
```

### 默认参数

一个通用的函数所需要的参数通常比处理简单情况所需的参数要多。在声明函数的时候 **可以给形式参数指定默认值**

```cpp
void f(int a = 10); // 注意，只能在声明时提供默认值
```

> [!tip] 
> 
> 默认参数在函数 **声明时执行类型检查**，在 **调用时求值**
> 
> 因此，**避免使用值可能发生变化的默认参数**，否则可能会引入对上下文的微妙依赖
> 


提供默认值的形式参数只能位于形参列表的最后

```cpp
int f(int, int = 0, char * = nullptr);

int g(int = 0, int = 0, char *);  // 错误的
int g(int = 0, int, char * = nullptr);  // 错误的
```

> [!attention] 
> 
> 注意 `* =` 之间的空格是必须的。运算符 `*=` 赋值运算符
> 

**同一作用域内** 的一些列声明语句中，默认参数 **不能重复** 或者 **不能改变**

```cpp
void f(int x = 7);
void f(int = 7); // 错误：不允许重复默认参数
void f(int 8); // 错误：不允许改变默认参数

void g()
{
	void f(int x = 9); // OK: 该作用域内覆盖上层作用域的声明
}
```

### 函数重载

在 C 语言中，函数应该拥有不同的名字。有时候，这些函数在不同类型的对象上执行相同概念的任务，为它们取一个相同的名字是更好的选择。在 C++ 提供了 **函数重载** 机制用于实现为不同函数取一个相同名字的方法

> [!tip] 重载
> 
> 给不同类型的同一种操作起相同的名字称为重载
> 

例如，我们要实现一个加法函数，它不仅能执行整数加法，还能执行浮点数加法，还可以执行这些类型之间的加法。在 C 语言中，我们必须为每种类型命名不同的函数

```c
int add_int(int a, int b) 
{
	return a + b;
}

double add_float(double a, double b)
{
	return a + b;
}

double add_int_float(int a, double b) 
{
	return (double)a + b;
}

double add_float_int(double a, int b) 
{
	return a + (double)b;
}
```

在 C++ 中，我们可以使用同一个名字

```cpp
int add(int a, int b)
{
	return a + b;
}

double add(double a, double b) 
{
	return a + b;
}

double add(int a, double b)
{
	return (double)a + b;
}

double add(double a, int b)
{
	return a + (double)b;
}
```

对于这些约定俗成的名字的函数来说，重载函数的便利性非常明显。如果一个名字具有明显的语义，重载函数的便利性就更强

>[!tip] 过上述分析，函数重载必须要求 **名字相同** 且 **参数列表不同**。无关返回类型
>
>参数列表不同包括：**参数类型不同** 和 **参数数量不同**。如果名字相同参数列表也相同则为重复定义
>
>重载不考虑返回类型可以确保对运算法或者函数调用的解析独立于上下文
>

> [!important] 
> 
> 重载发生在一组重载函数集的成员内部，也就是说，**重载函数应该位于同一个作用域内**。不同的非名字空间作用域中的函数不会重载
> 
> 基类和派生类属于两个不同的名字空间，因此基类和派生类中的同名函数不会发生重载
> 

```cpp
void f(int);

void g() 
{
	void f(double);  // 不会与第一行的 f() 重载，因为不在同一个作用域
	f(4);  // 显然调用的 f(double)
}
```

如果希望实现跨类作用域或者名字空间作用域的重载需要使用 `using` 声明或 `using` 指示

> [!tip] 
> 
> ADL 也会导致跨名字空间重载
> 

#### 自动重载解析

程序中存在多个同名的函数。当调用函数时，如何确定调用哪一个呢？**函数的调用由编译器决定**，依据实参类型与作用域中的函数的哪个函数的形式参数类型最匹配。

> [!tip] 
> 
> 编译根据 **实参类型** 与作用域中的函数的 **形参类型** 进行 **最佳匹配**
> 

```cpp

void f() 
{
	add(1, 2);     // 调用 add(int, int)
	add(1.1, 1.2); // 调用 add(double, double)
	add(1.2, 3);   // 调用 add(double, int)
	add(2, 1.2);   // 调用 add(int, double)
}
```

C++ 为了更合理确定调用哪个函数，定义了如下顺序的评判标准

> [!tip] **精确匹配** 的优先调用
> 
> 实参类型 **无需类型转换** 或者 **仅需简单的类型转换** 就可以与函数形参类型匹配
> 
> 简单类型转换包括：数组名转换为指针、函数名转换为函数指针、非 `cosnt` 转换为 `const`
> 

> [!tip] **执行提升后匹配** 的次优先调用
> 
> 整数提升或者 `float` 转换为 `double`
> 

> [!tip] **执行标准类型转换后匹配** 的次次优先调用
> 
> 标准类型转换示例如下
> 
> + `int` 转换为 `unsigned int`
> + `int` 转换为 `double`
> + `double` 转换为 `int`
> + `double` 转换为 `long double`
> + `T*` 转换为 `void *`
> + `Derived*` 转换为 `Base *` 
> 

> [!tip] **执行用户自定义类型转换后匹配** 的优先级更低
> 
> 比如 `double` 转换为 `complex<double>` 
> 

> [!tip] **不定长参数** 的优先级最低


根据上述优先级规则，如果某次函数调用可以匹配的最高优先级上发现了不止一个可以的函数，则本次调用因产生二义性而失败。这些复杂的解析规则主要是考虑到 C/C++ 的内置数值类型规则而制定

```cpp
void print(int);
void print(const char *);
void print(double);
void print(long);
void print(char);

void f(char c, int i, short s, float f)
{
	print(c);  // 精确匹配 print(char).因为 c 的类型是 char,有对应的函数版本
	print(i);  // 精确匹配 print(int)
	print(s);  // 提升匹配 print(int).因为 s 的类型是 short，没有对应的函数版本，但是进行整数提升后匹配到 print(int)
	print(f);  // 提升匹配 print(double).因为 f 的类型是 float，没有对应的函数版本，但是将 float 提升为 double 后匹配到 print(double)
	
	print('a');  // 精确匹配 print(char)
	print(49);   // 精确匹配 print(int)
	print(0);    // 精确匹配 print(int)
	print("a");  // 精确匹配 print(const char *)
	print(nullptr); // 标准类型转换后匹配到 print(const char *)
} 
```


> [!important] 
> 
> 重载解析与函数声明顺序无关
> 

#### 多实参解析

当重载函数包含两个或多个参数时，自动重载解析规则将应用于每个参数，并且选出该参数的最佳匹配函数。如果某个函数是 **其中一个参数的最佳匹配**，同时在 **其他参数上也是更优先** 的匹配或者至少 **不弱于** 别的函数，则该函数就是最终确定的最佳匹配函数

```cpp
int pow(int, int);
double pow(double, double);
complex pow(double, complex);
complex pow(complex, int);
complex pow(complex, complex);

void k(complex z) 
{
	int i = pow(2, 2);         // 精确匹配 pow(int, int)
	double d = pow(2.0, 2.0);  // 精确匹配 pow(double, double)
	complex z2 = pow(2, z);    // 调用 pow(double, complex)
	complex z3 = pow(z, 2);    // 精确匹配 pow(complex, int)
	complex z4 = pow(z, z);    // 精确匹配 pow(complex, complex)
	
	double d = pow(2.0, 2);   // 匹配到 pow(int, int) 和 pow(double, double)，会产生二义性，调用失败
}
```

#### 手动重载解析

**当某个函数的重载版本过少或者过多都可能导致二义性**。应该尽量把一组重载函数当成整体，考察其对于函数的语义来说是否有意义。很多时候，可以通过增加一个函数版本来解决二义性问题

```cpp
void f1(char);
void f1(long);

void f2(char *);
void f2(int *);

void k(int i)
{
	f1(i);  // 二义性：f1(char) 和 f1(long) 都可能被选中
	f2(0);  // 二义性：f2(char *) 和 f2(int *) 都可能被选中
}
```

当然可以利用显示类型转换解析特定的调用

```cpp
f2(static_cast<int *>(0)); 
```

### 名字修改

**名字修改**（也称为名称修饰）是编译器使用的一种技术，用于对有关其名称中标识符的范围、类型、链接或其他标识信息（函数名称、变量名称等）的其他信息进行编码。**名字修改** 的主要目的是支持函数重载，这允许名称相同但参数列表不同的多个函数在单个程序中共存。

在 C++ 中，编译器根据每个函数和变量的作用域和类型为它们 **生成一个破坏性的名称**。破坏性的名称通常由 **原始名称**、**参数类型** 和 **其他信息** 连接而成，通常使用前缀或后缀。例如，我们由如下函数

```cpp
int add(int a, int b)
{
    return a + b;
}
```

`g++` 编译器生成的汇编代码如下

```
_Z3addii:
	pushq	%rbp	
	movq	%rsp, %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	-4(%rbp), %edx
	movl	-8(%rbp), %eax
	addl	%edx, %eax
	popq	%rbp
	ret
```

名字 `_Z3addii`，它编码了 **名字的字符数** 和 **参数信息**：函数名有 `3` 个字符以及接收两个 `int` 类型的参数

再看另一个示例

```cpp
int mutply(int a, int b)
{
    return a * b;
}
```

它的 `g++ -S` 编译出的汇编代码如下

```cpp
_Z6mutplyii:
	pushq	%rbp	
	movq	%rsp, %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	-4(%rbp), %eax
	imull	-8(%rbp), %eax
	popq	%rbp
	ret
```

注意 `_Z6mutplyii`，函数名有 `6` 个字符已经接收两个 `int` 类型的参数

## 结构体和联合

C++ 中的 `struct` 和 `union` 在语法形式上与 [[C 语言：结构体]] 没有任何差异，但是在语义上存在巨大差异

> [!tip] 
> 
> C++ 中 `struct` 和 `union` 是类，而不仅仅是数据的集合。也就是说，C++ 中的 `struct` 和 `union` 可以为其定义成员函数
> 

 ```cpp
 struct Point{
	int x, y;
	Point(int a, int b);
 }

Point p0; // 危险行为
Point p1{}; // 默认构造
```

## 枚举类型

**枚举类型** 用于存放用户指定的一组整数值。枚举类型的每一种取值各自对应一个名字，我们把这些值称为 **枚举值**

C++ 中的 `enum` 类型与  [[C 语言：结构体#枚举]] 没有任何区别。枚举类型和枚举值位于同一作用域，不同枚举类型中不能使用相同的枚举名

```cpp
enum Traffic_light {red, yellow, green};  // 红绿灯
enum Warning {green, yellow, orange, red};  // 火警等级
// 这里会出现重复定义错误：yellow, red, green 被重复定义了
// 其中，yellow 取值相同，red 和green 的取值不同
```

C++ 还提供了 `enum class`，它是限制了作用域的强枚举类型，**不会隐式转换为其他类型**

```cpp
enum class Tranffic_light{red, yello, green};
enum class Warning {green, yellow, orange, red};

Warning a1 = 7;               // 错误的，不存在 int 想 Warning 的类型转换
int a2 = green;               // 错误，green 未定义的
int a3 = Warning::green;      // 错误，不存在 Warning 向 int 类型的转换
Warning a4 = Warning::green;  // ok
    
void f(Tranffic_light x)
{
	if(x == 2){/* .... */}             // 错误：2 不是 Tranffic_light 
    if(x == red){/* .... */}           // 错误：red 未定义
    if(x == Warning::red){/* ... */}    // 错误：x 不是一个 Warning
    if(x == Tranffic_light::red) {/* ... */} // OK
}
```

> [!tip] 
> 
> 两个 `enum` 枚举值不会相互冲突，它们位于各自的 `enum class` 的作用域中
> 

枚举常用一些整数类型表示，每一个枚举值是一个整数。用于表示枚举的类型称为枚举的 **基础类型** 且 **必须是整数类型**。默认使用 `int` 类型。可以显示指定

```cpp
enum class Warning : int {green, yellow, orange, red};
// sizeof(Warning) == sizeof(int)
```

当然可以使用 `char` 类型代替 `int`

```cpp
enum class Warning : char {green, yellow, orange, red}; 
// sizeof(Warning) == sizeof(char)
```

> [!tip] 
> 
> `enum class` 属于用户自定义类型，可以为它定义 `|` 和 `&` 等于运算符
> 

## 异常处理

**异常是程序在执行期间产生的问题**。C++ 异常是指在 **程序运行时发生的特殊情况**，比如尝试除以零的操作

异常的概念可以帮助我们将信息从检测到错误的地方转递到处理该错误的地方。如果函数无法处理某个问题，则 **抛出** 异常，并寄希望与函数的调用者能直接或间接的处理该问题。如果函数希望处理某个问题，可以 **捕获** 相应的异常

主调用函数想要处理某些失败情形，可以把这些可能产生异常的表达式置于 `try` 语句块内，并在 `catch` 语句块内处理对应的失败情形

```cpp
void taskmaster()
{
	try {
		// 可能产生异常的表达式
	}
	catch(/* 异常类型 */) {
		// 处理对应类型的错误
	}
}
```

下面是异常处理的完整示例


```cpp
#include <iostream>
using namespace std;

double division(double x, double y)
{
	if (y == 0)
	{
		throw "Division by zero";
	}
	return x / y;
}

int main()
{
	double x = 10;
	double y = 0;
	double result;
	try {
		result = division(x, y);
	}
	catch (const char * message) {
		cout << "call division() error: " << message << endl;
		return 1;
	}
	cout << "call division() result: " << result;
	return 0;
}
```

> [!attention] 
> 
> 注意：`catch(Type message)` 捕捉的是 `Type` 类型的消息，只要是 `Type` 类型的消息都会被捕获，并将捕捉到的消息赋值给 `message`
> 

### 标准异常

C++ 在库下提供了一组标准的异常类 `<stdexcept>`，这些类可以用作更具体的错误处理的异常类型。其中一些类包括：

| 异常类型                 | 描述             |
| :------------------- | :------------- |
| `std::exception`     | 所有异常类的基类       |
| `std::logic_error`   | 表示程序可以静态检测的错误  |
| `std::runtime_error` | 表示程序执行过程中发生的错误 |

```cpp
#include <iostream>
#include <stdexcept>

using namespace std;

double division(double x, double y)
{
	if (y == 0)
	{
		throw runtime_error("Division by zero condition!");
	}
	return x / y;
}

int main()
{
	double x = 10;
	double y = 0;
	double result;
	try {
		result = division(x, y);
	}
	catch (runtime_error &e) {
		cout << "call division() error: " << e.what() << endl;
		return 1;
	}
	cout << "call division() result: " << result;
	return 0;
}
```
