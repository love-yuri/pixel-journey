# 递归收集所有 C++ 模块单元；新增目录或模块文件时自动触发 CMake 重配置。
file(GLOB_RECURSE CPPM_MODULES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cppm
)

set(PROJECT_SOURCES ${CPPM_MODULES})
