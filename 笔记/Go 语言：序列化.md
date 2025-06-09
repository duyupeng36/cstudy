# 序列化

内存中的 `map` `slice` `array` 以及各种对象，包括自定义的结构体实例，如何保存到一个文件中？ 

>[!tip] 如何保存内存中的对象到文件中？
 
如何从文件中读取数据，并让它们在内存中再次恢复成自己对应的类型的实例？

>[!tip] 如何从文件中的数据并在内存中恢复对应类型的实例？

>[!summary] **序列化** 就是某种转换 **协议**
>
>要设计一套 **协议**，按照某种规则，把内存中数据保存到文件中。文件是一个字节序列，所以必须把数据转换成 **字节序列**，输出到文件。这就是 **序列化**
>
>简单的来说：**序列化** 就是将内存中的某个 **数据结构** 转换为一种 **字节序列**

## 序列化与反序列化

**序列化(`serialization`)**：将内存中的数据对象(**数据结构**)按照某种 **协议** 转换为 字节序列

**反序列化(`deserialization`)**：将 **字节序列** 按照某种 **协议** 恢复为内存中的对象(**数据结构**)

> [!important] 序列化协议需要解决三个问题
> + 对象的 **类型** 如何表示？
> + 对象的 **数据** 如何存储
> + 数据之间的 **间隔** 如何表示？

序列化之后可以将字节序列保存为文件(持久化)或通过网络传输。

有两种方法将对象转变为字节序列

> [!tip] 序列化的两种方案
> 
> **字符序列化**：使用 **字符序列** 表示内存中的对象，只是方便了人类阅读。本质也是字节序列，这些字节序列代表了某个字符
> 
> 常见的字符序列化方案
> 
> + JSON：Web 应用常用
> + XML
>
> **二进制序列化**：使用 **位序列** 表示内存中的对象
> 
> + Protocol Buffer
> + MessagePack
> + Gob 只适用于 Go 语言
> + pickle 只适用于 Python 语言

## Json

JSON (JavaScript Object Notation) 是一种 **_轻量级数据交换格式_**。人类很容易读和写。机器很容易解析和生成。它基于 JavaScript 编程语言标准 `ECMA-262` 第3版(1999年12月)的一个子集，采用 _**完全独立于编程语言**_ 的文本格式来存储和表示内存中的数据。应该说，目前 JSON 得到几乎所有浏览器的支持。查看 https://www.json.org/json-zh.html 了解更多内容


 > [!important] Json 是一种字符序列化协议
 > Json 将内存中的数据结构转变为一种字符串表示形式，这个字符串称为 **Json 字符串**
 > 
 > Json 字符串中不同的字符串会代表不同的类型值
 
### Json 的数据类型

Json 字符串中，使用不同形式字符串代表 **数据** 和 **数据类型**。这些数据类型是 JavaScript 语言中的基本类型

**字符串**：由 **双引号** 包围起来的任意 **字符的序列**，可以有转义字符。例如，`"hello, world"`，注意 `"` 是 Json 字符串的一部分，不是一个界定符

**数值**：整数和浮点数字面值

**对象**：使用 `{"key": value}` 代表类型，`"key"` 必须是一个字符串，需要双引号包围这个字符串。 `value` 可以是任意合法的值

**数组**：值的序列，表示为 `[value, ...., value]`

**布尔类型**：只有两个值 `true` 和 `false`

**空类型**：只有一个值 `null`

如下是一个 Json 字符串的示例

```json
{
	// 字符串: 数组
	"person": [
		{
			"name": "tom",
			"age": 18
		}, // 对象
		{
			"name": "jerry",
			"age": 16
		}
	],
	// 字符串: 数值
	"total": 2
}  // 对象
```

> [!important] 特别注意：**JSON是字符串，是文本**。JavaScript 引擎可以将这种字符串解析为某类型的数据

### `encoding/json`：基本用法

Go标准库中提供了 `encoding/json` 包，内部使用了 [[godev/笔记/反射]] 技术，效率较为低下。参看 https://go.dev/blog/json

+ `json.Marshal(v any) ([]byte, error)`，将 `v` 序列化成 **字符序列**（本质上也是 **字节序列**），这个过程称为 `Encode`

+ `json.Unmarshal(data []byte, v any) error`，将字符序列 `data` 反序列化为 `v`，这个过程称为 `Decode`
	+ `v` 通常是指针类型，因为反序列化时需要修改对象的内容

