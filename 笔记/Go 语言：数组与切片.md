# 数组与切片

在 Go 语言中，数组和切片都是 [[线性表]] 的顺序存储实现。区别于 C/C++，Go 中的数组是 **值类型**，长度是数组类型的一部分。因此，数组的长度是固定不变的。Go 语言中很少使用数组，而是使用 **切片**，切片本质上是对数组的封装。因此，在学习切片之前有必要了解一些数组相关的知识

## 数组

Go 中的数组和 C/C++ 中的数组的特性非常相似，唯一不同的就是 **Go 数组是值类型**

> [!tip] 
> 
> 这意味着 Go 数组作为函数参数传递时，会拷贝一份完整的数组。而不像 C/C++ 那样只会传递数组首元素的指针
> 
> 更意味着，Go 数组可以赋值给另一个相同类型的数组变量
> 

声明 Go 数组的语法如下

```go
var array [10]int  // 声明长度为 10 的 int 类型的数组
```

> [!tip] 
> 
> 注意：数组类型 `[n]Type`  中的 `n` 必须是一个常量表达式
> 

如果提供初始值，也可以向普通变量那样省略类型

```go
var array = [10]int{1, 2, 3}  
// 前 3 个元素初始化为 1 2 3。剩余元素初始化为 0
```

> [!important]
> 
> 在给定字面量时，数组的长度可以由编译器自动推断。
> 

```go
var c = [...]int{10, 2, 30, 9: 20}  // 长度推断为 10
```

> [!tip] 
> 
> 初始值 `[...]type{index:value}` 表示初始化索引为 `index` 的元素值为 `value`
> 
> 类似于 C99 标准提供的指示器
> 

**数组的每个元素可以通过索引下标来访问**，索引下标的范围是从 `0` 开始到数组长度减 `1` 的位置。内置的 `len` 函数将返回数组中元素的个数

```go
var a [3]int             // 3 个整数的数组
fmt.Println(a[0])        // 数组的第一个元素
fmt.Println(a[len(a)-1]) // 数组的最后一个元素
a[2] = 10
a[3] = 10 // invalid argument: index 3 out of bounds。编译期错误
```

> [!warning] 
> 
> 注意：Go 数组 **不支持负索引**。**索引不能越界**，越界的访问将导致编译失败
> 

如果需要遍历数组中的元素，可以向 C/C++ 那样通过索引就进行遍历

```go
var a [3]int             // 3 个整数的数组

// 使用索引遍历数组
for i := 0; i < len(a); i++ {
	fmt.Println(i, a[i])
}
```

也可以使用 `range` 关键字进行遍历

```go
for i, v := range a {
	// i 是当前遍历的索引
	// v 是当前遍历的元素
	fmt.Println(i, v)
}
```

与 C/C++ 一样，Go 语言也支持多维数组。例如，数组 `[N][M]Type`，它本质上还是一个一维数组，长度为 `N` 元素类型为 `[M]Type` 的一维数组。也就是说，Go 语言中的多维数组也是按照行主序存储的

```go
// 二维数组
var d [3][4]int = [3][4]int{
	{1, 2, 3, 4}, 
	{2, 3, 4, 5}, 
	{4, 5, 6, 7}
	}

fmt.Printf("%#v\n", d) 
// [3][4]int{
//    [4]int{1, 2, 3, 4}, 
//    [4]int{2, 3, 4, 5},
//    [4]int{4, 5, 6, 7}
// }
```

![[Pasted image 20250315213905.png]]

下面我们就探索一下 Go 数组是 **值类型** 的特性：**赋值和传参都会创建一个全新的副本**

```go
package main

import "fmt"

func main() {

	var a [3]int
	var b = a // 将会复制一份
	
	fmt.Printf("&a=%p &b=%p\n", &a, &b)
	b[1] = 100
	fmt.Printf("a=%v b=%v\n", a, b)
	
	fmt.Println("----------------------------")
	c := showAddr(a)

	fmt.Printf("从函数返回: &c=%p\n", &c)
	fmt.Printf("从函数返回: c=%v\n", c)
}

func showAddr(arr [3]int) [3]int {
	fmt.Printf("函数内部: &arr = %p\n", &arr)
	arr[0] = 200
	fmt.Printf("函数内部: arr = %v\n", arr)
	return arr
}
```

