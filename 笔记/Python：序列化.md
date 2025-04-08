# 序列化

> [!question] 
> 
> 为什么要序列化？序列化需要解决下面几个问题
> - 内存中的字典、列表、集合以及各种对象，如何保存到一个文件中?
> - 如果是自己定义的类的实例，如何保存到一个文件中?
> - 如何从文件中读取数据，并让它们在内存中再次恢复成自己对应的类的实例?

要设计一套 **协议**，按照某种规则，把内存中数据保存到文件中。文件是一个字节序列，所以必须把数据转换成 **字节序列**，输出到文件。这就是 **序列化**。

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

**序列化保存到文件就是持久化**。可以将数据序列化后持久化，或者网络传输；也可以将从文件中或者网络接收到的字节序列反序列化。

Python 提供了 `pickle` 库，是 Python 内建的序列化和反序列的标准模块，这个模块可以序列化任何 Python 对象

## pickle

标准库 `pickle` 实现了对一个 Python 对象结构的二进制序列化和反序列化。 `"pickling"` 是将 Python 对象及其所拥有的层次结构转化为一个字节流的过程，而 `"unpickling"` 是相反的操作，会将来自 `binary file` 或者 `bytes-like object `的字节流转化回一个对象层次结构

下表列出了标准库 `pickle` 中提供的序列化和反序列化的函数

| 函数               | 说明                   |
| :--------------- | :------------------- |
| `pickle.dumps()` | 对象序列化为 `bytes` 对象    |
| `pickle.dump()`  | 对象序列化并写入文件           |
|                  |                      |
| `pickle.loads()` | 从 `bytes` 对象中反序列化出对象 |
| `pickle.load()`  | 从文件中读取数据，并反序列化出对象    |

### dumps() 和 dump() 函数

这两个函数用于将 Python 中的对象序列化为字节序列，它们之间的区别在于 `pickle.dumps()` 会直接返回序列化后的 `bytes` 对象，而 `pickle.dump()` 会将序列化后的结果写入一个支持写操作的对象，通常是文件

```python
>>> import pickle
>>> data = {"name": "张三", "age": 28, "gender": "male"}
>>> pickle.dumps(data)
b'\x80\x04\x95-\x00\x00\x00\x00\x00\x00\x00}\x94(\x8c\x04name\x94\x8c\x06\xe5\xbc\xa0\xe4\xb8\x89\x94\x8c\x03age\x94K\x1c\x8c\x06gender\x94\x8c\x04male\x94u.'

>>> f = open("data.pkl", "wb")  # 打开文件
>>> pickle.dump(data, f) # 将 data 序列化并写入到文件中
>>> f.flush()  # 刷新文件缓冲区
>>>
```

> [!attention] 
> 
> 可以看到将对象序列化之后，对象的值是最重要的，为了保存对象的值并且以后可以还原它，至少需要保存对象的 `类型 数据 边界`
> 

### loads() 和 load() 函数

这两个函数用于将 `dumps()` 和 `dump()` 输出的字节序列反序列化为内存中的对象

```python
>>> d = pickle.loads(b'\x80\x04\x95-\x00\x00\x00\x00\x00\x00\x00}\x94(\x8c\x04name\x94\x8c\x06\xe5\xbc\xa0\xe4\xb8\x89\\x94\x8c\x03age\x94K\x1c\x8c\x06gender\x94\x8c\x04male\x94u.')
>>> d
{'name': '张三', 'age': 28, 'gender': 'male'}

>>> f = open("data.pkl", "rb")
>>> d = pickle.load(f)
>>> d
{'name': '张三', 'age': 28, 'gender': 'male'}
```

> [!attention] 
> 
> 请注意：`pickle.load()` 只会从文件中读取一行并反序列。如果文件中有多行数据，请使用 `for` 循环
> 

如下示例中：我们分多次将不同对象的序列化并写入文件，在读取时也应该按照写入的顺序读取

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

上述脚本执行后，我们查看生成的文件的内容如下

![[Pasted image 20250406233553.png]]

Python 的对象经过 `pickle` 序列化之后就变成了一个字节序列，这些字节序列采用某种 PICKLE 协议进行解析

> [!tip] 
> 
> 协议就规定了 **数据类型如何表示？**、**对象的值如何存储？** 以及 **对象的边界如果划分？**
> 

