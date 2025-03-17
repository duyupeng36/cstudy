# 指针

Go 语言中的函数传参都是 **_值拷贝_**，当我们想要修改某个变量的时候，我们可以创建一个指向该变量地址的指针变量。传递数据使用指针，而无须拷贝数据

Go 语言并没有像 Python 那样屏蔽对指针的使用，但是对指针的操作做出了一定的限制：**不能进行 _偏移_ 和 _运算_**

所谓 **指针** 就是数据在内存中的 **虚拟地址**，它是一个 _无符号整数_。为了存储这个地址，就需要一种能表达地址含义的类型，即 **_指针类型_**。

> [!attention] 
> 
>请注意： `uintptr` 并不是指针类型，表示存储的值是一个指针。这个类型需要在底层编程时才会使用，请忽略它
> 

Go 中的指针 **不能进行偏移和运算**，因此  Go 中的指针操作非常简单，只有 **取地址(`&`)** 和 **解引用 (`*`)**

## 指针变量的声明与初始化

在 Go 中的 **值类型都有其对应的指针类型**。在声明指针类型的变量时，只需要在其类型前面添加 `*` 就能表示指针类型。例如：
- `int` 类型的指针类型就是 `*int`
- `float32` 类型的指针类型就是 `*float32`
- `[length]T` 类型的指针类型就是 `*[length]T`
- `[]T` 类型的指针类型就是 `*[]T`。切片的本质还是一个结构体，而结构体也是一个值类型

声明指针类型的语法为:

```go
var pointerName *Type
```

该语句声明了一个指向 `Type` 类型变量的指针变量，**默认会被初始化为 `nil`**

> [!tip] 
> 
> 取地址运算符 `&` 可以获取一个变量的地址
> 

```go
package main

import "fmt"

func main() {
	var a int = 10

	var p *int        // nil
	fmt.Println(a, p) // 10 nil

	p = &a                                 // 给指针变量赋值
	fmt.Printf("a: %d a ptr: %p\n", a, &a) // a:10 ptr:0xc00001a078
	fmt.Printf("p: %p type: %T\n", p, p)   // b:0xc00001a078 type:*int
	fmt.Printf("&p: %p\n", &p)             // &p:0xc000006028
}
```

下图展示这个过程

![[Pasted image 20250317001554.png]]

> [!tip] 
> 
> 解引用运算符 `*` 通过指针变量获取其指向内存中的值
> 

```go
package main

import "fmt"

func main() {
	a := 10
	b := &a                                 // 取变量 a 的地址，将指针保存到 b 中
	fmt.Printf("type of b: %T\n", b)        // type of b:*int
	recv := *b                              // 解引用
	fmt.Printf("type of recv: %T\n", recv)  // type of recv:int
	fmt.Printf("value of recv: %v\n", recv) // value of recv:10
}
```

## 关于 `nil`

**`nil` 其实甚至不是 Go 的关键词，只是一个变量名**。定义在 `$GOROOT/src/buildin/buildin.go` 中

```go
// nil 是预先声明的标识符，代表指针、通道、func、接口、映射或切片类型的零值。
var nil Type // 类型必须是指针、通道、函数、接口、映射或片段类型

// Type 仅用于文档目的。它是任何 Go 类型的替身，但在任何给定的函数调用中都代表相同的类型.
type Type int
```

换句话说，我们也可以自己声明一个 `nil`，就会把预定义的 `nil` 覆盖了。肯定是不推荐的

根据这里的定义，也可以看出，**在 Go 中 `nil` 代表了 `pointer`, `channel`, `func`, `interface`, `map` 或者 `slice` 的 零值**。当我们直接写一个 `nil` 出来的时候，它是 _无类型常量_

无类型的 `nil` 被拿来与一个变量进行比较的时候，根据不同的变量，就会有不同的逻辑

```go
var a *int

fmt.Println(a == nil) // nil 被编译器隐式转换为 *int 类型
```

**`nil` 指针就是一个没有指向任何值的指针**，它的值是 `0x0`

```go
package main

import (
	"fmt"
	"unsafe"
)

func main() {
	var a = (*int)(unsafe.Pointer(uintptr(0x0)))
	fmt.Println(a == nil) // true
}
```

