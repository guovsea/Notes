---
tags:
  - ai/tools
  - type/analysis
mastery: 5
---

# Claude Code Skill 完整调研报告

> 调研时间：2026-04-19 | 重点侧重：开发实践

---

## 目录

1. [什么是 Skill](#一什么是-skill)
2. [Skill 的使用方式](#二skill-的使用方式)
3. [Skill 与 MCP 的区别](#三skill-与-mcp-的区别)
4. [好用的 Skill 推荐](#四好用的-skill-推荐)
5. [Skill 获取途径](#五skill-获取途径)
6. [如何创建 Skill（开发实践）](#六如何创建-skill开发实践)
7. [用 skill-creator 创建 Skill](#七用-skill-creator-创建-skill)
8. [最佳实践与反模式](#八最佳实践与反模式)
9. [总结与建议](#九总结与建议)

---

## 一、什么是 Skill

### 1.1 定义

**Skill 是一个结构化的 prompt 模板**——具体来说是一个 `SKILL.md` 文件，包含 YAML 前置元数据（frontmatter）和 Markdown 正文（指令）。它不是传统意义上的插件或编译工具，而是一个**带有发现元数据的 prompt 包**，深度集成在 Claude Code 运行时中。

Skill 遵循开放的 **Agent Skills 标准**（agentskills.io），可在不同 AI 工具间移植，Claude Code 在此基础上增加了自动调用、子代理执行、动态上下文注入等扩展能力。

### 1.2 核心特征

| 特征 | 说明 |
|------|------|
| **懒加载** | 仅 `name` + `description` 常驻上下文（~100 tokens），正文只在调用时加载 |
| **双触发模式** | 用户可通过 `/skill-name` 手动调用，Claude 也可根据上下文自动调用 |
| **零依赖** | 纯 Markdown 文件，无需运行服务器进程 |
| **可组合** | Skill 可引用其他 Skill，可包含脚本、模板、参考文档等辅助文件 |
| **可版本控制** | 作为纯文本文件可直接纳入 Git 管理 |

### 1.3 Skill 的演进

Skill 取代了旧的"自定义斜杠命令"系统（`.claude/commands/*.md`）。旧格式仍然兼容，但 Skill 支持更多功能：frontmatter 控制、辅助文件、子代理隔离、自动调用等。当 Skill 和旧命令同名时，**Skill 优先**。

### 1.4 文件结构

**最小结构：**

```
my-skill/
  SKILL.md           # 唯一必需文件
```

**完整结构：**

```
my-skill/
  SKILL.md           # 主指令文件（必需）
  template.md        # 输出模板
  examples/
    sample.md        # 示例输出
  scripts/
    validate.sh      # 可执行脚本
    helper.py        # 辅助工具
  references/
    api-docs.md      # 参考文档（按需加载，零 token 消耗直到被访问）
```

---

## 二、Skill 的使用方式

### 2.1 触发方式

Skill 有两种触发方式：

#### A. 用户手动调用（显式）

在 Claude Code CLI 中输入 `/` 后跟上 Skill 名称：

```
/commit                        # 无参数调用
/explain-code src/auth.ts      # 带参数调用
/fix-issue 1234                # 带参数调用
/smart-review                  # 执行代码审查
```

参数通过 `$ARGUMENTS`、`$0`、`$1` 等变量传递给 Skill。

#### B. 模型自动调用（隐式）

Claude 根据对话上下文和 Skill 的 `description` 自动判断是否需要调用某个 Skill。例如用户说"帮我审查一下代码变更"，Claude 可能自动调用 `smart-review` Skill。

可通过 frontmatter 控制此行为：

| 设置 | 用户可调用 | Claude 可调用 |
|------|-----------|--------------|
| 默认 | 是 | 是 |
| `disable-model-invocation: true` | 是 | 否 |
| `user-invocable: false` | 否 | 是 |

### 2.2 执行生命周期

```
1. 会话启动 → 扫描所有 Skill 目录，将 name + description 注入上下文
2. 触发调用 → 执行 `!`command`` 语法中的 Shell 命令（动态注入输出）
3. 变量替换 → $ARGUMENTS、$0、$1、${CLAUDE_SESSION_ID} 等变量解析
4. 内容加载 → 渲染后的内容作为单条消息进入对话
5. 上下文压缩 → 已调用的 Skill 前 5,000 tokens 被保留（总计预算 25,000 tokens）
```

### 2.3 动态上下文注入

Skill 支持 `` !`command` `` 语法，在调用时执行 Shell 命令并将输出注入内容：

```markdown
当前分支: !`git branch --show-current`

暂存的文件变更:
!`git diff --staged --stat`
```

命令在 Skill 内容发送给 Claude **之前**执行，所以 Claude 收到的是实际数据而非命令本身。

### 2.4 存储位置（作用域）

| 作用域 | 路径 | 适用范围 |
|--------|------|---------|
| 企业级 | 通过托管设置分发 | 全组织用户 |
| 个人级 | `~/.claude/skills/<skill-name>/SKILL.md` | 你的所有项目 |
| 项目级 | `.claude/skills/<skill-name>/SKILL.md` | 仅当前项目 |
| 插件级 | `<plugin>/skills/<skill-name>/SKILL.md` | 启用该插件的项目 |

**优先级**：企业 > 个人 > 项目。插件 Skill 使用 `plugin-name:skill-name` 命名空间，不会冲突。

### 2.5 权限控制

可通过 settings.json 限制 Skill 的使用：

```bash
# 禁止所有 Skill
Skill

# 仅允许特定 Skill
Skill(commit)
Skill(review-pr *)

# 禁止特定 Skill
Skill(deploy *)
```

### 2.6 实时更新

Claude Code 监视 Skill 目录的文件变更。**添加、编辑、删除 Skill 在当前会话内即时生效**，无需重启。仅新建顶级 `.claude/skills/` 目录需要重启。

---

## 三、Skill 与 MCP 的区别

### 3.1 什么是 MCP

**MCP（Model Context Protocol）** 是一个基于 JSON-RPC 2.0 的开放协议标准，用于连接 AI 应用与外部工具、数据源和 API。可以理解为"AI 的 USB-C 接口"——一个标准化协议连接多种服务。

MCP 服务器作为独立进程运行（本地 stdio 或远程 HTTP/SSE），Claude 在会话开始时发现其工具并通过协议调用。

### 3.2 对比表

| 维度 | Skill | MCP | Hook |
|------|-------|-----|------|
| **本质** | 可复用的 Markdown 指令包 | 连接外部工具的开放协议 | 生命周期事件自动化 |
| **层级** | 应用层（"怎么做好事"） | 基础设施层（"能做什么"） | 执行层（"必须做什么"） |
| **触发** | 用户 `/` 调用或 AI 自动判断 | AI 决定何时调用工具 | 确定性事件驱动 |
| **智能性** | AI 解释执行工作流 | AI 推理工具使用 | 无 AI 推理，纯脚本 |
| **Token 消耗** | ~100 tokens/skill（元数据） | ~2,000+ tokens/服务器 | 零（上下文外执行） |
| **跨平台** | 仅 Claude Code | 任何 MCP 兼容客户端 | 仅 Claude Code |
| **格式** | Markdown 文件 | JSON-RPC 服务器（任何语言） | Shell 命令 / HTTP |
| **外部访问** | 不能 | 能（API、数据库、浏览器等） | 能（Shell 命令） |
| **认证** | 不适用 | OAuth 2.0、API Key 等 | 不适用 |
| **确定性** | 非确定（LLM 解释执行） | 确定（代码执行） | 完全确定 |
| **依赖** | 无（纯文本） | 需要服务器进程 | 需要 Shell/HTTP |
| **版本控制** | 天然支持（纯文件） | 部分支持（配置文件） | 部分支持 |

### 3.3 成本对比

基于 Scalekit 的 75 次基准测试（Claude Sonnet 4，相同 GitHub 任务）：

| 方式 | Token 消耗（"这个仓库用的什么语言？"） | 预估月费（10K 次操作） |
|------|--------------------------------------|----------------------|
| CLI 裸调用 | 1,365 tokens | ~$3.20 |
| CLI + Skills | 4,724 tokens | ~$4.50 |
| MCP（直接） | 44,026 tokens | ~$55.20 |

**Skill 比 MCP 节省约 10-30 倍 Token**（MCP Tool Search 功能可缩小此差距）。

### 3.4 何时用哪个

| 场景 | 推荐 |
|------|------|
| 反复输入相同的多步骤 prompt | **Skill** |
| 编码团队规范和工作流知识 | **Skill** |
| 需要连接数据库、外部 API | **MCP** |
| 需要认证、实时数据、副作用 | **MCP** |
| 每次都必须执行的强制动作 | **Hook** |
| 文件编辑后自动格式化 | **Hook** |
| 阻止危险命令 | **Hook** |

### 3.5 三者协作示例

```
MCP 服务器 → 连接数据库和 Slack
Skill      → 定义部署工作流（验证、清单、通知格式）
Hook       → 每次编辑后自动运行 Linter + 关键文件变更时发 Slack 通知
```

---

## 四、好用的 Skill 推荐

### 4.1 官方内置 Skill（随 Claude Code 附带）

| Skill | 功能 | 调用方式 |
|-------|------|---------|
| `/commit` | 分析 git diff，生成规范的 commit message | `/commit` |
| `/simplify` | 审查变更代码的复用性、质量和效率，并修复问题 | `/simplify` |
| `/debug` | 系统化调试流程 | `/debug` |
| `/batch` | 批量处理多个相关任务 | `/batch` |
| `/loop` | 按间隔重复执行某个命令 | `/loop 5m /check-deploy` |
| `/claude-api` | 构建使用 Claude API 或 Anthropic SDK 的应用 | `/claude-api` |

### 4.2 官方验证插件 Skill

| Skill | 功能 | 安装量 |
|-------|------|--------|
| `skill-creator` | 创建、测试、评估、优化其他 Skill 的元技能 | 130,000+ |
| `find-skills` | 发现和安装社区 Skill | 随 Claude Code 附带 |

### 4.3 实用 Skill 模板示例

#### 代码审查 Skill

```yaml
---
name: smart-review
description: >
  审查 Git 暂存区的安全漏洞、性能问题和可维护性。
  当用户要求审查代码、审计变更或提到 code review 时触发。
allowed-tools: Read, Grep, Bash(git:*)
---

# 智能代码审查

## 流程

1. 运行 `git diff --staged` 查看所有暂存变更
2. 对每个变更文件分析：
   - **安全性**：SQL 注入、XSS、命令注入、暴露密钥、认证绕过
   - **性能**：N+1 查询、不必要重渲染、内存泄漏、缺失索引
   - **可维护性**：命名清晰度、函数长度、重复代码、错误处理

## 输出格式

- **[严重级别]**：Critical / Warning / Info
- **文件**：path/to/file.ts:行号
- **问题**：描述
- **修复建议**：具体代码修复

最后输出一行总结："发现 N 个问题，涉及 M 个文件。"
```

#### 部署检查清单 Skill

```yaml
---
name: deploy
description: 部署前运行完整验证流程
disable-model-invocation: true    # 防止 Claude 自动部署！
allowed-tools: Bash(git:*) Bash(npm:*)
context: fork
---

# 部署到 $ARGUMENTS

1. 运行测试套件
2. 运行类型检查
3. 运行 Lint
4. 构建应用
5. 推送到部署目标
6. 验证部署成功
```

---

## 五、Skill 获取途径

### 5.1 官方渠道

| 渠道 | 地址 | 说明 |
|------|------|------|
| Anthropic 官方文档 | `https://docs.claude.com/en/docs/claude-code/skills` | 最权威的参考 |
| 官方插件仓库 | `github.com/anthropics/claude-plugins-official` | Anthropic 验证的插件 |
| `find-skills` Skill | 在 Claude Code 中运行 `/find-skills` | 直接搜索和安装 |

### 5.2 社区渠道

| 渠道 | 说明 |
|------|------|
| GitHub 搜索 | 搜索 `claude-code-skill`、`agent-skills` 等关键词 |
| npx skills CLI | `npx skills add <org/repo>@<skill-name>` 一键安装 |
| Awesome 列表 | GitHub 上有社区维护的 skill 集合 |
| 社区论坛 | Anthropic Discord、Reddit r/ClaudeAI |

### 5.3 安装方式

#### 方式一：插件管理器

```bash
# 在 Claude Code 中
/plugin manage
# 搜索并安装

# 或命令行
claude plugin install skill-creator
```

#### 方式二：npx skills CLI

```bash
npx skills add denser-org/claude-skills@denser-retriever
```

#### 方式三：手动创建

```bash
# 项目级
mkdir -p .claude/skills/my-skill
# 创建 SKILL.md

# 个人级
mkdir -p ~/.claude/skills/my-skill
# 创建 SKILL.md
```

---

## 六、如何创建 Skill（开发实践）

### 6.1 SKILL.md 格式详解

SKILL.md 由两部分组成：**YAML frontmatter**（元数据）和 **Markdown 正文**（指令）。

```yaml
---
# ===== YAML Frontmatter =====

name: my-skill                    # 斜杠命令名（kebab-case，最长 64 字符）
description: 一句话描述功能和使用场景  # 触发判断依据（最长 1024 字符）
when_to_use: 补充触发条件           # 追加到 description 后
argument-hint: [参数提示]           # 自动补全提示
disable-model-invocation: false    # true = 仅用户可调用
user-invocable: true               # false = 对用户隐藏
allowed-tools: Read Grep Bash(git:*)  # 预授权工具列表
context: fork                      # fork = 在隔离子代理中运行
agent: Explore                     # 子代理类型（Explore / Plan / 自定义）
model: sonnet                      # 覆盖模型
effort: high                       # 覆盖努力级别
paths: "src/**/*.ts"               # 自动激活的文件路径匹配
shell: bash                        # Shell 类型（bash / powershell）
---

# ===== Markdown 正文 =====
这里是 Claude 遵循的指令内容...
```

### 6.2 完整 Frontmatter 字段参考

| 字段 | 必需 | 说明 |
|------|------|------|
| `name` | 否 | 显示名称，默认为目录名 |
| `description` | 推荐 | 功能描述 + 触发条件 |
| `when_to_use` | 否 | 补充触发上下文 |
| `argument-hint` | 否 | 参数自动补全提示 |
| `disable-model-invocation` | 否 | 是否禁止 Claude 自动调用（默认 false） |
| `user-invocable` | 否 | 是否对用户可见（默认 true） |
| `allowed-tools` | 否 | 预授权工具列表 |
| `model` | 否 | 覆盖使用的模型 |
| `effort` | 否 | 覆盖努力级别 |
| `context` | 否 | 设为 `fork` 则在隔离子代理中运行 |
| `agent` | 否 | 子代理类型 |
| `hooks` | 否 | 该 Skill 专属的生命周期钩子 |
| `paths` | 否 | 自动激活的 glob 路径 |
| `shell` | 否 | Shell 类型（bash/powershell） |

### 6.3 变量替换

| 变量 | 说明 |
|------|------|
| `$ARGUMENTS` | 所有传入参数 |
| `$ARGUMENTS[N]` / `$N` | 按索引取参数（从 0 开始） |
| `${CLAUDE_SESSION_ID}` | 当前会话 ID |
| `${CLAUDE_SKILL_DIR}` | Skill 所在目录 |

### 6.4 开发流程（推荐 A/B 模式）

```
1. 创建目录：    mkdir -p .claude/skills/my-skill
2. 编写 SKILL.md：frontmatter（name, description）+ markdown 指令
3. 添加辅助文件（可选）：references/、scripts/、templates/
4. 手动测试：    在 Claude Code 中输入 /my-skill
5. 使用 skill-creator 运行评估
6. 迭代优化：    Claude A（编写）→ Claude B（测试）循环
7. 发布分享：    提交到 Git 或发布到市场
```

**Claude A/B 迭代法**：
- **Claude A（编写会话）**：创建或编辑 SKILL.md
- **Claude B（测试会话）**：全新实例加载 Skill，在真实任务上测试
- 观察 Claude B 的偏差和遗漏，回到 Claude A 精准改进
- 重复直到满意

---

## 七、用 skill-creator 创建 Skill

### 7.1 什么是 skill-creator

`skill-creator` 是 Anthropic 官方的**元技能**——一个专门用来构建、测试和优化其他 Skill 的 Skill。安装量 130,000+，是 Skill 开发的核心工具。

**安装**：

```bash
# 在 Claude Code 中
/plugin manage
# 搜索 "skill-creator" 并安装

# 或命令行
claude plugin install skill-creator
```

### 7.2 四种工作模式

| 模式 | 用途 | 何时使用 |
|------|------|---------|
| **Create** | 从零开始创建新 Skill | 构建新 Skill |
| **Eval** | 对 Skill 运行测试并评分 | 验证 Skill 是否正常工作 |
| **Improve** | 执行 + 评分 + 盲测 A/B 对比 + 分析迭代 | 改进现有 Skill |
| **Benchmark** | 多次运行 + 方差分析 | 统计学级别的性能测量 |

### 7.3 四个子代理

| 代理 | 职责 |
|------|------|
| **Executor** | 对 eval prompt 运行 Skill |
| **Grader** | 根据定义的预期输出评估结果 |
| **Comparator** | 盲测 A/B 对比（Skill 输出 vs 基线） |
| **Analyzer** | 分析结果并提出针对性改进建议 |

### 7.4 实战：用 skill-creator 创建新 Skill

**Step 1**：调用
```
/skill-creator
```

**Step 2**：描述需求
```
创建一个名为 security-review 的 Skill，
审计 WordPress 插件的 PHP 和 JS 代码，
覆盖 8 个漏洞类别，输出按严重级别标记的结构化报告。
```

**Step 3**：回答 skill-creator 的结构化访谈问题
- 这个 Skill 应该做什么？
- 什么时候应该触发？
- 处理哪些文件类型？
- 期望什么输出格式？

**Step 4**：skill-creator 自动完成
1. 起草 SKILL.md
2. 生成测试用例
3. 使用隔离子代理并行运行测试（无交叉污染）
4. 评分并生成 eval viewer（HTML 侧边对比视图）
5. 产出基准报告（通过率、Token 用量、耗时）

**Step 5**：迭代优化
```
/skill-creator
"改进我的 security-review skill——缺少了 changelog 验证步骤"
```

### 7.5 Eval 文件格式

skill-creator 创建并使用 `evals/evals.json`：

```json
{
  "skill_name": "security-review",
  "evals": [
    {
      "id": 1,
      "prompt": "审查这个 WordPress 插件文件的安全问题",
      "expected_output": "按严重级别分类的发现，带行号引用",
      "files": ["test-files/vulnerable-plugin.php"],
      "assertions": [
        {"name": "has_severity_ratings", "expected": true},
        {"name": "covers_owasp_categories", "expected": true},
        {"name": "includes_code_fixes", "expected": true}
      ]
    }
  ]
}
```

### 7.6 Description 优化

skill-creator 可以专门优化 Skill 的触发准确性：

1. 生成 20 个触发测试查询（应触发 + 不应触发的混合）
2. 测试 Claude 是否正确激活该 Skill
3. 分析假阳性和假阴性
4. 建议描述修改方案

### 7.7 结果解读

| 胜率 | 建议 |
|------|------|
| 70%+ | 保留——明确有用 |
| 50%-70% | 边缘——考虑优化或等待模型更新后退休 |
| <50% | 移除或重写——在拖后腿 |

---

## 八、最佳实践与反模式

### 8.1 最佳实践

#### Practice 1：渐进式披露

保持 SKILL.md 在 500 行以内，详细内容放到辅助文件中：

```
skill/
  SKILL.md              # 概览 + 导航（< 500 行）
  references/
    advanced-topics.md  # 按需加载
    api-reference.md    # 按需加载
  scripts/
    validate.py         # 执行而非加载到上下文
```

#### Practice 2：反馈循环

```
1. 修改代码
2. 验证：python scripts/validate.py
3. 如果失败：修复，回到 Step 2
4. 通过后才继续
5. 重建：python scripts/pack.py output.docx
```

#### Practice 3：提供输入/输出示例

```markdown
## Commit message 格式

示例 1：
Input: "Added user authentication with JWT tokens"
Output: "feat(auth): implement JWT-based authentication"

示例 2：
Input: "Fixed bug where dates displayed incorrectly"
Output: "fix(reports): correct date formatting in timezone conversion"
```

#### Practice 4：聚焦且可组合

一个 Skill 对应一个工作流。三个聚焦的 Skill > 一个"万能" Skill。

```
# 好：聚焦
react-performance-expert
api-design-expert
css-layout-expert

# 差：臃肿
web-dev-expert    # 太宽泛，无法正确触发
```

#### Practice 5：使用工具脚本

预制的脚本比生成代码更可靠，节省 Token 并确保一致性。

```markdown
运行 `python scripts/analyze_form.py input.pdf` 来提取字段。
```

### 8.2 反模式（避免）

| 反模式 | 问题 | 正确做法 |
|--------|------|---------|
| **万能 Skill** | 一个 Skill 覆盖整个领域，太宽泛无法正确触发 | 拆分为多个聚焦的 Skill |
| **模糊描述** | "帮助处理 X, Y, Z 等"导致误触发或漏触发 | 明确写 [功能] + [触发条件] + [排除场景] |
| **模板堆砌** | 90% 模板 10% 实际指令 | 编码判断逻辑而非空模板 |
| **幽灵引用** | 引用了不存在的文件路径 | 验证所有引用路径 `find skill-dir/ -type f` |
| **孤立文件** | references/ 有文件但 SKILL.md 从未提及何时读取 | 明确写"当需要 X 时，读取 references/xxx.md" |
| **工具过载** | `allowed-tools: Read,Write,Edit,Bash,Glob,Grep,Task,...` | 只授权必需的最小工具集 |
| **对话语气** | "请帮我审查一下代码好吗？" | 使用祈使句："审查所有变更文件" |
| **安装过多** | 同时激活 10+ 个 Skill | 保持 5-8 个活跃 Skill |
| **深层嵌套引用** | SKILL.md → a.md → b.md → c.md | 引用层级不超过 1 层 |

### 8.3 Description 编写黄金法则

**用第三人称**（描述会被注入系统 prompt）：

```yaml
# 好
description: "提取 PDF 中的文本和表格，填写表单，合并文档。当用户处理 PDF 文件或提到 PDF、表单、文档提取时使用。"

# 差（第一人称）
description: "我可以帮你处理 PDF 文件"

# 差（模糊）
description: "帮助处理文档"
```

**描述模板**：

```yaml
description: >
  [一句话核心能力]。当 [具体场景] 时使用。
  触发关键词："[关键词1]"、"[关键词2]"。
  不适用于 [排除场景]。
```

---

## 九、总结与建议

### 9.1 推荐的 Skill 配置方案

```
MCP 服务器：2-4 个  → 连接日常外部服务（GitHub、数据库、Sentry 等）
Skill：     5-10 个 → 团队可复用工作流（部署、审查、迁移等）
Hook：      2-3 个  → 不可协商的自动化（编辑后 lint、阻止 .env 写入等）
```

### 9.2 Skill 的两种类型

| 类型 | 说明 | 注意事项 |
|------|------|---------|
| **能力增强型** | 教 Claude 做它自己做不到的事 | 随模型升级可能过时，定期评估 |
| **偏好编码型** | 将 Claude 能做的步骤按特定顺序编排 | 随模型升级更有价值，防止回归 |

### 9.3 关键要点

1. **Skill 是最轻量的扩展方式**——纯 Markdown、零依赖、Token 高效
2. **MCP 适合外部连接**——数据库、API、浏览器等
3. **Hook 适合强制执行**——每次都必须发生的动作
4. **skill-creator 是开发利器**——用 Skill 创建 Skill，自带评估和基准测试
5. **Description 是灵魂**——写好 description，触发准确性就解决了一半
6. **保持 Skill 数量克制**——5-8 个活跃 Skill 是最佳范围

---

## 参考资料

| 资源 | 链接 |
|------|------|
| 官方 Skill 文档 | https://docs.claude.com/en/docs/claude-code/skills |
| 官方 Best Practices | https://docs.claude.com/en/docs/agents-and-tools/agent-skills/best-practices |
| skill-creator 源码 | https://github.com/anthropics/claude-plugins-official |
| MCP 协议规范 | https://modelcontextprotocol.io/introduction |
| LangChain skills-benchmarks | https://github.com/langchain-ai/skills-benchmarks |
| Agent Skills 开放标准 | https://agentskills.io |
