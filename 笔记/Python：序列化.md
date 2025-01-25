# 序列化

程序产生的数据在内存被组织为各种各样的数据结构。内存中的数据是临时的易变的，为了持久化存储内存中的数据，应该将内存中的数据保存在磁盘上。

然而，在 [[文件系统]] 和 [[文件系统的实现]] 中介绍过，磁盘上存储的数据逻辑上是顺序存储的，不存在像内存中那样的数据结构。因此，将内存中的数据保存为文件存在一个天然的问题：**如何将一个复杂的数据结构存储在文件中**？当将复杂数据结构存储在文件中后，又应该 **如何将磁盘中的数据还原为内存中的数据结构？**

> [!tip] 
> 
> 为了让内存中的复杂数据结构存储在文件中，需要设计一套 **协议** 将复杂数据结构的信息和数据保存在文件中。当从文件中读取时，可以根据数据结构的信息在内存中还原已经存储下来的数据
> 

由于文件在逻辑上是字节的序列，将内存中的复杂数据结构保存在文件中的过程称为 **序列化(serialization)**。将文件中的数据还原为内存中的复杂数据结构的过程称为 **反序列化(deserialization)**

> [!tip] 序列化
> 
> 将内存中复杂的数据结构存储下来，把它变成一个个字节。将内存中的数据结构按照某种协议存储到文件中。为了方便反序列化，需要保存的数据包括
> + 复杂数据结构的 **类型**
> + 复杂数据结构中的 **数据**
> + 数据间的 **间隔**
> 


> [!tip] 反序列化
> 
> 根据文件中已经存储的信息，在内存中还原复杂的数据结构
> 

序列化后的数据可以保存到文件进行持久化；也可以通过网络传输。当我们有一个序列化的文件中或者从网络接收到的字节序列，可以进行反序列化，从而在内存中还原数据结构

> [!tip] 
> 
> Python 提供了专供 Python 自己使用的序列化协议 `pickle`
> 

## pickle 协议

模块 `pickle` 实现了对一个 Python 对象结构的二进制序列化和反序列化。 `"pickling"` 是将 Python 对象及其所拥有的层次结构转化为一个字节流的过程，而 `"unpickling"` 是相反的操作，会将（来自一个 `binary file` 或者 `bytes-like object `的）字节流转化回一个对象层次结构。 `pickling` 也被称为 **“序列化”**, **“编组”**  或者 **“平面化”**

`pickle` 模块中的序列化和反序列化函数如下表所示

| 函数      | 说明                   |
| :------ | :------------------- |
| `dumps` | 对象序列化为 `bytes` 对象    |
| `dump`  | 对象序列化并写入文件           |
| `loads` | 从 `bytes` 对象中反序列化出对象 |
| `load`  | 从文件中读取数据，并反序列化出对象    |

```python
import pickle

filename = "data.pkl"

i = 99
c = 'c'

l = list(range(1,4))

d = {"a": 1, "b": "abc", "c": [1,2,3,4]}

# 序列化
with open(filename, "wb") as f:
    pickle.dump(i, f)  # int类型 数据 边界
    pickle.dump(c, f)  # str类型 'c' 边界
    pickle.dump(l, f)  # list类型 [类型-1-边界, 类型-2-边界,类型-3-边界....] 边界
    pickle.dump(d, f)  # dict类型 {类型-key-边界: 类型-value-边界, ....} 边界

# 反序列化
with open(filename, "rb") as f:
    for i in range(4):
        print(pickle.load(f))
```

> [!tip] 
> 
> 可以看到二进制的序列化文件中，每一个数据最重要的，为了保存数据并在以后可以还原它，至少需要保存数据的 `类型 数据 边界`
> 

例如 $99$ 这个数据在内存中是二进制形式，用十六进制表示就是 `0x63`，如果以单字节来看，就可以理解为整型数字 `99`(十进制)，或理解为`bytes`类型`b'c'`，或理解为字符串类型`"c"` 在内存中的表达。**所以数据一定要赋予类型，否则可以做任意类型的解读**

序列化的文件分不清楚某一个数据和下一个数据如何分界，所以一定要用某些方式表示出数据的边界

当使用 `pickle` 序列化自定义类的实例时，只会保存实例相关的数据。反序列化时，可以修改类定义

```python
class AAA:
    tttt = "ABC"  # tttt 也不属于 AAA 的对象，它是类 AAA 的属性
    def show(self):
        print("abc")  # 输出数据，这个数据不属于 AAA 的对象

a1 = AAA()

# 序列化
ser = pickle.dumps(a1, protocol=3)
print(f"ser={ser}")  # ser=b'\x80\x03c__main__\nAAA\nq\x00)\x81q\x01.'

# 反序列化：此处如果修改 class AAA 的定义，反序列化得到的就是修改后的 AAA 的实例
a2 = pickle.loads(ser)
print(f"{a2.tttt}")  # ABC
a2.show() # abc
```

