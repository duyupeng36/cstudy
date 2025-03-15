# 基础语法

## Go 程序的结构

在 [[Go 语言：环境搭建]] 中编写了我们的第一个 Go 程序。下面我们介绍这个程序的结构

```go
package main  
  
import "fmt"  
  
func main() {  
    fmt.Println("hello world")  
}
```
 
Go 语言的代码通过 **包(package)** 组织，包类似于 C/C++ 中的库或者 Python 中的模块。一个包由位于单个目录下的 `.go` 源代码文件组成，目录定义包的作用域。该目录下的所有 `.go` 文件都必须以 `package` 声明语句开始。例如，上例中的 `package main`，表示该文件属于 `main` 包

> [!tip] 
> 
> Go 的标准库提供了 $100$ 多个包，以支持常见功能，如输入、输出、排序以及文本处理。比如 `fmt` 包，就含有格式化输出、接收输入的函数。 `Println` 是其中一个基础函数，可以打印以空格间隔的一个或多个值，并在最后添加一个换行符，从而输出一整行。
> 

> [!attention] 特殊的 `main` 包：它定义了一个 **独立可执行的程序**，而不是一个库
> 
> 在 `main` 包里的 `main`  函数也很特殊，它是整个程序执行时的入口，类似于 C/C++ 语言
> 
> `main` 函数所做的事情就是程序做的。当然了， `main` 函数一般调用其它包里的函数完成很多工作, 比如, `fmt.Println`
> 

在包定义后，紧跟着一系列的包 **导入** 语句，用于导入程序依赖的包。导入语句用于告知编译器，编译该源文件需要哪些包。

> [!tip] 
> 
> 必须恰当导入需要的包，缺少了必要的包或者导入了不需要的包，程序都无法编译通过。
> 
> 这个特性避免了程序开发过程中引入未使用的包。注意，Go 语言的编译器只有错误，没有警告
> 

导入语句必须在包声明语句之后。随后，则是组成程序的函数、变量、常量、类型的声明语句

## 注释

在计算机语言中，**注释** 是计算机语言的一个 **重要组成部分**，用于 **在源代码中解释代码的功用**，可以 **增强程序的可读性，可维护性**，或者 **用于在源代码中处理不需运行的代码段**，来调试程序的功能执行

Go 语言从 C 语言中继承了 `/* */` 和  `//` 两种注释语法。其中 `/* */` 称为块注释，在 `/*` 和 `*/` 之间的内容称为注释；`//` 称为行注释，从 `//` 开始直到当前物理行结束

> [!tip] 
> 
> `//` 行注释是常态。`/**/` 块注释通过用于禁用一大段代码
> 

```go
// main 包注释，解释包的作用
package main

import "fmt"

// add takes two integers and returns the sum of them
// 这是函数的说明文档
func add(a, b int) int {
	// This is a comment
	return a + b
}

func main() {
	/* this is also a comment */
	result := add(1, 2) // Call function add
	fmt.Println(result) // Print result (3)
}
```

请注意：通常，对函数的注释写在函数上方；对包的注释写在包声明语句的上方。

## 命名

Go 语言中对程序组件的命名采用一个简单的规则：必须以下划线或者 Unicode 字母开头；后面紧跟着任意数量的字母、数字和下划线。

> [!attention] 
> 
> Go 语言中的名字区分大小写的。例如，`heapSort` 和 `HeapSort` 是两个不同的名字
> 

此外，**关键字** 同样不能用作名字。因为关键字被编译器使用，如果将关键字定义成名字，会导致编译失败。Go 语言定义了下面 $25$ 个关键字

```go
break        default      func         interface    select
case         defer        go           map          struct
chan         else         goto         package      switch
const        fallthrough  if           range        type
continue     for          import       return       var
```

此外，还有大约 $30$ 多个 **预定义的名字**，它们在 Go 语言中具有默认的含义，不建议重新定义这些名字

```go
Types: 类型
	any    bool    byte    comparable
	complex64    complex128    error    float32     float64
	int    int8    int16    int32    int64    rune    string
	uint   uint8   uint16   uint32   uint64   uintptr
	
Constants: 常量
	true    false    iota
	
Zero value: 零值
	nil
	
Functions: 函数
	append    cap    close    complex    copy    delete    imag    len
	make    new    panic    print    println    real    recover
```

如果一个名字是在函数内部定义，那么它的就只在函数内部有效。如果是在函数外部定义，那么将在当前包的所有文件中都可以访问。名字的开头字母的大小写决定了名字在包外的可见性。如果一个名字是大写字母开头的，那么它将是导出的，也就是说可以被外部的包访问。包本身的名字一般总是用小写字母

> [!tip] 
> 
> 在函数外部定义的名字，如果其名字是以大写字母开头，则它是导出的。换句话说，就是一个 **以大写字母开头的名字可以在包为被访问**
> 

在习惯上，Go语言程序员推荐使用 **驼峰式** 命名，当名字有几个单词组成的时优先使用大小写分隔，而不是优先用下划线分隔(蛇形命名)，通常 Python 更希望采用蛇形命名

像 `ASCII` 和 `HTML` 这样的缩略词则避免使用大小写混合的写法，它们可能被称为`htmlEscape`、`HTMLEscape` 或 `escapeHTML`

> [!tip] 
> 
> 缩略词的命名规则：要么全是大写，要么全是小写。不要大小写混合
> 

## 声明

声明语句定义了程序的各种实体对象以及部分或全部的属性。Go 语言主要有四种类型的声明语句：`var` `const` `type` 和 `func`

| 关键字     | 描述   |
| :------ | :--- |
| `var`   | 声明变量 |
| `const` | 声明常量 |
| `type`  | 声明类型 |
| `func`  | 声明函数 |

### 变量

这里我们重点放在变量的声明。下面的例程中声明了一个 **常量** 和两个 **变量**

```go
// boliing.go: 程序打印出水的沸点
package main

import "fmt"

const boliingF = 212.0

func main() {
	var f = boliingF  // 声明了一个变量，其类型由 boliingF 的类型推断
	var c = (f - 32) * 5 / 9
	fmt.Printf("boiling point = %g°F or %g°C\n", f, c)
}
```

声明常量使用关键字 `const`，而声明变量使用关键字 `var`

> [!tip] 
> 
> 上述名字 `boliingF` 是在包内函数外声明的，对于包内的所有文件都是可见的。我们通常称这样的名字为 **全局名字**
> 
> 名字 `f` 和 `c` 声明在函数体内，只能在函数体内可见。这种只能在函数体内可见的名字称为 **局部名字**
> 

下面我们详细介绍变量的声明。和 C/C++ 一样，在 Go 语言中，变量必须先使用 `var` 关键字进行声明后才能使用。

> [!attention] 
> 
> 请注意：**不允许重复声明**！同时，Go 语言要求 **声明后的变量必须使用**
> 

Go 语言中，变量声明语法如下

```go
var 变量名 类型 = 表达式
```

其中，`"类型"` 和 `= "表达式"` 可以任意省略其中一项

```go
package main

import "fmt"

func main() {
	// 完整的变量声明语法
	var name string = "dyp"
	var age int = 28
	fmt.Printf("I'm %s. My age is %d", name, age)
}
```

如果没有指定类型，则变量的类型由表达式的类型确定。

```go
package main

import "fmt"

func main() {

	var name = "dyp" // 自动类型推导
	var age = 28     // 自动类型推到

	fmt.Printf("%#v, %#v\n", name, age)                                              // 字符串的零值是 ""，整数的零值是 0
	fmt.Printf("variable name, which is %T. variable age, which is %T\n", name, age) // %T 用于查看变量的类型： variable name, which is string. variable age, which is int
}
```

如果省略的表达式，则变量将采用 **零值初始化**

```go
package main

import "fmt"

func main() {

	var name string // 零值初始化
	var age int     // 零值初始化

	fmt.Printf("%#v, %#v\n", name, age)  // 字符串的零值是 ""，整数的零值是 0

	fmt.Printf("variable name, which is %T. variable age, which is %T\n", name, age) // %T 用于查看变量的类型： variable name, which is string. variable age, which is int
}
```

> [!tip] 
> 
> 零值初始化机制：Go 语言中，所有的变量都是初始化的，不存在未初始化的变量。这个特性简化了很多代码，并且没有增加额外工作的前提下确保边界条件下的合理行为
> 

对于不同的类型的变量 Go 语言规定了不同的零值。下表列出了 Go 语言基本类型的零值

