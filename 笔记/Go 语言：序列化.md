# 序列化

数据在内存中通常会存储为各种各样的结构，例如 `map` `array` 和 `struct`。由于内存是易失性存储，这些数据在断电后就全部消失了。因此，需要将内存中带有结构的数据保存在文件中，当再次启动计算机时方便从文件中将数据读取到内存并还原会原始结构

我们知道文件是一种顺序结构，保存的是字节序列。也就是说，内存中的数据要保存在文件中时需要转换为字节序列；当从文件中读取数据时，需要还原为原始结构。为了保证信息不丢失，不仅要存储内存中的数据，还要存储这些数据在内存中的结构信息

> [!attention] 
> 
> 也就是说，将内存中的数据保存到文件中时，需要保存 **类型信息** 以及 **数据本身**
> 

也就是说，要设计一套 **协议**，按照某种规则，把内存中数据保存到文件中。文件是一个字节序列，所以必须把数据转换成 **字节序列**，输出到文件。这就是 **序列化**。

> [!tip] 
> 
> **serialization** 序列化：将内存中对象存储下来，把它变成一个个字节。将内存对象按照某种协议存储到文件中。需要保存
> - 对象的 **类型**
> - 对象的 **数据**
> - 数据间的 **间隔**
> 

反之，从文件的字节序列恢复到内存并且还是原来的类型，就是 **反序列化**。

> [!tip] 
> 
> **deserialization** 反序列：将文件中的一个个字节恢复成内存中的对象
> 

> [!question] 
> 
> 序列化协议需要解决三个问题
> + 对象的 **类型** 如何表示？
> + 对象的 **数据** 如何存储
> + 数据之间的 **间隔** 如何表示？
> 

序列化之后可以将字节序列保存为文件(持久化)或通过网络传输。目前，有两种方法将对象转变为字节序列

> [!tip] 序列化的两种方案
> 
> **字符序列化**：使用 **字符序列** 表示内存中的对象，只是方便了人类阅读。本质也是字节序列，这些字节序列代表了某个字符。常见的字符序列化方案
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
> 

## Json

JSON (JavaScript Object Notation) 是一种 **轻量级数据交换格式**。人类很容易读和写。机器很容易解析和生成。它基于 JavaScript 编程语言标准 `ECMA-262` 第3版(1999年12月)的一个子集，采用 **完全独立于编程语言** 的文本格式来存储和表示内存中的数据。应该说，目前 JSON 得到几乎所有浏览器的支持。查看 https://www.json.org/json-zh.html 了解更多内容


 > [!hint] 
 > 
 > Json 将内存中的数据结构转变为一种字符串表示形式，这个字符串称为 **Json 字符串**
 > 
 > Json 字符串中不同的字符串会代表不同的类型值
 > 
 
### Json 的数据类型

Json 字符串中，使用不同形式字符串代表 **数据** 和 **数据类型**。这些数据类型是 JavaScript 语言中的基本类型。下表列出了 JSON 字符串中不同的字符串代表的数据类型

| 类型      | 描述                                  | 示例               |
| :------ | ----------------------------------- | ---------------- |
| string  | 由 **双引号** 包围起来的任意 **字符的序列**，可以有转义字符 | `"hello, world"` |
| number  | 包括 `int` 和 `float`，就是数字字面值          | `1024` `3.14`    |
| object  | 键值对 `{"key": value}`                | `{"name": "张三"}` |
| array   | 数组，表示为 `[value, ..., value]`        | `[1, 2, 3, 4]`   |
| Boolean | 布尔类型                                | `true` 或 `false` |
| null    | null 类型，只有一个值 `null`                | `null`           |


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

> [!important] 
> 
> 特别注意：**JSON是字符串，是文本**。JavaScript 引擎可以将这种字符串解析为某类型的数据
> 

### `encoding/json` 包

Go 的标准包 `encoding/json` 提供了 JSON 的官方支持，其内部使用了 [[Go 语言：反射]] 技术，效率较为低下。用于支持 JSON 序列化和反序列化的两个函数为 `json.Marshal()` 和 `json.Unmarshal()` 它们的函数签名如下

```go
// Marshal 将 v 序列号为 json 字符串，并以 []byte 类型对象返回
func Marshal(v any) ([]byte, error)

// Unmarshal 将 data 反序列化为 Go 语言中对象，并存储在 v 中
func Unmarshal(data []byte, v any) error
```