> [!attention] 警告：`pickle` 模块 **并不安全**。 你只应该对你信任的数据进行 unpickle 操作
> 
> 构建恶意的 `pickle` 数据来 **在 unpicking 时执行任意代码** 是可能的。 绝对不要对不信任来源的数据和可能被篡改过的数据进行解封。
> 
> 请考虑使用 `hmac` 来对数据进行签名，确保数据没有被篡改。
> 
> 在你处理不信任数据时，更安全的序列化格式如 `json` 可能更为适合
> 

## JSON 协议

JSON (JavaScript Object Notation) 是一种 **轻量级数据交换格式**。人类很容易读和写。机器很容易解析和生成。它基于 JavaScript 编程语言标准 `ECMA-262` 第3版(1999年12月)的一个子集

> [!tip] 
> 
> **JSON 是一种完全独立于语言的文本格式**，使用了 C 族语言程序员熟悉的约定。C 族语言包括 C、c++、c#、Java、JavaScript、Perl、Python等。这些特性使 JSON 成为理想的数据交换语言。
> 

JSON 由两种结构组成：**key-value的集合** 和 **有序的值列表**

> [!tip] key-value 的集合
> 
> 在各种语言中，这被实现为 _对象_ 记录 _结构体_ _字典_ _散列表_ 键列表或关联数组
> 

> [!tip] 有序的值列表
> 
> 在大多数语言中，这被实现为 _数组_、向量、_列表_ 或 _序列_
> 

这些是通用的数据结构。几乎所有的现代编程语言都以这样或那样的形式支持它们。可以与编程语言互换的数据格式也是基于这些结构的，这是有道理的。

JSON 中的值的类型包括下表列出的所有类型

| 类型               | 描述                      |
| :--------------- | ----------------------- |
| `string` 字符串     | 使用双引号引起来的值。例如，`"value"` |
| `number` 数值      | 包括整数和浮点数                |
| `true` 和 `false` | 布尔值 `true` 和 `false`    |
| `null`           | 空值                      |
| `object` 对象      | 键值对                     |
| `array` 数组       | 数组                      |

> [!tip] 字符串
> 
> 在 JSON 中，字符串值是使用 **双引号** 引起来的字符序列
> 

> [!tip] 对象
> 
> **对象** 是一个 **无序的键值对的集合**。一个对象以 `{` 开始， `}` 结束。每个“名称”后跟一个 冒号(`:`)；“‘`key-value`’ 对”之间使用 逗号(`,`) 分隔
> 

```js
{
	"person": [
		{
			"name": "tom",
			"age": 18
		},
		{
			"name": "jerry",
			"age": 16
		}
	],
	"total": 2
}
```

> [!tip] 数组
> 
> **数组是值（value）的有序集合**。一个数组以左中括号(`[`)开始，右中括号(`]`)结束。值之间使用逗号(`,`)分隔。
> 

Python 提供的标准库 `json` 实现了将 Python 内建类型的对象转换为 JSON 字符串。在 Python 的对象的值与 JSON 值对应情形如下表示

| Python           | JSON             |
| :--------------- | :--------------- |
| `dict`           | `object`         |
| `list`或`tuple`   | `array`          |
| `str`            | `string`         |
| `int` 或 `float`  | `number`         |
| `True` 或 `False` | `true` 或 `false` |
| `None`           | `null`           |

> [!tip] 
> 
> JSON 的数组只能还原为 Python 的 `list`
> 

JSON 模块中的提供的方法名和作用与 `pickle` 中一样

| 函数      | 说明                         |
| :------ | :------------------------- |
| `dumps` | 对象序列化为 JSON 字符串            |
| `dump`  | 对象序列化为 JSON 字符串并写入文件       |
|         |                            |
| `loads` | 从 JSON 字符串中反序列化出对象         |
| `load`  | 从文件中读取 JSON 字符串数据，并反序列化出对象 |

```python
import json

d = {
	"person": [
		{
			"name": "tom",
			"age": 18
		},
		{
			"name": "jerry",
			"age": 16
		}
	],
	"total": 2
}

# 序列化
s = json.dumps(d) 
print(type(s), len(s))  # <class 'str'> 82
print(s) # '{"person": [{"name": "tom", "age": 18}, {"name": "jerry", "age": 16}], "total": 2}'

# 反序列化
new = json.loads(s)
print(type(new), new)  # <class 'dict'> {'person': [{'name': 'tom', 'age': 18}, {'name': 'jerry', 'age': 16}], 'total': 2}

print(d == new)  # True
print(d is new)  # False
```


