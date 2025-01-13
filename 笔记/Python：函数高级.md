# 函数高级

## 函数对象

Python 中函数是一个对象，它会有一些属性。内置函数 `dir` 可以查看对象的属性

```python
def foo(a, b, /, x=5, y=10, *args, m=10, n, **kwargs):
    print(a, b, x, y, args, m, n, kwargs)

print(dir(foo)) # 输入如下
"""
['__annotations__', '__builtins__', '__call__', '__class__', '__closure__', '__code__', '__defaults__', '__delattr__', '__dict__', '__dir__', '__doc__', '__eq__', '__format__', '__ge__', '__get__', '__getattribute__', '__getstate__', '__globals__', '__gt__', '__hash__', '__init__', '__init_subclass__', '__kwdefaults__', '__le__', '__lt__', '__module__', '__name__', '__ne__', '__new__', '__qualname__', '__reduce__', '__reduce_ex__', '__repr__', '__setattr__', '__sizeof__', '__str__', '__subclasshook__']
"""
```

现在，我们需要关注几个属性
- `__annotations__` ：保存函数的类型注解
- `__defaults__`：保存函数的 [[Python：函数基础#位置和关键字参数]] 的缺省值
- `__kwdefaults__`：保存函数的仅关键字参数的缺省值
- `__doc__`：函数的文档字符串
- `__dict__`：函数的局部名字空间

分析下面的代码

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

> [!tip] 
> 
> `L+=[a]` 等价于 `L.extend([a])`，是在 `L` 指向的列表对象上进行扩展
> 

当我们把 `L = L + [a]` 情况就发生了变化。`L = L + [a]` 会产生一个全新的对象，而不是在 `L` 指向上的列表对象上进行添加

```python
def foo5(a, L=[]):
     L = L + [a]  # 会产生  一个全新的对象，覆盖 L 的引用
     print(L)

print(foo5.__defaults__)  # ([],)
foo5(1)  # [1]
foo5(1)  # [1]
foo5(1)  # [1]
print(foo5.__defaults__)  # ([],)
```

**重要补充**

> [!important] 重要的
> `x += y` 和 `x = x + y` 对于不可变对象没什么区别，但是对于可变对象而言，`x+=y` 是原地修改，`x = x + y` 会产生一个全新的可变对象

## 再提名字空间与名字解析顺序

- **内置名称集合**（包括 `abs()` 函数以及内置异常的名称等）
- **模块的全局名称**
- **函数调用中的局部名称**
- **对象的属性集合也是命名空间的一种形式**

![[Pasted image 20250113165736.png]]

命名空间是在 **不同时刻创建的，且拥有不同的生命周期**
+ **内置名称空间**：在 Python _解释器启动时创建_，并且永远不会删除
+ **模块的全局命名空间**：在 _读取模块定义时创建_，模块的命名空间也会持续到解释器退出
+ 从脚本文件读取或交互式读取的，由解释器顶层调用执行的语句是 `__main__` 模块调用的一部分，也拥有自己的 **全局命名空间**
+ **函数的局部命名空间**：在函数 _被调用时被创建_，并在函数返回或抛出未在函数内被处理的异常时，被删除。当然，每次递归调用都有自己的局部命名空间

> [!important] 名字解析顺序
> 名字由局部名字空间开始查询一直到内建名字空间，如果最后在内建名字空间都没查询到遍历，则触发 `NameError`

## 函数的销毁

定义一个函数就是创建一个函数对象，**函数名指向的就是函数对象**。可以使用 `del` 语句解除标识符与函数对象的引用，使其引用计数减。也可以使用同名标识符覆盖原有定义，**本质上也是使其外用计数减 $1$**

> [!important] 
> 
> 函数也是对象，也不例外，是否销毁，还是看引用计数是否减为 $0$
> 

## 匿名函数(Lambda 表达式)

**`lambda` 关键字用于创建 _小巧_ 的匿名函数**。例如，`lambda a, b: a+b` 函数返回两个参数的和

`Lambda` 函数 **可用于任何需要函数对象的地方**。在语法上，**匿名函数只能是单个表达式**。在语义上，它只 **是常规函数定义的语法糖**。与嵌套函数定义一样，`lambda` 函数 **可以引用包含作用域中的变量**

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

上例用 `lambda` 表达式返回函数。还 **可以把匿名函数用作传递的实参**

```python
>>> pairs = [(1, 'one'), (2, 'two'), (3, 'three'), (4, 'four')]
>>> pairs.sort(key=lambda pair:pair[1])  # 将函数当实参传递
>>> pairs
[(4, 'four'), (1, 'one'), (3, 'three'), (2, 'two')]
```

`lambda` 表达式是定义函数的语法糖

```python
lambda 形参列表:返回值表达式
``` 

```python
lambda pos,/,pos_or_kw,*,kw : (pos, pos_or_kw, ke)
```

下面是 `lambda` 表达式的一些练习

```python
>>> (lambda *args: {i for i in args})(*range(5)) # args 中搜集的是 0, 1,2,3,4
{0, 1, 2, 3, 4}
>>> (lambda *args: {i for i in args})(range(5)) # args 中收集的 range(0, 5) 这对象
{range(0, 5)}
>>> (lambda *args: {i+1 for i in args})(range(5)) # args 中收集的 range(0, 5) 这对象。range对象不能与 int 类型对象相加
Traceback (most recent call last):
  File "<stdin>", line 1, in <module>
  File "<stdin>", line 1, in <lambda>
TypeError: unsupported operand type(s) for +: 'range' and 'int'
>>> (lambda *args: {i % 3 for i in args})(*[1,2,3,4,5,6,7,8,10])
{0, 1, 2}
>>> sorted(["a", 1, "b", 12, "c", "28"], key=lambda x: x if isinstance(x, int) else int(x, base=16))
[1, 'a', 'b', 12, 'c', '28']
```

### `map(function, iterable, *iterables)`

内置函数 `map` 返回一个将 `function` 应用于 `iterable` 的每一项，并产生其结果的 **迭代器**。 **如果传入了额外的 `iterables` 参数，则 `function` 必须接受相同个数的参数** 

被用于到从所有可迭代对象中并行获取的项。 当有多个可迭代对象时，**当最短的可迭代对象耗尽则整个迭代将会停止**。 

> [!summary] 内置函数 `map` 将 `iterable` 中的元素使用 `function` 映射为另一个值
> - `function` 的参数个数与 `iterable` 的个数相同

```python
>>> map(lambda x, y:(x ** y), (5, 6, 7, 8), (1,2,3,4))  # 返回一个迭代器
<map object at 0x71fc9f3cec20>
>>> list(map(lambda x, y:(x ** y), (5, 6, 7, 8), (1,2,3,4)))
[5, 36, 343, 4096]
```

### `filter(function, iterable)`

返回 `iterable` 中的元素使得  `function` 返回真值的元素构造一个**迭代器**。 `iterable` 可以是一个序列，一个支持迭代的容器或者一个迭代器

如果 `function` 为 `None`，则会使用标识号函数，也就是说，`iterable` 中所有具有假值的元素都将被移除

```python
>>> filter(None, [1,0,2,10,0])
<filter object at 0x71fc9f2e6080>
>>> list(filter(None, [1,0,2,10,0]))
[1, 2, 10]
>>> list(filter(lambda x: x ** 2 % 3, [1,2,4,5,10,9]))  # 9 被滤除了
[1, 2, 4, 5, 10]
```

## 函数调用原理

函数调用机制的一个 **关键特性**（大多数语言也是如此）在于 **使用了 _栈_ 数据结构提供的 _后进先出_ 的内存管理原则**

```python
def Q():
	pass

def P():
	Q()  # 在栈上申请空间，Q返回时，栈空间才会被释放
	# 到此处，Q 的栈空间才会被释放
	pass

P()  # 调用 P 会在栈上申请空间
# 此处时，P的栈空间被销毁
```

在函数P 调用函数Q 的例子中，可以看到当 Q 在执行时，P 以及所有向上追溯到 P 的调用链中的过程，都是暂时被挂起的
+ **当 Q 运行时**，它只需要为 **局部变量分配新的存储空间**，或 **设置到另一个过程调用**
+ **当 Q 返回时**，**任何被它分配的存储空间都会被释放**

因此，程序可以用栈来管理它的过程所需要的存储空间，栈和程序寄存器存放在传递控制和数据、分配内存所需要的信息
+ **当 P 调用 Q 时**，控制和数据信息添加到栈顶
+ **当 Q 返回时**，这些信息被释放

## 递归

**函数** 直接或者间接 **调用自身** 就是 **递归**。递归需要 **有边界条件**、递归前进段、递归返回段

**递归一定要有边界条件**，当边界条件不满足的时候，递归前进；当边界条件满足的时候，递归返回

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

> [!bug] 这个实现存在性能问题，每次计算的结果在函数返回后就被丢弃了，当第二次计算相同的 $n$ 时，又会再次执行递归
> 
> 在 [[函数基础#定义与调用函数]] 中我们给出了一个循环实现的斐波拉契数列


需要计算 `fib(4)` 和 `fib(3)`；在计算 `fib(4)` 时，需要计算`fib(3)` 和 `fib(2)`，**但是 `fib(3)` 在计算 `fib(5)` 时就已经计算出来了**，但是被丢弃。如下图

![[Pasted image 20250113165932.png]]

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

> [!question] 该版本依旧有问题：使用了全局变量！
> 尽量不要使用全局变量，全局变量的使用让函数与当前环境紧密耦合，不利于函数的封装

下面我们参考 [[Python：函数基础#定义与调用函数]] 中提供的 `fib` 循环版本来进行改造

```python
def fib(n: int, /,first=1, second=1) -> int:
    if n <= 0:
        raise ValueError("n must be greater than 0")  # 防止调用者传入小于等于 0 的参数
    if n < 3:
        return second
    first, second = second, first + second
    return fib(n - 1, first, second)
```

> [!note]  该版本使用递归和参数传递 **模拟循环**。只是避免对同一个数的多次递归计
> 注意，Python 对函数的调用有深度限制。

下图描述了 `fib(5)` 的调用过程

![[Pasted image 20250113170008.png]]

## 生成器函数

返回一个 **生成器迭代器** 的函数。它看起来很像普通函数，不同点在于其包含 **`yield` 表达式以便产生一系列值** 供给 `for` 循环使用或是通过 `next()` 函数 **逐一获取**

通常 **简单称为 _生成器_**，但在某些情况下也可能是指 生成器迭代器。如果需要清楚表达具体含义，请使用全称以避免歧义

一个使用 `yield` 语句的函数或方法被称为 **生成器函数**。 这样的函数 **在被调用时**，总是返回一个 **可以执行该函数体的 `iterator` 对象**：调用该迭代器的 `iterator.__next__()`方法将导致这个函数一直 **运行到它使用 `yield` 语句提供一个值**。 当这个 **函数执行 `return` 语句** 或 **到达函数体末尾** 时，将引发 `StopIteration` 异常并且该迭代器将到达所返回的值集合的末尾

**生成器函数所创建的对象** 称为 _生成器迭代器_。

**每个 `yield` 会将其后的表达式的值返回，并 _临时暂停处理_**，_记住当前位置执行状态_（包括局部变量和挂起的 `try` 语句）。当该 生成器迭代器 恢复时，它会 **_从离开位置继续执行_，接收外部传入的值**（这与每次调用都从新开始的普通函数差别很大）

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

> [!summary] 生成器函数调用时返回的是一个生成器对象
> - 通过 `next()` 可以驱动生成器对象的执行
> - 遇到 `yield` 时，会返回它后面的值，并暂停其执行
> - 再次执行会执行到下一个 `yield` 语句又会暂停执行
> - 如果遇到 `return` 语句，就会触发 `StopIteration` 异常

### 应用

#### 计数器

```python
def counter():
    def inc():
        i = 0
        while True:
	        yield i
            i += 1
            
    c = inc()  # 创建一个生成器对象
    return lambda: next(c)  # 内层函数引用了上层函数的变量 c，产生一个闭包

c = counter() # 1. 返回一个 lambda 函数
print(c())  # 0
print(c())  # 1
print(c())  # 2
```

#### 斐波拉契数列

迭代生成前 $n$ 项斐波拉契数列

```python
def fib(n: int):
    a, b = 0, 1
    for _ in range(n):
        yield b
        a, b = b, a + b
```

### 生成器对象的方法

下面描述的方法可以用于控制生成器函数的执行

|                      方法                       | 描述                                               | 备注                                                        |
| :-------------------------------------------: | :----------------------------------------------- | :-------------------------------------------------------- |
|            `generator.__next__()`             | **开始一个生成器函数的执行** 或是 **从上次执行 `yield` 表达式的位置恢复执行** |                                                           |
|            `generator.send(value)`            | **恢复执行并向生成器函数“发送”一个值**                           | `value` 参数将成为当前 `yield` 表达式的结果                            |
|           `generator.throw(value)`            | 在生成器暂停的位置引发一个异常，并返回该生成器函数所产生的下一个值                | 如果生成器没有产生下一个值就退出，则将引发 `StopIteration` 异常                  |
| `generator.throw(type[, value[, traceback]])` |                                                  | 在 3.12 版本已被弃用并可能在未来的 Python 版本中移除。                        |
|              `generator.close()`              | 在生成器函数暂停的位置引发 `GeneratorExit`                    | 如果之后生成器函数正常退出、关闭或引发 `GeneratorExit` (由于未捕获该异常) 则关闭并返回其调用者 |

`generator.send(value)` 函数的补充
- `send() `方法会 **返回生成器所产生的下一个值**， 或者如果生成器没有产生下一个值就退出则会 **引发 `StopIteration`**
- 当调用 `send()` 来启动生成器时，它必须以 `None` 作为调用参数，因为这时没有可以接收值的 `yield` 表达式

```python
def counter():
    for i in range(10):
        x = yield i
        print(x, "+++++")

c = counter()  # 返回一个生成器
c.send(1)  # TypeError: 不能发生 non-None 值给刚开始的生成器对象
c.send(None)  # send (None) 等价于 next(c)，启动生成器
```


`generator.close()` 函数补充
- 如果生成器产生了一个值，关闭会引发 `RuntimeError`
- 如果生成器引发任何其他异常，它会被传播给调用者
- 如果生成器已经由于异常或正常退出则 `close()` 不会做任何事

#### 生成器重置

```python
def counter():
    def inc():
        i = 0
        while True:
            recv = yield i  # 接收发送进来的值，如果不是 None 就将 i 重置为 收到的值
            # 生成器重置
            if recv is not None and isinstance(recv, int):
                i = recv
            i += 1
            
    c = inc()
    return lambda reset=False, start=0: c.send(None) if reset else next(c)

c = counter()  # 返回一个函数
print(c())  # 0
print(c())  # 1
print(c())  # 2
print(c(reste=True, 10))  # 10
print(c())  # 11
print(c())  # 12
```

#### 协程

协程时生成器的高级用法。它比进程、线程轻量级，是在用户空间调度函数的一种实现

Python3 asyncio就是协程实现，已经加入到标准库。从 Python3.5 使用async、 await关键字直接原生支持协程

协程调度器实现思路
- 有2个生成器 `A`、`B`
- `next(A)` 后，`A` 执行到了 `yield` 语句暂停，然后去执行 `next(B)`，`B` 执行到 `yield` 语句也暂停，然后再次调用 `next(A)`，再调用 `next(B)` 周而复始，就实现了调度的效果

可以引入调度的策略来实现切换的方式

### yield from

从一个可迭代对象中 `yield` 每个一个元素

```python
def foo():
	yield from range(10)

g = foo()
next(g)  # 0
next(g)  # 1
```

上述 `foo` 定义等价如下定义

```python
def foo():
    for i in range(10):
        yield i
```

## 高阶函数

在 Python 中 函数是 **一等公民**。函数也是对象，是可调用对象
- 可以当作普通变量，也可以作为函数的返回值或参数

**高阶函数** 应该满足下面的条件
- 接收一个或多个 **函数作为参数**
- **返回一个函数**


分析下面的代码

```python hl:3
def counter(base):
	def inc(step=1):
		base += step  # 会在 inc 的局部名字空间中创建了 base 名字，base 并没有绑定对象，就对其进行了引用。这句会触发 UnboundLocalError 异常
		return base
	return inc
```

> [!question] 是否是高阶函数？是否有闭包？代码是否真确？
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

![[Pasted image 20250113170224.png]]

### 高阶函数的应用：`sorted` 

**仿照内建函数 `sorted`**，请自行实现一个 `sort` 函数(不用使用内建函数)，能够为列表元素排序
- 返回一个列表，使用 `key` 传递比较规则，`reverse` 控制降序还是升序

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

> [!tip] `result` 列表肯定是有序的，因此在查找插入点可以使用二分查找

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

