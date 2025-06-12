# 结构体

# 结构体

Go 中的结构体(`struct`)是一种 _值类型_，不同的是 Go 可以为结构体定义 _方法_。也就是说，Go 的结构体有点像面向对象语言中的 **类**，但不完全是，Go 也没打算真正实现面向对象范式

结构体通过关键字 `struct` 定义结构体。但是，单独的 `struct` 是不可以的，需要与 `var` 或 `type` 关键指一起使用
+ 与 `var` 使用是定义结构体变量（**匿名结构体**）
+ 与 `type` 一起使用则是定义 **结构体类型**

## 结构体类型

使用 `type` 定义结构体，可以把结构体看做 **类型** 使用。必须指定结构体的 **字段（属性）名称** 和 **类型**

> [!important] 使用 `type` 和 `struct` 可以定义一个结构体类型，它是一个结构体变量的蓝图

例如，我们想要描述一个网站用户的属性：`用户ID` `姓名` `年龄` `邮箱` `密码`。如果使用基本数据类型，我们可能会得到散落在各处的变量声明

```go
var ID       int    // 用户ID
var Age      int    // 年龄
var Name     string // 用户名
var Email    string // 邮箱
var Password string // 密码
```

这种散落的变量造成程序的混乱，并且难以管理。因此，我们可以使用 **结构体** 将这些散落的变量使用 `struct` 定义成一个新类型 `User`。即

```go
type User struct {
	ID       int    // 用户ID
	Age      int    // 年龄
	Name     string // 用户名
	Email    string // 邮箱
	Password string // 密码
}
```

> [!warning] 注意：仅仅是定义了结构体类型，并没有定义结构体变量(即，没有分配内存空间)
> + 只有像使用基本类型一样使用 `User` 定义变量之后，才会分配内存

在结构体内部定义的标识符称为 **字段**(或 **成员**)。相同类型的字段可以写在同一行

```go
type User struct {
	ID, Age int  // 用户ID，年龄
	Name, Email, Password string // 用户名，邮箱，密码
}
```

在定义好结构体类型之后，可以使用该类型声明变量

## 结构体变量

在完成结构体类型的定义之后，此时并没有在内存中创建对象。定义结构体变量与定义普通变量是一样的，可以通过 `var` 和 `:=` 两种方式定义

```go
package main

import "fmt"

type User struct {
	ID       int    // 用户ID
	Age      int    // 年龄
	Name     string // 用户名
	Email    string // 邮箱
	Password string // 密码
}

func main() {
	// 使用 var 声明一个结构体变量。结构体成员被初始化为其类型的零值。
	var u1 User
	fmt.Printf("%#v\n", u1) // main.User{ID:0, Age:0, Name:"", Email:"", Password:""}
	// 使用结构体字面值初始化
	u1 = User{1, 20, "Alice", "alice@example.com", "password123"}                                 // 按字段顺序初始化
	u1 = User{ID: 1, Age: 20, Name: "Alice", Email: "alice@example.com", Password: "password123"} // 指定字段初始化
	fmt.Printf("%#v\n", u1)
	// main.User{ID:1, Age:20, Name:"Alice", Email:"alice@example.com", Password:"password123"}
	
	// := 声明结构体变量
	u2 := User{ID: 2, Age: 25, Name: "Bob", Email: "bob@example.com", Password: "password456"}
	fmt.Printf("%#v\n", u2) // main.User{ID:2, Age:25, Name:"Bob", Email:"bob@example.com", Password:"password456"
}
```

> [!important] 结构体变量可以被认为是一个对象(实例)

## 可见性

> [!important] Go 最小封装单位：包
> Go 通过包封装数据和函数
> + 标识符以大写字母开头，包内和包外均可见
> + 标识符以小写字母开头，仅包内可见

结构体名称以大写开头时，`package` 内外皆可见，在此前提下，结构体中以大写开头的成员（属性、方法）在包外也可见

