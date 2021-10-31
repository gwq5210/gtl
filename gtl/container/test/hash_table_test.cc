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

using gtl::HashTable;

TEST(hash_table_test, constructor_test) {
  int n = 100;
  HashTable<int, int, gtl::SetKeyFunc<int, int>> ht;
  std::unordered_map<int, int> um;
  std::unordered_map<int, int> um2;
  printf("bucket_count: %zu\n", ht.bucket_count());
  for (int i = 0; i < n; ++i) {
    ht.insert_unique(i);
  }
  for (int i = 0; i < n; ++i) {
    ht.insert_equal(i);
  }
  printf("%p %p\n", ht.begin().node, ht.end().node);
  printf("count %d is %zu\n", 10, ht.count_equal(10));
  for (auto it = ht.begin(); it != ht.end(); ++it) {
    printf("%d ", *it);
  }
  printf("\n");
  for (size_t i = 0; i < ht.bucket_count(); ++i) {
    if (ht.bucket_size(i) == 0) {
      continue;
    }
    printf("%p %p\n", ht.begin(i).node, ht.end(i).node);
    printf("bucket(%zu) size: %zu = [", i, ht.bucket_size(i));
    for (auto it = ht.begin(i); it != ht.end(i); ++it) {
      printf("%d,", *it);
    }
    printf("]\n");
  }
  ht.erase(83);
  ht.erase(3);
  ht.erase(0);
  ht.rehash(32);
  for (auto it = ht.begin(); it != ht.end(); ++it) {
    printf("%d ", *it);
  }
  printf("\n");
  printf("insert begin\n");
  for (int i = 0; i < n; ++i) {
    ht.insert_equal(i);
  }
  printf("insert done.\n");
  for (auto it = ht.begin(); it != ht.end(); ++it) {
    printf("%d ", *it);
  }
  printf("\n");
  for (size_t i = 0; i < ht.bucket_count(); ++i) {
    if (ht.bucket_size(i) == 0) {
      continue;
    }
    printf("%p %p\n", ht.begin(i).node, ht.end(i).node);
    printf("bucket(%zu) size: %zu = [", i, ht.bucket_size(i));
    for (auto it = ht.begin(i); it != ht.end(i); ++it) {
      printf("%d,", *it);
    }
    printf("]\n");
  }
  printf("erase: ");
  for (auto it = gtl::next(ht.begin(), 10); it != gtl::next(ht.begin(), 15); ++it) {
    printf("%d ", *it);
  }
  printf("\n");
  ht.erase(gtl::next(ht.begin(), 10), gtl::next(ht.begin(), 15));
  for (size_t i = 0; i < ht.bucket_count(); ++i) {
    if (ht.bucket_size(i) == 0) {
      continue;
    }
    printf("%p %p\n", ht.begin(i).node, ht.end(i).node);
    printf("bucket(%zu) size: %zu = [", i, ht.bucket_size(i));
    for (auto it = ht.begin(i); it != ht.end(i); ++it) {
      printf("%d,", *it);
    }
    printf("]\n");
  }
  EXPECT_EQ(ht.size(), 2 * n);
}