# 函数基础

由若干语句组成的 **语句块**、函数**名称**、**参数列表**、**返回值** 构成，它是 **组织代码的最小单元**，完成一定的功能

> [!important] Go 函数由 **语句块**、**名称**、**参数列表**、**返回值列表** 组成
> 函数是组织代码的最小单元，它完成一定的功能

> [!tip] 函数的作用
> + 结构化编程对代码的最基本的 **封装**，一般按照功能组织一段代码
> + 封装的目的为了 **复用**，**减少冗余** 代码
> + 代码更加简洁美观、可读易懂

---
## 函数声明与调用

### 函数声明

Go 使用 `func` 关键字声明函数，包括 **函数名**、**形式参数列表**、**返回值列表**（可省略）以及 **函数体**。

```go
func 函数名(参数列表) [(返回值列表)]{
    函数体（代码块）
   [return 返回值]
}
```

**函数名** 就是标识符，命名要求和变量一样

**形式参数列表** 描述了函数的 **参数名** 以及 **参数类型**。这些参数作为 **局部变量**，**值由参数调用者提供**

> [!done] 如果一组 **形参相同的类型**，不必为每个形参都写出参数类型，**只需要在最后一个形参给出这组形参的类型** 即可

**返回值列表** 描述了函数返回值的 **变量名** 以及 **类型**。如果函数返回一个无名变量或者没有返回值，返回值列表的括号是可以省略的

> [!done] Go 函数的返回值 **可以命名** 并且 **可以返回多个值**
> + 返回一个值并且没有命名时，圆括号可以省略
> + 如果一个函数声明 **不包括返回值列表**，那么函数体执行完毕后，**不会返回任何值**
> + 如果对一组相同类型的返回值进行了命名，则只需要给出最后一个返回值的类型即可

> [!important]  Go语言中形参也被称为 **入参**，返回值也被称为 **出参**

`函数名(参数列表) [(返回值列表)]` 这部分称为 **函数签名**，每一个签名确定一个函数的类型

> [!warning] `{` 必须与关键字 `func` 在同一行，即返回值列表之后

下面给出 `4` 种方法声明拥有 `2` 个 `int` 型参数和 `1` 个 `int` 型返回值的函数。**空白标识符**(下文的 `_` 符号)可以 **强调某个参数未被使用**

```go

func add(x int, y int) int {
	return x + y
}

// 如果返回值进行了命名，则圆括号是不可省略的
func sub(x, y int) (z int) { 
	z = x - y
	return
}

func first(x int, _ int) int {
	return x 
}

func zero(int, int) int { 
	return 0 
}

fmt.Printf("%T\n", add)   // "func(int, int) int"
fmt.Printf("%T\n", sub)   // "func(int, int) int"
fmt.Printf("%T\n", first) // "func(int, int) int"
fmt.Printf("%T\n", zero)  // "func(int, int) int"
```

如果一组形参或返回值有相同的类型，不必为每个形参都写出参数类型。下面 2 个声明是等价的

```go
func f(i, j, k int, s, t string){
	/* ... */ 
}

func f(i int, j int, k int,  s string, t string) {
	/* ... */ 
}
```

### 函数调用

函数声明，只是声明了一个函数，它还未被执行，需要 **调用** 执行

调用的方式，就是 **函数名后加上小括号**，如有必要 **在括号内填写上实参列表**

> [!important] 实参
> 调用时写的参数是实际参数，是实实在在传入的值，简称 **实参**，这个过程称为 **传实参**，简称传参

如果定义了返回值列表，就需要配合使用 `return` 来返回这些值

### 示例

在 `hypot` 函数中,

```go
// 函数声明
// x、y是形式参数，result 是返回值
func hypot(x, y float64) float64 {
	return math.Sqrt(x * x + y * y)
}

func main() {
	// 调用函数
	result := hypot(3,4)  // 返回值保存在 result
	fmt.Println(result) // "5"
}
```

`x` 和 `y` 是形参名，`3` 和 `4` 是调用时的传入的实数，函数返回了一个 `float64` 类型的值。 **返回值也可以像形式参数一样被命名**

```go
func hypot(x, y float64) (z float64) {
	z = math.Sqrt(x * x + y * y)
	return 
}
```

在这种情况下，每个返回值被声明成一个局部变量，并根据该返回值的类型，将其初始化为 `0`。 如果一个函数在声明时，包含返回值列表，**该函数必须以 `return` 语句结尾**，除非函数明显无法运行到结尾处

