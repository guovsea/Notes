---
tags:
  - cpp/basics
  - type/howto
---
> Don't overuse structured binding; a named return type often gives more readable code;

## 函数返回~~多个返回值~~ （有多个成员的类对象）

函数只能返回一个返回值，但返回值可以是**有多个成员的类对象**

在 C++17 有结构化绑定之前：

```cpp
#include <iostream>
#include <tuple>

using namespace std;

std::tuple<int, std::string, float> foo()
{
    return std::make_tuple(1, "Hello world!", 3.14f);
}

int main()
{
    int i;
    std::string str;
    int j;
    std::tie(i, str, j) = foo();
    cout << str << endl;
    return 0;
}
```

之后

```cpp
#include <iostream>
#include <tuple>

using namespace std;

std::tuple<int, std::string, float> foo()
{
    return std::make_tuple(1, "Hello world!", 3.14f);
}

int main()
{
    auto [i, str, j] = foo();
    cout << str << endl;
    return 0;
}
```

非常地优雅
## 遍历 `std::map`

```cpp
#include <iostream>
#include <map>

int main()
{
    std::map<int, std::string> myMap;

    myMap.insert({1, "Apple"});
    myMap.insert({2, "Orange"});
    myMap.insert({2, "Lemon"});

    // 和普通的 range-for 相同
    for (auto [ip, name] : myMap) {
        std::cout << ip << "  " << name << std::endl;
    }
    for (auto &[ip, name] : myMap) {
        std::cout << ip << "  " << name << std::endl;
    }
    for (const auto &[ip, name] : myMap) {
        std::cout << ip << "  " << name << std::endl;
    }
    return 0;
}
```

## Array 数组

Well, is this really something good? I don’t know. But you can use structured bindings together with arrays.  

```
int arr[3] = { 1, 2, 3};
auto [x, y, z] = arr;

std::cout << x << y << z << std::endl;
```

The above code will bind the first value in the array to `x`, the second to `y` and so on. This looks pretty okay at first glance. But you end up with the same problem as with return values from functions - **the amount of variables must match with the array**. So it might be a bit tricky to use.

## Structs and variables

```cpp
struct Point {
    float x;
    float y;
    float z;
};

Point p { 1.f, 2.5f, -0.4f };

auto [fX, fY, fZ] = p;
std::cout << fX << " " << fY << " " << fZ << std::endl;
```

