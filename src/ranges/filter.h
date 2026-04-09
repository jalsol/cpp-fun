#pragma once

#include "base.h"
#include "concepts.h"

#include <utility>

namespace jal::ranges {

namespace internal {

template <range Range, typename Pred>
  requires std::predicate<Pred, decltype(*std::declval<Range &>().begin())>
class filter_view {
public:
  filter_view(Range range, Pred pred)
      : range_{range}
      , pred_{pred} {}

  struct iterator {
    using it_type = decltype(std::declval<Range &>().begin());

    it_type cur_, end_;
    Pred *pred_;

    void skip() {
      while (cur_ != end_ && !(*pred_)(*cur_)) {
        ++cur_;
      }
    }

    iterator(it_type cur, it_type end, Pred *pred)
        : cur_{cur}
        , end_{end}
        , pred_{pred} {
      skip();
    }

    auto operator*() const { return *cur_; }

    iterator &operator++() {
      ++cur_;
      skip();
      return *this;
    }

    bool operator!=(const iterator &o) const { return cur_ != o.cur_; }
  };

  auto begin() { return iterator{range_.begin(), range_.end(), &pred_}; }
  auto end() { return iterator{range_.end(), range_.end(), &pred_}; }

private:
  Range range_;
  Pred pred_;
};

template <typename Pred>
struct filter_adaptor : internal::view_interface<filter_adaptor<Pred>> {
  Pred pred;

  filter_adaptor(Pred p)
      : pred(p) {}

  auto apply(range auto &&range) {
    return filter_view<std::decay_t<decltype(range)>, Pred>(
      std::forward<decltype(range)>(range), pred);
  }
};

} // namespace internal

auto filter(auto pred) { return internal::filter_adaptor{pred}; }

} // namespace jal::ranges
