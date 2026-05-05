# 收集模块文件
file(GLOB CPPM_MODULES
    # 核心基础设施
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/*.cppm
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/profiling/*.cppm
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/utils/*.cppm
    ${CMAKE_CURRENT_SOURCE_DIR}/src/configuration/*.cppm

    # glfw
    ${CMAKE_CURRENT_SOURCE_DIR}/src/glfw/*.cppm

    # vulkan
    ${CMAKE_CURRENT_SOURCE_DIR}/src/vulkan/*.cppm
    ${CMAKE_CURRENT_SOURCE_DIR}/src/vulkan/detail/*.cppm
    ${CMAKE_CURRENT_SOURCE_DIR}/src/vulkan/api/*.cppm

    # skia
    ${CMAKE_CURRENT_SOURCE_DIR}/src/skia/*.cppm
    ${CMAKE_CURRENT_SOURCE_DIR}/src/skia/api/*.cppm
    ${CMAKE_CURRENT_SOURCE_DIR}/src/skia/resource/*.cppm

    # ui - core
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/*.cppm
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/animation/*.cppm

    # ui - render
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/render/*.cppm

    # ui - widgets
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/widgets/*.cppm
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/widgets/core/*.cppm
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/widgets/components/*.cppm

    # ui - layout
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/layout/*.cppm
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/layout/types/*.cppm
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/layout/core/*.cppm
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/layout/layouts/*.cppm

    # ui - algorithm
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/algorithm/*.cppm
)

# 收集源文件
file(GLOB_RECURSE PROJECT_SOURCES
    CONFIGURE_DEPENDS
    ${CPPM_MODULES}
)
