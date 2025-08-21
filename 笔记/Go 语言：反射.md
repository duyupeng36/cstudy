# 反射

**反射** 是指程序在 **运行时** 获取 **对象类型和值的信息**。这里的 **对象** 是一个表示 **数据存储区域**

> [!tip] 
> 
> 我们使用高级语言编写的程序在执行之前会经历一个称为 **翻译** 的阶段(编译或解释)，通常称为 **编译时**
> 
> 此时，编译器或解释器会将程序中的变量转变为 **内存地址**；而变量名不会被编译器写入到可执行文件中。在运行程序时，程序无法获取自身的信息。
> 

然而，支持反射的语言可以在程序编译期间将变量的 **反射信息**，如属性名称、类型信息等整合等记录下来，并给程序提供接口访问反射信息，这样就可以在程序运行期间获取类型的反射信息，并且有能力修改它们。

在 Go 中， `any` 类型的遍历可以存储任意类型的值，那我们如何知道这个 `any` 类型的变量保存的数据是什么呢？ 由两种方法：**类型断言** 和 **反射**

通过标准包 `reflect` 提供了反射接口，就可以在运行时动态的获取一个变量的 **类型信息** 和 **值信息**

## 反射类型信息

### `reflect.Type` 接口

`reflect.Type` 接口描述了变量的类型信息。通过 `reflect.TypeOf()` 函数可以获得该接口的实例。`reflec.TypeOf()` 函数的签名如下 

```go
func TypeOf(i any) Type
```

`reflect.Type` 接口定义在 `$GOROOT/src/relect/type.go` 中

```go
type Type interface {
	// 方法类型形象
	NumMethod() int // 方法的个数
    Method(int) Method // 字典序索引获取方法
    MethodByName(string) (Method, bool) // 根据名字获取方法

	// 类型信息
    Name() string // 获取类型名称
    PkgPath() string // 包路径
    Size() uintptr // 内存占用
    String() string // Type 的字符串表达
    Kind() Kind //  类型的种类

	// 接口信息
    Implements(u Type) bool // 该类型是否实现接口 u
    AssignableTo(u Type) bool // 该类型是否能赋值给另一种类型 u
	ConvertibleTo(u Type) bool // 该类型是否转换为另一种类型 u

    // 结构体
    NumField() int // 字段数量
    Field(i int) StructField // 结构体索引 i 的字段
    FieldByIndex(index []int) StructField // 获取 index 指定的所有字段
    FieldByName(name string) (StructField, bool) // 使用字段名获取字段的类型信息
    FieldByNameFunc(match func(string) bool) (StructField, bool) // 
    
    // 容器
    Len() int // 容器的长度，如果不是 Array 返回 panic
    Key() Type // 返回 map 的 key 类型，如果不是 map 则 panic
    Elem() Type // 获取容器元素的类型
    
    // 函数
    In(i int) Type // 入参
    Out(i int) Type // 出参
    NumIn() int  // 入参数量
    NumOut() int // 出参数量
}
```

### `reflect.Kind` 类型

除了 `reflect.Type` 这个接口外，我们还需要关注一个类型 `reflect.Kind` 类型。`reflect.Kind` 的定义在 `$GOROOT/src/reflect/type.go` 中，其定义如下

```go
type Kind uint

const (
	Invalid Kind = iota  // 非法类型
	Bool                 // 布尔型
	Int                  // 有符号整型
	Int8                 // 有符号8位整型
	Int16                // 有符号16位整型
	Int32                // 有符号32位整型
	Int64                // 有符号64位整型
	Uint                 // 无符号整型
	Uint8                // 无符号8位整型
	Uint16               // 无符号16位整型
	Uint32               // 无符号32位整型
	Uint64               // 无符号64位整型
	Uintptr              // 指针
	Float32              // 单精度浮点数
	Float64              // 双精度浮点数
	Complex64            // 64位复数类型
	Complex128           // 128位复数类型
	Array                // 数组
	Chan                 // 通道
	Func                 // 函数
	Interface            // 接口
	Map                  // 映射
	Ptr                  // 指针
	Slice                // 切片
	String               // 字符串
	Struct               // 结构体
	UnsafePointer        // 底层指针
)
```

> [!attention] 
> 
> 从中文角度来看，`Type` 和 `Kind` 意思接近，这里把 `Type` 翻译成 **类型**，把 `Kind` 翻译成 **种类**。意思很相近，怎么区分他们呢？
> 
> + `Type` 接口代表了一个具体类型
> + `Kind` 类型代表了一个较为宽泛的类型
> 

如下代码演示了展示了 `Type` 和 `Kind` 之间的差异

```go
package main

import (
	"fmt"
	"reflect"
)

type User struct {
	name string
	age  int
}

func main() {

	var user = User{
		name: "张三",
		age:  20,
	}

	t := reflect.TypeOf(user)
	fmt.Println("Type: ", t)        // main.User
	fmt.Println("Kind: ", t.Kind()) // struct

}
```

上述代码执行后输出的结果为

```shell
➜  go run main.go
Type:  main.User
Kind:  struct
```

> [!tip] 
> 
> 显然，`reflect.Type` 代表了具体类型，即我们在编程时使用的类型。而 `reflect.Kind` 代表了类型的种类，即是一种宽泛的类型
> 

提供 `reflect.Kind` 可以在某些使用场景中使用更宽松的判断条件。例如在某些只是需要使用数组类型就行，使用 `reflect.Kind` 判断更合适、更宽松

