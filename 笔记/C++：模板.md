# 模板

现代 C++ 提供了一种新的抽象：**模板**。**模板是一种通用的描述机制**，允许使用类型参数来定义函数或类。在使用时，类型参数可被具体的类型替代。

> [!tip] 
> 
> 也就是说，模板就是将类参数化
> 

C/C++ 这类静态强类型的编译型语言，要求先编译后运行。它们都有一个强大的类型系统，**希望在程序执行之前，尽可能地发现错误**，防止错误被延迟到运行时。像 Python 这种动态强类解释型语言没有编译步骤，变量的类型是在运行时确定的。因此，错误往往会在运行时才会被检查出来。

强类型程序设计中，参与运算的所有对象的类型在编译时即确定下来，并且编译程序将进行严格的类型检查。为了解决强类型的严格性和灵活性的冲突，也就是在严格的语法要求下尽可能提高灵活性，有以下 $3$ 种方式解决：**函数式宏**、 **重载函数** 和 **模板（将类型作为参数化）**

我们看下面的一个例子：假设我们要编写一个加法器，采用函数重载的方式就需要为每种类型甚至是他们的组合情况编写不同的函数。

```cpp
int add(int x, int y)
{
    return x + y;
}

double add(double x, double y)
{
    return x + y;
}

long add(long x, long y)
{
    return x + y;
}

string add(string x, string y)
{
    return x + y;
}
```

这显得代码比较冗余。采用模板就只用编写次，然后在运行的时候选择类型

```cpp
template <typename T>
T add(T x, T y)
{
    return x + y;
}
```

模板作为实现代码重用机制的一种工具，它可以实现 **类型参数化**，即把类型定义为参数， 从而实现了真正的代码可重用性。模板可以分为两类，一个是 **函数模版**，另外一个是**类模板**

模板本质上就是一个 **代码生成器**，它的作用就是 **让编译器根据实际调用来生成代码**。

> [!important] 
> 
> 请注意：**模板发生的时机是在编译时**
> 

## 函数模板

在 [[C++：移动语义和默认操作]] 中，我们接触了第一个函数模板：交换两个变量的值

```cpp
template<typename T>
void swap(T &a, T &b) {
    T tmp {a};  // 将 a 的副本放入 tmp，此时内存中有两份 a 的值
    a = b;      // 将 b 的值赋给 a，此时内存中有两份 b 的值
    b = tmp;    // 将 tmp 的值赋给 b，此时内存中有两份 tmp 的值
}
```

模板定义的第一句必须是 `template<typename T>`。它指出将要创建一个模板，并将类型命名为 `T`。关键字 `template` 和 `typename` 是必须的，如果使用较早的编译器，可以将 `typename` 替换为 `class`

> [!tip] 
> 
> 类型名的唯一要求就是必须符合变量的命名规范
> 

要让编译器知道程序需要一个特定形式的交换函数，只需在程序中使用 `swap()` 函数即可。编译器将检查所使用的参数类型，并生成相应的函数

```cpp
#include <iostream>

template<typename T>
void swap(T &a, T &b);

int main()
{
    int a = 10;
    int b = 20;

    swap(a, b);  // 会生成函数 void swap(int &, int &)

    std::cout << a << " " << b << std::endl;
    return 0;
}

void swap(T &a, T &b) {
    T tmp {a};
    a = b;    
    b = tmp;
}
```

 编译器会根据调用函数模板时传递的实参类型生成对应类型的函数。例如，上述示例中，我们使用两个 `int` 类型的实参调用函数模板 `swap()`，那么编译器就会生成 `void swap(int &, int &)` 函数

> [!attention] 
> 
> 请注意，函数模板不能缩短可执行程序。最终然后回有独立的函数定义，就像以手工方式定义了这些函数一样
> 

### 重载的模板

上述的 `swap()` 函数模板对于大多数类型而言都是符合要求的，比如 `int` `float` `double` 和 `struct` 等类型。但是，如果我们要交换的是两个数组的前 $n$ 项呢？上述的 `swap()` 将失效

