#pragma once

#include "base.h"
#include "concepts.h"
#include <utility>

namespace jal::ranges {

namespace internal {

template <container Container>
struct to_adaptor : internal::view_interface<to_adaptor<Container>> {
  auto apply(range auto &&range) {
    Container result;
    for (auto &&elem : range) {
      if constexpr (pushable_container<Container>) {
        result.push_back(std::forward<decltype(elem)>(elem));
      } else {
        result.insert(std::forward<decltype(elem)>(elem));
      }
    }
    return result;
  }
};

} // namespace internal

template <container Container> auto to() {
  return internal::to_adaptor<Container>{};
}

} // namespace jal::ranges