### 基本类型的信息

对于代表基本类型的 `reflect.Type` 接口，可使用的方法较少，下面的示例代码展示了大部分可使用的方法

```go
package main

import (
	"fmt"
	"reflect"
)

func reflectBaseType(b any) {

	// 获取 b 的类型信息
	t := reflect.TypeOf(b)

	// 基本类型的类型信息
	fmt.Printf("Type: %v\n", t)        // 本质上是调用的 t.String()
	fmt.Printf("Kind: %v\n", t.Kind()) //
	fmt.Printf("Name: %v\n", t.Name())
	fmt.Printf("Size: %v\n", t.Size())
}

func main() {

	var a = 10

	reflectBaseType(a)

}
```

### 数组类型信息

相比于基本类型，数据多了长度这个属性。因此，可以使用的 `reflect.Type` 接口中的方法要更多一点

```go
package main

import (
	"fmt"
	"reflect"
)

func reflectArrayType(b any) {

	// 获取 b 的类型信息
	t := reflect.TypeOf(b)

	// 基本类型的类型信息
	fmt.Printf("Type: %v\n", t)        // 本质上是调用的 t.String()
	fmt.Printf("Kind: %v\n", t.Kind()) //
	fmt.Printf("Name: %v\n", t.Name())
	fmt.Printf("Size: %v\n", t.Size())
	
	// 数组可以额外使用的方法
	fmt.Printf("Len: %v\n", t.Len())
	fmt.Printf("Eleme Type: %v\n", t.Elem()) // 获取元素的 reflect.Type
}

func main() {
	var a = [3]int{1, 2, 3}
	reflectArrayType(a)
}
```


> [!attention] 
> 
> 使用 `reflect.Type` 接口访问的是类型信息。对于切片而言，类型信息不包含长度 
> 

### Map 类型信息

Map 类型的信息包含 Key 和 Vlaue 两部分

```go
package main

import (
	"fmt"
	"reflect"
)

func reflectMapType(b any) {

	// 获取 b 的类型信息
	t := reflect.TypeOf(b)

	// 基本类型的类型信息
	fmt.Printf("Type: %v\n", t)        // 本质上是调用的 t.String()
	fmt.Printf("Kind: %v\n", t.Kind()) //
	fmt.Printf("Name: %v\n", t.Name())
	fmt.Printf("Size: %v\n", t.Size())
	// Map 可以额外使用的方法
	fmt.Printf("Key Type: %v\n", t.Key())    // 获取 key 的 reflect.Type
	fmt.Printf("Value Type: %v\n", t.Elem()) // 获取 value 的 reflect.Type
}

func main() {
	var a = map[string]int{
		"one":   1,
		"two":   2,
		"three": 3,
	}
	reflectMapType(a)
}
```

### Func 类型信息

在 Go 语言中，函数类型包括参数和返回值的类型，我们可以通过 `reflect.Type` 接口中提供的方法获取 Func 类型的信息

```go
package main

import (
	"fmt"
	"reflect"
)

func reflectFuncType(b any) {

	// 获取 b 的类型信息
	t := reflect.TypeOf(b)

	// 基本类型的类型信息
	fmt.Printf("Type: %v\n", t)        // 本质上是调用的 t.String()
	fmt.Printf("Kind: %v\n", t.Kind()) //
	fmt.Printf("Name: %v\n", t.Name())
	fmt.Printf("Size: %v\n", t.Size())
	// 函数可以额外使用的方法
	// 参数的类型
	for i := 0; i < t.NumIn(); i++ {
		fmt.Printf("%dth Param Type: %v\n", i, t.In(i)) // 获取函数类型的第 i 个参数 reflect.Type
	}
	// 返回值类型
	for i := 0; i < t.NumOut(); i++ {
		fmt.Printf("%dth Return Type: %v\n", i, t.Out(i)) // 获取函数类型的第 i 个返回值的 reflect.Type
	}
}

func main() {

	var f = func(a, b int) func() int {
		var c = a + b
		return func() int {
			return c
		}
	}
	reflectFuncType(f)
}
```

### 结构体类型信息

下表列出了 `reflect.Type` 接口中提供的用于获取结构体成员的信息的接口

| 方法                                           | 说明                                       |
| -------------------------------------------- | ---------------------------------------- |
| **字段相关的**                                    |                                          |
| `t.Field(i int)`                             | 返回索引 `i` 对应的结构体字段的类型信息 `StructField`     |
| `t.NumField()`                               | 返回结构体成员字段数量                              |
| `t.FieldByName(name string)`                 | 返回`name` 对应的结构体字段的l类型信息 `StructField`    |
| `t.FieldByIndex(index []int)`                | 多层成员访问时，根据 `[]int` 提供的每个结构体的字段索引，返回字段的信息 |
| `t.FieldByNameFunc(match func(string) bool)` | 根据传入的匹配函数匹配需要的字段                         |
|                                              |                                          |
| **方法相关的**                                    |                                          |
| `t.NumMethod()`                              | 返回该类型的方法集中方法的数目                          |
| `t.Method(i)`                                | 返回该类型方法集中的第 `i` 个方法                      |
| `t.MethodByName(name)`                       | 根据 `name` 返回该类型方法集中的方法                   |

#### `reflect.StructField` 类型

`reflect.StructField` 结构体用来描述结构体中的一个字段的信息，它的定义如下

