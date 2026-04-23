---
tags:
  - tools/editor
  - type/howto
mastery: 5
---

# VSCode 中使用 CMake 指定 include path

> 让 VSCode 的 IntelliSense 正确识别 CMake 项目中的头文件路径。

## 配置方法

在 `.vscode/c_cpp_properties.json` 中配置：

```json
{
    "configurations": [
        {
            "name": "Mac",
            "includePath": [
                "${workspaceFolder}/**"
            ],
            "defines": [],
            "macFrameworkPath": [
"/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/System/Library/Frameworks"
            ],
            "compilerPath": "/usr/bin/clang",
            // compile_commands.json 中包含 CMake 的编译命令，intelliSense 能据其提示
            "compileCommands": "${workspaceFolder}/build/compile_commands.json",
            "cStandard": "c17",
            "intelliSenseMode": "macos-clang-x64"
        }
    ],
    "version": 4
}
```

1. **使用插件 `CMake Tools` 编译**  ** 重点！**
   > 纯 CMake 不会在 `build` 中生成 `compile_commands.json` 文件
   
2. 创建 `.vscode/c_cpp_properties.json` 文件
   > 点击 `快速修复`，会自动创建该文件
3. 添加 `compileCommands` 属性

[c\_cpp\_properties.json reference](https://code.visualstudio.com/docs/cpp/c-cpp-properties-schema-reference)

