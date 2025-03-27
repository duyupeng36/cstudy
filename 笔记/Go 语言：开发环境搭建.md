# 开发环境搭建

## 安装 Go

在 [Go 安装](https://go.dev/doc/install) 的官方文档中，讲述了如何快速下载并按照 Go。下面我们以 Linux 系统为例，介绍如何安装 Go 编译器

需要下载最新版本的 Go 编译器二进制分发文件

```shell
wget https://go.dev/dl/go1.24.1.linux-amd64.tar.gz
```

删除 `/usr/local/go` 文件夹（如果存在）从而删除任何以前安装的 Go。然后将刚刚下载的存档解压到 `/usr/local`，这会在 `/usr/local/go` 中创建一个新的 Go 目录树：

```shell
 sudo rm -rf /usr/local/go && sudo tar -C /usr/local -xzf go1.24.1.linux-amd64.tar.gz
```

将 `/usr/local/go/bin` 目录添加到 `${HOME}/.profile` 或者 `/etc/profile` 中的 `PATH` 变量中

```shell
export PATH=$PATH:/usr/local/go/bin
```

最后，使用 `source` 命令让配置文件重新生效

```shell
source ${HOME}/.profile
```

通过打开命令提示符并键入以下命令来验证是否已成功安装 Go

```shell
➜ ~ go version
go version go1.24.1 linux/amd64
```

## 配置环境变量

我们需要配置两个环境变量 `GOROOT` 和 `GOPATH`。`GOROOT` 环境变量是 Go 的安装目录，而 `GOPATH` 是 Go 源文件的存放目录。然而，在 Go1.14 版本之后，Go 语言启用了 Go Module 用于管理 Go 语言的包，这样我们就可以在任意目录中编写 Go 代码，所以就不在需要配置 `GOPATH`

> [!tip] 
> 
> Go 语言默认设置了 `GOPATH` 的值为 `${HOME}/go`
> 

由于我们可能需要安装一些 Go 语言开发的工具，默认情况下，这些工具安装在 `${GOPATH}/bin` 中。因此，配置 `GOPATH` 环境变量也是有必要的。

```shell
export GOPATH=${HOME}/go
export PATH=${PATH}:${GOPATH}/bin
```

### GOPROXY

此外，还有一个非常重要的环境变量 `GOPROXY`。由于在 Go1.14 版本之后，推荐使用 Go Module 管理 Go 项目的依赖。

然而，在下载这些依赖的时候可能会由于网络环境的原因无法下载，因为默认的 `GOPROXY` 使用的是 Google 官方的镜像源：`GOPROXY=https://proxy.golang.org,direct`

这里推荐使用 `https://goproxy.io`或`https://goproxy.cn` 镜像代理。使用下面的命令修改 `GOPROXY` 

```shell
go env -w GOPROXY=https://goproxy.cn,https://goproxy.io,direct
```


## 安装 Visual Studio Code

Visual Studio Code 是微软公司开源的一款 **免费** 现代化轻量级代码编辑器，支持几乎所有主流的开发语言的语法高亮、智能代码补全、自定义热键、括号匹配、代码片段、代码对比 Diff、GIT 等特性，支持插件扩展，支持 Win、Mac 以及 Linux平台。虽然不如某些IDE功能强大，但是它添加Go扩展插件后已经足够胜任我们日常的Go开发。


访问 Visual Studio Code 下载地址：[https://code.visualstudio.com/Download](https://code.visualstudio.com/Download)，下载适合操作系统的安装包。双击下载好的安装文件，双击安装即可

![[Pasted image 20250323184814.png]]

### 配置 Go 开发环境

首先，需要安装 Go 扩展，让 Visual Studio Code 支持 Go 语言开发

![[Pasted image 20250323184942.png]]

然后，还需下载或更新 Go 语言开发工具包。Windows平台按下`Ctrl+Shift+P`，Mac平台按`Command+Shift+P`，这个时候VS Code界面会弹出一个输入框，如下图：

![[Pasted image 20250323185108.png]]

我们在这个输入框中输入`>go:install`，下面会自动搜索相关命令，我们选择`Go:Install/Update Tools`这个命令，按下图选中并会回车执行该命令（或者使用鼠标点击该命令）

![[Pasted image 20250323185122.png]]

在弹出的窗口选中所有，并点击“确定”按钮，进行安装

![[Pasted image 20250323185138.png]]

然后会弹出如下窗口，开始安装工具：

![[Pasted image 20250323185153.png]]

等待所有工具都安装成功，如下图所示:

![[Pasted image 20250323185207.png]]
