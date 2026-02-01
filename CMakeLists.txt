cmake_minimum_required(VERSION 4.0)

include(cmake/toolchain.cmake)   # 包含工具链配置
include(cmake/cxx_modules.cmake) # 包含 C++ 模块配置

project(yuri VERSION 1.0 LANGUAGES CXX)

include(cmake/source_files.cmake) # 包含源文件配置
include(cmake/dependencies.cmake) # 包含第三方依赖

# 生成lib
add_library(yuri_lib STATIC ${PROJECT_SOURCES})
target_include_directories(yuri_lib PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(yuri_lib ${THIRD_PARTY_LIBS})

# 构建测试
option(BUILD_TESTING "Build the testing tree." ON)
if (BUILD_TESTING)
  enable_testing()
  add_subdirectory(test)
endif ()

# yuri-music项目
add_subdirectory(test/yuri-music)

# 配置模块支持
target_sources(yuri_lib
    PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES FILES
    ${IXX_MODULES}
)
set_property(TARGET yuri_lib PROPERTY CXX_SCAN_FOR_MODULES ON)

# 包含编译器选项
include(cmake/compiler_options.cmake)