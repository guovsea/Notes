---
tags:
  - act/project
  - type/notes
---

# Chromium 源码学习

## 构建 (Build)

官方构建指南：[Mac Build Instructions](https://chromium.googlesource.com/chromium/src/+/main/docs/mac_build_instructions.md)

首先配置 depot_tools 路径：

```bash
export PATH="$PATH:/Users/guo/110cpp/chromium/depot_tools"
```

中文参考：[Chromium 源码编译指南](https://zhuanlan.zhihu.com/p/260645423)

## 调试 (Debug)

使用 CLion 开发调试：[Chromium Docs - CLion Dev](https://chromium.googlesource.com/chromium/src/+/HEAD/docs/clion.md)

需要在 CLion 中配置：
1. **Custom Build Target**（用于编译）
2. **Run/Debug Configuration**（用于调试）

LLDB 调试配置：[Usage of tools/lldb/lldbinit.py](https://chromium.googlesource.com/chromium/src/+/main/docs/lldbinit.md)

## 相关

- [[三大 C++ 工具链]]
- [[调试器能否调试取决于编译器生成的调试信息的格式]]