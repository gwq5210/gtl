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
#include "gtl/container/list_node.h"
#include "gtl/memory/default_allocator.h"
#include "gtl/port.h"

#define ListEntry(PTR, TYPE, MEMBER) ((TYPE*)((char*)PTR - GTL_OFFSETOF(TYPE, MEMBER)))
#define ListForEach(NODE, HEAD) \
  for (::gtl::doubly_list::ListNode* NODE = (HEAD).next; NODE != &(HEAD); NODE = NODE->next)
#define ListForEachSafe(NODE, HEAD)                                                                  \
  for (::gtl::doubly_list::ListNode* NODE = (HEAD).next, *NODE##_next = NODE->next; NODE != &(HEAD); \
       NODE = NODE##_next, NODE##_next = NODE->next)
#define ListForEachPrev(NODE, HEAD) \
  for (::gtl::doubly_list::ListNode* NODE = (HEAD).prev; NODE != &(HEAD); NODE = NODE->prev)
#define ListForEachPrevSafe(NODE, HEAD)                                                              \
  for (::gtl::doubly_list::ListNode* NODE = (HEAD).prev, *NODE##_prev = NODE->prev; NODE != &(HEAD); \
       NODE = NODE##_prev, NODE##_prev = NODE->prev)

namespace gtl {

namespace doubly_list {

template <typename T>
template <typename... Args>
ListNodeT<T>* ListNodeT<T>::New(Args&&... args) {
  ListNodeT* node = static_cast<ListNodeT*>(gtl::DefaultAllocator().Malloc(sizeof(ListNodeT)));
  node->ConstructVal(std::forward<Args>(args)...);
  node->next = nullptr;
  node->prev = nullptr;
  return node;
}

template <typename T>
void ListNodeT<T>::Delete(ListNode* node) {
  auto* t = From(node);
  t->DestroyVal();
  gtl::DefaultAllocator().Free(node);
}

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

inline ListNode* AddToTail(ListHead* list_head, ListNode* node) { return InsertBefore(list_head, node); }

inline ListNode* AddToTail(ListHead* list_head, ListNode* first, ListNode* last) {
  return InsertBefore(list_head, first, last);
}

inline ListNode* AddToHead(ListHead* list_head, ListNode* node) { return InsertBefore(list_head->next, node); }

inline ListNode* AddToHead(ListHead* list_head, ListNode* first, ListNode* last) {
  return InsertBefore(list_head->next, first, last);
}

inline ListNode* RemoveHead(ListHead* list_head) { return Remove(list_head->next); }

inline ListNode* RemoveTail(ListHead* list_head) { return Remove(list_head->prev); }

inline void SpliceBefore(ListNode* before, ListNode* curr) {
  Remove(curr);
  InsertBefore(before, curr);
}

}  // namespace doubly_list

}  // namespace gtl