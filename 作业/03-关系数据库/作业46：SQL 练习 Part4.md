# 作业46：SQL 练习 Part4

## [[数据库：子查询#不相关子查询]]

#### 找出比 Abel 工资高的员工的姓名和工资

```sql
-- 子查询
company=# SELECT last_name,salary
company-# FROM employees
company-# WHERE salary > (SELECT salary FROM employees WHERE last_name = 'Abel');
 last_name | salary
-----------+--------
 King      |  24000
 Kochhar   |  17000
 De Haan   |  17000
 Greenberg |  12000
 Russell   |  14000
 Partners  |  13500
 Errazuriz |  12000
 Ozer      |  11500
 Hartstein |  13000
 Higgins   |  12000
(10 行记录)

-- 自连接
company=# SELECT o.last_name, o.salary
company-# FROM employees o
company-# INNER JOIN employees abel ON true
company-# WHERE abel.last_name='Abel' AND abel.salary < o.salary;
 last_name | salary
-----------+--------
 King      |  24000
 Kochhar   |  17000
 De Haan   |  17000
 Greenberg |  12000
 Russell   |  14000
 Partners  |  13500
 Errazuriz |  12000
 Ozer      |  11500
 Hartstein |  13000
 Higgins   |  12000
(10 行记录)
```

#### 查询工资大于 `149` 号员工工资的员工的信息

```sql
company=# SELECT last_name, salary
company-# FROM employees
company-# WHERE salary > ( SELECT salary FROM employees WHERE employee_id = 149);
 last_name | salary
-----------+--------
 King      |  24000
 Kochhar   |  17000
 De Haan   |  17000
 Greenberg |  12000
 Raphaely  |  11000
 Russell   |  14000
 Partners  |  13500
 Errazuriz |  12000
 Cambrault |  11000
 Ozer      |  11500
 Abel      |  11000
 Hartstein |  13000
 Higgins   |  12000
(13 行记录)
```

#### 查询 `job_id` 与 `141` 号员工相同，`salary` 比 `143` 号员工多的员工姓名，`job_id` 和工资

```sql
-- 找到 141 号员工的 job_id
-- 找到 143 号员工的 salary
-- 将它们作为条件
company=# SELECT last_name, job_id, salary
company-# FROM employees
company-# WHERE job_id=(SELECT job_id FROM employees WHERE employee_id = 141) AND salary > (SELECT salary FROM employees
 WHERE employee_id = 143);
  last_name  |  job_id  | salary
-------------+----------+--------
 Nayer       | ST_CLERK |   3200
 Mikkilineni | ST_CLERK |   2700
 Bissot      | ST_CLERK |   3300
 Atkinson    | ST_CLERK |   2800
 Mallin      | ST_CLERK |   3300
 Rogers      | ST_CLERK |   2900
 Ladwig      | ST_CLERK |   3600
 Stiles      | ST_CLERK |   3200
 Seo         | ST_CLERK |   2700
 Rajs        | ST_CLERK |   3500
 Davies      | ST_CLERK |   3100
(11 行记录)
```

#### 查询公司工资最少的员工的 `last_name`, `job_id` 和 `salary`

```sql
-- 找到公式工资最低的员工的工资
-- 将这个工资作为条件
company=# SELECT last_name,job_id,salary
company-# FROM employees
company-# WHERE salary=(SELECT MIN(salary) FROM employees);
 last_name |  job_id  | salary
-----------+----------+--------
 Olson     | ST_CLERK |   2100
(1 行记录)
```

#### 查询与 `141` 号或 `174` 号员工的 `manager_id` 和 `department_id` 相同的其他员工的 `employee_id`，`manager_id`，`department_id`

```sql
-- 找到 141 号的 manager_id 和 department_id
-- 找到 147 号的 manager_id 和 department_id
-- 将他们作为条件
company=# SELECT employee_id,manager_id,department_id
company-# FROM employees
company-# WHERE (manager_id,department_id) IN (SELECT manager_id,department_id FROM employees WHERE employee_id IN (141, 174)) AND employee_id NOT IN (141,174);
 employee_id | manager_id | department_id
-------------+------------+---------------
         142 |        124 |            50
         143 |        124 |            50
         144 |        124 |            50
         175 |        149 |            80
         176 |        149 |            80
         177 |        149 |            80
         179 |        149 |            80
         196 |        124 |            50
         197 |        124 |            50
         198 |        124 |            50
         199 |        124 |            50
(11 行记录)
```

