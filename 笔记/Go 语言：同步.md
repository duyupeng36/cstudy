# 同步

## 监控 goroutines(`sync.WaitGroup`)

当 `man goroutine` 退出后，Go 程序将完全退出，为了让 `main goroutine` 等待子 `goroutine`，`sync.WaitGroup` 提供了一种让 `main goroutine` 等待子 `goroutine` 的方法

`WaitGroup` 等待 `goroutines` 集合完成。主 `goroutine` 调用 `Add` 来设置要等待的 `goroutines` 的数量。然后每个 `goroutines` 运行并在完成时调用 `Done`。同时，可以使用 `Wait` 来阻塞，直到所有 `goroutines` 完成

**`WaitGroup` 在第一次使用后不能被复制**。

用 Go 内存模型的术语来说，对 `Done` 的调用在它解除阻塞的任何等待调用返回之前 **“同步”**。

下面是一个示例：
```go
package main

import (
	"fmt"
	"sync"
)

var wg sync.WaitGroup

func hello(i int) {
	defer wg.Done() // goroutine结束就登记 -1
	fmt.Println("hello", i)
}
func main() {
	for i := 0; i < 10; i++ {
		wg.Add(1) // 启动一个 goroutine 就登记 +1
		go hello(i)
	}
	wg.Wait() // 等待所有登记的goroutine都结束
}
```

## 竞争条件

在有两个或更多 `goroutine` 的程序中，每一个 `goroutine` 内的语句也是按照既定的顺序去执行的，但是一般情况下我们没法去知道分别位于两个 `goroutine` 的事件 `x` 和 `y` 的执行顺序，`x`是在`y`之前还是之后还是同时发生是没法判断的。当我们没有办法自信地确认一个事件是在另一个事件的前面或者后面发生的话，就说明`x`和`y`这两个事件是 **并发** 的

一个函数在线性程序中可以正确地工作。如果在并发的情况下，这个函数依然可以正确地工作的话，那么我们就说这个函数是 **并发安全的**，并发安全的函数不需要额外的同步工作。我们可以把这个概念概括为一个特定类型的一些方法和操作函数，如果这个类型是并发安全的话，那么所有它的访问方法和操作就都是并发安全的。

**在一个程序中有非并发安全的类型的情况下，我们依然可以使这个程序并发安全**。确实，并发安全的类型是例外，而不是规则，所以只有当文档中明确地说明了其是并发安全的情况下，你才可以并发地去访问它。我们会避免并发访问大多数的类型，无论是 **将变量局限在单一的一个 `goroutine` 内** 还是用 **互斥条件** 维持更高级别的不变性都是为了这个目的

相反，**导出包级别的函数一般情况下都是并发安全的**。由于 `package` 级的变量没法被限制在单一的 `gorouine`，所以修改这些变量“必须”使用互斥条件

一个函数在并发调用时没法工作的原因太多了，比如 **死锁**(deadlock)、**活锁**(livelock)和 **饿死** (resource starvation)。我们没有空去讨论所有的问题，这里我们 **只聚焦在竞争条件上**

**_竞争条件_ 指的是程序在多个 `goroutine` 交叉执行操作时，没有给出正确的结果**。竞争条件是很恶劣的一种场景，因为这种问题会一直潜伏在你的程序里，然后在非常少见的时候蹦出来，或许只是会在很大的负载时才会发生，又或许是会在使用了某一个编译器、某一种平台或者某一种架构的时候才会出现。这些使得竞争条件带来的问题非常难以复现而且难以分析诊断

传统上经常用经济损失来为竞争条件做比喻，所以我们来看一个简单的银行账户程序

```go
// Package bank 实现了只有一个账户的银行
package bank

var balance int

func Deposit(amount int) {
	balance = balance + amount
}

func Balance() int {
	return balance
}
```

对于这个具体的程序而言，我们可以瞅一眼各种存款和查余额的顺序调用，都能给出正确的结果。也就是说，`Balance` 函数会给出之前的所有存入的额度之和。然而，当我们并发地而不是顺序地调用这些函数的话，`Balance` 就再也没办法保证结果正确了。考虑一下下面的两个 `goroutine`，其代表了一个银行联合账户的两笔交易：

```go
// Alice:
go func() {
    bank.Deposit(200)                // A1
    fmt.Println("=", bank.Balance()) // A2
}()

// Bob:
go bank.Deposit(100)                 // B
```

Alice 存了 `$200`，然后检查她的余额，同时 Bob 存了 `$100`。因为 `A1` 和 `A2` 是和 `B` 并发执行的，我们没法预测他们发生的先后顺序。直观地来看的话，我们会认为其执行顺序只有三种可能性：`“Alice先”`，`“Bob先”`以及`“Alice/Bob/Alice”`交错执行。下面的表格会展示经过每一步骤后 `balance` 变量的值。引号里的字符串表示余额单。

```go
Alice first Bob first Alice/Bob/Alice
0               0           0
A1 200          B     100   A1     200
A2 "=200"       A1    300   B      300
B   300         A2   "=300" A2    "=300"
```

所有情况下最终的余额都是 `$300`。唯一的变数是 `Alice` 的余额单是否包含了 `Bob` 交易，不过无论怎么着客户都不会在意

