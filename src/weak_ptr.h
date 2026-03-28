#pragma once

#include "shared_ptr.h"
#include <utility>

namespace jal {

template <typename T> class weak_ptr {
public:
  weak_ptr() = default;

  weak_ptr(const shared_ptr<T> &other)
      : ptr_{other.ptr_}
      , control_{other.control_} {
    if (control_ != nullptr) {
      control_->add_weak_ref();
    }
  }

  ~weak_ptr() { release(); }

  weak_ptr(const weak_ptr &other)
      : ptr_{other.ptr_}
      , control_{other.control_} {
    if (control_ != nullptr) {
      control_->add_weak_ref();
    }
  }

  weak_ptr &operator=(const weak_ptr &other) {
    if (this != &other) {
      release();
      ptr_ = other.ptr_;
      control_ = other.control_;
      if (control_ != nullptr) {
        control_->add_weak_ref();
      }
    }
    return *this;
  }

  weak_ptr &operator=(const shared_ptr<T> &other) {
    release();
    ptr_ = other.ptr_;
    control_ = other.control_;
    if (control_ != nullptr) {
      control_->add_weak_ref();
    }
    return *this;
  }

  weak_ptr(weak_ptr &&other) noexcept
      : ptr_{std::exchange(other.ptr_, nullptr)}
      , control_{std::exchange(other.control_, nullptr)} {}

  weak_ptr &operator=(weak_ptr &&other) noexcept {
    if (this != &other) {
      release();
      ptr_ = std::exchange(other.ptr_, nullptr);
      control_ = std::exchange(other.control_, nullptr);
    }
    return *this;
  }

  bool expired() const noexcept {
    return control_ == nullptr || control_->get_ref_count() == 0;
  }

  shared_ptr<T> lock() const noexcept {
    if (control_ == nullptr) {
      return shared_ptr<T>();
    }

    int old_count = control_->get_ref_count();
    while (old_count != 0) {
      if (control_->used_cnt.compare_exchange_weak(old_count, old_count + 1,
                                                   std::memory_order_acquire,
                                                   std::memory_order_relaxed)) {
        return shared_ptr<T>(ptr_, control_);
      }
    }

    return shared_ptr<T>();
  }

  void reset() noexcept {
    release();
    ptr_ = nullptr;
    control_ = nullptr;
  }

private:
  void release() {
    if (control_ != nullptr && control_->del_weak_ref() == 1) {
      delete control_;
    }
  }

  T *ptr_ = nullptr;
  internal::ctrl_block *control_ = nullptr;
};

} // namespace jal
