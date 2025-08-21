# GMP 调度策略

Go 诞生之初就为多核 CPU 并行而设计。

Go 协程中，非常重要的就是 **协程调度器** `scheduler` 和 **网络轮询器** `netpoller`

> [!tip] Go 协程调度中，有三个重要角色
> + **M：Machine Thread**，即工作线程。这是对系统线程的抽象和封装。所有代码最终都要在系统线程上运行
> + **G：Gorutine**，即 Go 封装的协程。存储量协程的执行栈信息、状态和任务函数。初始栈大小大约在 $2\sim 4$KB 
> + **P：Processor**，代表了一组资源。**M 想要执行 G 的代码必须持有一个 P 才行**

## GM 模型

在 Go 1.1 之前，调度模型中并没有 P，如下图所示

![Pasted image 20250730194606|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755787943526-7ba321d0e3f84214814d47238f1a0ffc.png)


用一个 **全局 Mutex** 保护一个 **全局 runq(就绪队列)**，所有 Groutine 的创建、结束，以及调度等操作都需要先获得锁

> [!tip] 锁的争抢非常严重
> Go 官方的测试，一台CPU利用率约为 70% 的 8 核心服务器上，锁的争抢的消耗占比约为 14%

Groutine 的每次执行都可能会被分配到不同的 M 上，**造成 G 在不同的 M 之间切换**，破坏了程序的局部性，因为只有一个全局 runq。

> [!tip]
> 新创建的 G 会被创建它 M 放入全局 runq，但是会被另一个 M 调度执行，会造成不必要的开销

每个 M 都会关联一个内存分配缓存 mcache，造成了大量的内存开销，进一步使得数据局部性变差

> [!tip] 
> 实际上 **只有执行 Go 代码的 M 才会需要 mcahce**，那些 **阻塞在系统调用中的 M 根本不需要**

在存在系统调用的情况下，工作线程经常被阻塞和解除阻塞，从而增加了很多开销

## GMP 模型

为了解决上述问题，Go 重新设计了调度器，**引入了处理器 P 的概念**，并在 P 上 **实现了工作窃取调度程序**。M 依旧是工作线程，P 表示执行 Go 代码需要的资源

> [!important] 当在 M 执行 G 时，M 就需要一个 P 
> 
> 当一个 M 在执行 Go 代码时，它需要关联一个 P
> 
> 当 M 执行系统调用或空闲时，则不需要 P

下图展示了 GMP 调度模型

![Pasted image 20250730194651|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755787943526-5ed7b21f31924406894d4ba30179897d.png)


**P：Process，虚拟处理器**，可以通过环境变量 `GOMAXPROCS` 或 `runtime.GOMAXPROCS(n)` 设置，**默认为 CPU 核心数**

> [!tip]
> P 的数量决定着最大可并行的 G 的数量
> 
> P 有自己的本地 runq (长度 $256$)，里面放着待执行的 G
>
>M 和 P 需要绑定在一起，这样 P 队列中的 G 才能真正在线程上执行

### 本地runq 和全局 runq 

本地 runq 和全局 runq 使用如下图。当一个 G 从等待状态变为就绪状态时，或者新建一个 G 时，这个 G 会被添加到当前 P 的本地 runq。当 M 执行完一个 G 后，它会先尝试从关联的 P 的本地 runq 中获取下一个 G， **如果本地 runq 为空，则会到全局 runq 中获取一个 G**

![Pasted image 20250730194719|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755787943526-d5989c893ddd4365bb2efe04f3509ed6.png)


如果全局 runq 也为空，则从其他 P 中的本地 runq 窃取一半的 G 到自己的本地 runq

![Pasted image 20250730194743|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755787943526-81b20ee7510541e1b1e0c1fe062afa19.png)

### M 自旋

当 M 进入系统调用时，必须确保有其他的 M 来执行 Go 代码。新的调度器引入来一定程度的 **自旋**，这样就 **避免了频繁的挂起和唤醒M**

> [!tip] 两种类型的自旋
> 
> 第一种自旋：如果 **M 关联了 P**，则 M 自旋 **寻找可执行的 G**
> 
> 第二种自旋：如果 **M 没有关联 P**，则 M 自旋 **寻找可用的 P**

当一个新的 G 被创建或者 M 即将进行系统调用，或者 M 从空闲状态变为忙碌状态时，会确保至少有一个处于自旋状态的 M（除非所有的 P 都忙碌），这样保证了处于可执行状态的 G 都可以得到调度，同时不会频繁的挂起和恢复 M

## GMP 主要数据结构

在 `$GOROOT/src/runtime/runtime2.go` 中提供了下面结构数据结构

#### runtime.g

下面列出的字段只是其中一部分，这些字段都与调度相关

