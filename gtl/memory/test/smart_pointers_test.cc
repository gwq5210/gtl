/**
 * @file smart_pointers_test.cc
 * @author gwq5210 (gwq5210@qq.com)
 * @brief 智能指针测试
 * @date 2021-11-04
 * 
 * @copyright Copyright (c) 2021. All rights reserved.
 */

#include <cstdio>

#include <memory>
#include <vector>

#include "gtest/gtest.h"

#include "gtl/vector.h"
#include "gtl/smart_pointers.h"
#include "gtl/test_class.h"

using gtl::test::Person;
using gtl::test::Student;
using gtl::test::Teacher;

TEST(smart_pointers_test, auto_ptr_test) {
  {
    gtl::auto_ptr<Person> p;
    EXPECT_EQ(p.get(), nullptr);
    p = p;
    EXPECT_EQ(p.get(), nullptr);
  }

  {
    gtl::auto_ptr<Person> p(nullptr);
    EXPECT_EQ(p.get(), nullptr);
    p = p;
    EXPECT_EQ(p.get(), nullptr);
  }

  {
    std::string name = "auto_ptr1";
    gtl::auto_ptr<Person> p(gtl::make_auto_ptr<Person>(name));
    auto* p_bak = p.get();
    EXPECT_NE(p.get(), nullptr);
    EXPECT_EQ(name, p->name());
    p = p;
    EXPECT_NE(p.get(), nullptr);
    EXPECT_EQ(name, p->name());

    gtl::auto_ptr<Person> p2(p);
    EXPECT_EQ(p.get(), nullptr);
    EXPECT_EQ(p2.get(), p_bak);
    p2 = p2;
    EXPECT_EQ(p2.get(), p_bak);
  }
  
  {
    std::string name2 = "auto_ptr2";
    std::string name3 = "auto_ptr3";
    gtl::auto_ptr<Person> p(gtl::make_auto_ptr<Person>(name2));
    auto* p_bak = p.get();
    EXPECT_NE(p.get(), nullptr);
    EXPECT_EQ(name2, p->name());
    p = p;
    EXPECT_NE(p.get(), nullptr);
    EXPECT_EQ(name2, p->name());

    gtl::auto_ptr<Person> p2(gtl::make_auto_ptr<Person>(name3));
    EXPECT_NE(p.get(), nullptr);
    EXPECT_EQ(name3, p2->name());
    printf("before p assign\n");
    p2 = p;
    printf("after p assign\n");
    EXPECT_EQ(p.get(), nullptr);
    EXPECT_EQ(p2.get(), p_bak);
    EXPECT_EQ(name2, p2->name());
  }

  {
    std::string name = "student1";
    double score = 100.0;
    gtl::auto_ptr<Student> p(gtl::make_auto_ptr<Student>(name, score));
    auto* p_bak = p.get();
    EXPECT_NE(p.get(), nullptr);
    EXPECT_EQ(name, p->name());
    EXPECT_EQ(score, p->score());
    gtl::auto_ptr<Person> p2(p);
    EXPECT_EQ(p.get(), nullptr);
    EXPECT_EQ(p2.get(), p_bak);
    EXPECT_EQ(name, p2->name());
    p2 = p2;
    EXPECT_EQ(p2.get(), p_bak);
    EXPECT_EQ(name, p2->name());

    gtl::auto_ptr<Person> p3;
    p3 = p2;
    EXPECT_EQ(p2.get(), nullptr);
    EXPECT_EQ(p3.get(), p_bak);
    EXPECT_EQ(name, p3->name());
    p3 = p3;
    EXPECT_EQ(p3.get(), p_bak);
    EXPECT_EQ(name, p3->name());
  }
}

