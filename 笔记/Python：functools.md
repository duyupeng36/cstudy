# functools 

在 [[Python：装饰器]] 中我们介绍了 `functools` 中的两个函数 `update_wrapper` 和 `wraps` 。下面我们看更多的函数

## `reduce` 归约

```python
functools.reduce(function, iterable[, initializer])
```

将两个参数的函数从左到右累加到 `iterable` 的项上，从而将 `iterable` 归约为单个值

例如,`reduce(lambda x, y: x + y, [1, 2, 3, 4, 5])` 计算 `((((1, 2), 3), 4), 5)`
- 左边的参数 `x` 是累加值，右边的参数 `y` 是迭代器更新后的值

如果存在可选初始化方法，它会在计算过程中放在可迭代对象的元素之前，并且在可迭代对象为空时充当默认值。如果没有给出 `initializer` 并且 `iterable` 只包含一个元素，则返回第一个元素

```python
>>> reduce(lambda x, y: x + y, [1, 2, 3, 4, 5])  # 累加
15
>>> reduce(lambda x, y: x * y, range(1,10))  # 阶乘
362880
```

## `partial` 偏函数

```python
functools.partial(func, /, *args, **keywords)
```

返回一个新的 部分对象，当被调用时其行为类似于 `func` 附带位置参数 `args` 和关键字参数 `keywords` 被调用。 如果为调用提供了更多的参数，它们会被附加到 `args`。 如果提供了额外的关键字参数，它们会扩展并重写 `keywords`

**把函数部分参数固定下来**，相当于为部分的参数添加了固定的默认值，形成一个新的函数，并 **返回这个新函数**。这个新函数是对原函数的封装

```python
>>> def add(x, y):
...     return x + y
...
>>> partial(add, x=10)
functools.partial(<function add at 0x00000231B437A160>, x=10)
>>> new_add = partial(add, x=10)  # 固定了 x 
>>> new_add(10)  # 固定了 x，这里还是会按位置传参传递给 x，两次获得 x 报错
Traceback (most recent call last):
  File "<stdin>", line 1, in <module>
TypeError: add() got multiple values for argument 'x'
>>> new_add(y=9) # 这里必须是关键字传参
19
>>> new_add(x=11, y=10)  # 关键传参
21
```

使用 `inspect` 查看`add` 和 `new_add` 的签名

```python
>>> inspect.signature(add)
<Signature (x, y)>
>>> inspect.signature(new_add) # 显然，参数 y 必须使用关键字传参了
<Signature (*, x=10, y)>
>>> new_add2 = partial(add, 10)
>>> inspect.signature(new_add2) # 现在，只能传递参数给 y 了
<Signature (y)>
```

```python
>>> def add(x, y, *args):
...     pass
...
>>> import functools
>>> import inspect
>>> functools.partial(add, y=2)
functools.partial(<function add at 0x00000211180B8A40>, y=2)

# y=2 以关键字传参的形式固定 add 的参数 a
# 因为 y 是关键字传参了，所以 *args 参数无法使用了
>>> newadd = functools.partial(add, y=2)  # 造成 args 参数丢失，只能传递 x 参数了
>>> inspect.signature(newadd)
<Signature (x, *, y=2)>

# 使用位置传参的形式固定了参数，由于 *args 是可变长的位置参数，可以添加其他元素 
>>> newadd = functools.partial(add, 1,2,3,4,5)  # 3 4 5 会传递给 args
>>> inspect.signature(newadd)
<Signature (*args)>
```

`partial` 的本质等价于如下闭包函数

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

新函数的等价分析

```python
>>> def add(x, y):
...     pass
...
>>> import functools
>>> newadd = functools.partial(add, x=10)
>>> newadd(10)  # 等价于 add(10, x=10)，两次获得 x 并且 缺少 y
Traceback (most recent call last):
  File "<stdin>", line 1, in <module>
TypeError: add() got multiple values for argument 'x'
>>> newadd = functools.partial(add, 10)
>>> newadd(11)  # 等价于 add(10, 11)  ok
```