```go
package draw

import "fmt"

// Rectangle 在包外可见
type Rectangle struct {
	Width  int // 包外可见
	height int // 仅包内可见
}

func (r Rectangle) Draw() {
	fmt.Println("Drawing a rectangle with width", r.Width)
}

// area 仅包内可见
func (r Rectangle) area() int {
	return r.Width * r.height
}

// circle 仅包内可见  
type circle struct {  
    Radius int // 仅包内可见  
}
```

> [!tip] 如果结构体在包外不可见，那么该结构体的字段包外也不可见

## 访问与修改

通过结构体变量名 `.` 可以使用 _字段名称_ 访问字段值

```go
type User struct {
	ID       int    // 用户ID
	Age      int    // 年龄
	Name     string // 用户名
}

u1 := User{103, "John", "Beijing"} // 无字段名称必须按照顺序给出全部字段值
fmt.Println(u1.id, u1.name)
```

通过字段也可以修改它的值

```go
u1 := User{103, "John", "Beijing"} // 无字段名称必须按照顺序给出全部字段值
fmt.Println(u1)
u1.name = "Tom"
u1.score = 88
fmt.Println(u1)
```

## 方法

Go 中结构体支持定义方法。当然，方法并不是只能在结构体上定义，而是所有的 **命名类型** 都可以定义方法

利用下面形式为结构体组合方法，这是很方便自由地扩展方式。

```go
package main

import "fmt"

type User struct {
	id         int
	name, addr string
	score      float32
}


func (u User) getName() string {
	return u.name
}

func main() {
	u1 := User{103, "John", "Beijing", 98.5}
	name := u1.getName()
	fmt.Println(name)
}
```

## 结构体指针

**_再次强调结构体类型是值类型_**，下面的代码演示了这个特性

```go
package main

import "fmt"

func main() {
	type Person struct { // 定义结构体类型
		name string
		city string
		age  int
	}

	person4 := Person{
		name: "dyy",
		city: "上海",
		age:  12,
	}

	person5 := person4

	fmt.Printf("%#v\n", person4) // main.Person{name:"dyy", city:"上海", age:12}
	fmt.Printf("%#v\n", person5) // main.Person{name:"dyy", city:"上海", age:12}
	
	person5.age = 25
	fmt.Printf("%#v\n", person4) // main.Person{name:"dyy", city:"上海", age:12}
	fmt.Printf("%#v\n", person5) // main.Person{name:"dyy", city:"上海", age:25}
}
```

> [!tip] 显然 `person5 := person4` 会创建一个全新的对象
> + `person5` 的修改不会影响到 `person4`

既然，结构是值类型，那么当结构体作为参数传递给函数时，在函数内部操作的只是实参的一个副本。**当结构体比较大时，拷贝结构体可能会导致程序性能降低**。而且，**不利于结构体的修改**。

> [!important] 当结构体作为函数参数时，通常使用结构体指针

```go
package main

import "fmt"

// User 描述用户的结构体
type User struct {
	name string
	age  uint8
	sex  string
}

func main() {
	var pu *User            // 默认值 nil
	fmt.Printf("%#v\n", pu) // (*main.User)(nil)
	// 需要为 pu 分配内存
	pu = new(User)
	fmt.Printf("%#v\n", pu) // &main.User{name:"", age:0x0, sex:""}
	pu.name = "Almi"
	pu.age = 25
	pu.sex = "female"
	fmt.Printf("%#v\n", pu) // &main.User{name:"Almi", age:0x19, sex:"female"}

	var pu2 = &User{"Almi", 25, "female"} // &main.User{name:"Almi", age:0x19, sex:"female"}
	fmt.Printf("%#v\n", pu2)
}
```

> [!important] 注意
> 在 Go 语言中支持 **对 _结构体指针_ 直接使用 `.` 来访问结构体的成员**。并没有像 C 一样提供类似于 `->` 运算符
> 
> `pu.name = "Almi"` 其实底层是 `(*pu).name = "Almi"`，这是 Go 帮我们实现的自动转换

因为 **结构体通常通过指针处理**，可以用下面的写法来创建并初始化一个结构体变量，并返回结构体的地址

```go
pu = &User{"Almi", 25, "female"} 
```

该语句和下面的语句是等价的

```go
pu := new(User)
*pu = User{"Almi", 25, "female"} 
```

