/**
 * @file poller.h
 * @author gwq5210 (gwq5210@qq.com)
 * @brief 网络poll的封装
 * @date 2022-08-09
 * 
 * @copyright Copyright (c) 2022. All rights reserved.
 */

#pragma once

class PollerBase {
 public:
  virtual bool Init() = 0;
};

class Poller {
 private:
};