---
tags:
  - design/pattern
  - type/concept
mastery: 5
---
单例模式是为了保证程序的[[400编程技术/340操作系统/线程安全]]。


## C++ 实现

主要思路

- 构造函数和析构函数声明为 private 或者 delete，禁止构造
- 拷贝构造函数和赋值构造函数是私有类型，目的是禁止外部拷贝和赋值，确保实例的唯一性。
- 类中有一个获取实例的静态方法，可以全局访问。

### 懒汉模式 Lazy Singleton

当第一次使用时，才去创建对象。

```cpp
// version 1.0， 非线程安全
class Singleton
{
private:
	static Singleton* instance;
private:
	Singleton() {};
	~Singleton() {};
	Singleton(const Singleton&);
	Singleton& operator=(const Singleton&);
public:
	static Singleton* getInstance() 
        {
		if(instance == NULL) 
			instance = new Singleton();
		return instance;
	}
};

// init static member
Singleton* Singleton::instance = NULL;
```

存在的问题：

无法访问析构函数，可能会出现资源泄露。（比如当设置连接池为单例对象时，当程序运行结束，连接池无法释放连接，因为无法调用析构函数。）

解决办法：

1. 使用智能指针
2. 使用静态的嵌套类对象

```cpp
// version 1.1， 非线程安全
// 使用静态的嵌套类对象
class Singleton
{
private:
	static Singleton* instance;
private:
	Singleton() { };
	~Singleton() { };
	Singleton(const Singleton&);
	Singleton& operator=(const Singleton&);
private:
	class Deletor {
	public:
		~Deletor() {
			if(Singleton::instance != NULL)
				delete Singleton::instance;
		}
	};
	static Deletor deletor;
public:
	static Singleton* getInstance() {
		if(instance == NULL) {
			instance = new Singleton();
		}
		return instance;
	}
};

// init static member
Singleton* Singleton::instance = NULL;
Singleton::Deletor Singleton::deletor;
// 一定也要初始化 deletor 静态成员，否则 deletor 根本不会存在，自然也不会析构

```
在程序运行结束时，系统会调用静态成员`deletor`的析构函数，可以在析构函数中释放资源。使用这种方法释放单例对象有以下特征：

- 在单例类内部定义专有的嵌套类。
- 在单例类内定义私有的专门用于释放的静态成员。
- 利用程序在结束时析构全局变量的特性，选择最终的释放时机。

这个代码在单线程环境下是正确无误的，但是当拿到多线程环境下时这份代码就会出现 race condition，注意 version 1.0与 version 1.1都不是线程安全的。

非线程安全分析：

多线程同时进入`getInstance`方法并发现`instance`为 NULL，因此它们可能会**创建多个不同的对象**。

要使其线程安全，能在多线程环境下实现单例模式，我们首先想到的是利用同步机制来正确的保护我们的 shared data。可以使用[[双检锁]]（DCL: Double-Checked Locking Pattern）：

```c 
// DCL 保证线程安全
static Singleton* getInstance() {
	if(instance == NULL) {
		Lock lock;  // 基于作用域的加锁，超出作用域，自动调用析构函数解锁
        if(instance == NULL) {
        	instance = new Singleton();
        }
	}
	return instance;
}
```

注意，线程安全问题仅出现在第一次初始化（new）过程中，而在后面获取该实例的时候并不会遇到，也就没有必要再使用lock。双检测锁很好地解决了这个问题，它通过加锁前检测是否已经初始化，避免了每次获取实例时都要首先获取锁资源。

加入 DCL 后，其实还是有问题的，关于 memory model。在某些内存模型中（虽然不常见）或者是由于编译器的优化以及运行时优化等等原因，使得 instance 虽然已经不是 nullptr 但是其所指对象还没有完成构造，这种情况下，另一个线程如果调用 getInstance()就有可能使用到一个不完全初始化的对象。换句话说，就是代码中第2行：if(instance == NULL)和第六行 instance = new Singleton();没有正确的同步，在某种情况下会出现 new 返回了地址赋值给 instance 变量而 Singleton 此时还没有构造完全，当另一个线程随后运行到第2行时将不会进入 if 从而返回了不完全的实例对象给用户使用，造成了严重的错误。在 C++11没有出来的时候，只能靠插入两个 memory barrier（内存屏障）来解决这个错误，但是 C++11引进了 memory model，提供了 Atomic 实现内存的同步访问，即不同线程总是获取对象修改前或修改后的值，无法在对象修改期间获得该对象。


因此，在有了C++11后就可以正确的跨平台的实现DCL模式了，利用atomic，代码如下：

```cpp
atomic<Widget*> Widget::pInstance{ nullptr };
Widget* Widget::Instance() {
    if (pInstance == nullptr) { 
        lock_guard<mutex> lock{ mutW }; 
        if (pInstance == nullptr) { 
            pInstance = new Widget(); 
        }
    } 
    return pInstance;
}
```

