# 上下文管理器

在 [[Python：文件 IO]] 介绍了上下文管理 `with` 语句，该语句会在进入时获取资源，并在结束时释放资源。并且，释放资源时绝对的，无论 `with` 语句时正常退出还是异常退出。

```python
with open("test.txt", "r") as f:
	# 操作文件
	pass
```

上述代码中，我们使用 `with` 管理打开文件的生命周期。进入 `with` 语句时，获取打开文件的对象；离开 `with` 语句时，会自动调用文件的 `f.close()` 方法关闭文件。

> [!tip] 
> 
> 请注意：`with` 后面跟着的是一个 **实例**。即使我们先打开文件，然后再用 `with` 去管理也是一样的
> 

```python
f = open("test.txt", 'r')

with f:
	# 操作文件
	pass

# 在这里文件已经被关闭，继续使用文件将抛出 ValueError: I/O operation on closed file.
```

下面我们介绍背后涉及到的特殊方法。用于上下文管理的特殊方法是 `__enter__()` 和 `__exit__()`。

> [!tip] 
> 
> 当我们 `with` 一个对象时，Python 会自动调用 `__enter__()` 做一些初始化的工作，当该函数返回时，就执行 `with` 内部的语句
> 
> 当 `with` 内部语句抛出错误或者执行完毕，Python 会主动调用 `__exit__()` 方法释放资源，从而保证资源及时释放
> 
> 如果一个类定义了 `__enter__()` 和 `__exit__()`，那么该类称为上下文管理器类，其实例称为上下文管理器对象
> 

```python
import time


class Context:

    def __init__(self):
        time.sleep(3)
        print("__init__ called")


    def __enter__(self):
        print("__enter__ called") 
        time.sleep(3)


    def __exit__(self, exc_type, exc_val, exc_tb):
        print("__exit__ called")


with Context() as context:
    print("context")  
```

上述代码执行时，会输出如下结果

```
__init__ called    # 3 秒后打印
__enter__ called   # 随后，立即打印
context            # 3 秒后打印
__exit__ called    # 随后立即打印
```

从上述输出结果来看，`with` 操作的一定是类的实例，非函数或类对象。并且，`with` 语句块内的代码要在 `__enter__()` 方法返回之后才会被执行。最后，离开 `with` 语句块时，才会调用 `__exit__()` 方法

下面的代码我们在 `with` 语句块中抛出了除零异常(`ZeroDivisionError`)

```python
import time


class Context:

    def __init__(self):
        time.sleep(3)
        print("__init__ called")


    def __enter__(self):
        print("__enter__ called")
        time.sleep(3)


    def __exit__(self, exc_type, exc_val, exc_tb):
        print("__exit__ called")
        print(exc_type, exc_val, exc_tb, sep="||")


with Context() as context:
    1 / 0  # 触发异常
    print("context")
```

执行这段代码的输出结果为

```
__init__ called
__enter__ called
__exit__ called
<class 'ZeroDivisionError'>||division by zero||<traceback object at 0x000002440D879B40>
Traceback (most recent call last):
  File "./test.py", line 22, in <module>
    1 / 0  # 触发异常
    ~~^~~
ZeroDivisionError: division by zero
```

结果显示，在抛出异常之前首先调用了 `__exit__()` 方法，并将 `with` 语句块内部的异常传递给 `__exit__()` 中，这里传递了 $3$ 个参数：**异常类型**、**异常值** 和 **异常堆栈跟踪信息**。

> [!tip] 
> 
> 在 `__exit__()` 中可以获取到 `with` 语句块内部抛出的异常，我们可以在 `__exit__()` 内对异常进行处理(记录日志等操作)。Python 会根据 `__exit__()` 的返回值选择是否继续向外抛出异常，还是拦截异常
> 
> + 如果 `__exit__()` 返回 `True`，则 Python 会拦截异常，不在继续抛出
> + 如果 `__exit__()` 返回 `False`，则 Python 会继续向外抛出异常
> 
> 当 `__exit__()` 结束时，整个 `with` 语句就结束了。如果触发异常位置后面还有指令，则 Python 会忽略。这与 [[Python：异常处理]] 机制时一样的：遇见异常，异常后的代码就不在执行
> 

```python
import time


class Context:

    def __init__(self):
        time.sleep(3)
        print("__init__ called")


    def __enter__(self):
        print("__enter__ called")
        time.sleep(3)


    def __exit__(self, exc_type, exc_val, exc_tb):
        print("__exit__ called")
        print(exc_type, exc_val, exc_tb, sep="||")
        return True  # 返回 True 拦截异常


with Context() as context:
    1 / 0  # 触发异常
    print("context")
```

