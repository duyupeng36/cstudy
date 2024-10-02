# 标准 IO

C 语言的输入/输出库是标准库中最大且最重要的部分。我们在之前使用的 `printf` 和 `scanf` 就是 `<stdio.h>` 中的函数

## 流

在 C 语言中，术语 **流** 表示 **任意输入的源** 或 **任意输出的目的地**。**流** 常常表示存储在不同介质(如硬盘驱动器、 CD、DVD 和闪存)上的 **文件**，但也很容易和不存储文件的 **设备**(如网络端口、打印机等)相关联

> [!tip]
> 这里我们主要关注文件，说流的时候，也使用文件。请千万记住一点：`<stdio.h>` 中的许多函数 **可以处理各种形式的流，而不仅限于表示文件的流**
> 

### 文件指针

C 程序中对流的访问是通过 **文件指针** 实现的。此指针的类型为`FILE *`(`FILE` 类型在 `<stdio.h>` 中声明)。例如，我们可以声明两个文件指针

```c
FILE *fp1, *fp2;
```

虽然操作系统通常会限制可以同时打开的流的数量，但程序可以声明任意数量的 `FILE *` 类型变量

程序运行时会自动打开 $3$ 个标准流。它们的名字在 `<stdio.h>` 中定义，不需要对其进行声明，也不用打开或关闭它们即可使用

| 文件指针     | 流     | 默认含义  |
| -------- | ----- | ----- |
| `stdin`  | 标准输入流 | 默认为键盘 |
| `stdout` | 标准输出流 | 默认为屏幕 |
| `stderr` | 标准错误流 | 默认为屏幕 |

### 文本文件与二进制文件

`<stdio.h>` 支持两种类型的文件：**文本文件** 和 **二进制文件**

> [!tip]
> 
> 在 **文本文件** 中，**字节表示字符**，这使人们可以检查或编辑文件
> 
> 在 **二进制文件** 中，**字节不一定表示字符**，字节组还可以表示其他类型的数据，比如整数和浮点数
> 

## 文件操作

文件操作，包括 **打开文件**、**关闭文件**、**改变缓冲文件的方式**、**删除文件** 以及 **重命名文件**

### 打开文件

函数 `fopen()` 用于打开文件，其函数原型如下

```c
FILE *fopen(const char * restrict filename, const char * restrict mode);
```

`fopen` 函数的第一个参数是含有要打开文件名的字符串；第二个参数是“模式字符串”，它用来指定打算对文件执行的操作。

> [!warning] 
> 
> 永远不要假设可以打开文件，每次都要测试 `fopen` 函数的返回值以确保不是空指针。
> 

#### 模式

给 `fopen` 函数传递哪种模式字符串不仅依赖于稍后将要对文件采取的操作，还取决于文件中的数据是文本形式还是二进制形式

要打开文本文件，采用下表列出的模式字符串

| 模式      | 含义                 |
| :------ | ------------------ |
| `"r"`   | 只读                 |
| `"w"`   | 只写                 |
| `"wx"`  | C11 引入的模式，独占创建用写   |
| `"w+x"` | C11 引入的模式，独占创建用于更新 |
| `"a"`   | 追加                 |
| `"r+"`  | 读写，从文件开头           |
| `"w+"`  | 读写，文件存在就截断         |
| `"a+"`  | 读写，追加              |

> [!tip] 要打开二进制文件，需要在模式字符串中包含字母 `b`

### 关闭文件

进程允许打开的流时有限的。一个打开流不再使用就需要立即关闭。函数 `fclose()` 允许程序关闭不再使用的流

```c
int fclose(FILE *stream);
```

如果成功关闭了文件，`fclose` 函数会返回零；否则，它会返回错误代码 `EOF`

### 为打开的流附加文件

进程中已存在打开的流，想要替换流关联的文件可以使用函数 `freopen()`

```c
FILE *freopen(const char * restrict filename, 
              const char * restrict mode, 
              FILE * restrict stream);
```

> [!tip] 最常见的用法是把文件和一个标准流相关联

`freopen` 函数的返回值通常是它的第三个参数（一个文件指针）。如果无法打开新文件，那么 `freopen` 函数会返回空指针

