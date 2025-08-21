# 内建数据结构

在介绍内建数据结构之前，我们再次回顾 **类型** 这一概念。在 [[计算机组成：信息存储]] 和 [[Go 语言：基本概念#基本数据类型]] 中介绍过，在计算机内部存储的就有 $0$ 和 $1$ 组成的 **位序列**，也称为 **位模式**。这些位模式表示的是什么样的数据完全取决应用程序使用哪种类型来解释这些位模式

> [!tip] 
> 
> 换句话说，类型规定了如何解释计算机存储在内存中的位模式。在没有类型这个概念时，我们无法知道位模式表示的数据是什么
> 

位模式 `0x63`，它究竟是什么？字符串？字符？还是整数？这完全取决于应用程序使用哪个类型来解释这个位模式。例如，如果使用 `int` 类型来解释，那么 `0x63` 表示了整数 $99$；如果使用了 `run` 类型来解释，那么 `0x63` 表示字符 `'c'` 等待 

## 线性数据结构

**线性表**，顾名思义，就是 **具有像线一样的性质的序列**。数学符号的记法如下：线性表记为 $(a_1,a_2,\cdots, a_n)$，则表中 $a_{i-1}$ 领先于 $a_i$，称 $a_{i-1}$ 是 $a_i$ 的 **直接前驱元素**，当然，$a_i$ 领先于 $a_{i+1}$ ，称 $a_{i+1}$ 是 $_{i}$ 的 **直接后继元素**

![[Pasted image 20240930214647.png]]

线性表的元素个数 $n(n \ge 0)$ 称为 **线性表的长度**，当 $n=0$ 是称为 **空表**。在 **非空表中的每个元素都有一个确定的位置**

> [!example] 
> 
> 例如，$1$ 是第一个数据元素，$n$ 是最后一个数据元素，$i$ 是第 $i$ 个数据元素，称 $i$ 为数据元素 $i$ 在线性表中的 **位序**
> 

> [!important] 逻辑结构 与 物理结构
> 
> 线性表是一种 **逻辑结构**。所谓的数据结构都是逻辑结构
> 
> 线性表中的元素需要存储在物理内存上中，在内存上存储的结构称为 **物理结构**。有两种常用的物理结构
> + **顺序结构**：元素存储在一片连续的内存空间中
> + **链式结构**：元素以结点的形式在内存中分散存储，每个结点可以找到它之后的结点
> 

Go 语言的内建类型 **数组** 就是顺序表的顺序存储实现

> [!tip] 
> 
> 几乎所有的程序设计语言都提供了数组这一个内建类型
> 

## 数组

数组是 **一片连续的内存区域** 并且这片内存区域通过 **基本数据类型** 将其 **划分 $n$ 个大小相等的小的内存区域**。也就是说，数组只能 **存储同一种 _数据类型_ 的对象**。

当 **_同种类型_** 的一组数据存储在一片连续的内存区域时，只要知道这片连续内存区域的 **_基地址_**，就能 **_随机访问_** 其中的任何一个元素

> [!tip] 随机访问
> 
> 访问一个元素的时间复杂度为 $O(1)$，常数时间复杂度
> 

在大多数语言中，包括 Go 语言，**数组的索引都是从 $0$ 开始的**。就其原因在于索引从 $0$ 开始使得 **元素地址的计算效率更高**。例如，要计算第 $i$ 个元素的地址($\text{Addr}_{i}$) 

$$
\text{Addr}_{i} = \text{Addr}_{\text{base}} + i \times \text{sizeof}(\text{type})
$$

其中 $\text{Addr}_{\text{base}}$  表示数组的 **基地址**，$\text{sizeof}(\text{type})$ 表示 `type`  类型的 **对象占用的内存空间**(字节)

假设索引从 $1$ 开始，则第 $i$ 个元素的地址

$$
\text{Addr}_{i} = \text{Addr}_{\text{base}} + (i-1) \times \text{sizeof}(\text{type})
$$

这样在每次计算第 $i$ 个元素的地址时，都会多执行一次减法运算


> [!summary] 
> 
> **数组** 是是用于 **存储多个相同类型数据** 的 **数据结构**  。数组中的 _值称为元素_，可以 _根据其在数组中的位置 **直接访问**_
>

下图展示了数组的内存结构

![Pasted image 20250615015722|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755786809255-96db4a77a09a4b7587a0e12030d368b5.png)

### 一维数组

在 Go 语言中，类型为 `[size]Type`，称为长度为 `size`  元素为  `Type` 类型的一维数组。例如，如下声明语句

```go
var a [10]int  // 声明长度为 3 元素为 int 类型的一维数组
```

定义了一个长度为 $10$ 的数组 `a`

![[Pasted image 20240925002338.png]]

换句话说它 **定义了一个由 10 个 `int` 对象组成的集合**，它们的名字分别为 `a[0], a[1], a[2], ..., a[9]`

#### 初始化

在声明数组时，我们可以提供 **初始值**，初始值使用 `[size]Type{value, ...}` 形式提供。例如

```go
var a [10]int = [10]int{1,2,3，4,5,6,7,8,9,10}
```

与声明普通变量一样，在提供初始值的情形下，可以省略类型

```go
var a = [3]int{1,2,3} // 编译器自动推断类型
```

Go 语言还支持 **指定索引初始化**，类似于 C99 标准中的指示器 ^[[[C 语言：数组#指示器]]]。上述初始化称为 **顺序初始化**，本质上就是按顺序指定索引。

```go
var a = [3]int{2: 3, 1: 2, 0: 1}
fmt.Printf("%#v\n", a) // [3]int{1, 2, 3}
```

> [!tip] 
> 
> 指定索引初始化与按顺序初始化可以混用
> 

编译器可以自动推断数组元素的个数

```go
var a = [...]int{1,2,3}   // [3]int
var b = [...]int{4:2, 4}  // [6]int
```

**数组的长度是数组类型的一个组成部分**，因此 `[3]int` 和 `[4]int` 是两种不同的数组类型。**数组的长度必须是常量表达式**，因为数组的长度需要在编译阶段确定

```go
q := [3]int{1, 2, 3}
q = [4]int{1, 2, 3, 4} 
// compile error: cannot assign [4]int to [3]int
```

#### 遍历

Go 语言有两种遍历数组方式：**使用索引** 和 **range 关键字**

我们首先来看使用索引的方式，这种遍历方式在 C 语言中常用

```go
var a [10]int

for i := 0; i < len(a); i++ {
	fmt.Printf("a[%d]= %d\n", i, a[i])
}
```

Go 语言的 `range` 关键字用于遍历内建数据结构，这里我们首先介绍如何使用 `range` 关键字遍历数组。

```go hl:3
var a [10]int

for index, value := range a {
	fmt.Printf("a[%d]= %d\n", i, a[i])
}
```

使用 `range` 遍历一个数组时，会按顺序返回两个值：**索引** 和 **元素**

#### 比较

回忆 [[C 语言：数组]] 中介绍的内容，在  C 语言中，数组是不可以比较的。因为编译器将数组名视为其首元素的地址。

然而，在 Go 语言中却是不同的，Go 语言中的数组和基本类型的变量相同，即 **Go 将数组视为一个值**。

```go
var a = [3]int{1, 2, 3}
var b = a

a[0] = 3

fmt.Printf("%#v\n", a)  // [3]int{3, 2, 3}
fmt.Printf("%#v\n", b)  // [3]int{1, 2, 3}
```

如果一个数组的元素类型是可以相互比较的，那么数组类型也是可以相互比较的；这时候我们可以直接通过`==` 比较运算符来比较两个数组，只有当两个数组的所有元素都是相等的时候数组才是相等的。不相等比较运算符 `!=` 遵循同样的规则

```go hl:6,7
a := [2]int{1, 2}
b := [...]int{1, 2}
c := [2]int{1, 3}
fmt.Println(a == b, a == c, b == c) // "true false false"

d := [3]int{1, 2}
fmt.Println(a == d) // compile error: cannot compare [2]int == [3]int
```

> [!attention] 
> 
> 再次强调，Go 语言中的运算符的两个操作数必须是相同类型的
> 

### 多维数组

数组可以有任意维数。例如，下面的声明产生一个二维数组(数学上称为矩阵)

```go
var m [5][10]int
```

数组 `m` 有 $5$ 行 $10$ 列。如下所示，数组的行和列下标都从 $0$ 开始索引

![[Pasted image 20240925111411.png]]

为了访问 $i$ 行 $j$ 列的元素，表达式需要写成 `m[i][j]`。表达式 `m[i]` 指明了数组 `m` 的第 $i$ 行，而 `m[i][j]` 则选择了此行中的第 $j$ 个元素

虽然我们以二维形式展示，但是在计算机内存中的存储方式是按照一维数组的方式存储的。Go 语言和 C 语言一样，按照行主序存储多为数组

![[Pasted image 20240925111050.png]]

> [!tip] 
> 
> 换句话说，二维数组就是一维数组的一维数组
> 

## 切片

在 Go 语言中，数组有一个缺陷就是长度是固定的。在使用数组之前需要预估数组的长度，预估往往是不准确的，从而导致程序运行时出现异常。

为了解决上述问题，Go 语言提供了另一个内建数据类型 **切片**，它是 **对 Go 数组的一层封装**；即，切片引用了一个底层数组，当底层数组消耗完毕时，Go 运行时会重新申请一片新的内存

一个切片是一个轻量级的数据结构，提供了访问数组子序列（或者全部）元素的功能，而且 **切片的底层确实引用一个数组对象**。切片由三个部分构成：**指针**、**长度** 和 **容量**。如下图展示了 Go 语言切片的内存模型

![Pasted image 20250615161530|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755786809256-b6e4c274a61f4481b2c855d8cb27799f.png)

![Pasted image 20250615163951|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755786809256-af66fa9e21e945cdbffe6ec385903405.png)

> [!tip] 
> 
> + **指针** 指向第一个切片元素对应的底层数组元素的地址，要注意的是切片的第一个元素并不一定就是数组的第一个元素
> 
> + **长度** 对应切片中元素的数目；长度不能超过容量
> 
> + **容量** 一般是从切片的开始位置到底层数据的结尾位置
> 
> 内置的 `len()` 和 `cap()` 函数分别返回切片的长度和容量。
> 

### 声明切片

如下代码声明了一个切片变量 `s` 

```go
var s []int  // 声明 int 类型的切片
```

> [!tip]
>  
> 此处，Go 语言的零值初始化机制发送作用
> + 指针被初始化为 `nill`
> + 长度和容量均被初始化为 $0$
> 

```go
package main

import (
	"fmt"
)

func main() {
	var s []int            // nil 切片：指针为 nil 的切片对象
	fmt.Printf("%#v\n", s) // []int(nil)
	
	fmt.Println(s[0])      
	// runtime error: index out of range [0] with length 0
	
	fmt.Println(s == nil)  // true
}

```

> [!attention] 
> 
> 使用 `var` 声明的切片对象并没有引用任何底层数组，无法通过索引进行操作
> 

因此，在声明切片时，我们应该显式初始化。初始值使用 `[]Type{value, ...}` 的方式提供

```go
var s = []int{1,2,3}

fmt.Printf("len(s) = %d\t cap(s) = %d", s, s) 
// len(s) = [1 2 3] cap(s) = [1 2 3]
```

注意：使用 `[]Type{}` 初始化的切片对象长度和容量都为 $0$；但是，指针部分并不是 `nil`

```go
var s []int    // len(s) == 0, s == nil
s = nil        // len(s) == 0, s == nil
s = []int(nil) // len(s) == 0, s == nil
s = []int{}    // len(s) == 0, s != nil
```

> [!tip] 
> 
> 如果你需要测试一个切片是否是空的，使用 `len(s) == 0` 来判断，而不应该用 `s == nil` 来判断。
> 

请注意：`nil` 切片除了不能索引操作外，其余操作是完全没有问题的

#### make() 函数

通常，我们期望切片对象已经持有一个底层数组。提前分配需要的空间，可以避免在程序执行过程中浪费时间去分配内存

为切片分配底层数组使用内置的 `make()` 函数，它需要提供 $3$ 个参数：`Type`、长度和容量。容量可以省略，此时容量等价于长度

```go
make([]T, len)
make([]T, len, cap) // same as make([]T, cap)[:len]
```

> [!attention] 
> 
> 请注意：使用 `make()` 函数创建的切片底层数组中的每个元素被初始化为类型 `T` 的零值
> 

### append() 函数

内置的 `append()` 函数用于向切片追加元素，当切片无法容纳新追加的元素时，就会分配新的底层数组，将原来的元素拷贝到新底层数组后才追加新元素

> [!tip] 
> 
> 这意味着可以直接向 `nil` 切片中 `append` 元素
> 

```go
package main

import "fmt"

func main() {
	var s []rune
	for _, v := range "hello, 世界" {
		s = append(s, v)
	}
	fmt.Printf("%#v\n", s) 
	// []int32{104, 101, 108, 108, 111, 44, 32, 19990, 30028}
	
	fmt.Printf("%#q\n", s) // ['h' 'e' 'l' 'l' 'o' ',' ' ' '世' '界']
}
```

在循环中使用 `append()` 函数构建一个由九个 `rune` 字符构成的切，当然对应这个特殊的问题我们可以通过 Go 语言内置的 `[]rune("Hello, 世界")` 强制类型转换操作完成。

上述实例中，切片 `s` 没有底层数组，当使用 `append()` 向切片 `s` 添加元素时，`append()` 触发切片扩容。下面我们介绍切片扩容策略

#### 扩容策略

我们使用如下函数 `appendInt()` 模拟 `append()` 函数的执行过程
+ 需要返回一个新切片
+ 根据添加元素个数判断是否需要扩容
+ 这里我们简单认为：扩容只是简单的容量翻倍

```go
func appendInt(x []int, y ...int) []int {

	// 新切片：返回的切片
	var z []int

	zlen := len(x) + len(y) // 新切片的长度

	// 判断是否还有容量可以存放元素
	if zlen <= cap(x) {
		// 我们还有成长的空间。扩展切片
		z = x[:zlen]
	} else {
		// 需要扩容
		// 重新分配空间
		zcap := zlen
		if zcap < 2*cap(x) {
			zcap = 2 * cap(x)
		}

		z = make([]int, zlen, zcap)

		// 将原来的元素复制到新的空间
		copy(z, x)
	}

	// 添加元素
	for i, v := range y {
		z[len(x)+i] = v
	}

	return z
}
```


> [!tip] 
> 内置函数 `copy(dst, src)` 可以方便的把 `src` 切片中的元素复制到 `dst` 切片中 

测试 `appendInt` 

```go
var a = make([]int, 3, 5)
fmt.Printf("a: %p, %p, len=%d, cap=%d, %v\n", &a, &a[0], len(a), cap(a), a)

b := appendInt(a, 1, 2)
fmt.Println("---------------------------------------------------------------------")
fmt.Printf("a: %p, %p, len=%d, cap=%d, %v\n", &a, &a[0], len(a), cap(a), a)
fmt.Printf("b: %p, %p, len=%d, cap=%d, %v\n", &b, &b[0], len(b), cap(b), b)

c := appendInt(b, 2, 3)
fmt.Println("---------------------------------------------------------------------")
fmt.Printf("a: %p, %p, len=%d, cap=%d, %v\n", &a, &a[0], len(a), cap(a), a)
fmt.Printf("b: %p, %p, len=%d, cap=%d, %v\n", &b, &b[0], len(b), cap(b), b)
fmt.Printf("c: %p, %p, len=%d, cap=%d, %v\n", &c, &c[0], len(c), cap(c), c)
```

输出结果为

```
a: 0xc000010018, 0xc00001c0c0, len=3, cap=5, [0 0 0]
---------------------------------------------------------------------
a: 0xc000010018, 0xc00001c0c0, len=3, cap=5, [0 0 0]
b: 0xc000100000, 0xc00001c0c0, len=5, cap=5, [0 0 0 1 2]
---------------------------------------------------------------------
a: 0xc000010018, 0xc00001c0c0, len=3, cap=5, [0 0 0]
b: 0xc000100000, 0xc00001c0c0, len=5, cap=5, [0 0 0 1 2]
c: 0xc000100048, 0xc000106000, len=7, cap=10, [0 0 0 1 2 2 3]
```

---

在 `$GOROOT/src/runtime/slice.go` 源码中，与扩容相关代码如下：

```go
// nextslicecap computes the next appropriate slice length.
func nextslicecap(newLen, oldCap int) int {
	newcap := oldCap
	doublecap := newcap + newcap  // 首先将容量翻倍
	// 如果申请的空间大于原来空间的两倍，则直接返回申请的容量
	if newLen > doublecap {
		return newLen
	}
	
	// 设定一个阈值
	const threshold = 256
	// 申请的空间小于原来空间的两倍 并且 原始容量小于阈值，将容量翻倍
	if oldCap < threshold {
		return doublecap
	}
	// 申请的空间小于原来空间的两倍 并且 原始容量大于或等于阈值
	for {
		// 从小切片增长 2 倍过渡到大切片增长 1.25 倍。这个公式可以在两者之间实现平滑过渡
		newcap += (newcap + 3*threshold) >> 2  // newcap = newcap + newcap / 4 + 3 * 256 / 4 => newcap = 1.25 * newcap + 192

		// 我们需要检查 `newcap >= newLen` 和 `newcap` 是否溢出。
		// newLen 保证大于零，因此当 newcap 溢出时，`uint(newcap) > uint(newLen)`。 
		// 这样就可以通过相同的比较对两者进行检查。
		if uint(newcap) >= uint(newLen) {
			break
		}
	}

	// 当 newcap 计算溢出时，将 newcap 设置为请求的上限。
	if newcap <= 0 {
		return newLen
	}
	return newcap
}
```

(新版本1.18+) 阈值变成了 $256$，当扩容后的 `cap<256` 时，扩容翻倍，容量变成之前的 `2` 倍；当 `cap>=256` 时，`newcap +=(newcap + 3*threshold)/4` 计算后就是 `newcap = newcap + newcap/4 + 192`，即 `1.25` 倍后再加 `192`

（老版本）实际上，当扩容后的 `cap<1024` 时，扩容翻倍，容量变成之前的 `2` 倍；当 `cap>=1024` 时，变成之前的 `1.25` 倍

> [!warning] 
> 
> 扩容是耗时操作：因为需要将元素拷贝到新的内存空间
> 
> + **扩容是 _创建新的内部数组_**，把原内存数据 **_拷贝到新内存空间_**，然后在新内存空间上执行元素追加操作
> 
> + **切片频繁扩容成本非常高，所以尽量早估算出使用的大小，一次性给够**，建议使用 `make`。常用 `make([]int, 0, 100)` 

思考一下：如果 `s1 := make([]int, 3, 100)` ，然后对 `s1` 进行 `append` 元素，会怎么样？添加元素个数少于 `97` 时，不会触发扩容策略；添加元素个数大于 `97` 时，触发扩容策略

### 内存模型

切片本质上只是一个 **结构体**，在 `${GOROOT}/src/runtime/slice.go` 中切片的定义如下

```go
type slice struct {
    array unsafe.Pointer
    len int
    cap int
}
```

通过 `array` 指向一个底层数组中的某些片段，使用 `len` 和 `cap` 管理这个底层数组片段中的元素。如下图所示

![Pasted image 20250605230620|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755786809256-16c453a1ca574510806c477ed40b583b.png)

> [!tip] 
> 
> 长度(`len`)：切片管理的元素个数
> 
> 容量(`cap`)：切片底层数组能容纳的元素个数
> 
> 注意：Go 的结构体与 C 中的结构体类似。也就是说，**结构体是一个可比较的对象**
> 

这就是说明了 `append()` 函数为什么要返回一个新的切片：因为，`append()` 在追加元素到切片中时，如果切片的容量无法容纳新增的元素，则会重新申请内存空间，从而改变了指针(`array`) 的指向。因此，`append()` 需要将这个新的切片返回

### 切片的比较

请注意：Go 语言的切片唯一能进行的就是和 `nil` 比较，两个切片之间是不能比较的。因为，切片本质上是一个结构体，结构体之间的比较只是比较成员，无法比较指针(`array`) 引用的底层数组中的内容。

```go
package main

func main() {
	var s1 = []int{1, 2, 3}
	var s2 = []int{1, 2, 3}

	s1 == s2 
	// invalid operation: slice can only be compared to nil
}
```

> [!tip] 
> 
> 切片只能与 `nil` 进行比较
> 

### 切片表达式

**多个 `slice` 之间可以共享底层的数据，并且引用的数组部分区间可能重叠**。下面的代码声明了一个数组表示一年中每个月份名字的字符串数组

```go
months := [...]string{
	1:  "January",
	2:  "February",
	3:  "March",
	4:  "April",
	5:  "May",
	6:  "June",
	7:  "July",
	8:  "August",
	9:  "September",
	10: "October",
	11: "November",
	12: "December",
}
```

这样，一月份是 `months[1]`，十二月份是 `months[12]`。数组的第一个元素从索引0开始，但是月份一般是从 `1` 开始的，因此我们声明数组时直接跳过第 `0` 个元素，第 `0` 个元素会被自动初始化为空字符串

`slice` 的切片操作 `s[i:j]`，其中 `0 <= i <= j <= cap(s)`，用于创建一个新的 `slice`，引用 `s` 的从第 `i` 个元素开始到第 `j-1` 个元素的子序列。新的 `slice` 将只有 `j-i` 个元素
+ 如果`i` 位置的索引被省略的话将使用 `0` 代替
+ 如果 `j` 位置的索引被省略的话将使用 `len(s)` 代替

因此，`months[1:13]` 切片操作将引用全部有效的月份，和 `months[1:]` 操作等价；`months[:]` 切片操作则是引用整个数组。让我们分别定义表示第二季度和北方夏天月份的 `slice`，它们有重叠部分

```go
Q2 := months[4:7]
summer := months[6:9]
fmt.Println(Q2, len(Q2), cap(Q2))             // [April May June] 3 9
fmt.Println(summer, len(summer), cap(summer)) // [June July August] 3 7
```

![Pasted image 20250605234500|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755786809257-47cd988213ad4523af63df42e2a4797c.png)

两个 `slice` 都包含了六月份，下面的代码是一个包含相同月份的测试（性能较低）

```go
for _, s := range summer {
    for _, q := range Q2 {
        if s == q {
            fmt.Printf("%s appears in both\n", s)
        }
    }
}
```

如果切片操作超出 `cap(s)` 的上限将导致一个 `panic` 异常，但是超出 `len(s)` 则是意味着扩展了 `slice`，因为新 `slice` 的长度会变大

```go
fmt.Println(summer[:20]) // panic: out of range
endlessSummer := summer[:5] // extend a slice (within capacity)
fmt.Println(endlessSummer)  // "[June July August September October]"
```

因为 `slice` 只包含指向第一个 `slice` 元素的指针，因此向函数传递 `slice` 将允许在函数内部修改底层数组的元素。换句话说，复制一个 `slice` 只是对底层的数组创建了一个新的 `slice` 别名。下面的 `reverse()` 函数在原内存空间将 `[]int` 类型的 `slice` 反转，而且它可以用于任意长度的 `slice`

```go
// reverse reverses a slice of ints in place.
func reverse(s []int) {
    for i, j := 0, len(s)-1; i < j; i, j = i+1, j-1 {
        s[i], s[j] = s[j], s[i]
    }
}
```

## Map

[[数据结构：散列表]] 是一种巧妙并且实用的数据结构。它是一个 **无序的 (key, value) 对的集合**，其中所有的 `key` 都是不同的，然后通过给定的 `key` 可以在 **常数时间复杂度** 内检索、更新或删除对应的 `value`

在Go语言中，一个 Map 就是一个哈希表的引用，Map 类型可以写为 `map[K]V` ，其中 `K` 和 `V` 分别对应 `key` 和 `value` 的类型

> [!important] 
> 
> `map` 中所有的 `key` 都有相同的类型，所有的 `value` 也有着相同的类型，但是 `key` 和 `value` 之间可以是不同的数据类型
> 
> 其中 `K` 必须是支持 `==` 比较运算符的数据类型，所以 `map` 可以通过测试 `key` 是否相等来判断是否已经存在
> 
> + 虽然浮点数类型也是支持相等运算符比较的，但是将浮点数用做 `key` 类型则是一个坏的想法，因为最坏的情况是可能出现的 `NaN` 和任何浮点数都不相等
> 
> 对于 `V` 则没有任何的限制
> 

### 创建 Map

内置的 `make()` 函数可以创建一个 Map。用于创建 Map 时，`make()` 只需要指定两个参数，分别是类型( `map[K]V`) 和容量。容量可以提供，这样 Go 会创建一个比较小的哈希表

```go
ages := make(map[string]int) // mapping from strings to ints
```

我们也可以用 `map` 字面值的语法创建 Map，同时还可以指定一些最初的 `(key,value)` 对：

```go
ages := map[string]int{
    "alice":   31,
    "charlie": 34,
}
```

上述代码相当于

```go
ages := make(map[string]int)
ages["alice"] = 31
ages["charlie"] = 34
```

### Map 的操作
#### 访问 Value

Map 中的通过 `key` 对应的下标语法访问 `value`：

```go
ages["alice"] = 32         // 增加或修改 (key,value) 键值对
fmt.Println(ages["alice"]) // 访问 key 对应的 value："32"
```

通过 `key` 作为索引下标来访问 Map 将产生一个 `value`。如果 `key` 在 Map 中是存在的，那么将得到与 `key` 对应的 `value` ；如果 `key` 不存在，那么将得到 `value` 对应类型的零值

这个规则很实用，但是有时候可能需要知道对应的元素是否真的是在 Map 之中。例如，如果元素类型是一个数字，你可以需要区分一个已经存在的 $0$，和不存在而返回零值的 $0$，可以像下面这样测试：

```go
age, ok := ages["bob"]
if !ok { 
	/* "bob" is not a key in this map; age == 0. */ 
}
```

你会经常看到将这两个结合起来使用，像这样

```go
if age, ok := ages["bob"], !ok {
	/* "bob" is not a key in this map; age == 0. */ 
}
```

> [!hint] 
> 
> 在这种场景下，Map 的下标语法将产生两个值；第二个是一个布尔值，用于报告元素是否真的存在。布尔变量一般命名为 `ok`，特别适合马上用于 `if` 条件判断部分。
> 

#### 删除 key

想要删除 Map 中的 `key`，需要使用内置函数 `delete()`

```go
delete(ages, "alice") // 删除元素 ages["alice"]
```

> [!important] 
> 
> 所有这些操作是安全的，即使这些元素不在 Map 中也没有关系；如果一个查找失败将返回 `value` 类型对应的零值

例如，即使 Map 中不存在 `"bob"` 下面的代码也可以正常工作，因为 `ages["bob"]` 失败时将返回 $0$

 ```go
 ages["bob"] = ages["bob"] + 1 // happy birthday!
```

而且 `x += y` 和 `x++` 等简短赋值语法也可以用在 Map 上，所以上面的代码可以改写成

```go
ages["bob"] += 1
ages["bob"]++ 
```

> [!warning] 
> 
> 但是 Map 中的元素并不是一个变量，因此我们不能对 Map 的元素进行取址操作
> 
> 禁止对 Map 元素取址的原因是 Map 可能随着元素数量的增长而重新分配更大的内存空间，从而可能导致之前的地址无效
> 

```go
_ = &ages["bob"] // compile error: cannot take address of map element
```

#### 遍历

要想遍历 Map 中全部的 `(key, value)` 对的话，可以使用 `range` 风格的 `for` 循环实现，和之前的 Slice 或数组遍历语法类似。下面的迭代语句将在每次迭代时设置 `name` 和 `age` 变量，它们对应下一个键/值对：

```go
for key, value := range ages {
	fmt.Printf("%s\t%d\n", key, value)
}
```

**Map 的迭代顺序是不确定的**，并且不同的哈希函数实现可能导致不同的遍历顺序。在实践中，遍历的顺序是随机的，每一次遍历的顺序都不相同。这是故意的，每次都使用随机的遍
历顺序可以强制要求程序不会依赖具体的哈希函数实现

如果要按顺序遍历 `(key, value)` 对，我们必须显式地对 `key` 进行排序，可以使用 `sort` 包的 `Strings` 函数对字符串 Slice 进行排序。下面是常见的处理方式：

```go
import "sort"

var names []string

for name := range ages {
	names = append(names, name)
}

sort.Strings(names)

for _, name := range names {
	fmt.Printf("%s \t%d\n", name, ages[name])
}
```

### 零值

Map 的零值是 `nil`，也就是没有引用任何哈希表。对于 `nil` Map 大部分操作，包括查找、删除、`len` 和 `range` 循环都可以安全工作在 `nil` Map 上，它们的行为和一个空的 Map 类似

>[!warning] 
>
>但是向一个 `nil` Map 存入元素将导致一个 `panic` 异常。因此，在向 Map 存数据前必须先创建 Map
>

```GO
var ages map[string]int

ages["carol"] = 21 // panic: assignment to entry in nil map
```

### 比较

和切片一样，Map 唯一能进行 `==` 或 `!=` 比较操作的就是和 `nil` 比较，两个 Map 之间是不能进行比较的。要判断两个 Map 是否包含相同的 `key` 和 `value`，我们必须通过一个循环实现：

```go
func equal(x, y map[string]int) bool {
    if len(x) != len(y) {
        return false
    }
    for k, xv := range x {
        if yv, ok := y[k]; !ok || yv != xv {
            return false
        }
    }
    return true
}
```
