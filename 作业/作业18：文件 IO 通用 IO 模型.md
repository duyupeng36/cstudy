# 作业18：[[文件 IO：通用 IO 模型]]

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


### tee 命令

`tee` 命令是从标准输入中读取数据，直至文件结尾，随后将数据写入标准输出和命令行参数所指定的文件。**请使用 I/O 系统调用实现 `tee` 命令**。默认情况下，若已存在与命令行参数指定文件同名的文件，`tee` 命令会将其覆盖。如文件已存在，请实现 `-a` 命令行选项（`tee -a file`）在文件结尾处追加数据。
 
```c
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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
                fprintf(stderr, "未知选项字符 `\\x%x`。\n", optopt);
                return EXIT_FAILURE;
            default:
                abort();
        }
    }

    // 打开所有文件名
    int fd[argc-optind + 1]; // 变长数组：不能被初始化

    fd[0] = STDOUT_FILENO;
    // 设置权限
    flags |= O_WRONLY | O_CREAT;

    // rw-r--r--
    const mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

    for(int i = optind ; i < argc; i++) {
        if((fd[i-optind + 1] = open(argv[i], flags, mode)) == -1) {
            perror("open");
        }
    }

    char buffer[BUFFER_SIZE];

    ssize_t bytes = 0;

    while((bytes = read(STDIN_FILENO, buffer, BUFFER_SIZE)) > 0) {
        for(int i = 0; i < sizeof(fd) / sizeof(fd[0]); i++) {
            if(write(fd[i], buffer, bytes) != bytes) {
                perror("write");
            }
        }
    }

    if(bytes == -1) {
        perror("read");
    }
    for(int i = 0; i < sizeof(fd) / sizeof(fd[0]); i++) {
        if(close(fd[i]) == -1) {
            perror("close");
        }
    }
    return EXIT_SUCCESS;
}
```


### cp 命令

编写一个类似于 `cp` 命令的程序，当使用该程序复制一个包含空洞（连续的空字节）的普通文件时，要求目标文件的空洞与源文件保持一致

```c title:base.h
//
// Created by duyup on 2024/10/11.
//

#ifndef BASE_H
#define BASE_H

// 常用头文件
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

// 文件系统相关头文件
#include <fcntl.h>
#include <sys/stat.h>

// unix 标准头文件
#include <unistd.h>

#endif //BASE_H
```

```c title:errors.h
#ifndef ERRORS_H
#define ERRORS_H

#include "base.h"

#define usageError(argc, target, format, ...)     \
    do                                            \
    {                                             \
        if ((argc) != (target))                   \
        {                                         \
            fprintf(stderr, format, __VA_ARGS__); \
            exit(EXIT_FAILURE);                   \
        }                                         \
    } while (0)

#define catchError(value, target, format, ...)    \
    do                                            \
    {                                             \
        if ((value) == (target))                  \
        {                                         \
            fprintf(stderr, format, __VA_ARGS__); \
            exit(EXIT_FAILURE);                   \
        }                                         \
    } while (0)

#endif // ERRORS_H
```

```c title:copy.c
#include "base.h"
#include "errors.h"

#ifndef BUFFER_SIZE  // 支持 gcc -D
#define BUFFER_SIZE 1024
#endif


int main(int argc, char *argv[]) {

    usageError(argc, 3, "Usage: %s <old-file> <new-file>\n", argv[0]);

    int flags = O_RDONLY;
    int src_fd = open(argv[1], O_RDONLY);
    catchError(src_fd, -1, "Open: %s\n", strerror(errno));

    flags = O_WRONLY | O_CREAT | O_TRUNC;                       // 创建文件用于写入，如果文件存在则截断
    const mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;  // rw-r--r--
    int dst_fd = open( argv[2], flags, mode);
    catchError(dst_fd, -1, "Open: %s\n", strerror(errno));

    // 循环读取
    char buffer[BUFFER_SIZE];
    ssize_t num_read;
    while ((num_read = read(src_fd, buffer, BUFFER_SIZE)) > 0) {
        if(write(dst_fd, buffer, num_read) != num_read) {
            fprintf(stderr, "Write: %s\n", strerror(errno));
            return -1;
        }
    }

    catchError(num_read, -1, "Read: %s\n", strerror(errno));

    // 关闭文件
    catchError(close(src_fd), -1, "Close: %s\n", strerror(errno));
    catchError(close(dst_fd), -1, "Close: %s\n", strerror(errno));
    return 0;
}
```
