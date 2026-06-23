if (WIN32)
  # 工具链配置
  find_program(_VCPKG_EXECUTABLE vcpkg)
  if (_VCPKG_EXECUTABLE)
    get_filename_component(VCPKG_ROOT "${_VCPKG_EXECUTABLE}" DIRECTORY)
    message(STATUS "找到 VCPKG_ROOT: ${VCPKG_ROOT}")
  else ()
    message(FATAL_ERROR "找不到 vcpkg，请设置环境变量 VCPKG_ROOT 或确保 vcpkg 在 PATH 中")
  endif ()

  # 设置 vcpkg 工具链文件
  set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "VCPKG 工具链文件路径")

  # windows默认静态链接
  set(CMAKE_MSVC_RUNTIME_LIBRARY MultiThreaded)

  message(STATUS "Windows 工具链已配置")
else ()
  set(CMAKE_CXX_COMPILER "clang++" CACHE STRING "C++ 编译器")
endif()