# 模拟可调用对象

Python 中的函数本质上是一个对象，通常称为 **可调用对象**。Python 为我们提供了一个特殊方法 `__call__()`，只要类定义了该方法，那么该类的实例就可以被当做函数调用。

> [!tip] 
> 
> 可以像函数一样被调用的对象称为可调用对象。在 Python 中，可调用对象有下面几类
> + 函数
> + 方法
> + 类
> + 实现了 `__call__()` 方法的类的实例
> 

```python
class A:  # 类对象

    def sum(self):
        pass

method = A().sum  # 方法对象


def foo():  # 函数
    pass


class B:

    def __call__(self):
        pass

b = B()  # 定义了 __call__() 方法 的实例对象


if __name__ == '__main__':
    print(callable(A))       # True
    print(callable(method))  # True
    print(callable(foo))     # True
    print(callable(b))       # True
```

在需要记录函数执行状态时，这个特性可以很简单的实现。例如，我的程序在运行的时候可能随时都需要斐波拉契数列的第 $n$ 项，如果每次都去计算就显得非常浪费了。我们可以记录下已经计算过的斐波拉契数列中的项，在需要的时候直接返回；如果记录中没有需要的项，则重新计算并记录下来

```python
class Fibonacci:

    def __init__(self):
        self.__results = [0, 1, 1]

    def __len__(self):
        return len(self.__results)

    def __getitem__(self, item):
        if not isinstance(item, int):
            raise TypeError("index must be a positive integer")

        if 0 <= item < len(self.__results):
            return self.__results[item]
        else:
            return self(item)

    def __iter__(self):
        yield from self.__results

    def __call__(self, index: int):
        """计算斐波拉契数列的第 n 项
        """

        if index <= 0:
            raise ValueError("n must be positive")

        if 0 < index <= 2:
            return 1

        if index < len(self):
            return self[index]

        for i in range(len(self), index + 1):
            self.__results.append(self[i - 1] + self[i - 2])

        return self.__results[index]

    def __repr__(self):
        return f"{self.__results}"



if __name__ == '__main__':

    fib = Fibonacci()
    print(fib(100))
    print(fib(200))
    print(fib(400))
    print(fib(1000))
    print(fib(1100))
    print(fib(1400))
    print(fib(1900))
    print(fib(2000))
    print(fib(2400))
    print(fib(2900))
```
