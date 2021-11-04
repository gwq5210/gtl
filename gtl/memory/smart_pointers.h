/**
 * @file smart_pointers.h
 * @author gwq5210 (gwq5210@qq.com)
 * @brief 智能指针实现
 * @date 2021-11-04
 *
 * @copyright Copyright (c) 2021. All rights reserved.
 */

#pragma once

#include <atomic>
#include <memory>

#include "compressed_pair.h"

namespace gtl {

template <typename T>
class AutoPtr {
 public:
  using element_type = T;

  explicit AutoPtr(element_type* data = nullptr) : data_(data) {}
  AutoPtr(AutoPtr& other) : data_(other.release()) {}
  ~AutoPtr() { destroy(); }

  AutoPtr& operator=(AutoPtr& other) { reset(other.release()); return *this; }

  element_type* get() const { return data_; }
  element_type* operator->() const { return get(); }
  element_type& operator*() const { return *get(); }

  void reset(element_type* data = nullptr) {
    destroy();
    data_ = data;
  }
  element_type* release() {
    element_type* ret = data_;
    data_ = nullptr;
    return ret;
  }

 private:
  void destroy() {
    if (data_) {
      delete data_;
      data_ = nullptr;
    }
  }
  element_type* data_;
};

template <typename T, typename Deleter = std::default_delete<T>>
class UniquePtr {
 public:
  using pointer = T*;
  using element_type = T;
  using deleter_type = Deleter;

  explicit UniquePtr(pointer data = nullptr) : data_(data) {}
  UniquePtr(const UniquePtr& other) = delete;
  UniquePtr(UniquePtr&& other) : data_(other.release()), deleter_(std::forward<deleter_type>(other.deleter_)) {}
  ~UniquePtr() { destroy(); }

  UniquePtr& operator=(const UniquePtr& other) = delete;
  UniquePtr& operator=(UniquePtr&& other) {
    reset(other.release());
    deleter_ = std::forward<deleter_type>(other.deleter_);
    return *this;
  }

  pointer operator->() const { return get(); }
  element_type& operator*() const { return *get(); }

  explicit operator bool() const { return data_ != nullptr; }
  deleter_type& get_deleter() { return deleter_; }
  const deleter_type& get_deleter() const { return deleter_; }

  pointer get() const { return data_; }
  void reset(pointer data = nullptr) {
    destroy();
    data_ = data;
  }
  pointer release() {
    pointer ret = data_;
    data_ = nullptr;
    return ret;
  }

  void swap(UniquePtr& other) {
    std::swap(data_, other.data_);
    std::swap(deleter_, other.deleter_);
  }

 private:
  void destroy() {
    if (data_) {
      deleter_(data_);
      data_ = nullptr;
    }
  }
  pointer data_;
  deleter_type deleter_;
};

template <typename T, typename Deleter>
class UniquePtr<T[], Deleter> {
 public:
  using pointer = T*;
  using element_type = T;
  using deleter_type = Deleter;

  explicit UniquePtr(pointer data = nullptr) : data_(data) {}
  UniquePtr(const UniquePtr& other) = delete;
  UniquePtr(UniquePtr&& other) : data_(other.release()), deleter_(std::forward<deleter_type>(other.deleter_)) {}
  ~UniquePtr() { destroy(); }

  UniquePtr& operator=(const UniquePtr& other) = delete;
  UniquePtr& operator=(UniquePtr&& other) {
    reset(other.release());
    deleter_ = std::forward<deleter_type>(other.deleter_);
    return *this;
  }

  element_type& operator[](std::size_t index) const { return data_[index]; }

  explicit operator bool() const { return data_ != nullptr; }
  deleter_type& get_deleter() { return deleter_; }
  const deleter_type& get_deleter() const { return deleter_; }

  pointer get() const { return data_; }
  void reset(pointer data = nullptr) {
    destroy();
    data_ = data;
  }
  pointer release() {
    pointer ret = data_;
    data_ = nullptr;
    return ret;
  }

  void swap(UniquePtr& other) {
    std::swap(data_, other.data_);
    std::swap(deleter_, other.deleter_);
  }

 private:
  void destroy() {
    if (data_) {
      deleter_(data_);
      data_ = nullptr;
    }
  }
  pointer data_;
  deleter_type deleter_;
};

template <typename T>
class SharedPtr {
 public:

 private:
  
};

template <typename T, typename... Args>
AutoPtr<T> make_auto_ptr(Args&&... args) {
  return AutoPtr<T>(new T(std::forward<Args>(args)...));
}

template <typename T, typename... Args>
std::enable_if_t<!std::is_array_v<T>, UniquePtr<T>> make_unique(Args&&... args) {
  return UniquePtr<T>(new T(std::forward<Args>(args)...));
}

template <typename T>
std::enable_if_t<std::is_array_v<T> && std::extent_v<T> == 0, UniquePtr<T>> make_unique(std::size_t size) {
  return UniquePtr<T>(new std::remove_extent_t<T>[size]());
}

template <typename T>
using auto_ptr = AutoPtr<T>;

template <typename T, typename Deleter = std::default_delete<T>>
using unique_ptr = UniquePtr<T, Deleter>;

}  // namespace gtl