#pragma once

#include "gtl_memory.h"

#include <algorithm>
#include <iterator>
#include <type_traits>

namespace gtl {

struct ListNode {
  ListNode* next;
  ListNode* prev;
  ListNode() : next(nullptr), prev(nullptr) {}
};

struct ListIterator {
  typedef std::bidirectional_iterator_tag iterator_category;
  typedef ListIterator Self;
  ListNode* node;
  ListIterator() : node(nullptr) {}
  ListIterator(ListNode* n) : node(n) {}

  Self& operator++() {
    node = node->next;
    return *this;
  }
  Self operator++(int) {
    auto* ret = node;
    node = node->next;
    return Self(ret);
  }
  Self& operator--() {
    node = node->prev;
    return *this;
  }
  Self operator--(int) {
    auto* ret = node;
    node = node->prev;
    return Self(ret);
  }
  bool operator==(const Self& other) const { return node == other.node; }
  bool operator!=(const Self& other) const { return node != other.node; }
};

inline ListNode* remove_node(ListNode* curr) {
  curr->prev->next = curr->next;
  curr->next->prev = curr->prev;
  curr->prev = nullptr;
  curr->next = nullptr;
  return curr;
}

inline ListNode* insert_node(ListNode* before, ListNode* node) {
  node->next = before;
  node->prev = before->prev;
  before->prev->next = node;
  before->prev = node;
  return node;
}

inline ListNode* remove_range(ListNode* first, ListNode* last) {
  last = last->prev;
  first->prev->next = last->next;
  last->next->prev = first->prev;
  first->prev = nullptr;
  last->next = nullptr;
  return first;
}

inline ListNode* insert_range(ListNode* before, ListNode* first, ListNode* last) {
  last->next = before;
  first->prev = before->prev;
  before->prev->next = first;
  before->prev = last;
  return first;
}

template <typename T>
class List {
 public:
  struct Node : public ListNode {
    T val;
    template <typename... Args>
    Node(Args&&... args) {
      construct_at(&val, std::forward<Args>(args)...);
    }
  };

  typedef T value_type;
  typedef T& reference;
  typedef const T& const_reference;
  typedef T* pointer;
  typedef const T* const_pointer;
  typedef size_t size_type;
  typedef ptrdiff_t difference_type;
  typedef std::allocator<Node> allocator_type;

  static Node* to_node(ListNode* list_node) { return (Node *)list_node; }
  static T& node_value(ListNode* list_node) { return to_node(list_node)->val; }

  struct CIterator : public ListIterator {
    typedef List::const_reference reference;
    typedef List::const_pointer pointer;
    typedef List::difference_type difference_type;
    typedef List::value_type value_type;
    typedef ListIterator BaseType;
    typedef CIterator Self;
    explicit CIterator(ListNode* node) : BaseType(node) {}
    reference operator*() const { return to_node(node)->val; }
    pointer operator->() const { return &(to_node(node)->val); }
    Self& operator++() {
      BaseType::operator++();
      return *this;
    }
    Self operator++(int) {
      auto ret = *this;
      BaseType::operator++();
      return ret;
    }
    Self& operator--() {
      BaseType::operator--();
      return *this;
    }
    Self operator--(int) {
      auto ret = *this;
      BaseType::operator--();
      return ret;
    }
  };

  struct Iterator : public CIterator {
    typedef List::reference reference;
    typedef List::pointer pointer;
    typedef List::difference_type difference_type;
    typedef List::value_type value_type;
    typedef CIterator BaseType;
    typedef Iterator Self;
    explicit Iterator(ListNode* node) : BaseType(node) {}
    reference operator*() const { return const_cast<reference>(BaseType::operator*()); }
    pointer operator->() const { return const_cast<pointer>(BaseType::operator->()); }
    Self& operator++() {
      BaseType::operator++();
      return *this;
    }
    Self operator++(int) {
      auto ret = *this;
      BaseType::operator++();
      return ret;
    }
    Self& operator--() {
      BaseType::operator--();
      return *this;
    }
    Self operator--(int) {
      auto ret = *this;
      BaseType::operator--();
      return ret;
    }
  };

  typedef Iterator iterator;
  typedef CIterator const_iterator;
  typedef std::reverse_iterator<iterator> reverse_iterator;
  typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

