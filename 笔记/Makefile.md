# Makefile

当源文件变得越来越多，每次都重新编译程序将花费很大的代价。为了减少编译的时间，可以使用 **增量编译** 策略

> [!tip] `make` 构建工具就是使用 **增量编译** 策略完成编译的
> 
> 也就是说 `make`**只编译 发生改动后的代码文件**, 而不是完全编译
>
 
`make` 构建工具采用 **"目标-依赖"** 这样的树形结构，分析那些目标需要重新编译

![[Pasted image 20241008232911.png]]

> [!tip] make 编译规则
> 
> 目标文件不存在时，会编译目标文件的依赖文件，从而生成目标文件
> 
> 依赖文件比目标文件新(根据依赖文件的修改时间晚于目标文件的修改时间)时，就会重新编译依赖文件，形成新的目标文件
> 

`make` 命令执行时，需要一个 `Makefile` 文件，以告诉 `make` 命令需要怎么样的去编译和链接程序。它是`make`读入的唯一配置文件

> [!attention] 注意 
> 
> `Makefile` 和 `makefile` 都是 `make` 接受的文件名。通常使用 `Makefile`
> 

## Makefile 介绍

`Makefile` 的内容是 **"目标-依赖" 树形结构** 的定义和生成目标的 **命令**，这种 **"目标-依赖" 和 命令 组成了规则**

> [!important] 
> 
> `Makefile` 由一些 **规则** 集合，这些规则定义了如何生成目标体
> 

Makefile 规则的语法如下

```shell
targets : prerequisites
	command
	...
```

> [!tip] `targets` 是文件名
> 
> `targets` 是文件名，以空格分开，可以使用通配符。一般来说，`targets` 基本上是一个文件，但也有可能是多个文件。
> 

> [!tip] `command` 是命令
> 
> `command` 是生成`targets` 需要执行的命令。如果其不与 `target:prerequisites` 在一行，那么，必须以 `Tab` 键开头
> 

> [!tip] `prerequisites` 是目标的依赖
> 
> `prerequisites` 也就是 **目标所依赖的文件或目标** 
> 
> 如果其中的某个文件要比目标文件要新，那么，目标就被认为是“过时的”，被认为是需要重生成的
> 

这是一个文件的依赖关系，也就是说，`target` 这一个或多个的目标文件依赖于 `prerequisites` 中的文件，其生成规则定义在 `command` 中

> [!tip] 增量编译策略
> 
> `prerequisites` 中如果有一个以上的文件比 `target` 文件要新的话，`command` 所定义的命令就会被执行。
> 

#### 简单示例

下面就是一个简单 Makefile 示例用于生成 `main.out`

```shell title:Makefile
main.out: main.c calc.o show.o
	gcc main.c calc.o show.o -o main.out

calc.o: calc.c calc.h
	gcc -c calc.c -o calc.o

show.o: show.c show.h
	gcc -c show.c -o show.o

.PHONY:clean
clean:
	rm *.o
```

### make 的工作方式

在默认的方式下，也就是我们只输入 `make` 命令

1.  `make` 会在当前目录下找名字叫 `"Makefile"` 或 `"makefile"` 的文件。
 2.  如果找到，它会找文件中的 **第一个目标**（`target`），例如，上例中的 `main.out`，并把这个文件作为最终的目标文件
 3. 如果 `main.out` 不存在，或者它依赖的文件的修改时间要比 `main.out` 更新，那么就会执行后面所定义的命令来生成 `main.out` 这个文件
 4. 如果 `main.out` 依赖的 `name.o` 文件不存在，就会找到 `name.o` 目标的规则并生成
 5. 当然，`.c` 文件和 `.h` 文件是存在的，于是 `make` 会生成 `.o` 文件，然后再用 `.o` 文件生成 `make` 的终极任务，也就是执行文件 `main.out` 

> [!tip] 
> 
> 这就是整个 `make` 的依赖性，`make` 会一层又一层地去找文件的依赖关系，直到最终编译出第一个目标文件
> 

### 变量

继续上面的例子，首先，我们看看 `main.out` 这个规则

```shell 
main.out: main.c calc.o show.o
	gcc main.c calc.o show.o -o main.out
```