标准包 `json` 将 Go 数据对象转变为 Json 字符串。将 Go 中的类型数据序列化为 Json 字符串时，Go 的类型转变为 Json 中类型的对应关系

|                               Go                               |       JSON       |
| :------------------------------------------------------------: | :--------------: |
|                         `map` `struct`                         |     `object`     |
|                        `array`或`slice`                         |     `array`      |
|                            `string`                            |     `string`     |
| `int` `int8/16/32/64` `uint` <br>`uint8/16/32/64` `float32/64` |     `number`     |
|                        `true` 或 `false`                        | `true` 或 `false` |
|                             `nil`                              |      `null`      |

将 Json 字符串反序列化为 Go 中对象时，可能会出现类型丢失的情况

> [!tip] `number` 反序列化为 Go 中 `float64` 的对象

#### 基本类型的序列化和反序列化

```go
package main

import (
	"encoding/json"
	"fmt"
)

func main() {
	var data = []any{
		100, 20.5, true, false, nil, "hllo, world!", // 基本类型
		[3]int{97, 98, 99},                  // Go array => JavaScript array
		[]int{97, 98, 99},                   // Go slice => JavaScript array
		map[string]int{"abc": 49, "aa": 50}, // Go map => JavaScript object
	}

	//存储序列化的结果
	var target = make([][]byte, 0, len(data))

	// 序列化
	for i, v := range data {
		b, err := json.Marshal(v)  // 序列化
		if err != nil {
			continue
		}
		fmt.Printf("%d %T: %[2]v => %T %#[3]v %q\n", i, v, b, string(b))
		target = append(target, b)
	}
	fmt.Print(target)
	fmt.Println("\n==========================================")
	// 反序列化
	for i, v := range target {
		var t any
		err := json.Unmarshal(v, &t) // 反序列
		if err != nil {
			continue
		}
		fmt.Printf("%d %T: %#[2]v => %T %[3]v\n", i, v, t)
	}
	fmt.Println()
}
```

**_序列化的结果_**：

```
0 int: 100 => []uint8 []byte{0x31, 0x30, 0x30} 100
1 float64: 20.5 => []uint8 []byte{0x32, 0x30, 0x2e, 0x35} 20.5
2 bool: true => []uint8 []byte{0x74, 0x72, 0x75, 0x65} true
3 bool: false => []uint8 []byte{0x66, 0x61, 0x6c, 0x73, 0x65} false
4 <nil>: <nil> => []uint8 []byte{0x6e, 0x75, 0x6c, 0x6c} null
5 string: hllo, world! => []uint8 []byte{0x22, 0x68, 0x6c, 0x6c, 0x6f, 0x2c, 0x20, 0x77, 0x6f, 0x72, 0x6c, 0x64, 0x21, 0x22} "hllo, world!"
6 [3]int: [97 98 99] => []uint8 []byte{0x5b, 0x39, 0x37, 0x2c, 0x39, 0x38, 0x2c, 0x39, 0x39, 0x5d} [97,98,99]
7 []int: [97 98 99] => []uint8 []byte{0x5b, 0x39, 0x37, 0x2c, 0x39, 0x38, 0x2c, 0x39, 0x39, 0x5d} [97,98,99]
8 map[string]int: map[aa:50 abc:49] => []uint8 []byte{0x7b, 0x22, 0x61, 0x61, 0x22, 0x3a, 0x35, 0x30, 0x2c, 0x22, 0x61, 0x62, 0x63, 0x22, 0x3a, 0x34, 0x39, 0x7d} {"aa":50,"abc":49}
[[49 48 48] [50 48 46 53] [116 114 117 101] [102 97 108 115 101] [110 117 108 108] [34 104 108 108 111 44 32 119 111 114 108 100 33 34] [91 57 55 44 57 56 44 57 57 93] [91 57 55 44 57 56 44 57 57 93] [123 34 97 97 34 58 53 48 44 34 97 98 99 34 58 52 57 125]]
```

以上是序列化结果，说明 **各种类型 _数据_ 被序列化成了 _字节序列_，这些字节表示的是 _字符_，也可以说转换成了 _字符串_**。转换到这里就行了，下面的事是把字符串交给 JavaScript 引擎。

**特别注意**，转换的结果都是字符串，但是这些字符串一旦交给 JavaScript 引擎，它能把它们转换成对应的数据类型

