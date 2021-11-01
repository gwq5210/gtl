/**
 * @file gtl_unordered_map.h
 * @author gwq5210 (gwq5210@qq.com)
 * @brief unordered_map的实现
 * @date 2021-11-01
 * 
 * @copyright Copyright (c) 2021. All rights reserved.
 */

#include "gtl_hash_table.h"

namespace gtl {

template <typename Key, typename T, typename Hash = std::hash<Key>, typename KeyEquel = std::equal_to<Key>>
class UnorderedMap {
 public:
  using key_type = Key;
  using mapped_type = T,
  using value_type = std::pair<const Key, T>;
  using MapHashTable = HashTable<key_type, value_type, MapKeyFunc, Hash, KeyEquel>;
  using size_type = typename MapHashTable::size_type;
  using difference_type = typename MapHashTable::difference_type;
  using hasher = typename MapHashTable::hasher;
  using key_equal = typename MapHashTable::key_equal;
  using allocator_type = typename MapHashTable::allocator_type;
  using reference = value_type&;
  using const_reference = const value_type&;
  using pointer = value_type*;
  using const_pointer = const value_type*;
  using iterator = typename MapHashTable::iterator;
  using const_iterator = typename MapHashTable::const_iterator;
  using local_iterator = typename MapHashTable::local_iterator;
  using const_local_iterator = typename MapHashTable::const_local_iterator;

 private:
  MapHashTable mht_;
};

} // namespace gtl