```go
type StructField struct {
    // 参见 http://golang.org/ref/spec#Uniqueness_of_identifiers
    Name    string      // Name 是字段的名字
    PkgPath string      // PkgPath 是非导出字段的包路径，对导出字段该字段为""。
    Type      Type      // 字段的类型
    Tag       StructTag // 字段的标签
    Offset    uintptr   // 字段在结构体中的字节偏移量
    Index     []int     // 用于 Type.FieldByIndex 时的索引切片
    Anonymous bool      // 是否匿名字段
}
```

#### 字段的类型信息

```go
package main

import (
	"fmt"
	"reflect"
)

type User struct {
	Name string `json:"name"` // 导出
	age  int    `json:"age"`  // 非导出
}

func reflectStructFieldType(b any) {

	// 获取 b 的类型信息
	t := reflect.TypeOf(b)

	// 基本类型的类型信息
	fmt.Printf("Type: %v\n", t)        // 本质上是调用的 t.String()
	fmt.Printf("Kind: %v\n", t.Kind()) //
	fmt.Printf("Name: %v\n", t.Name())
	fmt.Printf("Size: %v\n", t.Size())
	fmt.Println("-----------------------------------")
	fmt.Printf("FieldName \t FieldType \t FieldTag\n")
	// 获取字段信息
	switch t.Kind() {
	case reflect.Ptr:
		// 如果使结构体指针。要反射结构体的字段，需要通过 Elem() 方法去获取指针对应的对象
		for i := 0; i < t.Elem().NumField(); i++ {
			field := t.Elem().Field(i)
			fmt.Printf("%9v\t%9v\t %8v\n", field.Name, field.Type, field.Tag)
		}
	case reflect.Struct:
		// 使用结构体实例
		for i := 0; i < t.NumField(); i++ {
			field := t.Field(i)
			fmt.Printf("%9v\t%9v\t %8v\n", field.Name, field.Type, field.Tag)
		}
	}
}

func main() {
	var user = User{
		Name: "张三",
		age:  20,
	}
	reflectStructFieldType(user)
}
```

这段代码的运行结果为

```
Type: main.User
Kind: struct
Name: User
Size: 24
-----------------------------------
FieldName        FieldType       FieldTag
     Name          string        json:"name"
      age             int        json:"age"
```

#### 方法的类型信息

```go
package main

import (
	"fmt"
	"reflect"
)

type User struct {
	Name string `json:"name"` // 导出
	age  int    `json:"age"`  // 非导出
}

func (u *User) SetName(name string) {
	u.Name = name
}

func (u *User) SetAge(age int) {
	u.age = age
}

func (u User) Info() {
	u.info()
}

func (u User) info() {

}

func reflectMethodType(b any) {

	// 获取 b 的类型信息
	t := reflect.TypeOf(b)

	// 基本类型的类型信息
	fmt.Printf("Type: %v\n", t)        // 本质上是调用的 t.String()
	fmt.Printf("Kind: %v\n", t.Kind()) //
	fmt.Printf("Name: %v\n", t.Name())
	fmt.Printf("Size: %v\n", t.Size())
	fmt.Println("-----------------------------------")
	fmt.Printf("MethodName \t MethodType\n")
	fmt.Println("-----------------------------------")
	switch t.Kind() {
	case reflect.Ptr:
		// 通过 实例指针 可以反射到所有方法。当然，非导出方法不能被反射到
		for i := 0; i < t.NumMethod(); i++ {
			method := t.Method(i)
			fmt.Printf("%9v\t%9v\n", method.Name, method.Type)
		}
	case reflect.Struct:
		// 通过 实例 不能反射到 指针接收者 方法。非导出方法不能被反射到
		for i := 0; i < t.NumMethod(); i++ {
			method := t.Method(i)
			fmt.Printf("%9v\t%9v\n", method.Name, method.Type)
		}
	}
}

func main() {

	var user = User{
		Name: "张三",
		age:  20,
	}

	reflectMethodType(&user)

}
```

> [!important] 
> 
> + 结构体的 **指针** 可以反射到 **所有导出的方法**。即，方法的接收者可以是值接收者和指针接收者，它们都能被反射
>
>+ 结构体的 **值** 不能反射到 指针接收者 方法。即，只能反射导出的值接收者方法
>

### 接口类型的信息

我们通常会判断某个类型是否实现了某个接口。下面的伪代码片段展示了如何 **判断一个类型是否实现了某个接口**

```go
type Runner interface {
	Run()
}

type Person struct {
	Name   string `json:"name"`
	Age    int
}

tom := Person{"Tom", 20}
t1 := reflect.TypeOf(tom)

t1.Implements(需要接口Runner的Type，也就是要获取接口类型Type)
```

怎么拿到接口类型的`reflet.Type`呢？通过，前面的学习，我们很容易写成下面的形式

```go
tom := Person{"Tom", 20}
t1 := reflect.TypeOf(tom)
fmt.Println(t1)

var i Runner = &tom
t2 := reflect.TypeOf(i) // *main.Person
fmt.Println(t2)

t1.Implements(t2) //  reflect: non-interface type passed to Type.Implements
```

> [!tip]
> 
> 错误提示我们 `Implements()` 方法需要的是一个 **接口类型**  的`reflect.Type` 类型的对象作为参数。
> 
> 请注意：`var i Runner` 声明的变量在复制后，其动态类型变为了 `*main.Person`，**反射获取的是接口变量的动态类型**
> 

这里需要使用一个技巧，**接口类型的可以声明指针变量**。例如，这里我们可以声明 `*Runner` 类型的变量，然后通过这个变量获取 `*Runner` 的类型信息

