/**
 * @file list.h
 * @author gwq5210 (gwq5210@qq.com)
 * @brief 双向链表的实现
 * @date 2021-05-15
 *
 * @copyright Copyright (c) 2021. All rights reserved.
 */

#pragma once

#include <algorithm>
#include <iterator>
#include <type_traits>

#include "gtl/algorithm/algorithm.h"
#include "gtl/algorithm/memory_op.h"
#include "gtl/container/list_base.h"
#include "gtl/iterator/iterator.h"
#include "gtl/util/common.h"

namespace gtl {

using doubly_list::ListHead;
using doubly_list::ListNode;

struct ListIteratorBase {
  using iterator_category = std::bidirectional_iterator_tag;
  using Self = ListIteratorBase;
  ListNode* node;
  ListIteratorBase() : node(nullptr) {}
  ListIteratorBase(ListNode* n) : node(n) {}

  Self& operator++() {
    node = node->next;
    return *this;
  }
  Self operator++(int) {
    ListNode* ret = node;
    node = node->next;
    return Self(ret);
  }
  Self& operator--() {
    node = node->prev;
    return *this;
  }
  Self operator--(int) {
    ListNode* ret = node;
    node = node->prev;
    return Self(ret);
  }
  bool operator==(const Self& other) const { return node == other.node; }
  bool operator!=(const Self& other) const { return node != other.node; }
};

template <typename T, typename Difference = std::ptrdiff_t>
struct ListConstIterator : public ListIteratorBase {
  using reference = const T&;
  using pointer = const T*;
  using difference_type = Difference;
  using value_type = T;
  using Node = doubly_list::ListNodeT<T>;
  using Base = ListIteratorBase;
  using Self = ListConstIterator;
  ListConstIterator() : Base() {}
  explicit ListConstIterator(const ListNode* node) : Base(const_cast<ListNode*>(node)) {}
  reference operator*() const { return Node::Value(node); }
  pointer operator->() const { return std::pointer_traits<pointer>::pointer_to(**this); }
  Self& operator++() {
    Base::operator++();
    return *this;
  }
  Self operator++(int) {
    auto ret = *this;
    Base::operator++();
    return ret;
  }
  Self& operator--() {
    Base::operator--();
    return *this;
  }
  Self operator--(int) {
    auto ret = *this;
    Base::operator--();
    return ret;
  }
};

template <typename T, typename Difference = std::ptrdiff_t>
struct ListIterator : public ListConstIterator<T, Difference> {
  using reference = T&;
  using pointer = const T*;
  using Base = ListConstIterator<T, Difference>;
  using Self = ListIterator;
  ListIterator() : Base() {}
  explicit ListIterator(ListNode* node) : Base(node) {}
  reference operator*() const { return const_cast<reference>(Base::operator*()); }
  pointer operator->() const { return const_cast<pointer>(Base::operator->()); }
  Self& operator++() {
    Base::operator++();
    return *this;
  }
  Self operator++(int) {
    auto ret = *this;
    Base::operator++();
    return ret;
  }
  Self& operator--() {
    Base::operator--();
    return *this;
  }
  Self operator--(int) {
    auto ret = *this;
    Base::operator--();
    return ret;
  }
};

template <typename T>
class List {
 public:
  using Node = doubly_list::ListNodeT<T>;
  using node_type = Node;
  using value_type = T;
  using reference = T&;
  using const_reference = const T&;
  using pointer = T*;
  using const_pointer = const T*;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;

  using iterator = ListIterator<T, difference_type>;
  using const_iterator = ListConstIterator<T, difference_type>;
  using reverse_iterator = gtl::reverse_iterator<iterator>;
  using const_reverse_iterator = gtl::reverse_iterator<const_iterator>;

  List() : dummy_head_(nullptr), size_(0) { init(); }
  explicit List(size_type count) {
    init();
    insert(begin(), count, T());
  }
  List(size_type count, const T& v) {
    init();
    insert(begin(), count, v);
  }
  template <typename InputIt>
  List(InputIt first, InputIt last) {
    init();
    insert(begin(), first, last);
  }
  List(const List& other) {
    init();
    insert(begin(), other.begin(), other.end());
  }
  List(List&& other) {
    init();
    gtl::swap(dummy_head_, other.dummy_head_);
    gtl::swap(size_, other.size_);
  }
  List(std::initializer_list<T> il) {
    init();
    insert(begin(), il);
  }
  ~List() { destroy_list(); }

  List& operator=(const List& other) {
    assign(other);
    return *this;
  }
  List& operator=(List&& other) {
    assign(std::move(other));
    return *this;
  }
  List& operator=(std::initializer_list<T> il) {
    assign(il);
    return *this;
  }

