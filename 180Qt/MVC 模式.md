---
tags:
  - qt/core
  - type/concept
mastery: 5
---
## 相关

- [[QVarient]] -- MVC 中模型数据常用 QVariant 包装

## MVC 基本概念

模型（Model）、视图（View）和控制器（Controller）是一种将数据、界面和业务逻辑分离的设计模式：

- **Model**：管理和维护数据，提供数据访问接口
- **View**：负责数据的展示和用户交互界面
- **Controller**：处理用户输入，协调 Model 和 View

[谈谈MVC模式 - 阮一峰的网络日志](https://www.ruanyifeng.com/blog/2007/11/mvc.html)

## Qt 中的 Model/View 架构

Qt 采用了变体的 MVC 模式，将 Controller 的功能分散到 View 和 Delegate 中：

| 组件 | Qt 类 | 职责 |
|------|-------|------|
| **Model** | `QAbstractItemModel` | 管理数据，提供统一的访问接口 |
| **View** | `QListView`, `QTableView`, `QTreeView` | 展示数据 |
| **Delegate** | `QStyledItemDelegate` | 控制单个数据项的渲染和编辑 |

### Qt 提供的预置 Model

- `QStringListModel` -- 简单的字符串列表
- `QStandardItemModel` -- 通用的树形/表格数据模型
- `QFileSystemModel` -- 文件系统模型
- `QSqlQueryModel` / `QSqlTableModel` -- 数据库模型

### 自定义 Model

需要继承 `QAbstractItemModel` 并实现以下核心虚函数：

```cpp
// 必须实现
virtual int rowCount(const QModelIndex &parent) const = 0;
virtual QVariant data(const QModelIndex &index, int role) const = 0;

// 可编辑模型需要额外实现
virtual bool setData(const QModelIndex &index, const QVariant &value, int role);
virtual Qt::ItemFlags flags(const QModelIndex &index) const;
```
