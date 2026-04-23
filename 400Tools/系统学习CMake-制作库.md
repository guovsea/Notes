---
tags:
  - tools/cmake
  - type/howto
mastery: 5
---
## shared


## Header-only

```cmake
add_library(geo INTERFACE)
target_sources(geo INTERFACE 
    FILE_SET HEADERS 
    BASE_DIRS ${PROJECT_SOURCE_DIR}
    FILES "geometry.h")
```

or

```cmake
add_library(<name> INTERFACE 头文件集合)
```