上述代码中，由于 `__exit__()` 返回了 `True`，此时 `with` 语句块中的异常被拦截。因此，输出结果中，就不会存在异常的跟踪信息了

```
__init__ called
__enter__ called
__exit__ called
<class 'ZeroDivisionError'>||division by zero||<traceback object at 0x0000027946209B40>
```

---

在 `with` 语句后面还有一个 `as` 子句，那么 `as` 子句后的标识符的什么呢？

```python
import time


class Context:

    def __init__(self):
        time.sleep(3)
        print("__init__ called")


    def __enter__(self):
        print("__enter__ called")
        time.sleep(3)
        return 123


    def __exit__(self, exc_type, exc_val, exc_tb):
        print("__exit__ called")
        print(exc_type, exc_val, exc_tb, sep="||")
        return True  # 返回 True 拦截异常



with Context() as context:
    print(context)  # 123
```

上述代码中，我们让 `__enter__()` 方法返回了 $123$，在 `with` 语句块中输出了 `context`，其结果为 $123$。显然 `__enter__()` 的返回值被 `as` 语句绑定给了其后面的标识符。

> [!summary] 
> 
> Python 提供 `__enter__()` 和 `__exit__()` 两个特殊方法用于进行上下文管理，即让实例可以被 `with` 语句使用
> 
> 当实例被 `with` 时，会立即调用 `__enter__()` 方法；如果 `__enter__()` 方法有返回值，可以使用 `as` 语句将其绑定给一个标识符
> 
> 当 `with` 语句块执行完毕或者抛出异常时，Python 会调用 `__exit__()` 方法，如果发生异常就会将异常类型、异常实例、异常堆栈信息传递给 `__exit__()`。Python 根据 `__exit__()` 的返回值选择是否继续向外抛出异常
> + 如果返回 `True`，则不继续向外抛出异常
> + 如果返回 `False`，则继续向外抛出异常
>   
> Python **保证了 `__exit__()` 的执行**，无论何时都可以执行 `__exit__()`，对于需要必须执行的代码就可以写在 `__exit__()`。例如，关闭文件、释放锁等操作
> 

> [!tip] 
> 
> 上下文的应用场景
> 
> 增强功能：在代码执行前增加代码
> 
> 资源管理：在 `__enter__` 中申请资源，在 `__exit__` 中释放资源
> 
> 权限验证：在执行代码之前，做权限校验，在 `__enter__` 中处理
> 

## contextlib

标准库 `contextlib` 为涉及 `with` 语句的常见任务提供了实用的工具。这里我们介绍 `contextlib.contextmanager`。

`contextlib.contextmanager` 是一个装饰器，它可被用来定义一个支持 `with` 语句上下文管理器的工厂函数，而无需创建一个类或单独的 `__enter__()` 和 `__exit__() `方法。

> [!tip] 
> 
> 尽管许多对象都原生支持在 `with` 语句中使用，但有时需要被管理的资源本身并不是上下文管理器，并且没有实现 `close()` 方法以配合 `contextlib.closing` 使用。
> 

下面的实例代码展示了如何正确的使用 `contextlib.contextmanager` 装饰器

```python
from contextlib import contextmanager

@contextmanager
def managed_resource(*args, **kwds):
    # 获取资源的代码，例如：
    resource = acquire_resource(*args, **kwds)
    try:
        yield resource
    finally:
        # 释放资源的代码，例如：
        release_resource(resource)
```

随后可以这样使用此函数

```python
with managed_resource(timeout=3600) as resource:
     # 资源将在此代码块的末尾被释放，
     # 即使代码块中的代码引发了异常
```

下面我们来探究 `contextmanager` 装饰器的实现原理。下面给出了 `contextmanager` 的代码

```python title:contextlib.py
def contextmanager(func):
    @wraps(func)
    def helper(*args, **kwds):
        return _GeneratorContextManager(func, args, kwds)
    return helper
```

从 `contextmanager` 返回一个函数 `helper`。也就是说 `@contextmanager` 等价于 `func = contextmanager(func)`

```python
@contextmanager
def func():
	pass
```

此时，`func` 被替换为了内层函数 `helper`。内层函数 `helper` 又会返回一个 `_GeneratorContextManager` 类的实例对象。`_GeneratorContextManager` 类的定义如下

