# 函数高级

## 函数对象

Python 中函数是一个对象，它有一些属性。通过内置函数 `dir()` 可以查看对象的属性

```python
def foo(a, b, /, x=5, y=10, *args, m=10, n, **kwargs):  
    print(a, b, x, y, args, m, n, kwargs)  
  
print(dir(foo))
```

输入如下

```
['__annotations__', '__builtins__', '__call__', '__class__', '__closure__', '__code__', '__defaults__', '__delattr__', '__dict__', '__dir__', '__doc__', '__eq__', '__format__', '__ge__', '__get__', '__getattribute__', '__getstate__', '__globals__', '__gt__', '__hash__', '__init__', '__init_subclass__', '__kwdefaults__', '__le__', '__lt__', '__module__', '__name__', '__ne__', '__new__', '__qualname__', '__reduce__', '__reduce_ex__', '__repr__', '__setattr__', '__sizeof__', '__str__', '__subclasshook__', '__type_params__']
```

我们需要重点关注下表列出的几个属性

| 属性                | 描述             |
| :---------------- | -------------- |
| `__annotations__` | 函数的类型注解        |
| `__defaults__`    | 保存位置和关键字参数的默认值 |
| `__kwdefaults__`  | 保存仅关键字参数的默认值   |
| `__doc__`         | 函数的文档字符串       |
| `__closure__`     | 闭包函数引用的自由变量    |
| `__globals__`     | 函数引用的全局遍历      |

## 函数是如何调用的？

