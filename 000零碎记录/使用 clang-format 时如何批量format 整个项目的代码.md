### 1. **确保环境准备**

- 安装 `clang-format`（建议使用 LLVM 官方版本）
    
- 在项目根目录创建配置文件（如 `.clang-format` 或 `_clang-format`）
### 2. **批量格式化命令**

#### 在 **Linux/macOS**（使用 `find` + `xargs`）:  
在**linux/macOS** （使用`find` + `xargs` ）：

```bash
# 格式化所有 .h/.cpp 文件（排除第三方目录）
find . -type d \( -path ./third_party -o -path ./build \) -prune -o \
    \( -iname "*.h" -o -iname "*.cpp" -o -iname "*.hpp" -o -iname "*.c" \) \
    -print0 | xargs -0 clang-format -i --verbose
```

#### 在 **Windows**（使用 PowerShell）:

```bash
# 格式化所有 .h/.cpp 文件（排除 bin 目录）
Get-ChildItem -Recurse -Include *.h,*.cpp,*.hpp,*.c -Exclude bin | ForEach-Object {
    clang-format -i $_.FullName
    Write-Host "Formatted: $($_.FullName)"
}
```
### 3. **关键参数解释**

- `-i`: 直接修改文件（不加此参数会只输出预览）
    
- `--verbose`: 显示已格式化的文件名
    
- `-type d \( ... \) -prune`: 排除指定目录（如第三方库或构建目录）