> [!note] 结构体字面值
> 形如
>
> ```go
 > Person{
 > 	name: "dyy",
 > 	city: "上海",
 > 	age:  12,
 > }
> ```
>
> 或
>
> ```go
> User{"Almi", 25, "female"}
> ```
>
> 称为 **结构字面值**，通常用于初始化结构体变量

## 结构体比较

如果结构体的 **_全部成员都是可以比较的_**，那么 **_结构体也是可以比较的_**，那样的话两个结构体将可以使用 `==` 或 `!=` 运算符进行比较。比较运算符 `==` 将比较两个结构体的每个成员，因此下面两个比较的表达式是等价的：

```go
package main

import (
	"fmt"
)

type Point struct {
	X, Y int
}

func main() {
	p := Point{1, 2}
	q := Point{2, 1}
	fmt.Println(p.X == q.X && p.Y == q.Y) // "false"
	fmt.Println(p == q)                   // "false"
}
```

> [!tip] 如果结构体的字段都是可比较的，那么该结构体类型就是可比较的 
> 可比较的结构体类型和其他可比较的类型一样，可以用于 `map` 的 `key` 类型

## 匿名结构体

**匿名结构体**：标识符直接使用 `struct` 部分结构体本身来作为类型，而不是使用 `type` 定义的有名字的结构体的标识符。

可以使用 `var` 、 `const` 、 `:=` 来定义匿名结构体。

`type` 定义结构体类型的标识符，可以反复定义其结构体实例，但是匿名结构体是一次性的

![[Pasted image 20240711073602.png|600]]

在定义一些 **临时数据结构** 等场景下还可以使用 **匿名结构体**

```go
var user struct {  
    Name string  
    Age  int  
}
```

例如

```go
package main

import (
	"fmt"
)

func main() {
	var user struct {
		Name string
		Age  int
	}
	user.Name = "小王子"
	user.Age = 18
	fmt.Printf("%#v\n", user)
}
```

> [!important] 匿名结构体，只是为了快速方便地得到一个结构体实例，而不是使用结构体创建 `N` 个实例。


## 匿名成员和结构体嵌入

考虑一个二维的绘图程序，提供了一个各种图形的库，例如矩形、椭圆形、星形和轮形等几何形状。这里是其中两个的定义

```go
type Circle struct {
	X, Y, Radius int
}

type Wheel struct {
	X, Y, Radius, Spokes int
}
```

一个 `Circle` 代表的圆形类型包含了标准圆心的 `X` 和 `Y` 坐标信息，和一个 `Radius` 表示的半径信息。一个 `Wheel` 轮形除了包含 `Circle` 类型所有的全部成员外，还增加了 `Spokes` 表示径向辐条的数量。我们可以这样创建一个 `wheel` 变量

```go
package main

func main() {
	var w Wheel
	w.X = 8
	w.Y = 8
	w.Radius = 5
	w.Spokes = 20
}
```

随着库中几何形状数量的增多，我们一定会 **注意到它们之间的 _相似和重复_ 之处**，所以我们可能为了便于维护而 **_将相同的属性独立出来_**

```go
type Point struct {
	X, Y int
}

type Circle struct {
	Center Point
	Radius int
}

type Wheel struct {
	Circle Circle
	Spokes int
}
```

这样改动之后 **结构体类型变的清晰** 了，但是这种修改同时也 **导致了访问每个成员变得繁琐**

```go
package main

func main() {
	var w Wheel
	w.Circle.Center.X = 8
	w.Circle.Center.Y = 8
	w.Circle.Radius = 5
	w.Spokes = 20
}
```

Go语言有一个特性让我们 **_只声明一个成员对应的数据类型_** 而 **_不指名成员的名字_**；这类成员就叫 **_匿名成员_**

> [!hint] 匿名成员
> Go 结构体中只声明了类型但是没有指定名字的成员，称为匿名成员。

下面的代码中，`Circle` 和 `Wheel` 各自都有一个匿名成员。我们可以说 `Point` 类型被 _嵌入_ 到了 `Circle` 结构体，同时 `Circle` 类型被嵌入到了 `Wheel` 结构体

