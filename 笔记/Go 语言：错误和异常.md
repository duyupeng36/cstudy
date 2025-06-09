# 错误和异常

Go 创建者没有添加异常处理程序，而是利用了 Go 返回多个值的能力。发出错误的最常用的 Go 技术是将错误作为返回值返回

**`panic` 通常意味着出现了意外错误**。主要用于在正常运行中不应该出现的错误，或我们还没准备好从容应对的错误时快速失效

Go 语言中的 `panic` 中恢复依赖于该语言的一个特性--**延迟函数**。Go 能够保证在函数的父函数返回时执行该函数。无论父函数返回的原因是返回语句、函数块结束还是 `panic`，这都会发生

## 异常

当 Go 检测到一个错误时，程序就无法继续执行了，出现了一些错误的提示， 这就是所谓的 **"异常"**。所以为了保证程序的健壮性，要对异常的信息进行处。**Go 语言中把错误当成一种特殊的值来处理**，不支持其他语言中使用 `try/catch` 捕获异常的方式

### `error` 接口

`errors` 接口是 Go 语言提供的错误处理的标准模式，该接口的定义如下

```go
package errors

type error interface {
	Error() string
}

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

下面的示例展示了 `errors` 的用法

```go
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

我们可以根据需求自定义 `error`，最简单的方式是使用 `errors` 包提供的 `New` 函数创建一个错误。

#### `errors.New()`

`errors.New` 函数签名如下

```go
func New(text string) error
```

它接收一个字符串参数返回包含该字符串的错误。标准库 `io.EOF` 错误定义如下

```go
var EOF = errors.New("EOF")
```

#### `fmt.Errorf()`

当我们需要传入格式化的错误描述信息时，使用 `fmt.Errorf` 是个更好的选择

```go
fmt.Errorf("查询数据库失败，err:%v", err)
```

但是上面的方式会 **丢失原有的错误类型**，只拿到错误描述的文本信息

为了不丢失函数调用的错误链，使用 `fmt.Errorf` 时搭配使用特殊的 **格式转换符 `%w`**，可以实现 **_基于已有的错误再包装得到一个新的错误_**。

```go
fmt.Errorf("查询数据库失败，err:%w", err)
```

对于这种二次包装的错误，`errors` 包中提供了以下三个方法

```go
func Unwrap(err error) error                 // 获得 err 包含下一层错误

func Is(err, target error) bool              // 判断 err 是否包含 target

func As(err error, target interface{}) bool  // 判断 err 是否为 target 类型
```

### 自定义异常

此外我们还可以自己定义结构体类型，实现 `error` 接口

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

`error` 返回的是一般性的错误，但是 `panic` 函数返回的是让程序崩溃的错误
-  也就是当遇到 **不可恢复的错误** 状态的时候，如 _数组访问越界、空指针引用_ 等， 这些运行时错误会引起 **`painc` 异常**
- **不应通过调用 `panic` 函数来报告普通的错误**，而应该只把它作为报告致命错误的一种方式
- 某些 **不应该发生的场景发生时**，我们 **就应该调用 `panic`**

### 从 `panic` 中 `recover`

一般而言，**当 `panic` 异常发生时，程序会中断运行**。随后， 程序崩溃并输出日志信息。日志信息包括 `panic value` 和 函数调用的堆栈跟踪信息 

> [!NOTE]
> 当然，如果直接调用内置的 `panic` 函数也会引发 `panic` 异常； `panic` 函数接受任何值作为参数

> [!WARNING] 注意
> 我们在实际的开发过程中并不会直接调用`panic`函数，但是当我们的**程序遇到致命错误时**， 系统 **会自动调用该函数来终止整个程序的运行**，也就是系统内置了 `panic` 函数

程序执行时 `panic` 异常一旦被引发就会导致程序崩溃。这当然不是我们愿意看到的， 因为谁也不能保证程序不会发生任何运行时错误。为了保证程序的健壮性，Go语言为我们提供了专用于 **“拦截”运行时 `panic` 的内建函数 `recover`**。 它可以是当前的程序从运行时 `panic` 的状态中恢复并重新获得流程控制权

`recover` 函数声明

```go
func recover() interfacea{}
```

> [!WARNING] 警告：使用 `recover()` 需要注意点
> - **`recover` 只有在 `defer` 调用的函数中有效**
> - **`defer` 一定要在可能引发 `panic` 的语句之前定义**

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
