# 字符串

Python3 字符串都是使用 Unicode 类型。我们在 [[Python：基础语法#字符串]] 中已经有所介绍。这里我们介绍 Python 提供操作字符串的方法

> [!important] 
> 
> Python 中，字符串是是不可变序列。类似于 [[Python：列表和元组#元组]]
> 

## 通用序列操作

### 索引

字符串支持 **_索引_ （下标访问）**，第一个字符的索引是 $0$。**单字符没有专用的类型，就是长度为一的字符串**

```python
>>> "Py" "thon"
'Python'
>>> word = 'Python'
>>> word[0]
'P'
>>> word[5]
'n'
>>> word[7]
Traceback (most recent call last):
  File "<stdin>", line 1, in <module>
IndexError: string index out of range
```

> [!WARNING] 越界错误
> 当对字符串索引时，如果索引越界将会触发 `IndexError`

还支持负索引，使用负数索引时，从右边计数，第一个字符的索引为 $-1$

```python
>>> word[-1]
'n'
>>> word[-5]
'y'
>>> word[-6]
'P'
```

### 切片

除了索引操作，还支持 **_切片_**。 索引用来获取单个字符，而 **_切片_ 允许你获取子字符串**

```python
>>> word[0:2]   # 从位置0(包括)到2(不包括)的字符
'Py'
>>> word[2:5]  # 从位置2(包括)到位置5(不包括)的字符
'tho'
```

切片索引的默认值非常有用的。省略开始索引，默认值为 $0$，省略结束索引，默认值为字符串的结尾

```python
>>> word
'Python'
>>> word[:2]  # 从开头到位置2(排除)的字符
'Py'
>>> word[4:]  # 从第4位(包括在内)到末尾的字符
'on'
>>> word[-2:]  # 从倒数第二(包括在内)到末尾的字符
'on'
```

**切片只包含开始位置的元素，不包含结束位置的元素**

还可以这样理解切片，**索引指向的是字符 _之间_** ，第一个字符的左侧标为 0，最后一个字符的右侧标为 _n_ ，_n_ 是字符串长度。例如
```python
 +---+---+---+---+---+---+
 | P | y | t | h | o | n |
 +---+---+---+---+---+---+
 0   1   2   3   4   5   6
-6  -5  -4  -3  -2  -1
```

> [!attention] 
> 
> **切片会自动处理越界错误**
> 

```python
>>> word[2:42]
'thon'
>>> word[42:]
''
```

## 字符串是不可变序列

**Python 字符串不能修改**，因为 Python 中的字符串 是 **不可变**(`immutable`)  的。因此，为字符串中某个索引位置赋值会报错

```python
>>> word[0] = 'J'
Traceback (most recent call last):
  File "<stdin>", line 1, in <module>
TypeError: 'str' object does not support item assignment
>>> word[2:] = 'py'
Traceback (most recent call last):
  File "<stdin>", line 1, in <module>
TypeError: 'str' object does not support item assignment
```

要求生成不同的字符串，只能新建一个字符串

```python
>>> 'J' +word[1:]
'Jython'
>>> word[:2] + "py"
'Pypy'
```

### 拼接

#### `+` 和 `*` 运算符

Python 针对字符串将 `*` 和 `+` 运算符进行了从新定义
- 字符串与整数 `n` 执行 `*` 运算，表示将字符串 **重复拼接 `n` 次**，生成一个全新的字符串对象
- 对两个字符串执行 `+` 运算，表示将 **两个字符串拼接** 在一起

```python
>>> 3 * 'un' + 'ium'
'unununium'
```

> [!tip] 
> 
> `*` 和 `+` 都会创建一个全新的对象

对于相邻的两个字符串字面值，Python 解释器会自动将其拼接为一个字符串对象

```python
>>> "Py" "thon"
'Python'
```

> [!WARNING] 注意
> 只能是两个字面值才会自动拼接，如果其中一个是变量，将触发语法错误

#### `str.join(iterable[str])`

`iterable[str]` 表示 `iterable` 中的元素是 `str` 类型

```python
>>> "|".join(["h", "e", "l", "l", "o"])
'h|e|l|l|o'
>>> "|".join("hello")
'h|e|l|l|o'
```

> [!important] 返回一个全新的字符串对象

## 查找

`str.index(sub[,start[,end]])`: 同`str.find()`, 但是未找到则报 `ValueError` 错误

 `str.count(sub[,start[,end]])`: 返回`sub`在`s[start:end]`中出现的次数

`str.find(sub[, start[, end]])`: 返回子字符串 `sub` 在`s[start:end]`切片内被找到的最小索引, 没有则返回 $−1$

`str.rfind(sub[, start[, end]])`: 与 `str.find()` 类似，从右边开始, 没有则返回 $−1$

```python
>>> "world".find("o")
1
>>> "world".find("c")
-1
>>> "world".index("o")
1
>>> "world".count("o")
1
```

## 分割 

`str.split(sep=None, maxsplit=-1)`：从左到右，将 `str` 在 `sub` 处分割，返回一个列表对象。`maxsplit`指定分割次数,  $-1$ 表示在所有 `sub` 处分割

+ `sep` 默认在 **空白字符**(" ", "\\t", "\\n", "\\r")。尽可能的选择长的空白字符串

 `str.rsplit(sep=None, maxsplit=-1)`: 同 `str.split(）` 类似， 但是从右到左开始

`str.splitlines(keepends=False)`：在 **行边界** 的位置拆分，返回一个列表，结果列表中不包含行边界，除非给出了 `keepends` 且为真值。此方法会以下列行边界进行拆分

| 表示符           | 描述                 |
| ------------- | ------------------ |
| `\n`          | 换行                 |
| `\r`          | 回车                 |
| `\r\n`        | 回车 + 换行            |
| `\v` 或 `\x0b` | 行制表符，Python >=3.12 |
| `\f` 或 `\x0c` | 换表单，Python >=3.12  |
| `\x1c`        | 文件分隔符              |
| `\x1d`        | 组分隔符               |
| `\x1e`        | 记录分隔符              |
| `\x85`        | 下一行 (C1 控制码)       |
| `\u2028`      | 行分隔符               |
| `\u2029`      | 段分隔符               |

`str.partition(sep)`: 在`sep`首次出现的位置拆分字符串，返回一个 $3$ 元组，其中包含分隔符之前的部分、分隔符本身，以及分隔符之后的部分。 如果分隔符未找到，则返回的 $3$ 元组中包含字符本身以及两个空字符串

`str.rpartition(sep)`：同 `str.partition`，但是从右到左开始

```python
>>> "hello world".split()  # 默认在空白字符串处分割
['hello', 'world']
>>> "hello world".split("l", 1) # 分割一次
['he', 'lo world']
>>> "hello world".split("l")  # 
['he', '', 'o wor', 'd']
>>> "\n\ta\r\n ab\tc\n".split()
['a', 'ab', 'c']
>>> "\n\ta\r\n ab\tc\n".split('\n')
['', '\ta\r', ' ab\tc', '']

>>> "hello world".rsplit("l", 1)
['hello wor', 'd']

>>> " hello world ".partition("o") # 只在第一次出现子串的位置分割，并保留分割子串
(' hell', 'o', ' world ')
```

## 移除

`str.strip([chars])`: 移除字符串 _首尾_ 指定的字符

> [!seealso] 其他语言中，该方法可能称为 `trim`(修剪)

`str.lstrip([chars])`: 移除字符串 _首_ 指定的字符

`str.rstrip([chars])`: 移除字符串 _尾_ 指定的字符

> [!check] `chars` 是一个字符串，会在 `str` 的首尾移除 `chars` 中所有的字符

```python
>>> " hello world ".strip()  # 默认剥离空白字符
'hello world'
>>> " hello world ".strip("h d") # 可以指定要剥离的字符
'ello worl'

>>> " hello world ".lstrip()
'hello world '
>>> " hello world ".rstrip()
' hello world'
>>> " hello world ".rstrip(' d')
' hello worl'
>>> " hello world ".lstrip("h ")
'ello world '
>>> " hello world ".lstrip("h")
' hello world '
```

`str.removeprefix(prefix, /)`: 如果字符串以 `prefix` 字符串开头，返回 `string[len(prefix):]`。 否则，返回原始字符串的副本

```python
>>> 'TestHook'.removeprefix('Test')
'Hook'
>>> 'BaseTestCase'.removeprefix('Test')
'BaseTestCase'
```

`str.removesuffix(suffix, /)`：如果字符串以 `suffix` 字符串结尾，并且  `suffix` 非空，返回 `string[:-len(suffix)]`。 否则，返回原始字符串的副本:

```python
>>> 'MiscTests'.removesuffix('Tests')
'Misc'
>>> 'TmpDirMixin'.removesuffix('Tests')
'TmpDirMixin'
```

> [!tip] `removeprefix` 和 `removesuffix` 方法在 Python 3.9 添加

## 替换

`str.replace(old, new, [count])`: 使用 `new` 替换字符串中的 `old`. 可指定替换次数。该方法返回一个全新的字符串

```python
>>> " hello world ".replace("o", "", 1)
' hell world '
>>> " hello world ".replace("o", "")
' hell wrld '
>>> "c***b***c***c".replace("**", "*")
'c**b**c**c'
```

## 检查

1. `str.isalnum()`: 判断字符串是否全为字母或数字
2. `str.isalpha()`: 判断字符串中是否全为字母
3. `str.isisdigit()`: 判断字符串是否全为数字
4. `str.isdecimal()`: 判断字符串是否全为十进制数字
5. `str. isnumeric()`: 判断字符是否为数字字符
6. `str.isidentifier()`: 判断字符串是否为关键字
7. `str.isascii()`: 判断字符串是否为ascii码, python3.7以上才能使用此功能
8. `str.islower()`: 判断字符串是否全为小写字符
9. `str.isupper()`: 判断字符串是否全为大写字符
10. `str.isprintable()`: 判断字符串是否为可打印字符
11. `str.isspace()`: 判断字符串中是否全为空白字符
12. `str.istitle()`: 判断字符串是否为标题字符串
13. `str.startswith(prefix[, start[, end]])`: 判断字符是否为`prefix`开头, 可指定判断区间
14. `str.endswith(prefix[, start[, end]])`: 判断字符串是否为`prefix`结尾, 可指定判断区间

```python
>>> "abcd1234".isalnum()  # 判断是否为数字或字母, 是返回Ture
True
>>> "abcd1234 ".isalnum()
False
>>> "abcd".isalpha()  # 判断是否全为字母, 是返回True
True
>>> "abc d".isalpha()
False
>>> "3.12".isdigit()  # 判断是否全为数字, . 也是字符
False
>>> "3".isdigit()
True
>>> "3.14".isdecimal()  # 判断是否为十进制数字, . 也是字符
False
>>> "3".isdecimal()
True
>>> "一二三".isnumeric()  # 判断是否全为数字字符，Unicode 码
True
>>> "一二三12".isnumeric()
True
>>> "一二三12Ⅰ".isnumeric()  
True
>>> "else".isidentifier()  # 判断是否为关键字
True
>>> "hello world".islower()  # 判断是否全为小写字符
True
>>> "hello World".islower()
False
>>> "ABCD".isupper()
True
>>> "\n".isprintable()  # 判断字符串是否为可打印字符
False
>>> "10".isprintable()
True
>>> "Title V".istitle()  # 判断字符串是否为标题化字符串
True
>>> "hello world".startswith("he")  # 判断字符串是否为'he'开头
True
>>> "hello world".startswith("he", 0, 3)
True
>>> "hello world".endswith("th")  # 判断字符串是否为'th'结尾
False
>>> "hello world".endswith("ld")
True
```

## 大小写变换

1. `str.title()`: 标题化字符串, 即是每个单词首字母大写
2. `str.upper()`: 大写所有字母
3. `str.lower()`: 小写所有字母
4. `str.swapcase`: 字符串大小写互换
5. `str.capitalize()`: 大写首字母, 其余部分小写
6. `str.casefold()`: 消除大小写

```python
>>> string = "hello world"
>>> string.title()  # 标题化
'Hello World'
>>> string.upper()  # 大写字母
'HELLO WORLD'
>>> string.swapcase()  # 大小写互换
'HELLO WORLD'
>>> string.capitalize()  # 大写首字母
'Hello world'
>>> string.casefold()  # 消除大小写
'hello world'
>>> "HELLO WORLD".casefold() == string.casefold()
True
```

## 格式化

`str.center(width, [fillchar])`: 返回长度为`width`的字符串，原字符串在其正中。 使用指定的`fillchar`填充两边的空位, 默认为空格

`str.zfill(width)`: 在左边填充 ASCII '0' 数码使其长度变为 `width`

```python
>>> "world".center(20, "*")
'*******world********'

>>> "world".zfill(20)
'000000000000000world'
```

Python 提供三种方式进行字符串格式化：`c-style` 风格，`format` 风格 和 `f-string` 风格

### `c-style`格式化

字符串具有一种特殊的内置操作：使用 `%` (取模) 运算符。 这也被称为字符串的 __格式化__ 或 __插值__ 运算符。例如，

```python
format % value
```

将 `value` 值插入到 `format` 字符串中 转换标记(`%[可选项]转换符`)  的位置上。这是继承自 C 语言 `printf` 函数的格式化方式

**转换标记** 按照从左到右顺序包括如下几个部分
+ **`"%"` 字符**，标记转换符的起始位置。该字符为 **必选字符**
+ **映射建`(somename)`**，可选
+ **转换 `flag`**，可选
+ **最小字段宽度(`m`)**，可选。可以为 `*` ，此时需要在 `values` 元组的下一元素中读取
+ **精度(`.n`)**，可选。在以 `.` 之后的 `n` 。可以为 `*`，此时需要在 `values` 元组中的下一元素中读取
+ **长度修饰符**，可选
+ **转换类型**。**必选字符**

**转换类型** 由下表给出:

|  转换符  |                含义                | 备注                                           |
| :---: | :------------------------------: | -------------------------------------------- |
| `"d"` |             有符号十进制整数             |                                              |
| `"i`  |             有符号十进制整数             |                                              |
| `"o"` |              有符号八进制              | 会在第一个数码之前插入标示八进制数的前缀 (`'0o'`)                |
| `"u"` |          过时类型，等价于`"d"`           | 在 C 中表示无符号十进制整数                              |
| `"x"` |          有符号十六进制整数；字母小写          | 在第一个数码之前插入 `'0x'`                            |
| `"X"` |          有符号十六进制整数；字母大写          | 在第一个数码之前插入 `'0X'`                            |
| `"e"` |         浮点数指数形式；字母`e` 小写         | 在结果中包含一个小数点，即使其后并没有数码。小数点后的数码位数由精度决定，默认为 $6$ |
| `"E"` |        浮点数指数形式；字母 `E` 大写         | 在结果中包含一个小数点，即使其后并没有数码。小数点后的数码位数由精度决定，默认为 $6$ |
| `"f"` |             浮点十进制格式              | 在结果中包含一个小数点，即使其后并没有数码。小数点后的数码位数由精度决定，默认为 $6$ |
| `"F"` |             浮点十进制格式              | 在结果中包含一个小数点，即使其后并没有数码。小数点后的数码位数由精度决定，默认为 $6$ |
| `"g"` |              浮点数格式               | 根据数字选择 `e` 或 `f` 对应的格式                       |
| `"G"` |              浮点数格式               | 根据数字选择 `e` 或 `f` 对应的格式                       |
| `"c"` |               单个字符               | 可以接受整数                                       |
| `"r"` | 字符串，使用 `repr()` 转换的任何 Python 对象  | 如果精度为 `N`，输出将截断为 `N` 个字符                     |
| `"s"` |  字符串，使用 `str()` 转换的任何 Python 对象  | 如果精度为 `N`，输出将截断为 `N` 个字符                     |
| `"a"` | 字符串，使用 `ascii()` 转换的任何 Python 对象 | 如果精度为 `N`，输出将截断为 `N` 个字符                     |
| `"%"` |          不转换参数，直接输出 `%`          |                                              |

注意：由于 **Python 字符串显式指明长度**，`%s` 转换不会将 `'\0'` 视为字符串的结束。可以给出长度修饰符 (`h`, `l` 或 `L`)，但会被忽略，因为对 Python 来说没有必要，所以 `%ld` 等价于 `%d`

**转换 `flag`** 可以由下表给出

| `flag` |              含义              |         备注         |
| :----: | :--------------------------: | :----------------: |
| `"#"`  |         值的转换将使用替代形式          |                    |
| `"0"`  |       转换将为数字填充字符`"0"`        |                    |
| `"-"`  |           转换值将靠左对齐           |                    |
| `" "`  |     空格，符号位转换产生整数将留出一个空格      |                    |
| `"+"`  | 符号字符(`"+"`或`"-"`)将显示转换为结果的开头 | 覆盖空格`flag` (`" "`) |

两个示例：
```python
>>> print('%(language)s has %(number)03d quote types.'%{'language': "Python", "number": 2})
Python has 002 quote types.
>>> print('%s has %03d quote types.' % ("Python", 2))
Python has 002 quote types.
```

> [!WARNING] 注意：在键值映射格式化过程中，不能出现 `*` 标记符

### `format` 方法

可以调用 `format` 字符串的 `str.format()` 方法产生一个 `format` 的副本，其中 `{}` 的位置被替换为 `format` 中指定的参数

下面的例子是使用 `str.foramt()` 方法的最简单的形式，按照 `{}` 在 `format` 字符串中的位置与 `str.format()` 中给出的参数位置一一对应即可，称为 **位置映射**

```python
>>> print('{} has {} quote types.'.format("Python", 2))
Python has 2 quote types.
```

位置映射还可以是这样的形式`{i}` ，其中 `i` 表示 `str.format()` 给出的参数索引

```python
>>> print("{1} has {0} quote types.".format(2, "Python"))
Python has 2 quote types.
```

可以和 `c-style` 风格一样的方式指定格式化的形式，只需要在 花括号中添加冒号(`:`) 并在冒号后自定格式化参数。其中，格式化参数和 `c-style` 转换类型和转换 `flag` 一致

```python
>>> print('{} has {:>04d} quote types.'.format("Python", 2))  # 右对齐
Python has 0002 quote types.
>>> print('{} has {:<4d} quote types.'.format("Python", 2))  # 左对齐
Python has 2    quote types.
```

注意，此时如果想要格式化后的值靠左对齐需要使用 `<` 标志

**键值映射格式化**在 `format` 方法中也是可用的，只需要在花括号中指定名字 `{somename}` 并在 `str.format()` 方法中给出对应的名字和值即可

```python
>>> print('{language} has {number} quote types.'.format(language="Python", number=2))
Python has 2 quote types.
```

### `f-string` 方式

注意，`f-string` 要求 Python $\ge$ 3.6 才可以使用 `f-string` 方式进行格式化。只需要在 `str.format()` 方式的 `format` 字符串前加上 `f` 即可直接在 `{}` 中书写 Python 表达式

```python
first_name = "ada" 
last_name = "lovelace" 
full_name = f"{first_name} {last_name}"
print(full_name)


name = "Fred"
print(f"He said his name is {name!r}")  # !r 等价于 repr()

width = 10
precision = 4
value = decimal.Decimal("3.1415926")
print(f"result: {value:{width}.{precision}}")  # 嵌套的字段

now = datetime.datetime.now()
print(f"now:{now: %Y-%m-%d %H:%M:%S}")  # 使用日期格式说明符

number = 1024
print(f"{number:#0x}")  # 使用整数格式说明符

foo = "bar"
print(f"{foo = }")  # 保留空格

line = "The mill's closed"
print(f"{ line = }")
print(f"{line = :20}")
print(f"{line = !r:20}")
```
