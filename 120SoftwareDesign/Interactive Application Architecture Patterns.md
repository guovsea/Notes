---
tags:
  - design/pattern
  - type/reference
mastery: 5
---

# 交互式应用架构模式（MVC / MVP / MVVM / PAC）

**来源**：[Aspiring Craftsman 博客](https://web.archive.org/web/20110424040641/http://www.aspiringcraftsman.com/2007/08/25/interactive-application-architecture/)（Derek Greer, 2007）

---

## 1. 引言

- MVC、MVP 和 PAC 模式旨在**分离交互式应用中的不同关注点**。
- 设计模式受平台环境影响较大，现代开发环境可能不需要严格按照原始形式实现。
- **核心观点**：模式应从问题出发选择，而非为了使用模式而使用模式。

---

## 2. Model-View-Controller（MVC）

### 核心思想

分离 **Model（业务/数据）**、**View（显示）**、**Controller（用户输入）**。

### 历史

1978-79 年由 Trygve Reenskaug 提出（原名 Thing-Model-View-Editor）。Smalltalk-80 实现广为人知。Controller 的设计动机源于 Smalltalk 平台对输入输出的处理复杂性。

### 结构与职责

| 组件       | 职责                                 |
|------------|--------------------------------------|
| Model      | 数据与业务逻辑，不直接依赖 View/Controller |
| View       | 可视化模型，显示界面、界面控件         |
| Controller | 响应用户输入，调用 Model 或更新 View   |

- Model 使用 **Observer Pattern** 通知 View 和 Controller 更新。
- View 和 Controller 都可以访问和修改 Model。
- Controller 主要处理输入，View 主要处理输出。

### 常见误区

- Controller 并非用来分离 View 与 Model。分离关注点主要通过 Observer Pattern，而非 Controller。

---

## 3. Model-View-Presenter（MVP）

### 核心思想

从 MVC 演变而来，Controller 被 Presenter 取代。Presenter 处理更多业务逻辑，View 只负责展示。View 与 Presenter 通过接口交互，可完全脱离 UI 测试。

### 特点

- 提升可测试性
- 适合事件驱动 GUI 平台
- 变体：
  - **Passive View**：View 非常简单，Presenter 控制全部逻辑
  - **Supervising Controller**：部分逻辑留在 View 中

---

## 4. Model-View-ViewModel（MVVM）

### 核心思想

数据驱动：通过 Model 构建 ViewModel（数据模型），框架自动实现 ViewModel 和 View 的双向绑定。

### 特点

- View 和 ViewModel 自动同步，无需手动更新
- 高度可测试（ViewModel 不依赖 UI）
- 代表：声明式 UI 框架（QML、WPF、Jetpack Compose）

---

## 5. Presentation-Abstraction-Control（PAC）

### 核心思想

系统分为多个三元组（节点）：Presentation、Abstraction、Control。每个节点负责部分功能，节点之间可组合和嵌套。适合复杂、层次多的交互式应用。

### 与 MVC/MVP 区别

PAC 更强调层次结构和模块化。每个 PAC 节点既能独立工作，也能与其他节点协作。

---

## 6. 模式比较与选择建议

| 模式 | 优点                           | 适用场景           |
|------|--------------------------------|--------------------|
| MVC  | 分离界面与业务逻辑，支持多视图 | 早期 GUI、Web 框架 |
| MVP  | 可测试性高，逻辑集中           | 事件驱动 GUI       |
| MVVM | 数据驱动，自动绑定             | 声明式 UI 框架     |
| PAC  | 模块化、层次化                 | 大型多层系统       |

**选择原则**：从问题出发，理解模式背后的设计动机，不必严格按原始形式实现。核心价值在于**分离关注点，提高系统可维护性与可测试性**。

## Related

- [[设计模式]]
- [[观察者模式 Observer]]
- [[策略模式 Strategy]]
- [[中介者模式 Mediator]]