> [!warning] 声明函数时如果包含了返回值列表，则该函数必须以 `return` 语句结尾。除非函数明显无法运行到结尾处
> + 函数在结尾时调用了 `panic` 异常
> + 函数体中有无限循环

```go

func add(x, y int) int {
	result := x + y // 函数体
	return result   // 返回值
}

func main() {
	out := add(4, 5) // 函数调用，可能有返回值，使用变量接收这个返回值
	fmt.Println(out) // 对于Println函数来说，这也是调用，传入了out实参
}
```

上面代码解释：
+ 定义一个函数 `add`，函数名是 `add`，能接受 `2` 个 `int` 类型的参数
+ 该函数计算的结果，通过返回值返回，需要 `return` 语句
+ 调用时，通过函数名 `add` 后加 `2` 个参数，返回值可使用变量接收
+ 函数名也是标识符
+ 返回值也是值
+ **一般习惯上函数定义需要在调用之前，也就是说调用时，函数已经被定义过了**。请在书写代码时，也尽量这样做，便于阅读代码

---
## 函数调用机制

函数调用机制的一个 **关键特性**（大多数语言也是如此）在于 **使用了 _栈_ 数据结构提供的 _后进先出_ 的内存管理原则**

```go
func Q(x, y int){

}
	

func P(){
	// 在栈上为 Q 申请栈空间
	Q(3, 4)  // 调用 Q
	// 到此处，Q 函数返回，它的栈空间会被释放	 
}

func main() {
	// main 函数为 P 申请栈空间
	P()  // 调用 P 函数
	// 此处时 P 调用完成，P 的栈空间被销毁
}
```

![[Pasted image 20241006215440.png]]

在 **函数P** 调用 **函数Q** 的例子中，可以看到当 Q 在执行时，P 以及所有向上追溯到 P 的调用链中的过程，都是暂时被挂起的
+ **当 Q 运行时**，它只需要为 **局部变量分配新的存储空间**，或 **设置到另一个过程调用**
+ **当 Q 返回时**，**任何被它分配的存储空间都会被释放**

因此，程序可以用栈来管理它的过程所需要的存储空间，栈和程序寄存器存放在传递控制和数据、分配内存所需要的信息
+ **当 P 调用 Q 时**，控制和数据信息添加到栈顶
+ **当 Q 返回时**，这些信息被释放

