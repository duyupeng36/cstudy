# 指针高级

## 动态内存分配

**C 语言数组通常长度是固定的**。一旦完成编译，数组元素的数量在运行过程中就不能改变。即使是 C99 开始的变长数组在程序运行过程中元素的个数也是固定的。虽然可以增加数组的大小并且重新编译程序。但是，无论如何增大数组，始终有可能填满数组。幸运的是，还有别的办法。**C 语言支持 动态存储分配**，即在程序执行期间分配内存单元的能力。利用动态存储分配，可以设计出能根据需要扩大(和缩小)的数据结构

为了动态地分配存储空间，需要调用三种内存分配函数的一种，这些函数都是声明在 `<stdlib.h>` 头
- `malloc` 函数分配内存块，但是不进行初始
- `calloc` 函数分配内存块，并将内存初始化为 `0`
- `recalloc` 函数可以调整之前分配的内存块大小

这个三函数的原型如下

```c
void *malloc (size_t size);
void *calloc (size_t nmemb, size_t size);
void *realloc (void *ptr, size_t size)
```

> [!tip] `malloc` 函数分配 `size` 字节的内存块，并且返回指向该内存块的指针
> 
> 注意，`size` 的类型是 `size_t`，这是在 C 语言库中定义的无符号整数类型。除非正在分配一个非常巨大的内存块，否则可以只把 `size` 当成普通整数
> 

> [!tip] `calloc` 函数为 `nmemb` 个元素的数组分配内存空间，其中每个元素的长度都是 `size` 字节

> [!tip] `realloc` 函数的第一个参数 `ptr` 必须指向先前通过 `malloc`, `calloc` 或 `realloc` 的调用获得的内存块。`size` 表示内存块的新尺寸，新尺寸可能会大于或小于原有尺寸
> 
> 针对 `realloc` 函数 C 标准列出来几个规则
> +  当扩展内存块时，`realloc` 函数不会对添加进内存块的字节进行初始化
> + `realloc` 函数无法扩展内存时，返回 `NULL`，同时原有的内存块不会发生变化
> + 第一个参数 `ptr` 为 `NULL` 时，效果与 `malloc` 一样
> + 第二个参数 `size` 为 $0$ 时，会释放内存块。从 C23 起，这是未定义行为
> 

**`realloc` 函数应该“在原先的内存块上”直接进行缩减，而不需要移动存储在内存块中的数据**。同理，扩大内存块时也不应该对其进行移动。如果无法扩大内存块（因为内存块后边的字节已经用于其他目的），`realloc` 函数会在别处分配新的内存块，然后把旧块中的内容复制到新块中

> [!warning] 
> 一旦 `realloc` 函数返回，请一定要对指向内存块的所有指针进行更新，因为 `realloc` 函数可能会使内存块移动到其他地方

当为申请内存块而调用内存分配函数时，因为函数无法知道计划存储在内存块中的数据是什么类型的，所以它不能返回int类型、char类型等普通类型的指针。因此，函数会返回 `void *` 类型的值。**`void *` 类型的值是“通用”指针，它本质上只是内存地址**
### 空指针

当调用内存分配函数时，总存在这样的可能性：找不到满足我们需要的足够大的内存块。如果真的发生了这类问题，函数会返回 **空指针**。

**空指针是 不指向任何地方的指针，这是一个区别于所有有效指针的特殊值**。在把函数的返回值存储到指针变量中后，需要判断该指针变量是否为空指针
- **空指针用名为 `NULL` 的宏来表示**

### 释放内存

`malloc` 函数和其他内存分配函数所获得的内存块都来自一个叫作 **堆** 的存储池
- 过于频繁地调用这些函数(或者让这些函数申请大内存块)可能会耗尽堆，这会导致函数返回空指针
- **程序可能分配了内存块，然后又丢失了对这些块的记录**，因而浪费了空间

如下两个内存分配

