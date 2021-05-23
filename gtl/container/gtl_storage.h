/**
 * @file gtl_storage.h
 * @author gwq5210 (gwq5210@qq.com)
 * @brief 管理连续内存区域的类，用于实现Vector和Deque
 * @date 2021-05-16
 *
 * @copyright Copyright (c) 2021. All rights reserved.
 */

#pragma once

#include "gtl_memory.h"

namespace gtl {

template <typename T, size_t fixed_capacity = 0>
class UStorage : public std::allocator<T> {
 public:
  typedef T value_type;
  typedef T& reference;
  typedef const T& const_reference;
  typedef T* pointer;
  typedef const T* const_pointer;
  typedef T* iterator;
  typedef const T* const_iterator;
  typedef size_t size_type;
  typedef ptrdiff_t difference_type;
  typedef std::allocator<T> allocator_type;

  UStorage() : data_(nullptr) {}
  explicit UStorage(size_type n) { allocate(); }
  UStorage(const UStorage& other) = delete;
  UStorage& operator=(const UStorage& other) = delete;
  UStorage(UStorage&& other) {
    data_ = other.data_;

    other.init();
  }
  UStorage& operator=(UStorage&& other) { assign(std::move(other)); }
  ~UStorage() { release(); }

  void assign(UStorage&& other) {
    release();

    data_ = other.data_;

    other.init();
  }

  // Element access
  reference at(size_type i) { return *(begin() + i); }
  const_reference at(size_type i) const { return *(begin() + i); }
  reference front() { return *begin(); }
  const_reference front() const { return *begin(); }
  reference operator[](size_type i) { return *(begin() + i); }
  const_reference operator[](size_type i) const { return *(begin() + i); }
  pointer data() { return data_; }
  const_pointer data() const { return data_; }

  // Iterators
  iterator begin() { return data(); }
  const_iterator begin() const { return data(); }
  const_iterator cbegin() const { return begin(); }
  iterator finish() { return begin() + capacity(); }
  const_iterator finish() const { return begin() + capacity(); }
  const_iterator cfinish() const { return finish(); }

  // Capacity
  size_type capacity() const { return fixed_capacity; }
  void allocate() { data_ = allocator_type::allocate(fixed_capacity); }
  void release() {
    if (begin()) {
      // printf("deallocate %zu %p\n", capacity_, data_);
      allocator_type::deallocate(begin(), capacity());
      data_ = nullptr;
    }
  }
  void swap(UStorage& other) { std::swap(data_, other.data_); }
  void init() { data_ = nullptr; }
  allocator_type get_allocator() const { return *this; }

  T* data_;
};  // class UStorage

template <typename T>
class UStorage<T, 0> : public std::allocator<T> {
 public:
  typedef T value_type;
  typedef T& reference;
  typedef const T& const_reference;
  typedef T* pointer;
  typedef const T* const_pointer;
  typedef T* iterator;
  typedef const T* const_iterator;
  typedef size_t size_type;
  typedef ptrdiff_t difference_type;
  typedef std::allocator<T> allocator_type;

  static constexpr size_type min_capacity_ = 16;

  UStorage() : data_(nullptr), capacity_(0) {}
  explicit UStorage(size_type n) {
    allocate(n);
    // printf("allocate %zu %zu %p\n", n, capacity(), begin());
  }
  UStorage(const UStorage& other) = delete;
  UStorage& operator=(const UStorage& other) = delete;
  UStorage(UStorage&& other) {
    data_ = other.data_;
    capacity_ = other.capacity_;

    other.init();
  }
  UStorage& operator=(UStorage&& other) { assign(std::move(other)); }
  ~UStorage() { release(); }

  void assign(UStorage&& other) {
    release();

    data_ = other.data_;
    capacity_ = other.capacity_;

    other.init();
  }

  // Element access
  reference at(size_type i) { return *(begin() + i); }
  const_reference at(size_type i) const { return *(begin() + i); }
  reference front() { return *begin(); }
  const_reference front() const { return *begin(); }
  reference operator[](size_type i) { return *(begin() + i); }
  const_reference operator[](size_type i) const { return *(begin() + i); }
  pointer data() { return data_; }
  const_pointer data() const { return data_; }

  // Iterators
  iterator begin() { return data(); }
  const_iterator begin() const { return data(); }
  const_iterator cbegin() const { return begin(); }
  iterator finish() { return begin() + capacity(); }
  const_iterator finish() const { return begin() + capacity(); }
  const_iterator cfinish() const { return finish(); }

