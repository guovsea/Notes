---
tags:
  - qt/core
  - type/pitfall
mastery: 5
---
## 相关

- [[MVC 模式]] -- MVC 中模型数据通过 QVariant 传递
- [[JSON support in Qt]] -- QJsonValue 可通过 QVariant 转换

## 概述

QVariant 是 Qt 的通用值包装类，可以容纳多种 Qt 和 C++ 数据类型（如 int、QString、QColor、QFont 等）。它常用于模型/视图架构中的数据传递、属性系统和设置存储。

参考：[Qt之QVariant用法-CSDN博客](https://blog.csdn.net/woniulx2014/article/details/82752835)

## 踩坑

如果返回值类型是 QVariant 时，必须得返回某个值（如 `return QVariant();`），否则会报编译错误或运行时崩溃，且错误信息不直观，很难排查。

## 基本用法

```cpp
QVariant v(42);                    // 存储 int
int i = v.toInt();                 // 取出 int

QVariant s("hello");               // 存储 QString
QString str = s.toString();        // 取出 QString

// 判断类型
if (v.canConvert<int>()) {
    int val = v.value<int>();
}

// 无效 QVariant
QVariant invalid;                  // isValid() 返回 false
```
