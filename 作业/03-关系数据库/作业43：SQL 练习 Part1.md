# 作业43：SQL 练习 Part1

## [[查询数据]]

使用 [[mysql-company.sql]] 中提供的数据，并将其导入到 MySQL 中。然后通过 navicat 导入 PostgreSQL 中

```sql
mysql> source mysql-company.sql  -- 导入数据
```

下面我们在基本的查询语句中，`employees` 这张表，通过 `DESC table_name` 查看表的结构

```sql
mysql> DESC employees;
+----------------+-------------+------+-----+---------+-------+
| Field          | Type        | Null | Key | Default | Extra |
+----------------+-------------+------+-----+---------+-------+
| employee_id    | int         | NO   | PRI | 0       |       |
| first_name     | varchar(20) | YES  |     | NULL    |       |
| last_name      | varchar(25) | NO   |     | NULL    |       |
| email          | varchar(25) | NO   | UNI | NULL    |       |
| phone_number   | varchar(20) | YES  |     | NULL    |       |
| hire_date      | date        | NO   |     | NULL    |       |
| job_id         | varchar(10) | NO   | MUL | NULL    |       |
| salary         | double(8,2) | YES  |     | NULL    |       |
| commission_pct | double(2,2) | YES  |     | NULL    |       |
| manager_id     | int         | YES  | MUL | NULL    |       |
| department_id  | int         | YES  | MUL | NULL    |       |
+----------------+-------------+------+-----+---------+-------+
11 rows in set (0.00 sec)
```

在 PostgreSQL 中使用 `\d table_name` 查看表结构

```sql
company=# \d employees
                      数据表 "public.employees"
      栏位      |         类型          | 校对规则 |  可空的  | 预设
----------------+-----------------------+----------+----------+------
 employee_id    | integer               |          | not null |
 first_name     | character varying(20) |          |          |
 last_name      | character varying(25) |          | not null |
 email          | character varying(25) |          | not null |
 phone_number   | character varying(20) |          |          |
 hire_date      | date                  |          | not null |
 job_id         | character varying(10) |          | not null |
 salary         | double precision      |          |          |
 commission_pct | double precision      |          |          |
 manager_id     | integer               |          |          |
 department_id  | integer               |          |          |

索引：
    "employees_pkey" PRIMARY KEY, btree (employee_id)
    "emp_dept_fk" btree (department_id)
    "emp_email_uk" UNIQUE, btree (email)
    "emp_emp_id_pk" UNIQUE, btree (employee_id)
    "emp_job_fk" btree (job_id)
    "emp_manager_fk" btree (manager_id)

外部键(FK)限制：
    "emp_dept_fk" FOREIGN KEY (department_id) REFERENCES departments(department_id)
    "emp_job_fk" FOREIGN KEY (job_id) REFERENCES jobs(job_id)
    "emp_manager_fk" FOREIGN KEY (manager_id) REFERENCES employees(employee_id)
由引用：
    TABLE "departments" CONSTRAINT "dept_mgr_fk" FOREIGN KEY (manager_id) REFERENCES employees(employee_id)
    TABLE "employees" CONSTRAINT "emp_manager_fk" FOREIGN KEY (manager_id) REFERENCES employees(employee_id)
    TABLE "job_history" CONSTRAINT "jhist_emp_fk" FOREIGN KEY (employee_id) REFERENCES employees(employee_id)
```

简单了解了 `employees` 表结构和其约束关系。下面，我们开始练习

#### 查询员工 $12$ 个月的工资总和，并起别名为 `ANNUAL SALARY`

```sql
company=# SELECT employee_id,last_name, salary * 12 as "ANNUAL SALARY"
company-# FROM employees;
 employee_id |  last_name  | ANNUAL SALARY
-------------+-------------+---------------
         100 | King        |        288000
         101 | Kochhar     |        204000
...
         205 | Higgins     |        144000
         206 | Gietz       |         99600
(107 行记录)
```

