/**
 * @file allocator_test.cc
 * @author gwq5210 (gwq5210@qq.com)
 * @brief allocator单元测试
 * @date 2022-08-30
 * 
 * @copyright Copyright (c) 2022. All rights reserved.
 */

#include "gtest/gtest.h"

#include "gtl/memory/allocator.h"

TEST(AllocatorTest, AllocatorTest) {
  gtl::GtlMemoryAllocator& allocator = gtl::GtlMemoryAllocator::GetInstance();
  {
    size_t size = 128;
    char* ptr = static_cast<char*>(allocator.Malloc(size));
    ASSERT_NE(ptr, nullptr);
    memset(ptr, 0, size);
    allocator.Free(ptr);
    GTL_INFO("{}", allocator.MemoryInfo());

    char* new_ptr = static_cast<char*>(allocator.Malloc(size));
    ASSERT_NE(ptr, nullptr);
    EXPECT_EQ(ptr, new_ptr);
    memset(ptr, 0, size);
    allocator.Free(ptr);
    GTL_INFO("{}", allocator.MemoryInfo());
  }
}