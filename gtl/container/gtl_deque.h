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
#include "gtl_vector.h"

namespace gtl {

template <typename T>
class Deque {
 public:
  using StorageType = Storage<T>;
  using StorageAllocator = std::allocator<StorageType>;
  using StoragePtrType = Storage<StorageType*>;
  using value_type = typename StorageType::value_type;
  using reference = typename StorageType::reference;
  using const_reference = typename StorageType::const_reference;
  using pointer = typename StorageType::pointer;
  using const_pointer = typename StorageType::const_pointer;
  using size_type = typename StorageType::size_type;
  using difference_type = typename StorageType::difference_type;
  using allocator_type = typename StorageType::allocator_type;

  static constexpr size_type block_capacity_ = 512;

  class CIterator {
   public:
    typedef std::random_access_iterator_tag iterator_category;
    typedef Deque::const_reference reference;
    typedef Deque::const_pointer pointer;
    typedef Deque::difference_type difference_type;
    typedef Deque::value_type value_type;
    typedef CIterator Self;
    CIterator() : curr_(), block_() {}
    CIterator(typename StorageType::iterator curr, typename StoragePtrType::iterator block)
        : curr_(curr), block_(block) {}
    CIterator(const CIterator& other) = default;
    CIterator(CIterator&& other) : curr_(), block_() {
      std::swap(curr_, other.curr_);
      std::swap(block_, other.block_);
    }
    typename StorageType::iterator curr() { return curr_; }
    typename StoragePtrType::reference block() { return *block_; }
    typename StoragePtrType::iterator block_iterator() { return block_; }
    typename StorageType::const_iterator curr() const { return curr_; }
    typename StoragePtrType::const_reference block() const { return *block_; }
    typename StoragePtrType::const_iterator block_iterator() const { return block_; }
    void set(typename StorageType::iterator curr, typename StoragePtrType::iterator block) {
      curr_ = curr;
      block_ = block;
    }
    reference operator*() { return *curr_; }
    pointer operator->() { return curr_; }
    Self& operator++() {
      ++curr_;
      if (curr_ == block()->finish()) {
        ++block_;
        curr_ = block()->begin();
      }
      return *this;
    }
    Self operator++(int) {
      auto ret = *this;
      operator++();
      return ret;
    }
    Self& operator--() {
      if (curr_ == block()->begin()) {
        --block_;
        curr_ = block()->finish();
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
        n += curr_ - block()->begin();
        block_ += n / block_capacity_;
        curr_ = block()->begin() + n % block_capacity_;
      }
      return *this;
    }
    Self& operator-=(difference_type n) {
      if (n < 0) {
        *this += (-n);
      } else if (n > 0) {
        n += block()->end() - curr_ - 1;
        block_ -= n / block_capacity_;
        block_ = block()->end() - n % block_capacity_ - 1;
      }
      return *this;
    }
    Self operator+(difference_type n) {
      Self ret = *this;
      *this += n;
      return ret;
    }
    Self operator-(difference_type n) {
      Self ret = *this;
      *this -= n;
      return ret;
    }
    reference operator[](difference_type n) { return *(*this + n); }
    difference_type operator-(const Self& other) {
      return (block_ - other.block_) * block_capacity_ + (other.block()->begin() - other.curr_) +
             (curr_ - block()->begin());
    }
    bool operator==(const CIterator& other) { return curr_ == other.curr_ && block_ == other.block_; }
    bool operator!=(const CIterator& other) { return !(*this == other); }
    bool operator<(const CIterator& other) {
      return block_ == other.block_ ? curr_ < other.curr_ : block_ < other.block_;
    }
    bool operator>(const CIterator& other) { return other < *this; }
    bool operator<=(const CIterator& other) { return !(*this > other); }
    bool operator>=(const CIterator& other) { return !(*this < other); }

   private:
    typename StorageType::iterator curr_;
    typename StoragePtrType::iterator block_;
  };

  class Iterator : public CIterator {
   public:
    using BaseType = CIterator;
    using reference = Deque::reference;
    using pointer = Deque::pointer;
    using difference_type = typename BaseType::difference_type;
    using value_type = typename BaseType::value_type;
    using Self = Iterator;
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
    Self operator+(difference_type n) {
      Self ret = *this;
      *this += n;
      return ret;
    }
    Self operator-(difference_type n) {
      Self ret = *this;
      *this -= n;
      return ret;
    }
    reference operator[](difference_type n) { return *(*this + n); }
  };

