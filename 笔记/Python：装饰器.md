# 装饰器

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

装饰器：在 **不修改对象的前提** 下，为对象添加新的功能的方式

从 `add` `minus` `multiply` `division` 开始

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

但是，如果要为其他很多个函数添加相同的显式参数功能呢？一个一个去修改显然是不合理的。而且有时候，我们 **可能没有这个函数的源码**，从而无法修改。

因此，我们可以将这些 **公共的功能** 封装为一个函数，让该函数去调用它增强的函数

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

现在还需要解决 `func` 的参数问题。`func` 函数的参数可能有多种情况，但是传参方式只有两种，因此使用不定长参数 `*args` 和 `**kwargs`

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

> [!important] `logger` 只是帮助 `func` 收集参数，不管参数是否正确

还有一个问题，每次要使用 `logger` 功能时，都要传递需要被记录的函数，及其参数。通常，参数不应该传递给在 `logger`，而是传递给它包装的函数，因此需要 **柯里化**

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

完整的代码分析

```python
# 1. 此处，创建一个 add函数对象，绑定到了 add 标识符，此时这个函数对象的引用计数 +1。此时add 函数对象的引用计数为 1
def add(x, y):
    return x + y

def logger(func):
	# 3， logger 被调用时，会参数一个 wapper 函数对象，绑定到了 wapper 标识符，引用计数 +1
    def wapper(*args, **kwargs):
        """记录函数参数"""

        # 函数增强的功能
        print(f'{func} was called by {args} and {kwargs}')
        # 4. wapper函数中引用了上层函数的变量，形成了 **闭包**，wapper 函数对象中的属性 __closure__ 中记录了 func 引用的对象，在又会使得 func 引用的 add 函数对象的引用计数 +1。此时，add函数对象引用计数为 3
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

![[Pasted image 20240616214317.png]]

### `@` 语法糖

Python 的装饰器语法 `@` 

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

`@标识符` 称为装饰器。`标识符` 是一个可调用对象，**需要一个必须参数**（单参数的可调用对象。可以有其他参数，但是必须要有默认值）。

> [!important] `@标识符` 称为无参装饰器，`标识符` 引用的对象称为装饰者(`wapper`)。它下面的标识符引用的对象称为被装饰者(`wapped`)

### 补丁：伪装更彻底

装饰者 `wapper` 不隐藏 `wapped` 的属性。会看上述示例
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

函数对象中有两个属性 `__doc__` 和 `__name__` 分别表示文档字符串和函数名称。我们下面我们输出这个两个参数的值

```python
print(add.__doc__)  # 记录函数参数
print(add.__name__)  # wapper
```

我们发现，函数名字和文档字符串都被改变了

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

显然，我不能做侵入式修改。将其抽离出来，重写为一个函数，命名为 `waps`

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
    return wapper 
```

现在，我们期望如下方式使用 `copy_properties`

```python
def logger(func):
	@copy_properties(func)  # @copy_properties(func) 等价于 @_copy; 则 wapper = dest <=> wapper = _copy(wapper)。即是：copy_properties(func)(wapper)
    def wapper(*args, **kwargs):
        """记录函数参数"""
        print(f'{func} was called by {args} and {kwargs}')
        result = func(*args, **kwargs)
        print("got result is %d" % result)
        return result
    return wapper 
```

`@copy_properties(func)` 就是 `@(copy_properties(func))` 。因此，我们需要 `copy_properties(func)` 返回一个接受一个参数的函数。目前，返回的是 `_copy`。因此，`@copy_properties(func)` 等价于 `@_copy`。因此，完整的等价式为 `wapper = copy_properties(func)(wapper)`

因此，需要将 `copy_properties` 进行柯里化

```python
def copy_properties(src):
    def _copy(dest):
        dest.__name__ = src.__name__  # 将 src 对象的名字赋值给 dest 对象
        dest.__doc__ = src.__doc__  # 将 src 的文档字符串赋值给 dest 对象
        return dest  # 这里必须返回 dest。否则 _copy 返回 None
    return _copy
```

