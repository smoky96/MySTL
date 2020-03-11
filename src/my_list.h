#ifndef __MY_LIST_H
#define __MY_LIST_H

#include <algorithm>
#include <initializer_list>
#include "my_alloc.h"
#include "my_iterator.h"

namespace gd {

template <typename T>
struct list_node {
  typedef void* void_pointer;
  // T 是泛型，不确定，所以要声明为 void* 类型
  void_pointer _prev;
  void_pointer _next;
  T            _data;
};

template <typename T>
struct list_iterator : public iterator<bidirectional_iterator_tag, T> {
  typedef T                          value_type;
  typedef T*                         pointer;
  typedef T&                         reference;
  typedef size_t                     size_type;
  typedef ptrdiff_t                  difference_type;
  typedef bidirectional_iterator_tag iterator_category;

  typedef list_node<T>     l_node;
  typedef l_node*          link_type;
  typedef list_iterator<T> self;

  link_type node;  // pointer to current node

  // constructor
  list_iterator() = default;
  list_iterator(link_type nd) : node(nd) {}
  list_iterator(const self& rhs) : node(rhs.node) {}

  // operators
  bool operator==(const self& rhs) const {
    return node == rhs.node;
  }

  bool operator!=(const self& rhs) const {
    return !(*this == rhs);
  }

  // 这里的 * 操作符和 *this 中的 * 可不一样啊,
  // this 是指向这个自己这个类的一个指针，类型是 list_iterator<T>*，
  // 而我们重载的这个 * 是对这个对象使用的 *，比如 *it (it 的类型为 list_iterator<T>)
  reference operator*() const {
    return node->_data;
  }

  pointer operator->() const {
    return &(operator*());
  }

  self& operator++() {
    node = static_cast<link_type>(node->_next);
    return *this;
  }

  self operator++(int) {
    self tmp = *this;
    ++*this;
    return tmp;
  }

  self& operator--() {
    node = static_cast<link_type>(node->_prev);
    return *this;
  }

  self operator--(int) {
    self tmp = *this;
    --*this;
    return tmp;
  }
};

template <typename T>
struct list_const_iterator : public iterator<bidirectional_iterator_tag, T> {
  typedef T                          value_type;
  typedef const T*                   pointer;    // 指向常量
  typedef const T&                   reference;  // 引用常量
  typedef size_t                     size_type;
  typedef ptrdiff_t                  difference_type;
  typedef bidirectional_iterator_tag iterator_category;

  typedef list_node<T>           l_node;
  typedef l_node*                link_type;
  typedef list_const_iterator<T> self;

  link_type node;  // pointer to current node

  // constructors
  list_const_iterator() = default;
  list_const_iterator(link_type nd) : node(nd) {}
  list_const_iterator(const list_iterator<T>& rhs) : node(rhs.node) {}
  list_const_iterator(const list_const_iterator& rhs) : node(rhs.node) {}

  // operators
  bool operator==(const self& rhs) const {
    return node == rhs.node;
  }

  bool operator!=(const self& rhs) const {
    return !(*this == rhs);
  }

  reference operator*() const {
    return node->_data;
  }

  pointer operator->() const {
    return &(operator*());
  }

  self& operator++() {
    node = static_cast<link_type>(node->_next);
    return *this;
  }

  self operator++(int) {
    self tmp = *this;
    ++*this;
    return tmp;
  }

  self& operator--() {
    node = static_cast<link_type>(node->_prev);
    return *this;
  }

  self operator--(int) {
    self tmp = *this;
    --*this;
    return tmp;
  }
};

template <typename T, typename Alloc = alloc>
class list {
 public:  // 内嵌性别定义
  typedef T                 value_type;
  typedef value_type*       pointer;
  typedef const value_type* const_pointer;
  typedef value_type&       reference;
  typedef const value_type& const_reference;
  typedef size_t            size_type;
  typedef ptrdiff_t         difference_type;

