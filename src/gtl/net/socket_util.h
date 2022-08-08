/**
 * @file socket_util.h
 * @author gwq5210 (gwq5210@qq.com)
 * @brief 网络工具函数
 * @date 2022-08-08
 * 
 * @copyright Copyright (c) 2022. All rights reserved.
 */

#pragma once

#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#include <string>

#include "gtl/net/socket_address.h"

int ClientConnect(const SocketAddress& socket_address, int type = SOCK_STREAM);
int ServerListen(const SocketAddress& socket_address, int type = SOCK_STREAM);
ssize_t SendMsg(int sockfd, const std::string& msg);
std::string RecvMsg(int sockfd, size_t msg_size);
ssize_t SendMsg(int sockfd, const SocketAddress& socket_address, const std::string& msg);
std::string RecvMsg(int sockfd, const SocketAddress& socket_address, size_t msg_size);
bool GetLocalAddr(int sockfd, SocketAddress& socket_address);
bool GetPeerAddr(int sockfd, SocketAddress& socket_address);
