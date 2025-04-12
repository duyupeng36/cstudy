# 异常

错误可（至少）被分为两种：**_错误_** 和 **_异常_**

## 错误

**_语法错误_ 又称解析错误**，是学习 `Python` 时最常见的错误

```python
>>> while True print('Hello world')
  File "<stdin>", line 1
    while True print('Hello world')
               ^^^^^
SyntaxError: invalid synta
```

解析器会重复存在错误的行并显示一个指向该行中检测到错误的词元的 **‘箭头’**。 错误可能是由于 **所指向的词元 _之前_ 缺少某个词元而导致的。**

文件名和行号也会被打印出来以便你在输入是来自脚本时可以知道要去哪里查找问题

> [!important] 错误分类
> + **逻辑错误**: 算法写错了，例如加法写成了减法
> + **笔误**: 例如变量名写错了，**语法错误**
> + 函数或类使用错误，其实这也属于逻辑错误
> 
> 总之，**错误是可以避免的**

## 异常

**异常** 本意就是 **意外情况**。这有个前提，**没有出现上面说的错误**，也就是说 **程序写的没有问题**，但是在某些情况下，会出现一些意外，导致程序无法正常的执行下去

例如 `open()` 函数操作一个文件，文件不存在，或者创建一个文件时已经存在了，或者访问一个网络文件，突然断网了，这就是异常，是个意外的情况

> [!attention] 
> 请注意： **异常不可能避免**，即使语句或表达式使用了正确的语法，执行时仍可能触发错误。
> 
> 执行时检测到的错误称为 **_异常_**，异常不一定导致严重的后果：很快我们就能学会如何处理 Python 的异常。**大多数异常不会被程序处理，而是显示下列错误信息**
> 

```python
>>> 10 * (1 / 0)
Traceback (most recent call last):
  File "<stdin>", line 1, in <module>
ZeroDivisionError: division by zero
>>> 4 + spam*3
Traceback (most recent call last):
  File "<stdin>", line 1, in <module>
NameError: name 'spam' is not defined
>>> '2' + 2
Traceback (most recent call last):
  File "<stdin>", line 1, in <module>
TypeError: can only concatenate str (not "int") to str
```

**错误信息的最后一行说明程序遇到了什么类型的错误**。异常有不同的类型，而 **类型名称** 会作为错误信息的一部分中打印出来：上述示例中的异常类型依次是：`ZeroDivisionError`， `NameError` 和 `TypeError`。

> [!tip] 
> 
> 作为异常类型打印的字符串是发生的内置异常的名称。对于所有内置异常都是如此，但对于用户定义的异常则不一定如此（虽然这种规范很有用）。**标准的异常类型是内置的标识符（不是保留关键字）**
> 

此行其余部分根据异常类型，结合出错原因，说明错误细节。错误信息开头 **用堆栈回溯形式展示发生异常的语境**。一般会列出源代码行的堆栈回溯；但不会显示从标准输入读取的行

### 触发异常

`raise` 语句支持 **强制触发指定的异常**

```python
>>> raise NameError("Hi There")
Traceback (most recent call last):
  File "<stdin>", line 1, in <module>
NameError: Hi There
```

**`raise` 唯一的参数就是要触发的异常**。这个参数必须是 **异常实例** 或 **异常类**（派生自 `BaseException` 类，例如 `Exception` 或其子类）。如果传递的是异常类，将通过调用没有参数的构造函数来隐式实例化

```python
raise ValueError  # shorthand for 'raise ValueError()'
```

如果**只想判断是否触发了异常**，但 **并不打算处理该异常**，则可以使用更简单的 `raise` 语句重新触发异常

```python
try:
    raise NameError('HiThere')
except NameError:
    print('An exception flew by!')
    raise
```

**程序会在异常抛出的地方中断执行**，如果不捕获，就会提前结束程序(其实是终止当前线程的执行)

### 异常的处理

可以编写程序处理选定的异常。下例会要求用户一直输入内容，直到输入有效的整数，但允许用户中断程序（使用 `Control-C` 或操作系统支持的其他操作）；注意，用户中断程序会触发 `KeyboardInterrupt` 异常

```python
while True:
    try:
        x = int(input("Please enter a number:"))
        break
    except ValueError:
        print("Oops! That was no valid number. Try again...")
```