但是事实是上面的直觉推断是错误的。第四种可能的结果是事实存在的，这种情况下 Bob 的存款会在 Alice 存款操作中间，在余额被读到(`balance + amount`)之后，在余额被更新之前
(`balance = ...`)，这样会导致 Bob 的交易丢失。而这是因为 Alice 的存款操作 `A1` 实际上是两个操作的一个序列，读取然后写；可以称之为 `A1r` 和 `A1w`。下面是交叉时产生的问题

```go
Data race
0
A1r      0     ... = balance + amount
B      100
A1w    200     balance = ...
A2  "= 200"
```

在 `A1r` 之后，`balance + amount` 会被计算为 $200$，所以这是 `A1w` 会写入的值，并不受其它存款操作的干预。最终的余额是 `$200`。银行的账户上的资产比 Bob 实际的资产多了 `$100`。(因为丢失了 Bob 的存款操作，所以其实是说 Bob 的钱丢了)

> [!NOTE] 数据竞争
> 这个程序包含了一个特定的竞争条件，叫作 **数据竞争**。无论任何时候，只要有两个 `goroutine` 并发访问同一变量，且 **至少其中的一个是写操作** 的时候就会发生数据竞争

如果数据竞争的对象是一个比一个机器字(32位机器上一个字=4个字节)更大的类型时，事情就变得更麻烦了，比如 `interface`，`string` 或者 `slice` 类型都是如此。下面的代码会并发地更新两个不同长度的 `slice`：

```go
var x []int
go func() { x = make([]int, 10) }()
go func() { x = make([]int, 1000000) }()
x[999999] = 1 // NOTE: undefined behavior; memory corruption possible!
```

最后一个语句中的 `x` 的值是未定义的；其可能是`nil`，或者也可能是一个长度为 `10` 的`slice`，也可能是一个长度为 `1,000,000` 的 `slice`。但是回忆一下 `slice` 的三个组成部分：指针(pointer)、长度(length)和容量(capacity)
- 如果指针是从第一个 `make` 调用来，而长度从第二个 `make` 来，`x` 就变成了一个混合体，一个自称长度为 `1,000,000` 但实际上内部只有 `10` 个元素的 `slice`
- 这样导致的结果是存储 `999,999` 元素的位置会碰撞一个遥远的内存位置，这种情况下难以对值进行预测，而且 `debug` 也会变成噩梦
这种语义雷区被称为 **未定义行为**，对 C 程序员来说应该很熟悉；幸运的是在 Go 语言里造成的麻烦要比 C 里小得多

> [!ERROR] 数据竞争的定义
> 
>  数据竞争会在 **两个以上的 `goroutine` 并发访问相同的变量** 且 **至少其中一个为写操作** 时发生

一个数据竞争的演示

```go
package main

import (
	"fmt"
	"sync"
)

var (
	x int64

	wg sync.WaitGroup // 等待组
)

// add 对全局变量x执行50000次加1操作
func add() {
	for i := 0; i < 50000; i++ {
		x = x + 1
	}
	wg.Done()
}

func main() {
	wg.Add(2)

	go add()
	go add()

	wg.Wait()
	fmt.Println(x)
}
```

该程序的运行结果如下

```shell
➜  gocode go run run.go  # 丢失了一半的结果
50000
➜  gocode go run run.go
94991
➜  gocode go run run.go
100000
➜  gocode go run run.go
97772
```

在上面的示例代码片中，我们开启了两个 `goroutine` 分别执行 `add` 函数，这两个 `goroutine` 在访问和修改全局的 `x` 变量时就会存在数据竞争，某个 `goroutine` 中对全局变量 `x` 的修改可能会覆盖掉另一个 `goroutine` 中的操作，所以导致最后的结果与预期不符

> [!WARNING] 解决数据竞争的三种方法
> - 不要去写变量
> - 避免从多个 `goroutine` 访问变量
> - 允许很多 `goroutine` 去访问变量，但是在同一个时刻最多只有一个 `goroutine` 在访问
> 	- 这种方式被称为 **“互斥”**
> 	- 这是我们解决竞争条件的常规方法

## 锁

### `sync.Mutex` 互斥锁

我们可以用一个容量只有 `1` 的 `channel` 来保证最多只有一个 `goroutine` 在同一时刻访问一个共享变量。一个只能为 `1` 和 `0` 的信号量叫做 **二元信号量**

```go
package main

import (
	"fmt"
	"sync"
)

var (
	// 申请一个只有一个元素的通道用于同步两个 goroutines
	mutex chan struct{} = make(chan struct{}, 1)
	x     int64
	wg    sync.WaitGroup // 等待组
)

// add 对全局变量x执行50000次加1操作
func add() {
	for i := 0; i < 50000; i++ {
		<-mutex // 获取锁
		x = x + 1
		mutex <- struct{}{} // 释放锁
	}
	wg.Done()
}

func main() {
	wg.Add(2)

	go add()
	go add()
	// 这一步时必须的，否则将出现死锁
	mutex <- struct{}{}
	wg.Wait()
	fmt.Println(x)
}
```