```cpp
#include <iostream>

template<typename T>
void swap(T &a, T &b) {
    T tmp {a};
    a = b;
    b = tmp;
}


int main() {

    int a[3] = {1,2,3};
    int b[3];
    swap(a, b);

    std::cout << a << std::endl;
    std::cout << b << std::endl;
}
```

编译上述代码时，编译器会提示如下错误

```
➜  cppfiles g++ main.cpp
main.cpp：在实例化 ‘void swap(T&, T&) [with T = int [3]]’ 时:
main.cpp:15:9:   在此处的要求
   15 |     swap(a, b);
      |     ~~~~^~~~~~
main.cpp:5:7: 错误：invalid conversion from ‘int*’ to ‘int’ [-fpermissive]
    5 |     T tmp {a};
      |       ^~~
      |       |
      |       int*
main.cpp:6:7: 错误：无效的数组赋值
    6 |     a = b;
      |     ~~^~~
main.cpp:7:7: 错误：无效的数组赋值
    7 |     b = tmp;
      |     ~~^~~~~
```


> [!tip] 
> 
> 对于同一种算法，某些类型并一定能够支持。为满足任意类型都可以支持同一种算法的需求，可以像重载常规函数定义那样 **重载模板定义**。和常规重载一样，被重载的模板的函数特征标必须不同
> 

如下所述，为了交换两个数组的前 $n$ 项，我们可以新增一个函数模板

```cpp
template<typename T>
void swap(T *a, T *b, int n);
```

> [!attention] 
> 
> 请注意：模板 `swap(T *a, T *b, int n)` 的第 $3$ 个参数是一个具体类型。并非所有的函数模板的参数都必须是模板参数类型
> 

```cpp
#include <iostream>

template <typename T>
void swap(T &a, T &b)
{
    T tmp{a};
    a = b;
    b = tmp;
}

template <typename T>
void swap(T *a, T *b, int n)
{
    T tmp;
    for (int i = 0; i < n; i++)
    {
        tmp = *(a + i);
        *(a + i) = *(b + i);
        *(b + i) = tmp;
    }
}

int main()
{
    int a = 10, b = 20;
    swap(a, b);  // 生成 void swap(int &a, int &b)
    std::cout << a << " " << b << std::endl;

    int array1[3] = {1, 2, 3};
    int array2[3] = {4, 5, 6};
    swap(array1, array2, 3);  // 生成 void swap(int * a, int * b, int n)

    for(int i = 0; i < 3; i++) {
        std::cout << array1[i] << " ";
    }
    std::cout << std::endl;

    for(int i = 0; i < 3; i++) {
        std::cout << array2[i] << " ";
    }
    std::cout << std::endl;
}
```

### 模板的局限性

虽然模板很好用，但是模板也存在一些局限的。假设有如下函数模板

```cpp
template<typename T> 
void f(T a, T b) {....}
```

通常，我们都会假定类型 `T` 支持某些操作。例如，下面的代码假定定义了赋值操作，但如果 `T` 为数组，这种假设将不成立

```cpp
a = b;  // 如果 T 是数组，这个操作时不成立的
```

同样，下面的语句假设定义了小于操作 (`<`)，但如果 `T` 为结构，该假设便不成立

```cpp
if (a < b) {}// 如果 T 为结构体，并且没有定义 operator<() 操作，那么该操作不成立
```

> [!attention] 
> 
> 请注意：如果 `T` 为数组，由于数组名是地址，因此比较的是地址大小。这通常不是我们期望的操作
> 

下面的语句假定为类型 `T` 定义了乘法运算符，但如果 `T` 为数组、指针或结构，这种假设便不成立：

```cpp
T c = a * b; // 如果 T 为数组、指针或结构，这个操作是非法的
```

### 特例化

假设我们定义了如下结构

```cpp
struct Job {
	string name;
	double salary;
	int floor;
};
```

