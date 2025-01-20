# 装饰器与偏函数

## 柯里化

在计算机科学中，**柯里化**（英语：Currying），是把 **接受多个参数的函数变换成接受一个 _单一参数_（最初函数的第一个参数）的函数**，并且 **返回接受余下的参数而且返回结果的新函数** 的技术

如下描述了柯里化的概念
$$
\begin{aligned}z_1 &= f(x, y) \Rightarrow z_1 = f(x)(y)\\
z_2 &= f(x,y,z) \Rightarrow z_2 = f(x)(x,y)\\
z_3 &= f(x, y,z) \Rightarrow z_3 = f(x)(y)(z)\end{aligned}
$$

> [!summary] 柯里化
> - 将多参函数变换为接受一个参数的函数，该参数是原始函数的第一个参数
> - 这个函数返回接受剩余参数并且返回结果的新函数

例如

```python
def add(x, y):
	return x + y
```

柯里化的目标是将调用形式 `add(x, y)` 变化为 `add(x)(y)`。显然 `add(x)` 需要返回一个函数(**可调用对象**)

```python
def add(x):
	def inner(y):
		return x + y
	return inner
```

## 装饰器

> [!tip] 装饰器
> 
> 在 **不修改对象的前提** 下，**为对象添加新的功能** 的方式
> 

假设我们有下面几个已经定义好的函数

```python
def add(x, y):
    return x + y

def minus(x, y):
    return x - y

def multiply(x, y):
    return x * y

def division(x, y):
    if y == 0:
        raise ValueError('除数不能为0')
    return x / y
```

现在，我们想要在函数被调用时，显式其参数。例如调用 `add(4, 5)` 输出 `add function was called by x=4 and y=5`。简单方法就是直接去 `add` 函数中修改

```python
def add(x, y):
    # 这段代码是侵入式的！如果需要修改将会带来极大的改动
    print('add function was called by x=%d and y=%d' % (x, y))
    return x + y
```

如果要为其他很多个函数添加相同的显式参数功能呢？一个一个去修改显然是不合理的。而且有时候，我们 **可能没有这个函数的源码**，从而无法修改。因此，我们可以将这些 **公共的功能** 封装为一个函数，让该函数去调用其他函数，从而增强函数的功能

```python
def add(x, y):
    return x + y

def logger(func):
    """记录函数参数"""

    # 函数增强的功能
    print(f'{func} was called by x={4} and y={5}')
    result = func(4, 5)
    print("got result is %d" % result)
    # 函数增强的功能
    return result

logger(add)
# 输出:
# <function add at 0x00000223E2EE3F60> was called by x=4 and y=5
# got result is 9
```

现在还需要解决 `func()` 的参数问题。`func()` 函数的参数可能有多种情况，但是传参方式只有两种，因此使用不定长参数 `*args` 和 `**kwargs`，收集传递给 `func()` 函数的所有参数

```python
def logger(func, *args, **kwargs):
    """记录函数参数"""

    # 函数增强的功能
    print(f'{func} was called by {args} and {kwargs}')
    result = func(*args, **kwargs)
    print("got result is %d" % result)
    # 函数增强的功能
    return result

logger(add, 5, 7)
```

> [!tip] 
> 
> `logger(0` 只是帮助 `func()` 收集参数，不管参数是否正确
> 

还有一个问题，每次要使用 `logger()` 功能时，都要传递需要被记录的函数，及其参数。通常，参数不应该传递给在 `logger`，而是传递给它包装的函数，因此需要 **柯里化**

```python
def logger(func):
    def wapper(*args, **kwargs):
        """记录函数参数"""

        # 函数增强的功能
        print(f'{func} was called by {args} and {kwargs}')
        # 使用了外层函数的局部变量 func，这里形成了闭包。func记住了传递给 logger 的函数
        result = func(*args, **kwargs)
        print("got result is %d" % result)
        # 函数增强的功能
        return result
    return wapper

logger(add)(2,6)

newfunc = logger(add)  # newfunc 又记录了 logger 调用时产生的函数对象 wapper 
```

