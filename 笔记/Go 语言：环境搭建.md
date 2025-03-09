# 环境搭建

Go 语言是由 Google 开源的静态编译型语言。从 2006 年开始，2009 年开源，国内快速发展。Go 的核心作者有三位

> [!tip] Ken Thompson
> 
> 1966年，`Ken Thompson` 就职于贝尔实验室，参与了 `MULTICS` 项目，独自开发了 B 语言，并利用一个月时间用 B 语言开发了一个精简的操作系统，起名 `UNICS`
> 
> 后来，同事 `Dennis Ritchie` 基于 B 语言发明了 C 语言。他们一起用 C 重写了`Unix` 系统
> 
> 2007 年，和 `Rob Pike`、`Robert Griesemer` 起设计了做出的 Go 语言
> 

> [!tip] Rob Pike
> 
> `UTF-8` 两个发明人之一。`Rob Pike` 是贝尔实验室 Unix 组成员，也是《Unix环境编程》和《程序设计实践》作者之一。1992 年和 `Ken Thompson` 共同设计了 UTF-8 编码。2002年加入 Google，研究操作系统
> 
> Go 设计团队第一任老大。如今也退体并被谷歌尊养起来了。`Rob Pike` 仍旧活跃在各个 Go 论坛组中，适当地发表自己的意见
> 

> [!tip] Robert Griesemer
> 
> Go语言三名最初的设计者之一，比较年轻。曾参与 V8 Javascript 引擎和 Java Hotspot 成拟机的研发。目前主要维护 Go 白皮书和代码解析器等
> 

2007 年 9 月 20 日，身处 Google 的 `Rob Pike` 对不断扩充新特性的 C++ 非常不满，与 `Robert Griesemer`(之前参与 `JavaScript V8` 引擎和 `Java HotSpot` 虚拟机)、`Ken Thompson` 进行了一次关于设计一门新语言的讨论。第二天，三人继续进行了对新语言设计的讨论会，并在会后有 `Robert Griesemer` 发出了一封邮件，总结了设计思路：**要在 C 语言基础上，修正一些明显缺陷，删除一些被诟病的特征，增加一些缺失的功能**

## 在 Linux 上安装 Go 编译器

首先，需要使用 `wget` 从 [Go 官网](https://go.dev/dl/) 上下载最新版本的 Go 编译器归档文件

```shell
wget https://go.dev/dl/go1.23.4.linux-amd64.tar.gz
```

然后，删除先前安装在 `/usr/local/go` 目录中的 Go 编译器并解压最新版本的 Go 编译到 `/usr/local/go` 目中

```shell
sudo rm -rf /usr/local/go && tar -C /usr/local -xzf go1.23.4.linux-amd64.tar.gz
```

> [!attention] 
> 
> 不要将归档文件解压到现有的 `/usr/local/go` 目录中。众所周知，这会产生错误的 Go 安装
> 

最后，将 `/usr/local/go/bin` 添加到 PATH 环境变量。Linux 可以在 `/etc/profile` 或 `$HOME/.profile` 中配置 PATH 环境变量。只需要将下面代码添加到 `/etc/profile` 或者 `$HOME/.profile` 中的一个即可

```shell
export PATH=$PATH:/usr/local/go/bin
```

> [!attention] 
> 
> 注意：对配置文件所做的更改可能要等到下次登录计算机时才会应用。要立即应用更改，只需直接运行 shell 命令或使用诸如 `source $HOME/.profile` 之类的命令从配置文件中执行它们
> 

打开命令提示符并键入以下命令，验证您是否已安装 Go

```shell
go version
```

## 在 Windows 上安装 Go 编译器

在 Windows 上的 Go 安装程序会自动将 Go 编译器安装 `C:/Program Files` 或者 `C:/Program Files(x86)` 中。我们可以根据自己的需求修改安装位置。当安装程序运行完毕时会自动添加 PATH 环境变量

![[Pasted image 20250107234908.png]]

![[Pasted image 20250107235000.png]]

![[Pasted image 20250107235051.png]]

![[Pasted image 20250107235117.png]]

![[Pasted image 20250107235348.png]]

## 配置 GOPROXY

Go1.14 版本之后，推荐使用 `go mod` 模式来管理依赖环境了，也不再强制我们把代码必须写在 `GOPATH` 下面的 `src` 目录了，你可以在你电脑的任意位置编写 `go` 代码

默认 `GOPROXY` 配置是：`GOPROXY=https://proxy.golang.org,direct`，由于国内访问不到`https://proxy.golang.org`，所以我们需要换一个 `PROXY`，这里推荐使用
+ `https://goproxy.io`
+ `https://goproxy.cn`

可以执行下面的命令修改 `GOPROXY`

```shell
go env -w GOPROXY=https://goproxy.cn,https://goproxy.io,direct
```

## 第一个 Go 程序 

Go 语言默认启用了 `go mod` 依赖管理工具。我们的所有代码都需要使用 `go mod` 进行管理。首先，创建一个文件夹，然后执行 `go mod` 命令

```shell 
mkdir hello && cd hello 
go mod init hello 
```

然后，使用 Visual Studio Code 打开文件夹，新建一个文件 `hello.o` ，键入如下内容
 
```go 
package main

import "fmt"

func main() {
	fmt.Print("hello, world!\n") 
} 
```

使用 `go build` 命令可以将上述代码编译为可执行文件，然后执行执行即可

```shell
➜ hello go build hello.go 
➜ hello ./hello      
Hello world
```

或者直接使用 `go run` 命令

```shell
➜ hello go run hello.go   
Hello world
```
