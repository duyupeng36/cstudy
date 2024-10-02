# AVL 树

如果按照排序后的序列插入到 [[二叉查找树]] 中，那么一连串的插入操作将花费二次时间。因为，此时二叉查找树退化为了一个单链表。单链表的插入花费的代价是非常大。

为了不让二叉查找树退化，一个好的解决方案就是让二叉查找树保证某个 **平衡条件** ：任何节点的深度不能过深

## 定义

**AVL 树是带有 _平衡条件_ 的二叉查找树**。这个平衡条件要容易保持，而且必须保证树的深度是 $O(\log N)$


> [!tip] 简单但是严格的平衡条件
> 
> 最简单的平衡条件：所有节点的左右子树具有相同的高度
> 
>+ 如果空子树的高度定义为 $-1$，那么只有具有 $2^k-1$ 个机电的理想平衡树满足这个条件
>  
> 这个平衡条件太严格了，难以使用

> [!tip] 稍微放宽的平衡条件
> 
> 更宽松的平衡条件：每个节点的左子树和右子树的高度差最多为 $1$
> 
> + 空树的高度定义为 $-1$
> 

![[Pasted image 20241001221809.png]]

一棵有 $N$ 个节点的 AVL 树的高度最多为 $1.44 \log(N+1) - 1.328$，实际上的高度只比 $\log N$ 稍微多一些

> [!tip] AVL树的所有操作可以以时间 $O(\log N)$ 执行
> 
> 因此，除去可能的插入外（假设使用懒惰删除），所有的树操作都可以以时间 $O(\log N)$ 执行
> 

**插入操作时，需要更新通向根节点路径上的那些节点的平衡信息**。插入操作的难点就是插入可能会破坏 AVL 树的平衡条件。例如，在上图描述的 AVL 树中插入 $6$，就会破坏节点 $8$ 的平衡条件

![[Pasted image 20241001221953.png]]

## 旋转

**一次插入操作需要包含修正节点平衡条件的操作**。事实上，总是可以通过 **_旋转_** 来修改节点的平衡条件

让我们把必须重新平衡的节点叫做 $\alpha$。由于任意任意节点最多两个儿子，因此高度不平衡时，$\alpha$ 点的两棵子树高度差 $2$。显然，这种不平衡会出现下面四种情况

> [!tip] 插入导致的 $4$ 种不平衡
> 情形一：对 $\alpha$ 的左子节点的左子树进行插入。即 **左-左** 情形
> 
> 情形二：对 $\alpha$ 的左子节点的右子树进行插入。即 **左-右** 情形
> 
> 情形三：对 $\alpha$ 的右子节点的左子树进行插入。即 **右-左** 情形
> 
> 情形四：对 $\alpha$ 的右子节点的右子树进行插入。即 **右-右** 情形
> 
> 显然，情形一和情形四是关于 $\alpha$ 对称的；情形二和情形三是关于 $\alpha$ 对称的。理论上，只有两种情形
> + 情形一和情形二称为插入发生在 _外边_ 的情形
> + 情形二和情形三称为插入发生在 _内部_ 的情形

对于插入发生在外边的情形，只需要对树进行一次 **_单旋转_** 即可完成调整。然而，对于插入在内部的情形，需要通过 **_双旋转_** 才能完成。

### 单旋转

#### 左-左情形

插入发生在失衡结点的左子结点的左子树上，通过一次 **右旋** 就能修复该失衡问题。如下图所示

![[Pasted image 20240728231121.png]]

下面我们来看看你具体是如何操作的。节点 $k_2$ 不满足 AVL 平衡条件，因为它的左子树比右子树深 $2$ 层

该图描述的是：在插入之前 $k_2$ 保持平衡，但是插入之后，子树 $X$ 多出一层，导致 $k_2$ 的左子树要比右子树 $Z$ 深 $2$ 层

> [!tip]
> 
> $X$ 和 $Y$ 不可能在同一层。因为那样在插入前就失去平衡
> 
> $Y$ 和 $Z$ 也不能在同一层，因为那样 $k_1$ 就会是在通向 root 的路径上破坏 AVL 平衡条件的第一个节点
> 

