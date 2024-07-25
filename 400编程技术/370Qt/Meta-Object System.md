由于 C++的 [[RTTI]] 机制只能提供有限的类型信息，于是 Qt 构建了自己的元对象系统（Meta-Object）。**使用该系统的基类 QObject 所创建的派生类对象，可以在运行期获取该对象的类名、父类名、枚举类型以及有哪些成员变量、有哪些成员函数等信息。基于这些信息，Qt 实现了强大的信号槽机制**。

## 1. 不完美的C++

![](https://pic1.zhimg.com/80/v2-28a5a95816ce7c333e198d6c1f35cef4_1440w.webp)

假设有一个形状基类Shape和三个派生类：代表正方形的Square类、代表三角形的Triangle类和代表圆形的Circle类。我们知道基类类型的指针可以指向派生类的对象，如下代码所示。虽然都是Shape *类型的指针，但是调用draw()函数时却是调用各自的派生类方法。所有的这些特性都是依赖于C++的多态性机制。

```cpp
Shape *shape1 = new Square();
Shape *shape2 = new Triangle();
Shape *shape3 = new Circle();
```

### 1.1 为什么需要RTTI？

一般情况下我们并不需要使用RTTI机制，C++的继承、多态性就能够解决大多数问题。**但是在编写程序调试器、对象I/O库时就特别需要知道类的信息**。

多态性机制并不能解决这样的场景。假设用一个容器保存这三个指针，此时想更改容器中“圆形”对象的颜色。但是容器中的每一个元素都是Shape *类型的数据，从表面上看是无法判断所指对象的类型，自然就找不到哪个元素是Circle类型的。**由于C++是静态类型语言，有关类的信息只在编译期被使用，编译后就不再保留，因此程序运行时无法获取类的信息。这时就需要使用「运行期类型信息」，即RTTI（Run-Time Type Information）**。一般拥有程序运行时保存对象类型信息能力的语言，我们就称该语言支持RTTI。C++标准定义了dynamic_cast和typeid两个关键字用于支持RTTI机制。

dynamic_cast的作用是将指向基类对象的指针转换为指向派生类对象的指针，如果转换失败则返回NULL。因此，dynamic_cast唯一的功能就是判断一个对象具有哪些类型。例如有4个类GrandFather、Father、Son和GrandSon，右边的类分别是左边类的派生类。此时有一个类型为GrandFather *的指针p，为了判断p所指对象是否具有Son的类型，可以使用语句Son *son = dynamic_cast<Son*>(p)。如果返回的son不为NULL，则p所指的对象具有Son类型。同理我们也可以得到该对象是否具有Father、GrandSon类型，

但是仅仅知道这些是不够的。因为有时候某个指针所指的对象行为不正常，如果无法知道所指对象的实际类型，就必须审查该基类及其所有派生类的代码。反过来，如果我们能够知道该对象的实际类型，那就只需要审查该类型的代码即可。typeid的作用是返回类型的名字。因此，typeid的功能就是在dynamic_cast的范围内进一步的确定指针所指对象的实际类型。

### 1.2 有限的RTTI能力

完整的描述一个类型需要很多信息，例如类的名字、有哪些父类、有哪些成员变量、有哪些成员函数、哪些是public的、哪些是private的、哪些是protected的等等。有时候一个工程项目可能包含成千上万个类，完整的保存这些信息将会消耗大量的内存资源。为了节省内存，C++标准约定typeid只能返回类名。因此，仅靠dynamic_cast和typeid两个关键字提供的类型信息实在有限。

**即使仅提供有限的类型信息，RTTI的实现仍然耗费了很大的时间和存储空间，这就会降低程序的性能**。因此，早期的标准委员会并没有将该特性纳入C++中。后来是出于以下的考虑才加入的：（1）RTTI可以作为一个编译选项提供给开发人员。如果不需要用到这个特性，可以手动禁用这个特性。（2）编译器只需要做很少的改动就可以实现RTTI。

另一方面，虽然C++定义了dynamic_cast和typeid两个关键字，但并没有说明如何实现这两个关键字。这就造成了不同的编译器的实现不同，更别说提供RTTI功能的库千差万别。**由此导致的最大问题就是程序的可移植性差，项目之间无法完美兼容**。

为了解决这些问题，Qt没有采用C++的RTTI机制，却提供了更为强大的元对象（Meta-Object）系统机制来实现动态获取类型信息。

## 2. 强大的Qt元对象系统

Qt元对象系统的强大在于“**即使编译器不支持RTTI，我们也能动态获取类型信息**”。例如在任何时候调用QMetaObject::className()函数都会返回类的名称。由于程序运行时保留了类型信息，那么自然就可以进行父子类之间的动态转换。qobject_cast()相比dynamic_cast()强制转换安全得多，而且速度更快。因此，对于QObject派生类之间的转换，推荐使用qobject_cast()。如下所示：

```cpp
QObject *obj = new QWidget();
QWidget *widget = qobject_cast<Qwidget *>(obj);
```

由于元对象系统提供了完整的动态类型信息，利用这些信息Qt构建了**信号槽机制**和**动态属性系统**。其中，信号槽机制是核心功能。

### 2.1 信号槽机制

**信号槽机制是Qt最大的特色功能，用于实例对象之间的通信**。信号和槽都是成员函数，绑定一个对象的信号和另一个对象的槽函数，当前者发射出信号时，与之相对应的会触发后者的槽函数，这样两个对象间就完成了一次通信过程。如下所示：

```cpp
class ObjectA : public QObject
{
    Q_OBJECT
public:
    explicit ObjectA(QObject *parent = nullptr);

    void setValue(const int& value)
    {
        emit valueChange(value);
    }

signals:
    void valueChange(const int& v);
};

class ObjectB : public QObject
{
    Q_OBJECT
public:
    explicit ObjectB(QObject *parent = nullptr);

public slots:
    void on_receive(const int& v)
    {
        qDebug() << "Receive value:" << v;
    }
};

ObjectA objA;
ObjectB objB;
connect(&objA, &ObjectA::valueChange, &objB, &ObjectB::on_receive);
objA.setValue(666);
```

**在没有信号槽机制的时代，C++对象间的交互一般使用回调函数来实现**。使用某对象时，用指针指向另一个对象的函数，这个函数就称为回调函数。使用回调函数有个弊端，当某个对象被多个对象通信时，需要一个容器来存放多个对象的回调函数。维护这个容器使得代码编写效率低、扩展性弱。

基于Qt元对象系统扩展的信号槽机制，使得处理对象间通信变得更加高效、灵活。

### 2.2 动态属性系统

利用元对象系统所提供的动态类型信息，Qt还构建了一套强大的属性系统。**由于元对象系统的特点，这就保证了Qt属性系统是独立于编译器和平台的**。不仅如此，我们还可以使用Q_PROPERTY()宏来定义编译期的静态属性，使用setProperty()函数动态添加属性。

在C++中是没有属性概念的，只有成员变量。因为面向对象的思想是抽象封装，属性是类给外部展示的特性。而成员变量属于类的内部信息，直接暴漏出去就破坏了封装性，因为使用者可以对类特性进行直接修改。而属性将取值、赋值的细节进行了封装，外部只能使用它而不能控制它。

通常在界面插件开发、QML中使用属性系统，例如在Qt Creator的设计界面中，控件都是以属性的方式暴露给外部使用者。

![](https://pic1.zhimg.com/80/v2-b626925ee6c2b33fcf156db5f1512ad8_1440w.webp)

## 3. 使用元对象系统的3个条件

使用元对象系统需要满足三个条件：

- 只有QObject派生类才可以使用元对象系统特性。
- 在类声明前使用 Q_OBJECT ()宏来开启元对象功能。
- 使用 Moc 工具为每个 QObject 派生类提供实现代码。