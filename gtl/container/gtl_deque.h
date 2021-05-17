/**
 * @file gtl_deque.h
 * @author gwq5210 (gwq5210@qq.com)
 * @brief deque的实现
 * @date 2021-05-16
 * 
 * @copyright Copyright (c) 2021. All rights reserved.
 */

#pragma once

#include "gtl_storage.h"

namespace gtl {

template <typename T>
class Deque {
 public:
  using StorageType = Storage<T>;
  using StoragePtrType = Storage<StorageType*>;
  using value_type = StorageType::value_type;
  using reference = StorageType::reference;
  using const_reference = StorageType::const_reference;
  using pointer = StorageType::pointer;
  using const_pointer = StorageType::const_pointer;
  using iterator = Deque::Iterator;
  using const_iterator = Deque::CIterator;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;
  using size_type = StorageType::size_type;
  using difference_type = StorageType::difference_type;
  using allocator_type = StorageType::allocator_type;

  static constexpr size_type block_capacity_ = 512;

  class CIterator {
   public:
    typedef std::random_access_iterator_tag iterator_category;
    typedef Deque::const_reference reference;
    typedef Deque::const_pointer pointer;
    typedef Deque::difference_type difference_type;
    typedef Deque::value_type value_type;
    typedef CIterator Self;
    CIterator() : d_(nullptr), curr_(), curr_block_() {}
    reference operator*() { return *curr_; }
    pointer operator->() { return curr_; }
    Self& operator++() {
      ++curr_;
      if (curr_ == (*curr_block_)->end()) {
        ++curr_block_;
        curr_ = (*curr_block_)->begin();
      }
      return *this;
    }
    Self operator++(int) {
      auto ret = *this;
      operator++();
      return ret;
    }
    Self& operator--() {
      if (curr_ == (*curr_block_)->begin()) {
        --curr_block_;
        curr_ = (*curr_block_)->end();
      }
      --curr_;
      return *this;
    }
    Self operator--(int) {
      auto ret = *this;
      operator--();
      return ret;
    }
    Self& operator+=(difference_type n) {
      if (n < 0) {
        *this -= (-n);
      } else if (n > 0) {
        n += curr_ - (*curr_block_)->begin();
        curr_block_ += n / block_capacity_;
        curr_ = (*curr_block_)->begin() + n % block_capacity_;
      }
      return *this;
    }
    Self& operator-=(difference_type n) {
      if (n < 0) {
        *this += (-n);
      } else if (n > 0) {
        n += (*curr_block_)->end() - curr_ - 1;
        curr_block_ -= n / block_capacity_;
        curr_block_ = (*curr_block_)->end() - n % block_capacity_ - 1;
      }
      return *this;
    }
    bool operator==(const CIterator& other) {
      return d_ == other.d_ && curr_ == other.curr_ && curr_block_ == other.curr_block_;
    }
    bool operator!=(const CIterator& other) {
      return !(*this == other);
    }
    StoragePtrType* d_;
    StorageType::iterator curr_;
    StoragePtrType::iterator curr_block_;
  };

  class Iterator : public CIterator {
   public:
    typedef Deque::reference reference;
    typedef Deque::pointer pointer;
    typedef Deque::difference_type difference_type;
    typedef Deque::value_type value_type;
    typedef CIterator BaseType;
    typedef Iterator Self;
    Iterator() = default;
    reference operator*() { return *BaseType::curr_; }
    pointer operator->() { return BaseType::curr_; }
    Self& operator++() {
      BaseType::operator++();
      return *this;
    }
    Self operator++(int) {
      auto ret = *this;
      BaseType::operator++();
      return ret;
    }
    Self& operator--() {
      BaseType::operator--();
      return *this;
    }
    Self operator--(int) {
      auto ret = *this;
      BaseType::operator--();
      return ret;
    }
    Self& operator+=(difference_type n) {
      BaseType::operator+=(n);
      return *this;
    }
    Self& operator-=(difference_type n) {
      BaseType::operator-=(n);
      return *this;
    }
  };

  // Element access
  T& at(size_type i) { return *(begin() + i); }
  const T& at(size_type i) const { return *(begin() + i); }
  T& front() { return *begin(); }
  const T& front() const { return *begin(); }
  T& back() { return *(end() - 1); }
  const T& back() const { return *(end() - 1); }
  T& operator[](size_type i) { return *(begin() + i); }
  const T& operator[](size_type i) const { return *(begin() + i); }

  // Iterators
  iterator begin() { return begin_; }
  const_iterator begin() const { return begin_; }
  const_iterator cbegin() const { return begin(); }
  iterator end() { return end_; }
  const_iterator end() const { return end_; }
  const_iterator cend() const { return end(); }
  reverse_iterator rbegin() { return reverse_iterator(end()); }
  const_reverse_iterator rbegin() const { return reverse_iterator(end()); }
  const_reverse_iterator crbegin() const { return reverse_iterator(end()); }
  reverse_iterator rend() { return reverse_iterator(begin()); }
  const_reverse_iterator rend() const { return reverse_iterator(begin()); }
  const_reverse_iterator crend() const { return reverse_iterator(begin()); }

 private:
  iterator begin_;
  iterator end_;
  StoragePtrType d_;
};  // class Deque

}  // namespace gtl