```go
type g struct {
	// stack 描述了 Groutine 的栈空间
	stack       stack  
	// stackguard0 被正常的 Groutine 使用，编译器安插在函数头部的栈增长代码，
	// 用来和 SP 进行比较，按需进行栈增长。值一般是 stack.lo + StackGuard，
	// 也可能是 StackPreempt 用于触发一次抢占
	stackguard0 uintptr
	// stackguard1 类似于 stackguard0，只不过被 g0 和 gsignal 使用
	stackguard1 uintptr 
	
	...
		
	// m 记录关联到正在执行当前 G 的工作线程 M
	m         *m
	// sched 被调度器用来
	sched     gobuf
	
	...
	
	// atomicstatus 当前 G 的状态
	atomicstatus atomic.Uint32
	...
	
	// goid 当前 Groutine 的全局 ID
	goid         uint64
	// schedlink 被调度器用于实现内部链表、队列，guintptr 逻辑上等价 *g，
	// 底层类型是 uintptr，为了避免写屏障
	schedlink    guintptr
	....
	
	// preempt 为 true 时，调度器会在合适的时机触发一次抢占
	preempt       bool 
	
	...
	
	// waiting 用于实现 channel 中的等待队列
	waiting       *sudog         
	
	....
	
	// timer runtime 内部实现的计时器，主要用于支持 time.Sleep()
	timer         *timer
	....
}
```

**stack** 类型是一个结构体，用于描述 Groutine 的栈，对应的内存区间是 `[stack.lo, stack.hi)`

```go
type stack struct {
	lo uintptr
	hi uintptr
}
```

**gobuf** 类型是一个结构体，用于存储 Groutine 执行上下文，与 Groutine 切换的底层实现直接相关

```go
type gobuf struct {
	// sp 栈指针
	sp   uintptr
	// pc 下一条要执行的指令的指针 
	pc   uintptr
	// g 反向关联 G
	g    guintptr
	// ctxt 指向闭包对象。使用 go 关键字创建协程时传递的是一个闭包，
	// 这里会存储闭包对象的地址
	ctxt unsafe.Pointer
	// ret 存储返回值
	ret  uintptr 
	// arm 架构上存储返回值，x86 上没有使用
	lr   uintptr
	// bp 栈基指针
	bp   uintptr
}
```

**atomicstatus** 字段描述了 G 的状态，主要有下面几种取值

```go
const (
	// _Gidle 代表 Groutine 刚被分配，尚未被初始化 
	_Gidle = iota // 0 

	// _Grunnable 代表 Groutine 在某个 runq 中，尚未运行用户代码，
	// 栈不归自己所有
	_Grunnable // 1

	// _Grunning 代表 Groutine 正在被执行，栈归自己所有。
	// 运行中的 Groutine 不在任何 runq 中，并且关联 M 和 P 
	_Grunning // 2

	// _Gsyscall 代表 Groutine 正在执行系统调用，没有执行用户代码。
	// 栈归自己所有，不在任何 runq 中，只关联了 M 
	_Gsyscall // 3

	// _Gwaiting 代表 Groutine 阻塞在 runtime 中，没有执行用户代码。
	// 不再任何 runq 中，而是在某个位置记录，例如 channel 的等待队列。
	// 它的栈不归自己所有
	_Gwaiting // 4

	// _Gmoribund_unused 目前未使用, 但是硬编码在 gdb 脚本中
	_Gmoribund_unused // 5

	// _Gdead 代表 Groutine 没有被用到，可能刚刚退出运行，在一个空闲链表中；
	// 或者刚刚完成初始化。没有执行用户代码，可能分配了栈，也可能没有
	_Gdead // 6

	// _Genqueue_unused 目前未使用
	_Genqueue_unused // 7

	// _Gcopystack 代表 Groutine 正在被移动。没有执行用户代码，也不在 runq 中。
	// 栈的所有权归把当前 Groutine 置为 _Gcopystack 的 Groutine
	_Gcopystack // 8

	// _Gpreempted 代表因为 suspendG 抢占而停止。
	// 和 _Gwaiting 很像，但没有谁将 Groutine 置为就绪状态
	_Gpreempted // 9

	// _Gscan 代表了上述所有状态的组合，除了 _Gscanrunning 以外，
	// 其余的都代表了 GC 正在扫描 Groutine 的栈空间
	_Gscan          = 0x1000
	_Gscanrunnable  = _Gscan + _Grunnable  // 0x1001
	_Gscanrunning   = _Gscan + _Grunning   // 0x1002
	_Gscansyscall   = _Gscan + _Gsyscall   // 0x1003
	_Gscanwaiting   = _Gscan + _Gwaiting   // 0x1004
	_Gscanpreempted = _Gscan + _Gpreempted // 0x1009
)
```


#### runtime.m

下面描述了 `m` 中和调度相关的部分字段

