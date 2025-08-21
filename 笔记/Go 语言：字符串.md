# 字符串

在 [[Go 语言：基本概念#字符类型]] 中我们介绍了计算机是如何存储字符。对于单个字符的处理往往是比较少的，通常我们需要处理字符序列，即 **字符串**

Go 认为字符串是不可变的，这和大多数语言一样。Go 中字符串采用顺序表存储每个字符的编码，默认采用 UTF-8 格式存储。因此，存储字符串只需要存储字符串的 **字节序列的起始地址** 和 **字节序列的长度**。如下字符串 `"hello, 世界!"` 的 UTF-8 编码为

```shell
"hello, 世界!"
字符 编号(hex)  位模式
h    68        01101000
e    65        01100101
l    6c        01101100
l    6c        01101100
o    6f        01101111
,    2c        00101100
     20        00100000
世   4E16      01001110 00010110
界   754C      01110101 01001100
!    21        00100001

UTF-8 编码为
01101000 01100101 01101100 01101100 01101111 00101100 00100000 11100100 10111000 10010110 11100111 10010101 10001100 00100001
```

在内存中按照下图方式进行存储的

![Pasted image 20250608000806|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755786843321-503cde8bdb4e40f382cfcb3e2cb8ba1f.png)


其中，`ptr` 指向及字节数组的起始地址；`len` 是字节数组的长度（存放字符串需要的字节数）

> [!important] 
> 
> 表示字符串的字节数组被存放在 **只读内存区域**。字节数组中存储的是字符串中每个字符的 **UTF-8 编码**
> 

## 字符串操作

### 索引

字符串在内存中 **按字节数组存储**。对字符串索引是 **按照字节的偏移量**。`s[i]` 获取索引 `i` 处的 UTF-8 编码的一个字节

```go
s1 := "abc"                          
s2 := "测试"                          

b1 := []byte(s1) // []byte() 类型转换。[]byte{} 字节切片字面量
b2 := []byte(s2) // []byte() 类型转换。[]byte{} 字节切片字面量
fmt.Println(b1)  // [97 98 99]
fmt.Println(b2)  // [230 181 139 232 175 149]
	
fmt.Println(s1[0], s1[1], s1[2]) // 97 98 99
fmt.Println(s2[0], s2[1])        // 230 181
```

### 遍历

遍历字符串时，如果使用 **索引** 遍历，相当于 **字节遍历**

```go
s := "hello, 世界!"
for i := 0; i < len(s); i++ {
	fmt.Printf("%d, %T, %[2]d %[2]c\n", i, s[i])
}
```

为了遍历每个字符，需要使用 `for-range` 进行遍历。在遍历时，Go 语言会将 UTF-8 编码转换为 Unicode 代码

```go
s := "hello, 世界!"
for i, v := range s {
	fmt.Printf("%d, %T: %[2]d %[2]c; %T: %[3]d %[3]c\n", i, s[i], v) // 注意 i 不连续。v 是汉字的 unicode
}
```

### 字符串拼接

在 Go 语言中，字符串可以使用运算符 `+` 将多个字符串拼接成一个字符串。请注意，这会创建一个全新的字符串，而不会修改操作数

```go
package main

import "fmt"

func main() {
	s0 := "hello"
	s1 := "世界"

	s2 := s0 + ", " + s1

	fmt.Printf("&s0 = %p\ts0 = %s\n", &s0, s0)  
	// &s0 = 0xc000012090      s0 = hello
	fmt.Printf("&s1 = %p\ts1 = %s\n", &s1, s1)
	// &s1 = 0xc0000120a0      s1 = 世界
	fmt.Printf("&s2 = %p\ts2 = %s\n", &s2, s2)
	// &s2 = 0xc0000120b0      s2 = hello, 世界
}
```


## 标准包 `strings`

Go 语言的标准包 `strings` 中提供了许多的操作字符串，下面我们会介绍标准包 `strings`

### 拼接

`strings.Join(source []string, sep string)` 用于将字符串切片 `source` 中的每个字符串拼接起来，并且每个字符串之间使用 `sep` 分隔

```go
package main

import (
	"fmt"
	"strings"
)

func main() {
	s := strings.Join([]string{"hello", "world"}, ", ")
	fmt.Printf("%s\n", s)  // hello, world
}
```

`strings.Repeat(s string, count int)` 函数用于将字符串 `s` 重复 `count` 次后返回

```go
package main

import (
	"fmt"
	"strings"
)

func main() {
	s := strings.Repeat("hello ", 2)
	fmt.Printf("%s\n", s)  // hello hello
}
```

此外，在 `strings` 包中还提供了一个类型 `strings.Builder`， 用于通过 `Builder.Write()` 方法高效构建字符串。它最小化内存复制。零值构建器即可使用。不要复制非零构建器

```go
package main

import (
	"fmt"
	"strings"
)

func main() {
	var b strings.Builder
	for i := 3; i >= 1; i-- {
		fmt.Fprintf(&b, "%d...", i)
	}
	b.WriteString("ignition")
	fmt.Println(b.String())

}
```

> [!tip] 
> 
> **简单拼接** 字符串常用`+`、`fmt.Sprintf`
> 
> 如果 **正好有字符串的序列**，可以考虑 `Join`
> 
> 如果 **反复多次拼接**，`strings.Builder` 是推荐的方式
> 

### 查询子串或字符

在字符串中查找子串或字符也是常用的操作，`strings` 包中提供了许多这样的函数

| 函数                                         | 描述                                                                                           |
| :----------------------------------------- | :------------------------------------------------------------------------------------------- |
| `strings.Index(s, sep string)`             | 子串 `sep` 在字符串 `s` 中第一次出现的位置，不存在则返回 `-1`                                                      |
| `strings.LastIndex(s, sep string)`         | 子串 `sep` 在字符串`s`中最后一次出现的位置，不存在则返回`-1`                                                        |
| `strings.IndexByte(s string, c byte)`      | 字符 `c` 在 `s` 中第一次出现的位置，不存在则返回 `-1`                                                           |
| `strings.LastIndexByte(s string, c byte) ` | 字符 `c` 在 `s` 中最后一次出现的位置，不存在则返回 `-1`                                                          |
| `strings.IndexRune(s string, r rune)`      | Unicode 码值 `r` 在 `s` 中第一次出现的位置，不存在则返回 `-1`                                                   |
| `strings.IndexAny(s, chars string)`        | 返回 `s` 中来自 `chars` 中任何 Unicode 代码点的第一个实例的索引，如果 `chars` 中没有任何 Unicode 代码点出现在 `s` 中，则返回 `-1`   |
| `strings.LastIndexAny(s, chars string)`    | 与`IndexAny` 相似，只不过返回最后一次查找到的索引                                                               |
|                                            |                                                                                              |
| `strings.Contains(s, substr string)`       | 报告 `substr` 是否在 `s` 中                                                                        |
| `strings.ContainsAny(s, chars string)`     | 报告字符串 `s` 中是否有 `chars` 中的任何 Unicode 代码点                                                      |
| `func ContainsRune(s string, r rune) bool` | 判断 Unicode 码点 `r` 是否在字符串 `s` 中                                                               |
|                                            |                                                                                              |
| `strings.Count(s, substr string) `         | 计算字符串 `s` 中 **非重叠出现** 的 `substr` 的次数。若 `substr` 为空字符串，则 `Count` 返回 `s` 中 Unicode 代码点的个数 `+1` |

### 大小写转换

将字符串中的所有字符串的转换为大写或者小写也是非常实用的操作

| 函数                          | 描述                   |
| :-------------------------- | :------------------- |
| `strings.ToLower(s string)` | 返回将所有字母都转为对应的小写版本的拷贝 |
| `strings.ToUpper(s string)` | 返回将所有字母都转为对应的大写版本的拷贝 |

### 前后缀检查

有时候我们可能需要检查字符串是否是以某个字符串开头或结尾

| 函数                                    | 描述                       |
| :------------------------------------ | :----------------------- |
| `strings.HasPrefix(s, prefix string)` | 判断 `s` 是否有前缀字符串 `prefix` |
| `strings.HasSuffix(s, suffix string)` | 判断 `s` 是否有后缀字符串 `suffix` |

### 移除字符

字符串常见的处理包括从字符串中移除左右两端指定的字符

| 函数                                           | 描述                                                   |
| :------------------------------------------- | :--------------------------------------------------- |
| `strings.Trim(s string, cutset string)`      | 返回字符串副本，它包含了字符串 `s` **移除左右两端** 在 `cutset` 中的字符后剩下的字符 |
| `strings.TrimSpace(s string)`                | 返回将字符串 `s` 左右两端所有空白（`unicode.IsSpace` 指定）都去掉的字符串     |
| `strings.TrimLeft(s string, cutset string)`  | 同 `Trim`，只是在字符串左端进行移除                                |
| `strings.TrimRight(s string, cutset string)` | 同 `Trim`，只是在字符串右端进行移除​                               |
| `strings.TrimPrefix(s, prefix string)`       | 返回字符串副本，在字符串 `s` 中移除前缀 `prefix`                      |
| `strings.TrimSuffix(s, suffix string)`       | 返回字符串副本，在字符串 `s` 中移除后缀 `suffix`                      |

### 分割

将字符串拆分成一个字符串切片也是常见的处理

| 函数                                          | 描述                                                                                                                                                    |
| :------------------------------------------ | :---------------------------------------------------------------------------------------------------------------------------------------------------- |
| `strings.Fields(s string)`                  | 返回将字符串按照空白（`unicode.IsSpace` 确定，可以是一到多个连续的空白字符）分割的多个字符串。如果字符串全部是空白或者是空字符串的话，会返回空切片                                                                    |
| `strings.Split(s, sep string)`              | 在字符串 `s` 中出现的 `sep` 的方式进行分割，会分割到结尾，并返回生成的所有片段组成的 **切片**（每一个 `sep` 都会进行一次切割，即使两个 `sep` 相邻，也会进行两次切割）。如果 `sep` 为空字符，`Split`会将 `s` 切分成单个字符组成的 `string` 切片 |
| `strings.SplitN(s, sep string, n int)`      | 与 `Split` 类似，`n` 指定返回切片的元素个数。如果 `n < 0` 等价于 `Split`                                                                                                   |
| `strings.SplitAfter(s, sep string)`         | 将字符串 `s` 在每个 **`sep` 的实例之后** 分割成所有子字符串，并返回这些子字符串的切片。                                                                                                  |
| `strings.SplitAfterN(s, sep string, n int)` | 将字符串 `s` 切成多个子串，每次 **在 `sep` 之后切割**，然后返回这些子串的切片。                                                                                                      |
| `strings.Cut(s, sep string)`                | `s` 在第一次出现的 `sep` 周围，返回 `sep` 之前和之后的文本。找到的结果报告 `sep` 是否出现在 `s` 中。如果 `sep` 不在 `s` 中出现，`Cut` 会返回 `s, "", false`。                                        |

### 替换

`strings.Replace(s, old, new string, n int)` 返回一个字符串副本，将 `s` 中的 `old` 子串被 `new` 替换
+ `n < 0`，等价 `ReplaceAll`，全部替换
+ `n == 0`，或 `old == new`，就返回 `s`
+ `n > 0`，至多替换 `n` 次，如果 `n` 超过找到 `old` 子串的出现次数 `x`，也就只能替换 `x` 次了
+ 未找到替换处，就返回 `s`

`strings.Map(mapping func(rune) rune, s string)` 返回一个字符串副本，将 `s` 中的每一个 Unicode 代码值 `r` 使用 `mapping(r)` 的返回值替换。如果 `mapping(r)` 返回一个负值，将丢弃该码值


### `Builder` 类型

`Builder` 用于 **使用 `Builder.Write` 方法高效地构建字符串**。它最大限度地减少了内存复制。零值可以随时使用。不要复制非零 `Builder`。该类型实现了 `io.Writer` 接口

```go
type Builder struct {
	addr *Builder // of receiver, to detect copies by value
	buf  []byte
}
```

#### `Builder` 的方法

```go
// Cap 返回构建器底层字节切片的容量。它是为正在构建的字符串分配的总空间，包括已写入的任何字节。
func (b *Builder) Cap() int

// 必要时，Grow 会增加 b 的容量，以保证再有 n 个字节的空间。在 Grow(n) 之后，至少有 n 个字节可以写入 b 而无需再次分配。
// 如果 n 为负数，Grow 就会 panic。
func (b *Builder) Grow(n int)

// Len 返回累积的字节数；b.Len() == len(b.String())。
func (b *Builder) Len() int

// Reset 将 Builder 设置为空
func (b *Builder) Reset()

// String 返回累积的字符串。
func (b *Builder) String() string

// Write 将 p 的内容追加到 b 的缓冲区。Write 总是返回 len(p)、nil。
func (b *Builder) Write(p []byte) (int, error)

// WriteByte 将字节 c 附加到 b 的缓冲区。返回的错误总是 nil。
func (b *Builder) WriteByte(c byte) error

// WriteRune 将 Unicode 代码点 r 的 UTF-8 编码追加到 b 的缓冲区。它将返回 r 的长度和 nil 错误信息
func (b *Builder) WriteRune(r rune) (int, error)

// WriteString 将 s 的内容追加到 b 的缓冲区。它返回 s 的长度和 null 错误信息
func (b *Builder) WriteString(s string) (int, error)
```

#### 示例

```go
package main

import (
	"fmt"
	"strings"
)

func main() {
	var b strings.Builder
	for i := 3; i >= 1; i-- {
		fmt.Fprintf(&b, "%d...", i)
	}
	b.WriteString("ignition")
	fmt.Println(b.String())
}
```

### `Reader` 类型

`Reader` 通过读取字符串实现了`io.Reader`、`io.ReaderAt`、`io.ByteReader`、`io.ByteScanner`、`io.RuneReader`、`io.RuneScanner`、`io.Seeker` 和`io.WriterTo` 接口。读取器的零值操作类似于空字符串的读取器

```go
type Reader struct {
	s        string
	i        int64 // current reading index
	prevRune int   // index of previous rune; or < 0
}
```

#### 构造函数 `strings.NewReader`

`NewReader` 返回一个从 `s` 读取数据的新 `Reader`。它类似于 `bytes.NewBufferString`，但效率更高，且不可写入

```go
func NewReader(s string) *Reader
```

#### `Reader` 的方法

```go
// Len 返回字符串未读部分的字节数
func (r *Reader) Len() int

// 读取实现了 io.Reader 接口
func (r *Reader) Read(b []byte) (n int, err error)

// ReadAt 实现了 io.ReaderAt 接口
func (r *Reader) ReadAt(b []byte, off int64) (n int, err error)

// ReadByte 实现了 io.ByteReader 接口
func (r *Reader) ReadByte() (byte, error)

// ReadRune 实现了 io.RuneReader 接口
func (r *Reader) ReadRune() (ch rune, size int, err error)

// 重置 reader，使其从 s 读取数据
func (r *Reader) Reset(s string)

// Seek 实现了 io.Seeker 接口
func (r *Reader) Seek(offset int64, whence int) (int64, error)

// Size 返回底层字符串的原始长度
// Size 是可通过 Reader.ReadAt 读取的字节数
// 返回值始终保持不变，不会受调用任何其他方法的影响
func (r *Reader) Size() int64

// UnreadByte 实现了 io.ByteScanner 接口
func (r *Reader) UnreadByte() error

// UnreadRune 实现了 io.RuneScanner 接口
func (r *Reader) UnreadRune() error

// WriteTo 实现了 io.WriterTo 接口
func (r *Reader) WriteTo(w io.Writer) (n int64, err error)
```

### `Replacer` 类型

`Replacer` 将字符串列表替换为替换字符串。多个程序可同时安全使用。

```go
type Replacer struct {
	once   sync.Once // guards buildOnce method
	r      replacer
	oldnew []string
}
```

#### 构造函数: `string.NewReplacer`

`NewReplacer` 会从新旧字符串对列表中返回一个新的 `Replacer`。替换会按照它们在目标字符串中出现的顺序进行，不会出现重叠匹配。旧字符串比较按参数顺序进行。如果参数数目为奇数，`NewReplacer` 就会 `panic`。

```go
func NewReplacer(oldnew ...string) *Replacer
```

#### 方法

```go
// Replace 返回包含所有替换操作的 s 副本
func (r *Replacer) Replace(s string) string

// WriteString 将 s 写入 w，并执行所有替换操作
func (r *Replacer) WriteString(w io.Writer, s string) (n int, err error)
```

#### 示例

```go
package main

import (
	"fmt"
	"strings"
)

func main() {
	r := strings.NewReplacer("<", "&lt;", ">", "&gt;")
	fmt.Println(r.Replace("This is <b>HTML</b>!"))
}
```

## 标准包 `strcov` 

有时候我们需要将字符串转化为其他类型的数据，比如需要将 `"3.14"` 转换为 `float64` 类型的值。在 Go 语言中，强制类型转换的语法是不适合这种转换的，需要使用其他方式完成。因此，Go 语言提供了标准包 `strcov` 用于支持此类转换

> [!tip] 
> 
> `strconv` 包实现了基本数据类型与其字符串表示的转换，主要有以下常用函数： `Atoi()`、`Itoa()`、`Parse` 系列(字符串转换为基本数据类型的值)、`Format` 系列(基本类型的值转换为字符串)、`Append` 系列
> 

### `string` 与 `int` 类型转换

这一组函数是我们平时编程中用的最多的

#### `Atoi()`

`Atoi()`函数用于将字符串类型的整数转换为 `int` 类型，函数签名如下。

```go
func Atoi(s string) (i int, err error)
```

如果传入的字符串参数无法转换为 `int` 类型，就会返回错误

#### `Itoa()`

`Itoa()`函数用于将 `int` 类型数据转换为对应的字符串表示，具体的函数签名如下。

```go
func Itoa(i int) string
```

> [!tip] 
> 
> 这是 C 语言遗留下的典故。C 语言中没有 `string` 类型而是用字符数组(`array`)表示字符串，所以 `Itoa` 对很多C系的程序员很好理解
> 

```go
func test2() {
	s1 := "100"
	// 字符串转换为 int 类型
	i1, err := strconv.Atoi(s1)
	if err != nil {
		fmt.Println("can't convert to int")
	} else {
		fmt.Printf("type:%T value:%#v\n", i1, i1) //type:int value:100
	}
	// int 类型转换为 字符串
	fmt.Println("int 类型值转换为字符类型", strconv.Itoa(i1))

}
```

### `Parse` 和 `Format` 系列函数

`Parse` 类函数用于 **转换字符串为给定类型的值**：`ParseBool()`、`ParseFloat()`、`ParseInt()`、`ParseUint()`。

`Format` 系列函数实现了将给定类型数据格式化为 `string` 类型数据的功能

#### `ParseBool()` 和 `FormatBool()`

这两个函数的签名如下：

```go
func ParseBool(str string) (value bool, err error) // 返回字符串表示的 bool 值
func FormatBool(b bool) string  // 根据b的值返回 "true" 或 "false"。
```

在 `strconv` 包中的定义定义如下

```go
package strconv

// ParseBool returns the boolean value represented by the string.
// It accepts 1, t, T, TRUE, true, True, 0, f, F, FALSE, false, False.
// Any other value returns an error.
func ParseBool(str string) (bool, error) {
	switch str {
	case "1", "t", "T", "true", "TRUE", "True":
		return true, nil
	case "0", "f", "F", "false", "FALSE", "False":
		return false, nil
	}
	return false, syntaxError("ParseBool", str)
}

// FormatBool returns "true" or "false" according to the value of b.
func FormatBool(b bool) string {
	if b {
		return "true"
	}
	return "false"
}
```

示例

```go
func test2() {
	s1 := "true"
	// 字符串 ==> bool 类型的值 Parse
	b, err := strconv.ParseBool(s1)
	if err != nil {
		fmt.Println(err)
	}
	fmt.Printf("%T, %t\n", b, b) // bool, true

	// bool 类型的值转换为字符串 Format
	fmt.Println(strconv.FormatBool(b))
}
```

#### `ParseInt()` 和 `FormatInt()`

```go
// 返回字符串表示的整数值，接受正负号
// base指定字符串的进制（2到36），如果base为0，则会从字符串前置判断，“0x"是16进制，“0"是8进制，否则是10进制；
// bitSize 指定结果必须能无溢出赋值的整数类型，0、8、16、32、64 分别代表 int、int8、int16、int32、int64；
func ParseInt(s string, base int, bitSize int) (i int64, err error)

// 返回 i 的base进制的字符串表示。base 必须在2到36之间，结果中会使用小写字母’a’到’z’表示大于10的数字。
func FormatInt(i int64, base int) string
```

```go
func test3() {
	// 整数->字符串  Format   字符串->整数
	s3 := "100000"
	// 整数： 数字、进制、大小
	// 参数：1、str   2、 进制（10）  3、大小
	i1, _ := strconv.ParseInt(s3, 10, 64)
	fmt.Printf("%T,%d\n", i1, i1) // int64,100

	s4 := strconv.FormatInt(i1, 10)
	fmt.Printf("%T,%s\n", s4, s4) // string,100000
}
```

####  `ParseUnit()` 和 `FormatUint()`

```go
// ParseUint类似ParseInt但不接受正负号，用于无符号整型。
func ParseUint(s string, base int, bitSize int) (n uint64, err error)

// 是FormatInt的无符号整数版本。
func FormatUint(i uint64, base int) string
```

####  `ParseFloat()` 和 `FormatFloat()`

```go
// 解析一个表示浮点数的字符串并返回其值。
// 如果 s 合乎语法规则，函数会返回最为接近 s 表示值的一个浮点数（使用IEEE754规范舍入）。
// bitSize指定了期望的接收类型，32是float32（返回值可以不改变精确值的赋值给float32），64是float64；
// 返回值err是*NumErr类型的，语法有误的，err.Error=ErrSyntax；结果超出表示范围的，返回值f为±Inf，err.Error= ErrRange。
func ParseFloat(s string, bitSize int) (f float64, err error)

// 函数将浮点数表示为字符串并返回
// bitSize表示f的来源类型（32：float32、64：float64），会据此进行舍入。
// fmt 表示格式：'f'（-ddd.dddd）、'b'（-ddddp±ddd，指数为二进制）、'e'（-d.dddde±dd，十进制指数）、'E'（-d.ddddE±dd，十进制指数）、'g'（指数很大时用’e’格式，否则’f’格式）、'G'（指数很大时用'E'格式，否则'f'格式）。
// prec控制精度（排除指数部分）：对 'f'、'e'、'E'，它表示小数点后的数字个数；对'g'、'G'，它控制总的数字个数。如果prec 为 -1，则代表使用最少数量的、但又必需的数字来表示 f。
func FormatFloat(f float64, fmt byte, prec, bitSize int) string
```

```go
func test4() {
	f, err := strconv.ParseFloat("3.1415", 64)
	if err != nil {
		fmt.Println(err)
	}
	fmt.Printf("%T, %v\n", f, f)
	fmt.Println(strconv.FormatFloat(f, 'E', 10, 64))
}
```

### 其他

#### `IsPrint()`

```go
// 返回一个字符是否是可打印的，和 unicode.IsPrint 一样，r 必须是：字母（广义）、数字、标点、符号、ASCII空格。
func IsPrint(r rune) bool
```

#### `CanBackquote()`

```go
// 返回字符串 s 是否可以不被修改的表示为一个单行的、没有空格和 tab 之外控制字符的反引号字符串
func CanBackquote(s string) bool
```
