# 结构体

> [!tip] 
> 
> **结构体是一种聚合的数据类型**， **多个任意类型的值聚合成的实体**。每个值称为结构体的 **成员**
> 

用结构体的经典案例处理公司的员工信息，每个员工信息包含一个唯一的 _员工编号_、_员工的名字_、_家庭住址_、_出生日期_、_工作岗位_、_薪资_、_上级领导_ 等等。所有的这些信息都需要绑定到一个实体中，可以作为一个整体单元被复制，作为函数的参数或返回值，或者是被存储到数组中，等等。

下面两个语句声明了一个叫 `Employee` 的命名的结构体类型，并且声明了一个 `Employee` 类型的变量`dilbert`：

```go
type Employee struct {
    ID        int
    Name      string
    Address   string
    DoB       time.Time
    Position  string
    Salary    int
    ManagerID int
}

var dilbert Employee
```

关键字 `type` 用于自定义类型，紧跟在 `type` 后的就是自定类型的名字，随后是类型元信息。
+ 语句 `type Employee struct{}` 定义了一个结构体类型 `Employee`，该类型包含了多个任意类型的成员
+ 语句 `var dilbert Employee` 是使用结构体类型 `Employee` 声明了一个变量。

> [!important] 
> 
> 也就是 `type` 关键字定义了一个类型，它是一个用于创建变量的模板。换句话说，就是仅仅是定义了结构体类型，并没有定义结构体变量(即，**没有分配内存空间**)
> 
> 使用 `var` 关键字定义变量 `dilbert` 时才为结构体 `Employee` 分配了内存空间
> 

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

> [!important] 
> 
> 结构体变量可以被认为是一个对象(实例)
> 

## 访问成员

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

## 结构体指针

**再次强调结构体类型是值类型**，下面的代码演示了这个特性

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

> [!tip] 
> 
> 显然 `person5 := person4` 会创建一个全新的对象
> + `person5` 的修改不会影响到 `person4`

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

> [!important] 
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

> [!tip] 
>  结构体字面值定义如下
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

## 匿名结构体

**匿名结构体**：标识符直接使用 `struct` 部分结构体本身来作为类型，而不是使用 `type` 定义的有名字的结构体的标识符。

可以使用 `var`   `const`  `:=` 来定义匿名结构体。`type` 定义结构体类型的标识符，可以反复定义其结构体实例，但是匿名结构体是一次性的

![[Pasted image 20240711073602.png]]

在定义一些 **临时数据结构** 等场景下还可以使用 **匿名结构体**

```go
var user struct {  
    Name string  
    Age  int  
}
```

例如，只需要在函数内创建一个结构体变量

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

> [!important] 
> 
> 匿名结构体，只是为了快速方便地得到一个结构体实例，而不是使用结构体创建 `N` 个实例
> 

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
