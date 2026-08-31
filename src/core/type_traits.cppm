//
// Created by love-yuri on 2026/3/25.
//

// utils/type_traits.cppm
export module yuri.core:type_traits;

export namespace meta {

// 成员函数指针萃取器
template<typename T>
struct member_class;

template<typename C, typename M>
struct member_class<M C::*> {
  using type = C;
};

// 辅助别名模板
template<typename T>
using member_class_t = member_class<T>::type;

}
