# 垃圾回收

Python 作为动态类型程序设计语言，变量无须事先声明，也不需要指定类型。当变量被赋值的那一刻，它就指向了一个 **对象**，此时变量的类型才被确定。该 **对象** 会被 Python 解释器 **创建** 在进程的虚拟内存空间中的 **堆区** 中

> [!note] 
> 
> Python 中的 **变量** 是一个标识符，指代的是进程的虚拟内存空间的 **栈区** 空间，该空间 **存放了指向对象的内存地址**
> 

Python 程序员中一般 **无须关心变量的存亡**，一般也 **不用关心内存的管理**。因为，Python 的 **引用计数** 机制记录所有对象的引用数。当对象引用数变为 $0$ 时，该对象不可被访问，它就变成了垃圾，可以被 **垃圾回收GC**

内存是宝贵的，因为它快。但再好的硬件资源，再多的机器，在高并发面前都嫌少。内存一定要合理利用。

> [!tip] **产生了垃圾并不是立即就被回收了**
> 
> 数据搬到内存中不易，在内存中的数据，即使是垃圾也不要毫无顾忌的回收，这非常没有效率。因为程序在后期可能还会利用到相同的数据，当程序要使用时，就将该垃圾内存中的数据直接拿来使用，不同重新分配内存或从磁盘重加载数据
> 

## 变量和对象的存储区域

Python 在定义变量时，变量与变量值(对象)都是需要存储的，分别对应内存中的两块区域：**堆区** 与 **栈区**。

下图展示了一个在 `Linux/x86_32` 中进程的内存结构

![[Pasted image 20241013130418.png]]

> [!tip] 
> 
> Python 将所以有的 **对象都存储在了堆区**，位于 **栈区的变量存储的是对象的内存地址**
> 

例如，如下代码

```python
x = 10
y = 20
```

Python 解释器执行后，解释器进程中产生如下所示的内存结构

![[Pasted image 20250113150850.png]]

此时，如果 Python 解释器执行如下语句

```python
x = y
```

![[Pasted image 20250113150916.png]]

很明显，变量 `x` 之前引用的对象已经不可被访问了。$10$ 这个对象所占用的内存就应该被释放出来供其他对象使用

## 直接引用和间接引用

如果一个对象时可访问的，即该对象可以通过 **直接引用** 或 **间接引用** 访问
- **直接引用** 指的是 **从栈区出发直接就能找到对象的内存地址**
- ​ **间接引用** 指的是 **从栈区出发引用到堆区后，再通过进一步引用才能到达的内存地址**

下图展示了直接引用和间接引用的区别

![[Pasted image 20250113151038.png]]

## 引用计数

Python中的每一个对象其实就是 **`PyObject` 结构体**，它的内部有一个名为 **`ob_refcnt`  的引用计数器** 成员变量。程序在运行的过程中 `ob_refcnt` 的值会被更新并以此来反映引用有多少个变量引用到该对象。当对象的引用计数值为 $0$ 时，它的内存就会被释放掉。

```c
typedef struct _object {
    _PyObject_HEAD_EXTRA
    Py_ssize_t ob_refcnt;
    struct _typeobject *ob_type;
} PyObject;
```

> [!tip] 
> 
> 一旦一个对象的引用计数变为 $0$，这个对象就无法被访问，此时，其占用的内存就应该被解释器的垃圾回收机制回收
> 

### 查看对象的引用计数

在 `sys` 模块中，函数 `getrefcount()` 可以用于获取对象的引用计数

```python
import sys

x = []

print(sys.getrefcount(x))  # 2
print(sys.getrefcount([]))  # 1 每一次定义都会创建一个新的对象

y = x
print(sys.getrefcount(x), sys.getrefcount(y))  # 3 3

x = 5  # 注意字面值常量
print(sys.getrefcount(x))  # 无法确定，解释器也可能使用了该字面值常量 
print(sys.getrefcount(y))  # 2

z = 5
print(sys.getrefcount(x))
```

## 标记清除

引用计数机制存在一个的致命弱点：对于 **循环引用** 引用的对象，其引用计数始终不会减少到 $0$，从而导致垃圾堆积

例如，下面定义两个列表 `li1` 和 `li2`，然后把两个列表相互添加到对方中

```python
>>> l1=['xxx']
>>> l2=['yyy']
>>> l1.append(l2)
>>> l2.append(l1)
>>> l1
['xxx', ['yyy', [...]]]
>>> l2
['yyy', ['xxx', [...]]]
```

下图展示了循环引用情形：`l1` 持有 `l2` 的对象，`l2` 持有 `l1` 的对象

![[Pasted image 20250113151916.png]]

> [!tip] 
> 
> 当代码变得很复杂的时候，就可能出现循环引用的问题
> 

现在我们删除 `l1` 和 `l2` 的直接引用。到此，我们无法再通过 变量名引用它们。而它们的引用计数始终不会减小到 $0$

```python
>>> del l1
>>> del l2
```

![[Pasted image 20250113152109.png]]

> [!attention] 
> 
> `l1` 和 `l2` 除了 **相互引用** 外，不在被其他变量或对象引用，因此它们所在内存不在属于可访问的，应该被回收。但是，因为它们相互引用，**引用计数不能减小到 $0$**，因此它们将永久占用内存，导致 **内存泄漏**
> 

