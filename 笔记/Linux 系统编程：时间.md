# 时间

时间是非常重要的，离开了时间，几乎没有那个生产环境数据能够有意义

程序可能会关注两种时间类型：**真实世界** 和 **进程时间**

> [!important] 真实时间
> 真实时间的 _**起点**_ 有两种度量方式：**_某个标准时间起点_** 和 **_进程生命周期内的某个时间_**
> + 某个 _标准时间起点_ 度量的时间称为 **_日历时间_**，适用于需要对数据库记录或文件打上时间戳的程序
> + 通过 _进程声明周期内的某个时间起点_ 度量的时间称为 **_流逝时间_** 或 **_墙时间_**，主要针对需要周期性操作或定期从外部输入设备进行度量的程序

> [!tip] 进程时间
> 进程时间是一个进程所使用的 CPU 时间总量，适用于对程序、算法性能的检查或优化

大多数计算机体系结构都内置有硬件时钟，使内核得以计算真实时间和进程时间

## 日历时间

无论地理位置如何，UNIX 系统内部对时间的表示方式均是 **以自 Epoch 以来的秒数** 来度量的

> [!tip] Epoch：即是 UTC 时间的 **1970 年 1 月 1 日早晨零点**
> Epoch 即是通用协调时间(UTC，也称为格林威治标准时间，或 GMT）的 **1970 年 1 月 1 日早晨零点**

日历时间存储于类型为 `time_t` 的变量中，此类型是由 SUSv3 定义的整数类型

> [!attention] 
> 
> 对于 $32$ 位的 Linux 系统，`time_t` 是一个有符号整数，可以表示的日期范围从 `1901-12-13 20:45:52` 至 `2038-01-19 03:14:07`
> 

系统调用 `gettimeofday()`，可于 `tv` 指向的缓冲区中返回日历时间

```c
#include <sys/time.h>

int gettimeofday(struct timeval *tv, struct timezone *tz);
/* 成功返回 0；失败返回 -1 */
```

> [!tip] 参数 `tv` ：指向如下数据结构的一个指针
> 
> ```c
> struct timeval {
> 	time_t tv_sec;              // 从 1970-01-01 00:00:00 到现在的秒数
> 	suseconds_t tv_usec;  // 额外的微秒 long int
> };
> ```
> 
> `tv_usec` 依赖于架构的具体实现来决定。在现代 x86-32 系统上，`gettimeofday()` 可以提供微妙精度
> 

> [!tip] 参数 `tz`：已被废弃，应该始终置位 `NULL`

`time()` 系统调用返回自 Epoch 以来的秒数，即上述数据结构 `tv.tv_sec` 字段的值

```c
#include <time.h>

time_t time(time_t *timep);
/* 返回自 Epoch 以来的秒数；错误返回 (time_t) -1 */
```

> [!tip] 参数 `timep`
> 
> 如果 `timep` 参数不为 `NULL`，那么还会将自 Epoch 以来的秒数置于 `timep` 所指向的位置
> 

## 时间转换函数

下图所示为用于 `time_t` 值和其他时间格式之间的转换函数

![[Pasted image 20241025173549.png]]

这些函数屏蔽了因 **时区**、**夏令时**（DST）制和 **本地化** 等问题给转换所带来的种种复杂性

### 将 `time_t` 转换为可打印格式

为了将 `time_t` 转换为可打印格式，`ctime()` 函数提供了一个简单方法

```c
#include <time.h>

char *ctime(const time_t timep);
/* 返回静态分配的字符串；错误返回 NULL */
```

把一个指向 `time_t` 的指针作为 `timep` 参数传入函数 `ctime()`，将返回一个长达 $26$ 字节的字符串，内含标准格式的日期和时间

```shell
Wed Jun 8 14:22:34 2011
```

> [!tip] 
> 
> `ctime()` 函数在进行转换时，会自动对本地时区和 DST 设置加以考虑。返回的字符串经由静态分配，下一次对 `ctime()` 的调用会将其覆盖
> 

> [!attention] 
> 
> 由于 `ctime()` 每次调用都会覆盖上一次的结果。因此， `ctime()` 是 **不可重入** 的。而 `ctime_r()` 是 `ctime()` 的可重入版本
> 

### `time_t` 和分解时间之间的转换

#### `time_t` 转换为分解时间

函数 `gmtime()` 和 `localtime()` 可将一 `time_t` 值转换为一个 **分解时间**（broken-down  time)。**分解时间被置于一个经由静态分配的结构中**，其地址则作为函数结果返回。