标准包 `json` 将 Go 语言的数据对象转变为 JSON 字符串；或者，将 Go 中的类型数据序列化为 JSON 字符串时，它们之间的类型对应关系如下表

| Go                                                             | JSON             |
| :------------------------------------------------------------- | :--------------- |
| `map` `struct`                                                 | `object`         |
| `array`或`slice`                                                | `array`          |
| `string`                                                       | `string`         |
| `int` `int8/16/32/64` `uint` <br>`uint8/16/32/64` `float32/64` | `number`         |
| `true` 或 `false`                                               | `true` 或 `false` |
| `nil`                                                          | `null`           |

> [!hint] 
> 
> 将 JSON 字符串反序列化为 Go 中的数据对象时，可能会出现类型丢失的情况。例如，
> 
> + JSON 字符串中的 `number` 反序列化为 Go 中 `float64` 的对象
> 

#### 基本类型

下列代码演示了如何将 Go 语言中的基本数据类型对象序列化为 JSON 字符串

```go
package main

import (
	"encoding/json"
	"fmt"
)

func main() {
	// 内建类型的数据
	var data = []any{
		100,                                 //
		20.5,                                // Go int float => JSON number
		true,                                //
		false,                               // Go bool => JSON Boolean
		nil,                                 // Go nil => JSON null
		"hllo, world!",                      // Go string => JSON string
		[3]int{97, 98, 99},                  // Go array => JavaScript array
		[]int{97, 98, 99},                   // Go slice => JavaScript array
		map[string]int{"abc": 49, "aa": 50}, // Go map => JavaScript object
	}

	//存储序列化的结果：每个数据都会返回一个 []byte 切片
	var target = make([][]byte, 0, len(data))

	// 序列化
	for _, v := range data {
		b, err := json.Marshal(v) // 序列化
		if err != nil {
			continue
		}
		target = append(target, b)
	}
	fmt.Printf("%q\n", target)
}
```

执行上述代码的输出结果为

```
["100" "20.5" "true" "false" "null" "\"hllo, world!\"" "[97,98,99]" "[97,98,99]" "{\"aa\":50,\"abc\":49}"]
```

> [!tip] 
> 
> 将 Go 字符串反序列会 JSON 字符串时，双引号 `"`  是 JSON 字符串的一部分，而不是字符串界定符 
> 

下面演示了反序列化

```go
package main

import (
	"encoding/json"
	"fmt"
)

func main() {
	// 序列化
	...
	// 反序列化
	for i, v := range target {
		var t any
		err := json.Unmarshal(v, &t)
		if err != nil {
			continue
		}
		fmt.Printf("%d %T: %[2]q => %T %[3]v\n", i, v, t)
	}
}
```

上述反序列化输出的结果如下

```
0 []uint8: "100" => float64 100
1 []uint8: "20.5" => float64 20.5
2 []uint8: "true" => bool true
3 []uint8: "false" => bool false
4 []uint8: "null" => <nil> <nil>
5 []uint8: "\"hllo, world!\"" => string hllo, world!
6 []uint8: "[97,98,99]" => []interface {} [97 98 99]
7 []uint8: "[97,98,99]" => []interface {} [97 98 99]
8 []uint8: "{\"aa\":50,\"abc\":49}" => map[string]interface {} map[aa:50 abc:49]
```

> [!attention] 
> 
> 注意到：J
> + JSON 中的 number(`"100"`  `"3.14"`) 在反序列化时全部转换为了 `float64` 类型值
> + JSON 中 的 Boolean`"true"` 和 `"false"`() 被转换为了 `bool` 类型的 `true` 和 `false`
> + JSON 中 `"null"` 被转换为预定义常量 `nil`
> + JSON 中的字符串(`"\"....\""`) 被转换为了 `string` 类型的值
> + JSON 字符串中数组(`[v,...,v]`) 被转换为 `[]any` 切片

