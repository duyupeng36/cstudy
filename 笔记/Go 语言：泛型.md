# 泛型

在 Go1.18 版本起正式支持 **泛型**，也是自 Go 语言开源以来所做的最大改变。我们从加法函数 `add()` 开始；Go 作为 **强类型** 语言，如果需要实现两个操作数相加时，不同类型就要定义不同的函数

```go
func addInt(a, b int) int {
	return a + b
}

func addFloat(a, b float64) float64 {
	return a + b
}

func addString(a, b string) string {
	return a + b
}
```

上述三个函数，除了类型之外，没有其他的差异。**我们期望函数的形参类型在调用时才指定**。因此函数在定义时，就需要使用一个类型占位符替代形参类型的位置。如下伪代码所示

```go
func add(a, b T) T {
	return a + b
}
```

这里 `T` 称为 **类型参数**，也就是说 **参数的类型也可以变**。`T` 最终一定会确定是某种类型，例如 `int`

## 泛型函数

我们之前已经知道函数定义时可以指定形参，函数调用时需要传入实参

```go
func add(a, b int) int {
		-----
		形参
	return a + b
}

add(10, 20)
	-----
	实参
```

从 Go1.18 开始，Go 中的函数和类型支持添加 **类型参数**。类型参数列表 **看起来像普通的参数列表**，只不过它 **使用方括号（`[]`）** 而不是圆括号（`()`）

```go
func add[T int | float64 | string](a, b T) T {
		--------------------------
		T 就是类型形参
		T 之后跟随的是类型约束
	return a + b
}

add[int](10, 20)
   ----
   类型实参
```

借助泛型，我们可以声明一个适用于一组 **不同类型** 但 **功能相同** 的函数。这种参数类型不确定的函数称为 **泛型函数**

### 实例化

请注意：泛型函数是编译时的概念。如果我们实例化泛型函数，编译器不会为泛型函数生成指令。在成功实例化之后，我们将得到一个 **非泛型函数**，它可以像任何其他函数一样被调用

```go
sadd := add[string]  // 类型实例化。编译器生成 T=string 的 add 函数

result := sadd("hello,", " Go")
```

当然，我们可以在泛型函数实例化之后立即调用。例如

```go
result := add[int](10, 20)
```

向函数的 *类型形参* 提供具体类型的过程称为 **实例化**。该过程分为两步：首先，编译器将类型形参替换为各自的具体类型；然后，检查每个具体类型是否满足各自的 **类型约束**

> [!attention] 
> 
> 请注意：泛型函数未被实例化时，编译器是不会生成具体的指令的。类似于 [[C++：模板]] 
> 

### 多个类型参数

如果需要多个类型参数，只需要将类型参数使用逗号(`,`)分开即可

```go
package main

import "fmt"

func add[T int | float64 | string, E string](a, b T) E {
	return E(fmt.Sprintf("%v + %v = %v", a, b, a+b))
}

func main() {
	fmt.Printf("result: %v", add(10, 20))
}
```

## 类型集合

在 Go1.18 版本开始，为了支持泛型，Go 对接口进行扩展。目前，接口可以是 **类型集合**，也可以是 **方法集合**。也就是说，接口类型现在可以用作值的类型，也可以用作 **类型约束**

例如，如下代码定义了一个由 `int` `uint` 和 `float64` 构成的类型集合

```go
type Value interface {
	int | uint | float64
}
```

类型集合也可以被 **嵌入** 另一个类型集合。例如

```go
type Unsigned interface {
	uint | uint8 | uint16 | uint32 | uint64
}

type Signed interface {
	int | int8 | int16 | int32 | int64
}

type Integer interface {
	Unsigned | Signed
}
```

 > [!attention] 
 > 
 > 自 Go1.18 版本开始，Go 内建了两个类型集合 `any`  和 `comparable`
 > + `any` 本质上就是 `interface{}` 表示任意类型
 > +  `comparable` 表示任意可比较的类型
 >   

此外，Go 还提供了 [constraints](https://pkg.go.dev/golang.org/x/exp/constraints) 包，其他提供了一些常用的类型集合。例如，表示任意复数的类型 `Complex`

```go
type Complex interface {
	~complex64 | ~complex128
}
```


> [!attention] 
> 
> 请注意 `~Type` 表示类型集合可以接受任何底层类型是 `Type` 的自定义类型
> 

下面代码演示了 `~Type` 的作用

```go
package main

import "fmt"

type Signed interface {
	~int | ~int8 | ~int16 | ~int32 | ~int64
}

type MyInt int

func Print[T Signed](v T) {
	fmt.Printf("%T: %[1]d\n", v)
}

func main() {
	var v MyInt = 10
	Print(v)  // main.MyInt: 10
}
```

## 泛型类型


> [!attention] 
> 
> 请注意：**类型集合只能用作类型约束**
> 

Go 语言允许我们自定义类型，但是这些自定义的类型都是确定的类型。例如，下面的代码定义了一个 `int` 类型的切片类型

```go
type IntSlice []int
```

然而，我们其他这个切片类型可以处理多种类型。也就是说，我们需要使用类型约束。例如，下面代码自定义了一个泛型切片，它可以处理 `int | uint | float64` 类型

```go
type Slice[T int | uint | float64] []T
```

下面代码演示了泛型Map 和 泛型结构体的定义方式

```go
type Map[K int | string, V float32 | float64] map[K]V

type Tree[T interface{}] struct {
	left, right *Tree[T]
	value       T
}
```

下面的代码演示了如何使用泛型类型

```go
package main

import "fmt"

type Signed interface {
	~int | ~int8 | ~int16 | ~int32 | ~int64
}

type Slice[T Signed] []T

func main() {
	var s Slice[int] = make(Slice[int], 0, 10)
	for i := 1; i <= 10; i++ {
		s = append(s, i)
	}
	fmt.Printf("%#v\n", s)
}
```
