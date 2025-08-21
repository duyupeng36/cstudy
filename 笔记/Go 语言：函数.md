# 函数

函数简单来说就是 **一连串语句**，这些语句 **被组合在一起**，并被指定了一个 **名字**。虽然 “函数” 这个术语来自数学，但是 Go 语言的函数不完全等同数学函数

每个函数本质上是一个 **自带声明和语句的小程序**。可以利用函数把程序划分成小块，这样便于人们理解和修改程序。并且 **函数可以复用**：一个函数最初可能是某个程序的一部分，但可以将其用于其他程序

## 函数基础
### 定义

在 Go 语言中，定义函数需要使用 `func` 关键字。

```go
func  函数名(形参列表) (返回值类型列表) {
	函数体
}
```

如下定义了一个函数 `average()` 用于求两个 `float64` 类型数据的平均值

```go
func average(a float64, b float64) float64 {
	var sum float64
	sum = a + b
	return sum / 2
}
```

#### 形参列表

 形参列表需要说明每个形参的类型，并且使用逗号分隔每个形参。如果函数不需要参数，则形参列表为空

> [!hint] 
> 
> 请注意：在 Go 语言中，对于连续的多个相同类型的形参可以省略除最后一个形参之外的类型
> 

也就是说 `average()` 的函数可以定义为

```go
func average(a, b float64) float64 {
	var sum float64
	sum = a + b
	return sum / 2
}
```

#### 返回类型列表

通常，函数需要返回值作为函数执行的结果。返回类型决定了函数返回值的类型。

> [!important] 
> 
> 在 Go 语言中，返回类型是任意的没有任何限制。也就是说，函数可以返回 Go 语言中的所有类型的值
> 
> 请注意：函数不返回值时，需要省略返回类型
> 

```go
func foo() {
	fmt.Printf("无返回值")
}
```

#### 函数体

一系列变量的声明和语句共同构成了函数体。在函数内部声明的名字只能在函数内部使用，离开函数后就无法再访问这些名字

```go hl:2-4
func average(a, b float64) float64 {
	var sum float64
	sum = a + b
	return sum / 2
}
```

>[!example] 
>
>例如，在 `average()` 函数内部声明的变量 `sum`，这个变量只能在 `average()` 函数内部访问
>

### 调用

**函数调用** 由 **函数名** 和跟随其后的 **实际参数列表** 组成，其中 **实际参数列表用圆括号括起来**

```go
average(x, y); 
average(10, y);
average(10, 20);
```

> [!attention] 
> 
> 请注意：不要丢失圆括号，如果丢失圆括号，那么将无法调用函数
> 
> ```go
> average  // 这是错误的
> ```
> 

函数 `average()` 会返回一个值，该值可以存储在变量中，用于测试或者其他用途

```go
avg := average(x, y)

if avg > 10 {
	// 满足条件
}
```

如果不需要函数的返回值也可以 **直接丢弃**

```go
// 丢弃函数的返回值
average(10, 20);
```

