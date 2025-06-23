# 异常处理

错误至少可以被分为两种：**错误** 和 **异常**。在 Go 语言中，没有专门的语句用于处理异常，而是利用 Go 语言函数可以返回多个值的特性用于异常处理

## 异常

当 Go 运行时检测到一个错误时，程序就无法继续执行了，出现了一些错误的提示， 这就是所谓的 **"异常"**。所以为了保证程序的健壮性，要对异常的信息进行处。**Go 语言中把错误当成一种特殊的值来处理**，不支持其他语言中使用 `try/catch` 捕获异常的方式

### errors 包

`error` 接口是 Go 语言提供的错误处理的标准模式，该接口的定义在 `builtin` 包中

```go
package builtin

type error interface {
	Error() string
}
```

Go 语言在 `errors` 包中提供了一个 `error` 接口的默认实现

```go
package errors

type errorString struct {
	text string
}

func New(text string) error{
	return &errorString{text}
}

func (e *errorString)Error() string {
	return e.text
}
```

下面的示例展示了 `error` 的用法

```go hl:10
package main

import (
	"errors"
	"fmt"
)

func div(x, y int) (int, error) {
	if y == 0 {
		return 0, errors.New("0 不能做除数")
	} else {
		return x / y, nil
	}
}

func main() {
	result, err := div(10, 0)
	if err != nil {
		fmt.Println(err)
		return
	}
	fmt.Println(result)
}
```

### 创建异常

在 Go 语言中，创建异常的最简单的方式是使用 `errors` 包提供的 `New()` 函数创建一个错误。

#### `errors.New()`

`errors.New()` 函数签名如下

```go
func New(text string) error
```

它接收一个字符串参数返回包含该字符串的错误。标准库 `io.EOF` 错误定义如下

```go
var EOF = errors.New("EOF")
```

#### `fmt.Errorf()`

当我们需要传入格式化的错误描述信息时，使用 `fmt.Errorf()` 是个更好的选择

```go
fmt.Errorf("查询数据库失败，err:%v", err)
```

> [!warning] 
> 
> 请注意：上面的方式会 **丢失原有的错误类型**，只拿到错误描述的文本信息
> 

为了不丢失函数调用的错误链，使用 `fmt.Errorf()` 时搭配使用特殊的 **格式转换符 `%w`**，可以实现 **基于已有的错误再包装得到一个新的错误**。

```go
fmt.Errorf("查询数据库失败，err:%w", err)
```

对于这种二次包装的错误，`errors` 包中提供了以下三个方法

```go
func Unwrap(err error) error  // 获得 err 包含下一层错误

func Is(err, target error) bool  // 判断 err 是否包含 target

func As(err error, target interface{}) bool 
// 判断 err 是否为 target 类型
```

### 自定义异常类型

我们只需要让一个类型实现 `error` 接口，就能自定义一个异常类型。例如

```go
// OpError 自定义结构体类型
type OpError struct {
	Op string
}

// Error OpError 类型实现error接口
func (e *OpError) Error() string {
	return fmt.Sprintf("无权执行 %s 操作", e.Op)
}
```


## 错误

使用 `error` 接口的错误往往只是一般性错误，这类错误不会导致程序崩溃。然而，程序有时候会出现 `panic` 错误，这种类型的错误往往意味着程序以及无法执行了

> [!tip] 
> 
> 在 Go 语言中，以下操作往往会导致 `panic` 错误
> + 调用内建函数 `panic()`
> + 数组访问越界
> + 解引用空指针
> + 除 $0$ 异常
> + 用未实现的方法
> + ...
> 

### recover

一般而言，当 `panic` 异常发生时，程序会中断运行。随后， 程序崩溃并输出日志信息。日志信息包括 `panic value` 和函数调用的堆栈跟踪信息 

> [!NOTE]
> 
> 当然，如果直接调用内置的 `panic()` 函数也会引发 `panic` 异常； `panic()` 函数接受任何值作为参数
> 

> [!WARNING] 
> 
> 我们在实际的开发过程中并不会直接调用`panic`函数，但是当我们的 **程序遇到致命错误时**， 系统 **会自动调用该函数来终止整个程序的运行**，也就是系统内置了 `panic` 函数
> 

程序执行时 `panic` 异常一旦被引发就会导致程序崩溃。这当然不是我们愿意看到的， 因为谁也不能保证程序不会发生任何运行时错误。为了保证程序的健壮性，Go语言为我们提供了专用于 **“拦截”运行时 `panic` 的内建函数 `recover`**。 它可以是当前的程序从运行时 `panic` 的状态中恢复并重新获得流程控制权

`recover` 函数声明

```go
func recover() interfacea{}
```

> [!attention] 
> 
> 警告：使用 `recover()` 需要注意点
> - **`recover` 只有在 `defer` 调用的函数中有效**
> - **`defer` 一定要在可能引发 `panic` 的语句之前定义**
> 

下面的示例是从 `painc` 中恢复的演示

```go
package main

import (
	"fmt"
)

func funcA() {
	fmt.Println("func A")
}

func funcB() {
	/* defer 延迟调用 recover() 函数 */
	defer func() {
		err := recover()
		if err != nil {
			fmt.Printf("recover in B, err: %#v\n", err)
		}
	}()
	panic("panic in func B")  // 触发 panic 异常
}

func funcC() {
	fmt.Println("func C")
}

func main() {
	funcA() // func A
	funcB() // recover in B, err: "panic in func B"
	funcC() // func C
}
```