这种 **互斥** 很实用，而且 **被 `sync` 包里的 `Mutex` 类型直接支持**。它的 **`Lock` 方法** 能够获取到 `token`(这里叫锁)，并且 **`Unlock` 方法** 会释放这个 `token`
```go
package main

import (
	"fmt"
	"sync"
)

var (
	mutex sync.Mutex
	x     int64
	wg    sync.WaitGroup // 等待组
)

// add 对全局变量x执行50000次加1操作
func add() {
	for i := 0; i < 50000; i++ {
		mutex.Lock() // 获取锁
		x = x + 1
		mutex.Unlock() // 释放锁
	}
	wg.Done()
}

func main() {
	wg.Add(2)

	go add()
	go add()
	wg.Wait()
	fmt.Println(x)
}
```
每次一个 `goroutine` 访问 `bank` 变量时(这里只有 `balance` 余额变量)，它都会调用 `mutex` 的 `Lock` 方法来获取一个互斥锁。如果其它的 `goroutine` 已经获得了这个锁的话，这个操作会被阻塞直到其它 `goroutine` 调用了 `Unlock` 使该锁变回可用状态。`mutex` 会保护共享变量

> [!NOTE] 编程惯例
> **惯例来说，被 `mutex` 所保护的变量是在 `mutex` 变量声明之后立刻声明的**。如果你的做法和惯例不符，确保在文档里对你的做法进行说明。

在 `Lock` 和 `Unlock` 之间的代码段中的内容 `goroutine` 可以随便读取或者修改，这个代码段叫做 **临界区**。`goroutine` 在结束后释放锁是必要的，无论以哪条路径通过函数都需要释放，即使是在错误路径中，也要记得释放

在银行程序例程中，可以使用 `Mutex` 包含共享变量
```go
import "sync"

var (
	mu      sync.Mutex // guards balance
	balance int
)

func Deposit(amount int) {
	mu.Lock()
	balance = balance + amount
	mu.Unlock()
}

func Balance() int {
	mu.Lock()
	b := balance
	mu.Unlock()
	return b
}
```

`bank` 程序例证了一种通用的并发模式。一系列的导出函数封装了一个或多个变量，那么访问这些变量唯一的方式就是通过这些函数来做(或者方法，对于一个对象的变量来说)

每一个函数在一开始就获取互斥锁并在最后释放锁，从而保证共享变量不会被并发访问。**这种函数、互斥锁和变量的编排叫作监控 `monitor`**(这种老式单词的 `monitor` 是受`"monitor goroutine"` 的术语启发而来的。两种用法都是一个代理人保证变量被顺序访问)

由于在存款和查询余额函数中的临界区代码这么短--只有一行，没有分支调用--在代码最后去调用 `Unlock` 就显得更为直截了当。在更复杂的临界区的应用中，尤其是必须要尽早处理错误并返回的情况下，就很难去(靠人)判断对 `Lock` 和 `Unlock` 的调用是在所有路径中都能够严格配对的了。Go 语言里的 `defer` 简直就是这种情况下的救星：**我们用 `defer` 来调用 `Unlock`，临界区会隐式地延伸到函数作用域的最后**，这样我们就从“总要记得在函数返回之后或者发生错误返回时要记得调用一次 `Unlock`”这种状态中获得了解放。Go会自动帮我们完成这些事情

```go
func Balance() int {
    mu.Lock()
    defer mu.Unlock()
    return balance
}
```

此外，**一个 `deferred Unlock` 即使在临界区发生 `panic` 时依然会执行，这对于用 `recover` 来恢复的程序来说是很重要的**。`defer` 调用只会比显式地调用 `Unlock` 成本高那么一点点，不过却在很大程度上保证了代码的整洁性。大多数情况下对于并发程序来说，代码的整洁性比过度的优化更重要。如果可能的话尽量使用 `defer` 来将临界区扩展到函数的结束。

考虑一下下面的 `Withdraw` 函数。成功的时候，它会正确地减掉余额并返回 `true`。但如果银行记录资金对交易来说不足，那么取款就会恢复余额，并返回  `false`
```go
func Withdraw(amount int) bool {
	Deposit(-amount)
	if Balance() < 0 {
		Deposit(amount)
		return false
	}
	return true
}
```

函数终于给出了正确的结果，但是还有一点讨厌的副作用。当过多的取款操作同时执行时， `balance` 可能会瞬时被减到 $0$ 以下。这可能会引起一个并发的取款被不合逻辑地拒绝。所以如果 Bob 尝试买一辆 `sports car` 时，Alice 可能就没办法为她的早咖啡付款了。这里的问题是取款不是一个原子操作：**它包含了三个步骤，每一步都需要去获取并释放互斥锁，但任何一次锁都不会锁上整个取款流程**。理想情况下，**取款应该只在整个操作中获得一次互斥锁**。下面这样的尝试是错误的：

```go
func Withdraw(amount int) bool {
	mu.Lock()  // 获取锁
	defer mu.Unlock()  // 延迟到函数返回时释放
	
	// 由于 Deposit() 会再次尝试获取锁，导致两次加锁从而导致死锁显现
	Deposit(-amount)
	if Balance() < 0 {
		Deposit(amount)
		return false
	}
	return true
}
```

