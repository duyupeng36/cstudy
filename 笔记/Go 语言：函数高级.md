# 函数高级

## 递归函数

简单来说，**递归函数就是函数自己调用自己**。有两种实现方式，一种是 **直接在自己函数中调用自己**，一种是 **间接在自己函数中调用的其他函数中调用了自己**。

> [!important] 递归需要 **有边界条件**、**递归前进段**、**递归返回段**
> + **递归一定要有边界条件**，当边界条件不满足的时候，递归前进；当边界条件满足的时候，递归返回

例如，使用递归实现 斐波拉契数列

$$
fib(n) = \begin{cases} 1 & n=1, n=2 \\ fib(n-1) + fib(n-2) & n \ge 3 \end{cases}
$$

```go
func fib(n int) int {
	if n <= 0 {
		panic("n must be positive number")
	}
	if n <= 2 {
		return 1
	}
	return fib(n-1) + fib(n-2)	
}
```

> [!question] 这个实现存在性能问题，每次计算的结果在函数返回后就被丢弃了，当第二次计算相同的  时，又会再次执行递归

![[Pasted image 20250402220045.png]]


如上图，左分支的计算包含了右分支的只，我只需要记录左分支的结果即可加速这个算法

```go
func fib(n int, cache []int) int {
    if n <= 0 {
        panic("n must be positive number")
    }

    if cache == nil {
        cache = make([]int, n+1)
    }

    if n <= 2 {
        return 1
    }
    if cache[n] == 0 {
        cache[n] = fib(n-1, cache) + fib(n-2, cache)
    }
    return cache[n]
}
```

对于斐波拉契数列而言，可以很方便的写为循环版本

```go
func fib(n int) int {
    pre, next := 0, 1
    for n > 0 {
        pre, next = next, pre+next
        n--
    }
    return pre
}
```

当然，这很容易将循环版本的斐波拉契数列修改为递归版本，使用递归模拟循环次数

```go
func fib(n, pre, next int) int {
    if n < 3 {  // 中条件依赖于 pre 和 next 的初始值 
        return next
    }
    
    pre, next = next, pre+next
    return fib(n-1, pre, next)
}

result := fib(10, 1, 1)
```

计算过程如下图

![[Pasted image 20250402220453.png]]


> [!important] 递归限制
> + 递归一定要有退出条件，递归调用一定要执行到这个退出条件。没有退出条件的递归调用，就是无限调用
> + 递归调用的深度不宜过深
> + Go语言 **不可能让函数无限调用**，栈空间终会耗尽
> 	+ goroutine stack exceeds 1000000000-byte limit

> [!important] 递归效率问题
> 以上4个斐波那契数列实现，请问那个效率高？递归效率一定低吗？哪个版本好？
> + 版本1 效率极低，是因为递归有大量重复计算。
> + 版本2 采用了使用更多内存空间换取更快的执行时间的策略，使用一个缓存切片保存已经计算过的值
> + 版本3 采用了循环
> + 版本4 将函数调用层次代替循环层次，效率还不错，和循环版效率差不多。
> + 那么版本3 和版本4 谁好？
> 	+ 循环版好些，因为递归有深度限制，再一个函数调用开销较大

### 间接递归

```go
func foo() {
	bar()
}

func bar() {
	foo()  // foo 间接调用 bar 形成递归调用
}

foo()
```

间接递归调用，是函数通过别的函数调用了自己，这一样是递归。

只要是递归调用，不管是直接还是间接，都要注意边界返回问题。但是间接递归调用有时候是非常不明显，代码调用复杂时，很难发现出现了递归调用，这是非常危险的。

所有，**使用良好的代码规范来避免这种递归的发生**。

> [!important] 总结
> + 递归是一种很自然的表达，符合逻辑思维
> + 递归相对运行效率低，每一次调用函数都要开辟栈帧
> + 递归有深度限制，如果递归层次太深，函数连续压栈，栈内存就溢出了
> + 如果是有限次数的递归，可以使用递归调用，或者使用循环代替，循环代码稍微复杂一些，但是只要不是死循环，可以多次迭代直至算出结果
> + 绝大多数递归，都可以使用循环实现
> + 即使递归代码很简洁，但是 **能不用则不用递归**

## 高阶函数

高阶函数是 **接收函数作为参数** 或 **将函数作为返回值** 的函数。高阶函数是对其他函数进行操作的函数，可以将它们作为参数，也可以返回它们

### 函数类型的参数

