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