```go
type Point struct {
	X, Y int
}

type Circle struct {
	Point  // Point 结构体嵌入到了 Circle 结构体
	Radius int
}

type Wheel struct {
	Circle // Circle 结构体又嵌入到了 Whell 结构体
	Spokes int
}
```

> [!important] 结构体 _匿名嵌入_
> 如果某个结构体类型是另一个结构体的匿名成员，则称该结构体嵌入了另一个结构体。

得益于 **_匿名嵌入_** 的特性，我们可以 **_直接访问叶子属性_ 而不需要给出完整的路径**

```go
package main

func main() {
	var w Wheel
	w.X = 8       // w.Circle.Point.X = 8
	w.Y = 8       // w.Circle.Point.Y = 8
	w.Radius = 5  // w.Circle.Radius = 5
	w.Spokes = 20 // w.Spokes = 20
}
```

**在右边的注释中给出的显式形式访问这些叶子成员的语法依然有效**，因此匿名成员并不是真的无法访问了。其中 **匿名成员** `Circle` 和 `Point` **都有自己的名字**——**就是命名的类型名字**——但是这些名字在点操作符中是可选的。我们在访问子成员的时候可以忽略任何匿名成员部分

> [!WARNING] 匿名成员也有自己的名字，默认采用 _类型名_ 作为名字
>
> 这里 **匿名成员的说法并不代表没有名字**，而是 **默认会采用 _类型名作为名字_**，
> 
> **结构体要求成员名必须唯一**，因此一个结构体中 **_同种类型的匿名成员只能有一个_**

```go
type User struct {
	int
	string
	// int // 错误
}
```

不幸的是，**结构体字面值并没有简短表示匿名成员的语法**， 因此下面的语句都不能编译通过：

```go
// 下面的语句会有如下两处编译错误
// cannot use 8 (untyped int constant) as draw.Circle value in struct literal
// too many values in struct literal of type draw.Wheel
var whell = draw.Wheel{8, 8, 5, 20}

// 下面的语句右三处编译错误
// unknown field X in struct literal of type draw.Wheel
// unknown field Y in struct literal of type draw.Wheel
// unknown field Radius in struct literal of type draw.Wheel
w := draw.Wheel{X: 8, Y: 8, Radius: 5, Spokes: 20}
```

**结构体字面值必须遵循形状类型声明时的结构**，所以我们只能用下面的两种语法，它们彼此是等价的

```go
package main

import "fmt"

func main() {

	// 列表方式
	var c = Circle{Point{8, 8}, 20}
	fmt.Println(c)

	// 键值方式
	var w = Wheel{
		Circle: Circle{
			Point: Point{X: 8, Y: 8},
			Radius: 5,
		},
		Spokes: 20,
	}
	fmt.Println(w)
}
```

到目前位置，我们看到 **匿名成员特性** 只是 **对访问嵌套成员的点运算符提供了简短的语法糖**

> [!ERROR] 注意
> 匿名成员并不要求是结构体类型；其实 **_任何命名的类型都可以作为结构体的匿名成员_**。但是为什么要嵌入一个没有任何子成员类型的匿名成员类型呢？
> + 原因在于为了获得 **匿名类型的方法集**
> 
> 简短的点运算符语法可以用于选择匿名成员嵌套的成员，也可以用于访问它们的方法
> 
> 实际上，外层的结构体不仅仅是获得了匿名成员类型的所有成员，而且也获得了该类型导出的全部的方法。这个机制可以用于将一个有简单行为的 **对象组合** 成有复杂行为的对象
> 
> **组合是 Go 语言中面向对象编程的核心**，

## 构造函数

Go 并没有从语言层面为结构体提供什么构造器，但是有时候可以通过一个函数为结构体初始化提供属性值，从而方便得到一个结构体实例。习惯上，函数命名为 `NewXXX` 的形式。

```go
package main

import "fmt"

type Animal struct {
	name string
	age  int
}

func NewAnimal(name string, age int) *Animal {
	return &Animal{
		name: name,
		age: age,
	}
}

func main() {
	a := NewAnimal("Tom", 20)
	fmt.Printf("%#v\n", a)
}
```