> [!tip] 
> 
> `.o` 文件的字符串被重复写了两次。如果需要加入一个新的 `.o` 文件，这一条规则就需要修改两个地方
> 

如果 `Makefile` 变得复杂，那么我们就有可能会忘掉一个需要加入的地方，而导致编译失败。为了 `Makefile` 的易维护，在 `Makefile` 中可以使用变量

我们可以声明一个变量 `OBJECTS`(命名是任意的，只要能够表示是 `.o` 文件即可)。在 `Makefile` 的一开就这样定义

```shell
CC := /usr/bin/gcc  # 定义使用的编译器，后面可以直接更换

OBJECTS := calc.o show.o
```

> [!attention] 
> 
> Makefile 中的变量的值都是字符串
> 

于是，我们就可以很方便地在我们的 `Makefile` 中以 `$(OBJECTS)` 的方式来使用这个变量了。下面是修改后的 `Makefile` 

```shell title:Makefile
OUT := main.out              # 输出文件名
CC := /usr/bin/gcc        # 定义使用的编译器，后面可以直接更换
OBJECTS := calc.o show.o  # 目标文件

$(OUT): main.c $(OBJECTS)
	$(CC) main.c $(OBJECTS) -o $(OUT)

calc.o: calc.c calc.h
	$(CC) -c calc.c -o calc.o

show.o: show.c show.h
	$(CC) -c show.c -o show.o

.PHONY: clean run
clean:
	-rm $(OBJECTS)  

run: $(OUT)
	./$(OUT)
```

### 通配符

`make` 支持下面 $3$ 个通配符

| 通配符    | 描述         |
| :----- | :--------- |
| $*$    | 匹配任意多个字符   |
| $\sim$ | 匹配 `$HOME` |
| $?$    | 匹配任意一个字符   |

例如，下面的例子

```shell
clean:
	rm -f *.o
```

通配符也可以在我们的规则中

```shell
print: *.c
	lpr -p $?
	touch print
```

> [!tip] 
> 
>  `$?` 是一个自动化变量，后续介绍
> 

### 文件搜索

在大型工程中，源文件会被分类存放在不同的目录中。当 `make` 需要查找文件依赖关系时，就需要指定文件路径。一种好的解决方案是把一个路径告诉 `make`，让 `make` 在自动去找

> [!tip] 特殊变量 `VPATH` 告诉 `make` 搜索文件的目录
> 
> 如果没有指明这个变量，`make` 只会在当前的目录中去找寻依赖文件和目标文件
> 
> 如果定义了这个变量，那么，`make` 就会在当前目录找不到的情况下，到所指定的目录中去找寻文件了
> 

```shell title:Makefile
VPATH = src:../headers
```

`make` 会按照 `src`  和 `../headers` 的顺序进行搜索。目录间使用冒号 `:` 隔开

指定搜索文件目录的另一种方式使用 `vpath`（注意，全小写）关键字。有三种使用方式

```shell title:Makefile
vpath <pattern> <directories> # 符合 <pattern> 的文件在 <directories> 中搜索

vpath <pattern> # 清除符合 <pattern> 的文件的搜索目录

vpath           #  清除所有已被设置好了的文件搜索目录
```

例如，要求 `make` 在 `"../headers"` 目录下搜索所有以 `.h` 结尾的文件

```shell
vpath %.h ../headers
```

### 伪目标

```shell
.PHONY:clean
clean:
	rm *.o
```

上述例子中，`clean` 目标  **没有依赖文件**，也 **没有被其他目标依赖**，这样的目标称为 **伪目标**

像 `clean` 这种伪目标后面所定义的命令将不会被自动执行

> [!hint] 
> 
> 可以使用 `make clean` 显示调用类似于 `clean` 这种没有被依赖的目标
> 

当然，**为了避免和文件重名的这种情况**，我们可以使用一个 **特殊的标记** `".PHONY"` 来显式地指明一个目标是 "伪目标"

> [!tip] 
> 
> 定义在特殊标记 `.PHONY` 后的目标显示指明它是伪目标。即，向 `make` 说明，不管是否有这个文件，这个目标就是“伪目标”
> 

**伪目标一般没有依赖的文件**。但是，也可以为伪目标指定所依赖的文件

