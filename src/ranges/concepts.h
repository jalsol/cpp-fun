#pragma once

#include <concepts>
#include <iterator>

namespace jal::ranges {

template <typename T>
concept has_begin_end = requires(T &t) {
  { t.begin() };
  { t.end() };
};

template <typename T>
concept range = has_begin_end<T> || requires(T &t) {
  { std::begin(t) };
  { std::end(t) };
};

template <typename C>
concept pushable_container =
  requires(C c, typename C::value_type val) { c.push_back(val); };

template <typename C>
concept insertable_container =
  requires(C c, typename C::value_type val) { c.insert(val); };

template <typename C>
concept container = pushable_container<C> || insertable_container<C>;

} // namespace jal::ranges
