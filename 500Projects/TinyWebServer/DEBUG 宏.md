---
tags:
  - act/project
  - cpp/basics
  - type/notes
mastery: 5
---

在 TinyWebServer 项目中，使用宏来简化调试输出。通过 `#ifdef TEST` 控制是否启用测试相关的调试信息。

```c
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

// 检查是否定义了 TEST 宏
#ifdef TEST

// 定义测试宏
#define TEST(description, expression) \
    do { \
        printf("Testing: %s\n", description); \
        if (expression) { \
            printf("  [PASS] %s\n", description); \
        } else { \
            printf("  [FAIL] %s\n", description); \
        } \
    } while (0)

#else

#define TEST(description, expression) ((void)0)

#endif
```

> 使用 `do { ... } while (0)` 包裹宏定义是为了让宏在使用时表现得像一条语句，避免在 `if-else` 分支中产生歧义。

## 相关

- [[Log]]
- [[http_conn]]
- [[项目架构]]
