# 上下文管理

在 Python 中，某个对象需要持有资源，并且在使用完毕后立即释放资源。最典型的例子就是 **文件对象**，文件使用完毕后就应该来立即关闭文件

Python 中的 `with` 语句就是就能做到在文件使用完之后立即关闭文件

```python
with open() as f:
	# 在 with 语句内部使用文件对象
	pass

# 此处，离开了 with 语句，此时文件对象已经被关闭
```

> [!attention] 
> 
> 注意：`with` 语句操作的是一个类的实例对象
> 

要让一个类的实例对象支持 `with` 上下文管理，需要定义两个魔术方法 `__enter__` 和 `__exit__`。其中 `__enter__` 在进入 `with` 语句时触发，用于获取资源；而 `__exit__` 在离开 `with` 语句时触发，用于释放资源

```python
import time


class Point:

    def __init__(self):
        print("start init----------------")
        time.sleep(2)
        print("end init-----------------")

    def __enter__(self):
        print("enter--------------")

    def __exit__(self, exc_type, exc_val, exc_tb):
        print("exit---------------")


with Point():
    print("with-----")
    time.sleep(2)
    1 / 0
    print("after 1/0 ....")

print("end with")
```

上述代码的执行结果为

```
start init----------------
end init-----------------
enter--------------
with-----
exit---------------
Traceback (most recent call last):
  File "/mnt/c/Users/duyup/Documents/pyfiles/test.py", line 21, in <module>
    1 / 0
    ~~^~~
ZeroDivisionError: division by zero
```

分析上述执行结果，我们知道
+ `with` 后面的一定是一个实例对象。也就是说，如果在 `with` 语句中初始化对象，一定会等待 `__init__()` 执行完毕后，才会触发 `__enter__()` 方法
+ 保证不论什么原因，都一定会执行 `__exit__()` 方法

> [!important] 
> 
> **保证无论任何原因退出 `with` 语句，都会触发 `__exit__`**。也就是说，`with` 上下文管理是安全的
> 

请注意：**`__enter__()` 方法可以返回一个值**。通常返回的就是对象本身。如果 `__enter__()` 有一个返回值，则可以使用 `as` 语句将其绑定到一个变量上

```python
class Point:

    def __enter__(self):
        return 7890

    def __exit__(self, exc_type, exc_val, exc_tb):
        pass


p = Point()

with p as q:  # __enter__ 函数的返回值被 q 引用 q = p.__enter__()
    print(p is q)  # False

    print(q)  # 7890

    print(p == q)  # False
```

> [!important] 
> 
> 我发主要到 `__exit__()` 需要接收 $3$ 参数
> + `exc_type`：代表了异常类型
> + `exc_val`：代表异常值
> + `exc_tb`：异常时的堆栈信息
> 
> 换句话说，在 `__exit__()` 方法中，我们可以查看触发 `__exit__()` 调用的原因。毫无疑问的，`__exit__()` 也可以有返回值，这个返回值用于拦截异常
> 
> + 返回值的逻辑值为 `True`，代表异常被拦截
> + 返回值的逻辑值为 `False`，代表异常不会被拦截
> 

下面代码给出了上下文管理的一个应用：时间记录的装饰器

```python
import functools
import time


class Timeit:

    def __init__(self, fn):
        self.__fn = fn
        # functools.update_wrapper(self, fn)
        functools.wraps(fn)(self)


    def __call__(self, *args, **kwargs):
        with self:
            result = self.__fn(*args, **kwargs)

        print(f"执行函数 {self.__name__} 消耗时间 {self.__execute_time()}")
        return result


    def __enter__(self):
        self.__start = time.time()
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        self.__end = time.time()

    def __execute_time(self):
        return self.__end - self.__start


@Timeit  # add = Timeit(add)
def add(a, b):
    """加法
    """
    time.sleep(3)
    return a + b


print(add(4, 5))


print(add.__doc__)
print(add.__name__)
```

