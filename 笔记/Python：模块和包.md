# 模块和包

退出 Python 解释器后，再次进入时，之前在 Python 解释器中定义的函数和变量就丢失了。因此，编写较长程序时，最好用文本编辑器代替解释器，执行文件中的输入内容，这就是编写 **脚本** 。

随着程序越来越长，为了方便维护，**最好把脚本拆分成多个文件**。编写脚本还一个好处，不同程序调用同一个函数时，不用把函数定义复制到各个程序。

为实现这些需求，Python 把各种定义存入一个文件，在脚本或解释器的交互式实例中使用。这个文件就是 **模块** ；模块中的定义可以 **导入** 到其他模块或 _主_ 模块（在顶层和计算器模式下，执行脚本中可访问的变量集）。

模块是包含 Python 定义和语句的文件。其文件名是模块名加后缀名 `.py` 。在模块内部，通过全局变量 `__name__` 可以获取模块名，这是一个字符串。例如，用文本编辑器在当前目录下创建 `fibo.py` 文件，输入以下内容

```python title:fibo.py
# 斐波那契数列模块

def fib(n):    # 打印斐波那契数列直到 n
    a, b = 0, 1
    while a < n:
        print(a, end=' ')
        a, b = b, a+b
    print()

def fib2(n):   # 返回斐波那契数列直到 n
    result = []
    a, b = 0, 1
    while a < n:
        result.append(a)
        a, b = b, a+b
    return result
```

现在，进入 Python 解释器，用以下命令导入该模块

```shell
➜ pyfiles nvim fibo.py
➜ pyfiles python
Python 3.13.2 (tags/v3.13.2:4f8bb39, Feb  4 2025, 15:23:48) [MSC v.1942 64 bit (AMD64)] on win32
Type "help", "copyright", "credits" or "license" for more information.
>>> import fibo
>>>
```

> [!tip] 
> 
> 模块本质上就是一个 `.py` 脚本文件。模块可以被导入到其他模块中使用
> 