```go
type m struct {
	// g0 并不是一个 groutine，它的栈由内核分配，
	// 初始大小比普通的 Groutine 要大很多，被用作调度执行的栈
	g0      *g 
	
	...

	// gsignal 本质上是用于处理信号的栈
	gsignal       *g
	
	...
	
	// curg 指向的是 M 当前正在执行的 G
	curg          *g       
	...
	
	// p GMP 中得而 P，即关联到当前 M 上的虚拟处理器
	p             puintptr
	// nextp 用来将 P 传递给 M,调度器一般在 M 阻塞时为 m.nextp 赋值，
	// 等 M 开始运行后，会尝试从 nextp 处获取 P 进行关联
	nextp         puintptr
	// oldp 用来暂存执行系统调用之前关联的 P
	oldp          puintptr 
	// id M 的唯一ID
	id            int64
	
	...
	
	// premptoff 不为 "" 时，表示要关闭对 curg 的抢占，它的值给出原因
	preemptoff    string // if != "", keep curg running on this m
	// locks 记录当前 Groutine 持有的锁的数量，不为零时能够阻止抢占的发生
	locks         int32
	
	...
	
	// spinning 代表 M 正在自旋
	spinning      bool 
	// park 用来支持 M 的挂起与唤醒，可以很方便的实现每个 M 的单独挂起和唤醒
	park          note
	// alllink 把所有的 M 连起来，构造 allm 链表
	alllink       *m
	// schedlink 被调度器用于实现链表，如空闲的 M 链表
	schedlink     muintptr
	// lockedg 关联到与当前 M 绑定的 G
	lockedg       guintptr
	
	...
	
	// freelink 用来把已经退出运行的 M 连起来，构成 sched.freem 链表，方便下一次分配
	freelink    *m // on sched.freem
	
	....
}
```

#### runtime.p

下面描述了 `p` 中用于调度的字段

```go

type p struct {
	// id 代表 P 的唯一ID，等于 allp 数组的下表
	id          int32
	// status 代表 P 的状态
	status      uint32 
	// link 没有写屏障的指针，被调度器用来实现链表
	link        puintptr
	// schedtick 记录调度次数。每发生一次 Groutine 切换，
	// 并且不继承时间片的情况下，该字段值 +1
	schedtick   uint32
	// syscalltick 记录发生系统调用的次数
	syscalltick uint32   
	// 被监控线程(sysmon) 用来存储上一次检查时的调度器时钟嘀嗒，
	// 用以实现时间片算法
	sysmontick  sysmontick
	// m 本质上是一个指针，反向关联到当前 P 绑定的 M
	m           muintptr
	
	...
	

	// goidcache 和 goidcacheend 用来从全局 sched.goidgen 处理申请 goid 分配区间，
	// 批量申请以减少全局范围的锁竞争
	goidcache    uint64
	goidcacheend uint64
 
	// runqhead runqtail runq 本地runq，是一个环形队列
	runqhead uint32
	runqtail uint32
	runq     [256]guintptr
	
	// runnext 如果不为nil,则指向被当前 G 准备好的(就绪)的 G，
	// 接下来将会继承当前 G 的时间片开始运行
	runnext guintptr

	// gFree 缓存已经退出运行的 G
	gFree struct {
		gList
		n int32
	}
	
	...
	
	// preempt Go 1.14 引入，用于支持异步抢占
	preempt bool

	...
}
```

**status** 字段有 $5$ 种不同的取值，代表 P 处于不同的状态

```go
const (
	// _Pidle 代表 P 处于空闲状态。没有执行用户代码或调度器代码
	_Pidle = iota

	// _Prunning 代表 P 处于运行中。当前 P 正在被某个 M 持有，
	// 并且用于执行用户代码或者调度器代码
	_Prunning

	// _Psyscall 代表 P 处于系统调用。此时 P 没有执行用户代码，
	// 它和一个处于 syscall 的 M 弱关联，随时会被其他的 M 窃取
	_Psyscall

	// _Pgcstop 代表 P 处于 GC 暂停状态。P被 STW 挂起执行 GC 代码，
	// 执行 STW 的 M 会继续使用处于 _Pgcstop 状态的 P
	_Pgcstop

	// _Pdead 代表 P 处于停用状态。
	_Pdead
)
```

#### runtime.schedt

用于保存调度器全局数据的 sched 变量对应的 scedt 类型，其中的大部分字段都和调度相关

