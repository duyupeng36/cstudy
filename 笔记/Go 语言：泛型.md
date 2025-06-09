# 泛型

Go 1.18 版本增加了对 **泛型** 的支持，泛型也是自 Go 语言开源以来所做的最大改变

>[!info] 泛即通用，可以自行指定类型

我们从 `add` 函数开始。Go 作为强类型语言，如果需要实现两个操作数相加时，不同类型就要定义不同的函数

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

上述三个函数，除了类型之外，没有其他的差异。我们期望函数的形参类型在调用时才指定。因此函数在定义时，就需要使用一个类型展位符替代形参类型的位置。如下伪代码所示

```go
func add(a, b T) T {
	return a + b
}
```

这里 `T` 称为 **类型参数**，也就是说 **参数的类型也可以变**。`T` 最终一定会确定是某种类型，例如 `int`

## 类型参数

### 泛型函数

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

从 1.18 开始，Go 中的函数和类型支持添加 **_类型参数_**。类型参数列表 **看起来像普通的参数列表**，只不过它 **使用方括号（`[]`）** 而不是圆括号（`()`）

```go
func add[T int | float64 | string](a, b T) T {
		-- -----------------------
		T 就是类型形参
		T 之后跟随的是类型约束
	return a + b
}

add[int](10, 20)
   ----
   类型实参
```

借助泛型，我们可以声明一个适用于 **一组类型不同** 单功能相同的函数。这种参数类型不确定的函数称为 **_泛型函数_**

#### 类型实例化

本例定义的 `add` 函数就同时支持 `int` 和 `float64` 和 `string` 三种类型，也就是说当调用 `add` 函数时，我们既可以传入 `int` 类型的参数

```go
result := add[int](10, 20)
```

也可以传入 `float64` 类型

```go
result := add[float64](1.2, 1.3)
```

还可以传入 `string` 类型

```go
result := add[string]("hello", " Go")
```

向函数的类型形参提供具体类型的过程称为实例化。该过程分为两步
+ 首先，编译器类型形参替换为各自的具体类型
+ 然后，检查每个具体类型是否满足各种的类型约束

在成功实例化之后，我们将得到一个 _非泛型函数_，它可以像任何其他函数一样被调用

```go
sadd := add[string]  // 类型实例化。编译器生成 T=string 的 add 函数

result := sadd("hello,", " Go")
```

#### 多个类型参数

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

### 泛型类型

除了函数中支持使用类型参数列表外，**_类型定义_ 也可以使用类型参数列表**

```go
type Map[K int|string, V float32|float64] map[K]V

type Node[T interface{}] struct {
	left, right * Node[T]
	value T
}
```

在类型定义时，如果使用了类型参数，这个类型称为 **_泛型类型_**。泛型类型也可以定义方法。例如，我们为 `Node `  定义一个查找元素的 `LookUp` 方法

```go
func (n *Node[T]) LookUp(x T) *Node[T] {
	....
}
```

在使用泛型类型定义变量时，必须指定具体类型对其进行实例化

```go
var head Node[string]
```

## 类型约束

**类型约束是一个接口**。为了支持泛型，Go 1.18 对接口语法进行了扩展

用在泛型中，接口含义是符合这些特征的 **类型集合**

**类型约束** 接口可以 **直接在类型参数列表中使用**

```go
// 类型约束字面量，通常外层interface{}可省略
func min[T interface{int | float64}](a, b T) T {
	if a <= b {
		return a
	}

	return b
}
```

作为 **类型约束** 使用的接口类型可以 **事先定义并支持复用**

```go
type Value interface {
	int | float64
}

func min[T Value](a, b T) T {
	if a <= b {
		return a
	}
	return b
}
```

类型约束字面值外层 `interface{}` 可省略，**如果省略了外层的 `interface{}` 会引起歧义，那么就不能省略**

```go
type IntPtrSlice [T *int] [] T  // T *int?

type IntPtrSlice [T interface{ *int }]  []T // 使用 interface{} 包裹，消除歧义 
```

> [!important] Go 内置的类型约束
> Go内置了2个约束
> + `any` 表示任意类型
> + `comparable` 表示类型的值应该可以使用 `==` 和 `!=` 比较

### 类型集

Go 1.18 扩展了接口类型的语法，让我们能够向接口中添加类型

```go
type V interface {
	int | string | bool
}
```

上面的代码就定义了一个包含 `int` `string` 和 `bool` 类型的类型集

当接口被用作 **类型约束** 时，由接口定义的类型集精确地指定允许作为相应类型参数的类型

**`|`符号**：`T1 | T2`表示类型约束为 `T1` 和 `T2` 这两个类型的并集，例如下面的 `Integer` 类型表示由 `Signed` 和 `Unsigned` 组成

```go
type Integer interface {
	Signed | Unsigned
}
```

 **`~` 符号**：`~T` 表示所以底层类型是 `T` 的类型，例如 `~ string` 表示所有底层类型是 `string` 的类型集合

```go
type MyString string  // MyString 的底层类型是 string
```

> [!important] `~` 符号后面只能是基本类型

```go
// Signed 是一个约束，允许任何有符号整数类型。
// 如果未来的 Go 版本添加新的预声明的有符号整数类型，则将修改此约束以包含它们。
type Signed interface {
	~int | ~int8 | ~int16 | ~int32 | ~int64
}
```

### `any` 接口

空接口在类型参数列表中很常见，在 Go 1.18 引入了一个新的预声明标识符，作为空接口类型的别名

```go
type any = interface{}  // 接收任意类型
```

由此，我们可以使用如下代码

```go
func foo[S ~[]E, E any]() {
	// ...
}
```

### 类型推断

类型推断让泛型函数的调用就如同普通函数一样自然

#### 函数参数类型推断

对于类型形参，需要传递类型实参，这可能导致代码冗长。回到我们通用的 `min`函数

```go
func min[T int | float64](a, b T) T {
	if a <= b {
		return a
	}
	return b
}
```

类型形参 `T` 用于指定 `a` 和 `b` 的类型。我们可以使用显式类型实参调用它

```go
var a, b, m float64

m = min[float64](a, b)
```

编译器可以从普通参数推断 `T` 的类型实参。这使得代码更短，同时保持清晰

```go
var a, b, m float64

m = min(a, b)
```
