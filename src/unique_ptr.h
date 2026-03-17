#pragma once

#include <utility>

namespace jal {

// NOTE: implement deleter later
template <typename T> class unique_ptr {
public:
  using pointer = T *;

  constexpr unique_ptr() noexcept = default;

  explicit constexpr unique_ptr(pointer p) noexcept
      : data_{p} {}

  constexpr ~unique_ptr() noexcept { delete data_; }

  constexpr unique_ptr(unique_ptr &&other) noexcept
      : data_{std::exchange(other.data_, nullptr)} {}

  constexpr unique_ptr &operator=(unique_ptr &&other) noexcept {
    if (this != &other) {
      delete data_;
      data_ = std::exchange(other.data_, nullptr);
    }
    return *this;
  }

  unique_ptr(const unique_ptr &other) = delete;

  unique_ptr &operator=(const unique_ptr &other) = delete;

  constexpr pointer get() const noexcept { return data_; }

  constexpr pointer release() noexcept { return std::exchange(data_, nullptr); }

  constexpr void reset(pointer ptr = pointer()) noexcept {
    auto old_ptr = get();
    data_ = ptr;
    delete old_ptr;
  }

  constexpr void swap(unique_ptr &other) noexcept {
    std::swap(data_, other.data_);
  }

  explicit constexpr operator bool() const noexcept { return data_ != nullptr; }

  constexpr pointer operator->() const noexcept { return get(); }

  constexpr T &operator*() const noexcept { return *get(); }

private:
  T *data_ = nullptr;
};

template <typename T> class unique_ptr<T[]> {
public:
  using pointer = T *;
  using element_type = T;

  constexpr unique_ptr() noexcept = default;

  explicit constexpr unique_ptr(pointer p) noexcept
      : data_{p} {}

  constexpr ~unique_ptr() noexcept { delete[] data_; }

  constexpr unique_ptr(unique_ptr &&other) noexcept
      : data_{std::exchange(other.data_, nullptr)} {}

  constexpr unique_ptr &operator=(unique_ptr &&other) noexcept {
    if (this != &other) {
      delete[] data_;
      data_ = std::exchange(other.data_, nullptr);
    }
    return *this;
  }

  unique_ptr(const unique_ptr &other) = delete;

  unique_ptr &operator=(const unique_ptr &other) = delete;

  constexpr pointer get() const noexcept { return data_; }

  constexpr pointer release() noexcept { return std::exchange(data_, nullptr); }

  constexpr void reset(pointer ptr = pointer()) noexcept {
    auto old_ptr = get();
    data_ = ptr;
    delete[] old_ptr;
  }

  constexpr void swap(unique_ptr &other) noexcept {
    std::swap(data_, other.data_);
  }

  explicit constexpr operator bool() const noexcept { return data_ != nullptr; }

  constexpr T &operator[](std::size_t i) const noexcept { return data_[i]; }

private:
  T *data_ = nullptr;
};

} // namespace jal
