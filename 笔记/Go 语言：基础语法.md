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

下面的例程中声明了一个 **常量** 和两个 **变量**

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





