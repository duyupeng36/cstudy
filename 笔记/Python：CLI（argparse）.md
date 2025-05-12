# argparse

在 [[Linux：命令和 vim]] 中我们学习过一个命令 `ls`，它用于列出指定目录下的所有内容。

```shell
➜  ~ ls
github.com  go
➜  ~ ls -al ./
total 348
drwxr-x--- 15 dyp  dyp    4096 May 11 22:58 .
drwxr-xr-x  3 root root   4096 Apr 27 21:49 ..
-rw-------  1 dyp  dyp    3316 May  5 00:44 .bash_history
-rw-r--r--  1 dyp  dyp     220 Apr 27 21:49 .bash_logout
-rw-r--r--  1 dyp  dyp    3807 May  8 21:31 .bashrc
drwx------  8 dyp  dyp    4096 May  8 21:27 .cache
drwx------  5 dyp  dyp    4096 May  7 22:09 .config
drwxr-xr-x  6 dyp  dyp    4096 May  9 19:55 .docker
drwxr-xr-x  3 dyp  dyp    4096 May  7 23:09 .dotnet
```

其中 `ls` 表示命令的名字，而其后面的是 `-al` 则是命令的 **选项参数**；而 `./` 是命令的 **位置参数**

> [!attention] 
> 
> 选项和参数的位置可能发生交换。此外，某些命令还有 **子命令**。
> 

下面我们来看 Python 提供了的用于支持 CLI 程序开发的标准库 `argparse` 

