#pragma once

#include <algorithm>
#include <iterator>
#include <vector>

namespace gtl {

template <class T>
class vector {
 public:
  typedef T value_type;
  typedef T& reference;
  typedef const T& const_reference;
  typedef T* pointer;
  typedef const T* const_pointer;
  typedef pointer iterator;
  typedef const_pointer const_iterator;
  typedef std::reverse_iterator<iterator> reverse_iterator;
  typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
  typedef size_t size_type;
  static const size_type min_capacity = 16;
  static const size_type max_allocated_size = 32 * 1024 * 1024;
  vector(size_t size = 0, const_reference v = value_type()) {
    printf("%s\n", __FUNCTION__);
    init();
    resize(size, v);
  }
  template <class II>
  vector(II first, II last) {
    printf("%s range\n", __FUNCTION__);
    init();
    resize(last - first);
    std::copy(first, last, begin());
  }
  vector(const std::initializer_list<value_type>& init_list) {
    printf("%s init_list\n", __FUNCTION__);
    init();
    resize(init_list.size());
    std::copy(init_list.begin(), init_list.end(), begin());
  }
  vector(const vector<value_type>& other) { *this = other; }
  vector(vector<value_type>&& other) { *this = other; }
  vector<value_type>& operator=(const vector<value_type>& other) {
    printf("%s\n", __FUNCTION__);
    if (this == &other) {
      return *this;
    }
    resize(other.size());
    std::copy(other.begin(), other.end(), begin());
    return *this;
  }
  vector<value_type>& operator=(vector<value_type>&& other) {
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
  vector<value_type>& operator=(std::initializer_list<value_type>& init_list) {
    printf("%s init_list\n", __FUNCTION__);
    resize(init_list.size());
    std::copy(init_list.begin(), init_list.end(), begin());
    return *this;
  }
  ~vector() { destory(); }

  // element access
  reference at(size_type i) { return *(begin() + i); }
  const_reference at(size_type i) const { return *(begin() + i); }
  reference front() { return *begin(); }
  const_reference front() const { return *begin(); }
  reference back() { return *(end() - 1); }
  const_reference back() const { return *(end() - 1); }
  reference operator[](size_type i) { return *(begin() + i); }
  const_reference operator[](size_type i) const { return *(begin() + i); }
  pointer data() { return data_; }
  const_pointer data() const { return data_; }

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
  void reserve(size_t new_capacity) { grow(new_capacity); }

  // modifiers
  void push_back(const_reference v) { insert(end(), v); }
  void push_back(T&& v) { insert(end(), v); }
  void insert(const_iterator pos, const_reference v) {
    auto insert_pos = const_cast<iterator>(pos);
    reserve(++size_);
    move_to(insert_pos, end() - 1, 1);
    *insert_pos = v;
  }
  void insert(const_iterator pos, T&& v) {
    auto insert_pos = const_cast<iterator>(pos);
    reserve(++size_);
    move_to(insert_pos, end() - 1, 1);
    *insert_pos = std::move(v);
  }
  void push_front(const_reference v) { insert(begin(), v); }
  void pop_back() { size_--; }
  void erase(const_iterator pos) { erase(pos, pos + 1); }
  void erase(const_iterator first, const_iterator last) {
    size_t new_size = size_ - (last - first);
    move_to(const_cast<iterator>(last), end(), first - last);
    size_ = new_size;
  }
  void resize(size_type new_size, const_reference v = value_type()) {
    reserve(new_size);
    if (new_size > size_) {
      assign_range(end(), end() + new_size - size_, v);
    }
    size_ = new_size;
  }
  void clear() { size_ = 0; }

 private:
  template <class Iterator>
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
  void assign_range(iterator first, iterator last, const_reference v) {
    while (first != last) {
      *first++ = v;
    }
  }
  void grow(size_t new_capacity) {
    if (cap_ >= new_capacity) {
      return;
    }
    size_t next_cap = std::max(cap_, min_capacity);
    while (next_cap < new_capacity) {
      next_cap *= 2;
    }
    auto* new_data = new value_type[next_cap];
    std::copy(begin(), end(), new_data);
    cap_ = next_cap;
    delete[] data_;
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
  value_type* data_;
};

}  // namespace gtl