# string 类和 vector 类
## string 类

我们已经学过具体类的是如何定义和使用的了。下面我学习 C++ 标准库提供的 `string` 类。C++ 在头文件 `<string>` 中提供了 `std::string` 类用与处理字符串

> [!tip] 
> 
> 在 C++ 中也可以使用 C 风格的字符串。C++ 将 `<string.h>` 包装为了 `<cstring>` 从而提供了 C 风格字符串处理的函数
> 
> 注意：尽量不要在 C++ 中使用 C 风格字符串。`std::string` 类是一个具体类，我们可以将它当作内置类型使用
> 

`std::string` 类是 `basic_string` 类模板的 `char` 实例。即 `std::string` 是 `std::basic_string<char>` 的别名

```cpp
template<
    class CharT,
    class Traits = std::char_traits<CharT>,
    class Allocator = std::allocator<CharT>> 
class basic_string;

using string = basic_string<char>;  // 类型别名
```

> [!tip] 
> 
> 现在请忽视模板的语法。后面才会学习 C++ 中的模板相关内容
> 

### string 构造函数

`string` 是一个具体类，提供了很多构造函数。下表列出了几个常用的构造函数

| 构造函数                                   | 描述                              |
| :------------------------------------- | ------------------------------- |
| `string()`                             | 默认构造。创建一个空字符串对                  |
| `string(const char * rhs)`             | 使用 C 风格字符串构造 string 对象          |
| `string(const char *rhs, size_t n)`    | 使用 C 风格字符串前 `n` 个字符构造 string 对象 |
| `string(const string &rhs)`            | 拷贝构造                            |
| `string(size_t n, char ch)`            | 使用 `n` 个字符 `ch` 构造 string 对象    |

```cpp
#include <iostream>
#include <string>

using namespace std;

int main() {

    string name1{"John"}; // C 风格字符串构造
    cout << name1 << endl;

    string name2(name1); // 拷贝构造函数
    cout << name2 << endl;

    string name3(5, 'a');  // 使用对应的构造函数
    cout << name3 << endl;
}
```

### 拼接

string 类通过重载 `operator+()` 运算符函数使得我们可以使用运算符 `+` 完成字符串拼接。下表列出了 `operator+()` 的运算符重载情形

| 运算符                                                      | 描述          |
| :------------------------------------------------------- | ----------- |
| `string operator+(const string& lhs, const string& rhs)` | 拼接 `string` |
| `string operator+(const string& lhs, const char *rhs)`   | 拼接 C 风格字符串  |
| `string operator+(const char *lhs, const string& rhs)`   | 拼接 C 风格字符串  |
| `string operator+(const string& lhs, const char rhs)`    | 拼接字符        |
| `string operator+(const char lhs, const string& rhs)`    | 拼接字符        |

```cpp
#include <iostream>
#include <string>

using namespace std;

int main() {
    string str{"Hello, World!"};
    cout << str << endl;

    str = "ccc " + str + " xxx" + 'a';
    cout << str << endl;
}
```

### string 成员函数

C++ 的 string 类提供许多成员函数用于操作 `string` 对象。下表列出了常用的成员函数

| 成员函数                                                                      | 描述                               |
| :------------------------------------------------------------------------ | :------------------------------- |
| `const CharT* data() const;`                                              | 返回 C 风格字符串                       |
| `const CHarT* c_str() const;`                                             | 返回 C 风格字符串                       |
| `bool empty() const;`                                                     | 判断字符串是否为空字符串                     |
| `size_type size() const;`                                                 | 获取字符串中的字符数                       |
| `size_type length() const;`                                               | 获取字符串中的字符数                       |
| `void push_back(CharT ch);`                                               | 在字符串末尾追加字符 `ch`                  |
| `basic_string& append(size_type n, CharT ch);`                            | 在字符串末尾追加 `n` 个 `ch`              |
| `basic_string& append(const basic_string& str);`                          | 在字符串末尾追加一个字符串                    |
| `basic_string& append(const charT* s);`                                   | 在字符串末尾追加一个字符串                    |
|                                                                           |                                  |
| `size_type find( const basic_string& str, size_type pos = 0 ) const;`     | 从 `pos` 开始查找 `str`               |
| `size_type find( CharT ch, size_type pos = 0 ) const;`                    | 从 `pos` 开查找 `ch`                 |
| `size_type find( const CharT* s, size_type pos, size_type count ) const;` | 从 `pos` 位开始，去查找字符串的前 `count` 个字符 |

### string 比较

C++ 对 `string` 类提供了比较运算符的重载

```cpp
//非成员函数
bool operator==(const string & lhs, const string & rhs);
bool operator!=(const string & lhs, const string & rhs);
bool operator>(const string & lhs, const string & rhs);
bool operator<(const string & lhs, const string & rhs);
bool operator>=(const string & lhs, const string & rhs);
bool operator<=(const string & lhs, const string & rhs);
```

### string 的遍历

string 实际上也可以看作是一种存储 `char` 型数据的容器，对 string 的遍历方法是后续学习的各自容器的遍历方法是相同的

`string` 对象可以使用下标操作符 `[]` 进行访问。由于 `string` 类对 `operator[]()` 运算符进行了重载

```cpp
//使用下标遍历
for(size_t idx = 0; idx < str.size(); ++idx){
    cout << str[idx] << " ";
}
cout << endl;
```

需要注意的是操作符 `[]` 并不检查索引是否有效，如果索引超出范围，会引起未定义的行为。而函数 `at()` 会检查，如果使用 `at()` 的时候索引无效，会抛出 `out_of_range` 异常