```go

type schedt struct {
	// goidgen 用作全局的 goid 分配器，保证 goid 的唯一性。
	// P 中的 goidcache 就是从这里批量获取的 goid
	goidgen   atomic.Uint64
	// lastpoll 上次执行 netpoll 的时间。
	// 0 代表某个线程正在阻塞式的执行 netpoll
	lastpoll  atomic.Int64 
	// pollUntil 代表阻塞式的 netpoll 何时被唤醒
	pollUntil atomic.Int64 
	
	// lock 全局调度器锁。访问 sched 中的很多字段都需要先获得该锁
	lock mutex

	// midle 空闲的 M 链表的表头
	midle        muintptr
	// nmidle 空闲 M 的数量，链表的长度
	nmidle       int32
	// nmidlelocked 与 G 绑定且处于空闲状态 M 的数量。
	// 绑定的 G 没有在运行，相应的 M 不能用来运行其他的 G，只能挂起
	nmidlelocked int32
	// mnext 记录了共创建多少个 M，同时也被用作下一个 M 的 ID
	mnext        int64 
	// maxmcount 限制允许 M 的个数，除去那些已经释放的
	maxmcount    int32
	// nmsys 系统 M 的个数，这些 M 不再死锁检测范围内
	nmsys        int32
	// nmfreed 累计释放了多少 M
	nmfreed      int64    // cumulative number of freed m's

	// ngsys 系统 Groutine 的数量，会被原子更新
	ngsys atomic.Int32 // number of system goroutines

	// pidle 空闲 P 链表的表头
	pidle        puintptr
	// npidle 空闲 P 的数量
	npidle       atomic.Int32
	// nmspinning 自旋状态的 M 的数量
	nmspinning   atomic.Int32
	// needspinning 
	needspinning atomic.Uint32
	
	// runq 全局 runq
	runq     gQueue
	// runqsize 全局 runq 中的 Groutine 数量
	runqsize int32

	// disable 用于禁止调度用户 Groutine；系统 Groutine 不受影响
	disable struct {
		// user 被赋值为 true 后将不再调度执行用户 Groutine
		user     bool
		// runnable 存储禁止期间就的 Groutine
		runnable gQueue
		// n 代表 runnalbe 的长度
		n        int32
	}

	// gFree 缓存已退出的 Groutine
	gFree struct {
		lock    mutex
		stack   gList // Gs with stacks
		noStack gList // Gs without stacks
		n       int32
	}


	sudoglock  mutex
	sudogcache *sudog

	
	deferlock mutex
	deferpool *_defer

	// freem 一组已经结束运行的 M 的链表。该链表中的内容会在分配新的 M 时复用
	freem *m
	
	// gcwaiting 表示正在等待 GC 运行
	gcwaiting  atomic.Bool 
	stopwait   int32 // 需要停止 P 的数量
	stopnote   note // 睡眠位置
	
	// sysmonwait 不为 0 代表 sysmon 正在 sysmonnote 上睡眠
	sysmonwait atomic.Bool
	sysmonnote note

	// safePointFn 是一个 Function value。
	// safePointWait 和 safePointNote 被 runtime.forEachP 
	// 用来确保每个 P 在下一个 GC 安全执行了 safeointFn
	safePointFn   func(*p)
	safePointWait int32
	safePointNote note

	profilehz int32 // 性能分析采样频率

	procresizetime int64 // 上次改变 gomaxprocs 花费的时间，纳秒
	totaltime      int64 // 

	// sysmonlock 监控线程 sysmon 访问 runtime 数据时会加上的锁，
	// 其他线程也可以通过它和 sysmon 同步
	sysmonlock mutex
}
```

在 Go 中，线程是运行 Goroutine 的实体，调度器的功能是把可运行的 Goroutine 分配到工作线程上

![Pasted image 20240414220513|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755787943526-5f64c249e3464e94ae53d3a9c5bb1be8.png)

**全局 runq（Global Runnable Queue）**：存放等待运行的 G

**P 的本地 runq(Local Runnable Queue)**：同全局队列类似，存放的也是等待运行的 G，存的数量有限，不超过 $256$ 个。新建 G 时，G 优先加入到 P 的本地队列，**如果队列满了，则会把本地队列中一半的 G 移动到全局队列**

**P 列表**：所有的 P 都在程序启动时创建，并保存在数组中，最多有 `GOMAXPROCS`(可配置) 个。

**M**：线程想运行任务就得获取 P，从 P 的本地队列获取 G，P 队列为空时，M 也会尝试从全局队列拿一部分 G 放到 P 的本地队列，或从其他 P 的本地队列偷一半放到自己 P 的本地队列。M 运行 G，G 执行之后，M 会从 P 获取下一个 G，不断重复下去

Goroutine 调度器和 OS 调度器是通过 M 结合起来的，**每个 M 都代表了 1 个内核线程**，OS 调度器负责把内核线程分配到 CPU 的核上执行。

## 调度流程

### 调度器初始化

Go 程序经过编译后，生成的是系统原生的可执行文件。可执行文件一般会有一个 **执行入口**，也就是 **被加载到内存后开始执行的第一条指令的地址**

> [!tip] 
> 
> 执行入口在不同的平台上不尽相同
> 

下图展示了一个可执行文件加载到内存后的虚拟内存布局

![Pasted image 20250730194906|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755787943527-0169e5b491f2400d8168e595b4a56655.png)

在 `amd64 + linux` 平台上，使用 `-buildmode = exe` 模式编译出来的可执行文件对外暴露的执行入口是 `_rt0_amd64_linux`，对应 `runtime` 源码中的汇编函数 `_rt0_amd64_linux()`，该函数只有一条指令 `JMP`，用于跳转到会变函数 `_rt0_amd64()`

> [!tip] `_rt0_amd64_linux()` 函数在 `$GOROOT/src/runtime/rt0_amd64_linux.s`
> 

