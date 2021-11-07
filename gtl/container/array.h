/**
 * @file array.h
 * @author gwq5210 (gwq5210@qq.com)
 * @brief array的实现
 * @date 2021-05-15
 *
 * @copyright Copyright (c) 2021. All rights reserved.
 */

#pragma once

#include <algorithm>
#include <iterator>
#include <type_traits>

#include "gtl/algorithm.h"
#include "gtl/memory_op.h"
#include "gtl/iterator.h"

namespace gtl {

template <typename T, size_t size_>
class Array {
 public:
  using value_type = T;
  using reference = T&;
  using const_reference = const T&;
  using pointer = T*;
  using const_pointer = const T*;
  using iterator = T*;
  using const_iterator = const T*;
  using reverse_iterator = gtl::reverse_iterator<iterator>;
  using const_reverse_iterator = gtl::reverse_iterator<const_iterator>;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;

  // Element access
  T& at(size_type i) { return *(begin() + i); }
  const T& at(size_type i) const { return *(begin() + i); }
  T& front() { return *begin(); }
  const T& front() const { return *begin(); }
  T& back() { return *(end() - 1); }
  const T& back() const { return *(end() - 1); }
  T& operator[](size_type i) { return *(begin() + i); }
  const T& operator[](size_type i) const { return *(begin() + i); }
  T* data() { return data_; }
  const T* data() const { return data_; }

  // Iterators
  iterator begin() { return data(); }
  const_iterator begin() const { return data(); }
  const_iterator cbegin() const { return begin(); }
  iterator end() { return begin() + size_; }
  const_iterator end() const { return begin() + size_; }
  const_iterator cend() const { return end(); }
  reverse_iterator rbegin() { return reverse_iterator(end()); }
  const_reverse_iterator rbegin() const { return reverse_iterator(end()); }
  const_reverse_iterator crbegin() const { return reverse_iterator(end()); }
  reverse_iterator rend() { return reverse_iterator(begin()); }
  const_reverse_iterator rend() const { return reverse_iterator(begin()); }
  const_reverse_iterator crend() const { return reverse_iterator(begin()); }

  // Capacity
  size_type size() const { return size_; }
  bool empty() const { return size_ == 0; }

  // Operations
  void fill(const T& v) { gtl::fill_n(begin(), size_, v); }
  void swap(Array& other) {
    if (this != &other) {
      gtl::swap_ranges(begin(), end(), other.begin());
    }
  }

  T data_[size_];
};  // class Array

template <typename T, size_t size_>
using array = Array<T, size_>;

template <typename T, size_t size_>
bool operator==(const Array<T, size_>& lhs, const Array<T, size_>& rhs) {
  return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename T, size_t size_>
bool operator!=(const Array<T, size_>& lhs, const Array<T, size_>& rhs) {
  return !(lhs != rhs);
}

template <typename T, size_t size_>
bool operator<(const Array<T, size_>& lhs, const Array<T, size_>& rhs) {
  return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename T, size_t size_>
bool operator>(const Array<T, size_>& lhs, const Array<T, size_>& rhs) {
  return rhs < lhs;
}

template <typename T, size_t size_>
bool operator<=(const Array<T, size_>& lhs, const Array<T, size_>& rhs) {
  return !(lhs > rhs);
}

template <typename T, size_t size_>
bool operator>=(const Array<T, size_>& lhs, const Array<T, size_>& rhs) {
  return !(lhs < rhs);
}

}  // namespace gtl