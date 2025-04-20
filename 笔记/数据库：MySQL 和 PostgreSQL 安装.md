# 数据库概述与 MySQL 和 PostgreSQL 安装

在正式介绍 MySQL 和 PostgreSQL 之前，我们需要回答下面两个问题

> [!question] 
> + 什么是数据库？
> 	+ 数据库是结构化格式存储和组织的数据。通常我们说的数据库是数据库管理系统
> + 为什么要使用数据库？
> 	+ 为了持久化数据，进程产生的数据存在在主存中，这是临时存储的。为了将主存中的数据保存下来，并且方便后续检索，就必须使用数据库管理系统。

## 数据库(DB)与数据库管理系统(DBMS)

**数据库**(Database, DB) 是 **存储数据的仓库**，本质上就是一个 **文件系统**。保存了一系列有数据结构

> [!tip] 数据库就是存储在磁盘上的文件夹，是 **数据的结构化集合**

> [!example] 
> + 当您想听自己喜欢的歌曲时，可以从智能手机打开播放列表。在这种情况下，播放列表本质上是一个数据库
> + 当您拍摄照片并将其上传到您在社交网络上的帐户时，您的照片库就变成了一个数据库
> 

数据库中的数据要么存在某种 **关系**，要么 **独立存在**。如果数据库中的数据存在某种关系，称为关系数据库；否则，称为非关系数据库。

**数据库管理系统**(Database Management System, DBMS) 是一种 **操纵** 和 **管理** 数据库的 **软件**。用于**建立**、**使用**和 **维护** 数据库，对数据库进行统一管理和控制。用户通过数据库管理系统访问数据库中表内的数据

> [!tip] 
> 
> DBMS 可以管理多个数据库，一般开发人员会针对 **每一个应用创建一个数据库**。为保存应用中实体的数据，一般会在 **数据库创建多个表**，以保存程序中实体用户的数据
> 

> [!tip] 
> 
> 我们常说的数据库是指数据库管理系统，而非真正的数据库
> 

### 关系型数据库

**关系数据库** 是一种数据库管理系统 (**DBMS**)，它以 **结构化格式** 存储和组织数据。这些表由 **行** 和 **列** 组成

| 术语    | 别称         |
| :---- | ---------- |
| 结构化格式 | **表**      |
| 行     | **记录**，或元组 |
| 列     | **字段**，或属性 |

> [!important] 
> 
> **关系** 源于这些表可以通过 **键** 和 **关系** 相互关联
> 

> [!tip] 关系型数据库的优势
> + 支持 **事务**：通过事务的 ACID 合规性实现的强大数据完整性
> + 支持 **SQL**，即结构化查询语言：“通过 SQL 提供强大的查询功能
> + 支持 **数据关系**：通过 **外键** 和 **连接** 对数据关系提供强大的支持
> 
> 它们具有高度的垂直可扩展性，可以可靠地处理复杂的事务
> 

> [!attention] 关系数据库的局限性
> 
> **难以进行水平扩展**：这可能会限制高度分布式系统的性能
> 
> 它们在 **架构更改方面可能不太灵活**，通常需要付出大量努力才能修改现有结构
> 