#### 查询最低工资大于 50 号部门最低工资的部门 id 和其最低工资

```sql
company=# SELECT department_id,MIN(salary)
company-# FROM employees
company-# GROUP BY department_id
company-# HAVING MIN(salary) > (SELECT MIN(salary) FROM employees WHERE department_id = 50);
 department_id |  min
---------------+-------
            70 | 10000
            80 |  6100
            20 |  6000
            10 |  4400
               |  7000
            90 | 17000
           100 |  6900
           110 |  8300
            30 |  2500
            40 |  6500
            60 |  4200
(11 行记录)
```

#### 查找其它 `job_id` 中比 `job_id` 为 `'IT_PROG' `部门任一工资低的员工的员工号、姓名、`job_id` 以及 `salary`

```sql
company=# SELECT employee_id,last_name,job_id,salary
company-# FROM employees
company-# WHERE job_id != 'IT_PROG'
company-# AND salary < ANY(SELECT salary FROM employees WHERE job_id = 'IT_PROG');
 employee_id |  last_name  |   job_id   | salary
-------------+-------------+------------+--------
         110 | Chen        | FI_ACCOUNT |   8200
         111 | Sciarra     | FI_ACCOUNT |   7700
...
         203 | Mavris      | HR_REP     |   6500
         206 | Gietz       | AC_ACCOUNT |   8300
(76 行记录)
```

#### 查询平均工资最低的部门id

```sql
-- 找到最低每个部门的最低工作
company=# SELECT department_id
company-# FROM employees
company-# GROUP BY department_id
company-# HAVING AVG(salary) <= ALL(SELECT AVG(salary) FROM employees GROUP BY department_id);
 department_id
---------------
            50
(1 行记录)
```
#### 查询和 `Zlotkey` 相同部门的员工姓名和工资

```sql
-- WHERE子查询
SELECT last_name, salary
FROM employees
WHERE department_id IN (SELECT department_id
                        FROM employees
                        WHERE last_name = 'Zlotkey');
-- FROM 子查询
SELECT last_name, salary
FROM employees
    JOIN (SELECT department_id
                        FROM employees
                        WHERE last_name = 'Zlotkey') AS ZL
        ON employees.department_id = ZL.department_id;
```

#### 查询工资比公司平均工资高的员工的员工号，姓名和工资

```sql
SELECT employee_id, last_name, salary  
FROM employees  
WHERE salary > (SELECT AVG(salary) 
				FROM employees);
```

#### 查询工资大于所有 `JOB_ID = 'SA_MAN'` 的员工的工资的员工的 `last_name`, `job_id`, `salary`

```sql
SELECT emp.last_name, emp.job_id, emp.salary
FROM employees AS emp
WHERE salary > ALL(SELECT salary FROM employees WHERE job_id = 'SA_MAN');
```

#### 查询与姓名中包含字母 `u` 的员工在相同部门的员工的员工号和姓名

```sql
SELECT emp.employee_id, emp.last_name
FROM employees AS emp
WHERE department_id IN (
                        SELECT DISTINCT department_id
                        FROM employees
                        WHERE last_name like '%u%'
                        ) ;
```

#### 查询在部门的 `location_id` 为 `1700` 的部门工作的员工的员工号

```sql
-- 子查询
SELECT emp.employee_id
FROM employees AS emp
WHERE department_id IN (SELECT department_id
                        FROM departments
                        WHERE location_id = 1700);
-- 连接查询
SELECT emp.employee_id
FROM employees AS emp
    JOIN departments AS dep
        ON emp.department_id = dep.department_id
WHERE dep.location_id = 1700
```

#### 查询管理者是 `King` 的员工姓名和工资

