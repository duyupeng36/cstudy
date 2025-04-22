# 单例模式

单例模式是 $23$ 种常用设计模式之一，它提供了一种确保 **只创建一个对象** 的对象创建方式

> [!tip] 
> 
> 换句话说就是，一个类只能实例化一个对象
> 

使用单例模式主要有 $2$ 个原因：**节省资源** 和 **简单方便的控制**

> [!tip] 节省资源
> 
> 一个类只有一个实例，不存在多份实例。对于那些复杂的大对象，整个系统中只有一份对象可以节省大量的资源
> 

> [!tip] 简单方便的控制
> 
> 在操作公共资源的场景时，可以避免多个对象引起的复杂
> 

在多线程环境中，实现单例模式需要充分考虑线程安全的问题。由于多个线程可能同时去创建单例对象，因此需要保证只有一个线程创建对象。

> [!seealso] 
> 
> 线程安全参考 [[Linux 系统编程：互斥量]] [[Linux 系统编程：条件变量]] [[Linux 系统编程：线程安全]] 
> 

## 单例模式的实现思路

单例模式只允许一个类创建一个对象，因此，我们 **不能提供构造函数**。同时，还需要 **静止对象的拷贝**；如果允许对象的拷贝，就会出现多个对象。由于不知道程序何时才不会使用该类的对象，因此还需要 **保证类对象创建出来之后就一直存在** 

> [!tip] 不提供构造函数
> 
> 可以将构造函数声明为 `private` 隐藏构造函数。对于不需要的构造函数，可以声明为 `=delete` 的
> 

> [!tip] 禁止对象的拷贝
> 
> 拷贝构造函数和拷贝赋值运算符函数必须是 `private` 或者 `=delete` 
> 

> [!tip] 保证类对象在创建出来之后就一直存在
> 
> 类对象应该在创建出来之后就需要一直存在，直到程序结束。这里考虑使用 **静态对象** 或者 **堆对象**
> 
> 静态空间中的对象可以保证存活到程序结束。而 堆对象只要没有使用 `delete` 运算符，该对象就不会被释放
> 

要实现单例模式，就需要同时保证上述 $3$ 个条件。缺少任何一个都会导致单例模式失效。单例模式有两种实现方式：**懒惰式** 和 **抢先式**

> [!tip] 懒惰式：也称懒汉式
> 
> 当程序需要的适合才会去创建对象。然而，懒汉式在多线程模式下存在竞争的问题，需要使用线程同步技术保证只有一个线程创建对象
> 

> [!tip] 抢先式：也称饿汉式
> 
> 当程序启动其他线程之前就创建好对象，当程序需要使用对象时直接读取即可。饿汉式会先创建好对象，在没有写操作的前提下，读取操作不会有任何竞争问题
> 

## 懒汉式单例模式

### 线程不安全的单例模式(堆对象)

由于将类的构造函数声明为 `private` 的。因此，我们需要自行提供一个创建类对象的静态成员函数 `getInstance()`。由于还需要使用一个变量记录已经创建的类对象，因此还需要一个静态成员 `instance`

由于静态成员 `instance` 在多线程的环境下就共享资源，可能存在竞争风险

```cpp
#include <iostream>
#include <pthread.h>


using namespace std;

class Singleton {
public:
    static Singleton& getInstance() {
        if (instance == nullptr) {
	        // 线程刚执行到此，可能时间片到了被内核中断
	        // 其他线程此时检查 instance 依旧为 nullptr,从而导致多个线程都会修改静态成员，造成竞争条件
            instance = new Singleton();
        }
        return *instance;  // 返回引用避免复制
    }
private:
    Singleton() = default;  // 生成默认构造函数，但是是私有的，外部无法调用
    // 禁止拷贝构造函数和拷贝赋值操作符
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
    // 禁止移动构造函数和移动赋值操作符
    Singleton(Singleton&&) = delete;
    Singleton& operator=(Singleton&&) = delete;

private:
	static Singleton* instance; // 声明类的静态成员：指向类的唯一实例
};

// 共享资源
Singleton* Singleton::instance = nullptr;  // 类的仅仅是声明，在类外部定义

void * routine(void *arg) {
    Singleton& instance = Singleton::getInstance();
    printf("thread[%ld] get instance at %p\n", pthread_self(), &instance);
    return nullptr;
}

#ifndef THREAD_NUM
#define THREAD_NUM 100
#endif


int main() {

    setvbuf(stdout, nullptr, _IONBF, 0);

    pthread_t tids[THREAD_NUM];
    for (int i = 0; i < THREAD_NUM; i++) {
        pthread_create(&tids[i], nullptr, routine, nullptr);
    }

    for (int i = 0; i < THREAD_NUM; i++) {
        pthread_join(tids[i], nullptr);
    }
    return 0;
}
```

