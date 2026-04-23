---
tags:
  - qt/core
  - type/reference
mastery: 5
---
## 相关

- [[Event 系统]] -- 拖拽事件中使用 QMimeData 传递数据

## 概述

QMimeData 用于在拖拽（Drag & Drop）和剪贴板（Clipboard）操作中携带和传递数据。它通过 MIME 类型来标识数据的格式。

## 基本用法

### 设置数据

```cpp
QMimeData *mimeData = new QMimeData;
mimeData->setText("Hello World");              // 纯文本
mimeData->setUrls(QList<QUrl>() << QUrl("file:///path/to/file")); // URL

// 自定义 MIME 类型
QByteArray encodedData;
// ... 填充数据 ...
mimeData->setData("application/x-mytype", encodedData);
```

### 读取数据

```cpp
if (mimeData->hasText()) {
    QString text = mimeData->text();
}
if (mimeData->hasUrls()) {
    QList<QUrl> urls = mimeData->urls();
}
```

## 常用 MIME 类型判断函数

| 函数 | MIME 类型 | 说明 |
|------|----------|------|
| `hasText()` | `text/plain` | 纯文本 |
| `hasHtml()` | `text/html` | HTML 文本 |
| `hasUrls()` | `text/uri-list` | URL 列表 |
| `hasImage()` | `image/*` | 图像数据 |
| `hasColor()` | `application/x-color` | 颜色数据 |

## 在拖拽中的使用

```cpp
void MyWidget::mousePressEvent(QMouseEvent *event)
{
    QMimeData *mimeData = new QMimeData;
    mimeData->setText("dragged text");

    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);
    drag->exec(Qt::CopyAction);
}
```

参考：[QMimeData-CSDN博客](https://blog.csdn.net/kenfan1647/article/details/116050820)