#### 伪目标作为默认目标

伪目标同样可以作为 "默认目标"，只要将其放在第一个。下面的示例可以 **一次性生成多个可执行文件**

```shell title:Makefile
all : prog1 prog2 prog3
.PHONY : all

prog1 : prog1.o utils.o
	cc -o prog1 prog1.o utils.o

prog2 : prog2.o
	cc -o prog2 prog2.o

prog3 : prog3.o sort.o utils.o
	cc -o prog3 prog3.o sort.o utils.o
```

> [!tip]
> 
> 我们可以看出，目标也可以成为依赖。所以，伪目标同样也可成为依赖
> 

```shell title:Makefile
.PHONY : cleanall cleanobj cleandiff

cleanall : cleanobj cleandiff
	rm program

cleanobj :
	rm *.o

cleandiff :
	rm *.diff
```

### 自动推导

GNU 的 `make` 很强大，它可以 **自动推导文件以及文件依赖关系后面的命令**，于是我们就没必要去在每一个 `.o` 文件后都写上类似的命令，因为，我们的 `make` 会自动识别，并自己推导命令

只要 `make` 看到一个 `.o` 文件，它就会自动的把 `.c` 文件加在依赖关系中，如果 `make` 找到一个 `whatever.o` ，那么 `whatever.c` 就会是 `whatever.o` 的依赖文件。并且 `cc -c whatever.c` 也会被推导出来

```shell title:Makefile
OUT := main.out              # 输出文件名
CC := /usr/bin/gcc        # 定义使用的编译器，后面可以直接更换
OBJECTS := main.o calc.o show.o  # 目标文件

$(OUT): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(OUT)

main.o: calc.h show.h

calc.o: calc.h

show.o: show.h


.PHONY: clean
clean:
	-rm $(OBJECTS) $(OUT)  
```

### 引用其他的 Makefile

在 `Makefile` 使用 `include` 关键字可以把别的 `Makefile` 包含进来，`include` 的语法是

```shell 
include <filename>
```

> [!tip] `Makefile` 的 `include` 关键字与 C 的 `#include` 指令类似
> 
> 被包含的文件会原模原样的放在当前文件的包含位置
> 

> [!warning] 
> 
> 在 `include` 前面可以有一些空字符，但是绝不能是 `Tab` 键开始
> 

`make` 命令开始时，会找寻 `include` 所指出的其它 `Makefile`，并把其内容安置在当前的位置

> [!tip] `make` 查找 `include` 文件的规则
> 
> + 如果 `make` 执行时，有 `-I` 或 `--include-dir` 参数，那么 `make` 就会在这个参数所指定的目录下去寻找
>   
> + 如果目录 `<prefix>/include` （一般是：`/usr/local/bin` 或 `/usr/include` ）存在的话，`make` 也会去找
> 

`make` 指令会读取环境变量 `MAKEFILES` 指定的文件，并执行类似 `include` 的动作

## 变量


定义变量简单的使用 `=` ，在 `= ` 的左侧是变量，右侧是变量的值

> [!tip] 
> 右侧变量的值可以定义在文件的任何一处，也就是说，右侧中的变量不一定非要是已定义好的值，其也可以使用后面定义的值
> 

```shell title:Makefile
foo = $(bar)
bar = $(ugh)
ugh = Huh?

all:
	echo $(foo)
```

我们执行 `"make all"` 将会打出变量 `$(foo)` 的值是 `Huh?` （`$(foo)` 的值是 `$(bar)` ，`$(bar)` 的值是 `$(ugh)` ，`$(ugh)` 的值是 `Huh?` ）

> [!tip] 
> 
> 可见，变量是可以使用后面的变量来定义的。
> 

```shell
CFLAGS = $(include_dirs) -O
include_dirs = -Ifoo -Ibar
```

当 `CFLAGS` 在命令中被展开时，会是 `-Ifoo -Ibar -O`。然而，下面的定义是错误的

```shell
CFLAGS = $(CFLAGS) -O
```

> [!attention] 
> 
> 这是递归定义，会让 `make` 陷入无限的变量展开过程中去
> 

为了避免上述问题，使用另一种 `:=` 操作符定义变量

