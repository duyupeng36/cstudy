# Python 环境搭建

Python 是一种高级、通用、解释型编程语言。它由 Guido van Rossum 于1991 年创建，并于2008年发布了Python3 版本，是一门受欢迎的编程语言之一。

Python 以其简洁、易读和可扩展性而闻名。它提供了丰富的标准库和第三方库，使开发者能够轻松地构建各种应用，从简单的脚本到复杂的 Web 应用和科学计算

## Windows 下安装 Python 解释器

首先，我们需要访问 [Python 官网](https://python.org)，点击 `Downloads` 进入 Python 解释器的下载页面。点击 `Download Python 3.13.2` 下载最新版本的 Python 解释器安装程序。等待下载完成

![[Pasted image 20250322154232.png]]

然后，双击 `python-3.13.2-amd64.exe` 文件启动 Python 解释器安装程序

![[Pasted image 20250322154843.png]]

然后点击 `Customize installation` 自定义安装。选项 `for all users` 需要勾选。然后点击 `Next`

![[Pasted image 20250322154954.png]]

下图给出了需要选择的高级选项。红框选中的选项必须勾选

![[Pasted image 20250322155234.png]]

点击 `Install`，等待安装结束，点击 `Close` 即可

![[Pasted image 20250322155633.png]]

## Linux 下安装 Python 解释器

Linux 发行版通常都包含了 Python 解释器，位于 `/usr/bin` 或 `/usr/local/bin` 目录下。在有的发行版本中，Python 解释器的名字为 `python3`。如果没有安装，则需要使用包管理工具进行安装即可。以 Ubuntu 为例

```shell
sudo apt install python-is-python3 pip
```

## 启动和退出 Python 解释器

当安装好 Python 解释器后，我们就可以启动 Python 解释器。首先，打开命令行窗口；然后，键入 `python` 并回车。

Python 解释器会首先输出一段信息，然后打印 **主提示符(>>>)**，等待用户键入 Python 指令 

```shell
@duyup ➜ ~ python
Python 3.13.2 (tags/v3.13.2:4f8bb39, Feb  4 2025, 15:23:48) [MSC v.1942 64 bit (AMD64)] on win32
Type "help", "copyright", "credits" or "license" for more information.
>>>
```

如果 Python 解释器接收到了 `EOF(End of File)`，那么 Python 解释器就会退出。在 UNIX 系统中使用快捷键 `Ctrl + D`，在 Winodws 系统中使用快捷键 `Ctrl + Z` 向标准输入中发送 `EOF`

```shell
@duyup ➜ ~ python
Python 3.13.2 (tags/v3.13.2:4f8bb39, Feb  4 2025, 15:23:48) [MSC v.1942 64 bit (AMD64)] on win32
Type "help", "copyright", "credits" or "license" for more information.
>>> ^Z   # Ctrl + Z 退出解释器
```

如果只是想让 Python 解释器执行一条 Python 指令，可以使用 `-c` 选项：`python -c command` 

```shell
@duyup ➜ ~ python -c "print(hash('123'))"
8800125475291727598
```

启动 Python 解释器时还可以提供一个 `.py` 的脚本文件，Python 会读取 `.py` 文件中的 Python 代码并执行

```shell
@duyup ➜ ~ python .\test.py
hello world!
```

### 传递参数

解释器读取命令行参数，把脚本名与其他参数转化为字符串列表存到 `sys` 模块的 `argv` 变量里。执行 `import sys`，可以导入这个模块，并访问该列表。该列表最少有一个元素；未给定输入参数时，`sys.argv[0]` 是空字符串。给定脚本名是 `'-'` （标准输入）时，`sys.argv[0]` 是 `'-'`。使用 `-c command` 时，`sys.argv[0]` 是 `'-c'` 。如果提供了一个脚本名字，那么 `sys.argv[0]` 就是脚本的名字。解释器不处理 `-c command` 或脚本名之后的选项，而是直接留在 `sys.argv` 中由命令或模块来处理。

### 解释器运行环境

默认情况下，Python 源码文件的编码是 `UTF-8`。这种编码支持世界上大多数语言的字符，可以用于字符串字面值、变量、函数名及注释 —— 尽管标准库只用常规的 ASCII 字符作为变量名或函数名，可移植代码都应遵守此约定。要正确显示这些字符，编辑器必须能识别 UTF-8 编码，而且必须使用支持文件中所有字符的字体。

如果不使用默认编码，则要声明文件的编码，文件的 **第一 行** 要写成 **特殊注释**。句法如下：

```python
# -*- coding: encoding -*-
```

第一行的规则也有一种例外情况，源码以 **UNIX "shebang" 行** 开头。此时，编码声明要写在文件的第二行。例如：

```python
#!/usr/bin/env python3
# -*- coding: cp1252 -*-
```
