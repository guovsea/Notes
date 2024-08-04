一般性语法：

```
<target> PRIVATE/PUBLIC/INTERFACE ...
```

尽量使用 target 命令
## target_sources

> 可以分布式、分多次添加源文件，可以在不同的 `CMakeLists.txt` 中链接同一目标


```cmake
# 在 add_subdirectory("shape")中
target_sources(app PRIVATE shape.cpp)
# 在 add_subdirectory("square") 中
target_sources(app PRIVATE square.cpp)

# 原本需要
add_executable(app shape/shape.cpp square/square.cpp)
```

精彩：

[Site Unreachable](https://crascit.com/2016/01/31/enhanced-source-file-handling-with-target_sources/)
### 进阶 FILE_SET (3.23)

 待学习

```cmake
target_sources(geo 
    PRIVATE shape.cpp 
    PUBLIC 
        FILE_SET HEADERS 
        BASE_DIRS ${PROJECT_SOURCE_DIR}
        FILES shape.h)
```

## target_include_directories


## target_link_libraries

