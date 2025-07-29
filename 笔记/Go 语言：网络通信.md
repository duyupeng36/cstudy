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

#### 服务端

```go
package main

import (
	"bytes"
	"fmt"
	"log"
	"net"
	"runtime"
)

func main() {
	localTCPAddr, err := net.ResolveTCPAddr("tcp", "localhost:8080") // 解析字符串形式的地址，返回 TCPAddr 结构体的指针
	if err != nil {
		log.Fatal(err)
	}

	// 创建 socket 绑定地址 开启监听这三步被封装成了一个函数
	server, err := net.ListenTCP("tcp", localTCPAddr)
	if err != nil {
		log.Fatal(err)
	}

	// 不断的接受客户端的连接
	for {

		conn, err := server.Accept()
		if err != nil {
			log.Printf("接受客户端连接失败，进行下一次接受")
		}

		// 重启一个 Goroutine 处理与客户端的通信，避免阻塞后续的客户端连接
		go func(conn net.Conn) {
			defer func() {
				if err := conn.Close(); err != nil {
					log.Printf("关闭与客户端 %v 的连接失败: %v", conn.RemoteAddr(), err)
					runtime.Goexit()
				}
			}()

			var buf [4096]byte
			for {
				n, err := conn.Read(buf[:])

				if n == 0 {
					log.Printf("客户端 %v 主动关闭连接\n", conn.RemoteAddr().String())
					break
				}

				if err != nil {
					log.Printf("读取出现错误")
					break
				}

				fmt.Printf("客户端发来消息: %s\n", buf[:n])
				conn.Write(bytes.ToUpper(buf[:n]))
			}

		}(conn)
	}
}
```

#### 客户端

```go
package main

import (
	"io"
	"log"
	"net"
	"os"
)

func catchErr(err error) {
	if err != nil {
		log.Fatal(err)
	}
}

func main() {
	remoteTCPAddr, err := net.ResolveTCPAddr("tcp", "localhost:8080")
	catchErr(err)

	conn, err := net.DialTCP("tcp", nil, remoteTCPAddr)
	catchErr(err)

	done := make(chan struct{})
	// 启动一个 Goroutine 用于处理服务端的返回
	go func(conn net.Conn) {
		_, err := io.Copy(os.Stdout, conn)
		if err != nil {
			log.Println("done")
		}
		done <- struct{}{}
	}(conn)

	// main Goroutine 处理向客户端发送数据
	_, err = io.Copy(conn, os.Stdin)
	if err != nil {
		log.Println(err)
	}
	conn.Close()
	<-done
}
```

### 粘包问题

#### 服务端

下面是用于演示粘包问题的服务端代码

```go
package main

import (
	"bufio"
	"fmt"
	"io"
	"net"
)

func process(conn net.Conn) {
	defer conn.Close()
	reader := bufio.NewReader(conn)
	var buf [1024]byte
	for {
		n, err := reader.Read(buf[:])
		if err == io.EOF {
			break
		}
		if err != nil {
			fmt.Println("read from client failed, err:", err)
			break
		}
		recvStr := string(buf[:n])
		fmt.Println("收到client发来的数据：", recvStr)
	}
}

func main() {

	listen, err := net.Listen("tcp", "127.0.0.1:30000")
	if err != nil {
		fmt.Println("listen failed, err:", err)
		return
	}
	defer listen.Close()
	for {
		conn, err := listen.Accept()
		if err != nil {
			fmt.Println("accept failed, err:", err)
			continue
		}
		go process(conn)
	}
}
```

#### 客户端

```go
package main

import (
	"fmt"
	"net"
)

// socket_stick/client/main.go

func main() {
	conn, err := net.Dial("tcp", "127.0.0.1:30000")
	if err != nil {
		fmt.Println("dial failed, err", err)
		return
	}
	defer conn.Close()
	for i := 0; i < 20; i++ {
		msg := `Hello, Hello. How are you?`
		conn.Write([]byte(msg))
	}
}
```

将上面的代码保存后，分别编译。先启动服务端再启动客户端，可以看到服务端输出结果如下：

```
收到client发来的数据： Hello, Hello. How are you?
收到client发来的数据： Hello, Hello. How are you?Hello, Hello. How are you?Hello, Hello. How are you?Hello, Hello. How are you?
收到client发来的数据： Hello, Hello. How are you?
收到client发来的数据： Hello, Hello. How are you?
收到client发来的数据： Hello, Hello. How are you?
```

客户端分 20 次发送的数据，在服务端并没有成功的输出 20 次，而是 **多条数据“粘”到了一起**。

#### 原因

> [!question] 为什么会出现粘包?
> 
> 主要原因就是 **TCP 数据传递模式是流模式**，在保持长连接的时候可以进行多次的收和发。

**“粘包"** 可可以发生在发送端，也可以发生在接收端

> [!tip] 发送端粘包：由拥塞避免算法（例如，Nagle 算法）
>`Nagle` 算法是一种改善网络传输效率的算法。简单来说就是当我们提交一段数据给 TCP 发送时，**TCP 并不立刻发送此段数据**，而是等待一小段时间看看在等待期间是否还有要发送的数据，若有则 **会一次把这多段数据发送出去**


> [!tip] 接收端粘包：接收数据不及时
> 
> TCP 会把接收到的数据存在自己的缓冲区中，然后通知应用层取数据。当应用层由于某些原因不能及时的把 TCP 的数据取出来，就会造成 TCP 缓冲区中存放了几段数据

#### 解决方案

出现"粘包"的关键在于 **_接收方不确定将要传输的数据包的大小_**，因此我们可以对数据包进行封包和拆包的操作

