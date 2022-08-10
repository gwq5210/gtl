/**
 * @file socket_address_test.cc
 * @author gwq5210 (gwq5210@qq.com)
 * @brief SocketAddress单元测试
 * @date 2022-08-08
 * 
 * @copyright Copyright (c) 2022. All rights reserved.
 */

#include "gtest/gtest.h"

#include "gtl/net/socket_address.h"

TEST(SocketAddressTest, SocketAddressTest) {
  {
    std::string address = "127.0.0.1:8080";
    gtl::net::SocketAddress socket_address;
    bool ret = socket_address.Parse(address);
    EXPECT_EQ(ret, true);
    EXPECT_EQ(socket_address.port(), 8080);
    EXPECT_EQ(socket_address.family(), AF_INET);
    EXPECT_EQ(socket_address.ToString(), address);
  }
}