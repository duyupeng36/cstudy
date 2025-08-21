# 时间

时间是非常重要的，离开了时间，几乎所有生产环境中的数据都将毫无意义。程序可能会关注两种时间类型：**真实世界** 和 **进程时间**

> [!important] 真实时间
> 真实时间的 _**起点**_ 有两种度量方式：_某个标准时间起点_ 和 _进程生命周期内的某个时间_
> + 某个 _标准时间起点_ 度量的时间称为 **_日历时间_**，适用于需要对数据库记录或文件打上时间戳的程序
> + 通过 _进程声明周期内的某个时间起点_ 度量的时间称为 **_流逝时间_** 或 **_墙时间_**，主要针对需要周期性操作或定期从外部输入设备进行度量的程序

> [!tip] 进程时间
> 进程时间是一个进程所使用的 CPU 时间总量，适用于对程序、算法性能的检查或优化

大多数计算机体系结构都内置有硬件时钟，使内核得以计算真实时间和进程时间。操作系统内部对时间的表示方式是 **以自 Epoch 以来的秒数来度量** 的

> [!tip] Epoch：即是 UTC 时间的 **1970 年 1 月 1 日早晨零点**
> Epoch 即是通用协调时间(UTC，也称为格林威治标准时间，或 GMT）的 **1970 年 1 月 1 日早晨零点**

Go 在标准包 `time` 中定义了一个 `Time` 结构体用于处理真实时间

## 时区

谈及真实时间，**时区** 是绕不开的话题。不同的国家（有时甚至是同一国家内的不同地区）使用不同的时区和夏时制。对于要输入和输出时间的程序来说，必须对系统所处的时区和夏时制加以考虑

> [!tip] 时区
> 
> 时区信息非常多并且又多变的，因此，系统没有将其直接编码于程序或函数库中，而是以标准格式保存于文件中，并加以维护
> 
> Unix 系统的时区信息存储目录 `/usr/share/zoneinfo` 中。该目录下的每个文件都包含了一个特定国家或地区内时区制度的相关信息，且往往根据其所描述的时区来加以命名
> 
> 系统的本地时间由时区文件 `/etc/localtime` 定义，通常链接到 `/usr/share/zoneinfo` 下的一个文件
> 

Go 为了处理时区，在 `time` 包中定义了 `time.Location` 类型。通常情况下，`Location` 代表一个地理区域内使用的时间偏移集合。对于许多地点来说，时间偏移会因时间时刻是否使用夏令时而变化

```go
type Location struct {
	name string
	zone []zone
	tx   []zoneTrans

	// 在 tzdata 信息后面可以加上一个字符串，说明如何处理 zoneTrans 中没有记录的 DST 转换。
	// 格式为不带冒号的 TZ 环境变量；参见 https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/V1_chap08.html。
	//  示例字符串，适用于 America/Los_Angeles： PST8PDT,M3.2.0,M11.1.0
	extend string

	// 大多数查询都是针对当前时间的
	// 为了避免通过 tx 进行二进制搜索，可以保留一个静态的单元素缓存，为创建位置的时间提供正确的区域
	// 如果 cacheStart <= t < cacheEnd，则查找会返回 cacheZone。
	// cacheStart 和 cacheEnd 的单位是自 1970 年 1 月 1 日（世界协调时）起的秒数，与 lookup 的参数一致
	cacheStart int64
	cacheEnd   int64
	cacheZone  *zone
}

// zone 代表一个时区，例如 CET.
type zone struct {
	name   string // zone 名字，例如 CET
	offset int    // UTC 以东 offset 秒
	isDST  bool   // 是否是夏令时区域
}

//zoneTrans 表示一次时区转换。
type zoneTrans struct {
	when         int64 // 过渡时间, 自 1970 GMT 以来的秒数
	index        uint8 // 当时生效的区域的 `index
	isstd, isutc bool  // 被忽略 - 不知道这些是什么意思
}
```

> [!important] 零时区：标准时区 
> 
> `zone.offset = 0` 的时区为 `GMT` 格林威治时间（零时区）
> 

`time` 包为 `Location` 类型提供了两个构造函数: `time.FixedZone()` 和 `time.LoadLocation()`

### 构造函数

#### `time.FixedZone()`

`time.FixedZone` 返回一个 `Location`，它总是使用给定的 **区域名** 和 **偏移量**(UTC以东秒)。

```go
func FixedZone(name string, offset int) *Location
```

#### `time.LoadLocation()`

`time.LoadLocation()` 返回给定名称所指定的 `Location`

```go
func LoadLocation(name string) (*Location, error)
```

+ 如果 `name` 是 `""` 或 `"UTC"`，  返回  `UTC`
+ 如果`name` 是 `"Local"`，返回  `Local`

`name` 是 `IANA` 时区数据库中对应文件的位置名称。参考 [全球时区列表](https://www.zeitverschiebung.net/cn/all-time-zones.html)

> [!tip] `name` 查找顺序：按顺序在以下位置查找 `IANA` 时区数据库
> 
> + 由 `ZONEINFO` 环境变量命名的目录或未压缩的 `zip` 文件
> + 在 Unix 系统上，系统标准安装位置（`/usr/share/zoneinfo`）
> + `$GOROOT/lib/time/zoneinfo.zip`
> + `time/tzdata` 包(如果已导入)
> 

### 本地时区(`Local`)和零时区(`UTC`)

**`Local` 表示系统的本地时区**。在 `Unix` 系统上，`Local` 会参考 `TZ` 环境变量来查找要使用的时区。无 `TZ` 表示使用系统默认的 `/etc/localtime`。`TZ=""` 表示使用 `UTC`。`TZ="foo"` 表示使用系统时区目录中的 `foo` 文件。

```go
var Local *Location = &localLoc
```

 `UTC` 时区是指 `GMT` 格林威治时区（零时区）
 
```go
var UTC *Location = &utcLoc
```

## `Month` 类型：月份

`time.Month` 指定一年中的月份 （`January = 1, ....`）

```go
type Month int
```

`time` 包中给定了每隔月份值的常量

```go
const (
	January Month = 1 + iota
	February
	March
	April
	May
	June
	July
	August
	September
	October
	November
	December
)
```

该类型实现了 `Stringer` 接口

```go
func (m Month) String() string
```

返回月份的英文名称（`"January", "February",...`）

## `Time` 类型：时间

`Time` 结构体代表一个 _纳秒精度_ 的时间点。使用 `Time` 的程序通常应将它们 **_存储并作为值传递_**，而 **不是指针**

> [!tip] 应该是使用 `Time` 而非 `*Time` 声明变量或结构体字段
>  
> 也就是说，时间变量和结构字段应为 `Time` 类型。时间，而不是`*Time`

`Time` 的结构体定义如下

```go
type Time struct {
	wall uint64 // 标记日历时间还是进程启动时间
	ext  int64  // 以秒存储日历时间；以纳秒存储进程启动时间

	loc *Location
}
```

> [!important] 字段 `wall` 和 `ext` 的含义
>  
> 字段 `wall` 用于区分记录的时间是 _日历时间_ 还是 _墙时间_
> 
> `wall` 最高位是一个标志位，紧接着的 $33$ 位是秒字段，剩余的 $30$ 位是纳秒字段
> 
> ![Pasted image 20250628224726|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755787344379-7394fc5f62b54b12b1d66a5620638c9c.png)
> 
> + 标志位为 $0$ 时，后续的 $33$ 位也设置为 $0$，并在 `ext` 字段中存储自 $1$ 月 $1$ 日起的秒数(以 $64$ 位有符号数存储)
> + 标志位 $1$，后续的 $33$ 位置设置为 1885 年 1 月 1 日以来的 $33$ 位无符号墙时间秒，并在 `ext` 中存储自进程启动以来的纳秒数($64$ 位有符号数存储)

### 构造函数

`time` 包为 `Time` 类型提供了三类构造函数
+ 使用具体的日期时间构造。`time.Date()` 和 `time.Now()`
+ 使用日期时间字符串构造。`time.Parse()` 和 `time.ParseInLocation()`
+ 使用时间戳构造。`time.Unix()` `time.UnixMicro()` 和 `time.UnixMilli()`

#### `time.Date()`

函数 `time.Date()` 根据参数指定的时间构造并返回一个 `time.Time` 类型的对象。该函数的签名如下

```go
func Date(year int, month Month, day, hour, min, sec, nsec int, loc *Location) Time
```


> [!tip] 参数
> + `year`：代表那一年，它是一个`int` 类型的整数
> + `month`：代表某一年的某个月，它是一个 `time.Month` 类型的对象，本质是一个`int` 类型的整数
> + `day`：代表某个月中的某天，它是一个`int` 类型的整数
> + `hour`：代表某天中的第几个小时，也是一个 `int` 类型的整数
> + `min`：代表在一个小时中的第几分，同样是一个 `int` 类型的整数
> + `sec`：代表一分钟中的几秒，也是`int` 类型的整数
> + `nsec`：代表剩余的纳秒数，同样`int` 类型的整数
> + `loc`：代表创建的 `time.Time` 类型的对象在那个时区
> 
> 请注意：如果 `loc` 为 `nil`，则 `Date` 会出现 `panic`
> 

示例：使用 `FixedZone` 函数设置 `loc`

```go
package main

