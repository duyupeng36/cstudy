# 缓冲 IO

标准包 `bufio` 实现了 **用户空间的缓冲 IO**。`bufio` 中提供了一个分别实现了 `io.Reader` 或 `io.Writer` 的类型，分别是 `Reader` 或 `Writer`

## `Reader` 类型

```go
// Reader 实现了对 io.Reader 对象的缓冲
type Reader struct {
	buf          []byte
	rd           io.Reader // 客户提供的 Reader 实例对象
	r, w         int       // buf  读写位置
	err          error
	lastByte     int // last byte read for UnreadByte; -1 means invalid
	lastRuneSize int // size of last rune read for UnreadRune; -1 means invalid
}
```

### 构造函数：`bufio.NewReader, bufio.NewReaderSize `

`NewReader` 返回一个新的 `Reader` 对象，其缓冲区大小为默认值

```go
func NewReader(rd io.Reader) *Reader
```

`NewReaderSize` 返回一个缓冲区至少有指定大小的 `Reader` 对象。如果参数 `io.Reader` 已经是一个足够大的读取器，它将返回底层读取器

```go
func NewReaderSize(rd io.Reader, size int) *Reader
```

### `Reader` 类型的方法

```go
// Buffered 返回可从当前缓冲区读取的字节数
func (b *Reader) Buffered() int

// Discard 跳过下 n 个字节，返回丢弃的字节数
// 如果 Discard 跳过的字节少于 n，也会返回错误
// 如果 0 <= n <= b.Buffered()，则保证 Discard 在不从底层 io.Reader 读取数据的情况下成功
func (b *Reader) Discard(n int) (discarded int, err error)

// Peek 返回下 n 个字节，读取器不向前推进。这些字节在下一次读取调用时不再有效
// 如果 Peek 返回的字节少于 n 个，它还会返回一个错误信息，解释读取时间短的原因
// 如果 n 大于 b 的缓冲区大小，则错误信息为 ErrBufferFull
func (b *Reader) Peek(n int) ([]byte, error)

// 它返回读入 p 的字节数。这些字节最多来自底层 Reader 的一次读取，因此 n 可能小于 len(p)
// 要精确读取 len(p) 字节，请使用 io.ReadFull(b,p)。如果底层 Reader 可以通过 io.EOF 返回非零计数，那么此读取方法也可以
func (b *Reader) Read(p []byte) (n int, err error)

// ReadByte 读取并返回一个字节。如果没有可用字节，则返回错误信息。
func (b *Reader) ReadByte() (byte, error)

// ReadBytes 一直读取到输入中第一次出现 delim 时为止，并返回一个包含分隔符（含分隔符）之前字节的切片
// 如果 ReadBytes 在找到分隔符前遇到错误，它会返回错误前读取的数据和错误本身（通常为 io.EOF）
// 只有当返回的数据不是以 delim 结束时，ReadBytes 才会返回 err != nil。对于简单的用途，使用 Scanner 可能更方便。
func (b *Reader) ReadBytes(delim byte) ([]byte, error)

// ReadLine 是一个低级的行读取原语。大多数调用者应该使用 Reader.ReadBytes('\n') 或 Reader.ReadString('\n') 来代替，或者使用 Scanner.ReadLine('\n') 来代替
func (b *Reader) ReadLine() (line []byte, isPrefix bool, err error)

// ReadRune 读取单个 UTF-8 编码的 Unicode 字符，并以字节为单位返回符文及其大小
// 如果编码符文无效，它将消耗一个字节并返回大小为 1 的 unicode.ReplacementChar (U+FFFFD)。
func (b *Reader) ReadRune() (r rune, size int, err error)

// ReadSlice 读取直到输入中第一次出现 delim，并返回一个指向缓冲区中字节的片段。下一次读取时，字节不再有效。
// 如果 ReadSlice 在找到分隔符之前遇到错误，它会返回缓冲区中的所有数据和错误本身（通常为 io.EOF）
// 如果缓冲区在没有分隔符的情况下被填满，ReadSlice 将以错误 ErrBufferFull 失败。由于 ReadSlice 返回的数据会被下一次 I/O 操作覆盖，大多数客户端应使用 Reader.ReadBytes 或 ReadString 代替
// 如果且仅如果行未以分隔符结束，ReadSlice 才会返回 err != nil。
func (b *Reader) ReadSlice(delim byte) (line []byte, err error)

// ReadString 一直读到输入中第一次出现分隔符为止，并返回一个包含分隔符（含分隔符）之前数据的字符串
// 如果 ReadString 在找到分隔符之前遇到错误，它会返回错误之前读取的数据和错误本身（通常为 io.EOF）。只有当返回的数据不是以 delim 结束时，ReadString 才会返回 err != nil。对于简单的用途，使用 Scanner 可能更方便。
func (b *Reader) ReadString(delim byte) (string, error)

// Reset丢弃缓冲中的数据，清除任何错误，将 b 重设为其下层从 r 读取数据。
func (b *Reader) Reset(r io.Reader)

// Size 返回底层缓冲区的大小（字节）。
func (b *Reader) Size() int

// UnreadByte 会取消读取最后一个字节。只有最近读取的字节才能被解读。
func (b *Reader) UnreadByte() error

// UnreadRune 会取消读取最后一个符文
// 如果 Reader 最近调用的方法不是 Reader.ReadRune，Reader.UnreadRune 将返回错误。(在这方面，Reader.UnreadByte 比 Reader.UnreadByte 更严格，后者会从任何读取操作中取消读取最后一个字节）
func (b *Reader) UnreadRune() error

// WriteTo 实现了 io.WriterTo。这可能会多次调用底层阅读器的 Reader.Read 方法
// 如果底层 Reader 支持 Reader.WriteTo 方法，则调用底层阅读器的 Reader.WriteTo 方法时无需缓冲
func (b *Reader) WriteTo(w io.Writer) (n int64, err error)
```

