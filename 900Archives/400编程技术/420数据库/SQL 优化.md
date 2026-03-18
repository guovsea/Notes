## 插入数据

### insert

1. 批量插入数据
2. 手动控制事务
3. 主键顺序插入，使用自增主键

自增主键是连续的，在插入过程中尽量减少页分裂，即使要进行页分裂，也只会分裂很少一部分。并且能减少数据的移动，每次插入都是插入到最后。总之就是减少页分裂和移动数据的次数。

### 大批量插入数据

```shell
-- 客户端连接服务端时，加上参数 -–local-infile 
mysql –-local-infile -u root -p

-- 设置全局参数local_infile为1，开启从本地加载文件导入数据的开关 
set global local_infile = 1;

-- 执行load指令将准备好的数据，加载到表结构中  
load data local infile '/root/sql1.log' into table tb_user fields terminated by ',' lines terminated by '\n' ;
```
## 主键优化

1. 满足业务需求的情况下，尽量降低主键的长度。
2. 插入数据时，尽量选择顺序插入，选择使用 AUTO_INCREMENT 自增主键。
3. 尽量不要使用自然主键 (如身份证)和 UUID 做主键 
4. 业务操作时，避免对主键的修改。

## ORDER BY 优化

- Using filesort : 通过表的索引或全表扫描，读取满足条件的数据行，然后在排序缓冲区 sort buffer 中完成排序操作，**所有不是通过索引**直接返回排序结果的排序都叫 FileSort 排序。
- Using index : **通过有序索引顺序扫描直接返回有序数据**，这种情况即为 using index，不需要额外排序，操作效率高。

对于以上的两种排序方式，Using index 的性能高，而 Using filesort 的性能低，在优化排序操作时，尽量要优化为 Using index。

优化方法： 
- **排序字段使用索引**，同样也适用[[最左前缀法则]]。
- **尽量使用覆盖索引。**
- **倒序排序时创建倒序索引。**
- 如果不可避免的出现 `filesort` ，大数据量排序时，可以**适当增大排序缓冲区大小** `sort_buffer_size`(默认256k)。
```sql
create index idx_user_age_phone_ad on tb_user(age asc ,phone desc);
```
## GROUP BY 优化

可以通过索引来提高效率，索引的使用也满足[[最左前缀法则]]的。

## LIMIT 优化

优化思路: 一般分页查询时，通过创建覆盖索引能够比较好地提高性能，可以通过覆盖索引加子查询形式进行优化。
```sql
explain select * from tb_sku t , (select id from tb_sku order by id limit 2000000,10) a where t.id = a.id;
```
## COUNT 优化

**数据量大，COUNT 操作非常费时**，因此可以自己计数（如借助 Redis ）
- MyISAM 引擎把一个表的总行数存在了磁盘上，因此执行 `count(*) `的时候会直接返回这个数，效率很高; 但是如果是带条件的 count，MyISAM 也慢。  
- InnoDB 引擎就麻烦了，它执`行 count(*`) 的时候，需要把数据一行一行地从引擎里面读出来，然后累积计数。

**COUNT 的用法：**  
`COUNT(*)、COUNT(主键)、COUNT(字段)、COUNT(数字)`
原理：
- `COUNT(主键)`
  InnoDB 引擎会遍历整张表，把每一行的主键 id 值都取出来，返回给服务层。服务层拿到主键后，直接按行进行累加 (主键不可能为 null)
- `COUNT(字段)`
  没有 not null 约束：InnoDB 引擎会遍历整张表把每一行的字段值都取出来，返回给服务层，服务层判断是否为 null，不为 null，计数累加。 
  有 not null 约束：InnoDB 引擎会遍历整张表把每一行的字段值都取出来，返回给服务层，直接按行进行累加。
- `COUNT(数字)`
  InnoDB 引擎遍历整张表，但不取值。服务层对于返回的每一行，放一个数字“1” 进去，直接按行进行累加。
- `COUNT(*)`
  InnoDB 引擎并不会把全部字段取出来，而是专门做了优化，不取值，服务层直接按行进行累加。

**效率排序**
**`COUNT(字段) < COUNT(主键) < COUNT(1) = COUNT(*)`**

## update优化

InnoDB的行锁是针对索引加的锁，不是针对记录加的锁 ,并且该索引不能失效，否则会从行锁升级为表锁。

因此，**update 操作的查询条件最好存在索引**