既然函数可以赋值给变量，那么 **函数就可以作为参数传递给函数**。

```go
package main

import "fmt"

func add(x, y int) int {
	return x + y
}

func calc(x, y int, op func(int, int) int) int {
	return op(x, y)
}

func main() {
	ret2 := calc(10, 20, add) // 函数作为参数
	fmt.Println(ret2)         //30
}
```

### 函数作为返回值

函数也可以作为返回值

```go
// 函数作为返回值
func do(s string) (func(int, int) int, error) {
	switch s {
	case "+":
		return add, nil
	case "-":
		return sub, nil
	default:
		err := errors.New("无法识别的操作符")
		return nil, err
	}
}
```

## 匿名函数

匿名函数是在 **_没有任何命名标识符_** 的情况下声明的函数。匿名函数可以接受输入并返回输出，就像标准函数一样。

```go
package main

import "fmt"

var (
	area = func(l int, b int) int {
		return l * b
	}
)

func main() {
	fmt.Println(area(20, 30))
}
```

所以，**匿名函数的定义格式如下**

```go
func(参数)(返回值){
	// 函数体
}
```

**匿名函数因为没有函数名**，所以没办法像普通函数那样调用，所以匿名函数需要 **_保存到某个变量_** 或者作为 **_立即执行函数_**

```go
func main() {
	// 将匿名函数保存到变量
	add := func(x, y int) {
		fmt.Println(x + y)
	}
	add(10, 20) // 通过变量调用匿名函数

	//自执行函数：匿名函数定义完加()直接执行
	func(x, y int) {
		fmt.Println(x + y)
	}(10, 20)
}
```

**匿名函数主要作用是用作高阶函数中**，是 _传入的逻辑_，函数允许传入参数，就是把 _逻辑外置_。

例如，给定 `2` 个整数，请给定计算函数，得到结果

```go
package main

import "fmt"

func calc(a, b int, fn func(int, int) int) int {
	return fn(a, b)
}

func main() {
	fmt.Println(
		calc(4, 5, 
			func(a, b int) int { 
				return a + b 
			}
		)
	) // 加法
	
	fmt.Println(
		calc(4, 5, 
			func(a, b int) int { 
				return a * b 
			}
		)
	) // 乘法
}
```

但是 Go 没有 `lambda` 表达式，也没有类似 JavaScript 的箭头函数，匿名函数写起来还是较为繁琐，只能使用类型别名简化，但是并没有什么太大的作用

```go
package main
import "fmt"

type MyFunc = func(int, int) int

func calc(a, b int, fn MyFunc) int {
	return fn(a, b)
}

func main() {
	fmt.Println(calc(4, 5, func(a, b int) int { return a + b })) // 加法
	fmt.Println(calc(4, 5, func(a, b int) int { return a * b })) // 乘法
}
```

## 函数嵌套

```go
package main

import "fmt"

func outer() {
	c := 99
	
	// 内嵌函数
	var inner = func() {
		fmt.Println("1 inner", c)
	}
	
	inner()
	fmt.Println("2 outer", c)
}

func main() {
	outer()
}
```

可以看到 `outer` 中定义了另外一个函数 `inner`，并且调用了 `inner`。`outer` 是包级变量，`main` 可见，可以调用。而 `inner` 是 `outer` 中的局部变量，`outer` 中可见

> [!important] 函数内部只能定义匿名函数
> + 命名函数不能再函数内部定义

### 嵌套作用域

```go hl:8-9,12
package main

import "fmt"

func outer() {
	c := 99
	var inner = func() {
		c = 100
		fmt.Println("1 inner", c) // 100
	}
	inner()
	fmt.Println("2 outer", c) // 100
}

func main() {
	outer()
}
```

第 `9, 12` 行都输出 `100`；说明内外用的同一个 `c` 声明，用的同一个标识符，也就是 `c` 是 `outer` 的局部变量，而不是 `inner` 的局部变量

```go hl:8-11,14
package main

import "fmt"

func outer() {
	c := 99
	var inner = func() {
		c = 100  // 上层作用域的变量 c
		fmt.Println("1 inner", c) // 100
		c := 101  // 在 inner 中声明了一个新的变量，隐藏上层作用域的同名变量 c
		fmt.Println("3 inner", c) // 101
	}
	inner()
	fmt.Println("2 outer", c) // 100
}

func main() {
	outer()
}
```

