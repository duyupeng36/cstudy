# 反射

**反射** 是指程序在 **运行时** 获取 **对象类型和值的信息**。这里的 **对象** 是一个表示 **数据存储区域**。

> [!tip] 
> 
> 我们使用高级语言编写的程序在执行之前会经历一个称为 **翻译** 的阶段(编译或解释)，通常称为 **编译时**。此时，编译器或解释器会将程序中的变量转变为 **内存地址**；而变量名不会被编译器写入到可执行部分。在运行程序时，程序无法获取自身的信息。
> 

支持反射的语言可以在程序编译期间将变量的 **反射信息**，如属性名称、类型信息等整合等记录下来，并给程序提供接口访问反射信息，这样就可以在程序运行期间获取类型的反射信息，并且有能力修改它们。

因为 Python 是解释型脚本语言，对象的反射信息被天然的记录在脚本中。因此，Python 是天然支持反射的并且反射接口非常简单

## 反射获取类型信息

Python提供了几个内置函数用于获取对象的类型信息，如下表

| 函数                       | 描述                             |
| :----------------------- | :----------------------------- |
| `type(obj)`              | 返回对象 `obj` 所属的类对象              |
| `isinstance(obj, cls)`   | 检查对象 `obj` 是否是类对象 `cls` 的实例    |
| `issubclass(cls1, cls2)` | 检查类对象 `cls1` 是否是类对象 `cls2` 的子类 |
| `callable(obj)`          | 检查对象 `obj` 是否是可调用对象            |

这些函数在之前的学习中我们已经介绍过了，这里不在重复介绍

## 反射获取对象属性

在介绍通过反射获取对象的属性之前，我们复习 Python 中对象的属性访问

```python
class Point:
    def __init__(self, x, y):
        self.x = x
        self.y = y


if __name__ == '__main__':

    p = Point(1, 2)

    # 通过 实例.属性名 的方式访问属性
    print(p.x, p.y)

    # 通过 实例.属性名 = 值 的方式修改属性值
    p.x = 20
    p.y = 10
    print(p.x, p.y)

    # 实例中的属性保存在其 __dict__ 属性上的
    print(p.__dict__)
    p.__dict__["x"] = 22
    p.__dict__["y"] = 23
    print(p.x, p.y)
```

通过 `__dict__` 的方式访问属性提供了 **使用属性名的 _字符串_**，而非标识符的方式访问属性。本质上就是利用反射的能力。但是，这些属性代码看起来很奇怪。Python 提供了几个内置函数用于操作对象的属性

| 函数                              | 描述                       |
| :------------------------------ | :----------------------- |
| `getattr(obj, name[, default])` | 获取对象 `obj` 的属性 `name`    |
| `setattr(obj, name, value)`     | 给对象 `obj` 设置属性 `name`    |
| `hasattr(obj, name)`            | 检查对象 `obj` 是否存在属性 `name` |
| `delattr(obj, name)`            | 删除对象 `obj` 的属性 `name`    |

> [!attention] 
> 
> 请注意，上述函数中参数 `name` 都必须是字符串
> 
> 对于 `getatter()` 函数，如果对象 `obj` 中没有属性 `name`，则会返回 `default` 指定的值。如果 `default` 没有指定值，则会抛出 `AttributeError`
> 

### getattr()

内置函数 `getattr(obj, name[, default])` 用于从对象 `obj` 中获取属性 `name`。如果 `name` 不存在，则返回 `default` 指定的值。如果未指定 `default` 参数，则抛出 `AttributeError`

```python
class Point:
    def __init__(self, x, y):
        self.x = x
        self.y = y


if __name__ == '__main__':

    p = Point(1, 2)

    print(getattr(p, 'x'), getattr(p, 'y'))  # 获取 p 的属性 x 的值

    print(getattr(p, "__dict__"))
    # print(getattr(p, "z"))  # 没有属性 z 并且没有指定 default 的值，抛出 AttributeError
    print(getattr(p, "z", 100))  # 没有属性 z，返回 default 的值
```

### setattr()

内置函数 `setattr(obj, name, value)` 用于设置对象 `obj` 的属性 `name`。如果对象存在属性 `name`，则会修改属性 `name` 的值；否则就添加属性 `name`