`import` 关键字不会直接把 `fibo` 中定义的函数名称添加到当前 `namespace` 中（请参阅 [[Python：函数基础#名字空间与作用域]] ）；它只是将模块名称 `fibo` 添加到那里。 使用该模块名称你可以访问其中的函数

```python
>>> fibo.fib(10)  # 通过模块名访问模块中的对象
0 1 1 2 3 5 8
>>> fibo.fib2(10)
[0, 1, 1, 2, 3, 5, 8]
>>> fibo.__name__
'fibo'
```

如果经常使用某个对象，可以把它赋值给局部变量

```python
>>> fib = fibo.fib
>>> fib(20)
0 1 1 2 3 5 8 13
```

## 模块详解

模块包含可执行语句及函数定义。这些语句用于初始化模块，且仅在 `import` 语句 **第一次** 遇到模块名时执行

> [!tip] 
> 
> 请注意：文件作为脚本运行时，也会执行这些语句。
> 

**每个模块都有自己的私有命名空间**，它会被用作模块中定义的所有函数的全局命名空间。 因此，模块作者可以在模块内使用全局变量而不必担心与用户的全局变量发生意外冲突。 另一方面，如果你知道要怎么做就可以通过与引用模块函数一样的标记法 `modname.itemname` 来访问一个模块的全局变量。

**模块可以导入其他模块**。 根据惯例可以将所有 `import` 语句都放在模块（或者也可以说是脚本）的 **开头** 但这并非强制要求。 如果被放置于一个模块的最高层级，则被导入的模块名称会被添加到该模块的全局命名空间

> [!tip] 
> 
> 在哪个名字空间中导入，关联模块对象的模块名就会被添加到该模块中对应的名字空间中
> 

`import` 语句还有另一种形式，即 `from ... import ...` 的方式直接将模块中的名称导入到导入方模块的命名空间中

```python
>>> from fibo import fib, fib2
>>> fib(500)
0 1 1 2 3 5 8 13 21 34 55 89 144 233 377
```

> [!attention] 
> 
> `from fibo import fib, fib2` 只会从模块 `fibo` 中导入名字 `fib, fib2` 到当前名字空间中
> 

如果需要导入模块中的所有名字，可以使用 `from ... import *` 的方式导入

```python
>>> from fibo import *
>>> fib(500)
0 1 1 2 3 5 8 13 21 34 55 89 144 233 377
```

> [!attention] 
> 
> `from ... import *` 会导入模块中所有不以下划线 (`_`) 开头的名字。因为，以下划线(`_`) 开头的名字在 Python 约定为被保护的名字，不应该对外使用
> 

在导入模块时，可以使用 `as` 语句给模块取别名

```python
>>> import fibo as fib  # 别名
>>> fib.fib(500)
0 1 1 2 3 5 8 13 21 34 55 89 144 233 377

>>> from fibo import fib as fibonacci, fib2 as fibonacci2
>>> fibonacci(500)
0 1 1 2 3 5 8 13 21 34 55 89 144 233 377
>>> fibonacci2(500)
[0, 1, 1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144, 233, 377]
```

> [!note] 
> 
> 为了保证运行效率，**每次解释器会话只导入一次模块**。如果更改了模块内容，必须重启解释器；仅交互测试一个模块时，也可以使用 `importlib.reload()`，例如 `import importlib; importlib.reload(modulename)`。
> 

### 以脚本方式执行模块

可以用以下方式运行 Python 模块

```shell
python fibo.py <arguments>
```

Python 会读取 `fibo.py` 中的代码并执行，和导入模块一样，但会把 `__name__` 赋值为 `"__main__"`。 也就是把下列代码添加到模块末尾：

```python
if __name__ == "__main__":
    import sys
    fib(int(sys.argv[1]))
```

这个文件既能被用作脚本，又能被用作一个可供导入的模块，因为解析命令行参数的那两行代码只有在模块作为 `"__main__"` 文件执行时才会运行

```shell
@duyup ➜ pyfiles python .\fibo.py 500
0 1 1 2 3 5 8 13 21 34 55 89 144 233 377
```

当这个模块被导入到其它模块时，那两行代码不运行

```python
>>> import fibo
>>>
```

这常用于为模块提供一个便捷的用户接口，或用于测试（把模块作为执行测试套件的脚本运行）。

### 模块搜索路径

当导入一个名为 `spam` 的模块时，它将在变量 `sys.path` 所给出的目录列表中搜索名为 `spam.py` 的文件。 `sys.path` 是从这些位置初始化的:

+ 被命令行直接运行的脚本所在的目录（或未指定文件时的当前目录）
+ `PYTHONPATH` （目录列表，与 `shell` 变量 `PATH` 的语法一样）
+ 依赖于安装的默认值（按照惯例包括一个 `site-packages` 目录，由 `site` 模块处理）。

```python
>>> import sys
>>> sys.path
['', '/usr/lib/python312.zip', '/usr/lib/python3.12', '/usr/lib/python3.12/lib-dynload', '/usr/local/lib/python3.12/dist-packages', '/usr/lib/python3/dist-packages']
```

`sys.path` 的第一个元素是被命令行直接运行的脚本所在的目录，换句话说，就是模块首先搜索的位置是被 Python 执行脚本的目录。

> [!warning] 
> 
> 警告：在为 Python 模块命名时，请注意不要与标准库同名，否则会隐藏标准库
> 

## 包

包是通过使用 **"带点号模块名"** 来构造 Python 模块命名空间的一种方式。 例如，模块名 `A.B` 表示名为 `A` 的包中名为 `B` 的子模块。就像使用模块可以让不同模块的作者不必担心彼此的全局变量名一样，使用带点号模块名也可以让 `NumPy` 或 `Pillow` 等多模块包的作者也不必担心彼此的模块名冲突

假设要为统一处理声音文件与声音数据设计一个模块集（“包”）。声音文件的格式很多（通常以扩展名来识别，例如：`.wav`，`.aiff`，`.au`），因此，为了不同文件格式之间的转换，需要创建和维护一个不断增长的模块集合。为了实现对声音数据的不同处理（例如，混声、添加回声、均衡器功能、创造人工立体声效果），还要编写无穷无尽的模块流。下面这个分级文件树展示了这个包的架构：

```shell
sound/                          最高层级的包
      __init__.py               初始化 sound 包
      formats/                  用于文件格式转换的子包
              __init__.py
              wavread.py
              wavwrite.py
              aiffread.py
              aiffwrite.py
              auread.py
              auwrite.py
              ...
      effects/                  用于音效的子包
              __init__.py
              echo.py
              surround.py
              reverse.py
              ...
      filters/                  用于过滤器的子包
              __init__.py
              equalizer.py
              vocoder.py
              karaoke.py
              ...
```

导入包时，Python 搜索 `sys.path` 里的目录，查找包的子目录。

**需要有 `__init__.py` 文件才能让 Python 将包含该文件的目录当作包来处理**（除非使用 [namespace package](https://docs.python.org/zh-cn/3.13/glossary.html#term-namespace-package)，这是一个相对高级的特性）。 这可以防止重名的目录如 `string` 在无意中屏蔽后继出现在模块搜索路径中的有效模块。 在最简单的情况下，`__init__.py` 可以只是一个空文件，但它也可以执行包的初始化代码或设置 `__all__` 变量，这将在稍后详细描述。

从包中导入单个模块时，可以使用 `import` 语句。例如：

```python
import sound.effects.echo
```

这将加载子模块 `sound.effects.echo`。在使用它必须通过其全名来引用。

```python
sound.effects.echo.echofilter(input, output, delay=0.7, atten=4)
```

另一种导入子模块的方法是使用 `from ... import ...` 语句：

```python
from sound.effects import echo
```

这也会加载子模块 `echo`，并使其不必加包前缀，因此可按如下方式使用:

```python
echo.echofilter(input, output, delay=0.7, atten=4)
```

Import 语句的另一种变体是直接导入所需的函数或变量：

```python
from sound.effects.echo import echofilter
```

同样，这将加载子模块 `echo`，但这使其函数 `echofilter()` 直接可用:

```python
echofilter(input, output, delay=0.7, atten=4)
```

> [!warning] 
> 
> 注意，使用 `from package import item` 时，`item` 可以是包的子模块（或子包），也可以是包中定义的函数、类或变量等其他名称。
> 
> `import` 语句首先测试包中是否定义了 `item`；如果未在包中定义，则假定 `item` 是模块，并尝试加载。如果找不到 `item`，则触发 [`ImportError`](https://docs.python.org/zh-cn/3.13/library/exceptions.html#ImportError "ImportError") 异常
> 
> 相反，使用 `import item.subitem.subsubitem` 句法时，除最后一项外，每个 item 都必须是包；最后一项可以是模块或包，但不能是上一项中定义的类、函数或变量。
> 

### 从包中导入 *

使用 `from sound.effects import *` 时会发生什么？你可能希望它会查找并导入包的所有子模块，但事实并非如此。因为这将花费很长的时间，并且可能会产生你不想要的副作用，如果这种副作用被你设计为只有在导入某个特定的子模块时才应该发生。

唯一的解决办法是提供包的显式索引。[`import`](https://docs.python.org/zh-cn/3.13/reference/simple_stmts.html#import) 语句使用如下惯例：如果包的 `__init__.py` 代码定义了列表 `__all__`，运行 `from package import *` 时，它就是被导入的模块名列表。发布包的新版本时，包的作者应更新此列表。

如果包的作者认为没有必要在包中执行导入 * 操作，也可以不提供此列表。例如，`sound/effects/__init__.py` 文件可以包含以下代码：

```python title:sound/effects/__init__.py
__all__ = ["echo", "surround", "reverse"]
```

这意味着 `from sound.effects import *` 将导入 `sound.effects` 包的三个命名子模块。

请注意子模块可能会受到本地定义名称的影响。 例如，如果你在 `sound/effects/__init__.py` 文件中添加了一个 `reverse` 函数，`from sound.effects import *` 将只导入 `echo` 和 `surround` 这两个子模块，但 **不会** 导入 `reverse` 子模块，因为它被本地定义的 `reverse` 函数所遮挡:

```python title:sound/effects/__init__.py
__all__ = [
    "echo",      # 指向 'echo.py' 文件
    "surround",  # 指向 'surround.py' 文件
    "reverse",   # !!! 现在指向 'reverse' 函数 !!!
]

def reverse(msg: str):  # <-- 此名称将覆盖 'reverse.py' 子模块
    return msg[::-1]    #     针对 'from sound.effects import *' 的情况
```

如果没有定义 `__all__`，`from sound.effects import *` 语句 _不会_ 把包 `sound.effects` 中的所有子模块都导入到当前命名空间；它只是确保包 `sound.effects` 已被导入（可能还会运行 `__init__.py` 中的任何初始化代码），然后再导入包中定义的任何名称。 这包括由 `__init__.py` 定义的任何名称（以及显式加载的子模块）。 它还包括先前 [`import`](https://docs.python.org/zh-cn/3.13/reference/simple_stmts.html#import) 语句显式加载的包里的任何子模块。 请看以下代码:

```python
import sound.effects.echo
import sound.effects.surround
from sound.effects import *
```

在本例中，`echo` 和 `surround` 模块被导入到当前命名空间，因为在执行 `from...import` 语句时它们已在 `sound.effects` 包中定义了。 （当定义了 `__all__` 时也是如此）。

虽然，可以把模块设计为用 `import *` 时只导出遵循指定模式的名称，但仍不提倡在生产代码中使用这种做法。

> [!summary] 
> 
> 记住，使用 `from package import specific_submodule` 没有任何问题！ 实际上，除了导入模块使用不同包的同名子模块之外，这种方式是推荐用法。
> 

### 相对导入

当包由多个子包构成（如示例中的 `sound` 包）时，可以使用绝对导入来引用同级包的子模块。 例如，如果 `sound.filters.vocoder` 模块需要使用 `sound.effects` 包中的 `echo` 模块，它可以使用 `from sound.effects import echo`。

你还可以编写相对导入代码，即使用 `from module import name` 形式的 import 语句。 这些导入使用前导点号来表示相对导入所涉及的当前包和上级包。 例如对于 `surround` 模块，可以使用:

```python
from . import echo
from .. import formats
from ..filters import equalizer
```

> [!attention] 
> 
> 注意，**相对导入基于当前模块名**。因为主模块名永远是 `"__main__"` ，所以如果计划将一个模块用作 Python 应用程序的主模块，那么该模块内的导入语句必须始终使用绝对导入。
> 
> 请记住：主模块中的导入语句必须使用绝对导入
> 
