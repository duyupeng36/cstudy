# 模块化

用任何语言来开发，如果软件规模扩大，会编写大量的函数、结构体、接口等代码。这些代码不可能写在一个文件中，这就会产生大量的文件。如果这些文件杂乱无章，就会造成名称冲突、重复定义、难以检索、无法引用、共享不便、版本管理等一系列问题。有一些功能模块如何复用，如何共享方便其他项目使用。所以，一定要有 **模块化** 管理，解决以上诸多问题

## 包

Go `package` 由多个 **`.go` 文件** 和 **目录** 组成。同一个目录下的 `.go` 文件的 **_开头_** 必须使用 `package <包名>` 定义包 

> [!important] 包定义的一些注意事项 
> + **包名** 一般都采用小写，符合标识符要求
> + 当前 **_目录名_** 和 `package <包名>` 中的 **_包名_** 不需要一致，但 **_最好保持一致_**
> + **_同级 `.go` 文件归属一个包_**，就是说每个包目录的当前目录中，只能统一使用同一个 `package` 的包名，否则编译出错

> [!WARNING] 注意
>  **一个文件夹下面直接包含的文件只能归属一个 `package`**， 同样一个 `package` 的文件不能在多个文件夹下
>  
>  **包名 _可以不和文件夹的名字一样_**，包名 **不能包含** `-` 符号
>  
>  包名为 `main` 的包为应用程序的入口包，这种包编译后会得到一个可执行文件， 而编译不包含 `main` 包的源代码则不会得到可执行文件。

### 包内标识符的可见性

一般来说，开发项目时，可以 _把 **相关功能的代码** 集中放在某个包里面_。_在 **包内** 定义的标识符属于 **同一个作用域**_，在不同的包内部声明的标识符就属于不同的作用域

想要在包的外部使用包内部的标识符就需要添加 **包名前缀**，例如`fmt.Println("Hello world!")`，就是指调用 `fmt` 包中的 `Println` 函数。

如果想让一个包中的标识符（如变量、常量、类型、函数等）能被外部的包使用，那么标识符必须是 **_对外可见的_** 。在 Go 语言中是通过标识符的 **_首字母大/小写_ 来控制标识符的 _对外可见 与 不可见_** 的

> [!important] 在一个包内部只有 **首字母大写的标识符才是对外可见的**
> 
> **同样的规则也适用于结构体**，结构体中可导出字段的字段名称必须首字母大写

例如在 `main` 包目录中新建一个 `calc` 包，将所有计算函数都放在其中，以供别的代码调用。

```go
package cacl  // 报名

// Add 导出的标识符
func Add(x, y int) int {
	return x + y
}
```

**_同一个目录就是同一个包_**，该包内 `.go` 文件里的变量、函数、结构体互相可见，可以直接使用

> [!important] 包内 **全局标识符** 是包内可见的
> 
> 同一个包内的所有 `.go` 文件中定义的全局标识符可以直接使用

**_跨目录就是跨包_**，使用时需要导入别的包，**导入需要指定该包的路径**

### `init` 函数

在每一个 Go 源文件中，都可以定义任意个如下格式的特殊函数

```go
func init(){
  // ...
}
```

这种特殊的函数 **_不接收任何参数_** 也 **_没有任何返回值_**，我们也不能在代码中主动调用它。**当程序启动的时候，`init` 函数会按照它们声明的顺序自动执行**。

> [!NOTE] 作用
> 
> **对包进行初始化**

在 Go 语言程序执行时 **导入包语句会自动触发包内部 `init()` 函数的调用**。 需要注意的是：`init()`函数 _没有参数_ 也 _没有返回值_。 `init()`函数在程序运行时 自动被调用执行，**不能在代码中主动调用它**

### `init()` 函数的执行时机

```go
package main

import "fmt"

var x int8 = 10

func init() {
	fmt.Printf("global variable x at %p\n", &x)
}

func main() {
	fmt.Println("Hello 沙河") 
}
```

