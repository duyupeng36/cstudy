# IO 流

在 [[C 语言：标准 IO 流]] 介绍了 C 语言提供的标准输入输出库，在 C++ 中依旧可以使用。然而， C++ 提供功能更强大的输入输出流库。

> [!tip] IO(Input and Output)：即输入输出
> 
> + Input：将 **文件** 中的内容读入到 **内存**(**文件 -> 内存**)
> + Output：将 **内存** 中的内容写入到 **文件**(**内存 -> 文件**)
> 
> 注意：这里的文件是广义的概念。在 [[Linux 系统编程：文件 IO]] 中我们已经介绍过了各种类型的文件；即使是设备也会被 Unix/Linux 视为文件进行处理
> 

C++ 中的 IO 操作以流(`stream`)的形式出现。所谓的流就是字节序列。**从 C++ 程序角度看，Input 操作就是将字节序列移入内存或者；Output 操作就将字节序列从内存中移出**

C++ 将 IO 操作封装为各种流。下表列出了各种流的类名

| 类名              | 作用       | 头文件          |
| --------------- | -------- | ------------ |
| `istream`       | 通用输入流    | `<iostream>` |
| `ostream`       | 通用输出流    | `<iostream>` |
| `iostream`      | 通用输入输出流  | `<iostream>` |
| `ifstream`      | 文件输入流    | `<fstream>`  |
| `oftream`       | 文件输出流    | `<fstream>`  |
| `fstream`       | 文件输入输出流  | `<fstream>`  |
| `istringstream` | 字符串输入流   | `<sstream>`  |
| `ostringstream` | 字符串输出流   | `<sstream>`  |
| `stringstream`  | 字符串输入输出流 | `<sstream>`  |

下图给出了这些类的继承关系

![[Pasted image 20250205184328.png]]

## 流的状态

IO 操作存在一个天然的问题就是会出现错误。某些错误是可以恢复，而某些错误无法恢复。在 C++ 中，使用 `iostate` 枚举类型表示流的状态。不同的实现可能提供不同的状态，但是，下面 $4$ 种状态都存在

| 状态        | 描述                                 |
| :-------- | :--------------------------------- |
| `badbit`  | 系统错误。流不能再被使用了                      |
| `failbit` | 可恢复错误。例如，期望读数字，但是读了字符              |
| `eofbit`  | 到达流结尾位置。`badbit` 和 `failbit` 均会被置位 |
| `goodbit` | 流处于有效状态                            |

流的状态是作为 `ios_base` 类的数据成员存在。通过状态成员函数获取流的状态

```cpp
bool good() const      //流是goodbit状态，返回true，否则返回false
bool bad() const       //流是badbit状态，返回true，否则返回false
bool fail() const      //流是failbit状态，返回true，否则返回false
bool eof() const       //流是eofbit状态，返回true，否则返回false
```

## 标准 IO 流

对系统指定的标准设备进行输入输出。即，从键盘输入数据；输出到显示屏幕上。这类从标准输入设备读取字节序列或向标准输入设备写入字节序列称为 **标准 IO**

C++ 标准库中预定义了 $3$ 个标准 IO 流对象。下表列出了标准 IO 流对象和其类型

| 标准 IO 流对象   | 描述    | 类型        |
| :---------- | ----- | --------- |
| `std::cin`  | 标准输入流 | `istream` |
| `std::cout` | 标准输出流 | `ostream` |
| `std::cerr` | 标准错误流 | `ostream` |

### 通用输入 istream

C++ 标准库为 `istream` 类重载了运算符 `operator>>()` 函数为读取操作，所以我们可以使用运算符 `>>` 从 `istream` 对象中读取数据。

> [!tip] 
> 
>  `operator>>()` 是带格式的输入，会根据右操作数进行转换，如果转换失败就会设置流的状态标志位
> 

