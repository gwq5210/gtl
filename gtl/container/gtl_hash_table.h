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

template <typename Key, typename Value, typename Hash = std::hash<Key>, typename KeyEquel = std::equal_to<Key>>
class HashTable {
 public:
  using Node = singly_list::SListNodeT<Value>;
  using value_type = Value
  using reference = value_type&;
  using const_reference = const value_type&;
  using pointer = value_type*;
  using const_pointer = const value_type*;
  using hasher = Hash;
  using key_equel = KeyEquel;
  using allocator_type = std::allocator<value_type>;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using iterator = SListIterator<value_type, difference_type>;
  using const_iterator = SListConstIterator<value_type, difference_type>;
  using local_iterator = SListIterator<value_type, difference_type>;
  using const_local_iterator = SListConstIterator<value_type, difference_type>;
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