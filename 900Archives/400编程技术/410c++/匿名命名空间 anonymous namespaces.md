---
---
一个没有名称的命名空间是一个匿名的命名空间。匿名命名空间是一个没有名字的命名空间，它的作用域是当前编译单元（即一个.cpp 文件）的内部。**在匿名命名空间中声明的函数、变量和类型对于编译单元内的代码是可见的，但是对于其他编译单元是隐藏的**。

使用匿名命名空间有几个优点：

1. 隐藏实现细节：可以隐藏实现细节，防止对外暴露不需要的信息。
2. 避免名称冲突：可以避免与其他代码中的同名函数、变量或类型发生冲突。
3. 编译单元范围：可以在编译单元范围内共享变量和函数，而不会影响其他编译单元。

使用匿名命名空间的一个常见的例子是在头文件中定义静态全局变量。这样可以防止该变量被其他编译单元引用，从而减少了全局变量带来的潜在问题。


```cpp
namespace {
    // Variables
    int i = 1;
}
```

Code:

```cpp
# a.cpp
int HelpFunc(int a, int b) {
    return a + b;
}

# b.cpp
int HelpFunc(int a, int b) {
    return a / b;
}

# main.cpp
#include <iostream>
int main(int argc, char const *argv[])
{
    std::cout << "hello world\n";
    return 0;
}

```

result:

```shell
 % g++ -o app main.cpp a.cpp b.cpp 
/usr/bin/ld: /tmp/cc7NesvN.o: in function `HelpFunc(int, int)':
b.cpp:(.text+0x0): multiple definition of `HelpFunc(int, int)'; /tmp/ccwhhRQO.o:a.cpp:(.text+0x0): first defined here
collect2: error: ld returned 1 exit status
```
> **链接阶段的错误**

使用匿名空间后：

```cpp
# a.cpp
namespace {
int HelpFunc(int a, int b) {
    return a + b;
}
} 

# b.cpp
namespace {
int HelpFunc(int a, int b) {
    return a / b;
}
}
# main.cpp
#include <iostream>
int main(int argc, char const *argv[])
{
    std::cout << "hello world\n";
    return 0;
}

```

```shell
% g++ -o app main.cpp a.cpp b.cpp  # OK 
```

