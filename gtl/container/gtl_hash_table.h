/**
 * @file gtl_hash_table.h
 * @author gwq5210 (gwq5210@qq.com)
 * @brief 哈希表的实现
 * @date 2021-05-28
 *
 * @copyright Copyright (c) 2021. All rights reserved.
 */

#pragma once

#include <cmath>

#include <functional>
#include <limits>
#include <memory>
#include <utility>

#include "gtl_common.h"
#include "gtl_compressed_pair.h"
#include "gtl_slist.h"
#include "gtl_storage.h"
#include "gtl_vector.h"

namespace gtl {

template <typename Key, typename Value>
class MapKeyFunc {
 public:
  const Key& operator()(const std::pair<Key, Value>& p) const { return MapKeyFunc::GetKey(p); }
  static const Key& GetKey(const std::pair<Key, Value>& p) { return std::get<0>(p); }
  static const Value& GetValue(const std::pair<Key, Value>& p) { return std::get<1>(p); }
};

template <typename Key, typename Value>
class SetKeyFunc {
 public:
  const Key& operator()(const Value& value) const { return value; }
};

template <typename Key, typename Value, typename ExtractKey, typename Hash = std::hash<Key>, typename KeyEquel = std::equal_to<Key>>
class HashTable {
 public:
  using Node = singly_list::SListNodeT<Value>;
  using key_type = Key;
  using value_type = Value;
  using reference = value_type&;
  using const_reference = const value_type&;
  using pointer = value_type*;
  using const_pointer = const value_type*;
  using hasher = Hash;
  using key_equal = KeyEquel;
  using allocator_type = std::allocator<value_type>;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using iterator = SListIterator<value_type, difference_type>;
  using const_iterator = SListConstIterator<value_type, difference_type>;
  using local_iterator = SListIterator<value_type, difference_type>;
  using const_local_iterator = SListConstIterator<value_type, difference_type>;
  // using NodeAllocator = typename std::allocator_traits<allocator_type>::rebind_alloc<Node>;
  using NodeAllocator = std::allocator<Node>;
  static constexpr size_type min_bucket_size_ = 16;
  static constexpr float default_max_load_factor_ = 0.85;
  struct HashNode {
    HashNode(): node_before_begin(nullptr), node_finish(nullptr) {}
    HashNode(SListNode* b, SListNode* f): node_before_begin(b), node_finish(f) {}
    local_iterator begin() { return node_before_begin ? iterator(node_before_begin->next) : iterator(nullptr); }
    const_local_iterator begin() const { return cbegin(); }
    const_local_iterator cbegin() const { return node_before_begin ? iterator(node_before_begin->next) : iterator(nullptr); }
    local_iterator end() { return node_finish ? iterator(node_finish->next) : iterator(nullptr); }
    const_local_iterator end() const { return cend(); }
    const_local_iterator cend() const { return node_finish ? iterator(node_finish->next) : iterator(nullptr); }
    size_type size() const { return gtl::distance(begin(), end()); }
    bool empty() const { return node_before_begin == nullptr; }
    void insert_node(SListNode* before, SListNode* first, SListNode* last) {
      assert(before->next == first);
      if (!node_before_begin) { // bucket为空，插入到所有元素的链表头部
        node_before_begin = before;
        node_finish = last;
      } else if (before == node_finish) {
        node_finish = last; // 在bucket尾部插入，更新bucket尾部节点
      }
    }
    void erase_node(SListNode* before, SListNode* last) {
      if (last == node_finish) {
        node_finish = before;
      }
      if (node_finish == node_before_begin) {
        node_before_begin = nullptr;
        node_finish = nullptr;
      }
    }
    void reset(SListNode* b = nullptr, SListNode* f = nullptr) {
      if (b == f) {
        node_before_begin = nullptr;
        node_finish = nullptr;
      } else {
        node_before_begin = b;
        node_finish = f;
      }
    }
    SListNode* node_before_begin;
    SListNode* node_finish;
  };
  struct FindNodeResult {
    FindNodeResult(): before(nullptr), node(nullptr) {}
    FindNodeResult(SListNode* b, SListNode* n): before(b), node(n) {}
    SListNode* before;
    SListNode* node;
  };
  using BucketStorage = UStorage<HashNode>;

