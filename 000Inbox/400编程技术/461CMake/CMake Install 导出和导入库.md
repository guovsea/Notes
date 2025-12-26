```CMake
set(EXPORT_NAME ${PROJECT_NAME})
set(EXPORT_TARGETS 
    Utility
    GMPT_CS_Server
)

install(
    TARGETS
        ${EXPORT_TARGETS}
    EXPORT ${EXPORT_NAME}Targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    PUBLIC_HEADER DESTINATION include/${PROJECT_NAME}
)

install(EXPORT ${PACKAGE_NAME}Targets
    FILE ${PACKAGE_NAME}Targets.cmake
    NAMESPACE my::
    DESTINATION cmake
)

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/${EXPORT_NAME}Config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/${EXPORT_NAME}Config.cmake" 
    @ONLY
)
```

```
// Config.cmake.in
@PACKAGE_INIT@

include("${CMAKE_CURRENT_LIST_DIR}/@EXPORT_NAME@Targets.cmake")

set(@EXPORT_NAME@_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/../../include)
set(@EXPORT_NAME@_LIBRARY_DIR ${CMAKE_CURRENT_LIST_DIR}/../../lib)
set(@EXPORT_NAME@_RUNTIME_DIR ${CMAKE_CURRENT_LIST_DIR}/../../bin)
```


## find_package

```CMake
find_package(A REQUIRED)

target_link_libraries(
    ${LIB_NAME} my::
)
```