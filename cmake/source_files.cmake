# 收集模块文件
file(GLOB IXX_MODULES
    # 工具配置相关
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/*.ixx
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/*.cppm
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/profiling/*.cppm
    ${CMAKE_CURRENT_SOURCE_DIR}/src/configuration/*.ixx

    # glfw
    ${CMAKE_CURRENT_SOURCE_DIR}/src/glfw/*.ixx
    ${CMAKE_CURRENT_SOURCE_DIR}/src/glfw/*.cppm

    # vulkan
    ${CMAKE_CURRENT_SOURCE_DIR}/src/vulkan/*.cppm
    ${CMAKE_CURRENT_SOURCE_DIR}/src/vulkan/detail/*.ixx
    ${CMAKE_CURRENT_SOURCE_DIR}/src/vulkan/api/*.ixx

    # skia
    ${CMAKE_CURRENT_SOURCE_DIR}/src/skia/*.ixx
    ${CMAKE_CURRENT_SOURCE_DIR}/src/skia/api/*.ixx
    ${CMAKE_CURRENT_SOURCE_DIR}/src/skia/resource/*.ixx

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
)

# 收集源文件
file(GLOB_RECURSE PROJECT_SOURCES
    CONFIGURE_DEPENDS
    ${IXX_MODULES}
)