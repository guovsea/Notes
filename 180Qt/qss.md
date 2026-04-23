---
tags:
  - qt/ui
  - type/reference
mastery: 5
---
## 相关

- [[ToolTip]] -- ToolTip 外观可通过 QSS 定制

## 概述

QSS（Qt Style Sheets）是 Qt 的样式表机制，语法类似 CSS，用于自定义控件的外观（颜色、字体、边框、背景等）。

## 基本语法

```css
/* 选择器 { 属性: 值; } */
QPushButton {
    background-color: #4CAF50;
    color: white;
    border-radius: 5px;
    padding: 5px;
}

QPushButton:hover {
    background-color: #45a049;
}
```

## 非 UI 生成的对象使用 QSS

通过 `setObjectName()` 为控件指定名称，然后在 QSS 中通过 `#名称` 选择：

```css
QDialog#LoginDialog{
    background-color: rgb(0, 255, 255);
}
```

```cpp
LoginDialog::LoginDialog(QWidget *parent) : QDialog(parent)
{
    setObjectName("LoginDialog");
}
```

## 常用选择器

| 选择器 | 示例 | 说明 |
|--------|------|------|
| 通用 | `*` | 匹配所有控件 |
| 类型 | `QPushButton` | 匹配指定类型 |
| 类 | `.QPushButton` | 匹配指定类（不含子类） |
| ID | `QPushButton#okButton` | 匹配指定 objectName |
| 属性 | `QPushButton[checked="true"]` | 匹配动态属性 |
| 子控件 | `QComboBox::drop-down` | 匹配子控件 |
| 伪状态 | `QPushButton:hover` | 匹配控件状态 |

## 应用 QSS 的方式

```cpp
// 1. 直接代码设置
widget->setStyleSheet("QPushButton { color: red; }");

// 2. 从文件加载
QFile file(":/style.qss");
file.open(QFile::ReadOnly);
qApp->setStyleSheet(file.readAll());
```