```c
#include <time.h>

struct tm * gmtime(const time_t *timep);
struct tm * localtime(const time_t *timep);
/* 返回静态分配 struct tm 结构的指针；失败返回 NULL */
```

函数 `gmtime()` 返回 `timt_t` 对应的 UTC 分解时间，其中 `gm` 表示 `GMT`，即格林威治时间

函数 `localtime()` 返回 `time_t` 对应的本地时间

> [!attention] 
> 
> `gmtime_r()` 和 `localtime_r()` 分别是这些函数的可重入版
> 

在这些函数所返回的 `tm` 结构中，日期和时间被分解为多个独立字段，其形式如下：

```c
struct tm
{
  int tm_sec;			/* 秒.	[0-60] (1 leap second) */
  int tm_min;			/* 分.	[0-59] */
  int tm_hour;			/* 时.	[0-23] */
  int tm_mday;			/* 日.	[1-31] */
  int tm_mon;			/* 月.	[0-11] */
  int tm_year;			/* 年.	+1900 */
  int tm_wday;			/* 星期.	[0-6] */
  int tm_yday;			/* 第几天.[0-365]	*/
  int tm_isdst;			/* 夏令时.[-1/0/1]*/
};
```

字段 `tm.tm_sec` 的上限为 $60$ 而非 $59$ 是因为考虑了闰秒。偶尔会用其将人类日历调整至精确的天文年

#### 分解时间转换为 `time_t`

函数 `mktime()` 将分解时间转换为 `time_t` 值，并将其作为结果返回。调用者将分解时间置于一个 `tm` 结构，再以 `tmptr` 指针指向该结构。这一转换会忽略输入 `tm` 结构中的 `tm_wday` 和 `tm_yday` 字段。

```c
#include <time.h>

time_t mktime(struct tm *tmptr);
/* 返回自 Epoch 以来的秒数；失败返回 (time_t) -1 */
```

> [!tip] 
> 
> `mktime()` 函数会修改 `tmptr` 指向的 `tm` 结构，至少保证 `tm_wday` 和 `tm_yday` 和其他输入字段对应起来
> 

`mktime()` 会自动调整每个字段到正确的范围之内，同时调整其他字段。所有这些调整，均发生于 `mktime()` 更新 `tm_wday` 和 `tm_yday` 字段并计算返回值 `time_t` 之前

`mktime()` 在进行转换时会对时区进行设置。此外，DST 设置的使用与否取决于输入字段 `tm_isdst` 的值

> [!tip] `tm.tm_isdst` 控制夏令时 
> 
> + `tm.tm_isdst ==  0`，则将时间视为标准时间：忽略夏令时
> + `tm.tm_isdst > 0`，则将时间视为夏令时
> + `tm.tm_isdst < 0`，自动判断夏令时是否生效
> 
> 在转换完整后，如果夏令时生效 `tm.tm_isdst` 被设置为 `1`；否则设置为 `0`
> 

### 分解时间和打印格式之间的转换

#### 从分解时间转换为打印格式

在参数 `tm` 中提供一个指向分解时间结构的指针，`asctime()` 则会返回一指针，指向经由静态分配的字符串，内含时间，格式则与 `ctime()` 相同

