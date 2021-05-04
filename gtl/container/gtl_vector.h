#pragma once

#include "gtl_memory.h"

#include <algorithm>
#include <iterator>
#include <type_traits>

namespace gtl {

template <typename T, typename SizeType = ptrdiff_t, typename Allocator = std::allocator<T>>
struct Storage : public Allocator {
  static constexpr SizeType min_capacity = 16;
  Storage() : begin(nullptr), end(nullptr), size(0) {}
  Storage(SizeType n) : size(0) {
    allocate(n);
    printf("allocate %zu %zu %p\n", n, capacity(), begin);
  }
  ~Storage() { release(); }
  SizeType capacity() const { return SizeType(end - begin); }
  bool full(SizeType n) const { return size + n > capacity(); }
  SizeType next_capacity(SizeType new_size) {
    SizeType next_cap = 1;
    while (next_cap < new_size) {
      next_cap <<= 1;
    }
    next_cap = std::max(next_cap, min_capacity);
    printf("next_cap %zu %zu\n", new_size, next_cap);
    return next_cap;
  }
  void allocate(SizeType n) {
    n = next_capacity(n);
    begin = Allocator::allocate(n);
    end = begin + n;
  }
  void allocate_fill(SizeType n, SizeType count, const T& v) {
    if (n) {
      allocate(n);
      std::uninitialized_fill_n(begin, count, v);
      size = count;
    }
  }
  template<typename II>
  void allocate_copy(SizeType n, II first, II last) {
    if (n) {
      allocate(n);
      std::uninitialized_copy(first, last, begin);
      size = std::distance(first, last);
    }
  }
  template<typename II>
  void allocate_move(SizeType n, II first, II last) {
    if (n) {
      allocate(n);
      std::uninitialized_move(first, last, begin);
      size = std::distance(first, last);
    }
  }
  void release() {
    if (begin) {
      std::destroy(begin, begin + size);
      // printf("deallocate %zu %p\n", end - begin, begin);
      Allocator::deallocate(begin, end - begin);
      begin = end = nullptr;
    }
    size = 0;
  }
  void swap(Storage& other) {
    std::swap(begin, other.begin);
    std::swap(end, other.end);
    std::swap(size, other.size);
  }
  void init() {
    begin = nullptr;
    end = nullptr;
    size = 0;
  }
  Allocator get_allocator() const { return *this; }
  T* begin;
  T* end;
  SizeType size;
};

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
  typedef Storage<T, size_type, allocator_type> Storage;

