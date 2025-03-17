# map

Go 语言中的 `map` 类型是 [[散列表]] 的实现。它 **将一种类型的值(_键_)映射到另一种类型的值(_元素或值_)**

> [!tip] 
> 
> **键** 可以是 **定义相等运算符的任何类型**，例如 _整数_ _浮点数_ 和 _复数_ _字符串_ _指针_ _接口_（只要动态类型支持相等 _结构_ 和 _数组_ 
> 
> _切片_ `map` 和 _管道_  不能用作 `map` 键，因为它们未定义相等性比较运算符
> 

> [!seealso]
> 
>  [Map长啥样儿？](https://www.bilibili.com/video/BV1Sp4y1U7dJ)
> 

## map 类型的底层结构

与切片一样，`map` 也是一个结构体，引用了底层的一个大数组

```go
// A header for a Go map.
type hmap struct {
	count     int    // 记录当前存储的键值对个数
	flags     uint8  // 
	B         uint8  // 容量 2^B 个 
	noverflow uint16 // 记录使用溢出桶的数量
	hash0     uint32  

	buckets    unsafe.Pointer  // 指向存储键值对的桶 
	oldbuckets unsafe.Pointer  // 扩容时，指向原始存储键值对的桶
	nevacuate  uintptr         // 记录下一次迁移的旧桶编号

	extra *mapextra            // 记录溢出桶相关的信息
}
```

现在，还存在一个疑问：桶是怎样的结构呢？

```go
// A bucket for a Go map.
type bmap struct {
	// `tophash` 通常包含此 `bucket` 中每个 `key` 的哈希值的最高字节。
	// 如果 `tophash[0] < minTopHash`，
	// 则 `tophash[0]` 为 `bucket` 撤离状态。
	// 其中，minTopHash 为 5
	tophash [abi.OldMapBucketCount]uint8 // 是 ldMapBucketCount 的值是 8
	// tophash 后面还有如下内容
	// bucketCnt 个键，然后是 bucketCnt 个元素
	// 随后是溢出指针
}
```

由于 `tophash` 是一个长度为 $8$ 的 `uint8` 数组，因此，有一个桶可以存放 $8$ 个键值对。下图展示了 `bmap` 的结构

![[Pasted image 20250316200349.png]]


> [!tip] 
> 
> 将 **键** 存储在一起，然后 **值** 存储在一起，这可以让内存排列更加紧凑
> 
> 溢出桶的引入是为了减少扩容次数，当一个桶存满了之后，还有溢出桶时就会存储在溢出桶中
> 
> Go 语言认为，如果需要分配的桶的数目大于 $2^4$ 个，就会额外分配 $2^{B-4}$ 个桶并作用做溢出桶
> 

`hmap` 中的 `mapextra` 字段记录溢出桶使用的情况，其结构如下

```go
// mapextra 记录溢出桶的信息
type mapextra struct {
	overflow    *[]*bmap  // 记录已经被使用的溢出桶的地址，这是一个切片
	oldoverflow *[]*bmap  // 扩容阶段指向那些已经使用的溢出桶
	nextOverflow *bmap    // 指向下一个空闲溢出桶
}
```

常规桶和溢出桶在内存上是连续存储的。下图展示了整个 `hmap` 结构的完整内存结构

![[Pasted image 20250316221626.png]]

> [!tip] 
> 
> 也就是说，在 Go 语言中，一个桶可以存储 $8$ 个键值对。选择桶时使用的是 `key` 的哈希值
> 

下面我们来看一个例子

```go
a := map[string]string{}
a["k1"] = "v1"
```

变量 `a` 本质上就是一个 `hmap` 的结构体。下图展示了此时变量 `a` 的内存状态

![[Pasted image 20250316224345.png]]

## map 扩容规则

假设我们已经将上述例子中的哈希表中桶存满了。此时，如果继续存储新的键值对时，此时哈希表是创建溢出桶，还是发生扩容呢？

### 翻倍扩容

Go 语言中，填充因子 $\frac{\text{count}}{2^{B}} > 6.5$ 时，`map` 就会翻倍扩容，即：分配新桶的数目是旧桶的 $2$ 倍。下图展示上述例子扩容后的结果

![[Pasted image 20250316225627.png]]

请注意：每个旧桶的键值对都会分流到新两个新桶中。思考：键值对是如何分流的呢？例如，假设旧桶有 $4=2^2$ 个，那么新桶就有 $8=2^3$ 个。

> [!tip] 
> 
> 如果一个哈希值选择了 $0$ 号旧桶，那么该哈希值的最后 $2$ 位一定是 $0$；该哈希值分流到新桶中的编号就取决于倒数第 $3$ 位
> 

 ```
----------------|
  xxxxxx00      | 在旧桶中选择编号为 0 的哈希值
& 00000011      | 旧桶数目-1
————————————————|
  00000000      |
----------------|

----------------|
  xxxxx000      | 在旧桶中选择编号为 0 的哈希值，倒数第 3 为 0
& 00000111      | 新桶数目-1
————————————————|
  00000000      | 选择的新桶编号为 0
----------------|

----------------|
  xxxxx100      | 在旧桶中选择编号为 0 的哈希值，倒数第 3 为 1
& 00000111      | 新桶数目-1
————————————————|
  00000100      | 选择的新桶编号为 4
----------------|
```

> [!important] 
> 
> 由于桶的数目一定是 $2^n$，翻倍扩容后，每个旧桶中的中的键值对一定会按照上述规则迁移到两个新桶中
> 

### 等量扩容

如果填充因子 $\frac{\text{count}}{2^{B}} \le 6.5$，但是使用了较多的溢出桶，就会触发等量扩容机制。

> [!question] 
> 
> 思考：如何判定溢出桶较多呢？
> 
> + 常规桶数目小于或等于 $2^{15}$。如果使用的溢出桶超过了常规桶，则判定为使用了较多的溢出桶
> + 常规桶数目大于 $2^{15}$。如果使用的溢出桶超过了 $2^{15}$，则判定为使用了较多的溢出桶
> 

所谓的等量扩容，就是创建和原来相同数目的新桶。思考：等量扩容有什么用处？什么情况下填充因子没有超出上限，但是使用了较多的溢出桶呢？

> [!tip] 
> 
> 显然，存在大量的键值对被删除的情况。此时，桶中有效键值对排列的就会非常稀疏。
> 
> 等量扩容就会让这些散落在溢出桶中键值对迁移到常规桶中，从而减少溢出桶的使用
> 

下图展示了等量扩容的意义：减少溢出桶的使用

![[Pasted image 20250316233022.png]]

到此，我们就已经将 Go 语言的 map 类型的底层实现简单梳理的一下。下面，我们来看如何使用 map

## 创建 map

声明 `map` 类型的变量的语法如下

```go
var 变量名 map[keyType]valueType
```

> [!tip] 
> 
> `keyType` 可以是任何定义了 `==` 运算符的类型。例如，
> + 数值类型
> + 字符串类型
> + 数组
> + 结构体
> + 接口：注意。需要动态类型定义了 `==` 运算符
> 
> 然而，`valueType` 是任意的类型
> 

和切片一样，`map` 的零值是 `nil`，即 `hmap.buckets` 为 `nil`。根据 Go 的零值初始化规则，上述声明仅仅只是声明一个 `hmap` 结构体变量，其成员被初始化为其类型的零值

> [!tip] 
> 
> 请注意：`map` 是零值不可用的。即，必须要先分配了底层的桶数组，才能使用该 `map` 变量
> 

当然，我们可以提供字面值用于初始化一个 `map` 类型的变量

```go
package main

import "fmt"

func main() {
	var a = map[string]string{
		"brand": "Ford",
		"model": "Mustang",
		"year":  "1964",//请注意：这个逗号不能被丢弃
	}
	b := map[string]int{
		"Oslo":      1,
		"Bergen":    2,
		"Trondheim": 3,
		"Stavanger": 4,
	}

	fmt.Printf("a\t%v\n", a)
	fmt.Printf("b\t%v\n", b)
}
```

一般情况下，创建 `map` 变量也是使用 `make()` 函数，此时需要提供两个参数：类型和容量

```go
var 变量名 = make(map[keyType]valueType, cap)
```

> [!tip] 
> 
> 参数 `cap` 表示创建的 `map` 可以容纳的键值对个数。
> 
> 请注意，由于 Go 的 `map` 要求按照分配 $2^n$ 个桶，每个桶容纳 $8$ 个键值对。因此，实际上创建的空间要比 `cap` 大
> 
> 参数 `cap` 可以省略的，这样会分配较小的容量
> 

```go
package main  
  
import "fmt"  
  
func main() {  
    var m map[string]int  // 声明一个 map 类型的变量  
    fmt.Println(m)        // map[]  
    fmt.Println(m == nil) // true  
    // m["a"] = 1            // panic: assignment to entry in nil map  
    m = make(map[string]int, 10) // 为 m 分配存储空间  
    fmt.Println(m == nil)        // false  
    m["a"] = 1  
    m["b"] = 2  
    m["c"] = 3  
    fmt.Println(m) // map[a:1 b:2 c:3]  
}
```

## 更新和添加键值对

Go 语言中更新和添加键值对使用的是同一个操作，即：

```go
mapName[key] = value
```

> [!tip] 
> 
> `key` 在 `mapName` 中时，触发更新操作
> 
> `key` 不在 `mapName` 中时，触发插入操作
> 

```go
package main

import "fmt"

func main() {
	/* 添加或更新 map 元素 */
	var a = make(map[string]string)
	a["brand"] = "Ford"
	a["model"] = "Mustang"
	a["year"] = "1964"

	fmt.Println(a) //map[brand:Ford model:Mustang year:1964]

	a["year"] = "1970" // Updating an element
	a["color"] = "red" // Adding an element

	fmt.Println(a) // map[brand:Ford color:red model:Mustang year:1970]
}
```

## 删除元素

删除 `map` 中的元素需要使用内置函数 `delete()`：将 `mapName` 中键为 `key` 的元素删除

```go
delete(mapName, key)
```

如下示例代码演示了如何删除 `map` 中的键值对

```go
package main

import "fmt"

func main() {
	/* 删除 map 元素 */
	var a = make(map[string]string)
	a["brand"] = "Ford"
	a["model"] = "Mustang"
	a["year"] = "1964"

	fmt.Println(a) // map[brand:Ford model:Mustang year:1964]

	delete(a, "year") // 删除 key = "year" 的元素

	fmt.Println(a) // map[brand:Ford model:Mustang]
}
```

## 获取 key 对应的 value

通过 `key` 获取 `value` 的语法如下：可以通过 `key` 直接获取到 `map` 中保存的数据

```go
value, ok := mapName[key]
```

Go 语言会根据 `key` 是否在 `mapName` 中返回不同的值

> [!tip] 
> 
> 如果 `key` 在 `map` 中，则 `value` 就是 `key` 对应的值，`ok` 的值为 `true`
> 
> 如果 `key` 不在 `map` 中，则 `value` 就是 `valueType` 的默认值，`ok` 的值为 `false`
> 

如果只想检查某个键是否存在，可以使用匿名变量( `_` ) 代替 `value`

```go
package main

import "fmt"

func main() {
	var a = map[string]string{"brand": "Ford", "model": "Mustang", "year": "1964", "day": ""}

	val1, ok1 := a["brand"] // 检查 key 是否存在并获取其值
	val2, ok2 := a["color"] // 检查 key 是否存在并获取其值
	val3, ok3 := a["day"]   // 检查 key 是否存在并获取其值
	_, ok4 := a["model"]    // 仅检查 key 是否存在

	fmt.Println(val1, ok1) // Ford true
	fmt.Println(val2, ok2) //   false
	fmt.Println(val3, ok3) //   true
	fmt.Println(ok4)       // true
}
```

> [!tip] 
> 
> `key` 为 `color` 的值不存在，则返回 `string` 类型的默认值 `""`
> 

## 遍历 map

遍历 `map` 需要使用 `range` 关键字，它以 `key, value` 的形式返回 `map` 中的元素

```go
package main

import "fmt"

func main() {
	a := map[string]int{"one": 1, "two": 2, "three": 3, "four": 4}

	for k, v := range a {
		fmt.Printf("%v : %v, ", k, v)
	}
	// one : 1, two : 2, three : 3, four : 4,
}
```

> [!attention] 
> 
> 注意：**`map` 的 `key` 是无序的**，千万不要从遍历结果来推测其内部顺序
> 
> 如果需要按照 `key` 的顺序访问 `map`，需要两次遍历
> + 第一次遍历：将 `key` 存放在一个切片中，然后对 `key` 的切片进行排序
> + 第二次遍历：遍历 `key` 的切片，然后访问 `map`
> 