```c
#include <time.h>

char *asctime(const struct tm *tmptr);
/* 返回静态分配的字符串；错误返回 NULL */
```

> [!tip] 
> 
> 本地时区对 `asctime()` 几乎没有影响，因为 `tm` 结构体通常是从 `gmtime()` 或 `localtime()` 函数返回的
> 

如同 `ctime()` 一样，`asctime()` 也无法控制其所生成字符串的格式

> [!attention] 
> 
> `asctime()` 的可重入版本为 `asctime_r()`
> 

函数 `strftime()` 将分解时间转换为打印格式，不过该函数提供了更加精细的格式控制

```c
#include <time.h>

size_t strftime(char *buffer, size_t buffer_size, const char * format, const struct tm *tmptr);
/* 成功返回写入 buffer 中的字节数；失败返回 0 */
```

写入 `buffer` 中的内会按照 `format` 定义的格式进行格式化。`buffer_size` 指定了 `buffer` 的最大容量

> [!tip] 
> 
> `strftime()` 不会在字符串的结尾附加 `'\n'`，除非 `format` 中定义了 `'\n'`
> 

下表列出了 `strftime()` 的转换说明符选集

| 说明符      | 描述                        |
| :------- | ------------------------- |
| `%%`     | `%`                       |
| `%a`     | 星期缩写                      |
| `%A`     | 星期全称                      |
| `%b, %h` | 月份缩写                      |
| `%B`     | 月份全称                      |
| `%c`     | 日期和时间                     |
| `%d`     | 一个月的一天（2 位数字，01 至 31 天）   |
| `%D`     | 美国日期格式，与 `%m/%d/%y` 相同    |
| `%e`     | 一个月中的一天（2 个字符）            |
| `%F`     | ISO 日期格式，与 `%Y-%m-%d` 相同  |
| `%H`     | 小时，24 小时制                 |
| `%I`     | 小时，12 小时制                 |
| `%j`     | 一年中的一天（3 位数字，从 001 到 366） |
| `%m`     | 十进制月（2 位，01 到 12）         |
| `%M`     | 分（2 位数）                   |
| `%p`     | AM/PM                     |
| `%P`     | 上午/下午（GNU 扩展）             |
| `%R`     | 24 小时制的时间，与 `%H:%M` 格式相同  |
| `%S`     | 秒（00 至 60）                |
| `%T`     | 时间，与 `%H:%M:%S` 格式相同      |
| `%u`     | 星期几编号（1 至 7，星期一= 1）       |
| `%U`     | 以周日计算、一年中的周数（00 到 53）     |
| `%w`     | 星期几编号（0 至 6，星期日= 0）       |
| `%W`     | 以周一计算、一年中的周数（00 到 53）     |
| `%x`     | 日期（本地化）                   |
| `%X`     | 时间（本地化）                   |
| `%y`     | 2 位数字年份                   |
| `%Y`     | 4 位数字年份                   |
| `%Z`     | 时区名称                      |

#### 将打印格式时间转换为分解时间

函数 `strptime()` 是 `strftime()` 的逆向函数，将包含日期和时间的字符串转换成一分解时间

```c
#define _XOPEN_SOURCE 
#include <time.h>
 char *strptime(const char *restrict s, const char *restrict format,
				 struct tm *restrict tm);
/* 成功返回指向 s 中下一个未被解析的字符；失败返回 NULL */
```

> [!tip] 
> 
> 函数 `strptime()` 按照参数 `format` 内的格式要求，对由日期和时间组成的字符串 `s`  加以解析，并将转换后的分解时间置于指针 `tm` 所指向的结构体中。
> 

## 时区

不同的国家（有时甚至是同一国家内的不同地区）使用不同的时区和夏时制。对于要输入和输出时间的程序来说，必须对系统所处的时区和夏时制加以考虑。所幸的是，**所有这些细节都已经由 C 语言函数库包办了**

