/**
 * @file smart_pointers.h
 * @author gwq5210 (gwq5210@qq.com)
 * @brief 智能指针实现
 * @date 2021-11-04
 *
 * @copyright Copyright (c) 2021. All rights reserved.
 */

#pragma once

#include <cassert>

#include <atomic>
#include <memory>

#include "gtl/container/compressed_pair.h"

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
  operator AutoPtrRef<Y>() {
    return AutoPtrRef<Y>(release());
  }
  template <typename Y>
  operator AutoPtr<Y>() {
    return AutoPtr<Y>(release());
  }

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

struct RefCount {
  RefCount() : use_count(1), weak_count(1) {}
  void IncUseCount() { ++use_count; }
  void IncWeakCount() { ++weak_count; }
  std::size_t DecUseCount() { return --use_count; }
  std::size_t DecWeakCount() { return --weak_count; }
  std::atomic<std::size_t> use_count;
  std::atomic<std::size_t> weak_count;
};

template <typename T>
class PtrBase {
 public:
  using element_type = typename std::remove_extent<T>::type;

  template <typename Y>
  friend class PtrBase;

  PtrBase(element_type* data = nullptr) : ref_count_(nullptr), data_(nullptr) { UnsafeReset(data); }
  PtrBase(const PtrBase& other) = default;
  template <typename Y>
  PtrBase(const PtrBase<Y>& other) : ref_count_(other.ref_count_), data_(other.data_) {}
  PtrBase(PtrBase&& other) : ref_count_(other.ref_count_), data_(other.data_) { other.UnsafeReset(); }
  template <typename Y>
  PtrBase(PtrBase<Y>&& other) : ref_count_(other.ref_count_), data_(other.data_) {
    other.UnsafeReset();
  }
  ~PtrBase() {}

  PtrBase& operator=(const PtrBase& other) = default;
  PtrBase& operator=(PtrBase&& other) {
    PtrBase(std::move(other)).swap(*this);
    return *this;
  }

  void DecUseCount() {
    if (ref_count_) {
      if (ref_count_->DecUseCount() == 0) {
        DeleteData();
        DecWeakCount();
      }
    }
  }
  void IncUseCount() {
    if (ref_count_) {
      ref_count_->IncUseCount();
    }
  }
  void DecWeakCount() {
    if (ref_count_) {
      if (ref_count_->DecWeakCount() == 0) {
        DeleteRefCount();
      }
    }
  }
  void IncWeakCount() {
    if (ref_count_) {
      ref_count_->IncWeakCount();
    }
  }
  void UnsafeReset(element_type* data = nullptr) {
    if (data) {
      ref_count_ = new RefCount();
      data_ = data;
    } else {
      data_ = nullptr;
      ref_count_ = nullptr;
    }
  }

  void DeleteData() {
    std::default_delete<T>()(data_);
    printf("delete data %p\n", data_);
    data_ = nullptr;
  }

  void DeleteRefCount() {
    delete ref_count_;
    ref_count_ = nullptr;
  }

  void swap(PtrBase& other) {
    std::swap(ref_count_, other.ref_count_);
    std::swap(data_, other.data_);
  }

  element_type* get() const { return data_; }

  std::size_t UseCount() const { return ref_count_ ? ref_count_->use_count.load() : 0; }
  std::size_t Weakcount() const { return ref_count_ ? ref_count_->weak_count.load() : 0; }

 public:
  RefCount* ref_count_;
  element_type* data_;
};

template <typename T>
class SharedPtr {
 public:
  using element_type = typename PtrBase<T>::element_type;

  template <typename Y>
  friend class SharedPtr;
  template <typename Y>
  friend class WeakPtr;

  explicit SharedPtr(element_type* p = nullptr) : ptr_base_(p) {}
  SharedPtr(const SharedPtr& other) : ptr_base_(other.ptr_base_) { ptr_base_.IncUseCount(); }
  SharedPtr(SharedPtr&& other) = default;
  template <typename Y>
  SharedPtr(const SharedPtr<Y>& other) : ptr_base_(other.ptr_base_) {
    ptr_base_.IncUseCount();
  }
  template <typename Y>
  SharedPtr(SharedPtr<Y>&& other) : ptr_base_(std::move(other.ptr_base_)) {}
  ~SharedPtr() { ptr_base_.DecUseCount(); }

  SharedPtr& operator=(const SharedPtr& other) {
    SharedPtr(other).swap(*this);
    return *this;
  }
  SharedPtr& operator=(SharedPtr&& other) {
    SharedPtr(std::move(other)).swap(*this);
    return *this;
  }

  template <typename Y>
  SharedPtr& operator=(const SharedPtr<Y>& other) {
    SharedPtr(other).swap(*this);
    return *this;
  }
  template <typename Y>
  SharedPtr& operator=(SharedPtr<Y>&& other) {
    SharedPtr(std::move(other)).swap(*this);
    return *this;
  }