  // constructor
  HashTable(): HashTable(min_bucket_size_) {}
  HashTable(size_type bucket_count): max_load_factor_(default_max_load_factor_), buckets_(alloc_buckets(bucket_count)), size_alloc_(0) {}
  template <typename InputIt>
  HashTable(bool unique, InputIt first, InputIt last, size_type bucket_count = 0): HashTable(bucket_count ? bucket_count : gtl::distance(first, last)) {
    insert(unique, first, last);
  }
  HashTable(const HashTable& other): HashTable(false, other.begin(), other.end(), other.bucket_count()) {
    max_load_factor_ = other.max_load_factor_;
  }
  HashTable(HashTable&& other) { move_from(std::move(other)); }
  HashTable(bool unique, std::initializer_list<value_type> ilist, size_type bucket_count = 0): HashTable(unique, ilist.begin(), ilist.end(), bucket_count ? bucket_count : ilist.size()) {}
  ~HashTable() { release(); }

  HashTable& operator=(const HashTable& other) { return assign(other); }
  HashTable& operator=(HashTable&& other) { return assign(std::move(other)); }
  HashTable& operator=(std::initializer_list<value_type> ilist) { return assign(false, ilist); }

  allocator_type get_allocator() const { return static_cast<allocator_type>(get_node_allocator()); }

  // Iterators
  iterator begin() { return iterator(head_.next); }
  const_iterator begin() const { return const_iterator(head_.next); }
  const_iterator cbegin() const { return const_iterator(head_.next); }
  iterator end() { return iterator(nullptr); }
  const_iterator end() const { return const_iterator(nullptr); }
  const_iterator cend() const { return const_iterator(nullptr); }

  // Capacity
  bool empty() const { return head_.next == nullptr; }
  size_type size() const { return get_size(); }
  size_type max_size() const { return std::numeric_limits<difference_type>::max(); }