**`Deposit` 会调用 `mu.Lock()` 第二次去获取互斥锁，但因为 `mutex` 已经锁上了，而无法被重入**(Go里没有重入锁)
- **也就是说没法对一个已经锁上的mutex来再次上锁--这会导致程序死锁**，没法继续执行下去，`Withdraw` 会永远阻塞下去。

一个通用的解决方案是将一个函数分离为多个函数，比如我们把 `Deposit` 分离成两个：一个不导出的函数 `deposit`，这个函数假设锁总是会被保持并去做实际的操作，另一个是导出的函数 `Deposit`，这个函数会调用 `deposit`，但在调用前会先去获取锁。同理我们可以将 `Withdraw` 也表示成这种形式：

```go
import "sync"

var (
	mu      sync.Mutex // guards balance
	balance int
)

// 导出的函数，该函数会调用 deposit，但在调用前会先去获取锁
func Deposit(amount int) {
	mu.Lock()
	defer mu.Unlock()
	deposit(amount)
}

// 这是一个不导出的函数，它假设锁总是会被保持并去做实际的操作
func deposit(amount int) { balance += amount }

func Balance() int {
	mu.Lock()
	defer mu.Unlock()
	return balance
}

func Withdraw(amount int) bool {
	mu.Lock()
	defer mu.Unlock()
	deposit(-amount)
	if Balance() < 0 {
		deposit(amount)
		return false
	}
	return true
}
```

**封装** 用限制一个程序中的意外交互的方式，可以使我们 **获得数据结构的不变性**。因为某种原因，封装还帮我们 **获得了并发的不变性**。当你使用 `mutex` 时，确保 `mutex` 和其保护的变量没有被导出(在 Go 里也就是小写，且不要被大写字母开头的函数访问啦)，无论这些变量是包级的变量还是一个 `struct` 的字段

###  `sync.RWMutex` 读写锁

在 $100$ 刀的存款消失时不做记录多少还是会让我们有一些恐慌，Bob 写了一个程序，每秒运行几百次来检查他的银行余额。他会在家，在工作中，甚至会在他的手机上来运行这个程序。银行注意到这些陡增的流量使得存款和取款有了延时，因为所有的余额查询请求是顺序执行的，这样会互斥地获得锁，并且会暂时阻止其它的 `goroutine` 运行

**由于 `Balance` 函数只需要读取变量的状态**，所以我们同时让 **多个 `Balance` 调用并发运行事实上是安全的**，只要在运行的时候没有存款或者取款操作就行。在这种场景下我们需要一种特殊类型的锁，其 **允许多个只读操作并行执行**，但 **写操作会完全互斥**。这种锁叫作 **“多读单写”锁** (multiple readers, single writer lock)，Go语言提供的这样的锁是 **`sync.RWMutex`**：

```go
var (
	mu      sync.RWMutex // guards balance
	balance int
)

func Balance() int {
	mu.RLock()
	defer mu.RUnlock()
	return balance
}
```

`Balance` 函数现在调用了 `RLock` 和 `RUnlock` 方法来获取和释放一个读取或者共享锁。 `Deposit` 函数没有变化，会调用 `mu.Lock` 和 `mu.Unlock` 方法来获取和释放一个写或互斥锁

在这次修改后，Bob 的余额查询请求就可以彼此并行地执行并且会很快地完成了。锁在更多的时间范围可用，并且存款请求也能够及时地被响应了

**`RLock` 只能在临界区共享变量没有任何写入操作时可用**。一般来说，我们不应该假设逻辑上的只读函数/方法也不会去更新某一些变量。比如一个方法功能是访问一个变量，但它也有可能会同时去给一个内部的计数器`+1`(可能是记录这个方法的访问次数啥的)，或者去更新缓存--使即时的调用能够更快。如果有疑惑的话，请使用互斥锁

`RWMutex` 只有当获得锁的大部分 goroutine 都是读操作，而锁在竞争条件下，也就是说，`goroutine` 们必须等待才能获取到锁的时候，`RWMutex` 才是最能带来好处的。`RWMutex` 需要更复杂的内部记录，所以会让它比一般的无竞争锁的 `mutex` 慢一些

## 内存同步

你可能比较纠结为什么 `Balance` 方法需要用到互斥条件，无论是基于 `channel` 还是基于互斥量。毕竟和存款不一样，它只由一个简单的操作组成，所以不会碰到其它 `goroutine` 在其执行"期间"执行其它的逻辑的风险。这里使用 `mutex` 有两方面考虑
- 第一 `Balance` 不会在其它操作比如 `Withdraw` “中间”执行
- 第二(更重要)的是"同步"不仅仅是一堆 `goroutine` 执行顺序的问题；同样也会涉及到内存的问题

在现代计算机中可能会有一堆处理器，每一个都会有其 **本地缓存**(local cache)。为了效率，**对内存的写入一般会在每一个处理器中缓冲，并在必要时一起 `flush` 到主存**。这种情况下这些数据可能会以与当初 `goroutine` 写入顺序不同的顺序被提交到主存。像 `channel` 通信或者互斥量操作这样的原语会使处理器将其聚集的写入 `flush` 并 `commit`，这样 `goroutine` 在某个时间点上的执行结果才能被其它处理器上运行的 `goroutine` 得到。

