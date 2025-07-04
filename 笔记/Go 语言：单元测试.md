# 单元测试

程序写好就能正常运行，那是运气。要保证每个环节 **可靠**，**可测试**，**可验证**。工程的复杂度上来后, 影响的程序正常运行的因数很多, 需要本地调试，才能找到原因

> [!tip] 程序员的关键技能: debug
> 
> 直接开发功能(会遇到一些非常奇怪的问题)
>   
>  接手别人项目(需要关键步骤上进行 debug)
>  

> [!tip] debug 的阶段:
> 
> 开发阶段(**单元测试** 的debug)
> 
> **整体调试**/接口调试(需要启动程序进行 debug)
> 
> 线上调试(不建议, remote)

我们说测试的时候一般是指 **自动化测试**，也就是写一些小的程序用来 **检测被测试代码（产品代码）的行为和预期的一样**，这些通常都是 **精心设计的执行某些特定的功能** 或者是通过随机性的输入 **验证边界** 的处理

**Go 语言的测试技术是相对低级的**。它依赖一个 **go test 测试命令** 和一组 **按照约定** 方式编写的测试函数，测试命令可以运行这些测试函数。编写相对轻量级的纯测试代码是有效的，而且它很容易延伸到基准测试和示例文档

> [!tip] 
> 
> **go test 命令** 是一个按照一定约定和组织的测试代码的 **驱动程序**。在包目录内，**所有以 `_test.go` 为后缀名的源代码文件都是 go test 测试的一部分**，不会被 `go build` 编译到最终的可执行文件中
> 

在 `*_test.go` 文件中有三种类型的函数，**单元测试函数**、**基准测试函数** 和 **示例函数** 

| 类型       | 格式                 | 作用              |
| -------- | ------------------ | --------------- |
| **测试函数** | 函数名前缀为 `Test`      | 测试程序的一些逻辑行为是否正确 |
| **基准函数** | 函数名前缀为 `Benchmark` | 测试函数的性能         |
| **示例函数** | 函数名前缀为 `Example`   | 为文档提供示例文档       |

`go test` 命令会遍历所有的 `*_test.go` 文件中符合上述命名规则的函数，然后生成一个临时的 `main` 包用于调用相应的测试函数，然后构建并运行、报告测试结果，最后清理测试中生成的临时文件

> [!tip] Go 测试文件的格式 `*_test.go`  
> 
> 每个测试文件文件中，必须导入 `testing` 包

## 测试函数

测试函数的格式如下

```go
func TestXxxx(t *testing.T) {
	// 测试代码
}
```

> [!tip]
>  测试函数的名字 **必须以 `Test`开头**，可选的 **后缀名必须以大写字母开头**

测试函数的参数 `t` 用于报告 **测试失败** 和 **附加的日志** 信息。 `testing.T` 的拥有的方法如下：

```go
// Error 相当于 Log 后跟 Fail
func (c *T) Error(args ...interface{})

func (c *T) Errorf(format string, args ...interface{})

// Fail 将函数标记为失败但继续执行。
func (c *T) Fail()

// FailNow 将函数标记为失败，并通过调用 runtime.Goexit（然后运行当前 goroutine 中的所有延迟调用）停止其执行。执行将在下一个测试或基准测试中继续。 FailNow 必须从运行测试或基准函数的 goroutine 中调用，而不是从测试期间创建的其他 goroutine 中调用。调用 FailNow 不会停止其他 goroutine
func (c *T) FailNow()

// Failed 报告该功能是否失败
func (c *T) Failed() bool

// Fatal 相当于 Log 后跟 FailNow
func (c *T) Fatal(args ...interface{})
func (c *T) Fatalf(format string, args ...interface{})

// Log 使用默认格式（类似于 Println）来格式化其参数，并将文本记录在错误日志中
// 对于测试，仅当测试失败或设置了 -test.v 标志时才会打印文本
// 对于基准测试，始终打印文本以避免性能依赖于 -test.v 标志的值。
func (c *T) Log(args ...interface{})
func (c *T) Logf(format string, args ...interface{})

// Name 返回正在运行的（子）测试或基准测试的名称
func (c *T) Name() string

// Parallel 表明此测试将与（且仅与）其他并行测试并行运行。当由于使用 -test.count 或 -test.cpu 多次运行测试时，单个测试的多个实例永远不会彼此并行运行。
func (t *T) Parallel()

// Run 将 f 作为名为 name 的 t 子测试来运行。它在单独的 goroutine 中运行 f 并阻塞，直到 f 返回或调用 t.Parallel 成为并行测试
// Run 报告 f 是否成功（或者至少在调用 t.Parallel 之前没有失败）
func (t *T) Run(name string, f func(t *T)) bool

// Skip 相当于 Log 后跟 SkipNow
func (c *T) Skip(args ...interface{})

// SkipNow 将测试标记为已跳过，并通过调用runtime.Goexit停止其执行
func (c *T) SkipNow()

// Skipf 相当于 Logf 后跟 SkipNow
func (c *T) Skipf(format string, args ...interface{})

// Skipped 报告测试是否被跳过
func (c *T) Skipped() bool
```

