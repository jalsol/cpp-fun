#pragma once

#include <type_traits>
#include <utility>

namespace jal {

// substitution for std::forward_like because I can't compile that somehow
template <typename Owner, typename Member>
constexpr auto &&forward_like(Member &&member) noexcept {
  using OwnerDecayed = std::remove_reference_t<Owner>;
  using MemberDecayed = std::remove_reference_t<Member>;

  constexpr bool is_const = std::is_const_v<OwnerDecayed>;
  using ConstType =
    std::conditional_t<is_const, const MemberDecayed, MemberDecayed>;

  if constexpr (std::is_lvalue_reference_v<Owner>) {
    return static_cast<ConstType &>(std::forward<Member>(member));
  } else {
    return static_cast<ConstType &&>(std::forward<Member>(member));
  }
}
} // namespace jal
