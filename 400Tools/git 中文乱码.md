---
tags:
  - tools/git
  - type/pitfall
mastery: 5
---

# Git 中文文件名乱码

> Git 默认会转义非 ASCII 字符（如中文文件名），导致 `git status` 显示乱码。

## 解决方法

```shell
# 设置 Git 不转义非 ASCII 字符
git config --global core.quotepath false
```