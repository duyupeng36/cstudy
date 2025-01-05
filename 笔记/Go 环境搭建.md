
# 简介和Go环境搭建

## 简介

Go 语言是由 Google 开源的静态编译型语言。从 2006 年开始，2009年开源，国内快速发展。Go 的核心作者有三位

> [!tip] Ken Thompson
> 
> 1966年，`Ken Thompson` 就职于贝尔实验室，参与了 `MULTICS` 项目，独自开发了 B 语言，并利用一个月时间用 B 语言开发了一个精简的操作系统，起名 `UNICS`。后来，同事 `Dennis Ritchie` 基于 B 语言发明了 C 语言。他们一起用 C `重写了Unix` 系统。2007 年，老爷子和 `Rob Pike`、`Robert Griesemer` 起设计了做出的 Go 语言
> 

> [!tip] Rob Pike
> 
> `UTF-8` 两个发明人之一。`Rob Pike` 是贝尔实验室 Unix 组成员，也是《Unix环境编程》和《程序设计实践》作者之一。1992 年和 `Ken Thompson` 共同设计了 UTF-8 编码。2002年加入 Google，研究操作系统
> 
> Go 设计团队第一任老大。如今也退体并被谷歌尊养起来了。`Rob Pike` 仍旧活跃在各个 Go 论坛组中，适当地发表自己的意见
> 

> [!tip] Robert Griesemer
> 
> Go语言三名最初的设计者之一，比较年轻。曾参与 V8 Javascript 引擎和 Java Hotspot 成拟机的研发。目前主要维护 Go 白皮书和代码解析器等
> 

2007 年 9 月 20 日，身处 Google 的 `Rob Pike` 对不断扩充新特性的 C++ 非常不满，与 `Robert Griesemer`(之前参与 `JavaScript V8` 引擎和 `Java HotSpot` 虚拟机)、`Ken Thompson` 进行了一次关于设计一门新语言的讨论。第二天，三人继续进行了对新语言设计的讨论会，并在会后有 `Robert Griesemer` 发出了一封邮件，总结了设计思路：要在 C 语言基础上，修正一些明显缺陷，删除一些被诟病的特征，增加一些缺失的功能。

2007年9月25日，Rob Pike在一封回复邮件中把新语言命名为Go。

2009年10月30日，Go语言首次公之于众。

**2009年11月10日正式开源**，这一天被 Go 官方确定为 Go 语言诞生日。

Go 语言也拥有了自己的吉祥物（`Rob Pike` 夫人 `Renee French` 设计的地鼠）。Go 程序员被称为 Gopher

里程碑
+ 2012年3月28日，Go 1.0 正式发布
+ 2015年8月19日，Go 1.5 这个里程碑版本发布。
	+ Go 不在依赖 C 编译器，Go 编译器和运行时都使用 Go 代码了，实现了自举
	+ GOMAXPROCS 的默认值 1 改为运行环境的 CPU 核数
+ 2018年8月25日，Go1.11 版本发布
	+ 引入Go Module包管理机制
+ 2021年2月18日，Go 1.16 版本发布。
	+ Go Module-aware模式成为默认模式，即GO111MODULE默认从auto改为on
	+ go build/run命令不再自动更新go.mod和go.sum文件
+ 2022年3月15日，Go 1.18 版本发布
	+ 引入了泛型 Generic
	+ 支持工作区 Workspace

## Go 环境搭建

[Go官网](https://go.dev/)
### Linux

下载

```shell
wget https://golang.google.cn/dl/go1.22.2.linux-amd64.tar.gz
```

解压

```shell
sudo rm -rf /usr/local/go && sudo tar -C /usr/local -xzvf go1.22.2.linux-amd64.tar.gz
```

配置环境变量：Linux 下有两个文件可以配置环境变量，其中 `/etc/profile` 是对所有用户生效的；`$HOME/.profile` 是对当前用户生效的，根据自己的情况自行选择一个文件打开，添加如下两行代码，保存退出

```shell
export GOROOT=/usr/local/go
export PATH=$PATH:$GOROOT/bin
```

配置GOPATH：虽然现在不需要了
```shell
export GOPATH=${HOME}/go
export PATH=${PATH}:${GOPATH}/bin
```

安装 `git` 工具

```shell
sudo apt install git  # Ubuntu 系列

sudo pacman -Syy git  # Archlinux
```

### Windows

#### 安装 scoop

在 Windows 上安装 [Scoop](https://scoop.sh/) 包管理器。根据官网的提示，我们使用下面的命令进行安装

```powershell
> Set-ExecutionPolicy RemoteSigned -Scope CurrentUser
> irm get.scoop.sh | iex
```

这会安装在命令执行目录。如果需要修改安装路径等配置信息，使用离线安装脚本进行安装

```powershell
# 下载安装脚本
irm get.scoop.sh -outfile 'install.ps1'
# 安装到指定目录
.\install.ps1 -ScoopDir 'C:\scoop' -ScoopGlobalDir 'C:\scoop\global' -NoProxy
```

#### 安装 go

```powershell
scoop install git  # go 下载第三方包需要的依赖
scoop install go
```

### GOPROXY 非常重要

`Go1.14` 版本之后，都推荐使用 `go mod` 模式来管理依赖环境了，也不再强制我们把代码必须写在 `GOPATH` 下面的 `src` 目录了，你可以在你电脑的任意位置编写 `go` 代码

默认 `GOPROXY` 配置是：`GOPROXY=https://proxy.golang.org,direct`，由于国内访问不到`https://proxy.golang.org`，所以我们需要换一个 `PROXY`，这里推荐使用

- `https://goproxy.io`

- `https://goproxy.cn`

可以执行下面的命令修改 `GOPROXY`

```shell
go env -w GOPROXY=https://goproxy.cn,https://goproxy.io,direct
```

## 第一个 Go 程序

创建一个目录

```shell
mkdir hello
cd hello
```

初始化项目

```shell
go mod init example/hello
```

创建一个 `hello.go` 文件，然后编写代码

```go
// package main 申明包名为 main。main 包是编写可执行 Go 程序的入口包
package main

// import "fmt" 是将 "fmt" 包导入到 main 包的全局空间中，并使用 fmt 作为访问 "fmt" 包中导入标识符的入口
import "fmt"

// main 函数，可执行程序的入口函数
func main() {
	fmt.Printf("hello, world!\n")
}
```

使用 `go` 命令运行

```shell
$ go run .
hello, world!
```

### 包

每个 Go 程序都是由包组成的。程序从`main` 包开始运行。这个程序使用的包的导入路径为 `"fmt"` 和 `"math/rand"`。

```go
package main

import (
	"fmt"
	"math/rand"
)

func main() {
	fmt.Println("My favorite number is", rand.Intn(10))
}
```

按照惯例，**包的名称与导入路径的最后一个元素相同**。例如，`"math/rand"` 包由以语句 `package rand` 开始的文件组成

### 导入包

```go
package main

import (
	"fmt"
	"math"
)

func main() {
	fmt.Printf("Now you have %g problems.\n", math.Sqrt(7))
}
```

这段代码将导入语句分组到一个带括号的分解导入语句中

也可以编写多个导入语句，如

```go
import "fmt"
import "math"
```

> [!important] 分组导入是更好的风格

