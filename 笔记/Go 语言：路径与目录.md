# 路径与目录

存储设备保存着数据，但是得有一种方便的模式 **让用户可以定位资源位置**，操作系统采用一种 **路径字符串** 的表达方式，这是一棵倒置的层级目录树，从根开始

> [!tip] 相对路径
> 
> 不是以根目录开始的路径，例如 `a/b`  ` a/b/c`   `.`   `../a/b`  `./a/b/c`
> + `.` 当前目录
> + `..` 当前路径的上一级目录
> 

> [!tip] 绝对路径
> 
> 以斜杆开始的路径，例如 `/a/b` 、` /a/b/c` 、 `/t/../a/b` ，Window 需要盘符 `e:\a\b\c`
> 

> [!important] 路径分隔符
> 
> 不同操作系统采用不同的路径分隔符
> + Windows 采用 `\` ，但是也支持 `/` 
> + 类 Unix 系统等采用 `/`
> 

为了方便路径处理，Go 标准库提供 `path` 包和更加方便的 `path/filepath` 包，我们通常只需要使用 `filepath` 包即可

## 路径操作

包 `filepath` 实现了与目标操作系统定义的文件路径兼容的文件名路径操作实用程序。`filepath` 包使用 *正斜杠* 或 *反斜杠*，具体取决于操作系统

### 获取基名: `filepath.Base(path)`

> [!important] 基名
> 
> 路径字符串中的最后一部分
> 

函数 `filepath.Base()` 的函数签名如下，它会返回路径字符串 `path` 的最后一部分

```go
func Base(path string) string
```

> [!attention] 
> 
> 请注意：路径的最后一部分指删除尾随的斜杠的那一部分。此外，还需要注意
> + `path == ""` 返回 `"."`
> + `path == "///"` 返回 `"/"`
> 

```go
package main

import (
	"fmt"
	"path"
)

func main() {
	fmt.Println(path.Base("///"))     // "/"
	fmt.Println(path.Base(""))        // "."
	fmt.Println(path.Base("/a/b/c/")) // "c"
}
```

### 获取绝对路径: `filepaht.Abs(path)`

函数 `filepath.Abs(path)` 的签名如下，它用于返回路径字符串 `path` 的绝对路径

```go
func Abs(path string) (string, error)
```

> [!attention] 
> 
> 如果 `path` 不是绝对路径，就在 `path` 之前拼接当前工作目录，将其转换为绝对路径。给定文件的绝对路径名不能保证是唯一的
> 

```go
package main

import (
	"fmt"
	"os"
	"path/filepath"
)

func main() {
	abspath, err := filepath.Abs("test.txt")
	if err != nil {
		fmt.Fprintf(os.Stderr, "Error getting absolute path: %v\n", err)
		return
	}
	fmt.Println(abspath)  // $HOME/github.com/duyupeng36/gostudy/test.txt
}
```

### 清理路径: `filepath.Clean(path)`


函数 `filepath.Clean()` 会将 `path` 进行清理返回一个与 `path` 等价的最短路径名。其函数签名如下

```go
func Clean(path string) string
```

> [!tip] 清理规则
> 
> `filepath.Clean(path)` 它反复应用以下规则，直到无法进行进一步的处理:
> + 用单个路径分隔符替换多个路径分隔符。例如 `"a//b"` 会被清理为 `"a/b"`
> + 消除每一个 `"."` 路径名元素(当前目录)
> + 消除每个内部的 `".."` 路径名元素（父目录）以及其前的非 `".."` 路径名元素
> + 消除 `".."` 以根路径开始的元素: 即，将 `"/.."` 替换为 `"/"`，假设分隔符是 `"/"`
> 

```go
package main

import (
	"fmt"
	"path"
)

func main() {
	paths := []string{
		"a/c",               // 被清理为 "a/c"
		"a//c",              // 被清理为 "a/c"
		"a/c/.",             // 被清理为 "a/c"
		"a/c/b/..",          // 被清理为 "a/c"
		"/../a/c",           // 被清理为 "/a/c"
		"/../a/b/../././/c", // 被清理为 "/a/c"
		"",                  // 被清理为 "."
	}

	for _, p := range paths {
		fmt.Printf("Clean(%q) = %q\n", p, path.Clean(p))
	}
}
```

### 删除最后一个路径元素: `filepath.Dir(path)`

函数 `filepath.Dir(path)` 返回 `path` 中除最后一部分以外的所有元素。其函数签名如下

```go
func Dir(path string) string
```

删除 `path` 的最后一部分后，还会调用 `filepath.Clean()`，并删除尾随斜杠

```go
package main

import (
	"fmt"
	"path/filepath"
)

