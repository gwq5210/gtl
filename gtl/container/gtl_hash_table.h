/**
 * @file gtl_hash_table.h
 * @author gwq5210 (gwq5210@qq.com)
 * @brief 哈希表的实现
 * @date 2021-05-28
 * 
 * @copyright Copyright (c) 2021. All rights reserved.
 */

#pragma once

#include <functional>
#include <memory>

#include "gtl_storage.h"
#include "gtl_slist.h"

namespace gtl {

template <typename T, typename Hash = std::hash<T>, typename KeyEquel = std::equal_to<T>>
class HashTable {
 public:
  struct HashNode : public SListNode {
    T val;
    template <typename... Args>
    HashNode(Args&&... args) {
      gtl::construct_at(&val, std::forward<Args>(args)...);
    }
  };

  using NodeAllocator = std::allocator<HashNode>;
  using BucketStorage = Storage<HashNode*>;

 private:
  BucketStorage buckets_;
  NodeAllocator node_allocator_;
}; // class HashTable

}  // namespace gtl