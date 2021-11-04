/**
 * @file slist_base.h
 * @author gwq5210 (gwq5210@qq.com)
 * @brief 侵入式单链表实现
 * @date 2021-05-28
 *
 * @copyright Copyright (c) 2021. All rights reserved.
 */

#pragma once

#include <utility>

#include "memory_op.h"

namespace gtl {

namespace singly_list {

struct SListNode {
  SListNode* next;
  SListNode() : next(nullptr) {}
  void init() { next = nullptr; }
};

struct SListHead {
  SListNode head;
  SListNode* tail;
  SListHead() : tail(&head) {}
  void init() { head.init(), tail = &head; }
};

template <typename T>
struct SListNodeT : public SListNode {
  T val;

  SListNodeT() = default;
  SListNodeT(const T& v) : val(v) {}
  SListNodeT(T&& v) : val(std::move(v)) {}

  template <typename... Args>
  void ConstructVal(Args&&... args) {
    gtl::construct_at(&val, std::forward<Args>(args)...);
  }
  void DestroyVal() { gtl::destroy_at(&val); }

  static SListNodeT* From(SListNode* slist_node) { return static_cast<SListNodeT*>(slist_node); }
  static T& Value(SListNode* slist_node) { return From(slist_node)->val; }
  static const SListNodeT* From(const SListNode* slist_node) { return static_cast<const SListNodeT*>(slist_node); }
  static const T& Value(const SListNode* slist_node) { return From(slist_node)->val; }
  static const T& CValue(const SListNode* slist_node) { return From(slist_node)->val; }
  template <typename Allocator, typename... Args>
  static SListNodeT* New(Allocator& alloc, Args&&... args) {
    SListNodeT* node = alloc.allocate(1);
    node->ConstructVal(std::forward<Args>(args)...);
    node->next = nullptr;
    return node;
  }
  template <typename Allocator>
  static void Delete(Allocator& alloc, SListNode* node) {
    auto* t = From(node);
    t->DestroyVal();
    alloc.deallocate(t, 1);
  }
};

inline SListNode* RemoveAfter(SListNode* prev) {
  SListNode* curr = prev->next;
  prev->next = curr->next;
  curr->next = nullptr;
  return prev->next;
}

inline SListNode* InsertAfter(SListNode* after, SListNode* node) {
  node->next = after->next;
  after->next = node;
  return node;
}

/**
 * @brief 移除(prev, last]之间的节点
 *
 * @param prev 移除节点的前驱节点
 * @param last 移除的结束节点，包含该节点
 * @return SListNode* 返回移除区间的后一个节点，即last->next节点
 */
inline SListNode* RemoveAfter(SListNode* prev, SListNode* last) {
  prev->next = last->next;
  last->next = nullptr;
  return prev->next;
}

/**
 * @brief 将[first, last]之间的节点插入到after节点后
 *
 * @param after 需要插入的位置，在该节点之后插入
 * @param first 插入区间的起始节点，包含该节点
 * @param last 插入区间的结束节点，包含该节点
 * @return SListNode* 返回插入的第一个节点，即first节点
 */
inline SListNode* InsertAfter(SListNode* after, SListNode* first, SListNode* last) {
  last->next = after->next;
  after->next = first;
  return first;
}

inline SListNode* PrevNode(SListNode* head, SListNode* node) {
  SListNode* prev = head;
  while (prev->next != node) {
    prev = prev->next;
  }
  return prev;
}

inline std::size_t Size(SListHead* slist_head) {
  std::size_t size = 0;
  SListNode* node = slist_head->head.next;
  while (node) {
    ++size;
    node = node->next;
  }
  return size;
}

inline bool Empty(SListHead* slist_head) { return slist_head->head.next == nullptr; }

inline SListNode* AddToTail(SListHead* slist_head, SListNode* node) {
  InsertAfter(slist_head->tail, node);
  slist_head->tail = node;
  return node;
}

inline SListNode* AddToTail(SListHead* slist_head, SListNode* first, SListNode* last) {
  InsertAfter(slist_head->tail, first, last);
  slist_head->tail = last;
  return first;
}

inline SListNode* AddToHead(SListHead* slist_head, SListNode* node) {
  if (Empty(slist_head)) {
    slist_head->tail = node;
  }
  return InsertAfter(&slist_head->head, node);
}

inline SListNode* AddToHead(SListHead* slist_head, SListNode* first, SListNode* last) {
  if (Empty(slist_head)) {
    slist_head->tail = last;
  }
  return InsertAfter(&slist_head->head, first, last);
}

inline SListNode* InsertAfter(SListHead* slist_head, SListNode* after, SListNode* node) {
  if (after == &slist_head->head) {
    return AddToHead(slist_head, node);
  } else if (after == slist_head->tail) {
    return AddToTail(slist_head, node);
  }
  return InsertAfter(after, node);
}

inline SListNode* InsertAfter(SListHead* slist_head, SListNode* after, SListNode* first, SListNode* last) {
  if (after == &slist_head->head) {
    return AddToHead(slist_head, first, last);
  } else if (after == slist_head->tail) {
    return AddToTail(slist_head, first, last);
  }
  return InsertAfter(after, first, last);
}

inline SListNode* RemoveHead(SListHead* slist_head) {
  if (slist_head->head.next->next == nullptr) {
    slist_head->tail = &slist_head->head;
  }
  return RemoveAfter(&slist_head->head);
}

inline SListNode* RemoveAfter(SListHead* slist_head, SListNode* after) {
  if (after->next == slist_head->tail) {
    slist_head->tail = after;
  }
  return RemoveAfter(after);
}

inline void SpliceAfter(SListNode* slist_head, SListNode* after, SListHead* other_head, SListNode* prev) {
  SListNode* node = prev->next;
  RemoveAfter(other_head, prev);
  InsertAfter(slist_head, after, node);
}

}  // namespace slist

}  // namespace gtl