func main() {
	fmt.Println("On Linux:")

	var path = []string{
		"/foo/bar/baz.js", // directory is "/foo/bar"
		"/foo/bar/baz",    // directory is "/foo/bar"
		"/dirty//path///", // directory is "/dirty/path"
		"dev.txt",         // directory is "."
		"../todo.txt",     // directory is ".."
		"..",              // directory is "."
		".",               // directory is "."
		"/",               // directory is "/"
		"",                // directory is "."
	}
	for _, p := range path {
		fmt.Printf("Directory: %s\n", filepath.Dir(p))
	}
}
```

### 获取扩展名: `filepath.Ext(path)`

函数 `filepath.Ext()` 返回路径字符串 `path` 中的文件扩展名，即是最后一个点开始的后缀。如果没有，则返回空字符串

```go
func Ext(path string) string
```


```go
package main

import (
	"fmt"
	"path/filepath"
)

func main() {
	fmt.Printf("No dots: %q\n", filepath.Ext("index"))         // ""
	fmt.Printf("One dot: %q\n", filepath.Ext("index.js"))      // ".js"
	fmt.Printf("Two dots: %q\n", filepath.Ext("main.test.js")) // ".js"
}

```

### 路径拼接: `filepath.Join(elem...)`

函数 `filepath.Join(elem...)` 将 `elem` 的所有元素拼接成一个路径字符串，其中空元素会被忽略。最终凭借的结果会调用 `filepath.Clean()` 进行清理

```go
func Join(elem ...string) string
```

> [!attention] 
> 
> 在不同的操作系统上会使用不同的分隔符
> 

```go
package main

import (
	"fmt"
	"path/filepath"
)

func main() {
	fmt.Println("On Unix:")
	fmt.Println(filepath.Join("a", "b", "c"))         // "a/b/c"
	fmt.Println(filepath.Join("a", "b/c"))            // "a/b/c"
	fmt.Println(filepath.Join("a/b", "c"))            // "a/b/c"
	fmt.Println(filepath.Join("a/b", "/c"))           // "/c"
	fmt.Println(filepath.Join("a/b", "../../../xyz")) // "../xyz"
}
```

### 路径分割: `filepath.Split(path)`

函数 `filepath.Split(path)` 拆分紧跟在最后一个分隔符后面的路径，将其分为 **目录** 和 **文件名** 两个部分

```go
func Split(path string) (dir, file string)
```

> [!attention] 
> 
> 如果 `path` 中没有分隔符，那么返回值 `dir` 为空字符串，`file` 为 `path`。
> 
> 也就是说，返回值 `dir + file` 就是参数 `path`
> 

```go
import (
	"fmt"
	"path/filepath"
)

func main() {
	paths := []string{
		"/home/arnie/amelia.jpg", // dir: "/home/arnie/"; file: amelia.jpg
		"/mnt/photos/",           // dir: "/mnt/photos/"; file: ""
		"rabbit.jpg",             // dir: "";  			  file: "rabbit.jpg"
		"/usr/local//go",         // dir: "/usr/local//"; file: "gp"
	}
	fmt.Println("On Unix:")
	for _, p := range paths {
		dir, file := filepath.Split(p)
		fmt.Printf("input: %q\n\tdir: %q\n\tfile: %q\n", p, dir, file)
	}
}
```

## 遍历目录

函数 `filepath.WalkDir()` 和 `filepath.Walk()` 两个函数用于 **递归遍历目录树**。这两个函数每遍历的一个节点，都会执行回调函数，只不过返回的参数略有不同。它们的函数签名如下

```go
func Walk(root string, fn WalkFunc) error

func WalkDir(root string, fn fs.WalkDirFunc) error
```

> [!important] 
> 
> 这两个函数都是递归遍历，对于每个目录都会执行回调函数
> + 都不跟踪软链接
> + 内部都是按照字典序输出
> + 采用深度优先遍历
>   
>  Go 1.16 加入的 `WalkDir` 效率更高一些 
>  

函数类型 `WalkFunc` 的定义如下

```go
type WalkFunc func(path string, info fs.FileInfo, err error) error
```

函数类型 `fs.WaldDirFunc` 定义下

```go
type WalkDirFunc func(path string, d DirEntry, err error) error
```


`DirEntry` 类型定义在 `fs` 包中，代表递归遍历的每个目录节点

```go
type DirEntry interface {
	// 描述 DirEntry 的名字，只是路径名的最后一个元素
	// 例如："home/gopher/hello.go" ==> "hello.go"
	Name() string

	// entry 是否为目录
	IsDir() bool
	// entry 的类型位
	Type() FileMode
	// 返回 entry 的 FileInfo
	Info() (FileInfo, error)
}
```

> [!tip] 
> 
> `os.ReadDir(path)` 读取目录。注意，该函数不会递归
> + 早期使用 `ioutil` 包中的工具函数，不过目前 `ioutil` 包被弃用了
> 

### 示例

```go
package main

import (
	"fmt"
	"io/fs"
	"os"
	"path/filepath"
)

