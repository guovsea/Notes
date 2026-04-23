---
tags:
  - qt/core
  - type/reference
mastery: 5
---
## Qt 命名约定

**设置属性：`set属性名()`**

**获取属性：`属性名()`** 而不是 `get属性名()`

## QByteArray

字节数组类，用于存储原始二进制数据和 UTF-8 字符串。适用于网络通信、文件 IO 等场景。

```cpp
QByteArray ba("hello");
ba.append('\0');
ba.size();  // 6（包含 '\0'）
```

## QString

Unicode 字符串，Qt 中最常用的字符串类。内部使用 UTF-16 编码。

```cpp
QString s = "Hello %1";
s.arg("Qt");           // "Hello Qt"
s.toUtf8();            // 转 QByteArray
QString::number(42);   // "42"
```

## QVariant

通用值包装类，可容纳多种数据类型。详见 [[QVarient]]。

## QPoint

二维点，包含 x 和 y 坐标。

```cpp
QPoint p(10, 20);
p.x();  // 10
p.y();  // 20
```

## QLine

直线段，由两个 QPoint 定义。

```cpp
QLine line(QPoint(0,0), QPoint(100,100));
line.translate(int dx, int dy);
// 直线在 x 轴方向上平移 dx，在 y 轴方向上平移 dy
```

## QSize

尺寸，包含 width 和 height。

```cpp
QSize s(800, 600);
s.width();   // 800
s.height();  // 600
s.scale(400, 300, Qt::KeepAspectRatio);  // 等比缩放
```

## QRect

矩形区域，由左上角坐标 (x, y) 和尺寸 (width, height) 定义。

```cpp
QRect rect(10, 10, 100, 80);
rect.contains(QPoint(50, 50));  // true
rect.intersects(otherRect);      // 判断是否相交
```

## QDate

日期类，只包含年月日。

## QTime

时间类，只包含时分秒毫秒。

## QDateTime

完整的日期时间，结合了 QDate 和 QTime，支持时区转换。

```cpp
QDateTime::currentDateTime();          // 当前时间
QDateTime::fromString(s, format);      // 从字符串解析
```

## QElapsedTimer

高精度计时器，用于测量时间间隔。

```cpp
QElapsedTimer timer;
timer.start();
// ... 执行操作 ...
qDebug() << timer.elapsed() << "ms";
```

## QTimer

定时器，到达设定间隔后发射 `timeout` 信号。

```cpp
QTimer *timer = new QTimer(this);
connect(timer, &QTimer::timeout, this, &MyClass::onTimeout);
timer->start(1000);  // 每 1000ms 触发一次
```