**_反序列化的结果_**：

```
==========================================
0 []uint8: []byte{0x31, 0x30, 0x30} => float64 100
1 []uint8: []byte{0x32, 0x30, 0x2e, 0x35} => float64 20.5
2 []uint8: []byte{0x74, 0x72, 0x75, 0x65} => bool true
3 []uint8: []byte{0x66, 0x61, 0x6c, 0x73, 0x65} => bool false
4 []uint8: []byte{0x6e, 0x75, 0x6c, 0x6c} => <nil> <nil>
5 []uint8: []byte{0x22, 0x68, 0x6c, 0x6c, 0x6f, 0x2c, 0x20, 0x77, 0x6f, 0x72, 0x6c, 0x64, 0x21, 0x22} => string hllo, world!
6 []uint8: []byte{0x5b, 0x39, 0x37, 0x2c, 0x39, 0x38, 0x2c, 0x39, 0x39, 0x5d} => []interface {} [97 98 99]
7 []uint8: []byte{0x5b, 0x39, 0x37, 0x2c, 0x39, 0x38, 0x2c, 0x39, 0x39, 0x5d} => []interface {} [97 98 99]
8 []uint8: []byte{0x7b, 0x22, 0x61, 0x61, 0x22, 0x3a, 0x35, 0x30, 0x2c, 0x22, 0x61, 0x62, 0x63, 0x22, 0x3a, 0x34, 0x39, 0x7d} => map[string]interface {} map[aa:50 abc:49]
```

以上是反序列化结果，从字符串（字节序列）反序列化为Go的某类型数据。因为从浏览器发来的数据都是字符串

需要注意的是，JSON字符串中，**_数值被转换成了 Go 的 float64 类型_**；`true`、`false` 转成了 `bool` 型；`null` 转成了 `nil`；字符串转成了 `string`；数组转成了 `[]interface{}`


> [!important] 理解 
> + Go 中的不同类型的数据，序列化之后的 Json 字符串表示
> 	+ Go 的数值类型(`int` `uint` `float32` `float64` 等) ==> Json 的 `number` 形式的字符串
> 	+ Go 的字符串类型(`string`) ==> Json 的字符串形式，`"...."`，符号 `"` 仅仅是字符而不是界定符
> 	+ Go 的数组和切片(`array` `slice`) ==> Json 的数组形式字符串
> 	+ Go 的 `map`  ==> Json 的 `object` 形式
> 	+ Go 中的 `true` `false` ==> Json 的 `true` `false`
> 	+ Go 中的 `nil` ==> Json 的 `null`
> + Json 字符串表示反序列化，不同形式的 Json 字符串对应 Go 中的类型
> 	+  Json 的 `number` 形式的字符串 ==> Go 中 `float64`
> 	+ Json 的字符串形式，`"...."` ==> Go 中的 `string`
> 	+ Json 的数组形式字符串 ==> Go 中的切片
> 	+ Json 的 `object` 形式 ==> Go 中的 `map` `strut` 
> 	+ Json 的 `true` 和 `false` ==> Go 中的 `bool` 类型的值 `true` 和 `false`
> 	+ Json 的 `null` ==> Go 中的 `nil`

#### 结构体序列化和反序列化

`json` 包将结构体序列化为 Json 中的对象

```go
package main

import (
	"encoding/json"
	"fmt"
)

type Person struct {
	Name string
	Age  int
}

func main() {

	var tom1 = Person{Name: "tom", Age: 20}

	// 序列化
	b, err := json.Marshal(tom1)

	if err != nil {
		panic(err)
	}

	fmt.Printf("%T %#[1]v; %T %#[2]v\n", tom1, b)
	fmt.Println(string(b)) // 丢失了类型，但是保留了基本的数据

	fmt.Println("---------------------------------------------------")

	// 反序列化
	jsonString := `{"Name":   "tom","Age":20}`

	// 1. 知晓目标类型
	var tom2 Person
	err = json.Unmarshal([]byte(jsonString), &tom2) // 需要修改 tom 的内容，请传递指针
	if err != nil {
		panic(err)
	}

	fmt.Printf("%T %#[1]v\n", tom2)

	fmt.Println("=======================================================")

	// 2. 不知晓目标类型

	var i interface{}
	err = json.Unmarshal([]byte(jsonString), &i)
	if err != nil {
		panic(err)
	}

	fmt.Printf("%T %#[1]v\n", i)

}
```

