#pragma once

#include <utility>

namespace jal::ranges::internal {

template <typename Derived> struct view_interface {
  template <typename Range>
  friend auto operator|(Range &&range, Derived adaptor) {
    return adaptor.apply(std::forward<Range>(range));
  }
};

} // namespace jal::ranges::internal
