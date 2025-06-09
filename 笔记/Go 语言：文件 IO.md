# 文件 IO

**文件是 _进程_ 创建的 _信息逻辑单元_**。进程可以读取已存在的文件，并在需要时创建新文件。存储在文件中的 **_信息必须是持久_** 的，不会因为进程的创建与终止而受到影响。它 **只有在被其所有者明确表明删除时才会消失**

文件被操作系统管理。文件的操作(创建，命名，访问，修改等)都是操作系统提供的

> [!tip] 文件是一种抽象
> 文件是一种抽象，提供了一种 _在磁盘上保存信息_ 而且 _方便以后读取_ 的方法
> 
> 文件使得用户不必了解存储信息的方法、位置和实际磁盘工作方式等有关细节

对文件任何操作需要使用操作系统提供的 _系统调用_。其中，所有执行 IO 操作的系统调用都是以 _文件描述符_ 指代打开的文件

> [!important] 文件描述符
> + 文件描述符是一个非负小整数
> + 文件描述符可以表示所有类型的以打开文件

Unix 系统提供的文件类型由
+ 常规文件
+ 目录文件
+ 字符设备文件
+ 块设备文件
+ FIFO和管道文件
+ 套接字文件
+ 符号链接文件

文件的 IO 操作主要有 $4$ 个系统调用：`open` `read` `write` `close`。Go 在 `os` 包中对操作系统打开文件描述符进行了一层封装，提供了更为方便的文件操作

## 文件类型

Go 在标准包 `os` 中提供了一个文件类型 `File`，它封装了打开文件描述符。关于 `File` 的结构如下

```go
// File 代表一个打开的文件描述符
type File struct {
	*file // os specific
}

// file 是 *File 的真实表示。额外的间接级别可确保 os 客户端无法覆盖此数据，否则可能导致终结器关闭错误的文件描述符。
type file struct {
	pfd         poll.FD  // 代表系统文件描述符
	name        string   // 文件名
	dirinfo     *dirInfo // nil 除非正在读取的目录
	nonblock    bool     // 是否设置了非阻塞模式
	stdoutOrErr bool     // 是 stdout 还是 stderr
	appendMode  bool     // 是否打开文件进行追加
}

// FD 是文件描述符。net 和 os 软件包使用这种类型作为代表网络连接或操作系统文件的较大类型的一个字段。
type FD struct {
	// 锁定 sysfd 并序列化 "读" 和 "写" 方法的访问权限。
	fdmu fdMutex

	// 系统文件描述符。在关闭前不可更改
	Sysfd int

	// 与平台相关的文件描述符状态.
	SysFile

	// I/O poller.
	pd pollDesc

	// 文件关闭时发出 Semaphore 信号。
	csema uint32

	// 如果该文件已设置为阻塞模式，则非 0。
	isBlocking uint32

	// 这是否是一个流描述符，而不是像 UDP 套接字那样的 基于数据包的描述符（如 UDP 套接字）。不可变。
	IsStream bool

	// 读取零字节是否表示 EOF。对于 对于基于消息的套接字连接，此值为假。
	ZeroReadIsEOF bool

	// 是否是文件而不是网络套接字。
	isFile bool
}
```

## 打开文件

Go 提供了三个常用的打开文件的方法：`os.OpenFile()` `os.Open()` 和 `os.Create()`
+ `OpenFile` 提供了更完整的打开文件操作
+ `Open` 以只读方式打开文件
+ `Create` 创建一个文件

### `os.OpenFile()`

`os.OpenFile()` 函数提供了一个更一般的文件打开函数。它可以指定打开文件 **选项**，和打开文件的 **权限**。如果操作成功，返回的文件对象可用于 `I/O` 操作

```go
func OpenFile(name string, flag int, prem FileMode) (file *File, err error)
```

参数 `name` 是一个文件路径字符串，可以指定 **相对路径** 和 **绝对路径**
+ 相对路径：相对于程序执行的路径
+ 绝对路径：从根目录出发

参数 `flag` 用于指定打开文件的选项。`os` 包中定义的标准有下几个