上述代码的执行结果为

```
&a=0xc0000180c0 &b=0xc0000180d8  # a 和 b 的地址不同
a=[0 0 0] b=[0 100 0]            # 修改 b 不会影响 a
----------------------------
函数内部: &arr = 0xc000018138     # 实参拷贝了一份
函数内部: arr = [200 0 0]
从函数返回: &c=0xc000018120        # 返回也拷贝了一份
从函数返回: c=[200 0 0] a=[0 0 0]
```

> [!important] Go 语言在 **赋值** **传参**和 **从函数返回** 时都对数组进行了 **值拷贝**，都生成了一份副本

## 切片

Go 数组是值类型，将数组传递给函数或从函数中返回可能会带来性能损失(数组元素过多，导致拷贝耗时增加)。同时，由于数组的长度是固定不变的，在使用时要预估数组的长度。预估太大，会导致内存浪费；预估太小，可能导致程序异常

为了解决上述问题，Go 语言提供了另一个类型 **切片**。切片本质上是在数组之上的一层封装。切片类型由 $3$ 部分组成：**指针** **长度** 和 **容量**

![[Pasted image 20250315222218.png]]

> [!tip] 
> 
> `ptr` 指向 **切片管理的首元素** 所在底层数组中的位置。
> 
> `len` 是切片管理的元素的个数
> 
> `cap` 是从切片首元素到底层数组的末尾的元素个数
> 

切片本质是对一个底层数组的连续 **片段** 的引用。此片段可以是整个底层数组，也可以是由起始和终止索引标识的一些项的子集。在 `${GOROOT}/src/runtime/slice.go` 中切片的定义如下

```go
type slice struct {
    array unsafe.Pointer
    len int
    cap int
}
```

> [!tip] 
> 
> 也就是说，切片本质上只是一个结构体。该结构体有 $3$ 个成员
> + `array` 指针指向切片的首元素
> + `len` 切片管理元素的个数
> + `cap` 切片首元素到底层数组末尾包含的元素个数
> 
> Go 语言提供的结构体与 C/C++ 类似，是一个用户自定义的 **值类型** 
> 

内置函数 `len()` 和 `cap()` 分别用于获取切片的长度和容量

### 定义切片

切片中的每个元素都是相同的类型，切片类型变量的声明语法如下

```go
var slice []int  // 声明一个元素为 int 类型的切片
```

如果访问上述状态下来的切片，Go 语言将会抛出 `panic`。因此，此时的切片还处于未分配底层数组的状态，即是 `nil` 切片

```go
package main  
  
import "fmt"  
  
/* 创建切片的方法 */  
func main() {  
    var slice []int       // 仅仅只是声明的一个切片，它还没有管理任何底层数组  
    fmt.Println(slice[0]) // panic: runtime error: index out of range [0] with length 0  
    fmt.Println(slice == nil)  // true
}
```

> [!attention] 
> 
> 请注意，这仅仅只是创建了一个代表切片的结构体变量，其中的指针成员还是 `nil`。换句话说，就是此时这个切片变量尚未管理任何底层数组
> 
> Go 语言认为这种状态下的切片是不可以用的状态，其值为 `nil`。**切片的默认值就是 `nil`**
> 

当然，可以向普通变量一样提供初始值。切片的字面值使用 `[]Type{...}` 方式创建。因此，我们可以像下面的方式一样声明切片

```go
var slice []int = []int{1,2,3,4}
```

这样写显得有点啰嗦，和声明普通变量一样，可以让编译器自动推到类型

```go
var slice = []int{1,2,3,4}
```

### make() 函数：定义切片

创建切片最常用的方式就是使用 `make()` 函数，这是 Go 语言内置的函数，可以用于创建 切片 map 和管道。用于创建切片时需要提供 $3$ 参数：切片类型，长度和容量