在 [[作业48：Go 复合数据类型#计算 $10$ 万内的素数的个数]] 中介绍了判定素数的算法，现在我们将判断素数的代码抽取出来，封装为一个函数

```go hl:33
package main

import (
	"fmt"
	"math"
	"time"
)

func isPrime(number int, primes []int) bool {
	end := int(math.Sqrt(float64(number))) + 1
	for _, prime := range primes {
		if prime > end {
			return true
		}
		if number%prime == 0 {
			return false
		}
	}
	return true
}

func main() {
	var count int = 1

	primes := []int{2}

	start := time.Now()

	// 只检查奇数
	for n := 3; n < 100000; n += 2 {

		// 是素数，让 count 加 1
		if isPrime(n, primes) {
			count++
			primes = append(primes, n)
		}
	}

	duration := time.Since(start)

	fmt.Printf("耗时 %v 找到 count = %d 个素数\n", duration, count)
}
```

### 实际参数

这里我们有必要理解 **实际参数(argument)** 和 **形式参数(parameter)** 的差异

> [!tip] 
> 
> **形式参数(parameter)** 出现 **在函数定义中的变量声明**，是 _预先声明的变量_，其值在函数被调用时获得
> 
> **实际参数(argument)** 是出现 **在函数调用中的表达式**
> 

在 Go 语言中，**实际参数是值传递的**：调用函数时，计算出每个实际参数的值并且把它赋值给相应的形式参数

> [!tip]
> 
> **形式参数的值是实际参数的值的副本**。也就是说，函数内部对形式参数的改变并不会影响实际参数的值
> 
> 从效果上来说，每个形式参数的行为好像是把变量初始化成与之匹配的实际参数的值
> 

例如，有时候我们需要交换两个变量值，我们可以定义如下函数

```go title:swap.go
package main

import "fmt"

func swap(a, b int) {
	var temp int
	temp = a
	a = b
	b = temp
}

func main() {
	var a, b = 10, 20
	fmt.Printf("交换前: a=%d,b=%d\n", a, b)
	swap(a, b)
	fmt.Printf("交换后: a=%d,b=%d\n", a, b)
}
```


上述代码运行的结果为

```shell
$ go run swap.go
交换前: a=10,b=20
交换后: a=10,b=20
```

如下图所示，显示了函数调用的过程的 **栈帧变化**，当函数返回时，属于该函数的栈帧就立即被销毁

![[Pasted image 20240925220821.png]]

### 返回值

对于需要返回值的函数，必须使用 `return` 语句指定将要返回的值。`return` 语句有如下格式：

```go
return 表达式
```

在不需要返回值的函数中，`return` 语句也是可以出现的，但是后面不能跟随表达式

```go
return
```

我们之前的示例中定义的函数的返回值是匿名返回值，Go 语言允许我们将返回值命名。例如，下面定义的函数用于计算切片中所有元素的和以及所有元素的乘积

```go hl:1
func arraySumProduct(array []int) (sum, product int) {

	product = 1
	for _, v := range array {
		sum += v
		product *= v
	}

	return
}
```

> [!attention] 
> 
> 请注意：使用命名返回值时，需要使用括号将其包围起来，即是只有一个返回值
> 

当使用命名返回值时，`return` 语句后面可以不跟随任何表达式。此外，也可以跟随表达式

```go
return expr1, expr2 ...
```

这样，命名返回值就会被赋值为表达式 `expr` 的值

> [!attention] 命名返回值的作用域只在函数内部；函数被允许返回多个值
> 
> 请注意，命名返回值的作用域依旧只是在函数内部，离开函数后将无法访问返回值
> 
> 在 Go 语言中，**函数被允许返回多个值**，参考上述的示例，这里不做过多介绍

### 程序终止

在 Go 语言中，应用程序从 `main.main()` 函数开始执行，请注意 `main.main()` 函数没有返回值。当 `main.main()` 函数执行结束，就意味着应用程序已经执行完毕

> [!tip] 
> 
> 此外，我们可以调用 `os.Exit(code)` 函数用于 **在任何位置终止程序**
> 

## 程序结构

现在，我们需要讨论两种类型的变量：**局部变量** 和 **全局变量**；已经它们的 **存储期**

> [!tip] 变量的作用域与存储期限
> 
> **作用域**：作用域是 _编译时_ 概念。即，**可以引用该变量的那一部分程序文本**
> 
> **存储期**：存储期是 _运行时_ 概念，即能够确保 **变量的存储空间必定存在的那一部分时间**
> 

### 局部变量

在 Go 语言中，声明在函数或者方法内部的变量称为 **局部变量**，其作用域仅限在函数或方法内部

例如，如下函数用于计算一个整数的位模式中 $1$ 的个数

```go
package main

import (
	"fmt"
)

func digitNumber(number uint) int {
	var result int
	for number > 0 {
		if number&0x1 == 1 {
			result++
		}
		number >>= 1
	}
	return result
}

func main() {
	a := digitNumber(0xef) // 11101111
	fmt.Printf("%d\n", a)
}
```

默认情形下，局部变量具有 **块作用域** 和 **自动存储期限**

> [!tip] 
> 
> 所谓的块作用域就是从变量声明的点开始一直到所在函数体的末尾
> 
> 因为局部变量的作用域不能延伸到其所属函数之外，所以其他函数可以把同名变量用于别的用途
> 

```go
func digitNumber(number uint) int {
	var result int                 ----
	for number > 0 {                  |
		if number&0x1 == 1 {          |
			result++                  |
		}                             |-- result 的作用域
		number >>= 1                  |
	}                                 |
	return result                 ----|
}
```

> [!tip] 
> 
> 局部变量的存储空间是在包含该变量的 **函数被调用时“自动”分配的**，**函数返回时回收**，所以称这种变量具有自动存储期
> 
> 具有自动存储期的局部变量分配在进程虚拟内存中的 **栈区域**。函数调用时分配栈空间，函数返回时 **回收栈空间**，导致存储在栈中的变量不可用
> 

> [!attention] 
> 
> 请注意：在 Go 语言中，如果返回来局部变量的地址，那么变量的存储期限就会延长，这些变量会被 Go 编译器存储在虚拟内存中的堆区域
> 
> 换句话说，在 Go 语言中，只要返回来局部变量的地址，那么这个局部变量就会分配在堆空间中
> 

### 全局变量

声明在任何函数和方法之外的变量称为 **包全局变量**。这些包全局变量的作用域是整个包。也就是说，在同一个包内，可以访问任意 `.go` 文件中的所有包全局变量

>[!tip] 
>
> 包全局变量具有 **静态存储期限**，因为包全局变量存放在虚拟内存空间中的 **数据段** 中，它们在 **程序启动是会被初始化** 并且 **只会被初始化一次**。在程序的整个执行期间都会保留变量的值
>

### 程序块

在 Go 语言中，任何 `{}` 包围的代码称为一个 **程序块**

默认情况下，**声明在程序块中的变量的存储期是自动的**：进入程序块时为变量分配存储单元，退出程序块时收回分配的空间。**变量具有块作用域，也就是说，不能在程序块外引用**

### 作用域规则

Go 语言的 **作用域规则** 使得程序员和编译器可以确定程序中同名标识符的含义。Go 语言中的作用域规则如下

- 一个程序块就是一个作用域。当程序块嵌套时，会出现作用域嵌套的情况
- 内层作用域声明的标识符会覆盖上层作用域出现的同名标识符
- 当离开内层作用域时，原来的标识符恢复

下面的程序片段是作用域规则的一个示例

```go
package main

var i int // 包级作用域变量

func f(i int) { // i 是函数块作用域
    i = 1 // 访问并修改函数块作用域内的变量i
}

func g() {
    i := 2 // 函数块作用域
    if i > 0 {
        i := 3 // if 块作用域 (使用 := 声明新变量)
        _ = i  // 使用变量以避免未使用错误
    }
    i = 4 // 访问并修改函数块作用域中的变量
}

func h() {
    i = 5 // 访问并修改包级作用域的变量
}
```

## 函数进阶

### 递归函数

简单来说，**递归函数就是函数 _自己调用自己_**。有两种实现方式，一种是 **_直接_ 在自己函数中调用自己**，一种是 **_间接_ 在自己函数中调用的其他函数中调用了自己**。

> [!important] 递归需要 **有边界条件**、**递归前进段**、**递归返回段**
> + **递归一定要有边界条件**，当边界条件不满足的时候，递归前进；当边界条件满足的时候，递归返回
> 

#### 斐波拉契数列

例如，使用递归实现 **斐波拉契数列**

$$
fib(n) = \begin{cases} 0 & n=0 \\ 1 & n=1 \\ fib(n-1) + fib(n-2) & n \ge 2 \end{cases}
$$

```go
func fib(n int) int {
	if n <= 2 {
		return 1
	}
	return fib(n-1) + fib(n-2)
}
```

这个实现存在性能问题，**每次计算的 `fib(n)` 结果在函数返回后就被丢弃了**，当第二次计算 `fib(n)` 时，又会再次执行递归

![Pasted image 20250402220045|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755786002442-6b51f078893d476bad08bfe0a9786461.png)

如上图，**左分支的计算包含了右分支的值**，我只需要记录左分支的结果即可加速这个算法

```go
func fib(n int, results []int) int {
	// 记忆数组
	if results == nil {
		results = make([]int, n+1)
	}

	if n <= 2 {
		return 1
	}

	if results[n] == 0 {
		results[n] = fib(n-1, results) + fib(n-2, results)
	}
	return results[n]
}
```

根据  **斐波拉契数列** 的递归定义，可以采用 **动态规划** 的思想进行顺序计算，编写出下面的代码

```go
func fib(n int) int {
	a, b := 0, 1
	for i := 0; i < n; i++ {
		a, b = b, a+b
	}
	return a
}
```

 **递归也可以用于模拟循环**，因此 `fib()` 函数还可以写成如下形式

```go
package main

import (
	"fmt"
)

func fib(n, pre, next int) int {
	// 这个条件依赖于 pre 和 next 提供的初始值
	if n < 2 {
		return next
	}
	return fib(n-1, next, pre+next)
}


func main() {
	result := fib(10, 0, 1)
	fmt.Println(result)
}

```

计算过程如下图

![Pasted image 20250402220453|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755786002442-5920fded44bb41c7b30b1420e6f27ab9.png)

#### 矩阵幂运算计算斐波那契数列

利用线性代数的知识，斐波拉契数列可以使用矩阵该表示

$$
\begin{cases}
F_{n+1} &= F_{n} + F_{n-1} \\
F_{n} &= F_{n}
\end{cases} 
$$

上述线性方程组可以使用矩阵表示为

$$
\begin{pmatrix}
F_{n+1}\\ F_{n}
\end{pmatrix} = 
\begin{pmatrix}
1 & 1 \\ 1 & 0
\end{pmatrix} \begin{pmatrix}
F_{n}\\ F_{n-1}
\end{pmatrix}
$$

因此我们有

$$
\begin{pmatrix}
F_{n+1}\\ F_{n}
\end{pmatrix} = 
\begin{pmatrix}
1 & 1 \\ 1 & 0
\end{pmatrix} ^{n} \begin{pmatrix}
F_{1}\\ F_{0}
\end{pmatrix} = \begin{pmatrix}
1 & 1 \\ 1 & 0
\end{pmatrix} ^{n} \begin{pmatrix}
1\\ 0
\end{pmatrix}
$$

```go
package main

import "fmt"

// 定义 2x2 矩阵
type Matrix [2][2]int

// 矩阵乘法
func multiply(a, b Matrix) Matrix {
	return Matrix{
		{a[0][0]*b[0][0] + a[0][1]*b[1][0], a[0][0]*b[0][1] + a[0][1]*b[1][1]},
		{a[1][0]*b[0][0] + a[1][1]*b[1][0], a[1][0]*b[0][1] + a[1][1]*b[1][1]},
	}
}

// 矩阵快速幂
func matrixPower(mat Matrix, power int) Matrix {
	result := Matrix{{1, 0}, {0, 1}} // 单位矩阵
	for power > 0 {
		if power%2 == 1 {
			result = multiply(result, mat)
		}
		mat = multiply(mat, mat)
		power /= 2
	}
	return result
}

// 使用矩阵方法计算斐波那契数
func fib(n int) int {
	if n == 0 {
		return 0
	}
	if n == 1 {
		return 1
	}

	// 基础矩阵
	base := Matrix{{1, 1}, {1, 0}}
	// 计算矩阵的 (n-1) 次幂
	power := matrixPower(base, n-1)
	// 结果在矩阵的 [0][0] 位置
	return power[0][0]
}

func main() {
	// 测试
	for i := 0; i <= 10; i++ {
		fmt.Printf("fib(%d) = %d\n", i, fib(i))
	}
	
	// 大数测试
	fmt.Printf("fib(50) = %d\n", fib(50))
	fmt.Printf("fib(90) = %d\n", fib(90))
}
```

> [!tip] 
> 
> 这里，我们应用了 **快速幂** 算法，其基本数学原理如下
> + 当指数是偶数时： $a^n=(a^2)^{n/2}$
> + 当指数是奇数时： $a^n=a*a^{n-1}$
> 

基本的快速幂算法如下

```go
func pow(a, n int) int {
    result := 1
    for n > 0 {
        if n&1 == 1 {  // 如果n是奇数
            result *= a
        }
        a *= a  // 平方
        n /= 2  // 减半
    }
    return result
}
```

#### 汉诺塔

汉诺塔是一个经典的递归问题，如下图所示

![Pasted image 20240926135946|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755701810528-5b74fe69444a47d2ab22db1829b9d849.png)

将 A 柱上的 $n$ 个盘片移动到 C 柱上，在移动过程中需要遵循以下规则：

1. 一次只能移动一个盘子
2. 任何时候较大的盘子不能放在较小的盘子上
3. 可以使用一个辅助柱子(B 柱)

问移动 $n$ 个盘片需要多少步？怎么移动？我们将盘片从上到下编号为 $1,2,3\cdots,n$

> [!tip] $n=1$ 时
> 
> 此时，只有 $1$ 个盘片，只需要 $1$ 步即可移动到 C 柱上

> [!tip] $n=2$ 时
> 
> 此时，有 $2$ 个盘片。首先将 $1$ 号盘片移动到 B 柱上，再将 $2$ 号盘片移动到 C 柱上，最后将 $1$ 号盘片移动到 C 柱上
> 

> [!tip] $n=3$ 时
> 
> 此时，有 $3$ 个盘片。首先要将前 $2$ 个盘片移动到 B 柱上，再将 $3$ 号盘片移动到 C 柱上，最后将前 $2$ 个盘片移动到 C 柱上
> 

如果需要移动 $n$ 个盘片，就需要将前 $n-1$ 个盘片移动到 B 柱上，再将 $n$ 号盘片移动到 C 柱上，最后将 B 柱上的 $n-1$ 个盘片移动到 C 柱上。

> [!hint] 
> 
> 显然，这是一个递归过程
> 

移动 $n$ 个盘片需要的步骤的递推公式为

$$
F(n) = 2F(n-1) + 1 = 2^{n}-1
$$

下面的视频展示了汉诺塔的移动过程

<iframe width="560" height="315" src="https://www.youtube-nocookie.com/embed/KlY-A8gk5sA" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

```go
package main

import "fmt"

func hanoi(n int, source, target, auxiliary byte) {
	if n == 1 {
		fmt.Printf("Move disk 1 from %c to %c\n", source, target)
		return
	}

	// Move n-1 disks from source to auxiliary peg
	hanoi(n-1, source, auxiliary, target)
	
	// Move the nth disk from source to target
	fmt.Printf("Move disk %d from %c to %c\n", n, source, target)
	
	// Move the n-1 disks from auxiliary to target
	hanoi(n-1, auxiliary, target, source)
}

func main() {
	var n int
	fmt.Print("Enter number of disks: ")
	fmt.Scan(&n)
	
	hanoi(n, 'A', 'C', 'B')
}
```

### 函数类型

在 Go 语言中，**函数是一等公民**。所谓的一等公民，就是函数具有变量的性质。换句话说，**函数可以赋值给函数类型的变量**

例如，如下定义的函数

```go
func average(a, b float64) float64 {
	return a + b
}
```

其类型为 `func(float64, float64) float64`

> [!tip] 
> 
> 也就是说，函数类型与参数和返回值相关
> 

知道了函数类型，我们就可以声明函数类型的变量，并将函数赋值给该变量。后续就可以像使用函数一样使用函数变量

```go
package main

import "fmt"

func average(a, b float64) float64 {
	return a + b
}

func main() {
	avg := average
	c := avg(10, 20)
	fmt.Printf("%f\n", c)
}
```

### 高阶函数

如果一个函数需要另一个函数作为参数，或者返回一个函数，那么该函数称为高阶函数

例如，在标准包 `sort` 中，就一个函数 `sort.Slice()` 用于给任何切片进行排序

```go
func Slice(x any, less func(i, j int) bool)
```

该函数需要一个类型 `func(int, int) bool` 的函数，根据函数的返回值确定切片 `x` 的排序规则

```go
package main

import (
	"fmt"
	"sort"
)

var s = []int{7, 9, 2, 1, 0, 8, 6, 5, 4}

func less(i, j int) bool {
	return s[i] < s[j]
}

func main() {

	fmt.Printf("%#v\n", s)
	sort.Slice(s, less)
	fmt.Printf("%#v\n", s)
}
```

### 匿名函数

**匿名函数就是没有名字的函数**，直接使用 `func()` 的形式定义。继续上述例子中使用 `sort.Slice()` 函数，我们可以使用匿名函数作为其第二个参数

```go hl:12-14
package main

import (
	"fmt"
	"sort"
)

func main() {

	var s = []int{7, 9, 2, 1, 0, 8, 6, 5, 4}
	fmt.Printf("%#v\n", s)
	sort.Slice(s, func(i, j int) bool {
		return s[i] < s[j]
	})
	fmt.Printf("%#v\n", s)
}
```

### 函数嵌套

在 Go 语言中，**函数内部不能定义具名函数**；但是，可以定义匿名函数。这样就会存在函数嵌套的行为。

```go
package main

import (
	"fmt"
)

func outer() {
	c := 99
	var inner = func() {
		fmt.Printf("inner: c = %d\n", c)
	}
	inner()
	fmt.Printf("outer: c = %d\n", c)
}

func main() {

	outer()
}
```

> [!important] 
> 
> 请注意：`inner` 只是一个函数类型的变量，并不是函数
> 

#### 嵌套作用域

函数内部定义的匿名函数会形成另一个内嵌的局部作用域。同时适用于前面介绍的作用域规则

> [!tip] 
> 
> 也就是，在内层函数中，可以引用外层函数的变量，前提是内层函数并未声明与外层函数中同名的变量
> 

下面的示例代码中，演示了内层函数如何引用外层函数的变量的

```go
func outer() {
	c := 99
	var inner = func() {
		fmt.Printf("inner: c = %d\n", c) // c 的值是 100
	}
	c = 100
	inner()
	fmt.Printf("outer: c = %d\n", c) // c 的值是 99
}
```

如果在内层函数中声明了同名的变量，那么内层函数将就近引用变量

```go
func outer() {
	c := 99
	var inner = func() {
		var c = 100
		fmt.Printf("inner: c = %d\n", c) // c 的值是 100
	}
	inner()
	fmt.Printf("outer: c = %d\n", c) // c 的值是 99
}
```

### 闭包

在 [[Python：函数基础#闭包]] 中介绍了闭包的概念。所谓的闭包只是一种 **词法绑定** 技术，通常使用结构体将 **函数** 和 **函数的关联环境** 绑定在一起

> [!tip] 函数的关联环境
> 
> 函数的环境里是 **符号和值的对应关系**，它既要包括 **约束变量**（该函数内部绑定的符号），也要包括 **自由变量**（在函数外部定义但在函数内被引用），有些函数也可能没有自由变量
> 

实现闭包的程序设计语言都必须允许函数嵌套，如果函数 `outer` 内定义了函数 `inner`，那么如果函数 `inner` 引用了自由变量，且这些自由变量没有被编译器优化掉，那么将产生闭包。

> [!tip] 
> 
> 换句话说，产生闭包的必要条件就是：**函数嵌套** 并且 内层函数引用外层函数局部作用域中声明的变量
> 

如下示例，`outer` 中定义的变量 `non`，对于内层函数 `inner` 而言是非局部非全局变量

```go
package main

import "fmt"

// 返回一个函数
func add() func(int) int {
	var x int
	return func(y int) int {
		x += y
		return x
	}
}

func main() {
	var f = add()
	fmt.Println(f(10))
	fmt.Println(f(20))
	fmt.Println(f(30))
	fmt.Println("-----------")
	f1 := add()
	fmt.Println(f1(40))
	fmt.Println(f1(50))
}
```

### defer

Go 中的 `defer` 语句会将其后面跟随的语句进行 **延迟处理**。 在某函数中使用 `defer` 语句，会使得 `defer` 后跟的语句延迟到该函数 **即将返回** 时，或 **发生 panic** 时，`defer` 后语句开始执行

同一个函数可以有多个 `defer` 语句，依次加入调用栈中（`LIFO`），函数返回或 `panic` 时，从栈顶依次执行 `defer` 后语句。**执行的先后顺序和注册的顺序正好相反，也就是后注册的先执行**。

> [!attention] 
> 
> 请注意：`defer` 后的语句必须是一个函数或方法的调用语句
> 

下面演示了 `defer` 语句的使用案例

```go
func main() {
	fmt.Println("start")
	defer fmt.Println(1)
	defer fmt.Println(2)
	defer fmt.Println(3)
	fmt.Println("end")
}
```

执行上述代码片段的输出结果为

```
start
end
3
2
1
```


> [!tip] 
> 
> 由于`defer`语句延迟调用的特性，所以`defer`语句能非常方便的处理资源释放问题。比如：**资源清理**、**文件关闭**、**解锁** 及 **记录时间** 等
> 

再看下面的例子，猜猜结果是什么？

```go
package main

import "fmt"

func main() {
	count := 1
	fmt.Println("start")

	defer fmt.Println(count) // defer fmt.Println(1)

	count++
	defer fmt.Println(count) // defer fmt.Println(2)

	count++
	defer fmt.Println(count) // defer fmt.Println(3)

	fmt.Println("end")
}

// Out:
// start
// end
// 3
// 2
// 1
```

> [!tip] 
> 
> 因为 `defer` 注册时就把其后语句的延迟执行的函数的参数准备好了，也就是 **注册时计算形参值**
> 

#### defer 的执行时机

在 Go 的函数中 `return` 语句在底层并不是原子操作，它分为给 **返回值赋值** 和 执行 `RET` 指令两步。而 `defer` 语句执行的时机就在返回值赋值操作后，`RET` 指令执行前。具体如下图所示

![Pasted image 20240410223231|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755786864554-11679bb424854f53a5ae9cef517c95c8.png)

下面的程序演示了 `defer` 语句的执行时机：**通过注释进行解释**

```go
package main

import "fmt"

func f1() int {
	x := 5
	
	defer func() {
		x++  // 2. 修改了 f1 中的局部变量 x 的值，不会影响返回值
	}()
	
	return x // 1. 已经设置好了返回值为 5
}

func f2() (x int) {
	defer func() {
		x++ // 2. 有修改了 x 的值，x 是命名返回值
	}()
	return 5 // 1. 设置返回值 x 为 5
}

func f3() (y int) {
	x := 5
	defer func() {
		x++ // 2. 修改的是 x 与 返回值 y 无关
	}()
	return x // 1. 设置返回值 y 为 5
}

func f4() (x int) {
	defer func(x int) {
		x++ // 2. 修改的是局部变量，不会影响 返回值
	}(x)
	return 5 // 1. 设置返回值 x 为 5
}

func main() {
	fmt.Println(f1()) // 返回值：5
	fmt.Println(f2()) // 返回值：6
	fmt.Println(f3()) // 返回值：5
	fmt.Println(f4()) // 返回值：5
}
```

## 函数执行原理

> [!quote] 
> 
> 本节内容来自 bilibili 幼麟实验室视频
> 
> + https://www.bilibili.com/video/BV1WZ4y1p7JT/
> + https://www.bilibili.com/video/BV1tZ4y1p7Rv/
> 
> 

### 栈帧布局

对于像 Go 这种编译型语言，我们定义的函数会被编译器编译为 **机器指令** 并写入可执行文件中。当程序运行时，可执行文件会被加载到内存中，位于进程虚拟内存中的 **程序代码段**。如下图所示，展示了 Linux-x86 下的进程虚拟内存布局

![[Pasted image 20241013130418.png]]

> [!tip] 
> 
> 机器指令相关内容参考
> 
> + [[X86-64汇编：访问信息]]
> + [[X86-64汇编：控制]]
> + [[X86-64汇编：过程]]
> 

如果在一个函数中调用另一个函数，编译器就会生成对应的函数调用指令 `call`。当执行 `call` 指令时，就会跳转到被调用函数的入口地址开始执行。被调用函数最后会有一条 `ret` 指令，用于返回到被调用函数的调用位置继续执行

函数执行时，需要有足够的内存空间供它存放 **局部变量**、**被调用函数的参数**、**被调用函数的返回值** 等，这段空间通常被分配在进程虚拟地址空间中的 **栈**。

> [!tip] 
> 
> 关于栈的可以参考 [[数据结构：栈和队列#栈]]
> 

运行时，栈上面是高地址，向下增长。分配给函数的栈空间被称为 **函数栈帧**。栈底通常称为 **栈基(base pointer, bp)**；栈顶又被称为 **栈指针(stack pointer, sp)**。在 Go 语言中，函数栈帧布局如下图

![Pasted image 20250618193938|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755786864555-0e6d3155f18e467ebe90581be34fd378.png)

> [!tip] 
> 函数栈帧从上到下分别是 
> + **调用者栈基**
> + **局部变量**
> + **被调函数的返回值**
> + **被动掉函数的参数**
> 

在 [[X86-64汇编：过程#转移控制]] 中，我们介绍了 `call` 指令。它只做两件事情：**将下一条指令地址入栈**，这个入栈的地址称为 **函数的返回地址**；然后，**跳转到被调用函数的入口开始执行**。在返回地址后面，就是被调用函数的栈帧了；所有的函数栈帧都采用相同的栈帧布局，因此被调用函数可以通过栈指针加上偏移的方式定位参数和返回值

![Pasted image 20250618194506|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755786864555-e23352747f2d4b45abfff84b384201c1.png)

### 分配栈帧

CPU 在执行指令时，需要使用特定的寄存器存储运行时 **栈基** 和 **栈指针**。在 [[X86-64汇编：过程]] 中存储栈基的寄存器为 `%rbp`；存储栈指针的寄存器为 `%rsp`

> [!attention] 
> 
> 请注意：不同的指令集的 CPU 使用的寄存器名字不同
> 

同时，编译器还需要指定下一条要执行指针的地址，CPU 使用 **指令指针寄存器(程序计数器)** 存储该地址。在 [[X86-64汇编：过程]] 中指令指针寄存器为 `%rip`

在  Go 语言中，分配栈栈帧时，会直接将栈指针移动到所需最大栈空间的位置，然后通过 **栈指针+偏移量** 这种相对寻址的方式使用函数栈帧，如下图所示

![Pasted image 20250618200755|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755786864557-a9aec63caa9d44938ec0cba7588aee6d.png)

### 无参数无返回值函数的调用
#### call 指令

下面我们来看 `call` 指令时如何实现函数跳转的。如下图，展示了执行 `call` 指令之前的栈帧布局和寄存器状态

![Pasted image 20250618202112|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755786864558-5fa4c3eb37944c709c49748dc2ae8c93.png)


此时指令指针寄存器 `%rip` 指向了 `call b1` 指令，这就是下一条要执行的指针。`call` 指令需要执行两个动作：**将下一条指令地址(a2)入栈**；然后，将指令指针寄存器 `%rip` 设置为函数的入口地址 `b1`。当 `call` 指令执行完成之后，栈帧和寄存器的状态如下

![Pasted image 20250618202755|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755786864558-3ee845b49be64721b6da68f584db20ea.png)

下面，函数 `b()` 开始执行。首先，会为自己分配足够大的栈帧，假设分配 $24$ 字节，对应上图，就是将 `%rsp` 移动到 `s7` 处；然后，会把调用者栈基保存在自己的栈帧中，即把 `%rbp` 的值保存在 `%rsp + 16` 的位置；最后，使用 `%rsp + 16` 的更新 `%rbp` 寄存器。最终，栈帧和寄存器的状态如下图

![Pasted image 20250618203907|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755786864559-304a0c26db0548e9933e7eb071f56e57.png)

接下来就是执行函数 `b()` 的剩余指令，直到遇到 `ret` 指令。

#### ret 指令

在执行 `ret` 指令之前，编译器还会插入额外的两条指令。第一条指令是 **恢复调用者栈基**，在上图描述中，调用栈基地址保存在 `%rsp + 16` 地址处；第二条指令是 **释放自己的栈帧空间**，分配时向下移动多少字节，释放时也要相应的向上移动多少字节。这两条指令执行完毕后栈帧和寄存器状态如下图

![Pasted image 20250618210229|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755786864561-1cec14875357427584510f42ed5ca19f.png)

然后，就需要执行 `ret` 指令，也要做两件事情：首先，弹出 `call` 压栈的返回地址；然后，将 `%rip` 设置为这个返回地址。`ret` 指令执行完后的栈帧和寄存器状态如下

![Pasted image 20250618210646|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755786864563-62fffac7b79c40828fe4fc827cb9f9b8.png)

现在，函数 `a()` 就可以从 `a2` 这条指令继续执行了

### 有参数

之前的内容我们介绍了无参数无返回值的函数调用过程了。下面我们来看有参数的情况。继续在 [[Go 语言：函数#实际参数]] 中定义的 `swap()` 函数

```go hl:6
package main

import "fmt"

func swap(a, b int) {
	a,b = b, a
}

func main() {
	var a, b = 10, 20
	fmt.Printf("交换前: a=%d,b=%d\n", a, b)  // 交换前: a=10,b=20
	swap(a, b)
	fmt.Printf("交换后: a=%d,b=%d\n", a, b)  // 交换后: a=10,b=20
}
```

我们在执行上述代码时，发现数据交换失败了。下面我们通过函数的调用栈来看看发生了什么事。`swap()` 函数获得参数时的栈帧布局如下

![Pasted image 20250618215709|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755786864565-25ce17184d3143ef8ae1d9f0c3a17192.png)

> [!warning] 
> 
> 注意，参数的入栈顺序与声明顺序是相反的
> 

执行交换变量的指令交换的仅仅是参数的值，而不是调用者的局部变量。也就是说，当执行完 `swap()` 函数中第 `6` 行时，函数的栈帧状态如下

![Pasted image 20250618215947|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755786864565-99c9ed0527a645aa90abbc161fd00f21.png)

上图展示的内容显示，我们发现 `swap()` 交换的是其参数值，而非调用者的局部变量。

下面我们修改 `swap()` 函数，让其接受两个执行调用者局部变量的指针

```go
package main

import "fmt"

func swap(pa, pb *int) {
	*pa, *pb = *pb, *pa
}

func main() {
	var a, b = 10, 20
	fmt.Printf("交换前: a=%d,b=%d\n", a, b) // 交换前: a=10,b=20
	swap(&a, &b)
	fmt.Printf("交换后: a=%d,b=%d\n", a, b) // 交换后: a=20,b=10
}
```

执行上述代码，发现这次交换时成功的。下面我们来看该示例的函数栈帧

![Pasted image 20250618223824|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755786864566-b389437db9a24727a25235dbc2d8ebab.png)

显然，`swap()`  通过指针交换的是 `main()` 函数局部空间中的变量

### 返回值

在 [[X86-64汇编：过程#传递数据]]  中我们知道，函数的返回值是通过寄存器 `%rax` 进行返回的。然而，在 Go 语言中，函数可以返回多个值。因此，在栈上分配返回值空间更合适

#### 匿名返回值

我们首先来看匿名返回值的例子

```go
package main

import "fmt"

func incr(a int) int {
	var b int
	defer func() {
		a++
		b++
	}()
	a++
	b = a
	return b
}

func main() {
	var a, b = 10, 0
	fmt.Printf("调用 incr 前: a=%d,b=%d\n", a, b) // 调用 incr 前: a=10,b=0
	b = incr(a)
	fmt.Printf("调用 incr 后: a=%d,b=%d\n", a, b) // 调用 incr 后: a=10,b=11
}
```

在上述例子中，`main()` 函数调用 `incr()`，然后将 `incr()` 的返回值赋值给局部变量。在 `main()` 函数中执行到 `incr()` 函数的调用时，此时的函数栈帧如下图

![Pasted image 20250618225041|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755786864566-e91853534f8d464db4abb42e722fef6f.png)

函数 `incr()` 继续执行到第 $11$ 行 `a++` 时，将参数 `a` 的值自增 $1$，此时的函数栈帧如下

![Pasted image 20250618225245|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755786864567-b65cf6997e2a40b6a449789b27fa11bf.png)

函数 `incr()` 继续执行第 $12$ 行 `b = a` 时，将参数 `a` 的值赋值给 `incr()` 的局部变量 `b`，此时函数的栈帧如下图

![Pasted image 20250618225424|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755786864567-96b630f42b65429bbf96e268397f8df5.png)

函数 `incr()` 继续执行到 $13$ 行 `return b` 时，我们需要明白一个关键问题。我们说过，函数最后由编译器插入的指令负责恢复调用者栈基和释放栈帧。不过，在此之前，需要给返回值赋值并且执行 `defer` 函数

> [!important] 
> 
> 请注意：给返回值赋值要先于 `defer` 函数执行。即，先给返回值赋值，然后执行 `defer` 函数
> 

所以，在执行 `return b` 这条语句时，先将 `incr()` 的局部变量 `b` 的值拷贝到返回值空间，此时的函数栈帧如下图

![Pasted image 20250618230014|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755786864567-3495efe333d543ce8c9f2572cc0a213d.png)

现在，继续执行 `defer` 函数，第 $8$ 行 `a++` 会导致 `incr()` 的参数自增 $1$，第 $9$ 行 `b++` 会导致 `incr()` 的局部变量 `b` 自增 $1$。此时，函数的栈帧如下图

![Pasted image 20250618230249|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755786864568-8146f540856b48e68bf9da330cd39d2f.png)

到此，`incr()` 函数的代码执行完毕，此时返回值为 $11$，赋值给 `main()` 函数中的局部变量 `b`。此时，函数栈帧如下图

![Pasted image 20250618230410|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755786864568-fc4dd1a4222c4035b10ff10d5611eca0.png)

#### 命名返回值

现在，我们来看命名返回值的情况，代码相比较于匿名返回值的唯一变化就是将局部变量 `b` 改成了命名返回值 `b`

```go
package main

import "fmt"

func incr(a int) (b int) {
	defer func() {
		a++
		b++
	}()
	a++
	return a
}

func main() {
	var a, b = 10, 0
	fmt.Printf("调用 incr 前: a=%d,b=%d\n", a, b) // 调用 incr 前: a=10,b=0
	b = incr(a)
	fmt.Printf("调用 incr 后: a=%d,b=%d\n", a, b) // 调用 incr 后: a=10,b=12
}
```

此时，输出的结果就与匿名返回值的情况不同了。下面我们通过函数栈帧状态变化分析其中原因。在开始执行 `incr()` 函数的栈帧如下图，`main()` 函数的栈帧完全一样，区别在于 `incr()` 函数没有了局部变量

![Pasted image 20250618230824|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755786864569-58632e75e1a441acbd81480655cac9ab.png)

函数 `incr()` 执行到第 $10$ 行 `a++` 时，将参数 `a` 自增 $1$，此时函数的栈帧如下

![Pasted image 20250618231002|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755786864570-62dd99693c4142f186bea22224d7a4f6.png)

函数 `incr()` 执行第 $11$ 行 `return a`，首先将 `incr()` 的参数 `a` 的值赋值给返回值，此时函数的栈帧如下图

![Pasted image 20250618231110|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755786864570-ffc6de531a5447c7bd8972b0e122f2a3.png)

然后，继续 `defer` 函数，第 $7$ 行 `a++` 继续将 `incr()` 的参数 `a` 自增 $1$，第 $8$ 行 `b++` 将 `incr()` 的返回值自增 $1$。此时，函数栈帧的状态如下

![Pasted image 20250618231250|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755786864571-9a9b0e89591c418d9f754a252d2616e6.png)

## 闭包的实现

> [!quote] 
> 本节内容来着 bilibili 幼麟实验室视频
> + https://www.bilibili.com/video/BV1ma4y1e7R5/
> 


### Function Value

在之前的内容中我们知道了函数在 Go 语言中是头等对象，它可以作为函数的参数与返回值，以及赋值给一个变量。Go 语言称这样的参数、返回值或变量为 **Function Value**

函数在编译时被编译器翻译为了机器指令，而 Function Value 本质上是一个指针，指向一个 `runtime.funcval` 结构体，该结构体中包含一个指针成员，指向了函数的第一条指令，即函数的入口

```go
type funcval struct {
	fn uintptr
}
```

下图展示了 Function Value 的结构

![Pasted image 20250619223705|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755786864572-d2cc3c572d9a4354a5a1eb08acae2d6c.png)

下图展示了一个完整示例

![Pasted image 20250619223901|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755786864573-bc20392f143a4c8b973056594d0e8aa8.png)

函数 `A` 被赋值给了 `f1` 和 `f2` 两个变量，这种情况编译器会进行优化，让 `f1` 和 `f2` 共用一个 `funcval` 结构体。如上图，`funcval` 类型的变量被分配在了只读数据段。`f1` 和 `f2` 保存了在只读数据段中的 `funcval` 类型变量的地址。通过 `f1` 和 `f2` 调用函数时，就会通过 `f1` 和 `f2` 存储的地址找到 `funcval` 类型的变量，然后通过 `funcval` 类型的变量找到函数的入口地址，从而调用函数

### 闭包

既然有函数的入口地址就能调用函数，那么为什么要通过 `funcval` 结构体包装函数的入口地址呢？这主要是为了实现闭包。在 [[Go 语言：函数#函数进阶#闭包]] 中我们就介绍了闭包的概念，其中有两个关键点

> [!tip] 
> 
> 第一：要存在 **在函数外部定义但在函数内部引用** 的自由变量
> 
> 第二：即便脱离变量定义的上下文，它照样可以运行
> 

如下示例代码

```go
func create() func() int {
	c := 2
	return func() int {
		return c
	}
}

func main() {
	f1 := create()
	f2 := create()
	fmt.Println(f1())
	fmt.Println(f2())
}
```

函数 `create()` 的返回值是一个函数，但是这个被返回的函数内部引用了外部定义的变量 `c`，即是函数 `create()` 执行结束，通过 `f1()` 和 `f2()` 依然可以调用这个闭包函数，并且使用定义在 `create()` 函数中的变量 `c`。

> [!tip] 
> 
> 通常，变量 `c` 被成为 **捕获变量**
> 

**闭包函数的指令也是在编译时生成**。但是，因为每个闭包对象都要保存自己的捕获变量，所以要到 **运行时才会创建对应的闭包对象**

函数 `main()` 执行第 $9$ 行 `f1:=create()` 时，`create()` 函数会在堆空间上分配一个 `funcval` 结构体，其中 `fn` 存储了闭包函数的入口地址，随后跟随着闭包的捕获变量。当 `main()` 函数的第 $9$ 行执行完毕时，栈帧状态如下图 

![Pasted image 20250619232021|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755786864573-adb0917de1074463b1b0592dea60f337.png)

下面继续执行 `main()` 函数的第 $10$  行 `f2:=create()` 时，`create()` 函数重新在堆上分配另一个 `funcval` 结构体并返回该结构体的地址。当 `main()` 函数的第 $10$ 行执行完毕时，栈帧状态如下图

![Pasted image 20250619232518|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755786864574-a978f09573b24ca6af6053fec5f722fe.png)

后续就可以通过 Function Value `f1` 和 `f2` 调用闭包函数时，会找到各自的 `funcval` 结构体，拿到同一个函数入口。但是，它们会使用各自的捕获列表

> [!tip] 
> 
> 调用闭包函数时会使用各自对应的捕获列表，这就是成闭包为有状态的函数的原因
> 

那么闭包函数是如何找到对应的捕获列表呢？在 Go 语言中，通过 Function Value 调用函数时，
会把对应的 `funcval` 结构体地址存入一个特定的寄存器，在 x86-64 架构中会使用 `%rdx` 寄存器。这样，就可以通过 `%rdx` 寄存器取出 `funcval` 结构体的地址，然后加上相应的偏移，找到每一个被捕获的变量

> [!tip] 
> 
> 因此，在 Go 语言中，闭包就是有捕获列表的 `funcval` 结构体
> 

### 捕获列表

请注意，闭包的捕获变量不仅仅只是拷贝变量值。因为，**被捕获的变量要在外层函数中与闭包函数中表现一致**，好像它们在使用同一个变量。Go 语言的编译器会根据不同的情况自行处理

在上述例子中，被捕获变量除了初始化赋值以外没有任何修改，那么只需要拷贝变量值到捕获列表即可。但是，被捕获变量可能可能会有额外的修改情形，这样就会在做细分。来看下面这个例子

```go
package main

import "fmt"

func create() (fs [2]func()) {
	var i = 0
	for ; i < 2; i++ {
		fs[i] = func() {
			fmt.Println(i)
		}
	}
	return
}

func main() {
	fs := create()
	for i := 0; i < len(fs); i++ {
		fs[i]()
	}
}
```

该示例中，闭包的捕获变量是局部变量，除了初始化赋值外，还有修改。下面我们通过栈帧的变化来看看这种情况是如何处理的

![Pasted image 20250619235913|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755786864575-fa704c8e0b184f149bdd6f29871ccd3a.png)

> [!tip] 
> 
> 其他情况的处理方式比较复杂，就不在讨论。无论怎样处理，都是需要保证闭包函数与外层函数使用同一个对象
> 
