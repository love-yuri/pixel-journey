# MSVC 特定编译选项
if(MSVC)
  target_compile_options(yuri_lib
      PUBLIC
      /Zc:preprocessor  # 启用符合标准的预处理器
      /utf-8            # 源文件和执行字符集使用 UTF-8
  )

  # 关闭警告: 模块未使用 UTF-8 编码
  # UTF8 is defined in current command line and not in module command line
  target_compile_options(yuri_lib
      PUBLIC
      /wd5050
  )
endif()
