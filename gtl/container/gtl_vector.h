/**
 * @file gtl_vector.h
 * @author gwq5210 (gwq5210@qq.com)
 * @brief vector的实现
 * @date 2021-05-15
 * 
 * @copyright Copyright (c) 2021. All rights reserved.
 */

#pragma once

#include <algorithm>
#include <iterator>
#include <type_traits>

#include "gtl_memory.h"
#include "gtl_storage.h"

namespace gtl {

template <typename T>
class Vector {
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
  typedef std::allocator<T> allocator_type;
  typedef gtl::Storage<T, allocator_type> Storage;

  // constructor
  Vector() = default;
  explicit Vector(size_type size) : d_(size) {
    printf("%s\n", __FUNCTION__);
    d_.unsafe_append_value_construct(size);
  }
  Vector(size_type size, const T& v) : d_(size) {
    printf("%s\n", __FUNCTION__);
    d_.unsafe_append_fill(size, v);
  }
  template <typename II, typename Category = typename std::iterator_traits<II>::iterator_category>
  Vector(II first, II last) {
    printf("%s range\n", __FUNCTION__);
    size_type count = std::distance(first, last);
    d_.allocate(count);
    d_.unsafe_append_copy(first, last, count);
  }
  Vector(std::initializer_list<T> il) : d_(il.size()){
    printf("%s il %zu\n", __FUNCTION__, il.size());
    d_.unsafe_append_copy(il.begin(), il.end(), il.size());
  }
  Vector(const Vector& other) : d_(other.size()) { d_.unsafe_append_copy(other.begin(), other.end(), other.size()); }
  Vector(Vector&& other) { d_.swap(other.d_); }
  Vector& operator=(const Vector& other) {
    assign(other);
    return *this;
  }
  Vector& operator=(Vector&& other) {
    assign(std::move(other));
    return *this;
  }
  Vector& operator=(std::initializer_list<T> il) {
    assign(il);
    return *this;
  }
  void assign(const Vector& other) {
    printf("%s\n", __FUNCTION__);
    if (this != &other) {
      assign_range(other.size(), other.begin(), other.end());
    }
  }
  void assign(Vector&& other) {
    printf("%s move\n", __FUNCTION__);
    if (this != &other) {
      d_.release();
      d_.swap(other.d_);
    }
  }
  void assign(std::initializer_list<T> il) {
    printf("%s il\n", __FUNCTION__);
    assign_range(il.size(), il.begin(), il.end());
  }
  template <typename II>
  void assign(II first, II last) {
    printf("%s range\n", __FUNCTION__);
    assign_range(std::distance(first, last), first, last);
  }

  allocator_type get_allocator() const { return d_.get_allocator(); }

  // element access
  T& at(size_type i) { return *(begin() + i); }
  const T& at(size_type i) const { return *(begin() + i); }
  T& front() { return *begin(); }
  const T& front() const { return *begin(); }
  T& back() { return *(end() - 1); }
  const T& back() const { return *(end() - 1); }
  T& operator[](size_type i) { return *(begin() + i); }
  const T& operator[](size_type i) const { return *(begin() + i); }
  T* data() { return d_.begin(); }
  const T* data() const { return d_.begin(); }

  // iterators
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

  // capacity
  size_type capacity() const { return d_.capacity(); }
  size_type size() const { return d_.size(); }
  bool empty() const { return size() == 0; }
  void reserve(size_type new_capacity) { grow(new_capacity); }

