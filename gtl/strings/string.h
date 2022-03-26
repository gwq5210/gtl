/**
 * @file string.h
 * @author gwq5210 (gwq5210@qq.com)
 * @brief 字符串实现
 * @date 2021-05-28
 *
 * @copyright Copyright (c) 2021. All rights reserved.
 */

#pragma once

#include <cstdint>
#include <cstring>

#include <string>

#include "gtl/iterator/iterator.h"
#include "gtl/container/storage.h"

namespace gtl {

template <typename Char, typename Traits = std::char_traits<Char>>
class StringCore {
 public:
  using traits_type = Traits;
  using value_type = Char;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using reference = value_type&;
  using const_reference = value_type&;
  using pointer = value_type*;
  using const_pointer = const value_type*;
  using iterator = pointer;
  using const_iterator = const_pointer;
  using reverse_iterator = gtl::reverse_iterator<iterator>;
  using const_reverse_iterator = gtl::reverse_iterator<const_iterator>;

  struct StringStorage;

  constexpr static size_type kMaxSmallBytesSize = sizeof(StringStorage) - 1;
  constexpr static size_type kMaxMediumBytesSize = 256;
  constexpr static size_type kMaxSmallSize = kMaxSmallBytesSize / sizeof(value_type) - 1;
  constexpr static size_type kMaxMediumSize = kMaxMediumBytesSize / sizeof(value_type) - 1;

  enum class StringType {
    kSmall,
    kMedium,
    kLarge,
  };

  struct StringStorage {
    StringStorage() : size(0), capacity(0), data(nullptr) {}
    void UnsafeCopyFrom(const_pointer str, size_type len) {
      assert(str && data);
      size = len;
      std::memcpy(data, str, len);
      data[len] = '\0';
    }
    size_type size;
    size_type capacity;
    pointer data;
  };

  struct LargeString {
    LargeString() : ref_count(1) { data[0] = '\0'; }
    std::atomic<size_type> ref_count;
    value_type data[];

    static size_type GetDataOffset() { return offsetof(LargeString, data); }

    static LargeString* LargeStringPtr(StringStorage& store) {
      return reinterpret_cast<LargeString*>(static_cast<char*>(store.data) - GetDataOffset());
    }

    static const LargeString* LargeStringPtr(const StringStorage& store) {
      return reinterpret_cast<const LargeString*>(static_cast<const char*>(store.data) - GetDataOffset());
    }

    static void ConstrctAt(StringStorage& store) { gtl::construct_at(LargeStringPtr(store)); }

    static LargeString& From(StringStorage& store) { return *LargeStringPtr(store); }

    static const LargeString& From(const StringStorage& store) { return *LargeStringPtr(store); }

    static pointer Data(StringStorage& store) { return From(store).data; }

    static const_pointer Data(const StringStorage& store) { return From(store).data; }

    static std::atomic<size_type>& RefCount(StringStorage& store) { return From(store).ref_count; }

    static const std::atomic<size_type>& RefCount(const StringStorage& store) { return From(store).ref_count; }

    static void IncRefCount(StringStorage& store) { ++RefCount(store); }

    static bool DecRefCount(StringStorage& store) { return --RefCount(store) == 0; }
  };

  struct SmallString {
    SmallString() { Init(); }
    void Init() {
      size = 0;
      data[size] = '\0';
    }
    void Init(const_pointer str, size_type len) {
      if (str) {
        assert(len <= kMaxSmallSize);
        size = len;
        memcpy(data, str, sizeof(value_type) * len);
        data[len] = '\0';
      } else {
        Init();
      }
    }
    uint8_t size;
    value_type data[kMaxSmallSize + 1];
  };

  StringCore() : small_(), type_(StringType::kSmall) {}
  StringCore(const_pointer str, size_type size = 0) : StringCore() { UnsafeInit(str, size); }
  StringCore(const StringCore& other) { UnsafeCopyFrom(other); }
  StringCore(StringCore&& other) { UnsafeMoveFrom(std::move(other)); }
  ~StringCore() { Destroy(); }

  StringCore& operator=(const StringCore& other) {
    if (this != &other) {
      StringCore(other).swap(*this);
    }
    return *this;
  }
  StringCore& operator=(StringCore&& other) {
    if (this != &other) {
      StringCore(std::move(other)).swap(*this);
    }
    return *this;
  }

  size_type capacity() const { return GetCapacity(); }
  size_type size() const { return GetSize(); }
  bool empty() const { return size() == 0; }
  StringType string_type() const { return type_; }

  pointer c_str() { return data(); }
  const_pointer c_str() const { return data(); }
  const_pointer cc_str() const { return data(); }

  pointer data() { return MutableData(); }

  const_pointer cdata() const { return data(); }
  const_pointer data() const { return type_ == StringType::kSmall ? small_.data : store_.data; }

  void swap(StringCore& other) {
    assert(sizeof(SmallString) == sizeof(StringStorage));
    std::swap(small_, other.small_);
    std::swap(type_, other.type_);
  }

  size_type use_count() const { return type_ == StringType::kLarge ? LargeString::RefCount(store_).load() : 1; }

  void reserve(size_type new_capacity) { Reserve(new_capacity); }
  void resize(size_type new_size) { Resize(new_size); }

