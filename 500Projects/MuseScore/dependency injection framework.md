---
tags:
  - act/project
  - design/pattern
  - type/concept
mastery: 5
---
[GitHub - igorkorsukov/kors\_modularity](https://github.com/igorkorsukov/kors_modularity)

## 概述

MuseScore 使用了一套基于 `kors_modularity` 的依赖注入（DI）框架，用于解耦模块间的依赖关系。

### 核心概念

- **Module（模块）**：每个功能模块（如 Notation、Playback、Plugins）都是一个独立的 Module。
- **Injector（注入器）**：负责创建模块实例并解析依赖关系。
- **Export / Import**：模块通过 Export 暴露接口，通过 Import 声明对其他模块的依赖。

### 为什么 MuseScore 需要 DI？

MuseScore 是一个大型桌面应用，功能模块众多（乐谱编辑、播放、插件系统、导入导出等）。DI 框架的好处：

1. **降低耦合**：模块间通过接口通信，不直接依赖具体实现。
2. **方便测试**：可以替换模块为 Mock 实现。
3. **延迟初始化**：模块按需加载，减少启动时间。
4. **生命周期管理**：框架控制模块的创建和销毁顺序。

## 相关

- [[Learning Path]]