```sql
-- 子查询
SELECT emp.last_name, emp.salary
FROM employees AS emp
WHERE manager_id IN (SELECT employee_id
                    FROM employees AS manger
                    WHERE manger.last_name = 'King');
-- 连接查询(自连接)
SELECT emp.last_name, emp.salary
FROM employees AS emp
    JOIN employees AS manager
        ON manager.employee_id = emp.manager_id
WHERE manager.last_name = 'King';

-- EXISTS
SELECT emp.last_name, emp.salary
FROM employees AS emp
WHERE EXISTS(SELECT *
             FROM employees as manager
             WHERE manager.employee_id = emp.manager_id AND manager.last_name = 'King');
```

#### 查询工资最低的员工信息: last_name, salary

```sql
SELECT last_name, salary  
FROM employees  
WHERE salary = (SELECT MIN(salary)  
                FROM employees);
```

#### 查询平均工资最低的部门信息

```sql
-- 查询每个部门的平均工资
SELECT department_id, AVG(salary) AS dep_avg
FROM employees
group by department_id;

-- 找到部门最小的平均值
SELECT MIN(dep_avg_sal)
FROM (SELECT AVG(salary) AS dep_avg_sal
      FROM employees
      group by department_id) AS dep_avg_sal_tabel;

-- 平均工资最小的部门
SELECT department_id
FROM employees
GROUP BY department_id
HAVING AVG(salary) = (SELECT MIN(dep_avg_sal)
                      FROM (SELECT AVG(salary) AS dep_avg_sal
                            FROM employees
                            group by department_id) AS dep_avg_sal_tabel);
                            
-- 使用部门ID 取部门信息
SELECT *
FROM departments
WHERE department_id = (SELECT department_id
                       FROM employees
                       GROUP BY department_id
                       HAVING AVG(salary) = (SELECT MIN(dep_avg_sal)
                                             FROM (SELECT AVG(salary) AS dep_avg_sal
                                                   FROM employees
                                                   group by department_id) AS dep_avg_sal_tabel));
```

优化上述的 `SELECT`

```sql
-- 使用 ALL 减少一层 SELECT
SELECT *
FROM departments
WHERE department_id = (SELECT department_id
                       FROM employees
                       GROUP BY department_id
                       HAVING AVG(salary) <= ALL (SELECT AVG(salary)
                                                   FROM employees
                                                   group by department_id));
-- ORDER BY + LIMIT 将子查询嵌套减少一层
SELECT *
FROM departments
WHERE department_id = (SELECT department_id
                       FROM employees
                       GROUP BY department_id
                       HAVING AVG(salary) = (SELECT AVG(salary)
                                             FROM employees
                                             group by department_id
                                             ORDER BY AVG(salary)
                                             LIMIT 1 OFFSET 0));
-- ORDER BY + LIMIT + 连接查询，将子查询嵌套减少到一层
SELECT dep.*
FROM departments AS dep,
     (SELECT department_id, AVG(salary) AS dep_avg
      FROM employees
      group by department_id
      ORDER BY dep_avg
      LIMIT 1 OFFSET 0) AS dep_avg_sal_table
WHERE dep.department_id = dep_avg_sal_table.department_id
```


## [[数据库：子查询#相关子查询]]

#### 查询工资大于本部门平均工资的员工的 `last_name`,`salary` 和其 `department_id`

```sql
company=# SELECT last_name,salary,department_id
company-# FROM employees "outer"
company-# WHERE salary > (SELECT AVG(salary) FROM employees "inner" WHERE "outer".department_id = "inner".department_id GROUP BY department_id);
 last_name | salary | department_id
-----------+--------+---------------
 King      |  24000 |            90
 Hunold    |   9000 |            60
 ...
 Hartstein |  13000 |            20
 Higgins   |  12000 |           110
(38 行记录)


```

#### 查询公司管理者的 `employee_id`，`last_name`，`job_id`，`department_id` 信息

