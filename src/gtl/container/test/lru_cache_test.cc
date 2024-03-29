/**
 * @file lru_cache_test.cc
 * @author gwq5210 (gwq5210@qq.com)
 * @brief LRUCache单元测试
 * @date 2021-11-14
 *
 * @copyright Copyright (c) 2021. All rights reserved.
 */

#include "gtest/gtest.h"

#include "gtl/container/lru_cache.h"
#include "gtl/container/vector.h"

using gtl::LRUSet;

TEST(LRUCacheTest, LRUCacheTest) {
  gtl::vector<int> arr = {4, 7, 0, 7, 1, 0, 1, 2, 1, 2, 6};
  gtl::vector<gtl::vector<int>> res = {
      {4},          {7, 4},          {0, 7, 4},       {7, 0, 4},       {1, 7, 0, 4},    {0, 1, 7, 4},
      {1, 0, 7, 4}, {2, 1, 0, 7, 4}, {1, 2, 0, 7, 4}, {2, 1, 0, 7, 4}, {6, 2, 1, 0, 7},
  };
  ASSERT_EQ(res.size(), arr.size());
  {
    int capacity = 5;
    LRUSet<int> lru_set(capacity);
    for (size_t i = 0; i < arr.size(); ++i) {
      int v = arr[i];
      lru_set.Set(v);
      EXPECT_TRUE(std::equal(res[i].begin(), res[i].end(), lru_set.begin()));
    }
    EXPECT_EQ(lru_set.Size(), capacity);
  }
}