C++11中的 atomic 类的默认 memory_order_seq_cst 保证了3、6行代码的正确同步，由于上面的 atomic 需要一些性能上的损失，因此我们可以写一个优化的版本：

```cpp
atomic<Widget*> Widget::pInstance{ nullptr };
Widget* Widget::Instance() {
    Widget* p = pInstance;
    if (p == nullptr) { 
        lock_guard<mutex> lock{ mutW }; 
        if ((p = pInstance) == nullptr) { 
            pInstance = p = new Widget(); 
        }
    } 
    return p;
}
```
优化之处在于：使用一个局部对象 p 来，减少使用 pInstance 的次数。因为对原子变量进行操作的代价更高。

#### 最终版本

**C++11规定了 local static 在多线程条件下的初始化行为，要求编译器保证了内部静态变量的线程安全性**。在 C++11标准下，《Effective C++》提出了一种更优雅的单例模式实现，使用函数内的 local static 对象。这样，只有当第一次访问`getInstance()`方法时才创建实例。这种方法也被称为 **Meyers' Singleton**。C++0x 之后该实现是线程安全的，C++0x 之前仍需加锁。

```cpp
class Singleton
{
private:
	Singleton() { };
	~Singleton() { };
	Singleton(const Singleton&);
	Singleton& operator=(const Singleton&);
public:
	static Singleton& getInstance() 
        {
		static Singleton instance;
		return instance;
	}
};
// 使用时必须使用引用，或者将接口的返回值定义为指针
Singleton & s = Sigleton::getInstance();
```
```cpp
class A {
    public:
    static A& getInstance() {
        static A a;
        return a;
    }
    int val_;
    private:
    A() {}
};
int main() {
    A& a = A::getInstance();
    a.val_ = 3;
    A& a1 = A::getInstance();
    a1.val_ = 4;
    std::cout << "a = " << a.val_ << "   " << "a1 = " << a1.val_ << std::endl;
    std::cout << "&a = " << &a << "   " << "&a1 = " << &a1<< std::endl;
}
```
**在使用时必须声明类型为引用才能保证单例模式**
```shell
# 使用 A a = A::getInstance();
a = 3   a1 = 4
&a = 0x7ffeede1c1e8   &a1 = 0x7ffeede1c1e0
# 使用 A& a = A::getInstance();
a = 4   a1 = 4
&a = 0x102d3a0e0   &a1 = 0x102d3a0e0
```

### 饿汉模式 Eager Singleton

单例实例在程序运行时被立即执行初始化


```cpp
class Singleton
{
private:
	static Singleton instance;
private:
	Singleton();
	~Singleton();
	Singleton(const Singleton&);
	Singleton& operator=(const Singleton&);
public:
	static Singleton& getInstance() {
		return instance;
	}
}

// initialize defaultly
Singleton Singleton::instance;

```

由于在 main 函数之前初始化，所以没有线程安全的问题。但是潜在问题在于 no-local static 对象（函数外的 static 对象）在不同编译单元中的初始化顺序是未定义的。也即，static Singleton instance;和 static Singleton& getInstance()二者的初始化顺序不确定，如果在初始化完成之前调用 getInstance() 方法会返回一个未定义的实例。  
  
总结：

- Eager Singleton 虽然是线程安全的，但存在潜在问题；
- Lazy Singleton通常需要加锁来保证线程安全，但局部静态变量版本在C++11后是线程安全的；
- **局部静态变量版本（Meyers Singleton）最优雅**。

**补充：C++中 static 对象的初始化**

**non-local static对象（函数外）**

C++规定，non-local static 对象的初始化发生在main函数执行之前，也即main函数之前的单线程启动阶段，所以不存在线程安全问题。但C++没有规定多个non-local static 对象的初始化顺序，尤其是来自多个编译单元的non-local static对象，他们的初始化顺序是随机的。

**local static 对象（函数内）**

对于local static 对象，其初始化发生在控制流第一次执行到该对象的初始化语句时。多个线程的控制流可能同时到达其初始化语句。

在 C++11之前，在多线程环境下 local static 对象的初始化并不是线程安全的。具体表现就是：如果一个线程正在执行 local static 对象的初始化语句但还没有完成初始化，此时若其它线程也执行到该语句，那么这个线程会认为自己是第一次执行该语句并进入该 local static 对象的构造函数中。这会造成这个 local static 对象的重复构造，进而产生**内存泄露**问题。所以，local static 对象在多线程环境下的重复构造问题是需要解决的。

而 C++11则在语言规范中解决了这个问题。C++11 规定，在一个线程开始 local static 对象的初始化后到完成初始化前，其他线程执行到这个 local static 对象的初始化语句就会等待，直到该 local static 对象初始化完成。


[C++ 单例模式 - 知乎](https://zhuanlan.zhihu.com/p/37469260)

## Related

- [[设计模式]]
- [[单例模式 Singleton]]
- [[享元模式 FlyWidget]]