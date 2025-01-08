# 使用 Python 解释器

在 Unix/Linux 机器上，`python` 解释器通常安装在 `/usr/local/bin` 或者 `/usr/bin` 中。通常，Unix/Linux 系统会自带 Python 解释器。如果系统环境中没有 Python 解释器，可以通过下列命令进行安装

```shell
# 在 Ubuntu 中安装 Python 解释器
sudo apt install python-is-python3 python3-pip
```

安装完成后，通过 Unix/Linux shell 键入 `python` 即可启动 Python 解释器

```shell
ubuntu@ubuntu2404:~$ python
Python 3.12.3 (main, Nov  6 2024, 18:32:19) [GCC 13.2.0] on linux
Type "help", "copyright", "credits" or "license" for more information.
>>> 
```

在 **主提示符** 中，输入文件结束符(Unix 里是 Control-D，Windows 里是 Control-Z)，就会退出解释器，退出状态码为 $0$。如果不能退出，还可以输入这个命令：`quit()`。

> [!tip] 
> 
> 执行命令 `python` 之后，在显示一些提示信息之后，就会显示符号`>>>`，该符号称为 **主提示符**
> 

另一种启动解释器的方式是 `python -c command [arg] ...`，这将执行 _command_ 中的语句，相当于 shell 的 `-c` 选项。 由于 Python 语句经常包含空格或其他会被 shell 特殊对待的字符，通常建议用引号将整个 _command_ 括起来。

Python 模块也可以当作脚本使用。输入：`python -m module [arg] ...`，会执行 _module_ 的源文件，这跟在命令行把路径写全了一样。

## 交互模式

在终端输入并执行指令时，解释器在 _交互模式(interactive mode)_ 中运行。在这种模式中，会显示 _主提示符_，提示输入下一条指令，主提示符通常用三个大于号（`>>>`）表示；输入连续行时，显示 _次要提示符_，默认是三个点（`...`）。进入解释器时，首先显示欢迎信息、版本信息、版权声明，然后才是提示符

```shell
ubuntu@ubuntu2404:~$ python
Python 3.12.3 (main, Nov  6 2024, 18:32:19) [GCC 13.2.0] on linux
Type "help", "copyright", "credits" or "license" for more information.
>>> 
```

输入多行架构的语句时，要用连续行。以 `if` 为例：

```python
the_world_is_flat = True
>>> if the_world_is_flat:
...     print("Be careful not to fall off!")
...
Be careful not to fall off!
```

退出交互模式需要可以键入 `EOF` 或者调用 `quite()` 或者 `exit()` 函数

## 传入参数

解释器读取命令行参数，把脚本名与其他参数转化为字符串列表存到 `sys` 模块的 `argv` 变量里。执行 `import sys`，可以导入这个模块，并访问该列表。该列表最少有一个元素；未给定输入参数时，`sys.argv[0]` 是空字符串。给定脚本名是 `'-'` （标准输入）时，`sys.argv[0]` 是 `'-'`。使用 `-c command` 是包含目录的模块全名。解释器不处理 `-c command` 或 `-m module` 之后的选项，而是直接留在 `sys.argv` 中由命令或模块来处理。

## 解释器的运行环境

默认情况下，Python 源码文件的编码是 UTF-8。这种编码支持世界上大多数语言的字符，可以用于字符串字面值、变量、函数名及注释 。要正确显示这些字符，编辑器必须能识别 UTF-8 编码，而且必须使用支持文件中所有字符的字体。

> [!tip] 
> 
> 尽管标准库只用常规的 ASCII 字符作为变量名或函数名，可移植代码都应遵守此约定
> 

如果不使用默认编码，则要声明文件的编码，文件的 **第一行**要写成特殊注释。句法如

```python
# -*- coding: encoding -*-
```

其中，`encoding` 是 Python 支持的任意一种编码。详细内存查看 [codecs](https://docs.python.org/zh-cn/3.13/library/codecs.html#module-codecs)

_第一行_ 的规则也有一种例外情况，源码以 [UNIX "shebang" 行](https://docs.python.org/zh-cn/3.13/tutorial/appendix.html#tut-scripts) 开头。此时，编码声明要写在文件的第二行。例如

```python
#!/usr/bin/env python3
# -*- coding: cp1252 -*-
```