### 示例

就像细胞是构成我们身体的基本单位，一个软件程序也是由很多单元组件构成的。单元组件可以是函数、结构体、方法和最终用户可能依赖的任意东西。总之我们需要确保这些组件是能够正常运行的。**单元测试是一些利用各种方法测试单元组件的程序**，它会 **将结果与预期输出进行比较**。

下面是 `debug/split/split.go` 中的内容

```go
package split  
  
import (  
    "strings"  
)  
  
// Split 将 s 在 sep 处切分为一个 []string
func Split(s string, sep string) (result []string) {  
    // s: "a:b:c"  
    // sep: ":"  
    i := strings.Index(s, sep) // 1  
  
    for i > -1 {  
  
       result = append(result, s[:i])  
       s = s[i+1:]  
       i = strings.Index(s, sep)  
    }  
  
    result = append(result, s)  
  
    return result  
}
```

为了测试 `Split` 函数，我们在相同的目录下的 `split_test.go` 文件中的内容

```go
package split_test

import (
	"debug/split"
	"reflect"
	"testing"
)

func TestSplit(t *testing.T) {
	got := split.Split("a:b:c", ":")
	want := []string{"a", "b", "c"}
	if !reflect.DeepEqual(got, want) {
		t.Errorf("split.Split => got %v want %v", got, want)
	}
}
```

在 `split` 包路径下，执行 `go test` 命令，可以看到输出结果如下

```powershell
@duyup ➜ split go test       
PASS
ok      debug/split     1.599s
```

我们再编写一个测试使用多个字符切割字符串的例子，在`split_test.go`中添加如下测试函数

```go
func TestMoreSepSplit(t *testing.T) {
	got := split.Split("abcd", "bc")
	want := []string{"a", "d"}
	if !reflect.DeepEqual(got, want) {
		t.Errorf("split.Split => got %v want %v", got, want)
	}
}
```

再次执行 `go test`

```powershell
@duyup ➜ split go test
--- FAIL: TestMoreSepSplit (0.00s)
    split_test.go:21: split.Split => got [a cd] want [a d]
FAIL
exit status 1
FAIL    debug/split     1.431s
```

这一次，我们的测试失败了

> [!tip] 
> 
> `go test -v`  查看测试函数名称和运行时间 
> 

这次我们执行 `go test -v`

```powershell
@duyup ➜ split go test -v               
=== RUN   TestSplit
--- PASS: TestSplit (0.00s)
=== RUN   TestMoreSepSplit
    split_test.go:21: split.Split => got [a cd] want [a d]
--- FAIL: TestMoreSepSplit (0.00s)
FAIL
exit status 1
FAIL    debug/split     0.371s
```

这一次我们能清楚的看到是 `TestMoreSplit` 这个测试没有成功

> [!tip] 
> 
> `go test -run=正则表达式`  测试函数名匹配 _正则表达式_ 就执行该测试函数
> 

```powershell
@duyup ➜ split go test -v -run="MoreSep"
=== RUN   TestMoreSepSplit
    split_test.go:21: split.Split => got [a cd] want [a d]
--- FAIL: TestMoreSepSplit (0.00s)
FAIL
exit status 1
FAIL    debug/split     0.350s
```

现在我们回过头来解决我们程序中的问题。很显然我们最初的 `Split` 函数并 **没有考虑到 `sep` 为多个字符的情况**，我们来修复下这个 `Bug`

