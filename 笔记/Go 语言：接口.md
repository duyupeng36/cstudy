# 接口

接口类型是对其它类型行为的抽象和概括；因为接口类型不会和特定的实现细节绑定在一起，通过这种抽象的方式我们可以让我们的函数更加灵活和更具有适应能力

在 Go 语言中，接口类型的独特之处在于它是 **隐式实现的**。换句话说，没有必要对于给定的具体类型定义所有满足的接口类型；简单地拥有一些必需的方法就足够了。这种设计可以让你创建一个新的接口类型满足已经存在的具体类型却不会去改变这些类型的定义；当我们使用的类型来自于不受我们控制的包时这种设计尤其有用

## 接口既是约定

我们之前学习的过程中，遇见的所有的类型都称为 **具体类型**。所谓的具体类型就是可以准确的描述它所代表的值，并且展示出对类型本身的一些操作方式

> [!tip] 
> 
> 就像数字类型的算术操作，切片类型的取下标、添加元素和范围获取操作
> 

具体的类型还可以通过它的内置方法提供额外的行为操作。总的来说，当你拿到一个具体的类型时你就知道它的本身是什么和你可以用它来做什么

在 Go 语言中还存在着另外一种类型：**接口类型**。接口类型是一种 **抽象的类型**。它 **不会暴露** 出它所代表的 **对象的内部值的结构** 和 **这个对象支持的基础操作的集合**；它们 **只会展示出它们自己的方法**

> [!attention] 
> 
> 也就是说当你有看到一个接口类型的值时，你不知道它是什么，唯一知道的就是可以通过它的方法来做什么
> 

> [!summary] 
> 
> 接口类型是一种抽象类型，它只暴露对象中的部分方法，而不暴露对象的内部实现(值结构)
> 

我们首先来看几个常用的接口

### io.Writer 接口

下面我们来看一个字符串的格式化函数：`fmt.Printf()` 它把结果 **写到标准输出**。正在 `$GOROOT/src/fmt/print.go` 文件中的定义如下

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

`Fprintf()` 的前缀 `F` 表示文件(`File`)，即表明格式化输出结果应该被写入第一个参数提供的文件中。在 `Printf()` 函数中的第一个参数 `os.Stdout` 是 `*os.File` 类型

然而，`Fprintf()` 函数中的第一个参数却不是一个文件类型。它是 `io.Writer `类型这是一个接口类型定义在 `$GOROOT/src/io/io.go`

```go
type Writer interface {
	Write(p []byte) (n int, err error)
}
```

`io.Writer` 接口类型定义了函数(`Fprintf`)和这个函数调用者之间的约定。这个约定需要 **调用者提供具体类型的值** 就像 `*os.File`，这些类型都有一个特定签名和行为的 `Write()` 的方法

这个约定保证了 `Fprintf()` 接受任何满足 `io.Writer` 接口的值都可以工作。`Fprintf()` 函数可能没有假定写入的是一个文件或是一段内存，而是写入一个可以调用 `Write()` 方法的值也是可以的

换句话说，只要第一个参数可以安全地传入一个任何具体类型的值只需要满足 `io.Writer` 接口的值即可

> [!important] 可替换性
> 
> **一个类型可以自由的使用另一个满足相同接口的类型来进行替换被称作 _可替换性(LSP里氏替换)_**。这是一个面向对象的特征
> 

下面是使用 `io.Writer` 接口的一个示例。下面 `*ByteCounter` 类型里的 `Write` 方法，仅仅在丢失写向它的字节前统计它们的长度。(在这个 `+=` 赋值语句中，让 `len(p)` 的类型和 `*c `的类型匹配的转换是必须的。)

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

`Fprintf()` 和 `Fprintln()` 函数向类型提供了一种控制它们值输出的途径。给一个类型定义 `String()` 方法，可以让它满足最广泛使用之一的接口类型 `fmt.Stringer`，在输出时就会按照 `String()` 方法的定义进行输出。例如我们给 `*BitSet` 添加的 `String()` 方法

## 接口类型

在 Go 语言中，**接口类型就是方法集合**，只要一个具体类型实现了接口要求的所有方法，那么就称这个具体类型实现了该接口类型

> [!important] 实现接口类型
> 
> 一个实现了接口类型的所有方法的 _具体类型_，就说该具体类型实现了该接口类型 
> 

`io.Writer` 类型是用的最广泛的接口之一，因为它提供了所有的类型写入 `bytes` 的抽象，包括文件类型，内存缓冲区，网络链接，HTTP，压缩工具，哈希等等。`io` 包中定义了很多其它有用的接口类型。
+ `Reader` 可以代表任意可以读取 `bytes` 的类型
+ `Closer` 可以是任意可以关闭的值，例如一个文件或是网络链接

```go
package io

type Reader interface {
	Read(p []byte) (n int, err error)
}

type Closer interface {
	Close() error
}
```

现在，我们应该可以注意到了很多 Go 语言中 **单方法接口** 的命名习惯。在 Go 语言中，单方法接口的命名通常以 `er` 结尾

