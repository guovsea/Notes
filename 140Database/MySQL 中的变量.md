---
tags:
  - database/sql
  - type/reference
mastery: 5
---

## 系统变量

系统变量是 MySQL 服务器提供，不是用户定义的，属于服务器层面。分为全局变量(GLOBAL)、会话变量(SESSION)。

- 全局变量(GLOBAL): 全局变量针对于所有的会话。  
- 会话变量(SESSION): 会话变量针对于单个会话，在另外一个会话窗口就不生效了。
### 查看系统变量
```sql
SHOW [ SESSION | GLOBAL ] VARIABLES ; -- 查看所有系统变量  
SHOW [ SESSION | GLOBAL ] VARIABLES LIKE '...'; -- 可以通过LIKE模糊匹配方式查找变量

SELECT @@[SESSION | GLOBAL].系统变量名;  -- 查看指定变量的值
```
**注意**

如果没有指定 SESSION/GLOBAL，默认是 SESSION，会话变量。
>mysql 服务重新启动之后，所设置的全局参数会失效，要想不失效，可以在 /etc/my.cnf 中配置。

### 设置系统变量

```sql
SET [ SESSION | GLOBAL ] 系统变量名 = 值 ; 
SET @@[SESSION | GLOBAL].系统变量名 = 值 ;
```
```sql
-- 查看系统变量  
show session variables ;

show session variables like 'auto%';
show global variables like 'auto%';

select @@global.autocommit; 
select @@session.autocommit;

-- 设置系统变量  
set session autocommit = 1;

set global autocommit = 0;

set @@global.autocommit = 0;

select @@autocommit = 0;  -- 默认是 SESSSION 变量
```

## See also

- [[存储过程]] - 存储过程中使用局部变量
- [[MySQL中的流程控制]] - 流程控制中使用变量

## 用户定义变量

用户根据需要自己定义的变量，用户变量不用提前声明，在用的时候直接用 `@变量名` 使用就可以。
### 赋值

```sql

SET @var_name = expr [, @var_name = expr] ... ;   -- 可以同时设置多个变量的值
SET @var_name := expr [, @var_name := expr] ... ;

SELECT @var_name := expr [, @var_name := expr] ... ; -- select 设置变量值必须用 := 
SELECT 字段名 INTO @var_name FROM 表名;  -- select 的结果必须只有1个元素
``` 
### 使用

```sql
SELECT @var_name ;
```
用户定义的变量无需对其进行声明或初始化，只不过获取到的值为NULL
## 局部变量

局部变量是根据需要定义的在局部生效的变量，访问之前，需要 DECLARE 声明。可用作存储过程内的局部变量和输入参数，局部变量的范围是在其内声明的BEGIN ... END块。

### 声明

```sql
DECLARE 变量名 变量类型 [DEFAULT ... ] ; --DEFAULT 默认值 
-- 变量类型就是数据库字段类型:INT、BIGINT、CHAR、VARCHAR、DATE、TIME等。
```

### 赋值

```sql
SET 变量名=值;  
SET 变量名:=值;  
SELECT 字段名 INTO 变量名 FROM 表名 ... ;
```
演示示例:
```sql
-- 声明局部变量 - declare -- 赋值  
create procedure p2() begin
    declare stu_count int default 0; -- 声明
    select count(*) into stu_count from student; -- 赋值
    select stu_count;  -- 返回值
end;

call p2();
```