import (
	"fmt"
	"time"
)

func main() {
	loc := time.FixedZone("UTC-8", -8*60*60)
	t := time.Date(2009, time.November, 10, 23, 0, 0, 0, loc)
	fmt.Println("The time is:", t) // The time is: 2009-11-10 23:00:00 -0800 UTC-8
}

```

示例：使用 `LoadLocation` 函数设置 `loc`

```go
package main

import (
	"fmt"
	"time"
)

func main() {
	loc, _ := time.LoadLocation("Asia/Shanghai")
	t := time.Date(2009, time.November, 10, 23, 0, 0, 0, loc)
	fmt.Println("The time is:", t) // The time is: 2009-11-10 23:00:00 +0800 CST
}
```

#### `time.Now()`

函数 `time.Now()` 返回当前的本地时间，其函数签名如下

```go
func Now() Time
```

```go
package main

import (
	"fmt"
	"time"
)

func main() {
	t := time.Now()
	fmt.Println(t) 
	// 2024-07-12 17:27:01.6023866 +0800 CST m=+0.002673601
}
```

> [!attention] 
> 
> `m=+0.002673601` 为单调时间，利用的是晶体振荡器的间隔时间，很多时间函数计算都舍弃了它。如果不是非常精准的时间间隔计算，请忽略它
> 

#### `time.Parse()` 和 `time.ParseInLocation()`

这两函数将时间字符串转换为 `time.Time` 类型的对象。其中，`time.Parse()` 函数通过 `layout` 指定的时间字符串的解析格式，将 `value` 指定的时间字符串转换为 `time.Time` 类型的对象，时区使用 `UTC`  

```go
func Parse(layout, value string) (Time, error)
```

而函数 `time.ParseInLocation()` 与 `time.Parse()` 函数类似，区别在于 `time.ParsInLocation()` 需要指定时区，其函数的签名如下

```go
func ParseInLocation(layout, value string, loc *Location) (Time, error)
```

```ad-tip 
title: 参数 `layout`

