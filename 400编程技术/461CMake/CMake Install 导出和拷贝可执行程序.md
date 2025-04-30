## 导出

```CMake
cmake_minimum_required(VERSION 3.21)
project(GMPT_CS_Server VERSION 1.0.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


add_executable(${PROJECT_NAME}
    main.cpp
)
set(PACKAGE_NAME ${PROJECT_NAME})

set(EXPORT_TARGETS ${PROJECT_NAME})

install(
    TARGETS
        ${EXPORT_TARGETS}
    EXPORT ${PACKAGE_NAME}Targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)


configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}Config.cmake" 
    @ONLY
)

install(EXPORT ${PACKAGE_NAME}Targets
    FILE ${PACKAGE_NAME}Targets.cmake
    DESTINATION cmake
)

install(FILES 
    "${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}Config.cmake" 
    DESTINATION cmake
)
```

## 导入

```cmake
cmake_minimum_required(VERSION 3.21)
project(test_install VERSION 1.0.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


add_executable(${PROJECT_NAME}
    main.cpp
)

set(CMAKE_PREFIX_PATH "C:/Dashboard/近期事务/GMPT CS Server/install/cmake")

find_package(GMPT_CS_Server REQUIRED)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        $<TARGET_FILE:GMPT_CS_Server>  # 来源可执行文件
        $<TARGET_FILE_DIR:${PROJECT_NAME}>             # 拷贝到当前项目的可执行文件目录
    COMMENT "Copying gmpt_cs_server executable into test_install output folder"
)

```


## ./cmake/Config.cmake.in

```
@PACKAGE_INIT@

include(CMakeFindDependencyMacro)

include("${CMAKE_CURRENT_LIST_DIR}/@PACKAGE_NAME@Targets.cmake")

set(@PACKAGE_NAME@_RUNTIME_DIR ${CMAKE_CURRENT_LIST_DIR}/../bin)
```