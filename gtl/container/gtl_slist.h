/**
 * @file gtl_slist.h
 * @author gwq5210 (gwq5210@qq.com)
 * @brief 单链表的实现
 * @date 2021-05-15
 *
 * @copyright Copyright (c) 2021. All rights reserved.
 */

#pragma once

#include "gtl_memory.h"

#include <algorithm>
#include <iterator>
#include <type_traits>

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
      construct_at(&val, std::forward<Args>(args)...);
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

  SList() : dummy_head_(nullptr), size_(0) { init(); }
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
    std::swap(dummy_head_, other.dummy_head_);
    std::swap(size_, other.size_);
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
    assign_range(std::distance(first, last), first, last);
  }
  void assign(std::initializer_list<T> il) { assign_range(il.size(), il.begin(), il.end()); }
  void assign(const SList& other) {
    if (this != &other) {
      assign_range(other.size(), other.begin(), other.end());
    }
  }
  void assign(SList&& other) {
    clear();
    std::swap(dummy_head_, other.dummy_head_);
    std::swap(size_, other.size_);
  }

  allocator_type get_allocator() const { return allocator_; }

  // Element access
  T& front() { return *begin(); }
  const T& front() const { return *begin(); }
  T& back() { return *before_end(); }
  const T& back() const { return *before_end(); }

  // Iterators
  iterator before_begin() { return iterator(dummy_head_); }
  const_iterator before_begin() const { return iterator(dummy_head_); }
  const_iterator cbefore_begin() const { return before_begin(); };
  iterator before_end() { return std::next(before_begin(), size_); }
  const_iterator before_end() const { return std::next(before_begin(), size_); }
  const_iterator cbefore_end() const { return before_end(); };
  iterator begin() { return iterator(dummy_head_->next); };
  const_iterator begin() const { return const_iterator(dummy_head_->next); };
  const_iterator cbegin() const { return begin(); };
  iterator end() { return iterator(nullptr); };
  const_iterator end() const { return const_iterator(nullptr); };
  const_iterator cend() const { return end(); };

  // Capacity
  bool empty() const { return size_ == 0; }
  size_type size() const { return size_; }

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
    ++size_;
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
    --size_;
    return iterator(next);
  }
  iterator erase_after(const_iterator first, const_iterator last) {
    SListNode* node = first.node->next;
    while (node != last.node) {
      SListNode* next = remove_node(first.node, node);
      destroy_node(node);
      node = next;
      --size_;
    }
    return iterator(last.node);
  }
  void pop_front() { erase_after(before_begin()); }
  void resize(size_type count) { resize(count, T()); }
  void resize(size_type count, const T& v) {
    if (count > size_) {
      insert_after(before_end(), count - size_, v);
    } else if (count < size_) {
      erase_after(std::next(before_begin(), count), end());
    }
  }
  void clear() { erase_after(before_begin(), end()); }

  // Operations
  void merge(SList& other) { merge(other, std::less<>()); }
  void merge(SList&& other) { merge(other, std::less<>()); }
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
    if (size_ <= 1) {
      return;
    }
    auto it = begin();
    size_type count = size_ - 1;
    while (count--) {
      splice_after(before_begin(), *this, it);
    }
  }
  void sort() { sort(std::less<>()); }
  template <typename Compare>
  void sort(Compare comp) {
    if (size_ <= 1) {
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
    left.sort(comp);
    right.sort(comp);
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
    if (count <= size_) {
      std::copy(first, last, begin());
      erase_after(std::next(before_begin(), count), end());
    } else {
      std::copy_n(first, size_, begin());
      insert_after(before_end(), std::next(first, size_), last);
    }
  }
  void assign_n(size_type count, const T& v) {
    if (count == 0) {
      return;
    }
    if (count <= size_) {
      std::fill_n(begin(), count, v);
      erase_after(std::next(before_begin(), count), end());
    } else {
      std::fill_n(begin(), size_, v);
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
          --other.size_;
          ++size_;
        }
        node = next;
      }
    }
  }
  template <typename... Args>
  Node* construct_node(Args&&... args) {
    Node* node = allocator_.allocate(1);
    construct_at(node, std::forward<Args>(args)...);
    return node;
  }
  void destroy_node(SListNode* node) {
    std::destroy_at(to_node(node));
    allocator_.deallocate(to_node(node), 1);
  }
  void init() {
    dummy_head_ = new SListNode();
    size_ = 0;
  }
  void destroy_list() {
    clear();
    delete dummy_head_;
    dummy_head_ = nullptr;
  }
  SListNode* dummy_head_;
  size_type size_;
  allocator_type allocator_;
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