现在编译上述代码，查看运行结果

```shell
➜  cppfiles g++ -std=c++11 -fno-elide-constructors -DTHREAD_NUM=200  test.cpp -o test
➜  cppfiles ./test > test.txt
```

查看 `test.txt` 文件中的内容，我们发现 `0x746d1c000b70` 出现 $2$ 次 `0x746d14000b70` 出现 $198$ 次。显然，因为竞争的关系，从而导致多次创建对象

### 线程安全的单例模式(堆对象)

> [!tip] 
> 
> 可以使用 [[互斥锁]] 实现线程同步
> 

下面的代码就是使用互斥锁实现线程同步

```cpp hl:16
#include <iostream>
#include <pthread.h>
#include <mutex>


using namespace std;

class Singleton {
public:
    static Singleton& getInstance() {
        if (instance == nullptr) {
            pthread_mutex_lock(&mutex);
            // 还须要再次判断，因为可能有多个线程同时通过第一个if
            if(instance == nullptr)
            {
                instance = new Singleton;
                // 第一步：创建内存空间
                // 第二步：初始化内存空间
                // 第三步：将内存空间的地址赋值给 instance
            }
            pthread_mutex_unlock(&mutex);
        }
        return *instance;  // 返回引用避免复制
    }
private:
    Singleton() = default;  // 生成默认构造函数，但是是私有的，外部无法调用
    // 禁止拷贝构造函数和拷贝赋值操作符
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
    // 禁止移动构造函数和移动赋值操作符
    Singleton(Singleton&&) = delete;
    Singleton& operator=(Singleton&&) = delete;

private:
    static Singleton* instance;  // 声明类的静态成员：指向类的唯一实例
    static pthread_mutex_t mutex;  // 声明类的静态成员：互斥锁
};

Singleton* Singleton::instance = nullptr;  // 类的仅仅是声明，在类外部定义
pthread_mutex_t Singleton::mutex = PTHREAD_MUTEX_INITIALIZER;  // 类的仅仅是声明，在类外部定义

void * routine(void *arg) {
    Singleton& instance = Singleton::getInstance();
    printf("thread[%ld] get instance at %p\n", pthread_self(), &instance);
    return nullptr;
}

#ifndef THREAD_NUM
#define THREAD_NUM 100
#endif

int main() {

    setvbuf(stdout, nullptr, _IONBF, 0);

    pthread_t tids[THREAD_NUM];
    for (int i = 0; i < THREAD_NUM; i++) {
        pthread_create(&tids[i], nullptr, routine, nullptr);
    }

    for (int i = 0; i < THREAD_NUM; i++) {
        pthread_join(tids[i], nullptr);
    }
    return 0;
}
```

> [!attention] 
> 
> 由于引入了锁机制，会降低程序运行的性能。此外，还存在一个问题，上述代码第 $16$ 行(高亮部分)，这个行代码可能会分为 $3$ 个步骤
> + 创建内存
> + 初始化内存
> + 将内存地址赋值给 `instance`
> 
> 由于编译器的优化，可能会导致初始化内存最后被执行，此时其他线程可能会得到一个未初始化的对象
> 

C++11 标准提供了 `std::call_once()` 函数可以确保只会调用一次，在多线程环境中只会有一个线程可以执行 `call_once()`，其他线程会等待。同时，使用 C++ 提供的 `atomic` 类型，保证内存顺序