假设希望能够交换两个这种结构的内容。使用最先定义的模板能够完成这项任务，因为在 C/C++ 中是运行使用一个结构体给另一个结构体赋值的

然而，假设只想交换 `salary` 和 `floor` 成员，而不交换 `name` 成员，则需要使用不同的代码，但 `swap()` 的参数将保持不变（两个 `Job` 结构的引用），因此无法使用模板重载来提供其他的代码

C++ 允许我们提供一个特例化模板定义

```cpp
#include <iostream>

template <typename T>
void swap(T &a, T &b)
{
    T tmp{a};
    a = b;
    b = tmp;
}

struct Job {
    std::string name;
    double salary;
    int floor;
};

template<>
void swap<Job>(Job &a, Job &b) {
    double s = a.salary;
    a.salary = b.salary;
    b.salary = s;

    int f = a.floor;
    a.floor = b.floor;
    b.floor = f;
}

int main()
{
    int a = 10, b = 20;
    swap(a, b);
    std::cout << a << " " << b << std::endl;

    Job j1{"Java", 35000, 1};
    Job j2{"Python", 22500, 2};

    swap(j1, j2);  // 生成 swap(Job&, Job&)
}
```

> [!tip] 
> 
> 对于给定的函数名，可以有非模板函数、模板函数和显式具体化模板函数以及它们的重载版本
> 
> 特例化模板以 `template<>` 打头，并通过名称来指出类型
> 
> 特例化优先于常规模板，而非模板函数优先于特例化化和常规模板
> 

> [!attention] 优先级
> 
> 常规函数 => 特例化模板函数 => 常规模板函数
> 

```cpp
struct Job {
    std::string name;
    double salary;
    int floor;
};

void swap(Job &a, Job &b) {
    double s = a.salary;
    a.salary = b.salary;
    b.salary = s;

    int f = a.floor;
    a.floor = b.floor;
    b.floor = f;
}

template<>
void swap<Job>(Job &a, Job &b) {
    double s = a.salary;
    a.salary = b.salary;
    b.salary = s;

    int f = a.floor;
    a.floor = b.floor;
    b.floor = f;
}

template <typename T>
void swap(T &a, T &b)
{
    T tmp{a};
    a = b;
    b = tmp;
}
```

### 实例化

请注意：在代码中包含函数模板本身并不会生成函数定义，它只是一个用于生成函数定义的方案。通过模板和模板参数列表生成一个函数的过程称为 **模板实例化**。

> [!tip]
> 
> 换句话说就是，编译器使用模板为特定类型生成函数定义时，得到的是模板实例
> 

例如，我们调用 `swap(i, j)`，其中 `i, j` 是 `int` 类型的实参，则编译器就会生成 `swap(int &, int &)` 的函数定义

**模板并非函数定义**，但使用 `int` 的 **模板实例是函数定义**。这种实例化方式被称为 **隐式实例化**，因为编译器之所以知道需要进行定义，是由于程序调用 `swap()` 函数时提供了 `int` 参数

> [!tip] 
> 
> 使用不同的参数去调用模板函数，编译器会根据参数的类型自动实例化一个函数模板。这种实例化方式称为隐式实例化
> 

最初，编译器只能通过隐式实例化，来使用模板生成函数定义，但现在 C++ 还允许显式实例化。这意味着可以直接命令编译器创建特定的实例，如 `swap<int>()`。其语法是，声明所需的种类——用 `<>` 符号指示类型，并在声明前加上关键字 `template`：

```cpp
template void swap<int>(int &, int &);
```

实现了这种特性的编译器看到上述声明后，将使用 `swap()` 模板生成一个使用 `int` 类型的实例。也就是说，该声明的意思是 “使用 `swap()` 模板生成 `int` 类型的函数定义”

还可通过在程序中使用函数来创建显式实例化。例如，请看下面的代码

```cpp
template<typename T>
T add(T a, T b)
{
	return a + b;
}

...

int m 6;
double x = 10.2

cout << add<double>(x, m) << endl;  //注意：不推荐这样写，而是让编译器推导
```