```go
var slice = make([]int, 10, 20)
```

如果未提供容量，`make()` 将切片的容量设置与长度相等

```go
package main

import "fmt"

func main() {
	a := make([]int, 2, 10) // 长度为 2 容量为 10
	fmt.Println(a)      //[0 0]
	fmt.Println(len(a)) //2
	fmt.Println(cap(a)) //10

	a = make([]int, 10) // 长度和容量均为 10
	fmt.Println(a)
	fmt.Println(len(a))
	fmt.Println(cap(a))
}
```

### 切片之间的比较

切片唯一支持的比较操作就是与 `nil` 进行比较。注意：我们不能使用 `==` 比较运算符来判断两个切片中的元素是否完全相同。如果需要比较两个切片中的元素是否完全相同，必须自定义函数完成

> [!tip] 
> 
> Go 标准包 `bytes` 提供了一个高度优化的函数 `bytes.Equal()` 用于比较 `[]byte` 切片是否完全相等
> 

例如，下面的 `equal()` 函数，用于比较 `[]string` 切片中的元素是否完全相等

```go
func equal(x, y []string) bool {
    if len(x) != len(y) {
        return false
    }
    for i := range x {
        if x[i] != y[i] {
            return false
        }
    }
    return true
}
```

思考，容量为 $0$ 的切片是否是 `nil` 切片？下面的代码用于验证这个问题

```go
package main

import "fmt"

func main() {
	var s1 = make([]int, 0) // 长度为 0 的切片
	var s2 []int            // nil 切片

	fmt.Println(s1 == nil) // false
	fmt.Println(s2 == nil) // true
}
```

> [!tip] 
> 
> `nil` 切片的容量一定为 $0$。但是，容量为 $0$ 的切片不一定是 `nil` 切片
> 
> 注意：要判断一个切片是否为 `nil` 切片，只能与 `nil` 进行比较，不要试图使用容量来判断
> 

### 向切片中添加元素

内置函数 `append()` 用于向切片中追加元素，同时返回修改后的切片句柄。也就是说，通常像下面这样使用 `append()` 函数

```go
Slice = append(Slice, element1, element2, ...)
```

调用 `append()` 时，需要指定元素添加在那个切片中。如果切片的底层输入可以容纳新添加的元素，`append` 只会导致切片的长度变化

由于切片本质就是一个结构体，参数传递时执行值拷贝，被调函数内部的修改无法影响到调用函数中变量的值，因此 `append()` 需要返回这个新切片

```go
package main

import "fmt"

func main() {
	s1 := make([]int, 3, 5)
	
	fmt.Printf("s1: %p, %p, len=%d, cap=%d, %v\n", &s1, &s1[0], len(s1), cap(s1), s1)
	
	s2 := append(s1, 1, 2) 
	// append 会返回一个新的切片
	fmt.Printf("s2: %p, %p, len=%d, cap=%d, %v\n", &s2, &s2[0], len(s2), cap(s2), s2)
}
```

上述代码运行的结果为

```
s1: 0xc000010018, 0xc00001c0c0, len=3, cap=5, [0 0 0]
s2: 0xc000116000, 0xc00001c0c0, len=5, cap=5, [0 0 0 1 2]
```

此时 `s1` 和 `s2` 的内存模型如下

![[Pasted image 20250315234343.png]]

目前没有超过容量，底层 **共用同一个数组**，但是，对底层数组 **使用的片段不一样**

下面我们继续向 `s1` 中添加一个元素

```go
// 继续向 s1 中添加元素 -1
s3 := append(s1, -1)
fmt.Println("---------------------------------------------------------------------")
fmt.Printf("s1: %p, %p, len=%d, cap=%d, %v\n", &s1, &s1[0], len(s1), cap(s1), s1)
fmt.Printf("s2: %p, %p, len=%d, cap=%d, %v\n", &s2, &s2[0], len(s2), cap(s2), s2)
fmt.Printf("s3: %p, %p, len=%d, cap=%d, %v\n", &s3, &s3[0], len(s3), cap(s3), s3)
```

