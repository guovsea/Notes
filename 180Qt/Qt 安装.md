---
tags:
  - qt/core
  - type/howto
mastery: 5
---
## Qt 安装方式

### Qt 5.14 及以前

可以直接下载预编译的二进制安装包，通过 Qt 官方维护工具（Maintenance Tool）管理安装。

下载地址：[Qt Official Download](https://download.qt.io/official_releases/qt/)

### Qt 5.15 及以后（包括 Qt 6）

自 2020 年起，Qt 公司改为在线安装器方式分发，需要在 [Qt 账户](https://www.qt.io/download) 注册并使用 **Qt Online Installer** 下载。

对于**商业/开源许可用户**，也可以从源码编译：

```shell
git clone https://code.qt.io/qt/qt5.git
cd qt5
git checkout <version-tag>
perl init-repository
mkdir build && cd build
../configure -prefix /path/to/install -opensource -nomake examples
make -j$(nproc)
make install
```

### 使用包管理器

```shell
# macOS (Homebrew)
brew install qt

# Ubuntu/Debian
sudo apt install qt6-base-dev

# vcpkg
vcpkg install qtbase
```
