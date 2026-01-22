#pragma once

#include "utility.h"

#include <initializer_list>
#include <memory>
#include <utility>

namespace jal {

template <typename Type, typename Allocator = std::allocator<Type>>
class vector {
public:
  constexpr vector() noexcept(noexcept(Allocator())) = default;

  constexpr explicit vector(const Allocator &alloc)
      : alloc_{alloc} {}

  constexpr explicit vector(std::size_t count,
                            const Allocator &alloc = Allocator())
      : alloc_{alloc}
      , capacity_{count}
      , size_{count} {
    if (capacity_ > 0) {
      data_ = alloc_.allocate(capacity_);

      try {
        std::uninitialized_default_construct_n(data_, count);
      } catch (...) {
        alloc_.deallocate(data_, capacity_);
        throw;
      }
    }
  }

  vector(std::size_t count, const Type &value,
         const Allocator &alloc = Allocator())
      : alloc_{alloc}
      , capacity_{count}
      , size_{count} {
    if (capacity_ > 0) {
      data_ = alloc_.allocate(capacity_);

      try {
        std::uninitialized_fill_n(data_, count, value);
      } catch (...) {
        alloc_.deallocate(data_, capacity_);
        throw;
      }
    }
  }

  constexpr vector(std::initializer_list<Type> init,
                   const Allocator &alloc = Allocator())
      : alloc_{alloc}
      , capacity_{init.size()}
      , size_{init.size()} {
    if (capacity_ > 0) {
      data_ = alloc_.allocate(capacity_);

      try {
        std::uninitialized_copy(init.begin(), init.end(), data_);
      } catch (...) {
        alloc_.deallocate(data_, capacity_);
        throw;
      }
    }
  }

  constexpr ~vector() noexcept {
    clear();
    if (data_) {
      alloc_.deallocate(data_, capacity_);
    }
  }

  constexpr vector(const vector &other)
      : capacity_{other.capacity_}
      , size_{other.size_} {
    if (capacity_ > 0) {
      data_ = alloc_.allocate(capacity_);

      try {
        std::uninitialized_copy_n(other.data_, other.size_, data_);
      } catch (...) {
        alloc_.deallocate(data_, capacity_);
        throw;
      }
    }
  }

  constexpr vector(vector &&other) noexcept
      : alloc_{std::move(other.alloc_)}
      , data_{std::exchange(other.data_, nullptr)}
      , capacity_{std::exchange(other.capacity_, 0)}
      , size_{std::exchange(other.size_, 0)} {}

  constexpr vector &operator=(const vector &other) {
    if (this != &other) {
      if (other.data_) {
        alloc_.deallocate(data_, capacity_);
        data_ = alloc_.allocate(other.capacity_);
        std::uninitialized_copy_n(other.data_, other.size_, data_);
      }
      capacity_ = other.capacity_;
      size_ = other.size_;
    }
    return *this;
  }

  constexpr vector &operator=(vector &&other) noexcept {
    if (this != &other) {
      alloc_.deallocate(data_, capacity_);
      data_ = std::exchange(other.data_, nullptr);
      capacity_ = std::exchange(other.capacity_, 0);
      size_ = std::exchange(other.size_, 0);
    }
    return *this;
  }

  constexpr void push_back(const Type &value) {
    if (size_ == capacity_) {
      reserve(capacity_ == 0 ? 1 : 2 * capacity_);
    }
    std::construct_at(data_ + size_++, value);
  }

  constexpr void push_back(Type &&value) {
    if (size_ == capacity_) {
      reserve(capacity_ == 0 ? 1 : 2 * capacity_);
    }
    std::construct_at(data_ + size_++, std::move(value));
  }

  template <class... Args> constexpr Type &emplace_back(Args &&...args) {
    if (size_ == capacity_) {
      reserve(capacity_ == 0 ? 1 : 2 * capacity_);
    }
    std::construct_at(data_ + size_, std::forward<Args>(args)...);
    return data_[size_++];
  }

  constexpr void pop_back() { std::destroy_at(data_ + --size_); }

  [[nodiscard]] constexpr bool empty() const noexcept { return size_ == 0; }
  [[nodiscard]] constexpr std::size_t size() const noexcept { return size_; }
  [[nodiscard]] constexpr std::size_t capacity() const noexcept {
    return capacity_;
  }

  constexpr Type *data() noexcept { return data_; }
  constexpr const Type *data() const noexcept { return data_; }

  constexpr Type *begin() noexcept { return data_; }
  constexpr const Type *begin() const noexcept { return data_; }

  constexpr Type *end() noexcept { return data_ + size_; }
  constexpr const Type *end() const noexcept { return data_ + size_; }

  template <typename Self> constexpr auto &&front(this Self &&self) noexcept {
    return jal::forward_like<Self>(std::forward<Self>(self).data_[0]);
  }

  template <typename Self> constexpr auto &&back(this Self &&self) noexcept {
    return jal::forward_like<Self>(
      std::forward<Self>(self).data_[self.size_ - 1]);
  }

  template <typename Self>
  constexpr auto &&operator[](this Self &&self, std::size_t index) {
    return jal::forward_like<Self>(std::forward<Self>(self).data_[index]);
  }

  constexpr void reserve(std::size_t new_capacity) {
    if (new_capacity > capacity_) {
      reallocate(new_capacity);
    }
  }

  constexpr void shrink_to_fit() {
    if (capacity_ > size_) {
      reallocate(size_);
    }
  }

  constexpr void clear() {
    std::destroy_n(data_, size_);
    size_ = 0;
  }

  constexpr void swap(vector &other) noexcept {
    if (std::allocator_traits<Allocator>::propagate_on_container_swap::value) {
      std::swap(alloc_, other.alloc_);
    }
    std::swap(data_, other.data_);
    std::swap(capacity_, other.capacity_);
    std::swap(size_, other.size_);
  }

private:
  constexpr void reallocate(std::size_t new_capacity) {
    auto *new_data = alloc_.allocate(new_capacity);
    try {
      std::uninitialized_move_n(data_, size_, new_data);
    } catch (...) {
      alloc_.deallocate(new_data, new_capacity);
      throw;
    }
    std::destroy_n(data_, size_);
    alloc_.deallocate(std::exchange(data_, new_data),
                      std::exchange(capacity_, new_capacity));
  }

  [[no_unique_address]] Allocator alloc_;
  Type *data_ = nullptr;
  std::size_t capacity_ = 0;
  std::size_t size_ = 0;
};

} // namespace jal
