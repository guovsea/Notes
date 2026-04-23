---
tags:
  - compiler/linking
  - type/concept
mastery: 5
---
## 相关

- [[链接静态库]] -- 静态库的链接解析顺序
- [[PE Portable Executable]] -- DLL 属于 PE 文件格式

## 静态库

在可执行程序运行前就已经加入到执行码中，成为执行程序的一部分。

## 共享库

在执行程序启动时加载到执行程序中，可以被多个执行程序共享使用。

### 创建方法

```shell
gcc -fPIC -c a.c 
gcc -fPIC -c b.c   
gcc -shared -Wl -o libmyab.so a.o b.o
```
1. `-c` 生成目标文件，并使用 `-fPIC`（或 `-fpic`）创建位置无关代码（Position Independent Code）
2. 使用 `-shared -o` 生成动态链接库（`-Wl` 用于传递链接器选项，这里可省略）

### 共享库的命名

**_libname. so. x. y. z_**

即**前缀**"_lib_"+库名称+**后缀**"_. so_"+三个数字组成的**版本号**，其中，**x** 表示**主版本号**，**y** 表示**次版本号**，**z** 表示**发布版本号**。

按照共享库的命名惯例，每个共享库有三个文件名: Real Name、SONAME 和 Linker Name。

- **Real Name**
  这是共享库的实际文件名，通常以 `.so` 作为文件扩展名。例如，`libexample.so.x.y.z` 的实际名称是 `libexample.so.x.y.z`。这个名称标识了共享库的具体文件。
- **SONAME**
  SONAME 就是把共享库的文件名**去掉次版本号和发布版本号，只保留主版本号**。`libname.so.x.y.z` 的 SONAME 就是 `libname.so.x`
- **Linker Name**
  这是在编译和链接应用程序时使用的名称，通常是没有版本号的实际名称，如 `libexample.so`。Linker Name 是在编译器和链接器的命令中使用的，用于指定要链接的共享库。在链接过程中，通常会创建一个软链接，将 Linker Name 链接到具体版本的共享库（SONAME）上。当共享库更新时，这个软链接会**始终指向主版本号相同、次版本号和发布版本号最新**的那个库文件。
```shell
lrwxrwxrwx   1 root root     16 Oct 31  2022 libdmmp.so -> libdmmp.so.0.2.0
-rw-r--r--   1 root root  34888 Oct 31  2022 libdmmp.so.0.2.0
lrwxrwxrwx   1 root root     18 Mar  3  2020 libhandle.so.1 -> libhandle.so.1.0.3
-rw-r--r--   1 root root  14416 Mar  3  2020 libhandle.so.1.0.3
lrwxrwxrwx   1 root root     16 Oct 31  2022 libmpathcmd.so -> libmpathcmd.so.0
-rw-r--r--   1 root root  14328 Oct 31  2022 libmpathcmd.so.0
lrwxrwxrwx   1 root root     20 Oct 31  2022 libmpathpersist.so -> libmpathpersist.so.0
-rw-r--r--   1 root root  34808 Oct 31  2022 libmpathpersist.so.0
lrwxrwxrwx   1 root root     17 Oct 31  2022 libmultipath.so -> libmultipath.so.0
-rw-r--r--   1 root root 384848 Oct 31  2022 libmultipath.so.0

# Linker Name 可以链接到 SONAME 上， SONAME 可以链接到 Real Name 上
```

**加载共享库的方法**

1. 拷贝自己制作的共享库到 `/lib` 或者 `/usr/lib ` 
2. 临时设置 `LD_LIBRARY_PATH`：`export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:库路径`
3. 永久设置，把 `export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:库路径` 设置到 `∼/.bashrc` 或者 `/etc/profile`
4. 设置 `/etc/ld.so.conf`, 把库所在路径追加到此配置文件里

## **链接动态库的方法**

可以在程序启动时加载动态库，也可以在程序运行时通过某些系统调用加载。

编写编译命令时，使用 SONAME 而不是具体的 Real Name。例如，如果您的共享库的 SONAME 为 `libexample.so.1`，则您可以使用以下方式来编译：

```shell
gcc -lexample
```

在程序运行中，可以使用系统提供的某些 API 来加载，例如

```c
void * dlopen(const char *filename, int flag);  // Unix中打开一个函数库
LoadLibrary // Windows 中

```

#### 动态库

Dynamically loaded libraries (DDL) 和共享库是同一个概念，只是“共享库”是 Unix-like 的术语，“DDL”是 Windows 上的术语。

动态库在 Unix-Like 系统中就是共享库 `.so` 文件，在 Windows 中是 `.dll` 文件，共享库既可以在


### 链接库查找路径

**静态库链接时搜索路径顺序**：

1. GCC 命令中的参数-L  
2. gcc 的环境变量 LIBRARY_PATH  
3. 内定目录 /lib /usr/lib /usr/local/lib 这是当初 compile gcc 时写在程序内的

**共享库链接时、执行时搜索路径顺序**:

1. GCC 命令中的参数-L   
2. 环境变量 LD_LIBRARY_PATH 指定的动态库搜索路径  
3. 配置文件/etc/ld.so.conf 中指定的动态库搜索路径  
4. 默认的动态库搜索路径/lib  
5. 默认的动态库搜索路径/usr/lib

有关环境变量：  
LIBRARY_PATH 环境变量：指定程序静态链接库文件搜索路径  
LD_LIBRARY_PATH 环境变量：指定程序动态链接库文件搜索路径