此外，我们发现有些 **新的接口类型通过组合已经有的接口来定义**

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

上面用到的语法 **和结构内嵌相似**，我们可以用这种方式以一个简写命名另一个接口，而不用声明它所有的方法。这种方式本称为 **接口内嵌**。尽管略失简洁，我们可以像下面这样，不使用内嵌来声明 `io.Writer` 接口

```go
type ReadWriter interface {
    Read(p []byte) (n int, err error)
    Write(p []byte) (n int, err error)
}
```

甚至，我们可以混合这两种风格

```go
type ReadWriter interface {
    Read(p []byte) (n int, err error)
    Writer
}
```

### 实现接口的条件

> [!tip] 
> 
> 如果某个类型如果拥有接口需要的所有方法，那么这个类型就实现了该接口
> 

例如， `*os.File` 类型实现了 `io.Reader`，`io.Writer`，`io.Closer` 接口。` *bytes.Buffer` 实现了 `io.Reader`，`io.Writer`，和 `io.ReadWriter` 这些接口，但是它没有实现 `io.Closer` 接口因为它不具有 `Close()` 方法

Go 的程序员经常会简要的把一个具体的类型描述成一个特定的接口类型。举个例子， `*bytes.Buffer `是 `io.Writer`； `*os.Files` 是 `io.ReadWriter` 

接口指定的规则非常简单：**表达一个类型属于某个接口只要这个类型实现这个接口**

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

因为 `io.ReadWriter` 和 `io.ReadWriteCloser` 包含所有 `io.Writer` 的方法，所以任何实现了 `io.ReadWriter` 和 `io.ReadWriteCloser` 的类型必定也实现了 `io.Writer` 接口

### 指针接收器和值接收器

> [!attention] 回顾： 一个类型持有一个方法当中的细节
> 
> 对于每一个命名的具体类型 `Type`；一些方法的接收器是类型 `Type` 本身，然而另一些则是一个 `*Type` 
> 
> - 在 `Type` 类型的参数上调用一个 `*Type` 的方法是合法的，只要这个参数是一个变量；编译器隐式的获取了它的地址
> 
> 但这仅仅是一个语法糖：`Type` 类型的接受器与 `*Type` 类型接受器是两个不同的类型，换句话说，`*Type` 类型的方法不是 `Type` 的方法
> 

