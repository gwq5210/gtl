#prama once

namespace gtl {

template <class T>
class vector {
 public:
  typedef T value_type;
  typedef T& const_reference;
  typedef const T& const_reference;
  typedef size_t size_type;
  static const int initial_capacity = 16;
  vector() : cap_(initial_capacity), size_(0), data_(nullptr) { data_ = new value_type(cap_); }
  ~vector() { delete[] data_; }
  size_type capacity() const { return cap_; }
  size_type size() const { return size_; }
  bool is_empty() const { return size_ == 0; }
  reference front() { return data_[0]; }
  const_reference front() const { return data_[0]; }
  reference back() { return data_[size_ - 1]; }
  const_reference back() const { return data_[size_ - 1]; }
  reference at(size_type i) { return data_[i]; }
  const_reference at(size_type i) const { return data_[i]; }
  void push_back(const_reference v) {}
  void push_back(T&& v) {}
  void insert(size_type i, const_reference v) {}
  void insert(size_type i, T&& v) {}
  void push_front(const_reference v) { insert(0, v); }
  void pop_back() { size_--; }

 private:
  size_type cap_;
  size_type size_;
  value_type* data_;
};

}  // namespace
   // gtl