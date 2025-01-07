# 环境安装

Go 语言是由 Google 开源的静态编译型语言。从 2006 年开始设计，并于 2009 年开源。Go 语言在国内发展非常迅速

Go 语言核心作者有 3 位：肯·汤普逊 (Ken Thompson)、罗勃·派克 (Rob Pike)、羅伯特·格瑞史莫 (Robert Griesemer)

> [!tip] 肯·汤普逊 (Ken Thompson)
> 
> 1966年，`Ken Thompson` 就职于贝尔实验室，参与了 `MULTICS` 项目，独自开发了 B 语言，并利用一个月时间用 B 语言开发了一个精简的操作系统，起名 `UNICS`。
> 
> 后来，同事 `Dennis Ritchie` 基于 B 语言发明了 C 语言。他们一起用 C 重写了Unix 系统
> 
> 2007 年，老爷子和 `Rob Pike`、`Robert Griesemer` 起设计了做出的 Go 语言
> 

> [!tip] 罗勃·派克 (Rob Pike)
> 
> `UTF-8` 两个发明人之一。`Rob Pike` 是贝尔实验室 Unix 组成员，也是《Unix环境编程》和《程序设计实践》作者之一
> 
> 1992 年和 `Ken Thompson` 共同设计了 UTF-8 编码。2002年加入 Google，研究操作系统
> 
> Go 设计团队第一任老大。如今也退体并被谷歌尊养起来了。`Rob Pike` 仍旧活跃在各个 Go 论坛组中，适当地发表自己的意见
> 

> [!tip] 羅伯特·格瑞史莫 (Robert Griesemer)
> 
> Go语言三名最初的设计者之一，比较年轻。曾参与 V8 Javascript 引擎和 Java Hotspot 成拟机的研发。目前主要维护 Go 白皮书和代码解析器等
> 



2007 年 9 月 20 日，身处 Google 的 `Rob Pike` 对不断扩充新特性的 C++ 非常不满，与 `Robert Griesemer`(之前参与 `JavaScript V8` 引擎和 `Java HotSpot` 虚拟机)、`Ken Thompson` 进行了一次关于设计一门新语言的讨论。第二天，三人继续进行了对新语言设计的讨论会，并在会后有 `Robert Griesemer` 发出了一封邮件，总结了设计思路：**要在 C 语言基础上，修正一些明显缺陷，删除一些被诟病的特征，增加一些缺失的功能**。

2007年9月25日，Rob Pike在一封回复邮件中把新语言命名为 Go。2009年10月30日，Go 语言首次公之于众，**2009年11月10日正式开源**，这一天被 Go 官方确定为 Go 语言诞生日

## Windows 上 Go 环境安装

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

Scoop 安装完成后，可以使用 `scoop` 命令进行工具包安装

```powershell
scoop install git  # go 下载第三方包需要的依赖
scoop install go
```

> [!tip] 
> 
> scoop 会自动配置 PATH 环境变量

## Linux 上 Go 环境安装

在 Linux 上，可以使用各个发行版的包管理工具直接安装 Go 编译器

```shell
# 在 Ubuntu 上可以使用 apt 或者 snap 安装 Go 编译器
sudo apt install golang-go  # 会将 go 编译器安装在 /user/lib/go 中
sudo　snap install --classic go 
```

此外，还可以在 [Go官网](https://go.dev/) 上下载二进制压缩包

首先，使用 `wget` 命令下载 Go 编译器二进制压缩包

```shell
wget https://golang.google.cn/dl/go1.22.2.linux-amd64.tar.gz
```

然后，删除系统中存在的 Go 编译器，然后最新的二进制压缩包解压

```shell
sudo rm -rf /usr/local/go && sudo tar -C /usr/local -xzvf go1.22.2.linux-amd64.tar.gz
```

最后，配置环境变量。Linux 下有两个文件可以配置环境变量，其中 `/etc/profile` 是对所有用户生效的；`$HOME/.profile` 是对当前用户生效的，根据自己的情况自行选择一个文件打开，添加如下两行代码，保存退出

```shell
export GOROOT=/usr/local/go
export PATH=$PATH:$GOROOT/bin
```

> [!tip] 配置GOPATH：虽然现在不需要了
> ```shell
> export GOPATH=${HOME}/go
> export PATH=${PATH}:${GOPATH}/bin
> ```

Go 包需要使用 `git` 工具，应该在此时安装好。详细参考 [[版本控制：git]]

```shell
sudo apt install git  # Ubuntu 系列

sudo pacman -Syy git  # Archlinux
```

## 配置 GOPROXY

Go1.14 版本之后，都推荐使用 `go mod` 模式来管理依赖环境了，也不再强制我们把代码必须写在 `GOPATH` 下面的 `src` 目录了，你可以在你电脑的任意位置编写 `go` 代码

默认 `GOPROXY` 配置是：`GOPROXY=https://proxy.golang.org,direct`，由于国内访问不到`https://proxy.golang.org`，所以我们需要换一个 `PROXY`，这里推荐使用
+ `https://goproxy.io`
+ `https://goproxy.cn`

可以执行下面的命令修改 `GOPROXY`

```shell
go env -w GOPROXY=https://goproxy.cn,https://goproxy.io,direct
```