```go
var pr *Runner
t2 := reflect.TypeOf(pr) // *main.Runner 的 reflect.Type
```

> [!attention] 
> 
> 请注意：反射 `*Runner` 类型的变量获取的是 `*Runner` 类型的  `reflect.Type`，想要获取 `Runner` 类型的 `reflect.Type`，只需要使用 `Elem()` 方法解引用即可
> 

```go
package main

import (
	"fmt"
	"reflect"
)

type Runner interface {
	Run()
}

type User struct {
	Name string `json:"name"` // 导出
	age  int    `json:"age"`  // 非导出
}

func (u *User) Run() {

}

func main() {

	var user = User{
		Name: "张三",
		age:  20,
	}

	t1 := reflect.TypeOf(user)  // main.User
	var pr *Runner
	t2 := reflect.TypeOf(pr) // *main.Runner 的 reflect.Type
	// 此时，通过 Elem() 就可以获取指针指向对象的类型了
	if t1.Implements(t2.Elem()) {
		fmt.Printf("Type %v implement interface %v\n", t1, t2.Elem())
	} else {
		fmt.Printf("Type %v don't implement interface %v\n", t1, t2.Elem())
	}
}
// Type main.User don't implement interface main.Runner
```

> [!tip] 
> 
> `t1` 是 `main.User` 类型的 `reflect.Type`。由于我们的方法实现为 `*main.User` 作为接收者类型，所以 `main.User` 并未实现 `main.Runner` 接口
> 

下面的示例中，我们使用 `*main.User` 类型的变量反射获取类型信息，

```go
package main

import (
	"fmt"
	"reflect"
)

type Runner interface {
	Run()
}

type User struct {
	Name string `json:"name"` // 导出
	age  int    `json:"age"`  // 非导出
}

func (u *User) Run() {

}

func main() {

	var user = User{
		Name: "张三",
		age:  20,
	}

	t1 := reflect.TypeOf(&user)
	var pr *Runner
	t2 := reflect.TypeOf(pr) // *main.Runner 的 reflect.Type
	// 此时，通过 Elem() 就可以获取指针指向对象的类型了
	if t1.Implements(t2.Elem()) {
		fmt.Printf("Type %v implement interface %v\n", t1, t2.Elem())
	} else {
		fmt.Printf("Type %v don't implement interface %v\n", t1, t2.Elem())
	}
}
// Type *main.User implement interface main.Runner
```

> [!attention] 
> 
> 也就是说，类型 `Type` 在实现接口时以指针接收者实现，那么就意味着只有 `*Type` 实现了该接口
> 

如果某个类型 `Type` 以 **值接收者** 实现了某个接口，那么 `*Type` 和 `Type` 都实现了这个接口

```go
package main

import (
	"fmt"
	"reflect"
)

type Runner interface {
	Run()
}

type Person struct {
	Name string `json:"name"`
	Age  int
}

func (p Person) Run() {

}

func main() {

	tom := &Person{"Tom", 20}
	t1 := reflect.TypeOf(tom) // main.Person
	fmt.Println(t1)

	var i *Runner           // (*Runnr)(nil)
	t2 := reflect.TypeOf(i) // *main.Runnr：依旧不是 Runner 的 Type
	fmt.Println(t2)
	t2 = t2.Elem() // Runner： 通过指针查找其对象的类型信息
	fmt.Println(t2)

	fmt.Printf("t1.Implements(t2): %v\n", t1.Implements(t2))
}

// Output
// t1.Implements(t2): true
```

如果某个类型 `Type` 以 **指针接收者** 实现的接口，那么仅有 `*Type` 实现这个接口，而 `Type` 并没有实现这个接口

```go
package main

import (
	"fmt"
	"reflect"
)

type Runner interface {
	Run()
}

type Person struct {
	Name string `json:"name"`
	Age  int
}

func (p *Person) Run() {

}

func main() {

	tom := Person{"Tom", 20}
	t1 := reflect.TypeOf(tom) // main.Person
	fmt.Println(t1)

	var i *Runner           // (*Runnr)(nil)
	t2 := reflect.TypeOf(i) // *main.Runnr：依旧不是 Runner 的 Type
	fmt.Println(t2)
	t2 = t2.Elem() // Runner： 通过指针查找其对象的类型信息
	fmt.Println(t2)

	fmt.Printf("t1.Implements(t2): %v\n", t1.Implements(t2)) 
}

// Output
// t1.Implements(t2): false；很明显 Person 没有实现 Runner 接口
```

```go
package main

import (
	"fmt"
	"reflect"
)

type Runner interface {
	Run()
}

type Person struct {
	Name string `json:"name"`
	Age  int
}

func (p *Person) Run() {

}

func main() {

	tom := &Person{"Tom", 20}
	t1 := reflect.TypeOf(tom) // main.Person
	fmt.Println(t1)

	var i *Runner           // (*Runnr)(nil)
	t2 := reflect.TypeOf(i) // *main.Runnr：依旧不是 Runner 的 Type
	fmt.Println(t2)
	t2 = t2.Elem() // Runner： 通过指针查找其对象的类型信息
	fmt.Println(t2)

	fmt.Printf("t1.Implements(t2): %v\n", t1.Implements(t2)) //  reflect: non-interface type passed to Type.Implements
}

// Output
// t1.Implements(t2): true
```

`var i *Runner` 的初始值是和 `(*Runner)(nil)` 是等价的。因此，我们可以简化代码

