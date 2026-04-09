#pragma once

#include "base.h"
#include "concepts.h"

#include <utility>

namespace jal::ranges {

namespace internal {

template <range Range, typename Fn>
  requires std::invocable<Fn, decltype(*std::declval<Range &>().begin())>
class transform_view {
public:
  transform_view(Range range, Fn fn)
      : range_{range}
      , fn_{fn} {}

  struct iterator {
    using it_type = decltype(std::declval<Range &>().begin());

    it_type cur_;
    Fn *fn_;

    auto operator*() const { return (*fn_)(*cur_); }
    iterator &operator++() {
      ++cur_;
      return *this;
    }
    bool operator!=(const iterator &other) const { return cur_ != other.cur_; }
  };

  auto begin() { return iterator{range_.begin(), &fn_}; }
  auto end() { return iterator{range_.end(), &fn_}; }

private:
  Range range_;
  Fn fn_;
};

template <typename Fn>
struct transform_adaptor : internal::view_interface<transform_adaptor<Fn>> {
  Fn fn;

  transform_adaptor(Fn f)
      : fn(f) {}

  auto apply(range auto &&range) {
    return transform_view<std::decay_t<decltype(range)>, Fn>(
      std::forward<decltype(range)>(range), fn);
  }
};

} // namespace internal

auto transform(auto fn) { return internal::transform_adaptor{fn}; }

} // namespace jal::ranges
