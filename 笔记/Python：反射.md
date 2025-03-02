# 反射

所谓的 **反射** 就是在 **_运行时_ 获取** 正在使用的对象的 _类型_ 和 _值_ 的信息。简单来讲，反射就像照镜子一样，可以自己观察到自己的信息

在 Python 中，反射是能够通过一个对象，找出其 **类型** 和 **属性** 的能力。下表列出了 Python 提供了几个内置函数用于获取对象的类型

| 函数                              | 描述                                   |
| :------------------------------ | ------------------------------------ |
| `type(obj)`                     | 获取 `obj` 所属的类                        |
| `isinstance(obj, cls)`          | 判断 `obj` 是否是类 `cls` 的实例对象            |
| `issubclass(cls1, cls2)`        | 判断 `cls1` 是否为 `cls2` 的子类             |
| `callable(obj)`                 | 判断 `obj` 是否为可调用对象                    |
|                                 |                                      |
| `getattr(obj, name[, default])` | 获取 `obj` 中的 `name` 属性。没有返回 `default` |
| `setatter(obj, name, value)`    | 给 `obj` 添加属性 `name`，其值为 `defalut`    |
| `hasatter(obj, name)`           | 判断 `obj` 是否存在 `name` 属性              |
| `delattr(obj, name)`            | 删除 `obj` 中的 `name` 属性                |

> [!attention] 
> 
> 请注意：上述函数中 `name` 参数必须为字符串
> 

```python
class Point:
    def __init__(self, x, y):
        self.x = x
        self.y = y


if __name__ == '__main__':

    p = Point(1, 2)

    # 通过 实例.属性名 的方式访问属性
    # print(p.x, p.y)

    # 通过反射的方式获取属性值
    print(getattr(p, 'x'), getattr(p, 'y'))  # 获取 p 的属性 x 的值

    # 通过 实例.属性名 = 值 的方式修改属性值
    # p.x = 20
    # p.y = 10

    setattr(p, 'x', 30)
    setattr(p, 'y', 20)
    print(p.x, p.y)

    getattr(p, "__dict__")["x"] = 100
    getattr(p, "__dict__")["y"] = 200

    print(getattr(p, "__dict__")["x"], getattr(p, "__dict__")["y"])
    print(p.x, p.y)
```

> [!tip]
> 
> 通过反射 **给 _实例_ 增加的 函数属性**，这个属性 **不是绑定实例的方法**，无法自动传递 `self`，在调用时需要自行传递
> 
> 通过反射 **给 _类_ 增加的函数属性**，通过实例访问属性时，就会自动 **绑定实例**
> 

> [!hint] 思考：反射，Mixin 和装饰器都可以增加属性，它们之间的区别？
> 
> 它们都可以做到非侵入式的增加属性。但是 Mixin 和 装饰器都是在定义时完成的，而 **_反射是在运行时_ 完成的** 
> 
> 通过 反射 可以实现与用户交互的方式增加属性或者访问属性
> 

## 反射相关的魔术方法

`getattr()` `setattr()` 和 `delattr()` 这三个反射对象属性的函数可以通过魔术方法定义其行为。

| 魔术方法                            | 描述                  |
| :------------------------------ | :------------------ |
| `__getattribute__(self, item)`  | 获取对象的属性 `item` 时的行为 |
| `__getattr__(self, item)`       | 定义获取属性出错时的行为        |
| `__setattr__(self, key, value)` | 定义设置属性时的行为          |
| `__delattr__(self, name)`       | 定义删除属性时的行为          |

在 **实例** 和 **实例的类** 以及其 **父类** 中都 **找不到属性** 时，在触发 `AttributeError` 异常之前检查是实例是否有 `__getattr__(self, item)` 方法，如果有则调用该方法，否则抛出 `AttributeError`

```python
class Point:
    def __init__(self, x, y):
        self.x = x
        self.y = y

    def __repr__(self):
        return f"<Point({self.x}, {self.y})>"

    def __getattr__(self, item):
        print("__getattr__：", item)
        return 100



if __name__ == '__main__':

    p1 = Point(1, 2)

    print(p1.x, p1.y)  
    # 不会触发 __getattr__，而是触发 __getattribute__() 方法
    
    print(p1.item) 
    # p1 中没有 item 属性，转而调用 __getattr__() 方法，
    # 从而抑制 AttributeError
```