```go
t2 := reflect.TypeOf((*Runner)(nil)).Elem() // Runner 的 Type
fmt.Println(t2)
```

> [!tip] 
> `(*Runner)(nil)` 是一个强制类型转换，`*Runner` 把 **非类型化的空指针**(`nil`) 强制类型转换为 `*Runner` 类型空指针
> 


## `reflect.Value` 类型

`reflect.Value` 是一个结构体，代表了一个变量的 **值信息**

```go
type Value struct {
	// typ_ 保存值所表示的值的类型
	// 使用 typ 方法进行访问以避免 v 的转义
	typ_ *abi.Type

	// 指针值数据，如果设置了 flagIndir，则是指向数据的指针。
	// 只要设置了 flagIndir 或 type.pointer()为true，就有效。
	ptr unsafe.Pointer

	// flag 保存了有关该值的元数据。
	//
	// 最低的 5 位表示值的类型，与 type.Kind() 类似。 底层类型信息
	//
	// 下一组比特位是标志位:
	//   - flagStickyRO: 通过未导出的非嵌入式字段获取，因此只读
	//   - flagember:    通过未导出的嵌入字段获取，只读
	//   - flagIndir:    val 保存了一个指向数据的指针
	//   - flagAddr:     v.CanAddr 为 true(意味着 flagIndir 和 ptr 非nil)
	//   - flagMethod:   v 是一个方法值。
	// 如果ifaceIndir(typp)，代码可以假定设置了flagIndir。
	//
	//剩余的22位表示方法的编号。
	//如果flag.kind() != Func，代码可以假定flag方法未设置。
	flag
}

type flag uintptr

const (
	flagKindWidth        = 5 // there are 27 kinds
	flagKindMask    flag = 1<<flagKindWidth - 1
	flagStickyRO    flag = 1 << 5
	flagEmbedRO     flag = 1 << 6
	flagIndir       flag = 1 << 7
	flagAddr        flag = 1 << 8
	flagMethod      flag = 1 << 9
	flagMethodShift      = 10
	flagRO          flag = flagStickyRO | flagEmbedRO
)
```

`reflect.ValueOf(rawValue)` 返回 `reflect.Value` 类型，包含着与原始值有关的信息

```go
package main

import (
	"fmt"
	"reflect"
)

func reflectValueOf(x any) {

	v := reflect.ValueOf(x) // 反射获取原始值信息
	fmt.Printf("type: %[1]T \t %[1]v\n", v)  // type: reflect.Value      10
}

func main() {
	var a int = 10
	reflectValueOf(a)
}
```

### 通过 `reflect.Value` 获取 `reflect.Type`

```go
// Kind 返回 v 的种类。如果 v 是零值(Value.IsValid 返回 false)， Kind 返回无效。
func (v Value) Kind() Kind

// Type返回v的类型
func (v Value) Type() Type
```

### `reflect.Value` 和原始值

`reflect.Value` 与原始值之间可以通过 **值包装** 和 **值获取** 相互转化

| 方法签名                    | 说明                                                          |
| ----------------------- | ----------------------------------------------------------- |
| 获取接口值                   |                                                             |
| `v.Interface()`         | 以 `any` 的形式返回 `v` 的基础值                                      |
|                         |                                                             |
| 获取数值                    |                                                             |
| `v.Int()`               | 以 `int64` 返回 `v` 的基础值。 `v` 的 `Kind` 必须是 `Int{xx}`           |
| `v.Uint()`              | 以 `uint64` 返回 `v` 的基础值。 `v` 的 `Kind` 必须是 `Uint{xx}`         |
| `v.Float()`             | 以 `float64` 返回 `v` 的基础值。`v`的 `Kind` 必须是 `Float{xx}`         |
|                         |                                                             |
| 获取布尔值                   |                                                             |
| `v.Bool()`              | 以 `bool` 返回 `v` 的基础值。`v`的 `Kind` 必须是 `Bool`                 |
|                         |                                                             |
| 获取字节切片                  |                                                             |
| `v.Bytes()`             | 以 `[]byte` 返回 `v` 的基础值。`v` 的基础值必须时字节数组                      |
|                         |                                                             |
| 获取字符串                   |                                                             |
| `v.String() string`     | 以 `string` 返回 `v` 的基础值                                      |
|                         |                                                             |
| 获取切片                    |                                                             |
| `v.Slice(i, j int)`     | 返回 `v[i:j]` 的 `Value`。`v` 的 `Kind` 必须是 `Array Slice String` |
| `v.Slice3(i, j, k int)` | 返回`v[i:j:k]` 的 `Value`                                      |

`reflect.ValueOf()` 返回的是 `reflect.Value` 类型，其中包含了原始值的值信息。 `reflect.Value` 与原始值之间可以互相转换

```go
var a = 100
v := reflect.ValueOf(a) // 原始值 => Value，原始值包装
i := v.Interface() // 等价于 var i interface{} = (v's underlying value)，Value => 原始值，值获取
```

### 基本类型的值信息

