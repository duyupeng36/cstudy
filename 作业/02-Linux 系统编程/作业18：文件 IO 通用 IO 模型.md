# 作业18：[[Linux 系统编程：文件 IO]]

## 编程题

在编写命令行工具或应用程序时，通常需要解析用户在命令行中传递的选项和参数。例如，`ls -l /home` 中的 `-l` 是一个选项，`/home` 是一个参数。`getopt` 函数提供了一种标准化的方法来解析这些选项和参数。

C 标准库提供了 `getopt()` 函数用于解析命令行选项

```c
#include <unistd.h>

int getopt(int argc, char * const argv[], const char *optstring);
```

> [!tip] 参数
> 
> + `argc`：命令行参数的个数。通常是 `main` 函数的参数
> + `argv`：指向命令行参数的数组
> + `optstring`：字符串，包含程序所支持的选项
>   
> 每个选项字符后可以跟一个冒号 `:`，表示该选项需要一个参数。例如，`"ab:c"` 表示支持 `-a`、`-b`（需要参数）、`-c`
> 

为了配合 `getopt()` 函数，标准库还定义了下面 $4$ 个全局变量来传递解析结果

```c
extern char *optarg;
extern int optind, opterr, optopt;
```

> [!tip] 全局变量
> 
> + `optarg`：指向当前选项的参数
> + `optind`：下一个要解析的参数的索引，默认为 $1$
> + `opterr`：如果为非零值，`getopt` 会在遇到错误时自动打印错误信息。默认值为 `1`
> + `optopt`：当 `getopt` 发现未知选项或缺少选项参数时，会存储该选项字符
> 

> [!warning] 
> 
> 当 `getopt` 解析到未知选项或缺少选项参数时，会返回 `?`，并将 `optopt` 设置为出错的选项字符
> 

下面是一个使用 `getopt` 的示例程序，演示如何解析命令行选项：

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    int opt;
    // 定义支持的选项字符串，'a'和'c'不需要参数，'b'需要参数
    while ((opt = getopt(argc, argv, "ab:c")) != -1) {
        switch (opt) {
            case 'a':
                printf("选项 a 被触发\n");
                break;
            case 'b':
                printf("选项 b 被触发，参数为: %s\n", optarg);
                break;
            case 'c':
                printf("选项 c 被触发\n");
                break;
            case '?':
                if (optopt == 'b') {
                    fprintf(stderr, "选项 -%c 需要一个参数。\n", optopt);
                } else if (isprint(optopt)) {
                    fprintf(stderr, "未知选项 `-%c`。\n", optopt);
                } else {
                    fprintf(stderr, "未知选项字符 `\\x%x`。\n", optopt);
                }
                return EXIT_FAILURE;
            default:
                abort();
        }
    }

    // 处理非选项参数
    if (optind < argc) {
        printf("非选项参数:\n");
        while (optind < argc)
            printf("  %s\n", argv[optind++]);
    }

    return EXIT_SUCCESS;
}
```


### cp 命令

编写一个类似于 `cp` 命令的程序，当使用该程序复制一个包含空洞（连续的空字节）的普通文件时，要求目标文件的空洞与源文件保持一致

```c title:include/base.h
#ifndef BASE_H
#define BASE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>

#define min(a,b) (((a) < (b)) ? (a) : (b))
#define max(a,b) (((a) > (b)) ? (a) : (b))

typedef unsigned char byte;

[[noreturn]] void errExit(const char *format, ...);
[[noreturn]] void usageErr(const char *format, ...);
[[noreturn]] void fatal(const char *message);

#endif //BASE_H
```

```c title:lib/base.c
#include "base.h"


void errExit(const char *format, ...) {
    va_list ap;
    va_start(ap, format);
    fprintf(stderr, "error: ");
    vfprintf(stderr, format, ap);
    fprintf(stderr, "%s", strerror(errno));
    va_end(ap);
    exit(EXIT_FAILURE);
}

void usageErr(const char *format, ...) {
    va_list ap;
    va_start(ap, format);
    fprintf(stderr, "usage: ");
    vfprintf(stderr, format, ap);
    va_end(ap);
    exit(EXIT_FAILURE);
}


void fatal(const char *message) {
    fprintf(stderr, "%s\n", message);
    exit(EXIT_FAILURE);
}
```

```c title:fileio/copy.c
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "base.h"

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
#endif