```go
const (
	//  O_RDONLY, O_WRONLY, or O_RDWR 只能三选一
	O_RDONLY int = syscall.O_RDONLY // 只读方式打开文件
	O_WRONLY int = syscall.O_WRONLY // 只写方式打开文件
	O_RDWR   int = syscall.O_RDWR   // 可读可写方式打开文件
	
	// 下面的标志控制行为
	O_APPEND int = syscall.O_APPEND // 写入时将数据追加到文件
	O_CREATE int = syscall.O_CREAT  // 如果不存在，则创建新文件
	O_EXCL   int = syscall.O_EXCL   // 与 O_CREAT 一起使用，文件必须不存在
	O_SYNC   int = syscall.O_SYNC   // 打开文件用于同步I/O
	O_TRUNC  int = syscall.O_TRUNC  // 打开时截断常规可写文件
)
```

参数 `prem` 代表文件的 **模式** 和 **权限位**。这些位在所有系统上具有相同的定义，因此有关文件的信息可以便携地从一个系统移动到另一个系统。并非所有位都适用于所有系统。目录中唯一需要的位是 `ModeDir`

`prem` 的类型是 `os.FileMode`，它是 `fs.FileMode` 的别名，本质上是 `uint32`  

```go
type FileMode = fs.FileMode  // os 包中

type FileMode uint32  // io/fs 包中
```

`prem` 的从最高位开始的连续 $12$ 为代表了文件模式，最低 $9$ 位分为 $3$ 组，代表文件属主 属组 其他用户的权限。结构如下图所示

![[Pasted image 20250609212132.png]]

> [!important] 文件模式和权限设置
> + 对于普通文件，文件模式位不能被设置
> + 文件权限设置通过三位八进制数设置。也可以通过 `syscall` 包提供了常量设置
> ```go
> const (
>	S_IRGRP = 0x20  // 属组 Read   000 100 000
>	S_IROTH = 0x4   // 其他 Read   000 000 100
>	S_IRUSR = 0x100 // 属主 Read   100 000 000
>
>	S_IWGRP = 0x10 // 属组 Write   000 010 000
>	S_IWOTH = 0x2  // 其他 Write   000 000 010
>	S_IWUSR = 0x80 // 属主 Write   010 000 000
>
>	S_IXGRP = 0x8  // 属组 Execute 000 001 000
>	S_IXOTH = 0x1  // 其他 Execute 000 000 001
>	S_IXUSR = 0x40 // 属主 Execute 001 000 000
>
>	S_IRWXG = 0x38  // 属组 Read-Write-Execute 000 111 000
>	S_IRWXO = 0x7   // 其他 Read-Write-Execute 000 000 111
>	S_IRWXU = 0x1c0 // 属主 Read-Write-Execute 111 000 000
>)
> ```

```go
package main

import (
	"fmt"
	"os"
)

func main() {

	// 以独占方式创建一个可写入文件对象
	flag := os.O_WRONLY | os.O_CREATE | os.O_EXCL

	// 属主可读可写，属组可读，其他用户可读
	prem := os.FileMode(0o644)

	// 打开文件
	f, err := os.OpenFile("notes.txt", flag, prem)
	if err != nil {
		panic(fmt.Errorf("文件打开失败: %w", err))
	}

	fmt.Println(f)
	f.Close()
}
```

### `os.Open()` 

`os.Open()`函数能够 **以只读方式打开一个文件**，返回一个 `*File` 和一个`err`。对应的文件描述符具有 `os.O_RDONLY` 模式

```go
package main

import (
	"log"
	"os"
)

func main() {
	// 以只读方式打开文件
	f, err := os.Open("test.txt")

	if err != nil {
		log.Println("文件打开失败: %v\n", err)
	}
	
	// 对文件进行操作

	err = f.Close()
	if err != nil {
		log.Printf("文件关闭失败: %v\n", err)
	}
}
```

> [!tip] `os.Open()` 等价于 `os.OpenFile(name, os.O_RDONLY, 0)`


### `os.Create()`

`os.Create()` 采用模式 `0o666`（任何人都可读写，不可执行）创建一个名为 `name` 的文件，如果文件已存在会截断它（为空文件）。如果成功，返回的文件对象可用于 `I/O`；对应的文件描述符具有 `O_RDWR` 模式。如果出错，错误底层类型是 `*PathError`

```go
func Create(name string) (file *File, err error)
```

> [!tip] `os.Create()` 等价于 `os.OpenFile(name, os.O_RDWR | os.O_CREATE | os.O_TRUNC, 0o666)`

