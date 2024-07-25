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

```
