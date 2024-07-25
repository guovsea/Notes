## 概述

**动机（Motivation）**

- 在软件系统中，经常有这样一些特殊的类，必须保证它们在系统中只存在一个实例，才能确保它们的逻辑正确性、以及良好的效率。
- 如何绕过常规的构造器，提供一种机制来保证一个类只有一个实例？
- 这应该是类设计者的责任，而不是使用者的责任。

**模式定义**

> 保证一个类仅有一个实例，并提供一个该实例的全局访问点。——《设计模式》GoF

## 代码

### 接口

```cpp
class Singleton {
private:
    Singleton();
    Singleton(const Singleton& other);

public:
    static Singleton* getInstance();
};


Singleton* Singleton::m_instance = nullptr;

```

### 非线程安全

```cpp
private:
    static Singleton* m_instance;

// 初始化类的静态成员变量
Singleton* Singleton::m_instance = nullptr;

Singleton* Singleton::getInstance() {
    if (m_instance == nullptr) {
        m_instance = new Singleton();
    }
    return m_instance;
}

```
### 线程安全，但锁代价过高

如果两个线程都只是读，那么就不需要加锁
```cpp
private:
    static Singleton* m_instance;

// 初始化类的静态成员变量
Singleton* Singleton::m_instance = nullptr;

Singleton* Singleton::getInstance() {
    Lock lock;
    if (m_instance == nullptr) {
        m_instance = new Singleton();
    }
    return m_instance;
}
```


### 双检锁，内存读写 reorder 不安全

可能执行的流程：

- 分配内存
- `new Singleton()`返回一个地址给 `m_instance`
- 在这个地址上构造 Singleton 对象

**原因是编译器为了优化代码，会对代码执行流程进行调整** *被成为 reorder *

*另一个进程进来看到 `m_instance != nullpter`就会直接返回 m_instance*

```cpp
private:
    static Singleton* m_instance;

// 初始化类的静态成员变量
Singleton* Singleton::m_instance = nullptr;

Singleton* Singleton::getInstance() {
    if (m_instance == nullptr) {
        Lock lock;
        if (m_instance == nullptr) {
            m_instance = new Singleton();
        }
    }
    return m_instance;
}
```
### C++ 11 标准库

```cpp
std::atomic<Singleton*> Singleton::m_instance;
std::mutex Singleton::m_mutex;

Singleton* Singleton::getInstance() {
    Singleton* tmp = m_instance.load(std::memory_order_relaxed);
    std::atomic_thread_fence(std::memory_order_acquire);  // 获取内存fence
    if (tmp == nullptr) {
        std::lock_guard<std::mutex> lock(m_mutex);
        tmp = m_instance.load(std::memory_order_relaxed);
        if (tmp == nullptr) {
            tmp = new Singleton;
            std::atomic_thread_fence(std::memory_order_release);  // 释放内存fence
            m_instance.store(tmp, std::memory_order_relaxed);
        }
    }
    return tmp;
}
```

### 返回静态变量

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
Singleton &s = Sigleton::getInstance();
```
## 总结


- Singleton 模式中的实例构造器可以设置为 protected 以允许子类派生。
- Singleton 模式一般不要支持拷贝构造和 Clone 接口，因为这有可能导致多个对象实例，与 Singleton 模式的初衷违背。
- 如何实现多线程环境下安全的 Singleton？ 注意对双检查锁的正确实现。