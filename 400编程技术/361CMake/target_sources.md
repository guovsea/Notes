```CMake
target_sources(${LIB_NAME}
    PRIVATE ScriptEditor.cpp 
    PUBLIC FILE_SET HEADERS 
    BASE_DIRS ${PROJECT_SOURCE_DIR}
    FILES ScriptEditor.h)

```