#### 查询 `employees` 表中去除重复的 `job_id` 以后的数据

```sql
company=# SELECT DISTINCT job_id 
company=# FROM employees;
   job_id
------------
 SH_CLERK
 AD_VP
 SA_MAN
 PU_MAN
 IT_PROG
 ST_CLERK
 FI_MGR
 PU_CLERK
 HR_REP
 ST_MAN
 MK_MAN
 AC_MGR
 SA_REP
 AD_ASST
 PR_REP
 MK_REP
 AD_PRES
 FI_ACCOUNT
 AC_ACCOUNT
(19 行记录)
```

#### 查询工资大于 $12000$ 的员工姓名和工资

```sql
company=# SELECT last_name || '.' || first_name AS "fullname", salary
company-# FROM employees
company-# WHERE salary > 12000;
     fullname      | salary
-------------------+--------
 King.Steven       |  24000
 Kochhar.Neena     |  17000
 De Haan.Lex       |  17000
 Russell.John      |  14000
 Partners.Karen    |  13500
 Hartstein.Michael |  13000
(6 行记录)
```

#### 查询员工号为 176 的员工的姓名和部门号

```sql
company=# SELECT last_name || '.' || first_name AS "fullname", department_id
company-# FROM employees
company-# WHERE employee_id = 176;
    fullname     | department_id
-----------------+---------------
 Taylor.Jonathon |            80
(1 行记录)
```

#### 查询工资不在 5000 到 12000 的员工的姓名和工资

```sql
company=# SELECT last_name || '.' || first_name fullname, salary
company-# FROM employees
company-# WHERE salary NOT BETWEEN 5000 AND 12000;
     fullname      | salary
-------------------+--------
 King.Steven       |  24000
 Kochhar.Neena     |  17000
 ....
 Whalen.Jennifer   |   4400
 Hartstein.Michael |  13000
(55 行记录)
```

#### 查询在 20 或 50 号部门工作的员工姓名和部门号

```sql
company=# SELECT last_name || '.' || first_name fullname, department_id
company-# FROM employees
company-# WHERE department_id IN (20, 50);
     fullname      | department_id
-------------------+---------------
 Weiss.Matthew     |            50
 Fripp.Adam        |            50
 ...
 Hartstein.Michael |            20
 Fay.Pat           |            20
(47 行记录)
```

#### 查询公司中没有管理者的员工姓名及 `job_id`

```sql
company=# SELECT last_name || '.' || first_name fullname, job_id
company-# FROM employees
company-# WHERE manager_id IS NULL;
  fullname   | job_id
-------------+---------
 King.Steven | AD_PRES
(1 行记录)
```

#### 查询公司中有奖金的员工姓名，工资和奖金级别

```sql
company=# SELECT last_name || '.' || first_name fullname, salary,commission_pct
company-# FROM employees
company-# WHERE commission_pct IS NOT NULL;
     fullname      | salary | commission_pct
-------------------+--------+----------------
 Russell.John      |  14000 |            0.4
 Partners.Karen    |  13500 |            0.3
 ...
 Grant.Kimberely   |   7000 |           0.15
 Johnson.Charles   |   6200 |            0.1
(35 行记录)
```

#### 选择员工姓名的第三个字母是 `'a'` 的员工姓名

```sql
company=# SELECT last_name || '.' || first_name fullname
company-# FROM employees
company-# WHERE last_name LIKE '__a%';
    fullname
-----------------
 Grant.Kimberely
 Grant.Douglas
 Whalen.Jennifer
(3 行记录)

company=# SELECT last_name || '.' || first_name fullname
company-# FROM employees
company-# WHERE last_name ~ '^.{2}a.*';  -- 正则匹配
    fullname
-----------------
 Grant.Kimberely
 Grant.Douglas
 Whalen.Jennifer
(3 行记录)
```