```go
#include "textflag.h"

TEXT _rt0_amd64_linux(SB),NOSPLIT,$-8
	JMP	_rt0_amd64(SB)

TEXT _rt0_amd64_linux_lib(SB),NOSPLIT,$0
	JMP	_rt0_amd64_lib(SB)
```

汇编函数 `_rt0_amd64()` 在 `$GOROOT/src/runtime/asm_amd64.s` 中，它只有 $3$ 条指令，用于立刻调用 `runtime.rt0_go()` 函数。`rt0_go()` 函数也是一个汇编函数，该函数包含了 Go 程序启动的大致流程

```go
TEXT _rt0_amd64(SB),NOSPLIT,$-8
	MOVQ	0(SP), DI	// argc
	LEAQ	8(SP), SI	// argv
	JMP	runtime·rt0_go(SB)
```

`runtime·rt0_go()` 也在 `$GOROOT/src/runtime/asm_amd64.s` 中，它的主要逻辑有

**(1)**： **初始化 `g0` 的栈区间**，检测 CPU 厂商及型号，按需调用 `_cgo_init()` 函数，设置和检测 TLS，**将 `m0` 和 `g0` 相互关联**，并将 `g0` 设置到 TLS 中。下图描述了这几个执行之后的内存布局

![Pasted image 20250730194950|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755787943527-a73fe1b0daba4edbae170605b69e01e1.png)

**(2)** ：调用 `runtime.args()` 函数暂存命令行参数，以便于后续解析。部分系统可能在此处获取与硬件相关的参数，必然物理页大小

**(3)** ：调用 `runtime.osinit()` 函数，所有系统都会在这里获取 CPU 核心数，如果上一步没有成功获取的与硬件有关参数，在此处会重新获取

**(4)** ：调用 `runtime.schedinit()` 函数，这个函数会初始化调度系统。该函数通过调用其他函数做了很多事情
1. `moduledataverify()`：检验程序的各个模块。校验各个模块的符号、ABI等，确保模块间一致
2. `stackinit()`：初始化栈分配。Groutine 的栈是动态分配，动态增长的，这一步会初始化用于栈分配的全局缓冲池，以及相关锁
3. `mallocinit()`：初始化堆分配
4. `fastrandinit()` 初始化 `fastrandseed`，接下来的 `mcommoninit()` 函数用到
5. `mcommoninit()` 为当前工作线程分配 ID、初始化 `gsignal`，并把 M 添加到 allm 链表中
6. `cpuinit()` 进行与当前 CPU 相关的初始化工作，检测 CPU 是否支持某些指令集，以及根据 GODEBUG 环境变量来启用或禁用某些硬件特性
7. `alginit()` 根据 CPU 对 AES 相关指令的支持情况，选择不同的 Hash 算法
8. `modulesinit()` 根据所有的加载模块，构造一个活跃模块切片 `moduleSlice`，并初始化 GC 需要的 Mask 数据
9. `typelinksinit()` 基于活跃模块列表构建模块级别的 `typemap`，实现全局范围内对类型元数据进行去重
10. `itabsinit()` 遍历活跃模块列表，将编译阶段生成的所有 `itab` 添加到 `itabTable` 中
11. `goargs()` 解析命令行参数，程序通过 `os.Args` 得到的参数是在这里初始化的
12. `goenvs()` 解析环境变量，程序通过 `os.Getenv()` 获取的环境变量是在这里初始化
13. `parsedebugvars()` 解析环境变量 GODEBUG，为 runtime 各个调试参数赋值
14. `gcinit()` 初始化与 GC 相关的参数，根据环境变量 GOGC 设置 gcpercent
15. `procresize()` 根据 CPU 的核心数或环境变量 GOMAXPROCS 确定 P 的数量，调用 procresize 进行调整

**直到 `runtime.schedinit()` 函数执行完，P 都已经初始化完毕**，此时还没有创建任何 Groutine，所有的 P 的本地 runq 都是空的。根据 `procresize()` 函数的逻辑，函数返回后当前线程会和第 $1$ 个 P 关联，即 `allp[0]`

> [!tip] runtime.schedinit() 函数执行完毕后，所有的 P 都已经创建完毕
> 当前没有任何 Groutine，此时 m0 正在执行 g0

接下来 `runtime.newproc()` 会创建第 $1$ 个 Groutine，并把它放到 P 的本地 runq 中

![Pasted image 20250730195023|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755787943527-a76c579b34674be5b1aa2a08b169da59.png)


**(5)**：调用 `runtime.newproc()` 函数，创建 **主 Goroutine**，指定的入口函数是 `runtime.main()` 函数

> [!tip] 
> 
> 这是程序启动后低第一个真正的 Goroutine
> 

![Pasted image 20250730195101|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755787943527-7ea114a2cc884c17a9c9cd314b7a0617.png)


**(6)** ：调用功能 `runtime.mstart()` 函数，当前线程进入 **调度循环**

> [!tip] 
> 一般情况下，线程调用 `mstart()` 函数进入调度循环后不再返回。进入调度循环的线程会执行上一步创建的 goroutine