  void assign(size_type count, const T& v) { insert(begin(), count, v); }
  template <typename InputIt>
  void assign(InputIt first, InputIt last) {
    assign_range(gtl::distance(first, last), first, last);
  }
  void assign(std::initializer_list<T> il) { assign_range(il.size(), il.begin(), il.end()); }
  void assign(const List& other) {
    if (this != &other) {
      assign_range(other.size(), other.begin(), other.end());
    }
  }
  void assign(List&& other) {
    clear();
    gtl::swap(dummy_head_, other.dummy_head_);
    gtl::swap(size_, other.size_);
  }

  // Element access
  T& front() { return *begin(); }
  const T& front() const { return *begin(); }
  T& back() { return Node::Value(dummy_head_->prev); }
  const T& back() const { return Node::Value(dummy_head_->prev); }

  // Iterators
  iterator begin() { return iterator(dummy_head_->next); };
  const_iterator begin() const { return const_iterator(dummy_head_->next); };
  const_iterator cbegin() const { return begin(); };
  iterator end() { return iterator(dummy_head_); };
  const_iterator end() const { return const_iterator(dummy_head_); };
  const_iterator cend() const { return end(); };
  reverse_iterator rbegin() { return reverse_iterator(end()); }
  const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
  const_reverse_iterator crbegin() const { return rbegin(); }
  reverse_iterator rend() { return reverse_iterator(begin()); }
  const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }
  const_reverse_iterator crend() const { return rend(); }

  // Capacity
  bool empty() const { return size() == 0; }
  size_type size() const { return size_; }

  // Modifiers
  iterator insert(const_iterator before, const T& v) { return emplace(before, v); }
  iterator insert(const_iterator before, size_type count, const T& v) {
    while (count--) {
      before = emplace(before, v);
    }
    return iterator(before.node);
  }
  template <typename InputIt, typename Category = typename std::iterator_traits<InputIt>::iterator_category>
  iterator insert(const_iterator before, InputIt first, InputIt last) {
    while (first != last) {
      emplace(before, *first++);
    }
    return iterator(before.node);
  }
  iterator insert(const_iterator before, std::initializer_list<T> il) { return insert(before, il.begin(), il.end()); }
  iterator insert(const_iterator before, T&& v) { return emplace(before, std::move(v)); }
  template <typename... Args>
  iterator emplace(const_iterator before, Args&&... args) {
    ++size_;
    return iterator(doubly_list::InsertBefore(before.node, new_node(std::forward<Args>(args)...)));
  }
  template <typename... Args>
  iterator emplace_back(Args&&... args) {
    return emplace(end(), std::forward<Args>(args)...);
  }
  template <typename... Args>
  iterator emplace_front(Args&&... args) {
    return emplace(begin(), std::forward<Args>(args)...);
  }
  iterator push_back(const T& v) { return emplace_back(v); }
  iterator push_back(T&& v) { return emplace_back(std::move(v)); }
  iterator push_front(const T& v) { return emplace_front(v); }
  iterator push_front(T&& v) { return emplace_front(std::move(v)); }
  iterator erase(const_iterator pos) {
    auto it = pos;
    return erase(pos, ++it);
  }
  iterator erase(const_iterator first, const_iterator last) {
    if (first != last) {
      ListNode* node = first.node;
      doubly_list::Remove(first.node, last.node->prev);
      while (node) {
        ListNode* next = node->next;
        delete_node(node);
        node = next;
        --size_;
      }
    }
    return iterator(last.node);
  }
  void pop_back() { erase(const_iterator(dummy_head_->prev)); }
  void pop_front() { erase(begin()); }
  void resize(size_type count) { resize(count, T()); }
  void resize(size_type count, const T& v) {
    if (count > size()) {
      insert(end(), count - size(), v);
    } else if (count < size()) {
      auto it = end();
      for (size_type i = size() - count; i > 0; --i) {
        it = erase(--it);
      }
    }
  }
  void clear() { erase(begin(), end()); }

