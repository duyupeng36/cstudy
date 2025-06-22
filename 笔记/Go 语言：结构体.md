# 结构体

> [!tip] 
> 
> 结构体是一种聚合的数据类型， **多个任意类型的值聚合成的实体**。每个值称为结构体的 **成员**
> 

## 结构体变量

下面我们介绍一个使用结构体的典型案例。假设一个网站需要管理其用户，用户需要使用多维度的信息进行描述，例如，网站需要记录 **用户名**、**密码**、**邮箱地址**、**手机号** 等信息。显然，这些信息不能单独存储，而是需要存储在一个变量中，这个变量的类型就是结构体。

在 Go 语言中，声明结构体变量使用 `var` 关键字，结构体类型写为 `struct {...}`，花括号中是一个一个的数据成员，例如

```go
var user struct {
	username string
	password string
	email    string
	phone    string
}
```

在上述示例中，`struct {...}` 指明了变量 `user` 的类型。这种格式与普通变量声明一致

结构体成员在内存中按照声明的顺序存储。假设变量 `user` 存储的地址为 $2000$，在 Go 语言中，字符串占用 $16$ 字节，那么 `user` 的内存布局如下

![[Pasted image 20250621014935.png]]

**每个结构代表一种新的作用域**。任何声明在此作用域内的名字都不会和程序中的其他名字冲突。在同一个程序中可以出现下面的程序片段

```go
var user struct {
	username string
	password string
	email    string
	phone    string
}

var employee struct {
	username string
	password string
	email    string
	phone    string
}
```

> [!tip] 
> 
> 结构都为它的成员设置了独立的作用域
> 

### 初始化

结构体变量的初始化语法为 `struct {成员列表}{初始值列表}`，其中初始值列表按照成员的声明顺序给出

```go
var user struct {
	username string
	password string
	email    string
	phone    string
} = struct{
	username string
	password string
	email    string
	phone    string
}{
	"张三", 
	"xxx", 
	"xxx@gmail.com", 
	"9994326",
}
```

同普通变量声明一样，我们可以省略类型描述

```go
var user = struct{
	username string
	password string
	email    string
	phone    string
}{
	"张三", 
	"xxx", 
	"xxx@gmail.com", 
	"9994326",
}
```

此外，我们可以指定成员初始化。这样就可以以任意顺序初始化结构体变量的成员

```go
var user = struct {
	username string
	password string
	email    string
	phone    string
}{
	phone:    "9994326",
	password: "xxx",
	email:    "xxx@gmail.com",
	username: "张三",
}
```

### 访问成员

访问结构成员是 **通过名字访问的**。如果需要访问结构变量 `user` 中的成员，使用 **成员运算符**。在 Go 语言中，成员运算符与 C/C++ 中的成员运算符一样都是使用的 **点(`.`)**。例如，`user.username` 可以访问结构体变量 `user` 中的 `username` 成员

```go
package main

import "fmt"

var user = struct {
	username string
	password string
	email    string
	phone    string
}{
	phone:    "9994326",
	password: "xxx",
	email:    "xxx@gmail.com",
	username: "张三",
}

func main() {
	fmt.Printf("%#v\n", user.username)
	fmt.Printf("%#v\n", user.password)
}
```

## 结构体类型

向之前那样直接使用 `struct{...}` 只是在方临时需要使用结构体的地方创建结构体对象，但是不适合在需要多个结构体变量的地方使用，因为每次都会重复写 `struct{...}`

并且，如果结构体成员声明顺序可能由于粗心导致顺序出现差异，进而导致类型不兼容。如下示例演示变量成员顺序不同导致的赋值失败的案例

```go hl:6,18
package main

func main() {
	var user1 = struct {
		username string
		age      int
		password string
		email    string
		phone    string
	}{
		phone:    "9994326",
		password: "xxx",
		email:    "xxx@gmail.com",
		username: "张三",
	}

	var user2 = struct {
		age      int
		username string
		password string
		email    string
		phone    string
	}{}

	user2 = user1
}
```

> [!attention] 
> 
> 注意成员 `age` 的位置，这就是导致赋值失败的根本原因
> 

Go 语言提供了一个 `type` 关键字，它用于自定义类型。我们可以通过 `type` 为 `struct{....}` 命名一个全新的类型名，之后就可以使用这个类型名声明结构体变量了


```go hl:5-11
package main

import "fmt"

type User struct {
	username string
	age      int
	password string
	email    string
	phone    string
}

func main() {
	var user1 = User{
		username: "张三",
		age:      20,
		password: "xxx",
		email:    "xxxa@gmail.com",
		phone:    "9964325",
	}

	var user2 = User{}
	user2 = user1
	fmt.Printf("%#v\n", user2)
	// main.User{username:"张三", age:20, password:"xxx", email:"xxxa@gmail.com", phone:"9964325"}
}
```

> [!hint] 
> 
> 显然，在语法层面上，只是使用一个确定的名字 `User` 替代了 `struct{...}`，其余并没有任何区别
> 

### 赋值

在 Go 语言中，相同类型的结构体变量之间可以相互 **赋值**，并且执行 **逐成员拷贝**。

```go
type Point struct {X, Y int}

p1 := Point{1, 2}
p2 := Point{}

p2 = p1
```

> [!attention] 
> 
> 将结构变量 `p1` 中的所有成员逐个复制到 `p2` 中的对应成员上
> 

### 比较

