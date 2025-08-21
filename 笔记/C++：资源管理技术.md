# 资源管理技术

## RAII

程序中使用的某些资源，例如：文件、堆空间、锁等，在使用完成之后就应该立即释放。然而，由于程序变得复杂，可能会导致我们忘记释放或者多次释放这些资源。

回想一下，在 [[C++：类与对象]] 中介绍的 **构造函数** 和 **析构函数**；以及在 [[C++：单例对象自动释放]]。这些就是我们将要介绍的 RAII(Resource Acquisition Is Initialization，**资源获取即初始化**) 的基础

> [!tip] 
> 
> RAII 是由 C++ 的发明者 Bjarne Stroustrup 提出。本质上就是利用类对象的生命周期管理资源，因为类对象在其生命周期结束时会自动调用析构函数，从而销毁对象持有的资源
> 

既然是要让对象管理资源，那么就需要让对象满足一些特征

> [!tip] 特征一：禁止拷贝
> 
> 对象持有的资源应该让对象独享：例如，如果对象持有堆空间的指针，允许拷贝会导致 `double free` 异常
> 

> [!tip] 特征二：初始化时获取资源
> 
> 对象应该在初始化的时候获得资源，而不是初始化完成后再获取资源
> 

> [!tip] 特征三：代理资源的访问
> 
> 对象应该提供资源的访问方法
> 

## 智能指针

智能指针是就一种典型的 RAII 的应用。在 [[C++：特殊运算符]] 中我们已经介绍了如何实现一个智能指针。下面我们介绍 C++ 为我们提供的三种智能指针

### unique_ptr

`unique_ptr` 是 C++ 标准库提供的用于管理单个对象或者数组的模板类。正如它的名字 `unique` 一样，`unique_ptr` 对象持有的资源被该对象独享，即独占资源的所有权。

> [!tip] 
> 
> 换句话说，就是同一个资源只能被一个 `unique_ptr` 对象持有。
> 
> 也就是说，`unique_ptr` 对象 **禁止拷贝** 的。但是，允许对象交出资源的所有权，即 **允许移动**
> 

C++ 提供了两种创建 `unique_ptr` 对象的方式

```cpp
#include <iostream>
#include <memory>

int main() {
    std::unique_ptr<int> p1(new int(5)); 
    // Initialize with pointer to a new integer
    std::unique_ptr<int> p2 = std::make_unique<int>(10); 
    // Preferred method (C++14 onwards)

    std::cout << *p1 << ", " << *p2 << std::endl;
    return 0;
}
```

> [!attention] 
> 
> 请注意：现代 C++ 要求我们不显式使用 `new` 和 `delete`，因此推荐使用 `std::make_unique()` 模板函数创建 `unique_ptr` 对象
> 

如果需要 `unique_ptr` 对象交出其持有的资源的所有权，请使用 `std::move()` 函数

```cpp
#include <iostream>
#include <memory>

int main() {
    std::unique_ptr<int> p1(new int(5));

    std::unique_ptr<int> p2 = std::move(p1); 
    // Ownership is transferred from p1 to p2

    if (p1) {
        std::cout << "p1 owns the object" << std::endl;
    } else if (p2) {
        std::cout << "p2 owns the object" << std::endl;
    }

    return 0;
}
```

### shared_ptr

`unique_ptr` 会独享资源的所有权，但是在某些场景下需要共享资源的所有权。C++ 提供了另一个智能指针 `shared_ptr` 

`shared_ptr` 对象之间允许复制获取另一个对象持有资源的所有权。复制可能会导致 `double free` 的错误，为了解决这个问题，`shared_ptr` 引入了一个引用计数。

> [!tip] 
> 
> 关于引用计数的概念，参考 [[C++：string 的底层实现]]
> 

```cpp
#include <iostream>
#include <memory>

class MyClass {
public:
    MyClass() { std::cout << "Constructor is called." << std::endl; }
    ~MyClass() { std::cout << "Destructor is called." << std::endl; }
};

int main() {
    // shared 指针管理一个 MyClass 对象
    std::shared_ptr<MyClass> ptr1(new MyClass());
    {
        // shared_ptr 拷贝构造。引用计数 +1
        std::shared_ptr<MyClass> ptr2 = ptr1;
        std::cout << "Inside the inner scope." << std::endl;
    }
    // 离开上述局部作用域：ptr2 被销毁，引用技术 -1
    std::cout << "Outside the inner scope." << std::endl;
    return 0;
    // 程序结束：ptr1 被销毁，对象被正式销毁
}
```

#### shared_ptr 的局限性：循环引用

当我们有两个对象相互通过 `shared_ptr` 持有对方的指针时，就形成了循环引用。例如

