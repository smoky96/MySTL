#ifndef __MY_MAP__H
#define __MY_MAP__H

#include <functional>
#include <initializer_list>
#include "my_tree.h"

namespace gd {

template <typename Pair>
struct select1st {
  typedef typename Pair::first_type result_type;
  typedef Pair                      argument_type;

  const typename Pair::first_type& operator()(const Pair& x) const {
    return x.first;
  }
};

template <typename Key, typename T, typename Compare = std::less<Key>, typename Alloc = alloc>
class map {
 public:
  typedef Key                     key_type;
  typedef T                       mapped_type;
  typedef std::pair<const Key, T> value_type;
  typedef Compare                 key_compare;

  // 比较元素 key 的大小
  class value_compare {
    friend class map;

   protected:
    Compare comp;
    value_compare(Compare c) : comp(c) {}

   public:
    typedef bool       result_type;
    typedef value_type first_argument_type;
    typedef value_type second_argument_type;

    bool operator()(const value_type& lhs, const value_type& rhs) const {
      return comp(lhs.first, rhs.first);
    }
  };

 private:
  typedef rb_tree<key_type, value_type, select1st<value_type>, key_compare, Alloc> __rep_type;
  // 底层数据结构
  __rep_type __tree;

 public:
  typedef typename __rep_type::pointer         pointer;
  typedef typename __rep_type::const_pointer   const_pointer;
  typedef typename __rep_type::reference       reference;
  typedef typename __rep_type::const_reference const_reference;
  typedef typename __rep_type::iterator        iterator;
  typedef typename __rep_type::const_iterator  const_iterator;
  typedef typename __rep_type::size_type       size_type;
  typedef typename __rep_type::difference_type difference_type;
  typedef typename __rep_type::allocator_type  allocator_type;

 public:  // constructor, copy, destructor
  map() = default;

  template <typename InputIterator>
  map(InputIterator first, InputIterator last) : __tree() {
    __tree.insert_unique(first, last);
  }

  map(std::initializer_list<value_type> il) : __tree() {
    __tree.insert_unique(il.begin(), il.end());
  }

  map(const map& rhs) : __tree(rhs.__tree) {}

  map(map&& rhs) noexcept : __tree(std::move(rhs.__tree)) {}

  map& operator=(const map& rhs) {
    __tree = rhs.__tree;
    return *this;
  }

  map& operator=(map&& rhs) {
    __tree = std::move(rhs.__tree);
    return *this;
  }

  map& operator=(std::initializer_list<value_type> il) {
    __tree.clear();
    __tree.insert_unique(il.begin(), il.end());
    return *this;
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
    return __tree.max_size;
  }

 public:  // element access
  mapped_type& operator[](const key_type& k) {
    iterator it = __tree.lower_bound(k);
    if (it == end() || key_compare()(k, it->first))
      it = __tree.emplace_unique(it, k, T{});
    return it->second;
  }

  mapped_type& operator[](const key_type&& k) {
    iterator it = __tree.lower_bound(k);
    if (it == end() || key_compare()(k, it->first))
      it = __tree.emplace_unique(it, std::move(k), T{});
    return it->second;
  }

  // TODO(dong) mapped_type& at(const key_type& k);

  // TODO(dong) const mapped_type& at(const key_type& k) const;

 public:  // modifiers
  template <typename... Args>
  std::pair<iterator, bool> emplace(Args&&... args) {
    return __tree.emplace_unique(std::forward<Args>(args)...);
  }

  template <typename... Args>
  iterator emplace_hint(iterator pos, Args&&... args) {
    return __tree.emplace_unique(pos, std::forward<Args>(args)...);
  }

  std::pair<iterator, bool> insert(const value_type& v) {
    return __tree.insert_unique(v);
  }

  std::pair<iterator, bool> insert(value_type&& v) {
    return __tree.insert_unique(std::move(v));
  }

  iterator insert(iterator pos, const value_type& v) {
    return __tree.insert_unique(pos, std::move(v));
  }

  template <typename InputIterator>
  void insert(InputIterator first, InputIterator last) {
    __tree.insert_unique(first, last);
  }

  void insert(std::initializer_list<value_type> il) {
    __tree.insert_unique(il.begin(), il.end());
  }

  // TODO(dong) not c++11 yet
  void erase(iterator pos) {
    __tree.erase(pos);
  }

  size_type erase(const key_type& k) {
    return __tree.erase(k);
  }