func main() {
	path := "."

	// WalkDir() 遍历递归遍历，包含自身
	filepath.WalkDir(path, func(path string, d fs.DirEntry, err error) error {
		fmt.Println(path, d.IsDir(), d.Name(), err) // 递归读出目录和文件
		// fmt.Println(d.Info())
		return err
	})

	fmt.Println("~~~~~~~~~~~~~~~~~~~~~~~~~~~")

	// Walk() 遍历递归遍历，包含自身
	filepath.Walk(path, func(path string, info fs.FileInfo, err error) error {
		fmt.Println(path, info.IsDir(), info.Name(), err) // 递归读出每一个目录和文件
		// fmt.Println(info)
		return err
	})

	// 不递归遍历当前目录
	entrySlice, err := os.ReadDir(path)
	if err != nil {
		panic(err)
	}
	for _, entry := range entrySlice {
		fmt.Println(entry.IsDir(), entry.Name())
		fmt.Println(filepath.Join(path, entry.Name()))
	}
}
```

## 目录操作

标准包 `os` 提供了一组用于操作目录的函数，它们是 [[Linux 系统编程：目录与链接]] 中介绍的系统调用的封装

### 获取家目录

函数 `os.UserHomeDir()` 返回用户家目录，并且返回可能出现的错误

```go
func UserHomeDir() (string, error)
```

在 Unix 上，包括 MacOS，它返回 `$HOME` 环境变量。在 Windows 上，它返回 `%USERPROFILE%`

### 获取当前工作目录

函数 `os.Getwd()` 用于获取当前工作目录

```go
func Getwd() (dir string, err error)
```

返回对应于当前目录的根路径名。如果当前目录可以通过多条路径到达(由于符号链接)，可能会返回其中任何一条路径

### 修改当前工作目录

函数 `os.Chdir(dir)` 修改当前进程的工作目录为 `dir`

```go
func Chdir(dir string) error
```

### 获取临时文件目录

函数 `os.TempDir()` 返回默认的用于保存临时文件的目录

```go
func TempDir() string
```

在 Unix 系统上，如果 `$TMPDIR` 非空，则返回 `$TMPDIR`，否则返回 `/tmp`

在 Windows 上，它使用 `GetTempPath`，从 `%TMP%` `%TEMP%` `%USERPROFILE%` 或 Windows 目录返回第一个非空值

### 创建目录

函数 `os.Mkdir(name, prem)` 创建 `name` 指定的目录，父目录必须存在

```go
func Mkdir(name string, perm FileMode) error
```

函数 `os.MkdirAll(path, prem)` 递归创建 `path` 指定的目录

```go
func MkdirAll(path string, perm FileMode) error
```

> [!tip] 
> 
> `MkdirAll` 函数等价于 Unix 命令 `mkdir -p`
> 

### 删除目录

`os.Remove(name)` 删除文件或空目录

```go
func Remove(name string) error
```

`os.RemoveAll(name)` 递归删除目录或文件

```go
func RemoveAll(path string) error
```

### 目录重命名

`os.Rename(oldpath, newpath)` 将 `oldpah` 重命名(移动)到 `newpah`。如果 `newpath` 已经存在并且不是一个目录，`os.Rename()` 将替换它

```go
func Rename(oldpath, newpath string) error
```

### 存在性检查

标准包 `os` 中定义了两个与文件相关的错误

```go
var (
	ErrExist      = fs.ErrExist      // "file already exists"
	ErrNotExist   = fs.ErrNotExist   // "file does not exist"
)
```

> [!note] `ErrExist` 和 `ErrNotExist` 都定义在 `fs` 包中

为了检测一个目录是否已经存在，`os` 包中还提供了 `os.IsExist(err)` 和 `os.IsNotExist(err)` 

```go
func IsExist(err error) bool

func IsNotExist(err error) bool
```

这两个函数执行结果总结如下表

| 文件是否存在 | `err` 的值 | `os.IsExist(err)` | `os.IsNotExist(err)` |
| :----: | :------: | ----------------- | -------------------- |
|  不存在   | 非 `nil`  | `false`           | `true`               |
|   存在   |  `nil`   | `false`           | `false`              |

> [!tip] 
> 
> 使用 `os.IsExist(err)` 容易让人疑惑，建议使用 `os.IsNotExist(err)`
> 

## 练习：统计指定目录的总大小

```go
package main

import (
	"fmt"
	"os"
	"path/filepath"
)

func TotalSize(path string) (total int64) {

	entries, err := os.ReadDir(path)

	if err != nil {
		panic(err)
	}

	for _, entry := range entries {
		if entry.IsDir() {
			total += TotalSize(filepath.Join(path, entry.Name()))
		} else {

			entryInfo, err := entry.Info()
			if err != nil {
				panic(err)
			}
			fmt.Printf("%v: %4v bytes\n", path+entry.Name(), entryInfo.Size())
			total += entryInfo.Size()
		}
	}

	return total
}

func main() {
	home, err := os.Getwd()
	if err != nil {
		panic(err)
	}

	total := TotalSize(home)

	fmt.Printf("%+v bytes\n", total)
}
```
