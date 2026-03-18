## if

```sql
IF 条件1 THEN 结果1

[ELSEIF 条件2 THEN 结果2] -- 可选

[ ... ]

[ELSE] -- 可选
END IF;
```
ELSE IF 结构可以有多个，也可以没有。 ELSE结构可以有，也可以没有

## CASE


```sql
-- 简单 CASE 函数 简单 CASE 函数仅检查是否相等!!!

CASE 条件表达式 
WHEN 值1 THEN 结果1  -- 条件表达式 == 值1
WHEN 值2 THEN 结果2  -- 条件表达式 == 值2
... 
[ELSE 默认结果] 
END; 

-- CASE 搜索函数

CASE 
WHEN 条件1 THEN 结果2 
WHEN 条件2 THEN 结果2 
... 
[ELSE 默认结果] 
END;
-- 注意:如果判定条件有多个，多个条件之间，可以使用 and 或 or 进行连接。
```
## WHILE

```sql
-- 先判定条件，如果条件为true，则执行逻辑，否则，不执行逻辑 
WHILE 条件 DO
    SQL逻辑... 
END WHILE;
```
## REPEAT

repeat 是有条件的循环控制语句, 当满足 UNTIL 声明的条件的时候，则退出循环

```sql
REPEAT
SQL 逻辑 ...
UNTIL 条件 
END REPEAT;
```
## LOOP

LOOP 实现简单的循环，如果不在 SQL 逻辑中增加退出循环的条件，可以用其来实现简单的死循环。

LOOP：可以配合一下两个语句使用: LEAVE :配合循环使用，退出循环。

ITERATE：必须用在循环中，作用是跳过当前循环剩下的语句，直接进入下一次循环。

```sql
[begin_label:] LOOP 
SQL逻辑...
END  LOOP [end_label];|

LEAVE label; -- 退出指定标记的循环体
ITERATE label; -- 直接进入下一次循环 ITERATE : iterate，迭代
```
```sql
-- 示例
-- A. 定义局部变量, 记录累加之后的值;  
-- B. 每循环一次, 就会对 n 进行 -1 , 如果n减到0, 则退出循环 ----> leave xx 
-- C. 如果当次累加的数据是奇数, 则直接进入下一次循环. --------> iterate xx

create procedure p10(in n int)
begin
    declare total int default 0;
sum:loop  
    -- n <= 0 则退出循环
    if n <= 0 then
        leave sum;
    end if;
    -- n 为奇数则直接进入下一次循环
    if n % 2 = 1 then 
        set n := n - 1;
        iterate sum;
    end if;
    -- 累加为偶数的的n
    set total := total + n;
    set n := n - 1; 
end loop sum;
select total;
end;

call p10(100);
```