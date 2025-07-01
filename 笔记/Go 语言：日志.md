# 日志

**日志** 是指系统所指定 **_对象的某些操作_** 和其 **_操作结果_** 按时间有序的集合。通常记录在一个文件中，称为 **Log 文件**(**日志文件**)

> [!tip] 
> 
> 通常，日志文件采用 `.log` 作为扩展名，日志文件的格式并没有任何要求
> 

日志文件为服务器、工作站、防火墙和应用软件等 IT 资源相关活动记录 **必要的**、**有价值** 的信息，这对系统监控、查询、报表和安全审计是十分重要的。日志文件中的记录可提供以下用途
+ **监控系统资源**
+ **审计用户行为**
+ 对可疑行为进行告普
+ 确定入侵行为的范围
+ 为恢复系统提供帮助
+ 生成调查报告
+ 为打击计算机犯罪提供证据来源

Go 语言标准包 `log` 提供了基础的日志处理能力，但是该包的功能非常简陋，通常需要借助第三方包提供的日志处理能力。下面我们会有限介绍标准包 `log`，然后介绍一个第三方日志处理包 `zerolog`

## 标准包 `log`

Go 标准包 `log` 提供了一个简单的日志记录功能。主要包含三种种日志输出函数。下表列出了这些常用的日志输出函数

| 输出            | 格式化输出          | 换行输出            | 描述                            |
| :------------ | :------------- | :-------------- | :---------------------------- |
| `log.Print()` | `log.Printf()` | `log.Println()` | 类似 `fmt.Print*`               |
| `log.Fatal()` | `log.Fatalf()` | `log.Fatalln()` | 相当于 `log.Print* + os.Exit(1)` |
| `log.Panic()` | `log.Panicf()` | `log.Panicln()` | 相当于 `log.Print* + panic()`    |

输出日志需要使用一个称为 **日志记录器(Logger)** 的对象。在标准包 `log` 中的定义如下

```go
type Logger struct {
	outMu sync.Mutex                  // 用于保护输出的互斥锁
	out   io.Writer                   // 日志输出的位置
	prefix    atomic.Pointer[string]  // 前缀，用于表示 logger 对象 
	flag      atomic.Int32            // logger 的标志
	isDiscard atomic.Bool
}
```

> [!tip] 
>
> 上表列出的函数会使用一个标准 Logger 对象
>

### 创建 Logger 对象

在标准包 `log` 中提供了一个函数 `log.New()` 用于创建 `Logger` 对象，该函数的签名如下

```go
func New(out io.Writer, prefix string, flag int) *Logger
```

> [!tip] 
> 
> 我们需要提供 $3$ 个参数
> + `out`：代表了日志输出的位置，它必须是一个实现了 `io.Writer` 的类型
> + `prefix`：每条日志输出的前缀
> + `flag`：一些标志，用于控制日志记录器的行为
> 

标准包 `log` 中规定了如下几个 `flag` 参数能够接受的值，它们可以使用 **位或** 进行组合

```go
const (
	Ldate         = 1 << iota     // 1 本地日期: 2009/01/23
	Ltime                         // 2 本地时间: 01:23:23
	Lmicroseconds                 // 4 微妙: 01:23:23.123123.
	Llongfile                     // 8 完整文件名和行号: /a/b/c/d.go:23
	Lshortfile                    // 16 文件基名： d.go:23. 覆盖 Llongfile
	LUTC                          // 32 使用UTC（GMT），而不是本地时区
	Lmsgprefix                    // 64 默认 prefix 放行首，把前缀 prefix 放到消息之前
	LstdFlags     = Ldate | Ltime // 标准 logger 的初始值
)
```

### Logger 对象的方法

Logger 对象的方法可以分为两类：**输出日志** 和 **配置Logger**。下表列出了 Logger 对象支持的方法