```cpp
#include <iostream>
#include <mutex>
#include <atomic>

#include <pthread.h>


using namespace std;

class Singleton {

public:
    void show() {
        printf("thread[%ld] Singleton instance at %p, a: %d, b: %d\n", pthread_self(), this, a, b);
    }

public:
    static Singleton* getInstance() {
	    // call_once 确保了 new Singleton 只会执行一次
        std::call_once(flag, []() {
	        Singleton * tmp = instance.load(std::memory_order_acquire);  // 顺序获取
            if (tmp == nullptr)
            {
                tmp = new Singleton{10, 20};
                instance.store(tmp, std::memory_order_release);
            }
        });
        return instance;  // 返回指针避免复制
    }

    // 确保在程序结束时才调用该函数销毁单例
    static void destroy() {
        if(instance != nullptr) {
            delete instance;
            instance = nullptr;
        }
    }

private:
    Singleton():a{0}, b{0} {};  // 生成默认构造函数，但是是私有的，外部无法调用
    Singleton(int a, int b) : a(a), b(b) {}  // 生成有参构造函数，但是是私有的，外部无法调用    

    ~Singleton() =default; // 生成默认析构函数

    // 禁止拷贝构造函数和拷贝赋值操作符
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
    // 禁止移动构造函数和移动赋值操作符
    Singleton(Singleton&&) = delete;
    Singleton& operator=(Singleton&&) = delete;

private:
    static std::atomic<Singleton*> instance;  // 声明类的静态成员：指向类的唯一实例
    static std::once_flag flag;  // 声明类的静态成员：保证线程安全
private:
    int a;
    int b;
};

std::atomic<Singleton*> Singleton::instance = nullptr;  // 类的仅仅是声明，在类外部定义
std::once_flag Singleton::flag;  // 类的仅仅是声明，在类外部定义

void * routine(void *arg) {
    Singleton * instance = Singleton::getInstance();
    instance->show();
    return nullptr;
}

#ifndef THREAD_NUM
#define THREAD_NUM 100
#endif

int main() {

    setvbuf(stdout, nullptr, _IONBF, 0);

    pthread_t tids[THREAD_NUM];
    for (int i = 0; i < THREAD_NUM; i++) {
        pthread_create(&tids[i], nullptr, routine, nullptr);
    }

    for (int i = 0; i < THREAD_NUM; i++) {
        pthread_join(tids[i], nullptr);
    }

    // 程序结束时销毁单例
    Singleton::destroy();
    return 0;
}
```

> [!question] 
> 
> 上述单例模式虽然线程安全的，但是 `destory()` 可能在多个线程中被调用。需要进行双检
> 

```cpp
#include <iostream>
#include <pthread.h>
#include <mutex>


using namespace std;

class Singleton {

public:
    void show() {
        printf("thread[%ld] Singleton instance at %p, a: %d, b: %d\n", pthread_self(), this, a, b);
    }

public:
    static Singleton* getInstance() {
        std::call_once(flag, []() {
	        Singleton * tmp = instance.load(std::memory_order_acquire);  // 顺序获取
            if (tmp == nullptr)
            {
                tmp = new Singleton{10, 20};
                instance.store(tmp, std::memory_order_release);
            }
        });
        return instance;  // 返回指针避免复制
    }

    // 确保在程序结束时才调用该函数销毁单例
    static void destroy() {
        std::call_once(destroyflag, []() {
            if (instance != nullptr) {
                delete instance;
                instance = nullptr;
            }
        });
    }

private:
    Singleton():a{0}, b{0} {};  // 生成默认构造函数，但是是私有的，外部无法调用
    Singleton(int a, int b) : a(a), b(b) {}  // 生成有参构造函数，但是是私有的，外部无法调用    

    ~Singleton() =default; // 生成默认析构函数

    // 禁止拷贝构造函数和拷贝赋值操作符
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
    // 禁止移动构造函数和移动赋值操作符
    Singleton(Singleton&&) = delete;
    Singleton& operator=(Singleton&&) = delete;

private:
    static std::atomic<Singleton*> instance;  // 声明类的静态成员：指向类的唯一实例
    static std::once_flag newflag;  // 声明类的静态成员：保证线程安全
    static std::once_flag destroyflag;  // 声明类的静态成员：保证线程安全
private:
    int a;
    int b;
};

std::atomic<Singleton*> Singleton::instance = nullptr;  // 类的仅仅是声明，在类外部定义
std::once_flag Singleton::newflag;  // 类的仅仅是声明，在类外部定义
std::once_flag Singleton::destroyflag;  // 类的仅仅是声明，在类外部定义

void * routine(void *arg) {
    Singleton * instance = Singleton::getInstance();
    instance->show();
    return nullptr;
}

#ifndef THREAD_NUM
#define THREAD_NUM 100
#endif

int main() {

    setvbuf(stdout, nullptr, _IONBF, 0);

    pthread_t tids[THREAD_NUM];
    for (int i = 0; i < THREAD_NUM; i++) {
        pthread_create(&tids[i], nullptr, routine, nullptr);
    }

    for (int i = 0; i < THREAD_NUM; i++) {
        pthread_join(tids[i], nullptr);
    }

    // 程序结束时销毁单例
    Singleton::destroy();
    return 0;
}
```