### 示例：`Reader` 使用

```go
package main

import (
	"bufio"
	"fmt"
	"log"
	"os"
)

func main() {
	
	file, err := os.Open("colon_21HT000006.csv")
	if err != nil {
		log.Fatal(err)
	}
	
	reader := bufio.NewReader(file)
	
	for {
		line, err := reader.ReadString('\n')
		if err != nil {
			log.Println(err)
			break
		}
		fmt.Printf("line: %s\n", line)
	}
}
```

## `Scanner` 类型

```go
type Scanner struct {
	r            io.Reader // The reader provided by the client.
	split        SplitFunc // The function to split the tokens.
	maxTokenSize int       // Maximum size of a token; modified by tests.
	token        []byte    // Last token returned by split.
	buf          []byte    // Buffer used as argument to split.
	start        int       // First non-processed byte in buf.
	end          int       // End of data in buf.
	err          error     // Sticky error.
	empties      int       // Count of successive empty tokens.
	scanCalled   bool      // Scan has been called; buffer is in use.
	done         bool      // Scan has finished.
}
```

### 构造函数: `bufio.NewScanner`

`NewScanner` 返回一个从 `r` 读取的新扫描仪

```go
func NewScanner(r io.Reader) *Scanner
```

### `Scanner` 的方法

```go
// Buffer 设置扫描时要使用的初始缓冲区，以及扫描时可能分配的最大缓冲区大小
// 最大标记大小必须小于 max 和 cap(buf) 的较大值。如果 max <= cap(buf)，
// Scanner.Scan 将只使用该缓冲区，而不进行分配。
func (s *Scanner) Buffer(buf []byte, max int)

// Bytes 返回调用 Scanner.Scan 生成的最新标记
// 底层数组可能指向会被后续 Scan 调用覆盖的数据。它不进行分配。
func (s *Scanner) Bytes() []byte

// Err 返回扫描仪遇到的第一个非 EOF 错误。
func (s *Scanner) Err() error

// Scan 会将扫描仪推进到下一个标记，然后可通过 Scanner.Bytes 或 Scanner.Text 方法获得下一个标记
// 当没有更多标记时，它会返回 false，返回的原因可能是输入结束或出现错误
// Scan 返回 false 后，Scanner.Err 方法将返回扫描过程中发生的任何错误，但如果是 io.EOF，Scanner.Err 将返回 nil
// 如果 split 函数在不推进输入的情况下返回过多的空标记，Scan 就会 panic。这是扫描程序常见的错误模式。
func (s *Scanner) Scan() bool

// Split 设置扫描仪的 split 函数。默认的分割功能是 ScanLines
// 如果在 Scan 开始后调用 Split 就会崩溃
func (s *Scanner) Split(split SplitFunc)

// Text 将调用 Scanner.Scan 生成的最新标记返回为一个新分配的字符串，其中包含其字节。
func (s *Scanner) Text() string
```