| 方法                            | 描述                                                 |
| :---------------------------- | :------------------------------------------------- |
| `logger.Print()`              | 记录一条普通的日志，类似于 `fmt.Print()`                        |
| `logger.Printf()`             | 记录一条普通的日志，类似于 `fmt.Println()`                      |
| `logger.Println()`            | 记录一条普通的日志，类似于 `fmt.Printf()`                       |
|                               |                                                    |
| `logger.Fatal()`              | 记录一条错误的日志，类似于 `logger.Print() + os.Exit(1)`        |
| `logger.Fatalf()`             | 记录一条错误的日志，类似于 `logger.Printf() + os.Exit(1)`       |
| `logger.Fatalln()`            | 记录一条错误的日志，类似于 `logger.Println() + os.Exit(1)`      |
|                               |                                                    |
| `logger.Panic()`              | 记录一条崩溃的日志，类似于 `logger.Print() + panic()`           |
| `logger.Panicf()`             | 记录一条崩溃的日志，类似于 `logger.Printf() + panic()`          |
| `logger.Panicln()`            | 记录一条崩溃的日志，类似于 `logger.Println() + panic()`         |
|                               |                                                    |
| `logger.Output(calldepth, s)` | 将消息 `s` 写入 Logger 的 `out` 成员，`calldepth` 用于恢复 `PC` |
|                               |                                                    |
| `logger.Flag()`               | 获取 Logger 对象的 `flag` 成员的值                          |
| `logger.SetFlags(flag)`       | 设置 Logger 对象的 `flag` 成员                            |
| `logger.Prefix()`             | 获取 Logger 对象的 `prefix` 成员的值                        |
| `logger.SetPrefix()`          | 设置 Logger 对象的 `prefix` 成员                          |
| `logger.Writer()`             | 获取 Logger 对象的 `out` 成员                             |

### 示例

下面的代码演示了 `log` 包的使用

```go
package main

import (
	"log"
	"os"
)

func main() {
	// 创建一个前缀为 main 的 Logger 对象。
	flag := log.Ldate | log.Ltime | log.Lshortfile
	var logger = log.New(os.Stderr, "main ", flag)
	logger.Printf("这是一条测试日志")
}
```

### 标准日志记录器

`log` 包提供了一个缺省的 Logger 对象，即 `std`。`std` 是小写的，包外不可见。`log` 包中提供了 `log.Default()` 方法，获取 `std` 给包外使用

```go
// 标准日志记录器
var std = New(os.Stderr, "", LstdFlags)

func Default() *Logger {
	return std
}
```

## zerolog

标准包 `log` 模块太简陋了，实际使用并不方便。日志记录通常应该有 **日志级别**。目前市面上有很多第三方包

