# 最低 CMake 版本要求（支持 C++11 及 Threads 模块）
cmake_minimum_required(VERSION 3.10)

# 项目名称 + 语言（C++）
project(MVPLoginDemo LANGUAGES CXX)

# ========== 全局配置 ==========
# 强制启用 C++11 标准（项目核心依赖）
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)  # 禁用编译器扩展（跨平台兼容）

# 设置输出目录（可执行文件统一生成到 build/bin 下）
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# 查找线程库（跨平台处理 pthread 依赖）
find_package(Threads REQUIRED)

# ========== 主程序编译 ==========
# 收集主程序源文件（排除测试相关文件）
set(MAIN_SOURCES
    LoginPresenter.cpp
    LoginModelImpl.cpp
    ConsoleLoginView.cpp
    main.cpp
)

# 收集主程序头文件（用于 IDE 索引，非必需但推荐）
set(MAIN_HEADERS
    MVPInterfaces.h
    LoginPresenter.h
    LoginModelImpl.h
    ConsoleLoginView.h
)

# 创建主程序可执行文件
add_executable(MVPLoginDemo 
    ${MAIN_SOURCES}
    ${MAIN_HEADERS}  # IDE 友好：让头文件显示在项目结构中
)

# 链接线程库（Linux/macOS 需 pthread，Windows 自动忽略）
target_link_libraries(MVPLoginDemo PRIVATE Threads::Threads)

# 包含头文件目录（当前目录，确保 #include 能找到头文件）
target_include_directories(MVPLoginDemo PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# ========== 测试程序编译 ==========
# 收集测试程序源文件
set(TEST_SOURCES
    LoginPresenter.cpp       # 复用 Presenter 实现
    LoginModelImpl.cpp
    LoginPresenterTest.cpp   # 测试用例
)

# 收集测试程序头文件
set(TEST_HEADERS
    MVPInterfaces.h
    LoginPresenter.h
    MockLoginView.h
    MockLoginModel.h
)

# 创建测试程序可执行文件
add_executable(MVPLoginTest
    ${TEST_SOURCES}
    ${TEST_HEADERS}
)

# 链接线程库（测试程序也用到 mutex/thread）
target_link_libraries(MVPLoginTest PRIVATE Threads::Threads)

# 包含头文件目录
target_include_directories(MVPLoginTest PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# ========== 可选：编译选项优化 ==========
# 调试模式下添加 -g，发布模式下添加优化
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-g -Wall -Wextra)  # 开启警告，方便调试
else()
    add_compile_options(-O2)  # 发布模式优化
endif()