### 临时文件

现实世界中的程序经常需要产生临时文件，即只在程序运行时存在的文件。标准库提供下列两个函数支持临时文件

```c
FILE *tmpfile(void);    // 创建文件
char *tmpnam(char *s);  // 为临时文件产生名字
```

`tmpnam` 函数为临时文件产生名字。 如果它的实际参数是空指针，那么 `tmpnam` 函数会把文件名存储到一个静态变量中，并且返回指向此变量的指针

### 文件缓冲

**向磁盘驱动器传入数据或者从磁盘驱动器传出数据都是相对较慢的操作**。因此，在每次程序想读或写字符时都直接访问磁盘文件是不可行的。获得较好性能的诀窍就是 **缓冲** 

> [!tip]
> 
> **把写入流的数据存储在内存的缓冲区域内**；当缓冲区满了（或者关闭流）时，对缓冲区进行“清洗”（写入实际的输出设备）
> 
> 输入流可以用类似的方法进行缓冲
> 

`<stdio.h>` 中的函数会在缓冲有用时自动进行缓冲操作。缓冲是在后台发生的，我们通常不需要关心它的操作。然而，极少的情况下我们可能需要更主动。如果真是如此，可以使用 `fflush` 函数 `setbuf` 函数和 `setvbuf` 函数；它们的函数原型如下

```c
int fflush(FILE *stream);  // 刷新缓冲区                        

void setbuf(FILE * restrict stream, char * restrict buf);

int setvbuf(FILE * restrict stream, char * restrict buf, int mode, size_t size);
```

`setvbuf` 函数允许改变缓冲流的方法，并且允许控制缓冲区的大小和位置。函数的第三个实际参数指明了期望的 **缓冲类型**。缓冲类型由下面上宏指定

| 缓冲类型      | 描述                               |
| :-------- | -------------------------------- |
| `_IOFBUF` | 满缓冲：当缓冲区为空时，从流读入数据；当缓冲区满时，向流写入数据 |
| `_IOLBUF` | 行缓冲：每次从流读入一行数据或者向流写入一行数据         |
| `_IONBUF` | 无缓冲：直接从流读入数据或者直接向流写入数据，而没有缓冲区    |

> [!tip] 
> 
> `setbuf` 函数是一个较早期的函数。现在不用关注它
> 

### 删除和重命名

`remove` 函数和 `rename` 函数允许程序执行基本的文件管理操作

```c
int remove(const char *filename);                
int rename(const char *old, const char *new);
```

## 文件顺序访问

### 字符输入输出

**向流写入字符(字节)** 的函数有

```c
int fputc (int c, FILE * stream);
int putc (int c, FILE * stream);
```

这两个函数均向 `stream` 中写入字符 `c`。它们的工作原理相同，但是 `putc` 通常作为宏来实现(也可以作为函数实现)，`fputc` 只作为函数实现。如果写入成功，则返回该字符，否则返回 `EOF`

`putchar` 函数的实现方式如下

```c
#define putchar(c) putc((c), stdout)
```

**从流读取字符(字节)** 的函数有

```c
int fgetc (FILE * stream);
int getc (FILE * stream);
```

这两个函数都从流 `stream` 读取一个字符。`getc` 通常作为宏实现，`fgetc` 只作为函数实现。读取成功返回字符；遇见文件末尾指示符或出现错误均返回 `EOF`

`getchar` 函数的实现方式如下

```c
#define getchar() getc(stdin)
```

**有时候可能会多读取一个字符，并且需要将其放回流中**，函数 `ungetc` 可以完成该操作，它的函数原型如下

```c
int ungetc (int c, FILE *stream);
```

将字符 `c` 放回流 `stream`。放回成功返回字符 `c`，否则返回 `EOF`

现在，我们可以实现 `cat` 函数了

