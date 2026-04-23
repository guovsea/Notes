---
tags:
  - act/project
  - database/sql
  - type/notes
---

# Redis 源码学习

## 参考资源

### Redis 3.0（带注释版本）

- [Redis 3.0 带详细注释源码](https://github.com/huangzworks/redis-3.0-annotated)
- [《Redis设计与实现》在线阅读](https://github.com/mailjobblog/book-redis-design)

下载 Redis 3.0 源码：

```shell
wget -O redis-3.0.tar.gz https://github.com/redis/redis/archive/refs/tags/3.0.tar.gz
cd ~/src
tar xvf redis-3.0.tar.gz
rm redis-3.0.tar.gz
```

> macOS 15 上编译 3.0 版本会出错，有两处代码需要修改。

### Redis 5.0.4

- [Redis源码解析（有赞技术）](https://tech.youzan.com/redisyuan-ma-jie-xi/)

## 相关

- [[Learning Path]]