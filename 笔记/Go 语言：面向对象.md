# 面向对象

面向对象编程（OOP）是如今多种编程语言所实现的一种编程范式，包括 Java、C++、Python 等。

虽然 Go 语言不是面向对象的程序设计语言，但是 Go 也实现了面向对象的三大特征：**封装**、**继承**、**多态**

## 封装

当我们使用面向对象编程的术语对一个问题进行建模时，我们会创建一系列抽象的定义，这些定义代表了系统中存在的各类对象

例如，如果我们要对一个学校进行建模，我们可能会建立许多用于代表 **教授** 的对象，所有教授通常都存在以下 **属性**
+ 教授们都有一个名字
+ 都有一门他们各自所教的课程

此外，所有教授都可以 **做一些特定的事情**
+ 他们可以为学生的论文打分
+ 他们可以在学年的开始向学生介绍自己

因此，教授可以成为系统中的 `Professor` **类**。`Professor` 类的定义包括了所有教授都拥有的数据（成员）与行为（方法）。对于 Go 语言而言，则是定义 `Professor` 结构体

```go
package proferssor

type Proferssor struct {
	name  string   // 教授的名字
	class string   // 课程名字
}

func (p *Proferssor) Grade(score int) {
	// 学生论文打分
}

func (p *Proferssor) IntroduceSelf() {
	// 向学生介绍自己
}
```

就其本身而言，**类并不做任何事情**，类只是一种用于 **创建具体对象的模板**。`Professor` 类可以创建一个具体的教授，我们称这样创建出来的具体教授为 `Professor` 类的 **实例**。由类创建实例的过程是由一个称为 **构造函数** 的特殊函数完成。开发人员将类所需要的值传入构造函数，构造函数即可根据传入的值初始化实例的内部状态。

>[!attention]  
>
> 在 Go 语言中，并没有构造函数这一概念，我们通常会在包中自定义一个以 `New` 开头的函数模拟其他语言中的构造函数
>

```go
package proferssor


func New(name, class string) *Proferssor {
	return &Proferssor{
		name: name,
		class: class,
	}
}
```

通常，让 `New()` 函数返回实例的指针，避免对象拷贝。上面我们描述了在 Go 语言中定义创建类的过程。为了能够隐藏实例的属性，Go 语言通过使用标识符的首字母是否为大小写实现

> [!important] 
> 
> 在 Go 语言中，最小封装单位是 **包**。在包内可以查看包的所有全局标识符。然而，在包外，只能查看导出的包全局标识符。
> 
> Go 语言使用标识符的首字母大写表示该表示符是导出的；而标识符的首字母小写表示该标识符只能在包内使用
> 

通过标识符首字母的大小写在一定程度上实现了 `public` 和 `private` 的访问控制

## 继承

Go语言没有提供继承的语法，实际上需要通过 **结构体的匿名嵌入**（即：组合）来实现类似效果

```go
package main

import "fmt"

type Animal struct {
	name string
	age  int
}

func (*Animal) run() {
	fmt.Println("Animal run~~~")
}

type Cat struct {
	Animal // 结构体的匿名嵌入
	color  string
}

func main() {
	cat := new(Cat)
	cat.run()
	cat.Animal.run()
}
```

通过结构体的匿名嵌入，结构体就拥有了被嵌入结构体的属性 `name`、`age`，和 `run` 方法

> [!attention] 
> 
> 注意：Go 的结构体匿名嵌入与继承是不同的
> + 继承表达的 `is-a` 的关系
> + Go 结构体嵌入是一种组合关系，称为 `has-a` 的关系

### 重写：override

通过匿名嵌入获得的方法，从实现角度看，就是从新定义了一个方法，并对匿名嵌入方法的一层包装

```go
type Animal struct {
	name string
	age  int
}

func (*Animal) run() {
	fmt.Println("Animal run~~~")
}

type Cat struct {
	Animal // 结构体的匿名嵌入
	color  string
}

func (c *Cat) run() {
	c.Animal.run()
}
```

显然，我们可以对嵌入的方法进行重写，即 **覆盖** `override`。注意不是重载 `overload`

```go
type Animal struct {
	name string
	age  int
}

func (*Animal) run() {
	fmt.Println("Animal run~~~")
}

type Cat struct {
	Animal // 结构体的匿名嵌入
	color  string
}

// 完全覆盖 Animal 的 run 方法
func (*Cat) run() {
	fmt.Println("Cat run+++")
}
```

为 `Cat` 增加一个 `run` 方法，这就是覆盖。特别注意 `cat.run()` 和  `cat.Animal.run()` 的区别

上例增加 `run` 方法是 **完全覆盖**，就是不依赖父结构体方法，重写功能。如果是依赖父结构体方法，那就要在子结构体方法中调用它。

```go
// 依赖 Animal 的 run 方法
func (*Cat) run() {
	c.Animal.run()
	fmt.Println("Cat run+++")
}
```

## 多态

Go 通过引入 `interface` 类型来实现多态。一个类型实现了接口要求的所有方法，这该类型就是接口的实例

```go
package main

import "fmt"

type Runner interface {
	run()
}

// Animal 父结构体
type Animal struct {
	name string
	age  int
}

func (*Animal) run() {
	fmt.Println("Animal run~~~")
}

type Cat struct {
	Animal // 匿名结构体嵌入
	color  string
}

func (c *Cat) run() {
	c.Animal.run()
	fmt.Println("Cat run+++")
}

type Dog struct {
	Animal // 匿名结构体嵌入
	color  string
}

func (d *Dog) run() {
	d.Animal.run()
	fmt.Println("Dog run+++")
}

func test(a Runner) { // 多态
	a.run()
}

func main() {
	d := new(Dog)
	d.name = "snoopy"
	test(d)
	c := new(Cat)
	c.name = "Garfield"
	test(c)
}
```

`test` 使用同一个类型的同一个接口却表现不同，这就是多态