### 定制 operator new() 实现单例模式

当我们将对象创建在堆空间时，一定会调用 `operator new()` 运算符函数。因此，我们可以在 `operator new()` 中拦截对象的创建

同拦截对象创建的过程，我们可以让构造函数是 `public`，这样每次获取实例时都可以创建提供不同的初始值或者不修改已有的对象的值

```cpp
#include <iostream>
#include <pthread.h>
#include <mutex>


using namespace std;

class Singleton {

public:

    void show() {
        printf("thread[%ld] Singleton instance at %p, a: %d, b: %d\n", pthread_self(), this, a, b);
    }

public:
    // 普通构造函数: explicit 防止隐式转换
    explicit Singleton(int a=0, int b=0):a{a}, b{b} {};

    // 定制 operator new 函数
    void * operator new(size_t size) {
        // 先检查是否已经存在实例，如果存在则直接返回
        if(instance == nullptr) {
            // 如果不存在实例，则调用::operator new(size)分配内存
            // 在分配内存之前，应该加锁，防止多线程下出现问题
            mtx.lock();  // 加锁
            // 再次检查是否已经存在实例，因为在等待锁的过程中，可能其他线程已经创建了实例
            if(instance == nullptr)
            {
                instance = (Singleton *)::operator new(size);
            }
            mtx.unlock(); // 解锁
        }
        return instance;
    }

    // 定制 operator delete 函数: 防止外部调用 delete 删除实例
    void operator delete(void *p) {
       throw std::runtime_error("Do not delete Singleton instance");
    }

public:
    static void destroy() {
        // 释放实例内存
        if (instance) {
            // 在释放内存之前，应该加锁，防止多线程下出现问题
            mtx.lock();  // 加锁
            // 再次检查是否已经存在实例，因为在等待锁的过程中，可能其他线程已经释放了实例
            if(instance) {
                // 调用::operator delete(instance)释放内存
                // 因为 operator delete 函数限制，所以外部无法调用 delete 删除实例
                ::operator delete(instance);
                instance = nullptr;
            }
            mtx.unlock(); // 解锁
        }
    }

private:

    ~Singleton() =default; // 生成默认析构函数，但是是私有的，外部无法调用。因此，无法创建栈对象

    // 禁止拷贝构造函数和拷贝赋值操作符
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
    // 禁止移动构造函数和移动赋值操作符
    Singleton(Singleton&&) = delete;
    Singleton& operator=(Singleton&&) = delete;

private:
    static Singleton* instance;  // 声明类的静态成员：指向类的唯一实例
    static std::mutex mtx;       // 声明类的静态成员：互斥锁

private:
    int a {0};
    int b {0};
};

Singleton* Singleton::instance = nullptr; // 定义类的静态成员
std::mutex Singleton::mtx;                // 定义类的静态成员

void * routine(void *arg) {
    Singleton *instance = new Singleton{10, 20};
    instance->show();
    return nullptr;
}

#ifndef THREAD_NUM
#define THREAD_NUM 100
#endif

int main() {

    setvbuf(stdout, nullptr, _IONBF, 0);

    pthread_t tids[THREAD_NUM];
    for (int i = 0; i < THREAD_NUM; i++) {
        pthread_create(&tids[i], nullptr, routine, nullptr);
    }

    for (int i = 0; i < THREAD_NUM; i++) {
        pthread_join(tids[i], nullptr);
    }

    Singleton::destroy();  // 释放实例内存
    return 0;
}
```

