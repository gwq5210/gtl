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

#include "gtl_slist.h"
#include "gtl_vector.h"

namespace gtl {

template <typename T, typename Hash = std::hash<T>, typename KeyEquel = std::equal_to<T>>
class HashTable {
 public:
  using Node = singly_list::SListNodeT<T>;
  using value_type = T;
  using reference = T&;
  using const_reference = const T&;
  using pointer = T*;
  using const_pointer = const T*;
  using hasher = Hash;
  using key_equel = KeyEquel;
  using allocator_type = std::allocator<T>;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using iterator = SListIterator<T, difference_type>;
  using const_iterator = SListConstIterator<T, difference_type>;
  using local_iterator = SListIterator<T, difference_type>;
  using const_local_iterator = SListConstIterator<T, difference_type>;
  using NodeAllocator = typename std::allocator_traits<allocator_type>::rebind_alloc<Node>;
  struct HashNode {
    Node* node_begin;
    Node* node_end;
  };
  using BucketVector = Vector<HashNode*>;

  allocator_type get_allocator() const { return static_cast<allocator_type>(get_node_allocator()); }

 private:
  template <typename... Args>
  Node* new_node(Args&&... args) const {
    return Node::New(get_node_allocator(), std::forward<Args>(args)...);
  }
  void delete_node(SListNode* node) const { return Node::Delete(get_node_allocator(), node); }
  size_type& get_size() { return size_alloc_.first(); }
  const size_type& get_size() const { return size_alloc_.first(); }
  NodeAllocator& get_node_allocator() { return size_alloc_.second(); }
  const NodeAllocator& get_node_allocator() const { return size_alloc_.second(); }

  BucketStorage buckets_;
  CompressedPair<size_type, NodeAllocator> size_alloc_;
};  // class HashTable

}  // namespace gtl