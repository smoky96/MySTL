#ifndef __MY_SET__H
#define __MY_SET__H

#include <initializer_list>
#include "my_tree.h"

namespace gd {

// 临时性使用，暂时还没写 functional，而 std::_Identity 在 MSVC 下会报错
template <typename _Tp>
struct identity {
  _Tp& operator()(_Tp& __x) const {
    return __x;
  }

  const _Tp& operator()(const _Tp& __x) const {
    return __x;
  }
};

template <typename Key, typename Compare = std::less<Key>, typename Alloc = alloc>
class set {
 public:
  typedef Key     key_type;
  typedef Key     value_type;
  typedef Compare key_compare;
  typedef Compare value_compare;

 private:  // 以红黑树为底层数据结构
  typedef rb_tree<key_type, value_type, identity<value_type>, key_compare, Alloc> __rep_type;

  __rep_type __tree;

 public:
  typedef typename __rep_type::const_pointer   pointer;
  typedef typename __rep_type::const_pointer   const_pointer;
  typedef typename __rep_type::const_reference reference;
  typedef typename __rep_type::const_reference const_reference;
  typedef typename __rep_type::const_iterator  iterator;
  typedef typename __rep_type::const_iterator  const_iterator;
  typedef typename __rep_type::size_type       size_type;
  typedef typename __rep_type::difference_type difference_type;
  typedef typename __rep_type::allocator_type  allocator_type;

 public:  // constructor, copy, destructor
  set() = default;

  template <typename InputIterator>
  set(InputIterator first, InputIterator last) : __tree() {
    __tree.insert_unique(first, last);
  }

  set(std::initializer_list<value_type> il) : __tree() {
    __tree.insert_unique(il.begin(), il.end());
  }

  set(const set& rhs) : __tree(rhs.__tree) {}

  set(set&& rhs) noexcept : __tree(std::move(rhs.__tree)) {}

  set& operator=(const set& rhs) {
    __tree = rhs.__tree;
    return *this;
  }

  set& operator=(set&& rhs) {
    __tree = std::move(rhs.__tree);
    return *this;
  }

  set& operator=(std::initializer_list<value_type> il) {
    __tree.clear();
    __tree.insert_unique(il.begin(), il.end());
    return *this;
  }

  allocator_type get_allocator() const noexcept {
    return allocator_type();
  }

 public:  // iterators
  iterator begin() noexcept {
    return __tree.begin();
  }

  const_iterator begin() const noexcept {
    return __tree.begin();
  }

  iterator end() noexcept {
    return __tree.end();
  }

  const_iterator end() const noexcept {
    return __tree.end();
  }

  const_iterator cbegin() const noexcept {
    return __tree.begin();
  }

  const_iterator cend() const noexcept {
    return __tree.end();
  }

 public:  // capacity
  bool empty() const noexcept {
    return __tree.empty();
  }

  size_type size() const noexcept {
    return __tree.size();
  }

  size_type max_size() const noexcept {
    return __tree.max_size();
  }

 public:  // modifiers
  template <typename... Args>
  std::pair<iterator, bool> emplace(Args&&... args) {
    return __tree.emplace_unique(std::forward<Args>(args)...);
  }

  // TODO(dong) iterator emplace_hint(iterator pos, Args&&... args);

  std::pair<iterator, bool> insert(const value_type& value) {
    return __tree.insert_unique(value);
  }

  std::pair<iterator, bool> insert(value_type&& value) {
    return __tree.insert_unique(std::move(value));
  }

  // TODO(dong) iterator insert(iterator pos, const value_type& value);

  // TODO(dong) iterator insert(iterator pos, value_type&& value);

  template <typename InputIterator>
  void insert(InputIterator first, InputIterator last) {
    __tree.insert_unique(first, last);
  }

  // TODO(dong) not c++11 yet
  void erase(iterator pos) {
    __tree.erase(pos.node);
  }

  size_type erase(const key_type& k) {
    return __tree.erase(k);
  }

  // TODO(dong) not c++11 yet
  void erase(iterator first, iterator last) {
    __tree.erase(first.node, last.node);
  }

  void swap(set& rhs) {
    __tree.swap(rhs.__tree);
  }

  void clear() noexcept {
    __tree.clear();
  }

 public:  // observers
  key_compare key_comp() const {
    return key_compare();
  }

  value_compare value_comp() const {
    return value_compare();
  }

 public:  // set operations
  iterator find(const key_type& k) {
    return __tree.find(k);
  }

  const_iterator find(const key_type& k) const {
    return __tree.find(k);
  }

  size_type count(const key_type& k) const {
    return __tree.count(k);
  }

