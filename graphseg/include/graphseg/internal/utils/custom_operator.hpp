#ifndef GRAPHSEG_CPP_GRAPHSEG_INTERNAL_UTIL_CUSTOM_OPERATOR_HPP
#define GRAPHSEG_CPP_GRAPHSEG_INTERNAL_UTIL_CUSTOM_OPERATOR_HPP

#include "graphseg/internal/utils/custom_traits.hpp"

#include <ostream>
#include <set>
#include <type_traits>

namespace GraphSeg::internal::utils {
template <typename T, typename = std::enable_if_t<is_iterable<T>::value> *>
T operator&(const T &v1, const T &v2) {
  T result;
  set_intersection(v1.begin(), v1.end(), v2.begin(), v2.end(),
                   inserter(result, result.end()));
  return result;
}

template <typename T, typename = std::enable_if_t<is_iterable<T>::value> *>
T operator+(const T &v1, const T &v2) {
  T result;
  set_union(v1.begin(), v1.end(), v2.begin(), v2.end(),
            inserter(result, result.end()));
  return result;
}

template <typename T, typename = std::enable_if_t<is_iterable<T>::value> *>
T operator-(const T &v1, const T &v2) {
  std::set<T> result;
  set_difference(v1.begin(), v1.end(), v2.begin(), v2.end(),
                 inserter(result, result.end()));
  return result;
}

template <typename T,
          typename = std::enable_if_t<is_valid_iterable<T>::value> *>
std::ostream &operator<<(std::ostream &os, const T &segments) {
  for (const auto &segment : segments) {
    std::string vertex_node;
    for (const auto &segment_vertex : segment) {
      vertex_node += std::to_string(segment_vertex);
      vertex_node += " ";
    }
    os << vertex_node << std::endl;
  }
  return os;
}
} // namespace GraphSeg::internal::utils

#endif