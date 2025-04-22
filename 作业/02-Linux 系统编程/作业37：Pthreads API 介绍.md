# 作业37：[[Linux 系统编程：Pthreads API]]

## 问答题

若一线程执行了如下代码，可能会产生什么结果？

```c
pthread_join(pthread_self(), nullptr);
```

>[!error] 
>
>由于 `pthread_join()` 会阻塞线程，等待第一个参数指定的线程结束。
> 
> `pthread_join(pthread_self())` 可能会阻塞并等待自己结束，从而造成 **死锁**
> 

然而，在 Linux 编写如下程序进行验证

```c
#include <pthread.h>

#include "base.h"

void * routine(void * arg) {
    printf("Before join\n");
    if(pthread_join(pthread_self(), nullptr) != 0) {
	    perror("pthread_join");
    }

    printf("After join\n");
    return (void *)1;
}

int main() {

    pthread_t tid;
    int err = pthread_create(&tid, nullptr, routine, nullptr);
    if (err != 0) {
        fprintf(stderr, "Error creating thread: %s\n", strerror(err));
    }
    void *retval;
    err = pthread_join(tid, &retval);
    if (err != 0) {
        fprintf(stderr, "Error joining thread: %s\n", strerror(err));
    }
    printf("[TID %lu]: thread[TID %lu] return value: %d\n", pthread_self(), tid, (int)retval);
    return 0;
}
```

编译运行上述代码的结果为

![[Pasted image 20241111190607.png]]

> [!success] 
> 
> `pthread_join()` 尝试连接线程本身时，会立即返回错误
> 

-- -

设代码中有一变量 `tid` 其中包含了某个线程 ID，在自身发起 `pthread_join(tid, NULL)`调用时，要避免造成与上述语句相同的后果，该线程应采取何种措施？

> [!success] 
> 
> 在调用 `pthread_join()` 之前，应该判断 `tid` 与 `pthread_self()` 的返回值是否相同。只有当 `tid` 与 `pthread_self()` 的返回值不同时才进行连接
> 


```c
// tid 不等于 pthread_self() 才进行连接
if(!pthread_equal(tid, pthread_slef())) {
	pthread_join(tid, nullptr);
}
```

---

除了缺少错误检查，以及对各种变量和结构的声明外，下列程序还有什么问题？

```c
static void * threadFunc(void *arg) {
	struct someStruct *pbuf =(struct someStruct *)arg;
	/* Do some work with structure pointed to by 'pbuf' */
}

int main(int argc, char *argv[]) {
	struct someStruct buf;
	pthread_create(&thr, NULL,threadFunc,(void *)&buf);
	pthread_exit(NULL);
}
```

> [!success] 
> `pthread_exit()` 调用后会导致主线程终止，从而导致主线程的栈被销毁。而此时由主线程创建的线程(称为线程 B)会继续执行
> 
> 因此，上述代码中主线程栈中的变量 `buf` 的声明周期终止，导致**线程 B 试图访问已经无效的内存区域，导致未定义行为**。
> 

---

分析下面代码的输出

```c
#include <pthread.h>
#include <unistd.h>

#include "base.h"

void * routine(void * arg) {
    printf("I am child thread\n");
    return NULL;
}

int main() {

    pthread_t tid;
    int err = pthread_create(&tid, nullptr, routine, nullptr);

    if(err != 0) {
        fprintf(stderr, "Error creating thread: %s\n", strerror(err));
        return -1;
    }
    printf("I am main thread\n");
    usleep(100);  // 暂停 100 微秒
    return 0;
}
```

`printf` 写入标准 IO 流，这个操作并不是原子操作的，这操作可以分为下面 $3$ 步 

```shell
#   I 退出点
A: 从用户空间拷贝到标准 IO 流缓冲区
#  II 退出点
B: 从标准 IO 缓冲区拷贝到内核缓冲区
# III 退出点
C: 清空标准 IO 缓冲区
#  IV 退出点
```

主线程在输出 `"I am main thread\n"` 后，可能在上述三个步骤中的任意一个退出点退出
+ 退出点 Ⅰ：子线程尚未执行，只有一行主线程的输出 
+ 退出点 Ⅱ：进程正常结束，`stdout` 被清理，有两行输出
+ 退出点 Ⅲ：子线程已经完成输出，但是标准 IO 流中缓存尚未清除。进程结束，`stdout` 被清理，有 $3$ 行输出
+ 退出点 Ⅳ：正常情形，有 $2$ 行输出

---

如下示例代码：分析其中可能出现的错误

```c
#include <pthread.h>
#include <unistd.h>

#include "base.h"

void * routine(void * arg) {
    char *msg = (char *) arg;
    printf("I am child thread: %s\n", msg);
    return NULL;
}

void func() {
    pthread_t tid;
    char *msg = malloc(1024 * sizeof(char));
    strcpy(msg, "This is a test heap memory\n");
    int err = pthread_create(&tid, nullptr, routine, msg);
    if(err != 0) {
        fprintf(stderr, "Error creating thread: %s\n", strerror(err));
        return;
    }
    free(msg);  // 此时 free() 之后，子线程可能尚未读取堆中的数据，从而导致子线程访问不受进程管理的内存
}

int main() {
    func();
    sleep(1);
    return 0;
}
```

> [!success] 
> 
> **主线程在创建子线程之后立即释放了传递给子线程的堆内存**。这可能导致子线程在尝试访问这块内存时遇到问题，因为这块内存可能已经被操作系统回收或分配给其他用途
> 

## 编程题

在主线程中创建一个链表，并插入若干节点。然后，将其传递给子线程，在子线程中遍历。链表的代码参考 [[数据结构：线性表#链表]]

```c

#include <pthread.h>
#include "list.h"

void apply(const void *e) {
    elem_t elem = *(elem_t *)e;
    printf("%d ", elem);
}

void *routine(void *arg) {
    list_t *list = (list_t *) arg;
    traversalList(list, apply);
    return NULL;
}

int main() {

    list_t *list = createList((elem_t []){9, 7, 9, 0, 1}, 5);
    pthread_t thread;
    int err = pthread_create(&thread, nullptr, routine, list);
    if (err != 0) {
        fprintf(stderr, "pthread_create() failed: %s\n", strerror(err));
        exit(EXIT_FAILURE);
    }
    pthread_join(thread, nullptr);
    destroyList(list);
    return 0;
}
```

---

编程测试在主线程中打开的文件对象是否能被子线程访问

```c
#include <fcntl.h>
#include <pthread.h>
#include <unistd.h>

#include "base.h"

void * routine(void * arg) {
    int fd = *(int *) arg;

    char buf[100];
    while (read(fd, buf, 100) > 0) {
        write(STDOUT_FILENO, buf, 100);
    }

    return NULL;
}


int main(int argc, char *argv[]) {

    if(argc != 2) {
        usageErr("Usage: test <testfile>\n");
    }

    int fd = open(argv[1], O_RDONLY);

    pthread_t thread;
    pthread_create(&thread, nullptr, routine, (void *)&fd);

    pthread_join(thread, nullptr);
    close(fd);
    return 0;
}
```
