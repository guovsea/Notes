---
tags:
  - cpp/basics
  - type/reference
---
摘自：[C++ 中的中文编码 | SF-Zhou's Blog](https://sf-zhou.github.io/programming/chinese_encoding.html)

[[字符集]]

UTF-8 编码还保持着一个优秀的特性，无论是使用左对齐（字符串排序），还是右对齐（数值排序），UTF-8 编码始终保持着与 Unicode 一致的大小顺序。举个栗子🌰，字符串 u8"A" < u8"一", 同时宽字符 wchar_t(L'A') < wchar_t(L'一')。仔细想想还是蛮有意思的。

根据映射关系表可知，英文字符在 UTF-8 中使用一个字节存储，中文字符使用三个字节存储。可以根据映射关系轻松地写出 Unicode 与 UTF-8 间的转换程序。根据汉字在 Unicode 中的编码范围，可以计算所有汉字的 UTF-8 编码并输出。C++ 示例程序如下（C++ 14）：

```cpp
/* Copyright [2018] <Copyright SF-Zhou> */

#include <iostream>
#include <fstream>
#include <memory>

std::unique_ptr<char[]> unicode_to_utf8(uint16_t unicode) {
  const int k_buffer_length = 3;
  auto buffer = std::make_unique<char[]>(k_buffer_length);
  buffer[0] = 0xE0 | (unicode >> 12);
  buffer[1] = 0x80 | ((unicode >> 6) & 0x3F);
  buffer[2] = 0x80 | (unicode & 0x3F);
  return buffer;
}

int main() {
  const auto k_chinese_encoding_begin = 0x4E00;
  const auto k_chinese_encoding_end = 0x9FFF;
  const auto k_output_filename = "test_01.txt";

  std::ofstream out(k_output_filename, std::ios::out | std::ios::binary);
  for (int i = k_chinese_encoding_begin; i <= k_chinese_encoding_end; i ++) {
    uint16_t unicode = i;
    auto buffer = unicode_to_utf8(unicode);
    out.write(buffer.get(), 3);
    out.write("\n", 1);
  }

  out.close();
}
```

UTF-16 与 UTF-8 类似，使用变长编码。不同的是 UTF-16 使用一个或者两个 16bit 大小的编码单元，这样单个 Unicode 字符在 UTF-16 编码下字节长度为 2 或者 4。常用汉字在 UTF-16 中均可使用 2 个字节表示，但数字和字母也是 2 个字节。换句话说，如果是英文较多，使用 UTF-16 会产生较大的浪费；如果是中文较多，相较于 UTF-8 会节约不少空间。坏消息是 UTF-16 与 ASCII 码不兼容，应用上不如 UTF-8 广泛。

### char 与 std::string

从上一节的代码中可以看到，直接在文件中写入 UTF-8 编码对应的三个 char 字节，即可输出文字。实际上程序输出的、文件记录的，都是字节流，类似于 Python3 中的 `bytes` 的概念。字节流本身并不清楚自己的编码方式，而打开文件的软件会根据字节流本身推测使用的编码。例如字节流 `[0xE4, 0xB8, 0x80]`，GBK 编码解析到 0x80 后无法找到后续的字节、解析失败，ASCII 无法解析 0xE4 同样解析失败，而 UTF-8 则可以很好地完成解码。

`char` 与 `std::string` 的功能与字节流一致。日常使用中可以通过 C++ 中的 `cout << "雾失楼台，月迷津渡" << endl;`，直接打印得到对应的汉字字符串，是因为代码文件编码和控制台编码显示的编码一致。以 Mac 为例，可以尝试将代码文件的编码切换为 CP939，vim 的话指令为 `:e ++enc=cp936`，再尝试编译和运行，就会得到编译器提出的 Warning 及输出的乱码。

然而直接使用字节流会产生一些问题，例如无法直接得到字符串的真实字符长度。UTF-8 编码的单个汉字长度为 3，英文长度为 1，中英文混合的字符串无法直接得到字符长度，需要先进行完整的解码。进而字节流也无法直接实现字符串的截取。例如：

```cpp
#include <iostream>
#include <string>
using std::cout;
using std::endl;
using std::string;

int main() {
  const string love_cpp = "我爱C++";
  cout << love_cpp.length() << endl;
  // 打印 9
}
```

直到现在为止，C++ 中获取 UTF-8 编码的字符串字符长度都是一个麻烦的问题，一般需要依赖第三方库。这一点感觉可以借鉴 Python3，将字节流 `bytes` 和字符串 `str` 分割开，并提供方便的转换。

### wchar_t 与 std::wstring

目前为止 C++ 没有采用上一节说的方案，而是大力出奇迹，提出使用 `wchar_t` 解决多字节编码带来的问题。既然多字节计算长度不准确，那就直接使用一个类型代替多字节。目前 UTF-8、Unicode 等编码至多需要 4 个字节，那就直接定义 `wchar_t` 为 4 个字节，所有问题都可以迎刃而解了。



是的，这不是开玩笑。`wchar_t` 具体定义为 2 个字节，还是 4 个字节并没有明确规定，视平台而定。在笔者的 Mac 上可以找到以下的定义：

```cpp
typedef int __darwin_wchar_t;
typedef __darwin_wchar_t wchar_t;
```

而 `std::wstring` 则是 `string` 的 `wchar_t` 版，其定义为：

```cpp
typedef basic_string<wchar_t> wstring;
```

使用宽字符类型可以解决字符串长度计算的问题，但会浪费了大量的存储空间。并且当前的 `std::wstring` 无法直接使用 `std::cout` 输出到标准输出流、无法使用 `std::ofstream` 输出到文件，而需要使用适配的 `std::wcout` 和 `std::wofstream`。综合来看，并不易用。

### 七、String Literal

C++ 代码中可以使用 "Hello World" 和 L"Hello World" 来声明字符串和宽字符串常量。C++ 11 开始支持 UTF-8、UTF-16 和 UTF-32 字符串常量的声明，分别使用 u8""、u"" 和 U"" 作为声明的标志，详细说明如下（复制自[参考文献2](https://en.cppreference.com/w/cpp/language/string_literal)）：

1. Narrow multibyte string literal. The type of an unprefixed string literal is `const char[]`.
2. Wide string literal. The type of a `L"..."` string literal is `const wchar_t[]`.
3. UTF-8 encoded string literal. The type of a `u8"..."` string literal is `const char[]`.
4. UTF-16 encoded string literal. The type of a `u"..."` string literal is `const char16_t[]`.
5. UTF-32 encoded string literal. The type of a `U"..."` string literal is `const char32_t[]`.

### 八、最佳实践

本段仅为个人经验，仅供参考。

IO 时统一使用 UTF-8，处理时统一用 Unicode。举个例子🌰：

```cpp
#include <codecvt>
#include <fstream>
#include <iostream>

int main() {
  const std::locale utf8( std::locale(), new std::codecvt_utf8<wchar_t> );
  const std::wstring love_cpp = L"\x6211\x7231""C++";  // 我爱C++

  std::wcout.imbue(utf8);
  std::wcout << love_cpp << std::endl;  // 输出 "我爱C++"
  std::wcout << love_cpp.length() << std::endl;  // 输出 5
  std::wcout << love_cpp.substr(0, 2) << std::endl;  // 输出 "我爱"

  const auto k_output_filename = "test_02.txt";
  std::wofstream f_out(k_output_filename, std::ios::out);
  f_out.imbue(utf8);
  f_out << love_cpp << std::endl;
  f_out.close();

  std::wifstream f_in(k_output_filename, std::ios::in);
  f_in.imbue(utf8);
  std::wstring value;
  f_in >> value;
  std::wcout << (value == love_cpp ? "Yes": "Opps") << std::endl;  // 输出 "Yes"
  f_in.close();
}
```

### [参考文献](https://sf-zhou.github.io/programming/chinese_encoding.html#%E5%8F%82%E8%80%83%E6%96%87%E7%8C%AE)

1. [阮一峰. "字符编码笔记：ASCII，Unicode 和 UTF-8."](http://www.ruanyifeng.com/blog/2007/10/ascii_unicode_and_utf-8.html)
2. [C++ reference. "C++ Language String Literal".](https://en.cppreference.com/w/cpp/language/string_literal)
3. [Wikipedia. "UTF-16".](https://en.wikipedia.org/wiki/UTF-16)