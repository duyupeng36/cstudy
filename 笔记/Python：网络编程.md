# 网络编程

我们在 [[网络：链路层协议]] [[网络：网络层协议]] 和 [[网络：传输层协议]] 中介绍了常用的 **网络协议**。操作系统将这些协议封装并向应用程序提供访问网络的接口，我们在 [[Linux 系统编程：SOCKET 概览]] 中介绍了 Unix/Linux 提供的网络编程接口 SOCKET。

Python 在操作系统提供的网络编程接口 SOCKET 上进一步封装，以标准库 `socket` 的形式提供 Python 的网络编程接口

> [!tip] 
> 
> 这个 Python 接口是将 Unix 系统调用和套接字库接口直接转写为 Python 的面向对象风格：函数 [`socket()`](https://docs.python.org/zh-cn/3.13/library/socket.html#socket.socket "socket.socket") 返回一个 _套接字对象_，其方法是对各种套接字系统调用的实现。 形参类型相比 C 接口更高级一些：如同在 Python 文件上的 `read()` 和 `write()` 操作那样，**接受操作的缓冲区分配是自动进行的**，**发送操作的缓冲区长度则是隐式的**。
> 