```cpp
/* 读入 整数 */
basic_istream& operator>>( short& value );
basic_istream& operator>>( unsigned short& value );
basic_istream& operator>>( int& value );
basic_istream& operator>>( unsigned int& value );
basic_istream& operator>>( long& value )
basic_istream& operator>>( unsigned long& value );
basic_istream& operator>>( long long& value );
basic_istream& operator>>( unsigned long long& value );

/* 读入 浮点数 */
basic_istream& operator>>( float& value );
basic_istream& operator>>( double& value );
basic_istream& operator>>( long double& value );
/* 读入 布尔值 */
basic_istream& operator>>( bool& value );

/* 读入 任意的数据。分配内存，并见内存的首地址存储在 value 中 */
basic_istream& operator>>( void*& value );

basic_istream& operator>>( std::basic_streambuf<CharT, Traits>* sb );

/* 调用 func(*this) */
basic_istream& operator>>( std::ios_base& (*func)(std::ios_base&) );
basic_istream& operator>>( std::basic_ios<CharT, Traits>& (*func)(std::basic_ios<CharT, Traits>&));
basic_istream& operator>>( basic_istream& (*func)(basic_istream&) );
```

> [!tip]
> 
> 使用 `operator>>()` 进行输入时，默认使用 **空白字符** 作为两次输入的分隔符。所谓的空白符就是打印不可见的符号，包括 _换行_ _空格_ _制表符_  等
> 

下面来看一个例子，每次从 `cin` 中获取一个字符:

```cpp
#include <iostream>

using std::cin;
using std::cout;
using std::endl;

void printStreamStatus(std::istream & is){ 
    cout << "is's goodbit:" << is.good() << endl;
    cout << "is's badbit:" << is.bad() << endl;
    cout << "is's failbit:" << is.fail() << endl;
    cout << "is's eofbit:" << is.eof() << endl;
}

int main(){
    printStreamStatus(cin);  //goodbit状态
    int num = 0;    
    cin >> num;   
    cout << "num:" << num << endl;
    printStreamStatus(cin);  //进行一次输入后再检查cin的状态
	cin >> num;
    cout << "num:" << num << endl;
    printStreamStatus(cin);  //进行第二次输入后再检查cin的状态
    return 0;
}
```

上述代码编译并运行。如果第一次输入错误，那么将导致流不能被使用

```shell
➜  cppfiles g++ -g -std=c++11 -fno-elide-constructors main.cpp
➜  cppfiles ./a.out
is's goodbit:1
is's badbit:0
is's failbit:0
is's eofbit:0
a                  # 第一次错误输入
num:0
is's goodbit:0
is's badbit:0
is's failbit:1
is's eofbit:0
num:0           # 第二次也无法获得正常的结果
is's goodbit:0
is's badbit:0
is's failbit:1
is's eofbit:0
```

思考，如何完成一个输入整型数据的实现（如果是非法输入则继续要求输入）?

> [!tip] 
> 
> 当输入无法转换为指定类型的值时，就会将流的状态置为 `failbit`。同时，读入的字符会被放回流中
> 
> 为了让流恢复，应该清空状态标志和忽略流缓冲区中的字符
> + 调用成员函数 `clear()` 用于清空流状态标志
> + 调用成员函数 `ignore(count, delim)` 忽视 `count` 个字符或者遇见字符 `delim`。注意，`delim` 字符也会被忽视。
> 

```cpp
#include <iostream>
#include <limits>

using std::cin;
using std::cout;
using std::endl;

int main(){
    int num;
	// 逗号表达式，其值为最后一个表达式的值
    while (cin >> num, !cin.eof())
    {
        // 检查流的状态
        if(cin.bad()) {
		    // 流处于系统错误
            cout << "IO stream corrupted!" << endl;
            return -1;
        }
        if(cin.fail()){
	        // 流处于可恢复的错误
            cout << "data illegal, please input an integer!" << endl;
            cin.clear();
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }
        cout << "num:" << num << endl;
    }
    return 0;
}
```

`istream` 还提供一些无格式输入的成员函数

