# 作业45：SQL 练习 Part3

下面开始练习 **分组与聚合**

## [[数据库：分组与聚合]]

####  WHERE 子句可否使用聚合函数进行过滤?

> [!attention] 
> 
> `WHERE` 是对原始表中的数据进行筛选，执行位置在 `GROUP BY` 之前，无法使用聚合函数
> 

#### 查询公司员工工资的最大值，最小值，平均值，总和

```sql
company=# SELECT MAX(salary), MIN(salary), AVG(salary), SUM(salary)
company-# FROM employees;
  max  | min  |        avg        |  sum
-------+------+-------------------+--------
 24000 | 2100 | 6461.682242990654 | 691400
(1 行记录)
```

#### 查询各 `job_id` 的员工工资的最大值，最小值，平均值，总和

```sql
company=# SELECT job_id, MAX(salary), MIN(salary), AVG(salary), SUM(salary)
company-# FROM employees
company-# GROUP BY job_id;
   job_id   |  max  |  min  |  avg  |  sum
------------+-------+-------+-------+--------
 SH_CLERK   |  4200 |  2500 |  3215 |  64300
 AD_VP      | 17000 | 17000 | 17000 |  34000
 SA_MAN     | 14000 | 10500 | 12200 |  61000
 PU_MAN     | 11000 | 11000 | 11000 |  11000
 IT_PROG    |  9000 |  4200 |  5760 |  28800
 ST_CLERK   |  3600 |  2100 |  2785 |  55700
 FI_MGR     | 12000 | 12000 | 12000 |  12000
 PU_CLERK   |  3100 |  2500 |  2780 |  13900
 HR_REP     |  6500 |  6500 |  6500 |   6500
 ST_MAN     |  8200 |  5800 |  7280 |  36400
 MK_MAN     | 13000 | 13000 | 13000 |  13000
 AC_MGR     | 12000 | 12000 | 12000 |  12000
 SA_REP     | 11500 |  6100 |  8350 | 250500
 AD_ASST    |  4400 |  4400 |  4400 |   4400
 PR_REP     | 10000 | 10000 | 10000 |  10000
 MK_REP     |  6000 |  6000 |  6000 |   6000
 AD_PRES    | 24000 | 24000 | 24000 |  24000
 FI_ACCOUNT |  9000 |  6900 |  7920 |  39600
 AC_ACCOUNT |  8300 |  8300 |  8300 |   8300
(19 行记录)
```

#### 查询各个 `job_id` 的员工人数

```sql
company=# SELECT job_id, COUNT(*) members  -- COUNT(*) 包括 NULL 和重复值
company-# FROM employees
company-# GROUP BY job_id;
   job_id   | members
------------+---------
 SH_CLERK   |      20
 AD_VP      |       2
 SA_MAN     |       5
 PU_MAN     |       1
 IT_PROG    |       5
 ST_CLERK   |      20
 FI_MGR     |       1
 PU_CLERK   |       5
 HR_REP     |       1
 ST_MAN     |       5
 MK_MAN     |       1
 AC_MGR     |       1
 SA_REP     |      30
 AD_ASST    |       1
 PR_REP     |       1
 MK_REP     |       1
 AD_PRES    |       1
 FI_ACCOUNT |       5
 AC_ACCOUNT |       1
(19 行记录)
```

#### 查询员工最高工资和最低工资的差距（`DIFFERENCE`）

```sql
company=# SELECT MAX(salary) - MIN(salary) DIFFERENCE
company-# FROM employees;
 difference
------------
      21900
(1 行记录)
```

#### 查询各个管理者手下员工的最低工资，其中最低工资不能低于 `6000`，没有管理者的员工不计算在内

```sql
company=# SELECT manager_id, MIN(salary)  -- 不在聚合函数内的列，一定要在 GROUP BY 中
company-# FROM employees
company-# WHERE manager_id IS NOT NULL
company-# GROUP BY manager_id
company-# HAVING MIN(salary) >= 6000;
 manager_id | min
------------+------
        102 | 9000
        205 | 8300
        146 | 7000
        108 | 6900
        147 | 6200
        201 | 6000
        145 | 7000
        148 | 6100
        149 | 6200
(9 行记录)
```

#### 查询所有部门的名字，`location_id`，员工数量和平均工资，并按平均工资升序排列

```sql
company=# SELECT d.department_name, d.location_id, COUNT(e.employee_id), AVG(salary)
company-# FROM departments d
company-# LEFT JOIN employees e USING(department_id)
company-# GROUP BY  d.department_name, d.location_id
company-# ORDER BY AVG(salary);
   department_name    | location_id | count |        avg
----------------------+-------------+-------+--------------------
 Shipping             |        1500 |    45 | 3475.5555555555557
 Purchasing           |        1700 |     6 |               4150
 Administration       |        1700 |     1 |               4400
 IT                   |        1400 |     5 |               5760
 ....
 Manufacturing        |        1700 |     0 |
 NOC                  |        1700 |     0 |
 IT Support           |        1700 |     0 |
(27 行记录)
```

#### 查询每个工种、每个部门的部门名、工种名和最低工资

```sql
company=# SELECT emp.job_id, jobs.job_title, dep.department_name, MIN(salary)
company-# FROM employees AS emp
company-# RIGHT JOIN jobs ON emp.job_id = jobs.job_id
company-# RIGHT JOIN departments as dep ON emp.department_id = dep.department_id
company-# GROUP BY emp.job_id, jobs.job_title, dep.department_name;
   job_id   |            job_title            |   department_name    |  min
------------+---------------------------------+----------------------+-------
            |                                 | Government Sales     |
            |                                 | Construction         |
 AD_ASST    | Administration Assistant        | Administration       |  4400
 AD_PRES    | President                       | Executive            | 24000
 ...
 SA_MAN     | Sales Manager                   | Sales                | 10500
            |                                 | IT Support           |
 AD_VP      | Administration Vice President   | Executive            | 17000
(35 行记录)

```
