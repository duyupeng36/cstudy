# 作业44：SQL 练习 Part2

下面做多表查询的练习。将涉及 $3$ 张表

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


company=# \d departments
                     数据表 "public.departments"
      栏位       |         类型          | 校对规则 |  可空的  | 预设
-----------------+-----------------------+----------+----------+------
 department_id   | integer               |          | not null |
 department_name | character varying(30) |          | not null |
 manager_id      | integer               |          |          |
 location_id     | integer               |          |          |
索引：
    "departments_pkey" PRIMARY KEY, btree (department_id)
    "dept_id_pk" UNIQUE, btree (department_id)
    "dept_loc_fk" btree (location_id)
    "dept_mgr_fk" btree (manager_id)
外部键(FK)限制：
    "dept_loc_fk" FOREIGN KEY (location_id) REFERENCES locations(location_id)
    "dept_mgr_fk" FOREIGN KEY (manager_id) REFERENCES employees(employee_id)
由引用：
    TABLE "employees" CONSTRAINT "emp_dept_fk" FOREIGN KEY (department_id) REFERENCES departments(department_id)
    TABLE "job_history" CONSTRAINT "jhist_dept_fk" FOREIGN KEY (department_id) REFERENCES departments(department_id)


company=# \d locations
                      数据表 "public.locations"
      栏位      |         类型          | 校对规则 |  可空的  | 预设
----------------+-----------------------+----------+----------+------
 location_id    | integer               |          | not null |
 street_address | character varying(40) |          |          |
 postal_code    | character varying(12) |          |          |
 city           | character varying(30) |          | not null |
 state_province | character varying(25) |          |          |
 country_id     | character(2)          |          |          |
索引：
    "locations_pkey" PRIMARY KEY, btree (location_id)
    "loc_c_id_fk" btree (country_id)
    "loc_id_pk" UNIQUE, btree (location_id)
外部键(FK)限制：
    "loc_c_id_fk" FOREIGN KEY (country_id) REFERENCES countries(country_id)
由引用：
    TABLE "departments" CONSTRAINT "dept_loc_fk" FOREIGN KEY (location_id) REFERENCES locations(location_id)
```

## [[连接表]]

#### 显示所有员工的姓名，部门号和部门名称

```sql
company=# SELECT e.first_name,e.last_name,d.department_id,d.department_name
company-# FROM employees e
company-# LEFT JOIN departments d ON e.department_id = d.department_id;
 first_name  |  last_name  | department_id | department_name
-------------+-------------+---------------+------------------
 Steven      | King        |            90 | Executive
 ...
 Diana       | Lorentz     |            60 | IT
 Nancy       | Greenberg   |           100 | Finance
 ...
 Den         | Raphaely    |            30 | Purchasing
 ...
 Peter       | Vargas      |            50 | Shipping
 John        | Russell     |            80 | Sales
 ....
 Jennifer    | Whalen      |            10 | Administration
 ...
 William     | Gietz       |           110 | Accounting
(107 行记录)
```

#### 查询 90 号部门员工的 `job_id` 和 90 号部门的 `location_id`

```sql
company=# SELECT e.department_id,e.job_id,d.location_id
company-# FROM employees e
company-# INNER JOIN departments d ON e.department_id = d.department_id
company-# WHERE e.department_id = 90;
 department_id | job_id  | location_id
---------------+---------+-------------
            90 | AD_PRES |        1700
            90 | AD_VP   |        1700
            90 | AD_VP   |        1700
(3 行记录)
```

#### 选择所有有奖金的员工的 `last_name` , `department_name` ,  `location_id` , `city`

```sql
-- last_name 在 employees 表中  
-- department_name 和 location_id 在 departments 表中  
-- location_id 和 city 在 locations 表中

company=# SELECT e.last_name,d.department_name,d.location_id,l.city
company-# FROM employees e
company-# LEFT JOIN departments d ON e.department_id = d.department_id
company-# LEFT JOIN locations l ON d.location_id = l.location_id
company-# WHERE commission_pct IS NOT NULL;
 last_name  | department_name | location_id |  city
------------+-----------------+-------------+--------
 Russell    | Sales           |        2500 | Oxford
 ...
 Grant      |                 |             |
 Johnson    | Sales           |        2500 | Oxford
(35 行记录)
```

#### 选择 `city` 在 `Toronto` 工作的员工的 `last_name` , `job_id` , `department_id` , `department_name`

```sql
-- city 需要在 locations 中查询  
-- last_name , job_id , department_id 可在 employees 中查询  
-- department_name 需要在 department 中查询

company=# SELECT e.last_name,e.job_id,e.department_id, d.department_name,l.city
company-# FROM employees e
company-# INNER JOIN departments d USING(department_id)
company-# INNER JOIN locations l USING(location_id)
company-# WHERE l.city='Toronto';
 last_name | job_id | department_id | department_name |  city
-----------+--------+---------------+-----------------+---------
 Hartstein | MK_MAN |            20 | Marketing       | Toronto
 Fay       | MK_REP |            20 | Marketing       | Toronto
(2 行记录)
```

#### 查询员工所在的部门名称为 `'Executive'` 的 部门地址、姓名、工作、工资

```sql
company=# SELECT d.department_name,street_address,last_name,job_id,salary
company-# FROM employees e
company-# INNER JOIN departments d USING(department_id)
company-# INNER JOIN locations l USING(location_id)
company-# WHERE d.department_name = 'Executive';
 department_name | street_address  | last_name | job_id  | salary
-----------------+-----------------+-----------+---------+--------
 Executive       | 2004 Charade Rd | King      | AD_PRES |  24000
 Executive       | 2004 Charade Rd | Kochhar   | AD_VP   |  17000
 Executive       | 2004 Charade Rd | De Haan   | AD_VP   |  17000
(3 行记录)
```

#### 选择指定员工的姓名，员工号，以及他的管理者的姓名和员工号，结果类似于下面的格式

```
employees  Emp#  manager   Mgr#
kochhar    101     king    100
```

```sql
company=# SELECT e.last_name AS "employees", e.employee_id AS "Emp#", m.last_name AS "manager", m.employee_id AS "Mgr#"
company-# FROM employees e
company-# LEFT JOIN employees m ON e.manager_id = m.employee_id;  -- 老板是没有管理者的。因此，不能使用 INNER JOIN。这里 员工表在左边，因此使用 LEFT JOIN
  employees  | Emp# |  manager  | Mgr#
-------------+------+-----------+------
 King        |  100 |           |
 Kochhar     |  101 | King      |  100
 ...
 Higgins     |  205 | Kochhar   |  101
 Gietz       |  206 | Higgins   |  205
(107 行记录)
```

#### 查询哪些部门没有员工

```sql
company=# SELECT d.department_id,d.department_name
company-# FROM departments d
company-# LEFT JOIN employees e USING(department_id)
company-# WHERE e.department_id IS NULL;
 department_id |   department_name
---------------+----------------------
           120 | Treasury
           270 | Payroll
           230 | IT Helpdesk
           160 | Benefits
           250 | Retail Sales
           140 | Control And Credit
           170 | Manufacturing
           180 | Construction
           190 | Contracting
           200 | Operations
           220 | NOC
           210 | IT Support
           150 | Shareholder Services
           260 | Recruiting
           130 | Corporate Tax
           240 | Government Sales
(16 行记录)
```