为了使树恢复平衡，将 $X$ 上移一层，并把 $Z$ 下移一层。为了满足 AVL 树的特性要求，重新安排节点，形成一棵等价树。上图右侧展示了调整后的树结构

> [!tip] 左-左情形：右旋转
> 
> $k_1$ 变成新的 root。由于 AVL 树是一棵二叉查找树，所以 $k_2 \gt k_1$，于是新树中，$k_2$ 成了 $k_1$ 的右子树。
> 
> $X$ 和 $Z$ 仍然分别是 $k_1$ 的左子树和 $k_2$ 的右子树
> 
> $Y$ 包含的元素大于 $k_1$ 小于 $k_2$ 之间，因此将其放在 $k_2$ 的左子树
> 

#### 右-右情形

插入发生在失衡结点的右子结点的右子树上，通过一次 **左旋** 就能修复该失衡问题。如下图所示

![[Pasted image 20240728233720.png]]

> [!tip] 右-右情形：左旋转
> $k_2$ 变为新的 root。由于 AVL 树是一棵二叉查找树，所以 $k_2 \gt k_1$，于是新树中，$k_1$ 变成了 $k_2$ 的左子树
> 
> $X$ 和 $Z$ 分别是 $k_1$ 的左子树和 $k_2$ 的右子树
> 
> $Y$ 包含的元素大于 $k_1$ 小于 $k_2$ 之间，因此将其放在 $k_1$ 的右子树
> 

### 双旋转

对于情形二和情形三，单旋转是无效的。原因在与子树 $Y$ 太深了，从先前分析中知道：$Y$ 的深度并没有发生任何改变。为了解决这个问题，需要执行双旋转

#### 左-右情形

插入发生在失衡结点的左子结点的右子树上，通过一次 **左旋-右旋** 就能修复该失衡问题。如下图所示

![[Pasted image 20240729011221.png]]

由于子树 $Y$ 已经有一项插入，这就保证了 $Y$ 非空。因此，可以假设它有一个根和两棵子树

 如上图，子树 $Y$ 被拆分成了 $k_2$ 和其子树 $B,C$。恰好树 $B$ 或者树 $C$ 中有一颗比 $D$ 深  $2$ 层，但是不能确定是哪一棵；事实上，这是无关紧要的，这里我们将 $B$ 和 $C$ 都画成比 $D$ 深 $1\frac{1}{2}$

> [!tip] 左-右情形：先左旋后右旋
> 
> 为了重新平衡，$k_3$ 不能再作为根了，单旋右无法调整。因此，只能让 $k_2$ 做根
> 
> $k_1$ 和 $k_3$ 作为 $k_2$ 的左子树和右子树
> 

#### 右-左情形

插入发生在失衡结点的右子结点的左子树上，通过一次 **右旋-左旋** 就能修复该失衡问题。如下图所示

![[Pasted image 20240729013445.png]]

> [!tip] 右-左情形：先右旋后左旋

### 演示

假设初始的AVL 树开始插入关键字 $3, 2, 1$，然后插入 $4,5,6,7$

插入关键字 $1$ 时，第一个不平衡问题出现了，AVL 特性在根处被破坏

![[Pasted image 20241001223456.png]]

插入 $4$ 没有任何问题，但是插入 $5$ 又会导致 AVL 树在节点 $3$ 处失去平衡

![[Pasted image 20241001223530.png]]

接着插入 $6$ 会导致 AVL 树在节点 $2$ 处失去平衡

![[Pasted image 20241001223558.png]]

接着插入 $7$ 会导致 AVL 树在节点 $5$ 处失去平衡

![[Pasted image 20241001223642.png]]

---

继续插入关键字 $16,15,14,13,12,11,10$，接着插入 $8$，然后在插入 $9$

插入 $16$ 没有任何问题，当插入 $15$ 时会导致 AVL 树在节点 $7$ 处失去平衡

![[Pasted image 20241001223702.png]]

继续插入 $14$，导致 AVL 树在节点 $6$ 处失去平衡

![[Pasted image 20241001223740.png]]

继续插入 $13$，导致 AVL 树在节点 $4$ 处失去平衡

![[Pasted image 20241001223848.png]]

插入 $12$ ，导致 AVL 树在节点 $14$ 处失去平衡

![[Pasted image 20241001223927.png]]

