# Goroutines 与 Channels

## Goroutines

在 Go 语言中，并发执行的单元叫作一个 Goroutine。Goroutine 是对协程的改进，让协程可以自由移动到不同的线程中执行。

> [!tip] 
> 
> 所谓的协程就是在合适的时候可以自动让出 CPU 控制流的函数
> 

设想这里的一个程序有两个函数，一个函数做计算，另一个输出结果，假设两个函数没有相互之间的调用关系。一个线性的程序会先调用其中的一个函数，然后再调用另一个。如果程序中包含多个 Goroutines，对两个函数的调用则可能发生在同一时刻。马上就会看到这样的一个程序。

当一个 Go 程序启动时，其主函数会在一个单独的 Goroutine 中运行，我们叫它 `main Goroutine`。新的 Goroutine 会用 `go` 语句来创建。在语法上，`go` 语句是一个普通的函数或方法调用前加上关键
字 `go`。`go` 语句会使其语句中的函数在一个新创建的 Goroutine 中运行。而 go 语句本身会迅速地
完成。

```go
// 同步调用：等待函数 f() 的返回
f()    // call f(); wait for it to return

// 在一个新的 Goroutine 中调用 f(), 并且不等待
go f() // create a new goroutine that calls f(); don't wait
```

下面的示例就是我们在 [[Python：进程]] 中使用的示例：为计算斐波拉契数列的第 $n$ 项添加一个动画

```go
package main

import (
	"fmt"
	"time"
)

func fib(n int) int {
	if n <= 2 {
		return 1
	}
	return fib(n-1) + fib(n-2)
}

func spinner(delay time.Duration, prefix string) {
	for {
		for _, r := range `-\|/` {
			fmt.Printf("\r%s%c", prefix, r)
			time.Sleep(delay)
		}
	}
}

func main() {
	n := 50
	start := time.Now()
	go spinner(300*time.Millisecond, fmt.Sprintf("fib(%d)=", n))
	res := fib(n)
	fmt.Printf("\rfib(%d)=%d\n", n, res)
	fmt.Printf("Execution time: %v\n", time.Since(start))
}
```

执行上述代码后，会首先显示动画，之后就会输出结果，然后 `main()` 函数返回

![[spinner-go.gif]]

当 `main()` 函数返回时，所有的 Goroutines 都会被直接打断，程序退出。除了从主函数退出或者直接终止程序之外，没有其它的编程方法能够让一个 Goroutine 来打断另一个的执行，但是之后可以看到一种方式来实现这个目的，通过 Goroutine 之间的通信来让一个 Goroutine 请求其它的 Goroutine，并让被请求的 Goroutine 自行结束执行。

> [!attention] 
> 
> 请注意：`fib()` 和 `spinner()` 两个函数在同时执行
> 

### 示例: 并发的 Clock 服务

网络编程是并发大显身手的一个领域，由于服务器是最典型的需要同时处理很多连接的程
序，这些连接一般来自于彼此独立的客户端。下面我们会简单介绍 Go 语言的 `net` 包，这个
包提供编写一个网络客户端或者服务器程序的基本组件，无论两者间通信是使用 TCP，UDP
或者 Unix domain sockets

下的例子是一个顺序执行的 **时钟服务器**，它会每隔一秒钟将当前时间写到客户。这里我们采用 TCP SOCKET

```go
package main

import (
	"io"
	"net"
	"time"

	"github.com/rs/zerolog/log"
)

func handleConnection(conn net.Conn) {
	defer func() {
		if err := conn.Close(); err != nil {
			log.Error().Msgf("Failed to close connection: %v", err)
		} else {
			log.Info().Msgf("Connection closed: %s", conn.RemoteAddr())
		}
	}()
	log.Info().Msgf("Handling connection from %s", conn.RemoteAddr())

	for {
		_, err := io.WriteString(conn, time.Now().Format("2006-01-02 15:04:05 MST\n"))
		if err != nil {
			log.Error().Msgf("Failed to write to connection: %v", err)
			return
		}
		time.Sleep(time.Second)
	}
}

func main() {

	listener, err := net.Listen("tcp", ":8080")
	if err != nil {
		log.Error().Msgf("Failed to start server: %v", err)
	}
	defer listener.Close()

	for {
		conn, err := listener.Accept()
		if err != nil {
			log.Error().Msgf("Failed to accept connection: %v", err)
			continue
		}
		log.Info().Msgf("Accepted connection from %s", conn.RemoteAddr())
		handleConnection(conn)
	}
}
```

`net.Listen()` 函数创建了一个实现了 `net.Listener` 接口的对象，这个对象会监听一个网络端口上到来的连接，在这个例子里我们用的是 TCP 的 `localhost:8080` 端口。`listener` 对象的 `listener.Accept()` 方法会直接阻塞，直到一个新的连接被创建，然后会返回一个 `net.Conn` 对象来表示这个连接

`handleConnection()` 函数会处理一个完整的客户端连接。在一个 `for` 死循环中，用 `time.Now()` 获取当前时刻，然后写到客户端。由于 `net.Conn` 实现了 `io.Writer` 接口，我们可以直接向其写入内容。这个死循环会一直执行，直到写入失败。最可能的原因是客户端主动断开连接。这种情况下 `handleConnection`函数会用 `defer` 调用关闭服务器侧的连接，然后返回到主函数，继续等待下一个连接请求。