这段代码的输出结果为

```go
global variable x at 0x3a5280
Hello 沙河
```

可见变量 `x` 在 `init` 执行之前就已经创建完成。所以包中 `init()` 在 `main()` 函数执行之前，全局变量初始化之后

![[Pasted image 20240720203327.png]]

### `init()` 函数执行顺序

包的初始化过程是 **_按照代码中引入的顺序来进行的_**，所有在该包中声明的 `init` 函数都将被 **_串行调用_** 并且 **_仅调用执行一次_**。每一个包初始化的时候都是先执行依赖的包中声明的 `init` 函数再执行当前包中声明的 `init` 函数。确保在程序的 `main` 函数开始执行时所有的依赖包都已初始化完成。

`Go`语言编译器会从 `main` 包开始检查其导入的所有包， 每个包中又可能导入了其他的包。`Go` 编译器由此构建出一个树状的包引用关系， 再根据引用顺序决定编译顺序，依次编译这些包的代码

![[Pasted image 20240720203434.png]]

## 包依赖管理

### GOPATH

Go 1.11 版本之前，项目依赖包存于 `GOPATH`。`GOPATH` 是一个环境变量，指向一个目录，其中存放项目依赖包的源码

> [!tip] 
> `GOPATH` 的默认值是 `$HOME/go`

开发的代码放在 `$GOPATH/src` 目录中，编译这个目录的代码，生成的二进制文件放到 `$GOPATH/bin` 目录下。

> [!question] GOPATH 进行包管理的缺陷
> 
> + `GOPATH` 不区分项目，代码中任何 `import` 的路径均从 `GOPATH` 作为根目录开始。如果有多个项目，**不同项目依赖不同库的不同版本，这就很难解决了**
> + 所有项目的依赖都放在 `GOPATH` 中，**很难知道当前项目的依赖项是哪些**

### GOPATH + vendor 机制

Go 1.5 引入 `vendor` 机制

`vendor`：将项目依赖包复制到项目下的 `vendor` 目录，在编译时使用项目下的`vendor` 目录的包进行编译

依然不能解决不同项目依赖不同包版本问题

>[!Ttip] 包搜索顺序
>+ 在当前包 `vendor` 目录查找
>+ 向上级目录查找，直到 `$GOPATH/src/vendor` 目录
>+ 在 `$GOPATH` 目录查找
>+ 在 `$GOROOT` 目录查找标准库

### Go Modules

Go Modules 是从 Go 1.11 版本引入，到 1.13 版本之后已经成熟，Go Modules 成为官方的依赖包管理解决方案。

> [!tip] Go Modules 优势
> + 不受 `$GOPATH` 限制， **代码可放在任意目录**
> + 自动管理和下载依赖，且 **可以控制使用依赖的版本**
> + **不允许使用相对导入**

#### `GO111MODULE` 配置
`GO111MODULE `控制 Go Module 模式是否开启，有 `off` `on` `auto`（默认）三个值，`auto` 是默认值。

`GO111MODULE=on` ，开启 Go Modules，Go 会忽略 `GOPATH` 和 `vendor` 目录，只根据 `go.mod` 下载依赖，在 `$GOPATH/pkg/mod` 目录搜索依赖包
+ Go 1.13 后默认开启

`GO111MODULE=off `，关闭 Go Modules，Go 会从 `GOPATH` 和 `vendor` 目录寻找包

`GO111MODULE=auto` ，在 `$GOPATH/src` 外面构建项目且根目录有 `go.mod` 文件时，开启模块支持。否则使用 `GOPATH` 和 `vendor` 机制

#### `GOPROXY` 和 `GOPRIVATE` 环境变量

`GOPROXY` 环境变量可以指定包下载镜像源。可选择的镜像源有

+ https://goproxy.cn
+ https://goproxy.io
+ https://mirrors.aliyun.com/goproxy/
+ https://mirrors.cloud.tencent.com/go/