 private:
  void* Allocate(size_type size) const { return ::operator new(size); }
  void Deallocate(void* p) const { ::operator delete(p); }
  pointer MutableData() {
    if (type_ == StringType::kSmall) {
      return small_.data;
    }
    if (type_ == StringType::kLarge && LargeString::RefCount(store_) > 1) {
      LargeString::DecRefCount(store_);
      UnsafeInitLarge(store_.data, store_.size);
    }
    return store_.data;
  }
  void Reserve(size_type new_capacity) {
    if (new_capacity <= capacity()) {
      return;
    }
    StringStorage new_storage;
    if (new_capacity <= kMaxMediumSize) {
      // small to medium
      assert(type_ == StringType::kSmall);
      new_storage = AllocateMediumStorage();
      new_storage.UnsafeCopyFrom(cdata(), size());
    } else if (type_ == StringType::kMedium) {
      // 1. medium to large
      // 2. large reserve
      new_storage = AllocateLargeStorage(new_capacity);
      new_storage.UnsafeCopyFrom(cdata(), size());
      Destroy();
      type_ = StringType::kLarge;
    }
    store_ = new_storage;
  }
  void Resize(size_t new_size) {
    if (new_size > capacity()) {
      Reserve(new_size);
    }
    if (new_size > size()) {
      gtl::fill_n(data() + size(), new_size - size(), value_type());
    }
    SetSize(new_size);
  }
  void Destroy() {
    if (type_ == StringType::kMedium) {
      Deallocate(store_.data);
    } else if (type_ == StringType::kLarge && LargeString::DecRefCount(store_)) {
      Deallocate(LargeString::LargeStringPtr(store_));
    }
  }
  size_type GetSize() const { return type_ == StringType::kSmall ? small_.size : store_.size; }
  void SetSize(size_type new_size) {
    if (type_ == StringType::kSmall) {
      assert(new_size <= kMaxSmallSize);
      small_.size = new_size;
    } else {
      store_.size = new_size;
    }
    data()[new_size] = '\0';
  }
  void SetCapacity(size_type new_capacity) {
    assert(type_ == StringType::kLarge);
    store_.capacity = new_capacity;
  }
  size_type GetCapacity() const { return type_ == StringType::kSmall ? kMaxSmallSize : store_.capacity; }
  void UnsafeInitSmall(const_pointer str = nullptr, size_type len = 0) {
    small_.Init(str, len);
    type_ = StringType::kSmall;
  }
  StringStorage AllocateMediumStorage() {
    StringStorage store;
    store.size = 0;
    store.capacity = kMaxMediumSize;
    store.data = static_cast<pointer>(Allocate(kMaxMediumSize + 1));
    return store;
  }
  void UnsafeInitMedium(const_pointer str, size_t len) {
    assert(len <= kMaxMediumSize);
    store_ = AllocateMediumStorage();
    store_.UnsafeCopyFrom(str, len);
    type_ = StringType::kMedium;
  }
  StringStorage AllocateLargeStorage(size_type capacity) {
    assert(capacity > kMaxMediumSize);
    StringStorage store;
    store.size = 0;
    store.capacity = capacity;
    store.data = static_cast<pointer>(static_cast<char*>(Allocate(LargeString::GetDataOffset() + capacity + 1)) +
                                      LargeString::GetDataOffset());
    LargeString::ConstrctAt(store);
    return store;
  }
  void UnsafeInitLarge(const_pointer str, size_type len, size_type c = 0) {
    store_ = AllocateLargeStorage(c > 0 ? c : len);
    store_.UnsafeCopyFrom(str, len);
    type_ = StringType::kLarge;
  }
  void UnsafeInit(const_pointer str = nullptr, size_type len = 0) {
    if (str == nullptr) {
      UnsafeInitSmall();
    } else {
      len = len > 0 ? len : std::strlen(str);
      if (len <= kMaxSmallSize) {
        UnsafeInitSmall(str, len);
      } else if (len <= kMaxMediumSize) {
        UnsafeInitMedium(str, len);
      } else {
        UnsafeInitLarge(str, len, len);
      }
    }
  }
  void UnsafeCopyFrom(const StringCore& other) {
    type_ = other.type_;
    if (other.type_ == StringType::kSmall) {
      small_ = other.small_;
    } else if (other.type_ == StringType::kMedium) {
      UnsafeInitMedium(other.cdata(), other.size());
    } else {
      store_ = other.store_;
      LargeString::IncRefCount(store_);
    }
  }
  void UnsafeMoveFrom(StringCore&& other) {
    type_ = other.type_;
    if (other.type_ == StringType::kSmall) {
      small_ = other.small_;
    } else {
      store_ = other.store_;
    }
    other.UnsafeInit();
  }

  union {
    SmallString small_;
    StringStorage store_;
  };
  StringType type_;
};

template <typename Char, typename Traits>
constexpr typename StringCore<Char, Traits>::size_type StringCore<Char, Traits>::kMaxSmallBytesSize;
template <typename Char, typename Traits>
constexpr typename StringCore<Char, Traits>::size_type StringCore<Char, Traits>::kMaxMediumBytesSize;
template <typename Char, typename Traits>
constexpr typename StringCore<Char, Traits>::size_type StringCore<Char, Traits>::kMaxSmallSize;
template <typename Char, typename Traits>
constexpr typename StringCore<Char, Traits>::size_type StringCore<Char, Traits>::kMaxMediumSize;

}  // namespace gtl