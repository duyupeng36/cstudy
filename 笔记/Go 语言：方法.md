# 方法

从90年代早期开始，**面向对象编程(OOP)** 就成为了称霸工程界和教育界的编程范式，所以之后几乎所有大规模被应用的语言都包含了对 OOP 的支持，Go 语言也不例外

尽管 Go 中没有被大众所接受的明确的 OOP 的定义。从我们的理解来讲，**一个对象其实也就是一个简单的值或者一个变量**，在这个对象中会包含一些 **方法**，而方法就是 **和特殊类型关联的函数**

> [!tip] 
> 
> 面向对象的程序会用 **方法来表达其属性和对应的操作**，这样使用这个对象的用户就不需要直接去操作对象，而是借助方法来做这些事情
> 

在正式介绍方法的声明前，我们看一个方法的例子。我们为 [[Go 语言：结构体#匿名成员]] 中定义 `Point` 结构体附加一个方法

```go
type Point struct {
	X, Y int
}

// 函数 Distance 位于包全局作用域
func Distance(p, q Point) float64 {
	return math.Hypot(float64(q.X-p.X), float64(q.Y-p.Y))
}

// 方法 Distance 独属于类型 Point 的函数
func (p Point) Distance(q Point) float64 {
	return math.Hypot(float64(q.X-p.X), float64(q.Y-p.Y))
}
```

在上述代码中，我们声明了两个同名的实体 `Distance()`。其中 `func Distance(p, q Point) float64` 是一个函数，而 `func (p Point) Distance(q Point) float64` 是属于 `Point` 结构的方法。

## 方法的声明

在 Go 语言中，方法的声明语法如下

```go
func (receiver Type) methodName(param Type....)(ret returnType) {

}
```

**`receiver`** 称为方法的 **接收器**，早期的面向对象语言留下的遗产将 **调用一个方法称为“向一个对象发送消息”**

在Go语言中，我们并不会像其它语言那样用 `this` 或者 `self` 作为接收器的名字；而可以任意的选择接收器的名字。由于接收器的名字经常会被使用到，所以保持其在方法间传递时的一致性和简短性是不错的主意

> [!tip] 
> 
> 这里的 **建议是可以使用其类型的第一个字母**，比如这里使用了 `Point` 的首字母 `p`
> 

在方法调用过程中，**接收器参数一般会在方法名之前出现**。这和方法声明是一样的，都是接收器参数在方法名字之前。下面的代码演示了 Go 语言中方法是如何调用的

```go hl:27
package main

import (
	"fmt"
	"math"
)

type Point struct {
	X, Y int
}

func Distance(p, q Point) float64 {
	return math.Hypot(float64(q.X-p.X), float64(q.Y-p.Y))
}

func (p Point) Distance(q Point) float64 {
	return math.Hypot(float64(q.X-p.X), float64(q.Y-p.Y))
}

func main() {
	var p, q = Point{0, 0}, Point{3, 4}
	// 调用函数
	fmt.Printf("%#v 到 %#v 的距离为: %#f\n", p, q, Ditance(p, q))
	// main.Point{X:0, Y:0} 到 main.Point{X:3, Y:4} 的距离为: 5.000000

	// 调用方法
	fmt.Printf("%#v 到 %#v 的距离为: %#f\n", p, q, p.Ditance(q))
	// main.Point{X:0, Y:0} 到 main.Point{X:3, Y:4} 的距离为: 5.000000
}
```

可以看到，上面的两个函数调用都是 `Distance()`，但是却没有发生冲突。第一个 `Distance()` 的调用实际上用的是包级别的函数 `main.Distance()`，而第二个则是使用刚刚声明的 `Point` 类型的变量，调用的是 `Point` 类下声明的 `Point.Distance()` 方法

这种 `p.Distance` 的表达式叫做 **选择器**，因为他会选择合适的对应 `p` 这个对象的 `Distance()` 方法来执行。选择器 **也会被用来选择一个 `struct` 类型的字段**，比如 `p.X`

> [!attention] 
> 
> 由于方法和字段都是在同一命名空间，所以如果我们在这里声明一个 `X()` 方法的话，编译器会报错，因为在选择器 `p.X` 会有歧义
> 

每种类型都有其方法的命名空间，我们在用 `Distance` 这个名字的时候，不同的 `Distance` 调用指向了不同类型里的 `Distance` 方法

让我们来定义一个 `Path` 类型，这个 `Path` 代表一个线段的集合，并且也给这个 `Path` 定义一个叫 `Distance` 的方法

```go
type Path []Point

func (path Path) Distance() (sum float64) {
	for i := range path {
		if i > 0 {
			sum += path[i-1].Distance(path[i])
		}
	}
	return
}
```

**`Path` 是一个命名的 `slice` 类型**，而不是 `Point` 那样的 `struct` 类型，然而我们依然可以为它定义方法。在能够给任意类型定义方法这一点上，Go 和很多其它的面向对象的语言不太一样。因此在 Go 语言中，我们为一些简单的数值、字符串、切片、Map 定义一些附加行为很方便

> [!attention] 
> 
> 注意命名类型的底层类型不能是指针或 `interface`
> 
> 我们可以给同一个包内的任意命名类型定义方法，只要这个 **命名类型的底层类型 _不是_ 指针或者 `interface`**
> 

这个上述例子中，底层类型是指 `[]Point` 这个 切片，`Path` 就是命名类型。这两个 `Distance()` 方法有不同的类型，他们两个方法之间没有任何关系，尽管 `Path` 的 `Distance()` 方法会在内部调用 `Point.Distance()` 方法来计算每个连接邻接点的线段的长度

下面的示例代码演示了

```go
package main

import (
	"fmt"
)

func main() {
	var p, q = Point{0, 0}, Point{3, 4}
	var d = Point{4, 0}

	var perim = Path{p, q, d, p}
	fmt.Printf("三角形的周长是: %f\n", perim.Distance())
}
```

在上面两个对 `Distance` 名字的方法的调用中，编译器会根据方法的名字以及接收器来决定具体调用的是哪一个函数。第一个例子中 `path[i-1]` 数组中的类型是 `Point`，因此 `Point.Distance` 这个方法被调用；在第二个例子中 `perim` 的类型是 `Path`，因此 `Distance` 调用的是 `Path.Distance`

**对于一个给定的类型，其内部的方法都必须有唯一的方法名**，但是**不同的类型却可以有同样的方法名**，比如我们这里 `Point` 和 `Path` 就都有 `Distance` 这个名字的方法；所以我们没有必要非在方法名之前加类型名来消除歧义，比如 `PathDistance`

> [!tip] 
> 
> 这里我们已经看到了方法比之函数的一些好处：**方法名可以简短**。当我们 **在包外调用的时候** 这种好处就会被放大，因为我们可以使用这个短名字，而 **可以省略掉包的名字**。如果要在包外调用 `Distance()` 函数，就必须加上包名 `draw`，即 `draw.Distance(p, q)`
> 

## 指针接收器

当调用一个函数时，会对其每一个参数值进行拷贝，如果一个函数需要 **更新一个变量**，或者函数的其中一个参数实在太大我们希望能够 **避免进行这种默认的拷贝**，这种情况下我们就需要用到 **指针** 了。对应到我们这里用来 **更新接收器的对象** 的方法，当这个接受者变量本身比较大时，我们就可以用其指针而不是对象来声明方法，如下

```go
// 指针接收器
func (p *Point) ScaleBy(factor int) {
	p.X *= factor
	p.Y *= factor
}
```

这个方法的名字是 `(*Point).ScaleBy`。这里的括号是必须的；没有括号的话这个表达式可能会被理解为 `*(Point.ScaleBy)` 

> [!hint] 
> 
> 在现实的程序里，一般会约定如果 `Point` 这个类有一个指针作为接收器的方法，那么所有 `Point` 的方法都必须有一个指针接收器，即使是那些并不需要这个指针接收器的函数
> 

**只有类型 `(Point)` 和指向他们的指针 `(*Point)` ，才是可能会出现在接收器声明里的两种接收器**。此外，为了避免歧义，在声明方法时，**如果一个类型名本身是一个指针的话，是不允许其出现在接收器中的**，比如下面这个例子

```go
type P *int

func (p P) f() { /* ... */ } // compile error: invalid receiver type
```

如何调用指针接收器方法？想要调用指针类型方法 `(*Point).ScaleBy` ，只要提供一个 `Point` 类型的指针即可，像下面这样

```go
// 创建该类型的指针 这是正统的调用接收器是指针的方法
r := &draw.Point{X: 1, Y: 2}
r.ScaleBy(2)
fmt.Println(*r)

// 对该类型的变量取地址
var p = draw.Point{X: 1, Y: 2}
pp := &p
pp.ScaleBy(2)
fmt.Println(p)

p := draw.Point{X: 1, Y: 2}
(&p).ScaleBy(2)
fmt.Println(p)
```

不过后面两种方法有些笨拙。幸运的是，Go 语言本身在这种地方会帮到我们。如果接收器 `p` 是一个 `Point` 类型的变量，并且其方法需要一个 `*Point` 指针作为接收器，我们可以用下面这种简短的写法

```go
p := draw.Point{X: 1, Y: 2}
p.ScaleBy(2)
```

**编译器会隐式地帮我们用 `&p` 去调用 `ScaleBy` 这个方法**。这种简写方法 **只适用于“变量”**，包括 `struct` 里的字段比如 `p.X`，以及 `array` 和 `slice` 内的元素比如 `perim[0]`

> [!WARNING] 
> 我们 **不能通过一个无法取到地址的接收器来调用指针方法**，比如 _临时变量的内存地址就无法获取得到_

当然，**我们也可以用一个 `*Point` 这样的接收器来调用 `Point` 的方法**，因为我们可以通过地址来找到这个变量，只要 **用解引用符号`*` 来取到该变量即可**。**编译器在这里也会给我们隐式地插入 `*` 这个操作符**，所以下面这两种写法等价的

```go
pptr.Distance(q)  // 编译器隐式添加 * 进行解引用
(*pptr).Distance(q) // 自行解引用
```

> [!tip] 指针接收器方法的调用
> 
> + 接收器的实参类型和形参类型相同。即都是 `T` 或 `*T` 的情况下
> 
> ```go
> Point{1, 2}.Distance(q) //  Point
> pptr.ScaleBy(2)         // *Point
> ```
>
> + **接收器 实参 是类型`T`，但形参是类型 `*T`** ，这种情况下 _编译器会隐式地为我们取变量的地址_
> 
> ```go
> p.ScaleBy(2) // implicit (&p)
> ```
>
> + **接收器实参是类型 `*T` ，形参是类型 `T`**。_编译器会 隐式地为我们解引用_，取到指针指向的实际变量
> 
> ```go
> pptr.Distance(q) // implicit (*pptr)
> ```


> [!important] 总结
> - 不管你的 `method` 的 `receiver` 是 _指针类型_ 还是 _非指针类型_，都是可以 **通过指针/非指针类型的变量进行调用**的，编译器会帮你做类型转换
> 
> 
> - 在声明一个 `method` 的 `receiver` 是指针还是非指针类型时，你需要考虑两方面的内部
> 	- 第一方面是这个 **对象本身是不是特别大**，如果声明为非指针变量时，调用会产生一次拷贝
> 	- 第二方面是如果你用指针类型作为 `receiver`，那么你一定要注意，这种指针类型指向的始终是一块内存地址，就算你对其进行了拷贝。熟悉 `C` 或者 `C++`的人这里应该很快能明白

###  `nil` 也是合法的接收器

就像一些 **函数允许 `nil` 指针作为参数** 一样，方法理论上 **也可以用 `nil` 指针作为其接收器**，尤其当 `nil` 对于对象来说是合法的零值时，比如 `map` 或者 `slice`。在下面的简单 `int` 链表的例子里，`nil` 代表的是空链表

```go
// IntList 是一个整数链接列表。
// nil *IntList 表示空列表
type IntList struct {
    Value int
    Tail  *IntList
}
// Sum 返回列表元素的总和。
func (list *IntList) Sum() int {
    if list == nil {
        return 0
    }
    return list.Value + list.Tail.Sum()
}
```

当定义一个允许 `nil` 作为接收器值的方法的类型时，在类型前面的注释中指出 `nil` 变量代表的意义是很有必要的，就像我们上面例子里做的这样

## 通过匿名成员扩展类型

现在，我们需要对 `Point` 进行着色，来看看 `ColoredPoint` 这个类型：

```go
type ColoredPoint struct {
	Point
	Color color.RGBA
}
```

我们完全可以将 `ColoredPoint` 定义为一个有三个字段的结构体，但是我们却将 `Point` 这个类型嵌入到 `ColoredPoint` 来提供 `X` 和 `Y` 这两个字段。像我们在 [[Go 语言：结构体#匿名成员]] 中看到的那样，内嵌可以使我们在定义 `ColoredPoint` 时得到一种句法上的简写形式，并使其包含 `Point` 类型所具有的一切字段，然后再定义一些自己的

如果我们想要的话，我们可以直接认为通过嵌入的字段就是 `ColoredPoint` 自身的字段，而完全不需要在调用时指出 `Point`，比如下面这样

```go
cp := draw.ColoredPoint{}
cp.X = 1
cp.Y = 2
fmt.Println(cp.Point.X) // 1
fmt.Println(cp.Point.Y) // 2
```

对于 `Point` 中的方法我们也有类似的用法，我们可以把 `ColoredPoint` 类型当作接收器来调用 `Point` 里的方法，即使 `ColoredPoint` 里没有声明这些方法

```go
red := color.RGBA{255, 0, 0, 255}
blue := color.RGBA{0, 0, 255, 255}
var p = draw.ColoredPoint{draw.Point{1, 1}, red}
var q = draw.ColoredPoint{draw.Point{5, 4}, blue}
fmt.Println(p.Distance(q.Point))
p.ScaleBy(2)
q.ScaleBy(2)
fmt.Println(p.Distance(q.Point))
```

`Point` 结构体的方法也被引入了 `ColoredPoint` 结构体中。内嵌可以使我们定义字段特别多的复杂类型，我们可以将字段先按小类型分组，然后定义小类型的方法，之后再把它们组合起来

> [!WARNING] 注意，结构体内嵌和不是 `is-a` 的关系
> 
> 如果对基于类来实现面向对象的语言比较熟悉的话，可能会倾向于将 `Point` 看作一个基类，而 `ColoredPoint` 看作其子类或者继承类，或者将 `ColoredPoint` 看作 `"is a" Point`类型
> 
> 但这是错误的理解。请注意上面例子中对 `Distance` 方法的调用。`Distance` 有一个参数是 `Point` 类型，但 `q` 并不是一个 `Point` 类，所以尽管 `q` 有着 `Point` 这个内嵌类型，我们也必须要显式地选择它

尝试直接传 `q` 的话你会看到下面这样的错误

```go
p.Distance(q) 
// cannot use q (variable of type ColoredPoint) as Point value in argument to p.Distance
```

一个 `ColoredPoint` 并不是一个 `Point`，但他 `"has a" Point`，并且它有从 `Point` 类里引入的 `Distance()` 和 `ScaleBy()` 方法。如果你喜欢从实现的角度来考虑问题，**内嵌字段会指导编译器去生成额外的包装方法来委托已经声明好的方法**，和下面的形式是等价的

```go
// 相当于 Go 将 Point 的 Distance 在 ColoredPoint 上进行了一层包装
func (p ColoredPoint) Distance(q Point) float64 {
    return p.Point.Distance(q)
}

// 相当于 Go 将 Point 的 ScaleBy 在 ColoredPoint 上进行了一层包装
func (p *ColoredPoint) ScaleBy(factor float64) {
    p.Point.ScaleBy(factor)
}
```

当 `Point.Distance` 被第一个包装方法调用时，它的接收器值是 `p.Point`，而不是 `p`，当然了，在 `Point` 结构的方法里，你是访问不到 `ColoredPoint` 的任何字段的

一个 `struct` 类型也可能会有多个匿名字段。我们将 `ColoredPoint` 定义为下面这样。然后 **这种类型的值便会拥有 `Point` 和 `RGBA` 类型的所有方法**，以及直接定义在 `ColoredPoint` 中的方法

```go
type ColoredPoint struct {
	Point
	color.RGBA
}
```

> [!WARNING] 选择器选择方法的顺序
> 当编译器解析一个选择器到方法时，比如 `p.ScaleBy`，它会首先去找直接定义在这个类型里的 `ScaleBy` 方法，然后找被 `ColoredPoint` 的内嵌字段们引入的方法，然后去找 `Point` 和 `RGBA` 的内嵌字段引入的方法，然后一直递归向下找。如果选择器有二义性的话编译器会报错，比如你在同一级里有两个同名的方法

**方法只能在命名类型(像 `Point`)或者指向命名类型的指针上定义**，但是多亏了内嵌，有些时候我们给匿名 `struct` 类型来定义方法也有了手段。例如

```go
var cache = struct {
    sync.Mutex
    mapping map[string]string
}{
    mapping: make(map[string]string),
}

func Lookup(key string) string {
    cache.Lock()  // 给 cache 加锁
    v := cache.mapping[key] // 访问 cache
    cache.Unlock()  // 给 cache 解锁
    return v
}
```

我们给新的变量起了一个更具表达性的名字：`cache`。因为 `sync.Mutex` 字段也被嵌入到了这个 `struct` 里，其 `Lock` 和 `Unlock` 方法也就都被引入到了这个匿名结构中了，这让我们能够以一个简单明了的语法来对其进行加锁解锁操作


## 示例程序：实现 BitSet 集合

Go 语言里的集合一般会用 `map[T]bool` 这种形式来表示，`T` 代表元素类型。集合用 `map` 类型来表示虽然非常灵活，但我们可以以一种更好的形式来表示它。例如在数据流分析领域，集合元素通常是一个非负整数，集合会包含很多元素，并且集合会经常进行并集、交集操作，这种情况下，`bit` 数组会比 `map` 表现更加理想

> [!example] 
> 
> 我们执行一个 `http` 下载任务，把文件按照 `16kb` 一块划分为很多块，需要有一个全局变量来标识哪些块下载完成了，这种时候也需要用到 `bit` 数组
> 

一个 `bit` 数组通常会用一个 **无符号数** 或者称之为“字”的 `slice` 来表示，每一个元素的每一位都表示集合里的一个值。当集合的第 `i` 位被设置时，我们才说这个集合包含元素 `i`。下面的这个程序展示了一个简单的 `bit` 数组类型，并且实现了三个函数来对这个 `bit` 数组来进行操作


```go
package bitset

import (
	"fmt"
	"strings"
)

// BitSet 位集合表示一个非负整数集合
type BitSet struct {
	// words 是一个 uint64 的切片,切片中的每个元素存储 64 个整数
	// [0000 0000][0000 0000] ... [0000 0000][0000 0000] 共 8 个字节
	// 从右到左存储，1 表示该位置存储了元素，否则表示没有存储元素
	words []uint64
}

// Has 判断 value 是否在 BitSet 中
func (bitset *BitSet) Has(value int) bool {
	// index 表示 value 存储在 words 切片中的索引
	// bit 表示 value 所在的 word 的第 bit 位
	if bitset.words == nil {
		return false
	}
	index, bit := value/64, value%64
	return index < (len(bitset.words)) && (bitset.words[index]&(1<<bit)) != 0
}

// Add 将 value 添加到 bitset 中
func (bitset *BitSet) Add(value int) {
	// 计算 value 在 words 切片中的位置
	index, bit := value/64, uint(value%64)

	// 判断 value 是否已经可以放入集合中
	for index >= len(bitset.words) {
		// 此时，value 不能存储在集合中，需要对集合进行扩容
		bitset.words = append(bitset.words, 0)
	}
	// 将 value 放入集合中
	bitset.words[index] |= 1 << bit
}

// Union：对 bitset 和 other 求并集，并更新 bitset
func (bitset *BitSet) Union(other *BitSet) {
	// 只需要遍历 other 的 words 切片，将其放入 bitset.words 对应的索引位置即可
	for index, owords := range other.words {
		if index < len(bitset.words) {
			bitset.words[index] |= owords
		} else {
			bitset.words = append(bitset.words, owords)
		}
	}
}

// String 返回 BiteSet 的字符串表示形式
func (bitset *BitSet) String() string {
	// 遍历 bitset.words 中的每个元素，并将其转换为字符串
	builder := strings.Builder{}
	builder.WriteString("{")
	for index, word := range bitset.words {
		// word 为 0 表示没有存储值
		if word == 0 {
			continue
		}
		// 遍历 word 的每个位，并将其对应的数字转为字符串
		for j := range 64 {
			// 第 j 位上存储了数字
			if word&(1<<uint(j)) != 0 {
				// 缓冲区中已经存储了其他数字，使用空格隔开
				if builder.Len() > len("{") {
					builder.WriteString(" ")
				}
				builder.WriteString(fmt.Sprintf("%d", 64*index+j))
			}
		}
	}
	builder.WriteString("}")
	return builder.String()
}
```

> [!NOTE] 实现细节
> 
> 因为每一个字都有 $64$ 个二进制位，所以为了定位 `value` 的 `bit` 位，我们用了 `value / 64` 的商作为字的下标，并且用 `value % 64` 得到的值作为这个字内的 `bit` 的所在位置
> 
> 在实现 `Union()` 方法时，需要使用位或(`|`) 运算符将切片中对应位置上的值融合在一起
> 
> 为了显示集合中的值，我们定义了 `String()` 方法。该方法将集合中存储的值显示出来
> 

现在，我们就可以在其他包中使用了

```go
package main

import (
	"fmt"

	"github.com/duyupeng36/roadmap/gostudy/bitset"
)

func main() {
	var set1, set2 bitset.BitSet
	set1.Add(10)
	set1.Add(100)
	set1.Add(9)
	fmt.Println(set1.String()) // {9 10 100}

	set2.Add(9)
	set2.Add(144)
	fmt.Println(set2.String()) // {9 144}

	set1.Union(&set2)
	fmt.Println(set1.String())                // {9 10 100 144}
	fmt.Println(set1.Has(144), set1.Has(123)) // true false

}
```


> [!WARNING] 
> 
> 我们声明的 `String()` 和 `Has()` 两个方法都是以指针类型 `*BitSet` 来作为接收器的，但实际上对于这两个类型来说，把接收器声明为指针类型也没什么必要
> 
> 然而，对于 `Add()` 和 `Union()` 方法就必须使用 `*BitSet` 来作为接收器，因为它们要操作的是`bitset.words` 对象。如果你不把接收器声明为指针对象，那么实际操作的是拷贝对象，而不是原来的那个对象

因此，因为我们的 `String()` 方法定义在 `*BitSet` 上，所以当我们的变量是 `BitSet` 类型而不是 `*BitSet` 时，可能会有下面这样让人意外的情况

```go
x := bitset.BitSet{}  // 变量是 BitSet 类型，而不是 *BitSet 类型
x.Add(1)
x.Add(9)
x.Add(42)
x.Add(144)

fmt.Println(&x)         // {1 9 42 144}
fmt.Println(x.String()) // {1 9 42 144}
fmt.Println(x)          // {[4398046511618 0 65536]}
```

第一个 `Println()` 中，我们打印一个`*BitSet` 的指针，这个类型的指针确实有自定义的 `String()` 方法

第二 `Println()`，我们直接调用了 `x` 变量的 `String()` 方法；这种情况下编译器会隐式地在 `x` 前插入 `&` 操作符，这样相当远我们还是调用的 `BitSet` 指针的 `String` 方法

第三个 `Println()` 中，因为 `BitSet` 类型没有 `String()` 方法，所以 `Println()` 方法会直接以原始的方式理解并打印。所以在这种情况下 `&` 符号是不能忘的。在我们这种场景下，你把 `String()` 方法绑定到 `IntSet` 对象上，而不是 `*IntSet` 指针上可能会更合适一些，不过这也需要具体问题具体分析

## 封装

一个 **对象的变量或者方法如果对调用方是不可见** 的话，一般就被定义为 **“封装”**。封装有时候也被叫做 **信息隐藏**，同时也是面向对象编程最关键的一个方面

Go 语言只有一种控制可见性的手段：**大写首字母的标识符会从定义它们的包中被导出**，**小写字母的则不会**。这种限制包内成员的方式同样适用于 `struct` 或者一个类型的方法。因而 **如果我们想要封装一个对象，我们必须将其定义为一个 `struct`**

这也就是前面的小节中 `BitSet` 被定义为 `struct` 类型的原因，尽管它只有一个字段

```go
type BitSet struct {
    words []uint64  // words 不会被包的使用者发现
}
```

> [!NOTE] 信息隐藏
> `words` 怎样存储的，客户端(包的使用者)并不清楚，这样就避免了客户端直接使用底层实现

当然，我们也可以把 `BitSet` 定义为一个 `slice` 类型，尽管这样我们就需要把代码中所有方法里用到的 `s.words` 用 `*s` 替换掉了

```go
type BitSet []uint64
```

尽管这个版本的 `BitSet` 在本质上是一样的，他也可以允许其它包中可以直接读取并编辑这个  `slice`。换句话说，相对 `*bitset` 这个表达式会出现在所有的包中，`bitset.words` 只需要在定义 `IntSet` 的包中出现

这种基于名字的手段使得 **在 Go 语言中最小的封装单元是 `package`**，而不是像其它语言一样的类型。**一个 `struct` 类型的字段对同一个包的所有代码都有可见性**，无论你的代码是写在一个函数还是一个方法里

**封装提供了三方面的优点**
- 首先，因为 **调用方不能直接修改对象的变量值**，其只需要关注少量的语句并且只要弄懂少量变量的可能的值即可

- 第二，**隐藏实现的细节**，可以防止调用方依赖那些可能变化的具体实现，这样使设计包的程序员在不破坏对外的 `api` 情况下能得到更大的自由

- 封装的第三个优点也是最重要的优点，是 **阻止了外部调用方对对象内部的值任意地进行修改**。因为 **对象内部变量只可以被同一个包内的函数修改**，所以包的作者可以让这些函数确保对象内部的一些值的 **不变性**