**`GOPRIVATE` 的值也可以设置多个**，多个地址之间使用英文逗号 `“,”` 分隔。我们通常会把自己公司 **内部的代码仓库** 设置到 `GOPRIVATE` 中，例如

```go
go env -w GOPRIVATE="git.mycompany.com"
```

### Module 模式

在 Go 语言的早期版本中，我们编写 Go 项目代码时所依赖的所有第三方包都需要保存在 `GOPATH` 这个目录下面。这样的依赖管理方式存在一个致命的缺陷，那就是不支持版本管理，同一个依赖包只能存在一个版本的代码。可是我们本地的多个项目完全可能分别依赖同一个第三方包的不同版本

直到 Go1.11 版本开始，Go module 依赖管理方案发布，从 Go1.14 版本开始推荐在生产环境使用，于 Go1.16 版本默认开启。Go module 提供了以下命令供我们使用

| 命令                | 介绍                          |
| ----------------- | --------------------------- |
| `go mod init`     | 初始化项目依赖，生成 `go.mod` 文件      |
| `go mod download` | 根据 `go.mod` 文件下载依赖          |
| `go mod tidy`     | 比对项目文件中引入的依赖与 `go.mod` 进行比对 |
| `go mod graph`    | 输出依赖关系图                     |
| `go mod edit`     | 编辑 `go.mod` 文件              |
| `go mod vendor`   | 将项目的所有依赖导出至 `vendor` 目录     |
| `go mod verify`   | 检验一个依赖包是否被篡改过               |
| `go mod why`      | 解释为什么需要某个依赖                 |

#### 构建 Module 模式项目

构建项目根目录 `utils`，并初始化模块 `go mod init /utils` ，会产生 `go.mod` 文件，内容如下

```go
module gocode/utils  // module 模块名

go 1.22.5 // go version
```

根目录下新建 `main.go`，内容如下

```go
package main

import (
	"fmt"  
)

func main() {
	fmt.Println("hello, world")

}
```

> [!tip]
>  + `package` 指定包名，同一个目录包名必须相同
>  + `import` 导入包。这里是**绝对导入**，且 `fmt` 是标准库中的包
> 	 + 标准库搜索 `$GOROOT/src`
> + `main` 函数，必须在 `main` 包中，且只能有一个 `main` 函数。如果要编译成可执行文件，必须要有 `main` 函数

#### 导入子包

构建本地子包 `calc`，其下创建 `calc.go`

```go
package cacl

// Add
func Add(x, y int) int {
	return x + y
}
```

如何在 `main.go` 中使用子包的函数 `Add` 呢？**使用绝对导入**，本地包需要使用 `Module名/子包路径` 进行导入

```go
import "gocode/cacl"
```

如果在 `calc` 下再创建 `minus/minus.go`

```go
package minus

func Minus(x, y int) int {
	return x - y
}
```

那么 `main.go` 中就要如下导入

```go
package main

import (
	"fmt"
	"gocode/cacl"
	"gocode/cacl/minus"
)

func main() {
	fmt.Println("hello, world")
	fmt.Println(cacl.Add(10, 20))
	fmt.Println(minus.Minus(20, 10))

}
```

项目目录结构

```
📦gocode
┣ 📂calc
┃ ┣ 📂minus
┃ ┃ ┗ 📜minus.go
┃ ┗ 📜calc.go
┣ 📜go.mod
┗ 📜main.go
```

#### `import` 关键字

##### 绝对导入

```go
import (
	"fmt"
	"gocode/cacl"
	"gocode/cacl/minus"
)

fmt.Println(minus.Minus(20, 10))
```

##### 别名导入

如果有 **两个导入的包冲突** 时，可以 **重命名** 包来避免冲突

```go
package main

import (
	"fmt"
	c "gocode/cacl"
	"gocode/cacl/minus"
)

fmt.Println(c.Add(10, 20))
```

##### 点导入