  // TODO(dong) not c++11 yet
  void erase(iterator first, iterator last) {
    __tree.erase(first, last);
  }

  void clear() {
    __tree.clear();
  }

 public:  // observers
  key_compare key_comp() const {
    return __tree._key_compare();
  }

  value_compare value_comp() const {
    return value_compare(__tree._key_compare());
  }

 public:  // map operations
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

  void swap(map& rhs) noexcept {
    __tree.swap(rhs.__tree);
  }

 public:  // operators
  bool operator==(const map& rhs) const {
    return __tree == rhs.__tree;
  }

  bool operator<(const map& rhs) const {
    return __tree < rhs.__tree;
  }
};

// operators:

template <typename Key, typename T, typename Compare = std::less<Key>, typename Alloc = alloc>
bool operator==(const map<Key, T, Compare, Alloc>& lhs, const map<Key, T, Compare, Alloc>& rhs) {
  return lhs.operator==(rhs);
}

template <typename Key, typename T, typename Compare = std::less<Key>, typename Alloc = alloc>
bool operator!=(const map<Key, T, Compare, Alloc>& lhs, const map<Key, T, Compare, Alloc>& rhs) {
  return !(lhs == rhs);
}

template <typename Key, typename T, typename Compare = std::less<Key>, typename Alloc = alloc>
bool operator<(const map<Key, T, Compare, Alloc>& lhs, const map<Key, T, Compare, Alloc>& rhs) {
  return lhs.operator<(rhs);
}

template <typename Key, typename T, typename Compare = std::less<Key>, typename Alloc = alloc>
bool operator>=(const map<Key, T, Compare, Alloc>& lhs, const map<Key, T, Compare, Alloc>& rhs) {
  return !(lhs < rhs);
}

template <typename Key, typename T, typename Compare = std::less<Key>, typename Alloc = alloc>
bool operator>(const map<Key, T, Compare, Alloc>& lhs, const map<Key, T, Compare, Alloc>& rhs) {
  return rhs.operator<(lhs);
}

template <typename Key, typename T, typename Compare = std::less<Key>, typename Alloc = alloc>
bool operator<=(const map<Key, T, Compare, Alloc>& lhs, const map<Key, T, Compare, Alloc>& rhs) {
  return !(lhs > rhs);
}

template <typename Key, typename T, typename Compare = std::less<Key>, typename Alloc = alloc>
void swap(const map<Key, T, Compare, Alloc>& lhs, const map<Key, T, Compare, Alloc>& rhs) {
  lhs.swap(rhs);
}

template <typename Key, typename T, typename Compare = std::less<Key>, typename Alloc = alloc>
class multimap {
 public:
  typedef Key                     key_type;
  typedef T                       mapped_type;
  typedef std::pair<const Key, T> value_type;
  typedef Compare                 key_compare;

  // 比较元素 key 的大小
  class value_compare {
    friend class multimap;

   protected:
    Compare comp;
    value_compare(Compare c) : comp(c) {}

   public:
    typedef bool       result_type;
    typedef value_type first_argument_type;
    typedef value_type second_argument_type;

    bool operator()(const value_type& lhs, const value_type& rhs) const {
      return comp(lhs.first, rhs.first);
    }
  };

 private:
  typedef rb_tree<key_type, value_type, select1st<value_type>, key_compare, Alloc> __rep_type;
  // 底层数据结构
  __rep_type __tree;

 public:
  typedef typename __rep_type::pointer         pointer;
  typedef typename __rep_type::const_pointer   const_pointer;
  typedef typename __rep_type::reference       reference;
  typedef typename __rep_type::const_reference const_reference;
  typedef typename __rep_type::iterator        iterator;
  typedef typename __rep_type::const_iterator  const_iterator;
  typedef typename __rep_type::size_type       size_type;
  typedef typename __rep_type::difference_type difference_type;
  typedef typename __rep_type::allocator_type  allocator_type;

 public:  // constructor, copy, destructor
  multimap() = default;

  template <typename InputIterator>
  multimap(InputIterator first, InputIterator last) : __tree() {
    __tree.insert_equal(first, last);
  }

  multimap(std::initializer_list<value_type> il) : __tree() {
    __tree.insert_equal(il.begin(), il.end());
  }

  multimap(const multimap& rhs) : __tree(rhs.__tree) {}

  multimap(multimap&& rhs) noexcept : __tree(std::move(rhs.__tree)) {}

