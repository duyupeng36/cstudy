# 可调用对象

在 [[Python：函数高级]] 中我们就已经说过，函数也是一个对象，称为 **可调用对象**。Python 提供了 `__call__(self, *args, **kwargs)` 方法用于模拟函数的行为

> [!tip] 
> 
> 可调用对象：如果一个实例所属类中定义了 `__call__(self, *arg, **kwargs)` 方法，那么该类的实例对象称为可调用对象
> 

除了函数外，Python 中的可调用对象还有：**方法**、 **类**、 **类实例**

> [!tip] 
> 
> 请注意，只有当类实例所属类对象中定义了 `__call__(self, *arg, **kwargs)` 时，该类实例对象才是可调用对象
> 

在 Python 中，可以使用内置函数 `callable()` 查看对象是否为可调用对象

```python
class A:
    pass


if __name__ == '__main__':
    print(callable(A))    # True：类是可调用对象
    print(callable(A()))  # False：类 A 中未定义 __call__()，所以 A 的实例不是可调用对象
```

为了让一个类的实例变为可调用对象，只需要将定义 `__call__` 方法即可

```python
class A:
    def __call__(self, *args, **kwargs):
        print('call')


if __name__ == '__main__':
    print(callable(A))    # True
    print(callable(A()))  # True
```

下面是可调用对象的一个应用：定义一个斐波拉契数列的类，方便调用，计算第 $n$ 项。增加迭代数列的方法，返回数列的长度，支持索引查找数列项的方法

```python

class Fib:

    def __init__(self):
        self.__sequence = [0, 1]

    def __call__(self, index):
        if index < 0:
            raise ValueError("Fib index must be greater than or equal to 0.")

        if index >= len(self.__sequence):
            for i in range(len(self.__sequence), index + 1):
                self.__sequence.append(self.__sequence[i-1] + self.__sequence[i-2])
            return self.__sequence[index]
        else:
            return self.__sequence[index]

    def __iter__(self):
        yield from self.__sequence[1:]

    def __repr__(self):
        return self.__sequence[1:].__repr__()


fib = Fib()

print(fib(4))

print(fib(5))

print(fib(6))

for i in fib:
    print(i)

print(fib)  # [1, 1, 2, 3, 5, 8]
```
