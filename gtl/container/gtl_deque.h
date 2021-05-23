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

template <typename DequeType>
class ConstDequeIterator {
 public:
  typedef std::random_access_iterator_tag iterator_category;
  typedef DequeType::const_reference reference;
  typedef DequeType::const_pointer pointer;
  typedef DequeType::difference_type difference_type;
  typedef DequeType::value_type value_type;
  typedef DequeType::size_type size_type;
  typedef DequeType::StoragePtrType::iterator BlockIterator;
  typedef DequeType::StoragePtrType::const_iterator ConstBlockIterator;
  typedef DequeType::StorageType::iterator ValueIterator;
  typedef DequeType::StorageType::const_iterator ConstValueIterator;
  typedef ConstDequeIterator Self;

  ConstDequeIterator() : block_(), curr_() {}
  ConstDequeIterator(BlockIterator block, ValueIterator curr) : block_(block), curr_(curr) {}
  ConstDequeIterator(const ConstDequeIterator& other) = default;
  ConstDequeIterator(ConstDequeIterator&& other) : block_(), curr_() {
    std::swap(curr_, other.curr_);
    std::swap(block_, other.block_);
  }
  ValueIterator curr() { return curr_; }
  BlockIterator block() { return block_; }
  ConstValueIterator curr() const { return curr_; }
  ConstBlockIterator block() const { return block_; }
  void set(BlockIterator new_block, size_type offset = 0) {
    block_ = new_block;
    curr_ = block_->begin() + offset;
  }
  reference operator*() const { return *curr_; }
  pointer operator->() const { return curr_; }
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
    if (n != 0) {
      n += curr_ - block()->begin();
      if (n >= 0) {
        block_ += n / DequeType::block_capacity_;
        curr_ = block()->begin() + n % DequeType::block_capacity_;
      } else {
        n -= DequeType::block_capacity_ - 1;
        block_ += n / DequeType::block_capacity_;
        curr_ = block()->begin() + n % DequeType::block_capacity_ + DequeType::block_capacity_ - 1;
      }
    }
    return *this;
  }
  Self& operator-=(difference_type n) {
    *this += -n;
    return *this;
  }
  Self operator+(difference_type n) const {
    Self ret = *this;
    ret += n;
    return ret;
  }
  Self operator-(difference_type n) const {
    Self ret = *this;
    ret -= n;
    return ret;
  }
  difference_type operator-(const Self& other) const {
    return (block_ - other.block_) * DequeType::block_capacity_ + (other.block()->begin() - other.curr_) +
           (curr_ - block()->begin());
  }
  reference operator[](difference_type n) { return *(*this + n); }
  bool operator==(const ConstDequeIterator& other) const { return curr_ == other.curr_ && block_ == other.block_; }
  bool operator!=(const ConstDequeIterator& other) const { return !(*this == other); }
  bool operator<(const ConstDequeIterator& other) const {
    return block_ == other.block_ ? curr_ < other.curr_ : block_ < other.block_;
  }
  bool operator>(const ConstDequeIterator& other) const { return other < *this; }
  bool operator<=(const ConstDequeIterator& other) const { return !(*this > other); }
  bool operator>=(const ConstDequeIterator& other) const { return !(*this < other); }

 private:
  BlockIterator block_;
  ValueIterator curr_;
};

template <typename DequeType>
class DequeIterator : public ConstDequeIterator<DequeType> {
 public:
  using Base = ConstDequeIterator<DequeType>;
  using reference = DequeType::reference;
  using pointer = DequeType::pointer;
  using difference_type = Base::difference_type;
  using BlockIterator = Base::BlockIterator;
  using ValueIterator = Base::ValueIterator;
  using Self = DequeIterator;
  DequeIterator() : Base() {}
  DequeIterator(BlockIterator block, ValueIterator curr) : Base(block, curr) {}
  void CopyFrom(const Base& other) { Base::set(BlockIterator(other.block()), other.curr() - other.block()->begin()); }
  reference operator*() { return *Base::curr(); }
  pointer operator->() { return Base::curr(); }
  Self& operator++() {
    Base::operator++();
    return *this;
  }
  Self operator++(int) {
    auto ret = *this;
    Base::operator++();
    return ret;
  }
  Self& operator--() {
    Base::operator--();
    return *this;
  }
  Self operator--(int) {
    auto ret = *this;
    Base::operator--();
    return ret;
  }
  Self& operator+=(difference_type n) {
    Base::operator+=(n);
    return *this;
  }
  Self& operator-=(difference_type n) {
    Base::operator-=(n);
    return *this;
  }
  Self operator+(difference_type n) const {
    Self ret = *this;
    ret += n;
    return ret;
  }
  Self operator-(difference_type n) const {
    Self ret = *this;
    ret -= n;
    return ret;
  }
  using Base::operator-;
  // or
  // difference_type operator-(const Base& other) const { return Base::operator-(other); }
  reference operator[](difference_type n) { return *(*this + n); }
};

