/**
 * @file gtl_slist.h
 * @author gwq5210 (gwq5210@qq.com)
 * @brief 单链表的实现
 * @date 2021-05-15
 *
 * @copyright Copyright (c) 2021. All rights reserved.
 */

#pragma once

#include <algorithm>
#include <iterator>
#include <type_traits>

#include "gtl_algorithm.h"
#include "gtl_common.h"
#include "gtl_compressed_pair.h"
#include "gtl_memory.h"

namespace gtl {

struct SListNode {
  SListNode* next;
  SListNode() : next(nullptr) {}
};

inline SListNode* remove_node(SListNode* prev, SListNode* curr) {
  prev->next = curr->next;
  curr->next = nullptr;
  return prev->next;
}

inline SListNode* insert_node_after(SListNode* after, SListNode* node) {
  node->next = after->next;
  after->next = node;
  return node;
}

/**
 * @brief 移除[first, last]之间的节点
 *
 * @param prev first节点的前驱节点
 * @param first 移除的起始节点，包含该节点
 * @param last 移除的结束节点，包含该节点
 * @return SListNode* 返回移除区间的后一个节点，即last->next节点
 */
inline SListNode* remove_range(SListNode* prev, SListNode* first, SListNode* last) {
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
inline SListNode* insert_range_after(SListNode* after, SListNode* first, SListNode* last) {
  last->next = after->next;
  after->next = first;
  return first;
}

struct SListIteratorBase {
  using iterator_category = std::forward_iterator_tag;
  using Self = SListIteratorBase;
  SListNode* node;
  SListIteratorBase() : node(nullptr) {}
  SListIteratorBase(SListNode* n) : node(n) {}

  Self& operator++() {
    node = node->next;
    return *this;
  }
  Self operator++(int) {
    SListNode* ret = node;
    node = node->next;
    return Self(ret);
  }
  bool operator==(const Self& other) const { return node == other.node; }
  bool operator!=(const Self& other) const { return node != other.node; }
};

template <typename SListType>
struct ConstSListIterator : public SListIteratorBase {
  using reference = typename SListType::const_reference;
  using pointer = typename SListType::const_pointer;
  using difference_type = typename SListType::difference_type;
  using value_type = typename SListType::value_type;
  using Base = SListIteratorBase;
  using Self = ConstSListIterator;
  explicit ConstSListIterator(const SListNode* node) : Base(const_cast<SListNode*>(node)) {}
  reference operator*() const { return SListType::node_value(node); }
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
};

template <typename SListType>
struct SListIterator : public ConstSListIterator<SListType> {
  using reference = typename SListType::reference;
  using pointer = typename SListType::pointer;
  using difference_type = typename SListType::difference_type;
  using value_type = typename SListType::value_type;
  using Base = ConstSListIterator<SListType>;
  using Self = SListIterator;
  explicit SListIterator(SListNode* node) : Base(node) {}
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
};

template <typename T>
class SList {
 public:
  struct Node : public SListNode {
    T val;
    template <typename... Args>
    Node(Args&&... args) {
      gtl::construct_at(&val, std::forward<Args>(args)...);
    }
  };

  using value_type = T;
  using reference = T&;
  using const_reference = const T&;
  using pointer = T*;
  using const_pointer = const T*;
  using size_type = size_t;
  using difference_type = ptrdiff_t;
  using allocator_type = std::allocator<Node>;
  using iterator = SListIterator<SList>;
  using const_iterator = ConstSListIterator<SList>;

  static Node* to_node(SListNode* list_node) { return static_cast<Node*>(list_node); }
  static T& node_value(SListNode* list_node) { return to_node(list_node)->val; }

  SList() : dummy_head_(nullptr), size_alloc_(0) { init(); }
  explicit SList(size_type count) {
    init();
    insert_after(before_begin(), count, T());
  }
  SList(size_type count, const T& v) {
    init();
    insert_after(before_begin(), count, v);
  }
  template <typename InputIt>
  SList(InputIt first, InputIt last) {
    init();
    insert_after(before_begin(), first, last);
  }
  SList(const SList& other) {
    init();
    insert_after(before_begin(), other.begin(), other.end());
  }
  SList(SList&& other) {
    init();
    gtl::swap(dummy_head_, other.dummy_head_);
    gtl::swap(get_size(), other.get_size());
  }
  SList(std::initializer_list<T> il) {
    init();
    insert_after(before_begin(), il);
  }
  ~SList() { destroy_list(); }

  SList& operator=(const SList& other) {
    assign(other);
    return *this;
  }
  SList& operator=(SList&& other) {
    assign(std::move(other));
    return *this;
  }
  SList& operator=(std::initializer_list<T> il) {
    assign(il);
    return *this;
  }

  void assign(size_type count, const T& v) { insert_after(before_begin(), count, v); }
  template <typename InputIt>
  void assign(InputIt first, InputIt last) {
    assign_range(gtl::distance(first, last), first, last);
  }
  void assign(std::initializer_list<T> il) { assign_range(il.size(), il.begin(), il.end()); }
  void assign(const SList& other) {
    if (this != &other) {
      assign_range(other.size(), other.begin(), other.end());
    }
  }
  void assign(SList&& other) {
    clear();
    gtl::swap(dummy_head_, other.dummy_head_);
    gtl::swap(get_size(), other.get_size());
  }

  allocator_type get_allocator() const { return allocator(); }

  // Element access
  T& front() { return *begin(); }
  const T& front() const { return *begin(); }
  T& back() { return *before_end(); }
  const T& back() const { return *before_end(); }

  // Iterators
  iterator before_begin() { return iterator(dummy_head_); }
  const_iterator before_begin() const { return iterator(dummy_head_); }
  const_iterator cbefore_begin() const { return before_begin(); };
  iterator before_end() { return gtl::next(before_begin(), size()); }
  const_iterator before_end() const { return gtl::next(before_begin(), size()); }
  const_iterator cbefore_end() const { return before_end(); };
  iterator begin() { return iterator(dummy_head_->next); };
  const_iterator begin() const { return const_iterator(dummy_head_->next); };
  const_iterator cbegin() const { return begin(); };
  iterator end() { return iterator(nullptr); };
  const_iterator end() const { return const_iterator(nullptr); };
  const_iterator cend() const { return end(); };

  // Capacity
  bool empty() const { return size() == 0; }
  size_type size() const { return get_size(); }

  // Modifiers
  iterator insert_after(const_iterator after, const T& v) { return emplace_after(after, v); }
  iterator insert_after(const_iterator after, size_type count, const T& v) {
    while (count--) {
      after = emplace_after(after, v);
    }
    return iterator(after.node);
  }
  template <typename InputIt, typename Category = typename std::iterator_traits<InputIt>::iterator_category>
  iterator insert_after(const_iterator after, InputIt first, InputIt last) {
    while (first != last) {
      after = emplace_after(after, *first++);
    }
    return iterator(after.node);
  }
  iterator insert_after(const_iterator after, std::initializer_list<T> il) {
    return insert_after(after, il.begin(), il.end());
  }
  iterator insert_after(const_iterator after, T&& v) { return emplace_after(after, std::move(v)); }
  template <typename... Args>
  iterator emplace_after(const_iterator after, Args&&... args) {
    ++get_size();
    return iterator(insert_node_after(after.node, construct_node(std::forward<Args>(args)...)));
  }
  template <typename... Args>
  iterator emplace_front(Args&&... args) {
    return emplace_after(before_begin(), std::forward<Args>(args)...);
  }
  iterator push_front(const T& v) { return emplace_front(v); }
  iterator push_front(T&& v) { return emplace_front(std::move(v)); }
  iterator erase_after(const_iterator pos) {
    SListNode* curr = pos.node->next;
    SListNode* next = remove_node(pos.node, curr);
    destroy_node(curr);
    --get_size();
    return iterator(next);
  }
  iterator erase_after(const_iterator first, const_iterator last) {
    SListNode* node = first.node->next;
    while (node != last.node) {
      SListNode* next = remove_node(first.node, node);
      destroy_node(node);
      node = next;
      --get_size();
    }
    return iterator(last.node);
  }
  void pop_front() { erase_after(before_begin()); }
  void resize(size_type count) { resize(count, T()); }
  void resize(size_type count, const T& v) {
    if (count > size()) {
      insert_after(before_end(), count - size(), v);
    } else if (count < size()) {
      erase_after(gtl::next(before_begin(), count), end());
    }
  }
  void clear() { erase_after(before_begin(), end()); }

  // Operations
  void merge(SList& other) { merge(other, std::less<T>()); }
  void merge(SList&& other) { merge(other, std::less<T>()); }
  template <typename Compare>
  void merge(SList&& other, Compare comp) {
    merge(other, comp);
  }
  template <typename Compare>
  void merge(SList& other, Compare comp) {
    if (this != &other) {
      auto prev = before_begin();
      auto left = begin();
      auto right = other.begin();
      while (left != end() && right != other.end()) {
        if (comp(*right, *left)) {
          splice_after(prev, other, other.before_begin());
          right = other.begin();
          ++prev;
        } else {
          prev = left;
          ++left;
        }
      }
      if (right != other.end()) {
        splice_after(prev, other);
      }
    }
  }
  void splice_after(const_iterator after, SList& other) {
    transfer_after(after, other, other.before_begin(), other.end());
  }
  void splice_after(const_iterator after, SList& other, const_iterator it) {
    if (after != it) {
      auto next = it;
      ++next;
      if (after != next && next != other.end()) {
        transfer_after(after, other, it, ++next);
      }
    }
  }
  void splice_after(const_iterator after, SList& other, const_iterator first, const_iterator last) {
    transfer_after(after, other, first, last);
  }
  void splice_after(const_iterator after, SList&& other) { splice_after(after, other); }
  void splice_after(const_iterator after, SList&& other, const_iterator it) { splice_after(after, other, it); }
  void splice_after(const_iterator after, SList&& other, const_iterator first, const_iterator last) {
    splice_after(after, other, first, last);
  }
  void reverse() {
    if (size() <= 1) {
      return;
    }
    auto it = begin();
    size_type count = size() - 1;
    while (count--) {
      splice_after(before_begin(), *this, it);
    }
  }
  void merge_sort() { merge_sort(std::less<T>()); }
  template <typename Compare>
  void merge_sort(Compare comp) {
    if (size() <= 1) {
      return;
    }
    SList right;
    right.splice_after(right.before_begin(), *this, gtl::next(before_begin(), size() / 2), end());
    merge_sort(comp);
    right.merge_sort(comp);
    merge(right, comp);
  }
  void sort() { sort(std::less<T>()); }
  template <typename Compare>
  void sort(Compare comp) {
    if (size() <= 1) {
      return;
    }
    SList left;
    SList right;
    for (size_type i = 1; i < size(); i *= 2) {
      // printf("i = %zu\n", i);
      size_type c = 2 * i;
      auto it = before_begin();
      size_type n = size() - size() % c;
      for (size_type j = 0; j < n; j += c) {
        left.splice_after(left.before_begin(), *this, it, gtl::next(it, i + 1));
        right.splice_after(right.before_begin(), *this, it, gtl::next(it, i + 1));
        // print_range("left", left.begin(), left.end());
        // print_range("right", right.begin(), right.end());
        left.merge(right);
        splice_after(it, left);
        it = gtl::next(it, c);
        // print_range("this", begin(), end());
      }
      n = size() % c;
      if (n > i) {
        // printf("process_end\n");
        left.splice_after(left.before_begin(), *this, it, gtl::next(it, i + 1));
        right.splice_after(right.before_begin(), *this, it, end());
        // print_range("left", left.begin(), left.end());
        // print_range("right", right.begin(), right.end());
        left.merge(right);
        splice_after(it, left);
        // print_range("this", begin(), end());
      }
    }
  }
  void qsort() { qsort(std::less<T>()); }
  template <typename Compare>
  void qsort(Compare comp) {
    if (size() <= 1) {
      return;
    }
    SList left;
    SList right;
    auto it = begin();
    auto prev = begin();
    ++it;
    while (it != end()) {
      if (comp(*it, front())) {
        left.splice_after(left.before_end(), *this, prev);
        it = prev;
        ++it;
      } else {
        prev = it;
        ++it;
      }
    }
    right.splice_after(right.before_begin(), *this, begin(), end());
    left.qsort(comp);
    right.qsort(comp);
    left.splice_after(left.before_end(), *this);
    left.splice_after(left.before_end(), right);
    *this = std::move(left);
  }

 private:
  template <typename InputIt>
  void assign_range(size_type count, InputIt first, InputIt last) {
    if (count == 0) {
      return;
    }
    if (count <= size()) {
      gtl::copy(first, last, begin());
      erase_after(gtl::next(before_begin(), count), end());
    } else {
      gtl::copy_n(first, size(), begin());
      insert_after(before_end(), gtl::next(first, size()), last);
    }
  }
  void assign_n(size_type count, const T& v) {
    if (count == 0) {
      return;
    }
    if (count <= size()) {
      gtl::fill_n(begin(), count, v);
      erase_after(gtl::next(before_begin(), count), end());
    } else {
      gtl::fill_n(begin(), size(), v);
      insert_after(before_end(), count, v);
    }
  }
  void transfer_after(const_iterator after, SList&& other, const_iterator first, const_iterator last) {
    transfer_after(after, other, first, last);
  }
  void transfer_after(const_iterator after, SList& other, const_iterator first, const_iterator last) {
    if (first != last && after != first) {
      SListNode* node = first.node->next;
      SListNode* after_node = after.node;
      if (node == after_node) {
        return;
      }
      while (node != last.node) {
        SListNode* next = remove_node(first.node, node);
        after_node = insert_node_after(after_node, node);
        if (this != &other) {
          --other.get_size();
          ++get_size();
        }
        node = next;
      }
    }
  }
  template <typename... Args>
  Node* construct_node(Args&&... args) {
    Node* node = allocator().allocate(1);
    construct_at(node, std::forward<Args>(args)...);
    return node;
  }
  void destroy_node(SListNode* node) {
    gtl::destroy_at(to_node(node));
    allocator().deallocate(to_node(node), 1);
  }
  void init() {
    dummy_head_ = new SListNode();
    get_size() = 0;
  }
  void destroy_list() {
    clear();
    delete dummy_head_;
    dummy_head_ = nullptr;
  }
  size_type& get_size() { return size_alloc_.first(); }
  const size_type& get_size() const { return size_alloc_.first(); }
  allocator_type& allocator() { return size_alloc_.second(); }
  const allocator_type& allocator() const { return size_alloc_.second(); }

  SListNode* dummy_head_;
  CompressedPair<size_type, allocator_type> size_alloc_;
};  // class SList

template <typename T>
using forward_list = SList<T>;

template <typename T>
bool operator==(const SList<T>& lhs, const SList<T>& rhs) {
  return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename T>
bool operator!=(const SList<T>& lhs, const SList<T>& rhs) {
  return !(lhs != rhs);
}

template <typename T>
bool operator<(const SList<T>& lhs, const SList<T>& rhs) {
  return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename T>
bool operator>(const SList<T>& lhs, const SList<T>& rhs) {
  return rhs < lhs;
}

template <typename T>
bool operator<=(const SList<T>& lhs, const SList<T>& rhs) {
  return !(lhs > rhs);
}

template <typename T>
bool operator>=(const SList<T>& lhs, const SList<T>& rhs) {
  return !(lhs < rhs);
}

}  // namespace gtl