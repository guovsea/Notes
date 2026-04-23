---
tags:
  - cpp/basics
  - type/reference
---

## 可变参数

有时候会希望函数带有可变数量的参数，则需要使用到 C 的可变参数

```c
# include <stdarg.h>
int f(int arg1, double arg2, ...);    // 不同于 C++ 的参数包，C语言中的可变参数包没有名字
typedef __builtin_va_list va_list;
#define va_start(ap, param) __builtin_va_start(ap, param)
#define va_end(ap)          __builtin_va_end(ap)
#define va_arg(ap, type)    __builtin_va_arg(ap, type)
```
在 `<stdarg.h>`中定义了如下 API 用于支持可变参数：

- **va_list 类型**：用于存储可变参数数据结构
- **va_start ()宏函数**：初始化 **va_list** 变量为传入函数的**所有实参**。
  - ap：va_list 类型的变量
  - param：参数列表中**最后一个具名变量**
- **va_arg ()函数**：访问 **va_list** 变量中存储的所有函数实参
  - ap：va_list 类型的变量
  - type：变量类型
  第 n 次访问将访问第 n 个实参
- **va_end ()**：释放**va_list** 变量的内存



## myPrintf 

```cpp
size_t putInteger(int num) {
    size_t put_cnt = 0;
    if (num < 0) {
        putchar('-');
        put_cnt++;
        putInteger(-num);
    }
    int dec = 1;
    int tmp = num;
    while (tmp > 9) {    // dec 比 num 少 1 位
        tmp /= 10;
        dec *= 10;
    }
    while (num > 0) {
        putchar('0' + num / dec);
        put_cnt++;
        num %= dec;
        dec /= 10;
    }
    return put_cnt;
}
size_t putString(const char * str) {
    const char *p = str;
    while (*p != '\0') {
        putchar(*p);
        p++;
    }
    return size_t(p - str);
}
#include <stdarg.h>
size_t myPrintf(const char * format...) {
    va_list valist;
    va_start(valist, format);
    const char* p = format;
    size_t cnt = 0;
    int num;
    const char* str = NULL;
    while (*p != '\0') {
        if (*p == '%') {
            p++;
            switch(*p) {
                case 'd':
                num = va_arg(valist, int);
                cnt += putInteger(num);
                break;
                case 's':
                str = va_arg(valist, const char*);
                cnt += putString(str);
                break;
            }
        } else {
            putchar(*p);
            cnt++;
        }
        p++;
    }
    va_end(valist);
    return cnt; // 返回字符数(没有处理错误)
}

int main(void) {
    // int num = -973;
    // putInteger(num);
    // putString("abcdefg");
    size_t cnt = myPrintf("int %d, char %s\n", 3, "abc");
    std::cout << cnt << std::endl;
    size_t cnt1 = printf("int %d, char %s\n", 3, "abc");
    std::cout << cnt1 << std::endl;
```