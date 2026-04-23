---
tags:
  - tools
  - tools/clion
  - type/howto
mastery: 5
---
## 解决 CMake 报错不支持 Intel C++ Compiler 2025 toolset

不需要在 `CMakePresets.txt` 中设置 `toolset` 变量

## 每次 CMake 构建前自动加载 Intel oneAPI 环境

要让 CLion 在每次 CMake 构建前自动加载 Intel oneAPI 环境（Windows），最稳定、官方推荐的方式是：**直接在 CLion 工具链中绑定 `setvars.bat`**，让 IDE 每次生成 / 构建时自动初始化 oneAPI 环境。

### 一、核心原理（Windows）

Intel oneAPI 提供统一环境脚本：

```
C:\Program Files (x86)\Intel\oneAPI\setvars.bat
```

或（新版统一布局）

```
C:\Program Files (x86)\Intel\oneAPI\<version>\oneapi-vars.bat
```

运行它会一次性设置：`PATH`、`INCLUDE`、`LIB`、`MKLROOT`、`TBBROOT` 等所有编译器 / 库环境变量。

### 二、CLion 配置（推荐：工具链 + 环境文件）

#### 1. 打开工具链设置

- File → Settings → Build, Execution, Deployment → **Toolchains**
- 选中你当前用的工具链（如 Visual Studio /x64），点 **复制** 一份（命名为 `Intel oneAPI`）。
    
     ![image](https://p26-flow-imagex-sign.byteimg.com/labis/image/b9b9b05ce12f9ca73c3b15bd5b0fd93b~tplv-a9rns2rl98-pc_smart_face_crop-v1:512:384.image?lk3s=8e244e95&rcl=20260402143048183A8D82839C55C80AB9&rrcfp=cee388b0&x-expires=2090471458&x-signature=69uV9MkNNVioejXxbnJawzKEGWg%3D)
    

#### 2. 添加 oneAPI 环境

> 关键

- 点右上角 **添加环境 → 来自文件**
- 在 **环境文件** 填入：
    
    ```
    C:\Program Files (x86)\Intel\oneAPI\setvars.bat
    ```
    
    （如果是自定义安装目录，改成你的路径）
- 勾选 **为 CMake 使用此环境**、**为运行 / 测试使用此环境**JetBrains。

#### 3. 指定 Intel 编译器（可选）

- C 编译器：`icx.exe`
- C++ 编译器：`icx.exe` / `icpx.exe`
- 路径一般在：

    ```
    C:\Program Files (x86)\Intel\oneAPI\compiler\latest\windows\bin\icx.exe
    ```
    
- 应用 → 确定。

#### 4. 切换到该工具链

- 进入 **Build, Execution, Deployment → CMake**
- 在 **工具链** 下拉选你刚建的 **Intel oneAPI**。
- 重新加载 CMake 项目（Reload CMake Project）。

**效果**：

每次 CMake 配置 / 重新加载 / 构建时，CLion 都会先执行 `setvars.bat`，把完整 oneAPI 环境注入 CMake 进程，**不需要手动打开 Command Prompt**。