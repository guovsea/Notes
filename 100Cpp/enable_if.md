---
---
`enable_if` 主要是配合 SFINAE 让某个模板（包括特化模板）不生效

``
**正确模式**：

1. 主模板最后一个参数是 `typename = void`
2. 偏特化模板的 argument list 的最后一个参数必须是 `enable_if<cond>` or `enable_if<cond, void>` 。如果 cond == true，一定要是 void，**和主模板默认实参一致**

```cpp
template<typename T, typename = void>
struct is_int : std::false_type {};

// template<typename T>
// struct is_int<
//     T,                                                  // 必须 void
//     typename std::enable_if<std::is_same<T, int>::value, void>::type
// > : std::true_type {};

template<typename T>
 struct is_int<
     T,
     typename std::enable_if<std::is_same<T, int>::value>::type
 > : std::true_type {};
```

**错误模式**：

```cpp
template<typename T, typename = void>
struct is_int : std::false_type {};

template<typename T>
struct is_int<
    T,
    typename std::enable_if<std::is_same<T, int>::value, int>::type
> : std::true_type {};
```
## MRE

```cpp
#include <type_traits>

// 主模板：第三参数默认是 void
template<typename T, typename = void>
struct is_int : std::false_type {};

//  错误：enable_if 的第二参数不是 void
template<typename T>
struct is_int<
    T,
    typename std::enable_if<std::is_same<T, int>::value, int>::type
> : std::true_type {};

static_assert(!is_int<int>::value, "BUG: int 被判断为 false");

```

## 原因分析

在 `T = [int]` 的时候，有以下模板列表


1. 主模板

```cpp
template<typename T, typename = void>
struct is_int : std::false_type {};
```

2. 偏特化模板

> 逐步替换

```cpp
template<typename T>
struct is_int<
    T,
    typename std::enable_if<true, int>::type
> : std::true_type {};
```

```cpp
template<typename T>
struct is_int<
    T,
    int
> : std::true_type {};
```

因为实例化的时候**并没有传入第二个模板参数**，所以实例化的情况是：

```cpp
is_int<int, void>::value
```

导致永远无法匹配偏特化版本的模板。

只有当主模板和偏特化模板分别是：

```cpp
template<typename T, typename = void>
struct is_int : std::false_type {};

template<typename T>
struct is_int<T, void> : std::false_type {};
```

才能匹配偏特化模板