  element_type* get() const { return ptr_base_.get(); }
  template <typename Ty = T>
  typename std::enable_if<!std::is_array<Ty>::value, element_type*>::type operator->() const {
    return get();
  }
  template <typename Ty = T>
  typename std::enable_if<!std::is_array<Ty>::value, element_type&>::type operator*() const {
    return *get();
  }
  template <typename Ty = T>
  typename std::enable_if<std::is_array<Ty>::value && std::extent<T>::value == 0, element_type&>::type operator[](
      std::size_t index) const {
    return get()[index];
  }

  operator bool() const { return get() != nullptr; }

  void reset(element_type* data = nullptr) {
    ptr_base_.DecUseCount();
    ptr_base_.UnsafeReset(data);
  }

  std::size_t use_count() const { return ptr_base_.UseCount(); }
  bool unique() const { return use_count() == 1; }

  void swap(SharedPtr& other) { ptr_base_.swap(other.ptr_base_); }

 private:
  SharedPtr(const PtrBase<T>& ptr_base) : ptr_base_(ptr_base) {
    if (ptr_base_.UseCount()) {
      ptr_base_.IncUseCount();
    } else {
      ptr_base_.UnsafeReset();
    }
  }
  PtrBase<T> ptr_base_;
};

template <typename T>
class WeakPtr {
 public:
  using element_type = typename PtrBase<T>::element_type;

  template <typename Y>
  friend class WeakPtr;

  WeakPtr() {}
  WeakPtr(const WeakPtr& other) : ptr_base_(other.ptr_base_) { ptr_base_.IncWeakCount(); }
  WeakPtr(WeakPtr&& other) = default;
  template <typename Y>
  WeakPtr(const WeakPtr<Y>& other) : ptr_base_(other.ptr_base_) {
    ptr_base_.IncWeakCount();
  }
  template <typename Y>
  WeakPtr(WeakPtr<Y>&& other) : ptr_base_(std::move(other.ptr_base_base)) {}
  template <typename Y>
  WeakPtr(const SharedPtr<Y>& other) : ptr_base_(other.ptr_base_) {
    ptr_base_.IncWeakCount();
  }
  ~WeakPtr() { ptr_base_.DecWeakCount(); }

  WeakPtr& operator=(const WeakPtr& other) {
    WeakPtr(other).swap(*this);
    return *this;
  }
  template <typename Y>
  WeakPtr& operator=(const WeakPtr<Y>& other) {
    WeakPtr(other).swap(*this);
    return *this;
  }
  WeakPtr& operator=(WeakPtr&& other) {
    WeakPtr(std::move(other)).swap(*this);
    return *this;
  }
  template <typename Y>
  WeakPtr& operator=(WeakPtr<Y>&& other) {
    WeakPtr(std::move(other)).swap(*this);
    return *this;
  }
  template <typename Y>
  WeakPtr& operator=(SharedPtr<Y>& other) {
    WeakPtr(other).swap(*this);
    return *this;
  }

  SharedPtr<T> lock() const { return SharedPtr<T>(ptr_base_); }
  bool expired() const { return use_count() == 0; }
  std::size_t use_count() const { return ptr_base_.UseCount(); }

  void swap(WeakPtr& other) { ptr_base_.swap(other.ptr_base_); }

  void reset() { ptr_base_.UnsafeReset(); }

 private:
  PtrBase<T> ptr_base_;
};

template <typename T, typename... Args>
AutoPtr<T> make_auto_ptr(Args&&... args) {
  return AutoPtr<T>(new T(std::forward<Args>(args)...));
}

// 不是数组
template <typename T, typename... Args>
typename std::enable_if<!std::is_array<T>::value, UniquePtr<T>>::type make_unique(Args&&... args) {
  return UniquePtr<T>(new T(std::forward<Args>(args)...));
}

// 数组且未知边界
template <typename T>
typename std::enable_if<std::is_array<T>::value && std::extent<T>::value == 0, UniquePtr<T>>::type make_unique(
    std::size_t size) {
  using Element = typename std::remove_extent<T>::type;
  return UniquePtr<T>(new Element[size]());
}

// 数组且已知边界
template <typename T>
typename std::enable_if<std::is_array<T>::value && std::extent<T>::value != 0, UniquePtr<T>>::type make_unique(
    std::size_t size) = delete;

template <typename T, typename... Args>
typename std::enable_if<!std::is_array<T>::value, SharedPtr<T>>::type make_shared(Args&&... args) {
  return SharedPtr<T>(new T(std::forward<Args>(args)...));
}

template <typename T, typename... Args>
typename std::enable_if<std::is_array<T>::value && std::extent<T>::value == 0, SharedPtr<T>>::type make_shared(
    std::size_t size) {
  using Element = typename std::remove_extent<T>::type;
  return SharedPtr<T>(new Element[size]());
}

template <typename T>
using auto_ptr = AutoPtr<T>;

template <typename T, typename Deleter = std::default_delete<T>>
using unique_ptr = UniquePtr<T, Deleter>;

template <typename T>
using shared_ptr = SharedPtr<T>;

}  // namespace gtl