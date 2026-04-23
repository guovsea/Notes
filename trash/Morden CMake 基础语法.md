---
tags:
  - tools
  - tools/cmake
mastery: 5
---
##  [[变量]]

## programing

### control flow

#### if-else

```CMake
if(variable)
    # If variable is `ON`, `YES`, `TRUE`, `Y`, or non zero number
else()
    # If variable is `0`, `OFF`, `NO`, `FALSE`, `N`, `IGNORE`, `NOTFOUND`, `""`, or ends in `-NOTFOUND`
endif()
```

**关键字**

- Unary: `NOT`, `TARGET`, `EXISTS` (file), `DEFINED`, etc.
- Binary: `STREQUAL`, `AND`, `OR`, `MATCHES` (regular expression), `VERSION_LESS`, `VERSION_LESS_EQUAL` (CMake 3.7+), etc.
#### foreach

```CMake
foreach(每个元素 ${列表1} ${列表2} ...)
  循环体
endforeach()
```

**注意事项**


### generator expression

[[generator expression]]
## 与文件内容交互

### 写文件

原理：用 CMake 中定义的变量替换某个文件中以 `@` 包裹的变量

```CMake
configure_file (
    "${PROJECT_SOURCE_DIR}/Version.h.in" # 被替换的文件
    "${PROJECT_BINARY_DIR}/Version.h"    # 替换后生成的文件
)
```

[[传递 CMake 中的变量到文件中]]

## 运行其他程序

### Running a command at configure time

在配置期间执行命令

```CMake
execute_process()

# 调用系统的可执行程序 echo
execute_process(COMMAND echo "hello cmake")

# 调用 CMake
execute_process(COMMAND ${CMAKE_COMMAND} -E echo "hello")

# ${CMAKE_COMMAND} 是可执行程序 CMake 
message(${CMAKE_COMMAND}) # /usr/local/Cellar/cmake/3.26.4/bin/cmake
```

### Running a command at build time

在构建期间执行命令

```CMake
# 定义生成某文件的规则
add_custom_command(
    OUTPUT output.txt  # 生成的文件名
    COMMAND echo "${PROJECT_NAME}" > output.txt
    DEPENDS
    COMMENT "Running custom command to generate output.txt"
)
# 添加一个 target
add_custom_target(hello echo "hello")
```