把包内所有 **导出成员** 直接 **导入到本地**。很少使用，很有可能导入的标识符导致冲突。

```go
package main

import (
	"fmt"
	. "gocode/cacl"
	"gocode/cacl/minus"
)

fmt.Println(Add(10, 20))
```

`go-staticcheck` 对于点导入会有警告， `should not use dot imports (ST1001)go-staticcheck`

##### 匿名导入

```go
import _ "magedu.com/tools/calc/minus"
```

使用下划线作为别名，就意味着无法使用了，那其目的何在？

这种情况下，只能执行导入的包内的所有 `init` 函数了。**主要作用是做 _包的初始化_**

#### 导入本地其它项目

把 `calc` 包挪到本地其它目录中，如何导入呢？例如把 `calc` 包挪到 `$HOME/calc`，同时在 `calc` 目录使用 `go mod init ccc` ，打开增加的`go.mod`，内容如下

```go
module ccc

go 1.22.5
```

`main.go` 中的导入和使用，如下

```go
package main

import (
	c "ccc" // 随便写的包路径
	"fmt"
)

func main() {
	fmt.Println(c.Add(10, 20))
}
```

需要手动在 `go.mod` 中增加

```go
module gocode

go 1.22.5

require ccc v0.0.0  // // 指定伪版本号，满足格式要求

replace ccc => /home/dyp/cacl  // replace指令指定包搜索路径，而不是去 $GOPATH/pkg/mod
```

`require` 指令，用于设置一个特定的依赖的版本

+ `// indirect`：注释表示该模块为 **间接依赖**，也就是在当前应用程序中的 import 语句中，并没有发现这个模块的明确引用，有可能是你先手动 `go get` 拉取下来的，也有可能是你所依赖的模块所依赖的

`exclude` 指令：用于从使用中排除一个特定的依赖的版本

`replace` 指令：用于将一个依赖版本替换为另外一个依赖版本

#### 导入第三方包

第一种方法是在项目目录下执行 `go get` 命令手动下载依赖的包

```go
➜ go get rsc.io/quote                                          
go: added golang.org/x/text v0.0.0-20170915032832-14c0d48ead0c
go: added rsc.io/quote v1.5.2
go: added rsc.io/sampler v1.3.0
```

> [!NOTE] 默认下载最新版本
> 
> 这样 **默认会下载最新的发布版本**，你也可以指定想要下载指定的版本号的
>
> ```shell
> ➜ go get rsc.io/quote@v1.5.0
> go: downloading rsc.io/quote v1.5.0
> go: downgraded rsc.io/quote v1.5.2 => v1.5.0
> ```

如果依赖包没有发布任何版本则会拉取最新的提交，最终 `go.mod` 中的依赖信息会变成类似下面这种由默认 `v0.0.0` 的版本号和最新一次 `commit` 的时间和 `hash` 组成的版本格式：

```
golang.org/x/text v0.0.0-20170915032832-14c0d48ead0c
```

> [!NOTE] 指定下载 `commit` 对应的代码
> 如果想指定下载某个 `commit` 对应的代码，可以直接指定 `commit hash`，不过没有必要写出完整的 `commit hash`，一般前 `7` 位即可
> ```shell
> go get rsc.io/quote@b44a0b1
>```

**直接编辑`go.mod`文件**，将依赖包和版本信息写入该文件。

```go
module holiday 
  
go 1.22.2  
  
require (  
    rsc.io/quote latest  
)
```

表示当前项目需要使用 `rsc.io/quote` 库的最新版本，然后在项目目录下执行 `go mod download` 下载依赖包

```shell
go mod download
```

如果不输出其它提示信息就说明依赖已经下载成功，此时 `go.mod` 文件已经变成如下内容

```go
module holiday 
  
go 1.22.2  
  
require rsc.io/quote v1.5.2
```

## 发布包

