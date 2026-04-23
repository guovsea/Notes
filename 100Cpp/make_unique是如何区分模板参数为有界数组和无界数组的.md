---
tags:
  - cpp/memory
  - type/howto
---
```cpp
template<class _Tp>
struct __unique_if
{
    typedef unique_ptr<_Tp> __unique_single;
};

template<class _Tp>
struct __unique_if<_Tp[]>
{
    typedef unique_ptr<_Tp[]> __unique_array_unknown_bound;
};

template<class _Tp, size_t _Np>
struct __unique_if<_Tp[_Np]>
{
    typedef void __unique_array_known_bound;
};

template<class _Tp, class... _Args>
inline _LIBCPP_INLINE_VISIBILITY
typename __unique_if<_Tp>::__unique_single
make_unique(_Args&&... __args)
{
    return unique_ptr<_Tp>(new _Tp(_VSTD::forward<_Args>(__args)...));
}

template<class _Tp>
inline _LIBCPP_INLINE_VISIBILITY
typename __unique_if<_Tp>::__unique_array_unknown_bound
make_unique(size_t __n)
{
    typedef typename remove_extent<_Tp>::type _Up;
    return unique_ptr<_Tp>(new _Up[__n]());
}

template<class _Tp, class... _Args>
    typename __unique_if<_Tp>::__unique_array_known_bound
    make_unique(_Args&&...) = delete;

#endif  // _LIBCPP_STD_VER > 11
```

使用了 [[SFINAE（Substitution Failure Is Not An Error）]]。通过使用模板元编程中的 __unique_if 结构体，根据传入的类型是否为数组类型以及数组的长度情况，选择不同的重载函数进行调用。这种技术可以根据模板参数的特性选择不同的函数重载，从而实现更灵活的代码逻辑。==还是不理解==

```cpp
template<class _Tp>
struct __unique_if
{
    typedef void __unique_single;
};

template<class _Tp>
struct __unique_if<_Tp[]>
{
    typedef void __unique_array_unknown_bound;
};

template<class _Tp, size_t _Np>
struct __unique_if<_Tp[_Np]>
{
    typedef void __unique_array_known_bound;
};

template <class _Tp, class... _Args> 
    typename __unique_if<_Tp>::__unique_single test(_Args &&...__args) {
    std::cout << "unique_single\n";
}

template <class _Tp, class... _Args> 
    typename __unique_if<_Tp>::__unique_array_known_bound
test(_Args &&...) {
    std::cout << "unique_array_unknown_bound\n";
}

int main(int argc, const char **argv) {
    test<A>();
    test<A[3]>();
}
```

