#pragma once

#include <algorithm>
#include <iterator>
#include <type_traits>

namespace gtl {

template <typename T, typename SizeType, typename Allocator = std::allocator<T>>
struct Storage : public Allocator {
  T* first;
  T* last;
  SizeType size;
  Storage() : first(nullptr), last(nullptr), size(0) {}
  Storage(SizeType n) : size(0) {
    first = this->allocate(n);
    last = first + n;
  }
  ~Storage() { release(); }
  void release() {
    if (first) {
      std::destroy(first, first + size);
      this->deallocate(first, last - first);
      first = last = nullptr;
    }
    size = 0;
  }
  void init() {
    first = nullptr;
    last = nullptr;
    size = 0;
  }
  Allocator get_allocator() const { return *this; }
};

template <typename T, typename... Args>
T* construct_at(T* p, Args&&... args) {
  new (p) T(std::forward<Args>(args)...);
  return p;
}

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
  typedef Storage<T, size_type, allocator_type> storage_type;
  static constexpr size_type min_capacity = 16;
  static constexpr size_type max_allocated_size = 32 * 1024 * 1024;
  static constexpr bool is_copyable = std::is_trivially_copyable<T>::value;
  static constexpr bool is_pass_by_value = is_copyable && sizeof(T) <= sizeof(void*);
  typedef typename std::conditional<is_pass_by_value, T, const T&>::type VT;

  // constructor
  Vector() = default;
  explicit Vector(size_type size, VT v = T()) {
    printf("%s\n", __FUNCTION__);
    resize(size, v);
  }
  template <typename II, typename Category = typename std::iterator_traits<II>::iterator_category>
  Vector(II first, II last) {
    printf("%s range\n", __FUNCTION__);
    resize(std::distance(first, last));
    std::copy(first, last, begin());
  }
  Vector(std::initializer_list<T> init_list) {
    printf("%s init_list %zu\n", __FUNCTION__, init_list.size());
    *this = init_list;
  }
  Vector(const Vector& other) { *this = other; }
  Vector(Vector&& other) { *this = std::move(other); }
  Vector& operator=(const Vector& other) { return assign(other); }
  Vector& operator=(Vector&& other) { return assign(std::move(other)); }
  Vector& operator=(std::initializer_list<T> init_list) { return assign(init_list); }
  Vector& assign(const Vector& other) {
    printf("%s\n", __FUNCTION__);
    if (this == &other) {
      return *this;
    }
    resize(other.size());
    std::copy(other.begin(), other.end(), begin());
    return *this;
  }
  Vector& assign(Vector&& other) {
    printf("%s move\n", __FUNCTION__);
    if (this == &other) {
      return *this;
    }

    d_.release();
    d_ = other.d_;
    other.d_.init();

    return *this;
  }
  Vector& assign(std::initializer_list<T> init_list) {
    printf("%s init_list\n", __FUNCTION__);
    resize(init_list.size());
    std::copy(init_list.begin(), init_list.end(), begin());
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
  T* data() { return d_.first; }
  const T* data() const { return data(); }

  // iterators
  iterator begin() { return d_.first; }
  const_iterator begin() const { return begin(); }
  const_iterator cbegin() const { return begin(); }
  iterator end() { return d_.first + d_.size; }
  const_iterator end() const { return end(); }
  const_iterator cend() const { return end(); }
  reverse_iterator rbegin() { return reverse_iterator(end()); }
  const_reverse_iterator rbegin() const { return reverse_iterator(end()); }
  const_reverse_iterator crbegin() const { return reverse_iterator(end()); }
  reverse_iterator rend() { return reverse_iterator(begin()); }
  const_reverse_iterator rend() const { return reverse_iterator(begin()); }
  const_reverse_iterator crend() const { return reverse_iterator(begin()); }

  // capacity
  size_type capacity() const { return size_type(d_.last - d_.first); }
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
    if (d_.size + count > capacity()) {
      reserve(d_.size + count);
    }
    if (n <= count) {
      uninitialized_move(end() - count, end(), end());
      move_backward(begin() + insert_pos, begin() + n - count, begin() + n - count);
      fill_n(begin() + insert_pos, count, v);
    } else {
      uninitialized_move(end() - n, end(), end() + count - n);
      fill_n(begin() + insert_pos, n, v);
      uninitialized_fill_n(end(), count - n, v);
    }
    d_.size += count;
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
    if (d_.size + count > capacity()) {
      reserve(d_.size + count);
    }
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
    return begin() + insert_pos;
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
    std::move(first, last, iterator(first));
    std::destroy(iterator(last), end());
    d_.size -= last - first;
  }
  template <typename... Args>
  iterator emplace(const_iterator before, Args&&... args) {
    size_type insert_pos = before - begin();
    bool at_end = before == end();
    if (d_.size + 1 > capacity()) {
      reserve(d_.size + 1);
    }
    if (at_end) {
      construct_at(end(), std::forward<Args>(args)...);
    } else {
      std::uninitialized_move(end() - 1, end(), end());
      std::move_backward(begin() + insert_pos, end() - 1, begin() + insert_pos + 1);
      T tmp(std::forward<Args>(args)...);
      *(begin() + insert_pos) = std::move(tmp);
    }
    d_.size++;
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
  void resize(size_type new_size, VT v = T()) {
    reserve(new_size);
    if (new_size > d_.size) {
      // printf("%zu %zu %zu\n", new_size, d_.size, new_size - d_.size);
      // std::uninitialized_fill_n(end(), new_size - d_.size, v);
    }
    d_.size = new_size;
  }
  void clear() { erase(begin(), end()); }
  void swap(Vector& other) {
    if (this != &other) {
      std::swap(d_, other.d_);
    }
  }

 private:
  size_type next_capacity(size_type new_size) {
    size_type next_cap = std::max(capacity(), min_capacity);
    while (next_cap < new_size) {
      next_cap *= 2;
    }
    return next_cap;
  }
  void grow(size_type new_size) {
    if (capacity() >= new_size) {
      return;
    }
    size_type new_capacity = next_capacity(new_size);
    printf("grow capacity new capacity: %zu/%zu\n", capacity(), new_capacity);
    storage_type new_storage(new_capacity);
    std::uninitialized_copy(begin(), end(), new_storage.first);
    new_storage.size = d_.size;
    std::swap(d_, new_storage);
  }

  storage_type d_;
};

}  // namespace gtl