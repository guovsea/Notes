## 函数名和函数指针

普通函数名 == 函数指针，除了 decltype

对于普通函数而言，函数可以转换为函数指针并且不会影响函数匹配的精度。换句话说，任何需要函数指针的地方都可以使用函数名。**除了 decltype**

```cpp
void f1(int x) {
    std::cout << "f1(" << x  << ")" << std::endl;
}
void f2(int x) {
    std::cout << "f2(" << x  << ")" << std::endl;
}
void g(void (*p)(int x), int x) {
    p(x);
}

int main() {  
    g(f1, 3);      // 函数名，被转换为函数指针
    g(&f1, 3);     // 函数指针
    g(f2, 1);
    g(&f2, 1);
```

```text
ubuntu% /home/guo/leanC++/build/Draft/Draft
f1(3)
f1(3)
f2(1)
f2(1)
```
**`decltype(函数名)`的结果是“函数类型”；`decltype(&函数名)`的结果是“函数指针类型”**
```cpp
    decltype(&f1) pf = f1; // 编译成功
    decltype(f1) pf = f1;  // 编译失败
```