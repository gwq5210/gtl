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

#include "gtl/memory_op.h"

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
  template <typename Allocator, typename... Args>
  static ListNodeT* New(Allocator& alloc, Args&&... args) {
    ListNodeT* node = alloc.allocate(1);
    node->ConstructVal(std::forward<Args>(args)...);
    node->next = nullptr;
    node->prev = nullptr;
    return node;
  }
  template <typename Allocator>
  static void Delete(Allocator& alloc, ListNode* node) {
    auto* t = From(node);
    t->DestroyVal();
    alloc.deallocate(t, 1);
  }
};

inline ListNode* Remove(ListNode* curr) {
  ListNode* next = curr->next;
  curr->prev->next = curr->next;
  curr->next->prev = curr->prev;
  curr->prev = nullptr;
  curr->next = nullptr;
  return next;
}

inline ListNode* InsertBefore(ListNode* before, ListNode* node) {
  node->next = before;
  node->prev = before->prev;
  before->prev->next = node;
  before->prev = node;
  return node;
}

/**
 * @brief 移除[first, last]之间的节点
 *
 * @param first 移除的起始节点，包含该节点
 * @param last 移除的结束节点，包含该节点
 * @return ListNode* 返回移除区间的后一个节点，即last->next节点
 */
inline ListNode* Remove(ListNode* first, ListNode* last) {
  ListNode* next = last->next;
  first->prev->next = last->next;
  last->next->prev = first->prev;
  first->prev = nullptr;
  last->next = nullptr;
  return next;
}

/**
 * @brief 将[first, last]之间的节点插入到before节点前
 *
 * @param before 需要插入的位置，在该节点之前插入
 * @param first 插入区间的起始节点，包含该节点
 * @param last 插入区间的结束节点，包含该节点
 * @return ListNode* 返回插入的第一个节点，即first节点
 */
inline ListNode* InsertBefore(ListNode* before, ListNode* first, ListNode* last) {
  last->next = before;
  first->prev = before->prev;
  before->prev->next = first;
  before->prev = last;
  return first;
}

inline std::size_t Size(ListHead* list_head) {
  std::size_t size = 0;
  ListNode* node = list_head->next;
  while (node != list_head) {
    ++size;
    node = node->next;
  }
  return size;
}

inline bool Empty(ListHead* list_head) { return list_head->next == list_head; }

inline ListNode* AddToTail(ListHead* list_head, ListNode* node) {
  return InsertBefore(list_head->prev, node);
}

inline ListNode* AddToTail(ListHead* list_head, ListNode* first, ListNode* last) {
  return InsertBefore(list_head->prev, first, last);
}

inline ListNode* AddToHead(ListHead* list_head, ListNode* node) {
  return InsertBefore(list_head->next, node);
}

inline ListNode* AddToHead(ListHead* list_head, ListNode* first, ListNode* last) {
  return InsertBefore(list_head->next, first, last);
}

inline ListNode* RemoveHead(ListHead* list_head) {
  return Remove(list_head->next);
}

inline ListNode* RemoveTail(ListHead* list_head) {
  return Remove(list_head->prev);
}

inline void SpliceBefore(ListNode* before, ListNode* curr) {
  Remove(curr);
  InsertBefore(before, curr);
}

}  // namespace doubly_list

}  // namespace gtl