> [!tip] 
> 
> `argparse` 模块让编写用户友好的命令行接口变得容易。 程序定义它需要哪些参数，`argparse` 将会知道如何从 [`sys.argv`](https://docs.python.org/zh-cn/3.13/library/sys.html#sys.argv "sys.argv") 解析它们。
> 
> 此外，`argparse` 模块还能自动生成帮助和用法消息文本。 该模块还会在用户向程序传入无效参数时发出错误提示。
> 

## ArgumentParser 

`argparse` 模块对命令行界面的支持是围绕 `argparse.ArgumentParser` 实例建立的。 它是一个用于 **参数规格说明的容器** 并包含应用于 **解析器的一组选项**:

```python
parser = argparse.ArgumentParser(
                    prog='',  # 应用程序的名字
                    description='',  # 描述信息
                    epilog='',  # 选项后的描述信息
                    )
```

在实例化 `ArgumentParser` 时，通常需要提供 $3$ 个参数

| 参数            | 描述                     |
| :------------ | :--------------------- |
| `prog`        | 程序名字，默认为 `sys.argv[0]` |
| `description` | 程序的描述信息                |
| `epilog`      | 位于选项之后的额外程序描述信息        |

> [!tip] 
> 
> 通常情况下 `description` 是必须提供的
> 

### 解析参数

要求命令从 `sys.argv` 中解析参数时需要调用 `ArgumentParser.parse_args(args=None, namespace=None)` 方法。该方法将参数字符串转换为对象并将其设为命名空间的属性。 返回带有成员的 `Namespace` 对象

| 参数          | 描述                                 |
| :---------- | :--------------------------------- |
| `args`      | 待解析的字符串列表，默认使用 `sys.argv`          |
| `namespace` | 用于获取属性的名字空间。默认是一个空的 `Namespace` 对象 |

> [!attention] 
> 
> `parse_args(args, namespace)` 会从 `args` 中解析在调用 `parse_args()` 之前对 [`add_argument()`](https://docs.python.org/zh-cn/3.13/library/argparse.html#argparse.ArgumentParser.add_argument "argparse.ArgumentParser.add_argument") 的调用决定了哪些对象会被创建以及它们如何被赋值。

下面我们介绍 `add_argument()` 方法

### 添加参数

命令的参数可以为两类：**位置参数** 和 **选项参数**

+ **位置参数**：位置参数通常是命令需要操作的目标
+ **选项参数**：控制程序在操作目标时的行为，以及其他可选操作

要给命令添加参数就需要使用 `ArgumentParser.add_argument()` 方法，该方法接受下表列出的参数

| 参数              | 描述                            |
| :-------------- | :---------------------------- |
| `name or flags` | 一个 **名称** 或是由 **选项字符串** 组成的列表 |
| `action`        | 当参数在命令行中出现时使用的动作基本类型          |
| `nargs`         | 命令行参数应当消耗的数目                  |
| `const`         | 被 `action` 和 `nargs` 选择所需求的常数 |
| `default`       | 参数的默认值                        |
| `type`          | 命令行参数应当被转换成的类型                |
| `choices`       | 允许使用的参数值                      |
| `required`      | 参数是否可省略                       |
| `help`          | 参数的简短描述                       |
| `metavar`       | 参数值示例                         |
| `dest`          | 被添加到 `parse_args()` 返回对象上的属性名 |
| `deprecated`    | 参数是否被弃用                       |

下面我们详细介绍几个比较重要的参数

#### name or flags

`add_argument()` 方法必须知道是要接收一个 **可选参数(选项)**，如 `-f` 或 `--foo`，还是一个 **位置参数**，如由文件名组成的列表。 因此首先传递给 `add_argument()` 的参数必须是一组 **旗标**，或一个简单的 **参数名称**。

例如，假设我们想要实现 Unix/Linux 命令 `ls

```python
>>> import argparse
>>> parser = argparse.ArgumentParser(
...     prog="ls", # 应用程序名字
...     description="List information about the FILES(the current director by default)."
... )
```

可以这样创建可选参数

```python
parser.add_argument("-l", "--list")
```

而位置参数可以这么创建

```python
>>> parser.add_argument("file")
```

当 `parse_args()` 被调用，选项会以 `-` 前缀识别，剩下的参数则会被假定为位置参数

```python
>>> parser.parse_args("-l L file".split())
Namespace(file='file', l='L')
```

#### action

`ArgumentParser` 对象将命令行参数与动作相关联。这些动作可以做与它们相关联的命令行参数的任何事，尽管大多数动作只是简单的向 `parse_args()` 返回的对象上添加属性。`action` 命名参数指定了这个命令行参数应当如何处理。供应的动作有：

| action         | 描述                                   |
| :------------- | :----------------------------------- |
| `store`        | 存储参数的值。默认行为                          |
| `store_const`  | 存储由 `const` 关键参数指定的值                 |
| `store_true`   | `store_const` 的特例，用于存储 `True`        |
| `store_false`  | `store_const` 的特例，用于存储 `False`       |
|                |                                      |
| `append`       | 存储一个列表。运行多次指定选项的值                    |
| `append_const` | 并将由 `const` 关键字参数指定的值添加到列表           |
|                |                                      |
| `extend`       | 存储一个列表并添加来自与多值参数列表的每个条目              |
|                |                                      |
| `count`        | 计算关键字参数出现的数目或次数                      |
|                |                                      |
| `help`         | 打印当前解析器中的选项和参数的完整帮助信息，然后退出           |
|                |                                      |
| `version`      | 期望有一个 `version=` 命名参数，并打印版本信息并在调用后退出 |

> [!tip] 
> 
>  `'extend'` 动作通常配合 `nargs` 关键字参数值 `'+'` 或 `'*'` 使用。
>  
>  请注意当 `nargs` 为 `None` (默认值) 或 `'?'` 时，将把参数字符串中的每个字符添加到列表
> 

##### store_const

下面的示例演示了 `"store_const"` 的行为。它会将 `const` 关键字参数的值存储在 `Namespace` 对象中

```python
>>> parser = argparse.ArgumentParser()
>>> parser.add_argument('--foo', action='store_const', const=42)
>>> parser.parse_args(['--foo'])
Namespace(foo=42)
```

##### append

下面的示例演示了 `"append"` 的行为。它会将同一个选项的多个参数值存储在一个列表中，并将这个列表存储在 `Namespace` 对象中

```python
>>> parser = argparse.ArgumentParser()
>>> parser.add_argument('--foo', action='append')
>>> parser.parse_args('--foo 1 --foo 2'.split())  # 同一个选项的多个值
Namespace(foo=['1', '2'])
```

##### extend

下面的示例演示了 `"extend"` 的行为。它会将选项后面的 `nargs` 个参数保存在列表中，并将这个列表存储在 `Namespace` 对象中

```python
>>> parser = argparse.ArgumentParser()
>>> parser.add_argument("--foo", action="extend", nargs="+", type=str)
>>> parser.parse_args(["--foo", "f1", "--foo", "f2", "f3", "f4"])
Namespace(foo=['f1', 'f2', 'f3', 'f4'])

>>> parser = argparse.ArgumentParser()
>>> parser.add_argument("--foo", action="extend", nargs="+", type=str)
>>> parser.parse_args(["--foo", "f2", "f3", "f4"])
Namespace(foo=['f2', 'f3', 'f4'])
```

#### nargs

`ArgumentParser` 对象通常会将一个单独的命令行参数关联到一个单独的要执行的动作。 `nargs` 关键字参数会将不同数量的命令行参数关联到一个单独的动作。 受支持的值有：

| nargs | 描述                  |
| :---- | :------------------ |
| `N`   | 从命令行参数中消耗 `N` 个参数   |
| `?`   | 从命令行参数中消耗 $1$ 个参数   |
| `'*'` | 从命令行参数中消耗至少 $0$ 个参数 |
| `'+'` | 从命令行参数中消耗至少 $1$ 个参数 |

> [!tip] 
> 
> 此处的命令行参数是排除选项字符串以外的参数
> 

##### nargs=N

命令行中的 `N` 个参数会被聚集到一个列表中

```python
>>> parser = argparse.ArgumentParser()
>>> parser.add_argument('--foo', nargs=2)
>>> parser.add_argument('bar', nargs=1)
>>> parser.parse_args('c --foo a b'.split())
Namespace(foo=['a', 'b'], bar=['c'])
```

> [!tip] 
> 
> 注意观察输出的结果。`nargs=1` 会产生一个单元素列表。这和默认的元素本身是不同的
> 

##### nargs="?"

如果可能的话，会从命令行中消耗一个参数，并产生一个单独项。 如果当前没有命令行参数，将会产生 `default` 值。


> [!attention] 
> 
> 注意对于可选参数来说，还有一个额外情况：出现了选项字符串但没有跟随命令行参数，在此情况下将会产生 `const` 值。 一些说明这种情况的例子如下
> 

```python
>>> parser = argparse.ArgumentParser()
>>> parser.add_argument('--foo', nargs='?', const='c', default='d')
>>> parser.add_argument('bar', nargs='?', default='d')

>>> # 选项有命令行参数
>>> parser.parse_args(['XX', '--foo', 'YY'])
Namespace(foo='YY', bar='XX')

>>> # 出现选项但是没有跟随命令行参数：使用 const 值
>>> parser.parse_args(['XX', '--foo'])
Namespace(foo='c', bar='XX')

>>> # 没有选项：使用 default 值
>>> parser.parse_args([])
Namespace(foo='d', bar='d')
```

`nargs='?'` 的一个更普遍用法是 **允许可选的输入或输出文件**

```python
>>> parser = argparse.ArgumentParser()
>>> parser.add_argument('infile', nargs='?', type=argparse.FileType('r'),
...                     default=sys.stdin)
>>> parser.add_argument('outfile', nargs='?', type=argparse.FileType('w'),
...                     default=sys.stdout)

>>> # 使用参数创建文件对象
>>> parser.parse_args(['input.txt', 'output.txt'])
Namespace(infile=<_io.TextIOWrapper name='input.txt' mode='r' encoding='UTF-8'>, outfile=<_io.TextIOWrapper name='output.txt' mode='w' encoding='UTF-8'>)

>>> # 使用默认的文件对象
>>> parser.parse_args([])
Namespace(infile=<_io.TextIOWrapper name='<stdin>' mode='r' encoding='utf-8'>, outfile=<_io.TextIOWrapper name='<stdout>' mode='w' encoding='utf-8'>)
```

##### args="`*`"

所有当前命令行参数被聚集到一个列表中。

```python
>>> parser = argparse.ArgumentParser()
>>> parser.add_argument('--foo', nargs='*')
>>> parser.add_argument('--bar', nargs='*')
>>> parser.add_argument('baz', nargs='*')
>>> parser.parse_args('a b --foo x y --bar 1 2'.split())
Namespace(foo=['x', 'y'], bar=['1', '2'], baz=['a', 'b'])
```

> [!attention] 
> 
> 注意通过 `nargs='*'` 来实现多个位置参数通常没有意义，但是多个选项是可能的
> 

##### args="+"

和 `'*'` 类似，所有当前命令行参数被聚集到一个列表中。另外，当前没有至少一个命令行参数时会产生一个错误信息

```python
>>> parser = argparse.ArgumentParser(prog='PROG')
>>> parser.add_argument('foo', nargs='+')
>>> parser.parse_args(['a', 'b'])
Namespace(foo=['a', 'b'])

>>> parser.parse_args([])
usage: PROG [-h] foo [foo ...]
PROG: error: the following arguments are required: foo
```

#### type

**默认情况下，解析器会将命令行参数当作简单字符串读入**。 然而，命令行字符串经常应当被解读为其他类型，例如 `float` 或 `int`。 `add_argument()` 的 `type` 关键字允许执行任何必要的类型检查和类型转换

```python
import argparse
import pathlib

parser = argparse.ArgumentParser()
parser.add_argument('count', type=int)
parser.add_argument('distance', type=float)
parser.add_argument('street', type=ascii)
parser.add_argument('code_point', type=ord)
parser.add_argument('dest_file', type=argparse.FileType('w', encoding='latin-1'))
parser.add_argument('datapath', type=pathlib.Path)
```

## 子命令

许多程序都将其功能拆分为一系列子命令，例如，`svn` 程序可唤起像 `svn checkout`, `svn update` 和 `svn commit` 等子命令。 

当一个程序执行需要多组不同各类命令行参数时这种拆分功能的方式是一个非常好的主意。 `ArgumentParser` 通过 `add_subparsers()` 方法支持创建这样的子命令。 

`add_subparsers()` 方法通常不带参数地调用并返回一个特殊的动作对象。 该对象只有一个方法 `add_parser()`，它接受一个命令名称和任意多个 `ArgumentParser` 构造器参数，并返回一个可以通常方式进行修改的 `ArgumentParser` 对象。

```python
import argparse

parser = argparse.ArgumentParser(
    prog="git"
)

# 创建子命令 parser
subparser = parser.add_subparsers()

# 子命令 pull 的解析器
pull_parser = subparser.add_parser("pull", help="git pull")
pull_parser.add_argument("origin", help="origin of registry")

# 子命令 push 的解析器
push_parse = subparser.add_parser("push", help="git push")
push_parse.add_argument("origin", help="origin of registry", default="origin")
push_parse.add_argument("branch", help="branch of registry", default="master")

namerspace = parser.parse_args()

print(namerspace)
```
