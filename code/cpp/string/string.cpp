#include <iostream>
#include <string>

using namespace std;

string password{"NBwd1175上海"};

int main() {
    
    {   
        // 使用 C 风格的字符串进行初始化和赋值
        // 声明并初始化
        string name {"张三"};
        cout << "name: " << name << endl;
        // 给 name 从新赋值
        name = "李四";
        cout << "name: " << name << endl;
    }

    { 
        // 如果我们已经有 string 类型的变量了 
        // 就可以直接使用它初始化其他变量
        string password {::password};  // ::password 访问全局名字空间中的 password   
        cout << "password: " << password << endl;

        cout << "size: " << password.size() << endl;
    }

    {
        // 比较运算符
        string str1{"hello, world!"};
        string str2{"Hello, world!"};

        // 测试 str1 是否小于 str2: 按字符编码进行比较: h > H => false
        cout << "str1 < str2: " << (str1 < str2) << endl; 
        // 测试 str1 是否大于 str2: 按字符编码进行比较: h > H => true
        cout << "str1 > str2: " << (str1 > str2) << endl; 
    }

    {
        //
        string str1{"hello, world!"};
        string str2;
        str2 = str1;  // 复制
    }
    {
        string str1{"hello,"};
        string str2{" world!"};
        string str3{str1 + str2};

        cout << "str1 + str2: " << str3 << endl;
    }

    {
        string str{"hello, 北疆"};
        cout << "str.size(): " << str.size() << endl;  // 13
        cout << "str.length(): " << str.length() << endl; // 13
    }

}