> [!attention] 
> 
> 注意：`operator new()` 和 `operator delete()` 可能因为编译器或 CPU 对指令的重新排序导致线程不安全的情况
> 

为了解决因为指令重新排序导致线程不安全的问题，C++ 提供了 `std::atomic` 确保线程安全

```cpp
#include <iostream>
#include <pthread.h>
#include <mutex>
#include <atomic>


using namespace std;

class Singleton {

public:

    void show() {
        printf("thread[%ld] Singleton instance at %p, a: %d, b: %d\n", pthread_self(), this, a, b);
    }

public:
    // 普通构造函数: explicit 防止隐式转换
    explicit Singleton(int a=0, int b=0):a{a}, b{b} {};

    // 定制 operator new 函数
    void * operator new(size_t size) {
        // 先检查是否已经存在实例，如果存在则直接返回
        Singleton *tmp = instance.load(std::memory_order_acquire);  // 从内存中读取 instance 的值
        if(tmp == nullptr) {
            // 如果不存在实例，则调用::operator new(size)分配内存
            // 在分配内存之前，应该加锁，防止多线程下出现问题
            mtx.lock();  // 加锁
            // 再次检查是否已经存在实例，因为在等待锁的过程中，可能其他线程已经创建了实例
            tmp = instance.load(std::memory_order_acquire);  // 从内存中读取 instance 的值
            if(tmp == nullptr)
            {
                tmp = static_cast<Singleton*>(::operator new(size)); // 分配内存
                instance.store(tmp, std::memory_order_release);  // 将实例指针存入内存
            }
            mtx.unlock(); // 解锁
        }
        return tmp;
    }

    // 定制 operator delete 函数: 防止外部调用 delete 删除实例
    void operator delete(void *p) {
       throw std::runtime_error("Do not delete Singleton instance");
    }

public:
    static void destroy() {
        // 释放实例内存
        Singleton *tmp = instance.load(std::memory_order_acquire);  // 从内存中读取 instance 的值
        if (tmp) {
            // 在释放内存之前，应该加锁，防止多线程下出现问题
            mtx.lock();  // 加锁
            // 再次检查是否已经存在实例，因为在等待锁的过程中，可能其他线程已经释放了实例
            tmp = instance.load(std::memory_order_acquire);  // 从内存中读取 instance 的值
            if(tmp) {
                // 调用::operator delete(instance)释放内存
                // 因为 operator delete 函数限制，所以外部无法调用 delete 删除实例
                ::operator delete(tmp);
                instance.store(nullptr, std::memory_order_release);  // 将实例指针存入内存
            }
            mtx.unlock(); // 解锁
        }
    }

private:

    ~Singleton() =default; // 生成默认析构函数，但是是私有的，外部无法调用。因此，无法创建栈对象

    // 禁止拷贝构造函数和拷贝赋值操作符
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
    // 禁止移动构造函数和移动赋值操作符
    Singleton(Singleton&&) = delete;
    Singleton& operator=(Singleton&&) = delete;

private:
    static std::atomic<Singleton*> instance;  // 声明类的静态成员：指向类的唯一实例
    static std::mutex mtx;       // 声明类的静态成员：互斥锁

private:
    int a {0};
    int b {0};
};

std::atomic<Singleton*> Singleton::instance{nullptr}; // 定义类的静态成员
std::mutex Singleton::mtx;                // 定义类的静态成员

void * routine(void *arg) {
    Singleton *instance = new Singleton{10, 20};
    instance->show();
    return nullptr;
}

#ifndef THREAD_NUM
#define THREAD_NUM 100
#endif

int main() {

    setvbuf(stdout, nullptr, _IONBF, 0);

    pthread_t tids[THREAD_NUM];
    for (int i = 0; i < THREAD_NUM; i++) {
        pthread_create(&tids[i], nullptr, routine, nullptr);
    }

    for (int i = 0; i < THREAD_NUM; i++) {
        pthread_join(tids[i], nullptr);
    }

    Singleton::destroy();  // 释放实例内存
    return 0;
}
```

