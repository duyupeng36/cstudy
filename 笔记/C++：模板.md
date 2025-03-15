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