```go
package main

import (
	"fmt"
	"reflect"
)

func reflectValue(x interface{}) {
	v := reflect.ValueOf(x) // 反射值的信息
	k := v.Kind()           // 获取值的底层类型
	switch k {
	case reflect.Int, reflect.Int8, reflect.Int16, reflect.Int32, reflect.Int64:
		// 获取int类型的值
		fmt.Printf("Type is %v, value is %d\n", v.Type(), v.Int())
	case reflect.Uint, reflect.Uint8, reflect.Uint16, reflect.Uint32, reflect.Uint64, reflect.Uintptr:
		// 获取uint类型的值
		fmt.Printf("Type is %v, value is %d\n", v.Type(), v.Uint())
	case reflect.Float32, reflect.Float64:
		// 获取float类型的值
		fmt.Printf("Type is %v, value is %f\n", v.Type(), v.Float())
	case reflect.Bool:
		// 获取bool类型的值
		fmt.Printf("Type is %v, value is %t\n", v.Type(), v.Bool())
	case reflect.Complex64, reflect.Complex128:
		// 获取complex类型的值
		fmt.Printf("Type is %v, value is %v\n", v.Type(), v.Complex())
	case reflect.String:
		// 获取string类型的值
		fmt.Printf("Type is %v, value is %s\n", v.Type(), v.String())
	}
}

func main() {
	var i int = 100
	reflectValue(i) // Type is int, value is 100

	var i8 int8 = 0x7f
	reflectValue(i8) // Type is int8, value is 127
	var i16 int16 = 0x7fff
	reflectValue(i16) // Type is int16, value is 32767
	var i32 int32 = 0x7fffffff
	reflectValue(i32) // Type is int32, value is 2147483647
	var i64 int64 = 0x7fffffffffffff
	reflectValue(i64) // Type is int64, value is 9223372036854775807
	var f32 float32 = 3.14
	reflectValue(f32) // Type is float32, value is 3.1400000572204586
	var f64 float64 = 3.1415926
	reflectValue(f64) // Type is float64, value is 3.141592653589793
	var c64 complex64 = 5 + 10i
	reflectValue(c64) // Type is complex64, value is (5+10i)
	var c128 complex128 = 5 + 10i
	reflectValue(c128) // Type is complex128, value is (5+10i)
	var ui uint = 100
	reflectValue(ui) // Type is uint, value is 100
	var ui8 uint8 = 0x7f
	reflectValue(ui8) // Type is uint8, value is 127
	var ui16 uint16 = 0x7fff
	reflectValue(ui16) // Type is uint16, value is 32767
	var ui32 uint32 = 0x7fffffff
	reflectValue(ui32) // Type is uint32, value is 2147483647
	var ui64 uint64 = 0x7fffffffffffff
	reflectValue(ui64) // Type is uint64, value is 9223372036854775807
	var s string = "hello"
	reflectValue(s) // Type is string, value is hello
	var b bool = true
	reflectValue(b) // Type is bool, value is true
}
```

### 空值和有效性判断

| 方法签名          | 说明                                                                                         |
| ------------- | ------------------------------------------------------------------------------------------ |
| `v.IsValid()` | 判断 `v` 的原始值是否有效。当原始值本身非法时，返回 `false`                                                       |
| `v.IsZero()`  | 判断 `v` 的原始值是否是零值。如果值无效则 `panic`                                                            |
| `v.IsNil()`   | 判断 `v` 的原始值是否为 `nil`。原始值的类型须是 `chan` `func` `interface` `map` `pointer` `slice`，否则 `panic` |

```go
package main

import (
	"fmt"
	"reflect"
)

func main() {
	var a = 100
	v := reflect.ValueOf(a)
	fmt.Println(
		v.IsValid(), // true
		// v.IsNil(), // 必须是 chan、func、interface、map、pointer、slice，否则 panic
		v.IsZero(), // false
	)
	var b *int
	v = reflect.ValueOf(b)
	fmt.Println(
		v.IsValid(), // true
		v.IsNil(),   // true
		v.IsZero(),  // true
	)
}
```

> [!tip] 
> 注意，上例中 `reflect.ValueOf(nil).IsValid()` 为 `false`，而 `reflect.ValueOf(b).IsValid()` 为 `true`，因为 `b` 是有类型的，它是 `*int` 不过是空指针罢了，所以有效，而 `nil` 不是

### 反射结构体值

下表列出了获取结构体值信息的一些方法。当 `reflect.Value` 对象不是结构体的值信息时，则会 `panic`

| 方法签名                          | 说明                           |
| :---------------------------- | ---------------------------- |
| **获取字段值信息**                   |                              |
| `v.NumField()`                | 返回 **结构体成员字段数量**             |
| `v.Field(i int)`              | 返回索引 `i` 对应的字段的 `Value` 对象   |
| `v.FieldByName(name string)`  | 返回 `name` 对应的字段的 `Value` 对象  |
| `v.FieldByIndex(index []int)` | 返回 `index` 对应的字段的 `Value` 对象 |
|                               |                              |
| **获取方法值信息**                   |                              |
| `v.Method(i int)`             | 返回索引 `i` 对应的方法的 `Value` 对象   |
| `v.MethodByName(name string)` | 返回`name` 对应的方法的 `Value` 对象   |
|                               |                              |
| **调用方法**                      |                              |
| `v.Call(in []Value)`          | 调用方法                         |