一个进程启动后会自动打开三个文件。`Stdin` `Stdout` 和 `Stderr` 分别管理了标准输入文件描述符、标准输出文件描述符和标准错误文件描述符

```go
var (
    Stdin  = NewFile(uintptr(syscall.Stdin), "/dev/stdin")
    Stdout = NewFile(uintptr(syscall.Stdout), "/dev/stdout")
    Stderr = NewFile(uintptr(syscall.Stderr), "/dev/stderr")
)
```

## 关闭文件

当文件打开后不再被使用，必须使用文件对象的 `file.Close()` 方法，因为一个进程 **文件描述符资源是有限**。

> [!important] 资源限制
> Unix 系统可以使用 `ulimit` 设置资源限制
> ```shell
> $ ulimit -a
> real-time non-blocking time  (microseconds, -R) unlimited
> core file size              (blocks, -c) 0
> data seg size               (kbytes, -d) unlimited
> scheduling priority                 (-e) 0
> file size                   (blocks, -f) unlimited
> pending signals                     (-i) 6394
> max locked memory           (kbytes, -l) 214148
> max memory size             (kbytes, -m) unlimited
> open files                          (-n) 65535  # 打开文件限制
> pipe size                (512 bytes, -p) 8
> POSIX message queues         (bytes, -q) 819200
> real-time priority                  (-r) 0
> stack size                  (kbytes, -s) 8192
> cpu time                   (seconds, -t) unlimited
> max user processes                  (-u) 6394
> virtual memory              (kbytes, -v) unlimited
> file locks                          (-x) unlimited
> ```
> 
> 当资源耗尽时，会影响进程的运行

### `defer file.Close()`

文件打开成功后，立即 `defer file.Close()` 是一个比较好的习惯。然而，在循环中，不建议这样做的

```go
package main

import (
	"fmt"
	"log"
	"os"
)

func main() {
	path := []string{"test1.txt", "test2.txt"}

	for i, v := range path {
		file, err := os.Create(v)
		if err != nil {
			log.Fatalf("创建文件失败: %v\n", err)
		}
		fmt.Printf("打开文件: %v\n", file.Name())
		defer func(i int) {
			file.Close()
			fmt.Printf("关闭文件: %v\n", file.Name())
		}(i)
	}
}
```

这段代码输出的结果为

```shell
11:23:54 dyp@dyp-PC gocode → go build 
11:23:56 dyp@dyp-PC gocode → ./gocode 
打开文件: test1.txt
打开文件: test2.txt
关闭文件: test2.txt
关闭文件: test1.tx
```

> [!important] `defer` 语句的调用，在 `for` 循环退出以后才开始执行
> + 如果打开资源过多，而没有及时关闭，势必会造成资源的浪费，甚至因此而意外终止程序
> + 所以切记，**不要在循环中使用 `defer`**

我们可以使用匿名函数来解决这个问题：

```go
package main

import (
	"fmt"
	"log"
	"os"
)

func main() {
	path := []string{"test1.txt", "test2.txt"}

	for _, v := range path {
		file, err := os.Create(v)
		if err != nil {
			log.Fatalf("创建文件失败: %v\n", err)
		}
		fmt.Printf("打开文件: %v\n", file.Name())

		err = func() error {
			defer func() {
				file.Close()
				fmt.Printf("关闭文件: %v\n", file.Name())
			}()

			_, err := file.Write([]byte("你好"))
			return err
		}()

		if err != nil {
			log.Fatalf("操作文件失败: %v\n", err)
		}
	}
}
```

现在，这个输出符合需求了

```shell
11:32:31 dyp@dyp-PC gocode → go build 
11:32:38 dyp@dyp-PC gocode → ./gocode 
打开文件: test1.txt
关闭文件: test1.txt
打开文件: test2.txt
关闭文件: test2.tx
```

这个匿名函数可能在多个需要释放资源的位置使用，我们可以抽离出来，形成一个函数 `withClose(c io.Closer, f func())`，将操作逻辑外置

```go
package main

import (
	"fmt"
	"io"
	"log"
	"os"
)

func withClose(c io.Closer, f func()) {
	defer func() {
		c.Close()
		fmt.Printf("关闭: %v\n", c.(*os.File).Name())
	}()
	f()
}

func main() {
	path := []string{"test1.txt", "test2.txt"}

	for _, v := range path {
		file, err := os.Create(v)
		if err != nil {
			log.Fatalf("创建文件失败: %v\n", err)
		}
		fmt.Printf("打开: %v\n", file.Name())

		withClose(file, func() {
			file.Write([]byte("你好"))
		})
	}
}
```

