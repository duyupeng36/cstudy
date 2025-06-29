# 通用 IO 接口

标准包 `io` 提供对 I/O 原语的基本接口。 其主要任务是封装现有原语的实现，例如在包 `os` 中的实现，将其包装成共享的公共接口，以抽象功能，以及一些其他相关原语。

因为这些接口和原语用各种实现包装了底层操作，除非有其他通知，否则客户端不应假设它们对并行执行是安全的。

我们主要学习四个基本 IO 接口 `io.Reader` `io.Writer` `io.Closer`  和 `io.Seeker`

## 基本接口
### `io.Reader` 和 `io.Writer` 接口

`io.Reader` 接口是对所有实现了 `Read()` 方法的类型的抽象，即包装基本 `Read()` 方法的接口。

```go
type Reader interface {
	Read(p []byte) (n int, err error)
}
```

`Read()` 将最多 `len(p)` 个字节读取到字节切片 `p` 中。它返回读取的字节数(`0 <= n <= len(p)`)和遇到的任何错误

> [!important] 
> 
> 如果数据不能填满 `p`，那么 `Read` 会立即返回
> 
> + 即使 `Read()` 返回 `n < len(p)`，它也可能在调用期间使用整个 `p` 作为临时空间
>
> 如果一些数据是可用的，但不是 `len(p)` 字节，`Read()` 通常返回可用的数据，而不是等待更多的数据
> 

> [!attention] 
> 
> `Read()` 读取一定数据后出现错误或者遇见 `EOF` 时，返回已读取的字节数。但是对于错误的处理有两种情形
> 
> + 从同一个调用中返回错误(非 `nil`)
> + 从下一次调用中返回错误并且 `n == 0`
> 
> 一般情况的一个实例是，在输入流末尾返回非零字节数的 `Reader` 可能返回 `err == EOF` 或 `err == nil`。下一个 `Read` 应该返回 `0, EOF`
> 
> 在考虑错误 `err` 之前，调用者应该始终处理返回的 `n > 0` 字节。这样做可以正确地处理读取一些字节后发生的 IO 错误以及两种允许的 EOF 行为
> 

> [!tip] 
> 
> `len(p) == 0` 的情形始终返回 `n==0`，如果已知某些错误条件，例如 `EOF`，则可能返回非 `nil` 错误
> 
> 不鼓励 `Read()` 的实现返回带有 `nil` 错误的零字节计数，除非 `len(p) == 0`。调用者应该将返回 `0` 和 `nil` 视为没有发生任何事情；特别是它不表示 `EOF`
> 

`io.Writer`  接口是对所有实现了 `Write()` 方法的类型的抽象，即包装基本 `Write` 方法的接口。

```go
type Writer interface {
	Write(p []byte) (n int, err error)
}
```

`Write()` 从字节切片 `p` 向底层数据流写入 `len(p)` 个字节。它返回从 `p(0 <= n <= len(p))` 开始写入的字节数以及遇到的导致写入提前停止的任何错误

如果 `Write()` 返回 `n < len(p)`，则必须返回非 `nil` 错误。写操作不能修改片数据，即使是临时的

> [!hint] 
> 
> `*os.File` 对象实现了 `io.Reader` 和 `io.Writer` 接口
> 

下表列出了额外的一些实现了 `io.Reader` 和 `io.Writer` 接口的类型

| 类型                    | 描述                                 |
| :-------------------- | :--------------------------------- |
| `*os.File`            | 同时实现了 `io.Reader` 和 `io.Writer` 接口 |
| `strings.Reader`      | 实现了 `io.Reader` 接口                 |
| `bufio.Reader`        | 实现了 `io.Reader` 接口                 |
| `bufio.Writer`        | 实现了 `io.Writer` 接口                 |
| `bytes.Buffer`        | 实现了 `io.Reader` 和 `io.Writer` 接口   |
| `bytes.Reader`        | 实现了 `io.Reader` 接口                 |
| `gzip.Reader`         | 实现了 `io.Reader` 接口                 |
| `gzip.Writer`         | 实现了 `io.Writer` 接口                 |
| `cipher.StreamReader` | 实现了 `io.Reader` 接口                 |
| `cipher.StreamWriter` | 实现了 `io.Writer` 接口                 |
| `tls.Conn`            | 实现了 `io.Reader` 和 `io.Writer` 接口   |
| `csv.Reader`          | 实现了 `io.Reader` 接口                 |
| `csv.Writer`          | 实现了 `io.Writer` 接口                 |

### `io.Closer` 接口

`io.Closer` 接口是对所有实现了 `Close()` 方法的类型的抽象，即包装基本 `Close()` 方法的接口

```go
type Closer interface {
	Close() error
}
```

> [!tip] 
> 
> `*os.File` 对象实现了 `io.Closer`
> 

### `io.Seeker` 接口

`io.Seeker` 接口是对所有实现了 `Seek` 方法的类型的抽象，即包装基本 `Seek` 方法的接口。

```go
type Seeker interface {
	Seek(offset int64, whence int) (int64, error)
}
```

> [!tip] 
> 
> `*os.File` 对象实现了 `io.Seeker`
> 

我们介绍了标准包 `io` 中提供的 $4$ 个基本接口，将这些接口进行组合，形成了其他接口