`layout` 时间字符串的解析格式 。Go 语言在解析时间时使用字符串模板 `"2006/01/02 15:04:05 -0700"` 进行解析，其含义如下
+ `2006` 年 `01` 月 `02` 日 下午 `03` 点 `04` 分 `05` 秒 西七区

上述模板中的各个部分还可以使用如下替代模式

```go
Year: "2006" "06"
Month: "Jan" "January" "01" "1"
Day of the week: "Mon" "Monday"
Day of the month: "2" "_2" "02"
Day of the year: "__2" "002"
Hour: "15" "3" "03" (PM or AM)
Minute: "4" "04"
Second: "5" "05"
AM/PM mark: "PM"
```

`time` 包提供了几个预定义的 `layout` 常量，它们都是某种标准的时间字符串格式

```go
const (
	Layout      = "01/02 03:04:05PM '06 -0700" // The reference time, in numerical order.
	ANSIC       = "Mon Jan _2 15:04:05 2006"
	UnixDate    = "Mon Jan _2 15:04:05 MST 2006"
	RubyDate    = "Mon Jan 02 15:04:05 -0700 2006"
	RFC822      = "02 Jan 06 15:04 MST"
	RFC822Z     = "02 Jan 06 15:04 -0700" // RFC822 with numeric zone
	RFC850      = "Monday, 02-Jan-06 15:04:05 MST"
	RFC1123     = "Mon, 02 Jan 2006 15:04:05 MST"
	RFC1123Z    = "Mon, 02 Jan 2006 15:04:05 -0700" // RFC1123 with numeric zone
	RFC3339     = "2006-01-02T15:04:05Z07:00"
	RFC3339Nano = "2006-01-02T15:04:05.999999999Z07:00"
	Kitchen     = "3:04PM"
	// Handy time stamps.
	Stamp      = "Jan _2 15:04:05"
	StampMilli = "Jan _2 15:04:05.000"      // 毫秒
	StampMicro = "Jan _2 15:04:05.000000"   // 微秒
	StampNano  = "Jan _2 15:04:05.000000000" // 纳秒
	DateTime   = "2006-01-02 15:04:05"
	DateOnly   = "2006-01-02"
	TimeOnly   = "15:04:05"
)
```

