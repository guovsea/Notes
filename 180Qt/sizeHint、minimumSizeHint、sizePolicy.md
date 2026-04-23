---
tags:
  - qt/ui
  - type/concept
mastery: 5
---
## 相关

- [[Layout]] -- sizeHint 与 Layout 共同决定控件的实际尺寸

## sizeHint

[QT sizeHint 及 Policy的用法-CSDN博客](https://blog.csdn.net/qq_40732350/article/details/86703749)

```cpp
{
    QWidget *widget = new QWidget;
    widget->show();
    widget->setWindowTitle("child window");
    qDebug() << widget->sizeHint().isValid();
    qDebug() << widget->sizeHint();

    qDebug() << sizeHint().isValid();
    qDebug() << sizeHint();
}
```
````text
false           // 没有设置布局
QSize(-1, -1)
true            // QMainWindow 是有布局的
QSize(0, 0)
````

1) 在 widget 有 layout 的情况下，其 sizeHint() 函数返回的值有效，作为其自身实际尺寸的参考；  
2) sizeHint () 返回的值并不一定会作为 widget 的实际尺寸，因为 widget 的尺寸的决定还有其它因素作用；

## minimumSizeHint

Widget 最小 size 的推荐值

可以被 minimumSize 属性覆盖，换句话说：没有 set minimumSize 时，才会生效。


## sizePolicy

sizePolicy 控制着 sizeHint 属性会如何影响 Widget 的实际尺寸。

[[Layout]]

保存了该 widget 的默认布局属性，如果它有一个 layout 来布局其子 widgets，那么 sizePolicy 不会生效；如果该 widget 没有 layout ，那么它的 size policy 将被使用。默认的 policy 是 Preferred/Preferred。

## Related

- [[Layout]] -- 布局决定窗口的实际大小
