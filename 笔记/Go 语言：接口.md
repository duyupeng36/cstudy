# 接口

**接口类型是对其它 _类型行为的抽象_ 和概括**；因为接口类型不会和特定的实现细节绑定在一起，通过这种抽象的方式我们可以让我们的函数更加灵活和更具有适应能力

Go 语言中 **接口类型的独特之处在于它是满足 _隐式实现的_**。也就是说，我们没有必要对于给定的具体类型定义所有满足的接口类型；_简单地拥有一些必需的方法就足够了_。这种设计 **可以让你创建一个新的接口类型满足已经存在的具体类型却不会去改变这些类型的定义**；当我们使用的类型来自于不受我们控制的包时这种设计尤其有用

## 接口既是约定

目前为止，我们看到的类型都是 **具体的类型**。一个具体的类型 **可以准确的描述它所代表的值**，并且 **展示出对类型本身的一些操作方式**
- 就像数字类型的算术操作，切片类型的取下标、添加元素和范围获取操作

具体的类型还可以通过它的内置方法提供额外的行为操作。总的来说，**当你拿到一个具体的类型时你就知道它的本身是什么和你可以用它来做什么**

在 Go 语言中还存在着另外一种类型：**接口类型**。接口类型是一种 **抽象的类型**。它 **不会暴露** 出它所代表的 **对象的 _内部值的结构_** 和 **这个对象支持的基础操作的集合**；它们 **_只会展示出它们自己的方法_**。也就是说当你有看到一个接口类型的值时，你不知道它是什么，唯一知道的就是可以通过它的方法来做什么

> [!important] 接口类型
> 接口类型是一种抽象类型，它只暴露对象中的部分方法，而不暴露对象的内部实现(值结构)

### `io.Writer` 接口

下面我们来看一个字符串的格式化函数：`fmt.Printf` 把结果 **写到标准输出**。正在 `$GOROOT/src/fmt/print.go` 文件中的定义如下

```go

func Fprintf(w io.Writer, format string, a ...any) (n int, err error) {
	p := newPrinter()
	p.doPrintf(format, a)
	n, err = w.Write(p.buf)
	p.free()
	return
}

func Printf(format string, a ...any) (n int, err error) {
	return Fprintf(os.Stdout, format, a...)
}
```

`Fprintf` 的前缀 `F` 表示文件(`File`)，即表明格式化输出结果应该被写入第一个参数提供的文件中。在 `Printf` 函数中的第一个参数 `os.Stdout` 是 `*os.File` 类型

而 `Fprintf` 函数中的第一个参数也不是一个文件类型。它是 `io.Writer `类型这是一个接口类型定义在 `$GOROOT/src/io/io.go`

```go
type Writer interface {
	Write(p []byte) (n int, err error)
}
```

**`io.Writer` 接口类型定义了函数(`Fprintf`)和这个函数调用者之间的约定**
+ 这个约定需要 **_调用者提供具体类型的值_** 就像 `*os.File`，**这些类型都有一个特定签名和行为的 `Write` 的函数**

这个约定保证了 `Fprintf` 接受任何满足 `io.Writer` 接口的值都可以工作。`Fprintf` 函数可能没有假定写入的是一个文件或是一段内存，而是写入一个可以调用 `Write` 函数的值

因为 `fmt.Fprintf` 函数没有对具体操作的值做任何假设而是仅仅通过 `io.Writer` 接口的约定来保证行为，所以第一个参数可以安全地传入一个任何具体类型的值只需要满足 `io.Writer` 接口

> [!important] 可替换性
> **一个类型可以自由的使用另一个满足相同接口的类型来进行替换被称作 _可替换性(LSP里氏替换)_**。这是一个面向对象的特征

让我们通过一个新的类型来进行校验，下面 `*ByteCounter` 类型里的 `Write` 方法，仅仅在丢失写向它的字节前统计它们的长度。(在这个 `+=` 赋值语句中，让 `len(p)` 的类型和 `*c `的类型匹配的转换是必须的。)

