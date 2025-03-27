# 作业47：[[Python：基本概念]]

## 绘制赋值时的内存变化

绘制下面代码执行后变量的内存图

```python
bridegroom_name = "武大郎"
bride_name = "潘金莲"

temp = bridegroom_name
bridegroom_name = bride_name
bride_name = temp

print("交换后的新郎：" + bridegroom_name)  # "潘金莲"
print("交换后的新娘：" + bride_name)  # "武大郎"
```

![[Pasted image 20250108233747.png]]

## 预测孩子的身高

科学研究表明，孩子的身高取决于父母的身高。估计公式如下

$$
孩子身高 = (父亲身高+母亲身高) \times 0.54
$$

```python
# 估计孩子升高
father_height = float(input("输入父亲的身高: "))
mathor_height = float(input("输入母亲的身高："))

child_height = (father_height + mathor_height) * 0.54
print("父亲身高为:" + str(father_height) + " 母亲升高为:" + str(mather_height) + " 他们的孩子身高预计为：" + str(child_height))
```

## 温度转换

生活中，最常用的温度单位摄氏度($\degree C$) 和 华氏度($\degree F$)。它们之间的换算关系如下

$$
摄氏度 = (华氏度 - 32) \div 1.8
$$

```python
# 摄氏度转华氏度
celsius = float(input("输入一个摄氏温度："))
fahrenheit = celsius * 1.8 + 32

print("摄氏度: " + str(celsius) + " 等于 华氏度：" + str(fahrenheit))
```

## 计算个人社保缴纳费用
 
社保缴费根据工资按比例教你

$$
养老保险8\% + 医疗保险2\% + 3元 + 失业保险0.2\% + 公积金12\%
$$

```python
salary = float(input("输入月薪："))
pay = salary * (0.08 + 0.02 + 0.002 + 0.12) + 3
print("月薪：" + str(salary) + " 缴费: " + str(pay))
```

## 给一个半径，求圆的面积和周长

```python
import math

radius = float(input("请输入圆的半径："))

if radius < 0:
	print("半径必须是一个非负数")
else:
	circumference = 2 * math.pi * radius
	area = math.pi * radius ** 2
	print(f"半径为 {radius} 的圆的周长为 {circumference} 面积为 {area}")
```


## 判断人生处于什么阶段

| 年龄      | ⼈⽣阶段 |
| :------ | ---- |
| 0-6 岁   | 童年   |
| 7-17 岁  | 少年   |
| 18-40 岁 | ⻘年   |
| 41-65 岁 | 中年   |
| 65 岁之后  | ⽼年   |

```python
age = int(input("输入你的年龄："))

if age > 65:
	print("老年")
elif age > 41:
	print("中年")
elif age > 18:
	print("青年")
elif age > 7:
	print("少年")
else:
	print("中年")
```

## 回弹的小球

一个小球从 $100$ m高度落下，每次弹回原高度一半。计算:
+ 总共弹起多少次?(最小弹起高度0.01m)
+ 全过程总共移动多少米?

```python
height = 100  # 高度
count = 0  # 弹起次数
total = 0  # 移动路程

total += height  # 首先下落 100 m

while height / 2 >= 0.01:
	# 弹起并下落
	count += 1

	# 本次弹起高度
	height /= 2 
	
	# 本次移动路程
	total += 2 * height 

print("总共弹起 " + str(count) + " 次; 全程移动了 " + str(total) + " 米")
```

## 输入若干整数，打印出最大值。如果输入为空，则退出程序

```python
maximum = float("-inf")  # 无穷小

while True:
	line = input("输入一个整数：")
	if not line:
		break
	maximum = max(maximum, int(line))
	print("当前的最大值为: ",maximum)
```

## 判断一个正整数是否是素数

> [!tip] 
> 
> 素数只有 $1$ 和本身两个因数，换句话说就是，如果一个数只能被 $1$ 和它本身整除，那么这个数就是素数
> 