```python
class Point:
    def __init__(self, x, y):
        self.x = x
        self.y = y


if __name__ == '__main__':

    p = Point(1, 2)

    print(p.__dict__)    # 输出: {'x': 1, 'y': 2}
    setattr(p, 'x', 20)  # 修改属性的值
    print(p.__dict__)    # 输出: {'x': 20, 'y': 2}
    setattr(p, 'z', 30)  # 添加属性
    print(p.__dict__)    # 输出: {'x': 20, 'y': 2, 'z': 30}
```

> [!attention] 
> 
> 通过反射 **给 _实例_ 增加的 函数属性**，这个属性 **不是绑定实例的方法**，无法自动传递 `self`，在调用时需要自行传递
> 
> 然而，通过反射 **给 _类_ 增加的函数属性**，该函数属性会绑定给该类的实例。也就是说，**任何时候给类添加的函数属性都是该类实例的方法**
> 

### hasattr()

内置函数 `hasattr(obj, name)` 用于检测对象 `obj` 是否存在属性 `name`

```python
class Point:
    def __init__(self, x, y):
        self.x = x
        self.y = y


if __name__ == '__main__':

    p = Point(1, 2)

    print(hasattr(p, 'x'), hasattr(p, 'y'), hasattr(p, 'z'))  
    # 输出：True True False
```

### delattr()

内置函数 `delattr(obj, name)` 用于删除对象 `obj` 中的属性 `name`。如果属性 `name` 不存在，则会抛出 `AttributeError`

```python
class Point:
    def __init__(self, x, y):
        self.x = x
        self.y = y


if __name__ == '__main__':

    p = Point(1, 2)

    print(p.__dict__)
    delattr(p, 'x')
    print(p.__dict__)
    # delattr(p, 'z')  # 抛出 AttributeError
    print(p.__dict__)
```

> [!question]
> 
> 思考：**反射**，**Mixin** 和 **装饰器** 都可以用于添加属性，它们之间的区别是什么？
> 
> 它们都可以做到非侵入式的增加属性。但是 Mixin 和 装饰器都是在 **定义时** 完成的，而 **_反射是在运行时_ 完成的**。通过 反射 可以实现与用户交互的方式增加属性或者访问属性
> 

## 定制属性访问

在 Python 中，对象的属性方法都是通过调用下表列出的特殊方法进行的

| 特殊方法                           | 描述                           |
| :----------------------------- | :--------------------------- |
| `obj.__getattribute__(attr)`   | 获取对象 `obj` 的属性 `attr` 时调用的方法 |
| `obj.__getattr__(attr)`        | 获取对象 `obj` 的属性失败时调用该方法       |
| `obj.__setattr__(attr, value)` | 设置对象 `obj` 的属性 `attr` 时调用的方法 |
| `obj.__delattr__(attr)`        | 删除对象 `obj` 的属性 `attr` 时调用的方法 |

> [!tip] 
> 
> 这些方法除了 `__getattr__()` 之外，`object` 类都有默认实现，我们在定制属性访问时，通常都需要去调用 `object` 中的同名方法，从而避免无限递归
> 
> 注意：使用 `getattr()` `setattr()` `delattr()` 都会调用相应的特殊方法，请不要在这些方法中使用内置函数，否则会无限递归
> 

### `__getattribute__()` 

Python 在执行访问对象的属性的语句，Python 都会首先调用 `__getattribute__()` 方法。也就是说，在 Python 中对象的属性访问都会被 `__getattribute__()` 方法给拦截，这是实例属性访问的第一站。

>[!tip] 
>
>通过定义 `__getattribute__()` 方法就可以  _控制属性的访问顺序_
>

```python
class Point:
    def __init__(self, x, y):
        self.x = x
        self.y = y

    def show(self):
        return f"Point({self.x}, {self.y})"

    def __getattribute__(self, item):
        print("__getattribute__ called")
        return 123


if __name__ == '__main__':

    p1 = Point(1, 2)
    print(p1.show)  
    # 输出: "__getattribute__ called" 和 123。
    # 123 是 __getattribute__ 的返回值
    print(p1.abc)  # 没有属性，输出 123.
    print(getattr(p1, 'xyz'))  # 输出 123
    print(p1.__dict__)  # 输出 123
```