```go
type ByteCounter int

func (c *ByteCounter) Write(p []byte) (int, error) {
	*c += ByteCounter(len(p))
	return len(p), nil
}
```

因为 `*ByteCounter` 满足 `io.Writer` 的约定，我们可以把它传入 `Fprintf` 函数中；`Fprintf` 函数执行字符串格式化的过程不会去关注 `ByteCounter` 正确的累加结果的长度

```go
package main

import "fmt"

type ByteCounter int

func (c *ByteCounter) Write(p []byte) (int, error) {
	*c += ByteCounter(len(p))
	return len(p), nil
}

func main() {
	var c ByteCounter
	c.Write([]byte("heloo"))
	fmt.Println(c) //  "5", = len("hello")
	c = 0          // reset the counter
	var name = "Dolly"
	fmt.Fprintf(&c, "hello, %s", name)
	fmt.Println(c) // "12", = len("hello, Dolly")
}
```

### `fmt.Stringer` 和 `fmt.GoStringer` 接口

除了 `io.Writer` 这个接口类型，还有另一个对 `fmt` 包很重要的接口类型。它定义在 `$GOROOT/src/fmt/fmt.go` 中

```go
// 普通的Print
type Stringer interface {
	String() string
}

// %#v format
type GoStringer interface {
	GoString() string
}
```

`Fprintf` 和 `Fprintln` 函数向类型提供了一种控制它们值输出的途径。给一个类型定义 `String` 方法，可以让它满足最广泛使用之一的接口类型 `fmt.Stringer`，在输出时就会按照 `String` 方法的定义进行输出。例如我们给 `*IntSet` 添加的 `String` 方法

## 接口类型

接口类型：**_描述了一系列方法的集合_**

> [!important] 实现接口类型
> 一个实现了接口类型的所有方法的 _具体类型_，就说该具体类型实现了该接口类型 

`io.Writer` 类型是用的最广泛的接口之一，因为它提供了所有的类型写入 `bytes` 的抽象，包括文件类型，内存缓冲区，网络链接，HTTP客户端，压缩工具，哈希等等。`io` 包中定义了很多其它有用的接口类型。`Reader` 可以代表任意可以读取 `bytes` 的类型，`Closer` 可以是任意可以关闭的值，例如一个文件或是网络链接。（到现在你可能注意到了很多 Go 语言中单方法接口的命名习惯）

```go
package io

type Reader interface {
	Read(p []byte) (n int, err error)
}

type Closer interface {
	Close() error
}
```

再往下看，我们发现有些 **_新的接口类型通过组合已经有的接口来定义_**

```go
type ReadCloser interface {
	Reader
	Closer
}

type ReadWriteCloser interface {
	Reader
	Writer
	Closer
}
```

上面用到的语法 **和结构内嵌相似**，我们可以用这种方式以一个简写命名另一个接口，而不用声明它所有的方法。这种方式本称为 **_接口内嵌_**。尽管略失简洁，我们可以像下面这样，不使用内嵌来声明 `io.Writer` 接口

```go
type ReadWriter interface {
    Read(p []byte) (n int, err error)
    Write(p []byte) (n int, err error)
}
```

或者甚至使用种混合的风格：

```go
type ReadWriter interface {
    Read(p []byte) (n int, err error)
    Writer
}
```

### 实现接口的条件

> [!tip] **一个类型如果拥有一个接口需要的所有方法，那么这个类型就实现了这个接口**

例如， `*os.File` 类型实现了 `io.Reader`，`Writer`，`Closer`，和 `ReadWriter` 接口。` *bytes.Buffer` 实现了 `Reader`，`Writer`，和 `ReadWriter` 这些接口，但是它没有实现 `Closer` 接口因为它不具有 `Close` 方法

Go 的程序员经常会简要的把一个具体的类型描述成一个特定的接口类型。举个例子， `*bytes.Buffer `是`io.Writer`； `*os.Files` 是 `io.ReadWriter`。

接口指定的规则非常简单：**_表达一个类型属于某个接口只要这个类型实现这个接口_**