  typedef list_iterator<T>       iterator;
  typedef list_const_iterator<T> const_iterator;
  typedef list_node<T>           l_node;
  typedef l_node*                link_type;

 protected:
  typedef simple_alloc<T, Alloc>      allocator_type;
  typedef simple_alloc<T, Alloc>      data_allocator;
  typedef simple_alloc<l_node, Alloc> node_allocator;

  link_type _node;  // 指向双向环状链表的头节点
  size_type _size;  // 大小

  link_type _get_node() {
    return node_allocator::allocate();
  }

  void _put_node(link_type p) {
    node_allocator::deallocate(p);
    p = 0;
  }

  template <typename... Args>
  link_type _create_node(Args&&... args) {
    link_type p = _get_node();
    try {
      construct(&(p->_data), std::forward<Args>(args)...);
      p->_prev = nullptr;
      p->_next = nullptr;
    } catch (...) {
      _put_node(p);
      throw;
    }
    return p;
  }

  void _destroy_node(link_type p) {
    destroy(&(p->_data));
    _put_node(p);
  }

  // 将 [first, last) 迁移到 pos 之前
  void _transfer(const_iterator pos, const_iterator first, const_iterator last) {
    if (pos != last) {
      link_type p = pos.node;
      link_type fp = first.node;
      link_type lp = last.node;

      static_cast<link_type>(lp->_prev)->_next = p;
      static_cast<link_type>(fp->_prev)->_next = lp;
      static_cast<link_type>(p->_prev)->_next = fp;
      link_type tmp = static_cast<link_type>(p->_prev);
      p->_prev = lp->_prev;
      lp->_prev = fp->_prev;
      fp->_prev = tmp;
    }
  }

 private:  // helper functions
  // 将 [first, last] 接到头部
  void __link_front(link_type first, link_type last) {
    first->_prev = _node;
    last->_next = _node->_next;
    static_cast<link_type>(_node->_next)->_prev = last;
    _node->_next = first;
  }

  // 将 [first, last] 接到尾部
  void __link_back(link_type first, link_type last) {
    last->_next = _node;
    first->_prev = _node->_prev;
    static_cast<link_type>(_node->_prev)->_next = first;
    _node->_prev = last;
  }

  // 将 [first, last] 接到 p 前面
  void __link(link_type p, link_type first, link_type last) {
    first->_prev = p->_prev;
    last->_next = p;
    static_cast<link_type>(p->_prev)->_next = first;
    p->_prev = last;
  }

  // 将 [first, last] 断开
  void __unlink(link_type first, link_type last) {
    static_cast<link_type>(first->_prev)->_next = last->_next;
    static_cast<link_type>(last->_next)->_prev = first->_prev;
    first->_prev = nullptr;
    last->_next = nullptr;
  }

  void __fill_init(size_type n, const_reference value) {
    _node = _get_node();
    _node->_next = _node->_prev = _node;
    _size = n;
    try {
      for (; n > 0; --n) {
        link_type p = _create_node(value);
        __link_back(p, p);
      }
    } catch (...) {
      clear();
      _put_node(_node);
      throw;
    }
  }

  iterator __fill_insert(const_iterator pos, size_type n, const_reference value) {
    link_type first = _create_node(value);
    first->_next = first->_prev = nullptr;
    int n_copy = n;
    --n;
    try {
      link_type cur = first;
      for (; n > 0; --n) {
        link_type node = _create_node(value);
        cur->_next = node;
        node->_prev = cur;
        cur = node;
      }
      _size += n_copy;
      first->_prev = cur;
    } catch (...) {
      while (first) {
        link_type tmp = first;
        first = static_cast<link_type>(first->_next);
        _destroy_node(tmp);
      }
      throw;
    }
    __link(pos.node, first, static_cast<link_type>(first->_prev));
    return iterator(first);
  }