template <typename DequeType>
ConstDequeIterator<DequeType> operator+(typename ConstDequeIterator<DequeType>::difference_type n,
                                        const ConstDequeIterator<DequeType>& other) {
  auto ret = other;
  ret += n;
  return ret;
}

template <typename DequeType>
DequeIterator<DequeType> operator+(typename DequeIterator<DequeType>::difference_type n,
                                   const DequeIterator<DequeType>& other) {
  auto ret = other;
  ret += n;
  return ret;
}

template <typename T>
class Deque {
 public:
  using StorageType = UStorage<T>;
  using StorageAllocator = std::allocator<StorageType>;
  using StoragePtrType = UStorage<StorageType>;
  using value_type = typename StorageType::value_type;
  using reference = typename StorageType::reference;
  using const_reference = typename StorageType::const_reference;
  using pointer = typename StorageType::pointer;
  using const_pointer = typename StorageType::const_pointer;
  using size_type = typename StorageType::size_type;
  using difference_type = typename StorageType::difference_type;
  using allocator_type = typename StorageType::allocator_type;
  using iterator = DequeIterator<Deque>;
  using const_iterator = ConstDequeIterator<Deque>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  static constexpr difference_type block_capacity_ = 16;

  Deque() { init(); };
  Deque(size_type count) : Deque(count, T()) {}
  Deque(size_type count, const T& v) : Deque() { insert(end(), count, v); }
  template <typename InputIt, typename Category = typename std::iterator_traits<InputIt>::iterator_category>
  Deque(InputIt first, InputIt last) : Deque() {
    emplace(end(), first, last);
  }
  Deque(std::initializer_list<T> il) : Deque() { emplace(end(), il.begin(), il.end()); }
  Deque(const Deque& other) : Deque() { emplace(end(), other.begin(), other.end()); }
  Deque(Deque&& other) = default;
  ~Deque() { release(); }

  Deque& operator=(const Deque& other) {
    assign(other);
    return *this;
  }
  Deque& operator=(Deque&& other) {
    assign(std::move(other));
    return *this;
  }
  Deque& operator=(std::initializer_list<T> il) {
    assign(il);
    return *this;
  }

  void assign(size_type count, const T& v) {}
  template <typename InputIt, typename Category = typename std::iterator_traits<InputIt>::iterator_category>
  void assign(InputIt first, InputIt last) {}
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
  size_type size() const { return d_.capacity() ? end() - begin() : 0; }
  bool empty() const { return size() == 0; }

  // Modifiers
  void clear() { erase(begin(), end()); }
  iterator insert(const_iterator before, const T& v) { return emplace(before, v); }
  iterator insert(const_iterator before, T&& v) { return emplace(before, std::move(v)); }
  iterator insert(const_iterator before, size_type count, const T& v) { return insert_n(before, count, v); }
  template <typename InputIt, typename Category = typename std::iterator_traits<InputIt>::iterator_category>
  iterator insert(const_iterator before, InputIt first, InputIt last) {
    return insert_range(before, first, last);
  }
  iterator insert(const_iterator before, std::initializer_list<T> il) {
    return insert_range(before, il.begin(), il.end(), il.size());
  }
  template <typename... Args>
  iterator emplace(const_iterator before, Args&&... args);
  template <typename... Args>
  reference emplace_back(Args&&... args) {
    emplace(end(), std::forward<Args>(args)...);
    return back();
  }
  void push_back(const T& v) { emplace_back(v); }
  void push_back(T&& v) { emplace_back(std::move(v)); }
  void pop_back() { erase(end() - 1); }
  template <typename... Args>
  reference emplace_front(Args&&... args) {
    emplace(begin(), std::forward<Args>(args)...);
    return front();
  }
  void push_front(const T& v) { emplace_front(v); }
  void push_front(T&& v) { emplace_front(std::move(v)); }
  void pop_front() { erase(begin()); }
  iterator erase(const_iterator pos) { return erase(pos, pos + 1); }
  iterator erase(const_iterator first, const_iterator last);
  void resize(size_type count) {}
  void resize(size_type count, const T& v) {}

