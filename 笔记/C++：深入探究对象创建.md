# 深入探究对象创建

在 [[C++：类与对象]] [[C++：构造与清理]] 中我们介绍了对象的创建与销毁依赖于构造函数和析构函数。然而，构造函数仅仅只是对已有内存空间的初始化

> [!tip] 
> 
> 由于在宿主环境中，程序只能使用虚拟内存。参考 [[Linux 系统编程：进程属性#进程内存布局]] 和 [[操作系统：虚拟内存#Linux 虚拟内存系统]] 
> 
> 对象可以存储在 **数据区域**，包括 已初始化数据区(`.data`) 和 未初始化数据区(`.bss`)；**堆区** 和 **栈区**
> 

程序中具有静态存储期的对象存储在数据区域，该区域中的对象占用存储空间在程序进入 `main()` 函数之前就已经分配好了，同时也会自动调用构造函数，当程序结束时才会调用析构函数。我们不考虑数据区域中的对象创建与销毁

## 堆区中对象的创建

我们知道在堆区中创建对象，需要首先使用 `new` 运算符分配内存，然后初始化堆内存。当对象不再使用时，首先是否对象持有的资源，然后使用 `delete` 销毁对象占用的空间

下面我们就探究堆对象的创建和销毁过程

### 堆对象的创建过程

当我们使用 `new` 运算符创建一个堆对象时，需要经过下面 $3$ 个步骤

> [!tip] 步骤一：调用 `operator new()` 在堆空间中分配未初始化的对象的内存空间
> 
> `operator new()` 本质就是使用库函数 `malloc()` 分配一个未初始化的堆空间
> 

> [!tip] 步骤二：调用构造函数初始化步骤一中获得堆空间
> 

> [!tip] 步骤三：返回已初始化对象的指针

编译器为每个类都提供一个默认的 `operator new()` 成员函数：只是简单的调用 `malloc()` 并返回

```cpp
void * operator new(size_t sz){
    void * ret = malloc(sz);
	return ret;
}
```

### 堆对象的释放执行过程

使用 `delete` 运算符销毁一个堆对象时，需要经过下面 $2$ 个步骤

> [!tip] 步骤一：调用析构函数，回收数据成员持有的资源
> 
> 对象可能持有堆内存、文件、锁等有限资源
> 

> [!tip] 步骤二：调用 `operator delete()` 销毁对象占用的堆空间
> 

编译器为每个类都提供默认的 `operator delete()` 成员函数：只是简单调用 `free()` 

```cpp
void operator delete(void * p){
    free(p);
}
```

通过为类提供 `operator new()` 和 `operator delete()` 运算符函数可以控制对象的内存分配规则。如果没有提供，则使用全局空间中的 `::operator new()` 和 `::operator delete()`

下面的代码演示了创建堆对象和销毁堆对象时自动调用的成员函数

```cpp
#include <iostream>

class Student {

public:
    Student() {
        std::cout << "Student()" << std::endl;
    }
    
    ~Student() {
        std::cout << "~Student()" << std::endl;
    }

    void *operator new(size_t size) {
        std::cout << "operator new" << std::endl;
        return malloc(size);
    }

    void operator delete(void *p) {
        std::cout << "operator delete" << std::endl;
        free(p);
    }
};


int main() {

    Student *s = new Student;  // 创建堆对象
    delete s;                  // 销毁堆对象
    return 0;
}
```

上述代码编译运行的结果为

```shell
➜  cppfiles g++ -std=c++11 -fno-elide-constructors  test.cpp -o test
➜  cppfiles ./test
operator new        # 申请空间
Student()           # 调用构造函数
~Student()          # 调用析构函数
operator delet      # 销毁空间
```

下面我们逐步演示 构造/析构函数 和 operator new()/operator delete() 如何控制堆对象的创建

> [!tip] 步骤一：将 构造/析构函数 和 operator new()/operator delete() 全部声明为 `private` 或 `=delete` 观察堆对象是否能够成功创建
> 

```cpp
#include <iostream>

class Student {

private:
    Student() {
        std::cout << "Student()" << std::endl;
    }
    
    ~Student() {
        std::cout << "~Student()" << std::endl;
    }

    void *operator new(size_t size) {
        std::cout << "operator new" << std::endl;
        return ::operator new(size);  // 调用全局的 operator new()
    }

    void operator delete(void *p) {
        std::cout << "operator delete" << std::endl;
        ::operator delete(p); // 调用全局的 operator delete()
    }
};


int main() {

    Student *p = new Student;
    delete p;
    return 0;
}
```

上述代码编译时编译器提供许多信息，我们观察第一个错误信息

```shell
➜  cppfiles g++ -std=c++11 -fno-elide-constructors  test.cpp -o test
test.cpp: In function ‘int main()’:
test.cpp:28:22: 错误：‘static void* Student::operator new(size_t)’ is private within this context
   28 |     Student *p = new Student;
      |                      ^~~~~~~
test.cpp:14:11: 附注：declared private here
   14 |     void *operator new(size_t size) {
      |           ^~~~~~~~
```

由于 `operator new()` 是 `private`，不能在类外调用，因此无法为类对象分配空间。对象无法创建的错误在编译时就能被发现，从而导致编译失败

> [!tip] 步骤二：将 `operator new()` 修改为 `public`，继续观察堆对象是否能完成创建
> 

```cpp
#include <iostream>


class Student {

public:
    void *operator new(size_t size) {
        std::cout << "operator new" << std::endl;
        return ::operator new(size);  // 调用全局的 operator new()
    }


private:
    Student() {
        std::cout << "Student()" << std::endl;
    }

    ~Student() {
        std::cout << "~Student()" << std::endl;
    }


    void operator delete(void *p) {
        std::cout << "operator delete" << std::endl;
        ::operator delete(p); // 调用全局的 operator delete()
    }
};


int main() {

    Student *p = new Student;
    delete p;
    return 0;
}
```

上述代码编译时编译器提供许多信息，我们观察第一个错误信息

```shell
➜  cppfiles g++ -std=c++11 -fno-elide-constructors  test.cpp -o test
test.cpp: In function ‘int main()’:
test.cpp:32:22: 错误：‘Student::Student()’ is private within this context
   32 |     Student *p = new Student;
      |                      ^~~~~~~
test.cpp:14:5: 附注：declared private here
   14 |     Student() {
      |     ^~~~~~~
```

这时编译器提供的错误信息表明构造函数是 `private`，无法在类外调用。同理，会导致编译错误

> [!tip] 步骤三：将构造函数修改为 `public` 的，观察堆对象是否能成功创建
> 

```cpp
#include <iostream>


class Student {

public:
    void *operator new(size_t size) {
        std::cout << "operator new" << std::endl;
        return ::operator new(size);  // 调用全局的 operator new()
    }

    Student() {
        std::cout << "Student()" << std::endl;
    }

private:

    ~Student() {
        std::cout << "~Student()" << std::endl;
    }


    void operator delete(void *p) {
        std::cout << "operator delete" << std::endl;
        ::operator delete(p); // 调用全局的 operator delete()
    }
};


int main() {

    Student *p = new Student;
    delete p;
    return 0;
}
```

上述代码编译时编译器提供许多信息，我们观察第一个错误信息

```shell
➜  cppfiles g++ -std=c++11 -fno-elide-constructors  test.cpp -o test
test.cpp: In function ‘int main()’:
test.cpp:32:22: 错误：‘static void Student::operator delete(void*)’ is private within this context
   32 |     Student *p = new Student;
      |                      ^~~~~~~
test.cpp:23:10: 附注：declared private here
   23 |     void operator delete(void *p) {
      |          ^~~~~~~~
```

编译器提示我们 `operator delete()` 是 `private`，同理，这是导致编译错误的原因

> [!tip] 步骤四：将 `operator delete()` 修改为 `public` ，观察堆对象是否能够完成创建

```cpp
#include <iostream>


class Student {

public:
    void *operator new(size_t size) {
        std::cout << "operator new" << std::endl;
        return ::operator new(size);  // 调用全局的 operator new()
    }

    Student() {
        std::cout << "Student()" << std::endl;
    }

    void operator delete(void *p) {
        std::cout << "operator delete" << std::endl;
        ::operator delete(p); // 调用全局的 operator delete()
    }
private:

    ~Student() {
        std::cout << "~Student()" << std::endl;
    }


};


int main() {

    Student *p = new Student;
    delete p;
    return 0;
}
```

编译上述代码，编译器只提供了一个错误信息

```shell
➜  cppfiles g++ -std=c++11 -fno-elide-constructors  test.cpp -o test
test.cpp: In function ‘int main()’:
test.cpp:33:12: 错误：‘Student::~Student()’ is private within this context
   33 |     delete p;
      |            ^
test.cpp:22:5: 附注：declared private here
   22 |     ~Student() {
      |     ^
```

这个错误信息表面析构函数是 `private` 的，不能在类外调用。显然这时候，我们已经可以完成堆对象的创建了。只要我们不使用 `delete` 销毁对象

> [!summary] 
> 
> 堆对象的创建离不开 _构造函数_ 和  _operator new()_；堆对象的销毁离不开 _析构函数_ 和 _operator delete()_
> 
> 对于析构函数，在创建堆对象没有要求，只有在销毁的时候才需要调用
> 

## 栈区中对象的创建

在调用函数时该函数的栈帧会自动完成分配。所以，在存储在栈区中的对象不需要使用 `new` 运算符分配空间，同时也不需要使用 `delete` 运算符释放空间。对象可能需要持有资源，因此构造函数和析构函数是必须的

下面我们逐步演示 构造/析构函数 和 operator new()/operator delete() 如何控制堆对象的创建

> [!tip] 步骤一：将 构造/析构函数 和 operator new()/operator delete() 全部声明为 `private` 或 `=delete` 观察栈对象是否能够成功创建
> 

```cpp
#include <iostream>


class Student {

private:
    Student() {
        std::cout << "Student()" << std::endl;
    }

    ~Student() {
        std::cout << "~Student()" << std::endl;
    }

    void *operator new(size_t size) {
        std::cout << "operator new" << std::endl;
        return ::operator new(size);  // 调用全局的 operator new()
    }

    void operator delete(void *p) {
        std::cout << "operator delete" << std::endl;
        ::operator delete(p); // 调用全局的 operator delete()
    }

};


int main() {

    Student stu;
    return 0;
}
```

编译上述代码，编译器给出了两个错误提示，观察第一个错误

```shell
➜  cppfiles g++ -std=c++11 -fno-elide-constructors  test.cpp -o test
test.cpp: In function ‘int main()’:
test.cpp:30:13: 错误：‘Student::Student()’ is private within this context
   30 |     Student stu;
      |             ^~~
test.cpp:7:5: 附注：declared private here
    7 |     Student() {
      |     ^~~~~~~
```

该错误表明构造函数是 `private` 的，无法在类外调用，从而导致对象初始化失败。在编译阶段就能发现对象初始化失败的逻辑，从而导致编译错误

> [!tip] 步骤二：将构造函数修改为 `public`，观察栈对象是否能成功创建

```cpp
#include <iostream>


class Student {
public:
    Student() {
        std::cout << "Student()" << std::endl;
    }
private:

    ~Student() {
        std::cout << "~Student()" << std::endl;
    }

    void *operator new(size_t size) {
        std::cout << "operator new" << std::endl;
        return ::operator new(size);  // 调用全局的 operator new()
    }

    void operator delete(void *p) {
        std::cout << "operator delete" << std::endl;
        ::operator delete(p); // 调用全局的 operator delete()
    }

};


int main() {

    Student stu;
    return 0;
}
```

编译上述代码，编译器提示了一个错误信息

```shell
➜  cppfiles g++ -std=c++11 -fno-elide-constructors  test.cpp -o test
test.cpp: In function ‘int main()’:
test.cpp:30:13: 错误：‘Student::~Student()’ is private within this context
   30 |     Student stu;
      |             ^~~
test.cpp:11:5: 附注：declared private here
   11 |     ~Student() {
      |     ^
```

这个错误信息表面析构函数是 `private`，无法在类外调用。从而导致栈对象持有的资源无法回收，编译器禁止这样的栈对象被创建

> [!tip] 步骤三：将析构函数修改为 `public`，观察栈对象是否能成功创建
> 

```cpp
#include <iostream>


class Student {
public:
    Student() {
        std::cout << "Student()" << std::endl;
    }

    ~Student() {
        std::cout << "~Student()" << std::endl;
    }
    
private:
    void *operator new(size_t size) {
        std::cout << "operator new" << std::endl;
        return ::operator new(size);  // 调用全局的 operator new()
    }

    void operator delete(void *p) {
        std::cout << "operator delete" << std::endl;
        ::operator delete(p); // 调用全局的 operator delete()
    }

};


int main() {

    Student stu;
    return 0;
}
```

编译上述代码，编译器未提示任何错误

```shell
➜  cppfiles g++ -std=c++11 -fno-elide-constructors  test.cpp -o test
➜  cppfiles 
```

> [!summary] 
> 
> 栈对象的创建和销毁与 _operator new()_ 和 _operator delete()_ 无关，只需要构造函数和析构函数即可
> 
> 注意：如果没有析构函数，则不允许栈对象而创建
> 
