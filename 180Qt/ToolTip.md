---
tags:
  - qt/ui
  - type/howto
mastery: 5
---
## 相关

- [[qss]] -- ToolTip 的外观可通过 QSS 自定义

## 基本用法

ToolTip 是鼠标悬停在控件上时显示的提示信息。

```cpp
// 设置 ToolTip
widget->setToolTip("这是一个提示");

// 支持 HTML 富文本
widget->setToolTip("<b>粗体</b><br>换行提示");

// 获取 ToolTip
QString tip = widget->toolTip();
```

## 相关属性

| 属性 | 函数 | 说明 |
|------|------|------|
| ToolTip | `setToolTip()` / `toolTip()` | 鼠标悬停提示 |
| StatusTip | `setStatusTip()` / `statusTip()` | 状态栏提示 |
| WhatsThis | `setWhatsThis()` / `whatsThis()` | "这是什么"帮助 |

## 参考

- [PyQt 入门：toolTip、statusTip、whatsThis 属性 - 博客园](https://www.cnblogs.com/LaoYuanPython/p/12003012.html)
- [Qt 控件的 toolTip 函数详解 - CSDN](https://blog.csdn.net/qq_41453285/article/details/100035965)