考虑一下下面代码片段的可能输出

```go
var x, y int
go func() {
	x = 1                   // A1
	fmt.Print("y:", y, " ") // A2
}()

go func() {
	y = 1                   // B1
	fmt.Print("x:", x, " ") // B2
}()
```

因为两个 `goroutine` 是并发执行，并且访问共享变量时也没有互斥，会有数据竞争，所以程序的运行结果没法预测的话也请不要惊讶。我们可能希望它能够打印出下面这四种结果中的一种，相当于几种不同的交错执行时的情况

```
y:0 x:1
x:0 y:1
x:1 y:1
y:1 x:1
```

第四行可以被解释为执行顺序 `A1,B1,A2,B2` 或者 `B1,A1,A2,B2` 的执行结果。 然而实际的运行时还是有些情况让我们有点惊讶

```
x:0 y:0
y:0 x:0
```

但是根据所使用的编译器，CPU，或者其它很多影响因子，这两种情况也是有可能发生的。那么这两种情况要怎么解释呢？

在一个独立的 `goroutine` 中，每一个语句的执行顺序是可以被保证的；也就是说 `goroutine` 是顺序连贯的。但是在不使用 `channel` 且不使用 `mutex` 这样的显式同步操作时，我们就没法保证事件在不同的 `goroutine` 中看到的执行顺序是一致的了。尽管 `goroutine A` 中一定需要观察到 `x=1` 执行成功之后才会去读取 `y`，但它没法确保自己观察得到 `goroutine B` 中对 `y` 的写入，所以 `A` 还可能会打印出 `y` 的一个旧版的值

尽管去理解并发的一种尝试是去将其运行理解为不同 `goroutine` 语句的交错执行，但看看上面的例子，这已经不是现代的编译器和 cpu 的工作方式了。因为赋值和打印指向不同的变量，编译器可能会断定两条语句的顺序不会影响执行结果，并且会交换两个语句的执行顺序。**如果两个 `goroutine` 在不同的 CPU 上执行，每一个核心有自己的缓存，这样一个 `goroutine` 的写入对于其它 `goroutine` 的 `Print`，在主存同步之前就是不可见的了**

所有并发的问题都可以用一致的、简单的既定的模式来规避。所以可能的话，**将变量限定在 `goroutine` 内部**；如果是 **多个 `goroutine` 都需要访问的变量，使用互斥条件来访问**

## `sync.Once` 初始化

**如果初始化成本比较大的话，那么将初始化延迟到需要的时候再去做就是一个比较好的选择**。如果在程序启动的时候就去做这类的初始化的话会增加程序的启动时间并且因为执行的时候可能也并不需要这些变量所以实际上有一些浪费

```go
var icons map[string]image.Image
```

这个版本的 `Icon` 用到了 **懒初始化**`(lazy initialization)`。

```go
func loadIcons() {
    icons = map[string]image.Image{
        "spades.png":   loadIcon("spades.png"),
        "hearts.png":   loadIcon("hearts.png"),
        "diamonds.png": loadIcon("diamonds.png"),
        "clubs.png":    loadIcon("clubs.png"),
    }
}
// NOTE: not concurrency-safe!
func Icon(name string) image.Image {
    if icons == nil {
        loadIcons() // one-time initialization
    }
    return icons[name]
}
```

如果一个变量只被一个单独的 `goroutine` 所访问的话，我们可以使用上面的这种模板，但这种模板在 `Icon` 被 **并发调用时并不安全**。就像前面银行的那个 `Deposit` (存款)函数一样，`Icon` 函数也是由多个步骤组成的：首先测试 `icons` 是否为空，然后 `load` 这些 `icons`，之后将 `icons` 更新为一个非空的值。直觉会告诉我们 **最差的情况是 `loadIcons` 函数被多次访问会带来数据竞争**。当第一个 `goroutine` 在忙着 `loading` 这些 `icons` 的时候，另一个 `goroutine` 进入了 `Icon` 函数，发现变量是 `nil`，然后也会调用 `loadIcons` 函数

**不过这种直觉是错误的**。(我们希望现在你从现在开始能够构建自己对并发的直觉，也就是说对并发的直觉总是不能被信任的！)。因为缺少显式的同步，**编译器和 CPU 是可以随意地去更改访问内存的指令顺序**，以任意方式，只要保证每一个 `goroutine` 自己的执行顺序一致。其中一种可能 `loadIcons` 的语句重排是下面这样。它会在填写 `icons` 变量的值之前先用一个空 `map` 来初始化 `icons` 变量

```go
func loadIcons() {
    icons = make(map[string]image.Image)
    icons["spades.png"] = loadIcon("spades.png")
    icons["hearts.png"] = loadIcon("hearts.png")
    icons["diamonds.png"] = loadIcon("diamonds.png")
    icons["clubs.png"] = loadIcon("clubs.png")
}
```

因此，一个 `goroutine` 在检查 `icons` 是非空时，也并不能就假设这个变量的初始化流程已经走完了(**可能只是塞了个空 `map`，里面的值还没填完，也就是说填值的语句都没执行完呢**)

