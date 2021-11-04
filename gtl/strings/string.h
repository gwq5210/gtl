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

#include <string>

#include "storage.h"

namespace gtl {

template <typename Char, typename Traits = std::char_traits<Char>>
class StringImpl {
 public:
  using traits_type = Traits;
  using value_type = Char;
  using StringStorage = Storage<value_type>;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using reference = value_type&;
  using const_reference = value_type&;
  using pointer = value_type*;
  using const_pointer = const value_type*;
  using iterator = pointer;
  using const_pointer = const_pointer;
  using reverse_iterator = gtl::reverse_iterator<iterator>;
  using const_reverse_iterator = gtl::reverse_iterator<const_iterator>;

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
    size_type size;
    size_type capacity;
    value_type* data;
  };

  struct MediumLargeString {
    std::atomic<size_type> ref_count;
    value_type data[];

    static size_type GetDataOffset() {
      return offsetof(MediumLargeString, data);
    }

    static MediumLargeString& From(StringStorage& store) {
      return *static_cast<MediumLargeString*>(store.data());
    }

    static value_type* Data(StringStorage& store) {
      return From(store).data;
    }

    static std::atomic<size_type>& RefCount(StringStorage& store) {
      return From(store).ref_count;
    }
  };

  struct SmallString {
    void Init() {
      size = 0;
      data[size] = '\0';
    }
    uint8_t size;
    value_type data[kMaxSmallSize];
  };

  StringImpl() { Init(); }

  size_type capacity() const { return 0; }

  pointer c_str() { return data(); }
  const_pointer c_str() const { return data(); }

  pointer data() {
    switch (type_) {
      case kSmall:
      {
        return small_.data;
      }
      case kMedium:
      {
        return MediumLargeString::Data(store_);
      }
      case kLarge:
      {
        return MediumLargeString::Data(store_);
      }
    }
  }

  const_pointer data() const {
    switch (type_) {
      case kSmall:
      {
        return small_.data;
      }
      case kMedium:
      case kLarge:
      {
        return MediumLargeString::Data(store_);
      }
    }
  }

 private:
  void Init() {
    type_ = StringType::kSmall;
    small_.Init();
  }
  void UnsafeCopyFrom(StringImpl& impl) {
    if (impl.type_ == StringType::kSmall) {
      small_ = impl.small_;
    } else if (impl.type_ == StringType::kMedium) {
      StringStorage new_store(impl.store_.capacity());

    } else if (impl.type_ == StringType::kLarge) {

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