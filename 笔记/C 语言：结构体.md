# 结构体

我们介绍 C 语言中除 [[C 语言：数组]] 外的另一个复合类型 **结构体**。结构体不同于数组，结构是可能具有 **不同类型的值（成员）** 的集合

> [!tip] 数组与结构体的差异
> 
> 数组的所有元素具有相同的类型；为了选择数组元素需要指明元素的位置
> 
>结构的元素（在 C 语言中的说法是结构的**成员**）可能具有 **不同的类型**。**每个结构成员都有名字**，因此为了选择特定的结构成员需要指明结构成员的名字而不是它的位置

## 结构变量

声明结构变量需要使用关键字 `struct`。例如，如下声明的变量，**可以存储零件的 3 种信息**

```c
struct 
{
	int number;
	char name[NEAM_LEN + 1];
	int on_hand;
} part1, part2;
```

变量 `part1, part2` 称为结构变量，每个结构变量都有 $3$ 个成员：`number`（零件的编号）、`name`（零件的名称）和 `on_hand`（现有数量）

`struct {...}` 指明了类型，`part1, part2` 则是变量名。这样的格式与普通变量声明一致

> [!tip]
> 
> **如果结构声明后面没有变量表，则不会为它分配存储空间**，`struct{...}` 仅仅描述了一个结构的 **模板** 或轮廓
> 
> ```c
> struct {
> 	int number;
> 	char name[NEAM_LEN + 1];
> 	int on_hand;
> };
> ```

**结构的成员在内存中是按照声明的顺序存储的**。假设 `part1` 变量存储在地址为 $2000$ 的内存单元；每个整数在内存中占 $4$ 字节；`NAME_LEN` 的值为 $25$；成员之间没有间隙，那么 `part1` 再内存中的存储如下图

![Pasted image 20240929161114|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755702201680-78b668e7c8034713949d1228fa218723.png)

**每个结构代表一种新的作用域**。任何声明在此作用域内的名字都不会和程序中的其他名字冲突。在同一个程序中可以出现下面的程序片段

```c
struct { 
    int number; 
    char name[NAME_LEN+1]; 
    int on_hand; 
} part1, part2; 
 
struct { 
    char name[NAME_LEN+1]; 
    int number; 
    char sex; 
} employee1, employee2;
```

> [!tip] 
> 
> 用 C 语言的术语可表述为，**结构都为它的成员设置了独立的名字空间**
> 

### 初始化

结构变量的初始化语法与数组类似，**使用花括号把要存储在结构变量中的数据包围起来**。需要注意的是，**初始化器中的值必须按照结构成员的顺序来显示**

```c
struct {
    int number;
    char name[NAME_LEN + 1];
    int on_hand;
} part1 = {528, "Disk drive", 10}, 
    part2 = {914, "Printer cable", 5};
```

> [!tip] 
> 
> 如果初始化器中的成员数少于结构的成员，剩余的成员均被初始化为 `0`
> 

C99 的指示器语法也使用与结构体

```c
struct {
    int number;
    char name[NAME_LEN + 1];
    int on_hand;
} part1 = {.on_hand = 10, .name = "Disk drive", .number = 528}
```

> [!tip]
> 
> **不必记住原始声明时成员的顺序**。而且成员的顺序在之后还可以改变，不会影响指示器
> 

### 访问成员

访问结构成员是 **通过名字访问的**。如果需要访问结构变量 `part1` 中的程序，使用成员运算符 `.` 。例如，`part1.on_hand` 可以访问结构变量 `part1` 的 `on_hand` 成员

```c
printf("Part number: %d\n", part1.number); 
printf("Part name: %s\n", part1.name); 
printf("Quantity on hand: %d\n", part1.on_hand);
```

**结构的成员是可寻址的**，所以 它们可以出现在赋值运算的左侧，也可以 作为自增或自减表达式的操作数

```c
part1.number = 258;          /* 改变 part1 的零件号码 */ 
part1.on_hand++;            /* 增加 part1 的库存量 */
```

