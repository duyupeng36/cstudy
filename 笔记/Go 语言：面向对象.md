# 面向对象


面向对象三要素：

+ **封装** 即组装，将 _数据和操作组装在一起_。同时 **隐藏数据结构的具体实现**，只对外暴露一些接口，通过接口访问对象和对象的属性
> [!note] 封装
> 
> 不管你是土鳖还是土豪，不管你中午吃的是窝头还是鲍鱼，它们在你的肚子里。你不说给别人听，别就看不到你中午吃了什么！

+ **继承** 复用代码
> [!note] 你是土鳖，你们全家都是土鳖。这就是继承
> 冰冻三尺非一日之寒，你有今天，必定可以从你爸爸爷爷那里追根溯源
> 
> 正所谓虎父无犬子，正恩同学那么狠，他爹正日就不是什么善茬，更甭说他爷爷，明白了吗?

+ **多态** 相同父类的子类的方法表现行为不同
> [!note] 多态
> 哲学家说过，世上不会有两个一模一样的双胞胎。即使你从你父亲那里继承来的土鳖/豪气质，也不可能完全是从一个模子里刻出来的，总会有些差别
> - 比如你爸喜欢蹲在门前吃面，你喜欢骑在村口的歪脖子树上吃，或者反过来
> - 当然，也可能令尊爱吃龙虾鲍鱼时旁边有几个艺校小女生喝酒唱歌助兴，你可能更喜欢弄个街舞乐队来吹拉弹唱

实现了以上特征的语言，才能成为面向对象编程范式语言。

> [!important] Go 没有实现完整的面向对象范式
> 严格意义来说，Go 就是不想实现面向对象编程范式。但是面向对象又有一些不错的特性，Go 通过 **_组合_** 的方式实现了类似的功能。

只能说，Go 实现了一种非常有自我特征的面向对象

## 封装

Go 的最小封装单位是 **包**。通过自定义类型，可以把数据和操作数据的方法组装在一起。


为了实现访问控制：
+ 数据属性、方法标识符 **首字母大写**，实现了包外可见
+ 数据属性、方法标识符 **首字母小写**，仅包内可见

这些一定程度上实现了 `public`、`private` 的访问控制

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

> [!important] 注意：Go 的结构体匿名嵌入与继承是不同的
> + 继承表达的 `is-a` 的关系
> + Go 结构体嵌入是一种组合关系，称为 `has-a` 的关系

## 覆盖

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

显然，我们可以对嵌入的方法进行重写，即 **_覆盖_** `override`。注意不是重载 `overload`

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

上例增加 `run` 方法是 **完全覆盖**，就是不依赖父结构体方法，重写功能

如果是依赖父结构体方法，那就要在子结构体方法中调用它。

```go
// 依赖 Animal 的 run 方法
func (*Cat) run() {
	c.Animal.run()
	fmt.Println("Cat run+++")
}
```

## 多态

Go 通过引入 `interface` 接口来实现多态。一个类型实现了接口要求的所有方法，这该类型就是接口的实例

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