  // Modifiers
  void clear() { release(); }
  template <typename P>
  std::pair<iterator, bool> insert(bool unique, P&& value) {
    return emplace(unique, std::move(value));
  }
  template <typename P>
  std::pair<iterator, bool> insert(P&& value) {
    return emplace(false, std::move(value));
  }
  template <typename P>
  std::pair<iterator, bool> insert(const_iterator hint, P&& value) {
    return emplace_hint(false, hint, std::move(value));
  }
  template <typename P>
  std::pair<iterator, bool> insert(bool unique, const_iterator hint, P&& value) {
    return emplace_hint(unique, hint, std::move(value));
  }
  std::pair<iterator, bool> insert(const Value& value) {
    return emplace(false, value);
  }
  std::pair<iterator, bool> insert(bool unique, const Value& value) {
    return emplace(unique, value);
  }
  std::pair<iterator, bool> insert_unique(const Value& value) {
    return emplace_unique(value);
  }
  std::pair<iterator, bool> insert_equal(const Value& value) {
    return emplace_equal(value);
  }
  std::pair<iterator, bool> insert(Value&& value) {
    return emplace(false, std::move(value));
  }
  std::pair<iterator, bool> insert(bool unique, Value&& value) {
    return emplace(unique, std::move(value));
  }
  std::pair<iterator, bool> insert_unique(Value&& value) {
    return emplace_unique(std::move(value));
  }
  std::pair<iterator, bool> insert_equal(Value&& value) {
    return emplace_equal(std::move(value));
  }
  std::pair<iterator, bool> insert(const_iterator hint, const Value& value) {
    return emplace_hint(false, hint, value);
  }
  std::pair<iterator, bool> insert(const_iterator hint, Value&& value) {
    return emplace_hint(false, hint, std::move(value));
  }
  std::pair<iterator, bool> insert(bool unique, const_iterator hint, const Value& value) {
    return emplace_hint(unique, hint, value);
  }
  std::pair<iterator, bool> insert(bool unique, const_iterator hint, Value&& value) {
    return emplace_hint(unique, hint, std::move(value));
  }
  template <typename InputIt>
  void insert(InputIt first, InputIt last) {
    insert(false, first, last);
  }
  template <typename InputIt>
  void insert(bool unique, InputIt first, InputIt last) {
    check_for_rehash(gtl::distance(first, last));
    for (InputIt it = first; it != last; ++it) {
      emplace(unique, *it);
    }
  }
  void insert(std::initializer_list<value_type> ilist) {
    insert(false, ilist.begin(), ilist.end());
  }
  void insert(bool unique, std::initializer_list<value_type> ilist) {
    insert(unique, ilist.begin(), ilist.end());
  }
  template <typename... Args>
  std::pair<iterator, bool> emplace(Args&&... args) {
    return emplace_hint(false, const_iterator(), std::forward<Args>(args)...);
  }
  template <typename... Args>
  std::pair<iterator, bool> emplace(bool unique, Args&&... args) {
    return emplace_hint(unique, const_iterator(), std::forward<Args>(args)...);
  }
  template <typename... Args>
  std::pair<iterator, bool> emplace_hint(const_iterator hint, Args&&... args) {
    return insert_node(false, hint, std::forward<Args>(args)...);
  }
  template <typename... Args>
  std::pair<iterator, bool> emplace_hint(bool unique, const_iterator hint, Args&&... args) {
    return insert_node(unique, hint, std::forward<Args>(args)...);
  }
  template <typename... Args>
  std::pair<iterator, bool> emplace_unique(Args&&... args) {
    return emplace(true, std::forward<Args>(args)...);
  }
  template <typename... Args>
  std::pair<iterator, bool> emplace_equal(Args&&... args) {
    return emplace(false, std::forward<Args>(args)...);
  }
  iterator erase(const_iterator pos) {
    const value_type& value = Node::Value(pos.node);
    size_type bucket_idx = bucket(get_key(value));
    FindNodeResult result = find_node(value, bucket_idx);
    assert(result.node);
    erase_node(bucket_idx, result.before, result.node, 1);
    return iterator(result.before->next);
  }
  iterator erase(const_iterator first, const_iterator last) {
    if (first == last) {
      return iterator(last.node);
    }
    const value_type& value = Node::Value(first.node);
    size_type bucket_idx = bucket(get_key(value));
    SListNode* before = PrevNode(buckets_[bucket_idx].node_before_begin, first.node);
    SListNode* prev = before;
    printf("erase range before key: %d\n", get_key(Node::Value(before)));
    while (prev->next != last.node) {
      HashNode& hnode = buckets_[bucket_idx];
      SListNode* node_end = hnode.node_finish->next;
      printf("erase begin\n");
      size_type count = 0;
      while (prev->next != node_end && prev->next != last.node) {
        printf("erase range key: %d\n", Node::Value(prev->next));
        prev = prev->next;
        ++count;
      }
      erase_node(bucket_idx, before, prev, count);
      prev = before;
      printf("bucket %zu size %zu\n", bucket_idx, hnode.size());
      if (prev->next == last.node) {
        break;
      }
      bucket_idx = bucket(get_key(Node::Value(prev->next)));
    }
    return iterator(last.node);
  }
  size_type erase(const key_type& key) {
    size_type bucket_idx = bucket(key);
    FindNodeResult result = find_node(key, bucket_idx);
    if (result.node) {
      auto range_res = find_range(key, result.before);
      erase_node(bucket_idx, range_res.first.before, range_res.first.node, range_res.second);
      return range_res.second;
    }
    return 0;
  }
  void swap(HashTable& other) {
    std::swap(max_load_factor_, other.max_load_factor_);
    std::swap(head_, other.head_);
    buckets_.swap(other.buckets_);
    std::swap(get_size(), other.get_size());
  }