```go
var w io.Writer
w = os.Stdout           // OK: *os.File has Write method
w = new(bytes.Buffer)   // OK: *bytes.Buffer has Write method
w = time.Second         // compile error: time.Duration lacks Write method

var rwc io.ReadWriteCloser
rwc = os.Stdout         // OK: *os.File has Read, Write, Close methods
rwc = new(bytes.Buffer) // compile error: *bytes.Buffer lacks Close method
```

这个规则甚至适用于 **等式右边本身也是一个接口类型**

```go
w = rwc                 // OK: io.ReadWriteCloser has Write method
rwc = w                 // compile error: io.Writer lacks Close method
```

因为 `ReadWriter` 和 `ReadWriteCloser` 包含所有 `Writer` 的方法，所以任何实现了 `ReadWriter` 和 `ReadWriteCloser` 的类型必定也实现了 `Writer` 接口

### 指针接收者和值接收者

> [!ERROR] 回顾： 一个类型持有一个方法当中的细节
> 对于每一个命名的具体类型 `T`；一些方法的接收者是类型 `T` 本身然而另一些则是一个 `*T` 的指针
> - 在 `T` 类型的参数上调用一个 `*T` 的方法是合法的，只要这个参数是一个变量；编译器隐式的获取了它的地址
> 
> 但这仅仅是一个语法糖：**_`T` 类型的值不拥有所有 `*T` 指针的方法_**，那这样它就可能只实现更少的接口

