#pragma once

#include <algorithm>
#include <iterator>

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
  typedef typename std::make_signed<size_type>::type difference_type;
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
  vector<T>& operator=(const vector<T>& other) {
    printf("%s\n", __FUNCTION__);
    if (this == &other) {
      return *this;
    }
    resize(other.size());
    std::copy(other.begin(), other.end(), begin());
    return *this;
  }
  vector<T>& operator=(vector<T>&& other) {
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
  vector<T>& operator=(std::initializer_list<T> init_list) {
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
  void push_back(const T& v) { insert(end(), v); }
  void push_back(T&& v) { insert(end(), std::move(v)); }
  iterator insert(const_iterator pos, size_type count, const T& v) {
    size_type insert_pos = pos - begin();
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
  iterator insert(const_iterator pos, const T& v) { return insert(pos, 1, v); }
  iterator insert(const_iterator pos, T&& v) {
    size_type insert_pos = pos - begin();
    if (size_ + 1 > cap_) {
      reserve(size_ + 1);
    }
    move_to(begin() + insert_pos, end(), 1);
    at(insert_pos) = std::move(v);
    size_++;
    return begin() + insert_pos;
  }
  iterator insert(const_iterator pos, std::initializer_list<T> init_list) {
    return insert(pos, init_list.begin(), init_list.end());
  }
  template <typename II, typename Category = typename std::iterator_traits<II>::iterator_category>
  iterator insert(const_iterator pos, II first, II last) {
    size_type count = std::distance(first, last);
    if (count == 0) {
      return pos;
    }
    size_type insert_pos = std::distance(begin(), pos);
    if (size_ + count > cap_) {
      reserve(size_ + count);
    }
    move_to(begin() + insert_pos, end(), count);
    std::copy(first, last, begin() + insert_pos);
    size_ += count;
    return begin() + insert_pos;
  }
  void push_front(const T& v) { insert(begin(), v); }
  void push_front(T&& v) { insert(begin(), std::move(v)); }
  void pop_back() { size_--; }
  void remove_at(size_type idx) { erase(begin() + idx); }
  void erase(const_iterator pos) { erase(pos, pos + 1); }
  void erase(const_iterator first, const_iterator last) {
    size_type erase_count = std::distance(first, last);
    move_to(const_cast<iterator>(last), end(), size_ - erase_count);
    size_ = new_size;
  }
  template <typename Args...>
  iterator emplace(const_iterator pos, Args&&... args) {
    
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
    auto* new_data = new T[next_cap];
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
  T* data_;
};

}  // namespace gtl