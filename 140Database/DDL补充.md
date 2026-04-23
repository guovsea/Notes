---
tags:
  - database/sql
  - type/reference
mastery: 5
---

**关键字支持的操作**

**ADD**

1. 添加字段

2. 添加 `PRIMARY KEY`、`UNIQUE`、`FOREIGN KEY`

```sql
ALTER TABLE table_name [ADD CONSTRAINT constraint_name] UNIQUE | PRIMARY KEY | FOREIGN KEY ...;
```

**MODIFY**

1. 修改字段的类型

```sql
ALTER TABLE table_name MODIFY 字段名 新类型 [NULL | NOT NULL | DEFAULT | AUTO_INCREMENT];
```

**CHANGE**

1. 修改列名
```sql
ALTER TABLE 表名 CHANGE 旧字段名 新字段名 类型 [ COMMENT 注释 ] [ 约束 ]
```

**ALTER COLUMN**

1. 添加默认值
```sql
ALTER TABLE 表名 ALTER COLUMN 字段名 SET DEFAULT 默认值
```

**DROP**

1. 删除列
1. 删除约束
```sql
ALTER TABLE 表名 DROP 字段名;  
ALTER TABLE 表名 DROP 约束

```
[MySQL 8.0 参考手册\_MySQL 8.0 参考手册](https://mysql.net.cn/doc/refman/8.0/en/)

## See also

- [[SQL 语法]] - SQL DDL/DML/DQL/DCL 完整语法