在 [[Go 语言：方法#示例：Bit 数组]] 中定义的 `IntSet` 类型的 `String` 方法的接收者是一个指针类型，所以我们 _不能在一个不能寻址的 `IntSet` 值上调用这个方法_

```go
type IntSet struct { /* ... */ }
func (*IntSet) String() string
var _ = IntSet{}.String() // compile error: String requires *IntSet receiver
```

但是我们可以在一个可寻址 `IntSet` 值(`IntSet` 变量)上调用这个方法

```go
var s IntSet
var _ = s.String() // OK: s is a variable and &s has a String method
```

然而，**由于只有 `*IntSet` 类型有 `String` 方法，所以也只有 `*IntSet` 类型实现了 `fmt.Stringer` 接口**

```go
var _ fmt.Stringer = &s // OK
var _ fmt.Stringer = s  // compile error: IntSet lacks String method
```

### 接口隐藏具体类型的实现

就像信封封装信件将隐藏信件起来一样，接口类型封装并隐藏具体类型和它的值。**即使具体类型有其它的方法也 _只有接口类型暴露出来的方法会被调用到_**

```go
os.Stdout.Write([]byte("hello")) // OK: *os.File has Write method
os.Stdout.Close()                // OK: *os.File has Close method

var w io.Writer
w = os.Stdout
w.Write([]byte("hello")) // OK: io.Writer has Write method
w.Close()                // compile error: io.Writer lacks Close method
```

### 空接口

一个有更多方法的接口类型，比如 `io.ReadWriter`，和少一些方法的接口类型,例如 `io.Reader`，进行对比；**更多方法的接口类型会告诉我们更多关于它的值持有的信息，并且对实现它的类型要求更加严格**。那么关于 **`interface{}` 类型，它没有任何方法**，请讲出哪些具体的类型实现了它？

这看上去好像没有用，但实际上 **`interface{}` 被称为空接口类型** 是不可或缺的。因为空接口类型 **对实现它的类型没有要求**，所以我们可 **以将任意一个值赋给空接口类型**

> [!important] 空接口对类型没有任何要求，也就是说，所有类型都满足空接口

```go
var any interface{}
any = true
any = 12.34
any = "hello"
any = map[string]int{"one": 1}
any = new(bytes.Buffer)
```

> [!NOTE] 空接口类型的使用示例
> `fmt.Println` 的参数就空接口类型，使得该函数可以处理任意类型的参数

对于创建的一个 `interface{}` 值持有一个 `boolean`，`float`，`string`，`map`，`pointer`，或者任意其它的类型；我们当然不能直接对它持有的值做操作，因为 `interface{}` 没有任何方法

### 断言类型是否实现某个接口

**因为接口实现只依赖于判断的两个类型的方法，所以 _没有必要定义一个具体类型和它实现的接口之间的关系_**

也就是说，尝试文档化和断言这种关系几乎没有用，所以并没有通过程序强制定义。下面的定义在编译期断言一个`*bytes.Buffer` 的值实现了 `io.Writer` 接口类型

```go
// *bytes.Buffer must satisfy io.Writer
var w io.Writer = new(bytes.Buffer)
```

因为任意 `*bytes.Buffer` 的值，甚至包括 `nil` 通过 `(*bytes.Buffer)(nil)` 进行显示的转换都实现了这个接口，所以我们不必分配一个新的变量。并且因为我们绝不会引用变量 `w`，我们可以使用空标识符来进行代替。总的看，这些变化可以让我们得到一个更朴素的版本

```go
// *bytes.Buffer must satisfy io.Writer
var _ io.Writer = (*bytes.Buffer)(nil)
```

**非空的接口类型比如 `io.Writer` 经常被指针类型实现**，尤其当一个或多个接口方法像 `Write` 方法那样隐式的给接收者带来变化的时候。一个结构体的指针是非常常见的承载方法的类型。

**但是并不意味着只有指针类型满足接口类型**，甚至连一些有设置方法的接口类型也可能会被 Go 语言中其它的引用类型实现。例如，在 `map` 类
型的方法(`url.Values`)，甚至基本的类型也可能会实现一些接口

**一个具体的类型可能实现了很多不相关的接口**。考虑在一个组织出售数字文化产品比如 **音乐**，**电影** 和 **书籍** 的程序中可能定义了下列的具体类型

```go
Album
Book
Movie
Magazine
Podcast
TVEpisode
Track
```

我们可以把每个抽象的特点用接口来表示。一些特性对于所有的这些 **文化产品都是共通的**，例如**标题**，**创作日期**和**作者列表**

```go
type Artifact interface {
    Title() string
    Creators() []string
    Created() time.Time
}
```

其它的一些特性只对特定类型的文化产品才有。和文字排版特性相关的只有 `books` 和 `magazines`，还有只有 `movies` 和 `TV` 剧集和屏幕分辨率相关

```go
type Text interface {
    Pages() int
    Words() int
    PageSize() int
}

type Audio interface {
    Stream() (io.ReadCloser, error)
    RunningTime() time.Duration
    Format() string // e.g., "MP3", "WAV"
}

type Video interface {
    Stream() (io.ReadCloser, error)
    RunningTime() time.Duration
    Format() string // e.g., "MP4", "WMV"
    Resolution() (x, y int)
}
```

这些接口不止是一种有用的方式来分组相关的具体类型和表示他们之间的共同特定。我们后面可能会发现其它的分组。举例，如果我们发现我们需要以同样的方式处理 `Audio` 和 `Video`，我们可以定义一个 `Streamer` 接口来代表它们之间相同的部分而不必对已经存在的类型做改变

```go
type Streamer interface {
    Stream() (io.ReadCloser, error)
    RunningTime() time.Duration
    Format() string
}
```

**每一个具体类型的组基于它们相同的行为可以表示成一个接口类型**。不像基于类的语言，他们一个类实现的接口集合需要进行显式的定义，**在 Go 语言中我们可以在需要的时候定义一个新的抽象或者特定特点的组，而不需要修改具体类型的定义**。当具体的类型来自不同的作者时这种方式会特别有用。当然也确实没有必要在具体的类型中指出这些共性。

## 示例：形状

在下面的程序中，显示计算各种形状的面积

```go
package shape

import "math"

// Point 平面上的点
type Point struct {
	X, Y float64
}

func (p Point) Area() float64 {
	return 0.0
}

// Line 线 由两个点表示
type Line struct {
	P1 Point
	P2 Point
}

func (l Line) Area() float64 {
	return 0.0
}

// Triangle 三角形 由平面上的三个点表示
type Triangle struct {
	A, B, C Point
}

func len(p1, p2 Point) float64 {
	return math.Hypot(p1.X-p2.X, p1.Y-p2.Y)
}

func (t Triangle) Area() float64 {
	line1 := len(t.A, t.B)
	line2 := len(t.A, t.B)
	line3 := len(t.B, t.C)
	p := (line1 + line2 + line3) / 2
	return math.Sqrt(p * (p - line1) * (p - line2) * (p - line3))
}

// Circle 圆 由平面上的一点点与半径表示
type Circle struct {
	Point
	Radius float64
}

func (c Circle) Area() float64 {
	return math.Pi * math.Pow(c.Radius, 2)
}
```

当我们在其他包中使用 `shape` 时，我们就会发现一个非常严重的问题

```go
package main

import (
	"fmt"
	"gocode/advanced/shape"
)

func main() {
	var p1 = shape.Point{X: 1, Y: 2}
	var p2 = shape.Point{X: 3, Y: 4}
	var p3 = shape.Point{X: 0, Y: 0}

	var triangle = shape.Triangle{A: p1, B: p2, C: p3}
	fmt.Println(triangle.Area())
	var circle = shape.Circle{Point: p3, Radius: 5.5}
	fmt.Println(circle.Area())
	// ....
	// 其他更多形状的面积
}
```

这里我们只有两个形状，但是当我们需要计算各种各样形状的面积时，就会在需要的地方插入其计算代码。导致代码冗余。显然这不是我们想要的。

一种更为常见的需求就是需要将形状放在同一个切片中，然后计算循环遍历它们并计算面积。这个存放各种形状的切片的类型是什么？很明显，它是约定了需要实现 `Area` 的接口类型。而且在 Go 中我们只需要简单的在包中声明该接口类型，无需修改具体类型的方法

```go
// Shape 一种抽象的形状
type Shape interface {
	Area() float64
}
```

我们可以这样修改我们使用的代码

```go
package main

import (
	"fmt"
	"gocode/advanced/shape"
)

func main() {
	var p1 = shape.Point{X: 1, Y: 2}
	var p2 = shape.Point{X: 3, Y: 4}
	var p3 = shape.Point{X: 0, Y: 0}
	var triangle = shape.Triangle{A: p1, B: p2, C: p3}
	var circle = shape.Circle{Point: p3, Radius: 5.5}
	var shapes = []shape.Shape{p1, p2, p3, triangle, circle} // 如果后续需要新添加形状，只需添加在这个切片中即可
	for _, shape := range shapes {
		fmt.Printf("面积位: %v\n", shape.Area())
	}
}
```

## 接口值

概念上讲一个接口的值，**接口值**，由两个部分组成，一个 **具体的类型** 和 **那个类型的值**。它们被称为 **接口的 _动态类型_ 和 _动态值_**。对于像 Go 语言这种静态类型的语言，**_类型是编译期的概念_**；因此 **_类型不是值_**

在我们的概念模型中，一些提供每个类型信息的值被称为 **_类型描述符_**，比如类型的名称和方法。在一个接口值中，**类型部分代表与之相关类型的描述符**

![[Pasted image 20250609205321.png]]


> [!important] 接口值：由两部分
> + 类型：这里的类型是一种 **类型描述符**，提供类型信息
> + 值：具体类型的值

下面 $4$ 个语句中，变量 `w` 得到了 $3$ 个不同的值。（开始和最后的值是相同的）

```go
var w io.Writer
w = os.Stdout
w = new(bytes.Buffer)
w = nil
```

让我们进一步观察在每一个语句后的 `w` 变量的值和动态行为。第一个语句定义了变量 `w`

```go
var w io.Writer
```

在 Go 语言中，**_变量总是被一个定义明确的值初始化_**，即使接口类型也不例外。对于一个 _接口的零值_ 就是它的 _类型_ 和 _值_ 的部分都是 `nil`

![[Pasted image 20250609205429.png]]

接口值通过其动态类型是否为 `nil`，来确定接口值是否为 `nil`

> [!warning] 注意：接口值的动态类型只要为 `nil`，那么这个接口值就 `nil`

可以通过使用 `w==nil` 或者 `w!=nil` 来判读接口值是否为空。调用一个空接口值上的任意方法都会产生 `panic` :

```go
w.Write([]byte("hello")) // panic: nil pointer dereference
```

> [!important] 接口类型的零值
> 接口类型的零值定义为：`type` 和 `value` 部分均为 `nil` 

第二个语句将一个 `*os.File` 类型的值赋给接口类型的变量 `w`

```go
w = os.Stdout
```

> [!important] 使用具体类型的值给接口变量赋值时。会触发具体类型到接口类型的转换
>  
> 这个赋值过程是调用 **具体类型到接口类型的隐式转换**，这和显式的使用 `io.Writer(os.Stdout)` 是等价的

对接口类型的变量赋值时，会将这个接口值的 
+ **动态类型被设为 `*os.Stdout` 指针的类型描述符**
+ **动态值持有 `os.Stdout` 的拷贝**

下图代表处理标准输出的 `os.File` 类型变量的指针

![[Pasted image 20250609205730.png]]

调用一个包含 `*os.File` 类型指针的接口值的 `Write` 方法，使得 `(*os.File).Write` 方法被调用。这个调用输出 `"hello"`

```go
w.Write([]byte("hello")) // "hello"
```

通常在编译期，我们不知道接口值的动态类型是什么，所以一个接口上的调用必须使用动态分配。因为不是直接进行调用，所以编译器必须把代码生成在类型描述符的方法 `Write` 上，然后间接调用那个地址。这个调用的接收者是一个接口动态值的拷贝，`os.Stdout`。效果和下面这个直接调用一样

```go
os.Stdout.Write([]byte("hello")) // "hello"
```

第三个语句给接口值赋了一个 `*bytes.Buffer` 类型的值

```go
w = new(bytes.Buffer)
```

现在动态类型是`*bytes.Buffer`并且动态值是一个指向新分配的缓冲区的指针

![[Pasted image 20250609205705.png]]

`Write` 方法的调用也使用了和之前一样的机制

```go
w.Write([]byte("hello")) // writes "hello" to the bytes.Buffers
```

这次类型描述符是 `*bytes.Buffer`，所以调用了`(*bytes.Buffer).Write` 方法，并且接收者是该缓冲区的地址。这个调用把字符串 `"hello"` 添加到缓冲区中

最后，第四个语句将 `nil` 赋给了接口值

```go
w = nil
```

这个重置将它所有的部分都设为 `nil` 值，把变量 `w` 恢复到和它之前定义时相同的状态图

![[Pasted image 20250609205801.png]]


### 接口值可以持有任意大小的动态值

**一个接口值可以持有任意大的动态值**。例如，表示时间实例的 `time.Time` 类型，这个类型有几个对外不公开的字段。我们从它上面创建一个接口值

```go
var x interface{} = time.Now()
```

从概念上讲，不论接口值多大，动态值总是可以容下它。（这只是一个概念上的模型；具体的实现可能会非常不同）

![[Pasted image 20250609205822.png]]

### 接口值的比较

接口值可以使用`==`和 `!=` 来进行比较。两个接口值相等仅当它们 **都是 `nil` 值** 或者它们的 **动态类型相同** 并且 **动态值也根据这个动态类型的`==` 操作相等**。因为接口值是可比较的，所以它们可以用在 `map` 的键或者作为 `switch` 语句的操作数

> [!important] 如果两个接口值的动态类型相同并且是可比较的，那么这个接口值可以作为 `map` 的间，或者 `switch` 的操作数

然而，如果两个接口值的 **动态类型相同**，但是这个 **动态类型是不可比较的（比如切片）**，将它们进行比较就会失败并且 `panic`:

```go
var x interface{} = []int{1, 2, 3}
fmt.Println(x == x) // panic: comparing uncomparable type []int
```

考虑到这点，接口类型是非常与众不同的。其它类型要么是安全的可比较类型（如基本类型和指针）要么是完全不可比较的类型（如切片，映射类型，和函数），但是 **在比较接口值或者包含了接口值的聚合类型时，我们必须要意识到潜在的 `panic`**。同样的风险也存在于使用接口作为 `map` 的键或者 `switch` 的操作数。只能比较你非常确定它们的动态值是可比较类型的接口值

> [!important] 接口类型的值的比较需要注意潜在的 `panic`

### `%T` 可以显示接口值的动态类型

当我们处理错误或者调试的过程中，得知接口值的动态类型是非常有帮助的。所以我们使用 `fmt` 包的 `%T` 动作

```go
var w io.Writer
fmt.Printf("%T\n", w) // "<nil>"
w = os.Stdout
fmt.Printf("%T\n", w) // "*os.File"
w = new(bytes.Buffer)
fmt.Printf("%T\n", w) // "*bytes.Buffer"
```

在 `fmt` 包内部，使用 [[Go 语言：反射]] 来获取接口动态类型的名称

### 接口动态值为 `nil` 指针的接口不是 `nil` 接口

> [!warning] 
> 
> 当用一个具体类型的 `nil` 指针给接口变量赋值时，接口变量就不在是 `nil` 接口了
> 

思考下面的程序。当 `debug` 变量设置为 `true` 时，`main` 函数会将 `f` 函数的输出收集到一个 `bytes.Buffer` 类型中

```go
const debug = true
func main() {
    var buf *bytes.Buffer  // nil
    if debug {
    
        buf = new(bytes.Buffer) // enable collection of output
    }
    
    f(buf) // NOTE: subtly incorrect!
    
    if debug {
        // ...use buf...
    }
}

// If out is non-nil, output will be written to it.
func f(out io.Writer) {
    // ...do something...
    if out != nil {
        out.Write([]byte("done!\n"))
    }
}
```

我们可能会预计当把变量 `debug` 设置为 `false` 时可以禁止对输出的收集，但是实际上在 `out.Write` 方法调用时程序发生了 `panic`

```go
if out != nil {
    out.Write([]byte("done!\n")) // panic: nil pointer dereference
}
```

当 `main` 函数调用函数 `f` 时，它给 `f` 函数的 `out` 参数赋了一个 `*bytes.Buffer` 的 `nil`，所以 `out` 的动态值是 `nil`。然而，它的动态类型是 `*bytes.Buffer`，意思就是 `out` 变量是一个 **包含空指针值的非空接口**，所以防御性检查 `out!=nil` 的结果依然是 `true`

![[Pasted image 20250609210018.png]]

动态分配机制依然决定 `(*bytes.Buffer).Write` 的方法会被调用，但是这次的接收者的值是 `nil`。对于一些如 `*os.File` 的类型，`nil` 是一个有效的接收者，但是 `*bytes.Buffer` 类型不在这些类型中。这个方法会被调用，但是当它尝试去获取缓冲区时会发生 `panic`

问题在于尽管一个 `nil` 的 `*bytes.Buffer` 指针有实现这个接口的方法，它也不满足这个接口具体的行为上的要求。特别是这个调用违反了 `(*bytes.Buffer).Write` 方法的接收者非空的隐含先决条件，所以将 `nil` 指针赋给这个接口是错误的。解决方案就是将 `main` 函数中的变量 `buf` 的类型改为 `io.Writer`，因此可以避免一开始就将一个不完全的值赋值给这个接口

```go
var buf io.Writer  // 这里是 io.Writer 接口

if debug {
    buf = new(bytes.Buffer) // enable collection of output
}

f(buf) // OK
```

## 类型断言

**类型断言是一个使用在接口值上的操作**。语法上它看起来像 **`x.(T)` 被称为断言类型**，这里`x`表示一个接口的类型和`T`表示一个类型。一个类型断言检查它操作对象的动态类型是否和断言的类型匹配

### 断言具体类型

这里有两种可能。第一种，如果 **断言的类型 `T` 是一个具体类型**，然后类型断言检查 `x` 的动态类型是否和 `T` 相同。该语法返回两个参数，第一个参数是 `x` 转化为 `T` 类型后的变量，第二个值是一个布尔值，若为 `true` 则表示断言成功，为 `false`则表示断言失败

```go
package main

import "fmt"

type Mover interface {
	Move()
}

type Dog struct {
	Name string
}

func (d Dog) Move() {
	fmt.Printf("%s 狗狗正在移动", d.Name)
}

type Cat struct {
	Name string
}

func (c Cat) Move() {
	fmt.Printf("%s 猫猫正在移动", c.Name)
}

func main() {
	var m Mover
	m = &Cat{"小白"}
	fmt.Printf("%T\n", m) // *main.Cat

	m = &Dog{"旺财"}
	fmt.Printf("%T\n", m) // *main.Dog

	v, ok := m.(*Dog)
	if ok {
		fmt.Printf("检测成功, 狗狗的名为 %s\n", v.Name) // 检测成功, 狗狗的名为 旺财
	}
}
```

### 断言接口类型

第二种，如果相反 **断言的类型 `T` 是一个接口类型**，然后 **类型断言检查是否 `x` 的动态类型满足 `T`**。如果这个检查成功了，**动态值没有获取到**；这个结果仍然是一个有相同类型和值部分的接口值，但是结果有类型 `T`。换句话说，**对一个接口类型的类型断言改变了类型的表述方式，改变了可以获取的方法集合（通常更大），但是它保护了接口值内部的动态类型和值的部分**

在下面的第一个类型断言后，`w` 和 `rw` 都持有 `os.Stdout` 因此它们每个有一个动态类型 `*os.File` ，但是变量 `w` 是一个 `io.Writer` 类型只对外公开出文件的 `Write` 方法，然而 `rw` 变量也只公开它的 `Read` 方法

```go
var w io.Writer
w = os.Stdout
rw := w.(io.ReadWriter) // success: *os.File has both Read and Write
w = new(ByteCounter)
rw = w.(io.ReadWriter) // panic: *ByteCounter has no Read method
```

如果断言操作的对象是一个 `nil` 接口值，那么不论被断言的类型是什么这个类型断言都会失败。我们几乎不需要对一个更少限制性的接口类型（更少的方法集合）做断言，因为它表现的就像赋值操作一样，除了对于 `nil` 接口值的情况

```go
w = rw             // io.ReadWriter is assignable to io.Writer
w = rw.(io.Writer) // fails only if rw == nil
```

经常地我们对一个接口值的动态类型是不确定的，并且我们更愿意去检验它是否是一些特定的类型。如果类型断言出现在一个预期有两个结果的赋值操作中，例如如下的定义，这个操作不会在失败的时候发生 `panic` 但是代替地返回一个额外的第二个结果，这个结果是一个标识成功的布尔值：

```go
var w io.Writer = os.Stdout
f, ok := w.(*os.File)      // success:  ok, f == os.Stdout
b, ok := w.(*bytes.Buffer) // failure: !ok, b == nil
```

第二个结果常规地赋值给一个命名为 `ok` 的变量。如果这个操作失败了，那么 `ok` 就是 `false` 值，第一个结果等于被断言类型的零值，在这个例子中就是一个`nil`的 `*bytes.Buffer` 类型

这个 `ok` 结果经常立即用于决定程序下面做什么。`if` 语句的扩展格式让这个变的很简洁

```go
if f, ok := w.(*os.File); ok {
    // ...use f...
}
```

当类型断言的操作对象是一个变量，你有时会看见原来的变量名重用而不是声明一个新的本地变量，这个 **重用的变量** 会覆盖原来的值
```go
if w, ok := w.(*os.File); ok {
    // ...use w...
}
```

### `switch-type`

如果对一个接口值有多个实际类型需要判断，推荐使用 `switch` 语句来实现

```go
// justifyType 对传入的空接口类型变量x进行类型断言
func justifyType(x interface{}) {
	switch v := x.(type) {
	case string:
		fmt.Printf("x's type is a string，value is %v\n", v)
	case int:
		fmt.Printf("x's type is  int is %v\n", v)
	case bool:
		fmt.Printf("x's type is a bool is %v\n", v)
	default:
		fmt.Println("unsupport type！")
	}
}
```