```shell
x := foo
y := $(x) bar
x := later
```

其等价于：

```shell
y := foo bar
x := later
```

> [!important] 
> 
> 使用 `:=` 定义变量时，不能使用其后面定义的变量，只能使用前面已经定义好的变量
> 

```shell
y := $(x) bar  # y := bar
x := foo 
```

### 值替换

替换变量中的共有的部分，其语法为

```shell
$(var:a=b)
# 或者
${var:a=b}
```

> [!tip] 
> 
> 把变量 `var` 中尾部的 `"a"` 替换为 `"b"` 
> 

例如，下面的 `bar` 的值展开之后为 `a.c b.c c.c`

```shell
foo := a.o b.o c.o
bar := $(foo:.o=.c)
```

### 值变量

可以把变量的值再当作变量使用

```shell
x = y
y = z
a := $($(x)) 
```

上面例子中 `$(x)` 的值是 `"y"`，所以 `$($(x))` 就是 `$(y)`

还可以使用更多的层次

```shell
x = y
y = z
z = u
a := $($($(x)))
```

### 追加变量的值

操作符 `+=` 可以用于追加变量的值

```shell
objects = main.o foo.o bar.o utils.o
objects += another.o
```

`+=` 操作符等价于

```shell
objects = main.o foo.o bar.o utils.o
objects := $(objects) another.o
```

不过 `+=` 操作符更简洁

> [!attention] 
> 
> 由于前次的赋值符是 `=` ，所以 `+=` 也会以 `=` 来做为赋值，那么岂不会发生变量的递归定义。
> 
> 但是，我们不必担心，`make` 会自动处理该问题
>  

如果变量之前没有定义过，那么，`+=` 会自动变成 `=`。如果变量是使用 `:=` 操作符定义的，那么 `+=` 操作符就会以 `:=` 作为其赋值符

```shell
variable := value
variable += more
```

等价于

```shell
variable := value
variable := $(variable) more
```

### override 指示符

变量通过 `make` 的命令行参数设置，那么 `Makefile` 中对这个变量的赋值会被忽略。如果想要在 `Makefile` 中设置此类变量，那么就需要使用 `override` 指示符

```shell
override <variable>; = <value>;

override <variable>; := <value>;

override <variable>; += <more text>;
```

### 预定义变量

某些变量有 `make` 预先定义好的。它们在下表中列出

| 变量名        | 功能        | 默认值        |
| :--------- | --------- | ---------- |
| `AR`       | 创建归档文件    | `ar`       |
| `AS`       | 汇编器       | `as`       |
| `CC`       | C 编译器     | `cc`       |
| `CPP`      | C 预处理器    | `$(CC) -E` |
| `CXX`      | C++ 编译器   | `g++`      |
| `RM`       | 删除        | `rm -f`    |
| `ARFLAGS`  | 库选项       |            |
| `ASFLAGS`  | 汇编器选项     |            |
| `CFLAGS`   | C 编译器选项   |            |
| `CPPFLAGS` | C 预处理器选项  |            |
| `CXXFLAGE` | C++ 编译器选项 |            |

### 自动变量

自动变量就某些具有特殊含义的变量，它的含义和当前规则有关

| 变量      | 描述                        |
| :------ | ------------------------- |
| `$@`    | 目标名字                      |
| `$*`    | 不包含扩展名的目标文件               |
| `$+`    | 所有依赖文件                    |
| `$^`    | 所有依赖文件（不重复）               |
| `$<`    | 第一个依赖文件                   |
| `$?`    | 当前规则中依赖新于目标的所有相关文件列表，逗号分隔 |
| `$(@D)` | 目标文件的目录名部分                |
| `$(@F)` | 目标文件的文件名部分                |

下面的 `Makefile` 是使用变量的一个示例

```shell title:Makefile
OUT := main
OBJECTS := main.o calc.o show.o 

CC := /usr/bin/gcc 
CFLAGS := -Wall  # 显示所有警告

# $^ 不重复的依赖
# $@ 目标名字
$(OUT): $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@

main.o: main.c
	$(CC) -c $(CFLAGS) $^ -o $@

# $< 第一个依赖
calc.o: calc.c calc.h
	$(CC) -c $(CFLAGS)  $< -o $@

show.o: show.c show.h
	$(CC) -c $(CFLAGS) $< -o $@

.PHONY:clean
clean:
	$(RM) $(OUT) $(OBJECTS)
```

