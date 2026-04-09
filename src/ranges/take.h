#pragma once

#include "base.h"
#include "concepts.h"
#include <utility>

namespace jal::ranges {

namespace internal {

template <range Range> class take_view {
public:
  take_view(Range range, std::size_t n)
      : range_{range}
      , n_{n} {}

  struct iterator {
    using it_type = decltype(std::declval<Range &>().begin());

    it_type cur_;
    std::size_t remaining_;

    auto operator*() const { return *cur_; }

    iterator &operator++() {
      if (remaining_ > 0) {
        ++cur_;
        --remaining_;
      }
      return *this;
    }

    bool operator!=(const iterator &other) const {
      return cur_ != other.cur_ && remaining_ > 0;
    }
  };

  auto begin() { return iterator{range_.begin(), n_}; }

  auto end() { return iterator{range_.end(), 0}; }

private:
  Range range_;
  std::size_t n_;
};

struct take_adaptor : internal::view_interface<take_adaptor> {
  std::size_t n;

  take_adaptor(std::size_t n_)
      : n(n_) {}

  auto apply(range auto &&range) {
    return take_view<std::decay_t<decltype(range)>>(
      std::forward<decltype(range)>(range), n);
  }
};

} // namespace internal

auto take(std::integral auto n) {
  return internal::take_adaptor{static_cast<std::size_t>(n)};
}

} // namespace jal::ranges
