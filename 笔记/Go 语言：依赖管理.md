# 依赖管理

在 Go 语言中，**包(package)** 是用于支持代码 **模块化** 和 **代码复用** 的工具，也是 Go 程序 **依赖管理** 的基本单位。一个包是由一个或多个 `.go` 文件组成，是一种高级的代码复用方案，Go 语言为我们提供了很多 [标准包](https://pkg.go.dev/std)。在前面的学习中，我们已经知道如何定以包，以及如何使用包。例如，

```go
package main

import "fmt"

func main(){
  fmt.Println("Hello world!")
}
```

上述短短的几行代码涉及了 Go 语言中包定义与包导入两个概念。下面我们详细介绍

## 包

我们可以根据自己的需要创建自定义包。一个包可以简单理解为一个存放 `.go` 文件的文件夹。该文件夹下面的所有 `.go` 文件都要在 **非注释的第一行添加包声明**，声明该文件归属的包

例如，下面的代码声明名了 `bitset.go` 文件属于 `bitset` 包。在定义包名时，需要满足标识符的 [[Go 语言：基本概念#标识符]] 中介绍的命名规则

```go title:bitset/bitset.go
package bitset
```

> [!tip] 
> 
> 通常，我们让包名与存放 `.go` 文件的文件夹同名。此外，还需要注意 **一个文件夹下面直接包含的文件只能归属一个包**，同一个包的文件不能在多个文件夹下
> 

在 Go 语言中，`main` 包是一个特殊的包，它是可执行 Go 程序的入口包，其中 `main` 包中的 `main()` 函数是可执行 Go 程序的入口函数。

> [!tip] 
> 
> 关于 `main` 包的作用，我们在后续的 [[Go 语言：Groutines]]  中介绍
> 

### 标识符可见性

在同一个包内部声明的标识符都位于同一个命名空间下，在不同的包内部声明的标识符就属于不同的命名空间。想要在包的外部使用包内部的标识符就需要添加包名前缀

> [!example] 
> 
> 例如`fmt.Println("Hello world!")`，就是指调用`fmt`包中的`Println`函数
> 

如果想让一个包中的标识符（如变量、常量、类型、函数等）能被外部的包使用，那么标识符必须是对外可见的。在 Go 语言中，**通过标识符首字母的大/小写来控制其在包外的可见性**。在一个包内部只有 **首字母大写的标识符才是对外可见的**。

```go
package demo

import "fmt"

// 包级别标识符的可见性

// num 定义一个全局整型变量
// 首字母小写，对外不可见(只能在当前包内使用)
var num = 100

// Mode 定义一个常量
// 首字母大写，对外可见(可在其它包中使用)
const Mode = 1

// person 定义一个代表人的结构体
// 首字母小写，对外不可见(只能在当前包内使用)
type person struct {
	name string
	Age  int
}

// Add 返回两个整数和的函数
// 首字母大写，对外可见(可在其它包中使用)
func Add(x, y int) int {
	return x + y
}

// sayHi 打招呼的函数
// 首字母小写，对外不可见(只能在当前包内使用)
func sayHi() {
	var myName = "七米" // 函数局部变量，只能在当前函数内使用
	fmt.Println(myName)
}
```

对于结构体而言，成员变量或方法对包外的可见性也是通过其标识符的首字母实现的。

```go
type Student struct {
	Name  string // 可在包外访问的方法
	class string // 仅限包内访问的字段
}

```

### 导入包

要在当前包中使用另外一个包的内容就需要使用 `import` 关键字引入这个包，并且 `import` 语句通常放在文件的开头，`package` 声明语句的下方。完整的引入声明语句格式如下:

```go
import importname "path/to/package"
// importname 是包的导入名。通常省略，此时包名时包路径的最后一部分
```

> [!important] 
> 
> 请注意：**包路径必须使用双引号包裹起来**
> 

同一个 `.go` 文件中可以导入多个包，例如

```go
import "fmt"
import "net/http"
import "os"
```

当然，可以使用批量引入的方式。

```go
import (
    "fmt"
  	"net/http"
    "os"
)
```

> [!hint] 
> 
> `importname` 可以是匿名变量。包被匿名引入的目的只有一个就是 **加载这个包**，从而使得这个 **包中的资源得以初始化**
> 

### init 函数

每个包可以定义一个特殊的函数 `init()` 函数，该函数没有参数也没有返回值，其主要作用就是对包内资源进行初始化

> [!attention] 
> 
> 请注意： 包全局作用域中 `init()` 函数不能在代码中主动调用它。程序启动时会按照包的加载顺序自动执行
> 

一个包的初始化过程是按照代码中引入的顺序来进行的，所有在该包中声明的 `init()` 函数都将被串行调用并且仅调用执行一次。每一个包初始化的时候都是先执行依赖的包中声明的 `init()` 函数再执行当前包中声明的 `init()` 函数。确保在程序的 `main()` 函数开始执行时所有的依赖包都已初始化完成。

> [!tip] 
> 
> 在执行 `main.main()` 函数之前，一定会调用所有被加载的包中的 `init()` 函数
> 

下图展示了导入包的顺序与 `init()` 函数执行的关系

![[Pasted image 20250625212847.png]]

> [!attention] 
> 
> 请注意：包全局作用中的变量初始化在 `init()` 函数调用之前。换句话说，包全局变量是最先被初始化的
> 

下列代码演示了全局变量的初始化与 `init()` 被调用的顺序关系

```go
package main

import "fmt"

var x int8 = 10

const pi = 3.14

func init() {
  fmt.Println("x:", x)
  fmt.Println("pi:", pi)
  sayHi()
}

func sayHi() {
	fmt.Println("Hello World!")
}

func main() {
	fmt.Println("你好，世界！")
}
```

执行上述代码输出的结果为

```shell
x: 10
pi: 3.14
Hello World!
你好，世界！
```

## Go Module

在 Go 语言的早期版本中，我们编写 Go 项目代码时所依赖的所有第三方包都需要保存在 `GOPATH` 这个目录下面。这样的依赖管理方式存在一个致命的缺陷，那就是不支持版本管理，同一个依赖包只能存在一个版本的代码。可是我们本地的多个项目完全可能分别依赖同一个第三方包的不同版本。

Go Module 是 Go1.11 版本发布的依赖管理方案，从 Go1.14 版本开始推荐在生产环境使用，于 Go1.16 版本默认开启。Go module 提供了以下命令供我们使用：

| 命令                | 介绍                          |
| ----------------- | --------------------------- |
| `go mod init`     | 初始化项目依赖，生成 `go.mod` 文件      |
| `go mod download` | 根据 `go.mod` 文件下载依赖          |
| `go mod tidy`     | 比对项目文件中引入的依赖与 `go.mod` 进行比对 |
| `go mod graph`    | 输出依赖关系图                     |
| `go mod edit`     | 编辑 `go.mod` 文件              |
| `go mod vendor`   | 将项目的所有依赖导出至 `vendor` 目录（忽略） |
| `go mod verify`   | 检验一个依赖包是否被篡改过               |
| `go mod why`      | 解释为什么需要某个依赖                 |

### **GOPROXY** 

`GOPROXY` 环境变量主要是用于设置 Go 模块代理（Go module proxy），其作用是用于使 Go 在后续拉取模块版本时能够脱离传统的 VCS 方式，直接通过镜像站点来快速拉取。

`GOPROXY` 的默认值是：`https://proxy.golang.org,direct`，由于某些原因国内无法正常访问该地址，所以我们通常需要配置一个可访问的地址。目前社区使用比较多的有两个`https://goproxy.cn`和`https://goproxy.io`，当然如果你的公司有提供 `GOPROXY` 地址那么就直接使用。设置 `GOPAROXY` 的命令如下：

```bash
go env -w GOPROXY=https://goproxy.cn,direct
```

> [!tip] 
> 
> `GOPROXY` 允许设置多个代理地址，多个地址之间需使用英文逗号 `","` 分隔。最后的 `"direct"` 是一个特殊指示符，用于指示 Go 回源到源地址去抓取（比如 GitHub 等）
> 
> 当配置有多个代理地址时，如果第一个代理地址返回 `404` 或 `410` 错误时，Go 会自动尝试下一个代理地址，当遇见 `"direct"` 时触发回源，也就是回到源地址去抓取。
> 

### **GOPRIVATE**

设置了 `GOPROXY` 之后，go 命令就会从配置的代理地址拉取和校验依赖包。当我们在项目中引入了非公开的包（公司内部 `git` 仓库或 `github` 私有仓库等），此时便无法正常从代理拉取到这些非公开的依赖包，这个时候就需要配置 `GOPRIVATE` 环境变量

>[!hint] 
>
> `GOPRIVATE` 用来告诉 `go` 命令哪些仓库属于私有仓库，不必通过代理服务器拉取和校验。
>

`GOPRIVATE` 的值也可以设置多个，多个地址之间使用英文逗号 `","` 分隔。我们通常会把自己公司内部的代码仓库设置到 `GOPRIVATE` 中，例如：

```bash
$ go env -w GOPRIVATE="git.mycompany.com"
```

这样在拉取以 `git.mycompany.com` 为路径前缀的依赖包时就能正常拉取了。

> [!tip] 
> 
> 此外，如果公司内部自建了 `GOPROXY` 服务，那么我们可以通过设置 `GONOPROXY=none`，允许通内部代理拉取私有仓库的包
> 

### 管理依赖

接下来我们将通过一个示例来演示如何在开发项目时使用 `go module` 拉取和管理项目依赖。

首先，我们需要使用 `go mod init` 初始化项目。我们在本地新建一个名为 `hello` 的项目，按如下方式创建一个名为 `hello` 的文件夹并切换到该目录下

```shell
➜  mkdir example/hello -p
➜  cd example/hello
➜  hello
```

目前我们位于 `test` 件夹下，接下来执行下面的命令初始化项目

```go
➜ hello go mod init example/hello
go: creating new go.mod: module example/hello
```

该命令会自动在项目目录下创建一个`go.mod`文件，其内容如下。

```go  title:hello/go.mod
module example/hello

go 1.24.4
```

`go.mod` 文件还会记录项目使用的第三方依赖包信息，包括包名和版本，由于我们的 `hello`项目目前还没有使用到第三方依赖包，所以 `go.mod` 文件暂时还没有记录任何依赖包信息，只有当前项目的一些信息。

> [!tip] 
> 
> 当你需要你的代码做某件事，而这件事可能已经被其他人实现时，你可以寻找一个包含你可以用于代码中的函数的 **包**。这个包就成为依赖
> 

我们可以使用 `pkg.go.dev` 网站查找已发布的模块，这些模块包含您可以在自己的代码中使用的函数。包在模块中发布，例如 `rsc.io/quote`，其他人可以使用它们。随着时间的推移，模块会通过新版本得到改进，您可以将代码升级以使用改进的版本。

如下示例代码展示了如何使用 `rsc.io/quote` 包

```go title:hello/hello.go hl:5
package main

import "fmt"

import "rsc.io/quote"

func main() {
    fmt.Println(quote.Go())
}
```

> [!attention] 
> 
> 请注意：在代码编辑器中，可能会提示 `import "rsc.io/quote"` 语句有误，因为此时还未在项目中引入该依赖
> 

#### 下载依赖

##### go get

第一种方法是在项目目录下执行 `go get` 命令手动下载依赖的包

```shell
➜  hello go get rsc.io/quote 
go: downloading rsc.io/quote v1.5.2
go: downloading rsc.io/sampler v1.3.0
go: downloading golang.org/x/text v0.0.0-20170915032832-14c0d48ead0c
go: added golang.org/x/text v0.0.0-20170915032832-14c0d48ead0c
go: added rsc.io/quote v1.5.2
go: added rsc.io/sampler v1.3.0
```

> [!tip] 
> 
> 未指定版本号会默认下载最新版本
> 

当然，我们还可以下载指定的版本号

```shell
go get rsc.io/quote@latest  # 下载最新版本
go get rsc.io/quote@v1.5.2  # 下载指定版本号
```

此时，我们再次打开 `go.mod` 文件就可以看到下载的依赖包及版本信息都已经被记录下来了。

```go title:hello/go.mod
module example/hello

go 1.24.4

require rsc.io/quote v1.5.2

require (
	golang.org/x/text v0.0.0-20170915032832-14c0d48ead0c // indirect
	rsc.io/sampler v1.3.0 // indirect
)
```

> [!tip] 
> 
> `require` 指令表示项目需要改依赖。而 `require` 行位的 `indirect` 表示这些是间接依赖，即依赖的依赖
> 
> 当依赖安装成功后，还会创建一个 `go.sum` 文件，其中记录每个依赖的版本和散列值
> 

##### go mod tidy

第二种方法就是使用 `go mod tidy` 命令。这个命令会检查项目中的所有 `import` 语句，下载缺失的依赖，移除未使用的模块，并更新 `go.mod` 和 `go.sum` 文件

> [!tip] 
> 
> 通常在现有项目中使用这个命名更新依赖
> 

##### go mod download

第三种方法是使用 `go mod download` 命令。这个命令根据 `go.mod` 文件下载依赖，并更新 `go.mod` 和 `go.sum` 文件

> [!tip] 
> 
> 通常也是在现有项目中使用这个命令下载依赖
> 

#### 导入包

当依赖下载完成后，就可以愉快的导入包了。此时，最初给出的代码就不会提示 `"rsc.io/quote"` 包不存在了

```go title:hello/hello.go hl:5
package main

import "fmt"

import "rsc.io/quote"

func main() {
    fmt.Println(quote.Go())
}
```

##### 导入内部包

当我们的项目功能越做越多，代码越来越多的时候，通常会选择在项目内部按功能或业务划分成多个不同包。Go 语言支持在一个项目中定义多个包

例如，我们在 `hello` 项目中新建一个内部包 `summer`

```shell
➜  hello tree
.
├── go.mod
├── go.sum
├── hello.go
└── summer     # 新增的子包
    └── summer.go
```

其中`summer/summer.go`文件内容如下：

```go title:summer/summer.go
package summer

import "fmt"

func Hello() {
	fmt.Printf("hello, summer\n")
}
```

对于内部包，我们可以无任何障碍的直接导入。例如，在 `hello.go` 中导入 `summer` 子包 

```go title:hello.go
package main

import (
	"example/hello/summer"  // 内部包
	"fmt"                   // 标准包

	"rsc.io/quote"   // 第三方依赖
)

func main() {
	fmt.Println(quote.Go())
	summer.Hello()
}
```

##### 导入本地包

**如果你想要导入本地的一个包，并且这个包也没有发布到到其他任何代码仓库**，这时候你可以在 `go.mod `文件中使用 `replace` 命令将依赖临时替换为本地的代码包。

例如，我们之前定义的 `bitset` 包，它和 `example/hello` 关系如下图

```shell
➜  gostudy tree
.
├── bitset  # 目标本地包
│   ├── bitset.go
│   ├── bitset_test.go
│   └── go.mod
├── example
│   └── hello   # 使用目标本地包的项目
│       ├── go.mod
│       ├── go.sum
│       ├── hello.go
│       └── summer
│           └── summer.go
└── test
    ├── go.mod
    └── main.go
```

此时我们需要自行编辑 `go.mod` 文件。

```go title:hello/go.mo hl:6,13
module example/hello

go 1.24.4

require rsc.io/quote v1.5.2
require github.com/duyupeng36/roadmap/gostudy/bitset v0.0.0

require (
	golang.org/x/text v0.0.0-20170915032832-14c0d48ead0c // indirect
	rsc.io/sampler v1.3.0 // indirect
)

replace github.com/duyupeng36/roadmap/gostudy/bitset => ../../bitset
```

上述高亮部分展示了如何导入本地包。这样我们就可以在 `hello.go` 中使用 `bitset` 包了

```go title:hello.go
package main

import (
	"example/hello/summer"
	"fmt"

	"github.com/duyupeng36/roadmap/gostudy/bitset"
	"rsc.io/quote"
)

func main() {
	fmt.Println(quote.Go())
	summer.Hello()

	set := bitset.BitSet{}
	set.Add(1, 2, 3)
	fmt.Printf("%#v\n", &set)
}
```

#### go.mod 文件

`go.mod`文件中记录了当前项目中所有依赖包的相关信息，声明依赖的格式如下：

```bash
require module/path v1.2.3
```

其中：

- `require`：声明依赖的关键字
- `module/path`：依赖包的引入路径
- `v1.2.3`：依赖包的版本号。支持以下几种格式：
    - `latest`：最新版本
    - `v1.0.0`：详细版本号
    - `commit hash`：指定某次commit hash

引入某些没有发布过 `tag` 版本标识的依赖包时，`go.mod` 中记录的依赖版本信息就会出现类似`v0.0.0-20210218074646-139b0bcd549d`的格式，由版本号、`commit` 时间和 `commit` 的 hash 值组成。如下图

![[Pasted image 20250626001233.png]]

#### go.sum 文件

使用 `go module` 下载了依赖后，项目目录下还会生成一个 `go.sum` 文件，这个文件中详细记录了当前项目中引入的依赖包的信息及其 hash 值。`go.sum`文件内容通常是以类似下面的格式出现。

```go
<module> <version>/go.mod <hash>
```

或者

```go
<module> <version> <hash>
<module> <version>/go.mod <hash>
```

不同于其他语言提供的基于中心的包管理机制，例如 npm 和 pypi等，Go并没有提供一个中央仓库来管理所有依赖包，而是采用分布式的方式来管理包

> [!important] 
> 
>  为了防止依赖包被非法篡改，Go module 引入了`go.sum`机制来对依赖包进行校验
>  

### 发布包

在前面的学习中，我们知道如何使用 `go module` 来管理项目的依赖。现在，我们学习如何基于 `go module` 发布一个包

现在，我们回到 `bitset` 包中，它是在 [[Go 语言：方法#示例程序：实现 BitSet 集合]] 中定义的包。现在，我们将其发布出去。

```shell
➜  duyupeng36 tree 
.
├── bitset
│   ├── bitset.go
│   ├── bitset_test.go
│   └── go.mod
```

首先，我们需要再 `github.com` 获取其他代码托管平台上建立一个 `bitset` 远程仓库。然后，将 `bitset` 包所在的目录使用 `git` 进行初始化

```shell
➜  duyupeng36 cd bitset
➜  bitset git init        
Initialized empty Git repository in /home/dyp/github.com/duyupeng36/bitset/.git/
```

然后，关联到远程仓库

```shell
➜  bitset git remote add origin git@github.com:duyupeng36/bitset.git
```

最后，将代码推送到远程仓库上即可

```shell
➜  bitset git add .
➜  bitset git commit -m "bitset finished"
[master (root-commit) d62733b] bitset finished
 4 files changed, 170 insertions(+)
 create mode 100644 README.md
 create mode 100644 bitset.go
 create mode 100644 bitset_test.go
 create mode 100644 go.mod

➜  bitset git push -u origin master
Enumerating objects: 6, done.
Counting objects: 100% (6/6), done.
Delta compression using up to 16 threads
Compressing objects: 100% (3/3), done.
Writing objects: 100% (6/6), 2.23 KiB | 78.00 KiB/s, done.
Total 6 (delta 0), reused 0 (delta 0), pack-reused 0 (from 0)
To github.com:duyupeng36/bitset.git
 * [new branch]      master -> master
branch 'master' set up to track 'origin/master'.
```

这样就对外发布了一个 Go 包。其他的开发者可以通过`github.com/q1mi/hello`这个引入路径下载并使用这个包了。

一个设计完善的包应该包含开源许可证及文档等内容，并且我们还应该尽心维护并适时发布适当的版本。**github 上发布版本号使用 `git tag` 为代码包打上标签即可**

```shell
➜  bitset git tag -a v1.0.0 -m "release version v1.0.0"  # 打标签
➜  bitset git push origin --tags  # 推送标签
Enumerating objects: 1, done.
Counting objects: 100% (1/1), done.
Writing objects: 100% (1/1), 185 bytes | 13.00 KiB/s, done.
Total 1 (delta 0), reused 0 (delta 0), pack-reused 0 (from 0)
To github.com:duyupeng36/bitset.git
 * [new tag]         v1.0.0 -> v1.0.0
```

经过上面的操作我们就发布了一个版本号为 `v1.0.0`的版本。Go modules 中建议使用语义化版本控制，其建议的版本号格式如下：

![[Pasted image 20250626003637.png]]

- 主版本号：发布了不兼容的版本迭代时递增（breaking changes）。
- 次版本号：发布了功能性更新时递增。
- 修订号：发布了bug修复类更新时递增。

#### **发布新的主版本**

如果某处代码改动巨大（修改了函数之前的调用规则），对之前使用该包作为依赖的用户影响巨大。因此我们需要发布一个主版本号递增的 `v2` 版本。在这种情况下，我们通常会修改当前包的引入路径，像下面的示例一样为引入路径添加版本后缀。

```go
// hello/go.mod

module github.com/duyupeng36/bitset/v2

go 1.24.4
```

把修改后的代码提交：

```bash
➜  bitset git add .
➜  bitset git commit -m "xxxxx"
➜  bitset git push
```

打好 tag 推送到远程仓库。

```bash
➜  bitset git tag -a v2.0.0 -m "release version v2.0.0"
➜  bitset git push origin v2.0.0
```

这样在不影响使用旧版本的用户的前提下，我们新的版本也发布出去了。想要使用`v2`版本的代码包的用户只需按修改后的引入路径下载即可。

```bash
go get github.com/duyupeng36/bitset/v2@v2.0.0
```

#### **废弃已发布版本**

如果某个发布的版本存在致命缺陷不再想让用户使用时，我们可以使用 `retract` 声明废弃的版本。例如我们在 `hello/go.mod` 文件中按如下方式声明即可对外废弃 `v0.1.2` 版本。

```go
module github.com/duyupeng36/example/hello

go 1.16


retract v0.1.2
```

用户使用 `go get` 下载`v0.1.2`版本时就会收到提示，催促其升级到其他版本。