| 类型                                  | 零值         |
| :---------------------------------- | ---------- |
| 数字(包括整数和浮点数)                        | $0$        |
| 布尔类型                                | `false`    |
| 字符串类型                               | `""`(空字符串) |
| 接口                                  | `nil`      |
| 引用类型(包括 `slice` `map` `chan` 指针和函数) | `nil`      |

和 C/C++ 一样，可以同时声明一组同类型的变量

```go
var i, j, k int  // 声明 3 个 int 类型的变量
var i, j, k = 2, 3, 4 // 类型自动推到
```

不同的是，Go 语言中支持一次声明一组不同类型的变量

```go
var (
	name string = "dyp"
	age  int    = 28
)
```

> [!attention] 
> 
> 请注意：初始化表达式可以是任何字面值或者任意的表达式
> 
> + 在包中的全局变量(包级变量)会在进入 `main` 函数之前完成初始化
> + 在函数中声明的局部变量需要等待声明语句执行时才完成初始化
> 

例如，如果声明一组变量，那么这组变量可以使用函数的返回值进行初始化

```go
var f, err = os.Open(path)  // os.Open() 返回文件指针和错误
```

#### 短变量声明

Go 语言中，如果在函数内部声明变量，可以采用一种简短形式的声明语句，即 `名字 := 表达式` 的声明形式

```go
package main

func main() {
	/*短变量声明*/
	name := "tome"
	age := 28
	ok := true
}
```

> [!important] 
> 
> 短变量声明必须提供初始值，因为编译器需要从初始值中推断变量的类型
> 

和使用关键字 `var` 的声明语句一样，短变量声明也可以声明声明一组变量

```go
i, j := 0, 1

// 函数的返回值来声明和初始化变量
f, err := os.Open(name)
if err != nil {
	return err
}
// ....
f.Close()
```

请注意：这种声明应该只用在可以提高代码可读性的地方

在短变量声明中，至少保证有 $1$ 个新变量的声明，其他已声明的变量就会被 **重新赋值**。例如，在下面的代码中，第一个语句声明了 `in` 和 `err` 两个变量。在第二个语句只声明了 `out` 一个变量，然后对已经声明的 `err` 进行了赋值操作

```go
in, err := os.Open(infile)
// ...
out, err := os.Create(outfile)
```

> [!attention] 
> 
> 请注意：短变量声明语句中 必须 **至少** 要声明一个新的变量
> 

下面的代码将不能编译通过

```go
f, err := os.Open(infile)
// ...
f, err := os.Create(outfile) 
// compile error: no new variables on left side of :=
```

#### 匿名变量

如果我们需要接收到多个值，有一些值使用不到，可以使用 **下划线 `_` 表示变量名称**，这种变量叫做 **匿名变量**

```go
package main

import "fmt"

func getNameAndAge() (string, int) {
	return "杜宇鹏", 30
}

func main() {
	/* 使用匿名变量忽略值 */
	name, _ := getNameAndAge()
	fmt.Printf("name: %s\n", name)
}
```

> [!attention] 
> 
> 匿名变量就像一个黑洞一样，只是简单的将值丢弃。换句话说，后续我们无法通过 `_` 访问值
> 

### 常量

Go 语言中，声明常量必须使用关键字 `const`。当使用 `const` 声明常量时，必须提供初始值。声明完成之后，常量就不允许修改了

```go
const age int = 10        // 字面值
const name = "dyp"        // 字面值
const nextyear = age + 10 // 常量表达式
```

> [!attention] 
> 
> 请注意：Go 语言的常量的初始化器必须是能够在编译时求值的表达式。换句话说，常量的初始化器必须是 **常量表达式**：所谓的常量表达式就是 **只由常量和字面值构成的表达式**
> 

请注意：Go 语言中不允许声明常量数组，这是与 C/C++ 不同的

```go
package main

func main() {
	const e = [2]int{1, 2}  
	// [2]int{…} (value of type [2]int) is not constant
}
```

#### 类型化常量和非类型化常量

Go 将常量划分两组：**类型化常量**(`typed constant`)和**非类型化常量**(`untyped constant`)

类型化常量：使用 `const` 关键字定义常量时，明确说明了这个常量的类型。例如，常量 `age` 如下声明，明确了 `age` 是 `int` 类型

```go
const age int = 10        // age 是类型化常量
```

非类型化常量：使用 `const` 关键字定义常量是，没有说明这个常量的类型。例如，常量 `name` 如下声明，没有指定 `name` 是什么类型。

```go
const name = "dyp"        // name 是非类型化的常量
```

> [!tip] 
> 
> 所有的 **字面值都是非类型化常量**，非类型化常量具有一个默认类型 
> 
> + 非类型化常量在需要时可以自动转换为当前上下文需要的类型。
> 

非类型化常量可以赋值给类型化常量，因为在需要的时候，**非类型化常量可以被 Go 编译器 _自动转换_ 为当前上下文需要的类型**。如下示例，说明了非类型化常量可以自动进行类型转换

```go
const a = 10
var b float64 = 10

result := a + b // 非类型化常量 a 被编译器自动转换为了 float64
fmt.Println(result)  // 20
```

如果 `a` 是类型化常量，`a+b` 则会报错

```go
const a int = 10  // 类型化常量
var a float64 = 10

result := a + age 
// invalid operation: a + age (mismatched types float64 and int)

fmt.Println(result)
```

当然，并不是所有非类型化常量都能自动转换完成。Go 自动给类型转换是禁止窄化类型转换。例如，

```go
const PI = 3.1415926535
var r = 10
result := PI * r * r // 错误 PI (untyped float constant 3.14159) truncated to int
```

这里 `PI` 是非类型化常量，在执行类转换时，将 `float64` 类型转换为 `int` 类型发生截断。Go 不允许这样的自动类型转换

#### 批量定义常量

在一行，使用 `,` 分隔每个常量标识符

```go
const a, b, c int = 2, 3, 4  // 批量声明同一类型化常量
const d, e, f = 6, 7.7, "tom" // 批量声明非类型化常量
```

使用括号 `()` 可以在多行上进行批量声明类型化或非类型化的常量

```go
const (
	name  = "tom"
	age   = 19
	hobby = "ping pang"
)
```

使用括号 `()` 批量声明常量时，除第一个常量声明外，其余常量均可以 **省略初始值**。如果省略初始值，则初始值使用 **上一行表达式重新计算的值**

```go
package main

import "fmt"

func main() {

	const (
		k = 1      // 第一行不能省略，此时 iota 被重置为 0
		m          // 等于上一行的表达式的值
		g          // 1
		t = k + 10 // 新的表达式
		p          // 上一行表达式的值 11
		q          // 11
	)
	
	fmt.Println(k, m, g, t, p, q)
	// 1 1 1 11 11 11
}
```

#### 特殊常量 iota

Go 语言提供了一个预定义标识符 `iota`  是一个 **_特殊的常量_**，可以被编译器修改，只要 `const` 关键字一出现就被设置为 $0$

> [!tip] 
> 
> 常量 `iota` 只能在常量表达式中使用
> 

```go
package main

import "fmt"

func main() {

	const iota1 = iota // iota 是一个特殊的常量，可以被编译器修改的常量
	const iota2 = iota
	fmt.Println(iota1, iota2) // 0 0
}
```

`const` 中每新增一行常量声明将使 `iota` 计数一次(加`1`) 

```go
const (
	a, b = iota + 1, iota + 2 //1, 2
	c, d                      //2, 3
	e, f                      //3, 4
)
```

`const` 声明的常量标识符只有一个下划线(`_`) 时，称为 **匿名常量**。该常量的 **值会被丢弃**，就像黑洞一样，不能使用 `_` 引用常量值

```go
// 批量写iota从0开始，即使第一行没有写iota，iota也从第一行开始从0开始增加
const (
	a = iota // 0
	b        // 1
	c        // 2
	_        // 按道理是3，但是丢弃了
	d        // 4
	e = 10   // 10
	f        // 10
	g = iota // 6
	h        // 7
)
// 可以认为 Go 的 const 批量定义实现了一种重复上一行机制的能力
```

利用 `iota` **智能重复计算上一行表达式的值**，可以用于定义一些枚举值。例如，**定义数量级**

```go
package main

import "fmt"

func main() {
	const (
		_  = iota
		KB = 1 << (10 * iota) // 1 << (10 * 1)
		MB                    // 1 << (10 * 2)
		GB                    // 1 << (10 * 3)
		TB                    // 1 << (10 * 4)
		PB                    // 1 << (10 * 5)
	)
	fmt.Println(KB, MB, GB, TB, PB)
}
```

