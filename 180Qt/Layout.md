---
tags:
  - qt/ui
  - type/concept
mastery: 5
---
## 相关

- [[sizeHint、minimumSizeHint、sizePolicy]] -- sizeHint 与 Layout 共同决定控件尺寸

## 核心概念

指定布局实际上是在指定 `setLayout()` 所在窗口的 size。窗口的实际大小是根据它的子控件和 layout 计算出来的。

Layout 负责：
- 自动排列子控件的位置和大小
- 在窗口缩放时自动调整子控件布局
- 管理子控件之间的间距（spacing）和边距（margin/contentsMargins）

## 常用布局类

| 布局类 | 排列方式 | 典型用途 |
|--------|---------|---------|
| `QHBoxLayout` | 水平排列 | 工具栏、按钮行 |
| `QVBoxLayout` | 垂直排列 | 表单、设置面板 |
| `QGridLayout` | 网格排列 | 表格型界面、计算器 |
| `QFormLayout` | 标签-字段对 | 输入表单 |
| `QStackedLayout` | 堆叠（一次显示一个） | 向导页面、选项卡内容 |

## 使用方式

```cpp
QWidget *window = new QWidget;
QVBoxLayout *layout = new QVBoxLayout;

layout->addWidget(new QPushButton("Button 1"));
layout->addWidget(new QPushButton("Button 2"));
layout->addStretch();  // 添加弹性空间

window->setLayout(layout);
window->show();
```
