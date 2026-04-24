
编译这段代码

```cpp
template <typename T>  
void destroy_at(T* ptr, std::true_type) {  
    // do nothing  
}  
  
template <typename T>  
void destroy_at(T* ptr, std::false_type) {  
    if (ptr)  
        ptr->~T();  
}  
  
template <typename T>  
void destroy(T* ptr) {  
    destroy_at(ptr, std::is_trivially_destructible<T>{});  
}
```

时，产生了这样一个包错：

```
no known conversion from 'bool' to 'std::true_type' (aka 'integral_constant<bool, true>')
```

## 原因分析

`integral_constant` 实现了 `operator T` 操作符，可以进行隐式类型转换。

`integral_constant<T, v>{}` （类型为 `integral_constant<T, v>` 的对象）可以隐式转换为值为 v ，类型为 T 的对象

**`!std::true_type{}` 的类型并不是 `std::false_type`，而是值为 `false` 的 `bool` 变量**


```cpp
template <typename T, T v>  
struct integral_constant {  
    static constexpr T value = v;  
    operator T() { return v; }  
};

TEST(integral_constant, auto_cast) {  
    EXPECT_EQ(!tinystl::true_type{}, false);  
    bool false_v = tinystl::false_type{};  
    bool true_v = tinystl::true_type{};  
    EXPECT_TRUE(true_v);  
    EXPECT_FALSE(false_v);  
  
    int rt = tinystl::integral_constant<int, 10>{};  
    EXPECT_EQ(rt, 10);  
}
```
