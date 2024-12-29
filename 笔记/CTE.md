# CTE：公用表表达式

## 公用表表达式
**公用表表达式 (Common Table Expression，CTE)** 允许您 **在查询中创建临时结果集**。CTE 通过将复杂查询分解为 **更小** **更可重用** 的部分来帮助您 **增强复杂查询的可读性**

以下是创建公用表表达式的基本语法：

```sql
WITH cte_name (column1, column2, ...) AS (
    -- CTE query
    SELECT ...
)
-- Main query using the CTE
SELECT ...
FROM cte_name;
```

> [!tip] 公用表表达式类似于子查询
> 
> 与子查询不同的是，CTE 可以被多次引用，并且可以被其他 CTE 引用
> 
>  

### 示例

我们将使用示例数据库中的 `customers` 表进行演示

![[Pasted image 20241229210106.png]]

以下示例说明如何使用 CTE 从示例数据库的 `customers` 表中查询数据

```sql
mysql> WITH customers_in_usa AS (SELECT customerName,state FROM customers WHERE country = 'USA')
    -> SELECT customerName FROM customers_in_usa WHERE state='CA'
    -> ORDER BY customerName;
+------------------------------+
| customerName                 |
+------------------------------+
| Boards & Toys Co.            |
| Collectable Mini Designs Co. |
| Corporate Gift Ideas Co.     |
| Men 'R' US Retailers, Ltd.   |
| Mini Gifts Distributors Ltd. |
| Mini Wheels Co.              |
| Signal Collectibles Ltd.     |
| Technics Stores Inc.         |
| The Sharp Gifts Warehouse    |
| Toys4GrownUps.com            |
| West Coast Collectables Co.  |
+------------------------------+
11 rows in set (0.01 sec)
```

## 递归共用表表达式

递归公用表表达式也是一种公用表表达式，只不过，除了公用表表达式的特点以外，它还有自己的特点，就是 **可以调用自己**。它的语法结构是

```sql
WITH RECURSIVE cte_name (column1, column2, ...)
AS(
    -- anchor member
    SELECT select_list FROM table1 WHERE condition

    UNION [ALL]

    -- recursive term
    SELECT select_list FROM cte_name WHERE recursive_condition
)

SELECT * FROM cte_name;
```

> [!attention] 
> 
> 递归成员不得包含以下结构
> + 聚合函数
> + GROUP BY 子句
> + ORDER BY 子句
> + LIMIT 子句
> + DISTINCT 子句
> 