  // modifiers
  void push_back(const T& v) { emplace_back(v); }
  void push_back(T&& v) { emplace_back(std::move(v)); }
  iterator insert(const_iterator before, size_type count, const T& v) {
    size_type insert_pos = before - begin();
    size_type n = end() - before;
    if (count == 0) {
      return begin() + insert_pos;
    }
    if (!d_.full(count)) {
      if (count <= n) {
        std::uninitialized_move(end() - count, end(), end());
        std::move_backward(begin() + insert_pos, begin() + insert_pos + n - count, end());
        std::fill_n(begin() + insert_pos, count, v);
      } else {
        std::uninitialized_move(end() - n, end(), end() + count - n);
        std::fill_n(begin() + insert_pos, n, v);
        std::uninitialized_fill_n(end(), count - n, v);
      }
      d_.size_ += count;
    } else {
      Storage new_storage(size() + count);
      new_storage.unsafe_append_move(begin(), end() - n);
      new_storage.unsafe_append_fill(count, v);
      new_storage.unsafe_append_move(end() - n, end());
      d_.swap(new_storage);
    }
    return begin() + insert_pos;
  }
  iterator insert(size_type idx, size_type count, const T& v) { return insert(begin() + idx, count, v); }
  iterator insert(const_iterator before, const T& v) { return emplace(before, v); }
  iterator insert(size_type idx, const T& v) { return emplace(begin() + idx, v); }
  iterator insert(const_iterator before, T&& v) { return emplace(before, std::move(v)); }
  iterator insert(size_type idx, T&& v) { return emplace(begin() + idx, std::move(v)); }
  iterator insert(const_iterator before, std::initializer_list<T> il) { return insert(before, il.begin(), il.end()); }
  iterator insert(size_type idx, std::initializer_list<T> il) { return insert(begin() + idx, il.begin(), il.end()); }
  template <typename II, typename Category = typename std::iterator_traits<II>::iterator_category>
  iterator insert(const_iterator before, II first, II last) {
    size_type count = std::distance(first, last);
    size_type insert_pos = before - begin();
    size_type n = end() - before;
    if (count == 0) {
      return begin() + insert_pos;
    }
    if (!d_.full(count)) {
      if (count <= n) {
        std::uninitialized_move(end() - count, end(), end());
        std::move_backward(begin() + insert_pos, begin() + n - count, begin() + n - count);
        std::copy(first, last, begin() + insert_pos);
      } else {
        std::uninitialized_move(end() - n, end(), end() + count - n);
        std::copy(first, first + n, begin() + insert_pos);
        std::uninitialized_copy(first + n, last, end());
      }
      d_.size_ += count;
    } else {
      Storage new_storage(size() + count);
      new_storage.unsafe_append_move(begin(), end() - n);
      new_storage.unsafe_append_copy(first, last);
      new_storage.unsafe_append_move(end() - n, end());
      d_.swap(new_storage);
    }
    return begin() + insert_pos;
  }
  template <typename II, typename Category = typename std::iterator_traits<II>::iterator_category>
  iterator insert_safe(const_iterator before, II first, II last) {
    Vector tmp_vec(first, last);
    return insert(before, tmp_vec.begin(), tmp_vec.end());
  }
  template <typename II, typename Category = typename std::iterator_traits<II>::iterator_category>
  iterator insert(size_type idx, II first, II last) {
    return insert(begin() + idx, first, last);
  }
  void pop_back() { erase(end() - 1); }
  iterator remove_at(size_type idx) { return erase(begin() + idx); }
  iterator erase(size_type idx) { return erase(begin() + idx); }
  iterator erase(size_type first, size_type last) { return erase(begin() + first, begin() + last); }
  iterator erase(const_iterator pos) { return erase(pos, pos + 1); }
  iterator erase(const_iterator first, const_iterator last) { return d_.erase(first, last); }
  template <typename... Args>
  iterator emplace(const_iterator before, Args&&... args) {
    size_type insert_pos = before - begin();
    bool at_end = before == end();
    if (!d_.full(1)) {
      if (at_end) {
        construct_at(end(), std::forward<Args>(args)...);
      } else {
        std::uninitialized_move(end() - 1, end(), end());
        std::move_backward(begin() + insert_pos, end() - 1, end() - 1);
        T tmp(std::forward<Args>(args)...);
        *(begin() + insert_pos) = std::move(tmp);
      }
      ++d_.size_;
    } else {
      Storage new_storage(size() + 1);
      new_storage.unsafe_append_move(begin(), begin() + insert_pos);
      new_storage.unsafe_append(std::forward<Args>(args)...);
      if (!at_end) {
        new_storage.unsafe_append_move(begin() + insert_pos, end());
      }
      d_.swap(new_storage);
    }
    return begin() + insert_pos;
  }
  template <typename... Args>
  iterator emplace(size_type idx, Args&&... args) {
    return emplace(begin() + idx, std::forward<Args>(args)...);
  }
  template <typename... Args>
  T& emplace_back(Args&&... args) {
    return *emplace(end(), std::forward<Args>(args)...);
  }
  template <typename... Args>
  iterator replace(size_type idx, Args&&... args) {
    return replace(begin() + idx, std::forward<Args>(args)...);
  }
  template <typename... Args>
  iterator replace(const_iterator pos, Args&&... args) {
    if (pos == end()) {
      return emplace(pos, std::forward<Args>(args)...);
    }
    T tmp(std::forward<Args>(args)...);
    *iterator(pos) = std::move(tmp);
    return iterator(pos);
  }
  template <typename II, typename Category = typename std::iterator_traits<II>::iterator_category>
  iterator replace(const_iterator pos, II first, II last) {
    size_type n = cend() - pos;
    size_type count = std::distance(first, last);
    if (count == 0) {
      return iterator(pos);
    } else if (pos == end()) {
      return insert(pos, first, last);
    }
    if (count <= n) {
      std::copy(first, last, iterator(pos));
    } else if (!d_.full(count - n)) {
      std::copy(first, first + n, iterator(pos));
      std::uninitialized_copy(first + n, last, end());
      d_.size_ += count - n;
    } else {
      Storage new_storage(size() + count - n);
      new_storage.unsafe_append_move(begin(), iterator(pos));
      new_storage.unsafe_append_copy(first, last, count);
      d_.swap(new_storage);
    }
    return end() - n;
  }
  void resize(size_type new_size, const T& v = T()) {
    if (new_size > size()) {
      insert(end(), new_size - size(), v);
    } else if (new_size < size()) {
      erase(begin() + new_size, end());
    }
  }
  void clear() { erase(begin(), end()); }
  void swap(Vector& other) {
    if (this != &other) {
      d_.swap(other.d_);
    }
  }

