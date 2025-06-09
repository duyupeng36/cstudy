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

## 结构体

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

### 结构体类型

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

### 结构体变量

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

### 访问成员

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

### 结构体指针

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

### 匿名结构体

**匿名结构体**：标识符直接使用 `struct` 部分结构体本身来作为类型，而不是使用 `type` 定义的有名字的结构体的标识符。

可以使用 `var` 、 `const` 、 `:=` 来定义匿名结构体。

`type` 定义结构体类型的标识符，可以反复定义其结构体实例，但是匿名结构体是一次性的

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

### 结构体嵌入和匿名成员

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
> 
> Go 结构体中 **只声明了类型但是没有指定名字的成员**，称为匿名成员。
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

> [!important] 结构体 _匿名嵌入_
> 如果某个结构体类型是另一个结构体的匿名成员，则称该结构体嵌入了另一个结构体。

得益于 **_匿名嵌入_** 的特性，我们可以 **_直接访问叶子属性_** 而不需要给出完整的路径

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

### 内存对齐

> [!important] 内存对齐提供内存的访问效率
> + 参考 [【Golang】这个内存对齐呀！](https://www.bilibili.com/video/BV1Ja4y1i7AF/)

#### 内存访问

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

#### 对齐要求

显然，访问任意地址必然会影响到性能。为了保证程序顺利高效的运行，编译器会把各种类型的数据 _安排到合适的地址_ 并 _占用合适的长度_，这就是 **内存对齐**。每种类型的对齐值就是其 **_对齐边界_**

> [!warning] 内存对齐的要求
> + 数据存储地址是对齐边界的整数倍
> + 数据占用的字节数也是对齐边界的整数倍
>   
>  ![[Pasted image 20240710171515.png]]
>   

#### 对齐边界的确定

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

