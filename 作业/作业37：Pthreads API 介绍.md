# 作业37：[[Pthreads API 介绍]]

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

> `pthread_exit()` 调用后会导致主线程终止，从而导致主线程的栈被销毁。而此时由主线程创建的线程(称为线程 B)会继续执行
> 
> 因此，上述代码中主线程栈中的变量 `buf` 的声明周期终止，导致**线程 B 试图访问已经无效的内存区域，导致未定义行为**。
> 
