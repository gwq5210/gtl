/**
 * @file hash_table_test.cc
 * @author gwq5210 (gwq5210@qq.com)
 * @brief hash_table的单元测试
 * @date 2021-10-29
 * 
 * @copyright Copyright (c) 2021. All rights reserved.
 */

#include <cstdio>

#include <unordered_map>

#include "gtest/gtest.h"

#include "gtl_algorithm.h"
#include "gtl_hash_table.h"
#include "gtl_iterator.h"
#include "gtl_vector.h"

using gtl::HashTable;
using gtl::Vector;

using IIMapValue = std::pair<int, int>;
using ISet = HashTable<int, int, gtl::SetKeyFunc<int, int>>;
using IIMap = HashTable<int, IIMapValue, gtl::MapKeyFunc<int, int>>;

void print_iset(const ISet& iset) {
  printf("iset size: %zu, [", iset.size());
  for (auto it = iset.begin(); it != iset.end(); ++it) {
    if (it != iset.begin()) {
      printf(",");
    }
    printf("%d", *it);
  }
  printf("]\n");
  for (size_t i = 0; i < iset.bucket_count(); ++i) {
    if (iset.bucket_size(i) == 0) {
      continue;
    }
    printf("bucket(%zu) size: %zu = [", i, iset.bucket_size(i));
    for (auto it = iset.begin(i); it != iset.end(i); ++it) {
      if (it != iset.begin(i)) {
        printf(",");
      }
      printf("%d", *it);
    }
    printf("]\n");
  }
}

TEST(hash_table_test, constructor_assign_test) {
  int n = 10000;
  Vector<IIMapValue> vec;
  for (int i = 0; i < n; i++) {
    vec.emplace_back(std::make_pair(i, i));
    vec.emplace_back(std::make_pair(i, i));
  }

  // map
  {
    IIMap iimap;
    EXPECT_EQ(iimap, IIMap());
    EXPECT_EQ(iimap.size(), 0);
    EXPECT_EQ(iimap.empty(), true);
    EXPECT_EQ(iimap.bucket_count(), IIMap::min_bucket_size_);
    iimap.insert(vec.begin(), vec.end());
    EXPECT_EQ(iimap.size(), 2 * n);
    EXPECT_EQ(iimap.empty(), false);
    iimap.clear();
    EXPECT_EQ(iimap.size(), 0);
    EXPECT_EQ(iimap.empty(), true);
    iimap.insert(true, vec.begin(), vec.end());
    EXPECT_EQ(iimap.size(), n);
    EXPECT_EQ(iimap.empty(), false);
  }

  {
    IIMap iimap(n);
    EXPECT_EQ(iimap, IIMap());
    EXPECT_EQ(iimap.size(), 0);
    EXPECT_EQ(iimap.empty(), true);
    EXPECT_EQ(iimap.bucket_count(), gtl::next2power(n));
    iimap.insert(vec.begin(), vec.end());
    EXPECT_EQ(iimap.size(), 2 * n);
    EXPECT_EQ(iimap.empty(), false);
    iimap.clear();
    EXPECT_EQ(iimap.size(), 0);
    EXPECT_EQ(iimap.empty(), true);
    iimap.insert(true, vec.begin(), vec.end());
    EXPECT_EQ(iimap.size(), n);
    EXPECT_EQ(iimap.empty(), false);
  }

  {
    IIMap iimap(true, vec.begin(), vec.end());
    EXPECT_NE(iimap, IIMap());
    EXPECT_EQ(iimap.size(), n);
    EXPECT_EQ(iimap.empty(), false);
    EXPECT_EQ(iimap.bucket_count(), gtl::next2power(2 * n));
    iimap.insert(vec.begin(), vec.end());
    EXPECT_EQ(iimap.size(), 3 * n);
    EXPECT_EQ(iimap.empty(), false);
    iimap.clear();
    EXPECT_EQ(iimap.size(), 0);
    EXPECT_EQ(iimap.empty(), true);
    iimap.insert(true, vec.begin(), vec.end());
    EXPECT_EQ(iimap.size(), n);
    EXPECT_EQ(iimap.empty(), false);
  }

  // set
  ISet iset;
  EXPECT_EQ(iset.size(), 0);
  EXPECT_EQ(iset.empty(), true);
  EXPECT_EQ(iset.bucket_count(), ISet::min_bucket_size_);
}

TEST(hash_table_test, modifiers_lookup_test) {
  int n = 10000;
  ISet iset;
  for (int i = 0; i < n; ++i) {
    iset.insert_unique(i);
    iset.insert_unique(i);
  }
  EXPECT_EQ(iset.size(), n);
  for (int i = 0; i < n; ++i) {
    auto res = iset.insert_equal(i);
    EXPECT_EQ(*res.first, i);
    EXPECT_EQ(res.second, true);
    res = iset.insert_equal(i);
    EXPECT_EQ(*res.first, i);
    EXPECT_EQ(res.second, true);
  }
  EXPECT_EQ(iset.size(), 3 * n);
  for (int i = 0; i < n; ++i) {
    EXPECT_EQ(iset.count(i), 3);
  }
  // print_iset(iset);
  for (int i = 0; i < n; ++i) {
    auto res = iset.equal_range(i);
    EXPECT_EQ(gtl::distance(res.first, res.second), 3);
    for (auto it = res.first; it != res.second; ++it) {
      EXPECT_EQ(*it, i);
    }
  }
  int erase_n = 3;
  for (int i = 0; i < erase_n; ++i) {
    iset.erase(i);
    EXPECT_EQ(iset.size(), 3 * (n - (i + 1)));
  }
  size_t old_size = iset.size();
  size_t before_bucket_count = iset.bucket_count();
  iset.rehash(n);
  EXPECT_EQ(iset.bucket_count(), before_bucket_count);
  for (int i = 0; i < n; ++i) {
    iset.insert_equal(i);
  }
  old_size += n;
  EXPECT_EQ(iset.size(), old_size);
  int erase_begin = iset.size() / 2;
  int erase_count = iset.size() / 3;
  iset.erase(gtl::next(iset.begin(), erase_begin), gtl::next(iset.begin(), erase_begin + erase_count));
  old_size -= erase_count;
  EXPECT_EQ(iset.size(), old_size);
}