 private:
  size_type end_block_capacity() const { return end_.block()->finish() - end_.curr(); }
  size_type begin_block_capacity() const { return begin_.curr() - begin_.block()->begin(); }
  void reserve_at_front(size_type count);
  void reserve_at_back(size_type count);
  void reallocate_storage_ptr(size_type count, bool add_at_front);
  size_type back_capacity() const {
    return (d_.finish() - end_.block()) * block_capacity_ - (end_.curr() - end_.block()->begin());
  }
  size_type front_capacity() const {
    return (begin_.block() - d_.begin()) * block_capacity_ + (begin_.curr() - begin_.block()->begin());
  }

  void init() {
    size_type n = block_capacity_;
    d_.allocate(n);
    gtl::construct_at(d_.begin() + n / 2, block_capacity_);
    begin_.set(d_.begin() + n / 2);
    end_.set(d_.begin() + n / 2);
  }
  void release() { clear(); }

 private:
  iterator begin_;
  iterator end_;
  StoragePtrType d_;
};  // class Deque

template <typename T>
template <typename... Args>
Deque<T>::iterator Deque<T>::emplace(const_iterator before, Args&&... args) {
  if (before == begin()) {
    reserve_at_front(1);
    --begin_;
    gtl::construct_at(begin_.curr(), std::forward<Args>(args)...);
    return begin();
  } else if (before == end()) {
    reserve_at_back(1);
    gtl::construct_at(end_.curr(), std::forward<Args>(args)...);
    ++end_;
    return end() - 1;
  }
  size_type left_count = before - begin();
  size_type right_count = end() - before;
  if (left_count <= right_count) {
    reserve_at_front(1);
  } else {
    reserve_at_back(1);
  }
  return end_;
}

template <typename T>
Deque<T>::iterator Deque<T>::erase(const_iterator first, const_iterator last) {
  if (first == last) {
    return iterator(last.block(), last.curr());
  }
  if (first == begin()) {
    gtl::destroy(first, last);
    begin_.CopyFrom(last);
    return begin();
  } else if (last == end()) {
    gtl::destroy(first, last);
    end_.CopyFrom(first);
    return end();
  }
  size_type left_count = first - begin();
  size_type right_count = end() - last;
  size_type count = last - first;
  if (left_count <= right_count) {
    auto new_begin = std::move_backward(cbegin(), first, iterator(last.block(), last.curr()));
    std::destroy(begin(), begin() + count);
    begin_.CopyFrom(new_begin);
    return iterator(last.block(), last.curr());
  } else {
    auto new_end = std::move(last, cend(), iterator(first.block(), first.curr()));
    std::destroy(end() - count, end());
    end_.CopyFrom(new_end);
    return iterator(first.block(), first.curr());
  }
}

template <typename T>
void Deque<T>::reserve_at_front(size_type count) {
  size_type n = begin_block_capacity();
  if (count <= n) {
    return;
  }
  n = front_capacity();
  printf("front_capacity %zu %zu\n", n, count);
  if (count > n) {
    count -= n;
    reallocate_storage_ptr(count / block_capacity_ + count % block_capacity_, true);
  }
  count = count / block_capacity_ + count % block_capacity_;
  auto it = begin_.block();
  printf("new block count %zu %zu %zu\n", size(), count, n);
  for (; count > 0; --count) {
    gtl::construct_at(--it, block_capacity_);
  }
}

template <typename T>
void Deque<T>::reserve_at_back(size_type count) {
  size_type n = end_block_capacity();
  if (count < n) {
    return;
  }
  n = back_capacity();
  if (count >= n) {
    reallocate_storage_ptr((count - n) / block_capacity_ + 1, false);
  }
  count = count / block_capacity_ + 1;
  auto it = end_.block();
  printf("new block count %zu %zu %zu\n", size(), count, n);
  for (; count > 0; --count) {
    gtl::construct_at(++it, block_capacity_);
  }
}

template <typename T>
void Deque<T>::reallocate_storage_ptr(size_type count, bool add_at_front) {
  StoragePtrType new_storage_ptr(d_.capacity() + count);
  auto first = begin_.block();
  auto last = end_.block() + 1;
  auto offset = (new_storage_ptr.capacity() - (last - first) - count) / 2;
  auto begin_block_offset = begin_.block() - d_.begin() + offset;
  auto end_block_offset = end_.block() - d_.begin() + offset;
  auto begin_offset = begin_.curr() - begin_.block()->begin();
  auto end_offset = end_.curr() - end_.block()->begin();
  if (add_at_front) {
    begin_block_offset += count;
    end_block_offset += count;
  }
  gtl::uninitialized_move(first, last, new_storage_ptr.begin() + begin_block_offset);
  begin_.set(new_storage_ptr.begin() + begin_block_offset, begin_offset);
  end_.set(new_storage_ptr.begin() + end_block_offset, end_offset);
  d_.swap(new_storage_ptr);
}

template <typename T>
using deque = Deque<T>;

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