  // Operations
  void merge(List& other) { merge(other, std::less<T>()); }
  void merge(List&& other) { merge(other, std::less<T>()); }
  template <typename Compare>
  void merge(List&& other, Compare comp) {
    merge(other, comp);
  }
  template <typename Compare>
  void merge(List& other, Compare comp) {
    if (this != &other) {
      auto left = begin();
      auto right = other.begin();
      while (left != end() && right != other.end()) {
        if (comp(*right, *left)) {
          splice(left, other, right);
          right = other.begin();
        } else {
          ++left;
        }
      }
      if (right != other.end()) {
        splice(left, other);
      }
    }
  }
  void splice(const_iterator before, List& other) { transfer(before, other, other.begin(), other.end()); }
  void splice(const_iterator before, List& other, const_iterator it) {
    if (before != it) {
      auto next = it;
      transfer(before, other, it, ++next);
    }
  }
  void splice(const_iterator before, List& other, const_iterator first, const_iterator last) {
    transfer(before, other, first, last);
  }
  void splice(const_iterator before, List&& other) { splice(before, other); }
  void splice(const_iterator before, List&& other, const_iterator it) { splice(before, other, it); }
  void splice(const_iterator before, List&& other, const_iterator first, const_iterator last) {
    splice(before, other, first, last);
  }
  void reverse() {
    if (size() <= 1) {
      return;
    }
    auto it = begin();
    ++it;
    while (it != end()) {
      auto next = it;
      transfer(begin(), *this, it, ++next);
      it = next;
    }
  }
  void merge_sort() { merge_sort(std::less<T>()); }
  template <typename Compare>
  void merge_sort(Compare comp) {
    if (size() <= 1) {
      return;
    }
    List right;
    right.splice(right.begin(), *this, gtl::next(begin(), size() / 2), end());
    merge_sort(comp);
    right.merge_sort(comp);
    merge(right);
  }
  void sort() { sort(std::less<T>()); }
  template <typename Compare>
  void sort(Compare comp) {
    if (size() <= 1) {
      return;
    }
    List left;
    List right;
    for (size_type i = 1; i < size(); i *= 2) {
      size_type c = 2 * i;
      auto it = begin();
      auto last = end();
      size_type n = size() - size() % c;
      for (size_type j = 0; j < n; j += c) {
        last = gtl::next(it, i);
        left.splice(left.begin(), *this, it, last);
        it = last;
        last = gtl::next(last, i);
        right.splice(right.begin(), *this, it, last);
        left.merge(right);
        splice(last, left);
        it = last;
      }
      n = size() % c;
      if (n > i) {
        last = gtl::next(it, i);
        left.splice(left.begin(), *this, it, last);
        right.splice(right.begin(), *this, last, end());
        left.merge(right);
        splice(end(), left);
      }
    }
  }
  void qsort() { qsort(std::less<T>()); }
  template <typename Compare>
  void qsort(Compare comp) {
    if (size() <= 1) {
      return;
    }
    List left;
    List right;
    auto it = begin();
    ++it;
    while (it != end()) {
      if (comp(*it, front())) {
        auto next = it;
        left.transfer(left.end(), *this, it, ++next);
        it = next;
      } else {
        ++it;
      }
    }
    it = begin();
    right.transfer(right.begin(), *this, ++it, end());
    left.qsort(comp);
    right.qsort(comp);
    left.splice(left.end(), *this);
    left.splice(left.end(), right);
    *this = std::move(left);
  }

 private:
  template <typename InputIt>
  void assign_range(size_type count, InputIt first, InputIt last) {
    if (count == 0) {
      return;
    }
    if (count <= size()) {
      auto it = gtl::copy(first, last, begin());
      erase(it, end());
    } else {
      gtl::copy_n(first, size(), begin());
      insert(end(), gtl::next(first, size()), last);
    }
  }
  void assign_n(size_type count, const T& v) {
    if (count == 0) {
      return;
    }
    if (count <= size()) {
      auto it = gtl::fill_n(begin(), count, v);
      erase(it, end());
    } else {
      auto it = gtl::fill_n(begin(), size(), v);
      insert(it, count, v);
    }
  }
  void transfer(const_iterator before, List&& other, const_iterator first, const_iterator last) {
    transfer(before, other, first, last);
  }
  void transfer(const_iterator before, List& other, const_iterator first, const_iterator last) {
    if (first != last && before != last) {
      ListNode* node = first.node;
      while (node != last.node) {
        ListNode* next = doubly_list::Remove(node);
        doubly_list::InsertBefore(before.node, node);
        if (this != &other) {
          --other.size_;
          ++size_;
        }
        node = next;
      }
    }
  }
  template <typename... Args>
  Node* new_node(Args&&... args) {
    return Node::New(std::forward<Args>(args)...);
  }
  void delete_node(ListNode* node) { return Node::Delete(node); }

  void init() {
    dummy_head_ = new ListNode();
    size_ = 0;
  }
  void destroy_list() {
    clear();
    delete dummy_head_;
    dummy_head_ = nullptr;
  }

  ListHead* dummy_head_;
  size_type size_;
};  // class List

template <typename T>
using list = List<T>;

template <typename T>
bool operator==(const List<T>& lhs, const List<T>& rhs) {
  return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename T>
bool operator!=(const List<T>& lhs, const List<T>& rhs) {
  return !(lhs != rhs);
}

template <typename T>
bool operator<(const List<T>& lhs, const List<T>& rhs) {
  return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename T>
bool operator>(const List<T>& lhs, const List<T>& rhs) {
  return rhs < lhs;
}

template <typename T>
bool operator<=(const List<T>& lhs, const List<T>& rhs) {
  return !(lhs > rhs);
}

template <typename T>
bool operator>=(const List<T>& lhs, const List<T>& rhs) {
  return !(lhs < rhs);
}

}  // namespace gtl