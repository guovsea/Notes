---
tags:
  - tools/git
  - type/howto
mastery: 5
---

# Git 设置代理

> 为 Git 的 HTTP/HTTPS 传输设置代理服务器。

## 设置代理

```bash
git config --global http.proxy 'http://192.168.0.1:1080'
git config --global https.proxy 'http://192.168.0.1:1080'
```

## 取消代理

```bash
git config --global --unset http.proxy
git config --global --unset https.proxy
```

## 仅对 GitHub 设置代理

```bash
git config --global http.https://github.com.proxy 'http://192.168.0.1:1080'
```