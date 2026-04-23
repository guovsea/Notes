---
tags:
  - tools/editor
  - type/reference
---
~/.zshrc

```bash
# User configuration

# 检查是否已经运行了 tmux 会话
if [[ -z "$TMUX" ]]; then
    # 检查是否有已存在的 tmux 会话
    if tmux has-session -t default 2>/dev/null; then
        # 如果存在，直接连接到该会话
        tmux attach -t default
    else
        # 如果不存在，创建一个新的 tmux 会话
        tmux new -s default
    fi
fi

export VCPKG_ROOT="/Users/guo/WorkSpace/vcpkg"
alias nv=nvim

alias proxyon="export http_proxy='http://127.0.0.1:4780';export https_proxy='http://127.0.0.1:4780'"
alias proxyoff="export http_proxy='';export https_proxy=''"
```