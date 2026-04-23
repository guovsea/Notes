---
tags:
  - qt/event
  - type/concept
mastery: 5
---
## 相关

- [[信号和槽]] -- 信号槽机制与事件系统是 Qt 的两大通信机制

## QEvent

事件产生时、传递给上层时，isAccepted 默认为 true 也就是说，Qt 默认**重载了某个事件处理函数的自定义控件会 accept 相应事件，并不会向上传递**。如果想向上传递，必须得调用 `event->ignore()` 将 isAccepted 设置为 false。

> 如果传递给上一层的自定义控件之后，上一层没有调用 `event->ignore()`，那么事件不会再向上传递。

事件处理函数：

- keyPressEvent
- keyReleaseEvent
- mousePressEvent
- mouseReleaseEvent
- mouseMoveEvent
- wheelEvent
- focusInEvent
- focusOutEvent
- resizeEvent
- closeEvent

事件分发：

- `bool QObject::event(QEvent *e)`

事件分发会在事件处理函数调用之前

如果传入的事件已被识别并且处理，则需要返回 true，否则返回 false。

如果返回值是 true，并且，该事件对象设置了`accept()`，那么 Qt 会认为这个事件已经处理完毕，不会再将这个事件发送给其它对象，而是会继续处理事件队列中的下一事件。

注意，在`event()`函数中，调用事件对象的`accept()`和`ignore()`函数是没有作用的，不会影响到事件的传播。

```cpp
    bool event(QEvent *event) override {
        if (event->type() == QEvent::MouseButtonPress) {
            qDebug("Mouse Button Press");
            return true;   // 已经处理，不在发送给其他对象
        }
        return QWidget::event(event);
    }
```

![[event.png]]


事件过滤：

安装事件过滤器

```cpp

void MainWindow::MainWindow()
{
    // 给任意 QObject 对象安装事件过滤器
    m_widget->installEventFilter(this); // this : 定义 eventFilter 的对象
}
```
```cpp
// 定义事件过滤器
 bool MainWindow::eventFilter(QObject *obj, QEvent *event)
 {
     if (obj == m_widget) {   // 针对的对象
         if (event->type() == QEvent::KeyPress) {
             return true;     // 不再发送给 obj 对象
         } else {
             return false;
         }
     } else {
         // pass the event on to the parent class
         return QMainWindow::eventFilter(obj, event);
     }
 }
```

**如果你在事件过滤器中 delete 了某个接收组件，务必将函数返回值设为 true。否则，Qt 还是会将事件分发给这个接收组件，从而导致程序崩溃。**

事件过滤器和被安装过滤器的组件必须在同一线程，否则，过滤器将不起作用

如果使用`installEventFilter()`函数给一个对象安装事件过滤器，那么该事件过滤器只对该对象有效

事件过滤器可以安装到任意`QObject`类型上面，并且可以安装多个

如果给`QApplication`对象安装事件过滤器，那么该过滤器对程序中的每一个对象都有效，任何对象的事件都是先传给 `QApplication` 的`eventFilter()`函数


事件传递次序：

![[事件传递次序.png]]


总结一下 Qt 的事件处理，实际上是有五个层次：

- 重写`paintEvent()`、`mousePressEvent()`等事件处理函数。这是最普通、最简单的形式，同时功能也最简单。
- 重写`event()`函数。`event()`函数是所有对象的事件入口，`QObject`和`QWidget`中的实现，默认是把事件传递给特定的事件处理函数。
- 在特定对象上面安装事件过滤器。该过滤器仅过滤该对象接收到的事件。
- 在`QCoreApplication::instance()`上面安装事件过滤器。该过滤器将过滤所有对象的所有事件，因此和`notify()`函数一样强大，但是它更灵活，因为可以安装多个过滤器。全局的事件过滤器可以看到 disabled 组件上面发出的鼠标事件。全局过滤器有一个问题：只能用在主线程。
- 重写`QCoreApplication::notify()`函数。这是最强大的，和全局事件过滤器一样提供完全控制，并且不受线程的限制。但是全局范围内只能有一个被使用（因为`QCoreApplication`是单例的）。

```cpp
class Label : public QWidget
{
public:
    Label()
    {
        installEventFilter(this);
    }
 
    bool eventFilter(QObject *watched, QEvent *event)
    {
        if (watched == this) {
            if (event->type() == QEvent::MouseButtonPress) {
                qDebug() << "eventFilter";
            }
        }
        return false;
    }
 
protected:
    void mousePressEvent(QMouseEvent *)
    {
        qDebug() << "mousePressEvent";
    }
 
    bool event(QEvent *e)
    {
        if (e->type() == QEvent::MouseButtonPress) {
            qDebug() << "event";
        }
        return QWidget::event(e);
    }
};
 
class EventFilter : public QObject
{
public:
    EventFilter(QObject *watched, QObject *parent = 0) :
        QObject(parent),
        m_watched(watched)
    {
    }
 
    bool eventFilter(QObject *watched, QEvent *event)
    {
        if (watched == m_watched) {
            if (event->type() == QEvent::MouseButtonPress) {
                qDebug() << "QApplication::eventFilter";
            }
        }
        return false;
    }
 
private:
    QObject *m_watched;
};
 
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    Label label;
    app.installEventFilter(new EventFilter(&label, &label));
    label.show();
    return app.exec();
}
```

## Related

- [[信号和槽]] -- 信号槽是 Qt 事件驱动的核心通信机制
- [[Meta-Object System]] -- 元对象系统是事件和信号槽的基础