最简单且正确的保证所有 `goroutine` 能够观察到 `loadIcons` 效果的方式，是用一个 `mutex` 来同步检查

```go
var mu sync.Mutex // guards icons
var icons map[string]image.Image

// Concurrency-safe.
func Icon(name string) image.Image {
    mu.Lock()
    defer mu.Unlock()
    if icons == nil {
        loadIcons()
    }
    return icons[name]
}
```

然而 **使用互斥访问 `icons` 的代价就是没有办法对该变量进行并发访问**，即使变量已经被初始化完毕且再也不会进行变动。这里我们可以引入一个允许多读的锁

```go
var mu sync.RWMutex // guards icons
var icons map[string]image.Image

// Concurrency-safe.
func Icon(name string) image.Image {
    mu.RLock()
    if icons != nil {
        icon := icons[name]
        mu.RUnlock()
        return icon
    }
    mu.RUnlock()
    // acquire an exclusive lock
    mu.Lock()
    if icons == nil { // NOTE: must recheck for nil
        loadIcons()
    }
    icon := icons[name]
    mu.Unlock()
    return icon
}
```

上面的代码有两个临界区。**`goroutine` 首先会获取一个写锁，查询 `map`，然后释放锁**。如果条目被找到了(一般情况下)，那么会直接返回。如果没有找到，那 `oroutine` 会获取一个写锁。不释放共享锁的话，也没有任何办法来将一个共享锁升级为一个互斥锁，所以我们必须重新检查 `icons` 变量是否为 `nil`，以防止在执行这一段代码的时候，`icons` 变量已经被其它 `gorouine` 初始化过了。

上面的模板使我们的程序能够更好的并发，但是有一点太复杂且容易出错。幸运的是，`sync` 包为我们提供了一个 **专门的方案来解决这种一次性初始化的问题：`sync.Once`**。概念上来讲，一次性的初始化需要一个互斥量 `mutex` 和一个 `boolean` 变量来记录初始化是不是已经完成了；互斥量用来保护 `boolean` 变量和客户端数据结构。`Do` 这个唯一的方法需要接收初始化函数作为其参数。让我们用 `sync.Once` 来简化前面的 `Icon` 函数吧
```go
var loadIconsOnce sync.Once
var icons map[string]image.Image
// Concurrency-safe.
func Icon(name string) image.Image {
    loadIconsOnce.Do(loadIcons)
    return icons[name]
}
```

每一次对 `Do(loadIcons)` 的调用都会锁定 `mutex`，并会检查 `boolean` 变量。在第一次调用时，变量的值是 `false`，`Do` 会调用 `loadIcons` 并会将 `boolean` 设置为 `true`。随后的调用什么都不会做，但是 `mutex` 同步会保证 `loadIcons` 对内存(这里其实就是指 `icons` 变量啦)产生的效果能够对所有 `goroutine` 可见。用这种方式来使用 `sync.Once` 的话，我们能够避免在变量被构建完成之前和其它 `goroutine` 共享该变量

## 示例：并发的非阻塞缓存

### 基于互斥锁实现

下面是我们要设计的 `cache` 的第一个“草稿”

```go
package cache

type Memo struct {
	f     Func
	cache map[string]result
}
type Func func(key string) (interface{}, error)

type result struct {
	value interface{}
	err   error
}

func New(f Func) *Memo {
	return &Memo{f: f, cache: make(map[string]result)}
}

// Get 非并发安全的
func (memo *Memo) Get(key string) (interface{}, error) {
	res, ok := memo.cache[key]
	if !ok {
		res.value, res.err = memo.f(key)
		memo.cache[key] = res
	}
	return res.value, res.err
}
```

`Memo` 实例会记录需要缓存的函数`f`(类型为`Func`)，以及缓存内容(里面是一个 `string` 到 `result` 映射的 `map`)。每一个 `result` 都是简单的函数返回的 **值对**：一个值和一个错误值

这个 `cache` 并不是并发安全的。有两个 `goroutine` 在没有同步干预的情况下可能更新了`cache map`

```go
func (memo *Memo) Get(key string) (interface{}, error) {
	res, ok := memo.cache[key]
	if !ok {
		res.value, res.err = memo.f(key)
		memo.cache[key] = res
	}
	return res.value, res.err
}
```

最简单的使 `cache` 并发安全的方式是使用基于监控的同步。只要给 `Memo` 加上一个 `mutex`，在 `Get` 的一开始获取互斥锁，`return` 的时候释放锁，就可以让 `cache` 的操作发生在临界区内了

```go
type Memo struct {
	f     Func
	mu    sync.Mutex
	cache map[string]result
}

func (memo *Memo) Get(key string) (interface{}, error) {
	memo.mu.Lock()
	defer memo.mu.Unlock()

	res, ok := memo.cache[key]
	if !ok {
		res.value, res.err = memo.f(key)
		memo.cache[key] = res
	}
	return res.value, res.err
}
```

不幸的是对于 `Memo` 的这一点改变使我们完全丧失了并发的性能优点。每次对 `f` 的调用期间都会持有锁，`Get` 将本来可以并行运行的 `I/O` 操作串行化了

我们可以通过两次加锁的方式解决该问题