这段代码的输出结果为

```
---------------------------------------------------------------------
s1: 0xc000010018, 0xc00001c0c0, len=3, cap=5, [0 0 0]
s2: 0xc000010048, 0xc00001c0c0, len=5, cap=5, [0 0 0 -1 2]
s3: 0xc000010090, 0xc00001c0c0, len=4, cap=5, [0 0 0 -1]
```

注意到，`s1` `s2` `s3` 是三个不同的切片，但是引用了同一个底层数组。下图画出来它们的内存模型

![[Pasted image 20250316000944.png]]

如果切片的底层数组无法容纳新添加的元素，就会触发切片的 **扩容策略**，现在，向 `s3` 中添加元素 `3, 4, 5`

```go
// 向 s3 中添加元素 3, 4, 5
s4 := append(s3, 3, 4, 5)
fmt.Println("---------------------------------------------------------------------")
fmt.Printf("s1: %p, %p, len=%d, cap=%d, %v\n", &s1, &s1[0], len(s1), cap(s1), s1)
fmt.Printf("s2: %p, %p, len=%d, cap=%d, %v\n", &s2, &s2[0], len(s2), cap(s2), s2)
fmt.Printf("s3: %p, %p, len=%d, cap=%d, %v\n", &s3, &s3[0], len(s3), cap(s3), s3)
fmt.Printf("s4: %p, %p, len=%d, cap=%d, %v\n", &s4, &s4[0], len(s4), cap(s4), s4)
```

上述代码的输出结果为

```
---------------------------------------------------------------------
s1: 0xc00012e000, 0xc000130000, len=3, cap=5, [0 0 0]
s2: 0xc00012e030, 0xc000130000, len=5, cap=5, [0 0 0 -1 2]
s3: 0xc00012e078, 0xc000130000, len=4, cap=5, [0 0 0 -1]
s4: 0xc00012e0d8, 0xc000144000, len=7, cap=10, [0 0 0 -1 3 4 5]
```

注意到，切片 `s4` 引用的底层数组发生了变化。下图说明了这样的变化

![[Pasted image 20250316001309.png]]

在 `s4` 的基础上，继续添加元素 `6, 7, 8, 9` 

```go
s5 := append(s4, 6, 7, 8, 9)
fmt.Println("---------------------------------------------------------------------")
fmt.Printf("s1: %p, %p, len=%d, cap=%d, %v\n", &s1, &s1[0], len(s1), cap(s1), s1)
fmt.Printf("s2: %p, %p, len=%d, cap=%d, %v\n", &s2, &s2[0], len(s2), cap(s2), s2)
fmt.Printf("s3: %p, %p, len=%d, cap=%d, %v\n", &s3, &s3[0], len(s3), cap(s3), s3)
fmt.Printf("s4: %p, %p, len=%d, cap=%d, %v\n", &s4, &s4[0], len(s4), cap(s4), s4)
fmt.Printf("s5: %p, %p, len=%d, cap=%d, %v\n", &s5, &s5[0], len(s5), cap(s5), s5)
```

上述代码段的输出结果为

```
---------------------------------------------------------------------
s1: 0xc000010018, 0xc00001c0c0, len=3, cap=5, [0 0 0]
s2: 0xc000010048, 0xc00001c0c0, len=5, cap=5, [0 0 0 -1 2]
s3: 0xc000010090, 0xc00001c0c0, len=4, cap=5, [0 0 0 -1]
s4: 0xc0000100f0, 0xc0000ae000, len=7, cap=10, [0 0 0 -1 3 4 5]
s5: 0xc000010168, 0xc0000260a0, len=11, cap=20, [0 0 0 -1 3 4 5 6 7 8 9]
```

注意到，`s5` 与 `s4` 的底层数组又不相同的，容量也进行了翻倍。下图展示了此时的内存模型

![[Pasted image 20250316002154.png]]

下面对 `append()` 函数进行总结

