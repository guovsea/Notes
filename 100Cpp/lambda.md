---
tags:
  - cpp/basics
  - type/concept
---
lambda 表达式：未命名的内联函数

lambda 表达式同样具有返回类型、参数列表、函数体。但 lambda 可以定义在函数内部。

```cpp
// lambda 表达式结构如下
[capture list](parameter list) option -> return type  {function body}
```
**要求**

- 如果要指定返回值类型，必须使用尾置返回。
- 可以省略参数列表和返回值类型，但是必须永远包含捕获列表和函数体.
- 形参不能有默认值。


**捕获列表: 捕获当前作用域下的变量**

捕获列表只是指定 lambda 表达式对当前作用域下的变量的使用方式，**lambda 表达式可以直接使用局部 static 变量和它所在函数之外声明的名字**。

- `[]` - 不捕捉任何变量
- `[&]` - 按引用捕获所有变量
- `[=]` - 按值捕获所有变量
- `[=, &foo]` - 按照引用捕获外部变量 foo，按值捕获外部作用域中其他变量
- `[bar]` - 只按值捕获 bar 变量
- `[&bar]` - 只按引用捕获 bar 变量

**按值捕获的变量在 lambda 表达式中默认是只读的**

```cpp
int main() {
    int a = 3;
    [=]() {
        a++; // increment of read-only variable ‘a’
        // lambda 可以直接使用 main 函数之外，在头文件<iostream> 中定义的 cout
        std::cout << "lambda" << std::endl;
        std::cout << a << std::endl;
    }();
}
```
**选项**

- mutable: 可以修改按值传递进来**的拷贝**（而不是值本身）
- exception: 声明函数能够 throw 异常;

**返回值类型:**

**编译器可以自动推断出返回值类型可以省略**

```cpp
// 编译器无法推断返回值类型，因为两个返回值表达式的类型不相同
int main() {
    int a = 3;
    [=]()mutable {
        a++;
        std::cout << "lambda" << std::endl;
        std::cout << a << std::endl;
        // inconsistent types ‘int’ and ‘double’ deduced for lambda return type
        if (a < 0) {
            return 3;
        } else {
            return 3.14;
        }
    }();
}
```