# 目录

存储设备保存着数据，但是得有一种方便的模式 **让用户可以定位资源位置**，操作系统采用一种 **目录字符串** 的表达方式，这是一棵倒置的层级目录树，从根目录(`/`)开始

操作系统支持两种形象的路径：**相对路径** 和 **绝对路径**

> [!tip] 
> 
> 相对路径 **不以根目录开始的路径字符串**。默认情况下，从当前目录开始解析，也可以指定从当前目录的上级目录开始解析
> 
> 例如，`a/b` `a/b/c` 就是从当前目开始查找是否存在目录 `a`，然后在目录 `a` 中查找目录 `b` 
> 
> 此外，使用 `"."` 表示当前目录；`".."` 表示上一级目录
> 

> [!tip] 
> 
> 绝对路径是以根目录开始对的路径字符串。
> 
> 例如，`"/a/b"` `"/a/b/c"` 这种形式的路径字符串称为绝对路径
> 

有时候我们需要操作路径字符串，为了方便操作路径字符串，Python 提供了两种方式：**os.path** 和 **pathlib** 两种方式

## os.path 模块

由于不同的操作系统具有不同的路径名称约定，因此标准库中有此模块的几个版本。`os.path` 模块始终是适合 Python 运行的操作系统的路径模块，因此可用于本地路径

但是，如果操作的路径总是以一种不同的格式显示，那么也可以分别导入和使用各个模块。它们都具有相同的接口，其中适用于 Unix 样式的路径的标准库是 `posixpath`；而适用于 Windows 样式的路径的标准库是 `ntpath`

> [!tip] 
> 
> `posixpath` 用于 Unix 样式的路径
> 
> `ntpath` 用于 Windows 路径
> 

### 路径分割

我们学习的第一个路径操作函数是 `os.path.split(path)`，该函数将 `path` 拆分为`(head, tail)`，其中 `tail`是路径的最后一部分， `head` 里是除最后部分外的所有内容

> [!attention] 
> 
> 请注意：`tail` 是不包含路径分隔符的。也就是说，如果 `path` 是 `"/a/b/"`，则 `tail` 就是空字符串 `""`，`head` 就是 `"/a/b"`
> 

```python
>>> import os.path as path
>>> path.split("a/b/c/")  # tail 不包含路径分隔符
('a/b/c', '')
>>> path.split("a/b/c")
('a/b', 'c')
```

某些操作系统有驱动器的概念，每个驱动器就是一个 **挂载点**，函数 `os.path.splitdrive(path)` 用于将路径 `path` 拆分 `(drive, tail)`，其中 `drive` 是挂载点或空字符串

```python
>>> path.splitdrive("c:/dir")  # 将 path 拆分为挂载点和 tail
('c:', '/dir')
```

有时我们希望读取路径中的扩展名，函数 `os.path.splitext(path)` 用于将路径 `path` 拆分为 `(root, ext)`，其中 `ext` 是扩展名

```python
>>> path.splitext("test.txt")
('test', '.txt')
```

### 路径拼接

函数 `os.path.join(path, *paths)` 将是 `path` 和所`*paths` 中的所有成员的拼接，其中每个非空部分后面都紧跟一个目录分隔符，最后一个除外

```python
>>> path.join("/", "documents", "hello")
'/documents/hello'
```

### 获取绝对路径

函数 `os.path.abspath(path)` 用于返回标准化的 `path` 的绝对路径

```python
>>> os.path.abspath(".")
'/home/dyp'
```

### 获取路径基本名字

函数 `os.path.basename(path)` 返回 `path` 的基本名字。本质上就是 `split(path)` 返回的 `tail` 部分

```python
>>> os.path.basename("/etc/mysql/mysql.conf.d")
'mysql.conf.d'
>>> os.path.basename("/etc/mysql/mysql.conf.d/")
''
```

> [!attention] 
> 
> 这与 Unix 中的 `basename` 程序存在差异，`basename` 在 `'/foo/bar/'` 上返回 `'bar'`，而 `basename()` 函数返回一个空字符串 (`''`)。
> 

### 获取路径名

函数 `os.path.dirname(path)` 返回 `path` 的目录名称。本质上就是 `split(path)` 返回的 `head` 部分

```python
>>> os.path.dirname("/etc/mysql/mysql.conf.d/")
'/etc/mysql/mysql.conf.d'
>>> os.path.dirname("/etc/mysql/mysql.conf.d")
'/etc/mysql'
```

## pathlib 模块

从 Python3.4 开始，Python 提供了 `pathlib` 模块，该模块提供 **表示文件系统路径的类**，其语义适用于不同的操作系统。**路径类** 被分为提供纯计算操作而没有 `I/O` 的 **纯路径**，以及从纯路径继承而来但提供` I/O` 操作的 **具体路径**

![Pasted image 20250404202743|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755786080226-98b34f96e62047e9b9bf1ebe60b6cf81.png)

我们主要使用 `Path` 类，该类会根据 Python 程序运行的平台自动选择实例化 `PosixPath` 还是 `WindowsPath`

> [!attention] 
> 
> 请注意：`Path` 类是具体路径类，通过 `Path` 类的实例对象可以执行 IO 操作
> 

### 创建 Path 对象

`Path` 类在实例化时，我们可以给他提供一些初始化参数，`Path` 会对这些参数进行清理，然后构造一个 `Path` 类的实例对象

> [!tip] 
> 
> 默认情况下，`Path()` 使用当前工作目录创建一个 `Path` 类的实例
> 

