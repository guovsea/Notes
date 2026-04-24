---
---
## 语义

**只能移动，不能拷贝**

```cpp
class A {
  public:
    A(const A& other) {std::cout << "拷贝构造\n";}
};

int main(int argc, const char **argv) {
    auto p1 = std::make_unique<A>();
    auto p2 = p1; // 错误，不能拷贝 unique_ptr
}
```

> 无论 `std::unique_ptr` 所指向的对象是否支持拷贝，`std:unique_ptr` 都不能拷贝

使用 `unique_ptr` 管理某个对象就意味着：**该对象只能被一个 `unique_ptr` 对象所指向**


## 删除器

### 默认删除器

```cpp
template <class _Tp>
// 删除器是仿函数
struct _LIBCPP_TEMPLATE_VIS default_delete {
    static_assert(!is_function<_Tp>::value,
                  "default_delete cannot be instantiated for function types");
#ifndef _LIBCPP_CXX03_LANG
  _LIBCPP_INLINE_VISIBILITY constexpr default_delete() _NOEXCEPT = default;
#else
  _LIBCPP_INLINE_VISIBILITY default_delete() {}
#endif
  template <class _Up>
  _LIBCPP_INLINE_VISIBILITY
  default_delete(const default_delete<_Up>&,
                 typename enable_if<is_convertible<_Up*, _Tp*>::value>::type* =
                     0) _NOEXCEPT {}

// 删除器被调用时：                           // 传入的指针
  _LIBCPP_INLINE_VISIBILITY void operator()(_Tp* __ptr) const _NOEXCEPT {
    static_assert(sizeof(_Tp) > 0,
                  "default_delete can not delete incomplete type");
    static_assert(!is_void<_Tp>::value,
                  "default_delete can not delete incomplete type");
    delete __ptr;
  }
};
```

默认删除器在 `delete` 传入的指针时，首先会断言 `static_assert(sizeof(_Tp) > 0` 然后才 `delete` 所传入的指针，目的是**为了断言传入的指针所指向的对象类型是否完整**，因为 **`delete` 只能删除类型完整的对象** [[new delete]]