# ============================================================================
# 第三方依赖配置
# ============================================================================

# ----------------------------------------------------------------------------
# GLFW (窗口管理)
# ----------------------------------------------------------------------------
find_package(glfw3 REQUIRED)
list(APPEND THIRD_PARTY_LIBS glfw)
message(STATUS "GLFW version: ${glfw3_VERSION}")

# ----------------------------------------------------------------------------
# skia
# ----------------------------------------------------------------------------
set(SKIA_ROOT /home/yuri/github/skia)
set(SKIA_OUT  ${SKIA_ROOT}/out/Shared)
add_library(skia STATIC IMPORTED GLOBAL)
set_target_properties(skia PROPERTIES
    IMPORTED_LOCATION             ${SKIA_OUT}/libskia.a
    INTERFACE_INCLUDE_DIRECTORIES ${SKIA_ROOT}
)
list(APPEND THIRD_PARTY_LIBS skia)

# ----------------------------------------------------------------------------
# freeType
# ----------------------------------------------------------------------------
find_package(Freetype REQUIRED)
list(APPEND THIRD_PARTY_LIBS Freetype::Freetype)
message(STATUS "FreeType version: ${Freetype_VERSION}")

# ----------------------------------------------------------------------------
# vulkan
# ----------------------------------------------------------------------------
find_package(Vulkan REQUIRED)
list(APPEND THIRD_PARTY_LIBS Vulkan::Vulkan)
if(Vulkan_FOUND)
    message(STATUS "Vulkan SDK Version: ${Vulkan_VERSION}")
endif()

# ----------------------------------------------------------------------------
# OpenGL (可选)
# ----------------------------------------------------------------------------
#find_package(OpenGL REQUIRED)
#if(OpenGL_FOUND)
#    list(APPEND THIRD_PARTY_LIBS OpenGL::GL)
#    message(STATUS "OpenGL version: ${OpenGL_VERSION}")
#
#    # 获取实际OpenGL版本需要运行时查询
#    add_executable(opengl_version_check dummy.cpp)
#    target_link_libraries(opengl_version_check OpenGL::GL)
#    # 这里需要实际创建一个窗口来查询OpenGL版本
#endif()

# ----------------------------------------------------------------------------
# 导出变量
# ----------------------------------------------------------------------------
set(THIRD_PARTY_LIBS ${THIRD_PARTY_LIBS} PARENT_SCOPE)