####  选择姓名中有字母 `'a'` 和 `'k'` 的员工姓名

```sql
company=# SELECT last_name || '.' || first_name fullname
company-# FROM employees
company-# WHERE last_name LIKE '%a%' AND last_name LIKE '%k%';
     fullname
------------------
 Markle.Steven
 Philtanker.Hazel
(2 行记录)


company=# SELECT last_name || '.' || first_name fullname
company-# FROM employees
company-# WHERE last_name ~ '.*(?=.*a+.*k+.*|.*k+.*a+.*).*';
     fullname
------------------
 Markle.Steven
 Philtanker.Hazel
(2 行记录)
```

#### 显示出表 `employees` 表中 `first_name` 以 `'e'` 结尾的员工信息

```sql
company=# SELECT last_name,first_name,salary,job_id
company-# FROM employees
company-# WHERE first_name LIKE '%e';
  last_name  | first_name | salary |  job_id
-------------+------------+--------+----------
 Ernst       | Bruce      |   6000 | IT_PROG
 Mikkilineni | Irene      |   2700 | ST_CLERK
 Atkinson    | Mozhe      |   2800 | ST_CLERK
 Ladwig      | Renske     |   3600 | ST_CLERK
 Cambrault   | Nanette    |   7500 | SA_REP
 King        | Janette    |  10000 | SA_REP
 Doran       | Louise     |   7500 | SA_REP
 Greene      | Danielle   |   9500 | SA_REP
 Jones       | Vance      |   2800 | SH_CLERK
(9 行记录)


company=# SELECT last_name,first_name,salary,job_id
company-# FROM employees
company-# WHERE first_name ~ '.*e$';
  last_name  | first_name | salary |  job_id
-------------+------------+--------+----------
 Ernst       | Bruce      |   6000 | IT_PROG
 Mikkilineni | Irene      |   2700 | ST_CLERK
 Atkinson    | Mozhe      |   2800 | ST_CLERK
 Ladwig      | Renske     |   3600 | ST_CLERK
 Cambrault   | Nanette    |   7500 | SA_REP
 King        | Janette    |  10000 | SA_REP
 Doran       | Louise     |   7500 | SA_REP
 Greene      | Danielle   |   9500 | SA_REP
 Jones       | Vance      |   2800 | SH_CLERK
(9 行记录)
```

#### 显示出表 `employees` 部门编号在 `80-100` 之间的姓名、工种

```sql
company=# SELECT last_name,first_name,job_id
company-# FROM employees
company-# WHERE department_id BETWEEN 80 AND 100;
 last_name  | first_name  |   job_id
------------+-------------+------------
 King       | Steven      | AD_PRES
 Kochhar    | Neena       | AD_VP
 ...
 Livingston | Jack        | SA_REP
 Johnson    | Charles     | SA_REP
(43 行记录)
```

#### 显示出表 `employees` 的 `manager_id` 是 `100,101,110` 的员工姓名、工资、管理者 id

```sql
company=# SELECT last_name,first_name,salary,manager_id
company-# FROM employees
company-# WHERE manager_id IN (100, 101, 110);
 last_name | first_name | salary | manager_id
-----------+------------+--------+------------
 Kochhar   | Neena      |  17000 |        100
 De Haan   | Lex        |  17000 |        100
 Greenberg | Nancy      |  12000 |        101
 Raphaely  | Den        |  11000 |        100
 Weiss     | Matthew    |   8000 |        100
 Fripp     | Adam       |   8200 |        100
 Kaufling  | Payam      |   7900 |        100
 Vollman   | Shanta     |   6500 |        100
 Mourgos   | Kevin      |   5800 |        100
 Russell   | John       |  14000 |        100
 Partners  | Karen      |  13500 |        100
 Errazuriz | Alberto    |  12000 |        100
 Cambrault | Gerald     |  11000 |        100
 Zlotkey   | Eleni      |  10500 |        100
 Whalen    | Jennifer   |   4400 |        101
 Hartstein | Michael    |  13000 |        100
 Mavris    | Susan      |   6500 |        101
 Baer      | Hermann    |  10000 |        101
 Higgins   | Shelley    |  12000 |        101
(19 行记录)
```