下图展示了两个客户端连接 `clock` 服务器时的行为。由于 `handleConnectione()` 执行一个无限循环，只要在写入客户端连接失败时才会返回，也就是说该服务器只能同时处理一个客户端的连接。


![[clock-go.gif]]

我们这里对服务端程序做一点小改动，使其支持并发：在 `handleConnectione()` 函数调用的地方增加 `go` 关键字，让每一次 `handleConnectione()` 的调用都进入一个独立的 Goroutine。

```go hl:46
package main

import (
	"io"
	"net"
	"time"

	"github.com/rs/zerolog/log"
)

func handleConnection(conn net.Conn) {
	defer func() {
		if err := conn.Close(); err != nil {
			log.Error().Msgf("Failed to close connection: %v", err)
		} else {
			log.Info().Msgf("Connection closed: %s", conn.RemoteAddr())
		}
	}()
	log.Info().Msgf("Handling connection from %s", conn.RemoteAddr())

	for {
		_, err := io.WriteString(conn, time.Now().Format("2006-01-02 15:04:05 MST\n"))
		if err != nil {
			log.Error().Msgf("Failed to write to connection: %v", err)
			return
		}
		time.Sleep(time.Second)
	}
}

func main() {

	listener, err := net.Listen("tcp", ":8080")
	if err != nil {
		log.Error().Msgf("Failed to start server: %v", err)
	}
	defer listener.Close()

	for {
		conn, err := listener.Accept()
		if err != nil {
			log.Error().Msgf("Failed to accept connection: %v", err)
			continue
		}
		log.Info().Msgf("Accepted connection from %s", conn.RemoteAddr())
		go handleConnection(conn)
	}
}
```

现在多个客户端可以同时接收到时间了

![[clock-go-concurrency.gif]]

### 示例：并发的 Echo 服务

`clock` 服务器每一个连接都会起一个 `goroutine`。下面我们会创建一个 `Echo` 服务器，这个服务在每个连接中会有多个 Goroutine。大多数 `echo` 服务仅仅会返回他们读取到的内容，就像下面这个简单的 `handleConn` 函数所做的一样

```go
func handleConn(c net.Conn) {
    io.Copy(c, c) // NOTE: ignoring errors
    c.Close()
}
```

一个更有意思的 `Echo` 服务应该模拟一个实际的 `Echo` 的“回响”，并且一开始要用大写 `"HELLO"` 来表示“声音很大”，之后经过一小段延迟返回一个有所缓和的 `"Hello"`，然后一个全小写字母的 `"hello"` 表示声音渐渐变小直至消失，像下面这个版本的 `handleConn`

```go
func echo(c net.Conn, shout string, delay time.Duration) {
    fmt.Fprintln(c, "\t", strings.ToUpper(shout))
    time.Sleep(delay)
    fmt.Fprintln(c, "\t", shout)
    time.Sleep(delay)
    fmt.Fprintln(c, "\t", strings.ToLower(shout))
}

func handleConn(c net.Conn) {
    input := bufio.NewScanner(c)
    for input.Scan() {
        echo(c, input.Text(), 1*time.Second)
    }
    // NOTE: ignoring potential errors from input.Err()
    c.Close()
}
```

我们需要编写一个客户端，这样它就可以发送终端的输入到服务器，并把服务端的返回输出到终端上，这使我们有了使用并发的另一个好机会

```go
package main

import (
	"io"
	"log"
	"net"
	"os"
)

func main() {
	conn, err := net.Dial("tcp", "localhost:8000")
	if err != nil {
		log.Fatal(err)
	}
	defer conn.Close()
	go mustCopy(os.Stdout, conn)
	mustCopy(conn, os.Stdin)
}

func mustCopy(dst io.Writer, src io.Reader) {
	if _, err := io.Copy(dst, src); err != nil {
		log.Fatal(err)
	}
}

```

当 Main Goroutine 从标准输入流中读取内容并将其发送给服务器时，另一个 Goroutine 会读取并打印服务端的响应。当 Main Goroutine  碰到输入终止时，例如，用户在终端中按了`Control-D(^D)`，在 Windows 上是`Control-Z`，这时程序就会被终止，尽管其它 Goroutine 中还有进行中的任务

```go
➜  echo ./netcat
Hello?
         HELLO?
         Hello?
         hello?
Is there anybody there?
         IS THERE ANYBODY THERE?
         Is there anybody there?
         is there anybody there?
^C      
➜  echo ./netcat
Hello?
         HELLO?
         Hello?
         hello?
Is there anybody there?
         IS THERE ANYBODY THERE?
Yooo-hooo!
         Is there anybody there?
         is there anybody there?
         YOOO-HOOO!
         Yooo-hooo!
         yooo-hooo!
2024/04/14 18:31:37 writeto tcp [::1]:41974->[::1]:8001: write /dev/stdout: use of closed network connection
➜  echo killall echo  
[1]  - 499260 terminated  ./echo
```

注意客户端的第三次 `shout` 在前一个 `shout` 处理完成之前一直没有被处理，这貌似看起来不是特别“现实”。真实世界里的回响应该是会由三次 `shout` 的回声组合而成的。为了模拟真实世界的回响，我们需要更多的 Goroutine 来做这件事情。这样我们就再一次地需要 `go` 这个关键词了，这次我们用它来调用 `echo`