```go
type ReadWriter interface {
	Reader
	Writer
}

type ReadWriteCloser interface {
	Reader
	Writer
	Closer
}

type ReadWriteSeeker interface {
	Reader
	Writer
	Seeker
}

type ReadCloser interface {
	Reader
	Closer
}

type ReadSeeker interface {
	Reader
	Seeker
}

type WriteCloser interface {
	Writer
	Closer
}

type WriteSeeker interface {
	Writer
	Seeker
}
```

## `io.ReadFrom` 和 `io.WriteTo` 接口

`io.ReaderFrom` 是包装 `ReadFrom()` 方法的接口。

```go
type ReaderFrom interface {
	ReadFrom(r Reader) (n int64, err error)
}
```

`ReadFrom()` 从实现 `Reader` 接口的对象 `r` 读取数据，直到 `EOF` 或错误。返回值 `n` 是读取的字节数。读取过程中遇到的除 `EOF` 以外的任何错误都将返回

`io.WriterTo` 是包装 `WriteTo` 方法的接口。

```go
type WriterTo interface {
	WriteTo(w Writer) (n int64, err error)
}
```

`WriteTo` 将数据写入实现了 `Writer` 接口的对象  `w`，直到没有更多的数据可写或发生错误。返回值 `n` 是写入的字节数。在写入过程中遇到的任何错误也将返回

> [!important] 文件对象 `*os.File` 实现了这两个接口
> + `file.ReadFrom(r io.Reader)` 从 `r` 中读取数据，并写入 `file` 中
> + `file.WriteTo(w io.Writer)` 将 `file` 中的数据写入到 `w` 中

## 单字节读写接口

```go
type ByteReader interface {
	ReadByte() (byte, error)  // 只读取一个字节
}

type ByteScanner interface {
	ByteReader
	UnreadByte() error // UnreadByte 导致下一次对 ReadByte 的调用返回最后读取的字节
}

type ByteWriter interface {
	WriteByte(c byte) error
}
```

## 单字符读写接口

```go
type RuneReader interface {
	ReadRune() (r rune, size int, err error)
}

type RuneScanner interface {
	RuneReader
	UnreadRune() error
}
```

## 工具函数

### `io.Copy()`

将 `src` 的数据拷贝到 `dst`，直到在 `src` 上到达 `EOF` 或发生错误。返回拷贝的字节数和遇到的第一个错误

```go
func Copy(dst Writer, src Reader) (written int64, err error)
```

对成功的调用，返回值 `err` 为 `nil` 而非 `EOF`，因为 `Copy()` 定义为从 `src` 读取直到 `EOF`，它不会将读取到`EOF` 视为应报告的错误
+ 如果 `src` 实现了 `WriterTo` 接口，本函数会调用 `src.WriteTo(dst)` 进行拷贝
+ 否则如果 `dst` 实现了 `ReaderFrom` 接口，本函数会调用`dst.ReadFrom(src)` 进行拷贝

### `io.CopyBuffer()`

`CopyBuffer()` 与 `Copy()` 相同，不同之处在于它会逐步遍历 `sr` 并使用提供的缓冲区(如果需要的话)，而不是分配一个临时的缓冲区

```go
func CopyBuffer(dst Writer, src Reader, buf []byte) (written int64, err error)
```

> [!attention] 
> 
> 如果 `buf` 为 `nil`，则分配一个；否则，如果长度为`0`,`CopyBuffer`就会 `panic`
> 

### `io.CopyN()`

`CopyN()` 从 `src` 复制 `n` 字节(或直到发生错误)到 `dst`。它返回复制的字节数和复制时遇到的最早错误。返回时，`written == n` 当且仅当 `err == nil`

```go
func CopyN(dst Writer, src Reader, n int64) (written int64, err error)
```

### `io.ReadAll()`

`ReadAll() ` 从 `r` 读取数据，直到出现错误或 `EOF`，并返回读取的数据。成功调用会返回 `err == nil`，而不是 `err == EOF`。由于 `ReadAll` 被定义为从 `src` 读取数据直到 `EOF`，因此它不会将 `Read` 的 `EOF` 视为需要报告的错误。

```go
func ReadAll(r Reader) ([]byte, error)
```

### `io.ReadAtLeast()`

`ReadAtLeast()` 从 `r` 读取数据到 `buf`，直到至少读取了 `min` 字节。如果读取的字节数较少，则返回复制的字节数和错误信息。

```go
func ReadAtLeast(r Reader, buf []byte, min int) (n int, err error)
```

只有在没有读取任何字节的情况下，才会返回 `EOF` 错误。如果在读取少于 `min` 字节后出现 `io.EOF`，`ReadAtLeast()` 将返回 `io.ErrUnexpectedEOF`。如果最小值大于 `buf` 的长度，`ReadAtLeast` 返回 `io.ErrShortBuffer`。返回时，如果且仅如果 `err == nil`，`n >= min`。如果 `r` 在读取至少 `min` 字节后返回错误，则放弃该错误

### `io.ReadFull()`

`ReadFull()` 从 `r` 中精确读取 `len(buf)` 字节到 `buf` 中。它返回拷贝的字节数，如果读取的字节数较少，则返回错误信息
- 只有在没有读取任何字节的情况下，才会出现 `io.EOF` 错误
- 如果在读取部分字节而非全部字节后出现 `EOF`，`ReadFull()` 返回 `io.ErrUnexpectedEOF`

返回时，如果且仅如果 `err == nil`，`n == len(buf)`。如果 `r` 在读取至少 `len(buf)` 字节后返回错误，则放弃该错误

```go
func ReadFull(r Reader, buf []byte) (n int, err error)
```