Python 引入了 **“标记-清除”** 解决引用计数的循环引用问题。这样在 **启用 “标记-清除” 算法** 时，从栈区出发，没有任何一条直接或间接引用可以访达 `l1` 与 `l2`，即 `l1` 与 `l2` 成了非存活对象，于是 `l1` 与 `l2` 都没有被标记为存活，二者会被清理掉，这样就解决了循环引用带来的内存泄漏问题

> [!tip]
> 
> **容器对象（比如：`list`，`set`，`dict`，`class`，`instance`）都可以包含对其他对象的引用，所以都可能产生循环引用**。而 “标记-清除” 就是为了解决循环引用的问题。
> 


 标记-清除算法应当在 **应用程序可用的内存空间将要被耗尽的时开启**。因为，该算法会导致程序 **STW(Stop The World)**，然后进行两项工作，第一项则是 **标记**，第二项则是 **清除**
- **标记**：标记的过程其实就是，遍历所有的 `GC Roots` 对象(**栈区中的所有内容** 或者线程都可以作为 `GC Roots` 对象），然后将所有 `GC Roots` 的对象可以直接或间接访问到的对象 **标记为存活的对象**，其余的均为 **非存活对象**。对于非存活应该被清除。
- **清除**: 清除的过程将遍历堆中所有的对象，将没有标记的对象全部清除掉

> [!success] 标记-清除算法的实现
> CPython 底层维护了 _两个_ **双端链表**，一个链表存放着 **需要被扫描的容器对象**（姑且称之为链表A），另一个链表存放着 **临时不可达对象**（姑且称之为链表B）
> + 为了实现“标记-清理”算法，链表中的每个节点除了有记录当前引用计数的`ref_count`变量外，还有一个 `gc_ref` 变量，这个 `gc_ref` 是 `ref_count` 的一个副本，所以初始值为 `ref_count` 的大小
> + 执行垃圾回收时，首先遍历链表A中的节点，并且将当前对象所引用的所有对象的 `gc_ref` 减 `1`，这一步主要作用是解除循环引用对引用计数的影响
> + 再次遍历链表`A` 中的节点，如果节点的`gc_ref`值为`0`，那么这个对象就被标记为“暂时不可达”（`GC_TENTATIVELY_UNREACHABLE`）并被移动到链表`B` 中
> 	+ 如果节点的`gc_ref`不为`0`，那么这个对象就会被标记为“可达“（`GC_REACHABLE`），对于”可达“对象，还要递归的将该节点可以到达的节点标记为”可达“
> 	+ 链表`B`中被标记为”可达“的节点要重新放回到链表A中
> + 在两次遍历之后，链表`B`中的节点就是需要释放内存的节点
> 

## 分代回收

在循环引用对象的回收中，**整个应用程序会被暂停**，然后 **遍历需要被扫描的容器对象链表，这是非常消耗时间的**，为了减少应用程序暂停的时间，Python 通过 **分代回收**（空间换时间）的方法提高垃圾回收效率

**分代** 指的是根据存活时间来为变量划分不同等级（也就是不同的代）
- 新定义的变量，放到 **新生代** 这个等级中，假设每隔 $1$ 分钟扫描新生代一次，如果发现变量依然被引用，那么该对象的权重（权重本质就是个整数）加一，当变量的权重大于某个设定得值（假设为$3$），会将它移动到更高一级的青春代
- **青春代** 的 `gc` 扫描的频率低于新生代（扫描时间间隔更长），假设 $5$ 分钟扫描青春代一次，这样每次 `gc` 需要扫描的变量的总个数就变少了，节省了扫描的总时间，
- 接下来，青春代中的对象，也会以同样的方式被移动到 **老年代** 中。也就是等级（代）越高，被垃圾回收机制扫描的频率越低

![[Pasted image 20240401205035.png]]

> [!tip] 
> 
> **分代回收** 依然是使用引用计数作为回收的依据
> 

虽然分代回收可以起到提升效率的效果，但也存在一定的缺点。例如一个变量刚刚从新生代移入青春代，该变量的绑定关系就解除了，该变量应该被回收，但青春代的扫描频率低于新生代，这就到导致了应该被回收的垃圾没有得到及时地清理

## GC 模块

gc 模块提供了垃圾回收器的接口。功能包括：**关闭收集器**、**调整收集频率**、**设置调试选项**。它同时 **提供对回收器找到但是无法释放的不可达对象的访问**

由于 Python 使用了带有引用计数的回收器，**如果你确定你的程序不会产生循环引用，你可以关闭回收器**，调用 `gc.disable()` 关闭自动垃圾回收

若要调试一个存在内存泄漏的程序，调用 `gc.set_debug(gc.DEBUG_LEAK)` ；需要注意的是，它包含 `gc.DEBUG_SAVEALL` ，使得被垃圾回收的对象会被存放在 `gc.garbage` 中以待检查。

> [!question] 下面的输出的 `id` 为什么会是一样的？
> 
> ```python
> >>> id([1,2,3]), id([1,2,3,4])
(2833029612736, 2833029612736)
> ```
> 
> 因为 `[1, 2, 3]`  是一个临时对象。`id()` 函数返回后，`[1, 2, 3]` 对象的引用计数变为 $0$，它的内存被标记为垃圾，很快就被另一个对象 `[1, 2, 3, 4]` 覆盖
> 