  using iterator = Iterator;
  using const_iterator = CIterator;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  Deque() = default;
  Deque(size_type count) : Deque(count, T()) {}
  Deque(size_type count, const T& v) { insert(end(), count, v); }
  template <typename II, typename Category = typename std::iterator_traits<II>::iterator_category>
  Deque(II first, II last) { emplace(end(), first, last); }
  Deque(std::initializer_list<T> il) { emplace(end(), il.begin(), il.end()); }
  Deque(const Deque& other) { emplace(end(), other.begin(), other.end()); }
  Deque(Deque&& other) = default;
  ~Deque() { erase(begin(), end()); }

  Deque& operator=(const Deque& other) { assign(other); return *this; }
  Deque& operator=(Deque&& other) { assign(std::move(other)); return *this; }
  Deque& operator=(std::initializer_list<T> il) { assign(il); return *this; }

  void assign(size_type count, const T& v) {}
  template <typename II, typename Category = typename std::iterator_traits<II>::iterator_category>
  void assign(II first, II last) {}
  void assign(const Deque& other) {}
  void assign(Deque&& other) {}
  void assign(std::initializer_list<T> il) {}

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

  // Capacity
  size_type size() const { return end() - begin(); }
  bool empty() const { return size() == 0; }

  // Modifiers
  iterator insert(const_iterator before, const T& v) { return emplace(before, v); }
  iterator insert(const_iterator before, size_type count, const T& v) { return emplace(before, v); }
  template <typename... Args>
  iterator emplace(const_iterator before, Args&&... args) {
    if (!d_.capacity()) {
      d_.allocate(16);
      d_.unsafe_append(new StorageType(block_capacity_));
      begin_.set(d_.front()->begin(), d_.begin());
      end_.set(d_.front()->begin(), d_.begin());
    }
    if (end_.curr() != end_.block()->finish() - 1) {
      d_.unsafe_append(new StorageType(block_capacity_));
    }
    end_.block()->unsafe_append(std::forward<Args>(args)...);
    ++end_;
    return end_;
  }
  void erase(const_iterator first, const_iterator last) {}

 private:
  iterator begin_;
  iterator end_;
  StoragePtrType d_;
};  // class Deque

template <typename T>
using deque = Deque<T>;

template <typename T>
typename Deque<T>::CIterator operator+(typename Deque<T>::CIterator::difference_type n,
                                       const typename Deque<T>::CIterator& other) {
  auto ret = other;
  ret += n;
  return ret;
}

template <typename T>
typename Deque<T>::CIterator operator-(typename Deque<T>::CIterator::difference_type n,
                                       const typename Deque<T>::CIterator& other) {
  auto ret = other;
  ret -= n;
  return ret;
}

template <typename T>
typename Deque<T>::Iterator operator+(typename Deque<T>::Iterator::difference_type n,
                                      const typename Deque<T>::Iterator& other) {
  auto ret = other;
  ret += n;
  return ret;
}

template <typename T>
typename Deque<T>::Iterator operator-(typename Deque<T>::Iterator::difference_type n,
                                      const typename Deque<T>::Iterator& other) {
  auto ret = other;
  ret -= n;
  return ret;
}

template <typename T>
bool operator==(const Deque<T>& lhs, const Deque<T>& rhs) {
  return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename T>
bool operator!=(const Deque<T>& lhs, const Deque<T>& rhs) {
  return !(lhs != rhs);
}

template <typename T>
bool operator<(const Deque<T>& lhs, const Deque<T>& rhs) {
  return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename T>
bool operator>(const Deque<T>& lhs, const Deque<T>& rhs) {
  return rhs < lhs;
}

template <typename T>
bool operator<=(const Deque<T>& lhs, const Deque<T>& rhs) {
  return !(lhs > rhs);
}

template <typename T>
bool operator>=(const Deque<T>& lhs, const Deque<T>& rhs) {
  return !(lhs < rhs);
}

}  // namespace gtl