再次明确一下，**装饰器本质上就是一个可调用对象，并且还需要返回一个可调用对象**

> [!attention] 上下文管理的一些应用场景
> 
> 增强功能：在代码执行前增加代码
> 
> 资源管理：在 `__enter__` 中申请资源，在 `__exit__` 中释放资源
> 
> 权限验证：在执行代码之前，做权限校验，在 `__enter__` 中处理
> 

## contextmanager：上下文管理器

Python 在 `contextlib` 标准库中提供了一个装饰 `contextmanager`。这个装饰器装饰一个函数，让被装饰函数变成一个可以适用于 `with` 语句的函数

> [!attention] 
> 
> 请注意：这个被装饰函数需要满足一个要求：有且只有一个 `yield` 语句
> 
> 换句话说，该装饰器接收一个生成器对象作为参数
> 

```python
import contextlib
import time


@contextlib.contextmanager  # foo = contextmanager(foo)
def foo():
    print("enter ======")  # 进入 __enter__
    yield
    print("exit ======")  # 退出 __exit__


with foo() as f:  # f 是 yield 出来的值
    print("with -----")
    time.sleep(2)
    print("with +++++")
```

如果 `with` 语句中出现异常之后，`foo()` 中 `yield` 后的代码就不能被执行，通常需要再 `foo()` 中使用 `try ... finally ...` 语句

```python
import contextlib
import time


@contextlib.contextmanager  # foo = contextmanager(foo)
def foo():
    print("enter ======")  # 进入 __enter__
     # try 之前就是 __enter__
    try:  # 捕 with 语句中错误
        yield  # 只是用于暂停 foo
    finally:
	    # finally 之后就是 __exit__
        print("exit ======")  # 退出 __exit__


with foo() as f:  # f 是 yield 出来的值
    print("with -----")
    1/0
    print("with +++++")
```

> [!seealso] 
> 
> Python 中支持异常处理，详细参见 [[Python：异常]]
> 

下面我们探讨 `contextmanager` 是如何实现的。下面的代码摘抄自 `contextlib.py`

```python
def contextmanager(func):
    """@contextmanager decorator.

    Typical usage:

        @contextmanager
        def some_generator(<arguments>):
            <setup>
            try:
                yield <value>
            finally:
                <cleanup>

    This makes this:

        with some_generator(<arguments>) as <variable>:
            <body>

    equivalent to this:

        <setup>
        try:
            <variable> = <value>
            <body>
        finally:
            <cleanup>
    """
    @wraps(func)
    def helper(*args, **kwds):
        return _GeneratorContextManager(func, args, kwds)
    return helper
```

上述代码提示我们，`contextmanager` 是一个装饰器，返回了一个 `helper` 函数对象。这个函数对象被调用时，又会返回一个 `_GeneratorContextManager` 类的实例对象 

```python hl:10,17
class _GeneratorContextManager(
    _GeneratorContextManagerBase,
    AbstractContextManager,
    ContextDecorator,
):

    def __enter__(self):
        del self.args, self.kwds, self.func
        try:
            return next(self.gen)  # 执行到 yield 之后，离开函数，然执行 with 语句块内部的代码
        except StopIteration:
            raise RuntimeError("generator didn't yield") from None

    def __exit__(self, typ, value, traceback):
        if typ is None:  # with 语句正常退出
            try:
                next(self.gen)  # with 正常结束，就继续执行函数
            except StopIteration:  # 如果函数只有一个 yield，就正常退出
                return False
            else:   # 未报错，证明函数有多个 yield，此时唐山
                try:
                    raise RuntimeError("generator didn't stop")
                finally:
                    self.gen.close()  # 将生成器关闭掉
        else:  # with 语句出现错误
            if value is None:
                value = typ()  # 错误类型对象 
            try:
                self.gen.throw(value)  # 即将错误类型提交给函数
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