  // 从 [first, last) 建立双向链表
  template <typename InputIterator>
  void __copy_init(InputIterator first, InputIterator last) {
    _node = _get_node();
    _node->_next = _node->_prev = _node;
    size_type n = distance(first, last);
    _size = n;
    try {
      for (; n > 0; --n, ++first) {
        // 记住用户在使用的是一个 sequential container，而不是链表！
        link_type cur = _create_node(*first);
        __link_back(cur, cur);
      }
    } catch (...) {
      clear();
      _put_node(_node);
      throw;
    }
  }

  // 将 [first, last) 插入到 pos 前, first != last
  template <typename InputIterator>
  iterator __copy_insert(const_iterator pos, InputIterator first, InputIterator last) {
    link_type start = _create_node(*first);
    start->_next = start->_prev = nullptr;
    size_type n = distance(first, last);
    size_type n_copy = n;
    ++first;
    --n;

    try {
      link_type cur = start;
      // 比较 n 会比 比较迭代器要快，所以这里用 n 判断循环结束条件
      link_type node;
      for (; n > 0; --n, ++first) {
        node = _create_node(*first);
        cur->_next = node;
        node->_prev = cur;
        cur = node;
      }
      _size += n_copy;
      start->_prev = cur;
    } catch (...) {
      while (start) {
        link_type tmp = start;
        start = static_cast<link_type>(tmp->_next);
        _destroy_node(tmp);
      }
      throw;
    }
    __link(pos.node, start, static_cast<link_type>(start->_prev));
    return iterator(start);
  }

  void __fill_assign(size_type n, const_reference value) {
    iterator first = begin();
    iterator last = end();
    for (; n > 0 && first != last; --n, ++first) {
      *first = value;
    }

    if (n > 0) {
      insert(last, n, value);
    } else {
      erase(first, last);
    }
  }

  // 用 [first, last) 为 list 重新赋值
  template <typename InputIterator>
  void __copy_assign(InputIterator first, InputIterator last) {
    iterator f_it = begin();
    iterator l_it = end();
    for (; f_it != l_it && first != last; ++f_it, ++first) {
      *f_it = *first;
    }

    if (f_it == l_it) {
      insert(f_it, first, last);
    } else {
      erase(f_it, l_it);
    }
  }

 public:  // constructors, copy and destructor
  list() {
    __fill_init(0, value_type());
  }

  explicit list(size_type n) {
    __fill_init(n, value_type());
  }

  list(size_type n, const_reference value) {
    __fill_init(n, value);
  }

  // TODO(dong): 下面这个模板初始化函数和上面的函数冲突了！e.g., list(6, 8) 会调用下面这个，而非上面的
  template <typename InputIterator>
  list(InputIterator first, InputIterator last) {
    __copy_init(first, last);
  }

  list(std::initializer_list<value_type> il) {
    __copy_init(il.begin(), il.end());
  }

  list(const list& rhs) {
    __copy_init(rhs.cbegin(), rhs.cend());
  }

  list(list&& rhs) noexcept : _node(rhs._node), _size(rhs._size) {
    // 一定要记得将 rhs._node 置为空！不然 rhs 析构的时候会 deallocate 已经被移走的资源
    rhs._node = 0;
    rhs._size = 0;
  }

  void assign(size_type n, const_reference value) {
    __fill_assign(n, value);
  }

  // TODO(dong): 下面这个模板 assign 函数和上面的函数冲突了！e.g., assign(6, 8) 会调用下面这个，而非上面的
  template <typename InputIterator>
  void assign(InputIterator first, InputIterator last) {
    __copy_assign(first, last);
  }

  void assign(std::initializer_list<value_type> il) {
    __copy_assign(il.begin(), il.end());
  }

  list& operator=(const list& rhs) {
    if (this != &rhs) {
      assign(rhs.begin(), rhs.end());
    }
    return *this;
  }

  list& operator=(list&& rhs) {
    clear();
    splice(end(), rhs);
    return *this;
  }

  ~list() {
    // 因为有移动构造函数的存在，所以一定要检查 _node 是否为空
    // 有可能 _node 的资源已经被移动走了，这时不能进行 deallocate
    if (_node) {
      clear();
      _put_node(_node);
      _size = 0;
    }
  }