```sql
company=# SELECT employee_id, last_name, job_id, department_id
company-# FROM employees manager
company-# WHERE EXISTS(SELECT * FROM employees emp WHERE manager.employee_id = emp.manager_id);
 employee_id | last_name | job_id  | department_id
-------------+-----------+---------+---------------
         100 | King      | AD_PRES |            90
         101 | Kochhar   | AD_VP   |            90
         102 | De Haan   | AD_VP   |            90
         103 | Hunold    | IT_PROG |            60
         108 | Greenberg | FI_MGR  |           100
         114 | Raphaely  | PU_MAN  |            30
         120 | Weiss     | ST_MAN  |            50
         121 | Fripp     | ST_MAN  |            50
         122 | Kaufling  | ST_MAN  |            50
         123 | Vollman   | ST_MAN  |            50
         124 | Mourgos   | ST_MAN  |            50
         145 | Russell   | SA_MAN  |            80
         146 | Partners  | SA_MAN  |            80
         147 | Errazuriz | SA_MAN  |            80
         148 | Cambrault | SA_MAN  |            80
         149 | Zlotkey   | SA_MAN  |            80
         201 | Hartstein | MK_MAN  |            20
         205 | Higgins   | AC_MGR  |           110
(18 行记录)
```

#### 查询管理者是 `King` 的员工姓名和工资

```sql
-- 子查询
SELECT emp.last_name, emp.salary
FROM employees AS emp
WHERE manager_id IN (SELECT employee_id
                    FROM employees AS manger
                    WHERE manger.last_name = 'King');
-- 连接查询(自连接)
SELECT emp.last_name, emp.salary
FROM employees AS emp
    JOIN employees AS manager
        ON manager.employee_id = emp.manager_id
WHERE manager.last_name = 'King';

-- EXISTS 相关子查询
SELECT emp.last_name, emp.salary
FROM employees AS emp
WHERE EXISTS(SELECT *
             FROM employees as manager
             WHERE manager.employee_id = emp.manager_id AND manager.last_name = 'King');
```

#### 查询出公司中所有 manager 的详细信息

```sql
-- 自连接
SELECT DISTINCT manager.manager_id, manager.last_name, manager.job_id, manager.department_id
FROM employees AS emp
    JOIN employees AS manager
        ON manager.employee_id = emp.manager_id;

-- 子查询
SELECT manager_id, last_name, job_id, department_id
FROM employees
WHERE employee_id IN (SELECT DISTINCT manager_id
                      FROM employees);

-- EXISTS
SELECT manager_id, last_name, job_id, department_id
FROM employees AS manager
WHERE EXISTS(SELECT *
             FROM employees as emp
             WHERE manager.employee_id = emp.manager_id);
```

#### 查询部门的部门号，其中不包括 `job_id` 是 `"ST_CLERK"` 的部门号

```sql
-- 查询包含 job_id = 'ST_CLERK' 的部门号
-- 从部门表中查询部门号不再其中的部门
SELECT department_id
FROM departments
WHERE department_id NOT IN (SELECT DISTINCT department_id
                            FROM employees
                            WHERE job_id = 'ST_CLERK')
-- EXISTS 改写
SELECT department_id
FROM departments AS dep
WHERE NOT EXISTS(SELECT DISTINCT department_id
                            FROM employees AS emp
                            WHERE job_id = 'ST_CLERK' AND dep.department_id = emp.department_id)
```


#### 查询各部门中工资比本部门平均工资高的员工的员工号, 姓名和工资（相关子查询）

```sql
-- 相关子查询
SELECT employee_id, last_name, salary
FROM employees as e1
WHERE salary > (SELECT AVG(salary)
                FROM employees as e2
                WHERE e1.department_id = e2.department_id);
-- FROM 子查询
SELECT employee_id, last_name, salary
FROM employees as emp, (SELECT department_id, AVG(salary) avg_sal
FROM employees
GROUP BY department_id) as dep_avg_table
where emp.department_id = dep_avg_table.department_id and emp.salary > dep_avg_table.avg_sal
```

#### 查询每个部门下的部门人数大于 5 的部门名称（相关子查询）

```sql
-- 多行子查询  
SELECT department_name  
FROM departments  
WHERE department_id in (SELECT department_id  
                        FROM employees  
                        GROUP BY department_id  
                        HAVING COUNT(employee_id) > 5);  
-- 相关子查询  
SELECT d.department_name  
FROM departments AS d  
WHERE 5 < (SELECT COUNT(e.employee_id)  
           FROM employees as e  
           WHERE d.department_id = e.department_id  
           );
```
