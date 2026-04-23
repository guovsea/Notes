---
tags:
  - os/linux
  - type/snippet
mastery: 5
---
## perror 函数

```c
#include <stdio.h> //使用此函数需引入此头文件
void perror(const char *str);

```
**参数**
- str 附加描述字符串

当一个系统调用或库函数发生错误时，通常会将`全局变量errno`设置为一个特定的错误码。perror函数读取errno的值，并根据这个值生成相应的错误描述。然后，将错误描述与传入的字符串参数拼接，并输出到标准错误流。