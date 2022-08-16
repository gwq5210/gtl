/**
 * @file deque.h
 * @author gwq5210 (gwq5210@qq.com)
 * @brief deque的实现
 * @date 2021-05-16
 *
 * @copyright Copyright (c) 2021. All rights reserved.
 */

#pragma once

#include "gtl/algorithm/algorithm.h"
#include "gtl/iterator/iterator.h"
#include "gtl/container/storage.h"
#include "gtl/container/vector.h"
#include "gtl/logging.h"

namespace gtl {

template <typename DequeType>
class ConstDequeIterator {
 public:
  using iterator_category = std::random_access_iterator_tag;
  using reference = typename DequeType::const_reference;
  using pointer = typename DequeType::const_pointer;
  using difference_type = typename DequeType::difference_type;
  using value_type = typename DequeType::value_type;
  using size_type = typename DequeType::size_type;
  using BlockIterator = typename DequeType::BlockStorage::iterator;
  using ConstBlockIterator = typename DequeType::BlockStorage::const_iterator;
  using ValueIterator = typename DequeType::Block::iterator;
  using ConstValueIterator = typename DequeType::Block::const_iterator;
  using Self = ConstDequeIterator;

  ConstDequeIterator() : block_(), offset_() {}
  ConstDequeIterator(BlockIterator block, size_type offset) : block_(block), offset_(offset) {}
  ConstDequeIterator(const ConstDequeIterator& other) = default;
  ConstDequeIterator& operator=(const ConstDequeIterator& other) = default;
  ConstDequeIterator(ConstDequeIterator&& other) : block_(), offset_() { other.swap(*this); }
  void init() {
    block_ = nullptr;
    offset_ = 0;
  }
  void clear() { init(); }
  void swap(ConstDequeIterator& other) {
    gtl::swap(offset_, other.offset_);
    gtl::swap(block_, other.block_);
  }
  size_type offset() const { return offset_; }
  BlockIterator block() { return block_; }
  ConstBlockIterator block() const { return block_; }
  ValueIterator curr() { return block_->begin() + offset_; }
  ConstValueIterator curr() const { return block_->begin() + offset_; }
  void set_block(BlockIterator block) { block_ = block; }
  void set_offset(size_type offset) { offset_ = offset; }
  void set(BlockIterator block, size_type offset = 0) {
    block_ = block;
    offset_ = offset;
  }
  reference operator*() const { return block_->at(offset_); }
  pointer operator->() const { return std::pointer_traits<pointer>::pointer_to(**this); }
  Self& operator++() {
    ++offset_;
    if (offset_ == block()->capacity()) {
      ++block_;
      offset_ = 0;
    }
    return *this;
  }
  Self operator++(int) {
    auto ret = *this;
    operator++();
    return ret;
  }
  Self& operator--() {
    if (offset_ == 0) {
      --block_;
      offset_ = block()->capacity();
    }
    --offset_;
    return *this;
  }
  Self operator--(int) {
    auto ret = *this;
    operator--();
    return ret;
  }
  Self& operator+=(difference_type n) {
    if (n != 0) {
      if (n >= 0) {
        n += offset_;
        block_ += n / DequeType::block_capacity_;
        offset_ = n % DequeType::block_capacity_;
      } else {
        n = -n;
        n += (DequeType::block_capacity_ - 1) - offset_;
        block_ -= n / DequeType::block_capacity_;
        offset_ = (DequeType::block_capacity_ - 1) - n % DequeType::block_capacity_;
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
    return (block_ - other.block_) * DequeType::block_capacity_ + offset_ - other.offset_;
  }
  reference operator[](difference_type n) { return *(*this + n); }
  bool operator==(const ConstDequeIterator& other) const { return offset_ == other.offset_ && block_ == other.block_; }
  bool operator!=(const ConstDequeIterator& other) const { return !(*this == other); }
  bool operator<(const ConstDequeIterator& other) const {
    return block_ == other.block_ ? offset_ < other.offset_ : block_ < other.block_;
  }
  bool operator>(const ConstDequeIterator& other) const { return other < *this; }
  bool operator<=(const ConstDequeIterator& other) const { return !(*this > other); }
  bool operator>=(const ConstDequeIterator& other) const { return !(*this < other); }

 private:
  BlockIterator block_;
  size_type offset_;
};

template <typename DequeType>
class DequeIterator : public ConstDequeIterator<DequeType> {
 public:
  using Base = ConstDequeIterator<DequeType>;
  using reference = typename DequeType::reference;
  using pointer = typename DequeType::pointer;
  using difference_type = typename Base::difference_type;
  using size_type = typename Base::size_type;
  using BlockIterator = typename Base::BlockIterator;
  using ValueIterator = typename Base::ValueIterator;
  using Self = DequeIterator;
  DequeIterator() : Base() {}
  DequeIterator(BlockIterator block, size_type offset) : Base(block, offset) {}
  void CopyFrom(const Base& other) { Base::set(BlockIterator(other.block()), other.offset()); }
  reference operator*() { return Base::block()->at(Base::offset()); }
  pointer operator->() { return std::pointer_traits<pointer>::pointer_to(**this); }
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
  using Base::operator-;
  // or
  // difference_type operator-(const Base& other) const { return Base::operator-(other); }
  Self operator-(difference_type n) const {
    Self ret = *this;
    ret -= n;
    return ret;
  }
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
  static constexpr std::ptrdiff_t block_capacity_ = 16;

  using Block = UStorage<T, block_capacity_>;
  using BlockAllocator = std::allocator<Block>;
  using BlockStorage = UStorage<Block>;
  using value_type = T;
  using reference = T&;
  using const_reference = const T&;
  using pointer = T*;
  using const_pointer = const T*;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using allocator_type = typename Block::allocator_type;
  using iterator = DequeIterator<Deque>;
  using const_iterator = ConstDequeIterator<Deque>;
  using reverse_iterator = gtl::reverse_iterator<iterator>;
  using const_reverse_iterator = gtl::reverse_iterator<const_iterator>;

  Deque() = default;
  Deque(size_type count) { insert_n(end(), count, T()); }
  Deque(size_type count, const T& v) { insert_n(end(), count, v); }
  template <typename InputIt, typename Category = typename std::iterator_traits<InputIt>::iterator_category>
  Deque(InputIt first, InputIt last) {
    insert_range(end(), first, last);
  }
  Deque(std::initializer_list<T> il) { insert_range(end(), il.begin(), il.end(), il.size()); }
  Deque(const Deque& other) { insert_range(end(), other.begin(), other.end(), other.size()); }
  Deque(Deque&& other) { assign(std::move(other)); };
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

  void assign(size_type count, const T& v) { assign_n(count, v); }
  template <typename InputIt, typename Category = typename std::iterator_traits<InputIt>::iterator_category>
  void assign(InputIt first, InputIt last) {
    assign_range(first, last);
  }
  void assign(const Deque& other) {
    if (this != &other) {
      assign_range(other.begin(), other.end(), other.size());
    }
  }
  void assign(Deque&& other) {
    release();
    other.swap(*this);
  }
  void assign(std::initializer_list<T> il) { assign_range(il.begin(), il.end(), il.size()); }

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
  template <typename InputIt, typename Category = typename std::iterator_traits<InputIt>::iterator_category>
  iterator insert_range(const_iterator before, InputIt first, InputIt last, size_type count = 0);
  iterator insert_n(const_iterator before, size_type count, const T& v);
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
  void resize(size_type count) { resize(count, T()); }
  void resize(size_type count, const T& v) {
    if (count <= size()) {
      erase(begin() + count, end());
    } else {
      insert(end(), count - size(), v);
    }
  }
  void swap(Deque& other) {
    gtl::swap(begin_, other.begin_);
    gtl::swap(end_, other.end_);
    d_.swap(other.d_);
  }

 private:
  size_type end_block_capacity() const { return end_.block()->capacity() - end_.offset(); }
  size_type begin_block_capacity() const { return begin_.offset(); }
  void reserve_at_front(size_type count);
  void reserve_at_back(size_type count);
  void reallocate_block_storage(size_type block_count, bool add_at_front);
  size_type back_capacity() const { return (d_.finish() - end_.block()) * block_capacity_ - end_.offset(); }
  size_type front_capacity() const { return (begin_.block() - d_.begin()) * block_capacity_ + begin_.offset(); }
  template <typename InputIt, typename Category = typename std::iterator_traits<InputIt>::iterator_category>
  void assign_range(InputIt first, InputIt last, size_type count = 0) {
    count = count ? count : gtl::distance(first, last);
    if (count <= size()) {
      gtl::copy(first, last, begin());
      erase(begin() + count, end());
    } else {
      gtl::copy_n(first, size(), begin());
      insert_range(end(), gtl::next(first, size()), last, count - size());
    }
  }
  void assign_n(size_type count, const T& v) {
    if (count <= size()) {
      gtl::fill_n(begin(), count, v);
      erase(begin() + count, end());
    } else {
      gtl::fill_n(begin(), size(), v);
      insert_n(end(), count - size(), v);
    }
  }
  void init() {
    begin_.init();
    end_.init();
    d_.init();
  }
  void release() {
    clear();
    d_.release();
    begin_.clear();
    end_.clear();
  }
  BlockStorage alloc_block_storage(size_type size) { return BlockStorage(next2power(size)); }

 private:
  iterator begin_;
  iterator end_;
  BlockStorage d_;
};  // class Deque

template <typename T>
constexpr std::ptrdiff_t Deque<T>::block_capacity_;

template <typename T>
typename Deque<T>::iterator Deque<T>::insert_n(const_iterator before, size_type count, const T& v) {
  size_type pos = before - begin();
  if (count > 0) {
    if (before == begin()) {
      reserve_at_front(count);
      begin_ -= count;
      gtl::uninitialized_fill_n(begin(), count, v);
    } else if (before == end()) {
      reserve_at_front(count);
      gtl::uninitialized_fill_n(end(), count, v);
      end_ += count;
    } else {
      size_type right_count = end() - before;
      if (pos <= right_count) {
        reserve_at_front(count);
        auto new_begin = begin_ - count;
        if (pos <= count) {
          gtl::uninitialized_move(begin(), begin() + pos, new_begin);
          gtl::uninitialized_fill_n(new_begin + pos, count - pos, v);
          gtl::fill_n(begin(), pos, v);
        } else {
          gtl::uninitialized_move(begin(), begin() + count, new_begin);
          gtl::move(begin() + count, begin() + pos, new_begin + count);
          gtl::fill_n(begin() + pos - count, count, v);
        }
        begin_ = new_begin;
      } else {
        reserve_at_back(count);
        auto new_end = end() + count;
        if (right_count <= count) {
          gtl::uninitialized_move(end() - right_count, end(), new_end - right_count);
          gtl::fill_n(end() - right_count, right_count, v);
          gtl::uninitialized_fill_n(end(), count - right_count, v);
        } else {
          gtl::uninitialized_move(end() - count, end(), end());
          gtl::move(end() - right_count, end() - count, end() - right_count + count);
          gtl::fill_n(end() - right_count, count, v);
        }
        end_ = new_end;
      }
    }
  }
  return begin() + pos;
}

template <typename T>
template <typename InputIt, typename Category>
typename Deque<T>::iterator Deque<T>::insert_range(const_iterator before, InputIt first, InputIt last,
                                                   size_type count /* = 0*/) {
  size_type pos = before - begin();
  if (first != last) {
    count = count ? count : gtl::distance(first, last);
    if (before == begin()) {
      reserve_at_front(count);
      begin_ -= count;
      gtl::uninitialized_copy(first, last, begin());
    } else if (before == end()) {
      reserve_at_front(count);
      gtl::uninitialized_copy(first, last, end());
      end_ += count;
    } else {
      size_type right_count = end() - before;
      if (pos <= right_count) {
        reserve_at_front(count);
        auto new_begin = begin_ - count;
        if (pos <= count) {
          gtl::uninitialized_move(begin(), begin() + pos, new_begin);
          gtl::uninitialized_copy_n(first, count - pos, new_begin + pos);
          gtl::copy(gtl::next(first, count - pos), last, begin());
        } else {
          gtl::uninitialized_move(begin(), begin() + count, new_begin);
          gtl::move(begin() + count, begin() + pos, new_begin + count);
          gtl::copy(first, last, begin() + pos - count);
        }
        begin_ = new_begin;
      } else {
        reserve_at_back(count);
        auto new_end = end() + count;
        if (right_count <= count) {
          gtl::uninitialized_move(end() - right_count, end(), new_end - right_count);
          gtl::copy_n(first, right_count, end() - right_count);
          gtl::uninitialized_copy(gtl::next(first, right_count), last, end());
        } else {
          gtl::uninitialized_move(end() - count, end(), end());
          gtl::copy(end() - right_count, end() - count, end() - right_count + count);
          gtl::copy(first, last, end() - right_count);
        }
        end_ = new_end;
      }
    }
  }
  return begin() + pos;
}

template <typename T>
template <typename... Args>
typename Deque<T>::iterator Deque<T>::emplace(const_iterator before, Args&&... args) {
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
  size_type pos = before - begin();
  size_type right_count = end() - before;
  if (pos <= right_count) {
    reserve_at_front(1);
    T tmp(std::forward<Args>(args)...);
    auto it = begin() - 1;
    gtl::construct_at(it.curr(), std::move(front()));
    gtl::move(begin() + 1, begin() + pos + 1, begin());
    --begin_;
    at(pos) = std::move(tmp);
  } else {
    reserve_at_back(1);
    T tmp(std::forward<Args>(args)...);
    gtl::construct_at(end_.curr(), std::move(back()));
    gtl::move_backward(begin() + pos, end() - 1, end() - 1);
    at(pos) = std::move(tmp);
    ++end_;
  }
  return begin_ + pos;
}

template <typename T>
typename Deque<T>::iterator Deque<T>::erase(const_iterator first, const_iterator last) {
  if (first == last) {
    return iterator(last.block(), last.offset());
  }
  if (first == begin()) {
    gtl::destroy(first, last);
    gtl::destroy(first.block(), last.block());
    begin_.CopyFrom(last);
    return begin();
  } else if (last == end()) {
    gtl::destroy(first, last);
    gtl::destroy(first.block() + 1, last.block() + 1);
    end_.CopyFrom(first);
    return end();
  }
  size_type left_count = first - begin();
  size_type right_count = end() - last;
  if (left_count <= right_count) {
    auto new_begin = gtl::move_backward(cbegin(), first, iterator(last.block(), last.offset()));
    gtl::destroy(begin(), new_begin);
    gtl::destroy(begin_.block(), new_begin.block());
    begin_.CopyFrom(new_begin);
    return iterator(last.block(), last.offset());
  } else {
    auto new_end = gtl::move(last, cend(), iterator(first.block(), first.offset()));
    gtl::destroy(new_end, end());
    gtl::destroy(new_end.block() + 1, end_.block() + 1);
    end_.CopyFrom(new_end);
    return iterator(first.block(), first.offset());
  }
}

template <typename T>
void Deque<T>::reserve_at_front(size_type count) {
  size_type n = begin_block_capacity();
  if (count <= n) {
    return;
  }
  n = front_capacity();
  GTL_DEBUG("front_capacity {} {}", n, count);
  size_type block_count = count / block_capacity_ + (count % block_capacity_ || empty() ? 1 : 0);
  if (count > n) {
    count -= n;
    block_count = count / block_capacity_ + (count % block_capacity_ || empty() ? 1 : 0);
    reallocate_block_storage(block_count, true);
  }
  auto it = begin_.block();
  GTL_DEBUG("new block count {} {} {}", size(), block_count, n);
  for (; block_count > 0; --block_count) {
    gtl::construct_at(--it, block_capacity_);
  }
  GTL_DEBUG("reserve_at_front end");
}

template <typename T>
void Deque<T>::reserve_at_back(size_type count) {
  size_type n = end_block_capacity();
  if (count < n) {
    return;
  }
  n = back_capacity();
  GTL_DEBUG("back_capacity {} {}", n, count);
  size_type block_count = count / block_capacity_ + 1;
  if (count >= n) {
    count -= n;
    block_count = count / block_capacity_ + 1;
    reallocate_block_storage(block_count, false);
  }
  if (empty()) {
    --block_count;
  }
  auto it = end_.block();
  GTL_DEBUG("new block count {} {} {}", size(), block_count, n);
  for (; block_count > 0; --block_count) {
    gtl::construct_at(++it, block_capacity_);
  }
}

template <typename T>
void Deque<T>::reallocate_block_storage(size_type block_count, bool add_at_front) {
  BlockStorage new_block_storage = alloc_block_storage(d_.capacity() + block_count);
  size_type mid_offset = (new_block_storage.capacity() - (end_.block() - begin_.block() + 1) - block_count) / 2;
  size_type begin_block_offset = begin_.block() - d_.begin() + mid_offset;
  size_type end_block_offset = end_.block() - d_.begin() + mid_offset;
  if (add_at_front) {
    begin_block_offset += block_count;
    end_block_offset += block_count;
  }
  if (!empty()) {
    gtl::uninitialized_move(begin_.block(), end_.block() + 1, new_block_storage.begin() + begin_block_offset);
  } else {
    gtl::construct_at(new_block_storage.begin() + begin_block_offset, block_capacity_);
  }
  begin_.set_block(new_block_storage.begin() + begin_block_offset);
  end_.set_block(new_block_storage.begin() + end_block_offset);
  d_.swap(new_block_storage);
  GTL_DEBUG("offset {} {} {} {}", block_count, begin_block_offset, end_block_offset, d_.capacity());
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