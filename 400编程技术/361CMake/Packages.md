
## Config-file Packages

### Packages Layout

```
<prefix>/include/foo-1.2/foo.h
<prefix>/lib/foo-1.2/libfoo.a
<prefix>/lib/cmake/foo-1.2/FooConfig.cmake
```

## Find-module Packages

Typically, a find module is needed when the upstream is **not built with CMake**, or is **not CMake-aware** enough to otherwise provide a package configuration file


```CMake
# install 所有子项目中的头文件并保持文件结构
function(exportHeaders LIB_NAME)
    file(GLOB_RECURSE HEADER_FILES
        RELATIVE ${SOURCES_PATH}
        ${SOURCES_PATH}/${LIB_NAME}/*.h
    )
    foreach(HEADER_FILE {HEADER_FILES})
        get_filename_component(HEADER_PATH ${HEADER_FILE} PATH)
        install(FILES ${SOURCES_PATH}/{HEADER_PAHT}
            DESTINATION include/${HEADER_PATH}
        )
    endforeach()
endfunction()
```