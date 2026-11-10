if (WIN32)
  find_program(_VCPKG_EXECUTABLE vcpkg REQUIRED)
  set(_VCPKG_EXECUTABLE_PATH "${_VCPKG_EXECUTABLE}")
  get_filename_component(_VCPKG_DIRECTORY "${_VCPKG_EXECUTABLE}" DIRECTORY)

  # Scoop 的 exe 是启动代理，真实程序路径保存在同名 .shim 文件中。
  if (EXISTS "${_VCPKG_DIRECTORY}/vcpkg.shim")
    file(STRINGS "${_VCPKG_DIRECTORY}/vcpkg.shim" _VCPKG_SHIM_PATH
        REGEX "^path = \"[^\"]+\"")
    if (_VCPKG_SHIM_PATH MATCHES "^path = \"([^\"]+)\"")
      file(TO_CMAKE_PATH "${CMAKE_MATCH_1}" _VCPKG_EXECUTABLE_PATH)
    else ()
      message(FATAL_ERROR "无法解析 ${_VCPKG_DIRECTORY}/vcpkg.shim")
    endif ()
  endif ()

  get_filename_component(_VCPKG_DIRECTORY "${_VCPKG_EXECUTABLE_PATH}" DIRECTORY)
  if (NOT EXISTS "${_VCPKG_DIRECTORY}/scripts/buildsystems/vcpkg.cmake")
    message(FATAL_ERROR "找不到 ${_VCPKG_DIRECTORY}/scripts/buildsystems/vcpkg.cmake")
  endif ()
  set(CMAKE_TOOLCHAIN_FILE "${_VCPKG_DIRECTORY}/scripts/buildsystems/vcpkg.cmake"
      CACHE FILEPATH "VCPKG 工具链文件路径")

  # windows默认静态链接
  set(CMAKE_MSVC_RUNTIME_LIBRARY MultiThreaded)

  message(STATUS "Windows 工具链已配置")
else ()
  set(CMAKE_CXX_COMPILER "clang++" CACHE STRING "C++ 编译器")
endif()