**习惯上返回值会采用指针类型**，避免实例的拷贝

## 深浅复制

+ `shadow copy`：影子拷贝，也叫浅拷贝。遇到引用类型数据，仅仅复制一个引用而已

+ `deep copy`：深拷贝，往往会递归复制一定深度。拷贝的是引用的对象，而不不拷贝引用

注意，深浅拷贝说的是拷贝过程中是否发生递归拷贝，也就是说 **如果某个值是一个地址，是 _只复制这个地址_ ，还是 _复制地址指向的内容_**。

下图展示浅拷贝与深拷贝之间的区别

![[Pasted image 20250611203531.png]]

> [!error] 错误说法：值拷贝是深拷贝，地址拷贝是浅拷贝，这种说法是错误的
> 因为地址拷贝只是拷贝了地址，因此本质上来讲也是值拷贝。

Go语言中，引用类型实际上拷贝的是标头值，这也是值拷贝，并没有通过标头值中对底层数据结构的指针指向的内容进行复制，这就是浅拷贝。非引用类型的复制就是值拷贝，也就是再造一个副本，这也是浅拷贝。因为你不能说对一个整数值在内存中复制出一个副本，就是深的拷贝。像整数类型这样的基本类型就是一个单独的值，没法深入拷贝，根本没法去讲深入的事儿

简单讲，大家可以用拷贝文件是否对软链接跟进来理解。直接复制软链接就是浅拷贝，钻进软链接里面复制其内容就是深拷贝

复杂数据结构，往往会有嵌套，有时嵌套很深，如果都采用深拷贝，那代价很高，所以，浅拷贝才是语言普遍采用的方案

## 内存对齐