![Pasted image 20250730195150|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755787943527-accb2c410af4491682f1140ecf588a83.png)

> [!tip] 
> 主 Groutine 得到执行后，`runtime.main()` 会做很多事情
> 1. 设置最大栈指针
> 2. 启动监控线程(`sysmon`)
> 3. 初始化 `runtime` 包
> 4. 开启 GC
> 5. 最后初始化 `main` 包，并调用 `main.main()` 函数
> 
> `main.main()` 就是用户代码了，整个初始化到此结束。**当 `main.main()` 返回后会调用 `exit(0)` 函数结束进程**

> [!faq] `runtime.main()` 在 `$GOROOT/src/rutime/proc.go` 中

> [!tip] 

### G 的创建与退出

函数 `runtime.newproc()` 用于创建一个新的协程。我们通过下面的例子开始分析

```go
func hello(name string) {
	fmt.Println("hello, ", name)
}

func main() {
	name := "Groutine"
	go hello(name)
}
```

我们已经了解到 Go 程序是如何启动并执行到 `main.main()`。这里 `main.main()` 的执行会通过 `go` 关键字创建一个协程，记为 `hello G`。`go` 关键字会被编译器转换为 `runtime.newproc()` 函数的调用

```go
func newproc(siz int32, fn *funcval) {
	argp := add(unsafe.Pointer(&fn), sys.PtrSize)
	gp := getg()  // gp: main G 的指针
	pc := getcallerpc() // pc: call newproc 指令后的返回地址
	systemstack(func() {
		newg := newproc1(fn, argp, siz, gp, pc) // 协程入口地址, 参数地址, 参数大小, 父协程, 返回地址(创建完协程后要返回到哪里)

		_p_ := getg().m.p.ptr()
		runqput(_p_, newg, true)

		if mainStarted {
			wakep()
		}
	})
}
```

我们从 `main.main()` 函数执行绘制创建 `hello G` 的 `runtime.newproc()` 函数调用栈的变化情况

![Pasted image 20250730195305|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755787943527-d1e5a2e75176465e8f3e3dc8cfe36e9a.png)

**(1)** argp 指针指向的位置在栈上位于参数 `fn` 之后，就像是 `newproc()` 的第三个参数一样。从 argp 开始 `siz` 字节的数据实际上是 `fn()` 函数的参数，被编译器追加在了栈上参数 `fn` 之后

> [!tip] 从 `newproc()` 函数原型来看，这些 **追加参数是不可见的**
> 
> 由于追加参数不可见，因此 `newproc()` 函数必须是 `nosplit`，避免移动栈时丢失这些追加的参数

**(2)** 通过 `getcallerpc()` 函数获取的是 `newproc()` 函数的返回地址，主要被新 Groutine 用于记录自己在哪里创建的

**(3)** 实际上真正的创建工作是在 `runtime.newproc1()` 中完成的，该函数比较复杂，可能会导致栈增长，同时又有 nosplit 限制，所以通过 `systemstack()` 切换到 g0 栈执行

> [!tip] g0 栈是分配在线程栈上的，栈空间比较大

> [!tip] `runtime.newproc()` 函数主要就是切换到 g0 栈，然后调用 `runtime.newproc1()` 函数

**(4)** 新建的 `newg` 通过 `runqput()` 函数放置在当前 P 的本地队列中。`mainStarted` 表示 `runtime.main()` 函数，即，主 Groutine 已经开执行，此后才会通过 `wakep()` 启动一个全新的工作线程，以保证 `main()` 函数总会被主线程调度执行

`runtime.newproc1(fn, argp, siz, gp, pc)` 函数主要逻辑入选
1. 分配新的 g，先尝试 `gfget()` 从空闲链表中获取一个 g。如果没有，则用 `malg()` 分配新的 g
2. 计算栈上需要空间的大小，用参数大小+额外预留空间，还有经过对齐
3. 根据计算结果去定 SP 的位置，把参数复制到新 g 的栈上，需要写屏障
4. 初始化执行上下文，使用 `gostartcallfn()` 
5. 将 G 的状态设置为 `_Grunnable`，并根据当前 P 的 goidcache 分配 G 的 ID

> [!tip] 新 Groutine 执行上下文的初始化比较关键
> 
> 初始化后的 g 会被放入 P 的本地 runq 中，等待接下来的调度。切换到新的 Groutine 执行会用到 `runtime.gogo()` 函数，也就是基于 `g.sched` 的 `gobuf` 来恢复执行现场

新的 Groutine 在初始化时需要在 `g.sched` 中模拟出一个执行现场。下面是实现了这个现场模拟的关键代码

```go
newg.sched.sp = sp
newg.stktopsp = sp
newg.sched.pc = abi.FuncPCABI0(goexit) + sys.PCQuantum
newg.sched.g = guintptr(unsafe.Pointer(newg))
gostartcallfn(&newg.sched, fn)
```

