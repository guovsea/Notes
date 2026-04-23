---
tags:
  - tools/docker
  - type/reference
mastery: 5
---

# Docker 常用操作

## 安装 MySQL

```shell
docker run \
--name mysql \
-v /home/mysql/log:/var/log/mysql \
-v /home/mysql/data:/var/lib/mysql \
-v /home/mysql/conf:/etc/mysql/conf.d \
-p 3306:3306 \
-e MYSQL_ROOT_PASSWORD=gchhgchh \
-d mysql \
--init-connect="SET collation_connection=utf8mb4_0900_ai_ci" \
--init-connect="SET NAMES utf8mb4" \
--skip-character-set-client-handshake
```
```shell
sudo apt-get install mysql-client

mysql -h 127.0.0.1 -P 3306 -u root -p
```

[写最好的Docker安装最新版MySQL8（mysql-8.0.31）教程（参考Docker Hub和MySQL官方文档）\_docker mysql8\_大白有点菜的博客-CSDN博客](https://blog.csdn.net/u014282578/article/details/127920419)

*设置密码后登录时仍然密码错误：可能时挂载时（-v）挂载的配置文件中设置了密码，覆盖了 mysql 环境变量密码*