```python
class _GeneratorContextManagerBase:

    def __init__(self, func, args, kwds):
        self.gen = func(*args, **kwds)
        self.func, self.args, self.kwds = func, args, kwds
        doc = getattr(func, "__doc__", None)
        if doc is None:
            doc = type(self).__doc__
        self.__doc__ = doc

    def _recreate_cm(self):
        return self.__class__(self.func, self.args, self.kwds)

class ContextDecorator(object):

    def _recreate_cm(self):
        return self

    def __call__(self, func):
        @wraps(func)
        def inner(*args, **kwds):
            with self._recreate_cm():
                return func(*args, **kwds)
        return inner

class _GeneratorContextManager(
    _GeneratorContextManagerBase,
    AbstractContextManager,
    ContextDecorator,
):
    """Helper for @contextmanager decorator."""

    def __enter__(self):
        del self.args, self.kwds, self.func
        try:
            return next(self.gen)
        except StopIteration:
            raise RuntimeError("generator didn't yield") from None

    def __exit__(self, typ, value, traceback):
        if typ is None:
            try:
                next(self.gen)
            except StopIteration:
                return False
            else:
                try:
                    raise RuntimeError("generator didn't stop")
                finally:
                    self.gen.close()
        else:
            if value is None:
                value = typ()
            try:
                self.gen.throw(value)
            except StopIteration as exc:
                return exc is not value
            except RuntimeError as exc:
                if exc is value:
                    exc.__traceback__ = traceback
                    return False
                if (
                    isinstance(value, StopIteration)
                    and exc.__cause__ is value
                ):
                    value.__traceback__ = traceback
                    return False
                raise
            except BaseException as exc:
                if exc is not value:
                    raise
                exc.__traceback__ = traceback
                return False
            try:
                raise RuntimeError("generator didn't stop after throw()")
            finally:
                self.gen.close()
```

首先，明确一点：被 `contextmanager` 装饰过的函数变成了内层的 `helper` 函数，当我们使用 `with func() as f` 语句时，调用的 `func()` 即是调用 `helper()`。由于 `helper()` 只是返回 `_GeneratorContextManager` 类的实例

所以，`with` 会首先调用 `_GeneratorContextManager` 类实例的 `__enter__()` 方法。下面，我们关注这个方法的实现

```python title:contextlib._GeneratorContextManager
def __enter__(self):
	del self.args, self.kwds, self.func
	try:
		return next(self.gen)
	except StopIteration:
		raise RuntimeError("generator didn't yield") from None
```

`__enter__()` 方法首先从当前对象中删除了一些属性，这些属性是在该对象初始化时引入的，后续不在需要。`_GeneratorContextManager` 实例的初始化方法如下

```python title:contextlib.py
class _GeneratorContextManagerBase:

    def __init__(self, func, args, kwds):
        self.gen = func(*args, **kwds)
        self.func, self.args, self.kwds = func, args, kwds
        doc = getattr(func, "__doc__", None)
        if doc is None:
            doc = type(self).__doc__
        self.__doc__ = doc

    def _recreate_cm(self):
        return self.__class__(self.func, self.args, self.kwds)
```

随后，`__enter__()` 执行了 `next(self.gen)` 驱动生成器执行，并返回生成器返回的值。如果生成器抛出 `StopIteration` 错误，则表明生成结束，此时会抛出 `RuntimeError`

> [!attention] 
> 
> 请注意：被装饰函数 `func` 必须是生成器函数
> 

进入 `with` 语句时执行的代码解释完毕，下面我们继续看`with` 语句块退出或者抛出异常时执行的 `__exit__()` 方法

```python title:contextlib._GeneratorContextManager
def __exit__(self, typ, value, traceback):
	# 检查 with 语句块是否抛出异常
	if typ is None:  
		# 没有抛出异常的逻辑
		try:
			next(self.gen)  # 继续驱动被装饰的生成器函数执行
		except StopIteration:
			# 如果抛出 StopIteration 证明生成函数执行完毕
			return False  
		else:
			# 没有抛出异常，那么生成器函数没有执行完毕
			try:
				# 此时，抛出 RuntimeError
				raise RuntimeError("generator didn't stop")
			finally:
				self.gen.close()  # 并且结束生成器的执行
	else:
		# with 语句块抛出异常
		if value is None:
			value = typ()  # 异常值为空，则创建一个异常对象
		try:
			self.gen.throw(value)  # 在生成器 yield 的位置引发异常
		except StopIteration as exc:
			return exc is not value
		except RuntimeError as exc:
			if exc is value:
				exc.__traceback__ = traceback
				return False
			if (
				isinstance(value, StopIteration)
				and exc.__cause__ is value
			):
				value.__traceback__ = traceback
				return False
			raise
		except BaseException as exc:
			if exc is not value:
				raise
			exc.__traceback__ = traceback
			return False
		try:
			raise RuntimeError("generator didn't stop after throw()")
		finally:
			self.gen.close()
```

从上述代码，我们看出被装饰函数的 `yield` 语句只能执行一次。

> [!tip] 
> 
> `contextmanager` 装饰器要求被装饰函数是一个生成器函数，并且 `yield` 语句有且只能执行一次
> 