  allocator_type get_allocator() const {
    return node_allocator();
  }

 public:  // iterators
  iterator begin() noexcept {
    return iterator(static_cast<link_type>(_node->_next));
  }

  const_iterator begin() const noexcept {
    return const_iterator(static_cast<link_type>(_node->_next));
  }

  iterator end() noexcept {
    return iterator(_node);
  }

  const_iterator end() const noexcept {
    return const_iterator(_node);
  }

  const_iterator cbegin() const noexcept {
    return const_iterator(static_cast<link_type>(_node->_next));
  }
  const_iterator cend() const noexcept {
    return const_iterator(_node);
  }

 public:  // capacity
  bool empty() const noexcept {
    return _node->_next == _node;
  }

  size_type size() const noexcept {
    return _size;
  }

  size_type max_size() const noexcept {
    // -1 的补码是 1111 1111 1111 1111 ...
    return static_cast<size_type>(-1);
  }

  void resize(size_type n) {
    resize(n, value_type());
  }

  void resize(size_type n, const_reference value) {
    iterator  i = begin();
    size_type len = 0;
    while (i != end() && len < n) {
      ++i;
      ++len;
    }

    if (len == n) {
      erase(i, end());
    } else {
      insert(begin(), n - len, value);
    }
  }

 public:  // element access
  reference front() {
    return *(begin());
  }

  const_reference front() const {
    return *(begin());
  }

  reference back() {
    iterator tmp = end();
    return *--tmp;
  }

  const_reference back() const {
    iterator tmp = end();
    return *--tmp;
  }

 public:  // modifiers
  template <typename... Args>
  void emplace_front(Args&&... args) {
    link_type node = _create_node(std::forward<Args>(args)...);
    __link_front(node, node);
    ++_size;
  }

  void pop_front() {
    erase(begin());
  }

  template <typename... Args>
  void emplace_back(Args&&... args) {
    link_type node = _create_node(std::forward<Args>(args)...);
    __link_back(node, node);
    ++_size;
  }

  void pop_back() {
    iterator tmp = end();
    erase(--tmp);
  }

  void push_front(const_reference value) {
    insert(begin(), value);
  }

  void push_front(T&& value) {
    insert(begin(), std::move(value));
  }

  void push_back(const_reference value) {
    insert(end(), value);
  }

  void push_back(T&& value) {
    insert(end(), std::move(value));
  }

  template <typename... Args>
  iterator emplace(const_iterator pos, Args&&... args) {
    // 此处可接收左值也可接收右值，采用 forward 可完整保留 args 的属性
    link_type node = _create_node(std::forward<Args>(args)...);
    __link(pos.node, node, node);
    ++_size;
    return iterator(node);
  }

  iterator insert(const_iterator pos, const_reference value) {
    link_type node = _create_node(value);
    __link(pos.node, node, node);
    ++_size;
    return iterator(node);
  }

  iterator insert(const_iterator pos, T&& value) {
    // 这里只接收右值，所以下面用 move
    link_type node = _create_node(std::move(value));
    __link(pos.node, node, node);
    ++_size;
    return iterator(node);
  }

  iterator insert(const_iterator pos, size_type n, const_reference value) {
    return __fill_insert(pos, n, value);
  }

  // TODO(dong): 下面这个模板初始化函数和上面的函数冲突了！e.g., insert(pos, 6, 8) 会调用下面这个，而非上面的
  template <typename InputIterator>
  iterator insert(const_iterator pos, InputIterator first, InputIterator last) {
    if (first != last)
      return __copy_insert(pos, first, last);
    else
      return iterator(pos.node);
  }

  iterator insert(const_iterator pos, std::initializer_list<T> il) {
    if (il.size() != 0)
      return __copy_insert(pos, il.begin(), il.end());
    else
      return iterator(pos.node);
  }