一般 JSON 编码的数据很少落地，数据都是通过网络传输。传输的时候，要考虑压缩它。本质上来说它就是个文本，就是个字符串。JSON 很简单，几乎编程语言都支持 JSON，所以应用范围十分广泛。

> [!tip] 
> 
> JSON 本质上使用文本编码内存中的复杂数据结构的。所以，序列化生产的数据就是一个字符串
> 
> 支持 JSON 的编程语言非常多，几乎是所有的编程语言都支持
> 

## MessagePack 协议

MessagePack 是一种 **高效的二进制序列化格式**。它允许你在多种语言(如 JSON)之间交换数据。但它更快、更小。**小的整数被编码成一个字节**，典型的短字符串除了字符串本身外只需要一个额外的字节。

> [!tip] 
> 
> MessagePack 很像 JSON。可以在许多种语言之间交换结构对象。但是它比 JSON 更快速也更轻巧。
> 

下图展示 MessagePack 和 JSON 序列化的对比

![[Pasted image 20240619235100.png]]


 支持 MessagePack 序列化的语言包括 Python、Ruby、Java、C/C++ 等众多语言。宣称比 Google Protocol Buffers 还要快 $4$ 倍

MessagePack 提供了与 `json` 和 `pickle` 相同的序列化和反序列化的函数名。为了使用 MessagePack 需要使用 `pip` 安装 `msgpack`

```shell
pip install msgpack-python
```

下面的代码对比了 MessagePack、JSON 和 pickle 序列化后占用的字节数

```python
import msgpack
import json
import pickle
d = {
	"person": [
		{
			"name": "tom",
			"age": 18
		},
		{
			"name": "jerry",
			"age": 16
		}
	],
	"total": 2
}


for module in (msgpack, json, pickle):
    s = module.dumps(d)
    print(type(s), len(s), s)

    new = module.loads(s)
    print(f"{new}")

```

上述脚本运行输出的结果为

```
<class 'bytes'> 48 b'\x82\xa6person\x92\x82\xa4name\xa3tom\xa3age\x12\x82\xa4name\xa5jerry\xa3age\x10\xa5total\x02'
{b'person': [{b'name': b'tom', b'age': 18}, {b'name': b'jerry', b'age': 16}], b'total': 2}

<class 'str'> 82 {"person": [{"name": "tom", "age": 18}, {"name": "jerry", "age": 16}], "total": 2}
{'person': [{'name': 'tom', 'age': 18}, {'name': 'jerry', 'age': 16}], 'total': 2}

<class 'bytes'> 82 b'\x80\x04\x95G\x00\x00\x00\x00\x00\x00\x00}\x94(\x8c\x06person\x94]\x94(}\x94(\x8c\x04name\x94\x8c\x03tom\x94\x8c\x03age\x94K\x12u}\x94(h\x04\x8c\x05jerry\x94h\x06K\x10ue\x8c\x05total\x94K\x02u.'
{'person': [{'name': 'tom', 'age': 18}, {'name': 'jerry', 'age': 16}], 'total': 2}
```

## Base64 编码

Base64（基底 $64$）是一种基于 $64$ 个可打印字符来表示二进制数据的表示方法

> [!tip] $6$ 位为一个单元，表示一个可打印字符
> 
> $\log_2 64 = 6$  所以 $6$ **个比特为一个单元**，对应某个可打印字符。$3$ 个字节相当于 $24$ 个比特，对应于 $4$ 个 Base64 单元，即 **$3$ 个字节可由 $4$ 个可打印字符来表示**
> 

在 Base64 中的可打印字符包括字母`A-Z`、`a-z`、数字 `0-9`，这样共有 $62$ 个字符，此外两个可打印符号在不同的系统中而不同

> [!important] 
> 
> Base64 编码的核心就是使用 $4$ 个可打印字符表示 $3$ 个字节
> 

RFC 4648 标准的 Base64 索引表如下