> [!important] 
> 
> + Go 中的不同类型的数据，序列化之后的 JSON 字符串表示
> 	+ Go 的数值类型(`int` `uint` `float32` `float64` 等) ==> JSON 的 `number` 形式的字符串
> 	+ Go 的字符串类型(`string`) ==> JSON 的字符串形式，`"...."`，符号 `"` 仅仅是字符而不是界定符
> 	+ Go 的数组和切片(`array` `slice`) ==> JSON 的数组形式字符串
> 	+ Go 的 `map`  ==> JSON 的 `object` 形式
> 	+ Go 中的 `true` `false` ==> JSON 的 `true` `false`
> 	+ Go 中的 `nil` ==> JSON 的 `null`
> + Json 字符串表示反序列化，不同形式的 JSON 字符串对应 Go 中的类型
> 	+  JSON 的 `number` 形式的字符串 ==> Go 中 `float64`
> 	+ JSON 的字符串形式，`"...."` ==> Go 中的 `string`
> 	+ JSON 的数组形式字符串 ==> Go 中的切片
> 	+ JSON 的 `object` 形式 ==> Go 中的 `map` `strut` 
> 	+ JSON 的 `true` 和 `false` ==> Go 中的 `bool` 类型的值 `true` 和 `false`
> 	+ JSON 的 `null` ==> Go 中的 `nil`
> 

#### 结构体

下列代码演示了 `json` 包中将结构体序列化为 JSON 中的对象

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

> [!important] 
> 
> `json` 包将 Json 字符串中表示 **对象** 的字符串反序列化为 `map[string]interface{}` 类型
> 

#### 结构体标签

`Tag` 是 **结构体的元信息**，可以在运行的时候通过 [[Go 语言：反射]] 机制读取出来。 `Tag` 在 **结构体字段** 的后方定义，由一对 **反引号** 包裹起来，具体的格式如下：

```go
`key1:"value1" key2:"value2"`
```

结构体 `Tag` 由一个或多个 **键值对** 组成。**键与值使用冒号分隔**，**值用双引号括起来**。同一个结构体字段可以设置多个键值对 `tag`，**不同的键值对之间使用空格分隔**。

> [!warning] 注意
> 为结构体编写 `Tag` 时，必须严格遵守键值对的规则。结构体标签的解析代码的容错能力很差，一旦格式写错，编译和运行时都不会提示任何错误，通过反射也无法正确取值。
> 
> 尤其注意：**不要在 `key` 和 `value` 之间添加空格**
> 

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
> 值 `-` 表示 **忽略该字段**，即该字段不被序列化也不被反序列
> 
> ```go
> Name string `json:"-"`
> ```


> [!hint] 
> 
> **JSON 序列化的 Go 实现效率较低**，由此社区和某些公司提供大量开源的实现，例如 `easyjson`、`jsoniter`、`sonic` 等
> 

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

![Pasted image 20250408214700|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755786113510-f8ea965424024221b2cc25be77cab3f8.png)

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

Base64 是一种基于 $64$ 个可打印字符来表示二进制数据的方法。由于 $\log_2 64 = 6$ ，所以只需要 $6$ 个比特就可以编码一个可打印字符

> [!important] 
> 
> 在 Base64 编码中，使用 $4$ 个 Base64 字符代表  $3$ 个字节（ $24$ 个比特）组成，即 **$3$ 个字节可由 $4$ 个可打印字符来表示**
> 

在 Base64 中的可打印字符包括字母`A-Z`、`a-z`、数字 `0-9`，这样共有 $62$ 个字符，此外两个可打印符号在不同的系统中而不同

> [!important] 
> 
> Base64 编码的核心就是使用 $4$ 个可打印字符表示 $3$ 个字节
> 

### RFC 4648 标准的 Base64 索引表