| 函数         | 描述                     |
| :--------- | ---------------------- |
| `get`      | 从流提取一个或多个字符            |
| `peek`     | 查看下一个字符，但是不会提取它        |
| `unget`    | 撤销上次提取的字符。令最近提取的字符再次可用 |
| `putback`  | 将字符回放到输入流，使得下一次能提取该字符  |
| `getline`  | 从流提取字符，直至行尾或指定的分隔符     |
| `ignore`   | 忽略指定个数的字符，或遇见终止符       |
| `read`     | 从流提取指定数量的字符            |
| `readsome` | 提取已经可用的字符区块            |
| `gcount`   | 返回上次无格式输入操作提取的字符数量     |

> [!seealso] 
> 
> `istream` 的更多成员函数及其详细描述参考 [basic_istream](https://zh.cppreference.com/w/cpp/io/basic_istream)
> 

### 通用输出 ostream

同理，由于 `ostream` 类重载了运算符 `operator<<()` 函数为写入操作，所以使用运算符 `<<` 向 `ostream` 写入数据

> [!tip] 
> 
> `operator<<()` 是带格式的输出，会根据右操作数进行转换，如果转换失败就会设置流的状态标志位
> 

```cpp
/* 输出 整数 */
basic_ostream& operator<<( short& value );
basic_ostream& operator<<( unsigned short& value );
basic_ostream& operator<<( int& value );
basic_ostream& operator<<( unsigned int& value );
basic_ostream& operator<<( long& value )
basic_ostream& operator<<( unsigned long& value );
basic_ostream& operator<<( long long& value );
basic_ostream& operator<<( unsigned long long& value );

/* 输出 浮点数 */
basic_ostream& operator<<( float& value );
basic_ostream& operator<<( double& value );
basic_ostream& operator<<( long double& value );
/* 输出 布尔值 */
basic_ostream& operator<<( bool& value );

/* 输出 指针的值 */
basic_ostream& operator<<( const void* value );
basic_ostream& operator<<( std::nullptr_t );

/* 调用 func(*this) */
basic_ostream& operator<<( std::ios_base& (*func)(std::ios_base&) );
basic_ostream& operator<<( std::basic_ios<CharT, Traits>& (*func)(std::basic_ios<CharT, Traits>&));
basic_ostream& operator<<( basic_ostream& (*func)(basic_ostream&) );

basic_ostream& operator<<( std::basic_streambuf<CharT, Traits>* sb );
```

C++ 为 `ostream` 提供了两个无格式输出的成员

| 函数      | 描述                         |
| :------ | -------------------------- |
| `put`   | 将一个字符写入流中。写入失败设置 `failbit` |
| `write` | 将指定数量的字符写入流中               |

> [!seealso] 
> 
> `ostream` 的更多成员函数及其详细描述参考 [basic_ostream](https://zh.cppreference.com/w/cpp/io/basic_ostream)
> 

## 文件 IO 流

下面我们学习在 C++ 中如何进行文件操作。在 C++ 中文件操作也被封装为了几个类

| 文件 IO 类    | 描述            |
| :--------- | ------------- |
| `ifstream` | 打开文件用于读取的类    |
| `ofstream` | 打开文件用于写入的类    |
| `fstream`  | 打开文件用于读取或写入的类 |

下面给出了这个三个类的构造函数

```cpp
// 未持有打开文件描述符
ifstream();  
// 以读取模式打开文件 filename
explicit ifstream(const char* filename, openmode mode = ios_base::in);
// 以读取模式打开文件 filename
explicit ifstream(const string & filename, openmode mode = ios_base::in);
```

```cpp
// 未持有打开文件描述符
ofstream();
// 以写入模式打开文件 filename
explicit ofstream(const char* filename, openmode mode = ios_base::out);
// 以写入模式打开文件 filename
explicit ofstream(const string & filename, openmode mode = ios_base::out);
```

```cpp
// 未持有打开文件描述符
fstream();
// 以读写模式打开文件 filename
explicit fstream(const char* filename, openmode mode = ios_base::in|out);
// 以读写模式打开文件 filename
explicit fstream(const string & filename, openmode mode = ios_base::in|out);
```

> [!tip] 文件模式
> 
> 每个文件流会使用才不同的文件模式打开文件。C++ 采用一个定义在 `ios_base` 中的枚举类型 `openmode` 定义， 共提供了 $6$ 中模式
> 
> + `ios_base::in`：输入，打开文件用于执行读操作。文件不存在时，则打开失败。类似于 C 标准输入输出库的 `"r"` 模式
> + `ios_base::out`：输出，打开文件用于执行写入操作。文件不存在时，则直接创建；文件存在，则清空文件。类似于 C 标准输入输出库的 `"w"` 模式
> + `ios_base::app`：追加，打开文件用于执行写入操作。注意，始终保证写入文件末尾。类似于 C 标准输入输出库的 `"a"` 模式
> + `ios_base::ate`：末尾写入，打开文件执行写入操作。注意，始终写入在文件末尾
> + `ios_base::trunc`：截断文件，如果打开的文件存在，将其大小截断为 $0$
> + `ios_base::binary`：二进制，操作二进制数据
>   
> 参考 [[C 语言：标准 IO 流#模式]] 和 [[Linux 系统编程：文件 IO#打开文件：`open()`]]
> 

| 模式                 | 描述      | C 标准输入输出模式 | `open` 标志                |
| :----------------- | :------ | ---------- | ------------------------ |
| `ios_base::in`     | 读操作     | `"r"`      | `O_RDONLY`               |
| `ios_base::out`    | 写操作     | `"w"`      | `O_WRONLY` \| `O_TRUNC`  |
| `ios_base::app`    | 写操作     | `"a"`      | `O_WRONLY` \| `O_APPEND` |
| `ios_base::ate`    | 写操作     |            |                          |
| `ios_base::trunc`  | 截断文件    |            | `O_TRUNC`                |
| `ios_base::binary` | 操作二进制文件 | `"b"`      | 不区分二进制和文本                |

### 文件读对象 ifstream

如果只是从文件中读取数据，请使用 `ifstream`  类的对象。类 `ifstream` 提供下列几个构造函数

```cpp
ifstream();
explicit ifstream(const char* filename, openmode mode = ios_base::in);
explicit ifstream(const string & filename, openmode mode = ios_base::in);
```

使用默认构造函数创建的 `ifstream` 对象是没有关联文件的。因此，需要额外调用成员函数 `open()` 用于打开文件

```cpp hl:9
#include <iostream>
#include <fstream>

using namespace std;

int main(){
    ifstream in;
    in.open("test.txt");  // c 风格字符串
    if(!in) {
        cout << "打开文件失败" << endl;
        return 1;
    }
    in.close();
    return 0;
}
```

`!in` 调用了 `operator!()` 运算符函数，用于检查流是否存在错误

> [!tip] 
> 
> 下图展示了 `ifstream` 的继承关系
> 
> ![[Pasted image 20250206104549.png]]
> 
> 由于 `basic_ios` 重载了 `operator bool()` 和 `operator!()`
> + `operator!()`：若关联流上 **已出现错误** 则返回 **true**
> + `operator bool()`：检查流是否无错误。明确说即返回 `!fail()`
> 

#### 读取一行

> [!tip] 
> 
> 格式化输入就是使用 `operator>>()` 运算符，这里不再介绍
> 

`ifstream` 继承了 `istream` 提供的成员函数 `getline(buf, size, delim='\n')` 用于读取一行数据。该函数与标准 C 函数 `fgets()` 类似。 

```cpp
#include <iostream>
#include <fstream>

using namespace std;

int main(){
    ifstream in;
    in.open("test.txt");  // c 风格字符串
    if(!in) {
        cout << "打开文件失败" << endl;
        return 1;
    }
    char buf[1024];
    in.getline(buf, 1024);
    cout << buf << endl;
    in.close();
    return 0;
}
```

> [!tip] 
> 
> 该函数要求我们准备好一个缓冲区用于接收内容，并指定缓冲区的大小。由于我们不清楚一行究竟有多少字节，`getline()` 读取可能会不足一行
> 

为了确保读取一行数据并且不用我们考虑一行的大小，可以使用 `<string>` 中提供的 `getline(istream&, string&, CharT)` 函数用于从 `istream` 中读取一行并存储到 `string` 中

> [!tip] 
> 
> 这里必须使用 C++ 风格的字符串
> 

```cpp hl:14-15
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int main(){
    ifstream in;
    in.open("test.txt");  // c 风格字符串
    if(!in) {
        cout << "打开文件失败" << endl;
        return 1;
    }
    string s;
    getline(in, s);
    cout << s << endl;
    in.close();
    return 0;
}
```

#### 读取一块

如果希望读取指定数量的字符，可以使用成员函数 `read(buf, count)`。和 C 标准输入输出库的 `fread()` 类似

```cpp
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int main(){
    ifstream in;
    in.open("test.txt");  // c 风格字符串
    if(!in) {
        cout << "打开文件失败" << endl;
        return 1;
    }
    char buf[1024] = {0};
    in.read(buf, sizeof(buf));
    cout << buf << endl;
    in.close();
    return 0;
}
```

#### 随机读取

如果想要在任意位置开始读取文件内容，可以使用成员函数 `seekg(pos)` 或者 `seek(offset, seekdir)` 指定可以从 `seekdir` 位置偏移 `offset` 个字符开始读取

![[Pasted image 20250206154656.png]]

如果想要一次读取文件的全部内容，首先第一步就是获取文件大小。通过 `seekg()` 和 `tellg()` 获取当流的大小，然后再将文件偏移设置为 `0` ，完成上述两步之后就可以开始读完整个文件了

```cpp
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int main(){
    ifstream in;
    in.open("test.txt");  // c 风格字符串
    if(!in) {
        cout << "打开文件失败" << endl;
        return 1;
    }
    // 获取文件的大小
    in.seekg(0, ios::end);    // 文件指针移动到文件末尾
    auto length = in.tellg(); // 获取文件指针的位置
    in.seekg(0, ios::beg);    // 文件指针移动到文件开头
    char *buf = new char[length];
    in.read(buf, length);
    cout << buf << endl;
    in.close();
    return 0;
}
```

### 文件写对象 ofstream

`ofstream` 类封装了用于将数据写入文件的操作。该类从 `ostream` 派生而来

![[Pasted image 20250206160943.png]]

> [!tip] 
> 
> `ostream` 支持的操作在 `ofstream` 中依旧支持
> 

```cpp
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int main(){
    ifstream in;
    in.open("test.txt");  // c 风格字符串
    if(!in) {
        cout << "打开文件失败" << endl;
        return 1;
    }
    ofstream out("out.txt");
    if(!out) {
        cout << "打开文件失败" << endl;
        return 1;
    }

    string str;
    while (getline(in, str))
    {
        out.write(str.c_str(), str.size());
        out.put('\n');
    }
    in.close();
    out.close();
    return 0;
}
```

## 字符串 IO 流

字符串I/O是内存中的字符串对象与字符串输入输出流对象之间做内容传输的数据流，通常用来做格式转换。

### 字符串输入对象 istringstream

**将字符串的内容传输给字符串输入流对象，再通过这个对象进行字符串的处理（解析）**

创建字符串输入流对象时传入 C++ 风格的字符串，字符串的内容就被保存在了输出流对象的缓冲区中。之后可以通过输入流运算符将字符串内容输出给不同的变量，起到了字符串分隔的作用。

例如，我们可以通过 `istrinstream` 将字符串转换为整数

```cpp
void test0(){
    string s("123 456");
    int num = 0;
    int num2 = 0;
    //将字符串内容传递给了字符串输入流对象  
    istringstream iss(s);
    iss >> num >> num2;
    cout << "num:" << num << endl;
    cout << "num2:" << num2 << endl;
}
```

### 字符串输出对象 ostringstream

通常的用途就是将各种类型的数据转换成字符串类型

``` c++
void test0(){
    int num = 123, num2 = 456;
    ostringstream oss;
    //把所有的内容都传给了字符串输出流对象
    oss << "num = " << num << " , num2 = " << num2 << endl;
    cout << oss.str() << endl;
}
```

将字符串、int 型数据、字符串、int 型数据统统传给了字符串输出流对象，存在其缓冲区中，利用它的 str 函数，全部转为 string 类型并完成拼接。