> [!tip] 
> 
> 结构成语访问运算符 `.` 与后缀的 `++, --` 以及数组的下标运算 `[]` 具有相同的优先级。它们高于绝大多数的其他运算符
> 

```c
scanf("%d", &part1.on_hand);
```

### 结构变量的赋值

结构变量之间可以使用赋值运算符进行相互赋值。例如

```c
part2 = part1;
```

> [!important] 
> 
> 将结构变量 `part1` 中的所有成员复制到 `part2` 中的对应成员中

> [!hint] **嵌入在结构中的数组也会被复制**
> 

```c
struct {int a[10]} a1, a2;
a1 = a2;
```

注意，运算符 `=` 仅可用于两个 **类型兼容** 的结构。例如，下面声明的两个变量，它们的类型就是不兼容的

```c
struct {
	int a;
	char c;
} a;

struct {
	int a;
	char c;
} b;
```

即使具有相同的成员，成员的类型是兼容的，变量 `a` 和 `b` 也是不兼容的

> [!important] **类型兼容**：ISO C 中使用的术语 “兼容类型” 表示 “足够接近” 的类型
> 
> + 数字类型是兼容
> + 相同类型的指针是兼容的，`void *` 指针与任何类型的指针都是兼容的
> + 两个数组要兼容，则它们的元素类型必须兼容
> + 两个同时声明的结构变量的类型(比如 `part1` 和 `part2`)是兼容的
> 

特别注意 **不能使用运算符 == 和 != 来判定两个结构相等还是不等**

目前，关于结构变量还有一个重要的问题等待解决。**命名结构的类型是什么？**

假设程序需要声明具有相同成员的结构变量。能一次声明全部的变量存在的问题会变少。但是，如果需要在程序中不同的位置声明变量，就会出现许多问题

例如，在某处编写了如下代码

```c
struct { 
	int number; 
	char name[NAME_LEN+1]; 
	int on_hand; 
} part1;
```

在另一处编写了

```c
struct { 
	int number; 
	char name[NAME_LEN+1]; 
	int on_hand; 
} part2;
```

这里就存在一些问题：重复的声明导致程序膨胀；难以保证这些是声明的一致性，带来修改程序复杂的风险；**最大的问题就是 `part1` 与 `part2` 是类型不兼容的**；同时，因为 `part1` 和 `part2` 没有具体的类型名字，无法用作函数参数

解决上述问题可以使用两个方法 **定义 _结构类型_**：**结构标记** 和 **类型定义**

## 结构类型

### 结构标记

给结构声明一个标记，只需要在 `struct` 后紧跟这一个**标识符**即可

```c
struct Part { 
    int number; 
    char name[NAME_LEN+1]; 
    int on_hand; 
};  // 注意，分号是必须的
```

如果无意间忽略了结构声明结尾的分号，可能会导致奇怪的错误

```c
struct part { 
	int number; 
	char name[NAME_LEN+1]; 
	int on_hand; 
}              /* 警告：分号丢失 */ 

f(void) 
{ 
	... 
	return 0;    /* 在这行检测到错误 */ 
}
```

因为前面的结构声明没有正常终止，所以编译器会假设函数 `f` 的返回值是`struct part` 类型的。编译器直到执行函数中第一条 `return` 语句时才会发现错误，结果得到含义模糊的出错消息


**使用结构标记在任何位置声明类型兼容的结构变量**

```c
struct Part part1, part2;
struct Part part3, part3;
// 这四个变量是类型兼容的，可以相互赋值
```

> [!tip] 
> 
> `Part` 前的 `struct` 关键不能丢失。只有 `struct Part` 才能保证 `Part` 是有意的
> 
> 程序使用 `Part` 给变量命名也是可以的，虽然阅读代码可能造成混淆，但是编译器并不会
> 

**结构标记和结构变量可以同时声明**

```c
struct Part { 
    int number; 
    char name[NAME_LEN+1]; 
    int on_hand; 
} part1, part2;
```

> [!important] 
> 
> 如果只声明结构体标记，那么仅仅是创建了一个结构体的蓝图，并没有定义结构体变量
> 

### 类型定义