> [!summary] 
> 
> `append()` 一定会返回一个新的切片
> 
> `append()` 可以一次添加多个元素到切片中，一旦底层数组无法容纳添加的元素，就会触发切片的扩容策略
> 
> 使用 `append()` 函数向切片添加元素时，通常让其返回的切片直接覆盖原来切片
> 

#### 扩容策略

分析上面的输出，我们模拟下一 `append` 函数的执行过程
+ 需要返回一个新切片
+ 根据添加元素个数判断是否需要扩容
+ 目前来看，扩容只是简单的容量翻倍

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
> 
> 内置函数 `copy(dst, src)` 可以方便的把 `src` 切片中的元素复制到 `dst` 切片中 
> 

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

(新版本1.18+) 阈值变成了 $256$，当扩容后的 `cap<256` 时，扩容翻倍，容量变成之前的 `2` 倍;当 `cap>=256` 时，`newcap +=(newcap + 3*threshold)/4` 计算后就是 `newcap = newcap + newcap/4 + 192`，即 `1.25` 倍后再加 `192`

（老版本）实际上，当扩容后的 `cap<1024` 时，扩容翻倍，容量变成之前的 `2` 倍；当 `cap>=1024` 时，变成之前的 `1.25` 倍

> [!warning] 
> 
> 警告：扩容是耗时操作，因为需要将元素拷贝到新的内存空间
> 
> **扩容是 _创建新的内部数组_**，把原内存数据 **_拷贝到新内存空间_**，然后在新内存空间上执行元素追加操作
> 
> **切片频繁扩容成本非常高，所以尽量早估算出使用的大小，一次性给够**，建议使用 `make`。常用 `make([]int, 0, 100)` 。

思考一下：如果 `s1 := make([]int, 3, 100)` ，然后对 `s1` 进行 `append` 元素，会怎么样？
+ 添加元素个数少于 `97` 时，不会触发扩容策略
+ 添加元素个数大于 `97` 时，触发扩容策略

### 切片是引用类型

```go
package main

import "fmt"

func main() {

	s1 := []int{10, 20, 30} // 创建一个切片类型的变量
	// 切片地址(&s1)  底层数组地址(&s1[0])  
	// 切片管理元素的个数即长度(len(s1))  底层数组的长度即容量(cap(s1))
	fmt.Printf("s1 %p, %p, %d, %d, %v\n", &s1, &s1[0], len(s1), cap(s1), s1)
	
	s2 := s1  // 使用 s1 创新创建了一个切片，共用同一个底层数组
	fmt.Printf("s2 %p, %p, %d, %d, %v\n", &s2, &s2[0], len(s2), cap(s2), s2)
	fmt.Println("~~~~~~~~~~~~~~~~~~~~~~~~~~~")
	
	s3 := showAddr(s1)
	fmt.Printf("s1 %p, %p, %d, %d, %v\n", &s1, &s1[0], len(s1), cap(s1), s1)
	fmt.Printf("s2 %p, %p, %d, %d, %v\n", &s2, &s2[0], len(s2), cap(s2), s2)
	fmt.Printf("s3 %p, %p, %d, %d, %v\n", &s3, &s3[0], len(s3), cap(s3), s3)
}

func showAddr(s []int) []int {
	fmt.Printf("调用 append 之前: %p, %p, %d, %d, %v\n", &s, &s[0], len(s), cap(s), s)
	// 修改一个元素
	if len(s) > 0 {
		s[0] = 123
	}
	fmt.Printf("调用 append 之前，修改s[0] 之后: %p, %p, %d, %d, %v\n", &s, &s[0], len(s), cap(s), s)

	s = append(s, 100, 200) // 覆盖s，请问s1会怎么样
	fmt.Printf("调用 append 之后: %p, %p, %d, %d, %v\n", &s, &s[0], len(s), cap(s), s)
	return s
}
```

`s1 := []int{10, 20, 30}` 将创建一个切片，并管理一个长度为 $3$ 底层数组。执行 `s2 := s1` 将创建另一个切片，管理与 `s1` 相同的底层数组

![[Pasted image 20250316003230.png]]

