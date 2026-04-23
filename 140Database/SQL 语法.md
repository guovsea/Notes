---
tags:
  - database/sql
  - type/reference
mastery: 5
---

## DDL
Data Definition Language(数据库模式定义语言)


-- 数据库操作
?
```sql
show databases;   -- 查询所有数据库
select database(); -- 查询当前数据库
create database [ if not exists ] 数据库名 [ default charset 字符集 ] [ collate 排序 规则 ];          -- 创建数据库
drop database [ if exists ] 数据库名;  -- 删除数据库
use 数据库名;       -- 切换数据库
```

-- 表操作/查询、描述、创建
?
```sql
show tables;      --  查询当前数据库所有表
desc 表名;         -- （describe）查看指定表结构描述
show create table 表名; -- 查询指定表的建表语句
-- 创建表
CREATE TABLE 表名( 
    列1 列1类型  [约束条件] [COMMENT '字段1注释,要用‘’引起来'],
    ...,
    列n 列n类型  [约束条件] [COMMENT '字段n注释'], -- 最后一个字段没有逗号
    [列约束条件],
    [列约束条件]
) [表约束条件 COMMENT '表注释'];
```
[[约束]]

[[MySql数据类型]]


-- 表操作/删除表、清空表、添加字段、修改字段数据类型、修改字段名和类型、删除字段

```sql
DROP TABLE [ IF EXISTS ] 表名;  -- 删除表
TRUNCATE TABLE 表名;            -- 删除指定表, 并重新创建表（清空）
-- 添加删除字段/约束
ALTER TABLE 表名 ADD 字段名 类型 [约束] [COMMENT 注释 ]   
        -- 添加字段
ALTER TABLE 表名 DROP 字段名;    
        -- 删除字段
-- 修改字段类型
ALTER TABLE 表名 MODIFY 字段名 新数据类型
        -- 修改字段数据类型
ALTER TABLE 表名 CHANGE 旧字段名 新字段名 类型 [ COMMENT 注释 ] [ 约束 ]
        -- 修改字段名和数据类型
-- 修改约束 PRIMARY KEY, UNIQUE, 
ALTER TABLE 表名 ADD 约束 
        -- 添加字段的约束，如果无法添加则会报错（比如要添加UNIQUE，但是表中存在重复值时）
ALTER TABLE 表名 DROP 约束
-- 修改默认值
ALTER TABLE 表名 ALTER COLUMN 字段名 SET DEFAULT 默认值
ALTER TABLE 表名 ALTER COLUMN 字段名 DROP DEFAULT
        -- 删除默认值
-- 修改字段值是否为空
ALTER TABLE 表名 MODIFY 字段名 字段类型 NULL
ALTER TABLE 表名 MODIFY 字段名 字段类型 NOT NULL

CREATE INDEX 索引名 ON 表名(列名)

```
`ALTER TABLE `的操作太多了太混乱了，并且也不经常用，所以不用太深究。因为每个张表表都是经过深思熟虑之后才构建的，如果还需要修改说明数据库设计得不好。并且，修改表是一种很大的变动，会影响很多业务逻辑。

[[DDL补充]]
## DML

Data Manipulation Language(数据操作语言)

-- 添加数据/指定字段添加、不指定字段全部添加、指定/不指定批量添加
?
```sql
INSERT INTO 表名 (字段名1, 字段名2, ...) VALUES (值1, 值2, ...); 
        -- 给指定字段添加数据
INSERT INTO 表名 VALUES (值1, 值2, ...);
        -- 给全部字段添加数据
-- 批量添加
INSERT INTO 表名 (字段名1, 字段名2, ...) VALUES (值1, 值2, ...), (值1, 值2, ...), (值 1, 值2, ...);
INSERT INTO 表名 VALUES (值1, 值2, ...), (值1, 值2, ...), (值1, 值2, ...) ;
```

-- 修改数据
?
```sql
UPDATE 表名 SET 字段名1= 值1, 字段名2= 值2,....[WHERE 条件 ];
        -- 如果没有条件，则会修改整张表的所有数据
```

-- 删除数据
?
```sql
DELETE FROM 表名 [WHERE 条件 ];
```
## DQL

-- 基本语法
?
```sql
SELECT   
    字段1 [AS 别名1], 字段2 [AS 别名2] ...
FROM
    表名列表
WHERE
    条件列表
GROUP BY
    分组字段列表
HAVING
    分组后条件列表
ORDER BY
    字段1 [ASC | DESC], 字段1 [ASC | DESC] ... -- 默认 ASC 升序
        -- 先根据第一个字段排序，再根据第二个排序，依次类推
LIMIT
    起始索引, 查询记录数
```
[[条件列表]]

[[聚合函数]]

**LIMIT**注意事项:  
- 起始索引从0开始，起始索引 = (查询页码 - 1)* 每页显示记录数。  
- 分页查询是数据库的方言，不同的数据库有不同的实现，MySQL 中是 LIMIT。 
- 如果查询的是第一页数据，起始索引可以省略，直接简写为 limit 10。

**执行顺序**

```sql
哪张表-> 哪些行 ->  对行分组 -> 哪些分组 -> 显示哪些 -> 怎么排序 -> 是否分页
FROM -> WHERE -> GROUP BY -> HAVING -> SELECT -> ORDER BY -> LIMIT
-- 执行顺序在前面的语句中的别名在后面的语句中可以使用
```
## DCL

Data Control Language(数据控制语言)。用来管理数据库用户、控制数据库的访 问权限。

-- 管理用户/查询用户、创建用户、修改用户名密码、删除用户、
?
```sql
select * from mysql.user;   -- 查询用户
CREATE USER '用户名'@'主机名' IDENTIFIED BY '密码';
        -- 创建用户
ALTER USER '用户名'@'主机名' IDENTIFIED WITH mysql_native_password BY '新密码'
        -- 修改用户名密码
DROP USER '用户名'@'主机名'; -- 删除用户

-- 主机名可以使用 % 通配
-- 这类SQL开发人员操作的比较少，主要是DBA( Database Administrator 数据库 管理员)使用。
```
-- 权限控制 (有点问题，反正也是 DBA 的事)
```sql
SHOW GRANTS FOR '用户名'@'主机名' ;
        -- 查询权限
GRANT 权限列表 ON 数据库名.表名 TO '用户名'@'主机名';
        -- 授予权限
REVOKE 权限列表 ON 数据库名.表名 FROM '用户名'@'主机名';
        -- 撤销权限
-- 多个权限之间，使用逗号分隔  
-- 授权时， 数据库名和表名可以使用 * 进行通配，代表所有。

权限列表：
ALL, ALL PRIVILEGES 所有权限  
SELECT 查询数据  
INSERT 插入数据  
UPDATE 修改数据  
DELETE 删除数据  
ALTER  修改表  
DROP   删除数据库/表/视图 
CREATE 创建数据库/表
```

