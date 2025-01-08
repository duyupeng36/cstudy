# 作业47：[[Python：基础语法]]


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