  iterator lower_bound(const key_type& k) {
    return __tree.lower_bound(k);
  }

  const_iterator lower_bound(const key_type& k) const {
    return __tree.lower_bound(k);
  }

  iterator upper_bound(const key_type& k) {
    return __tree.upper_bound(k);
  }

  const_iterator upper_bound(const key_type& k) const {
    return __tree.upper_bound(k);
  }

  std::pair<iterator, iterator> equal_range(const key_type& k) {
    return __tree.equal_range(k);
  }

  std::pair<const_iterator, const_iterator> equal_range(const key_type& k) const {
    return __tree.equal_range(k);
  }

 public:
  bool operator==(const set& rhs) const {
    return __tree == rhs.__tree;
  }

  bool operator<(const set& rhs) const {
    return __tree < rhs.__tree;
  }
};

// operators

template <typename Key, typename Compare = std::less<Key>, typename Alloc = alloc>
bool operator==(const set<Key, Compare, Alloc>& lhs, const set<Key, Compare, Alloc>& rhs) {
  return lhs.operator==(rhs);
}

template <typename Key, typename Compare = std::less<Key>, typename Alloc = alloc>
bool operator!=(const set<Key, Compare, Alloc>& lhs, const set<Key, Compare, Alloc>& rhs) {
  return !(lhs == rhs);
}

template <typename Key, typename Compare = std::less<Key>, typename Alloc = alloc>
bool operator<(const set<Key, Compare, Alloc>& lhs, const set<Key, Compare, Alloc>& rhs) {
  return lhs.operator<(rhs);
}

template <typename Key, typename Compare = std::less<Key>, typename Alloc = alloc>
bool operator>=(const set<Key, Compare, Alloc>& lhs, const set<Key, Compare, Alloc>& rhs) {
  return !(lhs < rhs);
}

template <typename Key, typename Compare = std::less<Key>, typename Alloc = alloc>
bool operator>(const set<Key, Compare, Alloc>& lhs, const set<Key, Compare, Alloc>& rhs) {
  return rhs < lhs;
}

template <typename Key, typename Compare = std::less<Key>, typename Alloc = alloc>
bool operator<=(const set<Key, Compare, Alloc>& lhs, const set<Key, Compare, Alloc>& rhs) {
  return !(rhs < lhs);
}

template <typename Key, typename Compare = std::less<Key>, typename Alloc = alloc>
void swap(const set<Key, Compare, Alloc>& lhs, const set<Key, Compare, Alloc>& rhs) {
  lhs.swap(rhs);
}

// multiset
template <typename Key, typename Compare = std::less<Key>, typename Alloc = alloc>
class multiset {
 public:
  typedef Key     key_type;
  typedef Key     value_type;
  typedef Compare key_compare;
  typedef Compare value_compare;

 private:  // 以红黑树为底层数据结构
  typedef rb_tree<key_type, value_type, identity<value_type>, key_compare, Alloc> __rep_type;

  __rep_type __tree;

 public:
  typedef typename __rep_type::const_pointer   pointer;
  typedef typename __rep_type::const_pointer   const_pointer;
  typedef typename __rep_type::const_reference reference;
  typedef typename __rep_type::const_reference const_reference;
  typedef typename __rep_type::const_iterator  iterator;
  typedef typename __rep_type::const_iterator  const_iterator;
  typedef typename __rep_type::size_type       size_type;
  typedef typename __rep_type::difference_type difference_type;
  typedef typename __rep_type::allocator_type  allocator_type;

 public:  // constructor, copy, destructor
  multiset() = default;

  template <typename InputIterator>
  multiset(InputIterator first, InputIterator last) : __tree() {
    __tree.insert_equal(first, last);
  }

  multiset(std::initializer_list<value_type> il) : __tree() {
    __tree.insert_equal(il.begin(), il.end());
  }

  multiset(const multiset& rhs) : __tree(rhs.__tree) {}

  multiset(multiset&& rhs) noexcept : __tree(std::move(rhs.__tree)) {}

  multiset& operator=(const multiset& rhs) {
    __tree = rhs.__tree;
    return *this;
  }

  multiset& operator=(multiset&& rhs) {
    __tree = std::move(rhs.__tree);
    return *this;
  }

  multiset& operator=(std::initializer_list<value_type> il) {
    __tree.clear();
    __tree.insert_equal(il.begin(), il.end());
    return *this;
  }

  allocator_type get_allocator() const noexcept {
    return allocator_type();
  }

 public:  // iterators
  iterator begin() noexcept {
    return __tree.begin();
  }

  const_iterator begin() const noexcept {
    return __tree.begin();
  }

  iterator end() noexcept {
    return __tree.end();
  }