**封包**：封包就是 **_给一段数据加上包头_**，这样一来数据包就分为包头和包体两部分内容了(过滤非法包时封包会加入"包尾"内容)。包头部分的长度是固定的，并且它存储了包体的长度，根据包头长度固定以及包头中含有包体长度的变量就能正确的拆分出一个完整的数据包

我们可以自己定义一个协议，比如数据包的前 $4$ 个字节为包头，里面存储的是发送的数据的长度

```go
// socket_stick/proto/proto.go
package proto

import (
	"bufio"
	"bytes"
	"encoding/binary"
)

// Encode 将消息编码
func Encode(message string) ([]byte, error) {
	// 读取消息的长度，转换成int32类型（占4个字节）
	var length = int32(len(message))
	var pkg = new(bytes.Buffer)
	// 写入消息头
	err := binary.Write(pkg, binary.LittleEndian, length)
	if err != nil {
		return nil, err
	}
	// 写入消息实体
	err = binary.Write(pkg, binary.LittleEndian, []byte(message))
	if err != nil {
		return nil, err
	}
	return pkg.Bytes(), nil
}

// Decode 解码消息
func Decode(reader *bufio.Reader) (string, error) {
	// 读取消息的长度
	lengthByte, _ := reader.Peek(4) // 读取前4个字节的数据
	lengthBuff := bytes.NewBuffer(lengthByte)
	var length int32
	err := binary.Read(lengthBuff, binary.LittleEndian, &length)
	if err != nil {
		return "", err
	}
	// Buffered返回缓冲中现有的可读取的字节数。
	if int32(reader.Buffered()) < length+4 {
		return "", err
	}

	// 读取真正的消息数据
	pack := make([]byte, int(4+length))
	_, err = reader.Read(pack)
	if err != nil {
		return "", err
	}
	return string(pack[4:]), nil
}
```

接下来在服务端和客户端分别使用上面定义的`proto`包的`Decode`和`Encode`函数处理数据

服务端

```go
// socket_stick/server2/main.go

func process(conn net.Conn) {
	defer conn.Close()
	reader := bufio.NewReader(conn)
	for {
		msg, err := proto.Decode(reader)
		if err == io.EOF {
			return
		}
		if err != nil {
			fmt.Println("decode msg failed, err:", err)
			return
		}
		fmt.Println("收到client发来的数据：", msg)
	}
}

func main() {

	listen, err := net.Listen("tcp", "127.0.0.1:30000")
	if err != nil {
		fmt.Println("listen failed, err:", err)
		return
	}
	defer listen.Close()
	for {
		conn, err := listen.Accept()
		if err != nil {
			fmt.Println("accept failed, err:", err)
			continue
		}
		go process(conn)
	}
}
```

客户端

```go
// socket_stick/client2/main.go

func main() {
	conn, err := net.Dial("tcp", "127.0.0.1:30000")
	if err != nil {
		fmt.Println("dial failed, err", err)
		return
	}
	defer conn.Close()
	for i := 0; i < 20; i++ {
		msg := `Hello, Hello. How are you?`
		data, err := proto.Encode(msg)
		if err != nil {
			fmt.Println("encode msg failed, err:", err)
			return
		}
		conn.Write(data)
	}
}
```

## 数据报 SOCKET

下图显示了数据报 SOCKET 的相关系统调用的使用

![[Pasted image 20241120233738.png]]

### 服务端

由于 **UDP 是“无连接”的**，所以，服务器端 **不需要额外创建监听套接字**， 只需要指定好 IP 和port，然后 **监听该地址**，等待客户端与之建立连接，即可通信

- 创建`UDPAddr`: `func ResolveUDPAddr(network, address string) (*UDPAddr, error)`
- 创建用于通信的`socket`: `func ListencUDP(network string, laddr *UDPAddr) (*UDPConn, error)`
- 接收 udp 数据: `func (c *UDPConn) ReadFromUDP(b []byte) (int, *UDPAddr, error)`
- 写出数据到 udp: `func (c *UDPConn) WriteToUDP(b []byte, addr *UDPAddr) (int, error)`

```go
// UDP/server/main.go

// UDP server端
func main() {
	listen, err := net.ListenUDP("udp", &net.UDPAddr{
		IP:   net.IPv4(0, 0, 0, 0),
		Port: 30000,
	})
	if err != nil {
		fmt.Println("listen failed, err:", err)
		return
	}
	defer listen.Close()
	for {
		var data [1024]byte
		n, addr, err := listen.ReadFromUDP(data[:]) // 接收数据
		if err != nil {
			fmt.Println("read udp failed, err:", err)
			continue
		}
		fmt.Printf("data:%v addr:%v count:%v\n", string(data[:n]), addr, n)
		_, err = listen.WriteToUDP(data[:n], addr) // 发送数据
		if err != nil {
			fmt.Println("write to udp failed, err:", err)
			continue
		}
	}
}
```

### 客户端

使用Go语言的`net`包实现的 UDP 客户端代码与 TCP 客户端代码相似

```go
// UDP 客户端
func main() {
	socket, err := net.DialUDP("udp", nil, &net.UDPAddr{
		IP:   net.IPv4(0, 0, 0, 0),
		Port: 30000,
	})
	if err != nil {
		fmt.Println("连接服务端失败，err:", err)
		return
	}
	defer socket.Close()
	sendData := []byte("Hello server")
	_, err = socket.Write(sendData) // 发送数据
	if err != nil {
		fmt.Println("发送数据失败，err:", err)
		return
	}
	data := make([]byte, 4096)
	n, remoteAddr, err := socket.ReadFromUDP(data) // 接收数据
	if err != nil {
		fmt.Println("接收数据失败，err:", err)
		return
	}
	fmt.Printf("recv:%v addr:%v count:%v\n", string(data[:n]), remoteAddr, n)
}
```
