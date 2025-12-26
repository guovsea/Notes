---
tags:
  - cpp
  - Concepts
  - blogs
  - template
  - reference
---

# C++20 Concepts 核心笔记（源自 cppstories 博客）
## 一、核心概述
- **是什么**：C++20 引入的模板参数约束机制，通过 `concept`（概念）和 `requir#es`（要求）关键字，在编译时验证模板参数是否满足指定条件
- **核心价值**：提升代码可读性、简化模板语法、加速编译、提供清晰的编译器错误提示（替代原有复杂的 SFINAE 技巧）
- **适用场景**：类模板、函数模板的参数约束，控制函数重载和部分特化

## 二、概念（Concept）的定义
### 1. 基本语法
概念是编译时可计算的布尔约束，两种常见定义方 #式 ：
```cpp
// 1. 基于类型特征（type trait）的简洁定义
template <class T>
concept integral = std::is_integral_v<T>; // 约束 T 为整数类型

// 2. 基于 requires 表达式的复杂约束（检测成员、接口等）
template <typename T>
concept ILabel = requires(T v) {
    {v.buildHtml()} -> std::convertible_to<std::string>; // 要求 T 有 buildHtml() 方法，返回值可转为 string
};
```

## 三、Concepts 的使用方式
### 1. 三种常用语法形式
```cpp
// 形式1：requires 子句紧跟模板声明
template <typename T>
requires std::integral<T> || std::floating_point<T> // 组合预定义概念
void DoSomething(T param) { }

// 形式2：requires 子句放在函数声明末尾
template <typename T>
void DoSomething(T param) requires std::integral<T> { }

// 形式3：直接将概念作为模板参数类型（最简洁）
template <std::integral T> // 等价于 template <typename T> requires std::integral<T>
auto sum(const std::vector<T>& vec) { /* 实现 */ }
```

### 2. 实用示例：计算容器平均值
```cpp
#include <numeric>
#include <vector>
#include <concepts>

// 约束 T 为整数或浮点类型
template <typename T> 
requires std::integral<T> || std::floating_point<T>
constexpr double Average(std::vector<T> const &vec) {
    const double sum = std::accumulate(vec.begin(), vec.end(), 0.0);        
    return sum / vec.size();
}

// 调用示例
int main() {
    std::vector ints {1,2,3,4,5};
    std::cout << Average(ints); // 输出 3.0
}
```

## 四、核心优势：更友好的编译器错误
- 未使用 Concepts 时：模板实例化失败会产生冗长、深层的调用栈错误
- 使用 Concepts 后：直接提示「模板参数不满足约束」，例如传入字符串容器时：
```
error: 没有匹配的函数调用 'Average(std::vector<const char*>&)'
note: 候选函数：template<class T> requires (integral<T>) || (floating_point<T>) constexpr double Average(...)
```

## 五、C++20 预定义概念（`<concepts>` 头文件）
无需自定义，直接使用标准库提供的约束，分类整理核心常用：

| 类别         | 关键概念（举例）                          | 说明                                  |
|--------------|-------------------------------------------|---------------------------------------|
| 核心语言概念 | `integral`/`floating_point`               | 整数/浮点类型                         |
|              | `same_as`/`derived_from`/`convertible_to` | 类型相同/继承关系/可转换              |
|              | `copy_constructible`/`movable`            | 可拷贝构造/可移动                     |
| 比较概念     | `equality_comparable`/`totally_ordered`   | 可相等比较/全序比较（需 `<compare>`） |
| 对象概念     | `copyable`/`regular`/`semiregular`        | 可拷贝/正则类型（可拷贝+可比较）      |
| 可调用概念   | `invocable`/`predicate`                   | 可调用（函数/函数对象）/谓词（返回bool） |

## 六、语法简化技巧
### 1. 缩写函数模板（Abbreviated Function Templates）
用 `auto` 替代显式模板声明，自动推导模板参数：
```cpp
// 原写法
template <typename T>
void print(const std::vector<T>& vec) { /* 遍历输出 */ }

// 简化写法（等价于上面）
void print2(const std::vector<auto>& vec) { /* 遍历输出 */ }

// 无参数类型限制的通用函数
auto func(auto param) { /* 实现 */ } // 等价于 template <typename T> auto func(T param)
```

### 2. 受限 auto（Constrained Auto）
结合概念限制 `auto` 类型，支持所有满足「范围概念」的容器：
```cpp
// 约束容器为 std::ranges::range（支持范围 for 循环的类型）
void print3(const std::ranges::range auto& container) {
    for (size_t i = 0; auto && elem : container)
        std::cout << elem << (++i == container.size() ? "\n" : ", ");
}
```

## 七、requires 表达式的高级用法
`requires` 表达式是 Concepts 的核心，支持检测：成员函数、数据成员、操作符、返回值等，灵活定义接口约束：
```cpp
// 检测数据成员（要求 T 有 string 类型的 name_ 成员）
template<typename T>
concept has_string_data_member = requires(T v) { 
    { v.name_ } -> std::convertible_to<std::string>; 
};

// 检测接口（要求 T 有 start/stop/getTime 成员函数，不限制返回值）
template <typename T>
concept Clock = requires(T c) { 
    c.start();  
    c.stop();
    c.getTime();
};

// 编译时验证
struct Person { int age_; std::string name_; };
struct Box { double weight_; double volume_; };

static_assert(has_string_data_member<Person>);   // 成立
static_assert(!has_string_data_member<Box>);    // 不成立
```
## 八、关键参考资料
1. 官方文档：[Constraints and concepts (C++20) - cppreference.com](https://en.cppreference.com/w/cpp/language/constraints)
2. 深入阅读：[Requires-expression | Andrzej’s C++ blog](https://akrzemi1.wordpress.com/2015/03/23/requires-expression/)
3. 书籍：《C++20》by Rainer Grimm、《Programming with C++20》by Andreas Fertig
4. 原博客链接：[C++20 Concepts - a Quick Introduction](https://www.cppstories.com/2021/concepts-intro/)

## 笔记技巧总结
1. **重点标注**：用加粗/高亮突出 `concept`、`requires`、受限 auto 等核心语法
2. **代码精简**：只保留关键示例代码（去掉冗余的头文件/主函数细节）
3. **分类整理**：按「定义→使用→优势→语法→工具支持」逻辑分层，方便快速查阅
4. **关联对比**：记录 Concepts 与旧版 SFINAE 的差异（简化、可读性提升）
5. **实用备注**：标注编译选项、常用预定义概念，避免后续踩坑
