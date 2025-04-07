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

对于 PICKLE 协议，我们只需要了解它是如何工作的即可。下面我们详细介绍 JSON 是如何组织数据的

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






