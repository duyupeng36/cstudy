# 函数高级

## 函数对象

在 Python 中，函数也是一个对象，函数名仅仅是对函数对象的引用。作为对象存在的函数，会有一些属性。内置函数 `dir()` 就可以查看通过对象能够访问的属性

```python
def foo(a, b, /, x=5, y=10, *args, m=10, n, **kwargs):
    print(a, b, x, y, args, m, n, kwargs)

print(dir(foo)) # 输出的结果如下
"""
['__annotations__', '__builtins__', '__call__', '__class__', '__closure__', '__code__', '__defaults__', '__delattr__', '__dict__', '__dir__', '__doc__', '__eq__', '__format__', '__ge__', '__get__', '__getattribute__', '__getstate__', '__globals__', '__gt__', '__hash__', '__init__', '__init_subclass__', '__kwdefaults__', '__le__', '__lt__', '__module__', '__name__', '__ne__', '__new__', '__qualname__', '__reduce__', '__reduce_ex__', '__repr__', '__setattr__', '__sizeof__', '__str__', '__subclasshook__']
"""
```

现在，我们需要了解下表列出的几个属性

| 属性                | 描述         |
| :---------------- | :--------- |
| `__annotations__` | 函数注解       |
| `__defaults__`    | 默认参数       |
| `__kwdefaults__`  | 仅关键字参数的默认值 |
| `__doc__`         | 文档字符串      |

下面我们通过 `__defaults__` 来研究默认参数的求值规则

```python
def foo5(a, L=[]):
     L += [a]  # 等价于 L.extend([a])  是在 L 指向的列表对象上扩展
     print(L)

print(foo5.__defaults__)  # ([],)
foo5(1)  # [1]
foo5(1)  # [1, 1]
foo5(1)  # [1, 1, 1]
print(foo5.__defaults__)  # ([1, 1, 1],)
```

`L += [a]` 等价于 `L.extend([a])`，即在 `L` 指向的列表对象上进行扩展。让我们将 `L += [a]` 替换为 `L = L + [a]`

```python
def foo5(a, L=[]):
     L = L + [a]  # L + [a] 会生成一个全新的列表对象，并让 L 引用这个新列表对象
     print(L)

print(foo5.__defaults__)  # 默认值在 __defaults__ 中，依旧引用最初的列表对象
foo5(1)  # [1]
foo5(1)  # [1]
foo5(1)  # [1]
print(foo5.__defaults__)  # ([],)
```

> [!important]
> 
> `x += y` 和 `x = x + y` 对于不可变对象没什么区别，但是对于可变对象而言，`x+=y` 是原地修改，`x = x + y` 会产生一个全新的可变对象
> 

## 匿名函数

在 Python 匿名函数使用关键字 `lambda` 定义，例如，`lambda a, b: a + b` 函数返回两个参数的和

匿名函数 **可用于任何需要函数对象的地方**。在语法上，**匿名函数只能是单个表达式**。在语义上，它只 **是常规函数定义的语法糖**。与嵌套函数定义一样，匿名函数可以引用包含作用域中的变量

```python
>>> def make_incrementor(n):
...     return lambda x: x+n  # 闭包
... 
>>> f = make_incrementor(10)
>>> f(0)
10
>>> f(2)
12
```

上例用 `lambda` 表达式返回函数。此外，还可以把匿名函数用作传递的实参

```python
>>> pairs = [(1, 'one'), (2, 'two'), (3, 'three'), (4, 'four')]
>>> pairs.sort(key=lambda pair:pair[1])  # 将函数当实参传递
>>> pairs
[(4, 'four'), (1, 'one'), (3, 'three'), (2, 'two')]
```

一个完整的 `lambda` 表达式的语法如下

```python
lambda 形参列表:返回值表达式
```

> [!tip] 
> 
> 形参列表可以向定义普通函数那样定义形参列表
> 
> 返回值表达式只能是简单的表达式
> 

## 递归

