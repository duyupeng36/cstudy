# 作业34：[[Linux 系统编程：System V 信号量]] 和 [[Linux 系统编程：System V 共享内存]]

## 编程题

使用 [[Linux 系统编程：System V 信号量]] 将 [[Linux 系统编程：System V 共享内存#使用共享内存]] 中的示例代码修改为使用信号量同步

为了保证一次只有一个进程访问共享内存段，这里我们使用 [[Linux 系统编程：System V 信号量]] 作为同步工具。这里我们需要使用两个信号用于控制读者和写者
+ `WRITE_SEM`：写者信号量，初始可用
+ `READ_SEM`：读者信号量，初始被占用

首先，我们先实现一个二元信号量协议
+ `initSemAvilable()`：初始化信号量为可用的
+ `initSemInUse()`：初始化信号量为被占用的
+ `reserveSem()`：预留信号量，让信号量减少
+ `releaseSem()`：释放信号量，让信号量增加

```c title:include/binary_sems.h

#ifndef BINARY_SEMS_H
#define BINARY_SEMS_H

/* 函数控制标志 */

extern bool bsUseSemUndo; // 是否使用 SEM_UNDO
extern bool bsRetryOnEintr; // 是否在被信号中断时重试

int initSemAvailable(int semid, int semNum);
int initSemInUse(int semid, int semNum);
int reserveSem(int semid, int semNum);
int releaseSem(int semid, int semNum);

#endif //BINARY_SEMS_H
```

```c title:lib/binary_sems.c
#include <sys/types.h>
#include <sys/sem.h>
#include <errno.h>

#include "binary_sems.h"


#include "semun.h"

/**
 * 初始化信号量为可用状态，即信号量值为 1
 * @param semid 信号量集标识符
 * @param semNum 信号量索引
 * @return 0，代表成功；-1，代表失败
 */
int initSemAvailable(int semid, int semNum) {
    union semun arg;

    arg.val = 1;
    return semctl(semid, semNum, SETVAL, arg);
}

/**
 * 初始化信号量为不可用状态，即信号量值为 0
 * @param semid 信号量集标识符
 * @param semNum 信号量索引
 * @return 0，代表成功；-1，代表失败
 */
int initSemInUse(int semid, int semNum) {
    union semun arg;
    arg.val = 0;
    return semctl(semid, semNum, SETVAL, arg);
}

/**
 * 信号量值 -1。全局变量 bsRetryOnEintr 控制是否重试
 * @param semid 信号量集标识符
 * @param semNum 信号量索引
 * @return 0, 代表成功；-1，代表失败
 */
int reserveSem(int semid, int semNum) {
    struct sembuf op;
    op.sem_num = semNum;
    op.sem_op = -1;
    op.sem_flg = bsUseSemUndo ? SEM_UNDO : 0;

    while (semop(semid, &op, 1) == -1) {
        if(errno != EINTR || !bsRetryOnEintr) {
            return -1;
        }
    }
    return 0;
}

/**
 * 信号量 +1
 * @param semid 信号量集标识符
 * @param semNum 信号量索引
 * @return  0, 代表成功；-1，代表失败
 */
int releaseSem(int semid, int semNum) {
    struct sembuf op;
    op.sem_num = semNum;
    op.sem_op = 1;
    op.sem_flg = bsUseSemUndo ? SEM_UNDO : 0;

    return semop(semid, &op, 1);
}
```

下图描述了使用共享内存通信的两个进程之间如何使用信号量进行同步的

![Pasted image 20241108110423|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755788497812-40ee24006f2f4360b1658a09a6dde7ea.png)

下面的代码是写者进程的代码
+ 提供了

```c title:svshm/svshm_writer.c
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>

#include "base.h"
#include "binary_sems.h"
#include "semun.h"
#include "current_time.h"
#include "svshm_xfr.h"


int main(int argc, char *argv[]) {
    setvbuf(stdout, nullptr, _IONBF, 0);
    // 信号量集初始化
    int semid = semget(SEM_KEY, 2, IPC_CREAT | IPC_EXCL | OBJ_PERMS);
    if(semid != -1) {  // 成功创建

        // 写者信号量初始化为可用状态
        if(initSemAvailable(semid, WRITE_SEM) == -1) {
            errExit("initSemAvailable error: ");
        }
        // 读者信号初始化被使用状态
        if(initSemInUse(semid, READ_SEM) == -1) {
            errExit("initSemInUse error: ");
        }
        struct sembuf sops[2];
        // 执行一个空操作，改变 semid_ds.sem_otime 字段，让其他进程看到我们已经初始化信号集
        sops[WRITE_SEM].sem_num = WRITE_SEM;
        sops[WRITE_SEM].sem_op = 1;
        sops[WRITE_SEM].sem_flg = 0;
        sops[READ_SEM].sem_num = READ_SEM;
        sops[READ_SEM].sem_op = 0;
        sops[READ_SEM].sem_flg = 0;

        if(semop(semid, sops, 2) == -1) {
            errExit("semop error: ");
        }
    } else {
        // 创建失败
        constexpr int MAX_TRIES = 10;
        int j;
        union semun argW, argR;
        struct semid_ds dsW, dsR;

        if(errno != EEXIST) {
            // 非 EEXIST 错误
        }
        semid = semget(SEM_KEY, 1, OBJ_PERMS); // 获取已存在的信号量集标识符
        if(semid == -1) {
            errExit("semget error: ");
        }

        // 等待其他进程调用 semop(): 尝试 MAX_TRIES 次，每秒尝试 1 次
        argW.buf = &dsW;
        argR.buf = &dsR;
        for(j = 0; j < MAX_TRIES; j++) {
            memset(&dsW, 0, sizeof(struct sembuf));
            memset(&dsR, 0, sizeof(struct sembuf));
            if(semctl(semid, 0, IPC_STAT, argW) == -1) {
                errExit("semctl error: ");
            }
            if(semctl(semid, 1, IPC_STAT, argR) == -1) {
                errExit("semctl error: ");
            }

            if(dsW.sem_otime != 0 && dsR.sem_otime != 0) {
                break;
            }
            sleep(1);
        }

        if(dsW.sem_otime == 0 && dsR.sem_otime == 0) {
            errExit("semaphore init error");
        }
    }
    printf("Writer[PID %ld] semaphore init success at %s\n", (long)getpid(), current_time());

    // 创建共享内存
    int shmid = shmget(SHM_KEY, sizeof(struct shmseg),  IPC_CREAT | OBJ_PERMS);
    if(shmid == -1) {
        errExit("shmget error: ");
    }
    printf("Writer[PID %ld] create/get shared memory success at %s\n", (long)getpid(), current_time());

    // 共享内存创建成功，附加到该进程的虚拟内存
    struct shmseg *addr = shmat(shmid, nullptr, SHM_RND);
    if(addr == (void *) -1) {
        errExit("shmat error: ");
    }
    printf("Writer[PID %ld] attach shared memory success at %s\n", (long)getpid(), current_time());

    //
    printf("Writer[PID %ld] start read from stdin\n", (long)getpid());

    while (true) {
        // 等待读进程修改该信号量
        if(reserveSem(semid, WRITE_SEM) == -1) {
            errExit("reserveSem error: ");
        }
        printf("Writer[PID %ld] read: ", (long)getpid());
        addr->cnt = read(STDIN_FILENO, addr->buf, BUF_SIZE);
        if(addr->cnt == -1) {
            errExit("read error: ");
        }

        // 让读者运行
        if(releaseSem(semid, READ_SEM) == -1) {
            errExit("releaseSem error: ");
        }

        if(addr->cnt == 0) {
            // EOF 退出输入
            break;
        }
    }
    // 等待读者让我们继续输入，此时读者已经完成读取，可以删除 IPC 对象
    if(reserveSem(semid, WRITE_SEM) == -1) {
        errExit("reserveSem error: ");
    }
    printf("Writer[PID %ld] received reader finished reading  at %s\n", (long)getpid(), current_time());

    // 分离共享内存
    if(shmdt(addr) == -1) {
        errExit("shmdt error: ");
    }
    printf("Writer[PID %ld] detach shared memory success at %s\n", (long)getpid(), current_time());

    // 删除信共享内存
    if(shmctl(shmid, IPC_RMID, nullptr) == -1) {
        errExit("shmctl error: ");
    }
    printf("Writer[PID %ld] deleted shared memory success at %s\n", (long)getpid(), current_time());

    // 删除信号量
    if(semctl(semid, 0, IPC_RMID) == -1) {
        errExit("semctl error: ");
    }
    printf("Writer[PID %ld] delete semaphore at %s\n", (long)getpid(), current_time());

    return 0;
}
```

```c title:svshm/svshm_reader.c
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>

#include "base.h"
#include "binary_sems.h"
#include "semun.h"
#include "current_time.h"
#include "svshm_xfr.h"


int main(int argc, char *argv[]) {
    setvbuf(stdout, nullptr, _IONBF, 0);
    // 信号量集初始化
    int semid = semget(SEM_KEY, 2, IPC_CREAT | IPC_EXCL | OBJ_PERMS);
    if(semid != -1) {  // 成功创建

        // 写者信号量初始化为可用状态
        if(initSemAvailable(semid, WRITE_SEM) == -1) {
            errExit("initSemAvailable error: ");
        }
        // 读者信号初始化被使用状态
        if(initSemInUse(semid, READ_SEM) == -1) {
            errExit("initSemInUse error: ");
        }
        struct sembuf sops[2];
        // 执行一个空操作，改变 semid_ds.sem_otime 字段，让其他进程看到我们已经初始化信号集
        sops[WRITE_SEM].sem_num = WRITE_SEM;
        sops[WRITE_SEM].sem_op = 1;
        sops[WRITE_SEM].sem_flg = 0;
        sops[READ_SEM].sem_num = READ_SEM;
        sops[READ_SEM].sem_op = 0;
        sops[READ_SEM].sem_flg = 0;

        if(semop(semid, sops, 2) == -1) {
            errExit("semop error: ");
        }
    } else {
        // 创建失败
        constexpr int MAX_TRIES = 10;
        int j;
        union semun argW, argR;
        struct semid_ds dsW, dsR;

        if(errno != EEXIST) {
            // 非 EEXIST 错误
        }
        semid = semget(SEM_KEY, 1, OBJ_PERMS); // 获取已存在的信号量集标识符
        if(semid == -1) {
            errExit("semget error: ");
        }

        // 等待其他进程调用 semop(): 尝试 MAX_TRIES 次，每秒尝试 1 次
        argW.buf = &dsW;
        argR.buf = &dsR;
        for(j = 0; j < MAX_TRIES; j++) {
            memset(&dsW, 0, sizeof(struct sembuf));
            memset(&dsR, 0, sizeof(struct sembuf));
            if(semctl(semid, 0, IPC_STAT, argW) == -1) {
                errExit("semctl error: ");
            }
            if(semctl(semid, 1, IPC_STAT, argR) == -1) {
                errExit("semctl error: ");
            }

            if(dsW.sem_otime != 0 && dsR.sem_otime != 0) {
                break;
            }
            sleep(1);
        }

        if(dsW.sem_otime == 0 && dsR.sem_otime == 0) {
            errExit("semaphore init error");
        }
    }
    printf("Reader[PID %ld] semaphore init success at %s\n", (long)getpid(), current_time());

    // 获取或创建共享内存
    int shmid = shmget(SHM_KEY, sizeof(struct shmseg), IPC_CREAT | OBJ_PERMS);
    if(shmid == -1) {
        errExit("shmget error: ");
    }
    printf("Reader[PID %ld] create/get shared memory success at %s\n", (long)getpid(), current_time());

    // 共享内存创建成功，附加到该进程的虚拟内存
    struct shmseg *addr = shmat(shmid, nullptr, SHM_RND);
    if(addr == (void *) -1) {
        errExit("shmat error: ");
    }
    printf("Reader[PID %ld] attach shared memory success at %s\n", (long)getpid(), current_time());

    while (true) {
        // 等待写者释放
        if(reserveSem(semid, READ_SEM) == -1) {
            errExit("reserveSem error: ");
        }
        // 写者写入 EOF 退出
        if(addr->cnt == 0) {
            break;
        }
        printf("Reader[PID %ld] read at %s: ", (long)getpid(), current_time());
        ssize_t totalWritten = 0;
        while (totalWritten < addr->cnt) {
            ssize_t written = write(STDOUT_FILENO, addr->buf + totalWritten, addr->cnt - totalWritten);
            if(written == -1) {
                errExit("write error: ");
            }
            totalWritten += written;
        }
        // 释放写信号量
        if(releaseSem(semid, WRITE_SEM) == -1) {
            errExit("releaseSem error: ");
        }
    }

    // 分离共享内存
    if(shmdt(addr) == -1) {
        errExit("shmdt error: ");
    }
    printf("Reader[PID %ld] detach shared memory success at %s\n", (long)getpid(), current_time());
    // 通知写进程我已读取完毕
    if(releaseSem(semid, WRITE_SEM) == -1) {
        errExit("releaseSem error: ");
    }

    return 0;
}
```


