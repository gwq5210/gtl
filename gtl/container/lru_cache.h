/**
 * @file lru_cache.h
 * @author gwq5210 (gwq5210@qq.com)
 * @brief LRUCache实现
 * @date 2021-11-14
 * 
 * @copyright Copyright (c) 2021. All rights reserved.
 */

#pragma once

#include <functional>

#include "gtl/hash_table.h"
#include "gtl/list.h"

namespace gtl {

template <typename Key, typename Value, typename Hash = std::hash<Key>, typename KeyEqual = std::equal_to<Key>>
class LRUCache {
 public:
  using KeyType = Key;
  using ValueType = Value;
  using NodeValue = std::pair<Key, Value>;
  using ListNodeT = doubly_list::ListNodeT<NodeValue>;
  using Iterator = gtl::ListIterator<NodeValue>;
  using ConstIterator = gtl::ListConstIterator<NodeValue>;
  using SizeType = std::size_t;
  using HashTableType = HashTable<Key, std::pair<Key, ListNode*>, MapKeyFunc<Key, ListNode*>>;
  using Allocator = std::allocator<ListNodeT>;

  constexpr static SizeType kLRUCacheMinBucketSize = 16;

  LRUCache(SizeType capacity) : capacity_(capacity) {}
  LRUCache(const LRUCache& other) = delete;
  LRUCache(LRUCache&& other) {}
  ~LRUCache() {}

  LRUCache& operator=(const LRUCache& other) = delete;
  LRUCache& operator=(LRUCache&& other) { return *this; }

  Iterator begin() { return Iterator(lru_.next); }
  ConstIterator begin() const { return cbegin(); }
  ConstIterator cbegin() const { return ConstIterator(lru_.next); }
  Iterator end() { return Iterator(&lru_); }
  ConstIterator end() const { return cend(); }
  ConstIterator cend() { return ConstIterator(&lru_); }

  std::pair<Iterator, bool> Set(const Key& key, const Value& value) {
    auto* node = Find(key);
    if (node) {
      ListNodeT::Value(node).second = value;
      return std::make_pair(Iterator(node), true);
    } else {
      auto *new_node = ListNodeT::New(allocator_, std::make_pair(key, value));
      doubly_list::AddToHead(&lru_, new_node);
      ht_.emplace(key, new_node);
      if (Size() > Capacity()) {
        auto* tail = lru_.prev;
        ht_.erase(ListNodeT::Value(tail).first);
        doubly_list::Remove(tail);
        ListNodeT::Delete(allocator_, tail);
      }
      return std::make_pair(Iterator(new_node), true);
    }
  }

  Value& Get(const Key& key) {
    return GetValue(key);
  }
  const Value& Get(const Key& key) const {
    return GetValue(key);
  }

  bool Exists(const Key& key) const {
    return FindListNode(key) != nullptr;
  }

  SizeType Size() const { return ht_.size(); }

  SizeType Capacity() const { return capacity_; }

 private:
  doubly_list::ListNode* FindListNode(const Key& key) const {
    auto it = ht_.find(key);
    if (it != ht_.end()) {
      return it->second;
    } else {
      return nullptr;
    }
  }
  doubly_list::ListNode* Find(const Key& key) {
    doubly_list::ListNode* node = FindListNode(key);
    if (node) {
      doubly_list::Remove(node);
      doubly_list::AddToHead(&lru_, node);
      return node;
    } else {
      return nullptr;
    }
  }
  Value& GetValue(const Key& key) {
    auto* node = Find(key);
    if (!node) {
      throw std::out_of_range("key not found");
    }
    return ListNodeT::Value(node).second;
  }

  HashTableType ht_;
  doubly_list::ListHead lru_;
  Allocator allocator_;
  SizeType capacity_;
};

template <typename Key, typename Value, typename Hash, typename KeyEqual>
constexpr typename LRUCache<Key, Value, Hash, KeyEqual>::SizeType LRUCache<Key, Value, Hash, KeyEqual>::kLRUCacheMinBucketSize;

}  // namespace gtl