```go
package split

import (
	"strings"
)

// Split 将 s 在 sep 处切分为一个 []string
func Split(s string, sep string) (result []string) {
	// s: "a:b:c"
	// sep: ":"

	i := strings.Index(s, sep) // 1

	for i > -1 {

		result = append(result, s[:i])
		s = s[i+len(sep):]
		i = strings.Index(s, sep)
	}

	result = append(result, s)

	return result
}
```

再次运行所有测试函数

```powershell
@duyup ➜ split go test -v
=== RUN   TestSplit
--- PASS: TestSplit (0.00s)
=== RUN   TestMoreSepSplit
--- PASS: TestMoreSepSplit (0.00s)
PASS
ok      debug/split     0.358s
```

我们通过了测试

### 测试组

我们现在还想要测试一下 `split` 函数对中文字符串的支持，这个时候我们可以再编写一个 `TestChineseSplit` 测试函数，但是我们也可以使用如下更友好的一种方式来添加更多的测试用例

> [!tip] 
> 
> 测试组：把多个测试用例合到一起
> 

```go
func TestSplit(t *testing.T) {
	type TestCase struct {
		input string
		sep   string
		want  []string
	}

	// 测试组
	tests := []TestCase{
		{input: "a:b:c", sep: ":", want: []string{"a", "b", "c"}},
		{input: "a:b:c", sep: ",", want: []string{"a:b:c"}},
		{input: "abcd", sep: "bc", want: []string{"a", "d"}},
		{input: "沙河有沙又有河", sep: "沙", want: []string{"河有", "又有河"}},
	}

	// 遍历测试用例
	for _, test := range tests {
		got := split.Split(test.input, test.sep)
		if !reflect.DeepEqual(got, test.want) {
			t.Errorf("split.Split => got %#v want %#v", got, test.want)
		}
	}
}
```

我们通过上面的代码把多个测试用例合到一起，再次执行 `go test` 命令

```powershell
@duyup ➜ split go test -v
=== RUN   TestSplit
    split_test.go:27: split.Split => got []string{"", "河有", "又有河"} want []string{"河有", "又有河"}
--- FAIL: TestSplit (0.00s)
FAIL
exit status 1
FAIL    debug/split     1.127s
```

### 子测试

看起来都挺不错的，但是 **如果测试用例比较多的时候，我们是没办法一眼看出来具体是哪个测试用例失败了**。Go 1.7+ 中新增了 **子测试**，我们可以按照如下方式 **使用 `t.Run` 执行子测试**

> [!tip] 
> 
> 子测试：Go >= 1.7 中可以使用 `t.Run(name, fn)` 执行子测试 
> 

```go
func TestSplit(t *testing.T) {
	type TestCase struct {
		input string
		sep   string
		want  []string
	}

	tests := map[string]TestCase{
		"simple":    {input: "a:b:c", sep: ":", want: []string{"a", "b", "c"}},
		"wrong sep": {input: "a:b:c", sep: ",", want: []string{"a:b:c"}},
		"more sep":  {input: "abcd", sep: "bc", want: []string{"a", "d"}},
		"chinese":   {input: "沙河有沙又有河", sep: "沙", want: []string{"河有", "又有河"}},
	}

	// 遍历测试用例
	for name, test := range tests {
		// 执行子测试
		t.Run(name, func(t *testing.T) {
			got := split.Split(test.input, test.sep)
			if !reflect.DeepEqual(got, test.want) {
				t.Errorf("want: %#v, got: %#v", test.want, got)
			}
		})
	}
}
```

此时我们再执行 `go test` 命令就能够看到更清晰的输出内容了：

```powershell
@duyup ➜ split  go test -v
=== RUN   TestSplit
=== RUN   TestSplit/simple
=== RUN   TestSplit/wrong_sep
=== RUN   TestSplit/more_sep
=== RUN   TestSplit/chinese
    split_test.go:29: want: []string{"河有", "又有河"}, got: []string{"", "河有", "又有河"}
--- FAIL: TestSplit (0.00s)
    --- PASS: TestSplit/simple (0.00s)
    --- PASS: TestSplit/wrong_sep (0.00s)
    --- PASS: TestSplit/more_sep (0.00s)
    --- FAIL: TestSplit/chinese (0.00s)
FAIL
exit status 1
FAIL    debug/split     1.193s
```

这个时候我们要把测试用例中的错误修改回来

