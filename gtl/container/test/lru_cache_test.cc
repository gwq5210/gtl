/**
 * @file lru_cache_test.cc
 * @author gwq5210 (gwq5210@qq.com)
 * @brief LRUCache单元测试
 * @date 2021-11-14
 * 
 * @copyright Copyright (c) 2021. All rights reserved.
 */

#include "gtest/gtest.h"

#include "gtl/lru_cache.h"
#include "gtl/vector.h"

using gtl::LRUCache;

TEST(lru_cache_test, lru_cache_test) {
  gtl::vector<int> arr = {4, 7, 0, 7, 1, 0, 1, 2, 1, 2, 6};
  gtl::vector<gtl::vector<int>> res = {
    {4},
    {7, 4},
    {0, 7, 4},
    {7, 0, 4},
    {1, 7, 0, 4},
    {0, 1, 7, 4},
    {1, 0, 7, 4},
    {2, 1, 0, 7, 4},
    {1, 2, 0, 7, 4},
    {2, 1, 0, 7, 4},
    {6, 2, 1, 0, 7},
  };
  {
    int capacity = 5;
    LRUCache<int, int> lru_cache(capacity);
    for (int v: arr) {
      lru_cache.Set(v, v);
    }
    for (auto it = lru_cache.begin(); it != lru_cache.end(); ++it) {
      printf("%d\n", it->first);
    }
  }
}