第 `9, 14` 行都输出 `100`，第 `11` 行输出 `101`；输出结果说明第 `9, 14` 行是同一个 `c`，都是 `outer` 的 `c`；而第 `10` 行的 `c` 是 `inner` 的 `c`，因为这是定义，即在当前作用域中定义新的局部变量，而这个 **局部变量只能影响当前作用域，不能影响其外部作用域**，对外不可见

## 闭包

> [!important] 视频参考
> + [【Golang】是闭包啊...](https://www.bilibili.com/video/BV1ma4y1e7R5)

**闭包就是延伸了变量生命周期的函数**，包括 **函数（姑且叫 `f` 吧）主体中引用的 _非全局非局部变量_**。这些 **_变量_ 必须 _来自_  `f`  的 _外层函数_ 的局部作用域**

假如有个名为 `avg` 的函数，它的作用是计算不断增加的系列值的平均值。如何创建 `avg` 这样的函数呢？现在，实现一个基本的求累积平均的函数，它不是闭包，因为它引用的是全局变量

> [!example] 
> 
> 例如，计算整个历史中某个商品的平均收盘价。新价格每天都在增加，因此计算平均值时要考虑到目前为止的所有价格


```go
package main

import "fmt"

var series []float64

func average(x float64) float64 {
	series = append(series, x)
	var sum float64
	for _, v := range series {
		sum += v
	}
	return sum / float64(len(series))
}

func main() {
	fmt.Println(average(1.0)) // 1.0
	fmt.Println(average(2.0)) // 1.5
	fmt.Println(average(3.0)) // 2.0
	fmt.Println(average(4.0)) // 2.5
}
```

然而，由于 `series` 存在于全局作用域中，可能无意间向 `series` 中添加了一个值，导致 `average` 计算结果出现错误。

为了解决该问题，我们可以将这段代码封装在一个函数内部，并让其返回该函数。此时，就会形成闭包。由于 Go 在函数体内部只能定义匿名函数，因此我们有

```go
package main

import "fmt"

func makeAverage() func(float64) float64 {
	var series []float64
	average := func(x float64) float64 { // 内层函数
		series = append(series, x) // 使用了非局部非全局的变量
		var sum float64
		for _, v := range series {
			sum += v
		}
		return sum / float64(len(series))
	}
	return average
}

func main() {
	average := makeAverage()
	fmt.Println(average(1.0)) // 1.0
	fmt.Println(average(2.0)) // 1.5
	fmt.Println(average(3.0)) // 2.0
	fmt.Println(average(4.0)) // 2.5
}
```

**闭包是一个函数，它保留了定义函数时存在的自由变量的绑定**。如此一来，调用函数时，**虽然定义作用域不可用了，但是仍能使用那些绑定**

上述 `make_average` 函数的实现方式导致低效。我们可以通过 **统计当前计算的次数**和 **总的值**，而不需要记录每个值

```go
package main

import "fmt"

func makeAverage() func(float64) float64 {
	var count int64
	var total float64
	average := func(x float64) float64 { // 内层函数
		total += x  // 使用了非局部非全局变量
		count++ // 非局部非全局变量
		return total / float64(count)
	}
	return average
}

func main() {
	average := makeAverage()
	fmt.Println(average(1.0)) // 1.0
	fmt.Println(average(2.0)) // 1.5
	fmt.Println(average(3.0)) // 2.0
	fmt.Println(average(4.0)) // 2.5
}
```

自由变量：**未在本地作用域中定义的变量**。例如定义在内层函数外的外层函数的作用域中的变量。

> [!important] 闭包的概念
> 
> 闭包：就是一个概念，出现在嵌套函数中，指的是 **_内层函数引用到了外层函数的自由变量_**，就形成了闭包。很多语言都有这个概念，最熟悉就是JavaScript。**闭包是 _运行期动态的概念_**
> 
> + 函数有 **_嵌套_**，函数内定义了其它函数
> + 内部函数使用了 _外部函数的局部变量_
> + _内部函数被返回_（非必须）

```go hl:15
package main

import "fmt"

func outer() func() {
	c := 99
	fmt.Printf("outer %d %p\n", c, &c)
	var inner = func() {
		fmt.Printf("inner %d %p\n", c, &c)
	}
	return inner
}

func main() {
	var fn = outer()
	fn()
}
```

第 `15` 行调用 `outer` 函数并返回 `inner` 函数对象，并使用标识符 `fn` 记住了它。`outer` 函数执行完了，在其内部定义的局部变量应该释放，这个内部定义的 `inner` 函数也是局部的，但是并不能释放，因为 `fn` 要用它

在某个时刻，`fn` 函数调用时，需要用到 `c`，但是其内部没有定义 `c`，它是 `outer` 的局部变量，如果这个 `c` 早已随着 `outer` 的调用而释放，那么 `fn` 函数调用一定出现错误，所以，这个 `outer` 的 `c` 不能释放，但是 `outer` 已经调用完成了，怎么办？**闭包**，让 `inner` 函数记住自由变量 `c`（内存地址）

> [!hint] 由于 `inner` 中引用了 `outer` 中的局部变量 `c`，`c` 这个对象是可达的，一定不会被垃圾回收

## defer

Go 中的 `defer` 语句会将其后面跟随的语句进行 **延迟处理**。 

在某函数中使用 `defer` 语句，会使得 `defer` 后跟的语句进行延迟处理，当该函数 **即将返回** 时，或 **发生 `panic`** 时，`defer` 后语句开始执行

同一个函数可以有多个 `defer` 语句，依次加入调用栈中（`LIFO`），函数返回或 `panic` 时，从栈顶依次执行 `defer` 后语句。**执行的先后顺序和注册的顺序正好相反，也就是后注册的先执行**。

**`defer` 后的语句必须是一个函数或方法的 _调用_**。

```go
func main() {
	fmt.Println("start")
	defer fmt.Println(1)
	defer fmt.Println(2)
	defer fmt.Println(3)
	fmt.Println("end")
}
```

输出结构为

```
start
end
3
2
1
```

由于`defer`语句延迟调用的特性，所以`defer`语句能非常方便的处理资源释放问题。比如：资源清理、文件关闭、解锁及记录时间等

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

> [!tip] 因为 `defer` 注册时就把其后语句的延迟执行的函数的参数准备好了，也就是 **注册时计算形参值**

### `defer` 执行时机

在 Go 的函数中 `return` 语句在底层并不是原子操作，它分为给返回值赋值和 `RET` 指令两步。而 `defer` 语句执行的时机就在返回值赋值操作后，`RET` 指令执行前。具体如下图所示

![[Pasted image 20240410223231.png]]

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
		x++ // 2. 有修改了 x 的值，x 是返回值值
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

分析下面的代码。写成输出结果

```go
func calc(index string, a, b int) int {
	ret := a + b
	fmt.Println(index, a, b, ret)
	return ret
}

func main() {
	x := 1
	y := 2
	defer calc("AA", x, calc("A", x, y)) 
	// defer cacl("AA", 1, cacl("A", 1, 2)) => defer cacl("AA", 1, 3)
	x = 10
	defer calc("BB", x, calc("B", x, y))
	// defer cacl("BB", 10, cacl("B", 10, 1)) => defer cacl("BB", 10, 12)
	y = 20
}
```

- 由于 `calc("A",x,y)` 和 `calc("B",x,y)` 没有被 `defer` 修饰。所以按顺序最先调用。
- **`defer` 延迟调用，当要执行 `defer` 时，被 `defer` 修饰的函数的所有参数值都确定**
- 最后被 `defer` 修饰的函数最先调用
- 最后的调用输出结果为

```
A 1 2 3
B 10 2 12
BB 10 12 22
AA 1 3 4
```

再看下面的代码，猜猜结果是什么？

```go hl:9-11
package main

import "fmt"

func main() {
	count := 1
	fmt.Println("start")

	defer func() {
		fmt.Println(count)
	}() // defer func(fmt.Println(count)){}()

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
// 3
```

因为第 `9-11` 行注册时要确定实际参数，而这是个匿名无参函数，没法准备参数。延迟执行时，打印是才要用 `count`，其外部作用域有一个 `count`，目前是 `3`

那么下例结果又是什么？为什么？

```go hl:9-11
package main

import "fmt"

func main() {
	count := 1
	fmt.Println("start")

	defer func(count int) {
		fmt.Println(count)
	}(count) // defer func(count int) {} (count) => defer func(count int) {} (1)

	count++
	defer fmt.Println(count) // defer fmt.Println(2)

	count++
	defer fmt.Println(count) // defer fmt.Println(3)

	fmt.Println("end")
}
// Out
// start
// end
// 3
// 2
// 1
```

因为第 `9-11` 行注册时要确定实际参数，而这是个匿名有参函数，准备实参 `count`，目前就是 `1`。延迟执行时，打印时使用内层作用域中的形参 `count` ，目前是 `1`
