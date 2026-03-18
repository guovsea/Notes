
## 包的分类 

### Config-file Packages

A config-file package is a set of files provided by upstreams for downstreams to us

### Find-module Packages

Typically, a find module is needed when the upstream is **not built with CMake**, or is **not CMake-aware** enough to otherwise provide a package configuration file

## 制作 Cnfig-file Package

> 可参考 CMake-Cookbook 10.3

1. 将 target 的相关文件安装到 `CMAKE_INSTALL_PREFXI` / `DESTINATION` 下

```cmake
set(EXPORT_NAME ScriptEditor)

install(TARGETS 
    ScriptEditor
    GUI
    ####
    EXPORT ${EXPORT_NAME}Targets
    ####
    LIBRARY DESTINATION lib/$<CONFIG>
    ARCHIVE DESTINATION lib/$<CONFIG>
    RUNTIME DESTINATION bin/$<CONFIG>
    INCLUDES DESTINATION include
)
```


 同时，将 target 及其所有依赖添加到导出集合

`EXPORT <export-name>` 导出集合最好命名为惯用的 `${EXPORT_NAME}Targets`

```cmake
set(EXPORT_NAME ScriptEditor)

install(TARGETS 
    ScriptEditor
    GUI
    EXPORT ${EXPORT_NAME}Targets
)
```

如果要将某个 target 添加到某个导出集合，那么该 target 依赖的所有 target 都要被添加到导出集合中

2. 生成导出文件(用于定位各种 lib、头文件的位置）并安装到指定 `DESTINATION`

`install(EXPORT <export-name>` 中的 `export-name` （导出集）必须已经在 `install Targets` 中声明

```cmake
install(EXPORT ${EXPORT_NAME}Targets
    FILE ${EXPORT_NAME}Targets.cmake 
    NAMESPACE GMPT::
    DESTINATION lib/cmake/${EXPORT_NAME}
)
```

3. 以 Config.cmake.in 作为一个模板生成 `<包名>Config.cmake` 文件

```cmake
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/${EXPORT_NAME}Config.cmake"
    @ONLY
)

install(FILES 
    "${CMAKE_CURRENT_BINARY_DIR}/${EXPORT_NAME}Config.cmake"
    DESTINATION lib/cmake
)
EXPORT_NAME
```

可以在配置文件中添加上游包的更多信息

```cmake
# 模版文件 Config.cmake.in
@PACKAGE_INIT@

# include(CMakeFindDependencyMacro)

include("${CMAKE_CURRENT_LIST_DIR}/@EXPORT_NAME@Targets.cmake")

# 添加更多信息
set(@EXPORT_NAME@_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../include")
set(@EXPORT_NAME@_LIBRARY_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../lib")
set(@EXPORT_NAME@_RUNTIME_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../bin")
```

拷贝dll

```cmake
add_custom_command(TARGET ${PROJECT_NAME}
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${ScriptEditor_RUNTIME_DIR}/$<CONFIG>
    $<TARGET_FILE_DIR:${PROJECT_NAME}>
)
```