下图展示了 `runtime.newproc1()`函数模拟出来的执行现场

![Pasted image 20250730195411|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755787943528-a619f54e19094c8ebd35d0d28095683d.png)

`seched.sp` 就是从栈底留出参数和额外空间后的位置。`seched.pc` 这是 `runtime.goexit()` 函数起始地址加上 $1$ 字节。这样初始化 pc 是为了让调用栈看起来向是起始于 `runtime.goexit()` ，然后 `goexit()` 调用了 `fn()` 函数，也就是 `hello()` 函数。

> [!tip] 这样，函数 `fn()` 执行完毕之后，会返回到 `goexit()` 中，`goexit()` 中实现了 Groutine 推出后的清理操作

`seched.pc` 的值设置为 `&goexit + 1`  就是为了看起来是 `goexit()` 函数调用了 `fn()` 函数。在 `goexit()` 函数的首尾都插入了一条 `NOP` 指令，所以 `&goexit + 1` 后并不会有任何问题

```go
TEXT runtime·goexit(SB),NOSPLIT|TOPFRAME|NOFRAME,$0-0
	BYTE	$0x90	// NOP
	CALL	runtime·goexit1(SB)	// does not return
	// traceback from goexit1 must hit code range of goexit
	BYTE	$0x90	// NOP
```

下面我们来看 `gostartcall()` 函数中的实现逻辑

```go
func gostartcall(buf *gobuf, fn, ctxt unsafe.Pointer) {
	sp := buf.sp
	sp -= goarch.PtrSize
	*(*uintptr)(unsafe.Pointer(sp)) = buf.pc
	buf.sp = sp
	buf.pc = uintptr(fn)
	buf.ctxt = ctxt
}
```

`gostartcall()` 函数的主要逻辑就是：先把 SP 向下移动一个指针大小，然后把 PC 的值写入 SP 指向的内存。这就相当于在栈上压入了一个新的栈帧，原 PC 成为返回地址。最后更新 gobuf 的 sp 和 pc 字段，新的 pc 字段就是 `fn`

> [!tip] 
> 最终构造的执行现场就像是 `goexit()` 函数刚刚调用 `fn()` 函数，**刚刚完成跳转还没来得及执行 _fn()_ 函数的指令**

到此，新的 Groutine 就创建完毕，新 Groutine 已经被放到来 P 的本地 runq 中，会在后续调度循环中得到执行。`hello G` 创建完毕之后的 GMP 模型如下

![Pasted image 20250730195450|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755787943528-b8e7d001a44a4cbe8e2fd304a8642570.png)

> [!tip] 
> `main G` 在   `main.main()` 函数返回后会在 `runtime.main()` 中调用 `exit()` 函数结束进程
> 
> 上述示例的 `hello G` 还没有来得及调度执行，整个进程就退出了

Groutine 的退出就比较简单了，`runtime.goexit1()` 只是通过 `mcall()` 调用了 `goexit0()` 函数。因为当前 Groutine 即将退出，不能继续执行，必须切换至系统栈完成收尾工作。`goexit0()` 才真正收尾的函数，该函数的主要逻辑为
1. 将 g 置为 `_Gdead`
2. g 的一些字段需要做清零处理
3. 通过 `dropg()` 将 g 和 m 解绑
4. 调用 `gfput()` 将 g 放入空闲队列
5. 调用 `sechedule()` 函数，调度执行其他已经就绪的 Groutine

> [!tip] 
> 最后一步调用 `runtime.sechedule()` 函数其实就是调度循环中的一次循环。工作现场不断调用 `sechedule()` 函数来执行下一个 Groutine

### 调度循环

#### runtime.schedule()

工作线程通过调用 `runtime.schedule()` 函数 **进行一次调度**，该函数就是调度逻辑的主要实现。主要逻辑如下图

![Pasted image 20250730195520|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755787943529-ebf0f06e10374855a87036bb6d812d05.png)

这里没有发现任务窃取等逻辑，这些逻辑在 `runtime.findrunnable()` 函数中

#### runtime.findrunnable()

`findrunnable()` 函数的逻辑可分为前后两部分，前半部部分完成 `timer` 触发、`netpoll` 和 任务窃取；后半部分针对没有找到任务的情况，会处理 GC 后台标记任务、按需执行 `netpoll`，实在没有任务就挂起等待。`findrunnable()` 函数逻辑如下图

![Pasted image 20250730195543|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755787943529-683dedd753344715bc89b08d4dc3b754.png)

现在我们知道了 Groutine 是如何调度的。现在存在一个问题？**当 Groutine 开始执行用户代码，执行流如何再回到 `runtime` 中去调用 `sechedule()` 呢？**

### 抢占式调度

在 Go1.13 中的抢占依赖于 Groutine 检测到 `stackPreempt` 标记而自动让出 CPU，`stackPreempt` 是一个很大的值，栈增长不可能这么多，Groutine 检测到这个标记，转而执行 `sechedule()`

> [!attention] 缺陷
> 
> 一个空的 `for` 就能让程序挂起