> [!warning] 
> 
> 参数 `value` 的形式必须与 `layout` 的模板相同，否则解析将会失败
> 

#### `time.Unix()`

通过时间戳构造 `time.Time` 对象

> [!important] 时间戳：自 1970 年 1 月 1 日早上零点起经历的 _秒数_
> 如果需要更精确，也可以使用 _微妙_ 或 _毫秒_

`time.Unix()` 返回自 $1970$ 年 $1$ 月 $1$ 日（世界协调时）起与给定 Unix 时间
+ `sec`(秒) 和 `nsec` (纳秒)相对应的本地时间。传递范围 `[0, 999999999]` 以外的 `nsec` 也是有效的
+ 并非所有 `sec` 值都有相应的时间值。其中一个值是 `1<<63-1`（最大的 `int64` 值）

```go
func Unix(sec int64, nsec int64) Time
```

#### `time.UnixMicro()`

`UnixMicro` 返回与给定 `Unix` 时间相对应的本地时间。`usec` 为自 1970 年 1 月 1 日世界协调时起的**微秒**

```go
func UnixMicro(usec int64) Time
```

#### `time.UnixMilli()`

`UnixMilli` 返回与给定 Unix 时间相对应的本地时间，`msec` 为 1970 年 1 月 1 日（世界协调时）以来的 **毫秒数**

```go
func UnixMilli(msec int64) Time
```

### 方法

下面给出了 `time.Time` 结构体支持的方法

#### 格式化：`Format()` 方法

将 `time.Time` 类型的对象转换为字符串形式。常用的有三种方法 `Format` 方法和 `GoString` 与 `String` 方法

```go
// Format 返回根据参数定义的布局格式化的时间值的文本表示形式
func (t Time) Format(layout string) string

// AppendFormat 类似于 Format，但将文本表示形式追加到 b 并返回扩展缓冲区
func (t Time) AppendFormat(b []byte, layout string) []byte

// GoString 实现 fmt.GoStringer 接口
func (t Time) GoString() string

// String 实现 fmt.Stringer 接口
func (t Time) String() string
```

`Format` 方法需要指定 `layout`，与 `Parse` 系列构造函数中的  `layout` 参数相同

```go
package main

import (
	"fmt"
	"time"
)

func main() {
	t := time.Now()

	fmt.Println(t.Format("2006-01-02 15:04:05.000000000 -0700"))

	b := []byte("当前时间为: ")

	b = t.AppendFormat(b, "2006-01-02 15:04:05.000000000 -0700")

	fmt.Println(string(b))

}

// Out:
// 2024-07-12 18:03:09.259479600 +0800
// 当前时间为: 2024-07-12 18:06:42.842229700 +0800
```

#### 获取属性

```go
// Year 返回 t 指定的年份
func (t Time) Year() int

// YearDay 返回 t 指定当前经历的天数，非闰年范围为 [1,365]，闰年范围为 [1,366]。
func (t Time) YearDay() int

// Month 返回 t 指定年份的月份。
func (t Time) Month() Month

// Weekday 返回 t 指定的星期
func (t Time) Weekday() Weekday

// Day 返回 t 指定月份的日期。
func (t Time) Day() int

// Hour 返回 t 指定的一天内的小时数，范围为 [0, 23]。
func (t Time) Hour() int

// Minute 返回 t 指定的小时内的分钟偏移量，范围为 [0，59]。
func (t Time) Minute() int

// Second 返回 t 指定的分钟内的秒偏移量，范围为 [0, 59]
func (t Time) Second() int

// Nanosecond 返回 t 指定秒内的纳秒偏移量，范围为 [0, 999999999]。
func (t Time) Nanosecond() int

// Location 返回与 t 关联的时区信息、
func (t Time) Location() *Location

// Date 返回 t 发生的年、月、日。
func (t Time) Date() (year int, month Month, day int)

// Clock 返回 t 指定日期内的时、分、秒。
func (t Time) Clock() (hour, min, sec int)

// Zone 计算 t 时有效的时区，返回时区的缩写名称（如 "CET"）及其在 UTC 以东的偏移量（以秒为单位）
func (t Time) Zone() (name string, offset int)

// ZoneBounds 返回时间 t 时有效的时区范围。时区从 start 开始，下一个时区从 end 开始
// 如果时区开始于时间的起点，则 start 返回的时间为零。如果时区一直持续下去，则结束时将返回零时间。返回的时间位置与 t 相同
func (t Time) ZoneBounds() (start, end Time)
```