```go
func handleConn(c net.Conn) {
	input := bufio.NewScanner(c)
	for input.Scan() {
		go echo(c, input.Text(), 1*time.Second)
	}
}
```

`go` 后跟的函数的参数会在 `go` 语句自身执行时被求值；因此 `input.Text()` 会在 `main goroutine` 中被求值。 现在回响是并发并且会按时间来覆盖掉其它响应了

```
➜  echo ./netcat 
Is there anybody there?
         IS THERE ANYBODY THERE?
Yooo-hooo!
         YOOO-HOOO!
         Is there anybody there?
         Yooo-hooo!
         is there anybody there?
         yooo-hooo!
```

**让服务使用并发不只是处理多个客户端的请求，甚至在处理单个连接时也可能会用到**，就像我们上面的两个 `go` 关键词的用法。然而在我们使用 `go` 关键词的同时，**需要慎重地考虑  `net.Conn` 中的方法在并发地调用时是否安全**，事实上对于大多数类型来说也确实不安全

## runtime 包

包 `runtime` 包含与 Go 运行时系统交互的操作，例如 **控制 `goroutines` 的函数**。它还包含 `reflect` 包使用的底层类型信息。这里，我们先看几个关于控制 `goroutines` 的函数

### `runtime.Gosched()`

`runtime.Gosched()` 该函数用于 **让出当前 `goroutine` 占用的 CPU 时间片**， 调度器安排其他等待的任务运行，并在下次再获得 CPU 时间轮片的时候， 从该出让CPU的位置恢复执行

```go
package main

import (
	"fmt"
	"runtime"
	"sync"
)

var wg sync.WaitGroup

func sing() {
	defer wg.Done()
	for i := 0; ; i++ {
		runtime.Gosched() // Gosched放弃了处理器，允许其他 goroutines 运行。它不会挂起当前的 goroutine，因此执行会自动恢复。
		fmt.Printf("---唱歌---%d\n", i+1)
	}
}

func dance() {
	defer wg.Done()
	for i := 0; ; i++ {
		fmt.Printf("---跳舞---%d\n", i+1)
	}
}

func main() {
	wg.Add(1)
	go sing()
	wg.Add(1)
	go dance()
	wg.Wait()
}
```

运行结果为

```
....
---跳舞---40188
---跳舞---40189
---跳舞---40190
---跳舞---40191
---跳舞---40192
---跳舞---40193
---唱歌---807
---跳舞---40194
---跳舞---40195
---跳舞---40196
....
---跳舞---41112
---跳舞---41113
---唱歌---833
---跳舞---41114
---跳舞---41115
```

### `runtime.Goexit()`

**`Goexit` 会终止调用它的 `goroutine`**。其他 `goroutine` 不会受到影响。`Goexit` 会在终止 `goroutine` 之前运行所有延迟调用。由于 `Goexit` 并非`panic`，因此这些延迟函数中的任何 `recover` 调用都将返回 `nil`

```go
package main

import (
	"fmt"
	"runtime"
)

func main() {
	go func() {
		defer fmt.Println("A.defer")

		func() {
			defer fmt.Println("B.defer")
			runtime.Goexit() // 终止当前 goroutine, import "runtime"
			fmt.Println("B") // 不会执行
		}()

		fmt.Println("A") // 不会执行
	}() //不要忘记()

	//死循环，目的不让主goroutine结束
	for {
		;
	}
}
```

### `runtime.GOMAXPROCS()`

`GOMAXPROCS` 设置可以同时执行的 CPU 的最大数目，并返回前一个设置。默认值为`runtime.NumCPU`。如果 `n < 1`，则不改变当前设置。当调度器的值发生改变时，返回之前的值。

```go
package main

import (
	"fmt"
	"runtime"
	"time"
)

func main() {
	fmt.Println(runtime.GOMAXPROCS(2)) // 16 返回的是前一次的最大cpu数

	for {
		go fmt.Print(1)
		fmt.Print(0)
		time.Sleep(1 * time.Second)
	}
}
```

## Channels

在 [[Linux 系统编程：进程间通信简介]] 中介绍了进程之间需要通过管道等 IPC 工具进行通信。在 Go 语言中，Goroutines 之间也需要通信，使用的通信工具称为通道(Channels)

一个 Channel 是一个通信机制，它可以让一个 Goroutine 通过它给另一个 Goroutine 发送值信息。每个 Channel 都有一个特殊的类型，也就是 Channels 可发送数据的类型。一个可以发送 `int` 类型数据的 Channel 一般写为 `chan int`