```powershell
@duyup ➜ split  go test -v
=== RUN   TestSplit
=== RUN   TestSplit/simple
=== RUN   TestSplit/wrong_sep
=== RUN   TestSplit/more_sep
=== RUN   TestSplit/chinese
--- PASS: TestSplit (0.00s)
    --- PASS: TestSplit/simple (0.00s)
    --- PASS: TestSplit/wrong_sep (0.00s)
    --- PASS: TestSplit/more_sep (0.00s)
    --- PASS: TestSplit/chinese (0.00s)
PASS
ok      debug/split     1.063s
```

> [!tip] 
> 
> `go test -run=RegEXP` 也可以选中子测试
> 

```powershell
@duyup ➜ split go test -v -run="Split/chinese"
=== RUN   TestSplit
=== RUN   TestSplit/chinese
--- PASS: TestSplit (0.00s)
    --- PASS: TestSplit/chinese (0.00s)
PASS
ok      debug/split     0.345s
```

### 测试覆盖率

**测试覆盖率是你的代码被测试套件覆盖的百分比**。通常我们使用的都是 **语句的覆盖率**，也就是在测试中至少被运行一次的代码占总代码的比例。

> [!tip]
>  Go 提供内置功能来检查你的代码覆盖率。我们可以使用 **`go test -cover`来查看测试覆盖率**

```go
@duyup ➜ split go test -v -cover              
=== RUN   TestSplit
=== RUN   TestSplit/simple
=== RUN   TestSplit/wrong_sep
=== RUN   TestSplit/more_sep
=== RUN   TestSplit/chinese
--- PASS: TestSplit (0.00s)
    --- PASS: TestSplit/simple (0.00s)
    --- PASS: TestSplit/wrong_sep (0.00s)
    --- PASS: TestSplit/more_sep (0.00s)
    --- PASS: TestSplit/chinese (0.00s)
PASS
coverage: 100.0% of statements
ok      debug/split     1.099s
```

## 基准函数

> [!tip] 
> 
> 基准测试：就是在一定的工作负载之下 **检测程序性能的一种方法**
> 

基准测试函数的基本格式如下：

```go
func BenchmarkName(b *testing.B){
    // ...
}
```

> [!tip] 
> 
> 基准测试以 `Benchmark`为前缀，需要一个`*testing.B` 类型的参数 `b`，**基准测试必须要执行 `b.N` 次**，这样的测试才有对照性，`b.N` 的值是系统根据实际情况去调整的，从而保证测试的稳定性
> 

`testing.B`拥有的方法如下

```go
func (c *B) Error(args ...interface{})
func (c *B) Errorf(format string, args ...interface{})
func (c *B) Fail()
func (c *B) FailNow()
func (c *B) Failed() bool
func (c *B) Fatal(args ...interface{})
func (c *B) Fatalf(format string, args ...interface{})
func (c *B) Log(args ...interface{})
func (c *B) Logf(format string, args ...interface{})
func (c *B) Name() string
func (b *B) ReportAllocs()
func (b *B) ResetTimer()
func (b *B) Run(name string, f func(b *B)) bool
func (b *B) RunParallel(body func(*PB))
func (b *B) SetBytes(n int64)
func (b *B) SetParallelism(p int)
func (c *B) Skip(args ...interface{})
func (c *B) SkipNow()
func (c *B) Skipf(format string, args ...interface{})
func (c *B) Skipped() bool
func (b *B) StartTimer()
func (b *B) StopTimer()
```

### 示例

我们为 `split` 包中的 `Split` 函数编写基准测试如下

```go
func BenchmarkSplit(b *testing.B) {  
    for i := 0; i < b.N; i++ {  
       split.Split("a:b:c", ",")  
    }  
}
```

> [!tip] 
> 
> 基准测试并不会默认执行，需要增加 `-bench` 参数
> 

通过执行 `go test -bench=Split`命令执行基准测试，输出结果如下：

```powershell hl:6
@duyup ➜ split go test -bench=Split
goos: windows
goarch: amd64
pkg: debug/split
cpu: 13th Gen Intel(R) Core(TM) i5-13500H
BenchmarkSplit-16       52076778                22.83 ns/op
PASS
ok      debug/split     2.771s
```

