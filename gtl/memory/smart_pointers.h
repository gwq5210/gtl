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
struct AutoPtrRef {
  explicit AutoPtrRef(T* p) : ptr(p) {}
  T* ptr;
};

template <typename T>
class AutoPtr {
 public:
  using element_type = T;

  explicit AutoPtr(element_type* data = nullptr) : data_(data) {}
  AutoPtr(AutoPtr& other) : data_(other.release()) {}
  // 为了实现右值的初始化
  AutoPtr(AutoPtrRef<element_type> ref) : data_(ref.ptr) {}
  template <typename Y>
  AutoPtr(AutoPtr<Y>& other) : data_(other.release()) {}
  ~AutoPtr() { Destroy(); }

  AutoPtr& operator=(AutoPtr& other) {
    printf("calling AutoPtr::operator=(AutoPtr& other)\n");
    reset(other.release());
    return *this;
  }
  template <typename Y>
  AutoPtr& operator=(AutoPtr<Y>& other) {
    printf("calling AutoPtr::operator=(AutoPtr<Y>& other)\n");
    reset(other.release());
    return *this;
  }
  template <typename Y>
  AutoPtr& operator=(AutoPtrRef<Y> other) {
    printf("calling AutoPtr::operator=(AutoPtrRef<Y> other)\n");
    reset(other.ptr);
    return *this;
  }

  element_type* get() const { return data_; }
  element_type* operator->() const { return get(); }
  element_type& operator*() const { return *get(); }

  template <typename Y>
  operator AutoPtrRef<Y>() { return AutoPtrRef<Y>(release()); }
  template <typename Y>
  operator AutoPtr<Y>() { return AutoPtr<Y>(release()); }

  void reset(element_type* data = nullptr) {
    if (data != data_) {
      Destroy();
      data_ = data;
    }
  }
  element_type* release() {
    element_type* ret = data_;
    data_ = nullptr;
    return ret;
  }

 private:
  void Destroy() {
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

  explicit UniquePtr(pointer data = nullptr, deleter_type deleter = deleter_type()) : data_(data), deleter_(deleter) {}
  UniquePtr(const UniquePtr& other) = delete;
  UniquePtr(UniquePtr&& other) : data_(other.release()), deleter_(std::forward<deleter_type>(other.deleter_)) {}
  template <typename Y>
  UniquePtr(UniquePtr<Y>&& other) : data_(other.release()), deleter_(std::forward<deleter_type>(other.get_deleter())) {}
  ~UniquePtr() { Destroy(); }

  UniquePtr& operator=(const UniquePtr& other) = delete;
  UniquePtr& operator=(UniquePtr&& other) {
    reset(other.release());
    deleter_ = std::forward<deleter_type>(other.deleter_);
    return *this;
  }
  template <typename Y>
  UniquePtr& operator=(UniquePtr<Y>&& other) {
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
    if (data != data_) {
      Destroy();
      data_ = data;
    }
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
  void Destroy() {
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
  ~UniquePtr() { Destroy(); }

  UniquePtr& operator=(const UniquePtr& other) = delete;
  UniquePtr& operator=(UniquePtr&& other) {
    reset(other.release());
    deleter_ = std::forward<deleter_type>(other.deleter_);
    return *this;
  }
  template <typename Y>
  UniquePtr& operator=(UniquePtr<Y>&& other) {
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
    Destroy();
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
  void Destroy() {
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
  using element_type = typename std::remove_extent<T>::type;

  template <typename Y>
  friend class SharedPtr;

  explicit SharedPtr(element_type* p = nullptr) { UnsafeReset(p); }
  SharedPtr(const SharedPtr& other) : ref_count_(other.ref_count_), data_(other.data_) {
    if (data_) {
      assert(ref_count_);
      ref_count_->fetch_add(1);
    }
  }
  SharedPtr(SharedPtr&& other) : ref_count_(other.ref_count_), data_(other.data_) { other.UnsafeReset(); }
  template <typename Y>
  SharedPtr(const SharedPtr<Y>& other) : ref_count_(other.ref_count_), data_(other.data_) {
    if (data_) {
      assert(ref_count_);
      ref_count_->fetch_add(1);
    }
  }
  template <typename Y>
  SharedPtr(SharedPtr<Y>&& other) : ref_count_(other.ref_count_), data_(other.data_) { other.UnsafeReset(); }
  ~SharedPtr() { release(); }

  SharedPtr& operator=(const SharedPtr& other) {
    if (this != &other) {
      release();
      UnsafeCopy(other);
    }
    return *this;
  }
  SharedPtr& operator=(SharedPtr&& other) {
    release();
    ref_count_ = other.ref_count_;
    data_ = other.data_;
    other.UnsafeReset();
    return *this;
  }

  template <typename Y>
  SharedPtr& operator=(const SharedPtr<Y>& other) {
    if (this != &other) {
      release();
      UnsafeCopy(other);
    }
    return *this;
  }
  template <typename Y>
  SharedPtr& operator=(SharedPtr<Y>&& other) {
    release();
    ref_count_ = other.ref_count_;
    data_ = other.data_;
    other.UnsafeReset();
    return *this;
  }

  element_type* get() const { return data_; }
  element_type* operator->() const { return get(); }
  element_type& operator*() const { return *get(); }

  void reset(element_type* data = nullptr) {
    release();
    UnsafeReset(data);
  }

  element_type* release() {
    auto* ret = data_;
    if (data_) {
      assert(ref_count_);
      if (ref_count_->fetch_sub(1) == 1) {
        Destroy();
      }
    }
    return ret;
  }

  std::size_t use_count() const {
    return ref_count_ ? ref_count_->load() : 0;
  }

 private:
  void UnsafeReset(element_type* data = nullptr) {
    if (data) {
      ref_count_ = new std::atomic<std::size_t>(1);
      data_ = data;
    } else {
      ref_count_ = nullptr;
      data_ = nullptr;
    }
  }
  template <typename Y>
  void UnsafeCopy(const SharedPtr<Y>& other) {
    ref_count_ = other.ref_count_;
    data_ = other.data_;
    if (data_) {
      assert(ref_count_);
      ref_count_->fetch_add(1);
    }
  }
  void Destroy() {
    delete data_;
    delete ref_count_;
    data_ = nullptr;
    ref_count_ = nullptr;
  }

  std::atomic<std::size_t>* ref_count_;
  element_type* data_;
};

template <typename T, typename... Args>
AutoPtr<T> make_auto_ptr(Args&&... args) {
  return AutoPtr<T>(new T(std::forward<Args>(args)...));
}

template <typename T, typename... Args>
typename std::enable_if<!std::is_array<T>::value, UniquePtr<T>>::type make_unique(Args&&... args) {
  return UniquePtr<T>(new T(std::forward<Args>(args)...));
}

template <typename T>
typename std::enable_if<std::is_array<T>::value && std::extent<T>::value == 0, UniquePtr<T>>::type make_unique(std::size_t size) {
  using Element = typename std::remove_extent<T>::type;
  return UniquePtr<T>(new Element[size]());
}

template <typename T, typename... Args>
SharedPtr<T> make_shared(Args&&... args) {
  return SharedPtr<T>(new T(std::forward<Args>(args)...));
}

template <typename T>
using auto_ptr = AutoPtr<T>;

template <typename T, typename Deleter = std::default_delete<T>>
using unique_ptr = UniquePtr<T, Deleter>;

template <typename T>
using shared_ptr = SharedPtr<T>;

}  // namespace gtl