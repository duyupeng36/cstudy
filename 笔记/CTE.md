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

查询员工所在的部门的详细信息

```sql
SELECT  
    *  
FROM  
    departments  
WHERE department_id IN (  
    SELECT  
        department_id  
    FROM  
        employees  
    );
```

可以使用 CTE 完成

```sql
WITH emp_dep_id AS (
    SELECT DISTINCT employees.department_id FROM employees
)
SELECT
    *
FROM
    departments
INNER JOIN emp_dep_id
ON emp_dep_id.department_id = departments.department_id;
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

```sql
WITH RECURSIVE cte  
                   AS  
                   (SELECT employee_id, last_name, manager_id, 1 AS n  
                    FROM emps  
                    WHERE employee_id = 100  -- 子查询，找到第一代领导  
                    
                    UNION ALL  
                    SELECT a.employee_id, a.last_name, a.manager_id, n + 1  
                    FROM emps AS a  
                             JOIN cte  
                                  ON (a.manager_id = cte.employee_id) -- 递归查询，找出以递归公用表表达式的人为领导的人  
                   )  
SELECT employee_id, last_name  
FROM cte  
WHERE n >= 3;
```
