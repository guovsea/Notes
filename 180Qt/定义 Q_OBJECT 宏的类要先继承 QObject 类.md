---
tags:
  - qt/core
  - type/pitfall
mastery: 5
---
## 相关

- [[Meta-Object System]] -- Q_OBJECT 宏是元对象系统的入口

## 踩坑

具有 Q_OBJECT 宏的类都需要 MOC（Meta-Object Compiler）处理，也就是说需要把这个类所在的头文件传给编译器。

### 问题一：继承顺序

在多重继承中，**必须先继承 QObject，再继承其他类**，否则 MOC 生成的文件会报错。

```cpp
// 正确
class HttpMgr : public QObject, public Singleton<HttpMgr>
{
    Q_OBJECT;
public:
    ~HttpMgr() {}
private:
    HttpMgr() {}
};

// 错误！
// class HttpMgr : public Singleton<HttpMgr>, public QObject
```

> 编译器报错类似：`no member named 'qt_metacast' in 第一个类 did you mean simply 'qt_metacall'`

> 很难 debug，因为报错出现在 MOC 生成的 `.cpp` 文件中，不易定位到根因。

**原因**：MOC 生成的代码假设第一个基类是 QObject，如果顺序不对，`staticMetaObject` 和虚函数表会与错误的基类关联。

### 问题二：头文件必须加入编译

```cmake
# 编译测试代码 -- HEADERS 必须加入，否则 MOC 不会处理 Q_OBJECT
add_executable(${PROJECT_NAME} ${TESTS} ${SOURCES} ${HEADERS})
```

如果只在 `.cpp` 文件中定义带 `Q_OBJECT` 的类而未通过头文件暴露给 MOC，链接时会出现 "undefined reference to vtable" 错误。