**`nil` 切片 就是 `pointer` 为 `nil` 且 `len` 和 `cap` 都为 $0$** 。在 `$GOROOT/src/runtime/slice.go` 中，切片的定义为

```go
type slice struct {
	pointer unsafe.Pointer
	len int
	cap int
}
```

我们做个操作

```go
package main

import (
	"fmt"
	"unsafe"
)

func main() {
	var a = []int{}       // 空切片
	fmt.Println(a == nil) // false

	// 这个结构就行 slice 的结构
	type aa struct {
		ptr unsafe.Pointer
		len int
		cap int
	}

	aaa := (*aa)(unsafe.Pointer(&a))
	aaa.ptr = nil
	fmt.Println(a == nil) // true
}
```

# `new` 和 `make`

对没有关联内存空间的指针变量进行 **解引用** 是非常危险的行为，例如：

```go
package main

import "fmt"

func main() {
	var a *int // 初始化为 *int(nil)
	*a = 100   // panic: runtime error: invalid memory address or nil pointer dereference
	fmt.Println(*a)
}
```

由于`a` 是一个指针变量，默认初始化为 `nil` 不知道地址在哪，对无效的地址进行赋值操作会引发 `panic` 错误

**在`Go`语言中对于 _引用类型的变量_ ，我们在使用的时候不仅要声明它， 还要为它分配内存空间，否则我们的值就没办法存储**

**而对于 _值类型_ 的声明不需要分配内存空间， 是因为它们在声明的时候已经 _默认分配好了内存空间_**

为了给引用类型的变量分配内存空间，Go 提供了两个函数 `new` 和 `make`

### `new` 函数

`new()` 函数通常是在 _堆区_ 分配内存空间。释放空间是不需要程序员管理。而是由 **垃圾回收器** 进行回收，参见 [[Go语言：垃圾回收]]

`new()` 函数通常用于 **分配值类型的内存空间** 并返回一个指针指向该内存空间，在 `$GOROOT/src/buildin/buildin.go` 中 `new` 函数的声明如下

```go
func new(Type) *Type
```

- `Type` 表示任意类型，`new` 函数只接受一个参数，这个参数是一个类型
- `*Type` 表示类型指针，`new` 函数返回一个**指向该类型内存地址的指针**

```go
package main

import "fmt"

func main() {
	var a *int = new(int)
	fmt.Println(a)  // 0xc000096068
	*a = 100        // 向指针执行的内存地址保存数据
	fmt.Println(*a) // 100
}
```

`var a *int` 只是声明了一个指针变量 `a` 但是没有初始化， 指针作为 _引用类型_  需要初始化后才会引用一块内存空间， 此后才可以给它赋值。应该按照如下方式使用内置的`new` 函数对 `a` 进行初始化之后 就可以正常对其赋值了

```go
package main

import "fmt"

func main() {
	var a *int      // 声明
	a = new(int)    // 初始化
	fmt.Println(a)  // 0xc00000a0b8
	*a = 100        // 向指针执行的内存地址保存数据
	fmt.Println(*a) // 100
}
```

### `make` 函数

`make` 也是用于内存分配的，区别于 `new`，它只用于 `slice` `map` 以及 `channel` 的内存创建，而且它返回的类型就是这三个类型本身的值，而不是他们的指针类型，因为这三种类型就是引用类型，所以就没有必要返回他们的指针了。在 `$GOROOT/src/buildin/buildin.go` 中 `make` 函数的声明如下

```go
func make(t Type, size ...IntegerType) Type
```

`make` 函数是无可替代的，我们在使用 `slice`、`map` 以及 `channel` 的时候，都需要使用 `make` 进行初始化，然后才可以对它们进行操作

例如，`var b map[string]int` 只是声明变量 `b` 是一个 `map` 类型的变量，并没有为其分配内存空间。需要使用 `make` 函数进行初始化之后，才能使用键值对进行赋值

```go
package main  
  
import "fmt"  
  
func main() {  
    var b map[string]int  
    b = make(map[string]int, 10)  
    b["沙河娜扎"] = 100  
    fmt.Println(b)  
}
```
