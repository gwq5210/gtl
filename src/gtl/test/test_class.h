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

#include "gtl/memory/smart_pointers.h"

namespace gtl {

namespace test {

class Teacher;

class Person {
 public:
  Person(const std::string& name = "") : name_(name) { printf("person[%p] %s constructed\n", this, name_.c_str()); }
  ~Person() { printf("person[%p] %s destroyed\n", this, name_.c_str()); }

  std::string& name() { return name_; }
  const std::string& name() const { return name_; }
  void set_name(const std::string& name) { name_ = name; }

 private:
  std::string name_;
};

class Student : public Person {
 public:
  Student(const std::string& name = "", double score = 0.0) : Person(name), score_(score) {}
  Student(const Student& other) = delete;
  Student(Student&& other) = default;

  Student& operator=(const Student& other) = delete;
  Student& operator=(Student&& other) = default;

  double score() const { return score_; }
  double& score() { return score_; }
  void set_score(double score) { score_ = score; }
  const SharedPtr<Teacher>& teacher() { return teacher_; }
  void set_teacher(const SharedPtr<Teacher>& teacher) { teacher_ = teacher; }

 private:
  double score_;
  SharedPtr<Teacher> teacher_;
};

class Teacher : public Person {
 public:
  Teacher(const std::string& name = "", int id = 0) : Person(name), id_(id) {}
  Teacher(const Teacher& other) = delete;
  Teacher(Teacher&& other) = default;

  Teacher& operator=(const Teacher& other) = delete;
  Teacher& operator=(Teacher&& other) = default;

  int id() const { return id_; }
  int& id() { return id_; }
  void set_id(int id) { id_ = id; }
  const SharedPtr<Student>& student() const { return student_; }
  void set_student(const SharedPtr<Student>& student) { student_ = student; }
  SharedPtr<Student> student_weak() const { return student_weak_.lock(); }
  void set_student_weak(const SharedPtr<Student>& student) { student_weak_ = student; }

 private:
  int id_;
  SharedPtr<Student> student_;
  WeakPtr<Student> student_weak_;
};

}  // namespace test

}  // namespace gtl