+ [`logrus`](https://github.com/sirupsen/logrus) 有日志级别、Hook 机制、日志格式输出
+ [`zap`](https://github.com/uber-go/zap) 是 Uber 的开源高性能日志库
+ [`zerolog`](https://zerolog.io/) 更注重开发体验，高性能、有日志级别、链式API，_JSON 格式日志记录_，号称 **零内存分配**

想要在项目中使用 `zerolog`，就需要使用 `go get` 进行安装

```shell
go get -u github.com/rs/zerolog/log
```

### 快速入门

标准包 `log` 中，记录日志是由 **日志记录器(Logger)** 完成的。其实，`zerolog` 包也是如此。在 `zerolog/log` 包中，提供了一个 **全局日志记录器(Global Logger)**，该全局日志记录器创建的代码如下

```go title:zerolog/log/log.go.14
// Logger is the global logger.
var Logger = zerolog.New(os.Stderr).With().Timestamp().Logger()
```

这行代码在 `zerolog/log/log.go` 文件的第 $14$ 行处声明的

#### 简单日志记录

对于简单的日志记录，导入全局 logger 所在的包 `github.com/rs/zerolog/log`

```go
package main

import (
	"github.com/rs/zerolog"
	"github.com/rs/zerolog/log"
)

func main() {
	// 时间字段设置为时间戳
	zerolog.TimeFieldFormat = zerolog.TimeFormatUnix // 秒
	log.Print("hello world")
	// {"level":"debug","time":1751299300,"message":"hello world"}
}
```

> [!tip] 
> 
> 注意：`log.Print()` 使用全局 `Logger` 记录日志，它将日志写入 `os.Stderr` 中 
> 

#### 上下文记录

`zerolog` 允许将数据以 `key: value` 的形式添加到日志消息中。这些数据添加了有关日志事件的 **上下文**，这对于 **调试** 以及无数其他目的可能至关重要。下面是一个例子：

```go
package main

import (
	"github.com/rs/zerolog"
	"github.com/rs/zerolog/log"
)

func main() {
	// 时间字段设置为时间戳
	zerolog.TimeFieldFormat = zerolog.TimeFormatUnix // 秒
	log.Debug().
		Str("Scale", "833 cents").
		Float64("Interval", 833.09).
		Msg("Fibonacci is everwhere")
	// {"level":"debug","Scale":"833 cents","Interval":833.09,"time":1751299633,"message":"Fibonacci is everwhere"}
	
	log.Debug().
		Str("Name", "张三").
		Send()
	// {"level":"debug","Name":"张三","time":1751299633}
}
```


> [!tip] 
> 
> 在添加上下文字段时，这些字段是 **强类型** 化的。使用了与 Go 语言基本类型同名的函数添加上下文字段
>
> - `Str()` 字符串
> - `Bool()` 布尔类型
> - `Int`, `Int8`, `Int16`, `Int32`, `Int64`  有符号整数
> - `Uint`, `Uint8`, `Uint16`, `Uint32`, `Uint64`  无符号整数
> - `Float32`, `Float64`  浮点数
> 

#### 日志级别


> [!attention] 
> 
> 标准包 `log` 中 `log.Print()` 系列的函数输出程序的执行行为，这本质上就是一种日志。然而，这样记录的消息有一个缺点：**消息没有等级**。
> 

日志是用来记录程序在 **运行过程中发生的状况**，在程序开发过程中添加日志能够帮助我们了解程序运行过程中发生了哪些状况。请注意，这些 **状况也有轻重之分**。

`zerolog` 定义了 $7$ 个日志等级。下表列出了这些日志级别

| 级别      | 值(常量=>数值)                    | 描述                              |
| ------- | ---------------------------- | ------------------------------- |
| `panic` | `zerolog.PanicLevel` => $5$  | **恐慌**。非常严重的错误，导致 **程序崩溃**      |
| `fatal` | `zerolog.FatalLevel` =>$4$   | **致命**。严重的错误，导致 **程序不能运行**      |
| `error` | `zerolog.ErrorLevel`=> $3$   | **错误**。由于某些原因，导致 **程序不能执行某些功能** |
| `warn`  | `zerolog.WarnLevel`=> $2$    | **警告**。出现了异常，但是不影响正常工作          |
| `info`  | `zerolog.InfoLevel` => $1$   | **消息**。确认程序按预期工作                |
| `debug` | `zerolog.DebugLevel` => $0$  | **调试**。记录详细信息，通常仅在诊断问题时才受到关注    |
| `trace` | `zerolog.TraceLevel` => $-1$ | **跟踪**。默认的日志级别                  |

这些常量定义在 `zerolog/log.go` 中

```go
// Level 定义了日志级别
type Level int8

const (
	// DebugLevel DEBUG 调试日志。记录详细信息，通常仅在诊断问题时才受到关注
	DebugLevel Level = iota
	// InfoLevel INFO 消息。确认程序按预期工作
	InfoLevel
	// WarnLevel WARNING 警告。出现了异常，但是不影响正常工作
	WarnLevel
	// ErrorLevel ERROR 错误。由于某些原因，程序 不能执行某些功能
	ErrorLevel
	// FatalLevel FATAL 致命。严重的错误，导致程序不能运行
	FatalLevel
	// PanicLevel PANIC 恐慌。非常严重的错误，导致程序崩溃
	PanicLevel
	
	// NoLevel 不存在的日志级别
	NoLevel
	// Disabled 禁用日志记录器.
	Disabled

	// TraceLevel TRANCE 跟踪。 最低的日志级别
	TraceLevel Level = -1
	// 小于 TraceLevel 的值作为数字处理
)
```

想要创建 `InfoLevel` 的日志，可以使用 `log.Info()` 函数。如下示例代码演示了如何创建 `InfoLevel` 的日志

```go
package main

import (
	"github.com/rs/zerolog"
	"github.com/rs/zerolog/log"
)

func main() {
	// 时间字段设置为时间戳
	zerolog.TimeFieldFormat = zerolog.TimeFormatUnix // 秒
	log.Info().Msg("hello, world")
	// {"level":"info","time":1751378260,"message":"hello, world"}
}
```

> [!warning] 
> 
> 在 `zerolog` 的链式 API 调用中 **必须** 有 `Msg()` 或者 `Msgf()` 方法的调用。否则，则日志将不会被记录，并且也不会出现编译时错误来提醒您
> 

##### 设置全局日志级别

`zerolog.SetGlobalLevel(level)` 将设置 **全局日志级别**，`zerolog.GlobalLevel()` 获取全局日志级别

下面的实例演示了如何设置日志级别

```go
package main

import (
	"flag"

	"github.com/rs/zerolog"
	"github.com/rs/zerolog/log"
)

func main() {

	// 设置标记
	debug := flag.Bool("debug", false, "set log level to debug")

	// 解析标记
	flag.Parse()

	// 时间字段设置为时间戳
	zerolog.TimeFieldFormat = zerolog.TimeFormatUnix // 秒

	// 将全局日志记录等级设置 INFO
	zerolog.SetGlobalLevel(zerolog.InfoLevel)

	// 如果设置了 debug 标记，则将全局日志记录等级设置为 DEBUG
	if *debug {
		zerolog.SetGlobalLevel(zerolog.DebugLevel)
	}

	// Debug 级别的消息
	log.Debug().Msg("This message appears when log level set to Debug or Info")
	// Info 级别的消息
	log.Info().Msg("This message appears only when log level set to Debug")

	if e := log.Debug(); e.Enabled() {
		//  只有启用时才计算日志输出。
		value := "bar"
		e.Str("foo", value).Msg("some debug message")
	}
}
```

> [!important] 全局日志级别是控制所有记录器的最低级别
> 
> 也就是说，消息级别 `mLevel` 必须满足下面的关系这个消息才能输出
> $$
> \text{mLevel} \ge \max{(\text{gLevel}, \text{logger.Level})}
> $$
> + `mLevel` 表示消息级别
> + `gLevel` 表示全局日志级别
> + `logger.Level` 表示记录器的级别
> 
> 消息级别必须 **大于或等于** 全局级别和记录器级别的最大值时，消息才能被日志记录器输出

##### 无级别日志或消息

使用 `log.Log()` 方法可以选择 **不使用特定级别** 进行日志记录。也可以通过在`Msg()` 方法的 `msg string` 参数中设置一个空字符串来编写没有消息的内容。下面的示例演示了这两种方法

```go
package main

import (
	"github.com/rs/zerolog"
	"github.com/rs/zerolog/log"
)

func main() {

	// 时间字段设置为时间戳
	zerolog.TimeFieldFormat = zerolog.TimeFormatUnix // 秒
	
	// 无等级日志
	log.Log().
		Str("foo", "bar").
		Msg("")
}

// Output: {"foo":"bar","time":1721314722} 
// 没有 "level" 字段
```

#### 错误日志

##### ERROR 错误

可以使用 `Err()` 方法记录错误日志

```go

package main

import (
	"errors"

	"github.com/rs/zerolog"
	"github.com/rs/zerolog/log"
)

func main() {

	zerolog.TimeFieldFormat = zerolog.TimeFormatUnix

	err := errors.New("seems we have an error here")

	// zerolog 的链式 API 需要在链上调用 Msg 或者 Send 方法发送错误
	log.Error().Err(err).Msg("") // 消息为空
	log.Error().Err(err).Send()  // 发送错误
}

// Output: 
// {"level":"error","error":"seems we have an error here","time":1716885578} 
// {"level":"error","error":"seems we have an error here","time":1716885578}   
```

> [!tip] 注意 
> 
> 错误的默认字段名称是 `"error"`，可以通过修改 `zerolog.ErrorFieldName` 的值来修改错误日志字段名
> 

##### FATAL 错误

```go
package main

import (
	"errors"

	"github.com/rs/zerolog"
	"github.com/rs/zerolog/log"
)

func main() {

	// 时间字段设置为时间戳
	zerolog.TimeFieldFormat = zerolog.TimeFormatUnix // 秒

	err := errors.New("错误信息")
	service := "myservice"

	log.Fatal().Err(err).Str("service", service).Msgf("消息：%v\n", service)
}

// Output: {"level":"fatal","error":"错误信息","service":"myservice","time":1721345812,"message":"消息：myservice\n"}
// 		   exit status 1
```

> [!WARNING] 注意 
> 即使禁用记录器，使用 `Msgf` 也会生成一个分配

##### 错误日志堆栈信息

使用 `github.com/pkg/errors` ，可以将格式化的堆栈跟踪添加到错误中。

```go
package main

import (
	"github.com/pkg/errors"
	"github.com/rs/zerolog/pkgerrors"

	"github.com/rs/zerolog"
	"github.com/rs/zerolog/log"
)

func main() {
	zerolog.TimeFieldFormat = zerolog.TimeFormatUnix
	zerolog.ErrorStackMarshaler = pkgerrors.MarshalStack

	err := outer()
	log.Error().Stack().Err(err).Msg("")
}

func inner() error {
	return errors.New("seems we have an error here")
}

func middle() error {
	err := inner()
	if err != nil {
		return err
	}
	return nil
}

func outer() error {
	err := middle()
	if err != nil {
		return err
	}
	return nil
}


// Output: {"level":"error","stack":[{"func":"inner","line":"20","source":"main.go"},{"func":"middle","line":"24","source":"main.go"},{"func":"outer","line":"32","source":"main.go"},{"func":"main","line":"15","source":"main.go"},{"func":"main","line":"271","source":"proc.go"},{"func":"goexit","line":"1695","source":"asm_amd64.s"}],"error":"seems we have an error here","time":1721346403}
```

> [!attention] 
> 
> 必须设置 `zerolog.ErrorStackMarshaler` 才能让 `Error().Stack()` 输出任何内容
> 

#### 自定义 Logger

##### 创建 Logger

```go
package main

import (
	"os"

	"github.com/rs/zerolog"
)

func main() {
	zerolog.TimeFieldFormat = zerolog.TimeFormatUnix

	// 自定义一个 Logger
	logger := zerolog.New(os.Stderr).
		With().
		Timestamp().
		Logger()

	logger.Info().Str("foo", "bar").Msg("hello, world!")
}

// Output: {"level":"info","foo":"bar","time":1721346781,"message":"hello, world!"}
```

> [!tip] 
> 创建 Logger 实例来 **管理不同的输出**

##### 子 Logger

```go
package main

import (
	"os"

	"github.com/rs/zerolog"
)

func main() {
	zerolog.TimeFieldFormat = zerolog.TimeFormatUnix

	logger := zerolog.New(os.Stderr).
		With().
		Timestamp().
		Logger()

	sublogger := logger.With().Str("component", "foo").Logger()

	sublogger.Info().Msg("hello, world!")
}

// 额外的上下文
// Output: {"level":"info","component":"foo","time":1721347173,"message":"hello, world!"}
```

> [!tip] 
> 
> 子 Logger 可以链接具有 **额外上下文** 的记录器
> 

#### 美化日志

使用 `zerolog.ConsoleWriter` 可以让日志获得人性友好的彩色输出

```go
package main

import (
	"os"

	"github.com/rs/zerolog"
	"github.com/rs/zerolog/log"
)

func main() {
	zerolog.TimeFieldFormat = zerolog.TimeFormatUnix

	log.Logger = log.Output(zerolog.ConsoleWriter{Out: os.Stderr})

	log.Info().Str("foo", "bar").Msg("hello, world!")
	log.Warn().Str("foo", "bar").Msg("hello, world!")
	log.Error().Str("foo", "bar").Msg("hello, world!")
	log.Fatal().Str("foo", "bar").Msg("hello, world!")
	// log.Panic().Str("foo", "bar").Msg("hello, world!")
}

// Output:
// 8:09AM INF hello, world! foo=bar
// 8:09AM WRN hello, world! foo=bar
// 8:09AM ERR hello, world! foo=bar
// 8:09AM FTL hello, world! foo=bar

```

##### 自定义配置和格式

```go
output := zerolog.ConsoleWriter{Out: os.Stdout, TimeFormat: time.RFC3339}

// 级别格式
output.FormatLevel = func(i interface{}) string {
    return strings.ToUpper(fmt.Sprintf("| %-6s|", i))
}

// 消息格式
output.FormatMessage = func(i interface{}) string {
    return fmt.Sprintf("***%s****", i)
}

// 字段名格式
output.FormatFieldName = func(i interface{}) string {
    return fmt.Sprintf("%s:", i)
}

// 字段值格式
output.FormatFieldValue = func(i interface{}) string {
    return strings.ToUpper(fmt.Sprintf("%s", i))
}

// 创建记录器
log := zerolog.New(output).With().Timestamp().Logger()

log.Info().Str("foo", "bar").Msg("Hello World")

// Output: 2006-01-02T15:04:05Z07:00 | INFO  | ***Hello World**** foo:BAR
```

#### 子词典

`zerolog` 是采用 JSON 格式输出日志的，为了在日志中携带子日志，可以使用 `Dict()` 方法

```go
package main

import (
	"github.com/rs/zerolog"
	"github.com/rs/zerolog/log"
)

func main() {
	zerolog.TimeFieldFormat = zerolog.TimeFormatUnix
	log.Info().
		Str("foo", "bar").
		Dict("dict", zerolog.Dict().
			Str("bar", "baz").
			Int("n", 1),
		).Msg("hello world")
}
```

上述代码的输出结果为

```json
{
	"level":"info",
	"foo":"bar",
	// 子字典
	"dict":{
		"bar":"baz",
		"n":1
	},
	"time":1721349008,
	"message":"helloworld"
}
```

#### 自定义自动字段名称

`zerolog` 提供了一个默认的字段，这些字段的名字可以由我们自定义

```go
package main

import (
	"github.com/rs/zerolog"
	"github.com/rs/zerolog/log"
)

func main() {
	zerolog.TimeFieldFormat = zerolog.TimeFormatUnix

	zerolog.TimestampFieldName = "t" // 时间字段
	zerolog.LevelFieldName = "l"     // 等级字段
	zerolog.MessageFieldName = "m"   // 消息字段

	log.Info().
		Str("foo", "bar").
		Dict("dict", zerolog.Dict().
			Str("bar", "baz").
			Int("n", 1),
		).Msg("hello world")
}
```

上述代码的输出如下

```json
{
	"l":"info",  // level
	"foo":"bar",
	"dict":{
		"bar":"baz",
		"n":1},
	"t":1721349185, // time
	"m":"hello world" // message
}
```

#### 添加文件名+行号：`Caller`

等价于 标准包 `log` 中的 `log.Llongfile`

```go
package main

import (
	"github.com/rs/zerolog"
	"github.com/rs/zerolog/log"
)

func main() {
	zerolog.TimeFieldFormat = zerolog.TimeFormatUnix

	// 添加调用位置：文件名+行号
	log.Logger = log.With().Caller().Logger()
	
	log.Info().Msg("hello world")
}

// Output:
// {"level":"info","time":1721349486,"caller":"/home/dyp/Documents/github.com/gocode/main.go:14","message":"hello world"}
```

等价于 标准包 `log` 中的 `log.Lshortfile`

```go
package main

import (
	"path/filepath"
	"strconv"

	"github.com/rs/zerolog"
	"github.com/rs/zerolog/log"
)

func main() {
	zerolog.TimeFieldFormat = zerolog.TimeFormatUnix

	zerolog.CallerMarshalFunc = func(pc uintptr, file string, line int) string {
		return filepath.Base(file) + ":" + strconv.Itoa(line)
	}

	log.Logger = log.With().Caller().Logger()

	log.Info().Msg("hello world")
}

// Output:
// {"level":"info","time":1721349635,"caller":"main.go:22","message":"hello world"}
```

#### 线程安全-无锁-非阻塞 `Writer`

如果 `writer` 很慢或不安全，并且需要日志生成器永远不会因缓慢的写入器而减慢速度，则可以使用 `diode.Writer` 

```go
package main

import (
	"fmt"
	"os"
	"time"

	"github.com/rs/zerolog"
	"github.com/rs/zerolog/diode"
)

func main() {
	zerolog.TimeFieldFormat = zerolog.TimeFormatUnix

	// log.Logger = log.Output(zerolog.ConsoleWriter{Out: os.Stderr})

	out := diode.NewWriter(os.Stderr, 1000, 10*time.Millisecond, func(missed int) {
		fmt.Printf("Logger Dropped %d message", missed)
	})
	// NewWriter创建了一个用多对一二极管包装的写入器，以便在写入器无法跟上数据流时永远不会阻塞日志生成器并丢弃事件

	logger := zerolog.New(out).With().Timestamp().Logger()

	logger.Print("test")
}

// Output:
```

> [!tip] 
> 需要安装 `code.cloudfoundry.org/go-diodes` 才能使用此功能。

#### 日志采样

简单的日志采样，使用 `log.Sample(&zerolog.BasicSampler{})`

```go
package main

import (
	"github.com/rs/zerolog"
	"github.com/rs/zerolog/log"
)

func main() {
	zerolog.TimeFieldFormat = zerolog.TimeFormatUnix

	// 每 3 条消息记录一条消息（记录第 1 条）
	sampled := log.Sample(&zerolog.BasicSampler{N: 3})
	
	sampled.Info().Msg("1: will be logged every 3 messages")
	sampled.Info().Msg("2: will be logged every 3 messages")
	sampled.Info().Msg("3: will be logged every 3 messages")
	
	sampled.Info().Msg("4: will be logged every 3 messages")
	sampled.Info().Msg("5: will be logged every 3 messages")
	sampled.Info().Msg("6: will be logged every 3 messages")

}

// Output:
// {"level":"info","time":1721387173,"message":"1: will be logged every 3 messages"}
// {"level":"info","time":1721387173,"message":"4: will be logged every 3 messages"}
```

高级的日志采样

```go
package main

import (
	"time"

	"github.com/rs/zerolog"
	"github.com/rs/zerolog/log"
)

func main() {
	zerolog.TimeFieldFormat = zerolog.TimeFormatUnix

	// 每 1 秒将发送 5 条 DEBUG 消息。
	// 超过 5 个 DEBUG 消息，每 100 个 DEBUG 消息中就有 1 个被记录。
	// 其他级别全部输出，不进行采样,
	sampled := log.Sample(&zerolog.LevelSampler{
		DebugSampler: &zerolog.BurstSampler{
			Burst:       5,
			Period:      1 * time.Second,
			NextSampler: &zerolog.BasicSampler{N: 100},
		},
	})

	sampled.Debug().Msg("1: will be logged every 3 messages")
	sampled.Debug().Msg("2: will be logged every 3 messages")
	sampled.Debug().Msg("3: will be logged every 3 messages")
	sampled.Debug().Msg("4: will be logged every 3 messages")
	sampled.Debug().Msg("5: will be logged every 3 messages")
	sampled.Debug().Msg("6: will be logged every 3 messages")
	sampled.Info().Msg("1: will be logged every 3 messages")
	sampled.Debug().Msg("2: will be logged every 3 messages")
	sampled.Debug().Msg("3: will be logged every 3 messages")
	sampled.Debug().Msg("4: will be logged every 3 messages")
	sampled.Debug().Msg("5: will be logged every 3 messages")
	sampled.Info().Msg("6: will be logged every 3 messages")

}
```

#### Hooks

```go
package main

import (
	"github.com/rs/zerolog"
	"github.com/rs/zerolog/log"
)

type SeverityHook struct{}

func (h SeverityHook) Run(e *zerolog.Event, level zerolog.Level, msg string) {
	if level != zerolog.NoLevel {
		e.Str("severity", level.String())
	}
}

func main() {
	zerolog.TimeFieldFormat = zerolog.TimeFormatUnix

	hooked := log.Hook(SeverityHook{})
	hooked.Warn().Msg("")
}

// Output:
// {"level":"warn","time":1721388523,"severity":"warn"}
```

#### 通过 `context` 传递子记录器

```go
package main

import (
	"context"

	"github.com/rs/zerolog"
	"github.com/rs/zerolog/log"
)

func main() {
	zerolog.TimeFieldFormat = zerolog.TimeFormatUnix

	ctx := log.With().Str("component", "module").Logger().WithContext(context.TODO())

	log.Ctx(ctx).Info().Msg("hello world")
}

// Output:
// {"level":"info","component":"module","time":1721388784,"message":"hello world"}

```

#### `context.Context` 集成

Go `context` 通常在 Go 代码中传递，这可以帮助我们将 Logger 传递到可能难以注入的地方。Logger 实例可以使用 `Logger.WithContext(ctx)` 附加到 Go `context.Context`，并使用 `zerolog.Ctx(ctx)` 从中提取。例如:

```go
package main

import (
	"context"
	"os"

	"github.com/rs/zerolog"
)

func f() {
	logger := zerolog.New(os.Stdout)
	ctx := context.Background()

	// 将记录器连接 context.Context
	ctx = logger.WithContext(ctx)
	someFunc(ctx)
}

func someFunc(ctx context.Context) {
	// 从 go 上下文获取 Logger。
	// 如果它是 nil，那么 zerlog.DefaultContextLogger 返回
	// 如果 DefaultContextLogger 为 nil，则返回一个 DISABLE 级别的 logger
	logger := zerolog.Ctx(ctx)
	logger.Info().Msg("Hello")
}

func main() {
	zerolog.TimeFieldFormat = zerolog.TimeFormatUnix

	f()

}

// Output:
// {"level":"info","message":"Hello"}
```

`context.Context` 集成的第二种形式允许传递当前 `context.Context` 到记录的事件中，并 **从钩子中检索它**。这对于记录 **Trace ID** 和 **Span ID** 或存储在 Go `context` 中的 **其他信息** 非常有用，并且有助于在某些系统中统一记录和跟踪:

```go
type TracingHook struct{}

func (h TracingHook) Run(e *zerolog.Event, level zerolog.Level, msg string) {
    ctx := e.GetCtx()
    spanId := getSpanIdFromContext(ctx) // as per your tracing framework
    e.Str("span-id", spanId)
}

func f() {
    // Setup the logger
    logger := zerolog.New(os.Stdout)
    logger = logger.Hook(TracingHook{})

    ctx := context.Background()
    // Use the Ctx function to make the context available to the hook
    logger.Info().Ctx(ctx).Msg("Hello")
}
```

#### 与 `net/http` 集成

`zerolog/hlog` 包提供了一些帮助程序将 Zerolog 与 `http.Handler` 集成。在本例中，我们使用 [`alice`](https://github.com/justinas/alice) 安装记录器以获得更好的可读性

```go
log := zerolog.New(os.Stdout).With().
    Timestamp().
    Str("role", "my-service").
    Str("host", host).
    Logger()

c := alice.New()

// 在控制台上安装带有默认输出的日志处理程序
c = c.Append(hlog.NewHandler(log))

// 安装一些提供的额外处理程序来设置一些请求的上下文字段。
// 由于该处理程序，我们所有的日志都将包含一些预填充字段。
c = c.Append(hlog.AccessHandler(func(r *http.Request, status, size int, duration time.Duration) {
    hlog.FromRequest(r).Info().
        Str("method", r.Method).
        Stringer("url", r.URL).
        Int("status", status).
        Int("size", size).
        Dur("duration", duration).
        Msg("")
}))
c = c.Append(hlog.RemoteAddrHandler("ip"))
c = c.Append(hlog.UserAgentHandler("user_agent"))
c = c.Append(hlog.RefererHandler("referer"))
c = c.Append(hlog.RequestIDHandler("req_id", "Request-Id"))

// 这是您的最终 处理器
h := c.Then(http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
    // 从请求的上下文获取记录器。
    //您可以放心地假设它将始终存在：
    // 如果处理程序被删除，hlog.FromResource 将返回无操作记录器。
    hlog.FromRequest(r).Info().
        Str("user", "current user").
        Str("status", "ok").
        Msg("Something happened")

    // Output: {"level":"info","time":"2001-02-03T04:05:06Z","role":"my-service","host":"local-hostname","req_id":"b4g0l5t6tfid6dtrapu0","user":"current user","status":"ok","message":"Something happened"}
}))
http.Handle("/", h)

if err := http.ListenAndServe(":8080", nil); err != nil {
    log.Fatal().Err(err).Msg("Startup failed")
}
```


### 多日志输出

`zerolog.MultiLevelWriter` 可以用来 **将日志消息发送到多个输出**。在这个示例中，我们将日志消息同时发送到 `os.Stdout` 和内置的 `ConsoleWriter` 中

```go
func main() {
	consoleWriter := zerolog.ConsoleWriter{Out: os.Stdout}

	multi := zerolog.MultiLevelWriter(consoleWriter, os.Stdout)

	logger := zerolog.New(multi).With().Timestamp().Logger()

	logger.Info().Msg("Hello World!")
}

// Output (Line 1: Console; Line 2: Stdout)
// 12:36PM INF Hello World!
// {"level":"info","time":"2019-11-07T12:36:38+03:00","message":"Hello World!"}
```

### 全局配置

某些配置可以更改并 **应用于所有记录器**：

#### 全局日志记录器

 `log.Logger`：修改此值来自定义全局记录器（包级方法使用的记录器）

#### 全局日志级别

 `zerolog.SetGlobalLevel`：可以提高所有记录器的最低级别。调用此命令`zerolog.Disabled`可完全禁用记录（安静模式）

#### 禁用采样 Logger

`zerolog.DisableSampling`：如果参数是`true`，所有采样记录器将停止采样并发出 `100％` 的日志事件

#### 自定义自动字段名

 `zerolog.TimestampFieldName`：可设置自定义`Timestamp`字段名称。

`zerolog.LevelFieldName`：可设置自定义级别字段名称。

`zerolog.MessageFieldName`：可设置自定义消息字段名称。

`zerolog.ErrorFieldName`：可设置自定义`Err`字段名称。

#### 格式化相关

`zerolog.TimeFieldFormat`：可设置自定义`Time`字段值格式。如果设置了`zerolog.TimeFormatUnix`、`zerolog.TimeFormatUnixMs`或`zerolog.TimeFormatUnixMicro`，时间将被格式化为 UNIX 时间戳

`zerolog.DurationFieldUnit`：可设置自定义时间的单位。由持续时间类型字段添加 `Dur`（默认值`time.Millisecond`）

`zerolog.DurationFieldInteger`：如果设置为 `true`，`Dur` 字段将被格式化为整数而不是浮点数（默认值`false`）

`zerolog.FloatingPointPrecision`：如果设置为 `-1` 以外的值，则控制在 JSON 中格式化浮点数时的位数

#### 错误处理器

`zerolog.ErrorHandler`：每当 `zerolog` 无法在其输出中写入事件时调用。如果未设置，则会在 `os.Stderr` 上打印错误。此处理程序必须是线程安全的且非阻塞的

### 二进制编码

除了默认的 JSON 编码，`zerolog` 还可以使用 [`CBOR`](https://cbor.io/) 编码生成 **二进制日志**。编码的选择可以 **在编译时** 使用构建标签 `binary_log` 决定，如下所示:

```shell
go build -tags binary_log .
```

要 **解码二进制编码的日志文件**，可以使用任何 `CBOR` 解码器。其中一个已经过测试的与 `zerolog` 一起工作是 [`CSD`](https://github.com/toravir/csd/)。