Python 中函数的调用使用 [[栈和队列#栈]] 进行管理。关于函数调用原理的详细内容请参考 [[过程]]

## 递归函数

**函数** 直接或者间接 **调用自身** 就是 **递归函数**。递归需要边界条件，当边界条件不满足时，向下递归；当边界条件满足时，递推结束并返回

例如，斐波拉契数列递推公式如下。编写递归函数求斐波拉契数列的第 $n$ 项是非常自然的

$$
fib(n) = \begin{cases} 1 & n=1, n=2 \\ fib(n-1) + fib(n-2) & n \ge 3 \end{cases}
$$

```python
def fib(n:int) -> int:
    
    if  n <= 2:
        return 1
    return fib(n-1) + fib(n-2)  # 尾递归

fib(5)
```

> [!warning] 
> 
> 这个实现 **存在性能问题**，每次计算的结果在函数返回后就被丢弃了，当第二次计算相同的 $n$ 时，又会再次执行递归
> 
> 在 [[Python：函数基础#定义与调用]] 中我们给出了一个循环实现的斐波拉契数列
> 

下图分析了使用递归计算斐波拉契数列第 $n$ 项出现性能问题的原因。以计算斐波拉契数列的第 $5$ 项为例

![[Pasted image 20250118173726.png]]

要计算 `fib(5)`，就需要计算 `fib(4)` 和 `fib(3)`；在计算 `fib(4)` 时，需要计算`fib(3)` 和 `fib(2)`，**但是 `fib(3)` 在计算 `fib(5)` 时就已经计算出来了**，但是被丢弃。这就是采用递推公式计算斐波拉契数列第 $n$ 项出现性能问题的主要原因

解决该问题的主要方法就是将已经经计算出来的值保存下来，当下一次在计算的时候就直接返回

```python
def fib2(n: int, *, result=None) -> int:
    if n <= 0:
        raise ValueError('n must be greater than 0')
    if result is None:
        result = [0] * (n + 1)  # 创建结果缓存集
    if n <= 2:  # 递归边界条件
        return 1
    if result[n] == 0:  # 第 n 项不存在
        result[n] = fib2(n-1, result=result) + fib2(n-2, result=result)
    return result[n]
```

## 高阶函数

Python 中的函数是一个对象，其他对象能做的事情函数对象也能做。也就是说，函数可以赋值给一个变量，也可以作为实参传递给函数，还可以作为函数的返回值

> [!tip] 高阶函数
> 
> 如果一个函数可以接收一个函数作为参数或者可以返回一个函数，那么该函数就是高阶函数
> 

```python hl:3
def counter(base):
	def inc(step=1):
		base += step  
		# 会在 inc 的局部名字空间中创建了 base 名字，
		# base 并没有绑定对象，就对其进行了引用。
		# 这句会触发 UnboundLocalError 异常
		return base
	return inc
```

> [!question] 上述函数是否是高阶函数？是否有闭包？代码是否正确？
> - 是高阶函数，`counter` 返回了一个函数 `inc`
> - 没有闭包，内层函数没有引用上层函数中的局部变量
> - 代码是不正确的，函数会触发 `UnboundLocalError`。因为，`inc` 函数在执行 `base += step` 时，在 `inc` 局部名字空间中创建了 `base` 名称，此时 `base` 尚未绑定对象，此时又引用了局部未绑定的变量 `base`，因此触发 `UnboundLocalError`

只需要在第 $3$ 行之前加上 `nonlocal` 声明即可

```python
def counter(base):
	def inc(step=1):
		nonlocal base
		base += step
		return base
	return inc
```

> [!question] `f1=counter(5), f2=counter(5)` 请问 `f1==f2` 吗？ `f1 is f2` 吗？
> `counter(5)` 每次调用 Python 解释器都会创建不同函数对象存放在堆内存中，然后将这新创建对象的引用保存在 `counter` 的栈空间中名字 `inc` 表示的内存中，最后返回 `inc` 保存的对象的引用，当 `counter(5)` 返回它的栈空间被销毁，所以 `f1 is not f2`
> 
> 函数对象是不可比较的，对于不可比较的对象， `==` 会默认比较其内存地址

两次调用 `f1 = counter(5)` 和 `f2 = counter(5)` 的内存空间引用变化

![[Pasted image 20250118181508.png]]

### 内置函数 sorted()

内置函数 `sorted(iterable, *, key=None, reverse=False)` 返回一个列表，其中的元素按照来自 `iterable`。`key` 指定应用于每个元素上的可调用对象

```python
>>> sorted((5,3,1,4,9))
[1, 3, 4, 5, 9]
>>> sorted((5,3,1,4,9, "a", "c"), key=str)
[1, 3, 4, 5, 9, 'a', 'c']
>>> sorted((5,3,1,4,9, "a", "c"), key=str, reverse=True)
['c', 'a', 9, 5, 4, 3, 1]
```

## 迭代器与生成器

### 迭代器

**迭代器** 表示一连串数据流的对象，可以通过 `next(iterator)` 从迭代器对象 `iterator` 中获取数据流中的项。当没有数据可用时则引发 `StopIteration` 异常

> [!tip] 
> 
> 当 `next(iterator)` 触发 `StopIteration` 异常时，表明迭代器对象 `iterator` 中的数据项以耗尽。此时，继续通过 `next(iterator)` 获取数据项也只会再次触发 `StopIteration` 异常
> 

内置函数 `iter(iterable)` 返回一个由可迭代对象 `iterable` 包装为一个迭代器。更多关于迭代器的其他相关知识我们将会在 [[Python：魔术方法]] 中介绍

```python
squares = [1, 4, 9, 16, 25]  # 可迭代对象
squares_iterator = iter(squares)  # 通过可迭代对象生成迭代器
print(next(squares_iterator))  # 1
print(next(squares_iterator))  # 4
print(next(squares_iterator))  # 9
print(next(squares_iterator))  # 16
print(next(squares_iterator))  # 25
print(next(squares_iterator))  # StopIteration
```

> [!tip] 迭代器也是一种可迭代对象
> 

### 生成器

使用 `iter()` 创建迭代器需要在内存中提前存放一个可迭代对象。这些就限制了迭代器可迭代的长度。Python 提供了另一种称为 **生成器** 的迭代器。

> [!tip] 生成器是一种特殊的迭代器

生成器是通过调用 **生成器函数** 得到的返回值创建。生成器函数和普通函数类似，但是不同的一点就是其中包含了 `yield` 表达式 以便于产生一系列的值供 `for` 循环或 `next()` 函数获取

> [!tip] 
> 
> **使用了 `yield` 语句的函数称为生成器函数**。这样的函数 **被调用时总是返回一个可以执行该函数体的迭代器对象**。
> + 通过 `next()` 可以导致该函数运行直到它使用 `yield` 返回一个值并暂停函数的执行
> + 当再次通过 `next()` 驱动该函数运行时，会从暂停位置开始继续执行直到遇见下一个 `yield`
> + 当函数执行 `return` 语句时，将触发 `StopInteration` 异常表示该迭代器将到达所返回的值集合的末尾
> 

内置函数 `range()` 就会返回一个生成器迭代器。其实现逻辑大致如下

```python
from typing import Iterator

def my_range(start: int, *args:tuple[int])->Iterator[int]:
    start = start
    stop = None
    step = None
    if len(args) == 0:
        stop = start
        start = 0
        step = 1
    elif len(args) == 1:
        stop = args[0]
        step = 1
    elif len(args) == 2:
        stop = args[0]
        step = args[1]
    while start < stop:
        yield start
        start += step
```

### 生成器对象的方法

生成器对象提供了下列方法用于控制生成器函数的执行

| 方法                       | 描述                                            |
| :----------------------- | :-------------------------------------------- |
| `generator.send(value)`  | 从生成器函数暂停位置执行，并将 `value` 发送给生成器，同时返回生成器产生的下一个值 |
| `generator.throw(value)` | 在生成器函数暂停位置引发一个异常，同时返回生成器产生的下一个值               |
| `generator.close()`      | 在生成器函数暂停位置引发 `GeneratorExit` 异常               |

> [!tip] 
> 
> 对于 `generator.send(value)` 方法发送一个非 `None` 值给刚开始的生成器对象时将触发 `TypeError`，因为生成器对象刚生成
> 
> 这也就意味着 `generator.send(None)` 等价于 `next(generator)`
> 

```python
def counter():
    for i in range(10):
        x = yield i
        print(x, "+++++")

c = counter()  # 返回一个生成器
# c.send(1)       # TypeError: 不能发送 non-None 值给刚开始的生成器对象
c.send(None)    # send (None) 等价于 next(c)，启动生成器
```

### yield from

如果生成器函数只需要从一个可迭代对象中 `yield` 每个一个元素时，我们可以遍历可迭代对象并 `yield` 每一个值。如下示例代码

```python
def counter(n):
    for i in range(n):
        yield i


g = counter(10)
next(g)
```

由于 `range(n)` 本质上就是一个可迭代对象，因此可以使用 `yield from` 简化上述代码

```python
def counter(n):
    yield  from range(n)

g = counter(10)
next(g)
```

