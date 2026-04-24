---
---

## 概念

一个 concept 是编译器可求值的针对模板参数的约束集合。

## 用法

可以用 `concept` 关键字来定义一个 concept，由 `requires` 关键字来引出该 concept 的约束条件。

可以在 template parameter list 后面使用 `requires` 关键字声明此模板所需要满足的 constraint。
## 作用

用来代替 SFINAE，能够写出更易读的约束以及产生更友好的报错信息