参考 [RDBMS 的 15 个优点和缺点](https://trainings.internshala.com/blog/advantages-and-disadvantages-of-rdbms/)

### SQL：结构化查询语言

SQL 是用于访问关系型数据库的标准化语言。**ANSI/SQL 定义了 SQL 标准**，当前 SQL 版本是 **SQL:2023**。当我们提到 SQL 标准时，我们谈论的是当前的 SQL 版本

SQL 由三部分组成：**数据定义语言**、**数据操作语言** 和 **数据控制语言**

> [!tip] 数据定义语言：Data definition language (**DDL**)
> 
> 用于定义数据库及其对象（例如表、视图、触发器、存储过程等）的语句
> 

> [!tip] 数据操作语言：Data manipulation language (**DML**)
> 
> 用于更新和查询数据的语句
> 

> [!tip] 数据控制语言：Data control language (**DCL**)
> 
> 允许授予用户访问数据库中特定数据的权限
> 

### 非关系型数据库

非关系型数据库，可看成传统关系型数据库的功能阉割版本 ，基于键值对存储数据，不需要经过 SQL 层的解析， 性能非常高 。同时，通过减少不常用的功能，进一步提高性能。

非关系型数据库有多种种类：**键值型数据库**、 **文档型数据库**、**搜索引擎数据库**

**键值型数据库**: 通过 `Key-Value` 键值的方式来存储数据，其中 `Key` 和 `Value` 可以是简单的对象，也可以是复杂的对象
- `Key` 作为唯一的标识符，优点是查找速度快，在这方面明显优于关系型数据库，缺点是无法像关系型数据库一样使用条件过滤（比如 `WHERE`），如果你不知道去哪里找数据，就要遍历所有的键，这就会消耗大量的计算。
- 键值型数据库典型的使用场景是作为 **内存缓存** 。 **Redis 是最流行的键值型数据库**

**文档型数据库**: 可存放并获取文档，可以是 `XML`、`JSON` 等格式。在数据库中文档作为处理信息的基本单位，一个文档就相当于一条记录。**文档数据库所存放的文档，就相当于键值数据库所存放的“值”**
- **`MongoDB` 是最流行的文档型数据库**。此外，还有 `CouchDB` 等

**搜索引擎数据库**: **采用了索引提升检索效率**，但是 **针对全文索引效率却较低**。搜索引擎数据库是应用在搜索引擎领域的数据存储形式，由于搜索引擎会爬取大量的数据，并以特定的格式进行存储，这样在检索的时候才能保证性能最优。核心原理是 **“倒排索引”**。
- 典型产品：`Solr`  `Elasticsearch` `Splunk` 等

## MySQL 安装

**MySQL** 是一个强大的**数据库管理系统**，**专为管理关系数据库而设计**。它是 Oracle 支持的开源软件，这意味着您可以 **免费使用 MySQL**。此外，还可以灵活地 **修改其源代码**，以适应您的特定要求。

> [!note] 
> 
> MySQL 的名字来源于其联合创始人之一 Monty Widenius 的女儿，她的名字是 My。将“My” 与 “SQL” 组合起来，我们就得到了 MySQL。
> 

MySQL 具有通用性，可以在各种平台上运行，包括 UNIX、Linux 和 Windows。您可以将其安装在服务器上，甚至可以安装在桌面上。此外，MySQL 以其可靠性、可扩展性和速度而闻名

### Windows 上安装 MySQL

MySQL 官方提供了 Windows 安装程序，随安装程序的引导完成安装即可。参考 [mysqltutorial-install-mysql](https://www.mysqltutorial.org/getting-started-with-mysql/install-mysql/)

此外，我们还可以使用 [scoop](https://scoop.sh/) 进行安装

```shell
scoop install mysql
scoop install extras/vcredist2022   # 这是 mysql 推进下载的内容
# scoop mysql 会将 mysql初始化完成，并将 root 密码设置为空
mysqld --install MySQL --defaults-file="C:\scoop\apps\mysql\current\my.ini"  # 安装mysql服务
```

> [!tip] 
> 
> 采用 scoop 安装的 MySQL，会完成初始化操作，并将 `root` 用户的密码被设置为空
> 

### Linux 上安装 MySQL

在 Red Hat 系的 Linux 发行版上，使用包管理工具 `dnf` 可以直接安装 MySQL

```shell
# 安装 MySQL 服务端和客户端
sudo dnf install mysql-server mysql
# 启动服务端：这里的 root 用户以空密码创建
sudo systemctl enable --now mysqld
```

在 Archlinux 上，使用包管理工具 `pacman` 也可以直接安装 MySQL。但是需要对 MySQL 进行初始化

```shell
# 安装 MySQL
sudo pacman -S mysql
# 初始化
sudo mysqld --initialize --user=mysql --basedir=/usr --datadir=/var/lib/mysql
```

注意初始化过程中的日志输出，里面有一行 `"temporary password"`，这一行记录了 MySQL 服务器初始化后 `root` 用户的密码

### 保护 MySQL 服务器

下面执行命令 `mysql_secure_installation` 以保护 MySQL 服务器

```shell
➜ ~ mysql_secure_installation
Securing the MySQL server deployment.

Enter password for user root: *************
The 'validate_password' component is installed on the server.
The subsequent steps will run with the existing configuration
of the component.
Using existing password for root.

Estimated strength of the password: 100
Change the password for root ? ((Press y|Y for Yes, any other key for No) : y

New password: *************

Re-enter new password: *************

Estimated strength of the password: 100
Do you wish to continue with the password provided?(Press y|Y for Yes, any other key for No) : y
By default, a MySQL installation has an anonymous user,
allowing anyone to log into MySQL without having to have
a user account created for them. This is intended only for
testing, and to make the installation go a bit smoother.
You should remove them before moving into a production
environment.

Remove anonymous users? (Press y|Y for Yes, any other key for No) : y
Success.


Normally, root should only be allowed to connect from
'localhost'. This ensures that someone cannot guess at
the root password from the network.

Disallow root login remotely? (Press y|Y for Yes, any other key for No) : y
Success.

By default, MySQL comes with a database named 'test' that
anyone can access. This is also intended only for testing,
and should be removed before moving into a production
environment.


Remove test database and access to it? (Press y|Y for Yes, any other key for No) : n

 ... skipping.
Reloading the privilege tables will ensure that all changes
made so far will take effect immediately.

Reload privilege tables now? (Press y|Y for Yes, any other key for No) : y
Success.

All done!
```
### 连接 MySQL 服务器

连接 MySQL 服务器需要使用 MySQL 客户端，包括 **命令行客户端** 或 **图像界面客户端**。MySQL 官方提供了这两种类型的客户端。我们主要使用命令行客户端

```shell
# mysql 命令行客户端的基本使用
mysql -h 主机名 -P 端口号 -u 用户名 -p
```

+ `-h 主机名`：指定 MySQL 服务器所在的主机名
+ `-P 端口号`：指定 MySQL 服务运行的端口
+ `-u 用户名`：连接 MySQL 服务器的用户
+ `-p` 表示需要密码

## PostgreSQL 安装

### Windows 上安装 PostgreSQL

使用 PostgreSQL 官方提供的安装程序，可以很方便的安装。参考 [install-postgresql](https://neon.tech/postgresql/postgresql-getting-started/install-postgresql)

此外，我们还可以使用 [scoop](https://scoop.sh/) 进行安装

```shell
scoop install postgresql
```

安装完成后，scoop 给出下列提示

```shell
Run run 'pg_ctl start' or 'pg_ctl stop' to start and stop the database or register it as a service by running 'pg_ctl register -N PostgreSQL' from an elevated shell. Default superuser login: postgres, password: <blank>
```

我们可以使用 `pg_ctl` 启动或终止 PostgreSQL 服务器。或者将其注册为系统服务。这里，我们选择将其注册为系统服务

```shell
# 注册为系统服务
sudo pg_ctl register -N PostgreSQL
# 启动服务
sudo sc start PostgreSQL
```

### Linux 上安装 PostgreSQL

在 Red Hat 系的 Linux 发行版上，使用包管理工具 `dnf` 可以直接安装 PostgreSQL

```shell
# 按照仓库 RPM 
sudo dnf install -y https://download.postgresql.org/pub/repos/yum/reporpms/EL-9-x86_64/pgdg-redhat-repo-latest.noarch.rpm  

# 禁用内置 PostgreSQL 模块
sudo dnf -qy module disable postgresql 

# 安装 PostgreSQL
sudo dnf install -y postgresql17-server 

# 可选择初始化数据库并启用自动启动
sudo /usr/pgsql-17/bin/postgresql-17-setup initdb  
sudo systemctl enable postgresql-17  
sudo systemctl start postgresql-17
```

在 Ubuntu 上，按照下列步骤配置

```shell
# 导入存储库签名密钥:
sudo apt install curl ca-certificates
sudo install -d /usr/share/postgresql-common/pgdg
sudo curl -o /usr/share/postgresql-common/pgdg/apt.postgresql.org.asc --fail https://www.postgresql.org/media/keys/ACCC4CF8.asc

# 创建存储库配置文件:
sudo sh -c 'echo "deb [signed-by=/usr/share/postgresql-common/pgdg/apt.postgresql.org.asc] https://apt.postgresql.org/pub/repos/apt $(lsb_release -cs)-pgdg main" > /etc/apt/sources.list.d/pgdg.list'

# 更新包列表:
sudo apt update

# 安装最新版本的PostgreSQL:
sudo apt -y install postgresql
# 如果您需要特定的版本，请使用 postgresql-16 或类似包名，而不是 postgresql

# 启动 PostgreSQL 服务
sudo systemctl enable postgresql
sudo systemctl start postgresql
sudo /usr/lib/systemd/systemd-sysv-install enable postgresql
```

### 连接 PostgreSQL 服务器

如果通过 UNIX domain 登陆 PostgreSQL 默认使用 `peer` 验证。即，使用系统用户进行验证。 `postgres` 用户是安装 PostgreSQL 时创建的超级用户。因此，连接 PostgreSQL 服务器可以使用如下命令

```shell
sudo -u postgres psql
```

然后，设置 `postgres` 用户的密码

```sql
ALTER USER postgres PASSWORD '<password>';
```

如果需要在 UNIX domain 登陆 PostgreSQL  采用密码认证，需要将配置文件 `pg_hba.conf` 中的

```shell
# Database administrative login by Unix domain socket
local   all             postgres                                peer
# TYPE  DATABASE        USER            ADDRESS                 METHOD

# "local" is for Unix domain socket connections only
local   all             all                                     peer
```

修改为

```shell
# Database administrative login by Unix domain socket
local   all             postgres                                 scram-sha-256
# TYPE  DATABASE        USER            ADDRESS                 METHOD

# "local" is for Unix domain socket connections only
local   all             all                                     scram-sha-256
```

最后，让 PostgreSQL 重新加载配置文件

```shell
sudo systemctl reload postgresql
```

现在我们就可以通过秘密登陆 PostgreSQL 服务器了。`psql` 是基于终端的应用程序。`psql` 命令的连接选项有下面几个

```shell
-h, --host=主机名        数据库服务器主机或socket目录(默认："localhost")
-p, --port=端口          数据库服务器的端口(默认："5432")
-U, --username=用户名    指定数据库用户名
-w, --no-password        永远不提示输入口令
-W, --password           强制口令提示 (自动)
```

连接本地的 PostgreSQL 服务器最简单的命令为

```shell
psql -U postgres -W
```

## 加载示例数据库

### MySQL

示例数据库下载地址 [Download MySQL Sample Database](https://www.mysqltutorial.org/wp-content/uploads/2023/10/mysqlsampledatabase.zip)。该示例数据库架构由下表组成
+ **customers**：存储客户的数据
+ **products**：存储比例模型汽车列表
+ **productlines**：存储产品线列表
+ **orders**：商店由客户下达的销售订单
+ **orderdetails**：存储每个销售订单的销售订单行项目
+ **payments**：存储客户根据其帐户进行的付款
+ **employees**：存储员工信息和组织结构
+ **offices**：存储销售办公室数据

下图是示例数据库的 **E-R 图**

![[Pasted image 20241224102843.png]]

数据库下载完成之后，使用 `mysql` 客户端登录到 MySQL 服务器

```shell
mysql -u root -p
```

使用 `source` 命令将数据加载到 MySQL 服务器中

```sql
mysql> source $HOME\mysqlsampledatabase.sql
Query OK, 1 row affected (0.04 sec)
```

### PostgreSQL 

下面将 MySQL 数据库导入到 PostgreSQL 中。首先，在 PostgreSQL 中准备好目标数据库

```sql
CREATE DATABASE classicmodels;
```

然后使用 Navicat 的 数据传输工具，从 MySQL 服务端传输到 PostgreSQL 服务端中

![[Pasted image 20241224231745.png]]

![[Pasted image 20241224231807.png]]

![[Pasted image 20241224231855.png]]

现在，我们就在 MySQL 和 PostgreSQL 中准备好了示例数据库