## [[过滤数据]]

#### 查询员工的姓名和部门号和年薪，按年薪降序,按姓名升序显示

```sql
company=# SELECT last_name,first_name,salary * 12 annual
company-# FROM employees
company-# ORDER BY annual DESC, last_name ASC;
  last_name  | first_name  | annual
-------------+-------------+--------
 King        | Steven      | 288000
 De Haan     | Lex         | 204000
 ...
 Markle      | Steven      |  26400
 Philtanker  | Hazel       |  26400
 Olson       | TJ          |  25200
(107 行记录)
```

#### 选择工资不在 8000 到 17000 的员工的姓名和工资，按工资降序，显示第 21 到 40 位置的数据

```sql
company=# SELECT last_name,first_name, salary
company-# FROM employees
company-# ORDER BY salary DESC
company-# LIMIT 20
company-# OFFSET 20;
 last_name  | first_name  | salary
------------+-------------+--------
 Bernstein  | David       |   9500
 Greene     | Danielle    |   9500
 Sully      | Patrick     |   9500
 Faviet     | Daniel      |   9000
 McEwen     | Allan       |   9000
 Hunold     | Alexander   |   9000
 Hall       | Peter       |   9000
 Hutton     | Alyssa      |   8800
 Taylor     | Jonathon    |   8600
 Livingston | Jack        |   8400
 Gietz      | William     |   8300
 Fripp      | Adam        |   8200
 Chen       | John        |   8200
 Smith      | Lindsey     |   8000
 Olsen      | Christopher |   8000
 Weiss      | Matthew     |   8000
 Kaufling   | Payam       |   7900
 Urman      | Jose Manuel |   7800
 Sciarra    | Ismael      |   7700
 Doran      | Louise      |   7500
(20 行记录)

company=# SELECT last_name,first_name, salary
company-# FROM employees
company-# ORDER BY salary DESC
company-# OFFSET 20 ROWS
company-# FETCH NEXT 20 ROWS ONLY;
 last_name  | first_name  | salary
------------+-------------+--------
 Bernstein  | David       |   9500
 Greene     | Danielle    |   9500
 Sully      | Patrick     |   9500
 Faviet     | Daniel      |   9000
 McEwen     | Allan       |   9000
 Hunold     | Alexander   |   9000
 Hall       | Peter       |   9000
 Hutton     | Alyssa      |   8800
 Taylor     | Jonathon    |   8600
 Livingston | Jack        |   8400
 Gietz      | William     |   8300
 Fripp      | Adam        |   8200
 Chen       | John        |   8200
 Smith      | Lindsey     |   8000
 Olsen      | Christopher |   8000
 Weiss      | Matthew     |   8000
 Kaufling   | Payam       |   7900
 Urman      | Jose Manuel |   7800
 Sciarra    | Ismael      |   7700
 Doran      | Louise      |   7500
(20 行记录)
```

#### 查询邮箱中包含 e 的员工信息，并先按邮箱的字符数降序，再按部门号升序

```sql
company=# SELECT last_name,first_name,email,department_id
company-# FROM employees
company-# WHERE email ILIKE '%e%'  -- ILIKE 忽略大小写。PostgreSQL 区分大小写的
company-# ORDER BY LENGTH(email) DESC, department_id ASC;
 last_name  | first_name  |  email   | department_id
------------+-------------+----------+---------------
 Hartstein  | Michael     | MHARTSTE |            20
 Raphaely   | Den         | DRAPHEAL |            30
 ....
 Gee        | Ki          | KGEE     |            50
 Lee        | David       | DLEE     |            80
(47 行记录)
```