![[Pasted image 20240616233606.png]]

给 `logger` 装饰器设置阈值，记录运行时间超过阈值的函数。这个阈值如何给定内？我们期望如下给定阈值

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
        # @functools.wraps(func)  # 复制函数属性
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
> - `logger()` 本身就是函数调用，可以如果需要参数，不在需要柯里化
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

### `update_wrapper`  和 `wraps`

属性更新 `functools.update_wrapper` 解决了 [[#补丁：伪装更彻底]] 中的问题。这是 Python 标准库中提供的一个函数

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

只需要在返回 `wapper` 之前调用该函数即可

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

`functools` 模块提供了一个 `wraps` 装饰器函数，本质上调用的是 `update_wrapper`，它就是一个属性复制函数。使用 **偏函数** 实现

```python
def wraps(wrapped,
          assigned = WRAPPER_ASSIGNMENTS,
          updated = WRAPPER_UPDATES):
    return partial(update_wrapper, wrapped=wrapped,
                   assigned=assigned, updated=updated)
```

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

## 装饰器进阶

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
> 	- 执行过两次：每调用一次 `logger` 就会执行一次 `wapper` 的定义，`wapper` 对象就会被 `wraps` 装饰一次
> - `wrapper` 的 `__name__` 等属性被覆盖过几次?
> 	- 不同 `wrapper` 对象的的 `__name__` 只会被 `wraps` 装饰的时候覆盖一次
> - `add.__name__` 打印什么名称?
> 	- 打印 `add`
> - `minus.__name__` 打印什么名称?
> 	- 打印 `minus`

## 注解 `annotation`

Python是动态语言，变量可以随时被赋值并改变类型，也就是说 Python 的变量是运行时决定的

```python
def add(x, y):
	return x + y

print(add(4, 5))  # 9
print(add("Py", "thon"))  # Python
print(add([10], [11]))  # [10, 11]

print(add(4, "add"))  # TypeError
```

动态语言缺点：

1. **难发现**: 由于不能做任何类型检查，往往到了运行时问题才显现出来，或到了线上运行时才暴露出关

2. **难使用**: 函数使用者看到函数时，并不知道函数设计者的意图，如果没有详尽的文档，使用者只能猜测数据的类型。对于一个新的 API，使用者往往不知道该怎么使用，对于返回的类型更是不知道该怎么使用

动态类型对类型的约束不强，在小规模开发的危害不大，但是随着 Python 的广泛使用，这种缺点确实对大项目的开发危害非常大

如何解决这种问题呢?

1. **文档字符串**。对函数、类、模块使用详尽的使用描述、举例，让使用者使用帮助就能知道使用方式。但是，大多数项目管理不严格，可能文档不全，或者项目发生变动后，文档没有更新等等

2. **类型注解**:函数注解、变量注解

> [!important] 类型注解 **只是做类型提示**，不做类型检验
> - 注解可以让一些开发工具做出提示

### 函数注解

Python 3.5 版本提供了函数注解

```python
def add(x: int, y: int) -> int:
    """add 加法计算
    :param x int
    :param y int
    :return int
    """
    return x + y
```

在参数后面使用 `:type` 提示参数的类型为 `type`，这仅仅只是提示。返回值类型使用 `->type` 进行注解

函数对象的注解保存在它的 `__annotation__` 属性中，这是一个字典

```python
def add(x: int, y: int) -> int:
    """add 加法计算
    :param x int
    :param y int
    :return int
    """
    return x + y


if __name__ == "__main__":
    
    print(add.__annotations__) # {'x': <class 'int'>, 'y': <class 'int'>, 'return': <class 'int'>}

```

### 变量注解

在 Python 3.6 提供了变量的类型注解

```python
age: int = 122
age = "122"  
```

### 类型检查

有了注解，可以实现类型检查。对于错误的类型，强制报错。
```python
def add(x: int, y: int) -> int:
    """add 加法计算
    :param x int
    :param y int
    :return int
    """
    
    # 硬编码参数类型检查
    if isinstance(x, int) and isinstance(y, int):
        return x + y
    else:
        raise TypeError
```

> [!attention] **硬编码** 的参数类型检查是不好的。而且 **类型检查对于所有函数都有用**
> 应用装饰器完成类型检查

```python
def check(func):
    """参数类型检查装饰器"""
    
    annotation = func.__annotation__  # 这是一个字典，它是无序的。没有办法没有与参数的顺序进行对应
    
    return func

@check
def add(x: int, y: int) -> int:
    """add 加法计算
    :param x int
    :param y int
    :return int
    """
    return x + y
```

> [!warning] 由于 `func.__annotation__` 无法获得参数的顺序。为了获得参数的顺序，使用 `inspect` 标准库

## `inspect`  模块

`inspect` 模块提供了一些有用的函数帮助 **获取对象的信息**，例如模块、类、方法、函数、回溯、帧对象以及代码对象。例如它可以帮助你检查类的内容，获取某个方法的源代码，取得并格式化某个函数的参数列表，或者获取你需要显示的回溯的详细信息。

该模块提供了4种主要的功能：**类型检查**、**获取源代码**、**检查类与函数**、**检查解释器的调用堆栈**

`inspect` 模块可以获取 `Python` 中各种对象信息
- `inspect.isfunction(add)`: 是否是函数
- `inspect.ismethod(pathlib.Path().absolute)`: 是否是类的方法，要绑定
+ `inspect.isgenerator(add))`: 是否是生成器对象
+ `inspect.isgeneratorfunction(add))`: 是否是生成器函数
+ `inspect.isclass(add))`: 是否是类
+ `inspect.ismodule(inspect))`: 是否是模块
+ `inspect.isbuiltin(print))`: 是否是内建对象

### 使用 `Signature` 对象对可调用对象进行内省

`Signature` 对象代表一个可调用对象的 **调用签名** 及其 **返回值标**。 要获取一个 `Signature` 对象，可使用 `signature()` 函数

`signature` 返回一个给定 `callable` 的 `Signature` 对象
```python
inspect.signature(callable, *, follow_wrapped=True, globals=None, locals=None, eval_str=False)
```

> [!seealso] 完整内容查看 [inspect](https://docs.python.org/zh-cn/3/library/inspect.html)

`inspect.Parameter` 对象提供了描述了可调用对象的参数类。其中 $4$ 个属性
- `name`，参数名，字符串
- `default`，缺省值。如果没有缺省值，标记为 `Parameter.empty`
- `annotation`, 类型注解
- `kind`，参数类型
	- `POSITIONAL_ONLY`，只接受仅位置传参
	- `POSITIONAL_OR_KEYWORD`，可以接受关键字或者位置传参
	- `VAR_POSITIONAL`，可变位置参数，对应 `*args`
	- `KEYWORD_ONLY`，对应 `*` 或者 `*args` 之后的出现的非可变关键字形参，只接受关键字传参
	- `VAR_KEYWORD`，可变关键字参数，对应 `**kwargs`
	- `empty`，特殊类，用来标记 `default` 属性或者 `annotation` 属性为空

## 应用
### 参数检查装饰器实现

请检查用户的输入是否符合参数类型注解的要求

分析:
+ 调用时，用户才会传入实参，才能判断实参是否符合类型要求
+ 调用时，让用户感觉上还是调用原函数
+ 如果类型不符，提示用户

先实现对 `add` 函数的参数类型的提取

```python
import functools
import inspect


def check(func):
    """参数类型检查装饰器"""
    
    @functools.wraps(func)
    def wrapper(*args, **kwargs):
        # annotation = func.__annotation__  # 这是一个字典，它是无序的。没有办法没有与参数的顺序进行对应
        
        # 参数检查
        signature = inspect.signature(func)  # 获取可调用对象 func 的签名
        
        parameters = signature.parameters  # 有序字典。保存了 func 参数的名字和注解
        # OrderedDict({'x': <Parameter "x: int">, 'y': <Parameter "y: int">})
        
        parameters_values = list(parameters.values())  # 获取所有的有序 Parameter 对象
        # args (3, 4)：位置传参，应该优先解决
        for arg, param in zip(args, parameters_values):
            if param.annotation is not param.empty and not isinstance(arg, param.annotation):
                raise TypeError(f"{param.name}'s type must be {param.annotation}, but got {type(arg)}")
        
        # kwargs: 关键字传参
        for key, value in kwargs.items():
            if parameters[key].annotation is not parameters[key].empty and not isinstance(value, parameters[key].annotation):
                raise TypeError(f"{key} must be {parameters[key].annotation},  but got {type(value)}")
        
        # 参数检查通过，调用原始可调用对象
        res = func(*args, **kwargs)
        
        # 还可以进行返回值检查
        return res
    return wrapper


@check
def add(x, y: int) -> int:
    """add 加法计算
    :param x int
    :param y int
    :return int
    """
    return x + y
```

> [!faq] `*args` 和 `**kwargs` 参数类型检查如何完成？

### `run_time` 装饰器

实现 `run_time` 装饰器，用于记录每次函数调用的运行时间。例如，统计递归计算第 $30$ 项斐波拉契数列的执行时间

```python

import functools
import sys
import time


def run_time(func):
    @functools.wraps(func)
    def wapper(*args, **kwargs):
        start = time.time()
        result = func(*args, **kwargs)
        end = time.time()
        print(f"{func.__name__}({args, kwargs}) run {end - start} s", file=sys.stderr)
        return result

    return wapper


@run_time  # fib = run_time(fib) ==> fib = wapper
def fib(n:int) -> int:
    if n <= 0:
        # 解决传递进来的参数小于等于 0 没有有定义
        raise ValueError("n must be greater than 0")
    if  n <= 2:
        return 1
    return fib(n-1) + fib(n-2)
```

执行 `fib(30)` 的输出如下
```
fib(((1,), {})) run 0.0 seconds
fib(((3,), {})) run 0.0 seconds
fib(((2,), {})) run 0.0 seconds
fib(((4,), {})) run 0.0 seconds
fib(((2,), {})) run 0.0 seconds
fib(((1,), {})) run 0.0 seconds
fib(((3,), {})) run 0.0 seconds
fib(((5,), {})) run 0.0 seconds
....
fib(((1,), {})) run 0.0 seconds
fib(((3,), {})) run 0.0 seconds
fib(((2,), {})) run 0.0 seconds
fib(((4,), {})) run 0.0 seconds
fib(((6,), {})) run 0.0 seconds
fib(((8,), {})) run 0.0 seconds
fib(((10,), {})) run 0.0010006427764892578 seconds
....
fib(((11,), {})) run 0.0010006427764892578 seconds
fib(((2,), {})) run 0.0 seconds
fib(((1,), {})) run 0.0 seconds
fib(((3,), {})) run 0.0 seconds
fib(((2,), {})) run 0.0 seconds
fib(((4,), {})) run 0.0 seconds
fib(((2,), {})) run 0.0 seconds
fib(((1,), {})) run 0.0 seconds
fib(((3,), {})) run 0.0 seconds
fib(((5,), {})) run 0.0 seconds
fib(((2,), {})) run 0.0 seconds
fib(((1,), {})) run 0.0 seconds
fib(((3,), {})) run 0.0 seconds
fib(((2,), {})) run 0.0 seconds
.....
fib(((12,), {})) run 0.0010006427764892578 seconds
fib(((14,), {})) run 0.002004384994506836 seconds
fib(((16,), {})) run 0.0065081119537353516 seconds
fib(((18,), {})) run 0.019594907760620117 seconds
fib(((20,), {})) run 0.06718230247497559 seconds
fib(((22,), {})) run 0.14810657501220703 seconds
fib(((24,), {})) run 0.3696153163909912 seconds
fib(((26,), {})) run 0.9358160495758057 seconds
fib(((28,), {})) run 2.405916929244995 seconds
fib(((30,), {})) run 5.889294862747192 seconds
```

>[!iimportant] 存在许多无用的函数调用，这就是斐波拉契数列递归计算的低效的原因
