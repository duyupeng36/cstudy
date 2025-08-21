# 单例对象自动释放

在 [[C++：单例模式]] 中我们介绍了多种实现单例模式的方法。然而，当我们将对象申请在堆空间中时，我们需要一个额外的静态成员函数来释放堆空间中的内存

> [!tip] 
> 
> 手动调用释放堆空间的函数是不明智的。我们可能忘记调用它，从而造成内存泄漏。甚至可能调用多次，造成 `double free` 的错误
> 

我们期望单例对象能够在程序结束时自动释放掉。显然，需要另一个静态对象保存单例对象的指针，当该静态对象被销毁时就调用析构函数，从而释放掉单例对象

如下图所示，使用 `AutoRelease` 类的对象管理单例对象的生命周期。这里，我们让 `AutoRelease` 类的对象存储在静态存储区，这样当程序结束时，`AutoRelease` 对象就会被销毁，我们定义 `AutoRelease` 的析构函数区释放单例对象

![Pasted image 20250209130343|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755785608651-3f12e92df0ae45dc9a068df19c2f2abd.png)

下面的代码展示了 `AutoRelease` 的实现方式。由于 `AutoRelease` 只是用于释放单例对象，因此，我们让 `AutoRelease` 成为为 `Singleton` 的嵌套类

> [!tip] 
> 
> 使用嵌套类的好处就是嵌套类可以直接访问外部类的所有成员
> 

```cpp hl:38-50,54,62
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


private:
    Singleton():a{0}, b{0} {};  // 生成默认构造函数，但是是私有的，外部无法调用
    Singleton(int a, int b) : a(a), b(b) {}  // 生成有参构造函数，但是是私有的，外部无法调用    

    ~Singleton() =default; // 生成默认析构函数。如果单例对象持有资源，需要自定义析构函数

    // 禁止拷贝构造函数和拷贝赋值操作符
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
    // 禁止移动构造函数和移动赋值操作符
    Singleton(Singleton&&) = delete;
    Singleton& operator=(Singleton&&) = delete;

private:
    class AutoRelease
    {
    public:
        ~AutoRelease() {
            // instacne 访问的外部类的静态成员
            if(instance) {
                cout << "~AutoRealse()" << endl;
                delete instance; // 先调用单例对象的析构函数，然后调用 operator delete 删除对象占用的内存 
                instance = nullptr;
            }
        }
    };

private:
    static Singleton* instance;  // 声明类的静态成员：指向类的唯一实例
    static AutoRelease ar;        // 声明类的静态成员
    static std::once_flag flag;  // 声明类的静态成员：保证线程安全
private:
    int a;
    int b;
};

Singleton* Singleton::instance = nullptr;  // 在类外部定义
Singleton::AutoRelease Singleton::ar;       // 在类外部定义
std::once_flag Singleton::flag;            //在类外部定义

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
    return 0;
}
```

上述方案是比较完善的一个单例对象自动释放的方案。下面我们介绍一些其他的方案

## atexit 注册对象销毁函数

[[Linux 系统编程：进程终止#退出处理程序]] 介绍了 **在进程终止前可以自动执行一些动作**。通过 `atexit()` 注册清理单例对象的函数，也可以做到单例对象的自动释放

```cpp hl:20,25-32
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
            atexit(destroy);  // 注册单例对象释放的函数
        });
        return instance;  // 返回指针避免复制
    }

private:
    // 让 destory() 称为静态成员函数，进制在类外调用该函数
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
```