  iterator erase(const_iterator pos) {
    link_type next = static_cast<link_type>(pos.node->_next);
    __unlink(pos.node, pos.node);
    _destroy_node(pos.node);
    --_size;
    return iterator(next);
  }

  iterator erase(const_iterator first, const_iterator last) {
    while (first != last) {
      erase(first++);
    }
    return iterator(last.node);
  }

  void swap(list& rhs) {
    std::swap(_node, rhs._node);
    std::swap(_size, rhs._size);
  }

  void clear() noexcept {
    link_type cur = static_cast<link_type>(_node->_next);
    while (cur != _node) {
      link_type tmp = cur;
      cur = static_cast<link_type>(cur->_next);
      _destroy_node(tmp);
    }
    _node->_next = _node->_prev = _node;
    _size = 0;
  };

 public:  // list operations:
  // 将 x 接合于 pos 之前，x 不为 *this
  void splice(const_iterator pos, list& x) {
    if (!x.empty()) {
      _transfer(pos, x.begin(), x.end());
      _size += x._size;
      x._size = 0;
    }
  }

  // 将 i 所指元素接合于 pos 之前，pos 和 i 可以为同一个 list
  void splice(const_iterator pos, list& x, const_iterator i) {
    iterator j = iterator(i.node);
    ++j;
    if (pos == i || pos == j)
      return;
    _transfer(pos, i, j);
    ++_size;
    --x._size;
  }

  // 将 [first, last) 内的元素接合到 pos 之前，pos 不能位于 [first, last) 内
  void splice(const_iterator pos, list& x, const_iterator first, const_iterator last) {
    if (first != last) {
      size_type n = distance(first, last);
      _transfer(pos, first, last);
      _size += n;
      x._size -= n;
    }
  }

  void remove(const_reference value) {
    iterator first = begin();
    iterator last = end();
    while (first != last) {
      iterator tmp = first;
      ++first;
      if (*tmp == value) {
        erase(tmp);
      }
    }
  }

  template <typename UnaryPredicate>
  void remove_if(UnaryPredicate pred) {
    iterator first = begin();
    iterator last = end();
    while (first != last) {
      iterator tmp = first;
      ++first;
      if (pred(*tmp)) {
        erase(tmp);
      }
    }
  }

  // 移除连续且相同的元素, 只保留一个
  void unique() {
    iterator first = begin();
    iterator last = end();
    iterator next = first;
    while (++next != last) {
      if (*first == *next) {
        erase(next);
        next = first;
      } else {
        first = next;
      }
    }
  }

  template <typename BinaryPredicate>
  void unique(BinaryPredicate pred) {
    iterator first = begin();
    iterator last = end();
    iterator next = first;
    while (++next != last) {
      if (pred(*first, *next)) {
        erase(next);
        next = first;
      } else {
        first = next;
      }
    }
  }

  // 将两个递增排序的 list 合并
  void merge(list& x) {
    if (this != &x) {
      iterator first_1 = begin();
      iterator last_1 = end();
      iterator first_2 = x.begin();
      iterator last_2 = x.end();

      while (first_1 != last_1 && first_2 != last_2) {
        if (*first_2 < *first_1) {
          iterator next = first_2;
          _transfer(first_1, first_2, ++next);
          first_2 = next;
        } else {
          ++first_1;
        }
      }
      if (first_2 != last_2) {
        _transfer(first_1, first_2, last_2);
      }
      _size += x._size;
      x._size = 0;
    }
  }

  template <typename Compare>
  void merge(list& x, Compare comp) {
    if (this != &x) {
      iterator first_1 = begin();
      iterator last_1 = end();
      iterator first_2 = x.begin();
      iterator last_2 = x.end();

      while (first_1 != last_1 && first_2 != last_2) {
        if (comp(*first_2, *first_1)) {
          iterator next = first_2;
          _transfer(first_1, first_2, ++next);
          first_2 = next;
        } else {
          ++first_1;
        }
      }
      if (first_2 != last_2) {
        _transfer(first_1, first_2, last_2);
      }
      _size += x._size;
      x._size = 0;
    }
  }