> [!important] 内存对齐提供内存的访问效率
> + 参考 [【Golang】这个内存对齐呀！](https://www.bilibili.com/video/BV1Ja4y1i7AF/)

### 内存访问
CPU 需要从内存中读取数据，就需要通过 **_地址总线_** ^[[[计算机组成原理#^3fe3bd|地址总线]]] 将地址发送给内存，内存通过 **_数据总线_** ^[[[计算机组成原理#^537218|数据总线]]] 将数据发送给 CPU

![[Pasted image 20240710163958.png|600]]

内存可以认为是一个大矩阵，通过 _行地址_ 和 _列地址_ 选择任意一个字节，然后将这个字节发送到数据总线上

![[Pasted image 20240710164826.png|600]]

为了实现更高的访问效率，下面给出了一个典型的内存布局

![[Pasted image 20240710165158.png|600]]

一个内存条的一面称为 `Rank`，由若干 `Chip` 组成，一个 `Chip` 有 $8$ 个 `Bank` 组成，每个 `Bank` 可以通过一个地址选择一个位，所以一个 `Chip` 可以选择一个字节。每个 `Chip` 公用一个地址，就可以一次性选择出 $8$ 个字节

![[Pasted image 20240710165957.png|600]]

> [!important] 这样的地址必须是 $8$ 的倍数
> 如果非要错开，由于最后一个字节对应的位置与前七个不同，不能在一次操作中被同一个地址选择，这样的地址是不可用的
> 
> ![[Pasted image 20240710170155.png|600]]

> [!tip] 访问任意地址
>有些 CPU 支持访问任意地址，是因为 CPU 多做了许多处理。例如，如果想从地址 $1$ 处读取 $8$ 字节的数据，需要分两次读取
> 
>+ 第一次，读取 $0 \sim 7$，但是只取后七个字节
> ![[Pasted image 20240710170530.png|600]]
> 
>+ 第二次，读取 $8\sim 15$，但是之前第一个字节
> ![[Pasted image 20240710170624.png|600]]
>   
>   
> 然后拼接两次读取的结果

### 对齐要求
显然，访问任意地址必然会影响到性能。为了保证程序顺利高效的运行，编译器会把各种类型的数据 _安排到合适的地址_ 并 _占用合适的长度_，这就是 **内存对齐**。每种类型的对齐值就是其 **_对齐边界_**

> [!warning] 内存对齐的要求
> + 数据存储地址是对齐边界的整数倍
> + 数据占用的字节数也是对齐边界的整数倍
>   
>   ![[Pasted image 20240710171515.png|600]]
>   

### 对齐边界的确定

一个类型的对齐边界与平台有关。Go 语言支持下面几种平台

|    平台名称    | 指针宽度 | 寄存器宽度 |
| :--------: | :--: | :---: |
|   `I386`   | $4$  |  $4$  |
|  `amd64`   | $8$  |  $8$  |
|   `arm`    | $4$  |  $4$  |
|  `arm64`   | $8$  |  $8$  |
|   `mips`   | $4$  |  $4$  |
|  `mipsle`  | $4$  |  $4$  |
|  `mips64`  | $8$  |  $8$  |
| `mips64le` | $8$  |  $8$  |
|  `ppc64`   | $8$  |  $8$  |
| `ppc64le`  | $8$  |  $8$  |
| `riscv64`  | $8$  |  $8$  |
|  `s390x`   | $8$  |  $8$  |
|   `wasm`   | $8$  |  $8$  |

> [!important] $32$ 位和 $64$ 位平台的指针宽度和寄存器宽度
> + 在 $32$ 位平台上的指针宽度和寄存器宽度都是 $4$ 字节
> + 在 $64$ 位平台上的指针宽度和寄存器宽度都是 $8$ 字节

被 Go 称为寄存器宽度的值可以理解位平台的 **_机器字长_**，也是该平台的 _最大对齐边界_

> [!important] 数据类型的对齐边界：数据类型的大小与机器字长的最小值确定
> ![[Pasted image 20240710172400.png|600]]

### 结构体的内存对齐边界

对于结构体而言，首先要确定各成员的对齐边界，其中最大值为结构体的对齐边界

```go
type T struct {
	a int8  // 对齐边界为 1byte
	b int64 // 对齐边界为 8byte
	c int32 // 对齐边界为 4byte
	d int16 // 对齐边界为 2byte
}  // 这个结构体的对齐边界为 8byte
```

内存对齐的第一个要求：存储结构的第一个地址必须是对齐边界的倍数。然后，结构体的每个成员都应该将该地址当作起始地址$0$，然后再用相对地址，确定自己应该存放再哪儿

![[Pasted image 20240710172956.png|600]]

+ 成员 `a` 的对齐边界为 `1byte`，所以 `a` 存储在起始地址处
![[Pasted image 20240710173103.png|600]]

+ 成员 `b` 的对齐边界为 `8byte`，所以 `b` 应该存放在相对地址为 `8` 的位置
![[Pasted image 20240710173201.png|600]]

+ 成员 `c` 的对齐边界为 `4byte`，所以 `c` 可以紧挨着 `b` 存储
![[Pasted image 20240710173309.png|600]]

+ 成员 `d` 的对齐边界为 `2byte`，所以 `d` 可以紧挨着 `c` 存储
![[Pasted image 20240710173409.png|600]]

内存对齐的第二要求：结构体整体占用字节数是对齐边界的整数倍，不够还需要向后扩张。所以，该结构体需要扩张到相对地址 `23` 处，共占用 `24` 字节

![[Pasted image 20240710173725.png|600]]


```go
package main

import (
	"fmt"
	"unsafe"
)

type T struct {
	a int8  // 对齐边界为 1byte
	b int64 // 对齐边界为 8byte
	c int32 // 对齐边界为 4byte
	d int16 // 对齐边界为 2byte
} // 这个结构体的对齐边界为 8byte

func main() {

	siz := unsafe.Sizeof(T{})
	fmt.Printf("size: %d\n", siz)

	t := T{}

	fmt.Printf("t: %p\t", &t)   // t: 0xc00009e030
	fmt.Printf("a: %p\t", &t.a) // a: 0xc00009e030
	fmt.Printf("b: %p\t", &t.b) // b: 0xc00009e038
	fmt.Printf("c: %p\t", &t.c) // c: 0xc00009e040
	fmt.Printf("d: %p\t", &t.d) // d: 0xc00009e044
}
```