这段代码的输出结构为

```
// 序列化
main.Person main.Person{Name:"tom", Age:20}; []uint8 []byte{0x7b, 0x22, 0x4e, 0x61, 0x6d, 0x65, 0x22, 0x3a, 0x22, 0x74, 0x6f, 0x6d, 0x22, 0x2c, 0x22, 0x41, 0x67, 0x65, 0x22, 0x3a, 0x32, 0x30, 0x7d}

{"Name":"tom","Age":20}

---------------------------------------------------
// 知晓目标类型
main.Person main.Person{Name:"tom", Age:20}

=======================================================

// 不知晓目标类型
map[string]interface {} map[string]interface {}{"Age":20, "Name":"tom"}
```

> [!important] `json` 包将 Json 字符串中表示 **对象** 的字符串反序列化为 `map[string]interface{}` 类型

#### 结构体标签

`Tag` 是 **结构体的元信息**，可以在运行的时候通过 [[godev/笔记/反射]] 的机制读取出来。 `Tag` 在 **结构体字段** 的后方定义，由一对 **反引号** 包裹起来，具体的格式如下：

```go
`key1:"value1" key2:"value2"`
```

结构体 `tag` 由一个或多个键值对组成。**_键与值使用冒号分隔_**，**_值用双引号括起来_**。同一个结构体字段可以设置多个键值对 `tag`，**_不同的键值对之间使用空格分隔_**。

> [!warning] 注意
> 为结构体编写 `Tag` 时，必须严格遵守键值对的规则。结构体标签的解析代码的容错能力很差，一旦格式写错，编译和运行时都不会提示任何错误，通过反射也无法正确取值。
> 
> 例如，**_不要在 `key` 和 `value` 之间添加空格_**

例如我们为 `Person` 结构体的每个字段定义 `json` 序列化时使用的 `Tag`

```go
package main

import (
	"encoding/json"
	"fmt"
)

type Person struct {
	Name string `json:"name"` // 通过指定 tag 实现 json 序列化该字段时的 key
	Age  int    `json:"age"`
}

func main() {

	var tom1 = Person{Name: "tom", Age: 20}

	// 序列化
	b, err := json.Marshal(tom1)

	if err != nil {
		panic(err)
	}

	fmt.Printf("%T %#[1]v; %T %#[2]v\n", tom1, b)
	fmt.Println(string(b)) // 丢失了类型，但是保留了基本的数据

	fmt.Println("---------------------------------------------------")

	// 反序列化
	jsonString := `{"Name":   "tom","Age":20}`

	// 1. 知晓目标类型
	var tom2 Person
	err = json.Unmarshal([]byte(jsonString), &tom2) // 需要修改 tom 的内容，请传递指针
	if err != nil {
		panic(err)
	}

	fmt.Printf("%T %#[1]v\n", tom2)

	fmt.Println("=======================================================")

	// 2. 不知晓目标类型

	var i interface{}
	err = json.Unmarshal([]byte(jsonString), &i)
	if err != nil {
		panic(err)
	}

	fmt.Printf("%T %#[1]v\n", i)

}
```

> [!tip] 忽略字段
>
> 值 `omitempty`  表示 **序列化时忽略**，即该字段不序列化
> 
>```go
> Name string `json:"name,omitempty"` 
> ```
> 
> 值 `-` 表示 **忽略该字段**，即该字段即不被序列化也不被反序列
> 
> ```go
> Name string `json:"-"`
> ```


**Json 序列化的 Go 实现效率较低**，由此社区和某些公司提供大量开源的实现，例如 `easyjson`、`jsoniter`、`sonic` 等

### `encoding/json`：`Decoder` 和 `Encoder`

#### `Decoder` 解码器：执行反序列化任务

**`Decoder` 从 _输入流_ 读取 和 解码  JSON 字符串** 。`json.NewDecoder(r)` 返回一个从 `r` 读取的新解码器。

```go
func NewDecoder(r io.Reader) *Decoder
```

解码器引入了自己的缓冲，并可能从 `r` 中读取超出请求的 JSON 值的数据

#### `Encoder` 编码器：执行序列化任务

**`Encoder` 将 JSON 字符串写入 _输出流_**。`json.NewEncoder(w)` 返回一个写入 `w` 的新编码器

```go
func NewEncoder(w io.Writer) *Encoder
```

#### 示例

