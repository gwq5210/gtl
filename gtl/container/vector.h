#pragma once

#include <algorithm>
#include <iterator>
#include <type_traits>

namespace gtl {

template <typename T>
class vector {
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
  static constexpr size_type min_capacity = 16;
  static constexpr size_type max_allocated_size = 32 * 1024 * 1024;
  static constexpr bool is_copyable = std::is_trivially_copyable<T>::value;
  static constexpr bool is_pass_by_value = is_copyable && sizeof(T) <= 16;
  typedef typename std::conditional<is_pass_by_value, T, const T&>::type VT;

  // constructor
  vector() { init(); }
  explicit vector(size_type size, VT v = T()) {
    printf("%s\n", __FUNCTION__);
    init();
    resize(size, v);
  }
  template <typename II, typename Category = typename std::iterator_traits<II>::iterator_category>
  vector(II first, II last) {
    printf("%s range\n", __FUNCTION__);
    init();
    resize(std::distance(first, last));
    std::copy(first, last, begin());
  }
  vector(std::initializer_list<T> init_list) {
    printf("%s init_list %zu\n", __FUNCTION__, init_list.size());
    init();
    *this = init_list;
  }
  vector(const vector<T>& other) {
    init();
    *this = other;
  }
  vector(vector<T>&& other) {
    init();
    *this = std::move(other);
  }
  vector<T>& operator=(const vector<T>& other) { return assign(other); }
  vector<T>& operator=(vector<T>&& other) { return assign(std::move(other)); }
  vector<T>& operator=(std::initializer_list<T> init_list) { return assign(init_list); }
  vector<T>& assign(const vector<T>& other) {
    printf("%s\n", __FUNCTION__);
    if (this == &other) {
      return *this;
    }
    resize(other.size());
    std::copy(other.begin(), other.end(), begin());
    return *this;
  }
  vector<T>& assign(vector<T>&& other) {
    printf("%s move\n", __FUNCTION__);
    if (this == &other) {
      return *this;
    }

    destory();
    size_ = other.size_;
    cap_ = other.cap_;
    data_ = other.data_;
    other.init();

    return *this;
  }
  vector<T>& assign(std::initializer_list<T> init_list) {
    printf("%s init_list\n", __FUNCTION__);
    resize(init_list.size());
    std::copy(init_list.begin(), init_list.end(), begin());
    return *this;
  }
  ~vector() { destory(); }

  // element access
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

  // iterators
  iterator begin() { return data_; }
  const_iterator begin() const { return data_; }
  const_iterator cbegin() const { return data_; }
  iterator end() { return data_ + size_; }
  const_iterator end() const { return data_ + size_; }
  const_iterator cend() const { return data_ + size_; }
  reverse_iterator rbegin() { return reverse_iterator(end()); }
  const_reverse_iterator rbegin() const { return reverse_iterator(end()); }
  const_reverse_iterator crbegin() const { return reverse_iterator(end()); }
  reverse_iterator rend() { return reverse_iterator(begin()); }
  const_reverse_iterator rend() const { return reverse_iterator(begin()); }
  const_reverse_iterator crend() const { return reverse_iterator(begin()); }

  // capacity
  size_type capacity() const { return cap_; }
  size_type size() const { return size_; }
  bool empty() const { return size_ == 0; }
  void reserve(size_type new_capacity) { grow(new_capacity); }

  // modifiers
  void push_back(const T& v) { emplace_back(v); }
  void push_back(T&& v) { emplace_back(std::move(v)); }
  iterator insert(const_iterator before, size_type count, const T& v) {
    size_type insert_pos = before - begin();
    if (count == 0) {
      return begin() + insert_pos;
    }
    if (size_ + count > cap_) {
      reserve(size_ + count);
    }
    move_to(begin() + insert_pos, end(), count);
    size_ += count;
    for (int i = 0; i < count; i++) {
      at(insert_pos + i) = v;
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
    vector<T> tmp_vec(first, last);
    size_type count = std::distance(first, last);
    size_type insert_pos = before - begin();
    if (count == 0) {
      return begin() + insert_pos;
    }
    if (size_ + count > cap_) {
      reserve(size_ + count);
    }
    move_to(begin() + insert_pos, end(), count);
    std::copy(tmp_vec.begin(), tmp_vec.end(), begin() + insert_pos);
    size_ += count;
    return begin() + insert_pos;
  }
  template <typename II, typename Category = typename std::iterator_traits<II>::iterator_category>
  iterator insert(size_type idx, II first, II last) {
    return insert(begin() + idx, first, last);
  }
  void push_front(const T& v) { emplace(begin(), v); }
  void push_front(T&& v) { emplace(begin(), std::move(v)); }
  void pop_back() { size_--; }
  void remove_at(size_type idx) { erase(begin() + idx); }
  void erase(size_type idx) { erase(begin() + idx); }
  void erase(size_type first, size_type last) { erase(begin() + first, begin() + last); }
  void erase(const_iterator pos) { erase(pos, pos + 1); }
  void erase(const_iterator first, const_iterator last) {
    size_type erase_count = std::distance(first, last);
    move_to(const_cast<iterator>(last), end(), -erase_count);
    size_ -= erase_count;
  }
  template <typename... Args>
  iterator emplace(const_iterator before, Args&&... args) {
    size_type insert_pos = before - begin();
    if (size_ + 1 > cap_) {
      reserve(size_ + 1);
    }
    move_to(begin() + insert_pos, end(), 1);
    T tmp(std::forward<Args>(args)...);
    at(insert_pos) = std::move(tmp);
    size_++;
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
    if (new_size > size_) {
      assign_range(end(), end() + new_size - size_, v);
    }
    size_ = new_size;
  }
  void clear() { size_ = 0; }
  void swap(vector<T>& other) {
    if (this != &other) {
      std::swap(data_, other.data_);
      std::swap(size_, other.size_);
      std::swap(cap_, other.cap_);
    }
  }

 private:
  template <typename Iterator>
  void move_to(Iterator first, Iterator last, int k) {
    if (k == 0) {
      return;
    } else if (k > 0) {
      return move_to(reverse_iterator(last), reverse_iterator(first), -k);
    }
    k = -k;
    auto begin = first;
    auto end = last;
    while (begin != end) {
      *(begin - k) = std::move(*begin);
      begin++;
    }
  }
  void assign_range(iterator first, iterator last, VT v) {
    while (first != last) {
      *first++ = v;
    }
  }
  void grow(size_type new_capacity) {
    if (cap_ >= new_capacity) {
      return;
    }
    size_type next_cap = std::max(cap_, min_capacity);
    while (next_cap < new_capacity) {
      next_cap *= 2;
    }
    printf("grow capacity new capacity: %zu/%zu\n", cap_, next_cap);
    auto* new_data = new T[next_cap];
    std::copy(begin(), end(), new_data);
    cap_ = next_cap;
    delete[] data_;
    for (int i = 0; i < size_; i++) {
      int tmp = 2 * i;
      memcpy(&data_[i], &tmp, sizeof(int));
    }
    data_ = new_data;
  }
  void destory() {
    if (data_) {
      delete[] data_;
      data_ = nullptr;
    }
    size_ = 0;
    cap_ = 0;
  }
  void init() {
    size_ = 0;
    cap_ = 0;
    data_ = nullptr;
  }

  size_type cap_;
  size_type size_;
  T* data_;
};

}  // namespace gtl