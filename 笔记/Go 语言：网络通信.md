# 网络通信

学习网络通信之前我们应该了解一下网络协议。由于网络协议我们已经学习过了，这里不再重复赘述，详细参考
+ [[网络：链路层协议]]：以太网协议和ARP协议
+ [[网络：网络层协议]]：IP 协议
+ [[网络：传输层协议]]：TCP 和 UDP 协议

网络编程需要使用 SOCKET，我们在 [[Linux 系统编程：SOCKET 概览]] 中介绍了 SOCKET 的概念。Go 语言对 SOCKET 进行了一层封装，让那些 **需要阻塞的网络 IO 操作在执行前让出 CPU 控制**，从而形成了标准包 `net` ，下面我们学习使用 `net` 包进行网络编程

## 流式 SOCKET

下图展示了流式 SOCKET 的编程模型。Go 语言对 SOCKET 的操作进行了一层封装，提供了更简洁的编程接口

![[Pasted image 20241120225128.png]]

在 `net` 包中提供了一个函数 `net.ListenTCP(network, laddr)`，该函数会创建并返回一个 `net.TCPListener` 结构体对象的指针和一个错误。`net.TCPListener` 结构体定义如下

```go
type TCPListener struct {
	fd *netFD        // 代表 SOCKET 文件描述符
	lc ListenConfig  // 监听套接字的配置
}
```

`net.TCPListener` 结构体实现了 `net.Listener` 接口，`net.Listener` 接口定义如下

```go
type Listener interface {
	// Accept 等待并接收客户端的连接，返回一个对等的 Conn 接口
	Accept() (Conn, error)

	// Close 关闭 SOCKET。任何阻塞的 Accept 操作将被解除阻塞并返回错误。.
	Close() error

	// Addr 返回 SOCKET 监听的地址
	Addr() Addr
}
```

调用 `net.TCPListener` 的 `Accept()` 方法可以接收一个客户端的连接，并返回一个 `net.Conn` 接口和一个错误。其中 `net.Conn` 接口定义如下

```go
// Conn是一个通用的面向流的网络连接。
//
// 多个 Goroutines 可以同时调用一个 Conn 上的方法。
type Conn interface {
	// Read 从 SOCKET 接收缓冲区 中读取数据。如果接收缓冲区为空，则阻塞
	// 可以设置 Read 超时并在固定时间限制后返回错误；
	// 参见 SetDeadline 和SetReadDeadline。
	Read(b []byte) (n int, err error)

	// Write 向 SOCKET 发送缓冲区 写入数据。如果发送缓冲区没有空间写入数据，则阻塞
	// Write 操作可以超时并在固定的时间限制后返回错误；
	// 参见SetDeadline和SetWriteDeadline。
	Write(b []byte) (n int, err error)

	// Close关闭连接。任何阻塞的 Read 或写 操作将被解除阻塞并返回错误。
	Close() error

	// LocalAddr返回本地网络地址（如果已知）
	LocalAddr() Addr

	// RemoteAddr 返回远程网络地址（如果已知）。
	RemoteAddr() Addr

	SetDeadline(t time.Time) error

	SetReadDeadline(t time.Time) error

	SetWriteDeadline(t time.Time) error
}
```

函数 `net.ListenTCP()` 需要两个参数，分别代表

| 参数        | 描述                                                               |
| :-------- | :--------------------------------------------------------------- |
| `network` | 代表通信域的字符串。可选 `"tcp"` `"tcp4"` `"tcp6"` `"unix"` 和 `"unixpacket"` |
| `laddr`   | `TCPAddr` 类型的指针。代表一个 TCP 监听地址                                    |

在 Go 语言中，使用 `net.ResolveTCPAddr(network, address)` 函数，它将会解析参数 `address` (代表地址的字符串)，创建 `TCPAddr` 对象并返回该对象的指针

客户端发起连接需要使用 `net.Dail(network, address)` 函数，它会向 `address` 指定的地址发起连接，并返回一个 `net.Conn` 接口对象和一个错误。之后就可以使用 `net.Conn` 接口提供的方法与服务端进行通信了

### 示例：回显服务器

回显服务器作为网络编程的一个经典示例，该示例展示了大部分网络IO接口的使用









