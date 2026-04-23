---
tags:
  - database/mysql
  - type/howto
mastery: 5
---

1. 安装 MySQL
```shell
sudo apt-get install mysql-server
```
2. 进行初始化配置
```shell
sudo mysql_secure_installation
```
3. 检查 MySQL 状态
```shell
systemctl status mysql.service
```
4. 安装 MySQL 连接库
```shell
apt-get install libmysqlclient-dev
```

## 常用命令

启动MySQL

```shell
service mysqld start # 5.0 以前
service mysql start  # 5.0 以后
```

关闭MySQL

```shell
service mysqld stop
service mysql stop
```

检查 MySQL 状态
```shell
systemctl status mysql.service
```

## See also

- [[MySQL体系结构]] - MySQL 整体架构
- [[存储引擎]] - InnoDB 与其他引擎对比
