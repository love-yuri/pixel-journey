if (WIN32)
  # 工具链配置
  if (DEFINED ENV{VCPKG_ROOT})
    set(VCPKG_ROOT $ENV{VCPKG_ROOT} CACHE PATH "VCPKG 根目录")
    message(STATUS "VCPKG_ROOT: ${VCPKG_ROOT}")
  else()
    set(VCPKG_ROOT "E:/love-yuri/github/vcpkg" CACHE PATH "默认 VCPKG 路径")
    message(WARNING "VCPKG_ROOT 未设置，使用默认值: ${VCPKG_ROOT}")
  endif()

  # 设置 vcpkg 工具链文件
  set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "VCPKG 工具链文件路径")
  message("-- Windows 工具链已配置")
else ()
  set(CMAKE_CXX_COMPILER "clang++" CACHE STRING "C++ 编译器")
endif()