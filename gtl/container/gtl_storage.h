/**
 * @file gtl_storage.h
 * @author gwq5210 (gwq5210@qq.com)
 * @brief 管理连续内存区域的类，用于实现Vector和Deque
 * @date 2021-05-16
 * 
 * @copyright Copyright (c) 2021. All rights reserved.
 */

#pragma once

#include <memory>

namespace gtl {

/**
 * @brief 管理连续内存区域的类，用于实现Vector和Deque
 * 
 * 例子：
 * size_t old_capacity = 16;
 * size_t new_capacity = 1024;
 * Storage old_storage(old_capacity);
 * Storage new_storage(new_capacity);
 * new_storage.unsafe_append_copy(new_capacity, old.begin(), old_storage.end(), old_storage.size());
 * new_storage.swap(old_storage); // or new_storage = std::move(old_storage);
 * 
 * 
 * @tparam T 存储的类型
 * @tparam Allocator 内存分配器，默认为std::allocator<T>
 */
template <typename T, typename Allocator = std::allocator<T>>
struct Storage : public Allocator {
  using SizeType = size_t;
  static constexpr SizeType min_capacity = 16;
  Storage() : data_(nullptr), capacity_(0), size_(0) {}
  Storage(SizeType n) : size_(0) {
    allocate(n);
    // printf("allocate %zu %zu %p\n", n, capacity(), begin());
  }
  Storage(const Storage& other) = delete;
  Storage& operator=(const Storage& other) = delete;
  Storage(Storage&& other) { assign(std::move(other)); }
  Storage& operator=(Storage&& other) { assign(std::move(other)); }
  ~Storage() { release(); }

  void assign(Storage&& other) {
    release();

    data_ = other.data_;
    capacity_ = other.capacity_;
    size_ = other.size_;

    other.init();
  }

  T* begin() { return data_; }
  T* begin() const { return data_; }
  const T* cbegin() const { return begin(); }
  T* end() { return data_ + size_; }
  T* end() const { return data_ + size_; }
  const T* cend() const { return end(); }
  T* data() { return begin(); }
  T* data() const { return begin(); }
  SizeType size() const { return size_; }
  SizeType capacity() const { return capacity_; }
  bool full(SizeType n) const { return size_ + n > capacity_; }
  SizeType next_capacity(SizeType new_size) {
    SizeType next_cap = 1;
    while (next_cap < new_size) {
      next_cap <<= 1;
    }
    next_cap = std::max(next_cap, min_capacity);
    // printf("next_cap %zu %zu\n", new_size, next_cap);
    return next_cap;
  }
  void allocate(SizeType n) {
    n = next_capacity(n);
    data_ = Allocator::allocate(n);
    capacity_ = n;
  }
  void unsafe_append_value_construct(SizeType count) {
    std::uninitialized_value_construct_n(end(), count);
    size_ += count;
  }
  void unsafe_append_fill(SizeType count, const T& v) {
    std::uninitialized_fill_n(end(), count, v);
    size_ += count;
  }
  template <typename II, typename Category = typename std::iterator_traits<II>::iterator_category>
  void unsafe_append_copy(II first, II last, SizeType count = 0) {
    std::uninitialized_copy(first, last, end());
    size_ += (count ? count : std::distance(first, last));
  }
  template <typename II, typename Category = typename std::iterator_traits<II>::iterator_category>
  void unsafe_append_move(II first, II last, SizeType count = 0) {
    std::uninitialized_move(first, last, end());
    size_ += (count ? count : std::distance(first, last));
  }
  template <typename... Args>
  void unsafe_append(Args&&... args) {
    construct_at(end(), std::forward<Args>(args)...);
    ++size_;
  }
  T* erase(const T* first, const T* last) {
    if (first != last) {
      SizeType count = std::distance(first, last);
      std::move(last, cend(), const_cast<T*>(first));
      std::destroy(end() - count, end());
      size_ -= count;
    }
    return const_cast<T*>(last);
  }
  void release() {
    if (begin()) {
      // printf("deallocate %zu %p\n", capacity_, data_);
      std::destroy(begin(), end());
      Allocator::deallocate(begin(), capacity());
      data_ = nullptr;
    }
    capacity_ = 0;
    size_ = 0;
  }
  void swap(Storage& other) {
    std::swap(data_, other.data_);
    std::swap(capacity_, other.capacity_);
    std::swap(size_, other.size_);
  }
  void init() {
    data_ = nullptr;
    capacity_ = 0;
    size_ = 0;
  }
  Allocator get_allocator() const { return *this; }

  T* data_;
  SizeType capacity_;
  SizeType size_;
};  // class Storage

}  // namespace gtl