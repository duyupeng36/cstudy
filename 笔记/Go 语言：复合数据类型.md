# 复合数据类型

## 数组

**数组是一个由固定长度的特定类型元素组成的序列**，一个数组可以由零个或多个元素组成。数组的 **每个元素可以通过 _索引下标_ 来访问**，索引下标的范围是从 $0$ 开始到数组长度减 $1$ 的位置

> [!tip] 
> 
> 内置的 `len()` 函数将返回数组中元素的个数
> 

因为数组的长度是固定的，因此在 Go 语言中很少直接使用数组。和数组对应的类型是 `Slice`（切片），它是可以增长和收缩动态序列，`Slice` 功能也更灵活，但是要理解 `Slice` 工作原理的话需要先理解数组

### 数组的声明与初始化

在 Go 语言中， `[size]Type` 称为长度为 `size` ，元素为  `Type` 类型的数组。由于 Go 语言的零值初始化机制，当我们声明一个 `Type` 类型的数组时，Go 会自动将数组的元素全部初始化为 `Type` 类型的零值

```go
var a [3]int  // 声明长度为 3 元素为 int 类型的数组

fmt.Println(a[0])         // 首元素：0
fmt.Println(a[len(a)-1])  // 尾元素：0
```

在声明数组时，我们可以提供 **初始值**，初始值使用 `[size]Type{value, ...}` 形式提供。例如

```go
var a [3]int = [3]int{1,2,3}

fmt.Println(a[0])         // 首元素：1
fmt.Println(a[len(a)-1])  // 尾元素：3
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

### 遍历

Go 语言有两种遍历数组方式：**使用索引** 和 **range 关键字**

我们首先来看使用索引的方式


```go
var a [10]int

for i := 0; i < len(a); i++ {
	fmt.Printf("a[%d]= %d\n", i, a[i])
}
```

### 比较

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

## 切片

**切片代表变长的序列**，序列中 **每个元素都有相同的类型**。切片类型一般写作 `[]Type`，其中`Type` 代表 切片中元素的类型；切片的语法和数组很像，只是没有固定长度而已。

**数组和切片之间有着紧密的联系**。一个切片是一个轻量级的数据结构，提供了访问数组子序列（或者全部）元素的功能，而且 **切片的底层确实引用一个数组对象**。切片由三个部分构成：**指针**、**长度** 和 **容量**

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

切片的类型为 `[]Type`，为了声明一个切片，其语法如下

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

![[Pasted image 20250605230620.png]]

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

	s1 == s2 // invalid operation: slice can only be compared to nil
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

![[Pasted image 20250605234500.png]]

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





