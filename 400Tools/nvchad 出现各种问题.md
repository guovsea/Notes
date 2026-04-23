---
tags:
  - tools/editor
  - type/pitfall
mastery: 5
---

# NvChad 出现各种问题

> NvChad 安装后出现各种异常，大多数情况下是网络问题导致依赖下载不完整。

## 原因

`git clone` 时网络不稳定，没有把依赖全部下载下来。

## 解决方法

删除已安装的 NvChad 目录，确保网络畅通后重新安装：

```shell
# 备份配置（如果有自定义修改）
mv ~/.config/nvim ~/.config/nvim.bak

# 删除 NvChad 及其数据
rm -rf ~/.config/nvim
rm -rf ~/.local/share/nvim
rm -rf ~/.local/state/nvim

# 重新安装
git clone https://github.com/NvChad/starter ~/.config/nvim
```