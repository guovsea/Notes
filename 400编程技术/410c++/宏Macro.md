---
tags:
  - cpp/技巧
---
**不要在宏中使用++**

```cpp
#define ISDIGIT(ch) ((ch) >= '0' && (ch) <= '9')
#define ISDIGIT(ch) ((ch) >= '0' && (ch) <= '9')
static int lept_parse_number(lept_context* c, lept_value* v) {
	const char* p = c->json;
	if (*p == '-') ++p;
	if (*p == '0') {
		++p;
	} else {
		if (!ISDIGIT1To9(*p++)) return LEPT_PARSE_INVALID_VALUE;
		// 消耗掉所有数字
		for (; ISDIGIT(*p);++p);
	}
}
```

**在宏中使用 ++，实际可能会调用多次++**。
## 基本语法

## `#define` 用法

### `#define`会忽略表达式中所有空格

**但是不会忽略换行符号**

以下宏定义等效：
```cpp
#define CONNECT(a, b) a##b
#define CONNECT(a, b) a ##b
#define CONNECT(a, b) a ##   b
```
### `#define`中的 `# ## \`

**#** 将参数转换为**字符串**

```cpp
#define STR(s) #s
STR(abc)   // "abc"
```

**##** 将参数进行拼接

并不是进行字符串的拼接，而是**将两个标识符拼接为新标识符**，新标识符必须有效，否则报错

```cpp
#define CONNECT(a, b) a ## b
int main(int argc, const char** argv) {
    int a_1 = 3;
    // std::cout << CONNECT(2, _a1); // 报错
    std::cout << CONNECT(a, _1);     // 3
    std::cout << CONNECT(a_, 1);     // 3
    return 0;
}
```

**\\** 跨行编写宏，忽略换行符

```cpp
#define CONNECT(a, b) a ## \
    b
```

> `\`后面不能有空格，否则会 warning: backslash and newline separated by space

[[GETSET 宏 快速设置值类型的成员变量]]