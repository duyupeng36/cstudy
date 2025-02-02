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
> 
> `const` 还可以限制拷贝构造函数中修改参数的值
> 

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