> [!tip] 时区
> 
> **时区信息** 往往是既浩繁又多变的。出于这一原因，系统没有将其直接编码于程序或函数库中，而是 **以标准格式保存于文件中，并加以维护**
> 

时区文件位于目录 `/usr/share/zoneinfo` 中。该目录下的每个文件都包含了一个特定国家或地区内时区制度的相关信息，且往往根据其所描述的时区来加以命名。此外，可以利用子目录对相关时区进行有层次的分组

![[Pasted image 20241025185334.png]]


> [!tip] 
> 
> 系统的本地时间由时区文件 `/etc/localtime` 定义，通常链接到 `/usr/share/zoneinfo` 下的一个文件。
> 

为运行中的程序指定一个时区，需要将 `TZ` 环境变量设置为由一冒号(`:`)和时区名称组成的字符串，其中时区名称定义于 `/usr/share/zoneinfo` 中

> [!tip] 
> 
> 设置时区会自动影响到函数 `ctime()`、`localtime()`、`mktime()` 和 `strftime()`
> 

为了获取当前的时区设置，上述函数都会调用 `tzset` ，对如下 $3$ 个全局变量进行了初始化

```c
char *tzname[2];
int daylight;
long timezone;
```

## 地区

世界各地在使用数千种语言，其中在计算机系统上经常使用的占了相当比例。此外，在显示诸如数字、货币金额、日期和时间之类的信息时，不同国家的习俗也不同

理想情况下，意欲在多个地理区位运行的任何程序都应处理地区（`locales`）概念，以期以用户的语言和格式来显示和输入信息。这也构成了一个相当复杂的课题——**国际化**(internationalization，**I18N**)。

在理想情况下，程序只要一次经编写，则不论运行于何处，总会自动以正确方式来执行 I/O 操作，也就是说，完成 **本地化**(localization)任务。尽管存在各种支持工具，程序国际化工作依然耗时不菲。诸如 `glibc` 之类的程序库也提供有工具，来帮助程序支持国际化

> [!tip] 地区
> 
> 和时区信息一样，**地区信息**同样是既浩繁且多变的。出于这一原因，与其要求各个程序和函数库来存储地区信息，还不如由系统 **按标准格式将地区信息存储于文件中**，并加以维护
> 

地区信息维护于 `/usr/share/local`之下的目录层次结构中。该目录下的每个子目录都包含一特定地区的信息

```shell
$ ls -l /usr/share/locale
total 816
drwxr-xr-x 3 root root 4096 Apr 23  2024 ab
...
drwxr-xr-x 3 root root 4096 Apr 23  2024 bn_BD
drwxr-xr-x 3 root root 4096 Apr 23  2024 bn_IN
...
drwxr-xr-x 3 root root 4096 Apr 23  2024 ca@valencia
...
drwxr-xr-x 3 root root 4096 Sep 21 18:13 cs.cp1250
...
drwxr-xr-x 4 root root 4096 Apr 23  2024 zh_CN
drwxr-xr-x 3 root root 4096 Sep 21 18:13 zh_CN.UTF-8
...
```

### 为程序设置地区

函数 `setlocale()` 既可设置也可查询程序的当前地区。

```c
#include <local.h>

char *setlocale(int category, const char *local);
/* 返回指向静态分配的字符串；失败返回 NULL */
```

## 更新系统时钟

系统调用 `settimeofday()` 是 `gettimeofday()` 的逆向操作。它将 `tv` 指向 `timeval` 结构体里的秒数和微秒数，设置到系统的日历时间

```c
#define _BSD_SOURCE
#include <sys/time.h>

int settimeofday(const struct timeval *tv, const struct timezone *tz);
/* 成功返回 0; 失败返回 -1 */
```


> [!tip] 
> 
> 和函数 `gettimeofday()` 一样，**`tz` 参数已被废弃，这个参数应该始终指定为 `NULL`**
> 