### 序列化自定义类型的对象

在上面我们使用的是 Python 的内建对象作为示例，下面我们看看 `pickle` 模块是如何序列化自定义类的实例对象

```python
>>> class AAA:
...     ttt = "aaa"
...     def show(self):
...         print("aaa") 
...
>>> a = AAA()
>>> a.show()
aaa

>>> pickle.dumps(a)
b'\x80\x04\x95\x17\x00\x00\x00\x00\x00\x00\x00\x8c\x08__main__\x94\x8c\x03AAA\x94\x93\x94)\x81\x94.'
```

在上述示例中，我们自定义一个类 `AAA`，该类对象有一个属性 `ttt`，值为 `"aaa"`；以及一个方法 `show()`

我们使用 `pickle.dumps()` 将类 `AAA` 的实例对象 `a` 进行序列化之后，输出的 `bytes` 对象中，我们只发现了 `"AAA"` 这三个熟悉的字符。这是因为 `pickle` 记录的实例对象 `a` 的类型就是 `AAA`，而这个实例并没有次有任何数据

现在，我们将 `pickle.dumps(a)` 输出的内容保存下来，然后重启解释器

```python
>>> class AAA:
...     t = "bbb"
...     def show(self):
...         print("ccc")
...
>>>
>>> import pickle
>>> a = pickle.loads(b'\x80\x04\x95\x17\x00\x00\x00\x00\x00\x00\x00\x8c\x08__main__\x94\x8c\x03AAA\x94\x93\x94)\x81\x94\.')
>>> a.show()
ccc
```

> [!attention] 
> 
> 请注意：这里我们在反序列化时重新定义了类 `AAA`，从而达到了修改对象行为的目的。这就说明了 `pickle` 模块是不安全的，它很容易被篡改
> 

---

通常，我们将内存中的对象序列化并不仅仅只是将其存储在文件，或者在本地的两个 Python 程序之间交换数据；而是想要通过网络将数据发送给其他计算机上运行的程序使用。

然而，不同的程序可能是用不同的编程语言编写的，如果我们使用 `pickle` 协议序列化之后发送给其他应用程序，它们可能无法识别。从而催生了 **其他公共协议** 的发展

> [!tip] 
> 
> 公共协议就是与语言无关的序列协议。例如 XML、JSON、Protocol Buffer、MessagePack 等。
> 

目前使用最广泛的就是 JSON 协议，他通常用于 Web 应用程序的前后端数据交互

## JSON