| 索引  | 二进制      | 字符  |     | 十进制 | 二进制      | 字符  |     | 十进制 | 二进制      | 字符  |     | 十进制 | 二进制      | 字符  |
| --- | -------- | --- | --- | --- | -------- | --- | --- | --- | -------- | --- | --- | --- | -------- | --- |
| 0   | `000000` | A   |     | 16  | `010000` | Q   |     | 32  | `100000` | g   |     | 48  | `110000` | w   |
| 1   | `000001` | B   |     | 17  | `010001` | R   |     | 33  | `100001` | h   |     | 49  | `110001` | x   |
| 2   | `000010` | C   |     | 18  | `010010` | S   |     | 34  | `100010` | i   |     | 50  | `110010` | y   |
| 3   | `000011` | D   |     | 19  | `010011` | T   |     | 35  | `100011` | j   |     | 51  | `110011` | z   |
| 4   | `000100` | E   |     | 20  | `010100` | U   |     | 36  | `100100` | k   |     | 52  | `110100` | 0   |
| 5   | `000101` | F   |     | 21  | `010101` | V   |     | 37  | `100101` | l   |     | 53  | `110101` | 1   |
| 6   | `000110` | G   |     | 22  | `010110` | W   |     | 38  | `100110` | m   |     | 54  | `110110` | 2   |
| 7   | `000111` | H   |     | 23  | `010111` | X   |     | 39  | `100111` | n   |     | 55  | `110111` | 3   |
| 8   | `001000` | I   |     | 24  | `011000` | Y   |     | 40  | `101000` | o   |     | 56  | `111000` | 4   |
| 9   | `001001` | J   |     | 25  | `011001` | Z   |     | 41  | `101001` | p   |     | 57  | `111001` | 5   |
| 10  | `001010` | K   |     | 26  | `011010` | a   |     | 42  | `101010` | q   |     | 58  | `111010` | 6   |
| 11  | `001011` | L   |     | 27  | `011011` | b   |     | 43  | `101011` | r   |     | 59  | `111011` | 7   |
| 12  | `001100` | M   |     | 28  | `011100` | c   |     | 44  | `101100` | s   |     | 60  | `111100` | 8   |
| 13  | `001101` | N   |     | 29  | `011101` | d   |     | 45  | `101101` | t   |     | 61  | `111101` | 9   |
| 14  | `001110` | O   |     | 30  | `011110` | e   |     | 46  | `101110` | u   |     | 62  | `111110` | +   |
| 15  | `001111` | P   |     | 31  | `011111` | f   |     | 47  | `101111` | v   |     | 63  | `111111` | /   |
| 填充  |          | =   |     |     |          |     |     |     |          |     |     |     |          |     |

### 示例

举例来说，一段引用自托马斯·霍布斯《利维坦》的文句

```
Man is distinguished, not only by his reason, but by this singular passion from other animals, which is a lust of the mind, that by a perseverance of delight in the continued and indefatigable generation of knowledge, exceeds the short vehemence of any carnal pleasure.
```

经过 Base64 编码之后变成：

```base64
TWFuIGlzIGRpc3Rpbmd1aXNoZWQsIG5vdCBvbmx5IGJ5IGhpcyByZWFzb24sIGJ1dCBieSB0aGlzIHNpbmd1bGFyIHBhc3Npb24gZnJvbSBvdGhlciBhbmltYWxzLCB3aGljaCBpcyBhIGx1c3Qgb2YgdGhlIG1pbmQsIHRoYXQgYnkgYSBwZXJzZXZlcmFuY2Ugb2YgZGVsaWdodCBpbiB0aGUgY29udGludWVkIGFuZCBpbmRlZmF0aWdhYmxlIGdlbmVyYXRpb24gb2Yga25vd2xlZGdlLCBleGNlZWRzIHRoZSBzaG9ydCB2ZWhlbWVuY2Ugb2YgYW55IGNhcm5hbCBwbGVhc3VyZS4=
```

编码 `"Man"` 的结果为`"TWFu"`，详细原理如下

![Pasted image 20240527193046|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755786113510-afe5a3909c0b4887a8025355d0509acd.png)

> [!tip] 
> 
> 在此例中，**Base64 算法将 3 个字节编码为 4 个字符**
> 

如果要编码的字节数不能被 $3$ 整除，最后会多出 $1$ 个或 $2$ 个字节，那么可以使用下面的方法进行处理：先使用 $0$ 值在末尾补足，使其能够被 $3$ 整除，然后再进行 Base64 的编码。

在编码后的 Base64 文本后加上一个或两个`=`号，代表补足的字节数。也就是说，当最后剩余两个八位(待补足)字节（`2`个 `bytes`）时，最后一个 6 位的 Base64 字节块有四位是 0 值，最后附加上两个等号；如果最后剩余一个八位(待补足)字节（`1` 个 `byte`）时，最后一个 `6` 位的 `base` 字节块有两位是 `0` 值，最后附加一个等号。 参考下表：

![Pasted image 20240527193447|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755786113510-9d3b163600d74f99a23a6edc2a87f275.png)

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