### **局部静态对象实现单例模式**

> [!tip] C++11 标准之后，使用局部静态变量实现单例模式是线程安全的
> 
> 如果在初始化变量时控制同时进入声明，则并发执行应等待初始化完成。
> 

```cpp
#include <iostream>
#include <pthread.h>
#include <mutex>


using namespace std;

class Singleton {
public:
    static Singleton& getInstance() {
        static Singleton instance;  // 局部静态变量，线程安全
        return instance;  // 返回引用避免复制
    }
private:
    Singleton() = default;  // 生成默认构造函数，但是是私有的，外部无法调用
    // 禁止拷贝构造函数和拷贝赋值操作符
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
    // 禁止移动构造函数和移动赋值操作符
    Singleton(Singleton&&) = delete;
    Singleton& operator=(Singleton&&) = delete;
};


void * routine(void *arg) {
    Singleton& instance = Singleton::getInstance();
    printf("thread[%ld] get instance at %p\n", pthread_self(), &instance);
    return nullptr;
}

#ifndef THREAD_NUM
#define THREAD_NUM 100
#endif


int main() {

    setvbuf(stdout, nullptr, _IONBF, 0);

    pthread_t tids[THREAD_NUM];
    for (int i = 0; i < THREAD_NUM; i++) {
        pthread_create(&tids[i], nullptr, routine, nullptr);
    }

    for (int i = 0; i < THREAD_NUM; i++) {
        pthread_join(tids[i], nullptr);
    }
    return 0;
}
```

## 饿汉式单例模式

为了避免加锁和解锁带来的性能损耗，可以在程序启动后就立即创建好对象，然后才创建其他线程。我们需要保证其他线程不去创建对象，只需要直接返回即可

```cpp
#include <iostream>
#include <pthread.h>

using namespace std;

class Singleton {
public:

    void show() {
        printf("thread[%ld] Singleton instance at %p, a: %d, b: %d\n", pthread_self(), this, a, b);
    }

public:
    static Singleton * getInstance() {
        // 懒汉模式:实例在程序启动时就已经创建好了，直接返回实例地址即可
        return instance;
    }

private:
    Singleton():a{0}, b{0} {};
    Singleton(int a, int b):a{a}, b{b} {};

    ~Singleton() = default;  // 禁止外部析构。无法创建栈空间对象

    Singleton(const Singleton &) = delete;
    Singleton &operator=(const Singleton &) = delete;
    Singleton(Singleton &&) = delete;
    Singleton &operator=(Singleton &&) = delete;

private:
    static Singleton * instance;


private:
    int a;
    int b;
};

// 程序启动时就创建实例
Singleton * Singleton::instance = new Singleton{10, 20};


/* 下面是测试代码 */
void * routine(void *arg) {
    Singleton *instance = Singleton::getInstance();
    instance->show();
    return nullptr;
}

#ifndef THREAD_NUM
#define THREAD_NUM 100
#endif

int main() {
    setvbuf(stdout, nullptr, _IONBF, 0);

    pthread_t tids[THREAD_NUM];
    for (int i = 0; i < THREAD_NUM; i++) {
        pthread_create(&tids[i], nullptr, routine, nullptr);
    }

    for (int i = 0; i < THREAD_NUM; i++) {
        pthread_join(tids[i], nullptr);
    }
    return 0;
}
```

## 总结

我们介绍了多种单例模式的实现方法。其中最推荐使用的还是 **局部静态对象** 和 `std::call_once` 的方式实现单例模式