这里的模板与函数调用 `add(x, m)` 不匹配，因为该模板要求两个函数参数的类型相同。但通过使用 `add<double>(x, m)`，可强制为 `double` 类型实例化，并将参数 `m` 隐式类型转换为 `double` 类型，以便与函数 `add<double>(double, double)` 的第二个参数匹配

### 分离模板的声明与实现

在一个源文件中，函数模板的声明与定义分离是可以的，即使把函数模板的实现放在调用之下也是可以的，与普通函数一致。

```cpp
//函数模板的声明
template <class T>
T add(T t1, T t2)；


void test1(){ 
    int i1 = 1, i2 = 2;
	cout << add(i1,i2) << endl;
}

//函数模板的实现
template <class T>
T add(T t1, T t2)
{
    return t1 + t2;
}
```

然而，如果像普通函数那样分别在 `.hpp` 文件中声明，然后在 `.cpp` 文件中实现，并在另一个 `.cpp` 文件中测试，那么编译将会错误

```cpp title:add.hpp
template <class T>
T add(T t1, T t2);
```

```cpp title:add.cpp
#include "add.hpp"

template<typename T>
T add(T lhs, T rhs) {
    return lhs + rhs;
}
```

```cpp title:main.cpp

#include "add.hpp"

int main() {
    int a = 10;
    int b = 20;
    int c = add(a, b);

    printf("%d + %d = %d\n", a, b, c);
    return 0;
}
```

当我们使用如下命令编译上述代码时，链接器提示未定义错误

```shell
➜  cppfiles g++ main.cpp add.cpp add.hpp 
/usr/bin/ld: /tmp/ccq1P7Vy.o: in function `main':
main.cpp:(.text+0x21): undefin
```

> [!important] 
> 
> 函数模板要被显示调用时，编译器才会为其生成对应的函数。也就是说，我们需要在 `add.cpp` 文件中调用一次 `add()`，那么编译器就会为其生成对应的指令
> 

```cpp title:add.cpp
#include "add.hpp"

template<typename T>
T add(T lhs, T rhs) {
    return lhs + rhs;
}

static int a = add(0, 0);  // 调用一次模板函数
```

然后我们再编译就会不会提示错误了

```shell
➜  cppfiles g++ main.cpp add.cpp add.hpp
➜  cppfiles ./a.out 
10 + 20 = 30
```

> [!attention] 
> 
> 在实现文件中调用函数通常是不符合编程规范的，也不是一个优雅的做法。
> 

如果要在使用文件调用函数模板时，推导的过程中，**看到的是完整的模板的代码**，那么应该可以解决问题。在头文件中加上 `#include "add.cpp"` 即可

```cpp title:add.hpp

#ifndef ADD_HPP

template<typename T>
T add(T lhs, T rhs);

#include "add.cpp"
#endif
```

```cpp title:add.cpp
template<typename T>
T add(T lhs, T rhs) {
    return lhs + rhs;
}
```

> [!tip] 
> 
> 请注意：在实现文件中，去调用头文件包含语句。如果加上，将会导致循环引用，从而出现重定义异常
> 


现在，我们只需要编译 `main.cpp` 也不会出现未定义异常了

```shell
➜  cppfiles g++ main.cpp
➜  cppfiles ./a.out 
10 + 20 = 30
```

> [!summary] 
> 
> 在 `.hpp` 中包含 `.cpp` 的内容，本质上就是相当于在头文件中实现了模板
> 
> 对模板的使用，必须要拿到模板的全部实现，如果只有一部分，那么推导也只能推导出一部分，无法满足需求。
> 
> 换句话说，**模板的使用过程中，其实没有了头文件和实现文件的区别**，在头文件中也需要获取模板的完整代码，不能只有一部分。
> 

C++的标准库都是由模板开发的，所以经过标准委员的商讨，**将这些头文件取消了后缀名，与 C 的头文件形成了区分；这些实现文件的后缀名设为了.tcpp**

