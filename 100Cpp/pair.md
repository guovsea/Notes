---
tags:
  - cpp/stl
  - type/reference
---
## pair

### operator<

```cpp
  template<typename _T1, typename _T2>
    inline _GLIBCXX_CONSTEXPR bool
    operator<(const pair<_T1, _T2>& __x, const pair<_T1, _T2>& __y)
    { return __x.first < __y.first
	     || (!(__y.first < __x.first) && __x.second < __y.second); }
```

值得说明的是：标准库实现的`std::pair` 的 `operator<` 只需要 `T1` `T2` 重载了 `operator<` 运算符。很合理。

实现原理 ：

```cpp
// 直接返回
__x.first < __y.first
         // 到这里条件表达式并没有截断返回，说明：x.first >= y.first
	     || (!(__y.first < __x.first) && __x.second < __y.second);
	         // 并且 .x.first 并不大于 y.first，那就只有 x.first == y.first 了



```