```c
p = malloc(...); 
q = malloc(...); 
p = q;
```

在执行完前两条语句后，`p` 和 `q` 分别指向两个内存块

![Pasted image 20240930131302|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755702392606-3d05c0865209436a8766a9e0ab01c2c0.png)

在把 `q` 赋值给 `p` 之后，两个指针现在都指向了第二个内存块

![Pasted image 20240930131348|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755702404123-35fb63fa05fd43b38b44cffbfcd684cf.png)

> [!tip] 
> 
> 因为 **没有指针指向第一个内存块**(阴影部分)，所以 **再也不能使用此内存块** 了
> 

对程序而言，不可再访问到的内存块被称作 **垃圾**。留有垃圾的程序存在 **内存泄漏** 现象。C 语言不提供垃圾回收。相反，**每个 C 程序负责回收各自的垃圾**，方法是调用 **`free` 函数** 来释放不需要的内存。`free` 函数的原型如下

```c
void free(void *Memory)
```

使用 `free` 函数很容易，只需要简单地把指向不再需要的内存块的指针传递给 `free` 函数就可以了
- 参数 `Memory` 可以为 `NULL`，只不过不起任何作用
- **参数 `Memory` 必须指向 `malloc, calloc, realloc` 函数申请的内存**。如果参数是指向其他对象（比如变量或数组元素）的指针，可能会导致未定义的行为
#### 悬空指针

`free` 函数允许收回不再需要的内存，但是使用此函数会导致一个新的问题：**悬空指针**

>[!tip] 调用 `free(p)` 函数会释放p指向的内存块，但是不会改变 `p` 本身

如果忘记了 `p` 不再指向有效内存块，混乱可能随即而来

```c
char *p = malloc(4); 
... 
free(p); 
... 
strcpy(p, "abc");     /*** WRONG ***/
```

修改 `p` 指向的内存是严重的错误，因为程序不再对此内存有任何控制权了。**试图访问或修改释放的内存块会导致未定义的行为**

> [!danger] 
> 
> 悬空指针是很难发现的，因为几个指针可能指向相同的内存块。在释放内存块后，全部的指针都悬空了
> 

## 指向函数的指针

函数和数组一样，它们本身不是变量，但是可以 **使用指针指向函数**。这种类型的指针可以被赋值，存放在数组中，传递给函数及作为函数的返回值等等。

我们来看一下 `<stdlib.h>` 中提供的 `qsort` 函数。它的函数声明如下

```c
typedef int (*compar_fn_t) (const void *, const void *);
extern void qsort (void *base, size_t nmemb, size_t size, compar_fn_t compar);
```

> [!tip] 
> 
> `qsort` 函数需要根据 `int (*compar_fn_t) (const void *, const void *)` 类型的函数传递给出的值对元素进行排序
> 

下面是 `qsort` 的使用例程

```c
#include <stdlib.h>
#include <stdio.h>

int cmp(const void *a, const void *b)
{
    return *(int *)a - *(int *)b;
}

int main() {
	int a[] = {9, 16, 47, 4, 66, 12, 3, 25, 51};
	
    qsort(a, sizeof(a) / sizeof(a[0]), sizeof(a[0]), cmp);

	for(int i = 0; i < sizeof(a)/sizeof(a[0]); i++) {
		printf("%d ", a[i]);
	}
    printf("\n");
	return 0;
}
```

### 指向函数的指针的其他用途

> [!tip] 指向函数的指针被当做普通数据使用
> 
> 我们可以 **把函数指针存储在变量** 中，或者 **用作数组的元素**，再或者 **用作结构或联合的成员**，甚至可以编写 **返回函数指针** 的函数

下面的例子中变量存储的就是指向函数的指针

```c
#include <stdio.h>

void f(int a){

}

int main()
{
	void (*pf)(int); // 声明一个变量 `pf` 指向返回值为空，需要一个 int 类型参数的函数
	pf = f; // 注意，
	printf("f = %p\n", f); // 函数名的值就是指向函数的第一条指令的地址
	printf("&f = %p\n", &f); 
	// 如何调用呢？
	(*pf)(2);
	pf(2);
	return 0;
}
```

