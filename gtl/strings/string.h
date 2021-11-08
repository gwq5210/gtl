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

#include "gtl/iterator.h"
#include "gtl/storage.h"

namespace gtl {

template <typename Char, typename Traits = std::char_traits<Char>>
class StringImpl {
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
  constexpr static size_type kMaxSmallSize = kMaxSmallBytesSize / sizeof(value_type);
  constexpr static size_type kMaxMediumSize = kMaxMediumBytesSize / sizeof(value_type);

  enum class StringType {
    kSmall,
    kMedium,
    kLarge,
  };

  struct StringStorage {
    StringStorage() : size(0), capacity(0), data(nullptr) {}
    StringStorage(const value_type* data, size_type len, size_type c) : size(len), capacity(c), data(nullptr) {
      data = alloc_buffer(capacity + 1);
      std::memcpy(MediumLargeString::Data(this), data, sizeof(value_type) * (len + 1));
    }
    size_type size;
    size_type capacity;
    void* data;

    void* alloc_buffer(size_type size) const { return ::operator new(size * sizeof(value_type)); }
  };

  struct MediumLargeString {
    MediumLargeString() : ref_count(1) { data[0] = '\0'; }
    std::atomic<size_type> ref_count;
    value_type data[];

    static size_type GetDataOffset() { return offsetof(MediumLargeString, data); }

    static void ConstrctAt(StringStorage& store) { gtl::construct_at((MediumLargeString*)store.data); }

    static MediumLargeString& From(StringStorage& store) { return *static_cast<MediumLargeString*>(store.data); }

    static value_type* Data(StringStorage& store) { return From(store).data; }

    static std::atomic<size_type>& RefCount(StringStorage& store) { return From(store).ref_count; }
  };

  struct SmallString {
    SmallString() { Init(); }
    void Init() {
      size = 0;
      data[size] = '\0';
    }
    void CopyFrom(const value_type* str, size_type len) {
      if (str) {
        assert(len < kMaxSmallSize);
        size = len;
        memcpy(data, str, sizeof(value_type) * len);
      } else {
        Init();
      }
    }
    uint8_t size;
    value_type data[kMaxSmallSize];
  };

  StringImpl() : small_(), type_(StringType::kSmall) {}
  StringImpl(const value_type* str) : StringImpl() { UnsafeInit(str); }
  StringImpl(const StringImpl& other) { UnsafeCopyFrom(other); }
  StringImpl(StringImpl&& other) { UnsafeMoveFrom(std::move(other)); }
  ~StringImpl() {}

  StringImpl& operator=(const StringImpl& other) {
    StringImpl(other).swap(*this);
    return *this;
  }
  StringImpl& operator=(StringImpl&& other) {
    StringImpl(other).swap(*this);
    return *this;
  }

  size_type capacity() const { return GetCapacity(); }
  size_type size() const { return GetSize(); }
  bool empty() const { return size() == 0; }

  pointer c_str() { return data(); }
  const_pointer c_str() const { return data(); }
  const_pointer cc_str() const { return data(); }

  pointer data() {
    switch (type_) {
      case StringType::kSmall: {
        return small_.data;
      }
      case StringType::kMedium: {
        return MediumLargeString::Data(store_);
      }
      case StringType::kLarge: {
        return MediumLargeString::Data(store_);
      }
    }
  }

  const_pointer cdata() const { return data(); }
  const_pointer data() const {
    switch (type_) {
      case StringType::kSmall: {
        return small_.data;
      }
      case StringType::kMedium:
      case StringType::kLarge: {
        return MediumLargeString::Data(store_);
      }
    }
  }

  void swap(StringImpl& other) {
    static_assert(sizeof(SmallString) == sizeof(StringStorage));
    std::swap(small_, other.small_);
    std::swap(type_, other.type_);
  }

 private:
  size_type GetSize() const {
    return type_ == StringType::kSmall ? small_.size : store_.size;
  }
  size_type GetCapacity() const {
    return type_ == StringType::kSmall ? kMaxSmallSize : store_.capacity;
  }
  void UnSafeInit(const value_type* str = nullptr, size_type len = 0) {
    if (str == nullptr) {
      type_ = StringType::kSmall;
      small_.Init();
    } else {
      size_type len = len > 0 ? len : std::strlen(str);
      if (len < kMaxSmallSize) {
        small_.CopyFrom(str, len);
      } else if (len < kMaxMediumSize) {
      } else {
      }
    }
  }
  void UnsafeCopyFrom(const StringImpl& other) {
    type_ = other.type_;
    if (other.type_ == StringType::kSmall) {
      small_ = other.small_;
    } else if (other.type_ == StringType::kMedium) {
    } else if (other.type_ == StringType::kLarge) {
    } else {
      assert(false);
    }
  }
  void UnsafeMoveFrom(StringImpl&& other) {
    type_ = other.type_;
    if (other.type_ == StringType::kSmall) {
      small_ = other.small_;
    } else if (other.type_ == StringType::kMedium) {
    } else if (other.type_ == StringType::kLarge) {
    } else {
      assert(false);
    }
  }

  union {
    SmallString small_;
    StringStorage store_;
  };
  StringType type_;
};

}  // namespace gtl