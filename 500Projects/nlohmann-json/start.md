---
tags:
  - act/project
  - type/notes
---

# nlohmann/json 学习笔记

[nlohmann/json - GitHub](https://github.com/nlohmann/json)

## 构建与测试

```bash
mkdir build
cd build
cmake .. -DJSON_BuildTests=On
cmake --build .
ctest --output-on-failure
```

## 简介

nlohmann/json 是一个现代化的 C++ JSON 库，特点：
- **Header-only**：只需包含一个头文件即可使用
- **直觉式 API**：用 `[]` 操作符像操作 STL 容器一样操作 JSON
- **STL 兼容**：支持迭代器、范围 for 等特性
- **类型转换**：支持自定义类型的序列化/反序列化

## 相关

- [[Learning Path]]