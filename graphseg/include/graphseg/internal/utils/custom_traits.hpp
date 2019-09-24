#ifndef GRAPHSEG_CPP_GRAPHSEG_INTERNAL_UTIL_CUSTOM_TRAITS_HPP
#define GRAPHSEG_CPP_GRAPHSEG_INTERNAL_UTIL_CUSTOM_TRAITS_HPP

#include <type_traits>

namespace GraphSeg::internal::utils
{
  template <typename T, typename = void>
  struct is_iterable : std::false_type
  {};

  template <typename T>
  struct is_iterable<T, std::void_t<decltype(std::declval<T>().begin()), decltype(std::declval<T>().end())>> : std::true_type
  {};

  template <typename T, bool = is_iterable<typename T::reference>::value>
  struct is_valid_iterable : std::false_type
  {};

  template <typename T>
  struct is_valid_iterable<T, true> : std::true_type
  {};
}

#endif