```go
package main

import (
	"encoding/json"
	"fmt"
	"io"
	"log"
	"os"
	"strings"
)

func main() {
	const jsonStream = `
	{"Name": "Ed", "Text": "Knock knock."}
	{"Name": "Sam", "Text": "Who's there?"}
	{"Name": "Ed", "Text": "Go fmt."}
	{"Name": "Sam", "Text": "Go fmt who?"}
	{"Name": "Ed", "Text": "Go fmt yourself!"}
`
	type Message struct {
		Name, Text string
	}

	// Decoder
	dec := json.NewDecoder(strings.NewReader(jsonStream))

	var messages []Message

	for {
		var m Message
		if err := dec.Decode(&m); err == io.EOF {
			break
		} else if err != nil {
			log.Fatal(err)
		}

		messages = append(messages, m)
	}

	fmt.Printf("%#v\n", messages)

	fmt.Println("========================================")

	// Encoder
	enc := json.NewEncoder(os.Stdout)
	enc.Encode(messages)
}
```

## MessagePack 

Json 序列化的占用的空间较多的。例如，序列化一个整数 `97`，Json 将 `97` 转换为字符串的 `97`，占用两个字节；然而，整数 `97` 只用一个字节就能表示

采用二进制序列化，可以极大程度的节省空间。[MessagePack](https://msgpack.org/) 是一个基于 **二进制** 高效的对象序列化类库，可用于跨语言通信。 它可以像 Json 那样，在许多种语言之间交换结构对象。但是它比JSON更快速也更轻巧。 支持 Python、Ruby、Java、C/C++、Go等众多语言。宣称比 Google Protocol Buffers还要快 $4$ 倍

> [!important] **MessagePack 的数据格式与 Json 类似**，但是在存储时对数字、多字节字符、数组等都做了很多优化
> 减少了无用的字符，二进制格式，也保证不用字符化带来额外的存储空间的增加

下图对比了 Json 和 MessagePack 序列化后占用的内存空间

![[Pasted image 20250408214700.png]]

### 压缩原理

MessagePack 的压缩原理，详细内容参考 [MessagePack 规范](https://github.com/msgpack/msgpack/blob/master/spec.md)

+ `true` `false` 采用 $1$ 个字节表示，（`0xc3` 表示 `true`，`0xc2` 表示 `false`）
+ **定长对象**：就是 **数字** 之类的，他们天然是定长的，是用一个字节表示后面的内容是什么
	+ `0xcc` 表示这后面的内容是个 `uint8`
	+ `oxcd` 表示后面的内容是个 `uint16`
	+ `0xca` 表示后面的是个 `float32`
	+ 对于数字做了进一步的压缩处理，根据大小选择用更少的字节进行存储，比如 `<256` 的 `int`，完全可以用一个字节表示
+ **不定长对象**：比如 _字符串_、_数组_、_二进制数据_，类型后面加 `1~4`个字节，用来存长度
	+ 比如字符串长度是 $256$ 以内的，只需要 $1$ 个字节
	+ MessagePack 能存的最长的字符串，是($2^{32} -1$ ) 最长的 `4G` 的字符串大小
+ **高级数据结构**：`map` ，就是 `k-v` 结构的数据，和数组差不多，加 `1~4` 个字节表示后面有多少个项
+ **Ext结构**：表示特定的小单元数据。也就是 **用户自定义数据结构**

Go 的 MessagePack 协议的实现有 $4$ 个
+ **推荐**：`github.com/vmihailenco/msgpack`： https://msgpack.uptrace.dev/ 提供了与 Json 兼容的接口

+ `github.com/ugorji/go/codec`: https://github.com/ugorji/go

+ `github.com/tinylib/msgp`:  https://github.com/tinylib/msgp

+ `github.com/shamaton/msgpack`: https://github.com/shamaton/msgpack

### 安装

```shell
go get github.com/vmihailenco/msgpack/v5
```

### 快速入门

基本使用方法和 `json` 包类似

```go
package main

import (
	"fmt"

	"github.com/vmihailenco/msgpack/v5"
)

type Person struct {
	Name string `msgpack:"name"`
	Age  int    `msgpack:"age"`
}

func main() {

	// 序列化
	var data = []Person{
		{Name: "Tom", Age: 16},
		{Name: "Jerry", Age: 32},
	}

	b, err := msgpack.Marshal(data)
	if err != nil {
		panic(err)
	}

	fmt.Println(b, len(b), string(b)) // 二进制

	// 反序列化：知道类型
	var result []Person

	err = msgpack.Unmarshal(b, &result)

	if err != nil {
		fmt.Println(err)
		return
	}

	fmt.Printf("%T: %#[1]v\n", result)
}
```

## Base64 编码

Base64是一种基于 $64$ 个 **可打印字符来表示二进制数据** 的表示方法

$\log_2 64 = 6$  所以 $6$ **个比特为一个单元**，对应某个可打印字符。$3$ 个字节相当于 $24$ 个比特，对应于 $4$ 个 Base64 单元，即 **$3$ 个字节可由 $4$ 个可打印字符来表示**

在 Base64 中的可打印字符包括 **字母`A-Z`、`a-z`、数字 `0-9`**，这样共有 $62$ 个字符；此外两个可打印符号在不同的系统中而不同，通常是 `+` 和 `/`

> [!tip] Base64 编码：使用可打印字符表示二进制数据
> 
> 可打印字符包括 `A-Z a-z 0-9` 共 $62$ 个字符，以及 `+ /` 两个字符；共计 $64$ 个字符
> 
> 主要应用在 `JWT`、网页图片传输等
> 

### RFC 4648 标准的 Base64 索引表
| 索引  |   二进制    | 字符  |     | 索引  |   二进制    | 字符  |     |
| :-: | :------: | :-: | :-: | :-: | :------: | :-: | :-: |
|  0  | `000000` |  A  |     | 32  | `100000` |  g  |     |
|  1  | `000001` |  B  |     | 33  | `100001` |  h  |     |
|  2  | `000010` |  C  |     | 34  | `100010` |  i  |     |
|  3  | `000011` |  D  |     | 35  | `100011` |  j  |     |
|  4  | `000100` |  E  |     | 36  | `100100` |  k  |     |
|  5  | `000101` |  F  |     | 37  | `100101` |  l  |     |
|  6  | `000110` |  G  |     | 38  | `100110` |  m  |     |
|  7  | `000111` |  H  |     | 39  | `100111` |  n  |     |
|  8  | `001000` |  I  |     | 40  | `101000` |  o  |     |
|  9  | `001001` |  J  |     | 41  | `101001` |  p  |     |
| 10  | `001010` |  K  |     | 42  | `101010` |  q  |     |
| 11  | `001011` |  L  |     | 43  | `101011` |  r  |     |
| 12  | `001100` |  M  |     | 44  | `101100` |  s  |     |
| 13  | `001101` |  N  |     | 45  | `101101` |  t  |     |
| 14  | `001110` |  O  |     | 46  | `101110` |  u  |     |
| 15  | `001111` |  P  |     | 47  | `101111` |  v  |     |
| 16  | `010000` |  Q  |     | 48  | `110000` |  w  |     |
| 17  | `010001` |  R  |     | 49  | `110001` |  x  |     |
| 18  | `010010` |  S  |     | 50  | `110010` |  y  |     |
| 19  | `010011` |  T  |     | 51  | `110011` |  z  |     |
| 20  | `010100` |  U  |     | 52  | `110100` |  0  |     |
| 21  | `010101` |  V  |     | 53  | `110101` |  1  |     |
| 22  | `010110` |  W  |     | 54  | `110110` |  2  |     |
| 23  | `010111` |  X  |     | 55  | `110111` |  3  |     |
| 24  | `011000` |  Y  |     | 56  | `111000` |  4  |     |
| 25  | `011001` |  Z  |     | 57  | `111001` |  5  |     |
| 26  | `011010` |  a  |     | 58  | `111010` |  6  |     |
| 27  | `011011` |  b  |     | 59  | `111011` |  7  |     |
| 28  | `011100` |  c  |     | 60  | `111100` |  8  |     |
| 29  | `011101` |  d  |     | 61  | `111101` |  9  |     |
| 30  | `011110` |  e  |     | 62  | `111110` |  +  |     |
| 31  | `011111` |  f  |     | 63  | `111111` |  /  |     |
| 填充  |          |  =  |     |     |          |     |     |

### Base64 编码原理

举例来说，一段引用自托马斯·霍布斯《利维坦》的文句

```
Man is distinguished, not only by his reason, but by this singular passion from other animals, which is a lust of the mind, that by a perseverance of delight in the continued and indefatigable generation of knowledge, exceeds the short vehemence of any carnal pleasure.
```

经过 Base64 编码之后变成：

```
TWFuIGlzIGRpc3Rpbmd1aXNoZWQsIG5vdCBvbmx5IGJ5IGhpcyByZWFzb24sIGJ1dCBieSB0aGlzIHNpbmd1bGFyIHBhc3Npb24gZnJvbSBvdGhlciBhbmltYWxzLCB3aGljaCBpcyBhIGx1c3Qgb2YgdGhlIG1pbmQsIHRoYXQgYnkgYSBwZXJzZXZlcmFuY2Ugb2YgZGVsaWdodCBpbiB0aGUgY29udGludWVkIGFuZCBpbmRlZmF0aWdhYmxlIGdlbmVyYXRpb24gb2Yga25vd2xlZGdlLCBleGNlZWRzIHRoZSBzaG9ydCB2ZWhlbWVuY2Ugb2YgYW55IGNhcm5hbCBwbGVhc3VyZS4=
```

 `"Man"` 的编码结果为`"TWFu"`，详细原理如下

![[Pasted image 20240527193046.png]]

**Base64 算法 _将 3 个字节编码为 4 个字符_**


如果要编码的 **字节数不能被 3 整除**，最后会 **多出 1 个或 2 个字节**，那么可以使用下面的方法进行处理
+ 首先，使用整数 $0$  在 **末尾补齐** ，使其能够被 $3$ 整除
+ 然后，再进行 Base64 的编码

在编码后的 Base64 文本后加上一个或两个`=`号，代表补足的字节数。也就是说，当剩余 $1$ 字节就需要补 $2$ 个字节；当剩余 $2$ 各字节就需要补 $1$ 各字节

> [!important] 使用 $0$ 进行补齐

![[Pasted image 20240527193447.png]]

### 编码实现

编码过程就是将 $3$ 个任意字节数据编码为 $4$ 个字符，每个字节的最高 $2$ 位不用了，只用 $6$ 位，而 $6$ 位的变化只有 $64$ 种，如上图，利用上图查表对应就得出编码了。字节数不是 $3$ 的倍数会补齐

```go
package main

import (
	"encoding/base64"
	"fmt"
)

func encodeTabel() []byte {
	var table []byte

	for c := 0x41; c < 0x41+26; c++ {
		table = append(table, byte(c))
	}

	for c := 0x61; c < 0x61+26; c++ {
		table = append(table, byte(c))
	}

	for c := 0x30; c < 0x30+10; c++ {
		table = append(table, byte(c))
	}

	table = append(table, '+', '/')

	return table
}

// Base64Encode Base64编码实现
func Base64Encode(src []byte) (result []byte) {
	table := encodeTabel()

	// 判断 len(b) 能否被 3 整除。
	remander := len(src) % 3
	// 不能被 3 整除，需要要补充 0
	if remander != 0 {
		// remander 表示剩余的字节数
		// 3 - remander 表示补充的 0 的个数
		for i := 0; i < 3-remander; i++ {
			src = append(src, 0)
		}
	}

	// 能被 3 整除，准备编码

	for i := 3; i < len(src)+1; i += 3 {
		// 每次迭代都获取 3 个字节
		// 将这 3 各字节保存到一个整数，方便进行移位运算
		var u uint32
		for i, v := range src[i-3 : i] {
			moveNdigits := 24 - uint32(i+1)*8
			u |= uint32(v) << moveNdigits
		}

		// 开始编码这 3 个字节。编码为 4 各字符(4 个字节)
		for j := 0; j < 4; j++ {
			// 移动的位数
			moveNdigits := 24 - (j+1)*6
			// 从左到右 每次迭代获取 6 个位
			index := (u & (0x3f << moveNdigits)) >> moveNdigits
			// 最后 3 字节需要特殊处理
			// 条件：remander != 0 有添加
			// 条件: i == len(src) 处理最后 3 个字节
			// 条件: j >= 2 最多填充 2 个 0
			// 条件: (u&(0x3f<<moveNdigits))>>moveNdigits == 0 当前处理的是补充的 0
			if remander != 0 && i == len(src) && j >= 2 && (u&(0x3f<<moveNdigits))>>moveNdigits == 0 {
				result = append(result, '=')
			} else {
				result = append(result, table[index])
			}
		}
	}

	return
}

func main() {
	fmt.Println("================Base64Encode================")
	fmt.Printf("%s\n", Base64Encode([]byte("Man")))
	fmt.Printf("%s\n", Base64Encode([]byte("M")))
	fmt.Printf("%s\n", Base64Encode([]byte("Ma")))
	fmt.Println("================encoding/base64================")
	fmt.Printf("%s\n", base64.StdEncoding.EncodeToString([]byte("Man")))
	fmt.Printf("%s\n", base64.StdEncoding.EncodeToString([]byte("M")))
	fmt.Printf("%s\n", base64.StdEncoding.EncodeToString([]byte("Ma")))
}
```

这段代码的输出是

```shell
09:15:31 dyp@dyp-PC gocode → go build .
09:15:32 dyp@dyp-PC gocode → ./gocode 
================Base64Encode================
TWFu
TQ==
TWE=
================encoding/base64================
TWFu
TQ==
TWE=
```

### 解码实现

解码过程是对 $4$ 每个字节去掉最高 $2$ 位然后合并为 $3$ 个字节

解码过程就是每次获取 Base64 编码的 $4$ 各字节，去掉最高两个字节，然后合并为 $3$ 个字节

```go
package main

import (
	"encoding/base64"
	"fmt"
)

func encodeTabel() []byte {
	var table []byte

	for c := 0x41; c < 0x41+26; c++ {
		table = append(table, byte(c))
	}

	for c := 0x61; c < 0x61+26; c++ {
		table = append(table, byte(c))
	}

	for c := 0x30; c < 0x30+10; c++ {
		table = append(table, byte(c))
	}

	table = append(table, '+', '/')

	return table
}

func decodeTable() map[byte]uint8 {
	var result = make(map[byte]uint8, 64)
	for i, v := range encodeTabel() {
		result[v] = uint8(i)
	}
	return result
}

// Base64Decode Base64解码实现
func Base64Decode(src []byte) (result []byte, err error) {

	if len(src)%4 != 0 {
		return nil, fmt.Errorf("%s 不是 base64 编码的字符串", src)
	}

	// 解码表
	table := decodeTable()

	/*
	   编码  索引
	   TWFu 19  22 5 46
	   T --> 0001 0011
	   W --> 0001 0110
	   F --> 0000 0101
	   u --> 0010 1110
	   --> 0100 1101 0110 0001 0110 1110
	   --> 0x4d616e
	*/

	// 解码
	for i := 4; i < len(src)+1; i += 4 {

		// 每次迭代取 4 个字节、
		// 查询出每个字节编码的字符对应的索引
		// 将这些索引拼接到一个 整数中方便移位
		var u uint32
		for index, char := range src[i-4 : i] {

			moveNdigits := 24 - (index+1)*6

			if char == '=' {
				continue
			}

			value, ok := table[byte(char)]
			if !ok {
				return nil, fmt.Errorf("%c 不是 base64 编码的基本字符", byte(char))
			}

			// 提取 value 的低 6 位
			u |= (uint32(value) & 0x3f) << moveNdigits
		}
		// fmt.Printf("%#x\n", u)

		// 从左到右，依次从 u 中提取 8 位
		for j := 0; j < 3; j++ {
			moveNdigits := 24 - (j+1)*8

			result = append(result, byte((u&(0xff<<moveNdigits))>>uint32(moveNdigits)))
		}
	}

	return result, nil
}

func main() {

	fmt.Println("=---------------=Base64Decode=---------------=")
	result, _ := Base64Decode([]byte("TWFu"))
	fmt.Printf("%s\n", result)
	result, _ = Base64Decode([]byte("TQ=="))
	fmt.Printf("%s\n", result)
	result, _ = Base64Decode([]byte("TWE="))
	fmt.Printf("%s\n", result)

	fmt.Println("=---------------=encoding/base64=---------------=")
	result, _ = base64.StdEncoding.DecodeString("TWFu")
	fmt.Printf("%s\n", result)
	result, _ = base64.StdEncoding.DecodeString("TQ==")
	fmt.Printf("%s\n", result)
	result, _ = base64.StdEncoding.DecodeString("TWE=")
	fmt.Printf("%s\n", result)
}
```

这段代码的输出为

```shell
10:23:18 dyp@dyp-PC gocode → go build
10:23:22 dyp@dyp-PC gocode → ./gocode 
=---------------=Base64Decode=---------------=
Man
M
Ma
=---------------=encoding/base64=---------------=
Man
M
Ma
```