这样也是符合我们的需求的
```shell
11:39:31 dyp@dyp-PC gocode → go build 
11:39:33 dyp@dyp-PC gocode → ./gocode 
打开: test1.txt
关闭: test1.txt
打开: test2.txt
关闭: test2.txt
```
> [!important] 这个思路来自于 Python 的 with 语句


## `Read` 和 `Write`

`*os.File` 对象上用于读取内容的方法为 `file.Read`

```go
// Read 读取 f 中的字节并存放在 b 中，返回读取的字数和出现的错误
// 读到文件末尾时，返回 0 和 io.EOF
func (f *File) Read(b []byte) (n int, err error)
```

`*os.File` 对象上用于写入内容的方法有 `file.Write` 和 `file.WriteString`

```go
// Write 将 b 中的字节写入到文件 f 中，返回写入的字数和出现的错误
func (f *File) Write(b []byte) (n int, err error)

// WriteString 将字符串 s 写入到文件 f 中，返回写入的字数和出现的错误
func (f *File) WriteString(s string) (n int, err error)
```

> [!important] `Read` 和 `Write` 是文件 IO 的基本操作，仅仅是对系统调用的封装

### 示例：`copy` 

```go
package main

import (
	"fmt"
	"io"
	"os"
)

func main() {

	if len(os.Args) < 3 {
		fmt.Fprintf(os.Stderr, "Usage: %v src dst", os.Args[0])
		return
	}

	src, err := os.Open(os.Args[1])
	if err != nil {
		fmt.Fprintf(os.Stderr, "打开文件 %s 失败: %v\n", os.Args[1], err)
	}

	defer src.Close() // 打开文件后，立即 defer 文件的 Close 方法

	flag := os.O_WRONLY | os.O_CREATE | os.O_EXCL
	prem := os.FileMode(0o644)

	dst, err := os.OpenFile(os.Args[2], flag, prem)
	if err != nil {
		fmt.Fprintf(os.Stderr, "打开文件 %s 失败: %v\n", os.Args[2], err)
	}

	defer dst.Close()

	buffer := make([]byte, 1024)

	for {
		n, err := src.Read(buffer)
		
		// 先处理 n > 0 的情形，这一定不会处理 n == 0 的情形
		// 这样避免 err != nil 但是 n > 0 的情形导致的数据丢失
		dst.Write(buffer[:n])
		
		if err != nil {
			if err == io.EOF {
				fmt.Fprintf(os.Stderr, "文件读取完成")
				break
			} else {
				panic(fmt.Errorf("文件读取失败: %w", err))
			}
		}
	}
}
```

## 改变文件偏移量

对于每个打开的文件，系统内核会记录其 **文件偏移量**，有时也将文件偏移量称为读写偏移量或指针

> [!important] 文件偏移量
> 
> 文件偏移量是指执行下一个 `Read()` 或 `Write()` 操作的文件起始位置，会以相对于文件头部起始点的文件当前位置来表示。文件第一个字节的偏移量为 $0$

文件打开时，会将文件偏移量设置为指向文件开始，以后每次 `Read()` 或 `Write()` 调用将自动对其进行调整，以指向已读或已写数据后的下一字节

`*os.File` 对象的 `Seek()` 方法用于设置下一次 `Read()` 和 `Write()` 的起始位置。参数 `offset` 为相对偏移量，它根据参数 `whence` 设置的相对偏移位置进行偏移。

```go
func (f *File) Seek(offset int64, whence int) (ret int64, err error)
```

参数 `offset` 可正可负。参数 `whence` 的取值如下

|     `whence`     | 描述        |
| :--------------: | :-------- |
|  `io.SeekStart`  | 相对于文件起始位置 |
| `io.SeekCurrent` | 相对于当前文件位置 |
|   `io.SeekEnd`   | 相对于文件结尾位置 |

> [!warning] 早期使用 `os` 包下定义的常量：但是，目前 `os` 包下的 `whence` 常量定义被废弃了
> + `SEEK_SET` 文件起始位置
> + `SEEK_CUR` 当前文件位置
> + `SEEK_END` 文件结尾位置