> [!tip] `__getattr__()` 的触发时机时在 **属性访问出错时**
> 
> 实例的属性查找顺序
> 
> `obj.__dict__` => `type(obj).__dict__` => `type(obj).mro()[0,...,n].__dict__` => 没找到 `obj.__getattr__()`
> 
> 最终找不到属性，才会触发 `__getattr__()`
> 

---

`__setattr__()` 方法，实例的 **任何设置值** 操作都会调用 `__setattr__()` 方法进行属性设置

```python
class Point:
    n = 66
    def __init__(self, x, y):
        self.x = x  # 触发 __setattr__，执行 self.__dict__['x'] = x
        self.y = y  # 触发 __setattr__，执行 self.__dict__['y'] = y

    def __repr__(self):
        return f"<Point({self.x}, {self.y})>"

    def __setattr__(self, key, value):  # 覆盖了 object.__setattr__() 方法
        print("__setattr__ key={}, value={}".format(key, value))


if __name__ == '__main__':

    p1 = Point(1, 2)
    print(p1.n)  # 类属性
    print(p1.x)  # 实例属性，由于在 __setattr__ 中没有设置属性，因此此时无法找到属性 x，并且没有提供 __getattr__ 方法，所以触发 AttributeError
    print(p1.y)  # 实例属性，同上
```

**设置属性** 就是在实例的 `__dict__` 中添加 `key:value` 对，因此 `__setattr__` 可以如下

```python
class Point:
    n = 66
    def __init__(self, x, y):
        self.x = x
        self.y = y

    def __repr__(self):
        return f"<Point({self.x}, {self.y})>"

    def __setattr__(self, key, value):  # 覆盖了 object.__setattr__() 方法
        print("__setattr__ key={}, value={}".format(key, value))
        self.__dict__[key] = value


if __name__ == '__main__':

    p1 = Point(1, 2)
    p1.a = 'aaa'
    print(p1.n)  # 类属性
    print(p1.x)  # 实例属性
    print(p1.y)  # 实例属性
    print(p1.a)
```

> [!attention] 
> 
> `__setattr__()` 方法，可以拦截实例属性的 **增加** 和 **修改** 操作，通常该方法是操作 `__dict__`
> 

---

通过 `del obj.name` 方式或者 `delattr(obj, name)` 方式删除属性时，都会调用 `__delattr__()`


> [!tip] 
> 
> 如何删除实例属性的操作都会调用 `__delattr__()` 方法
> 

---

`__getattribute__()` 在 **实例的任何属性访问** 时触发，它可以 _拦截实例的属性访问_，在这里我们可以 _控制实例属性的访问顺序_

```python
class Point:
    n = 66
    def __init__(self, x, y):
        self.x = x
        self.y = y

    def __repr__(self):
        return f"<Point({self.x}, {self.y})>"

    def __getattr__(self, item):
        print("__getattr__ {}".format(item))

    def __getattribute__(self, item): # 这是实例属性访问的第一站。任何实例的属性访问都会被该方法拦截
        print("__getattribute__ {}".format(item))
        return 1111

if __name__ == '__main__':

    p1 = Point(1, 2)
    p1.a = 'aaa'
    print(p1.n)  # 1111
    print(p1.x)  # 1111
    print(p1.y)  # 1111
    print(p1.a)  # 1111
```

如果在 `__getattribute__()` 中主动抛出 `AttributeError` 异常，Python 解释器会主动帮我们调用 `__getattr__()` 方法

```python
class Point:
    n = 66
    def __init__(self, x, y):
        self.x = x
        self.y = y

    def __repr__(self):
        return f"<Point({self.x}, {self.y})>"

    def __getattr__(self, item):
        print("__getattr__ {}".format(item))
        return 99999

    def __getattribute__(self, item): 
	    # 这是实例属性访问的第一站。任何实例的属性访问都会被该方法拦截
        print("__getattribute__ {}".format(item))
        raise AttributeError  
        # 抛出 AttributeError 从而调用触发 __getattr__
        # (前提是 __getattr__ 方法存在)

if __name__ == '__main__':

    p1 = Point(1, 2)
    p1.a = 'aaa'
    print(p1.n)  # 99999
    print(p1.x)  # 99999
    print(p1.y)  # 99999
    print(p1.a)  # 99999
```