  // Lookup
  iterator find(const key_type& key) {
    return iterator(find_node(key).node);
  }
  const_iterator find(const key_type& key) const {
    return const_iterator(find_node(key).node);
  }
  size_type count(const key_type& key) const { return count_equal(key); }
  size_type count_unique(const key_type& key) const { return count(true, key); }
  size_type count_equal(const key_type& key) const { return count(false, key); }
  size_type count(bool unique, const key_type& key) const {
    FindNodeResult result = find_node(key);
    if (unique) {
      return result.node ? 1 : 0;
    }
    size_type ret = 0;
    for (SListNode* n = result.node; n && key_equal_(key, Node::Value(n)); n = n->next) {
      ++ret;
    }
    return ret;
  }
  bool contains(const key_type& key) const {
    return find_node(key).node;
  }
  std::pair<iterator, iterator> equal_range(const key_type& key) { return equal_range(false, key); }
  std::pair<const_iterator, const_iterator> equal_range(const key_type& key) const { return equal_range(false, key); }
  std::pair<iterator, iterator> equal_range(bool unique, const key_type& key) {
    auto res = find_node(key);
    if (!res.node) {
      return std::make_pair(iterator(), iterator());
    }
    if (unique) {
      return std::make_pair(iterator(res.node), iterator(res.node->next));
    } else {
      auto range_res = find_range(key, res.before);
      return std::make_pair(iterator(res.node), iterator(range_res.first.node->next));
    }
  }
  std::pair<const_iterator, const_iterator> equal_range(bool unique, const key_type& key) const {
    auto res = find_node(key);
    if (!res.node) {
      return std::make_pair(const_iterator(), const_iterator());
    }
    if (unique) {
      return std::make_pair(const_iterator(res.node), const_iterator(res.node->next));
    } else {
      auto range_res = find_range(key, res.before);
      return std::make_pair(const_iterator(res.node), const_iterator(range_res.first.node->next));
    }
  }

  // Bucket interface
  local_iterator begin(size_type n) { return buckets_[n].begin(); }
  const_local_iterator begin(size_type n) const { return buckets_[n].begin(); }
  const_local_iterator cbegin(size_type n) const { return buckets_[n].begin(); }
  local_iterator end(size_type n) { return buckets_[n].end(); }
  const_local_iterator end(size_type n) const { return buckets_[n].end(); }
  const_local_iterator cend(size_type n) const { return buckets_[n].end(); }
  size_type bucket_count() const { return buckets_.capacity(); }
  size_type bucket_size(size_type n) const { return buckets_[n].size(); }
  size_type bucket(const key_type& key) const { return hasher_(key) % bucket_count(); }

  // Hash policy
  float load_factor() const { return float(size()) / bucket_count(); }
  void rehash(size_type count) {
    size_type new_bucket_count = next2power(size_type(std::ceil(count / max_load_factor())));
    if (bucket_count() >= new_bucket_count) {
      return;
    }
    do_rehash(new_bucket_count);
  }
  void reserve(size_type count) { rehash(count); }
  float max_load_factor() const { return max_load_factor_; }
  void max_load_factor(float load_factor) { max_load_factor_ = load_factor; }

  // Observers
  key_equal key_eq() const { return key_equal_; }
  hasher hash_function() const { return hasher_; }

  // Compare
  bool equals(const HashTable& other) const {
    if (size() != other.size()) {
      return false;
    }
    SListNode* prev = &head_;
    while (prev->next) {
      const key_type& key = get_key(Node::Value(prev->next));
      auto range_res = find_range(key, prev);
      auto node_it = other.find_node(key);
      if (!node_it.node) {
        return false;
      }
      auto other_range_res = other.find_range(key, node_it.before);
      if (other_range_res.count != range_res.count) {
        return false;
      }
      SListNode* l = range_res.before->next;
      SListNode* r = other_range_res.before->next;
      for (; l != range_res.node->next; l = l->next, r = r->next) {
        if (!(Node::Value(l) == Node::Value(r))) {
          return false;
        }
      }
      prev = range_res.node;
    }
    return true;
  }
  bool equals_unique(const HashTable& other) const {
    if (size() != other.size()) {
      return false;
    }
    SListNode* node = head_.next;
    while (node) {
      auto res = find_node(get_key(Node::Value(node)));
      if (!res.node || !(Node::Value(node) == Node::Value(res.node))) {
        return false;
      }
      node = node->next;
    }
    return true;
  }