TEST(smart_pointers_test, unique_ptr_test) {
  {
    gtl::unique_ptr<Person> p;
    EXPECT_EQ(p.get(), nullptr);
    EXPECT_EQ(bool(p), false);
    p = std::move(p);
    EXPECT_EQ(p.get(), nullptr);
    EXPECT_EQ(bool(p), false);
  }

  {
    gtl::unique_ptr<Person> p(nullptr);
    EXPECT_EQ(p.get(), nullptr);
    EXPECT_EQ(bool(p), false);
    p = std::move(p);
    EXPECT_EQ(p.get(), nullptr);
    EXPECT_EQ(bool(p), false);
  }

  {
    std::string name = "unique_ptr1";
    gtl::unique_ptr<Person> p(gtl::make_unique<Person>(name));
    auto* p_bak = p.get();
    EXPECT_NE(p.get(), nullptr);
    EXPECT_EQ(bool(p), true);
    EXPECT_EQ(name, p->name());
    p = std::move(p);
    EXPECT_NE(p.get(), nullptr);
    EXPECT_EQ(bool(p), true);
    EXPECT_EQ(name, p->name());

    gtl::unique_ptr<Person> p2(std::move(p));
    EXPECT_EQ(p.get(), nullptr);
    EXPECT_EQ(bool(p), false);
    EXPECT_EQ(p2.get(), p_bak);
    EXPECT_EQ(bool(p2), true);
    EXPECT_EQ(name, p2->name());
    p2 = std::move(p2);
    EXPECT_EQ(p2.get(), p_bak);
    EXPECT_EQ(bool(p2), true);
    EXPECT_EQ(name, p2->name());
  }
  
  {
    std::string name2 = "unique_ptr2";
    std::string name3 = "unique_ptr3";
    gtl::unique_ptr<Person> p(gtl::make_unique<Person>(name2));
    auto* p_bak = p.get();
    EXPECT_NE(p.get(), nullptr);
    EXPECT_EQ(bool(p), true);
    EXPECT_EQ(name2, p->name());
    p = std::move(p);
    EXPECT_NE(p.get(), nullptr);
    EXPECT_EQ(bool(p), true);
    EXPECT_EQ(name2, p->name());

    gtl::unique_ptr<Person> p2(gtl::make_unique<Person>(name3));
    EXPECT_NE(p2.get(), nullptr);
    EXPECT_EQ(bool(p2), true);
    EXPECT_EQ(name3, p2->name());
    printf("before p move\n");
    p2 = std::move(p);
    printf("after p move\n");
    EXPECT_EQ(p.get(), nullptr);
    EXPECT_EQ(p2.get(), p_bak);
    EXPECT_EQ(name2, p2->name());
    EXPECT_EQ(bool(p), false);
    EXPECT_EQ(bool(p2), true);
    p2 = std::move(p2);
    EXPECT_EQ(p2.get(), p_bak);
    EXPECT_EQ(bool(p2), true);
    EXPECT_EQ(name2, p2->name());
  }

  {
    std::string name = "student1";
    double score = 100.0;
    gtl::unique_ptr<Student> p(gtl::make_unique<Student>(name, score));
    auto* p_bak = p.get();
    EXPECT_NE(p.get(), nullptr);
    EXPECT_EQ(name, p->name());
    EXPECT_EQ(score, p->score());
    gtl::unique_ptr<Person> p2(std::move(p));
    EXPECT_EQ(p.get(), nullptr);
    EXPECT_EQ(p2.get(), p_bak);
    EXPECT_EQ(name, p2->name());
    p2 = std::move(p2);
    EXPECT_EQ(p2.get(), p_bak);
    EXPECT_EQ(name, p2->name());

    gtl::unique_ptr<Person> p3;
    p3 = std::move(p2);
    EXPECT_EQ(p2.get(), nullptr);
    EXPECT_EQ(p3.get(), p_bak);
    EXPECT_EQ(name, p3->name());
    p3 = std::move(p2);
    EXPECT_EQ(p3.get(), nullptr);
  }

  {
    int n = 10;
    gtl::unique_ptr<Person[]> p = gtl::make_unique<Person[]>(n);
    Person* p_bak = p.get();
    EXPECT_NE(p.get(), nullptr);
    EXPECT_EQ(bool(p), true);
    p = std::move(p);
    EXPECT_NE(p.get(), nullptr);
    EXPECT_EQ(bool(p), true);

    for (int i = 0; i < n; ++i) {
      p[i].set_name(std::to_string(i));
      EXPECT_EQ(std::to_string(i), p[i].name());
    }

    gtl::unique_ptr<Person[]> p2(std::move(p));
    EXPECT_EQ(p2.get(), p_bak);
    EXPECT_EQ(bool(p), false);
    EXPECT_EQ(bool(p2), true);
    for (int i = 0; i < n; ++i) {
      EXPECT_EQ(std::to_string(i), p2[i].name());
    }
    p2 = std::move(p2);
    EXPECT_EQ(p2.get(), p_bak);
    EXPECT_EQ(bool(p2), true);
    for (int i = 0; i < n; ++i) {
      EXPECT_EQ(std::to_string(i), p2[i].name());
    }

    gtl::unique_ptr<Person[]> p3;
    EXPECT_EQ(p.get(), nullptr);
    EXPECT_EQ(bool(p), false);
    p3 = std::move(p2);
    EXPECT_EQ(p3.get(), p_bak);
    EXPECT_EQ(bool(p2), false);
    EXPECT_EQ(bool(p3), true);
    for (int i = 0; i < n; ++i) {
      EXPECT_EQ(std::to_string(i), p3[i].name());
    }
    p3 = std::move(p3);
    printf("after move p3\n");
    EXPECT_EQ(p3.get(), p_bak);
    EXPECT_EQ(bool(p3), true);
    for (int i = 0; i < n; ++i) {
      EXPECT_EQ(std::to_string(i), p3[i].name());
    }
  }
}

