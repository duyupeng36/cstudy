# 正则表达式

在 [[正则表达式]] 中介绍了正则表达式的语法，下面我们介绍 Python 用于支持正则表达式的模块 `re`

## 标志

标准库 `re` 中提供了一个 `RegexFlag` 类，它是 `enum.IntFlag` 的子类，这类是一个 **枚举类** 包含了下表列出的几个选项

| 标志                      | 内联字符  | 含义          |
| :---------------------- | ----- | :---------- |
| `re.A` 或 `re.ASCII`     | `"a"` | 仅限 ASCII 匹配 |
| `re.I` 或`re.IGNORECASE` | `"i"` | 忽略大小写       |
| `re.L` 或 `re.LOCAL`     | `"L"` | 依赖于语言区域     |
| `re.M` 或 `re.MULTILINE` | `"m"` | 多行匹配        |
| `re.S` 或 `re.DOTALL`    | `"s"` | 点号匹配所有字符    |
| `re.U` 或 `re.UNICODE`   | `"u"` | Unicode 匹配  |
| `re.X`, 或`re.VERBOSE`   | `"x"` | 详细          |
| `re.NOFLAG`             |       | 不使用任何标志     |

> [!tip] 
> 
> 这些标准与在 [[正则表达式#分组]] 中介绍内联字符 `"a" "i" "L" "m" "s" "u" "x"` 对应
> 
> 注意，标志 `re.X` 会忽略正则表达式中的所有空格以及 Python 中的行注释
> 


```python
>>> import re
>>> a = re.compile(r"""\d +   # the integral part
...                    \.    # the decimal point
...                     \d *  # some fractional digits""", re.X)  
>>> a.match("3.14")
<re.Match object; span=(0, 4), match='3.14'>
>>> a.match("3.14").group()
'3.14'
```

## 正则表达式对象

在标准库 `re` 中提供了一个 `Pattern` 类，代表了一个正则表达式。通常使用 `re.compile(pattern, flags=0)` 函数将 `pattern` 编译为 `Pattern` 类的实例对象

> [!tip] 
> 
> 参数 `pattern` 是由 [[正则表达式]] 语法构成的字符串。
> 
> 参数 `flags` 指定正则表达式额外的匹配模式
> 

如果需要多次使用同一个正则表达式，使用 `re.compile()` 并且保存这个正则对象以便复用，可以让程序更加高效。

> [!caution] 
> 
> 通过 `re.compile()` 编译后的正则表达式，和模块级的函数会被缓存， 所以少数的正则表达式使用无需考虑编译的问题。
> 

`Pattern` 类的实例对象提供了一系列的方法用于匹配字符串

| 方法                           | 描述                                  |
| :--------------------------- | :---------------------------------- |
| `Pattern.search(string)`     | 扫描整个 `string`， 并返回第一个匹配的 `Match` 对象 |
| `Pattern.match(string)`      | `string` 的开头匹配，返回 `Match` 对象        |
| `Pattern.fullmatch(string)`  | 整个 `string` 匹配时，返回 `Match` 对象       |
| `Pattern.split(string)`      | 在匹配位置分开 `string`，返回一个列表             |
| `Pattern.findall(string)`    | 返回正则表达式在 `string` 中的所有非重叠匹配         |
| `Pattern.finditer(string)`   | 与 `findall()` 类似，只是返回迭代器            |
| `Pattern.sub(repl, string)`  | 将 `string` 中匹配的子串替换为 `repl`         |
| `Pattern.subn(repl, string)` | 类似于 `sub()`，只是一个元组 `(字符串, 替换次数)`    |

下面我们重点介绍几个常用的方法

### search() 和 match() 方法

这两个方法都是在 `string` 中匹配正则表达式。只是它们的含义有些不同

对于 `search()` 方法而言，它会在整个 `string` 中匹配，并返回第一个比配成功的 `Match` 对象；匹配失败则返回 `None`

对于 `match()` 方法，它会从 `string` **开始**位置进行匹配，如果匹配成功则返回 `Match` 对象，否则返回 `None`

> [!tip] 
> 
> 这个两个方法还有两个额外的参数 `pos` 和 `endpos`，它们分别自定从 `string` 中开始搜索和结束搜索的位置 
> 

```python
>>> import re
>>> pattern = re.compile(r"(\d+)")  # 至少 1 个十进制数码
>>> pattern.search("hello, 123, world")
<re.Match object; span=(7, 10), match='123'>

>>> print(pattern.match("hello, 123"))  # 匹配失败，返回 None
None

>>> print(pattern.match("hello, 123", 7))  # 指定开始匹配的位置
<re.Match object; span=(7, 10), match='123'>
```

### split() 方法

`Pattern.split(string)` 方法在正则表达式 _匹配_ 的位置对 `string` 进行分割，返回一个字符串列表

```python
>>> pattern = re.compile(r"\W+")  # 匹配任何非单词或数字字符
>>> pattern.split("hello, world") 
['hello', 'world']
```

> [!tip] 
> 
> 正则表达式 `\W+` 匹配至少 $1$ 个非字母或数字字符。上述示例将匹配 `", "`
> 
  
如果在正则表达式中存在捕获组，那么所有的组里的文字也会包含在列表里

```python
>>> pattern = re.compile(r"(\W+)")  # 匹配任何非单词或数字字符
>>> pattern.split("hello, world")
['hello', ', ', 'world']
```

### findall() 方法

`Pattern.findall(string)` 从 `string` 中搜索  `pattern` 所有 **非重叠匹配**，以字符串列表或字符串元组列表的形式。对 `string` 的扫描从左至右，匹配结果按照找到的顺序返回。 **空匹配也包括在结果中**。

返回结果取决于模式中捕获组的数量。如果没有分组，返回与整个模式匹配的字符串列表。如果有且仅有一个组，返回与该组匹配的字符串列表。如果有多个组，返回与这些组匹配的字符串元组列表。非捕获组不影响结果。

```python
>>> pattern = re.compile(r'\bf[a-z]*')  # 没有分组的情形
>>> pattern.findall('which foot or hand fell fastest')
['foot', 'fell', 'fastest']

>>> pattern = re.compile(r'(\w+)=(\d+)')
>>> pattern.findall('set width=20 and height=10')
[('width', '20'), ('height', '10')]
```

## Match 对象

正则表达式对象的 `search()` 和 `match()` 以及  `fullmatch()` 在正则表达式匹配成功时会返回 `Match` 对象，它代表了一个成功的匹配。

`Match` 对象提供了许多方法用于操作匹配的结果

| 方法                          | 描述                    |
| :-------------------------- | :-------------------- |
| `Match.expand(template)`    | 扩展 `template`         |
| `Match.group([group, ...])` | 获取捕获组的结果              |
| `Match.groups()`            | 获取所有捕获组的结果            |
| `Match.groupdict()`         | 获取所有命名分组的结果，以字典返回     |
| `Match.start([group])`      | 获取 `group` 组的开始位置     |
| `Match.end([group])`        | 获取 `group` 组的结束位置     |
| `Match.span([group])`       | 返回 `(start(), end())` |
| `Match.lastindex¶`          | 捕获组的最后一个匹配的整数索引值      |

## 模块级函数

通常我们直接使用模块级别的函数，这些函数 **本质上就是调用的 正则表达式对象的方法**，具有与正则表达式对象的方法相同的名字。因此，在使用这些函数是，需要将一个正则表达式传递给这些函数

### search() 和 match() 函数

这两个函数本质上就是去调用 `Pattern.search()` 和 `Pattern.match()`。

```python
>>> re.search(r"(\d+)", "hello, 123, world")
<re.Match object; span=(7, 10), match='123'>
```

### split() 方法

这个函数本质上就是去调用 `Pattern.split()` 

```python
>>> re.split(r"(\W+)", "hello, world")
['hello', ', ', 'world']
```

### findall() 方法

这个函数本质上就是去调用 `Pattern.findall()`

```python
>>> re.findall(r"(\w+)=(\d+)", "set width=20 and height=10")
[('width', '20'), ('height', '10')]
```
