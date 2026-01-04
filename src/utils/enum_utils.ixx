module;
#include <magic_enum/magic_enum.hpp>
export module enum_utils;

export namespace enum_utils {

template <typename T>
auto name(T t) {
  return magic_enum::enum_name(t);
}

}