```c
#include <stdio.h>

int main(int argc, char *argv[]) 
{
    FILE * fp;

    if(argc < 2) {
        printf("usage: %s file1 file2 ...", argv[0]);
        return 0;
    }

    for (int i = 1; i < argc; i++) {
        fp = fopen(argv[i], "r");
        if(fp == NULL){
            printf("error open file %s", argv[i]);
            return 0;
        }
        int c; 
        while ((c = getc(fp)) != EOF)
        {
            putc(c, stdout);
        }
        putc('\n', stdout);
        fclose(fp);
    }
    return 0;
}
```

### 格式化输入输出

在 [[基本概念#格式化输入和输出]] 中，我们介绍了格式化输入输出的基本内容。关于在长度指定符尚未解释。长度指定符需要与转换指定符一起确定类型

```sheet
|长度指定符|转换指定符|含义|
|---|---|---|
|`hh`|`d, i, o, u, x, X`|`signed char, unsigned char`|
|^|`n`|`char *`|
|`h`|`d, i, o, u, x, X`|`short int, unsigned short int`|
|^|`n`|`short int *`|
|`l`|`d, i, o, u, x, X`|`long int, unsigned long int`|
|^|`n`|`long int *`|
|^|`c`|`wint_t`|
|^|`s`|`wchar_t *`|
|^|`a, A, e, E, f, F, g, G`|无作用|
|`ll`|`d, i, o, u, x, X`|`long long int, unsigned long long int`|
|^|`n`|long long int *|
|`j`|`d, i, o, u, x, X`|`intmax_t, uintmax_t`|
|^|`n`|`intmax_t *`|
|`z`|`d, i, o, u, x, X`|`size_t`|
|^|`n`|`size_t *`|
|`t`|`d, i, o, u, x, X`|`ptrdiff_t`|
|^|`n`|`ptrdiff_t *`|
|`L`|`a, A, e, E, f, F, g, G`|`long double`|
```


函数 `fprintf` 向流写入格式化的字符串。该函数与 `printf` 函数除了第一个参数不同外，其余完全一样。其函数原型如下.

```c
int fprintf (FILE * restrict stream, 
			 const char * restrict fmt, ...)
```

函数 `fscanf` 从流中读取字符并转换为指定格式后存储在指定的位置。该函数与 `scanf` 函数也只有第一个参数的差异

```c
int fscanf (FILE *__restrict __stream,
		   const char *__restrict __format, ...)
```

> [!tip] 
> 
> 它们 **只能用于文本数据的输入输出**
> 

### 行输入输出

除了单个字符进行输入输出外，还可以按行进行。`fputs` 函数用于向流写入一行(可以是文本，也可以是二进制流)。`fgets` 函数用于从流中读取一行(可以是文本，也可以是二进制流)。它们的函数原型如下

```c
int fputs (const char * restrict s,
		   FILE * restrict stream);
char * fgets (char * restrict s, int n, 
			  FILE * restrict stream)
```
`fputs` 函数将 `s` 指向的一行写入流 `stream`。出现写错误时，返回 `EOF`，否则返回一个非负数。`fputs` 函数不会自己写入换行符

`fgets` 函数从 `stream` 流中读取 `n` 个字节到 `s` 指向的内存块中。出现读错误或者遇见文件末尾指示符时，返回 `EOF`；否则，返回自己的第一个实参`s`(指向保存输入的数组的指针)

> [!important] 
> 
> **`fgets` 和 `fputs` 虽然可以用于二进制数据的读写，但是主要还是用于文本数据读写**
> 

使用 `fgets` 和 `fputs` 实现 `cat` 命令

```c
#include <stdio.h>
#define BUFERSIZE 10

int main(int argc, char *argv[]) 
{
    FILE * fp;
    char buf[BUFERSIZE+1];
    if(argc < 2) {
        printf("usage: %s file1 file2 ...", argv[0]);
        return 0;
    }

    for (int i = 1; i < argc; i++) {
        fp = fopen(argv[i], "r");
        if(fp == NULL){
            printf("error open file %s", argv[i]);
            return 0;
        }
        while (fgets(buf, BUFERSIZE, fp) != NULL)
        {
            fputs(buf, stdout);
        }
        putc('\n', stdout);
        fclose(fp);
    }
    return 0;
}
```

### 块输入输出

一次读写一个大的数据块对于二进制流非常重要的。`fread` 函数从流中读取指定字节数的数据块。`fwrite` 函数向流写入指定字节数的数据块。它们的原型如下

```c
size_t fread (void * restrict ptr, size_t size, 
			  size_t nmemb, FILE * restrict stream);
size_t fwrite (const void * restrict ptr, size_t size,
			   size_t nmemb, FILE * restrict s);
```

`fread` 函数从 `stream` 流中读取`nmemb` 个 `size` 字节的数据存储在 `ptr` 指向的内存块中。**返回值是读取了多少个 `size` 字节的数据**，应该等于 `nmemb`，除非到达输入文件文件的末尾或出现错误

`fwrite` 函数向流 `stream` 中写入 `ptr` 指向的内存块中的数据，总共需要写入 `nmemb` 个 `size` 字节的块。**返回实际写入的 `size` 字节块的个数**。如果出现写入错误，那么此数就会小于 `nmemb`

> [!important] 
> 
> **`fread` 和 `fwrite` 函数虽然可以读写文本流，但是主要用于读写二进制流**
> 

它们可以读写任何数据块，不一定是数组。需要注意，**如果写入的元素是指针，那么读回时不能保证有效**

```c
#include <stdio.h>

struct person
{
    char name[20];
    int age;
};


int main() 
{
    struct person p1 = {"dyp", 25};
    FILE * fp = fopen("test.bin", "w");
    if(fp == NULL) 
    {
        fprintf(stderr, "error: can not open file %s\n", "test.txt");
        return 0;
    }
    fwrite(&p1, sizeof(p1), 1, fp); // 将 p1 的内容保存到 test.txt 中
    
    struct person p2;

    fclose(fp);

    fp = fopen("test.bin", "r");
    if(fp == NULL) 
    {
        fprintf(stderr, "error: can not open file %s\n", "test.txt");
        return 0;
    }

    fread(&p2, sizeof(p2), 1, fp);
    printf("person{name=%s, age=%d}", p2.name, p2.age);
    return 0;
}
```

## 随机访问

大多数情况下，顺序访问文件来说是很好的。但是，偶尔也需要随机访问文件中的内容。每个流都有相关联的 **文件位置**

> [!tip] 文件默认位置
> 
> 文件打开时，会将文件位置重置到起始处。如果以 `"a"` 模式打开，则文件位置被设置在文件末尾

随着读写操作的进行，文件位置会自动推进，并且允许按照顺序贯穿整个文件

`<stdio.h>` 中提供了 $5$ 个函数来支持文件的随机访问。它们的原型如下

```c
void rewind(FILE *stream); // 文件位置设置为起始位置

long int ftell(FILE *stream);  // 返回文件位置
int fseek(FILE *stream, long int offset, int whence); // 设置文件位置

int fgetpos(FILE * restrict stream, fpos_t * restrict pos); 
int fsetpos(FILE *stream, const fpos_t *pos); 
```

`rewind` 函数将 `stream` 流的文件位置设置为起始位置

`ftell` 和 `fseek` 函数是一组，用于操作文件位置可以存储在 `long int` 类型的文件

`ftell` 函数返回 `stream` 流的当前文件位置，稍后可能会使用到这个返回值。`ftell` 函数遇见错误会返回 `-1L` 并设置 `errno`

`fseek` 函数用于移动 `stream` 流的文件位置。它相对 `whence` 移动 `offset` 字节。对于文本文件，`offset` 必须是 $0$。`whence` 的取值是 `<stdio.h>` 中定义的 $3$ 个宏
- `SEEK_SET`: 流的起始位置，对应整数 $0$
- `SEEK_CUR`: 流的当前位置，对应整数 $1$
- `SEEK_END`: 流的末尾，对应整数 $2$

`fgetpos` 和 `fsetpos` 函数是一组，用于操作非常大的文件。使用 `fpos_t` 类型的值表示文件位置，该类型不一定是整数，可能是结构
- `fgetpos` 函数类似于 `ftell` 函数，它将 `stream` 流的当前文件位置存储在 `pos` 指向的内存区域中
- `fsetpos` 函数类似于 `fseek` 函数，它设置 `stream` 流的文件位置为 `*pos`
