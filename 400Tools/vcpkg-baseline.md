---
tags:
  - tools/cmake
  - type/reference
mastery: 5
---

# vcpkg Baseline 最佳实践

## 核心概念

vcpkg 清单模式（`vcpkg.json`）用 **`builtin-baseline`** 锁死依赖版本，取代了经典模式下的命令行 `install`。

```json
{
  "name": "my-project",
  "version": "0.1.0",
  "builtin-baseline": "296b89248ad13be09e9324488eb4f74dd322d26c",
  "dependencies": [
    "yaml-cpp",
    "eigen3",
    "gtest"
  ]
}
```

## 三件套

| 字段 | 作用 | 范围 |
|------|------|------|
| `builtin-baseline` | 锁定 vcpkg ports 树的 commit hash | 全局所有依赖 |
| `version>=` | 声明最低兼容版本 | 单个依赖 |
| `version<=` | 声明最高兼容版本 | 单个依赖 |

- **`builtin-baseline`** 是一个 commit hash，指向 vcpkg 仓库的某个提交。它确定了所有 port 的"版本天花板"——每个依赖只能装到那个 commit 时 ports 里定义的版本。
- **`version>=`** 是在 baseline 范围内的下限声明，防止 baseline 被回退到太旧的提交时，vcpkg 自动升级（不会超过 baseline）但仍低于你需要的版本。
- 两者配合：`builtin-baseline` 保证可重现，`version>=` 保证安全边界。

## 缓存机制

vcpkg 有三层缓存，**删除项目 `build` 目录不会影响它们**：

1. **下载缓存** (`$VCPKG_DOWNLOADS/`) — 源码 tarball 下载一次后永久缓存
2. **二进制缓存** (`$VCPKG_ROOT/packages/`) — 编译好的包
3. **归档缓存** (`~/.cache/vcpkg/archives/`) — 打包好的二进制压缩包，重装也能直接恢复

所以 `rm -rf build && cmake ...` 后，vcpkg 不会重新下载或重新编译。

## 获取 baseline hash

```bash
cd $VCPKG_ROOT
git log -1 --format=%H
```

## 升级依赖

1. 更新 vcpkg：`cd $VCPKG_ROOT && git pull`
2. 获取新 hash：`git log -1 --format=%H`
3. 更新 `vcpkg.json` 中的 `builtin-baseline`
4. 运行 `vcpkg install`

vcpkg 会自动检测 baseline 变化，只更新/重新编译受影响的包。

## 注意事项

- `builtin-baseline` **不是**你项目 vcpkg 子模块的 hash，而是 vcpkg 仓库本身的 commit hash
- `version>=` 不会超过 `builtin-baseline` 定义的版本范围
- manifest 模式下 `vcpkg install` 不能指定包名，直接运行即可
- 如果 vcpkg 端口文件被修改但 baseline 不变，vcpkg 不会感知——这就是为什么需要更新 baseline