上述代码的执行结果如下

```
__getattribute__ called  # 被 p1.show 触发
123
__getattribute__ called  # 被 p1.abc 触发
123
__getattribute__ called  # 被 getattr(p1, "xyz") 除非
123
__getattribute__ called  # 被 p1.__dict__ 除非
123
```

执行结果说明了 `__getattribute__()` 方法拦截了实例对象 `p1` 的任何属性访问操作。

> [!warning] 
> 
> 请注意：通常，我们不会自定义 `__getattribute__()` 方法。因为，类实例属性的访问属性时被该方法控制的，如果我们自定义了该方法，可能会破坏 Python 的类实例属性访问顺序
> 

### `__getattr__()`

`__getattribute__()` 是属性访问的第一站，并且属性的查找顺序就是在这里被控制的。通过属性查找顺序没有找到属性，`__getattribute__()` 就会抛出 `AttributeError`。然而，Python 会拦截这个异常，并尝试调用 `__getattr__()` 方法，如果 `__getattr__()` 调用成功，这个异常就会被忽略；否则就继续抛出该错误

```python
class Point:
    def __init__(self, x, y):
        self.x = x
        self.y = y

    def show(self):
        return f"Point({self.x}, {self.y})"

    def __getattr__(self, item):
        print("__getattr__ called")
        return 123


if __name__ == '__main__':

    p1 = Point(1, 2)
    print(p1.show)

	# 没有该属性，调用 __getattr__() 方法。
    print(p1.abc)  

	# 输没有该属性，调用 __getattr__() 方法
    print(getattr(p1, 'xyz'))
    
    print(p1.__dict__)  # 输出: {'x': 1, 'y': 2}
```

> [!tip] 
> 
> Python 在实例属性查找失败时，会在抛出 `AttributeError` 之前尝试调用 `__getattr__()` 方法。只要实例所属类定义 `__getattr__()` 方法，那么异常 `AttributeError` 就会被抑制
> 

到此，我们就可以总结一下 Python 中的属性查找顺序了

> [!summary] 
> 
> 当执行属性查找语句 `obj.attr` 时，Python 首先在 `obj.__dict__` 中查找属性 `attr`；如果没有找到，就会在 `type(obj).__dict__` 中进行查找，如果 `type(obj).__dict__` 中还是没查询到，就会按照类的继承顺序进行属性查找(注意，这是按照 [[Python：继承#C3 算法]] 获得继承顺序)
> 
> 按照上述顺序查找属性失败就会抛出 `AttributeError`，不过在正式抛出 `AttributeError` 之前，Python 会尝试调用 `__getattr__()`，只要 `__getattr__()` 调用成功，则属性查找成功；否则继续抛出错误
> 

### `__setattr__()`

特殊方法 `obj.__setattr__()` 会在设置属性的时候被调用。如果属性存在，则修改属性的值；否则，就添加新属性

```python
class Point:
    def __init__(self, x, y):
        self.x = x  # 这也会调用 __setattr__()
        self.y = y

    def show(self):
        return f"Point({self.x}, {self.y})"

    def __getattr__(self, item):
        print("__getattr__ called")
        return 123

    def __setattr__(self, key, value):
        print("__setattr__ called")
        self.__dict__[key] = value



if __name__ == '__main__':

    p1 = Point(1, 2)

    setattr(p1, 'x', 3)
    print(p1.__dict__)
    setattr(p1, 'z', 4)
    print(p1.__dict__)
```

> [!attention] 
> 
> `__setattr__()` 本质上就是操作 `obj.__dict__`
> 

### `__delattr__()`

特殊方法 `obj.__delattr__(attr)` 在对象 `obj` 的属性被删除是调用。如果属性不存在，咋抛出 `AttributeError`

```python
class Point:
    def __init__(self, x, y):
        self.x = x
        self.y = y

    def show(self):
        return f"Point({self.x}, {self.y})"


    def __delattr__(self, item):
        print("__delattr__ called")
        if item in self.__dict__:
            del self.__dict__[item]  # 删除属性
        else:
            raise AttributeError(f"{self.__class__.__name__!r} object has no attribute {item!r} ")


if __name__ == '__main__':

    p1 = Point(1, 2)

    delattr(p1, 'z')
```