```go
package main

import (
	"fmt"
	"reflect"
)

type person struct {
	Name string
	Age  int
}

func (p person) Speak(prefix, suffix string) string {
	fmt.Println("Speak Called")
	return fmt.Sprintf("%s I am %s and I am %d years old %s.", prefix, p.Name, p.Age, suffix)
}

func main() {
	var a = person{
		Name: "Alice",
		Age:  25,
	}

	t := reflect.TypeOf(a)
	v := reflect.ValueOf(a)

	fmt.Printf("Type: %v \t Value: %v\n", t, v)

	// Type 字段类型定义的信息
	for i := 0; i < t.NumField(); i++ {
		field := t.Field(i)
		fmt.Println(
			field.Name,         // 字段名
			field.Index,        // 字段索引
			field.Offset,       // 字段偏移字节数
			field.Anonymous,    // 是否匿名字段
			field.IsExported(), // 是否是导出的
		)
	}

	// Value 字段值的信息: 字段值是否有效 字段值是否为零  获取字段值
	for i := 0; i < v.NumField(); i++ {
		field := v.Field(i)
		fmt.Println(
			"字段值是否有效:",
			field.IsValid(), // 字段值是否有效
			"字段值是否为零值:",
			field.IsZero(), // 字段值是否为零值
			"字段值:",
			field.Interface(), // 字段值以 interface{} 类型返回。注意，需要字段需要导出
		)
	}
	fmt.Println("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~")
	// 获取方法的值信息
	for i := 0; i < v.NumMethod(); i++ {
		method := v.Method(i)
		// 判断方法是否有效
		fmt.Println(
			"方法是否有效: ",
			method.IsValid(),
			"方法是否为 nil: ",
			method.IsNil(),
		)

		if method.IsValid() && !method.IsNil() {
			// 调用方法
			// 构造参数
			params := []reflect.Value{
				reflect.ValueOf("!!!!!!!!"),
				reflect.ValueOf("!!!!!!!!"),
			}

			// 调用方法
			result := method.Call(params) 
			// Call 将 method 的返回值以 []reflect.Value 方式返回
			fmt.Println(result)
		}
	}
}
```

这段代码的输出结果为

```
Type: main.person        Value: {Alice 25}
Name [0] 0 false true
Age [1] 16 false true
字段值是否有效: true 字段值是否为零值: false 字段值: Alice
字段值是否有效: true 字段值是否为零值: false 字段值: 25
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
方法是否有效:  true 方法是否为 nil:  false
Speak Called
[!!!!!!!! I am Alice and I am 25 years old !!!!!!!!.]
```

### 设置值

|          方法签名           | 说明                                                                   |
| :---------------------: | :------------------------------------------------------------------- |
|       `v.Elem()`        | 返回 `v` 指向的元素的 `Value`，类似于语言层 `*` 操作。当值类型不是 **指针** 或 **接口** 时 `panic` |
|       `v.Addr()`        | 返回可寻址的 `Value` 地址，类似于语言层 `&` 操作。当值不可寻址时 `panic`                      |
|      `v.CanAddr()`      | 判断 `Value` 是否可寻址                                                     |
|      `v.CanSet()`       | 判断 `Value` 能否被设置值。要求值 **可寻址且是导出的字段**                                 |
|                         |                                                                      |
|         **设置值**         |                                                                      |
|    `v.Set(x Value)`     | 将值设置为传入的反射值对象的值                                                      |
|   `v.SetInt(x int64)`   | 使用 `int64` 设置值。当 `v` 的类型不是 `int{*}` 类类型时，则会 `panic`                  |
|  `v.SetUInt(x uint64)`  | 使用 `uint64` 设置值。当 `v` 的类型不是 `uint{*}` 时会发生 `panic`                   |
| `v.SetFloat(x float64)` | 使用 `float64` 设置值。当`v` 的类型不是 `float{*}` 时会发生 `panic`                  |
|   `v.SetBool(x bool)`   | 使用 `bool` 设置值。当 `v` 的类型不是 `bool` 时会发生 `panic`                        |
| `v.SetBytes(x []byte)`  | 设置字节数组 `[]bytes` 值。当值的类型不是 `[]byte` 时会发生 `panic`                     |
| `v.SetString(x string)` | 设置字符串值。当值的类型不是 `string` 时会发生 `panic`                                 |

> [!tip] 
> 如果 `CanSet()` 返回 `false`，调用以上 `Set*` 方法都会 `panic`

#### `Value` 可寻址概念

Go语言中类似 `x`, `x.f[1]` 和 `*p` 形式的表达式都可以表示变量，但是其它如 `x + 1` 和 `f(2)` 则不是变量。**一个变量就是一个可寻址的内存空间**，里面存储了一个值，并且 **存储的值可以通过内存地址来更新**。

对于 `reflect.Value` 也有类似的区别。有一些 `reflect.Value` 是可取地址的；其它一些则不可以。考虑以下的声明语句：

```go
x := 2                   // value   type    variable?
a := reflect.ValueOf(2)  // 2       int     no
b := reflect.ValueOf(x)  // 2       int     no
c := reflect.ValueOf(&x) // &x      *int    no
d := c.Elem()            // 2       int     yes (x)
```

- `a` 对应的变量不可取地址。因为 `a` 中的值仅仅是整数 `2` 的拷贝副本
- `b` 中的值也同样不可取地址。因为 `b` 中的值仅仅是 `x` 的拷贝
- `c` 中的值还是不可取地址，它只是一个指针 `&x` 的拷贝
- 但是对于 `d`，它是 `c` 的解引用方式生成的，指向另一个变量，因此是可取地址的。我们可以通过调用 `reflect.ValueOf(&x).Elem()`，来获取任意变量 `x` 对应的可取地址的 `Value`

> [!warning] 
> 
> 实际上，**所有通过 `reflect.ValueOf(x)` 返回的 `reflect.Value` 都是不可取地址的**