对于 PICKLE 协议，我们只需要了解它是如何工作的即可。下面我们详细介绍 [JSON](https://www.json.org/json-zh.html) 是如何组织数据的

> [!tip] 
>
>JSON 是基于字符串的序列化，本质上就是将对象使用字符串表示。
>

JSON (JavaScript Object Notation) 是一种 **轻量级数据交换格式**。人类很容易读和写。机器很容易解析和生成。它基于 JavaScript 编程语言标准 ECMA-262 第 3 版(1999年12月)的一个子集。**JSON 是一种完全独立于语言的文本格式**，使用了 C 族语言程序员熟悉的约定。这些特性使 JSON 成为理想的数据交换语言。

JSON 字符串由两种结构组成：**键值对集合** 和 **有序的值列表**

> [!tip] 
> 
> 键值对集合在大部分编程语言中都有实现。例如，在 Python 中实现为 **字典**，在 Go 语言中实现为 **结构体** 或 **Map** 等 
> 

> [!tip] 
> 
> 有序的值列表在大部分语言中也有对应实现。例如，在 Python 中实现为 **元组** 或 **列表**，在 Go 语言中实现为 **切片**
> 

这些数据结构是通用的，几乎所有的现代编程语言都支持它们。因此将 JSON 作为跨编程语言的数据交换协议是非常合适的

Python 提供了标准库 `json` 用于 **序列化** 和 **反序列化** 对象。标准库 `json` 提供的接口与标准库 `pickle` 提供的接口一样

| 函数             | 描述                                    |
| :------------- | :------------------------------------ |
| `json.dumps()` | 将 Python 中的对象序列化为 JSON 字符串            |
| `json.dump()`  | 与 `dumps()` 类似，只是它将结果写入到可写对象中         |
|                |                                       |
| `json.loads()` | 将 JSON 字符串反序列化为 Python 中的对象           |
| `json.load()`  | 与 `loads()` 类似，只是它从一个可读对象中读取 JSON 字符串 |

这些接口在执行时根据下表列出的映射规则完成 Python 对象与 JSON 字符串的转换

| Python          | JSON     |
| --------------- | -------- |
| `dict`          | `object` |
| `list`, `tuple` | `array`  |
| `str`           | `string` |
| `int`, `float`  | `number` |
| `True`          | `true`   |
| `False`         | `false`  |
| `None`          | `null`   |

> [!attention] 
> 
> 请注意：JSON 中的 `array` 只能被反序列化为 Python 中的 `list` 对象
> 

### JSON 数据类型

下表列出了 JSON 字符串中的类型

| 类型    | 描述                            |
| :---- | :---------------------------- |
| 布尔类型  | 逻辑值类型，有两个字面值 `true` 和 `false` |
| 数字类型  | 包括整数和浮点数                      |
| 字符串类型 | 字符序列                          |
| 空类型   | 没有值，只有一种值 `null`              |
| 对象类型  | 键值对的集合，在其他语言中可能称为字典 结构体等      |
| 数组类型  | 有序的值列表                        |

#### 布尔类型

如果一个 JSON 字符串只有 `true` 或则 `false` 这些字符，那么这个 JSON 字符串就是布尔类型对象序列化后的结果。即 `"true"` 和 `"false"`

```python
>>> import json
>>> json.loads("true")
True
>>> json.loads("false")
False
```

#### 数字类型

如果一个 JSON 字符串中只有数字，包括整数和浮点数，那么这个 JSON 字符串就是数字类型对象序列化后的结果。例如，`"3.14"` `"2048"`

```python
>>> json.loads("3.14")
3.14
>>> json.loads("2048")
2048
```

#### 字符串类型

如果一个 JSON 字符串中是另一个字符串，那么这个 JSON 字符串就是字符串类型对象序列化的结果。例如 `"\"hello, world\""`

> [!tip] 
> 
> 由于 JSON 本来就是一个字符串，在 JSON 中表示字符串必须使用双引号(`"..."`)包围字符序列
> 

```python
>>> json.loads("\"hello, world\"")
'hello, world'
```

#### 空类型

如果一个 JSON 字符串中 `null` 这个几个字符，那么这个 JSON 字符串就是空类型对象序列化的结果。即 `"null"`

```python
>>> print(json.loads("null"))
None
```

#### 对象类型

JSON 字符串中的对象与 Python 中字典相似。即 `"{\"age\": 20, \"name\": \"张三\"}"`

```python
>>> json.loads("{\"age\": 20, \"name\": \"张三\"}")
{'age': 20, 'name': '张三'}
```

#### 数组类型

数组是有序的上述类型的值的序列，与 Python 中的列表相似。例如，一个字符串数组的 JSON 字符串如下`"[\"hello\", \"世界\"]"`

```python
>>> json.loads("[\"hello\", \"世界\"]")
['hello', '世界']
```

### json 模块

> [!warning] 
> 
> 在解析来自不受信任恶劣的 JSON 数据时要小心谨慎。 恶意的 JSON 字符串可能导致解码器消耗大量 CPU 和内存资源。 建议对要解析的数据大小进行限制。
> 

#### dumps() 和 dump() 函数

函数 `json.dumps(obj)` 会将 `obj` 序列化为 JSON 字符串。此外，这函数还有几个参数用于控制 `json.dumps()` 的行为

| 参数               | 描述                                     |
| :--------------- | :------------------------------------- |
| `skipkeys`       | 是否跳过非标准类型的键。 默认为 `False`               |
| `ensure_ascii`   | 是否对输入的非 ASCII 字符进行转移。默认为 `True`        |
| `check_circular` | 是否检查容器类型的循环引用检查。默认为 `True`             |
| `allow_nan`      | 是否序列化超范围的 `float` 值。默认为 `True`         |
| `cls`            | 指定 `JSONEncoder`                       |
| `indent`         | 指定 JSON 数组元素和对象成员的缩进层级美化               |
| `separators`     | 二元组: `(item_separator, key_separator)` |
| `default `       | 当对象无法被序列化时将被调用的函数                      |
| `sort_keys `     | 是否对 `key`  进行排序                        |

> [!tip] 
> 
> 参数 `skipkeys` 默认值为 `False`。此时，当  `json.dumps()` 在序列化字典对象时，如果遇见非标准类型的键将抛出 `TypeError`
> 
> 如果该参数指定为 `True`，则会跳过非标准类型的键
> 

```python
>>> class A:
...     pass
...
>>> json.dumps({"name": "张三", "age": 28, A(): "a"})
Traceback (most recent call last):
  File "<python-input-31>", line 1, in <module>
    json.dumps({"name": "张三", "age": 28, A(): "a"})
    ~~~~~~~~~~^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
  File "C:\Program Files\Python313\Lib\json\__init__.py", line 231, in dumps
    return _default_encoder.encode(obj)
           ~~~~~~~~~~~~~~~~~~~~~~~^^^^^
  File "C:\Program Files\Python313\Lib\json\encoder.py", line 200, in encode
    chunks = self.iterencode(o, _one_shot=True)
  File "C:\Program Files\Python313\Lib\json\encoder.py", line 261, in iterencode
    return _iterencode(o, 0)
TypeError: keys must be str, int, float, bool or None, not A

>>> json.dumps({"name": "张三", "age": 28, A(): "a"}, skipkeys=True)  # 跳过非标准类型的键
'{"name": "\\u5f20\\u4e09", "age": 28}'
```

> [!tip] 
> 
> 参数 `ensure_ascii` 默认值为 `True`，此时 `json.dumps()` 会使用 ASCII 字符对输入的非 ASCII 字符进行转移。如果想要非 ASCII 字符原样输出，则将该参数指定为 `False`
> 

```python
>>> json.dumps({"name": "张三", "age": 28, A(): "a"}, skipkeys=True, ensure_ascii=False) # 非 ASCII 字符原样输出
'{"name": "张三", "age": 28}'
```

参数 `check_circular` 默认值为 `True`，此时 `json.dumps()` 会检查容器是否存在循环引用。如果存在，此时会抛出 `ValueError`。请注意：如果 **确认没有循环引用**，可以将该参数指定为 `False` 

```python
>>> li = [1,2,3]
>>> li.append(li)
>>> li
[1, 2, 3, [...]]
>>> json.dumps(li)
Traceback (most recent call last):
  File "<python-input-37>", line 1, in <module>
    json.dumps(li)
    ~~~~~~~~~~^^^^
  File "C:\Program Files\Python313\Lib\json\__init__.py", line 231, in dumps
    return _default_encoder.encode(obj)
           ~~~~~~~~~~~~~~~~~~~~~~~^^^^^
  File "C:\Program Files\Python313\Lib\json\encoder.py", line 200, in encode
    chunks = self.iterencode(o, _one_shot=True)
  File "C:\Program Files\Python313\Lib\json\encoder.py", line 261, in iterencode
    return _iterencode(o, 0)
ValueError: Circular reference detected
```

参数 `allow_nan` 默认值为 `True`，此时 `json.dumps()` 可以序列超范围 `float` 类型的对象。如果 `float` 类型的对象没有超范围的值，那么可以将该参数设置为 `False`；此时，如果有 `float` 对象超出范围，则会抛出 `ValueError`

```python
>>> json.dumps(float("inf"))
'Infinity'

>>> json.dumps(float("inf"), allow_nan=False)
Traceback (most recent call last):
  File "<python-input-41>", line 1, in <module>
    json.dumps(float("inf"), allow_nan=False)
    ~~~~~~~~~~^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
  File "C:\Program Files\Python313\Lib\json\__init__.py", line 238, in dumps
    **kw).encode(obj)
          ~~~~~~^^^^^
  File "C:\Program Files\Python313\Lib\json\encoder.py", line 200, in encode
    chunks = self.iterencode(o, _one_shot=True)
  File "C:\Program Files\Python313\Lib\json\encoder.py", line 261, in iterencode
    return _iterencode(o, 0)
ValueError: Out of range float values are not JSON compliant: inf
```

> [!important] 
> 
> 参数 `cls` 默认为 `None`。可以设置一个重写为一个带有 `default()` 方法的自定义 JSON 编码器，用以序列化为自定义的数据类型。该参数的默认值为 `None` ，此时使用 JSONEncoder
> 

`json.dumps()` 默认情况下是不能对 `complex` 对象进行序列化的

```python
>>> c = 3 + 3j
>>> json.dumps(c)
Traceback (most recent call last):
  File "<python-input-43>", line 1, in <module>
    json.dumps(c)
    ~~~~~~~~~~^^^
  File "C:\Program Files\Python313\Lib\json\__init__.py", line 231, in dumps
    return _default_encoder.encode(obj)
           ~~~~~~~~~~~~~~~~~~~~~~~^^^^^
  File "C:\Program Files\Python313\Lib\json\encoder.py", line 200, in encode
    chunks = self.iterencode(o, _one_shot=True)
  File "C:\Program Files\Python313\Lib\json\encoder.py", line 261, in iterencode
    return _iterencode(o, 0)
  File "C:\Program Files\Python313\Lib\json\encoder.py", line 180, in default
    raise TypeError(f'Object of type {o.__class__.__name__} '
                    f'is not JSON serializable')
TypeError: Object of type complex is not JSON serializable
```

为了能让 `complex` 对象被序列化，我们可以自定义一个用于编码 `complex` 对象的编码器，这个编码器必须继承 `JSONEncoder`

```python
>>> class ComplexEncoder(json.JSONEncoder):
...     def default(self, obj):
...         if isinstance(obj, complex):
...             return f"{obj.real}+{obj.imag}i"
...         return supper().default(obj)
...
>>> json.dumps(c, cls=ComplexEncoder)
'"3.0+3.0i"'
```

> [!tip] 
> 
> 参数 `indent` 用于给 JSON 数组元素和对象成员将按其所指定的 **缩进层级** 美化打印
> 
> 该参数的默认值为 `None`，才是会将 JSON 字符串紧凑输出
> 

```python
>>> r = json.dumps({"name": "张三", "age": 28, A(): "a"}, skipkeys=True, ensure_ascii=False, indent=4)
>>> print(r)
{
    "name": "张三",
    "age": 28
}
```

> [!warning] 
> 
> 参数 `separators` 是一个二元组，用于指定项和键的分割符
> 
> 这个参数请保持默认，不要去修改
> 

```python
>>> r = json.dumps({"name": "张三", "age": 28, A(): "a"}, skipkeys=True, ensure_ascii=False, separators=(";", "::"))
>>> r
'{"name"::"张三";"age"::28}'
```

> [!tip] 
> 
> 参数 `default` 默认值为 `None`，该参数用于指定当对象无法序列化时调用的函数
> 
> 请注意，这个函数应该返回一个可被 JSON 编码的版本或是引发 `TypeError`。默认情况下抛出 `TypeError`
> 

```python
>>> r = json.dumps({"name": "张三", "age": 28, "a": A()},  ensure_ascii=False, default=lambda a: "a")
>>> r
'{"name": "张三", "age": 28, "a": "a"}'
```

---

`json.dump(obj, f)` 函数将 `obj` 序列化为 JSON 字符串，并将其写入可写对象中，通常情况下这个可写对象是文件。此外，上述参数也在这个函数中使用

#### loads() 和 load() 函数

函数 `json.loads(string)` 将 JSON 字符串 `string` 反序列化为 Python 对象。此外，这函数还有几个参数用于控制 `json.loads()` 的行为

| 参数                   | 描述                  |
| :------------------- | :------------------ |
| `cls`                | 指定 JSONDecoder      |
| `object_hook`        | 指定一个函数拦截对象的反序列化     |
| `object_pairs_hook ` | 指定一个函数拦截对象的反序列化     |
| `parse_float`        | 指定一个函数拦截浮点数的反序列化    |
| `parse_int`          | 指定一个函数拦截整数的序列化      |
| `parse_constant`     | 指定一个函数拦截无效的 JSON 数字 |

> [!tip] 
> 
> 参数 `cls` 指定在反序列化时使用的 JSON 解码器，默认使用 JSONDecoder。我们可以继承 JSONDecoder 并重写 `loads()` 方法用于自定义 JSON 解码规则
> 

```python
import re
import json


class ComplexEncoder(json.JSONEncoder):
    def default(self, o):
        if isinstance(o, complex):
            return f"{o.real} + {o.imag}i"

        return super().default(o)


class ComplexDecoder(json.JSONDecoder):
    def decode(self, s, *args, **kwargs):
        res = re.search(r"(\d+\.\d+) \+ (\d+\.\d+)i", s)
        return complex(float(res.group(1)), float(res.group(2)))

c = 3 + 3j

r = json.loads(json.dumps(c, cls=ComplexEncoder), cls=ComplexDecoder)
print(r)  # (3+3j)
```

## MessagePack

JSON 是文本序列协议，使用 JSON 序列化出来的结果是字符串。如果序列化的对象本身就是字符串，那么这是毫无疑问的。然而，有时候我们需要将其他类型的数据序列化为字符串，这可能会造成字节的浪费

例如，对于整数 $255$，只需要 $1$ 个字节就能保存。然而，将其序列化为 JSON 字符串的结果为 `"255"`，这样就需要 $3$ 个字节，从而浪费了 $2$ 个字节

为了避免这种浪费，我们需要采用 **二进制序列化协议**。这里我们将介绍一个类似于 JSON 的二进制序列化协议 [MessagePack](https://msgpack.org/)

MessagePack 是一种 **高效的二进制序列化协议**。它允许你在多种语言(如 JSON)之间交换数据。但它更快、更小。例如，**小的整数被编码成一个字节**，典型的短字符串除了字符串本身外只需要一个额外的字节。

它可以像 JSON 那样，在许多种语言之间交换结构对象。但是它比 JSON 更快速也更轻巧。如下图，这是 MessagePack 官网上与 JSON 格式的对比

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


> [!tip] 
> 
> 关于 MessagePack 协议我们不做详细解释，如有需要请参看 [ [MessagePack 规范](https://github.com/msgpack/msgpack/blob/master/spec.md)
> 

### 接口

[message-pack](https://github.com/msgpack/msgpack-python) 提供了 Python 的 MessagePack 协议的实现。因此，我们应该首先安装这个第三方库

```shell
$ pip install -U msgpack
```

用于序列化和反序列的函数分别是 `msgpack.packb()` 和 `msgpack.unpackb()`

```python
>>> import msgpack
>>> person = {"name": "张三", "age": 28, "gender": "male"}
>>> msgpack.packb(person)
b'\x83\xa4name\xa6\xe5\xbc\xa0\xe4\xb8\x89\xa3age\x1c\xa6gender\xa4male'
>>> msgpack.unpackb(_)
{'name': '张三', 'age': 28, 'gender': 'male'}
```

此外，还提供了与标准库 `json` 同名的接口，它们只是 `msgpack.packb()` 和 `msgpack.ubpackb()` 的包装器

```python
>>> msgpack.dumps(person)
b'\x83\xa4name\xa6\xe5\xbc\xa0\xe4\xb8\x89\xa3age\x1c\xa6gender\xa4male'
>>> msgpack.loads(_)
{'name': '张三', 'age': 28, 'gender': 'male'}
```

## Base64

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

![[Pasted image 20240527193046.png]]

在此例中，**Base64 算法将 3 个字节编码为 4 个字符**

**如果要编码的字节数不能被 3 整除，最后会多出 1 个或 2 个字节，那么可以使用下面的方法进行处理：先使用 0 字节值在末尾补足，使其能够被 3 整除，然后再进行 Base64 的编码**。在编码后的 Base64 文本后加上一个或两个`=`号，代表补足的字节数。也就是说，当最后剩余两个八位(待补足)字节（`2`个 `bytes`）时，最后一个 6 位的 Base64 字节块有四位是 0 值，最后附加上两个等号；如果最后剩余一个八位(待补足)字节（`1` 个 `byte`）时，最后一个 `6` 位的 `base` 字节块有两位是 `0` 值，最后附加一个等号。 参考下表：

![[Pasted image 20240527193447.png]]

### Base64 编码实现


编码过程就是对 $3$ 个任意字节数据编码为 $4$ 个字节，每个字节的最高 $2$ 位不用了，只用 $6$ 位，而 $6$ 位的变化只有 $64$ 种，如上图，利用上图查表对应就得出编码了。字节不够 $3$ 会补齐

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