### 非类型模板参数

模板参数有两种类型：**类型化参数** 和 **非类型化参数**。下面我们介绍非类型化参数，因为类型化参数已经在之前介绍过了。

定义模板时，在模板的参数列表中可以加入非类型参数。请注意：**非类型参数只能是整数类型**。例如，`bool, char, short, int, long, size_t` 等

```cpp
#include <iostream>
using namespace std;

template<typename T, int base>
T multiply(T x, T y) {
	return x * y * base;
}

void f() {
	int i1 = 3, i2 = 4;
	int result = multiply<int, 10>(i1, i2);  // 3 * 4 * 10
	cout << result << endl; 
}

int main() {
    f();
}
```

非类型化参数可以提供默认值，在使用模板函数时，就可以让编译器执行隐式实例化，而不需要我们进行显式实例化

```cpp
#include <iostream>
using namespace std;

template<typename T, int base = 10>  // 非类型化参数可以提供默认值
T multiply(T x, T y) {
	return x * y * base;
}

void f() {
	int i1 = 3, i2 = 4;
	int result = multiply<int, 20>(i1, i2);  // 3 * 4 * 20。显式实例化
	cout << result << endl; 
	result = multiply(i1, i2); // 隐式实例化
	cout << result << endl;
}

int main() {
    f();
}
```

模板参数与普通的函数参数类似，对于类型化参数，也可以提供默认类型

```cpp
template <class T = int, int base = 10>
T multiply(T x, T y){
    return x * y * kBase;
}

void test0(){
    double d1 = 1.2, d2 = 1.2;
    cout << multiply(d1,d2) << endl;
    cout << multiply<int>(d1,d2) << endl;
}
```

> [!attention] 
> 
> 模板实例化的优先级规则如下：
> 
> + 显式实例化 => 隐式实例化 => 类型默认参数
> 
> 模板参数的默认值（不管是类型参数还是非类型参数）只有在没有足够的信息用于推导时起作用。当存在足够的信息时，编译器会按照实际参数的类型去调用，不会受到默认值的影响
> 

## 类模板

通过继承并不总是能够满足重用代码的需要。假设我们实现了一个 `Stack` 容器类保存 `int` 类型的元素。现在，需求变了，需要保存 `long` 类型的元素。

除了保存的对象类型不同外，这两种 `Stack` 类的代码是相同的。然而，与其编写新的类声明，不如编写一个 **泛型**（即独立于类型的）栈，然后将具体的类型作为参数传递给这个类。这样就可以使用通用的代码生成存储不同类型值的栈

> [!tip] 
> 
> 在 [[数据结构：栈和队列]] 中，我们通过 `typedef` 处理这种需求。然而，这种方法有两个缺点：
> + 首先，每次修改类型时都需要编辑头文件
> + 其次，在每个程序中只能使用这种技术生成一种栈，即不能让 `typedef` 同时代表两种不同的类型
> 
> 因此不能使用这种方法在同一个程序中同时定义 `int` 栈和 `long` 栈
> 

C++ 的类模板为生成通用的类声明提供了一种更好的方法。**模板提供参数化类型，即能够将类型名作为参数传递给接收方来建立类或函数**。例如，将类型名 `int` 传递给 `Queue` 模板，可以让编译器构造一个对 `int` 进行排队的 `Queue` 类

> [!tip] 
> 
> C++ 的标准库就提供了许多模板类。例如，`<vector>` `<string>` 等
> 

### 定义类模板

定义类模板和定义函数模板一样，都是使用 `template` 关键字。例如，我们声明一个 `Stack` 模板类

```cpp
template<typename T>
class Stack {
public:
	Stack();  // 默认构造函数
	explicit Stack(int siz); // 

private:
	T * _data;  // 指向一个存放 T 类型元素的动态内存
	int _top;
	int _siz;  
};
```

`template<typename T>` 告诉编译器将要定义一个模板，使用 `T` 泛指不同的类型。

