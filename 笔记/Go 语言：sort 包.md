# sort 包

包 `sort` 默认提供了对 `[]int, []float64, []string` 类型的 **排序** 和 **查找** 的算法

## 包级的排序和查找函数

`sort` 包提供了下面的函数用于执行 `[]int, []float64, []string` 类型的排序

```go
// Find 使用二进制搜索来查找并返回 cmp(i) <= 0 时 [0, n) 中的最小索引 i。如果没有这样的索引 i，Find 返回 i = n
func Find(n int, cmp func(int) int) (i int, found bool)

/* float64 int64 string 切片的排序 */
// Float64s 按递增顺序 float64切片 进行排序。非整数 (NaN) 值在其他值之前排序
// 注：从 Go 1.22 开始，该函数只需调用 slices.Sort
func Float64s(x []float64)


// Float64sAreSorted 报告切片 x 是否按递增顺序排序，非数值 (NaN) 值是否排在其他值之前
// 注：从 Go 1.22 开始，该函数只调用 slices.IsSorted。
func Float64sAreSorted(x []float64) bool

// Ints 按递增顺序对 int 切片进行排序
// 注：从 Go 1.22 开始，该函数只需调用 slices.Sort
func Ints(x []int)


// IntsAreSorted 报告片段 x 是否按递增顺序排序
// 注：从 Go 1.22 开始，该函数只调用 slices.IsSorted。
func IntsAreSorted(x []int) bool


// IsSorted 报告 data 是否排序
func IsSorted(data Interface) bool

// Strings按递增顺序排列字符串片段
// 注：从 Go 1.22 开始，该函数只需调用 slices.Sort
func Strings(x []string)

// StringsAreSorted 报告片段 x 是否按递增顺序排序
// 注：从 Go 1.22 开始，该函数只调用 slices.IsSorted
func StringsAreSorted(x []string) bool

/*二分查找：被查询的集合一定是排序的，除了 Search 函数外，其他的函数必须是升序排列 */
// Search使用二分查找来查找并返回在 [0,n) 中 f(i) 为真时的最小索引 i，假设在 [0,n) 范围内，f(i) == true 意味着 f(i+1) == true
// 也就是说，Search 要求输入范围 [0,n] 的某个前缀(可能为空) f 为 false，余数(可能为空)为 true; Search 返回第一个 true 索引。如果没有这样的索引，搜索返回n(注意，“未找到”返回值不是 -1，例如，在 strings.Index 中)。搜索仅当i在[0,n)范围内时调用 f(i)。
func Search(n int, f func(int) bool) int


// SearchFloat64s 在 float64s 的排序切片中搜索 x，并返回 Search 指定的索引。如果 x 不存在，返回值就是插入 x 的索引（可能是 len(a)）。切片必须以升序排序
func SearchFloat64s(a []float64, x float64) int


// SearchInts 在已排序的 ints 切片中搜索 x，并返回 Search 指定的索引。如果 x 不存在，返回值就是插入 x 的索引（可能是 len(a)）。片段必须以升序排序
func SearchInts(a []int, x int) int


// SearchStrings 在排序后的 strings 串切片中搜索 x，并返回 Search 指定的索引。如果 x 不存在，返回值就是插入 x 的索引（可能是 len(a)）。切片必须以升序排序
func SearchStrings(a []string, x string) int


/* 对任意类型的切片进行排序，只需要提供 less 函数 */
// Slice 根据提供的 less 函数对切片 x 进行排序。如果 x 不是切片，它就会 panic。
func Slice(x any, less func(i, j int) bool)


// SliceIsSorted 报告切片 x 是否按照提供的 less 函数排序。如果 x 不是切片，它就会 panic
// 注意：在许多情况下，较新的 slices.SortStableFunc 函数运行速度也更快。
func SliceIsSorted(x any, less func(i, j int) bool) bool

// SliceStable 使用提供的 less 函数对切片 x 排序，保持相等元素的原始顺序。如果 x 不是切片，它就会 panic
// less 函数必须满足与接口类型的 Less 方法相同的要求
func SliceStable(x any, less func(i, j int) bool)
```

### 示例：查找 `Find` 函数

```go
package main

import (
	"fmt"
	"sort"
)

func main() {
	s := []int{1, 4, 6, 7, 9, 20, 22, 27}
	x := 9
	i, found := sort.Find(len(s), func(i int) int {
		if x < s[i] {
			return -1
		} else if x == s[i] {
			return 0
		}
		return 1
	})
	if found {
		fmt.Printf("found %d at %d in %#v\n", x, i, s)
	} else {
		fmt.Printf("not found %d in %#v\n", x, s)
	}
}
```

