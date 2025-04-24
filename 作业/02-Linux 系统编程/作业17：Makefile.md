# 作业17：[[GNU：Makefile]]

编译一个 `Makefile`，在一个目录中有多个 `.c` 文件，每个文件都需要单独编译链接成一个可执行文件。尽量提升通用性

```shell title:Makefile
CC = /usr/bin/gcc
CFLAGS = -Wall

SRCS = $(wildcard *.c)
OBJECTS = $(patsubst %.c, %.o, $(SRCS))
EXECS = $(patsubst %.c, %, $(SRCS))

all: $(EXECS)

%:%.o
	$(CC) $(CFLAGS) $^ -o $@

.PHONY: all clean rebuild
clean:
	$(RM) ${OBJECTS} $(EXECS)

rebuild: clean all
```


一个项目由多个目录组成，其中可执行程序和 `Makefile` 放在项目根目录中。`.c` 文件放在 `src` 中，`.h` 中放在 `include` 中

```shell title:Makefile
OUT := main

SRCS := $(wildcard src/*.c)  # 读取所有 .c 文件的名字

SRCS += $(wildcard *.c)

OBJECTS := $(patsubst %.c, %.o, $(SRCS))

HEADERS_DIR := include

CC := /usr/bin/gcc 

CFLAGS := -Wall  -I$(HEADERS_DIR) # 显示所有警告

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