分析上面的代码，变量 `newfunc` 记录了 `logger` 调用时产生的函数对象 `wapper`，因此 `logger` 返回时，`wapper` 引用的函数对象不会被释放。同时，在 `wapper` 函数内部使用了 `logger` 中的局部变量 `func`，形成了闭包，`logger` 函数返回时 `func` 引用的对象也不会被销毁；`func` 记录的是传递给 `logger` 函数的参数。本例中，`func` 记录的是 `add` 引用的函数对象。因此，`add` 函数对象也不会被释放。所以，我们可以放心大胆的给 `add` 标识符绑定其他的对象

```python
def logger(func):
    def wapper(*args, **kwargs):
        """记录函数参数"""

        # 函数增强的功能
        print(f'{func} was called by {args} and {kwargs}')
        result = func(*args, **kwargs)
        print("got result is %d" % result)
        # 函数增强的功能
        return result
    return wapper

add = logger(add)  # 将 add 绑定给 wapper 的引用

add(4, 5)
```

```python
# 1. 此处，创建一个 add函数对象，绑定到了 add 标识符，此时这个函数对象的引用计数 +1。此时add 函数对象的引用计数为 1
def add(x, y):
    return x + y

def logger(func):
	# 3， logger 被调用时，会产生一个 wapper 函数对象，绑定到了 wapper 标识符，引用计数 +1
    def wapper(*args, **kwargs):
        """记录函数参数"""

        # 函数增强的功能
        print(f'{func} was called by {args} and {kwargs}')
        # 4. wapper 函数中引用了上层函数的变量，形成了 **闭包**，wapper 函数对象中的属性 __closure__ 中记录了 func 引用的对象，在又会使得 func 引用的 add 函数对象的引用计数 +1。此时，add函数对象引用计数为 3
        result = func(*args, **kwargs)  # 最终调用的原来的 add 函数对象
        print("got result is %d" % result)
        # 函数增强的功能
        return result
    return wapper  # 4 return 语句将 wapper 函数对象的引用返回

add = logger(add) # 2. 调用 logger 时，它的形式参数 func 又引用了 add 函数对象，使得 add函数对象的引用计数 +1。此时 add 函数对象的引用计数为 2

# 5. logger 函数调用的返回值 wpper 函数对象的引用被标识符 add 记录
# 5.1 标识符 add 引用了另一个对象 wapper 函数对象，wapper函数对象的引用计数 +1；与此同时，原来的 add 函数对象的引用计数 -1。由于 logger 函数的返回，导致其调用栈被销毁，使得 add函数对象引用计数-1。目前，add 函数对象只有闭包中的引用了，add 函数对象不会被垃圾回收
# 5.2 logger 函数调用的返回，调用栈被销毁，此时 wapper 函数对象的引用计数 -1。但是 wapper 函数对象被标识符 add 记录了下来，它的引用计数也为 1

add(4, 5)
```

### 装饰器语法糖 @

```python
def logger(func):
    def wapper(*args, **kwargs):
        """记录函数参数"""

        print(f'{func} was called by {args} and {kwargs}')
        result = func(*args, **kwargs)
        print("got result is %d" % result)
    
        return result
    return wapper 

@logger  # 将它下面的对象(函数对象) add 当作实参传递给 logger，并将 logger 返回的对象绑定到它下面的标识符
def add(x, y):  # 等价于 add = wapper  <=> add = logger(add)
	"""加法计算：计算 x 与 y 的和"""
    return x + y


add(4, 5)
```

`@标识符` 称为装饰器。`标识符` 是一个可调用对象，**需要一个必须参数**（单参数的可调用对象。可以有其他参数，但是必须要有默认值）

> [!important] 
> 
> `@标识符` 称为无参装饰器，`标识符` 引用的对象称为装饰者(`wapper`)。它下面的标识符引用的对象称为被装饰者(`wapped`)
> 

### 补丁：伪装更彻底

装饰者 `wapper` 不应该隐藏被装饰者 `wapped` 的属性。回看上述示例

