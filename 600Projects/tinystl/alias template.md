---
aliases:
  - cpp
  - cpp/template
  - refefence
---

```cpp
    template <typename...>
    using void_t = void;
```


## 1️⃣ 给复杂模板起短名字（最直观）

```cpp
template<class T>
using Vec = std::vector<T>;

Vec<int> v;           // 等价于 std::vector<int>
```

再比如常见的函数指针：

```cpp
template<class R, class... Args>
using Func = R(*)(Args...);

Func<void, int> f;    // void(*)(int)
```

---

## 2️⃣ 预绑定部分模板参数（类似“柯里化”）

```cpp
template<class T>
using StringMap = std::map<std::string, T>;

StringMap<int> m;     // std::map<std::string, int>
```

这在泛型库代码里非常方便。

---

## 3️⃣ “_t” 简写形式（标准库里大量使用）

你看到的：

```cpp
std::remove_reference_t<T>
std::enable_if_t<cond, T>
std::decay_t<T>
```

其实都是 **typedef + alias template 的语法糖**：

```cpp
template<class T>
using remove_reference_t = typename std::remove_reference<T>::type;
```

减少一层 `typename ... ::type` 的噪音 👍

---

## 4️⃣ 更好地写 traits / SFINAE

不用 `typename T::xxx` 这种啰嗦写法：

```cpp
template<class T>
using has_iterator_t = typename T::iterator;
```

然后搭配 `void_t`：

```cpp
template<class, class = void>
struct has_iterator : std::false_type {};

template<class T>
struct has_iterator<T, void_t<has_iterator_t<T>>> : std::true_type {};
```

阅读体验明显好了。

---

## 5️⃣ 跨平台 / 条件类型封装（隐藏实现）

```cpp
#ifdef USE_BOOST
template<class T>
using Json = boost::json::value<T>;
#else
template<class T>
using Json = nlohmann::json;
#endif
```

用户只看到 `Json<T>`，内部随便换实现。

---

## 6️⃣ 解决“不能给别名做部分特化”的问题

虽然 **别名模板不能直接特化**，  
但它通常搭配 `struct` 实现“包一层”：

```cpp
template<class T>
struct add_ptr_impl { using type = T*; };

template<class T>
using add_ptr_t = typename add_ptr_impl<T>::type;
```

---

## 7️⃣ 给模板模板参数更友好的接口

```cpp
template<template<class...> class Container>
void f();

f<std::vector>();    // OK
```

配合 alias：

```cpp
template<class T>
using MyVec = std::vector<T, MyAlloc<T>>;

f<MyVec>();          // 也行
```

---

## 8️⃣ “固定接口 + 可扩展实现”风格（架构层面）

假设你写库：

```cpp
template<class T>
using SmallVector = llvm::SmallVector<T, 8>;
```

未来想改默认容量，只改一处，不破坏用户代码。

---

## 🔚 小结（记住这 3 句话）

**alias template 可以：**

1️⃣ 给复杂模板起简单的名字  
2️⃣ 让 traits / SFINAE 更清晰  
3️⃣ 让库设计更灵活、更易替换实现

而且——很多现代 C++ 代码风格里：

> `using + alias template` 几乎完全取代了 `typedef`。

---

如果你愿意，我们可以继续深入任一方向，比如：

- 👉 `enable_if_t` / `void_t` 一整套检测技巧
    
- 👉 自己写 `xxx_t` traits 工具
    
- 👉 alias template 和 concepts 的结合（C++20）
    

告诉我你想看哪一块 👍