> [!tip] 函数名作为指针使用时，它是指向函数第一条指令的地址

## 受限指针

C99 引入了与指针相关的两种特性：**受限指针** 和 **[[#弹性数组成员|弹性数组成员]]**。受限指针在这里介绍。

从 C99 标准开始， `restrict`是一个可以在指针声明中使用的关键字。`restrict` 关键字是由程序员给编译器的一种意向声明

> [!tip] 
> `restrict` 指针表示在指针的生命周期内，只有指针本身或直接从指针派生的值(如指针`+ 1`)将用于访问指针指向的对象
>  
>  **这限制了指针别名的效果，有助于优化**
>  

> [!tip] 别名
> 
> 如果 **一个对象有多种访问方式**，通常把这些方式互称为 **别名**
> 

通过下面的描述，理解 **受限指针**。假设 `p` 和 `q` 的声明如下

```c
int * restrict p;
int * restrict q;
```

假设  `p` 指向一块动态分配的内存

```c
p = malloc(sizeof(int));
```


通常，可以将 `p` 赋值给 `q`，然后通过 `q` 对内存块中的内容进行修改

```c
q = p;
*q = 0;  // 导致未定义行为
```

> [!summary] 
> 
> 由于 `p` 是受限指针，语句 `*q = 0` 的执行效果是 **未定义** 的
> 
> 通过将 `p` 和 `q` 指针指向同一个对象，可以使 `*p` 和 `*q` 互为别名
> 

考虑下面这个函数

```c
int updatePtrs(int * ptrA, int * ptrB, int *value) {
	*ptrA += *value;
	*ptrB += *value;
}
```

指针 `ptrA` 、 `ptrB` 和 `value` 可能引用相同的内存位置，因此编译器可能会生成不太理想的代码

在没有开启优化的情况下

```
/* ptrA in %rdi; ptrB in %rsi; value in %rdx*/
0000000000000000 <updatePtrs>:
   0:   f3 0f 1e fa             endbr64 
   4:   55                      push   %rbp         // 保存当前栈底指针
   5:   48 89 e5                mov    %rsp,%rbp    // 将栈顶指针保存在 %rbp 中
   8:   48 89 7d f8             mov    %rdi,-0x8(%rbp)   // 保存 ptrA
   c:   48 89 75 f0             mov    %rsi,-0x10(%rbp)  // 保存 ptrB
  10:   48 89 55 e8             mov    %rdx,-0x18(%rbp)  // 保存 value
  14:   48 8b 45 f8             mov    -0x8(%rbp),%rax   // 读入 ptrA
  18:   8b 10                   mov    (%rax),%edx       // 读入 *ptrA
  1a:   48 8b 45 e8             mov    -0x18(%rbp),%rax  // 读入 value 的值
  1e:   8b 00                   mov    (%rax),%eax      // 读入 *vaule 的值
  20:   01 c2                   add    %eax,%edx     // *ptrA + *value
  22:   48 8b 45 f8             mov    -0x8(%rbp),%rax   // 读入 ptrA
  26:   89 10                   mov    %edx,(%rax)       // 保存 *ptrA 的值
  28:   48 8b 45 f0             mov    -0x10(%rbp),%rax  // 读入 ptrB
  2c:   8b 10                   mov    (%rax),%edx       // 读入 *ptrB
  2e:   48 8b 45 e8             mov    -0x18(%rbp),%rax   // 读入 value
  32:   8b 00                   mov    (%rax),%eax       // 读入 *value
  34:   01 c2                   add    %eax,%edx     // *ptrB + *value
  36:   48 8b 45 f0             mov    -0x10(%rbp),%rax  // 读入 ptrB
  3a:   89 10                   mov    %edx,(%rax)   // 保存 *ptrB
  3c:   90                      nop
  3d:   5d                      pop    %rbp
  3e:   c3                      ret  
```

在开启优化 `-O1` 的情况下生成的代码如下

```
/* ptrA in %rdi; ptrB in %rsi; value in %rdx*/
0000000000000000 <updatePtrs>:
   0:   f3 0f 1e fa             endbr64 
   4:   8b 02                   mov    (%rdx),%eax
   6:   01 07                   add    %eax,(%rdi)
   8:   8b 02                   mov    (%rdx),%eax
   a:   01 06                   add    %eax,(%rsi)
   c:   c3                      ret 
```

即使开启较高的优化情况下，指针 `value` 也会被读取两次，指针 `ptrA` 、 `ptrB` 和 `value` 可能引用相同的内存位置

考虑下面这个函数

```c
void updatePtrs(int *restrict ptrA, int *restrict ptrB, int * restrict value) {
    *ptrA += *value;
	*ptrB += *value;
}
```

指明了指针 `ptrA` 、 `ptrB` 和 `value` 引用不同的内存位置。当开启优化 `-O1` 的情况下会生成如下代码

```
000000000000000d <updatePtrs>:
   d:   f3 0f 1e fa             endbr64 
  11:   8b 02                   mov    (%rdx),%eax
  13:   01 07                   add    %eax,(%rdi)
  15:   01 06                   add    %eax,(%rsi)
  17:   c3                      ret 
```

指针 `value` 指向的值只会被读取一次，因为它不可能和指针 `ptrA` ， `ptrB` 引用相同的内存位置

## 弹性数组成员

有时我们需要定义一个结构，其中包括未知大小的数组。通常，一个字符串是一个以空字符标志结束的字符数组，但是用其他方式存储字符串是有好处的。**一种选择是将字符串的长度与字符存于一起（没有空字符）**。长度和字符可以存储在如下的结构中

```c
struct vstring { 
	int len;  
	char chars[N]; 
};
```

这里 `N` 是一个表示字符串最大长度的宏。但是，我们不希望使用这样的 **定长数组**，因为这样会迫使我们限制字符串的长度，而且会浪费内存（大多数字符串并不需要 `N` 个字符）

传统的解决方案是声明 `chars` 的长度为 $1$，然后动态分配每一个字符串

```c
struct vstring { 
  int len;  
  char chars[1]; 
}; 
... 
struct vstring *str = malloc(sizeof(struct vstring) + n - 1); // 为该结构分配了比它应该具有的内存更多
str->len = n;
```

上述方法 **不仅限于字符数组，它有很多用途**。被许多的编译器支持，有的编译器（包括 GCC）甚至允许 `chars` 数组的长度为零，这就使得这一技巧更明显了。但是 C89 标准并不能保证 `struct hack` 工作，也不允许数组长度为 `0`

从 C99 开始提供了 **弹性数组成员** 来达到同样的目的。**当结构的最后一个成员是数组时，其长度可以省略**

```c
struct vstring { 
  int len; 
  char chars[];    /* flexible array member – since C99 */ 
};
```

`chars` 数组的长度在为 `vstring` 结构分配内存时确定，通常调用 `malloc`

```c
struct vstring *str = malloc(sizeof(struct vstring) + n); 
str->len = n;
```

在这个例子中，`str` 指向一个 `vstring` 结构，其中 `char` 数组占有 `n` 个字符。 `sizeof` 操作在计算结构大小时忽略了 `chars` 的大小。**弹性数组成员的不同寻常之处在于，它在结构内并不占空间**

> [!tip] 
> 
> 
关于弹性数组，需要遵循下面这些专用规则
> + 弹性数组成员 **必须出现在结构的最后**，而且结构必须 **至少还有一个其他成员**
> + 复制包含弹性数组成员的结构时，**其他成员都会被复制** 但 **不复制弹性数组本身**
> 