```python
# 判断一个正整数是否是素数

number = int(input("Please enter a positive integer: "))

for i in range(2, number):
    if number % i == 0:
        break
else:
    print(f"{number} is a prime number.")
```

## 寻找水仙花数

> [!tip] 
> 水仙花数也被称为超完全数字不变数、自恋数、自幂数、阿姆斯特朗数，它是一个 $3$ 位数，该数字每个位上数字的立方之和正好等于它本身，例如：$1^3 + 5^3+ 3^3=153$。

```python
# 寻找水仙花数

for number in range(100, 1000):
    highest = number // 100  # 最高位
    middle = number // 10 % 10 # 中间位
    lowest = number % 10  # 最低位

    if highest ** 3 + middle ** 3 + lowest ** 3 == number:
        print(f"{number} is a daffodil number.")
```

## 整数反转

> [!tip] 
> 
> 将 $12345$ 变成 $54321$
> 

```python
number = int(input("Please enter a integer: "))

sign = -1 if number < 0 else 1

number = abs(number)

reversed_number = 0

while number > 0:
    reversed_number = reversed_number * 10 + number % 10
    number //= 10

reversed_number = reversed_number * sign

print(f"{number} reverse {reversed_number}.")
```

## 打印九九乘法表

算法一：按行拼接

```python
for i in range(1, 10):
	line = ""
	for j in range(1, i+1):
		line += f"{j} * {i} = {j * i:<{3 if j == 1 else 4 }}"
	print(line)
```

算法二：连续打印

```python
for i in range(1, 10):
	for j in range(1, i+1):
		print(f"{j} * {i} = {j * i:<{3 if j == 1 else 4}}", end="\n" if j == i else "")
###############################################

for i in range(1, 10):  
    for j in range(1, i+1):  
       print(f"{j} * {i} = {j * i}", end= "\n" if j == i else "   " if j == 2 and i <= 4 else "  ")
```

## 求 10 万内素数的个数

### 算法一：穷举因数

针对一个数 `c`，遍历 `2` 至 `c-1` 判断是否有数 `i` 整除 `c`

```python
count = 0

# 从 2 开始遍历到 100000
for number in range(2, 100000):
    # 判断 number 是否是素数
    for i in range(2, number):
        if number % i == 0:  # 检查 2 到 number-1 中是否由数能够整数 number
            break  # 有一个数能整除 number，则 number 不是素数
    else:
        # for 的 else 子句。当 for 没有被 break 结束，才会执行该子句
        count += 1
```

### 算法二：半穷举因数

针对一个数 `c`，`a * b = c, a < b`。如果 `a | c` 则 `b | c`。显然，`c` 关于 `sqrt(c)` 乘法对称。因此，只需要遍历 `2` 至 `sqrt(c)` 判断是否有数 `i` 整除 `c`

```python
import math
count = 0

# 从 2 开始遍历到 100000
for number in range(2, 100000):
    # 判断 number 是否是素数
    for i in range(2, int(math.sqrt(number)) + 1):
        if number % i == 0:  # 检查 2 到 sqrt(number) + 1 中是否由数能够整数 number
            break  # 有一个数能整除 number，则 number 不是素数
    else:
        # for 的 else 子句。当 for 没有被 break 结束，才会执行该子句
        count += 1
```

### 算法三：只遍历奇数

除 $2$ 外的素数都是奇数并且一个奇数的因数也只能是奇数。因此，我们只需要遍历奇数即可

```python
import math
count = 1

# 从 3 开始遍历到 100000 中的所有奇数
for number in range(3, 1000000, 2):
    # 判断 number 是否是素数:奇数的因数也只能是奇数
    for i in range(3, int(math.sqrt(number)) + 1, 2):
        if number % i == 0:  # 检查 2 到 sqrt(number) + 1 中的奇数是否有数能够整数 number
            break  # 有一个数能整除 number，则 number 不是素数
    else:
        # for 的 else 子句。当 for 没有被 break 结束，才会执行该子句
        count += 1
```