## 缓存

回想我们写的 [[Python：装饰器#`run_time` 装饰器|run_time]] 我们使用它统计了斐波拉契数列的计算的时长，它非常耗时。我们在 [[Python：函数高级#斐波拉契数列]] 的递归实现中说了需要缓存计算出来的结果。在之前的实现中，我们让 `fib` 函数自行记录。现在，我们为它提供一个装饰器

```python
def make_key(args, kwargs, typed=True):
    """构建key
    """
    key = args
    if kwargs:
        for item in kwargs.items():
            key += item
    if typed:
        key += tuple(type(item).__name__ for item in args)
        if kwargs:
            for value in kwargs.values():
                key += tuple((type(value).__name__,))
    return "-".join(map(str, key))


def cache(func):
    results = {}
    @functools.wraps(func)
    def wrapper(*args, **kwargs):
        # 将参数转换为字符串
        key = make_key(args, kwargs)
        if key in results:
            return results[key]
        results[key] = func(*args, **kwargs)
        return results[key]
    return wrapper
```

这样，`fib` 在计算过程中，就可以记录已经计算的结果。查询到结果就不会递归的调用 `fib` 函数了。调用 `fib30` 的输出结果为

```
fib(((2,), {})) run 0.0 seconds
fib(((1,), {})) run 0.0 seconds
fib(((3,), {})) run 0.0 seconds
fib(((4,), {})) run 0.0 seconds
fib(((5,), {})) run 0.0 seconds
fib(((6,), {})) run 0.0 seconds
fib(((7,), {})) run 0.0 seconds
fib(((8,), {})) run 0.0 seconds
fib(((9,), {})) run 0.0 seconds
fib(((10,), {})) run 0.0 seconds
fib(((11,), {})) run 0.0 seconds
fib(((12,), {})) run 0.0 seconds
fib(((13,), {})) run 0.0 seconds
fib(((14,), {})) run 0.0 seconds
fib(((15,), {})) run 0.0 seconds
fib(((16,), {})) run 0.0 seconds
fib(((17,), {})) run 0.0 seconds
fib(((18,), {})) run 0.0 seconds
fib(((19,), {})) run 0.0 seconds
fib(((20,), {})) run 0.0 seconds
fib(((21,), {})) run 0.0 seconds
fib(((22,), {})) run 0.0 seconds
fib(((23,), {})) run 0.0 seconds
fib(((24,), {})) run 0.0 seconds
fib(((25,), {})) run 0.0 seconds
fib(((26,), {})) run 0.0 seconds
fib(((27,), {})) run 0.0 seconds
fib(((28,), {})) run 0.0 seconds
fib(((29,), {})) run 0.0 seconds
fib(((30,), {})) run 0.0 seconds
```

添加缓存之后，对于相同输入缓存命中，直接返回缓存的结果。这就是 **使用大量内存空间换取较少的计算时间**

### `lru_cach` 最近最少使用缓存装饰器

我们已经介绍了缓存的实现方案。下面我没看 `functools` 中提供的 `LRU` 缓存装饰器

`LRU` 是 `Least Recently Used` 的缩写，即 **最近最少使用**

```python
@functools.lru_cache(user_function)

@functools.lru_cache(maxsize=128, typed=False)
```

一个 **为函数提供缓存功能** 的装饰器，缓存 `maxsize` 组传入参数，**在下次以相同参数调用时直接返回上一次的结果**。用以节约高开销或 `I/O` 函数的调用时间

**该缓存是线程安全的** 因此被包装的函数可在多线程中使用。 这意味着下层的数据结构将在并发更新期间保持一致性。

由于 **使用字典来缓存结果**，因此传给该函数的位置和关键字参数必须为 `hashable`

```python
def lru_cache(maxsize=128, typed=False):
    if isinstance(maxsize, int):
	    # 解决 maxsize 为负数的情形
        if maxsize < 0:
            maxsize = 0
    elif callable(maxsize) and isinstance(typed, bool):
	    # 将 lru_cache 作为无参装饰器使用
        user_function, maxsize = maxsize, 128  # 记录传入的函数，和默认的 maxsize
        # 构造一个 lru 缓存的包装器和缓存区域
        wrapper = _lru_cache_wrapper(user_function, maxsize, typed, _CacheInfo)
        # 添加缓存参数
        wrapper.cache_parameters = lambda : {'maxsize': maxsize, 'typed': typed}
        return update_wrapper(wrapper, user_function)  # 更新 wrapper 的属性
    elif maxsize is not None:
	    # maxsize 不是 int 也不是可调用的，或者不是 None 则报错
        raise TypeError(
            'Expected first argument to be an integer, a callable, or None')
	
	# 装饰函数
    def decorating_function(user_function):
		# 构造包装器和缓存区域
        wrapper = _lru_cache_wrapper(user_function, maxsize, typed, _CacheInfo)
        # 添加缓存参数
        wrapper.cache_parameters = lambda : {'maxsize': maxsize, 'typed': typed}
        return update_wrapper(wrapper, user_function)

    return decorating_function
```

> [!check] 显然，提供缓存的是 `_lru_cach_wrapper` 函数提供的缓存

```python
def _lru_cache_wrapper(user_function, maxsize, typed, _CacheInfo):
    # Constants shared by all lru cache instances:
    sentinel = object()          # unique object used to signal cache misses
    make_key = _make_key         # build a key from the function arguments
    PREV, NEXT, KEY, RESULT = 0, 1, 2, 3   # names for the link fields

    cache = {}  # 缓存
    hits = misses = 0  # 命中，命中失败次数
    full = False
    cache_get = cache.get    # bound method to lookup a key or return None
    cache_len = cache.__len__  # get cache size without calling len()
    lock = RLock()           # because linkedlist updates aren't threadsafe
    root = []                # root of the circular doubly linked list
    root[:] = [root, root, None, None]     # initialize by pointing to self

    if maxsize == 0:  # 没有缓缓存
        def wrapper(*args, **kwds):
            # 没有缓存——只有一个统计数据更新
            nonlocal misses
            misses += 1
            result = user_function(*args, **kwds)
            return result

    elif maxsize is None:
        def wrapper(*args, **kwds):
            # 简单的缓存，没有排序或大小限制
            nonlocal hits, misses
            key = make_key(args, kwds, typed)  # 构建缓存的键
            result = cache_get(key, sentinel)  # 命中失败返回 object()
            if result is not sentinel:
                hits += 1  # 命中次数
                return result
            misses += 1  # 命中失败次数
            result = user_function(*args, **kwds)
            cache[key] = result
            return result

    else:

        def wrapper(*args, **kwds): # 此处为 LRU 缓存
            # 大小有限的缓存，以最近的方式跟踪访问
            nonlocal root, hits, misses, full
            key = make_key(args, kwds, typed)  # 构建key
            with lock:  # 获取锁
                link = cache_get(key)  # 获取缓存
                if link is not None:  # 获取到了结果
	                # 更新
                    link_prev, link_next, _key, result = link
                    link_prev[NEXT] = link_next
                    link_next[PREV] = link_prev
                    last = root[PREV]
                    last[NEXT] = root[PREV] = link
                    link[PREV] = last
                    link[NEXT] = root
                    hits += 1
                    return result
                misses += 1
            result = user_function(*args, **kwds)
            with lock:
                if key in cache:
                    # Getting here means that this same key was added to the
                    # cache while the lock was released.  Since the link
                    # update is already done, we need only return the
                    # computed result and update the count of misses.
                    pass
                elif full:  # 缓存满了，移除最近最少未使用
                    # Use the old root to store the new key and result.
                    oldroot = root
                    oldroot[KEY] = key
                    oldroot[RESULT] = result
                    # Empty the oldest link and make it the new root.
                    # Keep a reference to the old key and old result to
                    # prevent their ref counts from going to zero during the
                    # update. That will prevent potentially arbitrary object
                    # clean-up code (i.e. __del__) from running while we're
                    # still adjusting the links.
                    root = oldroot[NEXT]
                    oldkey = root[KEY]
                    oldresult = root[RESULT]
                    root[KEY] = root[RESULT] = None
                    # Now update the cache dictionary.
                    del cache[oldkey]
                    # Save the potentially reentrant cache[key] assignment
                    # for last, after the root and links have been put in
                    # a consistent state.
                    cache[key] = oldroot
                else:
                    # Put result in a new link at the front of the queue.
                    last = root[PREV]
                    link = [last, root, key, result]
                    last[NEXT] = root[PREV] = cache[key] = link
                    # Use the cache_len bound method instead of the len() function
                    # which could potentially be wrapped in an lru_cache itself.
                    full = (cache_len() >= maxsize)
            return result

    def cache_info():
        """Report cache statistics"""
        with lock:
            return _CacheInfo(hits, misses, maxsize, cache_len())

    def cache_clear():
        """Clear the cache and cache statistics"""
        nonlocal hits, misses, full
        with lock:
            cache.clear()
            root[:] = [root, root, None, None]
            hits = misses = 0
            full = False

    wrapper.cache_info = cache_info
    wrapper.cache_clear = cache_clear
    return wrapper
```

在 `_lru_cache_wrapper` 中提供了 `cache` 和 `make_key`。`cache` 是一个 `dict` 类型用于缓存结果，`make_key` 用于构建访问 `dict` 的键

```python
def _make_key(args, kwds, typed,
             kwd_mark = (object(),),
             fasttypes = {int, str},
             tuple=tuple, type=type, len=len):
    key = args  # 元祖。例如 (4, 5)
    if kwds:
        key += kwd_mark  # 空对象
        for item in kwds.items():  # 例如 {"x": 4, "y": 5}
            key += item  # 将 (key, value) 拼接到 key 元祖上
    if typed:  # 如果 typed 为 True
        key += tuple(type(v) for v in args)  # 补上 args 中每个元素的类型
        if kwds: 
            key += tuple(type(v) for v in kwds.values()) # 补上 values 每个元素的类型
    elif len(key) == 1 and type(key[0]) in fasttypes:  # key 长度为 1 并且 它的类型是 int 或 str 直接返回
        return key[0]
    return _HashedSeq(key)  # 返回一个 _HashedSeq 对象
```

`_HashedSeq` 它继承了 `list`，并重写了 `__hash__` 方法。这样就 `_HashedSeq` 就是可哈希对象了。

```python
class _HashedSeq(list):
    __slots__ = 'hashvalue'

    def __init__(self, tup, hash=hash):
        self[:] = tup
        self.hashvalue = hash(tup)

    def __hash__(self):
        return self.hashvalue
```

> [!seealso] 在 [[Python：面向对象]] 中介绍

对于 `fib` 函数，我们只需要缓存三个值，就能做到快速计算
```python
import functools

@functools.lru_cache(maxsize=3)  # 缓存装饰器
def fib(n:int) -> int:
    if n <= 0:
        # 解决传递进来的参数小于等于 0 没有有定义
        raise ValueError("n must be greater than 0")
    if  n <= 2:
        return 1
    return fib(n-1) + fib(n-2)
```

下图分析了 `fib(5)` 的缓存过程

![[Pasted image 20250113170803.png]]

> [!important] 使用 cache 的前提
> - 数据不变：恒久不变 或 一段时间内不变。**幂等性**：给定一个输入值，一定可以得到不变的结果。
> - **计算代价高**：对于一个输入，得到结果需要很长的时间
> - **使用频率较高**：在一段时间内，使用频率较高