### 示例：`Scanner` 类型

```go
package main

import (
	"bufio"
	"fmt"
	"log"
	"os"
)

func main() {
	file, err := os.Open("colon_21HT000006.csv")
	if err != nil {
		log.Fatal(err)
	}
	scanner := bufio.NewScanner(file)
	for scanner.Scan() {
		fmt.Println(scanner.Text())
	}
	if err := scanner.Err(); err != nil {
		log.Println(err)
	}
}
```

## `Writer` 类型

```go
type Writer struct {
	err error
	buf []byte
	n   int
	wr  io.Writer
}
```

### 构造函数: `bufio.NewWriter, bufio.NewWriterSize` 

`NewWriter` 返回一个新的 `Writer`，其缓冲区大小为默认值。如果参数 `io.Writer` 已经是一个缓冲区足够大的 `Writer`，它将返回底层 `Writer`

```go
func NewWriter(w io.Writer) *Writer
```

`NewWriterSize` 会返回一个缓冲区至少有指定大小的新 `Writer`。如果参数 `io.Writer ` 已经是一个足够大的 `Writer`，它将返回底层的 `Writer`

```go
func NewWriterSize(w io.Writer, size int) *Writer
```

### `Writer` 的方法

```go
// Available 返回缓冲区中未使用的字节数
func (b *Writer) Available() int

// AvailableBuffer 返回一个容量为 b.Available() 的空缓冲区
// 该缓冲区将被追加并传递给紧随其后的 Writer.Write 调用
// 该缓冲区只在下次对 b.AvailableBuffer 进行写操作之前有效
func (b *Writer) AvailableBuffer() []byte

// Buffered 返回已写入当前缓冲区的字节数
func (b *Writer) Buffered() int

// Flush 会将缓冲数据写入底层的 io.Writer
func (b *Writer) Flush() error

// ReadFrom 实现了 io.ReaderFrom。如果底层 Writer 支持 ReadFrom 方法，则调用底层 ReadFrom
// 如果有缓冲数据和底层的 ReadFrom，则在调用 ReadFrom 之前先填充缓冲区并写入数据。
func (b *Writer) ReadFrom(r io.Reader) (n int64, err error)

// Reset丢弃缓冲中的数据，清除任何错误，将b重设为将其输出写入w。
func (b *Writer) Reset(w io.Writer)

// Size 返回底层缓冲区的大小（字节）。
func (b *Writer) Size() int

// Write 将 p 的内容写入缓冲区。它会返回写入的字节数
// 如果 nn < len(p)，它还会返回一个错误信息，解释为何写入的内容较少。
func (b *Writer) Write(p []byte) (nn int, err error)

// WriteByte 写入一个字节。
func (b *Writer) WriteByte(c byte) error

// WriteRune 写入单个 Unicode 代码点，并返回写入的字节数和错误信息
func (b *Writer) WriteRune(r rune) (size int, err error)

// WriteString 写入字符串。它会返回写入的字节数。如果计数小于 len(s)，它还会返回一个错误信息，解释为什么写入的字符串较短。
func (b *Writer) WriteString(s string) (int, error)
```

## 示例：`Scanner` 和 `Writer` 的使用

```go
package main

import (
	"bufio"
	"log"
	"os"
)

func main() {
	file, err := os.Open("colon_21HT000006.csv")
	if err != nil {
		log.Fatal(err)
	}
	
	scanner := bufio.NewScanner(file)

	newFile, err := os.Create("bak.csv")
	if err != nil {
		log.Fatal(err)
	}

	writer := bufio.NewWriter(newFile)

	for scanner.Scan() {
		writer.WriteString(scanner.Text())
	}

	if err := scanner.Err(); err != nil {
		log.Println(err)
	}
}
```