int main(const int argc, const char *argv[]) {

    if(argc != 3 || strcmp(argv[1], "--help") == 0) {
        usageErr("%s <src> <dst>\n", argv[0]);
    }
    const int src = open(argv[1], O_RDONLY);
    if(src == -1) {
        errExit("open file %s error: ", argv[1]);
    }

    const int dst = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if(dst == -1) {
        errExit("open file %s error: ", argv[2]);
    }

    ssize_t read_bytes;
    byte buffer[BUFFER_SIZE];
    while ((read_bytes = read(src, buffer, BUFFER_SIZE)) > 0) {
        if(write(dst, buffer, read_bytes) != read_bytes) {
            fatal("could not write whole buffer");
        }
    }

    if(read_bytes == -1) {
        errExit("read from file %s error: ", argv[1]);
    }

    if(close(src) == -1) {
        errExit("close file %s error: ", argv[1]);
    }

    if(close(dst) == -1) {
        errExit("close file %s error: ", argv[2]);
    }

    exit(EXIT_SUCCESS);
}
```

### seek 文件空洞

程序 `seek_io` 的第一个命令行参数为将要打开的文件名称，余下的参数则指定了在文件上执行的输入/输出操作。每个表示操作的参数都以一个字母开头，紧跟以相关值（中间无空格分隔）

| 操作          | 描述                              |
| :---------- | ------------------------------- |
| `s{offset}` | 以 `SEEK_SET` 为基准，偏移 `offset` 字节 |
| `r{length}` | 在当前文件偏移处，读取 `length` 字节，以文本显示   |
| `R{length}` | 在当前文件偏移处，读取 `length` 字节，以十六进制显示 |
| `w{str}`    | 在当前文件偏移处，写入 `str` 指定的字符串        |

```c title:fileio/seek_io.c
#include <ctype.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "base.h"


int main(int argc, const char *argv[]) {
    // ./seek_io filepath s{offset}|r{length}|R{length}|w{str}

    size_t len;
    off_t offset;

    char *buffer;
    ssize_t read_size, write_size;

    if(argc < 3 || strcmp(argv[1], "--help") == 0) {
        usageErr("%s file {s<offset> | r<length> | R<length> | w<str>}", argv[0]);
    }

    const int fd = open(argv[1], O_RDWR | O_CREAT, 0644);
    if(fd == -1) {
        errExit("open file %s error: ", argv[1]);
    }

    for(int ap = 2; ap < argc; ap++) {
        switch (argv[ap][0]) {
            case 'r':
            case 'R':
                len = strtol(&argv[ap][1], nullptr, 10);
                buffer = malloc(len);
                if(buffer == nullptr) {
                    errExit("malloc error: ");
                }
                read_size = read(fd, buffer, len);
                if(read_size == -1) {
                    errExit("read error: ");
                }
                if(read_size == 0) {
                    printf("%s: end-of-file\n", argv[ap]);
                } else {
                    printf("%s: ", argv[ap]);
                    for(int j = 0; j < read_size; j++) {
                        if(argv[ap][0] == 'r') {
                            printf("%c", isprint(((unsigned char)buffer[j])? buffer[j]: '?'));
                        } else {
                            printf("%02x ", (unsigned char)buffer[j]);
                        }
                    }
                    printf("\n");
                }
                free(buffer);
                break;

            case 'w':
                write_size = write(fd, &argv[ap][1], strlen(&argv[ap][1]));
                if(write_size == -1) {
                    errExit("write error: ");
                }
                printf("%s: wrote %ld bytes\n", argv[ap], (long)write_size);
                break;
            case 's':
                offset = strtol(&argv[ap][1], nullptr, 10);
                if(lseek(fd, offset, SEEK_SET) == -1) {
                    errExit("lseek error: ");
                }
                printf("%s: seek succeeded\n", argv[ap]);
                break;
            default:
                cmdLineErr("Argument must start with [rRws]: %s\n", argv[ap]);
        }
    }
    close(fd);
    return 0;
}
```

下面的 shell 会话展示上述例程的使用方法，还显示了从文件空洞中读取字节的情况

![[Pasted image 20241024230614.png]]

### tee 命令

`tee` 命令是从标准输入中读取数据，直至文件结尾，随后将数据写入标准输出和命令行参数所指定的文件。**请使用 I/O 系统调用实现 `tee` 命令**。默认情况下，若已存在与命令行参数指定文件同名的文件，`tee` 命令会将其覆盖。如文件已存在，请实现 `-a` 命令行选项（`tee -a file`）在文件结尾处追加数据。
 
```c title:fileio/tee.c
#include <fcntl.h>
#include <unistd.h>

#include "base.h"

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
#endif