  // Capacity
  size_type capacity() const { return capacity_; }
  size_type next_capacity(size_type new_size) {
    size_type next_cap = 1;
    while (next_cap < new_size) {
      next_cap <<= 1;
    }
    next_cap = std::max(next_cap, min_capacity_);
    // printf("next_cap %zu %zu\n", new_size, next_cap);
    return next_cap;
  }
  void allocate(size_type n) {
    capacity_ = next_capacity(n);
    data_ = allocator_type::allocate(capacity_);
  }
  void release() {
    if (begin()) {
      // printf("deallocate %zu %p\n", capacity_, data_);
      allocator_type::deallocate(begin(), capacity());
      data_ = nullptr;
    }
    capacity_ = 0;
  }
  void swap(UStorage& other) {
    std::swap(data_, other.data_);
    std::swap(capacity_, other.capacity_);
  }
  void init() {
    data_ = nullptr;
    capacity_ = 0;
  }
  allocator_type get_allocator() const { return *this; }

  T* data_;
  size_type capacity_;
};  // class UStorage

/**
 * @brief 管理连续内存区域的类，用于实现Vector
 *
 * 例子：
 * size_t old_capacity = 16;
 * size_t new_capacity = 1024;
 * Storage old_storage(old_capacity);
 * Storage new_storage(new_capacity);
 * new_storage.unsafe_append_copy(new_capacity, old.begin(), old_storage.end(), old_storage.size());
 * new_storage.swap(old_storage); // or new_storage = std::move(old_storage);
 */
template <typename T, size_t fix_capacity = 0>
class Storage : public UStorage<T, fix_capacity> {
 public:
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
  typedef UStorage<T, fix_capacity> Base;
  typedef Base::allocator_type allocator_type;

  static constexpr size_type min_capacity_ = 16;

  Storage() : size_(0) {}
  explicit Storage(size_type n) : Base(n), size_(0) {}
  Storage(const Storage& other) = delete;
  Storage& operator=(const Storage& other) = delete;
  Storage(Storage&& other) { assign(std::move(other)); }
  Storage& operator=(Storage&& other) { assign(std::move(other)); }
  ~Storage() { release(); }

  void assign(Storage&& other) {
    release();

    Base::data_ = other.data_;
    Base::capacity_ = other.capacity_;
    size_ = other.size_;

    other.init();
  }

  // Element access
  reference back() { return *(end() - 1); }
  const_reference back() const { return *(end() - 1); }

  // Iterators
  iterator end() { return Base::begin() + size(); }
  const_iterator end() const { return Base::begin() + size(); }
  const_iterator cend() const { return end(); }
  reverse_iterator rend() { return reverse_iterator(Base::begin()); }
  const_reverse_iterator rend() const { return reverse_iterator(Base::begin()); }
  const_reverse_iterator crend() const { return reverse_iterator(Base::begin()); }

  // Capacity
  size_type size() const { return size_; }
  void set_size(size_type value) { size_ = value; }
  void incr_size(difference_type value) { size_ += value; }
  bool empty() const { return size() == 0; }
  bool full(size_type n) const { return size() + n > Base::capacity(); }

  // Modifiers
  void unsafe_append_value_construct(size_type count) {
    gtl::uninitialized_value_construct_n(end(), count);
    size_ += count;
  }
  void unsafe_append_fill(size_type count, const T& v) {
    gtl::uninitialized_fill_n(end(), count, v);
    size_ += count;
  }
  template <typename InputIt, typename Category = typename std::iterator_traits<InputIt>::iterator_category>
  void unsafe_append_copy(InputIt first, InputIt last, size_type count = 0) {
    gtl::uninitialized_copy(first, last, end());
    size_ += (count ? count : std::distance(first, last));
  }
  template <typename InputIt, typename Category = typename std::iterator_traits<InputIt>::iterator_category>
  void unsafe_append_move(InputIt first, InputIt last, size_type count = 0) {
    gtl::uninitialized_move(first, last, end());
    size_ += (count ? count : std::distance(first, last));
  }
  template <typename... Args>
  void unsafe_append(Args&&... args) {
    gtl::construct_at(end(), std::forward<Args>(args)...);
    ++size_;
  }
  void release() {
    Base::release();
    gtl::destroy(Base::begin(), end());
    size_ = 0;
  }
  void swap(Storage& other) {
    Base::swap(other);
    std::swap(size_, other.size_);
  }
  void init() {
    Base::init();
    size_ = 0;
  }

  size_type size_;
};  // class Storage

}  // namespace gtl