## 用 `%` 字符管理格式关系

下面的示例 `Makefile` ，如果需要迁移到其他项目，其中高亮的行都需要被修改

```shell title:Makefile hl:2,12,16,19
OUT := main
OBJECTS := main.o calc.o show.o 

CC := /usr/bin/gcc 
CFLAGS := -Wall  # 显示所有警告

# $^ 不重复的依赖
# $@ 目标名字
$(OUT): $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@

main.o: main.c
	$(CC) -c $(CFLAGS) $^ -o $@

# $< 第一个依赖
calc.o: calc.c calc.h
	$(CC) -c $(CFLAGS)  $^ -o $@

show.o: show.c show.h
	$(CC) -c $(CFLAGS) $^ -o $@

.PHONY:clean
clean:
	$(RM) $(OUT) $(OBJECTS)
```

观察上面的 `Makefile`，我们发现第 $12$ 行，第 $16$ 行 和 第 $18$ 行中的 **目标** 和 **依赖** 名字都可以从第 $9$ 行规则中的依赖中匹配出来。并且第 $12$ 行，第 $16$ 行 和 第 $18$ 行规则的命令相同

因此，我们可以 **使用 `%` 从第一条规则的依赖中提取后续的目标和依赖**

```shell title:Makefile
OUT := main
OBJECTS := main.o calc.o show.o 

CC := /usr/bin/gcc 
CFLAGS := -Wall  # 显示所有警告

# $^ 不重复的依赖
# $@ 目标名字
$(OUT): $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@

# $< 依赖列表中的第一个
%.o: %.c %.h
	$(CC) -c $(CFLAGS) $< -o $@


.PHONY:clean rebuild
clean:
	$(RM) $(OUT) $(OBJECTS)

rebuild: clean $(OUT)
```

## 内置函数

虽然下面的 `Makefile` 迁移到其他项目中时，只需要修改一行

```shell title:Makefile
OUT := main
OBJECTS := main.o calc.o show.o 

CC := /usr/bin/gcc 
CFLAGS := -Wall 

$(OUT): $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c %.h
	$(CC) -c $(CFLAGS) $< -o $@


.PHONY:clean rebuild
clean:
	$(RM) $(OUT) $(OBJECTS)

rebuild: clean $(OUT)

```

但是，我们期待不需要修改任何一条语句。为了满足类似特殊的需求，在 `makefile` 里面也可以 **使用函数** 。`make` 函数的调用格式如下

```shell
$([function] [arguments])
```

### `wildcard` 函数

`wildcard` 会从当前目录(`make` 执行的目录)中取出符合模式的文件名。例如

```shell
SRCS := $(wildcard *.c)
```

`$(wildcard *.c)` 会搜索当前目录中所有以 `.c` 结尾的文件

### `patsubst` 函数

`patsubst` 函数执行模式替换。调用格式为 `$(patsubst <pattern>,<replacement>,<text>)`

> [!tip] 
> 
> 查找 `<text>` 中的单词（单词以“空格”、“Tab”或“回车”“换行”分隔）是否符合模式`<pattern>` ，如果匹配的话，则以 `<replacement>` 替换
> 
> 函数返回被替换过后的字符串
> 

```shell
OBJECTS := $(patsubst %.c, %.o, $(SRCS))
```

通过 `wildcard` 和 `patsubst` 两个函数的配合，不需要任何修改就能将一个 `Makefile` 迁移到其他项目中

```shell title:Makefile
OUT := main

SRCS := $(wildcard *.c)  # 读取所有 .c 文件的名字

OBJECTS := $(patsubst %.c, %.o, $(SRCS))

CC := /usr/bin/gcc 
CFLAGS := -Wall  # 显示所有警告

# $^ 不重复的依赖
# $@ 目标名字
$(OUT): $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c %.h
	$(CC) -c $(CFLAGS) $< -o $@

.PHONY:clean rebuild
clean:
	$(RM) $(OUT) $(OBJECTS)

rebuild: clean $(OUT)
```
