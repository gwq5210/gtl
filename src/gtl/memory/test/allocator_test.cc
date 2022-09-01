/**
 * @file allocator_test.cc
 * @author gwq5210 (gwq5210@qq.com)
 * @brief allocator单元测试
 * @date 2022-08-30
 *
 * @copyright Copyright (c) 2022. All rights reserved.
 */

#include "gtest/gtest.h"

#include "gtl/memory/simple_allocator.h"

TEST(AllocatorTest, AllocatorTest) {
  GTL_SET_LEVEL(gtl::LogLevel::kDebug);
  gtl::SimpleMemoryAllocator& allocator = gtl::SimpleMemoryAllocator::GetInstance();
  {
    size_t size = 128;
    GTL_INFO("new 128 begin\n{}", allocator.MemoryInfo());
    char* ptr = static_cast<char*>(allocator.Malloc(size));
    ASSERT_NE(ptr, nullptr);
    memset(ptr, 0, size);
    GTL_INFO("new 128 end\n{}", allocator.MemoryInfo());

    allocator.Free(ptr);
    GTL_INFO("free 128 end\n{}", allocator.MemoryInfo());

    GTL_INFO("new 128 begin");
    char* new_ptr = static_cast<char*>(allocator.Malloc(size));
    ASSERT_NE(ptr, nullptr);
    EXPECT_EQ(ptr, new_ptr);
    memset(ptr, 0, size);
    GTL_INFO("new 128 end\n{}", allocator.MemoryInfo());

    allocator.Free(ptr);
    GTL_INFO("free 128 end\n{}", allocator.MemoryInfo());

    size_t new_size = 256;
    char* ptr2 = static_cast<char*>(allocator.Malloc(new_size));
    ASSERT_NE(ptr2, nullptr);
    ASSERT_NE(ptr2, ptr);
    memset(ptr2, 0, new_size);
    GTL_INFO("new 256 end\n{}", allocator.MemoryInfo());

    allocator.Free(ptr2);
    GTL_INFO("free 256 end\n{}", allocator.MemoryInfo());

    char* ptr3 = static_cast<char*>(allocator.Malloc(size));
    ASSERT_NE(ptr3, nullptr);
    GTL_INFO("new 128 end\n{}", allocator.MemoryInfo());

    char* ptr4 = static_cast<char*>(allocator.Malloc(size));
    ASSERT_NE(ptr4, nullptr);
    GTL_INFO("new 128 end\n{}", allocator.MemoryInfo());
    allocator.Free(ptr3);
    GTL_INFO("free 128 end\n{}", allocator.MemoryInfo());
    allocator.Free(ptr4);
    GTL_INFO("free all\n{}", allocator.MemoryInfo());
  }

  {
    size_t size = 129;
    GTL_INFO("new 129 begin\n{}", allocator.MemoryInfo());
    char* ptr = static_cast<char*>(allocator.Malloc(size));
    ASSERT_NE(ptr, nullptr);
    memset(ptr, 0, size);
    GTL_INFO("new 129 end\n{}", allocator.MemoryInfo());

    size_t new_size = 256;
    char* realloc_ptr = static_cast<char*>(allocator.Realloc(ptr, new_size));
    ASSERT_NE(realloc_ptr, nullptr);
    EXPECT_EQ(realloc_ptr, ptr);
    GTL_INFO("realloc 256 end\n{}", allocator.MemoryInfo());

    allocator.Free(realloc_ptr);
    GTL_INFO("free 256 end\n{}", allocator.MemoryInfo());
  }
  GTL_SET_LEVEL(gtl::LogLevel::kInfo);
}