  List(): dummy_head_(nullptr), size_(0) { init(); }
  explicit List(size_type count) {
    init();
    insert(begin(), count, T());
  }
  List(size_type count, const T& v) {
    init();
    insert(begin(), count, v);
  }
  template <typename II>
  List(II first, II last) {
    init();
    insert(begin(), first, last);
  }
  List(const List& other) {
    init();
    insert(begin(), other.begin(), other.end());
  }
  List(List&& other) {
    init();
    std::swap(dummy_head_, other.dummy_head_);
    std::swap(size_, other.size_);
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

  void assign(size_type count, const T& v) {
    insert(begin(), count, v);
  }
  template <typename II>
  void assign(II first, II last) {
    assign_range(std::distance(first, last), first, last);
  }
  void assign(std::initializer_list<T> il) {
    assign_range(il.size(), il.begin(), il.end());
  }
  void assign(const List& other) {
    if (this != &other) {
      assign_range(other.size(), other.begin(), other.end());
    }
  }
  void assign(List&& other) {
    erase(begin(), end());
    std::swap(dummy_head_, other.dummy_head_);
    std::swap(size_, other.size_);
  }

  allocator_type get_allocator() const { return allocator_; }

  // element access
  T& front() { return *begin(); }
  const T& front() const { return *begin(); }
  T& back() { return node_value(dummy_head_->prev); }
  const T& back() const { return node_value(dummy_head_->prev); }

  // iterators
  iterator begin() { return iterator(dummy_head_->next); };
  const_iterator begin() const { return const_iterator(dummy_head_->next); };
  const_iterator cbegin() const { return begin(); };
  iterator end() { return iterator(dummy_head_); };
  const_iterator end() const { return const_iterator(dummy_head_); };
  const_iterator cend() const { return end(); };
  iterator rbegin() { return reverse_iterator(end()); }
  iterator rbegin() const { return const_reverse_iterator(end()); }
  iterator crbegin() const { return rbegin(); }
  iterator rend() { return reverse_iterator(begin()); }
  iterator rend() const { return const_reverse_iterator(begin()); }
  iterator crend() const { return rend(); }

  // capacity
  bool empty() const { return size_ == 0; }
  size_type size() const { return size_; }

  // modifiers
  iterator insert(const_iterator before, const T& v) { return emplace(before, v); }
  iterator insert(const_iterator before, size_type count, const T& v) {
    while (count--) {
      before = emplace(before, v);
    }
    return before;
  }
  template <typename II>
  iterator insert(const_iterator before, II first, II last) {
    while (first != last) {
      emplace(before, *first++);
    }
    return iterator(before.node);
  }
  iterator insert(const_iterator before, std::initializer_list<T> il) {
    return insert(before, il.begin(), il.end());
  }
  iterator insert(const_iterator before, T&& v) { return emplace(before, v); }
  template <typename... Args>
  iterator emplace(const_iterator before, Args&&... args) {
    size_++;
    return iterator(insert_node(before.node, construct_node(std::forward<Args>(args)...)));
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
  iterator push_back(T&& v) { return emplace_back(v); }
  iterator push_front(const T& v) { return emplace_front(v); }
  iterator push_front(T&& v) { return emplace_front(v); }
  iterator erase(const_iterator pos) {
    auto* curr = pos.node;
    if (pos != end()) {
      auto* next = curr->next;
      remove_node(curr);
      destroy_node(curr);
      curr = next;
      size_--;
    }
    return iterator(curr);
  }
  iterator erase(const_iterator first, const_iterator last) {
    if (first != last) {
      auto* node = remove_range(first.node, last.node);
      while (node) {
        auto* next = node->next;
        destroy_node(node);
        node = next;
        size_--;
      }
    }
    return iterator(last.node);
  }
  void pop_back() { erase(const_iterator(dummy_head_.prev)); }
  void pop_front() { erase(begin()); }
  void resize(size_type count) { resize(count, T()); }
  void resize(size_type count, const T& v) {
    if (count > size_) {
      insert(end(), count - size_, v);
    } else if (count < size_) {
      auto *it = end();
      for (int i = 0; i < size_ - count; i++) {
        erase(--it);
      }
    }
  }
  void clear() { erase(begin(), end()); }

  // operations
  void merge(List&& other) { merge(std::move(other), std::less()); }
  template <typename Compare>
  void merge(List&& other, Compare comp) {
    if (this != &other) {
      auto left = begin();
      auto right = other.begin();
      while (left != end() && right != other.end()) {
        if (comp(*right, *left)) {
          printf("%d %d\n", *right, *left);
          auto next = right;
          transfer(left, right, ++next, other);
          right = next;
        } else {
          ++left;
        }
      }
      if (right != other.end()) {
        printf("test\n");
        transfer(left, right, other.end(), other);
      }
    }
  }

 private:
  template <typename II>
  void assign_range(size_type count, II first, II last) {
    if (count == 0) {
      return;
    }
    if (count <= size_) {
      auto it = std::copy(first, last, begin());
      erase(it, end());
    } else {
      auto it = begin();
      for (int i = 0; i < size_; i++) {
        *it++ = *first++;
      }
      insert(end(), first, last);
    }
  }
  void assign_n(size_type count, const T& v) {
    if (count == 0) {
      return;
    }
    if (count <= size_) {
      auto it = std::fill_n(begin(), count, v);
      erase(it, end());
    } else {
      auto it = begin();
      for (int i = 0; i < size_; i++) {
        *it++ = v;
      }
      insert(end(), count, v);
    }
  }
  void transfer(const_iterator before, const_iterator first, const_iterator last, List& other) {
    if (first != last) {
      size_type count = std::distance(first, last);
      auto* tail = last.node->prev;
      remove_range(first.node, last.node);
      insert_range(before.node, first.node, tail);
      if (this != &other) {
        other.size_ -= count;
        size_ += count;
      }
    }
  }
  template <typename... Args>
  Node* construct_node(Args&&... args) {
    Node* node = allocator_.allocate(1);
    construct_at(node, std::forward<Args>(args)...);
    return node;
  }
  void destroy_node(ListNode* node) {
    std::destroy_at(to_node(node));
    allocator_.deallocate(to_node(node), 1);
  }

  void init() {
    dummy_head_ = allocator_.allocate(1);
    dummy_head_->next = dummy_head_;
    dummy_head_->prev = dummy_head_;
    size_ = 0;
  }
  void destroy_list() {
    erase(begin(), end());
    allocator_.deallocate(dummy_head_, 1);
    dummy_head_ = nullptr;
    size_ = 0;
  }
  Node* dummy_head_;
  size_type size_;
  allocator_type allocator_;
};

}  // namespace gtl