```cpp
#include <iostream>
#include <memory>

using namespace std;

class Child;

class Parent
{
public:
    Parent()
    {
        cout << "Parent()" << endl;
    }
    ~Parent()
    {
        cout << "~Parent()" << endl;
    }
    // 只需要Child类型的指针，不需要类的完整定义
    shared_ptr<Child> spChild;
};

class Child
{
public:
    Child()
    {
        cout << "child()" << endl;
    }
    ~Child()
    {
        cout << "~child()" << endl;
    }
    shared_ptr<Parent> spParent;
};

void f() {
    shared_ptr<Parent> parent{new Parent};
    shared_ptr<Child> child{new Child};

    parent->spChild = child;
    child->spParent = parent;
}

int main() {

    f();

    return 0;
}
```

编译运行上述代码

```shell
➜  cppfiles g++ main.cpp             
➜  cppfiles ./a.out 
Parent()
child()
```

我们发现当 `f()` 返回后 `Parent` 和 `Child` 的析构函数并没有被调用。下图说明了原因

![Pasted image 20250320222642|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755785810414-75db6ea904a54d26aaca0c505ead4048.png)

当函数 `f` 返回后，其栈帧被销毁，然而此时堆上的对象相互通过  `shared_ptr` 持有对方的指针，从而导致对象永远不会被销毁(`shared_ptr` 的引用计数永远不会为 $0$)

![Pasted image 20250320222744|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755785810414-bb9bb6e3220d42e6821fe1e1a4eda6ef.png)

### weak_ptr

为了解决 `shared_ptr` 带来的问题，C++ 提供了弱引用指针，即 `weak_ptr`。它不会增加其指向对象的引用计数。这是 `weak_ptr` 和 `shared_ptr` 之间的关键区别。

与对象关联的控制块维持两个计数：一个用于 `shared_ptr` 的数量（所有权计数），另一个用于 `weak_ptr` 的数量（弱计数）。`weak_ptr` 的存在并不能阻止对象被删除；一旦拥有它的最后一个 `shared_ptr` 被销毁，该对象就会被销毁，即使 `weak_ptr` 仍在引用对象。但是，直到所有权计数达到零，并且弱计数也达到零，允许 `weak_ptr` 可以安全地检测对象是否已经删除

> [!attention] 
> 
> 要使用 `weak_ptr` 必须使用其 `lock()` 方法将其转换为 `share_ptr`。
> 
> `lock()` 方法试图创建一个共享对象所有权的全新 `shared_ptr` 对象。如果成功，则增加对象的所有权引用计数，并返回 `shared_ptr` 对象
> 

```cpp
#include <iostream>
#include <memory>

class MyClass {
public:
    void DoSomething() {
        std::cout << "Doing something...\n";
    }
};

int main() {
    std::weak_ptr<MyClass> weak;

    {
        std::shared_ptr<MyClass> shared = std::make_shared<MyClass>();
        weak = shared;

        if (auto sharedFromWeak = weak.lock()) {
            sharedFromWeak->DoSomething(); // Safely use the object
            std::cout << "Shared uses count: " << sharedFromWeak.use_count() << '\n'; // 2
        }
    }

    // shared goes out of scope and the MyClass object is destroyed

    if (auto sharedFromWeak = weak.lock()) {
        // This block will not be executed because the object is destroyed
    }
    else {
        std::cout << "Object has been destroyed\n";
    }

    return 0;
}
```

### 删除器

C++ 提供的智能指针默认是管理堆内存的，因此它使用的是 `delete` 删除智能指针持有的资源。

然而，如果智能指针管理的是 `FILE` 类型的指针呢？使用 `delete` 显然是不合适，我们应该为智能指针定制删除器。

翻阅 [C++ 参考文档](https://zh.cppreference.com/w/%E9%A6%96%E9%A1%B5)，`unique_ptr` 的声明如下

```cpp
template<
	class T,
	class Deleter = std::default_delete<T>
>
class unique_ptr;

template<
	class T,
	class Deleter
>
class unique_ptr<T, Deleter>;
```

`unique_ptr` 提供了一个默认的删除器 `default_delete<T>`。下面我们看一下 `default_delete<T>` 的声明

```cpp
template< class T > struct default_delete;
template< class T > struct default_delete<T[]>;
```

`default_delete` 是一个 `struct`，提供了构造函数和 `operator()()` 运算符函数。

下面的示例代码给出了如何定义和使用一个删除器

```cpp
#include <iostream>
#include <memory>

using namespace std;


struct FileDeleter {
    void operator()(FILE * file) {
        printf("关闭文件\n");
        fclose(file);
    }
};


int main() {

    FILE *f = fopen("a.out", "rb");
    unique_ptr<FILE, FileDeleter> fptr{f};
    return 0;
}
```

---

对于 `shared_ptr` 而言，也可以指定自定义删除器。然而，`shared_ptr` 指定删除器并不是通过模板参数指定的，而在构造函数中提供一个删除器对象

```cpp hl:18
#include <iostream>
#include <memory>

using namespace std;


struct FileDeleter {
    void operator()(FILE * file) {
        printf("关闭文件\n");
        fclose(file);
    }
};


int main() {

    FILE *f = fopen("a.out", "rb");
    shared_ptr<FILE> fptr{f, FileDeleter{}};
    return 0;

}
```