  // constructor
  Vector() = default;
  explicit Vector(size_type size, const T& v = T()) {
    printf("%s\n", __FUNCTION__);
    d_.allocate_fill(size, size, v);
  }
  template <typename II, typename Category = typename std::iterator_traits<II>::iterator_category>
  Vector(II first, II last) {
    printf("%s range\n", __FUNCTION__);
    d_.allocate_copy(std::distance(first, last), first, last);
  }
  Vector(std::initializer_list<T> init_list) {
    printf("%s init_list %zu\n", __FUNCTION__, init_list.size());
    d_.allocate_copy(init_list.size(), init_list.begin(), init_list.end());
  }
  Vector(const Vector& other) { assign(other); }
  Vector(Vector&& other) { assign(std::move(other)); }
  Vector& operator=(const Vector& other) { return assign(other); }
  Vector& operator=(Vector&& other) { return assign(std::move(other)); }
  Vector& operator=(std::initializer_list<T> init_list) { return assign(init_list); }
  Vector& assign(const Vector& other) {
    printf("%s\n", __FUNCTION__);
    if (this == &other) {
      return *this;
    }
    assign_range(other.begin(), other.end());
    return *this;
  }
  Vector& assign(Vector&& other) {
    printf("%s move\n", __FUNCTION__);
    if (this == &other) {
      return *this;
    }

    d_.release();
    d_.swap(other.d_);

    return *this;
  }
  Vector& assign(std::initializer_list<T> init_list) {
    printf("%s init_list\n", __FUNCTION__);
    assign_range(init_list.begin(), init_list.end());
    return *this;
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
  T* data() { return d_.begin; }
  const T* data() const { return d_.begin; }

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
  size_type size() const { return d_.size; }
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
      d_.size += count;
    } else {
      Storage new_storage;
      new_storage.allocate_move(d_.size + count, begin(), end() - n);
      std::uninitialized_fill_n(new_storage.begin + new_storage.size, count, v);
      new_storage.size += count;
      std::uninitialized_move(end() - n, end(), new_storage.begin + new_storage.size);
      new_storage.size += n;
      d_.swap(new_storage);
    }
    return begin() + insert_pos;
  }
  iterator insert(size_type idx, size_type count, const T& v) {
    return insert(begin() + idx, count, v);
  }
  iterator insert(const_iterator before, const T& v) { return emplace(before, v); }
  iterator insert(size_type idx, const T& v) { return emplace(begin() + idx, v); }
  iterator insert(const_iterator before, T&& v) { return emplace(before, std::move(v)); }
  iterator insert(size_type idx, T&& v) { return emplace(begin() + idx, std::move(v)); }
  iterator insert(const_iterator before, std::initializer_list<T> init_list) {
    return insert(before, init_list.begin(), init_list.end());
  }
  iterator insert(size_type idx, std::initializer_list<T> init_list) {
    return insert(begin() + idx, init_list.begin(), init_list.end());
  }
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
      d_.size += count;
    } else {
      Storage new_storage;
      new_storage.allocate_move(d_.size + count, begin(), end() - n);
      std::uninitialized_copy(first, last, new_storage.begin + new_storage.size);
      new_storage.size += count;
      std::uninitialized_move(end() - n, end(), new_storage.begin + new_storage.size);
      new_storage.size += n;
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
  void remove_at(size_type idx) { erase(begin() + idx); }
  void erase(size_type idx) { erase(begin() + idx); }
  void erase(size_type first, size_type last) { erase(begin() + first, begin() + last); }
  void erase(const_iterator pos) { erase(pos, pos + 1); }
  void erase(const_iterator first, const_iterator last) {
    if (first != last) {
      std::move(last, cend(), iterator(first));
      std::destroy(iterator(last), end());
      d_.size -= last - first;
    }
  }
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
      d_.size++;
    } else {
      Storage new_storage;
      new_storage.allocate_move(d_.size + 1, begin(), begin() + insert_pos);
      T tmp(std::forward<Args>(args)...);
      *(new_storage.begin + new_storage.size) = std::move(tmp);
      new_storage.size++;
      if (!at_end) {
        std::uninitialized_move(begin() + insert_pos, end(), new_storage.begin + new_storage.size);
        new_storage.size += end() - (begin() + insert_pos);
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
  template <typename II>
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
      d_.size += count - n;
    } else {
      Storage new_storage;
      new_storage.allocate_move(d_.size + count - n, begin(), iterator(pos));
      std::uninitialized_copy(first, last, d_.begin + d_.size);
      new_storage.size += count;
      d_.swap(new_storage);
    }
    return end() - n;
  }
  void resize(size_type new_size, const T& v = T()) {
    if (new_size > d_.size) {
      insert(end(), new_size - d_.size, v);
    } else if (new_size < d_.size) {
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
  void assign_range(II first, II last) {
    size_type count = std::distance(first, last);
    if (count < d_.size) {
      erase(begin() + count, end());
    }
    replace(begin(), first, last);
  }
  void grow(size_type new_capacity) {
    if (capacity() >= new_capacity) {
      return;
    }
    Storage new_storage;
    new_storage.allocate_move(new_capacity, begin(), end());
    printf("grow capacity new capacity: %zu/%zu, ptr: %p/%p\n", capacity(), new_storage.capacity(),
           new_storage.begin, d_.begin);
    d_.swap(new_storage);
  }

  Storage d_;
};

}  // namespace gtl