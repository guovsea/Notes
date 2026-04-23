---
tags:
  - database/mysql
  - type/concept
mastery: 5
---

## Collation 和字符集

**Collation 是指根据某些规则对字符集进行了整理排序，确定了字符之间的关系**

- 每个[[字符集]]会对应多个 Collation
```shell
mysql> show collation like 'latin1%';
+-------------------+---------+----+---------+----------+---------+---------------+
| Collation         | Charset | Id | Default | Compiled | Sortlen | Pad_attribute |
+-------------------+---------+----+---------+----------+---------+---------------+
| latin1_bin        | latin1  | 47 |         | Yes      |       1 | PAD SPACE     |
| latin1_danish_ci  | latin1  | 15 |         | Yes      |       1 | PAD SPACE     |
| latin1_general_ci | latin1  | 48 |         | Yes      |       1 | PAD SPACE     |
| latin1_general_cs | latin1  | 49 |         | Yes      |       1 | PAD SPACE     |
| latin1_german1_ci | latin1  |  5 |         | Yes      |       1 | PAD SPACE     |
| latin1_german2_ci | latin1  | 31 |         | Yes      |       2 | PAD SPACE     |
| latin1_spanish_ci | latin1  | 94 |         | Yes      |       1 | PAD SPACE     |
| latin1_swedish_ci | latin1  |  8 | Yes     | Yes      |       1 | PAD SPACE     |
+-------------------+---------+----+---------+----------+---------+---------------+
8 rows in set (0.00 sec)
```
## Collation

- collation 定义了哪个字符和哪个字符是“等价”的。
- collation 也定义了字符的排序规则

一单、但涉及到字符的比较和匹配，便需要用到这些规则

**不同的 Collation**

- **utf8mb4_0900_ai_ci** 不区分音调、不区分大小写（一般用这个）
  
  0900，它对应的是 Unicode 9.0 的规范。
  
  \_ai_ci，ai 表示 accent insensitivity，也就是“不区分音调”，而 ci 表示 case insensitivity，也就是“不区分大小写”
  
- utf8mb4_0900_as_ci 同理

- utf8mb4_0900_as_cs

## See also

- [[字符集]] - 字符集详解
- [[约束]] - 建表时可指定 CHARSET 和 COLLATE