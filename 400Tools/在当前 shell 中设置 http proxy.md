---
tags:
  - type/howto
---
可以解决很多下载配置失败的问题


**Windows**

```shell
$env:HTTP_PROXY="socks5://127.0.0.1:4781"
$env:HTTPS_PROXY="socks5://127.0.0.1:4781"
```

**Unix**

```shell
export HTTP_PROXY="socks5://127.0.0.1:4781"
export HTTPS_PROXY="socks5://127.0.0.1:4781"
```