们首先在自己的 github 账号下新建一个项目，并把它下载到本地。我这里就以创建和发布一个名为`hello` 的项目为例进行演示。这个 `hello` 包将对外提供一个名为 `SayHi` 的函数，它的作用非常简单就是向调用者发去问候。

```shell
$ git clone https://github.com/q1mi/hello
$ cd hello
```

我们当前位于`hello`项目目录下，执行下面的命令初始化项目，创建`go.mod`文件。需要注意的是这里定义项目的引入路径为`github.com/q1mi/hello`，读者在自行测试时需要将这部分替换为自己的仓库路径。

```shell
hello $ go mod init github.com/q1mi/hello
go: creating new go.mod: module github.com/q1mi/hello
```

接下来我们在该项目根目录下创建 `hello.go` 文件，添加下面的内容：
```go
package hello

import "fmt"

func SayHi() {
	fmt.Println("你好，我是七米。很高兴认识你。")
}
```

然后将该项目的代码 push 到仓库的远端分支，这样就对外发布了一个Go包。其他的开发者可以通过`github.com/q1mi/hello`这个引入路径下载并使用这个包了。

一个设计完善的包应该包含开源许可证及文档等内容，并且我们还应该尽心维护并适时发布适当的版本。github 上发布版本号使用git tag为代码包打上标签即可。

```bash
hello $ git tag -a v0.1.0 -m "release version v0.1.0"
hello $ git push origin v0.1.0
```

经过上面的操作我们就发布了一个版本号为`v0.1.0`的版本。

Go modules中建议使用语义化版本控制，其建议的版本号格式如下：

![[Pasted image 20240720200531.png]]


其中：

- 主版本号：发布了不兼容的版本迭代时递增（breaking changes）
- 次版本号：发布了功能性更新时递增
- 修订号：发布了 bug 修复类更新时递增

### 发布新的主版本

现在我们的`hello`项目要进行与之前版本不兼容的更新，我们计划让`SayHi`函数支持向指定人发出问候。更新后的`SayHi`函数内容如下：

```go
package hello

import "fmt"

// SayHi 向指定人打招呼的函数
func SayHi(name string) {
	fmt.Printf("你好%s，我是七米。很高兴认识你。\n", name)
}
```

由于这次改动巨大（修改了函数之前的调用规则），对之前使用该包作为依赖的用户影响巨大。因此我们需要发布一个主版本号递增的`v2`版本。在这种情况下，我们通常会修改当前包的引入路径，像下面的示例一样为引入路径添加版本后缀。

```go
// hello/go.mod

module github.com/q1mi/hello/v2

go 1.16
```

把修改后的代码提交：

```bash
hello $ git add .
hello $ git commit -m "feat: SayHi现在支持给指定人打招呼啦"
hello $ git push
```

打好 `tag` 推送到远程仓库。

```bash
hello $ git tag -a v2.0.0 -m "release version v2.0.0"
hello $ git push origin v2.0.0
```

这样在不影响使用旧版本的用户的前提下，我们新的版本也发布出去了。想要使用`v2`版本的代码包的用户只需按修改后的引入路径下载即可。

```bash
go get github.com/q1mi/hello/v2@v2.0.0
```

在代码中使用的过程与之前类似，只是需要注意引入路径要添加 `v2` 版本后缀。

```go
package main

import (
	"fmt"

	"github.com/q1mi/hello/v2" // 引入 v2 版本
)

func main() {
	fmt.Println("现在是假期时间...")

	hello.SayHi("张三") // v2版本的SayHi函数需要传入字符串参数
}
```

### 废弃已发布版本

如果某个发布的版本存在致命缺陷不再想让用户使用时，我们可以使用`retract`声明废弃的版本。例如我们在`hello/go.mod`文件中按如下方式声明即可对外废弃`v0.1.2`版本。

```go
module github.com/duyupeng/hello

go 1.16


retract v0.1.2
```

用户使用 `go get `下载`v0.1.2`版本时就会收到提示，催促其升级到其他版本。


