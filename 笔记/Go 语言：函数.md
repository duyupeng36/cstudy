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

![[Pasted image 20240926104051.png]]

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

![[Pasted image 20241004004337.png]]

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
> + 当指数是偶数时：$a^n=(a^2)^{n/2}$
> + 当指数是奇数时：$a^n=a*a^{n-1}$
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

### 函数类型








