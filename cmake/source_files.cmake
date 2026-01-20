# 收集模块文件
file(GLOB IXX_MODULES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.ixx

    # 工具相关
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/*.ixx

    # glfw
    ${CMAKE_CURRENT_SOURCE_DIR}/src/glfw/*.ixx

    # vulkan
    ${CMAKE_CURRENT_SOURCE_DIR}/src/vulkan/*.ixx
    ${CMAKE_CURRENT_SOURCE_DIR}/src/vulkan/detail/*.ixx
    ${CMAKE_CURRENT_SOURCE_DIR}/src/vulkan/api/*.ixx

    # skia
    ${CMAKE_CURRENT_SOURCE_DIR}/src/skia/*.ixx
    ${CMAKE_CURRENT_SOURCE_DIR}/src/skia/api/*.ixx
    ${CMAKE_CURRENT_SOURCE_DIR}/src/skia/resource/*.ixx

    # ui
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/*.ixx
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/render/*.ixx
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/render/*.cppm
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/core/*.ixx
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/animation/*.cppm
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/widgets/*.ixx
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/widgets/core/*.ixx
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/widgets/core/*.cppm
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/widgets/components/*.cppm
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/layout/*.ixx
    ${CMAKE_CURRENT_SOURCE_DIR}/src/configuration/*.ixx
)

# 收集源文件
file(GLOB_RECURSE PROJECT_SOURCES
    CONFIGURE_DEPENDS
    ${IXX_MODULES}
)