这里的 `<<` 表示 **左移操作**，`1 << 10`表示将 `1` 的二进制表示向左移 `10`  位， 也就是由 `1` 变成了 `0b10000000000`，也就是十进制的 `1024`。 同理 `2 << 2` 表示将 `2` 的二进制表示向左移 `2` 位，也就是由 `0b10` 变成了 `0b1000`， 也就是十进制的 `8`

## 基本数据类型

虽然从底层而言，**所有的数据都是由 _位_ 组成**，但计算机一般操作的都是长度固定的位模式。

Go 语言提供了 $4$ 种数据类型：**基本类型**、**复合类型**、**引用类型**和 **接口类型**

| 数据类型 | 描述                  |
| :--- | :------------------ |
| 基本类型 | 数字 字符串和布尔类型         |
| 复合类型 | 数组和结构体              |
| 引用类型 | 指针 切片 map 函数 和 chan |
| 接口类型 |                     |

下面我们首先学习 Go 语言中的基本数据类型

### 整数类型（整型）

与 C/C++ 类似，Go 语言将整数类型分为 **有符号整数** 和 **无符号整数** 两大类；同时考虑不同的长度。下表列出了 Go 语言支持的整数类型

| 整数类型     | 尺寸     | 范围                          |
| :------- | :----- | --------------------------- |
| `int8`   | $8$ 位  | $[-128, 127]$               |
| `int16`  | $16$ 位 | $[-32768, 32767]$           |
| `int32`  | $32$ 位 | $[-2146483648, 2147483647]$ |
| `int64`  | $64$ 位 | $[-2^{63}, 2^{63}-1]$       |
| `int`    | 根据平台决定 |                             |
|          |        |                             |
| `uint8`  | $8$ 位  | $[0, 255]$                  |
| `uint16` | $16$ 位 | $[0, 65535]$                |
| `uint32` | $32$ 位 | $[0, 2^{32}-1]$             |
| `uint64` | $64$ 位 | $[0, 2^{64}-1]$             |
| `uint`   | 根据平台决定 |                             |