下面列出来常见的 `Path` 类实例化时提供的参数

```python
>>> from pathlib import Path
>>> p = Path()  # 默认以当前目录创建 Path 对象
>>> p
WindowsPath('.')
>>> p.absolute()  # 返回当前目录的绝对路径的 Path 对象
WindowsPath('C:/Users/duyup/Documents/cstudy')

>>> p = Path("/etc/", "sysconfig")  # 绝对路径
>>> p
WindowsPath('/etc/sysconfig')
>>> p.absolute()
WindowsPath('C:/etc/sysconfig')

>>> p = Path("a/b/c")  # 当前目录下的 a/b/c
>>> p
WindowsPath('a/b/c')
>>> p.absolute()
WindowsPath('C:/Users/duyup/Documents/cstudy/a/b/c')
```

### 路径拼接

`Path` 类重载了运算符 `/` 用于路径拼接。所以使用 `Path` 拼接路径就会非常简单

```python
>>> Path() / "a" / "e"  # 在当前路径后面拼上 a/e
WindowsPath('a/e')

>>> "a/e/" / Path("config")
WindowsPath('a/e/config')
```

> [!attention] 
> 
> 请注意：在路径拼接过程中，只要出现了绝对路径，在绝对路径之前的拼接全部失效
> 

```python
>>> Path() / "a" / "/etc" / "profile.d"
WindowsPath('/etc/profile.d')
```

此外，还可以使用 `Path.joinpath(*pathsegments)` 进行路径拼接

```python
>>> Path("/etc").joinpath("passwd")
WindowsPath('/etc/passwd')
>>> Path("/etc").joinpath("init.d", "apache2")
WindowsPath('/etc/init.d/apache2')
```

### Path 实例的属性

下表列出了 `Path` 对象的一些常用属性

| 属性                         | 描述                                |
| :------------------------- | :-------------------------------- |
| `Path.parts`               | 路径字符串中的所有目录元组                     |
| `Path.parent`              | 返回当前路径的上一级目录的 `Path` 对象           |
| `Path.parents`             | 路径的逻辑祖先的不可变序列                     |
| `Path.drive`               | 路径中的驱动器盘符的字符串                     |
| `Path.root`                | 根目录的字符串                           |
| `Path.anchor`              | 驱动器盘符字符串+根目录字符串                   |
| `Path.name`                | 路径的基本名字                           |
| `Path.suffix`              | 扩展名                               |
| `Path.suffixes`            | 扩展名列表                             |
| `Path.stem`                | 除去扩展名后的最后一个路径组件                   |
| `Path.as_uri()`            | 转换为 URI 风格的路径                     |
| `Path.as_posix()`          | 转换为 POSIX 风格的路径                   |
| `Path.is_absolute()`       | 检查会否为绝对路径                         |
| `Path.match(pattern)`      | 将路径与 `pattern` 进行模式匹配，成功返回 `True` |
| `Path.with_name(name)`     | 修改基本名字                            |
| `Path.with_stem(stem)`     | 修改除去扩展名的最后的路径组件名                  |
| `Path.with_suffix(suffix)` | 修改扩展名                             |
| `Path.cwd()`               | 获取当前工作目录                          |
| `Path.home()`              | 获取当前用户的家目录                        |

### 查询文件类型和状态

下表列出了 `Path` 对象用于查看当前路径确定的文件类型和状态的方法

| 方法                       | 描述                             |
| :----------------------- | :----------------------------- |
| `Path.stat()`            | 返回 `os.stat_result` 对象，代表了文件状态 |
| `Path.lstat()`           | 类似于 `Path.stat()`，但是它不会解析链接文件  |
| `Path.exists()`          | 当前路径是否已经存在                     |
| `Path.is_file()`         | 是否是普通文件或链接文件                   |
| `Path.is_dir()`          | 是否是目录文件                        |
| `Path.is_symlink()`      | 是否是链接文件                        |
| `Path.is_junction()`     | 是否是结合点，目前只有 Windows 支持         |
| `Path.is_mount()`        | 是否是挂载点                         |
| `Path.is_socket()`       | 是否是 SOCKET 文件                  |
| `Path.is_fifo()`         | 是否是管道文件                        |
| `Path.is_block_device()` | 是否是块设备文件                       |
| `Path.is_char_device()`  | 是否是字符设备文件                      |

### Path 提供的 IO 操作

`Path` 类是一个具体路径类，它提供了 IO 操作。下表列出来 `Path` 对象提供的 IO 操作的方法

| 方法                                           | 描述                    |
| :------------------------------------------- | :-------------------- |
| `Path.open(mode='r')`                        | 打开文件，参考内置函数 `open()`  |
| `Path.read_text(encoding=None, errors=None)` | 以文本方式读取内容             |
| `Path.read_bytes()`                          | 以二进制方式读取文件内容          |
| `Path.write_text(data)`                      | 以文本方式写入数据             |
| `Path.write_bytes(data)`                     | 以二进制序列写入数据            |
|                                              |                       |
| `Path.iterdir()`                             | 遍历路径                  |
| `Path.glob(pattern)`                         | 解析路径下符合 `pattern` 的文件 |
| `Path.rglob(patern)`                         | 递归解析                  |
| `Path.walk()`                                | 遍历目录中的内容              |
|                                              |                       |
| `Path.touch()`                               | 创建文件                  |
| `Path.mkdir()`                               | 创建目录                  |
| `Path.symlink_to()`                          | 创建符号链接文件              |
| `Path.hardlink_to()`                         | 创建硬链接文件               |