> [!important] 视频参考
> + [【Golang】函数调用栈（上）栈帧布局与函数跳转~](https://www.bilibili.com/video/BV1WZ4y1p7JT/)
> + [【Golang】函数调用栈（下）：传参和返回值](https://www.bilibili.com/video/BV1tZ4y1p7Rv)

---
## 函数类型与函数值

**函数的签名确定一种函数类型**，相同类型的函数具有相同的签名

```go
package main
import "fmt"
func fn1()             {}
func fn2(i int) int     { return 100 }
func fn3(j int) (r int) { return 200 }

func main() {
	fmt.Printf("%T\n", fn1)  // func()
	fmt.Printf("%T\n", fn2)  // func(int) int
	fmt.Printf("%T\n", fn3)  // func(int) int
}

```

输出如下

```
func()
func(int) int
func(int) int
```

在 Go 中，函数被看作 **第一类值**（first-class values）：函数像其他值一样，_**拥有类型**_，可以 _**被赋值给其他变量**_，_**传递给函数**_，_**从函数返回**_。对函数值（function value）的调用类似函数调用

```go
func square(n int) int { return n * n }      // 类型为 func(int) int
func negative(n int) int { return -n }       // 类型为 func(int) int
func product(m, n int) int { return m * n }  // 类型为 func(int, int) int

f := square
fmt.Println(f(3)) // "9"

f = negative
fmt.Println(f(3))     // "-3"
fmt.Printf("%T\n", f) // "func(int) int"

f = product // compile error: can't assign func(int, int) int to func(int) int
```

**函数类型的零值是 `nil`**。调用值为 `nil` 的函数值会引起 `panic` 错误

```go
var f func(int) int
f(3) // 此处f的值为nil, 会引起panic错误
```

> [!important] 函数值之间是不能比较的
> **函数值 _只能_ 与 `nil` 比较**，**_函数值之间是不可比较的_**，也不能用函数值作为 `map` 的 `key`

---

## 返回值

> [!important] 返回值变量是 **局部变量**，如果函数有返回值，在调用时可以使用变量接收
> + 如果没有返回值，这不能使用变量进行接收
> + 如果返回多个值，需要使用多个变量进行接收。如果不需要使用返回值，也可以不需要接收返回值

### 无返回值函数

在 Go 中仅仅一个 `return` 并不一定表示无返回值，只能说在一个无返回值的函数中，`return` 表示无返回值函数返回。

```go
// 无返回值函数，return 语句可有可无，必要时也需要使用 return
func fn1() {
	fmt.Println("无返回值")
	return // return 可有可无，如有需要，在必要的时候使用 return 来返回
}

t := fn1()         // 错误，无返回值函数无返回值可用
fmt.Println(fn1()) // 错误，无返回值函数无返回值可打印
```

### 返回一个值

在函数体中，必须显式执行 `return`

```go
// 返回一个值，没有变量名只有类型
func fn2() int {
	a := 100
	return a + 1 // return后面只要类型匹配就行
}

fmt.Println(fn2()) // 返回101
t := fn2()         // 返回101
```

**返回值值可以命名**：对于命名返回值 `return` 语句后可以省略

```go
// 返回一个值，有变量名和类型
func fn3() (r int) {
	r = 200
	return r - 1 // 类型匹配
}

fmt.Println(fn3())

//上面的函数还可以写成下面的形式
func fn3() (r int) {
	r = 200
	return // 如果返回的标识符就是返回值列表中的标识符，可以省略
}

fmt.Println(fn3())
```

### 返回多值

**Go 允许函数返回多个值**

```go
// 返回多个值。注意，返回值列表两边的圆括号是必不可少的
func fn4() (int, bool) {
	a, b := 100, true
	return a, b
}

fmt.Println(fn4())
x, y := fn4() // 需要两个变量接收返回值
```

> [!important] 多返回值列表需要使用 圆括号(`()`) 括起来
> + 圆括号是必不可少的

> [!important] 多返回值的函数调用时，需要使用多个变量接收函数的返回值

同样，多返回值也是可以命名的

```go
// 返回多个值
func fn4() (i int, b bool) {
	i, b = 100, true
	return // 如果和返回值列表定义的标识符名称和顺序一样，可省略
}

fmt.Println(fn4())
x, y := fn4() // 需要两个变量接收返回值
```

下面写法对吗？

```go
// 下面写法对吗？
func fn4() (i int, b bool) {
	return
}
```

> 显然，这是正确的。因为返回值 `i, b` 也是函数的局部变量，进入函数是，也会被初始化为零值


注意下面写法的错误：

```go
// 注意下面写法的错误
func fn5() (i int, err error) {
	if _, err := os.Open("o:/t"); err != nil {
		// return // 错误，因为err被重新定义，只能在if中使用，返回值的err就被覆盖了，就是上一行:=的问题
		
		// 正确的做法是指定返回值
		return -1, err // 正确
	}
	return
}
```

---

## 形式参数

形参可有可无，也可以有多个形参

> [!warning] Go 不支持形式参数的默认值

形参是局部变量，在函数调用时初始化

```go
func fn1()                   {} // 无形参

func fn2(int)                {} // 有一个int形参，但是没法用它，不推荐

func fn3(x int)              {} // 单参函数

func fn4(x int, y int)       {} // 多参函数

func fn5(x, y int, z string) {} // 相邻形参类型相同，可以写到一起

fn1()
fn2(5)
fn3(10)
fn4(4, 5)
fn5(7, 8, "ok")
```

### 可变参数

可变参数 `variadic`。其他语言也有类似的被称为 **剩余参数**，但 Go 有所不同。

```go
func fn6(nums ...int) { // 可变形参
	fmt.Printf("%T %[1]v, %d, %d\n", nums, len(nums), cap(nums))
}

fn6(1)       // []int, [1]
fn6(3, 5)    // []int, [3 5]
fn6(7, 8, 9) // []int, [7 8 9]
```

> [!important] 可变参 **数收集实参到一个切片** 中。如果一个函数声明有可变参数，那么这个可变参数 **必须位于形参列表的最后**
> + 例如， `func fn7(x, y int, nums ...int, z string){}` 这是错误的

```go
func fn7(x, y int, nums ...int) {
	fmt.Printf("%d %d; %T %[3]v, len(nums)=%d, cap(nums)=%d\n", x, y, nums, len(nums), cap(nums))
}
fn7(1, 2)       // 1 2; []int [],  len(nums)=0, cap(nums)=0
fn7(1, 2, 3)    // 1 2; []int [3], len(nums)=1, cap(nums)=1
fn7(1, 2, 3, 4) // 1 2; []int [3 4], len(nums)=2, cap(nums)=2
```

### 切片分解

也可以使用 **切片分解** 传递给可变参数，这个功能和其他语言的参数解构很像，但是不一样。

```go
func fn4(x int, y int)   {} // 多参函数

p := []int{4, 5}
fn4(p...) // 错误，这在Go中不行，不能用在非可变参数 non-variadic 上
```

```go
func fn7(x, y int, nums ...int) {
	fmt.Printf("%d %d; %T %[3]v, %d, %d\n", x, y, nums, len(nums), cap(nums))
}

p := []int{4, 5, 6}

fn7(p...) // 这在Go中不行，报奇怪的错，原因还是不能用在非可变参数上，就用 4、5 用在 x、y 上了
// 这个例子，本以为 p 被分解，4 和 5 分别对应 x 和 y，6 被可变参数 nums 收集，但是这在 Go 中是错误的
```

如果最终的实参是 **某类型的切片** 且其 **后跟着 `...`**，它将 **无变化** 的传递给 `...T` 的可变参数。注意，**这个过程无新的切片创建**

```go
func fn7(x, y int, nums ...int) {
	fmt.Printf("%d %d; %T %[3]v, %d, %d\n", x, y, nums, len(nums), cap(nums))
}
p := []int{4, 5}
fn7(p...)          // 错误，不能用在普通参数上
fn7(1, p...)       // 错误，不能用在普通参数上
fn7(1, 2, 3, p...) // 错误，不能用 2 种方式为可变参数传参，不能混用

// fn7(1, 2, p..., 9, 10) // 语法错误
// fn7(1, 2, []int{4, 5}..., []int{6, 7}...) // 语法错误，不能连续使用p...，只能一次

// 正确的如下
fn7(1, 2, []int{4, 5}...)
fn7(1, 2, p...)
fn7(1, 2, 3, 4, 5)
```

> [!important] 切片分解的限制
> + **切片分解只能用于给可变参数传参**，不能用在普通参数上
> + **不能用 $2$ 种方法为可变参数传参**
> + 不能连续使用切片分解，**只能一次**

---

## 作用域

函数会开辟一个 **局部作用域**，其中定义的标识符仅能在函数之中使用，也称为标识符在函数中的可见范围。

**这种对 _标识符约束的可见范围_，称为 _作用域_**

### 语句块作用域

Go 的 `if`、`for`、`switch` 等语句会开辟一个作用域，**作用域仅在该语句块中**，称为语句块作用域

```go
s := []int{1, 3, 5}

for i, v := range s {
	fmt.Println(i, v) // i 和  v在 for 块中可见
}

fmt.Println(i, v) // 错误，在for外不可见
```

```go
if f, err := os.Open("o:/t.txt"); err != nil {
	fmt.Println(f, err) // 可见
}

fmt.Println(f, err) // 错误，不可见
```

> [!tip] `swith`、`select` 语句中的 **每个子句都被视为一个隐式的代码块**。

### 显式的块作用域

在任何一个大括号中定义的标识符，其作用域只能在这对大括号中。

> [!important] 一对花括号(`{}`)就会创建一个作用域，称为块作用域

```go
{ 
	// 块作用域
	const a = 100
	var b = 200
	c := 300
	fmt.Println(a, b, c) // 可见
}

fmt.Println(a, b, c) // 错误，不可见
```

### 函数作用域

函数声明的时候使用了花括号，所以 **整个函数体就是一个显式代码块**。这个 **函数就是一个块作用域**

### 包级作用域(全局作用域)

**每一个 `package` 包含该包所有源文件，形成的作用域**。有时 **在 _包中顶层代码_ 定义标识符，也称为 _全局标识符_**。

**所有包内定义全局标识符，包内可见**。**_包外需要大写首字母导出_**，使用时也要加上包名。

```go
package intset

// 包内全局标识符
const (
	NULL = iota
	INT
	FLOAT
	...
)
```

> [!important] 在同一个包中，所有源文件形成了一个全局作用域

### 内建作用域

**预定义的标识符就在这个全局环境中**，因此像 `bool`、`int`、`nil`、`true`、`false`、`iota`、`append` 等标识符全局可见，随处可用