  multimap& operator=(const multimap& rhs) {
    __tree = rhs.__tree;
    return *this;
  }

  multimap& operator=(multimap&& rhs) {
    __tree = std::move(rhs.__tree);
    return *this;
  }

  multimap& operator=(std::initializer_list<value_type> il) {
    __tree.clear();
    __tree.insert_equal(il.begin(), il.end());
    return *this;
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

 public:  // cpacity
  bool empty() const noexcept {
    return __tree.empty();
  }

  size_type size() const noexcept {
    return __tree.size();
  }

  size_type max_size() const noexcept {
    return __tree.max_size;
  }

 public:  // modifiers
  template <typename... Args>
  iterator emplace(Args&&... args) {
    return __tree.emplace_equal(std::forward<Args>(args)...);
  }

  template <typename... Args>
  iterator emplace_hint(iterator pos, Args&&... args) {
    return __tree.emplace_equal(pos, std::forward<Args>(args)...);
  }

  iterator insert(const value_type& v) {
    return __tree.insert_equal(v);
  }

  iterator insert(value_type&& v) {
    return __tree.insert_equal(std::move(v));
  }

  iterator insert(iterator pos, const value_type& v) {
    return __tree.insert_equal(pos, std::move(v));
  }

  template <typename InputIterator>
  void insert(InputIterator first, InputIterator last) {
    __tree.insert_equal(first, last);
  }

  void insert(std::initializer_list<value_type> il) {
    __tree.insert_equal(il.begin(), il.end());
  }

  // TODO(dong) not c++11 yet
  void erase(iterator pos) {
    __tree.erase(pos);
  }

  size_type erase(const key_type& k) {
    return __tree.erase(k);
  }

  // TODO(dong) not c++11 yet
  void erase(iterator first, iterator last) {
    __tree.erase(first, last);
  }

  void clear() {
    __tree.clear();
  }

 public:  // observers
  key_compare key_comp() const {
    return __tree._key_compare();
  }

  value_compare value_comp() const {
    return value_compare(__tree._key_compare());
  }

 public:  // map operations
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

  void swap(multimap& rhs) noexcept {
    __tree.swap(rhs.__tree);
  }

 public:  // operators
  bool operator==(const multimap& rhs) const {
    return __tree == rhs.__tree;
  }

  bool operator<(const multimap& rhs) const {
    return __tree < rhs.__tree;
  }
};

// operators:

template <typename Key, typename T, typename Compare = std::less<Key>, typename Alloc = alloc>
bool operator==(const multimap<Key, T, Compare, Alloc>& lhs, const multimap<Key, T, Compare, Alloc>& rhs) {
  return lhs.operator==(rhs);
}

template <typename Key, typename T, typename Compare = std::less<Key>, typename Alloc = alloc>
bool operator!=(const multimap<Key, T, Compare, Alloc>& lhs, const multimap<Key, T, Compare, Alloc>& rhs) {
  return !(lhs == rhs);
}

template <typename Key, typename T, typename Compare = std::less<Key>, typename Alloc = alloc>
bool operator<(const multimap<Key, T, Compare, Alloc>& lhs, const multimap<Key, T, Compare, Alloc>& rhs) {
  return lhs.operator<(rhs);
}

template <typename Key, typename T, typename Compare = std::less<Key>, typename Alloc = alloc>
bool operator>=(const multimap<Key, T, Compare, Alloc>& lhs, const multimap<Key, T, Compare, Alloc>& rhs) {
  return !(lhs < rhs);
}

template <typename Key, typename T, typename Compare = std::less<Key>, typename Alloc = alloc>
bool operator>(const multimap<Key, T, Compare, Alloc>& lhs, const multimap<Key, T, Compare, Alloc>& rhs) {
  return rhs.operator<(lhs);
}

template <typename Key, typename T, typename Compare = std::less<Key>, typename Alloc = alloc>
bool operator<=(const multimap<Key, T, Compare, Alloc>& lhs, const multimap<Key, T, Compare, Alloc>& rhs) {
  return !(lhs > rhs);
}

template <typename Key, typename T, typename Compare = std::less<Key>, typename Alloc = alloc>
void swap(const multimap<Key, T, Compare, Alloc>& lhs, const multimap<Key, T, Compare, Alloc>& rhs) {
  lhs.swap(rhs);
}

}  // namespace gd

#endif  //!__MY_MAP__H