  const_iterator end() const noexcept {
    return __tree.end();
  }

  const_iterator cbegin() const noexcept {
    return __tree.begin();
  }

  const_iterator cend() const noexcept {
    return __tree.end();
  }

 public:  // capacity
  bool empty() const noexcept {
    return __tree.empty();
  }

  size_type size() const noexcept {
    return __tree.size();
  }

  size_type max_size() const noexcept {
    return __tree.max_size();
  }

 public:  // modifiers
  template <typename... Args>
  iterator emplace(Args&&... args) {
    return __tree.emplace_equal(std::forward<Args>(args)...);
  }

  // TODO(dong) iterator emplace_hint(iterator pos, Args&&... args);

  iterator insert(const value_type& value) {
    return __tree.insert_equal(value);
  }

  iterator insert(value_type&& value) {
    return __tree.insert_equal(std::move(value));
  }

  // TODO(dong) iterator insert(iterator pos, const value_type& value);

  // TODO(dong) iterator insert(iterator pos, value_type&& value);

  template <typename InputIterator>
  void insert(InputIterator first, InputIterator last) {
    __tree.insert_equal(first, last);
  }

  // TODO(dong) not c++11 yet
  void erase(iterator pos) {
    __tree.erase(pos.node);
  }

  size_type erase(const key_type& k) {
    return __tree.erase(k);
  }

  // TODO(dong) not c++11 yet
  void erase(iterator first, iterator last) {
    __tree.erase(first.node, last.node);
  }

  void swap(multiset& rhs) {
    __tree.swap(rhs.__tree);
  }

  void clear() noexcept {
    __tree.clear();
  }

 public:  // observers
  key_compare key_comp() const {
    return key_compare();
  }

  value_compare value_comp() const {
    return value_compare();
  }

 public:  // set operations
  iterator find(const key_type& k) {
    return __tree.find(k);
  }

  const_iterator find(const key_type& k) const {
    return __tree.find(k);
  }

  size_type count(const key_type& k) const {
    return __tree.count(k);
  }

  iterator lower_bound(const key_type& k) {
    return __tree.lower_bound(k);
  }

  const_iterator lower_bound(const key_type& k) const {
    return __tree.lower_bound(k);
  }

  iterator upper_bound(const key_type& k) {
    return __tree.upper_bound(k);
  }

  const_iterator upper_bound(const key_type& k) const {
    return __tree.upper_bound(k);
  }

  std::pair<iterator, iterator> equal_range(const key_type& k) {
    return __tree.equal_range(k);
  }

  std::pair<const_iterator, const_iterator> equal_range(const key_type& k) const {
    return __tree.equal_range(k);
  }

 public:
  bool operator==(const multiset& rhs) const {
    return __tree == rhs.__tree;
  }

  bool operator<(const multiset& rhs) const {
    return __tree < rhs.__tree;
  }
};

// operators

template <typename Key, typename Compare = std::less<Key>, typename Alloc = alloc>
bool operator==(const multiset<Key, Compare, Alloc>& lhs, const multiset<Key, Compare, Alloc>& rhs) {
  return lhs.operator==(rhs);
}

template <typename Key, typename Compare = std::less<Key>, typename Alloc = alloc>
bool operator!=(const multiset<Key, Compare, Alloc>& lhs, const multiset<Key, Compare, Alloc>& rhs) {
  return !(lhs == rhs);
}

template <typename Key, typename Compare = std::less<Key>, typename Alloc = alloc>
bool operator<(const multiset<Key, Compare, Alloc>& lhs, const multiset<Key, Compare, Alloc>& rhs) {
  return lhs.operator<(rhs);
}

template <typename Key, typename Compare = std::less<Key>, typename Alloc = alloc>
bool operator>=(const multiset<Key, Compare, Alloc>& lhs, const multiset<Key, Compare, Alloc>& rhs) {
  return !(lhs < rhs);
}

template <typename Key, typename Compare = std::less<Key>, typename Alloc = alloc>
bool operator>(const multiset<Key, Compare, Alloc>& lhs, const multiset<Key, Compare, Alloc>& rhs) {
  return rhs < lhs;
}

template <typename Key, typename Compare = std::less<Key>, typename Alloc = alloc>
bool operator<=(const multiset<Key, Compare, Alloc>& lhs, const multiset<Key, Compare, Alloc>& rhs) {
  return !(rhs < lhs);
}

template <typename Key, typename Compare = std::less<Key>, typename Alloc = alloc>
void swap(const multiset<Key, Compare, Alloc>& lhs, const multiset<Key, Compare, Alloc>& rhs) {
  lhs.swap(rhs);
}

}  // namespace gd

#endif  //!__MY_SET__H
