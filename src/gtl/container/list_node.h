/**
 * @file list_base.h
 * @author gwq5210 (gwq5210@qq.com)
 * @brief 侵入式双向链表实现
 * @date 2021-05-28
 *
 * @copyright Copyright (c) 2021. All rights reserved.
 */

#pragma once

#include <utility>

#include "gtl/algorithm/memory_op.h"

namespace gtl {

namespace doubly_list {

struct ListNode {
  ListNode* next;
  ListNode* prev;
  ListNode() : next(this), prev(this) {}
  ListNode(ListNode* n, ListNode* p) : next(n), prev(p) {}
};

using ListHead = ListNode;

template <typename T>
struct ListNodeT : public ListNode {
  T val;

  ListNodeT() = default;
  ListNodeT(const T& v) : val(v) {}
  ListNodeT(T&& v) : val(std::move(v)) {}

  template <typename... Args>
  void ConstructVal(Args&&... args) {
    gtl::construct_at(&val, std::forward<Args>(args)...);
  }
  void DestroyVal() { gtl::destroy_at(&val); }

  static ListNodeT* From(ListNode* list_node) { return static_cast<ListNodeT*>(list_node); }
  static T& Value(ListNode* list_node) { return From(list_node)->val; }
  static const ListNodeT* From(const ListNode* list_node) { return static_cast<const ListNodeT*>(list_node); }
  static const T& Value(const ListNode* list_node) { return From(list_node)->val; }
  static const T& CValue(const ListNode* list_node) { return From(list_node)->val; }
  template <typename... Args>
  static ListNodeT* New(Args&&... args);
  static void Delete(ListNode* node);
};

}  // namespace doubly_list

}  // namespace gtl