在 [[Go 语言：方法#示例程序：实现 BitSet 集合]] 中定义的 `BitSet` 类型的 `String()` 方法的接收者是一个指针类型，即 `*BitSet`，所以我们 **不能在一个不能寻址的 `BitSet` 值上调用这个方法**

```go
type BitSet struct { /* ... */ }
func (*BitSet) String() string
var _ = BitSet{}.String() 
// compile error: String requires *IntSet receiver
```

> [!attention] 
> 
> 请注意：字面值是不能寻址的
> 

但是我们可以在一个可寻址 `BitSet` 值(`BitSet` 变量)上调用这个方法

```go
var s BitSet
var _ = s.String() 
// OK: s is a variable and &s has a String method
```

然而，**由于只有 `*BitSet` 类型有 `String()` 方法，所以也只有 `*BitSet` 类型实现了 `fmt.Stringer` 接口**

```go
var _ fmt.Stringer = &s // OK
var _ fmt.Stringer = s  
// compile error: IntSet lacks String method
```

### 空接口

一个有更多方法的接口类型，比如 `io.ReadWriter`，和少一些方法的接口类型,例如 `io.Reader`，进行对比；更多方法的接口类型会告诉我们更多关于它的值持有的信息，并且对实现它的类型要求更加严格。那么关于 `interface{}` 类型，它没有任何方法，请讲出哪些具体的类型实现了它？

这看上去好像没有用，但实际上 `interface{}` 被称为 **空接口类型** 是不可或缺的。因为空接口类型 **对实现它的类型没有要求**，所以我们可 **以将任意一个值赋给空接口类型**

> [!important]
> 
> 空接口对类型没有任何要求，也就是说，所有类型都满足空接口
> 

```go
var any interface{}
any = true
any = 12.34
any = "hello"
any = map[string]int{"one": 1}
any = new(bytes.Buffer)
```

> [!example] 
> 
> `fmt.Println()` 的参数就空接口类型，使得该函数可以处理任意类型的参数
> 

> [!warning] 
> 
> 对于创建的一个 `interface{}` 值持有一个 `boolean`，`float`，`string`，`map`，`pointer`，或者任意其它的类型
> 
> 请注意：我们不能直接对它持有的值做操作，因为 `interface{}` 没有任何方法
> 

### 接口断言

**接口实现只依赖于类型的方法**，因此没有必要定义一个具体类型和它实现的接口之间的关系

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

非空的接口类型比如 `io.Writer` 经常被指针类型实现，尤其当一个或多个接口方法像 `Write()` 方法那样隐式的给接收者带来变化的时候。一个结构体的指针是非常常见的承载方法的类型。

**但是并不意味着只有指针类型满足接口类型**，甚至连一些有设置方法的接口类型也可能会被 Go 语言中其它的引用类型实现。例如，在 `map` 类型的方法(`url.Values`)，甚至基本的类型也可能会实现一些接口

## 接口变量

在 Go 语言中，接口是以类型的形式出现。也就是说，我们可以声明接口类型的变量。为了满足接口类型的动态特性，接口变量包含两个部分： **具体的类型** 和 **那个类型的值**。它们被称为接口的 **动态类型** 和 **动态值**

对于像 Go 语言这种静态类型的语言，**类型是编译期的概念**。因此，类型不是值。为了在运行时描述类型这一概念，Go 语言为每个类型都设置了 **类型描述符**，比如类型的名称和方法。在一个接口值中，**类型部分代表与之相关类型的描述符**。如下图，展示接口类型的值的结构

![[Pasted image 20250622222821.png]]

下面 $4$ 个语句中，变量 `w` 得到了 $3$ 个不同的值。（开始和最后的值是相同的）

```go
var w io.Writer        // 零值初始化 nil 接口
w = os.Stdout          // 接口被设置为 os.Stdout 
w = new(bytes.Buffer)  // 接口被设置为 *bytes.Buffer
w = nil                // 零值接口
```

### nil 接口

让我们进一步观察在每一个语句执行后的接口变量 `w` 的动态类型和值。第一个语句定义了变量 `w`

```go
var w io.Writer
```

在 Go 语言中，**变总是被一个定义明确的值初始化_**，即使接口类型也不例外。对于一个接口的零值就是它的 **类型** 和 **值** 两部分都是 `nil`。如下图所示

![[Pasted image 20250622223223.png]]

在 Go 语言中，接口变量通过其动态类型是否为 `nil`，来确定接口值是否为 `nil`

> [!attention] 
> 
> 接口值的动态类型只要为 `nil`，那么这个接口值就 `nil`
> 

可以通过使用 `w == nil` 或者 `w != nil` 来判读接口值是否为空。调用一个空接口上的任意方法都会产生 `panic` :

```go
w.Write([]byte("hello")) // panic: nil pointer dereference
```

### 接口变量赋值

第二个语句将一个 `*os.File` 类型的值赋给变量 `w`

```go
w = os.Stdout
```

> [!important] 
> 
> 使用具体类型的值给接口变量赋值时，会触发具体类型到接口类型的转换。这个赋值过程是调用 **具体类型到接口类型的隐式转换**，这和显式的使用 `io.Writer(os.Stdout)` 是等价的
> 

对接口类型的变量赋值时，会将这个接口变量的 动态类型被设为 `*os.Stdout` 的类型描述符，动态值持有 `os.Stdout` 的拷贝。下图代表处理标准输出的 `os.File` 类型变量的指针

![[Pasted image 20250622223815.png]]

第三个语句给接口值赋了一个 `*bytes.Buffer` 类型的值

```go
w = new(bytes.Buffer)
```

现在动态类型是`*bytes.Buffer`并且动态值是一个指向新分配的缓冲区的指针

![[Pasted image 20250622223921.png]]

### 动态值尺寸是任意的

**一个接口值可以持有任意大的动态值**。例如，表示时间实例的 `time.Time` 类型，这个类型有几个对外不公开的字段。我们从它上面创建一个接口值

```go
var x interface{} = time.Now()
```

从概念上讲，不论接口值多大，动态值总是可以容下它。（这只是一个概念上的模型；具体的实现可能会非常不同）

![[Pasted image 20250622224049.png]]

### 动态值为 nil 的接口不是 nil 接口

> [!attention] 
> 
> 请注意：动态值为 nil 的接口不一定是 nil 接口。只要动态类型不是 nil，那么接口就不是 nil 接口
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
	// var out io.Writer = (*byes.Buffer)(nil)
	// 此时，这个 out 不是 nil 接口，
	// 因为其动态值被设置为了 *bytes.Buffer
    // ...do something...
    if out != nil {
        out.Write([]byte("done!\n"))
    }
}
```

上述问题出现的原因在于尽管一个 `nil` 的 `*bytes.Buffer` 指针有实现这个接口的方法，它也不满足这个接口具体的行为上的要求。特别是这个调用违反了 `(*bytes.Buffer).Write` 方法的接收者非空的隐含先决条件，所以将 `nil` 指针赋给这个接口是错误的

解决方案就是将 `main()` 函数中的变量 `buf` 的类型改为 `io.Writer`，因此可以避免一开始就将一个不完全的值赋值给这个接口

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

### switch-type 语句

如果对一个接口值有多个实际类型需要判断，推荐使用 `switch` 语句来实现

```go
// justifyType 对传入的空接口类型变量x进行类型断言
func justifyType(x interface{}) {
	switch v := x.(type) {
	case string:
		fmt.Printf("x is a string，value is %v\n", v)
	case int:
		fmt.Printf("x is a int is %v\n", v)
	case bool:
		fmt.Printf("x is a bool is %v\n", v)
	default:
		fmt.Println("unsupport type！")
	}
}
```