Channel 底层是一个 **环形队列**，使用固定大小环形数组实现。关于环形队列，参考 [[数据结构：栈和队列#顺序队列(环形队列)]]。

![[Pasted image 20240808002408.png]]

Channel 的底层数据结构声明在 `$GOROOT/src/runtime/chan.go` 中的 `hchan` 结构体

```go
type hchan struct {
	qcount   uint           // total data in the queue
	dataqsiz uint           // size of the circular queue
	buf      unsafe.Pointer // points to an array of dataqsiz elements
	elemsize uint16
	closed   uint32
	elemtype *_type // element type
	sendx    uint   // send index
	recvx    uint   // receive index
	recvq    waitq  // list of recv waiters
	sendq    waitq  // list of send waiters

	// lock 保护 hchan 中的所有字段，以及该 channel 上被阻塞的 sudogs 中的几个字段。
	//
	// 在持有此锁时，不要改变另一个G的状态(特别是，不要准备一个G)，因为这可能会随着堆栈收缩而死锁。
	lock mutex
}
```

创建 Channel 最简单的方法就是使用内置的 `make()` 函数：

```go
// 指定底层环形数组最多容纳 0 个 int 类型的元素
ch := make(chan int)

// 指定底层环形数组最多可以容纳 20 个 int 类型元素
ch := make(chan int, 20)
```

和 Map 类似，Channel 也对应一个 `make` 创建的底层数据结构的引用。当我们复制一个 Channel 或用于函数参数传递时，我们只是拷贝了一个 Channel 引用，因此调用者和被调用者将引用同一个 Channel 对象。和其它的引用类型一样，Channel 的零值也是 `nil`

> [!attention] 
> 
> 两个相同类型的 Channel 可以使用运算符 `==` 进行比较。如果两个 Channel 引用的是相同的对象，那么比较的结果为真。一个 Channel 也可以和 `nil` 进行比较 
> 

Channel 支持 $3$ 种操作：**发送值**、**读取值** 和 **关闭 Channel**。下表列出了这个三种操作的语法

| 操作                   | 描述                                                   |
| :------------------- | :--------------------------------------------------- |
| `ch <- value`        | 向通道 `ch` 发送一个 `value`。通道处于 **满** 状态时将阻塞当前 Goroutine  |
| `value, ok := <- ch` | 从通道 `ch` 中接收一个 `value`。通道处于 **空** 状态时将阻塞当前 Goroutine |
| `close(ch)`          | 关闭通道 `ch`。发送操作导致 pannic 异常，接收操作可以正常执行                |

> [!attention] 
> 
> 请注意：如果 Channel 中没有数据，并且 Channel 已经被关闭，则接收操作将产生一个零值的数据并且返回一个 `false` 表示已经没有数据了
> 

### 无缓冲 Channel

底层环形数组的长度为 $0$ 的 Channel 称为 **无缓冲 Channel**。换句话说，无缓冲 Channel 对于发送者而言处于 **满** 状态，对于接收者而言处于 **空** 状态。因此，从无缓冲 Channel 中接收或者向无缓冲 Channel 中发送数据都将导致 Goroutine 阻塞，直到另一个 Goroutine 向无缓冲 Channel 中发送数据或者读取数据

如果接收操作先发生，那么执行接收操作的 Groutine 将阻塞，直到另一个 Groutinne 在相同的 Channel 上执行发送操作

> [!tip] 
> 
> 无缓冲 Channle 的 send 和 recv 操作将会导致两个 Groutine 进行一次同步
> 

因为这个原因，无缓冲 Channels 有时候也被称为 **同步 Channels**。当通过一个无缓冲 Channels 发送数据时，接收者收到数据发生在唤醒发送者 Goroutine 之前

> [!attention] Happens Before
> 
> 术语 `Happens Before` 是 Go 语言并发内存模型的一个关键术语。它建立了事件之间的部分顺序，它告诉我们 **哪些操作必须在其他操作之前完成**
> 

在无缓冲 Channel 的操作中，存在以下 happens-before 关系：

1. **发送完成 happens-before 接收完成**：
    - 对于无缓冲 Channel 的发送操作 `ch <- x` 的完成 happens-before 对应的接收操作 `<-ch` 的完成
    - 这意味着 **接收者一定能看到发送者发送的值**
    
2. **接收开始 happens-before 发送完成**：
    - 接收操作开始 happens-before 发送操作完成


在 [[Go 语言：Goroutines 与 Channels#示例：并发的 Echo 服务]] 中的客户端程序，我们要求可以同时从标准输入中复制数据到 Socket(Main Groutine)，并从 Socket 复制数据到标准输出(后台 Groutine)。当 Main Groutine 关闭标准输入时，后台 Groutine 可能尚未完成读取操作，因此需要让主 Groutine 等待后台 Groutine

```go
package main

import (
	"io"
	"log"
	"net"
	"os"
)

func main() {
	// 无缓冲 Channel 用于等待后台 Goroutine 退出
	done := make(chan struct{})

	conn, err := net.Dial("tcp", "localhost:8000")
	if err != nil {
		log.Fatal(err)
	}

	// 启动一个后台 Goroutine 从 conn 复制到标准输出
	go mustCopy(os.Stdout, conn, done)

	// 主 Groutine 从标准输入复制到 conn
	_, err = io.Copy(conn, os.Stdin)

	if err != nil {
		log.Println("close the stdin")
	}

	conn.Close() // 关闭连接
	<-done       // 等待后台 Groutine 发送消息
}

func mustCopy(dst io.Writer, src io.Reader, done chan struct{}) {
	if _, err := io.Copy(dst, src); err != nil {
		log.Fatal(err)
	}
	done <- struct{}{} // 通知主 Goroutine 退出
}
```

当用户关闭了标准输入，Main Goroutine 中的 `io.Copy(conn, os.Stdin)` 函数调用将返回，然后调用`conn.Close()` 关闭读和写方向的网络连接。关闭网络连接中的写方向的连接将导致 server 程序收到一个文件结束的信号(end-of-file, EOF)。关闭网络连接中读方向的连接将导致后台 Goroutine 的 `io.Copy` 函数调用返回一个“ read from closed connection” （“从关闭的连接读”）类似的错误

基于 Channels 发送消息有两个重要方面。首先 **每个消息都有一个值**，但是有时候通讯的事实和发生的时刻也同样重要。当我们更希望强调通讯发生的时刻时，我们将它称为消息事件

> [!tip] 
> 
> 有些消息事件并不携带额外的信息，它仅仅是用作两个 Goroutine 之间的同步，这时候我们可以用 `struct{}` 空结构体作为Channels 元素的类型
> 


### 串联的 Channels

Channels 也可以用于将多个 Goroutine 连接在一起，一个 Channel 的输出作为下一个 Channel 的输入。这种串联的 Channels 就是所谓的 **管道**。下面的程序用两个 Channels 将三个 Goroutine 串联起来

![[Pasted image 20250714214152.png]]

第一个 Goroutine 是一个计数器，用于生成整数序列，然后通过 Channel 将该整数序列发送给第二个Goroutine；第二个 Goroutine 是一个求平方的程序，对收到的每个整数求平方，然后将平方后的结果通过第二个 Channel 发送给第三个 Goroutine；第三个 Goroutine 是一个打印程序，打印收到的每个整数。

```go hl:18,25
package main

import "fmt"

func main() {
	natural := make(chan int)
	square := make(chan int)

	// 第一个 Goroution：用于产生一个自然数序列
	go func() {
		for i := 0; ; i++ {
			natural <- i
		}
	}()
	// 第二个 Goroution：用于计算自然数的平方
	go func() {
		// 遍历自然数序列，并计算每个数的平方
		for v := range natural {
			// 将自然数的平方发送到 square 通道
			square <- v * v
		}
	}()

	// 主 Goroution：从 square 通道接收平方数并打印
	for v := range square {
		fmt.Printf("%v ", v)
	}
}
```

在这个例子中，我们引入了遍历 Channel 的语法：使用 `range` 关键可以遍历 Channel，直到 Channel 中的元素被遍历完。

如您所料，上面的程序将生成自然数平方的无穷数列。像这样的串联 Channels 的管道可以用在需要长时间运行的服务中，每个长时间运行的 Goroutine 可能会包含一个死循环，在不同 Goroutine 的死循环内部使用串联的 Channels 来通信。但是，如果我们希望通过 Channels 只发送有限的数列该如何处理呢？

如果发送者知道，没有更多的值需要发送到 Channel 的话，那么让接收者也能及时知道没有多余的值可接收将是有用的，因为接收者可以停止不必要的接收等待。这可以通过内置的 `close()` 函数来关闭 Channel 实现：

```go hl:14,23
package main

import "fmt"

func main() {
	natural := make(chan int)
	square := make(chan int)

	// 第一个 Goroution：用于产生一个自然数序列
	go func() {
		for i := range 20 {
			natural <- i
		}
		close(natural) // 关闭 natural 通道，表示不再发送数据
	}()
	// 第二个 Goroution：用于计算自然数的平方
	go func() {
		// 遍历自然数序列，并计算每个数的平方
		for v := range natural {
			// 将自然数的平方发送到 square 通道
			square <- v * v
		}
		close(square) // 关闭 square 通道，表示不再发送数据
	}()

	// 主 Goroution：从 square 通道接收平方数并打印
	for v := range square {
		fmt.Printf("%v ", v)
	}
	fmt.Println() // 打印换行符以美化输出
}
```

### 单向 Channels

随着程序的增长，人们习惯于将大的函数拆分为小的函数。我们前面的例子中使用了三个 Goroutine，然后用两个 Channels 来连接它们，它们都是 `main` 函数的局部变量。将三个 Goroutine 拆分为以下三个函数是自然的想法

```go
func counter(out chan int)
func squarer(out, in chan int)
func printer(in chan int)
```

其中计算平方的 `squarer()` 函数在两个串联 Channels 的中间，因此拥有两个 Channel 类型的参数，一个用于输入一个用于输出。两个 Channel 都拥有相同的类型，但是它们的使用方式相反：一个只用于接收，另一个只用于发送。参数的名字 `in` 和 `out` 已经明确表示了这个意图

> [!attention] 
> 
> 请注意：参数名字并无法保证 `squarer()` 函数向一个 `in` 参数对应的 Channel 发送数据或者从一个 `out` 参数对应的 Channel 接收数据。
> 
> **当一个 Channel 作为一个函数参数时，它一般总是被专门用于 _只发送_ 或者 _只接收_**。
> 

为了表明这种意图并防止被滥用，Go 语言的类型系统提供了单向 Channel 类型，分别用于只发送或只接收的 Channel

| 单向 Channel 类型 | 描述                         |
| :------------ | :------------------------- |
| `chan<- Type` | 表示一个只发送 `Type` 类型的 Channel |
| `<-chan Type` | 表示一个只接收 `Type` 类型的 Channel |

> [!attention] 
> 
> 使用运算符 `<-` 限制 Channel 的行为，并且，这种限制将在编译期检测
> 
> 因为关闭操作只用于断言不再向 Channel 发送新的数据，所以只有在发送者所在的 Goroutine 才会调用 `close()` 函数，因此对一个只接收的 Channel 调用 `close()` 函数将是一个编译错误
> 

```go
package main

import "fmt"

func counter(out chan<- int) {
	for i := range 100 {
		out <- i
	}
	close(out) // 关闭通道，表示不再发送数据
}

func squarer(in <-chan int, out chan<- int) {
	for v := range in {
		out <- v * v // 计算平方并发送到输出通道
	}
	close(out) // 关闭输出通道，表示不再发送数据
}

func printer(in <-chan int) {
	for v := range in {
		fmt.Printf("%d ", v) // 打印接收到的整数
	}
	fmt.Println()
}

func main() {
	natural := make(chan int)
	square := make(chan int)

	// 第一个 Goroution：用于产生一个自然数序列
	go counter(natural)

	// 第二个 Goroution：用于计算自然数的平方
	go squarer(natural, square)

	// 主 Goroution：从 square 通道接收平方数并打印
	printer(square)
}
```

调用 `counter(natural)` 将导致将 `chan int` 类型的 `natural` 隐式地转换为 `chan<- int` 类型只发送型的 Channel。调用 `printer(squares)` 也会导致相似的隐式转换，这一次是转换为 `<-chanint` 类型只接收型的 Channel。

> [!attention] 
> 
> 请注意：任何双向 Channel 向单向 Channel 变量的赋值操作都将导致该隐式转换。这里并没有反向转换的语法：也就是不能将一个类似 `chan<- int` 类型的单向型的 Channel 转换为 `chan int` 类型的双向型的 Channel
> 
> **即**：不能将单向 Channel 转换为双向 Channel
> 

### 缓冲 Channel

带缓冲的 Channel 内部持有一个环形队列。队列的最大容量是在调用 `make()` 函数创建 Channel 时
通过第二个参数指定的。下面的语句创建了一个可以持有三个字符串元素的带缓冲 Channel

```go
ch = make(chan string, 3)
```

下图展示了 `ch` 变量对应的 Channel 的图形表示形式

![[Pasted image 20250714222313.png]]

> [!tip] 
> 
> 向缓冲 Channel 的发送操作就是向内部缓冲队列的尾部插入元素，接收操作则是从队列的头部删除元素
> + 如果内部缓冲队列是满的，那么发送操作将阻塞直到因另一个 Goroutine 执行接收操作而释放了新的队列空间。
> + 相反，如果 Channel 是空的，接收操作将阻塞直到有另一个 Goroutine 执行发送操作而向队列插入元素
> 

我们可以在无阻塞的情况下连续向新创建的 Channel 发送三个值：

```go
ch <- "A"
ch <- "B"
ch <- "C"
```

此刻，Channel 的内部缓冲队列将是满的，如果有第四个发送操作将发生阻塞。

![[Pasted image 20250714222545.png]]

如果我们接收一个值，那么 Channel 的缓冲队列将不是满的也不是空的，因此对该 Channel 执行的发送或接收操作都不会发生阻塞。通过这种方式，Channel 的缓冲队列解耦了接收和发送的 Goroutine。

![[Pasted image 20250714222744.png]]

有时候，我们可能需要知道 Channel 的容量是多少，此时，我们可以使用 `cap()` 内置函数获取

```go
fmt.Println(cap(ch))
```

同样，对于 `len()` 内置函数，如果传入的是 Channel，那么将返回 Channel 内部缓存队列中有效元素的个数。因为在并发程序中该信息会随着接收操作而失效，但是它对某些故障诊断和性能优化会有帮助。

```go
fmt.Println(len(ch))
```

## 示例：并发的 Web 爬虫

在这个示例中，我们将从一个 URL(统一资源定位符) 开始提取该 URL 指向 HTML 页面中的所有的 URL，然后继续遍历这些找到的 URL

首先，我们需要一个提取 HTML 页面中的 URL 的程序。下面给出了这个程序

```go title:links
package links

import (
	"fmt"
	"net/http"

	"golang.org/x/net/html"
)

// Extract 函数对特定的 url 发起 HTTP Get 请求，并解析返回的 HTML 中的链接
func Extract(url string) ([]string, error) {
	resp, err := http.Get(url)
	if err != nil {
		return nil, err
	}
	// 响应体的状态码检查
	defer resp.Body.Close() // 确保在函数结束时关闭响应体
	if resp.StatusCode != http.StatusOK {
		return nil, fmt.Errorf("parsing %s : %s", url, resp.Status)
	}
	doc, err := html.Parse(resp.Body)
	if err != nil {
		return nil, fmt.Errorf("parsing %s as HTML: %v", url, err)
	}
	var links []string
	visitNode := func(n *html.Node) {
		if n.Type == html.ElementNode && n.Data == "a" {
			for _, a := range n.Attr {
				if a.Key != "href" {
					continue
				}
				link, err := resp.Request.URL.Parse(a.Val)
				if err != nil {
					continue
				}
				links = append(links, link.String())
			}
		}
	}
	forEachNode(doc, visitNode, nil) // 遍历 HTML 节点
	return links, nil
}

func forEachNode(n *html.Node, pre, post func(n *html.Node)) {
	if pre != nil {
		pre(n) // 先执行 pre 函数
	}
	for c := n.FirstChild; c != nil; c = c.NextSibling {
		forEachNode(c, pre, post) // 递归遍历子节点
	}
	if post != nil {
		post(n) // 后执行 post 函数
	}
}
```

准备好这个辅助程序后，我们可以开始编写我们的 Web 爬虫程序了。

```go title:main.go
package main

import (
	"fmt"
	"log"
	"os"

	"github.com/duyupeng36/links"
)


func crawl(url string) []string {
	fmt.Println(url)
	list, err := links.Extract(url)
	if err != nil {
		log.Println(err)
	}
	return list
}

func main() {
	worklist := make(chan []string)

	// 从命令行读取参数：由于我们使用无缓冲 Channel，这里我们应该将命令行参数房子一个独立的
	// Goroutine 中，否则将阻塞 main Goroutine 的执行(is，接收者尚未准备好)
	go func() {
		worklist <- os.Args[1:]
	}()
	// 记录已经访问的网址
	seen := make(map[string]bool)
	for list := range worklist {
		for _, link := range list {
			if !seen[link] {
				seen[link] = true
				// 对于每个 URL 都开启一个 Goroutine 进行处理
				go func(link string) {
					worklist <- crawl(link)
				}(link)
			}
		}
	}
}
```

这样这个程序就可以并发的抓取每个 URL 中的 URL，从而遍历完所有的 URL。然而，当 URL 越来越多，开启的 Goroutine 也会越来越多。然而，一个进程的打开文件描述符是有上限的。每次访问一个 URL 都会消耗一个打开文件描述符。从而导致某些 URL 会打开失败

> [!attention] 
> 
> 上述例程并发量过大，无穷无尽地并发并不是什么好事情。因为计算机资源总是有限的
> + CPU核心数会限制你的计算负载
> + 硬盘转轴和磁头数限制了你的本地磁盘IO操作频率
> + 网络带宽限制了你的下载速度上限
> + 一个web服务的服务容量上限等等
> 

为了缓解这一个问题，我们需要限制并发的数量。对于我们的例子来说，最简单的方法就是限制对 `links.Extract()` 在同一时间最多不会有超过 $n$ 次调用，这里的 $n$ 是打开文件描述符的 `limit-20` 

这里我们可以使用缓冲 Channel 来限制并发数量

```go title:main.go

var sem = make(chan struct{}, 20)

func crawl(url string) []string {
	fmt.Println(url)
	// 获取信号量
	sem <- struct{}{}
	list, err := links.Extract(url)
	// 释放信号量
	<-sem
	if err != nil {
		log.Println(err)
	}
	return list
}
```

函数 `crawl()` 进入时就像 `sem` Channel 发送一个值。如果 `sem` Channel 此时是满的，就证明限制已经有 `cap(sem)` 个 Goroutine 在执行了。此时，需要等待其他 Goroutine 执行完毕。当 Goroutine 执行完毕时，就会从 `sem` Channel 中读取一个值

> [!tip] 
> 
> 这种限制并发数量的方法称为信号量。参考 [[Linux 系统编程：System V 信号量]] 和 [[Python：线程#信号量：Semaphore]]
> 

另一种限制并发数量的方式就是使用固定数量的 Goroutine

```go
func main() {
	worklist := make(chan []string)
	unseenLinks := make(chan string)

	go func() {
		worklist <- os.Args[1:]
	}()

	// 创建 20 个 Goroutine 用于执行 crawl 函数
	for range 20 {
		go func() {
			for link := range unseenLinks {
				// 执行 crawl() 获取 URL
				links := crawl(link)
				// 注意：
				go func() {
					worklist <- links
				}()
			}
		}()
	}

	// 记录已经访问的网址
	seen := make(map[string]bool)
	for list := range worklist {
		for _, link := range list {
			if !seen[link] {
				seen[link] = true
				unseenLinks <- link
			}
		}
	}
}
```


## 定时器

`time` 包中提供了两种类型定时器：`time.Timer` 和 `time.Ticker`

### Timer 定时器

`Timer` 类型表示 **单个事件**。定时器过期时，将在其通道 `C` 上发送当前时间，除非该定时器是用 `AfterFunc()` 创建的。必须使用 `NewTimer()` 或 `AfterFunc()` 创建定时器

```go
type Timer struct {
	C <-chan Time
	r runtimeTimer
}
```

使用构造函数 `time.NewTimer(duration)` 和 `time.AfterFunc(duration, f func())` ，可以创建定时器。此外，`Timer` 还支持两个方法

```go
// Reset 将计时器更改为 d 后过期。如果计时器处于活动状态，则返回 true，如果计时器已过期或已停止，则返回 false。
func (t *Timer) Reset(d Duration) bool

// Stop 阻止计时器启动。如果调用停止了定时器，则返回 true；如果定时器已过期或被停止，则返回 false。Stop 不会关闭通道，以防止通道读取错误成功。
func (t *Timer) Stop() bool
```

```go
package main

import (
	"fmt"
	"time"
)

func main() {
	fmt.Println(time.Now())
	timer := time.NewTimer(time.Second)
	fmt.Println(<-timer.C)

	fmt.Println(timer.Reset(2 * time.Second)) // 重置定时器
	fmt.Println(<-timer.C)
}
```

### Ticker 周期定时器

**`Ticker` 持有一个通道**，每隔一段时间就向通道中写入一个 **“滴答声”**

```go
type Ticker struct {
	C <-chan Time // 传送 时钟滴答 的通道
	r runtimeTimer
}
```

`NewTicker()` 返回一个新的 `Ticker`，其中包含一个通道，每个 `tick` 之后都会发送通道上的当前时间。刻度的周期由持续时间参数指定

```go
func NewTicker(d Duration) *Ticker
```

`Ticker` 会调整时间间隔或放弃刻度，以弥补接收速度慢的问题。持续时间 `d` 必须大于零；否则，`NewTicker` 就会崩溃。停止滴答器以释放相关资源

```go
// Reset 会停止一个刻度线，并将其周期重置为指定的持续时间 d
// 下一个刻度将在新的周期结束后到达
// 持续时间 d 必须大于零；否则，Reset 就会出错。
func (t *Ticker) Reset(d Duration)

// Stop 关闭 ticker。停止后，将不再发送 "tick"
// Stop 不会关闭通道，以防止同时从通道读取数据的程序看到错误的 "tick"
func (t *Ticker) Stop()
```

```go
package main

import (
	"fmt"
	"time"
)

func main() {
	fmt.Println(time.Now())
	ticker := time.NewTicker(time.Second)
	for {
		fmt.Println(<-ticker.C) // 通道每阻塞 1 秒就接收一次
	}
}

```

## 通道多路复用

Go语言提供了 `select` 来监听多个 Channels，它会随机挑选已就绪的通道进行操作

下面的程序会进行火箭发射的倒计时。`time.Tick` 函数返回一个 Channel，程序会周期性地像一个节拍器一样向这个 Channel 发送事件。每一个事件的值是一个时间戳，不过更有意思的是其传送方式

现在我们让这个程序支持在倒计时中，用户按下 `enter` 键时直接中断发射流程。首先，我们启动一个 `goroutine`，这个 `goroutine` 会尝试从标准输入中调入一个单独的 `byte` 并且，如果成功了，会向名为 `abort` 的 Channel 发送一个值

```go
abort := make(chan struct{})
go func() {
	os.Stdin.Read(make([]byte, 1)) // 读入一个 byte
	abort <- struct{}{}
}()
```

现在每一次计数循环的迭代都需要等待两个 Channel 中的其中一个返回事件了：`ticker channel` 当一切正常时或者异常时返回的 `abort` 事件

我们 **无法做到从每一个 Channel 中接收信息**，如果我们这么做的话，如果 **第一个Channel 中没有事件发过来那么程序就会立刻被阻塞**，这样我们就 **无法收到第二个 Channel 中发过来的事件**

这时候我们需要 **多路复用**(`multiplex`)这些操作了，为了能够多路复用，我们使用了 `select` 语句。`select` 的使用方式类似于之前学到的 `switch` 语句，它也有一系列 `case` 分支和一个默认的分支。每个 `case` 分支会对应一个通道的通信（接收或发送）过程。`select` 会一直等待，直到其中的某个 `case` 的通信操作完成时，就会执行该 `case` 分支对应的语句

```go
select {
case <-ch1:
    // ...
case x := <-ch2:
    // ...use x...
case ch3 <- y:
    // ...
default:
    // ...
}
```

在一个 `select` 语句中，Go 语言会按顺序从头至尾评估每一个发送和接收的语句。如果其中的任意一语句可以继续执行(即没有被阻塞)，那么就从那些可以执行的语句中 **任意选择** 一条来使用。

如果没有任意一条语句可以执行(**即所有的通道都被阻塞**)，那么有两种可能的情况

> [!tip] 
> 如果给出了`default`语句，那么就会执行 `default`语句，同时程序的执行会从`select`语句后的语句中恢复。这样 **出现忙轮询**，造成 CPU 资源浪费。使用`select`时，一般不写`default`
> 
>  如果没有 `default` 语句，那么 `select` 语句将被阻塞，直到至少有一个通信可以进行下去
>  

让我们回到我们的火箭发射程序。`time.After` 函数会立即返回一个 Channel，并起一个新的 Goroutine 在经过特定的时间后向该 Channel 发送一个独立的值。下面的 `select` 语句会会一直等待到两个事件中的一个到达，无论是 `abort` 事件或者一个 $10$ 秒经过的事件。如果 $10$ 秒经过了还没有 `abort` 事件进入，那么火箭就会发射

```go
package main

import (
	"fmt"
	"os"
	"time"
)

func main() {

	abort := make(chan struct{})
	go func() {
		os.Stdin.Read(make([]byte, 1)) // 读入一个 byte
		abort <- struct{}{}
	}()

	fmt.Println("Commencing countdown.  Press return to abort.")
	select {
	case <-time.After(10 * time.Second):
		// Do nothing.
	case <-abort:
		fmt.Println("Launch aborted!")
		return
	}
	fmt.Println("Launch complete.")
}
```

下面让我们的发射程序打印倒计时。这里的 `select` 语句会使每次循环迭代等待一秒来执行退出操作

```go
package main

import (
	"fmt"
	"os"
	"time"
)

func main() {
	abort := make(chan struct{})
	// 定时器：每隔指定时间就会向通道写入一个值
	ticker := time.Tick(1 * time.Second)

	go func() {
		os.Stdin.Read(make([]byte, 1))
		abort <- struct{}{}
	}()

	fmt.Println("Commencing countdown.  Press return to abort.")
	for countdown := 10; countdown > 0; countdown-- {
		fmt.Printf("\rcountdown %02v", countdown)
		select {
		case <-ticker:
		case <-abort:
			fmt.Println("Launch aborted!")
			return
		}
	}
	fmt.Println("\nLaunch Success!")
}
```