TEST(smart_pointers_test, shared_ptr_test) {
  {
    gtl::shared_ptr<Person> p;
    EXPECT_EQ(p.get(), nullptr);
    EXPECT_EQ(p.use_count(), 0);
    p = p;
    EXPECT_EQ(p.get(), nullptr);
    EXPECT_EQ(p.use_count(), 0);
  }

  {
    gtl::shared_ptr<Person> p(nullptr);
    EXPECT_EQ(p.get(), nullptr);
    EXPECT_EQ(p.use_count(), 0);
    p = p;
    EXPECT_EQ(p.get(), nullptr);
    EXPECT_EQ(p.use_count(), 0);
  }

  {
    std::string name = "shared_ptr1";
    gtl::shared_ptr<Person> p(gtl::make_shared<Person>(name));
    auto* p_bak = p.get();
    EXPECT_NE(p.get(), nullptr);
    EXPECT_EQ(name, p->name());
    EXPECT_EQ(p.use_count(), 1);
    p = p;
    EXPECT_NE(p.get(), nullptr);
    EXPECT_EQ(name, p->name());
    EXPECT_EQ(p.use_count(), 1);

    gtl::shared_ptr<Person> p2(p);
    EXPECT_EQ(p.get(), p_bak);
    EXPECT_EQ(p2.get(), p_bak);
    EXPECT_EQ(p.use_count(), 2);
    EXPECT_EQ(p2.use_count(), 2);
    p2 = p2;
    EXPECT_EQ(p.get(), p_bak);
    EXPECT_EQ(p2.get(), p_bak);
    EXPECT_EQ(p.use_count(), 2);
    EXPECT_EQ(p2.use_count(), 2);
  }
  
  {
    std::string name2 = "shared_ptr2";
    std::string name3 = "shared_ptr3";
    gtl::shared_ptr<Person> p(gtl::make_shared<Person>(name2));
    auto* p_bak = p.get();
    EXPECT_EQ(p.use_count(), 1);
    EXPECT_NE(p.get(), nullptr);
    EXPECT_EQ(name2, p->name());
    p = p;
    EXPECT_EQ(p.use_count(), 1);
    EXPECT_NE(p.get(), nullptr);
    EXPECT_EQ(name2, p->name());

    gtl::shared_ptr<Person> p2(gtl::make_shared<Person>(name3));
    EXPECT_EQ(p.use_count(), 1);
    EXPECT_EQ(p2.use_count(), 1);
    EXPECT_EQ(p.get(), p_bak);
    EXPECT_EQ(name3, p2->name());
    printf("before p assign\n");
    p2 = p;
    printf("after p assign\n");
    EXPECT_EQ(p.use_count(), 2);
    EXPECT_EQ(p2.use_count(), 2);
    EXPECT_EQ(p.get(), p_bak);
    EXPECT_EQ(p2.get(), p_bak);
    EXPECT_EQ(name2, p2->name());
  }

  {
    std::string name = "student1";
    double score = 100.0;
    gtl::shared_ptr<Student> p(gtl::make_shared<Student>(name, score));
    auto* p_bak = p.get();
    EXPECT_EQ(p.use_count(), 1);
    EXPECT_NE(p.get(), nullptr);
    EXPECT_EQ(name, p->name());
    EXPECT_EQ(score, p->score());
    gtl::shared_ptr<Person> p2(p);
    EXPECT_EQ(p.use_count(), 2);
    EXPECT_EQ(p2.use_count(), 2);
    EXPECT_EQ(p.get(), p_bak);
    EXPECT_EQ(p2.get(), p_bak);
    EXPECT_EQ(name, p2->name());
    p2 = std::move(p2);
    EXPECT_EQ(p.use_count(), 2);
    EXPECT_EQ(p2.use_count(), 2);
    EXPECT_EQ(p.get(), p_bak);
    EXPECT_EQ(p2.get(), p_bak);
    EXPECT_EQ(name, p2->name());

    gtl::shared_ptr<Person> p3;
    EXPECT_EQ(p3.use_count(), 0);
    p3 = p2;
    EXPECT_EQ(p.use_count(), 3);
    EXPECT_EQ(p2.use_count(), 3);
    EXPECT_EQ(p3.use_count(), 3);
    EXPECT_EQ(p.get(), p_bak);
    EXPECT_EQ(p2.get(), p_bak);
    EXPECT_EQ(p3.get(), p_bak);
    EXPECT_EQ(name, p3->name());
    p3 = p3;
    EXPECT_EQ(p.use_count(), 3);
    EXPECT_EQ(p2.use_count(), 3);
    EXPECT_EQ(p3.use_count(), 3);
    EXPECT_EQ(p.get(), p_bak);
    EXPECT_EQ(p2.get(), p_bak);
    EXPECT_EQ(p3.get(), p_bak);
    EXPECT_EQ(name, p3->name());
  }

  {
    int n = 10;
    gtl::shared_ptr<Student[]> p = gtl::make_shared<Student[]>(n);
    Student* p_bak = p.get();
    EXPECT_EQ(p.use_count(), 1);
    EXPECT_NE(p.get(), nullptr);
    EXPECT_EQ(bool(p), true);
    p = std::move(p);
    EXPECT_EQ(p.use_count(), 1);
    EXPECT_NE(p.get(), nullptr);
    EXPECT_EQ(bool(p), true);

    for (int i = 0; i < n; ++i) {
      p[i].set_name(std::to_string(i));
      EXPECT_EQ(std::to_string(i), p[i].name());
    }

    gtl::shared_ptr<Student[]> p2(std::move(p));
    EXPECT_EQ(p.use_count(), 0);
    EXPECT_EQ(p2.use_count(), 1);
    EXPECT_EQ(p2.get(), p_bak);
    EXPECT_EQ(bool(p), false);
    EXPECT_EQ(bool(p2), true);
    for (int i = 0; i < n; ++i) {
      EXPECT_EQ(std::to_string(i), p2[i].name());
    }
    p2 = p2;
    EXPECT_EQ(p2.use_count(), 1);
    EXPECT_EQ(p2.get(), p_bak);
    EXPECT_EQ(bool(p2), true);
    for (int i = 0; i < n; ++i) {
      EXPECT_EQ(std::to_string(i), p2[i].name());
    }

    gtl::shared_ptr<Student[]> p3;
    EXPECT_EQ(p3.use_count(), 0);
    EXPECT_EQ(p.get(), nullptr);
    EXPECT_EQ(bool(p), false);
    p3 = p2;
    EXPECT_EQ(p2.use_count(), 2);
    EXPECT_EQ(p3.use_count(), 2);
    EXPECT_EQ(p2.get(), p_bak);
    EXPECT_EQ(p3.get(), p_bak);
    EXPECT_EQ(bool(p2), true);
    EXPECT_EQ(bool(p3), true);
    for (int i = 0; i < n; ++i) {
      EXPECT_EQ(std::to_string(i), p3[i].name());
    }
    p3 = std::move(p2);
    printf("after move p3\n");
    EXPECT_EQ(p2.use_count(), 0);
    EXPECT_EQ(p3.use_count(), 1);
    EXPECT_EQ(p2.get(), nullptr);
    EXPECT_EQ(p3.get(), p_bak);
    EXPECT_EQ(bool(p2), false);
    EXPECT_EQ(bool(p3), true);
    for (int i = 0; i < n; ++i) {
      EXPECT_EQ(std::to_string(i), p3[i].name());
    }

    gtl::shared_ptr<Teacher> teacher(gtl::make_shared<Teacher>("teacher", 10));
    gtl::shared_ptr<Student> student(gtl::make_shared<Student>("student", 100.0));
    printf("%s count %zu\n", teacher->name().c_str(), teacher.use_count());
    printf("%s count %zu\n", student->name().c_str(), student.use_count());
    EXPECT_EQ(teacher.use_count(), 1);
    EXPECT_EQ(student.use_count(), 1);
    teacher->set_student(student);
    student->set_teacher(teacher);
    printf("%s count %zu\n", teacher->name().c_str(), teacher.use_count());
    printf("%s count %zu\n", student->name().c_str(), student.use_count());
    EXPECT_EQ(teacher.use_count(), 2);
    EXPECT_EQ(student.use_count(), 2);
  }

  {
    gtl::WeakPtr<Student> student_weak_bak;
    {
      gtl::shared_ptr<Teacher> teacher_weak(gtl::make_shared<Teacher>("teacher_weak", 10));
      gtl::shared_ptr<Student> student_weak(gtl::make_shared<Student>("student_weak", 100.0));
      printf("%s count %zu\n", teacher_weak->name().c_str(), teacher_weak.use_count());
      printf("%s count %zu\n", student_weak->name().c_str(), student_weak.use_count());
      EXPECT_EQ(teacher_weak.use_count(), 1);
      EXPECT_EQ(student_weak.use_count(), 1);
      teacher_weak->set_student_weak(student_weak);
      student_weak->set_teacher(teacher_weak);
      printf("%s count %zu\n", teacher_weak->name().c_str(), teacher_weak.use_count());
      printf("%s count %zu\n", student_weak->name().c_str(), student_weak.use_count());
      EXPECT_EQ(teacher_weak.use_count(), 2);
      EXPECT_EQ(student_weak.use_count(), 1);
      student_weak_bak = student_weak;
      EXPECT_EQ(student_weak_bak.use_count(), 1);
      EXPECT_EQ(student_weak_bak.expired(), false);
      EXPECT_EQ(bool(student_weak_bak.lock()), true);
      printf("student_weak ptr %p\n", student_weak.get());
    }
    printf("weak test\n");
    EXPECT_EQ(student_weak_bak.use_count(), 0);
    EXPECT_EQ(student_weak_bak.expired(), true);
    auto stucent_shared_ptr = student_weak_bak.lock();
    printf("student_weak ptr %p\n", stucent_shared_ptr.get());
    EXPECT_EQ(bool(stucent_shared_ptr), false);
  }
}
