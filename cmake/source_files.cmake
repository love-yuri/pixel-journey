# 收集模块文件
file(GLOB IXX_MODULES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.ixx
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/*.ixx
    ${CMAKE_CURRENT_SOURCE_DIR}/src/glfw/*.ixx
    ${CMAKE_CURRENT_SOURCE_DIR}/src/vulkan/*.ixx
    ${CMAKE_CURRENT_SOURCE_DIR}/src/vulkan/detail/*.ixx
    ${CMAKE_CURRENT_SOURCE_DIR}/src/vulkan/api/*.ixx
    ${CMAKE_CURRENT_SOURCE_DIR}/src/skia/*.ixx
    ${CMAKE_CURRENT_SOURCE_DIR}/src/skia/api/*.ixx
    ${CMAKE_CURRENT_SOURCE_DIR}/src/configuration/*.ixx
)

# 收集源文件
file(GLOB_RECURSE PROJECT_SOURCES
    CONFIGURE_DEPENDS
    ${IXX_MODULES}
)