`try` 语句的工作原理如下
- 首先，执行 `try` 子句 （`try` 和 `except` 关键字之间的（多行）语句）
- 如果 _没有触发异常_，则 _跳过 `except` 子句_，`try` 语句执行完毕
- 如果在执行 `try` 子句时 _发生了异常_，则 _跳过该子句中剩下的部分_
- 如果异常类型与 `except` 关键字后指定的异常相匹配，则会执行 `except` 子句，然后 _跳到 `try/except` 代码块之后继续执行_
- 如果发生的异常与 `except` 子句 中指定的 _异常不匹配_，则它会被 _传递到外层的 `try` 语句中_
- 如果 _没有找到处理句柄_，则它是一个 未处理异常 且 _执行将停止并输出一条错误消息_

`try` 语句 可以有 **多个 `except` 子句** 来为不同的异常指定处理程序 。但最多 **只有一个处理程序会被执行**。 处理程序只处理对应的 `try` 子句 中发生的异常，而不处理同一 `try` 语句内其他处理程序中的异常。 `except` 子句可以用带圆括号的 **元组来指定多个异常**

```python
while True:
    try:
        x = int(input("Please enter a number:"))
        break
    except (ValueError, TypeError, NameError):
        print("Oops! That was no valid number. Try again...")
```

如果 **`except` 子句中的类** 与发生 **异常的类是同一个类型，或者是异常的基类** 时，**该类与该异常相兼容**。反之则不成立 --- 列出派生类的 **_except 子句_** 与基类不兼容

下面的代码将依次打印 `B, C, D`:
```python
class B(Exception):
    pass


class C(B):
    pass


class D(C):
    pass


for cls in [B, C, D]:
    try:
        raise cls()
    except D:
        print("D")
    except C:
        print("C")
    except B:
        print("B")
```

请注意如果颠倒 `except` 子句 的顺序（**把 `except B` 放在最前**），则会输出 `B, B, B `--- 即触发了第一个匹配的 `except` 子句

发生异常时，它可能具有**关联值**，即 **异常 _参数_** 。是否需要参数，以及参数的类型取决于异常的类型。

`except` 子句 可能会 **在异常名称后面指定一个变量**。 这个 **变量将被绑定到异常实例**，该实例通常会有一个存储参数的 `args` 属性。 为了方便起见，内置异常类型定义了 `__str__()` 来打印所有参数而不必显式地访问 `.args`

```python
try:
    raise Exception('spam', 'eggs')
except Exception as inst:
    print(type(inst))    # <class 'Exception'>  类型
    print(inst.args)     # ('spam', 'eggs') 存在异常示例中的 args
    print(inst)          # ('spam', 'eggs') __str__
    x, y = inst.args     # unpack args 解包
    print('x =', x)
    print('y =', y)
```

**`Exception` 可以被用作通配符，捕获（几乎）一切**。然而，好的做法是，**尽可能具体** 地说明我们打算处理的异常类型，并 **允许任何意外的异常传播下去**。

处理 `Exception` 最常见的模式是打印或记录异常，然后重新提出（允许调用者也处理异常）:

```python
import sys

try:
    f = open('myfile.txt')
    s = f.readline()
    i = int(s.strip())
except OSError as err:
    print("OS error:", err)
except ValueError:
    print("Could not convert data to an integer.")
except Exception as err:
    print(f"Unexpected {err=}, {type(err)=}")
    raise  # 重新抛出该异常
```

### `else` 子句

`try ... except` 语句 **具有可选的 `else` 子句**，该子句如果存在，它 **必须放在所有 `except` 子句之后**。 它 **适用于 `try` 子句 _没有引发异常但又必须要执行的代码_**

```python
for arg in sys.argv[1:]:
    try:
        f = open(arg, 'r')
    except OSError:
        print('cannot open', arg)
    else:
        print(arg, 'has', len(f.readlines()), 'lines')
        f.close()
```

使用 `else` 子句比向 `try` 子句添加额外的代码要好，**可以避免意外捕获非 `try ... except` 语句保护的代码触发的异常**。

异常处理程序不仅会处理在 **_try 子句_** 中立刻发生的异常，还会处理在 **_try 子句_** 中调用（包括间接调用）的函数

```python
def this_fails():
    x = 1/0

try:
    this_fails()  # 处理函数调用中的异常
except ZeroDivisionError as err:
    print('Handling run-time error:', err)
```

##  异常链

如果一个 **未处理的异常发生在 `except` 部分内**，它将会有被处理的异常附加到它上面，并包括在错误信息中

```python
try:
    open("database.sqlite")
except OSError:
    raise RuntimeError("unable to handle error")
```

将报出两个错误