关于 [[C 语言：基本概念#类型定义]] 的语法，这里不再赘述。使用类型定义可以为结构类型命名

```c
typedef struct {
    int number; 
    char name[NAME_LEN+1]; 
    int on_hand; 
} Part;
```

类型 `Part` 的 **名字必须出现在定义的末尾**，而不是在单词 `struct` 的后边

`typedef` 将名字 `Part` 添加到了编译器可识别的类型表中，此后，就可以向使用内置类型那样使用 `Part`。例如，可以用 `Part` 声明变量

```c
Part part1, part2;
Part part3, part4;
```

> [!tip] 
> 
> **无论在哪里声明，所有的 `Part` 类型的变量都是兼容的**
> 

### 结构作为函数的参数与返回值

**函数可以有结构类型的实际参数和返回值**。下面来看两个例子。当把 `Part` 结构用作实际参数时，第一个函数显示出结构的成员

```c
void show_part(Part p)
{
	printf("Part number: %d\n", p.number); 
	printf("Part name: %s\n", p.name); 
	printf("Quantity on hand: %d\n", p.on_hand);
}
```

第二个函数返回 `Part` 结构，此结构由函数的实际参数构成

```c
struct Part build_part(int number, const char * name, int on_hand) 
{ 
	struct part p; 
	
	p.number = number; 
	strcpy (p.name, name); 
	p.on_hand = on_hand; 
	return p; 
}
```

**给函数传递结构** 和 **从函数返回结构** 都要求 **生成结构中所有成员的副本**。 这些操作对程序强加了一定数量的系统开销，特别是结构很大的时候

为了避免这类系统开销，有时用 **传递指向结构的指针** 来代替传递结构本身是很明智的做法。类似地，可以使函数 **返回指向结构的指针** 来代替返回实际的结构

> [!attention] 
> 
> 函数返会指针时，一定不能返回自动变量的指针
> 

#### 结构体指针


为了避免向函数传递数组和返回数组带来的系统开销，向函数传递或返回 **指向结构的指针** 来代替传递结构本身是很明智的做法。

```c
void print_part(Part *p) 
{ 
  printf("Part number: %d\n", (*p).number); 
  printf("Part name: %s\n", (*p).name); 
  printf("Quantity on hand: %d\n", (*p).on_hand); 
}
```

如果 `p` 是指向结构的指针，那么 `*p` 即为该结构，`(*p).number` 则是结构成员。其中，`(*p).number` 中的圆括号是必需的，因为结构成员运算符 `.` 的优先级比运算符 `*` 高

结构指针的使用频率非常高，为了使用方便，C 语言提供了结构指针成员运算符 `->` 访问成员。假设 `p` 是一个指向结构的指针，可以用

```c
p->结构成员
```

的形式引用相应的结构成员。上述的函数可以修改为

```c
void print_part(Part *p) 
{ 
  printf("Part number: %d\n", p->number); 
  printf("Part name: %s\n", p->name); 
  printf("Quantity on hand: %d\n", p->on_hand); 
}
```

#### 复合字面量

C99 引入的新特性  [[C 语言：函数#复合字面量]] 介绍，它可以创建无名 [[C 语言：数组]]。**复合字面量同样也可以用于创建一个无名结构变量**，而不需要先将其存储在变量中。生成的结构可以像参数一样传递，可以被函数返回，也可以赋值给变量

```c
show_part((struct Part){528, "Disk drive", 10});
print_part(&(struct Part){528, "Disk drive", 10});
```

表达式 `(struct Part){528, "Disk drive", 10}` 是结构复合字面值，依次包括成员 `528, "Disk drive", 10`

## 结构嵌套

把一种结构嵌套在另一种结构中经常是非常有用的

假设声明了如下的结构，此结构用来存储一个人的名、中间名和姓

```c
struct person_name { 
  char first[FIRST_NAME_LEN+1]; 
  char middle_initial; 
  char last[LAST_NAME_LEN+1]; 
};
```

可以用结构 `person_name` 作为更大结构的一部分内容

```c
struct student { 
  struct person_name name; 
  int id, age; 
  char sex; 
} student1, student2;
```

访问 `student1` 的名、中间名或姓需要应用两次 `.` 运算符

```c
strcpy(student1.name.first, "Fred");
```

### 匿名结构

从 C11 开始，可以 **在结构和 [[联合]] 中放在没有名字的结构**

如果结构或联合包含一个成员，该成员满足：(1) **没有名字**；(2)**被声明为结构**，但是 **没有结构标记只有成员列表**；则该成员称为**匿名结构**

```c
struct t
{
    int i;
    /*
    首先，没有名字
    其次，被声明为结构，且没有结构标签
    */
    struct
    {
        char c;
        float f;
    };
};
```

现在的问题是，如何才能访问匿名结构的成员：**匿名结构的成员被当作它的上一层结构或联合的成员**

> [!tip]
> 
>  若某个匿名结构 S 是结构或者联合 X  的成员，那么 S 的成员就被当作 X 的成员
> 

例如，结构 `t1` 包含了一个没有标记、没有名称的结构成员，这个结构成员的成员 `c` 和 `f` 被认为属于 `t1`

```c
struct t
{
    int i;
    // 有结构标记的成员
    struct s
    {
        int j, k : 3;
    };  // 不会被分配内存
    
    /* 匿名结构
    该结构的成员 c 和 f 属于上层结构的成员
    */
    struct
    {
        char c;
        float f;
    };

    // 命名的成员
    struct
    {
        double d;
    } s;
} t1;
```

如果匿名结构的成员名与它的上一层结构的成员出现同名的情况时，会出现编译错误

```c
struct tag
{   
    // 这个匿名结构成员 i 属于 struct tag
    struct
    {
        int i;
    };

    // 这个匿名结构成员 i, f 和 d 属于 struct tag
    struct
    {
        // 这个匿名结构成员 i 和 f 属于 上一层匿名结构
        struct
        {
            int i;
            float f;
        };
        double d;
    };
    char c;
};
```

> [!warning] 
> 
> 成员 `i` 出现重复
> 

## 结构数组

**结构类型的变量可以存放在数组中**。例如，下列结构 `part` 的数组能够存储 $100$ 种零件的信息

```c
struct part inventory[100];
```

为了访问数组种的某个元素，可以使用下标取值方式。例如，为了显示存储在位置 `i` 的零件可以写成

```c
show_part(inventory[i]);
```

访问结构 `part` 内的成员要求结合使用取下标和成员选择。为了给 `inventory[i]` 中的成员 `number` 赋值 $883$，可以写成

```c
inventory[i].number = 883;
```

> [!tip] **初始化结构数组与初始化多维数组的方法非常相似**
> + 每个结构都拥有自己的带有花括号的初始化器
> + 数组的初始化器简单地在结构初始化器的外围括上另一对花括号

有时程序需要存储一些在程序期间不变的信息。例如，假设程序在打国际长途电话时需要访问国家代码。首先，设置结构用来存储国家名和相应代码

```c
struct dialing_code { 
    char *country; 
    int code; 
};
```

> [!warning] 
> 
> `country` 是指针而不是字符数组，当初始化 `dialing_code` 结构时，`country` 会指向字面串
> 

声明这类结构的数组并对其进行初始化，从而使此数组包含一些世界上人口最多的国家的代码

```c
const struct dialing_code country_codes[] = 
  {{"Argentina",            54}, {"Bangladesh",      880}, 
   {"Brazil",               55}, {"Burma (Myanmar)",  95}, 
   {"China",                86}, {"Colombia",         57}, 
   {"Congo, Dem. Rep. of", 243}, {"Egypt",            20}, 
   {"Ethiopia",            251}, {"France",           33}, 
   {"Germany",              49}, {"India ",           91}, 
   {"Indonesia"             62}, {"Iran",             98}, 
   {"Italy",                39}, {"Japan",            81}, 
   {"Mexico",               52}, {"Nigeria",         234}, 
   {"Pakistan",             92}, {"Philippines",      63}, 
   {"Poland",               48}, {"Russia",            7}, 
   {"South Africa",         27}, {"Korea",            82}, 
   {"Spain",                34}, {"Sudan",           249}, 
   {"Thailand",             66}, {"Turkey",           90}, 
   {"Ukraine",             380}, {"United Kingdom",   44}, 
   {"United States",         1}, {"Vietnam",          84}};
```

C99 开始，允许指示器组合使用

```c
struct part inventory[100] = {
    [0].number = 528, [0].on_hand = 10, [0].name[0] = '\0'  
    };
```

## 内存对齐

CPU 需要从内存中读取数据，就需要通过 **地址总线** 将地址发送给内存，内存通过 **数据总线** 将数据发送给 CPU

内存可以认为是一个大矩阵，通过 **_行地址_** 和 **_列地址_** 选择任意一个字节，然后将这个字节发送到数据总线上

现代内存的结构如下

![Pasted image 20240710165158|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755702229696-b08056dc43d140db804fe02d637c6965.png)

内存条的一面称为 `Rank`，由若干 `Chip` 组成，一个 `Chip` 有 $8$ 个 `Bank` 组成，每个 `Bank` 可以通过一个地址选择一个位，所以一个 `Chip` 可以选择一个字节。每个 `Chip` 共用一个地址，就可以一次性选择出  $8$ 个字节

![Pasted image 20240710165957|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755702237401-4943516483b5423c8ce15b730377459c.png)

显然地址必须是 $8$ 的倍数才能保证访问效率

如果非要错开，由于最后一个字节对应的位置与前七个不同，不能在一次操作中被同一个地址选择，这样的地址是不可用的

![Pasted image 20240710170155|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755702248232-6a00da064f204110b58da3138da92151.png)

> [!tip] 访问任意地址
> 
> 有些 CPU 支持访问任意地址，是因为 CPU 多做了许多处理。例如，如果想从地址 $1$ 处读取 $8$ 字节的数据，需要分两次读取
> 
>  第一次，读取 $0 \sim 7$，但是只取后七个字节
>  
>  ![Pasted image 20240710170530|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755702256387-cfbde772fe104d70aa2ba3d48e8e0e51.png)
>  
>  第二次，读取 $8\sim 15$，但是之前第一个字节
> 
> ![Pasted image 20240710170624|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755702266545-c4be341b690e496b93e057af03dcb8ab.png)
> 

显然，访问任意地址必然会影响到性能。为了保证程序顺利高效的运行，编译器会把各种类型的数据 **_安排到合适的地址_** 并 **_占用合适的长度_**，这就是 **内存对齐**

> [!seealso] 
> 
> [[计算机组成：存储器层次结构#存储技术#随机访问存储器#DRAM]]
> 

### 对齐要求

每种类型的 **对齐值** 就是其 **_对齐边界_**

> [!tip] 
> 
> 内存对齐的要求
> + 数据 **存储地址** 是 **对齐边界的整数倍**
> + 数据 **占用的字节数** 也是 **对齐边界的整数倍**
> 

#### 对齐边界

一个类型的对齐边界与平台有关

|    平台名称    | 指针宽度 | 寄存器宽度 |
| :--------: | :--: | :---: |
|   `I386`   | $4$  |  $4$  |
|  `amd64`   | $8$  |  $8$  |
|   `arm`    | $4$  |  $4$  |
|  `arm64`   | $8$  |  $8$  |
|   `mips`   | $4$  |  $4$  |
|  `mipsle`  | $4$  |  $4$  |
|  `mips64`  | $8$  |  $8$  |
| `mips64le` | $8$  |  $8$  |
|  `ppc64`   | $8$  |  $8$  |
| `ppc64le`  | $8$  |  $8$  |
| `riscv64`  | $8$  |  $8$  |
|  `s390x`   | $8$  |  $8$  |
|   `wasm`   | $8$  |  $8$  |
|            |      |       |

> [!important] 
> 
> $32$ 位和 $64$ 位平台的指针宽度和寄存器宽度
> + 在 $32$ 位平台上的指针宽度和寄存器宽度都是 $4$ 字节
> + 在 $64$ 位平台上的指针宽度和寄存器宽度都是 $8$ 字节
> 

> [!tip]
> 
> 寄存器宽度的值可以理解位平台的 **_机器字长_**，也是该平台的 _最大对齐边界_
> 

类型的对齐边界 由下列公式提供

$$
\min{(\text{sizeof(}\text{type)}, \text{寄存器宽度})}
$$

> [!summary] 
> 
> 类型对象的对齐边界：类型的尺寸和寄存器宽度的最小值确定
> 

### 结构内存对齐

对于结构而言，首先要确定 **各成员的对齐边界**，其中 **最大值为结构的对齐边界**

如下声明，如果在 $64$ 位平台上，寄存器宽度为 $8$ 字节 

```c
struct T  {
	char a;      // 对齐到地址为 1 的倍数
	long int b;  // 对齐到地址为 8 的倍数
	int c;       // 对齐到地址为 4 的倍数
	short d;     // 对齐到地址为 2 的倍数
} 
```

结构 `T` 的成员的对齐边界的最大值为 $8$，对齐边界为 `min(8, 8) = 8`。根据内存对齐的第一个要求：**存储结构的第一个地址必须是对齐边界的倍数**。然后，结构体的每个成员都应该将该地址当作起始地址 $0$，然后再用相对地址，确定自己应该存放再哪儿

![Pasted image 20240710172956|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755702277703-b71446302e604a65afb5650098f2ebb9.png)

成员 `a` 的对齐边界为 $1$，所以 `a` 存储在起始地址处

![Pasted image 20240710173103|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755702286595-f4e26c2db27247d281ace15303d0a444.png)

成员 `b` 的对齐边界为 $8$，所以 `b` 应该存放在相对地址为 `8` 的位置

![Pasted image 20240710173201|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755702297467-72124b73ca59480f9daa8f2b1dcc9aaa.png)

成员 `c` 的对齐边界为 $4$，所以 `c` 可以紧挨着 `b` 存储

![Pasted image 20240710173309|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755702316445-f30af354abdb4b5b8c4d0243afa3b658.png)

成员 `d` 的对齐边界为 $2$，所以 `d` 可以紧挨着 `c` 存储

![Pasted image 20240710173409|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755702326852-c304347d1e7b43038ffd72dc664b7e76.png)

内存对齐的第二要求：结构整体占用字节数是对齐边界的整数倍，不够还需要向后扩张。所以，该结构体需要扩张到相对地址 `23` 处，共占用 `24` 字节

![Pasted image 20240710173725|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755702335114-79f8ae160e4547c6b13f761638777294.png)

```c
#include <stdio.h>

int main() {
    struct T  {
        char a;      // 对齐到地址为 1 的倍数
        long int b;  // 对齐到地址为 8 的倍数
        int c;       // 对齐到地址为 4 的倍数
        short d;     // 对齐到地址为 2 的倍数
    } t;

    printf("%zu\n", sizeof(t));  // 24
    printf("  t: %p\n", &t);     //   t: 0x7ffe8d015270
    printf("t.a: %p\n", &t.a);   // t.a: 0x7ffe8d015270
    printf("t.b: %p\n", &t.b);   // t.b: 0x7ffe8d015278
    printf("t.c: %p\n", &t.c);   // t.c: 0x7ffe8d015280
    printf("t.d: %p\n", &t.d);   // t.d: 0x7ffe8d015284
    return 0;
}
```

### 对齐运算

从 C11 开始，可以用 **运算符 `_Alignof` 得到指定类型的对齐边界**。`_Alignof` 运算符的操作数要求是用括号括起来的类型名

```c
_Alignof(type); // 获得 type 的对齐边界
```

从 C11 开始在变量的声明里新增了 **对齐指定符**。为此，还新增了关键字 `_Alignas`。对齐指定符的语法格式为

```c
_Alignas(类型名); // 等价于 _Alignas (_Alignof (类型名))

_Alignas(常量表达式);
```

**对齐指定符只能在声明里使用**，或者 **在复合字面量中使用**，强制被声明的变量按指定的要求对齐

```c
int _Alignas(8) foo; // foo 对齐到地址边界为 8 的倍数

struct s {int a; int _Alignas (8) bar;};
```

C11 新增了一个头 ，它很简单，只是定义了 $4$  个宏

> [!tip]
>  宏 `alignas` 被定义为关键字 `_Alignas`
>  
>  宏 `alignof` 被定义为关键字 `_Alignof`
>  
>  宏 `__alignas_is_defined` 和 `__alignof_is_defined` 分别被定义为整型常量 $1$ ，并分别表示 `alignas` 和 `alignof`已经定义
>  

> [!tip] C23 标准起 `alignas` 和 `alignof` 变为关键字

## 位字段

存储空间非常宝贵的情况下，可能需要将多个对象保存在一个机器字中。可以像之前一样使用位运算符进行。但是 C 语言提供的 **位字段** 可以更好的处理

直接定义和访问一个字中的位段的能力，而不需要通过按位逻辑运算符，即使用结构中的 **位字段**

```c
struct {
	unsigned int is_keyword : 1;
	unsigned int is_extern : 1;
	unsigned int is_static : 1;
} flags;
```

> [!tip] 说明
> + 定义了一个 `flags` 变量，包含 $3$ 个 $1$ 位的字段
> + `成员 : 字段宽度`，冒号后面的数字表示字段的宽度，即二进制位数
> + 字段被声明为 `unsigned int` 类型

一个更明显的例子，MS-DOS系统中存储文件的创建和最后修改时间的例子。由于日、月和年都是很小的数，将它们按整数存储会很浪费空间。DOS 只为日期分配了 $16$ 位：$5$ 位用于日（`day`）；$4$ 位用于月（`month`）；$7$ 位用于年（`year`）

![Pasted image 20231202170207|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755702347873-f8f2979ca7da4b638b49f646ddb63820.png)

文件日期的结构表示为

```c
struct file_date {
    unsigned int day: 5;
    unsigned int month: 4;
    unsigned int year: 7;
};
```

位域字段的类型必须是 **整数类型**（包括有符号整数和无符号整），不允许使用其他类型

> [!tip] 
> 
> 为了提高程序的可移植性，将所有的位域声明为 `unsigned` 或 `signed`
> 

**可以将位域像结构的其他成员一样使用**，如下面的例子所示

```c
struct file_date fd; 
 
fd.day = 28; 
fd.month = 12; 
fd.year = 8;     /* represents 1988 */
```

`year` 成员是根据其相距 `1980` 年（根据微软的描述，这是 DOS 出现的时间）的时间而存储的

上述语句执行后，`fd` 的形式如下

![Pasted image 20231202170526|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755702356150-4b34089c3eab4797b0145a922637c5dd.png)

> [!hint]
> 
> 使用位域有一个限制，这个限制对结构的其他成员不适用。因为通常意义上讲，**位域没有地址**，所以 **C 语言不允许将 `&` 运算符用于位域**

### 位字段如何存储

编译器在处理位域的时候，会将位域 **逐个放入存储单元**，**位域之间没有间隙**，直到剩下的空间不够存放下一个位域

> [!tip]
> 
> + 有些编译器会跳到一个存储单元，另一些则将位域拆分跨存储单元存储
> + 位域存放的顺序（从左至右，还是从右至左）也是由实现定义的

> [!tip]
> 
>  C 语言 **允许省略位域的名字**。未命名的位域经常用作 **字段间的“填充”**，以保证其他位域存储在适当的位置
>  

DOS 文件关联的时间，的存储方式如下

```c
struct file_time {
    unsigned int seconds: 5;
    unsigned int minutes: 6;
    unsigned int hours: 5;
};
```

DOS 将秒数除以 $2$，因此 `seconds` 成员实际存储的是`0~29`  范围内的数

如果并不关心 `seconds` 字段，则可以不给它命名

```c
struct file_time { 
    unsigned int : 5;   /* not used */ 
    unsigned int minutes: 6; 
    unsigned int hours: 5; 
};
```

> [!tip] 
> 
> 控制位域存储的另一个技巧是指定未命名的字段长度为 $0$
> 
> **长度为 $0$ 的位域是给编译器的一个信号，告诉编译器将下一个位域在一个存储单元的起始位置对齐**
> 

```c
struct s { 
    unsigned int a: 4; 
    unsigned int : 0;   /* 0-length bit-field */ 
    unsigned int b: 8; 
};
```

## 联合

**联合也是由一个或多个成员构成的**，而且这些成员可能具有不同的类型。**联合的成员在这个空间内彼此覆盖**。这样的结果是，给一个成员赋予新值也会改变其他成员的值。

下面的两个声明用于解释联合变量与结构变量的区别，

```c
union { 
    int i;  
    double d; 
} u;

struct { 
    int i;  
    double d; 
} s;
```

结构变量 `s` 中，成员 `i` 和 `d` 分别占据不同的内存单元

联合变量 `u` 中，成员 `i` 和 `d` 共享内存单元，相互交叠

![Pasted image 20240930124822|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755702365952-c84f0396c2bd4224b6c38acf0a0792a4.png)

联合与结构处理上述的差别之外，其他方面并没有任何差别。**唯有在给联合变量初始化时，只能初始化其中一个成员**

> [!important] 
> 
> 联合中的成员共享内存，结构中的成员顺序存放
> 

## 枚举

在许多程序中，我们会 **需要变量只具有少量有意义的值**。例如，标识扑克牌类型需要四个值：梅花，方片，红桃和黑桃

显然可以用声明成整数的方法来处理此类变量，并且用一组编码来表示变量的可能值。但是，在程序中直接使用整数表示会导致维护难度提高。在之前的内容中，我们可以 **使用宏定义解决该问题**。 **它不是最好的解决方案**，有如下几个原因

> [!tip]
> 
> + **没有为阅读程序的人指出宏表示具有相同“类型”的值**
> + 如果 **可能值的数量很多**，那么为每个值定义一个宏是很麻烦的
> + 预处理器会删除我们定义的名字，导致 **在调试期间没法使用这些名字**
> 

C 语言为具有可能值较少的变量提供了一种专用类型。**枚举类型** 是一种值由程序员列出（“枚举”）的类型，而且程序员必须为每个值命名（枚举常量）

```c
enum {CLUBS, DIAMONDS, HEARTS, SPADES} s1, s2;
```

**枚举与结构和联合在内存布局上存在很大的差别**，但是它们的声明完全一样。

枚举标记

```c
enum suit {CLUBS, DIAMONDS, HEARTS, SPADES};
```

类型定义

```c
typedef enum {CLUBS, DIAMONDS, HEARTS, SPADES} Suit; 
Suit s1, s2;
```

> [!attention] 
> 
> 注意，**枚举常量的名字必须不同于作用域范围内声明的其他标识符**
> 

枚举常量类似于用 `#define` 指令创建的常量，但是两者又不完全一样。特别地，**枚举常量遵循 C 语言的作用域规则**：如果枚举声明在函数体内，那么它的常量对外部函数来说是不可见的

**C 语言会把枚举中的值当作整数来处理** 。默认情况下，编译器会把整数  `0, 1, 2, ...` 赋给特定枚举中的常量。例如

```c
enum suit {CLUBS, DIAMONDS, HEARTS, SPADES};
```

> [!tip] `CLUBS` `DIAMONDS` `HEARTS` 和 `SPADES` 分别表示 `0` `1` `2` 和 `3`

**枚举常量的值可以是任意整数**，也可以不用按照特定的顺序列出

```c
enum dept {RESEARCH = 20, PRODUCTION = 10, SALES = 25};
```

> [!tip]
> 
> **当没有为枚举常量指定值时，它的值比前一个常量的值大 $1$**。第一个枚举常量的值默认为 $0$

```c
enum EGA_colors {BLACK, LT_GRAY = 7, DK_GRAY, WHITE = 15};
```

C23 起允许给枚举变量指定基础类型

```c
enum suit:unsigned int {CLUBS, DIAMONDS, HEARTS, SPADES};
```