> [!tip] 
> 
> 最广泛使用的整数类型是 `int` 类型。有符号整数采用 **补码** 编码^[[[数据存储#二进制补码表示法]]] ^[[[整数的表示与运算#补码编码]]]
> 

请注意：这些整数类型是完全不同的类型。不管它们的具体尺寸，`int` 和 `uint`  是不同类型的兄弟类型；`int` 和 `int32` 也是不同的类型，即使 `int` 的大小也是`32bit`，在需要将 `int` 当作 `int32` 类型的地方需要一个显式的类型转换操作

```go
package main

import "fmt"

func main() {
	var a int
	var b int64
	fmt.Printf("%+v\n", a+b)
	// compile error：invalid operation: a + b (mismatched types int and int64)
}
```

#### 整数字面值

整数字面值是表示整数常量的数字序列与可选前缀的组合。参考官方文档 [整数字面值词法](https://go.dev/ref/spec#Integer_literals)，Go 支持四种形式的整数字面值：**十进制** **二进制** **八进制** 和 **十六进制**

| 格式   | 词法                                            | 示例                 |
| :--- | --------------------------------------------- | ------------------ |
| 十进制  | `"0"` 或者 `("1" ... "9")(["_"]("0" ... "9"))+` | `0` `42` `4_2`     |
| 二进制  | `"0"("b", "B")(["_"]("0" ... "1"))+`          | `0b10` `0B1_01_10` |
| 八进制  | `"0"("o", "O")(["_"]("0" ... "7"))+`          | `0o600` `0O600`    |
| 十六进制 | `"0"("x", "X")(["_"]("0"..."9" "a"..."f"))+`  | `0x7eaf` `0X7EAF`  |

> [!tip] 
> 
> 单个 `0` 被视为十进制零。在十六进制字面值中，字母 `a` 到 `f` 和 `A` 到 `F` 代表值 `10` 到 `15`
> 

请务必按照上述字面值词法规则书写整数字面值，否则将会出现编译错误。例如，下面的示例中就不是正确的整数字面值

```go
_42         // 这是标识符，而不是整数字面值
42_         // invalid: _ 必须分割数字，而不能用于结尾
4__2        // invalid: 一次只能出现一个 _
0_xBadFace  // invalid: _ 必须分隔数字
```

> [!important] 
> 
> 整数字面值都是 `untyped constant`，默认的类型为 `int`
>

```go
package main

import "fmt"

func main() {
	var a int
	var b int64

	a = 10 // 十进制字面值
	fmt.Printf("a = %d; type is %T\n", a, a)
	b = 0o77 // 八进制字面值。现在更推荐使用 0o 或 0O 作为前缀，避免混淆
	fmt.Printf("a = %d; type is %T\n", b, b)
	c := 0xef // 十六进制字面值
	fmt.Printf("a = %d; type is %T\n", c, c)
}
```

### 字符类型

与 C/C++ 一样，Go 也是将字符当作整数处理的。不过，Go 语言默认处理的是 Unicode 字符

Go 语言中，使用 `rune` 类型处理字符的标准类型，它本质上是 `int32` 的别名

```go
type rune = int32 
// rune 是 int32的别名，4 个字节，
// 足够存储 Unicode 码点
```

> [!tip] 
> 
> `rune` 类型存储的是单个字符的 Unicode 码点值
> 

```go
package main

import "fmt"

func main() {
	var c rune
	c = '中'                      // 字面值，代表了字符 '中' 的 Unicode 码点
	fmt.Printf("%v %#[1]x\n", c) // 20013 0x4e2d
	c = 0x4e2d                   // 可以直接给 rune 赋值整数，表示 Unicode 码点
	fmt.Printf("%c\n", c)        // 中
}
```

此外，Go 语言还提供了一个 `byte` 类型，它本质上是 `uint8` 的类型名别

```go
type byte = uint8 
// byte 是 uint8 的别名，1 个字节
```

通常用于处理 **无类型** 的字节序列。例如，文件 IO，网络 IO 等。当然，也可以用于处理 ASCII 编码的字符

```go
package main

import "fmt"

func main() {

	var u byte
	// u = '中'  // error: constant 20013 overflows byte
	u = 'A'                      // 65
	fmt.Printf("%v %#[1]x\n", u) // 65 0x41
	fmt.Printf("%v %#[1]x\n", u) // 97 0x61
	u = '0'                      // 48
	fmt.Printf("%v %#[1]x\n", u) //  48 0x30
}
```

**字符字面值** 被表示为一个或多个字符 **被包裹在单引号中**，比如 `'x'` 或 `'\n'`。请注意：多个字符必须是转移字符序列

> [!tip] 
> 
> 字符字面值代表一个字符常量，它本质上是一个整数值，即是 Unicode 代码点
> 
 
### 浮点类型

Go 语言提供了两种格式的浮点数：`float32` 和 `float64`，采用  [IEEE 754](https://zh.wikipedia.org/wiki/IEEE_754) 标准，该标准被所有现代 CPU 支持


|    类型     |   大小   | 范围                                            | 备注                       |
| :-------: | :----: | :-------------------------------------------- | ------------------------ |
| `float32` | $32$ 位 | $[-3.4\times 10^{38}, 3.4\times 10^{38}]$     | 最大值定义在 `math.MaxFloat32` |
| `float64` | $64$ 位 | $[-1.8 \times 10^{308}, 1.8 \times 10^{308}]$ | 最大值定义在 `math.MaxFloat64` |

> [!WARNING] 
> 
> `float32` 与 `float64` 类型的变量不能相互赋值，它们是两个不同的数据类型
> 

```go
package main  
  
import (  
    "fmt"  
    "math")  
  
func main() {  
    var x float32 = 123.78  
    var y float32 = 3.4e+38  
    var max_flaot32 = math.MaxFloat32  
    fmt.Printf("Type: %T, value: %v\n", x, x)  
    fmt.Printf("Type: %T, value: %v\n", y, y)  
    fmt.Printf("Type: %T, value: %v", max_flaot32, max_flaot32)  
}
```

`float32` 类型在采用 IEEE 754 标准的浮点数中尾数只有 $23$ 位。

`float64` 类型在采用 IEEE 754 标准的浮点数中尾数有 $52$ 位。

`float64` 类型能够提供更高的精度，应该优先使用 `float64` 类型，因为 `float32` 类型的累计计算误差很容易扩散。

当整数大于 $23$ 位能表达的范围时，`float32` 的表示将出现误差

```go
var f float32 = 16777216 // 1 << 24
fmt.Println(f == f+1)    // "true"!
```

#### 浮点数字面值

Go 语言中的 [浮点字面值](https://go.dev/ref/spec#Floating-point_literals) 有两种形式： **十进制** 和 **十六进制**

| 格式   | 词法                                                                                                            | 示例                             |
| :--- | :------------------------------------------------------------------------------------------------------------ | ------------------------------ |
| 十进制  | `(十进制数字)+.[(十进制数字)+][(e, E)[+,-](十进制数字)+]`<br>`(十进制数字)+(e,E)[+,-](十进制数字+)`<br>`.(十进制数字)+[(e,E)[+,-](十进制数字)+]` | `10.2e6`<br>`10e2`<br>`.102e3` |
| 十六进制 | `(0x,0X)(十六进制数)+.[(十六进制数)+]p[+,-](十进制数)+`                                                                     | `0x2.p10 `                     |

> [!tip] 
> 
> 在十进制格式中 `(e,E)[+,-](n)` 代表对其前面的整数或小数缩放 $10^{\pm n}$ 倍。例如，`10.2e2` 等于 $10.2 \times 10^{2}$，即数学上的科学计数法
> 
> 在十六进制格式中 `p[+,-](n)` 代表对其前面的十六进制的整数或小数缩放 $2^{\pm n}$ 倍。例如 `0x2.1p10` 等于 $0x2.1 \times 2^{10}$
> 

下面给出了一些十六进制浮点数字面值的示例

```go
0x1p-2       // == 0.25
0x2.p10      // == 2048.0
0x1.Fp+0     // == 1.9375
0X.8p-0      // == 0.5
0X_1FFFP-16  // == 0.1249847412109375

0x15e-2      // (这是整数减法) == 0x15e - 2
```

### 复数类型

Go语言提供了两种精度的复数类型：`complex64` 和 `complex128`，分别对应 `float32` 和 `float64` 两种浮点数精度

内置的 `complex()` 函数用于构建复数，内建的 `real()` 和 `imag()` 函数分别返回复数的实部和虚部

```go
var x complex128 = complex(1, 2) // 1+2i
var y complex128 = complex(3, 4) // 3+4i
fmt.Println(x*y)                 // "(-5+10i)"
fmt.Println(real(x*y))           // "-5"
fmt.Println(imag(x*y))           // "10"
```

如果一个浮点数面值或一个十进制整数面值后面跟着一个 `i`，例如 `3.141592i` 或 `2i`，它将构成一个复数的虚部，复数的实部是 `0`

```go
fmt.Println(1i * 1i) // (-1+0i) -1
```

> [!important] 
> 
> 浮点数字面值是 `untyped constant`，默认类型是 `float64`
> 

### 布尔类型

Go 语言中的 `bool`类型只有两个预定常量，分别是 `true` 和 `false`。

> [!important] 
> 
> `bool` 类型是独立的一个类型，不要与其他语言混淆
> 
> 在 C/C++ 中，`bool` 类型与 `int` 是等价的。
> 

`if` 和 `for` 语句的条件部分都是布尔类型的值，并且 `==` 和 `<` 等比较操作也会产生布尔型的值

一元操作符 `!` 对应 **逻辑非** 操作，因此 `!true` 的值为 `false` ，更罗嗦的说法是 `(!true==false)==true` 

布尔值可以和 `&&（AND）` 和 `||（OR）` 操作符结合，并且有短路行为：如果运算符左边值已经可以确定整个布尔表达式的值，那么运算符右边的值将不再被求值，因此下面的表达式总是安全的

```go
s != "" && s[0] == 'x'
```

其中 `s[0]` 操作如果应用于空字符串将会导致 `panic` 异常

### 字符串类型

**字符串是一个 _不可改变_ 的字节序列**，包含任意的数据，但是通常是用来包含人类可读的文本。**文本字符串通常被解释为采用 `UTF-8` 编码的 `Unicode` 码点（`rune`）序列**^[[[数据存储#编码方式]]]

请注意：Go 语言底层将字符串按照字节编码的方式存储。内置函数 `len()` 可以返回字符串内容占用的字节数。

```go
s := "hello, world!"
fmt.Println(len(s))  // 13
```

索引操作 `s[i]` 返回第 `i` 个字节的值，`i` 必须满足 `0 ≤ i< len(s)` 条件约束。如果试图访问超出字符串索引范围的字节将会导致 `panic` 异常

```go
fmt.Println(s[0], s[7]) // 104 119
c := s[len(s)] // panic: index out of range
```

我们可以从字符串中切出一个子字符串。即 `s[i:j]` 基于原始的 `s` 字符串的第 `i` 个字节开始到第 `j` 个字节（并不包含 `j` 本身）生成一个新字符串。生成的新字符串将包含 `j-i` 个字节

```go
fmt.Println(s[0:5]) // "hello"
```

> [!attention] 
> 
> 如果索引超出字符串范围或者 `j` 小于 `i` 的话将导致 `panic` 异常
> 

不管 `i` 还是 `j` 都可能被忽略，当它们被忽略时将采用 `0` 作为开始位置，采用 `len(s)` 作为结束的位置

```go
fmt.Println(s[:5]) // "hello"
fmt.Println(s[7:]) // "world"
fmt.Println(s[:])  // "hello, world!"
```

字符串可以用 `==` 和 `<` 或 `>` 进行比较；比较通过 **逐个字节** 比较完成的，因此比较的是字符串字符的自然编码顺序

> [!important] 
> 
> **字符串的值是不可变的**：一个字符串包含的字节序列永远不会被改变
> 

当然我们也可以给一个字符串变量分配一个新字符串值。可以像下面这样将一个字符串追加到另一个字符串：

```go
s := "left foot"
t := s
s += ", right foot"
```

这并不会导致原始的字符串值被改变，但是变量 `s` 将因为 `+=` 语句持有一个新的字符串值，但是 `t` 依然是包含原先的字符串值

```go
fmt.Println(s) // "left foot, right foot"
fmt.Println(t) // "left foot"
```

> [!attention] 
> 
> 因为字符串是不可修改的，因此尝试修改字符串内部数据的操作也是被禁止的
> 

```go
s[0] = 'L'  // compile error: cannot assign to s[0]
```

#### 字符串字面值

字符串值也可以用字符串字面值方式编写，只要将一系列字节序列包含在双引号即可

```go
"hello, 世界"
```

因为 Go 语言源文件总是用 UTF8 编码，并且 Go 语言的文本字符串也以 UTF8 编码的方式处理，因此我们可以将 Unicode 码点也写到字符串面值中

在一个双引号包含的字符串面值中，可以用以反斜杠 `\` 开头的转义序列插入任意的数据。下面的换行、回车和制表符等是常见的 ASCII 控制代码的转义方式

| 名字    | 转义序列 |     | Unicode 代码点 |
| ----- | ---- | --- | ----------- |
| 响铃符   | `\a` |     | `U+0007`    |
| 回退符   | `\b` |     | `U+0008`    |
| 换页符   | `\f` |     | `U+000C`    |
| 换行符   | `\n` |     | `U+000A`    |
| 回车符   | `\r` |     | `U+000D`    |
| 水平制表符 | `\t` |     | `U+0009`    |
| 垂直制表符 | `\v` |     | `U+000B`    |
| 反斜杠   | `\\` |     | `U+005C`    |
| 问号    | `\?` |     |             |
| 单引号   | `\'` |     | `U+0027`    |
| 双引号   | `\"` |     | `U+0022`    |

可以通过十六进制或八进制转义在字符串面值包含任意的字节。
+ 十六进制的转义形式是 `"\xhh"` ，其中两个 `h` 表示十六进制数字（大写或小写都可以）
+ 八进制转义形式是 `"\ooo"` ，包含三个八进制的 `o` 数字，但是不能超过 `"\377"` 

> [!important] 
> 
> 数字转移字符序列限制的范围对应一个字节的范围
> 

一个 **原始的字符串面值形式** 是 \`...\`，使用反引号代替双引号。在原始的字符串面值中，**没有转义操作**；全部的内容都是字面的意思，包含退格和换行，因此一个程序中的原生字符串面值可能跨越多行

> [!tip] 
> 在原生字符串面值内部是无法直接写 \` 字符的，可以用八进制或十六进制转义或+ "\`" 链接字符串常量完成

有很多 Unicode 字符很难直接从键盘输入，并且还有很多字符有着相似的结构；有一些甚至是不可见的字符。Go语言字符串面值中的 Unicode 转义字符让我们可以通过 Unicode 码点输入特殊的字符。有两种形式
+ `"\uhhhh"` 对应 `16bit` 的码点值
+ `"\Uhhhhhhhh"` 对应 `32bit` 的码点值，其中 `h` 是一个十六进制数字

一般很少需要使用 `32bit` 的形式。每一个对应码点的 UTF8 编码。例如：下面的字母串面值都表示相同的值

```go
"世界"
"\xe4\xb8\x96\xe7\x95\x8c"
"\u4e16\u754c"
"\U00004e16\U0000754c"
```

Unicode 转义也可以使用在 `rune` 字符中。下面三个字符是等价的

```go
'世' '\u4e16' '\U00004e16'
```

对于小于 $256$ 码点值可以写在一个十六进制转义字节中，例如 `'\x41'` 对应字符 'A'，但是对于更大的码点则必须使用 `'\u'` 或 `'\U'` 转义形式。因此， `'\xe4\xb8\x96'`  并不是一个合法的 `rune` 字符，虽然这三个字节对应一个有效的 UTF8 编码的码点

### 类型别名和类型定义


#### 类型别名

类型别名是 Go1.9 版本添加的新功能。类型别名规定：**`TypeAlias` 只是 `Type` 的别名，本质上 `TypeAlias` 与 `Type` 是 同一个类型**
- 就像一个孩子小时候有小名、乳名，上学后用学名，英语老师又会给他起英文名， 但这些名字都指的是他本人

类型别名也是使用 `type` 关键创建的

```go
type TypeAlias = Type
```

我们之前见过的 `rune` 和 `byte` 就是类型别名，它们在 `$GOTOOT/src/builtin/builtin.go` 中定义

```go
//  byte 是 uint8 的别名，在所有方面都等同于 uint8。
// 按照惯例，它用于区分字节值和 8 位无符号整数值。
type byte = uint8

// rune 是 int32 的别名，在所有方面都等同于 int32。
// 它用于区分字符值和整数值。
type rune = int32
```

#### 类型定义

在 Go 语言中有一些基本的数据类型，如`string`、`整型`、`浮点型`、 `bool` 等数据类型。然而， Go 语言中可以使用 `type` 关键字来定义自定义类型

**自定义类型是定义了一个全新的类型**, 我们可以基于内置的基本类型定义，也可以基于后面的 **结构体类型** 定义

```go
// MyInt 将 MyInt 定义为 int 类型
type MyInt int
```

通过 `type` 关键字的定义，**`MyInt` 就是一种新的类型**，它具有 `int` 的特性。现在，我们使用 `MyInt` 定义一个变量，然后查看改变了的类型

```go
package main

import "fmt"

type MyInt int // 使用 int 类型自定义了一个新类型

func main() {
	var x MyInt
	fmt.Printf("x type: %T\n", x) // x type: main.MyInt
}
```

类型定义与类型别名是完全不同的

```go
package main

import "fmt"

// NewInt 类型定义
type NewInt int

// MyInt 类型别名
type MyInt = int

func main() {
	var a NewInt
	var b MyInt

	fmt.Printf("type of a: %T\n", a) // type of a: main.NewInt
	fmt.Printf("type of b: %T\n", b) // type of b: int
}
```
- `a` 的类型是 `main.NewInt`，表示 `main` 包下定义的 `NewInt` 类型

- `b` 的类型是 `int`。`MyInt` 类型只会在代码中存在，编译完成时并不会有 `MyInt` 类型

## 运算符

运算符用于对变量和值执行操作。例如，**`+` 运算符将两个值相加**，如下例所示

```go
package main

import "fmt"

func main() {

	var a = 15 + 25
	fmt.Println(a)
}

```

虽然 `+` 操作符通常用于将两个值相加，但它 **也可以用于将一个变量和一个值相加**，或者 **将一个变量和另一个变量相加**

```go
package main

import "fmt"

func main() {
	var (
		sum1 = 100 + 50    // 150 (100 + 50)  值 + 值
		sum2 = sum1 + 250  // 400 (150 + 250) 变量 + 值
		sum3 = sum2 + sum2 // 800 (400 + 400) 变量 + 变量
	)
	fmt.Println(sum1, sum2, sum3)
}
```

> [!warning] 
> 
> **运算符不会改变其操作数的内容**，运算符的结果回创建一个新的对象
> 

### 算术运算符

算术运算符用于 **执行常见的数学运算**。

| 运算符 | 名称  | 描述         | 示例       | 结果   |
| --- | --- | ---------- | -------- | ---- |
| `+` | 加法  | 将两个值相加     | `10 + 5` | $15$ |
| `-` | 减法  | 用一个值减去另一个值 | `10 - 5` | $5$  |
| `*` | 乘法  | 将两个值相乘     | `10 * 5` | $50$ |
| `/` | 除法  | 用一个值除以另一个值 | `10 / 5` | $2$  |
| `%` | 取模  | 返回除法的余数    | `7 % 3`  | $1$  |

算术运算符适用于整数、浮点数和复数，但是 **取模运算符 `%`  仅用于整数间的运算**

> [!tip] 
> 
> Go 中 `%` 取模运算符结果的符号和被取模数的符号总是一致的，因此 `-5%3` 和 `-5%-3` 结果都是 `-2`
> 

除法运算符 `/` 的行为则依赖于操作数是否为全为整数，比如  `5.0 / 4.0` 的结果是 `1.25`，但是 `5 / 4` 的结果是 `1`，因为 **整数除法会向着 `0` 方向截断余数**

```go
package main

/* 算术运算：执行常规算术运算符 */

import "fmt"

func main() {
	a := 10                          // int
	b := 3                           // int
	quotient := a / b                // quotient = 3 int
	remainder := a % b               // 返回的是 a / b 的余数 int
	fmt.Println(quotient, remainder) // 3 1
}
```

**两个浮点型变量** 执行除法运算(`/`)，得到 **结果为浮点型**，即真实值，**除数为 $0$ 时，得到 `Inf`**

如果一个算术运算的结果，不管是有符号或者是无符号的，如果需要更多的 `bit` 位才能正确表示的话，就说明计算结果是 **溢出** 了。**超出的高位的 `bit` 位部分将被丢弃**。如果原始的数值是有符号类型，而且最左边的 `bit` 为是 `1` 的话，那么最终结果可能是负的，例如 `int8` 的例子：

```go
var u uint8 = 255
fmt.Println(u, u + 1, u * u)  // 255, 0, 1

var i int8 = 127
fmt.Println(i, i+1, i*i) // "127 -128 1"
```

与 C 语言不同的时，**`++` 和 `--` 在 Go 中不以运算符的形式出现**，仅仅是单独的一条语句，且 **只能出现在变量的后面**

```go
package main

/* 算术运算：执行常规算术运算符 */

import "fmt"

func main() {
	a := 10                          // int
	b := 3                           // int
	
	a++
	b--
	
	fmt.Println(a, b) // 11 2

	//--a // 错误的
	//++b  // 错误的
	// result = a++ // 错误的，++ 不是运算符，而是语句，没有返回值
}
```

在执行操作时，**两个操作数必须具有相同的类型**。在 C 语言中，C 编译器提供了隐式类型转换，使得我们可以对不同类型的数据直接执行操作

然而，**Go 编译器并未提供隐式类型转换**，当遇到两个不同类型的变量执行算术运算时，需要进行 **强制类型转换**，通常，**将类型尺寸较小的操作数转换为类型尺寸较大的类型**

```go
package main

/* 算术运算：执行常规算术运算符 */

import "fmt"

func main() {
	a := 10   			// int
	b := 3.14		 	// float64
	//result := a * b 	//  由于 a 和 b 的类型不同，因此需要将 int 类型转换为 float64 类型

	result := float64(a) * b
	fmt.Println(result)
}
```

Go 只提供了 **强制类型转换**，_没有隐式类型转换_。该语法只能在 **两个类型之间支持相互转换** 的时候使用

强制类型转换的基本语法如下

```go
object_type(expr)
```
+ `object_type` 表示目标数据类型
+ `expr` 表达式包括 变量，复杂的表达式和函数的返回值等

```go
package main  
  
import "fmt"  
  
func main() {  
    var a int8 = 0x7f  
    var b int16 = int16(a) // 不能直接用 int8 的变量赋值给 int16    
    fmt.Println(a)  
    fmt.Println(b)  
}
```

案例1：计算 $46$ 天的整数周数和剩余天数

```go
package main

/* 算术运算：执行常规算术运算符 */

import "fmt"

func main() {
	fmt.Println(46/7, 46%7) // 6 4
}
```

案例2：$107653$ 秒是几天几小时几分几秒

```go
package main  
  
/* 算术运算：执行常规算术运算符 */  
import "fmt"  
  
func main() {  
  
    total_seconds := 107653                // 总秒数  
    day := total_seconds / (24 * 60 * 60)  // 天数  
    hour := total_seconds / (60 * 60) % 24 // 小时  
    minute := total_seconds / 60 % 60      // 分钟  
    seconds := total_seconds % 60          // 秒  
    fmt.Printf("%v s is %v day %v hour %v m %v s\n", total_seconds, day, hour, minute, seconds)  
}
```

### 位运算符

**位运算符对 _整数_ 在内存中的二进制位进行操作**

| 运算符  |  名字  | 描述                                 | 示例      |
| :--: | :--: | :--------------------------------- | ------- |
| `&`  | 按位与  | 如果两个位都是 $1$，则将位设置为 $1$             | `x & y` |
|  \|  | 按位或  | 如果两个位中的一个为 $1$，则将位设置为 $1$          | x \| y  |
| `^`  | 按位异或 | 如果两个位中只有一个为 $1$，则将位设置为 $1$         | `x ^ b` |
| `^`  | 按位取反 | 作为为一元运算符是，替代其他语言中 `~` 按位反运算符       | `^b`    |
| `&^` | 位清除  | `a &^ b` 等价于 `a & \~b`，`\~` 表示按位取反 |         |
| `<<` |  左移  |                                    |         |
| `>>` |  右移  |                                    |         |

```go
package main

import "fmt"

func main() {
	// 位运算符, 作用在二进制位上的操作
	// 5: 101
	// 2: 010
	// & 按位与，同位1取1，有0则取0
	fmt.Printf("5 & 2: %b\n", 5&2)
	// | 按位或, 有1取1
	fmt.Printf("5 | 1: %b\n", 5|2)
	// ^ 异或，不同则取1，同则取0
	fmt.Printf("5 ^ 2: %b\n", 5^2)
	// << 左移指定位数: 左移n位就是乘以2的n次方。
	fmt.Printf("5 << 1: %b\n", 5<<1) // 1010
	// >> 右移指定位数: 右移n位就是除以2的n次方。
	fmt.Printf("5 >> 1: %b\n", 5>>1) // 10
	// ~ 按位取反 ~n = -(n+1)，Go 使用 ^ 替换了其他语言的 ~
	fmt.Printf("~5: %b\n", ^5) // ~5 = -(5 + 1)
}
```

> [!seealso] 运算符规则另见 
> + [[数据运算#逻辑运算]]
> + [[数据运算#移位运算]]

### 比较运算符

比较运算符用于比较两个 **相同类型** 的值

> [!NOTE] 注意
> 比较运算符的返回值要么为 `true` ，要么为 `false`。

在下面的示例中，我们使用大于运算符 ( `>` ) 来确定 $5$ 是否大于 $3$：

```go
package main  
  
import "fmt"  
  
func main() {  
    var x = 5  
    var y = 3  
    fmt.Println(x > y) // true  
}
```

下表列出了所有比较运算符

| 运算符 | 名称    |   示例   |
| :-: | :---- | :----: |
| ==  | 等于    | x == y |
| !=  | 不等于   | x != y |
|  >  | 大于    | x > y  |
|  <  | 小于    | x < y  |
| >=  | 大于或等于 | x >= y |
| <=  | 小于或等于 | x <= y |

> [!important] 可比较的
> 布尔型、数字类型和字符串等基本类型都是可比较的，也就是说两个相同类型的值可以用 `==` 和 `!=` 进行比较
> 
> 整数、浮点数和字符串可以根据比较结果排序

### 逻辑运算符

用于布尔逻辑运算符。Go 提供下面三种逻辑运算符

| 运算符  | 名称  | 描述                           | 示例                   |
| ---- | --- | ---------------------------- | -------------------- |
| `&&` | 逻辑与 | 如果两个语句都为`true`，则返回 `true`    | `x < 5 &&  x < 10`   |
| \|\| | 逻辑或 | 如果两个语句都为 `false`，则返回 `false` | `x < 5` \|\| `x < 4` |
| `!`  | 逻辑非 | 反转结果，如果结果为`true` 则返回 `false` | `!(x < 5 && x < 10)` |
> [!warning] 逻辑运算符的操作数的类型只能是 `bool` 类型

- 逻辑非 `!` **作用在 `bool` 类型返回值的表达式之上**，对布尔值进行取反操作
- 逻辑或 `||`: 短路运算符，只要遇到 `true` 就返回 `true`，后续表达式就不再求值
- 逻辑与 `&&`: 短路运算符，只要遇到 `false` 就返回 `false`，后续表达式就不再求值

### 赋值运算符

赋值操作符用于为 **变量赋值**。

在下面的例子中，我们使用 **赋值运算符(`=`)** 将值 $10$ 赋值给一个名为 `x` 的变量

```go
package main  
  
import "fmt"  
  
func main() {  
    var x int  
    x = 10  
    fmt.Println(x)  
}
```

有时候，当我们对两个变量操作时，需要将结果存放在其中一个变量中，通常采用

```go
a := 10
b := 5
a = a + b
```

然而，这个操作会读取三次内存。Go 语言提供另一种赋值运算符来完成此操作

```g
a := 10
b := 5
a += b
```

`a += b` 等价于 `a = a + b`，但是 `a += b` 只会读取两次内存，提高了一定的执行效率

下表列出了所有赋值运算符

| 运算符 |   示例    |    等价物     | 备注         |
| :-: | :-----: | :--------: | ---------- |
|  =  |  x = 5  |   x = 5    | 普通的赋值运算符   |
| +=  | x += 3  | x = x + 3  |            |
| -=  | x -= 3  | x = x - 3  |            |
| *=  | x *= 3  | x = x * 3  |            |
| /=  | x /= 3  | x = x / 3  |            |
| %=  | x %= 3  | x = x % 3  |            |
| &=  | x &= 3  | x = x & 3  | & 是按位与运算符  |
| \|= | x \|= 3 | x = x \| 3 | \| 是按位或运算符 |
| ^=  | x ^= 3  | x = x ^ 3  | ^ 按位异或运算符  |
| >>= | x >>= 3 | x = x >> 3 | >> 右移运算符   |
| <<= | x <<= 3 | x = x << 3 | << 左移运算符   |

### 指针操作

数据是放在内存中，内存是线性编址的。任何数据在内存中都可以通过一个地址来找到它。

运算符 `&` 用于获取变量的内存地址。运算符 `*` 对指针变量解引用，通过指针取值

Go 中 **赋值执行拷贝**，每赋值一次，就会在内存中拷贝一份内容


```go
a := 123
b := &a // &取地址
c := *b
fmt.Printf("%d, %p, %d\n", a, b, c) // 123 0x.... 123

// 请问，下面相等吗？
fmt.Println(a == c, b == &c, &c)  // true false 0x....

var d = a
fmt.Println(a == d, &a, &d) // &a == &d? 不相等
```

### 运算符的优先级

一元运算符具有最高优先级。由于 `++` 和 `--` 运算符构成语句，而不是表达式，它们被排除在运算符层次结构之外。因此，语句 `*p++` 与 `(*p)++` 是相同的。

二进制运算符有五个优先级。**乘法运算符优先级最高**，**其次是加法运算符**，**比较运算符**，**逻辑与（&&**），最后是 **逻辑或（||）**

```
Precedence    Operator
    5             *  /  %  <<  >>  &  &^
    4             +  -  |  ^
    3             ==  !=  <  <=  >  >=
    2             &&
    1             ||
```

**相同优先级的二元运算符从左到右结合**。例如，`x / y * z` 等同于 `(x / y) * z`。

```go
+x                         // x
42 + a - b                 // (42 + a) - b
23 + 3*x[i]                // 23 + (3 * x[i])
x <= f()                   // x <= f()
^a >> b                    // (^a) >> b
f() || g()                 // f() || g()
x == y+1 && <-chanInt > 0  // (x == (y+1)) && ((<-chanInt) > 0)
```

## 控制流程

### if
- `if` 语句：如果一个条件为`true`，则执行一些代码
- `if-else` 语句：如果条件为 `true`，执行一些代码，如果条件为 `false` ，执行另一段代码
- `if-else if-else` 语句：为两个以上的条件执行不同的代码

`if` 语句仅在指定的条件求值为 `true` 时执行代码块

```go
if condition { 
    // condition 为 true 时执行的代码
}
```

> [!warning] 注意：Go语言中，花括号一定要跟着 `if`、`for`、`func` 等行的最后，否则语法出错

> [!important] 重要细节
> + `condition` 必须是一个 `bool` 类型，在 Go 中，不能使用其他类型等效为布尔值。 `if 1 {}` 是错误的
> + 语句块中可以写其他代码。如果 `condition` 为 `true`，才能执行其后代码块

--- 
#### `if-else`

`if....else` 语句允许您在指定条件为 `true` 时执行一个代码块，在指定条件为 `false` 时执行另一个代码块

```go
if condition { 
    // condition 为 true 时执行的代码
} else {
    // condition 为 false 时执行的代码
}
```

> [!warning] **`}` 后面放 `else {` 也是强制性的** ，如果不这样做，也将抛出 `syntax` 错误

---
#### `if-else if-else`

`if...else if...else` 语句是多个 `if...else` 语句的嵌套

```go
if  condition-1 { 
    // 如果condition-1 为 true，将执行的代码
} else if condition-2 {
    // 如果condition-2 为 true，将执行的代码
} else {
    // 如果condition-1 和condition-2 均为 false，将执行的代码
}
```

```go
a := 6
if a < 0 {
	fmt.Println("negative")
} else if a > 0 { // 走到这里一定 a 不小于 0
	fmt.Println("positive")
} else { // 走到这里一定 a 不大于、也不小于 0
	fmt.Println("zero")
}
```

多分支结构，从上向下依次判断分支条件，只要一个分支条件成立，其后语句块将被执行，那么其他条件都不会被执行

**前一个分支条件被测试过，下一个条件相当于隐含着这个条件**

一定要考虑一下else分支是有必要写，以防逻辑漏洞

`if ... else if ... else` 等价于下列嵌套的 `if`

```go
// 嵌套
a := 6
if a == 0 {
	fmt.Println("zero")
} else {
	if a > 0 {
		fmt.Println("negative")
	} else {
		if a >= 0 { // 走到这里一定 a 不小于 0
			fmt.Println("positive")
		}
	}
}
```

#### 带初始化的 `if`

`if` 语句可以携带一个初始化语句，在这种语法中，**被测试的表达式前面有一个初始化语句**
```go
if  var declaration; condition { 
    // condition 为 true 时执行的代码
}
```

例如

```go
package main
 
import (
	"fmt"
)
 
func main() {
	if x := 100; x == 100 {
		fmt.Println("Germany")
	}
}
```

### switch

`switch` 语句用于 **从多个代码块中选择一个执行**。它的完整语法如下

```go
switch initializer; expr {
	case value1:
		<statement-1>
		<....>
		<statement-n>
	case value2:
		<statement-1>
		<....>
		<statement-n>
	....
	default:
		<statement-1>
		<....>
		<statement-n>
}
```

- `initializer` 是一个简单的初始化语句，可以省略
- `expr` 是一个表达式，`value` 的值的类型必须与 `expr` 表达式的类型相同

> [!ERROR] 警告
> 
> **所有的  `case` 值都应该与 `switch` 后的表达式的值的类型相同**，否则将抛出错误
> - 不像 C 那样要求 `expr` 必须是整数

考虑以下示例，它在特定日期显示不同的消息

```go
package main  
  
import (  
    "fmt"  
    "time")  
  
func main() {  
    today := time.Now()  
  
    switch /*初始化语句;*/ today.Day() {  
    case 5:
       fmt.Println("Today is 5th. Clean your house.")  
    case 10:  
       fmt.Println("Today is 10th. Buy some wine.")  
    case 15:  
       fmt.Println("Today is 15th. Visit a doctor.")  
    case 25:  
       fmt.Println("Today is 25th. Buy some food.")  
    case 31:  
       fmt.Println("Party tonight.")  
    default:  
       fmt.Println("No information available for that day.")  
    }  
}
```

带有多 `case` 行语句的 `switch` 用于为许多类似情况选择共同的代码块

```go
package main

import (
	"fmt"
	"time"
)

func main() {
	today := time.Now()
	var t int = today.Day()

	switch /*初始化语句;*/ t {
	case 5, 10, 15:
		fmt.Println("Clean your house.")
	case 25, 26, 27:
		fmt.Println("Buy some food.")
	case 31:
		fmt.Println("Party tonight.")
	default:
		fmt.Println("No information available for that day.")
	}
}
```

Go 的 `case` 语句在匹配成功之后，不会连续执行多个 `case` 块。如确需要连续执行，使用关键字 `fallthrough`

```go
package main

import (
	"fmt"
	"time"
)

func main() {
	today := time.Now()

	switch /*初始化语句;*/ today.Day() {
	case 5:
		fmt.Println("Clean your house.")
		fallthrough // 强制控制流转到下一个 case 语句组
	case 10:
		fmt.Println("Buy some wine.")
		fallthrough //强制控制流转到下一个 case 语句组
	case 15:
		fmt.Println("Visit a doctor.")
		fallthrough // 强制控制流转到下一个 case 语句组
	case 25:
		fmt.Println("Buy some food.")
		fallthrough // 强制控制流转到下一个 case 语句组
	case 31:
		fmt.Println("Party tonight.")
	default:
		fmt.Println("No information available for that day.")
	}
}
```

`case` 语句也可以与比较运算符和逻辑运算符一起使用

```go
package main

import (
	"fmt"
	"time"
)

func main() {
	today := time.Now()

	switch /*初始化语句;*/ { // 此处没有表达式
	case today.Day() < 5: // case 一个条件
		fmt.Println("Clean your house.")
	case today.Day() <= 10: // case 一个条件
		fmt.Println("Buy some wine.")
	case today.Day() > 15: // case 一个条件
		fmt.Println("Visit a doctor.")
	case today.Day() == 25: // case 一个条件
		fmt.Println("Buy some food.")
	default:
		fmt.Println("No information available for that day.")
	}
}
```

#### 带初始化的 `switch`

**`switch` 关键字后面可以紧接一个简单的初始化语句**，其中可以声明和初始化 `switch` 代码块中的局部变量

```go
package main

import (
	"fmt"
	"time"
)

func main() {
	switch today := time.Now(); /*expr*/{
	case today.Day() < 5:
		fmt.Println("Clean your house.")
	case today.Day() <= 10:
		fmt.Println("Buy some wine.")
	case today.Day() > 15:
		fmt.Println("Visit a doctor.")
	case today.Day() == 25:
		fmt.Println("Buy some food.")
	default:
		fmt.Println("No information available for that day.")
	}
}
```

### for

**Go 只有一个循环语句，即 `for` 语句**。基本的 `for` 循环结构如下

```go
for init; condition; post {
	// 循环体执行的代码
}
```

- `init` 是循环的初始化，只在进入循环时执行一次
- `condition` 是条件表达式
	- 当 `condition` 为 `true` 时，执行 `for` 中的代码
	- 当 `condition` 为 `false` 时，结束循环
- `post` 在每次迭代结束时执行，通常用于改变循环条件

**`init` 语句通常是一个简短的变量声明**，在那里声明的变量仅在 `for` 语句的作用域内可见。一旦`condition` 条件表达式的计算结果为 `false` ，循环将停止迭代

> [!WARNING] 注意
> `for` 的 `init; condition; post` 没有被 `()` 包围， `{}` 是必须的，且 `{` 必须和 `for` 在同一行

例如，计算 $1 \sim 100$ 的和

```go
package main

import "fmt"

func main() {
	// 基本结构
	sum := 0
	for j := 1; j <= 100; j++ {
		sum += j
	}
	fmt.Println(sum)
}
```

**`init` 和 `post` 语句是可选的**，它类似于 C 的 `while` 循环。在这一点上，你可以 **去掉分号**

```go
for condition {}  // 类似 C 的 while
```

例如，计算 $1 \sim 100$ 的和

```go
package main

import "fmt"

func main() {
	// 省略 init 和 post 此时，类似 C 的 while，这样就可以省略两边的分号
	sum := 0
	i := 1
	for i <= 100 {
		sum += i
		i++
	}
	fmt.Println(sum)
}
```

**如果省略循环条件，则它将永远循环**，因此可以紧凑地表示无限循环

```go
for {}  // 类似于 C 的 for(;;)
```

如果 `inti, condition, post` 只省略其中之一，那么分号(`;`) 将不能省略

```go
i := 0
for ; i < 10; i++ {
	// 条件
}
```

#### `range` 关键字

关键 `range` 字用于更轻松地迭代数 **数组、切片、字符串、map 及 通道(channel)**

> [!NOTE] 注意
> 数组，切片，map 马上就会涉及，但是通道将在 `go coroutine` 部分介绍

使用 `range` 关键字的方式如下

```go
for index, value := array|slice|map {
   // 每次迭代执行的代码
}
```

鉴于我们只学习了 `string`，现在我们使用 `range` 关键字来遍历 `string` 类型的值

```go
package main

import "fmt"

func main() {
	str := "你好, 小爱同学"
	for i, v := range str {
		//fmt.Printf("i type: %T, v type: %T\n", i, v) // v 的类型是 int32 这是由于 Go 使用 Unicode 处理字符串使用
		fmt.Println(i, string(v))
	}
}
```

**Go 1.22 版本新增了 `range` 对整数的支持**

```go
for i := range 5 {
	fmt.Println(i)  // 0 1 2 3 4
}

for range 3 {
	fmt.Println("hello") // 输出 3 次 hello
}
```

#### `continue` 关键字

该 `continue` 语句用于跳过循环中的一个或多个迭代。然后，它继续进行循环中的下一次迭代

```go
package main

import "fmt"

func main() {
	for i:=0; i < 10; i++ {
		if i % 2 != 0 {
			continue
		}
		fmt.Printf("%d ", i)
	}
}
```

`continue`语句后可以添加标签，**表示开始标签对应的循环**

```go
package main

import "fmt"

func main() {
forloop1:
	for i := 0; i < 5; i++ {
		//forloop2:
		for j := 0; j < 5; j++ {
			if i == 2 || j == 2 {
				continue forloop1
			}
			fmt.Printf("%v-%v\n", i, j)
		}
	}
}
```

#### `break` 关键字

该 **`break`** 语句用于 **中断/终止循环执行**

```go
package main

import "fmt"

func main() {
	for i:=0; i < 10; i++ {
		fmt.Printf("%d ", i)
		if i == 5 {
			break  // 跳出本层循环
		}
	}
	fmt.Printf("\n")
}
```

`break`语句还可以在语句后面添加 **标签**，表示 **退出某个标签对应的代码块**

```go
package main

import "fmt"

func main() {
BREAKDEMO1:
	for i := 0; i < 10; i++ {
		for j := 0; j < 10; j++ {
			if j == 2 {
				break BREAKDEMO1 // 跳出标签指定的语句
			}
			fmt.Printf("%v-%v\n", i, j)
		}
	}
	fmt.Println("...")
}
```

> `break` 关键字还可以用于 `switch` 和 `select` 的代码块上

#### 嵌套 `for`

可以将一个循环放在另一个循环中。处于另一个循环内部的循环称为 **"内部循环"**。在这里，**“外部循环” 的每迭代执行一次，"内部循环" 将完整执行**：

示例：寻找 $2 \sim 100$ 内的素数

```go
package main

import "fmt"

func main() {
	/* 寻找 2 ~ 100 内的素数 */
	for num := 2; num < 100; num++ {
		var j int = 2
		for ; j < num; j++ {
			if num%j == 0 {
				fmt.Printf("%d is not a prime.\n", num)
				break // 跳出内层循环
			}
		}
		if j == num {
			fmt.Printf("%d is a prime.\n", num)
		}
	}
}
```

#### `goto` 关键字

`goto` 语句通过标签进行代码间的 **无条件跳转**。`goto` 语句可以在快速跳出循环、避免重复退出上有一定的帮助。Go 语言中使用 `goto` 语句能简化一些代码的实现过程。 例如双层嵌套的 `for` 循环要退出时

```go
func f() {
	a := 0
	if a == 1 {
		goto LABEL1
	} else {
		fmt.Println("other")
	}

LABEL1:
	fmt.Printf("next...")
}

func main() {
	f()
}
```

```go
func f() {
	for i := 0; i < 5; i++ {
		for j := 0; j < 5; j++ {
			if i == 2 && j == 2 {
				goto LABEL1
			}
		}
	}
LABEL1:
	fmt.Println("label1")
}

func main() {
	f()
}
```

## 随机数

标准库 `"math/rand"`

我们使用的是 **伪随机数**，是内部写好的公式计算出来的。这个公式运行 **提供一个种子**，有这个 **种子作为起始值开始计算**。

+ `src := rand.NewSource(100)`，使用种子 `100` 创建一个随机数源
+ `rand.New(rand.NewSource(time.Now().UnixNano()))` ，利用当前时间的纳秒值做种子
+ `r10 := rand.New(src)`，使用源创建随机数生成器
+ `r10.Intn(5)`，返回 `[0, 5)` 的随机整数


```go
package main

import (
	"fmt"
	"math/rand"
)
func main() {
	src := rand.NewSource(10)
	r10 := rand.New(src)
	r1 := rand.New(rand.NewSource(1))
	for i := 0; i < 10; i++ {
		fmt.Printf("%d, %d, %d\n", rand.Intn(5), r1.Intn(5), r10.Intn(5))
	}
}
```

全局随机数生成器 `globalRand`
+ 它的种子默认为 `1`
+ 如果要改变 `globalRand` 的种子，就需要使用 `rand.Seed(2)` 修改种子
+ `rand.Intn(5)` 就是使用它生成随机数

## 格式化输出

我们在前面使用了 Go  `fmt` 标准包中的 `fmt.Print` 系列的函数。其中 `fmt.Printf` 用于格式化输出。继承自 C 语言的格式化输出。其中，第一个参数 `format` 的格式如下图

![[Pasted image 20231202192431.png|900]]

### 标志

标志是可选的。允许下面 $5$ 个标志位

| 标志        | 含义                              |
| --------- | ------------------------------- |
| `-`       | 栏内左对齐，默认是右对齐的                   |
| `+`       | 有符号转换得到的数总是以 `+` 或 `-` 开头       |
| `" "`(空格) | 有符号转换得到的非负数前面加空格（`+`标志优先于空格标志）  |
| `0`(数字零)  | 用前导零在数的栏宽内进行填充（`-`标志优先于 `0` 标志） |
| `#`       | 显示 Go 字面值的表现形式                  |

### 最小栏宽

**可选项**。最小栏宽可以使用 **整数** 或 **字符`*`**。如果是字符 `*`，则需要在参数列表中提供它的值

如果 **数据项太小** 以至于无法达到这一宽度，那么会 **进行填充**，填充内容由 **标志** 位确定

如果 **数据项过大** 以至于超过了这个宽度，那么会 **完整地显示数据项**

### 精度

**可选项**。精度的形式 `.整数` 或 `.*`。如果是 `.*`，则需要在参数列表中提供它的值。精度的含义依赖于转换指定符。根据转换指定符不同，表示的含义不同

如果转换指定符是 **整数类型** 的(`d, i, o, u, x, X`)，那么精度表示 **_最少位数_** 。如果位数不够，则添加前导零

如果转换指定符是 **浮点数类型** 的(`a, A, e, E, f, F`)，那么精度表示 **_小数点后的位数_**

如果转换指定符是 `g, G`，那么精度表示有效数字的个数
 
如果转换指定符是 `s`，那么精度表示最大字节数

### 长度指定符

Go 不支持长度指定符

### 转换指定符

| 转换指定符        | 含义                                                |
| ------------ | ------------------------------------------------- |
| `d, i`       | 整数类型的十进制形式                                        |
| `o, u, x, X` | 无符号整数转换为八进制(`o`), 十进制(`u`), 十六进制(`x,X`)           |
|              |                                                   |
| `f, F`       | 浮点类型的十进制小数形式                                      |
| `e, E`       | 浮点类型的十进制指数形式                                      |
| `g, G`       | 浮点类型。`g` 转换为 `f` 形式或者 `e` 形式。`G` 转换为 `F` 或 `E` 形式 |
| `b`          | 十进制科学记数法，指数是二的幂， 例如 `-123456p-78`                 |
|              |                                                   |
| `c`          | `rune` 类型，格式为字符                                   |
| `s`          | `str` 或 `[]byte` 。当达到精度值或消耗完所有内容时停止               |
|              |                                                   |
| `p`          | 指针                                                |
| `%`          | 字符 `'%'`                                          |
|              |                                                   |
| `t`          | `bool` 类型，转换为 `"true"` 或 `"false"`                |
| `v`          | 所有类型的缺省格式                                         |
| `T`          | Go 语法表示的值类型                                       |

