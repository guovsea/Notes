---
tags:
  - tools/cmake
  - type/reference
mastery: 5
---

## add_custom_command

### OUTPUT signature

常用格式：

```cmake
add_custom_command(OUTPUT output1 [output2 ...]
                   COMMAND command1 [ARGS] [args1...]
                   [COMMAND command2 [ARGS] [args2...] ...]
                   [DEPENDS [depends...]]
                   [WORKING_DIRECTORY dir]
)
```

- `output1` 可以被其他 `target` 依赖，这个并不一定是文件名，并且和 COMMAND 最终会生成的文件无关。

```cmake
cmake_minimum_required(VERSION 3.16)

project(test)

find_package(PythonInterp REQUIRED)

# generate.py 会在执行命令的路径下生成 a.txt 文件
add_custom_command(OUTPUT output1
    COMMAND ${PYTHON_EXECUTABLE}  ${CMAKE_SOURCE_DIR}/generate.py
    # WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    DEPENDS ${CMAKE_SOURCE_DIR}/generate.py
)

add_custom_target(mytarget ALL
    DEPENDS output1
)
```

- 只有 `COMMAND` 的内容和 `WORKING_DIRECTORY` 才会决定生成的文件名


### TARGET signature

adds a custom command to a target such as a library or executable

```cmake
add_custom_command(TARGET <target>
                   PRE_BUILD | PRE_LINK | POST_BUILD
                   COMMAND command1 [ARGS] [args1...]
                   [COMMAND command2 [ARGS] [args2...] ...]
                   [BYPRODUCTS [files...]]
                   [WORKING_DIRECTORY dir]
)
```

`OUTPUT signature `中的 `cmake file` 等价于：

```cmake
cmake_minimum_required(VERSION 3.16)

project(test)

find_package(PythonInterp REQUIRED)

add_custom_target(mytarget ALL)

add_custom_command(TARGET mytarget
    COMMAND ${PYTHON_EXECUTABLE}  ${CMAKE_SOURCE_DIR}/generate.py 
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    DEPENDS ${CMAKE_SOURCE_DIR}/generate.py
)

```
## add_custom_target