插入 $11$ ，导致 AVL 树在节点 $15$ 失去平衡

![[Pasted image 20241001224004.png]]

继续插入 $10$，导致 AVL 树在节点 $12$ 处失去平衡

![[Pasted image 20241001224033.png]]

插入 $8$ 不会破坏节点的平衡情况。最后插入 $9$ 导致 AVL 树在节点 $10$ 失去平衡

![[Pasted image 20241001224053.png]]

## 实现

为了将关键字 $X$ 插入到一颗 AVL 树 $T$ 中，我们使用递归方法；即，递归的将 $X$ 插入到 AVL 树的相应子树($T_{L, R}$)中，如果该子树的高度不变，那么插入完成。否则，如果在 AVL 树中出现高度不平衡，那么根据 $X$ 以及 $T$ 和 $T_{L, R}$ 中的关键字做适当的单旋转或双旋转，更新这些高度(并解决好与树的其余部分的连接)，从而完成插入

关于高度信息的存储如果处理不好将带来效率问题

> [!tip] 存储高度信息的方案
> 
> 由于真正需要的是左右子树的高度差，应该保证它很小。如果保存左右子树的高度差，可以使用能两个二进制位(代表 `+1, 0, -1`)表示这个差
> 
> 然而，这样会让程序损失简明性。不如直接存储每个节点的高度来的简单。所以，采用存储节点的高度
> 

在每个节点中使用 `height` 保存它高度，其余的和二叉排序树保存一致

```c title:avl.h
#ifndef AVL_H
#define AVL_H

#include <stdio.h>
#include <stdlib.h>

typedef int elem_t;

typedef struct avl_node {
    elem_t data;
    struct avl_node *left;
    struct avl_node *right;
    int height;  // 存储当前树的高度
} avl_node_t;

typedef struct avl_tree {
    avl_node_t *root;
    int size;
} avl_tree_t;

/**
* bst_init 创建一个空的 AVL 树
* @return : 返回一棵空树
*/
avl_tree_t *avl_init(void);

#endif //AVL_H
```

```c title:avl.c
#include "avl.h"

avl_tree_t *avl_init(void) {
    avl_tree_t *tree = malloc(sizeof(avl_tree_t));
    if (!tree) {
        fprintf(stderr, "Error allocating memory for avl tree\n");
        return nullptr;
    }
    tree->root = nullptr;
    tree->size = 0;
    return tree;
}

static int height(avl_node_t *node)
{
	if (!node)
	{
		return  -1;
	}
	return node->height;
}
```

> [!tip]
> 
> 这里需要一个辅助函数 `height` 用于获取当前节点高度。当节点为 `nullptr` 时，返回 `-1`
> 

### 旋转

#### 单旋转

##### 左-左情形(右旋)

失衡结点的左子结点的左子树上插入

```c title:avl.c
static avl_node_t *signalRotateWithLeft(avl_node_t *k2) {
    /*
        K2					K1
        /\					/\
       /  \				   /  \
      K1   Z              X    K2
      /\                  |    /\
     /  \				  |	  /  \
    X    Y					 Y	  Z
    |
    |
    */

    // 调整树
    avl_node_t *k1 = k2->left;
    k2->left = k1->right;
    k1->right = k2;

    // 更新树的高度

    k2->height = max(height(k2->left), height(k2->right)) + 1;
    k1->height = max(height(k1->left), height(k1->right)) + 1;
    return k1;
}
```

##### 右-右情形(左旋)

失衡结点的右子结点的右子树上插入

```c title:avl.c
static avl_node_t *signalRotateWithRight(avl_node_t *k1) {
    /*
        K1						K2
        /\						/\
       /  \					   /  \
      X   K2                  K1   Z
          /\                  /\   |
         /  \                /  \  |
        Y    Z              X   Y
             |
             |
    */

    // 调整树
    avl_node_t *k2 = k1->right;
    k1->right = k2->left;
    k2->left = k1;

    // 更新高度
    k2->height = max(height(k2->left), height(k2->right)) + 1;
    k1->height = max(height(k1->left), height(k1->right)) + 1;
    return k2;
}
```

#### 双旋转


##### 左-右情形(左旋-右旋)

在失衡结点的左子结点的右子树上插入