 private:
  template <typename... Args>
  Node* NewNode(Args&&... args) {
    return Node::New(get_node_allocator(), std::forward<Args>(args)...);
  }
  void DeleteNode(SListNode* node) { return Node::Delete(get_node_allocator(), node); }
  size_type& get_size() { return size_alloc_.first(); }
  const size_type& get_size() const { return size_alloc_.first(); }
  void incr_size(size_type n) { get_size() += n; }
  NodeAllocator& get_node_allocator() { return size_alloc_.second(); }
  const NodeAllocator& get_node_allocator() const { return size_alloc_.second(); }
  BucketStorage alloc_buckets(size_type size) {
    size = std::max(size, min_bucket_size_);
    BucketStorage buckets(size);
    gtl::uninitialized_fill_n(buckets.begin(), size, HashNode());
    return buckets;
  }
  void init() {
    max_load_factor_ = default_max_load_factor_;
    head_.next = nullptr;
    buckets_ = alloc_buckets(min_bucket_size_);
    get_size() = 0;
  }
  void release () {
    SListNode* node = head_.next;
    while (node) {
      RemoveAfter(&head_, node);
      DeleteNode(node);
      node = head_.next;
    }
    head_.next = nullptr;
    get_size() = 0;
    gtl::fill_n(buckets_.begin(), buckets_.capacity(), HashNode());
  }
  FindNodeResult find_node(const key_type& key) const {
    return find_node(key, bucket(key));
  }
  FindNodeResult find_node(const key_type& key, size_type bucket_idx) const {
    const HashNode& hnode = buckets_[bucket_idx];
    if (hnode.empty()) {
      return {const_cast<SListNode*>(&head_), nullptr};
    }
    for (SListNode* prev = hnode.node_before_begin; prev->next != hnode.node_finish->next; prev = prev->next) {
      if (key_equal_(key, get_key(Node::Value(prev->next)))) {
        return {prev, prev->next};
      }
    }
    return {hnode.node_before_begin, nullptr};
  }
  std::pair<FindNodeResult, size_type> find_range(const key_type& key, SListNode* before) const {
    std::pair<FindNodeResult, size_type> res = std::make_pair(FindNodeResult(), 0);
    res.first.before = before;
    while (before->next && key_equal_(key, get_key(Node::Value(before->next)))) {
      before = before->next;
      ++res.second;
    }
    res.first.node = before;
    return res;
  }
  void erase_node(size_type bucket_idx, SListNode* before, SListNode* last, size_type count) {
    // 如果是最后一个元素，则下一个bucket的前一个节点需要更新
    if (last->next && last == buckets_[bucket_idx].node_finish) {
      buckets_[bucket(get_key(Node::Value(last->next)))].node_before_begin = before;
    }
    SListNode* first = before->next;
    SListNode* next = first->next;
    buckets_[bucket_idx].erase_node(before, last);
    RemoveAfter(before, last);
    incr_size(-count);
    while (first) {
      next = first->next;
      DeleteNode(first);
      first = next;
    }
  }
  void insert_node(size_type bucket_idx, SListNode* before, SListNode* node) {
    insert_node(bucket_idx, before, node, node, 1);
  }
  void insert_node(size_type bucket_idx, SListNode* before, SListNode* first, SListNode* last, size_type count) {
    InsertAfter(before, first, last);
    buckets_[bucket_idx].insert_node(before, first, last);
    incr_size(count);
    // 更新下一个bucket的前一个节点
    if (last->next && last == buckets_[bucket_idx].node_finish) {
      buckets_[bucket(get_key(Node::Value(last->next)))].node_before_begin = last;
    }
  }
  template <typename... Args>
  std::pair<iterator, bool> insert_node(bool unique, const_iterator hint, Args&&... args) {
    Node* node = NewNode(std::forward<Args>(args)...);
    SListNode* prev = hint.node;
    const key_type& key = get_key(Node::Value(node));
    size_type bucket_idx = bucket(key);
    if (prev && (key_equal_(get_key(Node::Value(prev)), key) || (prev->next && key_equal_(get_key(Node::Value(prev->next)), key)))) {
      if (unique) {
        DeleteNode(node);
        return std::make_pair(iterator(), false);
      }
    } else {
      FindNodeResult result = find_node(key, bucket_idx);
      if (unique && result.node) {
        DeleteNode(node);
        return std::make_pair(iterator(), false);
      }
      prev = result.before;
    }
    insert_node(bucket_idx, prev, node);
    // 可以先添加，然后再rehash
    check_for_rehash(0);
    return std::make_pair(iterator(node), true);
  }
  void check_for_rehash(size_type new_count) { reserve(size() + new_count); }
  void do_rehash(size_type new_bucket_count) {
    BucketStorage new_buckets = alloc_buckets(new_bucket_count);
    SListNode old_head = head_;
    buckets_.swap(new_buckets);
    head_.next = nullptr;
    get_size() = 0;

    printf("rehash new bucket count %zu %zu\n", bucket_count(), new_bucket_count);
    SListNode* prev = &old_head;
    SListNode* first = prev->next;
    while (first) {
      const key_type& key = get_key(Node::Value(first));
      size_type new_bucket_idx = bucket(key);
      auto range_res = find_range(key, prev);
      RemoveAfter(range_res.first.before, range_res.first.node);
      insert_node(new_bucket_idx, buckets_[new_bucket_idx].node_before_begin ? buckets_[new_bucket_idx].node_before_begin : &head_, first, range_res.first.node, range_res.second);
      first = prev->next;
    }
    printf("rehash done. new bucket count %zu\n", bucket_count());
  }
  const key_type& get_key(const value_type& value) const {
    return get_key_func_(value);
  }
  void move_from(HashTable&& other) {
    max_load_factor_ = other.max_load_factor_;
    get_size() = other.size();
    buckets_.swap(other.buckets_);
    gtl::swap(head_, other.head_);

    other.init();
  }
  HashTable& assign(const HashTable& other) {
    if (this != &other) {
      release();
      reserve(other.bucket_count());
      insert(false, other.begin(), other.end());
    }
    return *this;
  }
  HashTable& assign(std::initializer_list<value_type> ilist) {
    return assign(false, ilist);
  }
  HashTable& assign(bool unique, std::initializer_list<value_type> ilist) {
    release();
    reserve(ilist.size());
    insert(unique, ilist.begin(), ilist.end());
    return *this;
  }
  HashTable& assign(HashTable&& other) {
    release();
    reserve(other.bucket_count());
    move_from(other);
    return *this;
  }

  ExtractKey get_key_func_;
  key_equal key_equal_;
  hasher hasher_;
  SListNode head_;
  float max_load_factor_;
  BucketStorage buckets_;
  CompressedPair<size_type, NodeAllocator> size_alloc_;
};  // class HashTable

template <typename Key, typename Value, typename ExtractKey, typename Hash, typename KeyEqual>
bool operator==(const HashTable<Key, Value, ExtractKey, Hash, KeyEqual>& lhs, const HashTable<Key, Value, ExtractKey, Hash, KeyEqual>& rhs) {
  return lhs.equals(rhs);
}

template <typename Key, typename Value, typename ExtractKey, typename Hash, typename KeyEqual>
bool operator!=(const HashTable<Key, Value, ExtractKey, Hash, KeyEqual>& lhs, const HashTable<Key, Value, ExtractKey, Hash, KeyEqual>& rhs) {
  return !(lhs == rhs);
}

}  // namespace gtl