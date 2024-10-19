---
tags:
  - cpp/机制
---
## 概念

什么是链接属性：

Linkage determines whether identifiers that have identical names refer to the same object, function, or other entity, even if those identifiers appear in different translation units.

链接确定具有相同名称的标识符是否引用相同的对象、功能或其他**实体**，即使这些标识符出现在不同的翻译单元中。

[Program linkage - IBM Documentation](https://www.ibm.com/docs/en/xl-c-and-cpp-aix/16.1?topic=linkage-program)
## 链接属性和作用域的区别和联系

Scope and linkage are distinguishable in that **scope is for the benefit of the compiler, whereas linkage is for the benefit of the linker**. During the translation of a source file to object code, the compiler keeps track of the identifiers that have external linkage and eventually stores them in a table within the object file. The linker is thereby able to determine which names have external linkage, but is unaware of those with internal or no linkage.

> [Scope and linkage - IBM Documentation](https://www.ibm.com/docs/en/xl-c-and-cpp-aix/16.1?topic=end-scope-linkage)

- Linkage does not affect scoping, and normal name lookup considerations apply.
- Scope 是编译器将[[编译单元 Translation unit]]生成目标文件时，用于解决**编译单元内部标识符**可见性问题的
- Linkage 是链接器用于链接目标文件时，用于解决**编译单元之间标识符可见性**的

## 链接属性的分类

### Internal linkage

[Internal linkage - IBM Documentation](https://www.ibm.com/docs/en/xl-c-and-cpp-aix/16.1?topic=linkage-internal)

_Internal linkage_ refers to everything only **in scope of a translation unit**.  _internal_ linkage means that it is only accessible in one translation unit.

内部链接仅指翻译单元范围内的所有内容。内部链接意味着它只能在一个翻译单元中访问

### Externial linkage

_External linkage_ refers to things that exist beyond a particular translation unit. In other words, **accessible through the whole program**, which is the combination of all translation units (or object files).  

外部链接是指存在于特定翻译单元之外的事物。换句话说，可以通过整个程序访问，这是所有翻译单元（或目标文件）的组合。

### No linkage

**不表示对象或函数的标识符不具有任何linkage**。因为**只有具有实体的标识符才具有 linkage**

Identifiers that do not represent an object or a function, including labels, enumerators, `typedef` names that refer to entities with no linkage, type names, function parameters, and ![C++ only Begins](https://www.ibm.com/docs/en/SSGH3R_16.1.0/shared_source/graphics/ngcpp_begin.gif)template names
### Language linkage (C++ only)

```cpp
extern "C" { 
#include "shared.h" 
}
```

## 指定链接属性

[c++ - What is external linkage and internal linkage? - Stack Overflow](https://stackoverflow.com/questions/1358400/what-is-external-linkage-and-internal-linkage)

You can explicitly control the linkage of a symbol by using the `extern` and `static` keywords. If the linkage is not specified then the default linkage is `extern` (external linkage) for non-`const` symbols and `static` (internal linkage) for `const` symbols.  

可以使用 `extern` 和 `static` 关键字显式控制符号的链接。

**如果未指定链接，则 `const 变量` 默认 `internal linkage`；`non-const 变量` 默认 `external linkage`**

```cpp
// In namespace scope or global scope.
int i; // extern by default
const int ci; // static by default
extern const int eci; // explicitly extern
static int si; // explicitly static

// The same goes for functions (but there are no const functions).
int f(); // extern by default
static int sf(); // explicitly static 
```

Note that instead of using `static` (internal linkage), it is better to use [[匿名命名空间 anonymous namespaces]]]. 

请注意，**与其使用 `static` （内部链接），不如使用匿名命名空间**

```cpp
namespace { 
//anonymous namespace members in C++11 have internal linkage by default
  int i; 
  class C; 
}
```
