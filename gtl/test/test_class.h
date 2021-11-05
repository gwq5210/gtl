/**
 * @file test_class.h
 * @author gwq5210 (gwq5210@qq.com)
 * @brief 用于测试的类
 * @date 2021-11-04
 * 
 * @copyright Copyright (c) 2021. All rights reserved.
 */

#pragma once

#include <cstdio>

#include <string>

namespace gtl {

namespace test {

class Person {
 public:
  Person(const std::string& name = "") : name_(name) {
    printf("person %s constructed\n", name_.c_str());
  }
  ~Person() {
    printf("person %s destroyed\n", name_.c_str());
  }

  std::string& name() { return name_; }
  const std::string& name() const { return name_; }
  void set_name(const std::string& name) { name_ = name; }

 private:
  std::string name_;
};

class Student : public Person {
 public:
  Student(const std::string& name = "", double score = 0.0) : Person(name), score_(score) {}

  double score() const { return score_; }
  double& score() { return score_; }
  void set_score(double score) { score_ = score; }

 private:
  double score_;
};

}  // namespace test

} // namespace gtl