每当我们通过指针间接地获取的 `reflect.Value` 都是可取地址的，即使开始的是一个不可取地址的 `Value`。在反射机制中，所有关于是否支持取地址的规则都是类似的
- 例如，`slice` 的索引表达式 `e[i]` 将隐式地包含一个指针，它就是可取地址的，即使开始的 `e` 表达式不支持也没有关系
- 以此类推，`reflect.ValueOf(e).Index(i)` 返回的 `Value` 也是可取地址的，即使原始的 `reflect.ValueOf(e)` 不支持也没有关系

#### 修改普通变量值

要从变量对应的可取地址的 `reflect.Value` 来访问变量需要三个步骤

- 第一步是调用 `Addr()` 方法，它返回一个 `Value`，里面保存了指向变量的指针

- 然后，在 `Value` 上调用 `Interface()` 方法，也就是返回一个 `interface{}`，里面包含指向变量的指针

- 最后，如果我们知道变量的类型，我们可以使用类型的断言机制将得到的 `interface{}` 类型的接口强制转为普通的类型指针

这样我们就可以通过这个普通指针来更新变量了

```go
package main

import (
	"fmt"
	"reflect"
)

func main() {
	x := 2

	// 这是不可寻址的。reflect.Value 只是 x 的一个副本
	// v := reflect.ValueOf(x)

	// 这也是不可寻址的。 reflect.Value 只是 &x 的一个副本（一个整数）
	// v := reflect.ValueOf(&x)

	// 这是可寻址的了。reflect.Value 是 &x 的解引用，即是 x 的别名
	v := reflect.ValueOf(&x).Elem()

	// 1. 获取指向 x 指针的 reflect.Value
	addr := v.Addr()
	// 2. 获取 addr(reflect.Value 对象) 中保存指向 x 的指针，使用 Interface
	i := addr.Interface()
	// 3. 断言获得普通指针
	px, ok := i.(*int)

	if !ok {
		return
	}

	*px = 20
	fmt.Println(x)

}
```

或者，不使用指针，而是通过调用  _可寻址的 `reflect.Value`_ 的 `reflect.Value.Set` 方法来更新对于的值

```go
package main

import (
	"fmt"
	"reflect"
)

func main() {
	x := 2

	// 将 x 的地址包装到一个 reflect.Value 中，
	// 调用 Elem 方法，获得一个解引用值，并包装为一个 reflect.Value
	addr := reflect.ValueOf(&x).Elem()

	// 使用 Set 方法

	addr.Set(reflect.ValueOf(4)) // 将值设置为4
	fmt.Println(x)               // 4
	addr.SetInt(100)             // 设置 int 类型的值 100
	fmt.Println(x)               // 100
}
```

####  **修改结构体字段值**

> [!important] 
> **只有导出的字段，才能被反射修改值**
> 
> 结构体成员中，如果字段没有被导出，不能通过反射修改

```go
package main

import (
	"fmt"
	"reflect"
)

type person struct {
	Name string
	age  int
}

func main() {
	p := person{
		Name: "John",
		age:  30,
	}

	v := reflect.ValueOf(&p).Elem()

	// 导出字段
	nameField := v.FieldByName("Name")
	fmt.Println("Name: ", nameField, nameField.CanAddr(), nameField.CanSet())
	nameField.SetString("hello")

	fmt.Printf("%#v\n", p) // main.person{Name:"hello", age:30}

	// 非导出字段不能被设置
	ageField := v.FieldByName("age")
	fmt.Println("age: ", ageField, ageField.CanAddr(), ageField.CanSet())
	// ageField.SetInt(40) // panic
}
```

### 创建实例

通过反射可以创建一个实例：需要用到 `reflect.New(typ reflect.Type) reflect.Value` ，简单讲就是将 `Type` `New` 成 `Value`

```go
package main

import (
	"fmt"
	"reflect"
)

func main() {
	// 获取某个类型的 Type
	t := reflect.TypeOf(int(0))

	// 创建 *Type 的 Value。相当于 new()
	v := reflect.New(t)                               // 创建一个指向 int 类型的指针
	fmt.Printf("Type: %v \t Value:%v\n", v.Type(), v) // Type: *int 	 Value:0xc0000ac010

	// 设置值
	value := v.Elem()
	fmt.Printf("type: %v \t value: %v\n", value.Type(), value) // type: int 	 value: 0
	value.Set(reflect.ValueOf(10))                             // 设置值
	fmt.Printf("type: %v \t value: %v\n", value.Type(), value) // type: int 	 value: 10
}
```

## 总结

![Pasted image 20250701225836|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755787575447-12a70900d7a4490d829e5c6ae66cc81b.png)

反射可以从接口值当中通过 `TypeOf()` 获取 `Typ` 对象，通过 `ValueOf()` 获取 `Value` 对象
- `Type` 对象只有类型信息，如果想获得值、修改值，可以通过 `New()` 获得 `Value` 对象，这个 `Value` 对象是指向该类型的零值的指针
- `Value` 对象可以通过 `Interface()` 方法转换成接口值
- `Value` 对象可以通过 `Type()` 返回 `Type` 对象

反射是一个强大并富有表现力的工具，能让我们写出更灵活的代码。 但是反射不应该被滥用，原因有以下三个。
- 基于反射的代码是极其脆弱的，反射中的类型错误会在真正运行的时候才会引发 `panic`，那很可能是在代码写完的很长时间之后。
- 大量使用反射的代码通常难以理解。
- 反射的性能低下，基于反射实现的代码通常比正常代码运行速度慢一到两个数量级。
