/**
 * @file adaptors_test.cc
 * @author gwq5210 (gwq5210@qq.com)
 * @brief 容器适配器测试，包括Stack, Queue, PriorityQueue
 * @date 2021-05-17
 *
 * @copyright Copyright (c) 2021. All rights reserved.
 */

#include <numeric>

#include "gtest/gtest.h"

#include "gtl/container/list.h"
#include "gtl/container/priority_queue.h"
#include "gtl/container/queue.h"
#include "gtl/container/stack.h"
#include "gtl/container/vector.h"

using gtl::List;
using gtl::PriorityQueue;
using gtl::Queue;
using gtl::Stack;
using gtl::Vector;

TEST(stack_test, stack_test) {
  int n = 0;
  {
    Stack<int> s;
    Stack<int> s_copy(s);
    EXPECT_EQ(s.size(), n);
    EXPECT_EQ(s.empty(), true);
    EXPECT_EQ(s, s_copy);
    EXPECT_EQ(s < s_copy, false);
    EXPECT_EQ(s > s_copy, false);
    EXPECT_EQ(s <= s_copy, true);
    EXPECT_EQ(s >= s_copy, true);
  }

  n = 102400;
  List<int> l(n);
  std::iota(l.begin(), l.end(), 0);
  {
    Stack<int> s(l.begin(), l.end());
    EXPECT_EQ(s.size(), n);
    EXPECT_EQ(s.empty(), false);
    for (int i = n - 1; i >= 0; --i) {
      EXPECT_EQ(s.top(), i);
      s.pop();
    }
    EXPECT_EQ(s.size(), 0);
    EXPECT_EQ(s.empty(), true);
  }
  {
    Stack<int> s;
    for (int i = 0; i < n; ++i) {
      s.push(i);
      EXPECT_EQ(s.top(), i);
    }
    EXPECT_EQ(s.size(), n);
    EXPECT_EQ(s.empty(), false);
    for (int i = n - 1; i >= 0; --i) {
      EXPECT_EQ(s.top(), i);
      s.pop();
    }
    EXPECT_EQ(s.size(), 0);
    EXPECT_EQ(s.empty(), true);
  }
  {
    Stack<int> s1(l.begin(), l.end());
    EXPECT_EQ(s1.size(), n);
    EXPECT_EQ(s1.empty(), false);
    Stack<int> s(std::move(s1));
    EXPECT_EQ(s1.size(), 0);
    EXPECT_EQ(s1.empty(), true);
    EXPECT_EQ(s.size(), n);
    EXPECT_EQ(s.empty(), false);
    for (int i = n - 1; i >= 0; --i) {
      EXPECT_EQ(s.top(), i);
      s.pop();
    }
    EXPECT_EQ(s.size(), 0);
    EXPECT_EQ(s.empty(), true);
  }
}

TEST(queue_test, queue_test) {
  int n = 0;
  {
    Queue<int> q;
    Queue<int> q_copy(q);
    EXPECT_EQ(q.size(), n);
    EXPECT_EQ(q.empty(), true);
    EXPECT_EQ(q, q_copy);
    EXPECT_EQ(q < q_copy, false);
    EXPECT_EQ(q > q_copy, false);
    EXPECT_EQ(q <= q_copy, true);
    EXPECT_EQ(q >= q_copy, true);
  }

  n = 102400;
  List<int> l(n);
  std::iota(l.begin(), l.end(), 0);
  {
    Queue<int> q(l.begin(), l.end());
    EXPECT_EQ(q.size(), n);
    EXPECT_EQ(q.empty(), false);
    for (int i = 0; i < n; ++i) {
      EXPECT_EQ(q.front(), i);
      q.pop();
    }
    EXPECT_EQ(q.size(), 0);
    EXPECT_EQ(q.empty(), true);
  }
  {
    Queue<int> q;
    for (int i = 0; i < n; ++i) {
      q.push(i);
      EXPECT_EQ(q.back(), i);
    }
    EXPECT_EQ(q.size(), n);
    EXPECT_EQ(q.empty(), false);
    for (int i = 0; i < n; ++i) {
      EXPECT_EQ(q.front(), i);
      q.pop();
    }
    EXPECT_EQ(q.size(), 0);
    EXPECT_EQ(q.empty(), true);
  }
  {
    Queue<int> q1(l.begin(), l.end());
    EXPECT_EQ(q1.size(), n);
    EXPECT_EQ(q1.empty(), false);
    Queue<int> q(std::move(q1));
    // EXPECT_EQ(q1.size(), 0);
    // EXPECT_EQ(q1.empty(), true);
    EXPECT_EQ(q.size(), n);
    EXPECT_EQ(q.empty(), false);
    for (int i = 0; i < n; ++i) {
      EXPECT_EQ(q.front(), i);
      q.pop();
    }
    EXPECT_EQ(q.size(), 0);
    EXPECT_EQ(q.empty(), true);
  }
}

TEST(priority_queue_test, priority_queue_test) {
  int n = 0;
  {
    PriorityQueue<int> pq;
    PriorityQueue<int> pq_copy(pq);
    EXPECT_EQ(pq.size(), n);
    EXPECT_EQ(pq.empty(), true);
    EXPECT_EQ(pq, pq_copy);
    EXPECT_EQ(pq < pq_copy, false);
    EXPECT_EQ(pq > pq_copy, false);
    EXPECT_EQ(pq <= pq_copy, true);
    EXPECT_EQ(pq >= pq_copy, true);
  }

  n = 102400;
  Vector<int> vec(n);
  std::iota(vec.begin(), vec.end(), 0);
  {
    PriorityQueue<int> pq(vec.begin(), vec.end());
    EXPECT_EQ(pq.size(), n);
    EXPECT_EQ(pq.empty(), false);
    for (int i = n - 1; i >= 0; --i) {
      EXPECT_EQ(pq.top(), i);
      pq.pop();
    }
    EXPECT_EQ(pq.size(), 0);
    EXPECT_EQ(pq.empty(), true);
  }
  {
    PriorityQueue<int> pq(vec);
    EXPECT_EQ(pq.size(), n);
    EXPECT_EQ(pq.empty(), false);
    for (int i = n - 1; i >= 0; --i) {
      EXPECT_EQ(pq.top(), i);
      pq.pop();
    }
    EXPECT_EQ(pq.size(), 0);
    EXPECT_EQ(pq.empty(), true);
  }
  {
    PriorityQueue<int> pq;
    for (int i = 0; i < n; ++i) {
      pq.push(i);
      EXPECT_EQ(pq.top(), i);
    }
    EXPECT_EQ(pq.size(), n);
    EXPECT_EQ(pq.empty(), false);
    for (int i = n - 1; i >= 0; --i) {
      EXPECT_EQ(pq.top(), i);
      pq.pop();
    }
    EXPECT_EQ(pq.size(), 0);
    EXPECT_EQ(pq.empty(), true);
  }
  {
    PriorityQueue<int> pq(std::move(vec));
    EXPECT_EQ(vec.size(), 0);
    EXPECT_EQ(vec.empty(), true);
    EXPECT_EQ(pq.size(), n);
    EXPECT_EQ(pq.empty(), false);
    for (int i = n - 1; i >= 0; --i) {
      EXPECT_EQ(pq.top(), i);
      pq.pop();
    }
    EXPECT_EQ(pq.size(), 0);
    EXPECT_EQ(pq.empty(), true);
  }
}