调用 `showAddr(s1)` 时，会将 `s1` 中的内容拷贝到 `showAddr` 的形参 `s` 中。当执行 `s[0] = 123` 之后，导致所有的切片都能观察到底层数组的改变

![[Pasted image 20250316003302.png]]

当代码执行了 `s = append(s, 100, 200)` 之后。内存模型变成了

![[Pasted image 20250316003337.png]]

当 `showAddr(s1)` 返回后，内存模型变成了

![[Pasted image 20250316003446.png]]

根据上述分析，这段代码的执行结果为

```
s1 0xc000010018, 0xc0000180c0, 3, 3, [10 20 30]
s2 0xc000100000, 0xc0000180c0, 3, 3, [10 20 30]
~~~~~~~~~~~~~~~~~~~~~~~~~~~
调用 append 之前: 0xc000100048, 0xc0000180c0, 3, 3, [10 20 30]
调用 append 之前，修改s[0] 之后: 0xc000100048, 0xc0000180c0, 3, 3, [123 20 30]
调用 append 之后: 0xc000100048, 0xc00010e000, 5, 6, [123 20 30 100 200]
s1 0xc000010018, 0xc0000180c0, 3, 3, [123 20 30]
s2 0xc000100000, 0xc0000180c0, 3, 3, [123 20 30]
s3 0xc000100030, 0xc00010e000, 5, 6, [123 20 30 100 200]
```

可以看出，切片其实这里还是 **值拷贝**，不过拷贝的是 **切片的标头值（Header）**。**标头值内指针也被复制**，刚复制完大家指向同一个底层数组罢了。但是仅仅知道这些不够，因为一旦操作切片时扩容了，或另一个切片增加元素，那么就 **不能简单归结为“切片是引用类型，拷贝了地址”** 这样简单的话来解释了。要具体问题，具体分析。


> [!summary]
> 
> **切片仅仅只是一个结构体**，只是携带了 **底层数组的指针**、**当前管理的元素个数(长度)** 和 **底层数组的长度(容量)**。在 Go 中这样携带底层数据结构的指针的类型称为 **引用类型**
> + 不要混淆其他语言中的引用类型
> 

Go 语言中函数传递参数都是值传递的。整型、数组这样的类型的值是完全复制，`slice`、`map`、`channel`、`interface`、`function` 这样的引用类型也是值拷贝，不过复制的是标头值

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

![[Pasted image 20250316003750.png]]

`slice` 的切片操作 `s[i:j]`，其中 `0 <= i <= j <= cap(s)`，用于创建一个新的 `slice`，引用 `s` 的从第 `i` 个元素开始到第 `j-1` 个元素的子序列。新的 `slice` 将只有 `j-i` 个元素

因此，`months[1:13]` 切片操作将引用全部有效的月份，和 `months[1:]` 操作等价；`months[:]` 切片操作则是引用整个数组。让我们分别定义表示第二季度和北方夏天月份的 `slice`，它们有重叠部分

```go
Q2 := months[4:7]
summer := months[6:9]
fmt.Println(Q2, len(Q2), cap(Q2))             // [April May June] 3 9
fmt.Println(summer, len(summer), cap(summer)) // [June July August] 3 7
```

![[Pasted image 20250316003847.png]]

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

因为 `slice` 只包含指向第一个 `slice` 元素的指针，因此向函数传递 `slice` 将允许在函数内部修改底层数组的元素。换句话说，复制一个 `slice` 只是对底层的数组创建了一个新的 `slice` 别名。下面的 `reverse` 函数在原内存空间将 `[]int` 类型的 `slice` 反转，而且它可以用于任意长度的 `slice`

```go
// reverse reverses a slice of ints in place.
func reverse(s []int) {
    for i, j := 0, len(s)-1; i < j; i, j = i+1, j-1 {
        s[i], s[j] = s[j], s[i]
    }
}
```

> [!summary] 
> 
> **多个切片直接可以  _共用_ 底层数组，引用的底层数组 _部分区间可能重叠_**
> 
