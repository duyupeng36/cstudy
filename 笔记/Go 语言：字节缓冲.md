# 字节缓冲

`bytes` 包提供了对 **字节切片** 进行读写操作的一系列函数，字节切片处理的函数比较多分为基本处理函数、比较函数、后缀检查函数、索引函数、分割函数、大小写处理函数和子切片处理函数等. 与 [[字符串#标准包 `strings`|标准包 strings]] 中的方法非常相似

```go
func Clone(b []byte) []byte
func Compare(a, b []byte) int
func Contains(b, subslice []byte) bool
func ContainsAny(b []byte, chars string) bool
func ContainsFunc(b []byte, f func(rune) bool) bool
func ContainsRune(b []byte, r rune) bool
func Count(s, sep []byte) int
func Cut(s, sep []byte) (before, after []byte, found bool)
func CutPrefix(s, prefix []byte) (after []byte, found bool)
func CutSuffix(s, suffix []byte) (before []byte, found bool)
func Equal(a, b []byte) bool
func EqualFold(s, t []byte) bool
func Fields(s []byte) [][]byte
func FieldsFunc(s []byte, f func(rune) bool) [][]byte
func HasPrefix(s, prefix []byte) bool
func HasSuffix(s, suffix []byte) bool
func Index(s, sep []byte) int
func IndexAny(s []byte, chars string) int
func IndexByte(b []byte, c byte) int
func IndexFunc(s []byte, f func(r rune) bool) int
func IndexRune(s []byte, r rune) int
func Join(s [][]byte, sep []byte) []byte
func LastIndex(s, sep []byte) int
func LastIndexAny(s []byte, chars string) int
func LastIndexByte(s []byte, c byte) int
func LastIndexFunc(s []byte, f func(r rune) bool) int
func Map(mapping func(r rune) rune, s []byte) []byte
func Repeat(b []byte, count int) []byte
func Replace(s, old, new []byte, n int) []byte
func ReplaceAll(s, old, new []byte) []byte
func Runes(s []byte) []rune
func Split(s, sep []byte) [][]byte
func SplitAfter(s, sep []byte) [][]byte
func SplitAfterN(s, sep []byte, n int) [][]byte
func SplitN(s, sep []byte, n int) [][]byte
func Title(s []byte) []byteDEPRECATED
func ToLower(s []byte) []byte
func ToLowerSpecial(c unicode.SpecialCase, s []byte) []byte
func ToTitle(s []byte) []byte
func ToTitleSpecial(c unicode.SpecialCase, s []byte) []byte
func ToUpper(s []byte) []byte
func ToUpperSpecial(c unicode.SpecialCase, s []byte) []byte
func ToValidUTF8(s, replacement []byte) []byte
func Trim(s []byte, cutset string) []byte
func TrimFunc(s []byte, f func(r rune) bool) []byte
func TrimLeft(s []byte, cutset string) []byte
func TrimLeftFunc(s []byte, f func(r rune) bool) []byte
func TrimPrefix(s, prefix []byte) []byte
func TrimRight(s []byte, cutset string) []byte
func TrimRightFunc(s []byte, f func(r rune) bool) []byte
func TrimSpace(s []byte) []byte
func TrimSuffix(s, suffix []byte) []byte
```

## `bytes.Buffer` 类型

`bytes.Buffer` 是一个**大小可变的字节缓冲区**，具有 `Buffer.Read` 和 `Buffer.Write` 方法。`Buffer` 的零值表示缓冲区为空，可以随时使用

> [!important] `bytes.Buffer` 零值可用

```go
type Buffer struct {
	buf      []byte // 内容为字节 buf[off : len(buf)]
	off      int    // 在 &buf[off] 处读取，在 &buf[len(buf)] 处写入
	lastRead readOp // 最后一次读取操作，这样 Unread* 才能正常工作。
}
```

### 构造函数

#### `bytes.NewBuffer(buf)`

`bytes.NewBuffer(buf)` 使用 `buf` 创建并初始化一个新 `bytes.Buffer` 对象

> [!warning] 新 `bytes.Buffer` 对象拥有 `buf` 的所有权，调用者在调用后不应再使用 `buf`

`bytes.NewBuffer` 用于准备 `bytes.Buffer` 以读取现有数据。它也可用于设置内部缓冲区的初始大小，以便写入。为此，`buf` 应具有所需的容量，但长度为零

> [!tip] 在大多数情况下，`new(bytes.Buffer)`（或仅仅声明一个 `bytes.Buffer` 变量）就足以初始化一个 `Buffer`

#### `bytes.NewBufferString(s)`

`bytes.NewBufferString(s)` 使用字符串 `s` 创建并初始化一个新的缓冲区。其目的是为读取现有字符串准备缓冲区。

```go
func NewBufferString(s string) *Buffer
```

### 方法

> [!important] `bytes.Buffer` 对象与 `*os.File` 文件对象类似
> + `bytes.Buffer` 实现了 `io.Reader` 和 `io.Writer` 接口

```go
// Available 返回缓冲区中未使用的字节数
func (b *Buffer) Available() int

// AvailableBuffer 返回一个容量为 b.Available() 的空缓冲区
// 该缓冲区将被追加并传递给紧随其后的 Buffer.Write 调用
// 该缓冲区只在下次对 b.AvailableBuffer 进行写操作之前有效
func (b *Buffer) AvailableBuffer() []byte

// Bytes 返回一个长度为 b.Len() 的切片，其中包含缓冲区的未读部分
// 切片仅在下一次修改缓冲区之前有效（也就是说，仅在下一次调用 Buffer.Read、Buffer.Write、Buffer.Reset 或 Buffer.Truncate 等方法之前有效）
// 片段至少在下一次修改缓冲区之前别名了缓冲区内容，因此立即更改片段会影响未来的读取结果。
func (b *Buffer) Bytes() []byte

// Cap 返回缓冲区底层字节片的容量，即分配给缓冲区数据的总空间
func (b *Buffer) Cap() int

// 必要时，Grow 会增加缓冲区的容量，以保证能再容纳 n 个字节
// 在 Grow(n) 之后，至少有 n 个字节可以写入缓冲区而无需再次分配
// 如果 n 为负数，Grow 就会 panic
// 如果缓冲区无法增长，则会出现 ErrTooLarge 异常
func (b *Buffer) Grow(n int)

// Len 返回缓冲区未读部分的字节数；b.Len() == len(b.Bytes())
func (b *Buffer) Len() int

// Next 返回一个切片，其中包含缓冲区中下一个 n 字节，并将缓冲区向前推进，就像 Buffer.Read 所返回的字节一样
// 如果缓冲区中的字节少于 n 个，Next 将返回整个缓冲区。该分片仅在下一次调用读或写方法之前有效
func (b *Buffer) Next(n int) []byte

// Read 从缓冲区读取下一个 len(p) 字节或直到缓冲区耗尽。返回值 n 是读取的字节数
// 如果缓冲区没有要返回的数据，err 就是 io.EOF（除非 len(p) 为零）；否则就是 nil
func (b *Buffer) Read(p []byte) (n int, err error)

// ReadByte 从缓冲区读取并返回下一个字节。如果没有可用字节，则返回错误 io.EOF
func (b *Buffer) ReadByte() (byte, error)

// ReadBytes 一直读取到输入中第一次出现 delim 时为止，并返回一个包含分隔符（含分隔符）之前数据的片段
// 如果 ReadBytes 在找到分隔符前遇到错误，它会返回错误前读取的数据和错误本身（通常为 io.EOF）
// 只有当返回的数据不是以 delim 结束时，ReadBytes 才会返回 err != nil
func (b *Buffer) ReadBytes(delim byte) (line []byte, err error)

// ReadFrom 从 r 读取数据，直到 EOF，然后将其添加到缓冲区，并根据需要扩大缓冲区。返回值 n 是读取的字节数
// 读取过程中遇到的除 io.EOF 以外的任何错误也会返回。如果缓冲区过大，ReadFrom 会以 ErrTooLarge 引起 panic
func (b *Buffer) ReadFrom(r io.Reader) (n int64, err error)

// ReadRune 从缓冲区读取并返回下一个 UTF-8 编码的 Unicode 代码点
// 如果没有可用字节，则返回错误信息 io.EOF
// 如果字节是错误的 UTF-8 编码，则会消耗一个字节并返回 U+FFFD, 1。
func (b *Buffer) ReadRune() (r rune, size int, err error)

// ReadString 一直读到输入中第一次出现分隔符为止，并返回一个包含分隔符（含分隔符）之前数据的字符串
// 如果 ReadString 在找到分隔符之前遇到错误，它会返回错误之前读取的数据和错误本身（通常为 io.EOF）
// 只有当返回的数据不是以 delim 结束时，ReadString 才会返回 err != nil
func (b *Buffer) ReadString(delim byte) (line string, err error)

// Reset 会将缓冲区重置为空，但会保留底层存储空间供将来写入时使用。重置与 Buffer.Truncate(0) 相同
func (b *Buffer) Reset()

// String 以字符串形式返回缓冲区未读部分的内容。如果缓冲区是一个 nil 指针，则返回"<nil>"。
func (b *Buffer) String() string

// Truncate 会丢弃缓冲区中除前 n 个未读字节之外的所有字节，但会继续使用已分配的存储空间。如果 n 为负数或大于缓冲区的长度，它就会 panic
func (b *Buffer) Truncate(n int)

// UnreadByte 读取最近一次成功读取操作（至少读取一个字节）返回的最后一个字节
// 如果上次读取后发生了写操作，如果上次读取返回错误，或者如果读取的字节为零，UnreadByte 将返回错误信息
func (b *Buffer) UnreadByte() error
// UnreadRune 会读取 Buffer.ReadRune 返回的最后一个符文
// 如果最近一次对缓冲区的读取或写入操作不是成功的 Buffer.ReadRune，UnreadRune 将返回错误信息。(在这方面，UnreadRune 比 Buffer.UnreadByte 更严格，后者会从任何读取操作中取消读取最后一个字节）
func (b *Buffer) UnreadRune() error

// Write 将 p 的内容追加到缓冲区，并根据需要扩大缓冲区。返回值 n 是 p 的长度；err 始终为 nil
// 如果缓冲区变得过大，Write 会以 ErrTooLarge 引起 panic。
func (b *Buffer) Write(p []byte) (n int, err error)

// WriteByte 将字节 c 附加到缓冲区，并根据需要扩大缓冲区。
// 返回的错误总是 nil，但会包含在缓冲区中以匹配 bufio.Writer 的 WriteByte。如果缓冲区变得过大，WriteByte 会出现 ErrTooLarge 异常
func (b *Buffer) WriteByte(c byte) error

// WriteRune 会将 Unicode 代码点 r 的 UTF-8 编码追加到缓冲区，并返回其长度和错误信息，错误信息始终为 nil，但会包含在缓冲区中以匹配 bufio.Writer 的 WriteRune。缓冲区会根据需要不断扩大；如果缓冲区过大，WriteRune 会以 ErrTooLarge 引起 panic。
func (b *Buffer) WriteRune(r rune) (n int, err error)

// WriteString 将 s 的内容追加到缓冲区，并根据需要扩大缓冲区。返回值 n 是 s 的长度；err 始终为 nill。如果缓冲区过大，WriteString 会以 ErrTooLarge 引起 panic。
func (b *Buffer) WriteString(s string) (n int, err error)

// WriteTo 会向 w 写入数据，直到缓冲区耗尽或发生错误。返回值 n 是写入的字节数；它总是适合一个 int，但为了与 io.WriterTo 接口匹配，它是 int64。写入过程中遇到的任何错误也会返回
func (b *Buffer) WriteTo(w io.Writer) (n int64, err error)
```

### 示例：`Buffer` 作为 `Writer`

```go
package main

import (
	"bytes"
	"fmt"
	"os"
)

func main() {
	var buffer bytes.Buffer
	buffer.Write([]byte("hello, "))
	fmt.Fprintf(&buffer, "world!\n")
	buffer.WriteTo(os.Stdout)
}
```

### 示例: `Buffer` 作为 `Reader`

```go
package main

import (
	"bytes"
	"encoding/base64"
	"io"
	"os"
)

func main() {
	buf := bytes.NewBufferString("R29waGVycyBydWxlIQ==")
	
	// 需要从一个 Reader 对象中读取数据
	dec := base64.NewDecoder(base64.StdEncoding, buf)
	io.Copy(os.Stdout, dec)
}
```

### 示例：利用 `Buffer` 读取文件

```go
package main

import (
	"bytes"
	"log"
	"os"
)

func main() {
	if len(os.Args) < 3 {
		log.Fatalf("Usage: %v src dst\n", os.Args[0])
	}

	src, err := os.Open(os.Args[1])
	if err != nil {
		log.Fatalf("文件打开失败: %v\n", err)
	}

	var buf bytes.Buffer
	buf.ReadFrom(src)

	dst, err := os.OpenFile(os.Args[2], os.O_WRONLY|os.O_CREATE|os.O_EXCL, 0o644)
	if err != nil {
		log.Fatalf("打开文件失败: %v\n", err)
	}

	buf.WriteTo(dst)
}
```

## `bytes.Reader` 类型

`Reader` 通过从字节切片中读取数据来实现 `io.Reader`、`io.ReaderAt`、`io.WriterTo`、`io.Seeker`、`io.ByteScanner` 和 `io.RuneScanner` 接口。与 `Buffer` 不同，`Reader` 类型对象是只读的，并且支持寻道。`Reader` 类型的零值操作类似于读取空片段

> [!tip] `Reader` 实现了与 `io.Reader` 相关的接口

```go
type Reader struct {
	s        []byte
	i        int64 // 当前读取的索引
	prevRune int   // 上一次读取 rune 的索引
}
```

### 构造函数

#### `bytes.NewReader`

`bytes.NewReader(b)` 返回新的从 `b` 读取数据的 `bytes.Reader`

```go
func NewReader(b []byte) *Reader
```

### 方法

```go
// Len 返回未读取的字节数
func (r *Reader) Len() int

// Read 实现 io.Reader
func (r *Reader) Read(b []byte) (n int, err error)

// Read 实现 io.ReaderAt
func (r *Reader) ReadAt(b []byte, off int64) (n int, err error)

// ReadByte 和 UnreadByte 实现 io.ByteScanner
func (r *Reader) ReadByte() (byte, error)
func (r *Reader) UnreadByte() error

// ReadRune 和 UnreadRune 实现 io.RuneScanner
func (r *Reader) ReadRune() (ch rune, size int, err error)
func (r *Reader) UnreadRune() error

func (r *Reader) Reset(b []byte)

// Seek 实现 io.Seeker
func (r *Reader) Seek(offset int64, whence int) (int64, error)

func (r *Reader) Size() int64

// WriteTo 实现 io.WriterTo
func (r *Reader) WriteTo(w io.Writer) (n int64, err error)
```

### 示例: `bytes.Reader`

```go
package main

import (
	"bytes"
	"fmt"
)

func testReader() {
	data := "123456789"
	//通过[]byte创建Reader
	re := bytes.NewReader([]byte(data))
	
	//返回未读取部分的长度
	fmt.Println("re len : ", re.Len())
	
	//返回底层数据总长度
	fmt.Println("re size : ", re.Size())
	
	fmt.Println("------------")

	buf := make([]byte, 2)
	for {
		//读取数据
		n, err := re.Read(buf)
		if err != nil {
			break
		}
		fmt.Println(string(buf[:n]))
	}

	fmt.Println("------------")

	//设置偏移量，因为上面的操作已经修改了读取位置等信息
	re.Seek(0, 0)
	for {
		//一个字节一个字节的读
		b, err := re.ReadByte()
		if err != nil {
			break
		}
		fmt.Println(string(b))
	}
	fmt.Println("------------")

	re.Seek(0, 0)
	off := int64(0)
	for {
		//指定偏移量读取
		n, err := re.ReadAt(buf, off)
		if err != nil {
			break
		}
		off += int64(n)
		fmt.Println(off, string(buf[:n]))
	}
}

func main() {
	testReader()
}
```
