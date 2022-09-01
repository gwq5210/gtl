/**
 * @file unordered_map.h
 * @author gwq5210 (gwq5210@qq.com)
 * @brief unordered_map的实现
 * @date 2021-11-01
 *
 * @copyright Copyright (c) 2021. All rights reserved.
 */

#include "gtl/container/hash_table.h"

namespace gtl {

template <typename Key, typename T, typename Hash = std::hash<Key>, typename KeyEquel = std::equal_to<Key>>
class UnorderedMap {
 public:
  using key_type = Key;
  using mapped_type = T;
  using value_type = std::pair<const Key, T>;
  using MapHashTable = HashTable<key_type, value_type, MapKeyFunc, Hash, KeyEquel>;
  using size_type = typename MapHashTable::size_type;
  using difference_type = typename MapHashTable::difference_type;
  using hasher = typename MapHashTable::hasher;
  using key_equal = typename MapHashTable::key_equal;
  using reference = value_type&;
  using const_reference = const value_type&;
  using pointer = value_type*;
  using const_pointer = const value_type*;
  using iterator = typename MapHashTable::iterator;
  using const_iterator = typename MapHashTable::const_iterator;
  using local_iterator = typename MapHashTable::local_iterator;
  using const_local_iterator = typename MapHashTable::const_local_iterator;
  static constexpr bool unique_ = true;

  UnorderedMap() : mht_() {}
  UnorderedMap(size_type bucket_count) : mht_(bucket_count) {}
  template <typename InputIt>
  UnorderedMap(InputIt first, InputIt last, size_type bucket_count = gtl::distance(first, last))
      : mht_(unique_, first, last, bucket_count) {}
  UnorderedMap(const UnorderedMap& other) : mht_(other.mht_) {}
  UnorderedMap(UnorderedMap&& other) : mht_(std::move(other)) {}
  UnorderedMap(std::initializer_list<value_type> ilist, size_type bucket_count = ilist.size())
      : mht_(unique_, ilist, bucket_count) {}
  ~UnorderedMap() {}

  UnorderedMap& operator=(const UnorderedMap& other) = default;
  UnorderedMap& operator=(UnorderedMap&& other) = default;
  UnorderedMap& operator=(std::initializer_list<value_type> ilist) {
    mht_.assign(unique_, ilist);
    return *this;
  }

  // Iterators
  iterator begin() { return mht_.begin(); }
  const_iterator begin() const { return mht_.begin(); }
  const_iterator cbegin() const { return mht_.begin(); }
  iterator end() { return mht_.end(); }
  const_iterator end() const { return mht_.end(); }
  const_iterator cend() const { return mht_.end(); }

  // Capacity
  bool empty() const { return mht_.empty(); }
  size_type size() const { return mht_.size(); }
  size_type max_size() const { return mht_.max_size(); }

  // Modifiers
  void clear() { return mht_.clear(); }
  std::pair<iterator, bool> insert(const value_type& value) { return mht_.insert(unique_, value); }
  std::pair<iterator, bool> insert(value_type&& value) { return mht_.insert(unique_, std::move(value)); }
  template <typename P>
  std::pair<iterator, bool> insert(P&& value) {
    return mht_.insert(unique_, std::move(value));
  }
  std::pair<iterator, bool> insert(const_iterator hint, cosnt value_type& value) {
    return mht_.insert(unique_, hint, value);
  }
  std::pair<iterator, bool> insert(const_iterator hint, value_type&& value) {
    return mht_.insert(unique_, hint, std::move(value));
  }
  template <typename P>
  std::pair<iterator, bool> insert(const_iterator hint, P&& value) {
    return mht_.insert(unique_, hint, std::move(value));
  }
  template <typename InputIt>
  void insert(InputIt first, InputIt last) {
    mht_.insert(unique_, first, last);
  }
  void insert(std::initializer_list<value_type> ilist) { mht_.insert(unique_, ilist); }
  template <typename... Args>
  std::pair<iterator, bool> emplace(Args&&... args) {
    return mht_.emplace(unique_, std::forward<Args>(args)...);
  }
  template <typename... Args>
  std::pair<iterator, bool> emplace_hint(Args&&... args) {
    return mht_.emplace_hint(unique_, std::forward<Args>(args)...);
  }
  iterator erase(const_iterator pos) { return mht_.erase(pos); }
  iterator erase(const_iterator first, const_iterator last) { return mht_.erase(first, last); }
  size_type erase(const key_type& key) { return mht_.erase(key); }
  void swap(UnorderedMap& other) { mht_.swap(other.mht_); }

  // Lookup
  mapped_type& at(const key_type& key) {
    auto it = find(key);
    if (it != end()) {
      return it->second;
    } else {
      throw std::out_of_range();
    }
  }
  const mapped_type& at(const key_type& key) const {
    auto it = find(key);
    if (it != end()) {
      return it->second;
    } else {
      throw std::out_of_range();
    }
  }
  mapped_type& operator[](const key_type& key) {
    auto it = find(key);
    if (it != end()) {
      return it->second;
    }
    auto insert_it = insert(std::make_pair(key, value_type()));
    assert(insert_it.second);
    return insert_it.first->second;
  }
  size_type count(const key_type& key) const { return mht_.count(key); }
  iterator find(const key_type& key) { return mht_.find(key); }
  const_iterator find(const key_type& key) const { return mht_.find(key); }
  bool contains(const key_type& key) const { return mht_.contains(key); }
  std::pair<iterator, iterator> equal_range(const key_type& key) { return mht_.equal_range(unique_, key); }

  // Bucket interface
  local_iterator begin(size_type n) { return mht_.begin(n); }
  const_local_iterator begin(size_type n) const { return mht_.begin(n); }
  const_local_iterator cbegin(size_type n) const { return mht_.cbegin(n); }
  local_iterator end(size_type n) { return mht_.end(n); }
  const_local_iterator end(size_type n) const { return mht_.end(n); }
  const_local_iterator cend(size_type n) const { return mht_.cend(n); }
  size_type bucket_count() const { return mht_.bucket_count(); }
  size_type bucket_size(size_type n) const { return mht_.bucket_size(n); }
  size_type bucket(const key_type& key) const { return mht_.bucket(key); }

  // Hash policy
  float load_factor() const { return mht_.load_factor(); }
  float max_load_factor() const { return mht_.max_load_factor(); }
  void max_load_factor(float factor) { mht_.max_load_factor(factor); }
  void rehash(size_type n) { mht_.rehash(n); }
  void reserve(size_type n) { return mht_.reserve(n); }

  // Observers
  hasher hash_function() const { return mht_.hash_function(); }
  key_equal key_eq() const { return mht_.key_eq(); }

 private:
  MapHashTable mht_;
};

template <typename Key, typename T, typename Hash = std::hash<Key>, typename KeyEqual = std::equal_to<Key>>
using unordered_map = typename UnorderedMap<Key, T, Hash, KeyEqual>;

}  // namespace gtl