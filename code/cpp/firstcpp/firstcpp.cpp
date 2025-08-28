/**
* 第一个 CPP 程序
* + 理解第一个 CPP 程序
* + 名字空间：提供了更加细粒度的名字访问限制，从而避免名字冲突
* + cout 输出内容
*/


// #include <iostream> 预处理指令用于将 <iostream> 指定的文件拷贝到当前位置
// <> 表示从环境默认位置拷贝
#include <iostream>

// using namespace std; 指示编译器使用名字空间 std。此时，std 中的名字就好像在当前位置进行了声明
using namespace std;

// int main() {} 是一个函数的定义
// + int 表示函数返回值的类型
// + main 表示函数名
// + () 参数列表：函数可以接受的参数，单纯的一个 () 表示没有参数
// + {} 函数体，函数
int main()
{
    // std::cout 表示控制台对象。其中 std 表示 C++ 标准名字空间；cout 表示控制台对象
    // + << :运算符用于向 cout 的缓冲区写入内容
    // + "Hello World!\n" : 表示一个字符串
    std::cout << "Hello World!\n";

    // std::endl : 表示输入结束的对象，向 cout 中写入 \n 并刷新缓冲区
    cout << "Hello, firstcpp" << endl;

    // << 运算符自动将输出内容进行格式化，相比于 C 语言的 printf 会更见安全
    cout << "My name is" << " zhangsan. "  
        << "I'm " << 20 << " years old!"
        << endl;

    // return 用于返回函数的返回值
    return 0;
    // 对于 main() 函数而言，其返回值用于表示程序执行的状态
    // + return 0 : 表示程序正常结束
    // + return 非零: 表示程序出现异常 

}