示例

```go
package main

import (
	"fmt"
	"time"
)

func main() {
	t := time.Now()

	fmt.Printf("年: %v\n", t.Year())   // 年: 2024
	fmt.Printf("月: %v\n", t.Month())  // 月: July
	fmt.Printf("日: %v\n", t.Day())    // 日: 12
	fmt.Printf("时: %v\n", t.Hour())   // 时: 18
	fmt.Printf("分: %v\n", t.Minute()) // 分: 34
	fmt.Printf("秒: %v\n", t.Second()) // 纳秒: 951940700
	fmt.Printf("纳秒: %v\n", t.Nanosecond())

	fmt.Printf("当前经历的天数: %v\n", t.YearDay()) // 当前经历的天数: 194
	fmt.Printf("星期几: %v\n", t.Weekday())     // 星期几: Friday

	fmt.Printf("时区: %v\n", t.Location()) // 时区: Local
}
```

####  算术运算

算术运算符不支持自定义类型，因此需要定义方法完成

```go
// Add 返回新的时间对象 t + d
func (t Time) Add(d Duration) Time

// AddDate 返回将给定的年、月、日数加到 t 上所对应的时间
func (t Time) AddDate(years int, months int, days int) Time

// Sub 返回时间间隔 t-u
func (t Time) Sub(u Time) Duration

// Round 返回将 t 舍入到 d 的最接近倍数的结果（自零时间以来）
func (t Time) Round(d Duration) Time

// Truncate 返回将 t 向下舍入为 d 的倍数的结果（自零时间以来）
func (t Time) Truncate(d Duration) Time
```

> [!important] 两个时间相减，返回的类型是 `time.Duration`
> 
> `time.Duration` 代表时间间隔
> 

> [!WARNING] 
> 
> 警告：Go 语言中没有 `Time + Time` 的概念
> 
> 两个时间类型的实例相加是错误的，时间类型的实例只有加上一个 **时间间隔(`Duration`)** 才是正确的
> 
> 两个时间类型的实例相减得到一个 **时间间隔(`Duration`)** 类型的对象
> 

```go
package main

import (
	"fmt"
	"time"
)

func main() {
	t := time.Now()

	d := time.Duration(time.Hour) // 2024-07-12 18:42:44.9024974 +0800 CST m=+0.003244201

	fmt.Printf("%v\n", t)

	// t = t + d // invalid operation: t + d (mismatched types time.Time and time.Duration)
	t = t.Add(d)

	fmt.Printf("%v\n", t) // 2024-07-12 19:42:44.9024974 +0800 CST m=+3600.003244201
}
```

#### 比较

同样的，时间比较也需要提供方法

```go
// After 报告时间 t 是否在 u 之后
func (t Time) After(u Time) bool

// Before 报告时间 t 是否在 u 之前
func (t Time) Before(u Time) bool

// Compare 比较时间 t 和 u
// + 如果 t 在 u 之前，则返回-1；
// + 如果 t 在 u 之后，则返回+1；
// + 如果两者相同，则返回 0。
func (t Time) Compare(u Time) int

// Equal 报告 t 和 u 是否代表同一时间瞬间
// 即使位于不同地点，两个时间也可以相等
func (t Time) Equal(u Time) bool
```

```go
package main

import (
	"fmt"
	"time"
)

func main() {
	t := time.Now()

	d := time.Duration(time.Hour) // 2024-07-12 18:42:44.9024974 +0800 CST m=+0.003244201

	after := t.Add(d)

	fmt.Println(t.After(after)) // false
	fmt.Println(t.Compare(after))  // -1	
}
```

> [!WARNING] 注意
> Go `==` 运算符不仅比较时间时刻，还比较位置和单调时钟读数
> 
> 因此，在未首先保证已为所有值设置相同的 `Location` 之前，不应将 `Time` 值用作映射或数据库键，这可以通过使用 `UTC` 或 `Local` 方法实现，并且已通过设置 `t = t.Round(0)` 来剥离单调时钟读数
> 
> 一般来说，首选 `t.Equal(u)` 而不是 `t == u`，因为 `t.Equal` 使用最准确的比较，并且当只有一个参数具有单调时钟读数时，可以正确处理这种情况。

#### 当前时间的时间戳