`settimeofday()` 调用所造成的那种 **系统时间的突然变化**，可能会对依赖于系统时钟单调递增的应用造成有害的影响。出于这个原因，当对时间做 **微小调整** 时（几秒钟误差），通常是推荐使用库函数 `adjtime()`，它将系统时钟逐步调整到正确的时间

```c
#define _BSD_SOURCE
#include <sys/time.h>

int adjtime(struct timeval *delta, struct timeval *olddelta);
/* 成功返回 0；失败返回 -1 */
```

> [!tip] 参数 `delta`：指向一个 `timeval` 结构体，指定需要改变时间的秒和微秒数
> 
> + 如果值是正值，那么每秒系统时间都会额外拨快一点点，直到增加完所需的时间
> + 如果值是负值，那么每秒系统时间都会额外减慢一点点，直到减少完所需的时间
> 

> [!attention] 
> 
> Linux/x86-32 以每 2000 秒变化 1 秒（或每天 43.2 秒）的频率调整时钟
> 

在 `adjtime()` 函数执行的时间里，它可能无法完成时钟调整。在这种情况下，剩余未经调整的时间存放在 `olddelta` 指向的 `timeval` 结构体内

> [!tip] 
> 
> 如果我们 **不关心剩余未调整的时间**，我们可以指定 `olddelta` 为 `NULL`
> 
> 如果我们 **只关心当前未完成时间校正** 的信息，而并不想改变它，我们可以指定 `delta`  参数为 `NULL`
> 

## 软件时钟

时间相关的各种系统调用的精度是受限于 **系统软件时钟的分辨率**，它的度量单位被称为 `jiffies`。`jiffies` 的大小是定义在内核源代码的常量 `HZ`。这是内核按照 `round-robin` 的 **分时调度算法** 分配 CPU 进程的单位

> [!tip] 
> 
> 在 2.4 或以上版本的 Linux/x86-32 内核中，软件时钟速度是 $100$ 赫兹，也就是说，一个 jiffy 是 $10$ 毫秒。
> 
> 由于 CPU 的速度已大大增加，Linux / x86- 32 2.6.0 内核的软件时钟速度已经提高到 $1000$ 赫兹
> 

## 进程时间

**进程时间是进程创建后使用的 CPU 时间数量**。出于记录的目的，内核把 CPU 时间分成两部分：**用户 CPU 时间** 和 **系统 CPU 时间**

> [!tip] 用户 CPU 时间
> 
> 在 **用户模式** 下执行所花费的时间数量，也称为 **虚拟时间**，这对于程序来说，是它已经得到 CPU 的时间
> 

> [!tip] 系统 CPU 时间
> 
> 在 **内核模式** 中执行所花费的时间数量。
> 

有时候，进程时间是指处理过程中所消耗的总 CPU 时间

系统调用 `times()`，检索进程时间信息，并把结果通过 `buf` 指向的结构体返回。

```c
#include <sys/time.h>

clock_t times(struct tms *buf);
```

`buf` 指向的 `tms` 结构体有下列格式

```c
/* 描述进程及其子进程使用的 CPU 时间的结构。.  */
struct tms
{
	clock_t tms_utime;		/* 用户 CPU 时间.  */
	clock_t tms_stime;		/* 系统 CPU 时间.  */
	
	clock_t tms_cutime;		/* 死亡子进程的 用户 CPU 时间.  */
	clock_t tms_cstime;		/* 死亡子进程的 系统 CPU 时间.  */
};
```


函数 `clock()` 提供了一个简单的接口用于取得进程时间。它返回一个值描述了调用进程使用的总的 CPU 时间

```c
#include <time.h>

clock_t clock(void);

/* 返回总 CPU 时间；失败返回 (clock_t) -1*/
```

`clock()` 的返回值的计量单位是 `CLOCKS_PER_SEC`，所以我们必须除以这个值来获得进程所使用的 CPU 时间秒数