```python
def logger(func):
    def wapper(*args, **kwargs):
        """记录函数参数"""

        print(f'{func} was called by {args} and {kwargs}')
        result = func(*args, **kwargs)
        print("got result is %d" % result)
    
        return result
    return wapper 

@logger  
def add(x, y):  
	"""加法计算：计算 x 与 y 的和"""
    return x + y


add(4, 5)
```

函数对象中有两个属性 `__doc__` 和 `__name__` 分别表示文档字符串和函数名称。我们下面我们输出这个两个参数的值

```python
print(add.__doc__)
print(add.__name__)
```

输出结果为

```
记录函数参数
wapper
```

> [!tip] 
> 
> 查看函数对象的属性很容易发生函数是被修改过的。
> 

我们可以伪装的更彻底吗？也就是，**保留被装饰对象的重要属性**。也就是在返回 `wapper` 之前，使用 `func` 的属性替换掉 `wapper` 的属性即可

```python
def logger(func):
    def wapper(*args, **kwargs):
        
        """记录函数参数"""

        print(f'{func} was called by {args} and {kwargs}')
        result = func(*args, **kwargs)
        print("got result is %d" % result)
    
        return result
    wapper.__name__ = func.__name__  # 记录函数名
    wapper.__doc__ = func.__doc__  # 记录函数参数
    return wapper 
```

显然，我不能做侵入式修改。将其抽离出来，形成一个 `copy_properties()` 函数

```python
def copy_properties(src, dest):
    dest.__name__ = src.__name__  # 将 src 对象的名字赋值给 dest 对象
    dest.__doc__ = src.__doc__  # 将 src 的文档字符串赋值给 dest 对象

def logger(func):
    def wapper(*args, **kwargs):
        
        """记录函数参数"""

        print(f'{func} was called by {args} and {kwargs}')
        result = func(*args, **kwargs)
        print("got result is %d" % result)
        return result
    copy_properties(func, wapper)  # 拷贝 func 的属性给 wapper 对象
    return wapper 
```

现在我们期望将 `copy_properties`  做成装饰器。也就是像下面的使用方式使用 `copy_properties()`

```python hl:2,3
def logger(func):
	@copy_properties(func)
    def wapper(*args, **kwargs):
        """记录函数参数"""
        print(f'{func} was called by {args} and {kwargs}')
        result = func(*args, **kwargs)
        print("got result is %d" % result)
        return result
    return wapper 
```

由于 `@copy_properties(func)` 表示将 `copy_properties()` 的返回值作为装饰器使用。也就是说，`copy_properties(func)` 需要返回一个可调用对象。假设返回 `_copy()` 函数，那么 `@copy_properties(func)` 等价于 `@_copy`。所以上述代码中高亮部分本质上就是 `wapper = copy_properties(func)(wapper)`

通过上述分析，我们应该对 `copy_properties()` 定义为如下形式

```python
def copy_properties(src):
    def _copy(dest):
        dest.__name__ = src.__name__  # 将 src 对象的名字赋值给 dest 对象
        dest.__doc__ = src.__doc__  # 将 src 的文档字符串赋值给 dest 对象
        return dest  # 这里必须返回 dest。否则 _copy 返回 None
    return _copy
```

> [!tip] 
> 
> `@copy_properties()` 称为含参装饰器。本质上就是 `copy_properites()` 返回一个可调用对象。这个可调用对象也要返回一个可调用对象
> 

#### `update_wrapper`  和 `wraps`