```go
// Unix 返回 t 作为 Unix 时间，即自 1970 年 1 月 1 日 UTC 以来经过的秒数
func (t Time) Unix() int64

// UnixMilli 返回 t 作为 Unix 时间，即自 1970 年 1 月 1 日 UTC 以来经过的毫秒数
func (t Time) UnixMilli() int64

// UnixMicro 返回 t 作为 Unix 时间，即自 1970 年 1 月 1 日 UTC 以来经过的微秒数
func (t Time) UnixMicro() int64

// UnixNano 返回 t 作为 Unix 时间，即自 1970 年 1 月 1 日 UTC 以来经过的纳秒数
func (t Time) UnixNano() int64
```

```go
package main

import (
	"fmt"
	"time"
)

func main() {
	t := time.Now()

	fmt.Printf("经历秒数: %v\n", t.Unix())
	fmt.Printf("经历的毫秒数: %v\n", t.UnixMilli())
	fmt.Printf("经历的微妙数: %v\n", t.UnixMicro())
	fmt.Printf("经历的纳秒数: %v\n", t.UnixNano())
}

// Out:
// 经历秒数: 1720781353
// 经历的毫秒数: 1720781353476
// 经历的微妙数: 1720781353476177
// 经历的纳秒数: 1720781353476177300
```

####  `UTC` 时间和 `Local` 时间之间的转换

```go
// UTC 本地时间转换为 UTC 时间
func (t Time) UTC() Time

// Local 转换为本地时间
func (t Time) Local() Time
```

```go
package main

import (
	"fmt"
	"time"
)

func main() {
	t := time.Now()

	fmt.Printf("当前的本地时间: %v\n", t)

	u := t.UTC()

	fmt.Printf("当前本地时间对应的UTC时间: %v\n", u)
}

// Out:
// 当前的本地时间: 2024-07-12 18:51:25.7971827 +0800 CST m=+0.002747601
// 当前本地时间对应的UTC时间: 2024-07-12 10:51:25.7971827 +0000 UTC
```

## `Duration` 类型：时间间隔

`Duration` 以 `int64` _纳秒_ 计数表示 **_两个时刻之间的经过时间_**。这种表示法将可表示的最大持续时间限制在约 290 年

```go
type Duration int64
```

### 构造函数

#### `time.Since()`

`Since` 返回自 `t` 时刻起经过的时间。 它是 `time.Now().Sub(t)` 的简写

```go
func Since(t Time) Duration
```

#### `time.Until()`

`time.Until(t)` 返回到 `t` 的时间间隔。它是 `t.Sub(time.Now())` 的简写

```go
func Until(t Time) Duration
```

```go
package main

import (
	"fmt"
	"time"
)

func main() {
	t := time.Date(2025, time.July, 12, 17, 0, 0, 0, time.Local)

	fmt.Println(time.Since(t)) // 从 t 指定的时间到现在经历了：-8757h59m9.0318564s

	fmt.Println(time.Until(t)) // 从现在到 t 指定的时间需要经历: 8757h59m9.0313205s
}
```

### 方法

```go
// Abs 返回 d 的绝对值。作为特例，math.MinInt64 被转换为 math.MaxInt64。
func (d Duration) Abs() Duration

// Hours 以浮点数小时的形式返回时间间隔
func (d Duration) Hours() float64

// Microseconds 返回时间间隔的整数微秒数
func (d Duration) Microseconds() int64

// Milliseconds 返回时间间隔的整数毫秒数
func (d Duration) Milliseconds() int64

// Minutes 以浮点数分钟的形式返回时间间隔
func (d Duration) Minutes() float64

// Nanoseconds 以整数纳秒计数返回时间间隔
func (d Duration) Nanoseconds() int64

// Round 返回将 d 舍入到最接近的 m 倍数的结果
func (d Duration) Round(m Duration) Duration

// Seconds 以浮点数秒数的形式返回时间间隔
func (d Duration) Seconds() float64

// String 返回一个字符串，表示时间间隔，格式为“72h3m0.5s”.省略前导零单位
func (d Duration) String() string

// Truncate 将 d 舍入到零的结果返回为 m 的倍数
func (d Duration) Truncate(m Duration) Duration
```

## `Weekday` 类型

`Weekday` 指定一周中的某一天（`周日 = 0, ....`）

```go
type Weekday int
```

使用 `time` 包中的常量表示

```go
const (
	Sunday Weekday = iota
	Monday
	Tuesday
	Wednesday
	Thursday
	Friday
	Saturday
)
```
