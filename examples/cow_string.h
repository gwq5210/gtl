/**
 * @file cow_string.h
 * @author gwq5210 (gwq5210@qq.com)
 * @brief 简单的copy on write字符串实现
 * @date 2021-11-06
 * 
 * @copyright Copyright (c) 2021. All rights reserved.
 */

#pragma once

#include <cstring>

#include "gtl/smart_pointers.h"

namespace gtl {

namespace examples {

class CowString {
 public:
  using size_type = std::size_t;

  CowString() : size_(0), capacity_(0) {}
  CowString(const char* str) : CowString() { Init(str); }
  CowString(const CowString& other) : size_(other.size_), capacity_(other.capacity_), str_(other.str_) {}
  CowString(CowString&& other) : size_(other.size_), capacity_(other.capacity_), str_(std::move(other.str_)) { other.UnsafeReset(); }
  ~CowString() {}

  CowString& operator=(const CowString& other) {
    printf("&= %zu %zu %s\n", other.size_, other.capacity_, other.cc_str());
    CowString(other).swap(*this);
    return *this;
  }
  CowString& operator=(CowString&& other) {
    printf("&&= %zu %zu %s\n", other.size_, other.capacity_, other.cc_str());
    CowString(std::move(other)).swap(*this);
    return *this;
  }

  size_type size() const { return size_; }
  size_type capacity() const { return capacity_; }
  const char* cdata() const { return c_str(); }
  const char* data() const { return c_str(); }
  char *data() { return c_str(); }
  const char* cc_str() const { return str_.get(); }
  const char* c_str() const { return str_.get(); }
  char* c_str() { return MutableData(); }
  const char& operator[](size_type index) const { return str_[index]; }

  void swap(CowString& other) {
    std::swap(size_, other.size_);
    std::swap(capacity_, other.capacity_);
    str_.swap(other.str_);
  }

 private:
  void Init(const char* str, size_type size = 0) {
    if (str) {
      size_ = size > 0 ? size : std::strlen(str);
      capacity_ = size_;
      str_ = gtl::make_shared<char[]>(size_ + 1);
      std::memcpy(str_.get(), str, size_ + 1);
    }
  }
  void UnsafeReset() {
    size_ = 0;
    capacity_ = 0;
    str_.reset(nullptr);
  }
  char* MutableData() {
    if (str_) {
      Init(str_.get(), size_);
    }
    return str_.get();
  }
  size_type size_;
  size_type capacity_;
  SharedPtr<char[]> str_;
};

} // namespace examples

}  // namespace gtl