 private:
  template <typename II>
  void assign_range(size_type count, II first, II last) {
    if (count == 0) {
      return;
    }
    if (count < size()) {
      erase(begin() + count, end());
    }
    replace(begin(), first, last);
  }
  void grow(size_type new_capacity) {
    if (capacity() >= new_capacity) {
      return;
    }
    Storage new_storage(new_capacity);
    new_storage.unsafe_append_move(begin(), end());
    printf("grow capacity new capacity: %zu/%zu, ptr: %p/%p\n", capacity(), new_storage.capacity(), new_storage.begin(),
           d_.begin());
    d_.swap(new_storage);
  }

  Storage d_;
};  // class Vector

template <typename T>
using vector = Vector<T>;

template <typename T>
bool operator==(const Vector<T>& lhs, const Vector<T>& rhs) {
  return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename T>
bool operator!=(const Vector<T>& lhs, const Vector<T>& rhs) {
  return !(lhs != rhs);
}

template <typename T>
bool operator<(const Vector<T>& lhs, const Vector<T>& rhs) {
  return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename T>
bool operator>(const Vector<T>& lhs, const Vector<T>& rhs) {
  return rhs < lhs;
}

template <typename T>
bool operator<=(const Vector<T>& lhs, const Vector<T>& rhs) {
  return !(lhs > rhs);
}

template <typename T>
bool operator>=(const Vector<T>& lhs, const Vector<T>& rhs) {
  return !(lhs < rhs);
}

}  // namespace gtl