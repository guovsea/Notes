---
tags:
  - cpp/concurrency
  - type/reference
---
```cpp
template <class _Fp, class ..._Args,
          class
         >
thread::thread(_Fp&& __f, _Args&&... __args)
{
    typedef unique_ptr<__thread_struct> _TSPtr;
    _TSPtr __tsp(new __thread_struct);
    // 使用std::decay 去除了传入类型的引用修饰符，同时保留了数组和函数类型的信息
    // 因此，在将参数保存到 std::tuple 类型中的 _GP 的时候，都是值传递
    typedef tuple<_TSPtr, typename decay<_Fp>::type, typename decay<_Args>::type...> _Gp;
    _VSTD::unique_ptr<_Gp> __p(
            new _Gp(std::move(__tsp),
                    __decay_copy(_VSTD::forward<_Fp>(__f)),
                    __decay_copy(_VSTD::forward<_Args>(__args))...));
    int __ec = __libcpp_thread_create(&__t_, &__thread_proxy<_Gp>, __p.get());
    if (__ec == 0)
        __p.release();
    else
        __throw_system_error(__ec, "thread constructor failed");
}
```