在 Go 语言中，只要结构体的所有成员是可比较，那么结构体也是可以比较的，那样的话两个结构体将可以使用 `==` 或 `!=` 运算符进行比较。比较运算符 `==` 将比较两个结构体的每个成员，因此下面两个比较的表达式是等价的：

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

> [!tip] 
> 
> 如果结构体的字段都是可比较的，那么该结构体类型就是可比较的。可比较的结构体类型和其他可比较的类型一样，可以用于 Map 的 `key`
> 

### 匿名成员

考虑一个编写二维的绘图程序，提供了一个各种图形的库，例如矩形、椭圆形、星形和轮形等几何形状。这里是其中两个的定义

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

随着库中几何形状数量的增多，我们一定会注意到它们之间的 **相似和重复** 之处，所以我们可能为了便于维护而 **将相同的属性独立出来**

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

这样改动之后 **结构体类型变的清晰了** ，但是这种修改同时也 **导致了访问每个成员变得繁琐**

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

Go语言有一个特性让我们 **只声明一个成员对应的数据类型** 而 **不指名成员的名字**；这类成员就叫 **匿名成员**

> [!hint]
> 
> Go 结构体中只声明了类型但是没有指定名字的成员，称为匿名成员
> 

下面的代码中，`Circle` 和 `Wheel` 各自都有一个匿名成员。我们可以说 `Point` 类型被 **嵌入** 到了 `Circle` 结构体，同时 `Circle` 类型被嵌入到了 `Wheel` 结构体

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

> [!important] 结构体 **匿名嵌入**
> 
> 如果某个结构体类型是另一个结构体的匿名成员，则称该结构体嵌入了另一个结构体
> 

得益于 **匿名嵌入** 的特性，我们可以 **直接访问叶子属性** 而不需要给出完整的路径

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

> [!attention] 
> 
> 请注意： 在右边的注释中给出的 **显式形式访问这些叶子成员** 的语法依然有效，因此匿名成员并不是真的无法访问了
> 
> 其中 **匿名成员** `Circle` 和 `Point` **都有自己的名字**——**就是命名的类型名字**——但是这些名字在点操作符中是可选的。我们在访问子成员的时候可以忽略任何匿名成员部分
> 

匿名成员也有自己的名字，默认采用 _类型名_ 作为名字。也就是说， **匿名成员的说法并不代表没有名字**，而是 **默认会采用 _类型名作为名字_**

> [!attention] 
> 
> 请注意：**结构体要求成员名必须唯一**，因此一个结构体中 **同种类型的匿名成员只能有一个**
> 

```go
type User struct {
	int
	string
	// int // 错误
}
```

> [!attention] 
> 
> 不幸的是，**结构体字面值并没有简短表示匿名成员的语法**
> 

 因此下面的语句都不能编译通过
 
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

> [!attention] 
> 
> 匿名成员并不要求是结构体类型；其实 **_任何命名的类型都可以作为结构体的匿名成员_**。但是为什么要嵌入一个没有任何子成员类型的匿名成员类型呢？
> + 原因在于为了获得 **匿名类型的方法集**
> 
> 简短的点运算符语法可以用于选择匿名成员嵌套的成员，也可以用于访问它们的方法
> 
> 实际上，外层的结构体不仅仅是获得了匿名成员类型的所有成员，而且也获得了该类型导出的全部的方法。这个机制可以用于将一个有简单行为的 **对象组合** 成有复杂行为的对象
> 
> **组合是 Go 语言中面向对象编程的核心**，后续我们会详细介绍
> 

## 结构体指针

再次强调结构体类型是 **值类型**，下面的代码演示了这个特性

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

	fmt.Printf("%#v\n", person4) 
	// main.Person{name:"dyy", city:"上海", age:12}
	fmt.Printf("%#v\n", person5) 
	// main.Person{name:"dyy", city:"上海", age:12}
	
	person5.age = 25
	fmt.Printf("%#v\n", person4) 
	// main.Person{name:"dyy", city:"上海", age:12}
	fmt.Printf("%#v\n", person5) 
	// main.Person{name:"dyy", city:"上海", age:25}
}
```

显然 `person5 := person4` 会创建一个全新的对象，`person5` 的修改不会影响到 `person4`

既然，结构是值类型，那么当结构体作为参数传递给函数时，在函数内部操作的只是实参的一个副本。**当结构体比较大时，拷贝结构体可能会导致程序性能降低**。而且，**不利于结构体的修改**。

> [!important] 
> 
> 当结构体作为函数参数时，通常使用结构体指针
> 

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

> [!attention] 
> 
> 在 Go 语言中支持 **对 _结构体指针_ 直接使用 `.` 来访问结构体的成员**。并没有像 C 一样提供类似于 `->` 运算符
> 
> `pu.name = "Almi"` 其实底层是 `(*pu).name = "Almi"`，这是 Go 帮我们实现的自动转换
> 

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


## 内存对齐

> [!important] 内存对齐提供内存的访问效率
> + 参考 [【Golang】这个内存对齐呀！](https://www.bilibili.com/video/BV1Ja4y1i7AF/)

### 内存访问

CPU 需要从内存中读取数据，就需要通过 **_地址总线_**  将地址发送给内存，内存通过 **_数据总线_**  将数据发送给 CPU

![[Pasted image 20240710163958.png]]

内存可以认为是一个大矩阵，通过 _行地址_ 和 _列地址_ 选择任意一个字节，然后将这个字节发送到数据总线上

![[Pasted image 20240710164826.png]]

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
>  ![[Pasted image 20240710171515.png]]
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
> 
> ![[Pasted image 20240710172400.png]]
> 

#### 结构体的内存对齐边界

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