### 示例：排序 `Ints, Float64s, Strings` 函数

```go
package main

import (
	"fmt"
	"math"
	"sort"
)

func main() {
	/* float64 切片的排序 */
	f := []float64{5.2, -1.3, 0.7, -3.8, 2.6} // unsorted
	sort.Float64s(f)
	fmt.Println(f)

	f = []float64{math.Inf(1), math.NaN(), math.Inf(-1), 0.0} // unsorted
	sort.Float64s(f)
	fmt.Println(f)

	/* int64 切片的排序*/
	i := []int{5, 2, 6, 3, 1, 4} // unsorted
	sort.Ints(i)
	fmt.Println(i)

	/* string 切片的排序 */
	s := []string{"Go", "Bravo", "Gopher", "Alpha", "Grin", "Delta"}
	sort.Strings(s)
	fmt.Println(s)
}
```

### 示例：查找 `Search` 函数

```go
package main

import (
	"fmt"
	"sort"
)

func main() {
	a := []int{1, 3, 6, 10, 15, 21, 28, 36, 45, 55}
	x := 6
	i := sort.Search(len(a), func(i int) bool {
		return a[i] >= x
	})
	
	if i < len(a) && a[i] == x {
		fmt.Printf("found %d at index %d in %v\n", x, i, a)
	} else {
		fmt.Printf("%d not found in %v\n", x, a)
	}
}
```

## 排序接口  `sort.Interface`

```go
type Interface interface {
    // Len 方法返回集合中的元素个数
    Len() int
    // Less 方法报告索引 i 的元素是否比索引 j 的元素小
    Less(i, j int) bool
    // Swap 方法交换索引 i 和 j 的两个元素
    Swap(i, j int)
}
```

对于实现了上述接口的容器类型，可以采用下面的两个函数进行排序

```go
/* 对自定义容器类型进行排序 */
// Sort 按照 Less 方法确定的升序对数据进行排序。它调用一次 data.Len 来确定 n，并调用 O(n*log(n)) 次 data.Less 和 data.Swap。排序不保证稳定
// 注意：在许多情况下，较新的 slices.SortFunc 函数运行速度也更快。
func Sort(data Interface)
// Stable 按照 Lesss 方法确定的升序排序数据，同时保持相同元素的原始顺序。

func Stable(data Interface)

// Reverse 返回数据的相反顺序
func Reverse(data Interface) Interface
```

## `sort` 包中实现了 `Interface` 的类型

### `IntSlice`

```go
type IntSlice []int

func (p IntSlice) Len() int
func (p IntSlice) Less(i, j int) bool
func (p IntSlice) Swap(i, j int)

func (p IntSlice) Sort() // 相当于 sort.Sort(p)
func (p IntSlice) Search(x int) int  // 相当于 sort.SearchInts(p, x)
```

### `Float64Slice`

```go
type Float64Slice []float64

func (p Float64Slice) Len() int
func (p Float64Slice) Less(i, j int) bool
func (p Float64Slice) Swap(i, j int)

func (p Float64Slice) Sort()
func (p Float64Slice) Search(x float64) int
```

### `StringSlice`

```go
type StringSlice []string

func (p StringSlice) Len() int
func (p StringSlice) Less(i, j int) bool
func (p StringSlice) Swap(i, j int)

func (p StringSlice) Sort()
func (p StringSlice) Search(x string) int
```

## 示例：自定义容器的排序

```go
package main

import (
	"fmt"
	"sort"
)

type SliceSliceInt [][]int

func (ssi SliceSliceInt) Len() int {
	return len(ssi)
}

func (ssi SliceSliceInt) Less(i, j int) bool {
	len_i := len(ssi[i])
	len_j := len(ssi[j])
	k := 0
	isILessJ := true
	for ; k < len_i && k < len_j; k++ {
		if ssi[i][k] > ssi[j][k] {
			isILessJ = false
			return isILessJ
		}
	}
	if k < len_i {
		return false
	} else {
		return true
	}
}

func (ssi SliceSliceInt) Swap(i, j int) {
	ssi[i], ssi[j] = ssi[j], ssi[i]
}

func main() {
	ssi := SliceSliceInt{
		{1, 2, 5, 7},
		{2, 3, 4},
		{1, 2, 3},
	}
	sort.Sort(ssi)
	for _, v := range ssi {
		fmt.Println(v)
	}
	/*
		[1 2 3]
		[1 2 5 7]
		[2 3 4]
	*/
}
```