其中 `BenchmarkSplit-16` 表示对  `Split` 函数进行基准测试，数字 `16` 表示 `GOMAXPROCS` 的值，这个对于并发基准测试很重要。`52076778` 和 `22.83 ns/op` 表示每次调用 `Split` 函数耗时 `22.83ns`，这个结果是 `52076778` 次调用的平均值。

> [!tip] 
> 
> 参数 `-benchmem`  分析内存分配的统计数据
> 

```powershell hl:6
@duyup ➜ split  go test -bench=Split -benchmem
goos: windows
goarch: amd64
pkg: debug/split
cpu: 13th Gen Intel(R) Core(TM) i5-13500H
BenchmarkSplit-16       47720928                22.98 ns/op           16 B/op          1 allocs/op
PASS
ok      debug/split     1.504s

```

其中，` 16 B/op` 表示每次操作内存分配了 `16` 字节，`1 allocs/op` 则表示每次操作进行了 `1` 次内存分配。 

下面我们以中文进行基准测试

```go
func BenchmarkSplit(b *testing.B) {
	for i := 0; i < b.N; i++ {
		split.Split("沙河有沙又有河", "沙")
	}
}
```

运行基准测试的结果

```powershell
@duyup ➜ split go test -bench=Split -benchmem
goos: windows
goarch: amd64
pkg: debug/split
cpu: 13th Gen Intel(R) Core(TM) i5-13500H
BenchmarkSplit-16       14067351                88.15 ns/op          112 B/op          3 allocs/op
PASS
ok      debug/split     2.757s
```

其中，`112 B/op` 表示每次操作内存分配了 `112` 字节，`3 allocs/op` 则表示每次操作进行了 `3` 次内存分配。 我们将我们的`Split`函数优化如下

```go
// Split 将 s 在 sep 处切分为一个 []string
func Split(s string, sep string) (result []string) {
	// s: "a:b:c"
	// sep: ":"

	// 提前分配好内存
	result = make([]string, 0, strings.Count(s, sep)+1)

	i := strings.Index(s, sep) // 1

	for i > -1 {
		result = append(result, s[:i])
		s = s[i+len(sep):]
		i = strings.Index(s, sep)
	}

	result = append(result, s)

	return result
}
```

这一次我们提前使用 `make` 函数将 `result` 初始化为一个容量足够大的切片，而不再像之前一样通过调用 `append` 函数来追加。我们来看一下这个改进会带来多大的性能提升

```powershell
@duyup ➜ split go test -bench=Split -benchmem
goos: windows
goarch: amd64
pkg: debug/split
cpu: 13th Gen Intel(R) Core(TM) i5-13500H
BenchmarkSplit-16       22903210                52.14 ns/op           48 B/op          1 allocs/op
PASS
ok      debug/split     2.724s
```

### 性能比较函数

上面的 **基准测试只能得到给定操作的绝对耗时**，但是在 **很多性能问题是发生在两个不同操作之间的相对耗时**

> [!example] 
>  
> 比如同一个函数处理 $1000$ 个元素的耗时与处理 $1$ 万甚至 $100$ 万个元素的耗时的差别是多少？
> 
> 再或者对于同一个任务究竟使用哪种算法性能最佳？
> 

我们通常需要对两个不同算法的实现使用相同的输入来进行基准比较测试。**性能比较函数通常是一个带有参数的函数**，被多个不同的 `Benchmark` 函数传入不同的值来调用。举个例子如下

```go
func benchmark(b *testing.B, size int){/* ... */}
func Benchmark10(b *testing.B){ benchmark(b, 10) }
func Benchmark100(b *testing.B){ benchmark(b, 100) }
func Benchmark1000(b *testing.B){ benchmark(b, 1000) }
```

例如我们编写了一个计算斐波那契数列的函数如下

```go
package fib

// fib.go

// Fib 是一个计算第n个斐波那契数的函数
func Fib(n int) int {
	if n < 2 {
		return 1
	}
	return Fib(n-1) + Fib(n-2)
}
```

我们编写的性能比较函数如下