```c title:avl.c
/**
 * doubleRotateWithLeft 左-右情形(左旋-优选)

		k3					k3      				 K2
		/\					/\					    /  \
	   /  \				   /  \                    /    \
	  k1   D    先左旋      K2   D    再右旋        K1     K3
	  /\   		  		  /\					 / \     / \
	 /  \			     /  \					/   \   /   \
	A	K2	            K1   C                 A     B C    D
		/\				/\
	   /  \		   	   /  \
	  B    C          A    B

	现在 K1 左旋，然后在 k3 右旋

 * @param k3: 失衡节点
 * @return : 返回新的根节点
 */
static avl_node_t * doubleRotateWithLeft(avl_node_t *k3) {
    k3->left = signalRotateWithRight(k3->left);
    k3 = signalRotateWithLeft(k3);
    return k3;
}
```

##### 右-左情形(右旋-左旋)

在失衡结点的右子结点的左子树上插入

```c title:avl.c
/**
 * doubleRotateWithRight 右-左情形(右旋-左旋)

		K1     		   		K1						 K2
	   /  \				   /  \					    /  \
	  /    \ 			  /    \			       /    \
	 A     K3  先右旋  	 A      K2  后左旋        K1      K3
		  /  \    			   /  \              / \     /\
		 /	  \				  /	   \            /   \   /  \
		K2	   D             B		K3         A     B  C   D
	   /  \						   /  \
	  /    \                      /    \
	 B		C                    C     D

 * @param k1: 失衡节点
 * @return : 返回新的根节点
 */
static avl_node_t * doubleRotateWithRight(avl_node_t *k1) {
	k1->right = signalRotateWithLeft(k1->right);
	k1 = signalRotateWithRight(k1);
	return k1;
}

```

### Insert

AVL 树的插入比较和 BST 类似，但是 AVL 树插入后需要检查插入点的是否处于平衡状态

```c
/**
 * insert 辅助 Insert 函数完成插入操作
 * @param e : 待插入的元素
 * @param root : 指向 AVL 树的指针
 * @return : 返回新的根节点指针
 */
static avl_node_t * insert(elem_t e, avl_node_t *root)
{
	if (root == nullptr)
	{
		// 插入点
		if((root = malloc(sizeof(avl_node_t))) == nullptr )
		{
			fprintf(stderr, "Error allocating memory for avl node\n");
			return nullptr;
		}
		root->data = e;
		root->left = nullptr;
		root->right = nullptr;
		root->height = 0;
	}
	else if(e < root->data)
	{
		// e < root：插入在 root 的左子树上
		root->left = insert(e, root->left);
		// 判断是否需要重新平衡: 判断 root 的左子树与右子树高度差
		// 由于插入发生在左子树，所以左子树的高度一定大于右子树
		if(height(root->left) - height(root->right) == 2)
		{
			if(e < root->left->data)
			{
				// e 比 root 的左子结点小，插入在 root 的左子结点的左子树上(左-左情形)
				root = signalRotateWithLeft(root);
			}
			else
			{
				// e 比 root 的左子节点大，插入在 root 的左子结点的右子树上(左-右情形)
				root = doubleRotateWithLeft(root);
			}
		}
	}
	else if (e > root->data)
	{
		// e > root: 插入在 root 的右子树上
		root->right = insert(e, root->right);
		// 判断插入是否导致 root 失去平衡
		if (height(root->right) - height(root->left) == 2)
		{
			if(e > root->right->data)
			{
				// e 比 root 的右子结点大，插入在 root 的右子结点的右子树上(右-右情形)
				root = signalRotateWithRight(root);
			}
			else
			{
				// e 比 root 的右子节点小，插入在 root 的右子节点的左子树(右-左情形)
				root = doubleRotateWithRight(root);
			}
		}
	}
	// 更新 root 的高度
	root->height = max(height(root->left), height(root->right)) + 1;
	return root;
}

bool Insert(elem_t e, avl_tree_t *tree){
	if (!tree)
	{
		fprintf(stderr, "avl tree is null\n");
		return false;
	}
	if((tree->root = insert(e, tree->root)) == nullptr)
	{
		return false;
	}
	tree->size++;
	return true;
}
```

### Delete

如果删除的不多，建议使用懒惰删除。