属性更新 `functools.update_wrapper` 解决了 [[#补丁：伪装更彻底]] 中的问题。这是 Python 标准库中提供的一个函数，该函数的定义如下

```python
WRAPPER_ASSIGNMENTS = ('__module__', '__name__', '__qualname__', '__doc__',
                       '__annotations__', '__type_params__')
WRAPPER_UPDATES = ('__dict__',)
def update_wrapper(wrapper,
                   wrapped,
                   assigned = WRAPPER_ASSIGNMENTS,
                   updated = WRAPPER_UPDATES):

    for attr in assigned:
        try:
	        # 反射：在面向对象中介绍
            value = getattr(wrapped, attr)
        except AttributeError:
            pass
        else:
            setattr(wrapper, attr, value)
    for attr in updated:
        getattr(wrapper, attr).update(getattr(wrapped, attr, {}))
    wrapper.__wrapped__ = wrapped
    return wrapper
```

`update_wrapper()` 就类似于我们编写的 `copy_properites()`  的第一个版本。只需要在返回 `wapper` 之前调用该函数即可

```python
import functools

def logger(func):
    # @copy_properties(func)  # 等价于 @inner: wapper = dest <=> wapper = inner(dest)
    def wapper(*args, **kwargs):
        """记录函数参数"""

        print(f'{func} was called by {args} and {kwargs}')
        result = func(*args, **kwargs)
        print("got result is %d" % result)
        return result
    # copy_properties(wapper, func)  
    functools.update_wrapper(wapper, func)  # 更新属性
    return wapper 

@logger  # 将它下面的对象(函数对象) add 当作实参传递给 logger，并将 logger 返回的对象绑定到它下面的标识符
def add(x, y):  # 等价于 add = wapper  <=> add = logger(add)
	
    """加法计算：计算 x 与 y 的和
    """
    return x + y
```

`functools` 模块提供了一个 `wraps()` 装饰器函数，本质上是调用属性复制函数 `update_wrapper()`。使用 **偏函数** 实现

```python
def wraps(wrapped,
          assigned = WRAPPER_ASSIGNMENTS,
          updated = WRAPPER_UPDATES):
    return partial(update_wrapper, wrapped=wrapped,
                   assigned=assigned, updated=updated)
```

> [!tip] 偏函数
> 
> 偏函数会返回一个固定了部分参数的函数
> 

`wraps()` 函数返回的函数是将 `update_wrapper()` 函数的最后 $3$ 个参数固定后的函数。使用 `wraps()` 就像使用 `copy_properites()` 的装饰器版本一样

```python
import functools

def logger(func):
    # @copy_properties(func)  # 等价于 @inner: wapper = dest <=> wapper = inner(dest)
    @functools.wraps(func)  # wrapper = wraps(func)(wapper)
    # wraps(func) 必须返回一个单参数函数
    def wapper(*args, **kwargs):
        """记录函数参数"""

        print(f'{func} was called by {args} and {kwargs}')
        result = func(*args, **kwargs)
        print("got result is %d" % result)
        return result
    # copy_properties(wapper, func)  
    # functools.update_wrapper(wapper, func) 
    return wapper 

@logger  # 将它下面的对象(函数对象) add 当作实参传递给 logger，并将 logger 返回的对象绑定到它下面的标识符
def add(x, y):  # 等价于 add = wapper  <=> add = logger(add)
	
    """加法计算：计算 x 与 y 的和
    """
    return x + y
```

### 含参装饰器

给 `logger()` 装饰器设置阈值，记录运行时间超过阈值的函数。这个阈值如何给定内？我们期望如下给定阈值

```python
@logger(10)  # add = logger(10)(add)
def add(x, y):
    """加法计算：计算 x 与 y 的和
    """
    time.sleep(3)
    return x + y
```

因此，我们需要对 `logger` 进行如下柯里化

```python
def logger(duration=5):
    def _logger(func):
        @copy_properties(func)  # wapper = copy_properties(func)(wapper) ==> wapper = wapper
        def wapper(*args, **kwargs):
            """记录函数参数"""
            start = time.time()
            result = func(*args, **kwargs)
            end = time.time()
            if end - start > duration:
                print(f"慢运行：{func.__name__} 函数运行时间：{end - start}s 超过{duration}s")
            return result
        return wapper 
    return _logger

@logger(1)  # add = logger(1)(add) ==> add = _logger(add) => add = wapper
def add(x, y):
    """加法计算：计算 x 与 y 的和
    """
    time.sleep(3)
    return x + y
```

> [!tip] 到此，如果 `logger` 还需要参数 `logger(duration=5, name)` 不在需要进行柯里化了
> - `logger()` 本身就是函数调用，如果需要参数，可以不在需要柯里化
> - `@logger(1,a,b,c,d) ==> (@logger(1,a,b,c,d))(func)`。只要 `logger(1,a,b,c,d)` 的返回值为单参数函数，满足装饰器要求后，就没必要柯里化。

例如，我们将慢运行的函数输出到不同地方。只需要将

```python
def logger(duration, output=lambda name, delta: print(f"{name} 函数运行时间：{delta}s")):
    def _logger(func):
        @copy_properties(func)  # wapper = copy_properties(func)(wapper) ==> wapper = wapper
        # @functools.wraps(func)  # 复制函数属性
        def wapper(*args, **kwargs):
            """记录函数参数"""
            start = time.time()
            result = func(*args, **kwargs)
            end = time.time()
            if end - start > duration:
                output(func.__name__, end - start)  # 这是另一个业务了
            return result
        return wapper 
    return _logger


def save_log(name, delta):
    print(f"{name} 函数运行时间：{delta}s")

@logger(2, output=save_log)  # add = logger(1)(add) ==> add = _logger(add) => add = wapper
def add(x, y):
    """加法计算：计算 x 与 y 的和
    """
    time.sleep(3)
    return x + y
```

分析下面的代码

```python
import functools

def logger(func):
    @functools.wraps(func)
    def wapper(*args, **kwargs):
        """记录函数参数"""

        print(f'{func} was called by {args} and {kwargs}')
        result = func(*args, **kwargs)
        print("got result is %d" % result)
        return result
    return wapper 

@logger
def add(x, y):
	
    """加法计算：计算 x 与 y 的和
    """
    return x + y

@logger
def minus(x, y):
	"""减法计算：计算 x - y
	"""
	return x - y

print(add.__name__, add.__doc__)
print(minus.__name__, minus.__doc__)
```

> [!question] 思考下面的问题
> - `add` 函数、`minus` 函数执行过吗?
> 	- 没有执行，只是包装在了不同的 `wapper` 函数对象中
> - `logger` 什么时候执行?  `logger` 执行过几次?
> 	- 在 `add` 函数对象定义完成时和在 `minus` 函数定义完成时
> 	- 执行了两次
> - `wraps` 装饰器执行过几次?
> 	- 执行过两次：每调用一次 `logger` 就会执行一次 `wapper()` 函数的定义，`wapper` 对象就会被 `wraps` 装饰一次
> - `wrapper` 对象的 `__name__` 等属性被覆盖过几次?
> 	- 不同 `wrapper` 对象的的 `__name__` 只会被 `wraps` 装饰的时候覆盖一次
> - `add.__name__` 打印什么名称?
> 	- 打印 `add`
> - `minus.__name__` 打印什么名称?
> 	- 打印 `minus`

## 装饰器应用：参数类型检查

[[Python：函数基础#函数注解]] 提示函数的使用者传递对应类型的参数。然而，注解仅仅只是其提示作用，如何才能现在函数使用者传递对应类型的参数呢？答案就是装饰器，下面是参数类型检查装饰器函数的框架

```python
def check(func):
	"""参数检查装饰器
	"""
	def wrapper(*args, *kwargs):
		# 获取函数 func 的类型注解
		# 比对 wrapper 收集的参数是否符合 func 参数的注解
		return func(*args, **kwargs)
	return wrapper
```

### inspect 标准库

现在遇见了第一个问题：如何获取函数 `func` 的注解？很容易想到使用 `func.__annotation__` 属性，然而该属性是一个字典。由于字典是无序的，无法与位置参数进行对应

Python 提供了一个 [`inspect`](https://docs.python.org/zh-cn/3/library/inspect.html) 标准库，提供丰富的函数用于获取对象的信息。这里我们主要介绍 [`Signature`](https://docs.python.org/zh-cn/3/library/inspect.html#inspect.Signature "inspect.Signature")  对象

[`Signature`](https://docs.python.org/zh-cn/3/library/inspect.html#inspect.Signature "inspect.Signature") 对象代表一个可调用对象的调用签名及其返回值标。 要获取一个 `Signature` 对象，可使用 `signature()` 函数。

```python
import inspect

inspect.signature(callable, *, follow_wrapped=True, globals=None, locals=None, eval_str=False)
# 返回 callable 的 Signature 对象
```

> [!tip] 
> 
> 参数 `callable` 可以是 Python 中的各类可调用对象，包括函数、类、`partial()` 对象等
> 

`Signature` 对象表示一个函数的调用 **签名** 及其 **返回值标注**。它的属性 `parameters` 是一个**有序映射**，存储了表示函数所接受的每个形参的 `Parameter` 对象。下表列出来 `Signature` 对象的部分属性

| 属性                            | 描述                            |
| :---------------------------- | ----------------------------- |
| `Signature.empty`             | 特殊标记，表明函数的返回标注缺失              |
| `Signature.parameters`        | 函数的形参名到对应 `Parameter` 对象的有序映射 |
| `Signature.return_annotation` | 函数的返回标注                       |

`Parameter` 对象表示可调用对象的参数，它是一个 **不可变对象**。其中有 $4$ 个我们需要关注的属性

| 属性           | 描述                                                                                                                                                                                      |
| :----------- | :------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `name`       | 参数名。这是一个字                                                                                                                                                                               |
| `default`    | 参数的默认值。没有默认值则标记位 `emp                                                                                                                                                                   |
| `annotation` |                                                                                                                                                                                         |
| `kind`    参数类型<br>+ 仅位置参数：`POSITIONAL_ONLY`<br>+ 位置或关键字参数：`POSITIONAL_OR_KEYWORD`<br>+ 可变数量位置参数：`VAR_POSITIONAL`，即 `*args` <br>+ 仅关键字参数：`KEYWORD_ONLY`<br>+ 可变数量关键字参数：`VAR_KEYWORD`，即 `**kwargs` br>+  |

### 参数类型检查装饰器实现

目的是实现函数参数类型的检查。只有当函数被调用的时候，才会知道函数接收了哪些参数。因此，我们需要拦截函数的参数，然后获取函数的签名，再检查参数的类型是否与函数需要的类型匹配；如果参数的类型匹配成功则调用函数，否则提示用户参数类型错误

```python
import functools
import inspect

def check(func):
    # 获取 func 的签名
    signature = inspect.signature(func)
    # 获取函数形成的有序字典
    parameters = signature.parameters
    @functools.wraps(func)
    def wrapper(*args, **kwargs):

        # 首先检查位置参数
        for arg, param in zip(args, parameters.values()):
            # 当前参数有标注但是获得的参数值的类型不是标注的类型
            if param.kind is not param.empty and not isinstance(arg, param.annotation):
                raise TypeError(f"{param.name} must be {param.annotation}, but got {type(arg)}")
        # 然后检查关键字参数
        for key, value in kwargs.items():
            if parameters[key].kind is not parameters[key].empty and not isinstance(value, parameters[key].annotation):
                raise TypeError(f"{key} must be {parameters[key].annotation}, but got {type(value)}")
        # 参数检查通过，调用 func 并返回 func 的返回值
        return func(*args, **kwargs)
    return wrapper

@check
def add(a: int, b: int) -> int:
    return a + b


print(add(1, 2))
```

## 偏函数

标准库 `functools` 提供了 `partial()` 的函数，它的签名如下

```python
import functools

functools.partial(func, /, *args, **kwargs)

# 它返回一个新的 partial 对象
```

`partial` 对象是一个可调用对象。当调用该对象时，其行为类似于 `func()` 附带位置参数(`args`)和关键字参数(`kwargs`)被调用。如果提供了额外的位置参数，就会被附加在 `args` 之后；如果提供了额外的关键字参数，也会被添加到 `kwargs` 中

> [!tip] 
> 
> 换句话说，就是 `partial` 对象固定了函数`func` 的部分参数。相当于给 `func` 的部分参数提供了默认值，从而形成了一个新的函数  
> 

`functools.partial()` 被实现为一个类型。但是，它的本质等价于如下装饰器

```python
def partial(func, /, *args, **keywords):
    def newfunc(*fargs, **fkeywords):
        newkeywords = {**keywords, **fkeywords}
        return func(*args, *fargs, **newkeywords)
    newfunc.func = func  # 原函数
    newfunc.args = args  # 固定的位置参数
    newfunc.keywords = keywords  # 固定的关键字参数
    return newfunc
```
