---
tags:
  - database/sql
  - type/reference
mastery: 5
---

条件处理程序(Handler)可以用来定义在流程控制结构执行过程中遇到问题时相应的处理步骤。具体语法为:

```sql
DECLARE handler_action HANDLER FOR condition_value [, condition_value] ... statement ;

-- handler_action 的取值: 
    -- CONTINUE: 继续执行当前程序 
    -- EXIT: 终止执行当前程序

-- condition_value 的取值:  
    -- SQLSTATE 状态码，如 02000
    -- SQLWARNING: 所有以01开头的SQLSTATE代码的简写  
    -- NOT FOUND: 所有以02开头的SQLSTATE代码的简写  
    -- SQLEXCEPTION: 所有没有被SQLWARNING 或 NOT FOUND捕获的SQLSTATE代码的简写
```
具体的错误状态码，可以参考官方文档:

https://dev.mysql.com/doc/refman/8.0/en/declare-handler.html 

https://dev.mysql.com/doc/mysql-errors/8.0/en/server-error-reference.html

## See also

- [[存储过程]] - 存储过程中使用 Handler 处理异常
- [[游标]] - 游标遍历结合 Handler 处理结束条件