| 索引  | 二进制      | 字符  |     | 十进制 | 二进制      | 字符  |     | 十进制 | 二进制      |
| --- | -------- | --- | --- | --- | -------- | --- | --- | --- | -------- |
| 0   | `000000` | A   |     | 16  | `010000` | Q   |     | 32  | `100000` |
| 1   | `000001` | B   |     | 17  | `010001` | R   |     | 33  | `100001` |
| 2   | `000010` | C   |     | 18  | `010010` | S   |     | 34  | `100010` |
| 3   | `000011` | D   |     | 19  | `010011` | T   |     | 35  | `100011` |
| 4   | `000100` | E   |     | 20  | `010100` | U   |     | 36  | `100100` |
| 5   | `000101` | F   |     | 21  | `010101` | V   |     | 37  | `100101` |
| 6   | `000110` | G   |     | 22  | `010110` | W   |     | 38  | `100110` |
| 7   | `000111` | H   |     | 23  | `010111` | X   |     | 39  | `100111` |
| 8   | `001000` | I   |     | 24  | `011000` | Y   |     | 40  | `101000` |
| 9   | `001001` | J   |     | 25  | `011001` | Z   |     | 41  | `101001` |
| 10  | `001010` | K   |     | 26  | `011010` | a   |     | 42  | `101010` |
| 11  | `001011` | L   |     | 27  | `011011` | b   |     | 43  | `101011` |
| 12  | `001100` | M   |     | 28  | `011100` | c   |     | 44  | `101100` |
| 13  | `001101` | N   |     | 29  | `011101` | d   |     | 45  | `101101` |
| 14  | `001110` | O   |     | 30  | `011110` | e   |     | 46  | `101110` |
| 15  | `001111` | P   |     | 31  | `011111` | f   |     | 47  | `101111` |
| 填充  |          | =   |     |     |          |     |     |     |          |

举例来说，一段引用自托马斯·霍布斯《利维坦》的文句

```
Man is distinguished, not only by his reason, but by this singular passion from other animals, which is a lust of the mind, that by a perseverance of delight in the continued and indefatigable generation of knowledge, exceeds the short vehemence of any carnal pleasure.
```

经过 Base64 编码之后变成：

```base64
TWFuIGlzIGRpc3Rpbmd1aXNoZWQsIG5vdCBvbmx5IGJ5IGhpcyByZWFzb24sIGJ1dCBieSB0aGlzIHNpbmd1bGFyIHBhc3Npb24gZnJvbSBvdGhlciBhbmltYWxzLCB3aGljaCBpcyBhIGx1c3Qgb2YgdGhlIG1pbmQsIHRoYXQgYnkgYSBwZXJzZXZlcmFuY2Ugb2YgZGVsaWdodCBpbiB0aGUgY29udGludWVkIGFuZCBpbmRlZmF0aWdhYmxlIGdlbmVyYXRpb24gb2Yga25vd2xlZGdlLCBleGNlZWRzIHRoZSBzaG9ydCB2ZWhlbWVuY2Ugb2YgYW55IGNhcm5hbCBwbGVhc3VyZS4=
```

编码 `"Man"` 的结果为`"TWFu"`，详细原理如下。在此例中，**Base64 算法将 3 个字节编码为 4 个字符**

![[Pasted image 20240527193046.png]]

如果要编码的字节数不能被 $3$ 整除，最后会多出 $1$ 个或 $2$ 个字节，那么可以使用下面的方法进行处理：先使用 $0$ 字节值在末尾补足，使其能够被 $3$ 整除，然后再进行 $Base64$ 的编码。在编码后的 $Base64$ 文本后加上一个或两个`=`号，代表补足的字节数

![[Pasted image 20240527193447.png]]


Base64 编码过程就是将 $3$ 个任意字节数据编码为 $4$ 个字节，每个字节的最高 $2$ 位不用了，只用 $6$ 位，而 $6$ 位的变化只有 $64$ 种。利用 Base64 索引表查找出对应字符的编码即可。字节不够 $3$ 会补齐

```python
def encoding(src: bytes):
    """base64 编码
    """
    MAP_VALUE2CHAR: dict = {index : char for index, char in enumerate((ascii_uppercase + ascii_lowercase + "0123456789+/").encode())}

    # src = bytearray(src)
    # 计算字节数
    length = len(src)
    # 判断字节能否被 3 整除。如果不能，则需要要补充 0
    remainder = length % 3
    if remainder:
        # 不能被 3 整除，需要填充
        # 主要不要写为 src += b"0" * (3 - remainder)。b"0" 是 字符0的编码
        src += b"\x00" * (3 - remainder) # 填充 3 - remainder 个 0
    
    # 能被 3 整除，准备编码
    # 从 src 中获取 3 和字节，然后将其组合成一个大整数
    result = bytearray()  # 保存编码的结果
    for i in range(3, len(src)+1, 3):
        """ 
        u = 0
       
        for index, value in enumerate(src[i-3:i]):
            u |= value << (24 - (index + 1) * 8)

        print(hex(u))
        """
        # 获取三个字节，构建整数。使用位运算
        u = int.from_bytes(src[i-3:i], byteorder="big")

        for j in range(4):
            move_ndigits = 24 - (j + 1) * 6
            if  i == len(src) and remainder and j >= 2 and not ((u & (0x3f << move_ndigits)) >> move_ndigits):
                result.append(ord("="))  # 这里需要使用 ord() 将 "=" 获取它的编码
            else:
                result.append(MAP_VALUE2CHAR[(u & (0x3f << move_ndigits)) >> move_ndigits])

    return bytes(result)
```


