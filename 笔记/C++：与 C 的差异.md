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
