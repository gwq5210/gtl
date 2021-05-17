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
  typedef T value_type;
  typedef T& reference;
  typedef const T& const_reference;
  typedef T* pointer;
  typedef const T* const_pointer;
  typedef T* iterator;
  typedef const T* const_iterator;
  typedef std::reverse_iterator<iterator> reverse_iterator;
  typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
  typedef size_t size_type;
  typedef ptrdiff_t difference_type;
  typedef Allocator allocator_type;

  static constexpr size_type min_capacity = 16;

  Storage() : data_(nullptr), capacity_(0), size_(0) {}
  Storage(size_type n) : size_(0) {
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

  // Element access
  reference at(size_type i) { return *(begin() + i); }
  const_reference at(size_type i) const { return *(begin() + i); }
  reference front() { return *begin(); }
  const_reference front() const { return *begin(); }
  reference back() { return *(end() - 1); }
  const_reference back() const { return *(end() - 1); }
  reference operator[](size_type i) { return *(begin() + i); }
  const_reference operator[](size_type i) const { return *(begin() + i); }
  pointer data() { return data_; }
  const_pointer data() const { return data_; }

  // Iterators
  iterator begin() { return data(); }
  const_iterator begin() const { return data(); }
  const_iterator cbegin() const { return begin(); }
  iterator end() { return begin() + size(); }
  const_iterator end() const { return begin() + size(); }
  const_iterator cend() const { return end(); }
  reverse_iterator rbegin() { return reverse_iterator(end()); }
  const_reverse_iterator rbegin() const { return reverse_iterator(end()); }
  const_reverse_iterator crbegin() const { return reverse_iterator(end()); }
  reverse_iterator rend() { return reverse_iterator(begin()); }
  const_reverse_iterator rend() const { return reverse_iterator(begin()); }
  const_reverse_iterator crend() const { return reverse_iterator(begin()); }

  // Capacity
  size_type size() const { return size_; }
  size_type capacity() const { return capacity_; }
  bool empty() const { return size_ == 0; }
  bool full(size_type n) const { return size_ + n > capacity_; }
  size_type next_capacity(size_type new_size) {
    size_type next_cap = 1;
    while (next_cap < new_size) {
      next_cap <<= 1;
    }
    next_cap = std::max(next_cap, min_capacity);
    // printf("next_cap %zu %zu\n", new_size, next_cap);
    return next_cap;
  }
  void allocate(size_type n) {
    n = next_capacity(n);
    data_ = Allocator::allocate(n);
    capacity_ = n;
  }

  // Modifiers
  void unsafe_append_value_construct(size_type count) {
    std::uninitialized_value_construct_n(end(), count);
    size_ += count;
  }
  void unsafe_append_fill(size_type count, const T& v) {
    std::uninitialized_fill_n(end(), count, v);
    size_ += count;
  }
  template <typename II, typename Category = typename std::iterator_traits<II>::iterator_category>
  void unsafe_append_copy(II first, II last, size_type count = 0) {
    std::uninitialized_copy(first, last, end());
    size_ += (count ? count : std::distance(first, last));
  }
  template <typename II, typename Category = typename std::iterator_traits<II>::iterator_category>
  void unsafe_append_move(II first, II last, size_type count = 0) {
    std::uninitialized_move(first, last, end());
    size_ += (count ? count : std::distance(first, last));
  }
  template <typename... Args>
  void unsafe_append(Args&&... args) {
    construct_at(end(), std::forward<Args>(args)...);
    ++size_;
  }
  iterator erase(const_iterator first, const_iterator last) {
    if (first != last) {
      size_type count = std::distance(first, last);
      std::move(last, cend(), iterator(first));
      std::destroy(end() - count, end());
      size_ -= count;
    }
    return iterator(last);
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
  size_type capacity_;
  size_type size_;
};  // class Storage

}  // namespace gtl