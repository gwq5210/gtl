/**
 * @file logging_test.cc
 * @author gwq5210 (gwq5210@qq.com)
 * @brief 日志宏的单元测试
 * @date 2022-03-27
 * 
 * @copyright Copyright (c) 2022. All rights reserved.
 */

#include "gtest/gtest.h"

#include "gtl/logging.h"


TEST(LoggingTest, LoggingTest) {
  std::string name = "gwq5210";
  int val = 1000;
  int max_val = 100;
  GTL_ERROR("hello world! name: {}", name);
  GTL_ERROR_IF(val > max_val, "val {} must be less than {}", val, max_val);

  val = 10;
  GTL_ERROR_IF(val > max_val, "val {} must be less than {}", val, max_val);
}