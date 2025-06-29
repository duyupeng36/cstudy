# 文件属性

文件属性也称 **文件元数据**。包括文件元数据有很多

> [!important] 
> 
> + 设备ID
> + `i-node` 号
> + 文件类型和权限
> + 硬链接数
> + 属主 ID
> + 属组 ID
> + 特殊设备 ID
> + 文件大小
> + io 块字节数
> + 存储的块数
> + 最后访问时间
> + 最后修改时间
> + 最后改变状态时间
>   
> 大部分提取自文件 `i-node`
> 

 标准包 `os` 中提供了访问文件元数据的类型

## `os.FileInfo` 接口

`os.FileInfo` 接口描述文件属性，由函数 `os.Stat()` 和 `os.Lstat()` 返回一个实现了 `os.FileInfo` 接口类型的对象 。`os.FileInfo` 是 `fs.FileInfo` 的别名

```go
type FileInfo = fs.FileInfo

type FileInfo interface {
	Name() string       // 文件的基名
	Size() int64        // 普通文件的字节长度，其他文件依赖于操作系统
	Mode() FileMode     // 文件模式和权限位
	ModTime() time.Time // 修改时间
	IsDir() bool        // Mode().IsDir() 的缩写
	Sys() any           // 底层数据源(可以返回nil)
}
```

`os.Stat(name)` 返回 `name` 指定文件的 `FileInfo` 的实例对象。如果有一个错误，它的类型将是 `*PathError`

```go
func Stat(name string) (FileInfo, error)
```

> [!important] 
> 
> `os.Stat()` 会跟踪符号链接，最终返回的 `FileInfo` 是描述被链接的文件
> 

`os.Lstat(name)` 与 `os.Stat(name)` 类似，只是 `os.Lstat(name)` 不会跟踪符号链接

```go
func Lstat(name string) (FileInfo, error)
```

对于一个打开的文件对象 `*os.File`，其方法 `file.Stat()` 也会返回一个实现了 `os.FileInfo` 接口类型的对象

```go
func (f *File) Stat() (fi FileInfo, err error)
```

```go
package main

import (
	"fmt"
	"os"
)

func main() {
	fileinfo, err := os.Stat(`C:\Users\dyp\go\src\learn\main.exe`)
	if err != nil {
		panic(err)
	}

	// 文件基名
	fmt.Println(fileinfo.Name()) // main.exe
	// 文件大小
	fmt.Println(fileinfo.Size()) // 2051072
	// 文件模式和权限
	fmt.Println(fileinfo.Mode()) // -rw-rw-rw-
	// 是否为目录
	fmt.Println(fileinfo.IsDir()) // false
	// 修改时间
	fmt.Println(fileinfo.ModTime()) // 2024-07-13 11:17:06.1298703 +0800 CST
}
```

## `os.FileMode` 类型

`os.FileMode` 表示文件的 **模式** 和 **权限位**。这些位在所有系统上都有相同的定义，因此有关文件的信息可以从一个系统移植到另一个系统。并不是所有的位都适用于所有的系统。唯一需要的位是目录的 `ModeDir`。`os.FileMode` 只是 `fs.FileMode` 的别名

```go
type FileMode = fs.FileMode

type FileMode uint32

```

定义的文件模式位是 `FileMode` 的最高有效位。最低的 $9$ 位是标准的 `Unix   rwxrwxrwx` 权限。这些位的值应该被认为是公共 API 的一部分，并且可以在有线协议或磁盘表示中使用：**它们不能被更改，尽管可能会添加新的位**

![[Pasted image 20250629011048.png]]

在 `fs` 包中提供了一些常量，用于获取判断文件模式和权限

```go
type FileMode uint32

const (
    // 单字符是被String方法用于格式化的属性缩写。
    ModeDir        FileMode = 1 << (32 - 1 - iota) // d: 目录
    ModeAppend                                     // a: 只能写入，且只能写入到末尾
    ModeExclusive                                  // l: 独占
    ModeTemporary                                  // T: 临时文件（非备份文件）
    ModeSymlink                                    // L: 符号链接（不是快捷方式文件）
    ModeDevice                                     // D: 设备
    ModeNamedPipe                                  // p: 命名管道（FIFO）
    ModeSocket                                     // S: Unix域socket
    ModeSetuid                                     // u: 表示文件具有其创建者用户id权限
    ModeSetgid                                     // g: 表示文件具有其创建者组id的权限
    ModeCharDevice                                 // c: 字符设备，需已设置ModeDevice
    ModeSticky                                     // t: 只有root/创建者能删除/移动文件
    
    // 覆盖所有类型位（用于通过&获取类型位），对普通文件，所有这些位都不应被设置
    ModeType = ModeDir | ModeSymlink | ModeNamedPipe | ModeSocket | ModeDevice
    
    ModePerm FileMode = 0777 // 覆盖所有Unix权限位（用于通过&获取类型位）
)
```