```go
func (memo *Memo) Get(key string) (interface{}, error) {
	// 第一次加锁，让一个 goroutine 检测 key 是否被缓存 
	memo.mu.Lock()
	res, ok := memo.cache[key]
	memo.mu.Unlock()

	// 如果没有被缓存
	if !ok {
		// memo.f(key) 将会导致多个 goroutines 竞争的更新 res
		res.value, res.err = memo.f(key)
		// 第二次加锁，保证只有一个 goroutine 更新 map
		memo.mu.Lock()
		memo.cache[key] = res
		memo.mu.Unlock()
	}
	return res.value, res.err
}
```

多个 `goroutine` 一起查询 `cache`，发现没有值，然后一起调用 `f` 这个慢不拉叽的函数。在得到结果后，也都会去更新 `map`。其中一个获得的结果会覆盖掉另一个的结果。

**理想情况下是应该避免掉多余的工作的**。而这种“避免”工作一般被称为`duplicate suppression`(**重复抑制/避免**)。

在 `Memo` 每一个 `map` 元素都是指向一个 **条目的指针**。每一个条目包含对函数 `f` 调用结果的内容缓存。与之前不同的是这次 `entry` 还包含了一个叫 `ready` 的 Channel。在条目的结果被设置之后，这个 Channel 就会被关闭，以向其它 Goroutine 广播去读取该条目内的结果是安全的了

```go
package cache

import "sync"

type Memo struct {
	f     Func
	mu    sync.Mutex
	cache map[string]*entry
}
type Func func(key string) (interface{}, error)

type result struct {
	value interface{}
	err   error
}

type entry struct {
	res   result
	ready chan struct{}
}

func New(f Func) *Memo {
	return &Memo{f: f, cache: make(map[string]*entry)}
}

func (memo *Memo) Get(key string) (interface{}, error) {
	memo.mu.Lock() // 加锁
	e := memo.cache[key]
	// key 尚未被缓存
	if e == nil {
		// 这是对该 key 的第一个请求
		// 这个 goroutine 负责计算值和广播就绪状态。
		e := &entry{ready: make(chan struct{})} // 分配 entry 的空间
		memo.cache[key] = e                     // 更新 map
		memo.mu.Unlock()                        // 解锁，让其他的 goroutines 可以执行到后续的等待值完成的地方

		e.res.value, e.res.err = memo.f(key)
		e.ready <- struct{}{} // 通知其他 goroutines 数据已经准备完成

	} else {
		memo.mu.Unlock() // 解锁，让其他的 goroutines 可执行到此
		<-e.ready        // 等待 key 的缓存准备好
	}
	return e.res.value, e.res.err
}
```

### 使用 channels 实现

上面这样 `Memo` 的实现使用了一个互斥量来保护多个 `goroutine` 调用 `Get` 时的共享 `map` 变量。不妨把这种设计和前面提到的把 `map` 变量限制在一个单独的 `monitor goroutine` 的方案做一些对比，后者在调用 `Get` 时需要发消息

`Func`、`result` 和 `entry` 的声明和之前保持一致。然而 `Memo` 类型现在包含了一个叫做 `requests` 的 `channel`，`Get` 的调用方用这个 `channel` 来和 `monitor goroutine` 来通信。`requests channel` 中的元素类型是 `request`。`Get` 的调用方会把这个结构中的两组 `key` 都填充好，实际上用这两个变量来对函数进行缓存的。另一个叫 `response` 的 `channel` 会被拿来发送响应结果。这个 `channel` 只会传回一个单独的值

```go
package cache

type Func func(key string) (interface{}, error)

type result struct {
	value interface{}
	err   error
}

type entry struct {
	res   result
	ready chan struct{}
}

func (e *entry) call(f Func, key string) {
	// 执行 f 函数，获取 key 的值
	e.res.value, e.res.err = f(key)
	// 通知 memo 服务的其他 goroutines 数据已经准备完成
	// 可以向 请求的 goroutines 发送数据了
	close(e.ready)
}
func (e *entry) deliver(response chan<- result) {
	// 等待数据准备完成的通知
	<-e.ready
	// 发送数据
	response<-e.res
}

// request 请求结构体
type request struct {
	key      string  // 请求资源的 key
	response chan<- result // 响应通道
}

// Memo 缓存结构体
type Memo struct {
	requests chan request  // 请求通道
}

// server 缓存服务：用于处理其他 goroutines 的数据请求
func (memo *Memo) server(f Func) {
	cache := make(map[string]*entry) // 缓存由 cache 服务 goroutine 管理
	for req := range memo.requests {
		e := cache[req.key]
		// 缓存中尚未有 key 的数据
		if e == nil {
			// 为 entry 分配内存
			e = &entry{ready: make(chan struct{})}
			cache[req.key] = e // 更新map
			// 另起一个 goroutine 去获取 key 的值，让 server 可以服务其他的 请求
			go e.call(f, req.key)
		}
		// 另起一个 goroutine 向请求的 goroutines 返回数据
		go e.deliver(req.response)

	}
}

// New 创建一个缓存对象
func New(f Func) *Memo {
	memo := &Memo{requests: make(chan request)}
	go memo.server(f)  // 启动缓存服务
	return memo
}

func (memo *Memo) Get(key string) (interface{}, error) {
	response := make(chan result)
	memo.requests <- request{key: key, response: response} // 向 memeo 的 server 请求服务
	res := <-response                                      // 从响应中读取
	return res.value, res.err
}

func (memo *Memo) Close() {
	close(memo.requests) // 关闭 请求通道
}
```

