#pragma once

#include <array>
#include <atomic>
#include <utility>

// TODO: custom deleter

namespace jal {

namespace internal {

struct ctrl_block {
  ctrl_block() = default;
  virtual ~ctrl_block() = default;
  ctrl_block(const ctrl_block &) = delete;
  ctrl_block(ctrl_block &&) = delete;
  ctrl_block &operator=(const ctrl_block &) = delete;
  ctrl_block &operator=(ctrl_block &&) = delete;

  auto add_ref() { return used_cnt.fetch_add(1, std::memory_order_relaxed); }
  auto del_ref() { return used_cnt.fetch_sub(1, std::memory_order_acq_rel); }
  auto get_ref_count() const {
    return used_cnt.load(std::memory_order_relaxed);
  }
  auto add_weak_ref() {
    return weak_cnt.fetch_add(1, std::memory_order_relaxed);
  }
  auto del_weak_ref() {
    return weak_cnt.fetch_sub(1, std::memory_order_acq_rel);
  }

  std::atomic_int used_cnt{1};
  std::atomic_int weak_cnt{1};

  virtual void destroy() = 0;
};

template <typename T> struct ctrl_block_ptr : ctrl_block {
  ctrl_block_ptr(T *p)
      : ptr{p} {}
  T *ptr;

  void destroy() override { delete ptr; }
};

template <typename T> struct ctrl_block_inplace : ctrl_block {
  alignas(T) std::array<std::byte, sizeof(T)> buffer_;

  template <typename... Args> void construct(Args &&...args) {
    new (buffer_.data()) T(std::forward<Args>(args)...);
  }

  T *get() { return reinterpret_cast<T *>(buffer_.data()); }

  void destroy() override {
    std::destroy_at(reinterpret_cast<T *>(buffer_.data()));
  }
};

} // namespace internal

template <typename T> class shared_ptr {
public:
  shared_ptr() = default;

  shared_ptr(T *ptr)
      : ptr_{ptr}
      , control_{new internal::ctrl_block_ptr<T>{ptr}} {}

  ~shared_ptr() { release(); }

  shared_ptr(const shared_ptr &other)
      : ptr_{other.ptr_}
      , control_{other.control_} {
    if (control_ != nullptr) {
      control_->add_ref();
    }
  }

  shared_ptr &operator=(const shared_ptr &other) {
    if (this != &other) {
      release();
      ptr_ = other.ptr_;
      control_ = other.control_;
      if (control_ != nullptr) {
        control_->add_ref();
      }
    }
    return *this;
  }

  shared_ptr(shared_ptr &&other) noexcept
      : ptr_{std::exchange(other.ptr_, nullptr)}
      , control_{std::exchange(other.control_, nullptr)} {}

  shared_ptr &operator=(shared_ptr &&other) noexcept {
    if (this != &other) {
      release();
      ptr_ = std::exchange(other.ptr_, nullptr);
      control_ = std::exchange(other.control_, nullptr);
    }
    return *this;
  }

  T *operator->() noexcept { return ptr_; }
  T &operator*() noexcept { return *ptr_; }

private:
  shared_ptr(T *ptr, internal::ctrl_block *control)
      : ptr_{ptr}
      , control_{control} {}

  void release() {
    if (control_ != nullptr && control_->del_ref() == 1) {
      control_->destroy();
      if (control_->del_weak_ref() == 1) {
        delete control_;
      }
    }
  }

  T *ptr_ = nullptr;
  internal::ctrl_block *control_ = nullptr;

  template <typename U, typename... Args>
  friend shared_ptr<U> make_shared(Args &&...args);

  template <typename U> friend class weak_ptr;
};

template <typename T, typename... Args>
shared_ptr<T> make_shared(Args &&...args) {
  auto *control = new internal::ctrl_block_inplace<T>();
  control->construct(std::forward<Args>(args)...);
  return shared_ptr<T>(control->get(), control);
}

} // namespace jal