```go
package fib_test

import (
	"debug/fib"
	"testing"
)

func benchmarkFib(b *testing.B, n int) {
	for i := 0; i < b.N; i++ {
		fib.Fib(n)
	}
}

func BenchmarkFib1(b *testing.B) {
	benchmarkFib(b, 1)
}

func BenchmarkFib2(b *testing.B) {
	benchmarkFib(b, 2)
}

func BenchmarkFib3(b *testing.B) {
	benchmarkFib(b, 3)
}

func BenchmarkFib10(b *testing.B) {
	benchmarkFib(b, 10)
}

func BenchmarkFib20(b *testing.B) {
	benchmarkFib(b, 20)
}

func BenchmarkFib40(b *testing.B) {
	benchmarkFib(b, 40)
}
```

运行基准测试

```shell
@duyup ➜ fib go test -bench=Fib\d+
goos: windows
goarch: amd64
pkg: debug/fib
cpu: 13th Gen Intel(R) Core(TM) i5-13500H
BenchmarkFib1-16        1000000000               0.6451 ns/op
BenchmarkFib2-16        682463031                1.764 ns/op
BenchmarkFib3-16        419766525                2.891 ns/op
BenchmarkFib10-16        6280075               189.4 ns/op
BenchmarkFib20-16          53098             22749 ns/op
BenchmarkFib40-16              3         345120367 ns/op
PASS
ok      debug/fib       8.875s
```

> [!tip] 默认情况下，每个基准测试至少运行$1$秒
> 
> 如果在 `Benchmark` 函数返回时没有到 $1$ 秒，则 `b.N` 的值会按 `1,2,5,10,20,50，…` 增加，并且函数再次运行

最终的 `BenchmarkFib40` 只运行了三次，每次运行的平均值只有不到一秒

> [!tip] 
> 
> 可以使用  `-benchtime` 标志增加 **最小基准时间**，以产生更准确的结果
> 

```powershell
@duyup ➜ fib go test -bench=Fib40 -benchtime=20s 
goos: windows
goarch: amd64
pkg: debug/fib
cpu: 13th Gen Intel(R) Core(TM) i5-13500H
BenchmarkFib40-16             69         342817810 ns/op
PASS
ok      debug/fib       24.357s
```

这一次 `BenchmarkFib40` 函数运行了 $69$ 次，结果就会更准确一些了

### 重置时间

`b.ResetTimer` 之前的处理不会放到执行时间里，也不会输出到报告中，所以可以在之前做一些不计划作为测试报告的操作

```go
func BenchmarkSplit(b *testing.B) {
	time.Sleep(5 * time.Second) // 假设需要做一些耗时的无关操作
	b.ResetTimer()              // 重置计时器
	for i := 0; i < b.N; i++ {
		Split("沙河有沙又有河", "沙")
	}
}
```

### 并行测试

`b.RunParallel(body func(*PB))` 会以并行的方式执行给定的基准测试。`b.RunParallel()` 会创建出多个 `goroutine`，并将 `b.N` 分配给这些 `goroutine` 执行， 其中 `goroutine` 数量的默认值为`GOMAXPROCS`。用户如果想要增加非 CPU 受限（non-CPU-bound）基准测试的并行性， 那么可以在 `RunParallel` 之前调用 `SetParallelism` 。`RunParallel` 通常会与 `-cpu` 标志一同使用

```go
// 并行基准测试
func BenchmarkSplitParallel(b *testing.B) {
	// b.SetParallelism(1) // 设置使用的CPU数
	b.RunParallel(func(p *testing.PB) {
		for p.Next() {
			Split("沙河有沙又有河", "沙")
		}
	})
}
```

## 示例函数

被 `go test` 特殊对待的第三种函数就是示例函数，它们的函数名以`Example`为前缀。它们 **既没有参数也没有返回值**。标准格式如下

```go
func ExampleName() {
    // ...
}

```


下面的代码是我们为`Split`函数编写的一个示例函数：

```go
func ExampleSplit() {
	fmt.Println(split.Split("a:b:c", ":"))
	fmt.Println(split.Split("沙河有沙又有河", "沙"))
	// Output:
	// [a b c]
	// [ 河有 又有河]
}
```

为代码编写示例代码有如下三个用处

- **示例函数能够作为文档直接使用**，例如基于 `web` 的 `godoc` 中能把示例函数与对应的函数或包相关联。
- 示例函数 **只要包含了`// Output:`也是可以通过 `go test` 运行的可执行测试**
- 示例函数 **提供了可以直接运行的示例代码**，可以直接在 `golang.org` 的`godoc`文档服务器上使用 `Go Playground`运行示例代码