  void sort() {
    // 归并排序
    if (_size <= 1)
      return;

    list<T, Alloc> carry;
    list<T, Alloc> counter[64];
    int            fill = 0;
    while (!empty()) {
      carry.splice(carry.begin(), *this, begin());
      int i = 0;
      while (i < fill && !counter[i].empty()) {
        // 将 carry 合并到第 i 个 counter 上
        counter[i].merge(carry);
        // 再将合并后的链表放到 carry 当中，并把 i 加一
        // 直到合并到填充过的 counter, 或者当前 counter 空为止，有点像网络编程中的 select, poll
        carry.swap(counter[i++]);
        // CPRINT(carry, display_int);
      }
      carry.swap(counter[i]);
      // 如果 i == fill，就代表又多用了一个 counter，所以要把 fill 加一
      if (i == fill)
        ++fill;
    }
    for (int i = 1; i < fill; ++i) {
      counter[i].merge(counter[i - 1]);
    }
    swap(counter[fill - 1]);
  }

  template <typename Compare>
  void sort(Compare comp) {
    // 归并排序
    if (_size <= 1)
      return;

    list<T, Alloc> carry;
    list<T, Alloc> counter[64];
    int            fill = 0;
    while (!empty()) {
      carry.splice(carry.begin(), *this, begin());
      int i = 0;
      while (i < fill && !counter[i].empty()) {
        // 将 carry 合并到第 i 个 counter 上
        counter[i].merge(carry, comp);
        // 再将合并后的链表放到 carry 当中，并把 i 加一
        // 直到合并到填充过的 counter, 或者当前 counter 空为止，有点像网络编程中的 select, poll
        carry.swap(counter[i++]);
      }
      carry.swap(counter[i]);
      // 如果 i == fill，就代表又多用了一个 counter，所以要把 fill 加一
      if (i == fill)
        ++fill;
    }
    for (int i = 1; i < fill; ++i) {
      counter[i].merge(counter[i - 1], comp);
    }
    swap(counter[fill - 1]);
  }

  void reverse() noexcept {
    if (_size <= 1) {
      return;
    }

    iterator first = begin();
    ++first;
    while (first != end()) {
      iterator old = first;
      ++first;
      _transfer(begin(), old, first);
    }
  }
};

template <typename T, typename Alloc>
bool operator==(const list<T, Alloc>& lhs, const list<T, Alloc>& rhs) {
  if (lhs.size() != rhs.size()) {
    return false;
  }

  auto first_1 = lhs.begin();
  auto last_1 = lhs.end();
  auto first_2 = rhs.begin();
  auto last_2 = rhs.end();

  for (; first_1 != last_1 && first_2 != last_2 && *first_1 == *first_2; ++first_1, ++first_2)
    ;
  return first_1 == last_1 && first_2 == last_2;
}

template <typename T, typename Alloc>
bool operator<(const list<T, Alloc>& lhs, const list<T, Alloc>& rhs) {
  return std::lexicographical_compare(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
}

template <typename T, typename Alloc>
bool operator!=(const list<T, Alloc>& lhs, const list<T, Alloc>& rhs) {
  return !(lhs == rhs);
}

template <typename T, typename Alloc>
bool operator>(const list<T, Alloc>& lhs, const list<T, Alloc>& rhs) {
  return rhs < lhs;
}

template <typename T, typename Alloc>
bool operator>=(const list<T, Alloc>& lhs, const list<T, Alloc>& rhs) {
  return !(lhs < rhs);
}

template <typename T, typename Alloc>
bool operator<=(const list<T, Alloc>& lhs, const list<T, Alloc>& rhs) {
  return !(rhs < lhs);
}

// 对 list 进行特化
template <typename T>
void swap(list<T>& lhs, list<T>& rhs) noexcept {
  lhs.swap(rhs);
}

}  // namespace gd

#endif  // !__MY_LIST_H