在 Go1.14 引入了 **异步抢占**。类似于空 `for` 的代码也会被抢占，和内核线程调度非常类似。异步抢占是由 `runtime.preemptM()` 函数完成。该函数的主要逻辑就是：通过 `signalM()` 函数向指定的 M 发送 `sigPreempt` 信号。当 M 接收到信号后，就会调用 `runtime.sighandler()` 函数，如果接收到的信号是 `sigPreempt`，就会调用 `doSigPreempt()` 函数，该函数就会完成异步抢占

### 监控线程

监控线程(`sysmon`)是在 `runtime.main()` (主 Groutine) 中创建的。监控线程与 GMP 中的工作线程完全不同，`sysmon` 不需要 P，也不由 GMP 调度模型调度。它会重复执行一系列任务，只不过会视情况调整自己的休眠时间

> [!tip] 监控线程的主要任务
> + 按需执行 timer 和 netpoll
> + 抢占 G 和 P：对于执行时间超过阈值(`10ms`) 的 G，`sysmon` 会抢占 G；同时会抢占处于系统调用的 P
> + 强制执行 GC
> 
> sysmon 线程的主要任务就是保障计时器正常执行，执行 netpoll，抢占长时间运行或者处于系统调用的 P

## 协程的调度过程

下图展示了一个 GMP 调度的基本过程

![Pasted image 20240601160434|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755787943530-c5d16a03d9a44af1a092cea1ddf3794c.png)

**第一步，创建`Goroutine`** ：使用 `go func` 创建一个 `Goroutine G1`

**第二步， 将 `Goroutine` 放入局部队列**： 当前 `Process` 为 `P1`，将 `G1` 加入当前 `Process` 的本地队列 `LRQ(Local Run Queue)`
- 如果 `LRQ` 满了，就加入到 `GRQ(Global Run Queue)`

**第三步，将 `P1` 与 `M1`，并尝试从 `P1` 的 LRQ 获取 `G`**
- 如果 `LRQ` 中没有，就从 `GRQ` 中请求一个 `G` 
- 如果 `GRQ` 还没有，就随机从别的 `P` 的 `LRQ` 中 **偷（work stealing）** 一部分 `Gs` 到本地 `LRQ` 中

**第四步，`M1` 调度 `G`** 

**第五步，在 `M1` 上执行 `G` 中的 `G.func()` 函数**
- **`G` 正常执行完了（函数调用完成了）**，`G` 和 `M1` 解绑，执行第三步的获取下一个可执行的 `G`
- **`G` 中代码主动让出控制权**，`G` 和 `M1` 解绑，将 `G` 加入到 `GRQ` 中，执行第三步的获取下一个可执行的 `G`
- **`G` 中进行 `channel`、互斥锁等操作进入阻塞态**，`G` 和 `M1` 解绑，执行第三步的获取下一个可执行的`G`
	- 如果阻塞态的 `G` 被其他协程 `G` 唤醒后，就尝试加入到唤醒者的 `LRQ` 中，如果 `LRQ` 满了，就连同 `G` 和 `LRQ` 中一半转移到 `GRQ`中
- **如果 `G` 发生系统调用**，程序控制给到了内核
	- **_同步系统调用_** 时，执行如下
		- 如果遇到了同步阻塞系统调用，**`G` 阻塞，`M1` 也被阻塞了**，`M1` 和 `P` 解绑
		- 从休眠线程队列中获取一个空闲线程，和 `P` 绑定，并从 `P` 队列中获取下一个可执行的 `G` 来执行；如果休眠队列中无空闲线程，就创建一个 `M` 线程提供给 `P`
		- 如果 `M1` 阻塞结束，需要和一个空闲的 `P` 绑定，优先和原来的 `P` 绑定。如果没有空闲的 `P`，`G` 会放到 `GRQ` 中，`M1` 加入到休眠线程队列中
	- **_异步网络IO调用_** 时，如下
		![Pasted image 20240601195747|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755787943532-a17d40d552d5460bbec4acdebcaf3d5a.png)
	  
		- **网络 IO** 代码会被 Go 在底层变成 **非阻塞 IO**，这样就可以使用 **IO 多路复用** 了
		- `M1` 执行 `G`，执行过程中发生了非阻塞 IO 调用（读/写）时，`G` 和 `M1` 解绑，`G` 会被网络轮询器 `Netpoller` 接手。`M1` 再从 `P` 的 `LRQ` 中获取下一个 `G` 执行。注意，`M1` 和 `P` 不解绑
		- `G` 等待的 IO 就绪后，`G` 从网络轮询器移回 `P` 的 `LRQ`（本地运行队列）或全局 `GRQ` 中，重新进入可执行状态。就大致相当于网络轮询器 Netpoller 内部就是使用了 IO 多路复用和非阻塞 IO，类似 `select` 的循环。GO 对不同操作系统 `MAC(kqueue)`、`Linux(epoll)`、`Windows(iocp)` 提供了支持