权限位的设置可以采用 `syscall` 包中提供的常量

```go
const (
	S_IRGRP = 0x20  // 属组 Read   000 100 000
	S_IROTH = 0x4   // 其他 Read   000 000 100
	S_IRUSR = 0x100 // 属主 Read   100 000 000

	S_IWGRP = 0x10 // 属组 Write   000 010 000
	S_IWOTH = 0x2  // 其他 Write   000 000 010
	S_IWUSR = 0x80 // 属主 Write   010 000 000

	S_IXGRP = 0x8  // 属组 Execute 000 001 000
	S_IXOTH = 0x1  // 其他 Execute 000 000 001
	S_IXUSR = 0x40 // 属主 Execute 001 000 000

	S_IRWXG = 0x38  // 属组 Read-Write-Execute 000 111 000
	S_IRWXO = 0x7   // 其他 Read-Write-Execute 000 000 111
	S_IRWXU = 0x1c0 // 属主 Read-Write-Execute 111 000 000
)
```


### `FileMode` 的方法

`fs.FileMode` 提供了一些便捷的方法用户获取 **模式位** 和 **权限位**

```go
// IsDir 判断文件是否为目录
func (m FileMode) IsDir() bool

// IsRegular 判断文件是否为常规文件
func (m FileMode) IsRegular() bool

// Type 返回以 m 为单位的类型位(m & ModeType)
func (m FileMode) Type() FileMode

// Perm 以 (m & ModePerm) 的形式返回 Unix 权限位。
func (m FileMode) Perm() FileMode
```

### 示例：判断文件模式

```go
package main

import (
	"fmt"
	"io/fs"
	"os"
)

func main() {
	fileinfo, err := os.Stat(`C:\Users\dyp\go\src\learn\main.exe`)
	if err != nil {
		panic(err)
	}
	
	switch filemode := fileinfo.Mode(); {
	case filemode.IsDir():
		fmt.Printf("%v is a directory file\n", fileinfo.Name())
	case filemode.IsRegular():
		fmt.Printf("%v is a regular file\n", fileinfo.Name())
	case filemode&fs.ModeSocket != 0:
		fmt.Printf("%v is a socket file\n", fileinfo.Name())
	case filemode&fs.ModeSymlink != 0:
		fmt.Printf("%v is a symbol link file\n", fileinfo.Name())
	case filemode&fs.ModeNamedPipe != 0:
		fmt.Printf("%v is a FIFO file\n", fileinfo.Name())
	}
}

```

## 文件属主与权限的修改

对于一个打开的文件，`file.Chmod()` 可以修改文件的模式，如果出错，错误类型是`*PathError`

```go
func (f *File) Chmod(mode FileMode) error
```

`file.Chown()` 方法可以修改打开文件的属主和属组。只需要给定用户 ID 和 组 ID 即可。如果出错，错误类型是 `*PathError`

```go
func (f *File) Chown(uid, gid int) error
```

此外，`os` 包还提供了同名函数，用于修改文件模式和文件的属主和属组。这些函数只需要提供文件路径即可

`os.Chmod(name, filemode)` 将 `name` 指定的文件的模式修改为 `filemode`。`os.Chmod()` 会跟踪符号链接

```go
func Chmod(name string, mode FileMode) error
```

`os.Chown(name, uid, gid)` 将 `name` 指定的文件属主和属组分别修改为 `uid` 和 `gid`。`os.Chown()` 会跟踪符号链接；如果这不是所期望的，请使用 `os.Lchown(name, uid, gid)`

```go
func Chown(name string, uid, gid int) error

func Lchown(name string, uid, gid int) error
```

## 文件时间戳

`os.Chtimes(name, atime, mtime)` 修改 `name` 指定文件的最后访问时间和最后修改时间，底层的文件系统可能会截断/舍入时间单位到更低的精确度。如果出错，会返回 `*PathError` 底层类型的错误

```go
func Chtimes(name string, atime time.Time, mtime time.Time) error
```

- `atime`: `access time` 访问时间

- `mtime`: `modified time` 修改时间