```
Traceback (most recent call last):
  File "/home/dyp/code/Python/异常/error.py", line 74, in <module>
    open("database.sqlite")
FileNotFoundError: [Errno 2] No such file or directory: 'database.sqlite'

During handling of the above exception, another exception occurred:

Traceback (most recent call last):
  File "/home/dyp/code/Python/异常/error.py", line 76, in <module>
    raise RuntimeError("unable to handle error")
RuntimeError: unable to handle error
```

为了表明 **一个异常是另一个异常的直接后果**， **`raise` 语句允许一个可选的 `from` 子句**

```python
# exc must be exception instance or None.
raise RuntimeError from exc
```

转换异常时，这种方式很有用。例如

```python
def func():
    raise ConnectionError

try:
    func()
except ConnectionError as exc:
    raise RuntimeError('Failed to open database') from exc  # ConnectionError 导致了 RuntimeError
```

它还允许 **使用 `from None` 表达禁用自动异常链**。如下示例，只会抛出 `RuntimeError`

```python
try:
    open('database.sqlite')
except OSError:
    raise RuntimeError from None  # 禁用自动异常链
```

## 用户自定义异常

程序可以通过创建新的 **异常类** 命名自己的异常，不论是以直接还是间接的方式，**异常都应从 `Exception` 类派生**

异常类可以被定义成能做其他类所能做的任何事，**但通常应当保持简单**，它往往**只提供一些属性**，**允许相应的异常处理程序提取有关错误的信息**。

大多数异常命名都以` “Error” `结尾，类似标准异常的命名

## `finally` 子句：定义清理操作

`try` 语句还有一个可选的 `finally` 子句，**用于定义在所有情况下都必须要执行的清理操作**

```python
try:
    raise KeyboardInterrupt
finally:
    print('Goodbye, world!')
```

输出结果如下

```
Goodbye, world!  # finally 子句被指向
Traceback (most recent call last):
  File "/home/dyp/code/Python/异常/error.py", line 95, in <module>
    raise KeyboardInterrupt
KeyboardInterrupt
```

**如果存在 `finally` 子句，则 `finally` 子句是 `try` 语句结束前执行的最后一项任务**。**不论 `try` 语句是否触发异常，都会执行 `finally` 子句**

以下内容介绍了几种比较复杂的触发异常情景
- 如果执行 `try` 子句期间触发了某个异常，则某个 `except` 子句应处理该异常。如果该异常没有 `except` 子句处理，在 `finally` 子句执行后会被重新触发
- `except` 或 `else` 子句执行期间也会触发异常。 同样，该异常会在 `finally` 子句执行之后被重新触发
- 如果 `finally` 子句中包含 `break`、`continue` 或 `return` 等语句，异常将不会被重新引发
- 如果执行 `try` 语句时遇到 `break`,、`continue` 或 `return` 语句，则 `finally` 子句在执行 `break`、`continue` 或 `return` 语句之前执行
- 如果 `finally` 子句中包含 `return` 语句，则返回值来自 `finally` 子句的某个 `return` 语句的返回值，而不是来自 `try` 子句的 `return` 语句的返回值。

```python
def bool_return():
    try:
        return True
    finally:
        return False


print(bool_return())  # False
```

一个更复杂的例子

```python
def divide(x, y):
    try:
        result = x / y
    except ZeroDivisionError:
        print("division by zero!")
    else:
        print("result is", result)
    finally:
        print("executing finally clause")

divide(2, 1)  # else 和 finally 都会执行

divide(2, 0)  # except 和 finally 执行

divide("2", "1")  # finally 执行
```

**任何情况下都会执行 `finally` 子句**。`except` 子句不处理两个字符串相除触发的 `TypeError`，因此会在 `finally` 子句执行后被重新触发。

在实际应用程序中，**`finally` 子句对于释放外部资源（例如文件或者网络连接）非常有用，无论是否成功使用资源**

## 引发和处理多个不相关的异常

在有些情况下，**有必要报告几个已经发生的异常**。这通常是在并发框架中当几个任务并行失败时的情况，但也有其他的用例，有时需要是继续执行并收集多个错误而不是引发第一个异

**内置的 `ExceptionGroup` 打包了一个异常实例的列表**，这样它们就可以一起被引发

```python
def f():
    excs = [OSError('error 1'), SystemError('error 2')]
    raise ExceptionGroup('there were problems', excs)

f()
```

运行结果为

```
  + Exception Group Traceback (most recent call last):
  |   File "/home/dyp/code/Python/异常/error.py", line 130, in <module>
  |     f()
  |   File "/home/dyp/code/Python/异常/error.py", line 128, in f
  |     raise ExceptionGroup('there were problems', excs)
  | ExceptionGroup: there were problems (2 sub-exceptions)
  +-+---------------- 1 ----------------
    | OSError: error 1
    +---------------- 2 ----------------
    | SystemError: error 2
    +------------------------------------
```