如果个需要在类外部定义成员函数时，也需要使用 `template<typename T>` 的方式定义成员函数。例如

```cpp
template<typename T>
Stack<T>::Stack(): _data{nullptr}, _top{-1}, _siz{0}
{
}

template<typename T>
Stack<T>::~Stack()
{
    if(_data) {
        delete[] _data;
        _data = nullptr;
    }
}
```

> [!tip] 
> 
> 如果在类声明中定义了方法（内联定义），则可以省略模板前缀和类限定符
> 

模板并不是类和成员函数的定义，它们只是 C++ 的编译器指令，说明了如何生成类和成员函数的定义

> [!tip] 
> 
> 模板指示编译器如何生成类和函数的定义
> 

### 使用模板类

仅在程序包含模板并不能生成模板类，而必须请求实例化。为此，需要声明一个类型为模板类的对象，方法是使用所需的具体类型替换泛型名

例如，下面的代码创建两个栈，一个用于存储`int`，另一个用于存储 `string` 对象

```cpp
Stack<int> kernels;
Stack<string> colonels;
```

编译器看到上述声明后，将按 `Stack<T>` 模板来生成两个独立的类声明和两组独立的类方法

> [!attention] 
> 
> 请注意：必须显式地提供所需要的类型，这和函数模板是不同的。因为编译器可以根据调用函数是提供的参数推断类型
> 

## 可变模板参数

**可变模板参数** 是 C++11 新增的最强大的特性之一，它对参数进行了高度泛化，它能表示 $0$ 到任意个数、任意类型的参数。

> [!tip] 
> 
> 由于可变模版参数比较抽象，使用起来需要一定的技巧，所以它也是 C++11 中最难理解和掌握的特性之一
> 

可变参数模板和普通模板的语义是一样的，只是写法上稍有区别，声明可变参数模板时需要在 `typename` 或 `class` 后面带上省略号 `"..."`

```cpp
template <class ...Args>  
void func(Args ...args);
```

> [!important] 
> 
> **Args 叫做模板参数包**：里面包含的就是 `T1/T2/T3...` 这样的类型参数
> 
> **args 叫做函数参数包**：里面包含的就是函数的参数，分别对应类型 `T1/T2/T3...`
> 

我们在定义一个函数时，可能有很多个不同类型的参数，不适合一一写出，所以提供了可变模板参数的方法。

```cpp
template <class ...Args>//Args 模板参数包
void display(Args ...args)//args 函数参数包
{
    //输出模板参数包中类型参数个数
    cout << "sizeof...(Args) = " << sizeof...(Args) << endl;
    //输出函数参数包中参数的个数
    cout << "sizeof...(args) = " << sizeof...(args) << endl;
}

void test0(){
    display();
    display(1,"hello",3.3,true);
}
```

假设我们想要定义一个求和函数模板，该模板可以接收任意个参数

```cpp
#include <iostream>

// Base case for recursion
template <typename T>
T sum(T t) {
  return t;
}

// Variadic template
template <typename T, typename... Args>
T sum(T t, Args... args) {
  return t + sum(args...);
}

int main() {
  int result = sum(1, 2, 3, 4, 5);  // expands to 1 + 2 + 3 + 4 + 5
  std::cout << "The sum is: " << result << std::endl;

  return 0;
}
```

下面的示例给出了一个可变参数模板定义的元组类

```cpp
template <typename... Types>
class Tuple;

// Base case: empty tuple
template <>
class Tuple<> {};

// Recursive case: Tuple with one or more elements
template <typename Head, typename... Tail>
class Tuple<Head, Tail...> : public Tuple<Tail...> {
 public:
  Tuple(Head head, Tail... tail) : Tuple<Tail...>(tail...), head_(head) {}

  Head head() const { return head_; }

 private:
  Head head_;
};

int main() {
  Tuple<int, float, double> tuple(1, 2.0f, 3.0);
  std::cout << "First element: " << tuple.head() << std::endl;
  return 0;
}
```