> [!tip] 局部静态对象
> 
> C++11 标准之后，局部静态对象的初始化天然是线程安全的。只会有一个线程初始化局部静态对象，其他线程会等待该线程初始化完成后才会继续执行
> 

```cpp
#include <iostream>
#include <pthread.h>
#include <mutex>


using namespace std;

class Singleton {
public:
    static Singleton& getInstance() {
        static Singleton instance;  // 局部静态变量，线程安全
        return instance;  // 返回引用避免复制
    }
private:
    Singleton() = default;  // 生成默认构造函数，但是是私有的，外部无法调用
    // 禁止拷贝构造函数和拷贝赋值操作符
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
    // 禁止移动构造函数和移动赋值操作符
    Singleton(Singleton&&) = delete;
    Singleton& operator=(Singleton&&) = delete;
};


void * routine(void *arg) {
    Singleton& instance = Singleton::getInstance();
    printf("thread[%ld] get instance at %p\n", pthread_self(), &instance);
    return nullptr;
}

#ifndef THREAD_NUM
#define THREAD_NUM 100
#endif


int main() {

    setvbuf(stdout, nullptr, _IONBF, 0);

    pthread_t tids[THREAD_NUM];
    for (int i = 0; i < THREAD_NUM; i++) {
        pthread_create(&tids[i], nullptr, routine, nullptr);
    }

    for (int i = 0; i < THREAD_NUM; i++) {
        pthread_join(tids[i], nullptr);
    }
    return 0;
}
```

> [!tip] 局部静态对象只适合小对象：可以存储在数据区域的对象。为了存储较大的对象，需要在堆空间上申请内存
> 
> 在堆上分配内存，就需要确保堆内存可以被访问，因此需要使用一个指针记录堆对象的地址
> 
> `std::call_once` 可以确保只有一个线程调用 `new` 运算符
> 

```cpp
#include <iostream>
#include <pthread.h>
#include <mutex>


using namespace std;

class Singleton {

public:
    void show() {
        printf("thread[%ld] Singleton instance at %p, a: %d, b: %d\n", pthread_self(), this, a, b);
    }

public:
    static Singleton* getInstance() {
	    // call_once 确保了 new Singleton 只会执行一次
        std::call_once(flag, []() {
            instance = new Singleton{10, 20};
        });
        return instance;  // 返回指针避免复制
    }

    // 确保在程序结束时才调用该函数销毁单例
    static void destroy() {
        if(instance != nullptr) {
            delete instance;
            instance = nullptr;
        }
    }

private:
    Singleton():a{0}, b{0} {};  // 生成默认构造函数，但是是私有的，外部无法调用
    Singleton(int a, int b) : a(a), b(b) {}  // 生成有参构造函数，但是是私有的，外部无法调用    

    ~Singleton() =default; // 生成默认析构函数

    // 禁止拷贝构造函数和拷贝赋值操作符
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
    // 禁止移动构造函数和移动赋值操作符
    Singleton(Singleton&&) = delete;
    Singleton& operator=(Singleton&&) = delete;

private:
    static Singleton* instance;  // 声明类的静态成员：指向类的唯一实例
    static std::once_flag flag;  // 声明类的静态成员：保证线程安全
private:
    int a;
    int b;
};

Singleton* Singleton::instance = nullptr;  // 类的仅仅是声明，在类外部定义
std::once_flag Singleton::flag;  // 类的仅仅是声明，在类外部定义

void * routine(void *arg) {
    Singleton * instance = Singleton::getInstance();
    instance->show();
    return nullptr;
}

#ifndef THREAD_NUM
#define THREAD_NUM 100
#endif

int main() {

    setvbuf(stdout, nullptr, _IONBF, 0);

    pthread_t tids[THREAD_NUM];
    for (int i = 0; i < THREAD_NUM; i++) {
        pthread_create(&tids[i], nullptr, routine, nullptr);
    }

    for (int i = 0; i < THREAD_NUM; i++) {
        pthread_join(tids[i], nullptr);
    }

    // 程序结束时销毁单例
    Singleton::destroy();
    return 0;
}
```