`ExceptionGroup` 本身就是一个异常，所以它可以像其他异常一样被捕获

```python
try:
    f()
except Exception as e:
    print(f'caught {type(e)}: e')
```

通过 **使用 `except*`** 代替 `except` ，我们可以 **有选择地只处理组中符合某种类型的异常**

在下面的例子中，显示了一个嵌套的异常组，每个 `except*` 子句都从组中提取了某种类型的异常，而让所有其他的异常传播到其他子句，并最终被重新引发

```python
def f():
    raise ExceptionGroup(
        "group1",
        [
            OSError(1),
            SystemError(2),
            ExceptionGroup(
                "group2",
                [
                    OSError(3),
                    RecursionError(4)
                ]
            )
        ]
    )

try:
    f()
except* OSError as e:   # 选择捕获 OSError
    print("There were OSErrors")
except* SystemError as e:  # 选择捕获 SystemError
    print("There were SystemErrors")
```

```
  + Exception Group Traceback (most recent call last):
  |   File "/home/dyp/code/Python/异常/error.py", line 155, in <module>
  |     f()
  |   File "/home/dyp/code/Python/异常/error.py", line 139, in f
  |     raise ExceptionGroup(
  | ExceptionGroup: group1 (1 sub-exception)
  +-+---------------- 1 ----------------
    | ExceptionGroup: group2 (1 sub-exception)
    +-+---------------- 1 ----------------
      | RecursionError: 4
      +------------------------------------
There were OSErrors
There were SystemErrors
```

> [!attention] 
> 
> 注意，**嵌套在一个异常组中的异常必须是 _实例_**，而不是类型。
> 

在实践中，这些 **异常通常是那些已经被程序提出并捕获的异常**，其模式如下:

```python
excs = []
for test in tests:
    try:
        test.run()
    except Exception as e:
        excs.append(e)

if excs:
   raise ExceptionGroup("Test Failures", excs)
```

## 用注释细化异常情况

当一个异常被创建以引发时，它通常被初始化为描述所发生错误的信息。在有些情况下，**在异常被捕获后添加信息是很有用的**

为了这个目的，**异常有一个 `add_note(note) `方法接受一个字符串**，并将其添加到异常的注释列表。标准的回溯在异常之后按照它们被添加的顺序呈现包括所有的注释

```python
try:
    raise TypeError('bad type')
except Exception as e:
    e.add_note('Add some information')
    e.add_note('Add some more information')
    raise
```

```
Traceback (most recent call last):
  File "/home/dyp/code/Python/异常/error.py", line 162, in <module>
    raise TypeError('bad type')
TypeError: bad type
Add some information
Add some more information
```

例如，当把异常收集到一个异常组时，我们可能想为各个错误添加上下文信息。在下文中，组中的每个异常都有一个说明，指出这个错误是什么时候发生的

```python
def f():
    raise OSError('operation failed')


excs = []
for i in range(3):
    try:
        f()
    except Exception as e:
        e.add_note(f'Happened in Iteration {i+1}')
        excs.append(e)

raise ExceptionGroup('We have some problems', excs)
```

```
  + Exception Group Traceback (most recent call last):
  |   File "/home/dyp/code/Python/异常/error.py", line 181, in <module>
  |     raise ExceptionGroup('We have some problems', excs)
  | ExceptionGroup: We have some problems (3 sub-exceptions)
  +-+---------------- 1 ----------------
    | Traceback (most recent call last):
    |   File "/home/dyp/code/Python/异常/error.py", line 176, in <module>
    |     f()
    |   File "/home/dyp/code/Python/异常/error.py", line 170, in f
    |     raise OSError('operation failed')
    | OSError: operation failed
    | Happened in Iteration 1
    +---------------- 2 ----------------
    | Traceback (most recent call last):
    |   File "/home/dyp/code/Python/异常/error.py", line 176, in <module>
    |     f()
    |   File "/home/dyp/code/Python/异常/error.py", line 170, in f
    |     raise OSError('operation failed')
    | OSError: operation failed
    | Happened in Iteration 2
    +---------------- 3 ----------------
    | Traceback (most recent call last):
    |   File "/home/dyp/code/Python/异常/error.py", line 176, in <module>
    |     f()
    |   File "/home/dyp/code/Python/异常/error.py", line 170, in f
    |     raise OSError('operation failed')
    | OSError: operation failed
    | Happened in Iteration 3
    +------------------------------------
```