> [!tip] 
> 
> **函数** 直接或者间接 **调用自身** 就是 **递归**。**递归一定要有边界条件**，当边界条件不满足的时候，递归前进；当边界条件满足的时候，递归返回
> 

例如，使用递归实现 斐波拉契数列

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


> [!bug] 
> 
> 这个实现存在性能问题，每次计算的结果在函数返回后就被丢弃了，当第二次计算相同的 $n$ 时，又会再次执行递归
> 
> 在 [[Python：函数基础#函数的定义与调用]] 中我们给出了一个循环实现的斐波拉契数列
> 

当我们要计算 `fib(5)` 时，首先需要计算 `fib(4)` 和 `fib(3)`；在计算 `fib(4)` 时，需要计算`fib(3)` 和 `fib(2)`，**但是 `fib(3)` 在计算 `fib(5)` 时就已经计算出来了**，但是被丢弃。如下图

![[Pasted image 20250402220045.png]]

因此，需要保存第一次计算的结果，后续再次计算直接返回即可。

```python
n = 101

result = [0] * (n + 1)

def fib(n:int) -> int:
    if n <= 0:
         raise ValueError("n must be greater than 0")
    if 0 < n <= 2:
            return 1
    
    if result[n] == 0:
        result[n] = fib(n-1) + fib(n-2)
    
    return result[n]
```

> [!question] 
> 
> 该版本依旧有问题：使用了全局变量！
> 
> 尽量不要使用全局变量，全局变量的使用让函数与当前环境紧密耦合，不利于函数的封装
> 

参考 [[Python：函数基础#函数的定义与调用]] 中定义 `fib()` 函数，我们使用递归替代循环

```python
def fib(n: int, /,first=1, second=1) -> int:
    if n <= 0:
        raise ValueError("n must be greater than 0")  # 防止调用者传入小于等于 0 的参数
    if n < 3:
        return second
    first, second = second, first + second
    return fib(n - 1, first, second)
```

> [!note] 
> 
> 该版本使用递归和参数传递 **模拟循环**。只是避免对同一个数的多次递归计算
> 
> 注意，Python 对函数的调用有深度限制。
> 

下图描述了模拟循环版本的 `fib(5)` 的调用过程

![[Pasted image 20250402220453.png]]

## 生成器函数

我们在 [[Python：推导式]] 中简单介绍了生成器，并使用生成器推导式创建生成器对象。下面我们来介绍生成器函数

> [!tip] 
> 
> 所谓的生成器函数就调用时会返回一个生成器对象的函数。这个函数与普通函数非常相似，只是生成器函数使用了 `yield` 关键字返回一些列需要经过复杂计算的值
> 
> + 对于哪些简单的无需复杂计算的值我们通常使用生成器推导式
> 

一个使用 `yield` 语句的函数或方法被称为 **生成器函数**。 这样的函数在被调用时，总是返回 **可以执行该函数体的迭代器对象**。换句话说，调用生成器函数会创建一个对象，这对象称为生成器迭代器，简称生成器

> [!tip] 
> 
> 也就是说，生成器对象可以使用 `for` 循环或者 `next()` 函数驱动生成器函数的函数体执行，当遇见 `yield` 语句时，将 `yield` 后面表达式的值返回
> 

当这个生成函数体执行 `return` 语句，将引发 `StopIteration` 异常并且该迭代器将到达所返回的值集合的末尾

> [!attention] 
> 
> 请注意：任何函数默认都有一个 `return` 语句，该语句会返回 `None`
> 

> [!important] 
> 
> `yield` 关键字会将其后面表达式的值返回，与此同时，函数体的执行被暂停。当再次被 `next()` 函数或者 `for` 循环驱动时，继续从 `yield` 关键字位置继续执行
> 
> 也就是说：**yield 关键字提供了暂停函数执行并且可以在暂停处恢复执行的能力**
> 

内置函数 `range()` 就会返回一个生成器。下面看一些实现逻辑

```python
def my_range(start: int, end: int, step: int = 1):
    if isinstance(start, int) and isinstance(end, int) and isinstance(step, int):
        while start < end:
            yield start  # 返回 start 的值，并暂停函数的执行
            start += step
    else:
        raise TypeError("类型错误")
```

> [!summary] 
> 
> 生成器函数调用时返回的是一个生成器对象
> 
> + 通过 `next()` 可以驱动生成器对象的执行
> + 遇到 `yield` 时，会返回它后面的值，并暂停其执行
> + 再次执行会执行到下一个 `yield` 语句又会暂停执行
> + 如果遇到 `return` 语句，就会触发 `StopIteration` 异常
> 


从一个可迭代对象中 `yield` 每个一个元素，我们可以通过 `for` 循环遍历这个可迭代对象并使用 `yield` 返回

```python
def foo():
    for i in range(10):
        yield i
```

上述示例会显得比较冗余，因此 Python 提供了 `yield from` 简化上述代码

```python
def foo():
	yield from range(10)

g = foo()
next(g)  # 0
next(g)  # 1
```

### 生成器对象的方法

生成器对象提供了下面几个方法

| 方法                       | 描述                           |
| :----------------------- | :--------------------------- |
| `generator.send(value)`  | 向生成器发送一个值 `value`，并恢复生成器的执行  |
| `generator.throw(value)` | 在生成器暂停的位置引发一个异常              |
| `generator.close()`      | 生成器函数暂停的位置引发 `GeneratorExit` |

`generator.throw()` 不仅在暂停位置引发异常，还会返回生成器的下一个值。如果生成器没有产生下一个值，则抛出 `StopIteration` 异常。如果生成器函数没有捕获传入的异常，或是引发了另一个异常，则该异常会被传播给调用方。

## 高阶函数

在 Python 中函数是 **一等公民**，即函数也是对象，可以赋值给普通变量，也可以作为函数的返回值或参数

> [!tip] 
> 
> 所谓的高阶函数就是：**接收函数作为参数** 或者 **返回一个函数**
> 

例如下列代码中，`counter()` 函数返回了一个函数 `inc()`。

```python
def counter(base):
	def inc(step=1):
		base += step
		return base
	return inc
```

> [!question] 
> 
> 是否是高阶函数？是否有闭包？代码是否真确？
> - 是高阶函数，`counter` 返回了一个函数 `inc`
> - 没有闭包，内层函数没有引用上层函数中的局部变量
> - 代码是不正确的，函数会触发 `UnboundLocalError`。因为，`inc` 函数在执行 `base += step` 时，在 `inc` 局部名字空间中创建了 `base` 名称，此时 `base` 尚未绑定对象，此时又引用了局部未绑定的变量 `base`，因此触发 `UnboundLocalError`
> 

只需要在第 $3$ 行之前加上 `nonlocal` 声明即可

```python
def counter(base):
	def inc(step=1):
		nonlocal base
		base += step
		return base
	return inc
```

> [!question] 
> 
> `f1 = counter(5), f2 = counter(5)` 请问 `f1 == f2` 吗？ `f1 is f2` 吗？
> 
> `counter(5)` 每次调用 Python 解释器都会创建不同函数对象存放在堆内存中，然后将这新创建对象的引用保存在 `counter` 的栈空间中名字 `inc` 表示的内存中，最后返回 `inc` 保存的对象的引用，当 `counter(5)` 返回它的栈空间被销毁，所以 `f1 is not f2`
> 
> 函数对象是不可比较的，对于不可比较的对象， `==` 会默认比较其内存地址
> 

两次调用 `f1 = counter(5)` 和 `f2 = counter(5)` 的内存空间引用变化

![[Pasted image 20250402224303.png]]

### 高阶函数的应用：实现内置函数 `sorted` 

**仿照内建函数 `sorted`**，请自行实现一个 `sort` 函数(不用使用内建函数)，能够为列表元素排序：返回一个列表，使用 `key` 传递比较规则，`reverse` 控制降序还是升序

思考: 通过练习，思考 `sorted` 函数的实现原理，`map`、`filter` 函数的实现原理

```python
def sort(iterable, *, key=None, reverse=False):
    """仿照内置函数 sorted 的行为实现 sort 函数
    
    这里使用插入排序
    """
    result = []
    for item in iterable:
        result.append(0)  # 扩容列表

        # 开始排序
        i = len(result) - 1
        # 寻找插入位置
        # (result[i-1] > item) if not reverse else (result[i-1] < item)
        #  - 如果 reverse 为 False，则进行 (result[i-1] > item) 的判断，执行升序排列
        #  - 如果 reverse 为 True，则进行 (result[i-1] < item) 的判断，执行降序排列
        while i > 0 and (
            (
                # 下面的表达式是应用 key
                # 如果 key 没有定义，则直接比较元素
                # 如果 key 定义了，则比较 key 的结果
                (result[i-1] > item) 
                if key is None else 
                (key(result[i-1]) > key(item))
            ) 
            
            if not reverse else
            
            (
                (result[i-1] < item)
                if key is None else
                (key(result[i-1])< key(item))
            )
        ):
            result[i] = result[i-1]
            i-= 1
        result[i] = item
    return result
```

> [!tip] 
> 
> `result` 列表肯定是有序的，因此在查找插入点可以使用二分查找
> 

```python
from typing import Iterable, Callable, MutableSequence


def binary_search(seq: MutableSequence, target, *, key: Callable=None, reverse: bool=False)->int:
    """二分查找
    返回插入位置
    """
    low, high = 0, len(seq) - 1
    while low <= high:
        
        mid = (low + high) // 2


        if (
            (
                target < seq[mid]
                if key is None else 
                key(target) < key(seq[mid])
            ) 
            if not reverse else 
            (
                seq[mid] < target
                if key is None else 
                key(seq[mid]) < key(target)
            )
        ):
            
            high = mid - 1  

        elif (
            (
                seq[mid] < target
                if key is None else 
                key(seq[mid]) < key(target)
            ) 
            if not reverse else 
            (
                seq[mid] > target
                if key is None else 
                key(seq[mid]) > key(target)
            )
        ):
            low = mid + 1
        else:
            return mid
        
    return low
        

def sort(iterable: Iterable, *, key: Callable=None, reverse: bool=False):
    """仿照内置函数 sorted 的行为实现 sort 函数
    
    这里使用插入排序。使用二分查找所插入位置
    """
    result = []
    for item in iterable:
        # 寻找插入位置
        i = binary_search(result, item, key=key, reverse=reverse)
        
        result.append(0)  # 扩容列表

        # 移动插入之后的元素向后移动一位
        for j in range(len(result)-1, i, -1):
            result[j] = result[j-1]
        result[i] = item
    return result
```

简化比较的条件表达式：`key` 如果为 `None` 意味着使用原始值比较，可以默认给它一个返回原始值的函数 `lambda x : x`

```python
from typing import Iterable, Callable, MutableSequence


def binary_search(seq: MutableSequence, target, *, key: Callable=None, reverse: bool=False)->int:
    
    if key is None:
        key = lambda x: x
    
    low, high = 0, len(seq) - 1
    while low <= high:
        mid = (low + high) // 2


        if (
            (
                key(target) < key(seq[mid])
            ) 
            if not reverse else 
            (
                key(seq[mid]) < key(target)
            )
        ):
            
            high = mid - 1  

        elif (
            (
                key(seq[mid]) < key(target)
            ) 
            if not reverse else 
            (
                key(seq[mid]) > key(target)
            )
        ):
            low = mid + 1
        else:
            return mid
        
    return low
        

def sort(iterable: Iterable, *, key: Callable=None, reverse: bool=False):
    """仿照内置函数 sorted 的行为实现 sort 函数
    
    这里使用插入排序。使用二分查找所插入位置
    """
    result = []
    for item in iterable:
        # 寻找插入位置
        i = binary_search(result, item, key=key, reverse=reverse)
        
        result.append(0)  # 扩容列表

        # 移动插入之后的元素向后移动一位
        for j in range(len(result)-1, i, -1):
            result[j] = result[j-1]
        result[i] = item
    return result
```
