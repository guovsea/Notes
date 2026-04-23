---
tags:
  - type/notes
  - lang/rust
mastery: 5
---
## 学习资源

- [rust-by-example](https://doc.rust-lang.org/rust-by-example/) -- 通过示例学习 Rust，适合有编程基础的快速上手
- [The Rust Programming Language](https://doc.rust-lang.org/book/) -- 官方教程（The Book），系统全面
- [Rust by Practice](https://practice.rs/) -- 练习题集合，巩固语法知识
- [Rust Reference](https://doc.rust-lang.org/reference/) -- 语言参考手册，查阅语法细节

## 核心概念

- **所有权（Ownership）**：Rust 的核心内存管理机制，每个值有且仅有一个所有者
- **借用（Borrowing）**：通过引用访问数据，不可变引用 `&T` 和可变引用 `&mut T`
- **生命周期（Lifetime）**：编译器追踪引用有效性的机制
- **Trait**：类似接口的概念，定义共享行为
- **模式匹配**：`match` 表达式和 `if let` / `while let`

## 工具链

```shell
rustup update          # 更新 Rust 工具链
cargo new <project>    # 创建新项目
cargo build            # 编译
cargo run              # 编译并运行
cargo test             # 运行测试
cargo doc --open       # 生成并查看文档
```