## 原子操作

针对 **整数数据类型**（`int32`、`uint32`、`int64`、`uint64`）我们还可以 **使用原子操作来保证并发安全**，通常直接使用原子操作比使用锁操作效率更高。Go语言中原子操作由内置的标准库`sync/atomic`提供

| 方法                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       | 解释      |
| ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | ------- |
| `func LoadInt32(addr *int32) (val int32)` <br>`func LoadInt64(addr *int64) (val int64)` <br>`func LoadUint32(addr *uint32) (val uint32)` <br>`func LoadUint64(addr *uint64) (val uint64)` <br>`func LoadUintptr(addr *uintptr) (val uintptr)` <br>`func LoadPointer(addr *unsafe.Pointer) (val unsafe.Pointer)`                                                                                                                                                                          | 读取操作    |
| `func StoreInt32(addr *int32, val int32)` <br>`func StoreInt64(addr *int64, val int64)`<br>`func StoreUint32(addr *uint32, val uint32)`<br>`func StoreUint64(addr *uint64, val uint64)`<br>`func StoreUintptr(addr *uintptr, val uintptr)`<br>`func StorePointer(addr *unsafe.Pointer, val unsafe.Pointer)`                                                                                                                                                                              | 写入操作    |
| `func AddInt32(addr *int32, delta int32) (new int32)`<br>`func AddInt64(addr *int64, delta int64) (new int64)`<br>`func AddUint32(addr *uint32, delta uint32) (new uint32)`<br>`func AddUint64(addr *uint64, delta uint64) (new uint64)`<br>`func AddUintptr(addr *uintptr, delta uintptr) (new uintptr)`                                                                                                                                                                                | 修改操作    |
| `func SwapInt32(addr *int32, new int32) (old int32)` <br>`func SwapInt64(addr *int64, new int64) (old int64)`<br>`func SwapUint32(addr *uint32, new uint32) (old uint32)`<br>`func SwapUint64(addr *uint64, new uint64) (old uint64)`<br>`func SwapUintptr(addr *uintptr, new uintptr) (old uintptr)`<br>`func SwapPointer(addr *unsafe.Pointer, new unsafe.Pointer) (old unsafe.Pointer)`                                                                                               | 交换操作    |
| `func CompareAndSwapInt32(addr *int32, old, new int32) (swapped bool)`<br>`func CompareAndSwapInt64(addr *int64, old, new int64) (swapped bool)`<br>`func CompareAndSwapUint32(addr *uint32, old, new uint32) (swapped bool)`<br>`func CompareAndSwapUint64(addr *uint64, old, new uint64) (swapped bool)`<br>`func CompareAndSwapUintptr(addr *uintptr, old, new uintptr) (swapped bool)`<br>`func CompareAndSwapPointer(addr *unsafe.Pointer, old, new unsafe.Pointer) (swapped bool)` | 比较并交换操作 |

我们填写一个示例来比较下互斥锁和原子操作的性能

```go
package main

import (
	"fmt"
	"sync"
	"sync/atomic"
	"time"
)

type Counter interface {
	Inc()
	Load() int64
}

// 普通版
type CommonCounter struct {
	counter int64
}

func (c *CommonCounter) Inc() {
	c.counter += 1
}

func (c *CommonCounter) Load() int64 {
	return c.counter
}

// 互斥锁版
type MutexCounter struct {
	counter int64
	lock    sync.Mutex
}

func (m *MutexCounter) Inc() {
	m.lock.Lock()
	defer m.lock.Unlock()
	m.counter++
}

func (m *MutexCounter) Load() int64 {
	m.lock.Lock()
	defer m.lock.Unlock()
	return m.counter
}

// 原子操作版
type AtomicCounter struct {
	counter int64
}

func (a *AtomicCounter) Inc() {
	atomic.AddInt64(&a.counter, 1)
}

func (a *AtomicCounter) Load() int64 {
	return atomic.LoadInt64(&a.counter)
}

func test(c Counter) {
	var wg sync.WaitGroup
	start := time.Now()
	for i := 0; i < 10000000; i++ {
		wg.Add(1)
		go func() {
			c.Inc()
			wg.Done()
		}()
	}
	wg.Wait()
	end := time.Now()
	fmt.Println(c.Load(), end.Sub(start))
}

func main() {
	c1 := CommonCounter{} // 非并发安全
	test(&c1)             // 8900979 2.45315199s
	c2 := MutexCounter{}  // 使用互斥锁实现并发安全
	test(&c2)             // 10000000 5.079291076s
	c3 := AtomicCounter{} // 并发安全且比互斥锁效率更高
	test(&c3)             // 10000000 2.498915357s
}
```

`atomic`包提供了底层的原子级内存操作，对于同步算法的实现很有用。这些函数必须谨慎地保证正确使用。除了某些特殊的底层应用，使用通道或者 `sync` 包的函数/类型实现同步更好。