> [!important] `flie.Seek()` 方法会返回改变后的文件偏移量
> `file.Seek(0, io.SeekCurrent)` 就可以获取当前文件偏移量

## `ReadAt` 和 `WriteAt`

`*os.File` 对象的方法 `file.ReadAt()` 和 `file.WriteAt()` 从指定字节偏移开始读取或写入

```go
// ReadAt 从 off 字节偏移开始从文件中读取 len(b) 个字节
// 它返回读取的字节数和出现的错误
// 当 n < len(b) 时，ReadAt 总是返回一个非 nil 错误。
// 在文件末尾，该错误是 io.EOF。
func (f *File) ReadAt(b []byte, off int64) (n int, err error)

// WriteAt 从字节偏移量开始向文件 f 写入 len(b) 个字节
// 它返回写入的字节数和错误(如果有的话)
// 当 n != len(b) 时，WriteAt 返回一个非 nil 错误
func (f *File) WriteAt(b []byte, off int64) (n int, err error)
```

> [!important] `file.ReadAt()` 和 `file.WriteAt()` 不会修改文件指针
>
> ```go
> current, err := file.Seek(0, io.SeekCurrent)
> file.ReadAt(b, current + 10)
> ```
>
> 等价于
>
> ```go
> current, err := file.Seek(0, io.SeekCurrent)
> file.Read(b)
> file.Seek(current, io.SeekCurrent)
> ```
> 

## 文件缓冲

出于速度和效率考虑，系统 I/O 调用(即内核)在操作磁盘文件时会对数据进行 **缓冲**

**`Read()` 和 `Write()` 系统调用** 在操作磁盘文件时 **不会直接发起磁盘访问**，而是仅仅 **在 _用户空间缓冲区_ 与 _内核缓冲区_ 高速缓存之间复制数据**

> [!important] 由于缓冲的存在，系统调用与磁盘操作并不同步
> + 调用 `Write([]byte("abc"))`，将这 $3$ 个字节从用户缓冲区拷贝到内核缓冲区中，然后 `Write` 立即返回
> + 如果在缓冲期间，**另一进程试图读取该文件的这几个字节，那么内核将自动从缓冲区高速缓存中提供这些数据**，而不是从文件中（读取过期的内容）

**对 `Read` 而言，内核从磁盘中读取数据并存储到内核缓冲区中**。`Read()` 将从该缓冲区中读取数据，直至把缓冲区中的数据取完，这时，内核会将文件的下一段内容读入缓冲区高速缓存

> [!tip] 内核会尝试预先读取文件的内容到内核缓冲区
> 对于序列化的文件访问，内核通常会尝试执行预读，以确保在需要之前就将文件的下一数据块读入缓冲区高速缓存中

**_强制刷新内核缓冲区到输出文件是可能的_**。这有时很有必要，例如，当应用程序（诸如数据库的日志进程）要确保在继续操作前将输出真正写入磁盘（或者至少写入磁盘的硬件高速缓存中）

> [!important] **同步 IO 数据完整性** 和 **同步 IO 文件完整性**
> **同步 I/O 完成(`synchronized IO completion`)** 的定义：某一 IO 操作，要么 **已成功完成到磁盘的数据传递**，要么 **被诊断为不成功**
> 
> **同步 IO 数据完整性** 和 **同步 IO 文件完整性** 的区别在于对 _文件元数据_ 的处理
> + **同步 IO 数据完整性**(`synchronized IO data integrity completion`)：旨在确保针对文件的一次更新传递了足够的信息（到磁盘），以便于之后对数据的获取
> + **同步 IO 文件完整性**(`synchronized I/O file integrity completion`)：在对文件的一次更新过程中，**要将所有发生更新的文件元数据都传递到磁盘上**，即使有些在后续对文件数据的读操作中并不需要
> 

`file.Sync()` 将文件的当前内容提交到稳定存储。通常，这意味着将最近写入数据刷新到磁盘

```go
func (f *File) Sync() error
```

> [!important] `file.Sync()` 强制文件处于 **同步 IO 数据完整性**

## 截断文件

**`Truncate` 改变文件的大小，它不会改变 `I/O` 的当前位置**。 如果截断文件，多出的部分就会被丢弃。如果出错，错误底层类型是 `*PathError`

```go
func (f *File) Truncate(size int64) error
```