int main(int argc, char *argv[]) {

    int opt;
    int flags = 0;
    while((opt =getopt(argc, argv, "a")) != -1) {
        switch(opt) {
            case 'a':
                flags |= O_APPEND;
            break;
            case '?':
                cmdLineErr("unknown option `\\x%x` \n", optopt);
            default:
                abort();
        }
    }

    // 打开所有文件名
    int fd[argc - optind + 1]; // 变长数组：不能被初始化

    fd[0] = STDOUT_FILENO;
    // 设置权限
    flags |= O_WRONLY | O_CREAT;

    // rw-r--r--
    constexpr mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

    for(int i = optind ; i < argc; i++) {
        if((fd[i-optind + 1] = open(argv[i], flags, mode)) == -1) {
           errExit("open file %s error: ", argv[i]);
        }
    }

    byte buffer[BUFFER_SIZE];

    ssize_t bytes;

    while((bytes = read(STDIN_FILENO, buffer, BUFFER_SIZE)) > 0) {
        for(int i = 0; i < sizeof(fd) / sizeof(fd[0]); i++) {
            if(write(fd[i], buffer, bytes) != bytes) {
                errExit("write error: ");
            }
        }
    }
    if(bytes == 0) {
        fprintf(stderr, "read error: EOF\n");
    } else {
        errExit("read error: ");
    }

    for(int i = 0; i < sizeof(fd) / sizeof(fd[0]); i++) {
        if(close(fd[i]) == -1) {
            errExit("close error: ");
        }
    }
    return EXIT_SUCCESS;
}
```

---

### 结构体输入输出

定义一个学生结构体类型 `struct student`，里边含有学号(`int`)，姓名，分数(`float`)。定义结构体数组 `struct student s[3]`,给数组赋初值后，写入文件，然后通过 `lseek` 偏移到开头，然后再读取并使用 `printf` 输出

```c title:fileio/io_struct.c
#include <sys/types.h>  
#include <fcntl.h>  
#include <unistd.h>  
  
#include "base.h"  
  
  
typedef struct {  
    int id;  
    char name[20];  
    float score;  
} student_t;  
  
int main(int argc, char *argv[]) {  
  
    if(argc != 2 || strcmp(argv[1], "--help") == 0) {  
        usageErr("%s [filename]\n", argv[0]);  
    }  
  
    int fd = open(argv[1], O_RDWR | O_CREAT | O_TRUNC, 0644);  
    if(fd == -1) {  
        errExit("open %s error: ", argv[1]);  
    }  
  
    student_t stu[3] = {  
        {1, "Cai xu kun", 88.8f},  
        {2, "Wu yi fan", 59.9f},  
        {3, "Li yi feng", 77.7f}  
    };  
  
    if(write(fd, stu, sizeof(stu)) != sizeof(stu)) {  
        errExit("write error: ");  
    }  
  
    if(lseek(fd, 0, SEEK_SET) == -1) {  
        errExit("lseek error: ");  
    }  
  
    memset(stu, 0, sizeof(stu)/sizeof(stu[0]));  
  
    if(read(fd, stu, sizeof(stu)) == -1) {  
        errExit("read error: ");  
    }  
  
    for(int i =0; i < sizeof(stu)/sizeof(stu[0]); i++) {  
        printf("num = %d,score = %f,name = %s\n",  
              stu[i].id,stu[i].score,stu[i].name);  
    }  
  
    close(fd);  
    return 0;  
}
```

### 对比两个文件是否完成一致

编写C程序，判断任意两个文件，其内容是否是完全一致

```c title:fileio/file_cmp.c
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include "base.h"

int main(int argc, char *argv[]) {
    if(argc != 3) {
        usageErr("%s <pathname> <pathname>", argv[0]);
    }

    int fd1 = open(argv[1], O_RDONLY);
    if(fd1 == -1) {
        errExit("open %s error: ", argv[1]);
    }

    int fd2 = open(argv[2], O_RDONLY);

    if(fd2 == -1) {
        errExit("open %s error: ", argv[2]);
    }

    char buffer1[BUFSIZ];
    char buffer2[BUFSIZ];

    while (true) {
        // 缓冲区清零
        memset(buffer1, 0, BUFSIZ);
        memset(buffer2, 0, BUFSIZ);
        ssize_t bytesRead1 = read(fd1, buffer1, BUFSIZ);
        if(bytesRead1 < 0) {
            errExit("read %s error: ", argv[1]);
        }
        ssize_t bytesRead2 = read(fd2, buffer2, BUFSIZ);
        if(bytesRead2 < 0) {
            errExit("read %s error: ", argv[2]);
        }

        if(bytesRead1 != bytesRead2) {
            printf("Not the same\n");
            break;
        }
        if(memcmp(buffer1, buffer2, bytesRead1) != 0) {
            printf("Not the same\n");
            break;
        }
        if(bytesRead1 == 0) {
            printf("The same\n");
            break;
        }
    }
    close(fd1);
    close(fd2);
    return 0;
}
```

### 创建文件并写入

创建一个文件，里面有 $100$ 万个字符 `'1'`

```c title:fileio/create.c

#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include "base.h"


int main(int argc, char *argv[]) {
    if(argc != 2 || strcmp(argv[1], "--help") == 0) {
        usageErr("%s filename\n", argv[0]);
    }

    int fd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if(fd == -1) {
        errExit("open %s error: ", argv[1]);
    }

    char buf[10000];
    for(int i = 0; i < 100; i++) {
        memset(buf, '1', sizeof(buf));
        if(write(fd, buf, sizeof(buf)) == -1) {
            errExit("write error");
        }
    }
    close(fd);
    return 0;
}
```