```cpp
string str("hello");
cout << str.at(4) << endl;  //输出o
cout << str.at(5) << endl;  //运行时抛出异常
```

对容器进行遍历时，可以使用范围 `for`

```cpp
for(auto & ch : str){  //只要是str中的元素，就一一遍历
    cout << ch << " ";
}
cout << endl;
```

`string` 字符串利用连续空间存储字符，所以可以 **利用地址遍历**。这里我们提出一个概念——**迭代器**。迭代器可以理解为是广义的指针。它可以像指针一样进行解引用、移位等操作。**迭代器是容器用来访问元素的重要手段**，容器都有相应的函数来获取特定的迭代器（此处可以简单理解为指向特定元素的指针）。在 STL 的阶段，我们会对迭代器进行更详细的讲解，现在我们只需要掌握它的基本使用即可。

> [!tip] 、
> + `begin()` 函数返回容器中首元素的地址
> + `end()` 函数返回容器中最后一个元素后的地址
> 
> ![[Pasted image 20250205131448.png]]
> 

如指针一样，迭代器也有其固定的形式。

```cpp
// 某容器的迭代器形式为 容器名::iterator
// 此处 auto 推导出来 it 的类型为 string::iterator
auto it = str.begin();
while(it != str.end()){
    cout << *it << " ";
	++it;
}
cout << endl;
```

## vector 类

C++中，`std::vector`（向量）是一个动态数组容器，能存放任意类型的数据。

其动态性体现在以下几个方面：

1. **动态大小**：`std::vector` 可以根据需要自动调整自身的大小。它在内部管理一个动态分配的数组，可以根据元素的数量进行自动扩容或缩减。当元素数量超过当前容量时，`std::vector` 会重新分配内存，并将元素复制到新的内存位置。这使得 `std::vector` 能够根据需要动态地增长或缩小容量，而无需手动管理内存。
2. **动态插入和删除**：`std::vector` 允许在任意位置插入或删除元素，而不会影响其他元素的位置。当插入新元素时，`std::vector` 会自动调整容量，并将后续元素向后移动以腾出空间。同样地，当删除元素时，`std::vector` 会自动调整容量，并将后续元素向前移动以填补空缺。
3. **动态访问**：`std::vector` 提供了随机访问元素的能力。可以通过索引直接访问容器中的元素，而不需要遍历整个容器。这使得对元素的访问具有常数时间复杂度（O(1)），无论容器的大小如何。

### vector 的构造

无参构造，仅指明 vector 存放元素的种类，没有存放元素；

```cpp
vector <int> numbers;
```

传入一个参数，指明vector存放元素的种类和数量，参数是存放元素的数量，每个元素的值为该类型对应的默认值；

```cpp
vector<long> numbers2(10); //存放10个0
```

传入两个参数，第一个参数为vetor存放元素的数量，第二个参数为每个元素的值（相同）；

```cpp
vector<long> numbers2(10，20); //存放10个20
```

通过列表初始化vector，直接指明存放的所有元素的值

```cpp
vector<int> number3{1,2,3,4,5,6,7};
```

迭代器方式初始化 vector，传入两个迭代器作为参数，第一个为首迭代器，第二个为尾后迭代器；

```cpp
vector<int> number3{1,2,3,4,5,6,7};
vector<int> number4(number3.begin(),number3.end() - 3);//推测一下，number4中存了哪些元素
```

### vector 的成员函数

```cpp
iterator begin();  // 返回首位迭代器
iterator end();    // 返回尾后迭代器

bool empty() const; // 判空

size_type size() const;         // 返回容器中存放的元素个数
size_type capacity() const;     // 返回容器容量（最多可以存放元素的个数）

void push_back(const T& value); // 在最后一个元素的后面再存放元素

void pop_back();       // 删除最后一个元素
void clear();          // 清空所有元素，但不释放空间
void shrink_to_fit();  // 释放多余的空间（可以存放元素但没有存放运算的空间）

void reserve(size_type new_cap); // 申请空间，不存放元素
```

### vector 实现原理

在 $64$ 为系统中，利用 `sizeof` 查看 `vector` 对象的大小时，发现无论存放的元素类型、数量如何，其大小始终为 $24$ 个字节

```cpp
#include <iostream>
#include <vector>

using namespace std;

int main() {
    cout << sizeof(vector<int>) << endl;
    cout << sizeof(vector<double>) << endl;
    cout << sizeof(vector<string>) << endl;
}

```

编译运行的结果为

```shell
➜  cppfiles g++ -std=c++11 -fno-elide-constructors main.cpp
➜  cppfiles ./a.out 
24
24
24
```

显然， `vector` 对象仅仅只保存了指向元素的指，元素存储在堆空间中。在 $64$ 位环境中，由于一个 `vector` 对象占用了 $24$ 字节，因此一个 `vector` 对象持有 $3$ 个指针成员。`vector` 的具体实现如下图

![[Pasted image 20250205134140.png]]

成员 `_start` 指向第一个元素；成员 `_finish` 指向最后一个元素的后一个位置；成员 `_end_of_storage` 指向当前 `vector` 管理的堆空间中的下一个位置

### vector 的动态扩容

当 vector 存放满后，仍然追加存放元素，vector 会进行自动扩容。不同的实现可能采用不同的扩容策略。有些实现采用 $2$ 倍的扩容策略；有些实现则采用 $1.5$ 倍扩容的方式

下图展示了 `vector` 扩容的整个过程

![[Pasted image 20250205134834.png]]

