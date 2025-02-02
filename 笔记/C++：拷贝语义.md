# 拷贝语义

当我们需要从对象 `b` 传输一个值到对象 `a`，通常有两种逻辑上不同的方法：**拷贝** 和 **移动**

> [!tip] 拷贝
> 
> 拷贝是 `a = b` 的常规含义：结果是 `a` 和 `b` 的之都等于赋值前的 `b` 的值
> 
> 拷贝是默认的。无论是内置类型还是用户自定义类型，默认就是可拷贝的对象
> 

> [!tip] 移动
> 
> 移动是将 `a`  变为 `b` 的旧值，而 `b` 变为某种 **移出状态**
> 
> 对于那些不再使用的对象，通常就会将值移动到别处，而不是复制
> 

下面我们开始介绍 **拷贝** 相关的细节，在 [[C++：移动语义]] 中介绍移动相关的细节

## 拷贝构造函数

在 [[C++：类与对象#默认拷贝]] 中我们知道用户自定义类型的对象也是可以拷贝的。但是，默认情形下，拷贝是逐成员拷贝的。有时，默认的拷贝语义不是我们期望的。

如下定义的 `Person` 类

```cpp
class Person {

    char *_name {nullptr};         // 姓名
    int _age {0};                  // 年龄
    unsigned short _phone[11]{0};  // 电话号码

public:
    // 构造函数
    Person(const char *name, int age, const unsigned short *phone=nullptr);
    // 析构函数
    ~Person();
    void show();
};

void f() {
	Person p1{"张三", 20, (unsigned short[]){1,3,5,2,7,0,8,1,9,4,4}}
	Person p2 = p1;
} 
```

下图展示了这种默认的拷贝语义：`p1` 和 `p2` 的 `_name` 成员都指向了同一片内存区域(**浅拷贝**)。这显然不是我们希望的语义。

![[Pasted image 20250130000424.png]]

> [!warning] 
> 
> 在上述代码中，当函数 `f()` 返回时，会释放同一片内存区域两次，从而导致 `double free` 错误
> 

因此，我们可以为 `Person` 类提供一个 **拷贝构造函数**，并重新定义拷贝的语义

```cpp
class Person {
    char *_name {nullptr};         // 姓名
    int _age {0};                  // 年龄
    unsigned short _phone[11]{0};  // 电话号码

public:
	// ...
    // 拷贝构造函数
    Person(const Person &p);
    // ...
};
```

在拷贝构造函数中，为 `_name` 重新分配一片内存空间，然后从 `p` 中拷贝到新分配的空间中(**深拷贝**)

```cpp
Person::Person(const Person &p)
: _name{new char[strlen(p._name) + 1]}, _age{p._age}, _phone{0}
{
    if(_name == nullptr) {
        throw std::bad_alloc();
    }
    strcpy(_name, p._name);

    for (int i = 0; i < 11; i++) {
        _phone[i] = p._phone[i];
    }
    std::cout << "拷贝构造函数" << std::endl;
}
```

拷贝构造函数 `Person(const Person &p)` 覆盖了默认的拷贝的语义，这里不再是逐成员复制，而是会赋值指针指向的内容

```cpp
void f() {
	Person p1{"张三", 20, (unsigned short[]){1,3,5,2,7,0,8,1,9,4,4}}
	Person p2 = p1;
} 
```

下图展示了构造函数 `Person(const Person &p)` 的语义。`_name` 指向的内存也被复制了一份

![[Pasted image 20250202145201.png]]

关于拷贝构造函数，还需要深入理解下面几个问题

> [!question] 拷贝构造函数的参数是否必须是引用？
> 
> 如果为类 `X` 定义拷贝构造函数，那么拷贝构造函数形式一定不能是 `X(const X)` 的形式。因为，在执行拷贝操作时，由于不是引用就会导致实参对象被拷贝到形参中，从而再次调用拷贝构造函数。因此，会发生无穷的递归调用，导致栈空间被耗尽
> 

> [!question] 拷贝构造函数的参数是否必须为 `const` 左值引用？
> 
> 当为类 `X` 定义拷贝构造函数是，拷贝构造函数的形式可以是 `X(X&)`，编译器不会报错。然而，这样会限制我们必须使用一个类 `X` 的左值。因为非 `const` 左值引用不能绑定临时变量
> 
> 在 [[C++：与 C 的差异#引用]] 中我们介绍过 `const` 引用可以绑定临时变量
> 
> `const` 还可以限制拷贝构造函数中修改参数的值
> 

```
➜  cppfiles g++ -std=c++11 -fno-elide-constructors  main.cpp person.cpp -I.
main.cpp: In function ‘int main()’:
main.cpp:14:17: 错误：cannot bind non-const lvalue reference of type ‘Person&’ to an rvalue of type ‘Person’
   14 |     Person p3 = Person{"李四", 30, (const unsigned short[]){1, 3, 5, 7, 9, 0, 2, 4, 6, 8, 0}};  // 调用普通构造函数，然后调用拷贝构造函数
      |                 ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
In file included from main.cpp:5:
person.hpp:13:20: 附注：  初始化‘Person::Person(Person&)’的实参 1
   13 |     Person(Person &p);
      |            ~~~~~~~~^
```

## 拷贝赋值运算符

赋值的默认语义是浅拷贝(逐成员赋值)。对于含有指针成员的对象，浅拷贝往往意味着 `double free` 的错误。因此，我们需要重新定义赋值运算符，实现深拷贝

拷贝构造函数是用于初始化一片未初始化的内存空间；而拷贝赋值运算符是用于正确更新一片已经初始化好的内存空间。例如，可以为 `Person` 类提供拷贝赋值运算符

```cpp
class Person {
    char *_name {nullptr};         // 姓名
    int _age {0};                  // 年龄
    unsigned short _phone[11]{0};  // 电话号码

public:
	// ...
    // 拷贝赋值运算符
    Person& operator=(const Person &p);
    // ...
};
```

要实现拷贝赋值运算符，就需要正确处理已经初始化的内存，然后将对象的值拷贝到被赋值的对象

```cpp
Person& Person::operator=(const Person &p)
{
	// 检查是否是本对象复制给本对象
    if (this == &p) {
        return *this;
    }
    
    // 销毁已有空间
    if (_name) {
        delete[] _name;
    }
    // 从新获取资源
    _name = new char[strlen(p._name) + 1];
    if(_name == nullptr) {
        throw std::bad_alloc();
    }
    // 拷贝到新的位置
    strcpy(_name, p._name);

    _age = p._age;
    for (int i = 0; i < 11; i++) {
        _phone[i] = p._phone[i];
    }
    std::cout << "拷贝赋值运算符" << std::endl;
    return *this;
}
```

> [!important] 
> 
> 拷贝赋值运算符函数的实现分 $4$ 步执行
> + 首先，考虑复制本身
> + 然后，释放左操作数持有的资源
> + 再然后，执行深拷贝
> + 最后，返回 `*this`
> 

和拷贝构造函数一样，通过下面几个问题就能深入理解拷贝赋值运算符

> [!question]  赋值运算符函数的返回必须是一个引用吗？
> 
> 假设为类 `Computer` 定义拷贝赋值运算符，那么拷贝赋值运算符可以不返回引用，而是返回一个对象，即 `Computer Computer(const Computer&)` 
> 
> 编译器允许这样的拷贝赋值运算符函数，然而会造成一次拷贝，增加不必要的开销
> 

```cpp
Computer operator=(const Computer & rhs)
{
    // ...
    return *this;
}
```

> [!question] 赋值操作符函数的返回类型可以是 `void` 吗？
> 
> 可以，但是会导致连续赋值失效。因为赋值运算符默认会返回左操作数的值作为赋值表达式的值。如果自定义 `operator=` 返回类型为 `void` 会导致连续赋值失效

> [!question] 赋值操作符函数的参数一定要是引用吗？
> 
> 与拷贝构造函数类似
> 

> [!question] 赋值操作符函数的参数必须是一个 `const` 引用吗？
> 
> 与拷贝构造函数类似
> 

> [!important] 三合成原则
> 
> **拷贝构造函数、赋值运算符函数、析构函数，如果需要手动定义其中的一个，那么另外两个也需要手动定义。**
> 

## 拷贝的含义

拷贝必须满足两个准则：**等价性** 和 **独立性**

> [!tip] **等价性**：在 `x = y` 之后，对 `x` 和 `y` 执行相同的操作应该得到相同的结果
> 
> 如果它们的类型定义了 `==`，应该有 `x == y`，并且对任何 **只依赖于 `x` 和 `y` 的值** 的函数 `f()`，有 `f(x) == f(y)`
> 

> [!tip] **独立性**：在 `x = y` 之后，对 `x` 的操作不会隐式的改变 `y` 的状态
> 
> 即，只要 `f(x)` 未引用 `y`，它就不会改变 `y` 的值
> 

**等价性很少有人故意违背**，而且默认拷贝操作不会违反这一要求，因为 **默认拷贝操作执行逐成员拷贝**。但是，人们偶尔会使用一些小花招，令拷贝的含义依赖于不同的选项，这通常会导致混乱。而且，对象包含的成员不被看作对象值的一部分的情况并不罕见。例如，拷贝一个标准容器时不拷贝其分配器，因为分配器被认为是容器的一部分，但不是容器值的一部分。类似地，用于统计搜集和缓存值的计数器有时也不是简单拷贝的

**大多数与独立性相关的问题都涉及包含指针的对象**。默认拷贝语义是逐成员拷贝，因此一个**默认拷贝操作会拷贝指针成员，但不会拷贝指针指向的对象**。例如

```cpp
struct S
{
    int *p;
};

S x {new int{0}};

void f()
{
    S y {x};      // 拷贝构造
    *x.p = 1;
    *y.p = 2;
	// x.p 和 y.p 指向同一个对象
    // *x.p == *y.p == 2
    delete y.p; // 影响了 x 和 y
    y.p = new int{3}; // 影响了 x 和 y
    *x.p = 4;  // 错误：写入已经被释放的内存
}
```

> [!tip] 
> 
> 上述例程违反了独立性原则。将 `x` 拷贝到 `y` 后，我们通过 `y` 操纵 `x` 的部分状态。
> 

有时候，这种 **拷贝对象的部分状态** 的方式称为 **浅拷贝**，人们经常过度赞扬浅拷贝在效率方面的优势。一个明显的替代方法是 **拷贝对象的完整状态**，称为 **深拷贝**。通常，**比深拷贝更好的替代方法不是浅拷贝，而是移动操作**，它能最小化拷贝量而又不会增加复杂性

一次浅拷贝会令两个对象(本例中的 `x` 和 `y`)进入 **共享状态**，这会带来潜在的混乱和错误。如果违反了独立性要求，我们称对象 `x` 和 `y` **纠缠** 在一起。孤立地分析一个对象是否纠缠是不可能的。例如，从源码中很难分析出 `*x.p` 的两次赋值的效果有什么显著不同。下图展示了纠缠对象示例

![[Pasted image 20250202182153.png]]

> [!tip] 
> 
> 很多情况都能引发纠缠。通常，直到问题爆发时才能发现发生了纠缠现象
> 

对于与共享子对象生命周期相关的问题，我们可以通过引入某种形式的 **垃圾收集机制** 来解决

```cpp
struct S
{
    // int *p;
    shared_ptr<int> p; // 垃圾收集机制
};

S x {std::shared_ptr<int>(new int{0})};

void f()
{
    S y {x}; // 拷贝 x
    *y.p = 1;  // 改变 y，影响 x
    *x.p = 2;  // 改变 x，影响 y

    y.p.reset(new int{3}); // 改变 y，影响 x
    *x.p = 4;  // 改变 x，影响 y
}
```

> [!tip] 
> 
> `shared_ptr` 是 C++ 提供的智能指针，其中维护了一个计数器，每次复制 `shared_ptr` 的对象都会导致计数器增加。只有当计数器为 $0$，才会释放指针指向的内容
> 

**浅拷贝和对象纠缠都是导致需要垃圾收集的原因**。如果没有某种形式的垃圾收集器(本例的 `shared_ptr`)，对象纠缠会导致代码非常难以管理。这里由浅拷贝引起的对象纠缠是复杂性和错误之源，最好的情况也只能通过垃圾收集器部分解决

> [!important] 
> 
> 不可变的共享状态不存在任何问题。除非我们比较地址，否则不可能判断两个相同的值是同一个对象还是两个不同的对象
> 

基于不可变的共享状态不存在任何访问方面的问题，就催生了 **写时复制(copy-on-write)** 的概念

> [!tip] 写时复制
> 
> 在共享状态被修改之前，副本其实并不真的需要独立性，因此可以推迟共享状态的拷贝，直到**首次修改副本前才真正进行拷贝**
> 

考虑下面的代码处理图像的代码

```cpp
class Image
{
public:
	//...
	Image(const Image& ); // 拷贝构造函数
	// ...
	void write_block(Descriptor); 
private:
	Representation * clone(); // 拷贝 rep
	Representation * rep;
	bool shared;
}
```

 一个 `Representation` 可能很大。因此，与检测 `bool` 值相比，`write_block()` 会很耗时。这样，依赖于 `Image` 的使用，将拷贝构造函数实现为浅拷贝就很有意义的

```cpp
Image::Image(const Image& im): rep{im.rep}, shared{true}
{
}  // 浅拷贝。只复制对象的部分状态
```

为了保护传递给拷贝构造函数的参数，我们在修改它之前拷贝 `Representation`

```cpp
void Image::write_block(Descriptor d)
{
	if (shared)  // 处于贡献状态，则拷贝
	{
		rep = clone(); // 拷贝
		shared = false;
	}
	// 现在可以安全的修改我们自己的 rep 副本
}
```
