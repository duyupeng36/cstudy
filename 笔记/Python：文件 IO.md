# 文件

## 文件描述符

Python 在标准库 `os` 中提供操作文件描述符的系统调用接口。这些接口与 [[文件 IO：通用 IO 模型]] 中介绍的系统调用非常相似，可以无障碍的使用 `os` 标准库中与文件相关的操作

```python
import os
import stat

# 打开文件
fd = os.open("test.txt", os.O_RDWR | os.O_CREAT, stat.S_IRUSR | stat.S_IWUSR | stat.S_IRGRP | stat.S_IROTH)

# 读数据
buffer = os.read(fd, 1024)

print(buffer)

# 写数据
os.write(fd, buffer)

os.fdatasync(fd) # 同步 IO 数据完整性完成

# 关闭文件
os.close(fd)
```

> [!tip] 
> 
> 标准库 `stat` 提供了一些常量用用于表示文件类型和权限，参考 [[文件属性#文件类型及权限]]
> 

## 文件对象

Python 将文件的相关操作封装成了文件对象。在 Python 中我们通常使用文件对象操作文件，而不是通过文件描述符。

在 Python 中，使用内置函数 `open()` 用于打开文件并返回一个文件对象

```python
open(file, mode='r', buffering=-1, encoding=None, errors=None, newline=None, closefd=True, opener=None)
```

> [!tip] 参数 `file`
> 
> 参数 `file` 是一个 `path-like` 对象，代表一个文件系统路径的对象。可以是表示路径的字符串或者 `bytes` 对象；和可以是实现了 `os.PathLike` 协议的对象
> 
> `os.PathLike` 是表示文件系统中的路径的抽象基类。例如 `pathlib.PurePath` 就实现了这个抽象基类
> 
> 参数 `file` 还可以是一个代表打开文件的文件描述符
> 

> [!tip] 参数 `mode`
> 
> 参数 `mode` 是一个字符串，用于指明文件打开模式。下表列出了 `mode` 字符串的取值
> 

| 字符    | 含意                    | 对应常量                                           |
| ----- | --------------------- | ---------------------------------------------- |
| `'r'` | 读取（默认）                | `os.O_RDONLY`                                  |
| `'w'` | 写入，并先截断文件             | `os.O_WRONLY` \| `os.O_CREAT` \| `os.O_TRUNC`  |
| `'x'` | 排它性创建，如果文件已存在则失败      | `os.O_WRONLY` \| `os.O_CREAT` \| `os.O_EXEC`   |
| `'a'` | 打开文件用于写入，如果文件存在则在末尾追加 | `os.O_WRONLY` \| `os.O_CREAT` \| `os.O_APPEND` |
| `'b'` | 二进制模式                 |                                                |
| `'t'` | 文本模式（默认）              |                                                |
| `'+'` | 打开用于更新（读取与写入）         |                                                |
默认模式为 `'r'` （打开文件用于读取文本，与 `'rt'` 同义）。`'w+'` 和 `'w+b'` 模式将打开文件并清空内容。而 `'r+'` 和 `'r+b'` 模式将打开文件但不清空内容。

> [!attention]
> 
> Python 区分二进制和文本 IO。以 `'b'` 模式打开文件，Python 不会对文件进行任何解码操作。以 `'t'` 模式打开文件，Python 会使用参数 `encoding` 或者 `locale.getencoding()` 的返回值对文件内容进行解码 
> 
>  Python不依赖于底层操作系统的文本文件概念;所有处理都由Python本身完成，因此与平台无关。
> 

> [!tip] 参数 `buffering`
> 
> 设置缓冲策略。
> + `0`：无缓冲，仅在 `'b'` 模式下使用
> + `1`：行缓冲
> + `>1`：块缓冲
> 
> 以 `'b'` 模式打开的文件默认启用块缓冲，块大小默认为尝试确定底层设备的块大小并回退至 `io.DEFAULT_BUFFER_SIZE`
> 

> [!tip] 参数 `encoding`
> 
> 指定在以 `'t'` 模式打开文件时使用的字符集。请参阅 [`codecs`](https://docs.python.org/zh-cn/3.13/library/codecs.html#module-codecs "codecs: Encode and decode data and streams.") 模块获取受支持的编码格式列表
> 
> 注意：该参数只在 `'t'` 模式下使用
> 


> [!tip] 参数 `errors`
> 
> 用于指定如何处理编码和解码错误。可以指定标准错误处理程序名称，也可以使用通过 [`codecs.register_error()`](https://docs.python.org/zh-cn/3.13/library/codecs.html#codecs.register_error "codecs.register_error") 注册的任何错误处理名称。标准名称包括:
> + `"strict"`：严格处理，编码和解码错误触发 `ValueError` 异常
> + `"ignore"`：忽略错误
> + `"replace"`：将替换标记插入编码或解码错误的地方
> + `"surrogateescape"`：任何不正确的字节使用 `U+DC80` 至 `U+DCFF` 范围内的下方替代码位。在写入数据时，这些替代码位会被转换为原始字节。适用于处理未知编码格式的文件
> + `"xmlcharrefreplace"`：只在写入数据时工作。编码格式不支持的字符将被替换为相应的 XML 字符引用 `&#_nnn_;`
> + `'backslashreplace'` 用 Python 的反向转义序列替换格式错误的数据
> + `'namereplace'` （也只在编写时支持）用 `\N{...}` 转义序列替换不支持的字符。
> 

> [!tip] 参数 `newline`
> 
> 指定如何解析来自流的换行符。通常保持默认值即可
> 

> [!tip] 参数 `closefd`
> 
> 当 `file` 是文件描述符时，控制文件对象关闭时是否关闭底层的文件描述符。默认是 `False`，即不关闭底层文件描述符
> 

> [!tip] 参数 `opener`
> 
> 自定义打开器，必须返回一个文件描述符。`opener` 必须接收两个参数 `file` 和 `flags`
> 

下面是使用 `opener` 的一个场景：从给定目录中用相对路径打开文件

```python
import os  
  
dir_fd = os.open('test', os.O_RDONLY)  
  
def opener(path, flags):  
    return os.open(path, flags, dir_fd=dir_fd)  
  
file = open("test.txt", "w", opener=opener)  
  
file.write("Hello World")

file.close()

os.close(dir_fd)
```

> [!tip] 返回值
> 
> `open` 的返回值取决于参数 `mode`
> + 以 `'t'` 模式打开文件时，`open()` 返回 `io.TextIOBase` 的子类对象，具体是 `io.TextIOWrapper`
> + 以 `'b'` 模式打开文件时，`open()` 返回 `io.BufferedIOBase` 的子类。具体的子类会有多种
> 	+ `io.BufferedReader`：只读
> 	+ `io.BufferedWriter`：只写
> 	+ `io.BufferedRandom`：可读可写
> 	+ 禁用缓冲时，返回 `io.RawIOBase` 的一个子类 `io.FileIO`
> 


下表总结了 `open()` 在不同 `mode` 下的返回值

|            模式            | `file object` 的类型                          |
| :----------------------: | :----------------------------------------- |
|        `w, r, a`         | `io.TextIOBase` 的子类，具体为 `io.TextIOWrapper` |
|       `rb` 只读二进制模式       | `io.BufferedReader`                        |
|   `wb, ab` 写入和追加二进制模式    | `io.BufferedWriter`                        |
|   `r+b, w+b` 读/写二进制模式    | `io.BufferedRandom`                        |
| `buffering=0` 二进制模式且禁用缓冲 | `io.RawIOBase` 的子类 `io.FileIO`             |

### 文件对象方法

#### 检查相关的方法

`file.readable()` 和 `file.writeable()` 方法用于判断文件对象是否可读和可写

```python
file = open("hello.txt", mode="r", encoding="utf-8")   # r 模式打开，只能写

print(file.readable()) # 判断文件对象是否可
print(file.writable())  # 判断文件对象是否可写

# 输出:
# True
# False
```

`file.isatty()` 检查当前文件对象是否关联到终端。如果流是交互式的（即连接到终端/`tty`设备），则返回 `True` 

#### 从文件对象中读数据

`file.read(size)` 方法用于从文件对象 `file` 中 **读取最多 `size` 大小的内容**。如果文件对象 `file` 是 `io.BufferIOBase` 的实例(二进制文件对象)，则最多读取 `size` 字节；如果文件对象是 `io.TextIOBase` 的实例(文本文件对象)，则最多读取 `size` 个字符

| 文件对象的类型           | `size` 的单位 |
| :---------------- | ---------- |
| `io.BufferIOBase` | 字节         |
| `io.TextIOBase`   | 字符         |

> [!attention] 
> 
> 注意：当遇见 EOF，则 `file.read()` 返回空字符串或空字节
> 

---

`file.readline(size)` 方法用于从文件对象 `file` 中 **读取一行**。参数 `size` 的解释与 `file.read()` 中的相同。请注意：**一行的长度不足 `size` 则只读一行**

---

`file.readlines(hint)` 方法读用于从文件对象 `file` 中 **读取多行**。参数 `hint` 指定读取时的行为

| `hint` | 描述                                        |
| :----- | :---------------------------------------- |
| `-1`   | 读取所有行                                     |
| `>=0`  | 读取 `hint` 个字符/字节后没有遇见换行符，则继续读取，直到遇见下一个换行符 |

> [!tip] 
> 
> **文件对象时可迭代对象**。如果需要按行处理文件，请使用 `for` 循环
> 

#### 向文件对象中写入数据

`file.write(data)` 将字符串或 `bytes` 对象写入到文件对象 `file` 关联的文件中，同时返回写入的字符数(文本模式)或者字节数(二进制模式)

> [!attention] 
> 
> 请注意：`file.write()` 不会附加换行符
> 

```python
file.write("Hello, World!")

for line in lines:
    file.write(line)
```

写入其他类型的对象前，要先把它们转化为字符串（文本模式）或字节对象（二进制模式）：

```python
value = ('the answer', 42)
s = str(value)  # 将元祖转换为字符串
file.write(s)
```

---

`file.writelines(lines)` 将 `lines` 序列中的行写入到文件对象 `file` 关联的文件中。注意：不会在 `lines` 序列的每项后面附加行分隔符

```python
file.writelines(lines)  # 返回 None
```

#### 随机读写

`file.teel()` 返回当前文件偏移量。

> [!attention] 
> 
> 在文本模式下，`file.teel()` 返回的依旧是字节偏移，而不是字符偏移
> 

---

`file.seek(offset, whence=os.SEEK_SET)` 将当前文件偏移量设置为以 `whence` 为起点偏移 `offset` 个字节的位置。下表给出了 `whence` 参数的取值

| `whence`             | 描述   |
| :------------------- | :--- |
| `os.SEEK_SET` 或 `0`  | 文件开头 |
| `os.SEEK_CUR`  或 `1` | 当前位置 |
| `os.SEEK_END`  或 `2` | 文件末尾 |

> [!attention]
> 
> 在文本文件中，只允许相对于文件开头搜索，唯一有效的  `offset` 值是能从 `f.tell()` 中返回的，或 $0$。其他 `offset` 值都会产生未定义的行为
> 
> 例外情形：使用 `seek(0, 2)` 搜索到文件末尾是个例外
> 

#### 获取文件描述符

`file.fileno()` 返回底层文件描述符。如果 `IO` 对象不使用文件描述符，则会引发 `OSError` 

####  截断文件

`file.truncate(size=None)` 方法将文件对象的文件大小修改为 `size` 字节，**如果未指定 `size` 则调整至当前文件偏移位置**

## 上下文管理器

进程每打开一个文件需要占用一个打开文件描述符，这是一个有效的资源。在 Linux 上一个进程默认最多打开 $1024$ 个文件

所以，**打开一个文件使用完毕之后就应该立刻关闭**。否则，就会耗尽进程的文件描述符资源，并触发异常导致进程结束

```python
>>> filelist = []
>>> for _ in range(2000):
...     filelist.append(open("test", "w"))
...
Traceback (most recent call last):
  File "<stdin>", line 2, in <module>
OSError: [Errno 24] Too many open files: 'test'
```

Python 提供了 `with` 关键字用于执行上下文管理。Python 解释器在遇见`with` 关键字后，会将 `with` 后面的对象包装为上下文管理对象，并绑定给关键字 `as` 后的标识符

```python
with open("test") as f:  # open(test) 返回文件对象后。with语句会包装这个 文件对象，并将包装后的对象绑定给 f
	"""
	执行文件操作
	"""

# 只要退出 with 语句后，将自动调用 f 的 close 方法
##################################################
f = open("test")

with f:
	"""
	执行文件操作
	"""

# 只要退出 with 语句后，将自动调用 f 的 close 方法
```

> [!important] 
> 
> 哪怕是 `with` 语句块中的语句出错，也能保证 `close` 方法的调用 
> 

## 类文件对象

Python 标准库 `io` 提供了两个 **类文件对象**，它们不使用文件描述符，所有的操作都在内存中进行

> [!tip] 好处
> 
> 一般来说，磁盘的操作比内存的操作要慢得多，内存足够的情况下，一般的优化思路是少落地，减少磁盘IO的过程，可以大大提高程序的运行效率
> 

### StringIO：文本 IO 类文件对象


> [!tip] 
> 
> `StringIO` 是可读可写的。操作与文件非常类似，它可以用于快速构建字符串。会在内存开辟一个文件缓冲区，所有操作都在缓冲区中进行
> 

```python
import io

sio = io.StringIO()  # 创建字符io
sio.write('我是字符串io')  # 写入字符io

print(sio.getvalue())  # 读取字符io
sio.close()
```

### BytesIO：字节序列 IO 类文件对象

> [!tip] 
> 
> `BytesIO` 是可读可写的。操作与二进制文件对象非常类似。会在内存中开辟一个字节缓冲区，所有操作都在缓冲区中进行
> 

```python
import io

bio = io.BytesIO()  # 创建字节io
bio.write('hello world'.encode())  # 写入字节io

print(bio.getvalue().decode())  # 获取字节io
bio.close()
```

## 标准 IO 文件对象

Python `sys` 标准库中提供三个类文件对象

```python
from sys import stderr, stdout, stdin
```

| 标准 IO 文件对象 | 描述               |
| :--------- | :--------------- |
| `stdin`    | 标准输入：关联文件描述符 `0` |
| `stdout`   | 标准输出：关联文件描述符 `1` |
| `stderr`   | 标准错误：关联文件描述符 `2` |

> [!tip] 
> 
> `stdin` 一般只用于从中读取数据。而 `stdout` 和 `stderr` 只用于写入数据
>