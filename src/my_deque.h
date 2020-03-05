#ifndef __MY_DEQUE_H
#define __MY_DEQUE_H

#include "exceptdef.h"
#include "my_alloc.h"
#include "my_iterator.h"
#include "my_uninitialized.h"

namespace gd {

#ifndef INIT_MAP_SIZE
#define INIT_MAP_SIZE 8
#endif  // !INIT_MAP_SIZE

// 决定 buffer size 大小，
// 若 n 不为 0，则返回 n，
// 若 n 为 0，若 n < 512 则返回 512 / sz, 否则返回 1
template <typename T>
inline size_t __deque_buf_size(size_t n, T*) {
  size_t sz = sizeof(T);
  return n != 0 ? n : (sz < 512 ? size_t(512 / sz) : size_t(1));
}

template <typename T, typename Ref, typename Ptr, size_t BufSize>
struct deque_iterator : public iterator<random_access_iterator_tag, T> {
  typedef T                          value_type;
  typedef Ptr                        pointer;
  typedef Ref                        reference;
  typedef size_t                     size_type;
  typedef ptrdiff_t                  difference_type;
  typedef random_access_iterator_tag iterator_category;

  typedef deque_iterator<T, T&, T*, BufSize>             iterator;
  typedef deque_iterator<T, const T&, const T*, BufSize> const_iterator;

  // 这里由于 T* 和 Ptr 是分开的，所以不能这样声明：typedef pointer* map_pointer
  typedef T**            map_pointer;
  typedef deque_iterator self;

  pointer     cur;    // 指向当前元素
  pointer     first;  // 指向缓冲区第一个元素
  pointer     last;   // 指向 one past the 缓冲区的最后一个元素
  map_pointer node;   // 指回到中控台

  static size_type buffer_size() {
    return __deque_buf_size(BufSize, static_cast<pointer>(0));
  }

  // constructors
  deque_iterator() noexcept : cur(0), first(0), last(0), node(0){};

  deque_iterator(pointer vp, map_pointer mp) : cur(vp), first(*mp), last(*mp + buffer_size()), node(mp) {}

  deque_iterator(const iterator& rhs) : cur(rhs.cur), first(rhs.first), last(rhs.last), node(rhs.node) {}

  deque_iterator(iterator&& rhs) : cur(rhs.cur), first(rhs.first), last(rhs.last), node(rhs.node) {
    rhs.cur = 0;
    rhs.first = 0;
    rhs.last = 0;
    rhs.node = 0;
  }

  // 搞不懂这里怎么回事，pointer to const 和 referene to const 怎么可能可以给 nonconst 赋值？？
  // deque_iterator(const const_iterator& rhs) : cur(rhs.cur), first(rhs.first), last(rhs.last), node(rhs.node) {}

  self& operator=(const iterator& rhs) {
    if (&rhs != this) {
      cur = rhs.cur;
      first = rhs.first;
      last = rhs.last;
      node = rhs.node;
    }
    return *this;
  }

  // 跳向 new_node 所指示的缓冲区
  void set_node(map_pointer new_node) {
    node = new_node;
    first = *new_node;
    last = *new_node + buffer_size();
  }

  // operators
  reference operator*() const {
    return *cur;
  }

  pointer operator->() const {
    return &(operator*());
  }

  difference_type operator-(const self& rhs) const {
    return static_cast<difference_type>((node - rhs.node - 1) * buffer_size() + (rhs.last - rhs.cur) + (cur - first));
  }

  self& operator++() {
    ++cur;
    if (cur == last) {
      set_node(node + 1);
      cur = first;
    }
    return *this;
  }

  self& operator--() {
    if (cur == first) {
      set_node(node - 1);
      cur = last;
    }
    --cur;
    return *this;
  }

  self& operator+=(difference_type n) {
    difference_type offset = (cur - first) + n;
    if (offset >= 0 && offset < static_cast<difference_type>(buffer_size())) {
      cur += n;
    } else {
      // 这里不知道是咋算的额
      difference_type node_offset = offset > 0 ? (offset / static_cast<difference_type>(buffer_size()))
                                               : (-static_cast<difference_type>((-offset - 1) / buffer_size()) - 1);
      set_node(node + node_offset);
      cur = first + (offset - node_offset * static_cast<difference_type>(buffer_size()));
    }
    return *this;
  }

  self operator+(difference_type n) const {
    // 这里不能改变原迭代器，应返回右值
    self tmp = *this;
    return tmp += n;
  }

  self& operator-=(difference_type n) {
    return *this += -n;
  }

  self operator-(difference_type n) const {
    self tmp = *this;
    return tmp -= n;
  }

  // 迭代器跳跃，非 deque 的索引跳跃
  reference operator[](difference_type n) const {
    // 最终还是返回迭代器指向的值
    return *(*this + n);
  }

  bool operator==(const self& rhs) const {
    return cur == rhs.cur;
  }

  bool operator!=(const self& rhs) const {
    return !(cur == rhs.cur);
  }

  bool operator>(const self& rhs) const {
    return node == rhs.node ? cur > rhs.node : node > rhs.node;
  }

  bool operator<(const self& rhs) const {
    return rhs < *this;
  }

  bool operator>=(const self& rhs) const {
    return !(*this < rhs);
  }

  bool operator<=(const self& rhs) const {
    return !(*this > rhs);
  }
};

template <typename T, typename Alloc = alloc, size_t BufSize = 0>
class deque {
 public:  // 内嵌型别
  typedef T                 value_type;
  typedef value_type*       pointer;
  typedef const value_type* const_pointer;
  typedef value_type&       reference;
  typedef const value_type& const_reference;
  typedef size_t            size_type;
  typedef ptrdiff_t         difference_type;

  typedef deque_iterator<value_type, reference, pointer, BufSize>             iterator;
  typedef deque_iterator<value_type, const_reference, const_pointer, BufSize> const_iterator;

  typedef simple_alloc<T, Alloc>  allocator_type;
  typedef simple_alloc<T, Alloc>  data_allocator;
  typedef simple_alloc<T*, Alloc> map_allocator;

 protected:
  typedef T** map_pointer;  // 指向缓冲区的指针

  iterator    _start;     // 第一个节点
  iterator    _finish;    // 最后一个节点
  map_pointer _map;       // 指向中空台的指针
  size_type   _map_size;  // 中控台的大小

  static size_t _buffer_size() {
    return __deque_buf_size(BufSize, static_cast<pointer>(0));
  }

 private:  // helper functions
  void __fill_init(size_type n, const_reference value) {
    __map_nodes_init(n);
    if (n != 0) {
      for (map_pointer cur = _start.node; cur != _finish.node; ++cur) {
        uninitialized_fill(*cur, *cur + _buffer_size(), value);
      }
      uninitialized_fill(_finish.first, _finish.last, value);
    }
  }

  void __map_nodes_init(size_t n_elem) {
    size_type num_nodes = n_elem / _buffer_size() + 1;              // 所需节点数
    _map_size = std::max(size_type(INIT_MAP_SIZE), num_nodes + 2);  // 最少 8 个，最多为所需节点数 + 2
    _map = __allocate_map(_map_size);

    map_pointer mp_start = _map + (_map_size - num_nodes) / 2;  // 将 node 放在中间部分，两边都留有空余
    map_pointer mp_finish = mp_start + num_nodes - 1;

    map_pointer cur;
    try {
      for (cur = mp_start; cur <= mp_finish; ++cur) {
        *cur = __allocate_data();
      }
    } catch (...) {
      while (cur-- != mp_start) {
        __deallocate_data(*cur);
        *cur = 0;
        throw;
      }
    }

    _start.set_node(mp_start);
    _start.cur = _start.first;
    _finish.set_node(mp_finish);
    _finish.cur = _finish.first + n_elem % _buffer_size();
  }

  template <typename InputIterator>
  void __copy_init(InputIterator first, InputIterator last, input_iterator_tag) {
    size_type n = distance(first, last);
    __map_nodes_init(n);
    iterator cur = _start;
    for (; cur != _finish; ++cur, ++first) {
      construct(&(*cur), *first);
    }
  }

  template <typename ForwardIterator>
  void __copy_init(ForwardIterator first, ForwardIterator last, forward_iterator_tag) {
    size_type n = distance(first, last);
    __map_nodes_init(n);
    for (map_pointer cur = _start.node; cur != _finish.node; ++cur) {
      ForwardIterator mid = first;
      advance(mid, _buffer_size());
      uninitialized_copy(first, mid, *cur);
      first = mid;
    }
    uninitialized_copy(first, last, _finish.first);
  }

  template <typename InputIterator>
  void __copy_assign(InputIterator first, InputIterator last, input_iterator_tag) {
    size_type n = distance(first, last);
    iterator  first_t = begin();
    iterator  last_t = end();

    for (; first != last && first_t != last_t; ++first, ++first_t) {
      *first_t = *first;
    }
    if (first != last) {
      insert(end(), first, last);
    } else {
      erase(first_t, last_t);
    }
  }

  template <typename ForwardIterator>
  void __copy_assign(ForwardIterator first, ForwardIterator last, forward_iterator_tag()) {
    size_type n = distance(first, last);
    if (n > size()) {
      iterator mid = first;
      advance(mid, size());
      std::copy(first, mid, begin());
      insert(end(), mid, last);
    } else {
      erase(std::copy(first, last, begin()), end());
    }
  }

  template <typename... Args>
  iterator __insert_aux(iterator pos, Args&&... args) {
    const size_type elem_before = pos - _start;
    value_type      value_copy = value_type(std::forward<Args>(args)...);
    if (elem_before < size() / 2) {
      emplace_front(front());
      iterator front1 = _start;
      ++front1;
      iterator front2 = front1;
      ++front2;
      // 经过 emplace 过后，pos 可能已经实效
      pos = _start + elem_before;
      iterator pos1 = pos;
      ++pos1;
      std::copy(front2, pos1, front1);
    } else {
      emplace_back(back());
      iterator back1 = _finish;
      --back1;
      iterator back2 = back1;
      --back2;
      // 经过 emplace 过后，pos 可能已经实效
      pos = _start + elem_before;
      std::copy_backward(pos, back2, back1);
    }
    *pos = std::move(value_copy);
    return pos;
  }

  void __insert_aux(iterator pos, size_type n, const_reference value) {
    const size_type elem_before = pos - _start;
    const size_type len = size();
    if (elem_before < len / 2) {
      iterator new_start = __reserve_elem_at_front(n);
      // 这时的 _start 还是指向已经存了元素的部分的尾部，而新分配的空间的尾部由 new_start 指示
      iterator old_start = _start;
      pos = _start + elem_before;
      try {
        if (elem_before >= n) {
          iterator start_n = _start + difference_type(n);
          uninitialized_copy(_start, start_n, new_start);
          _start = new_start;
          std::copy(start_n, pos, old_start);
          std::fill_n(pos - difference_type(n), n, value);
        } else {
          uninitialized_fill_n(uninitialized_copy(_start, pos, new_start), n - elem_before, value);
          _start = new_start;
          std::fill_n(old_start, elem_before, value);
        }
      } catch (...) {
        __destroy_buffer(new_start.node, _start.node);
        throw;
      }
    } else {
      iterator new_finish = __reserve_elem_at_back(n);
      // 这时的 _finish 还是指向已经存了元素的部分的尾部，而新分配的空间的尾部由 new_finish 指示
      iterator        old_finish = _finish;
      const size_type elem_after = len - elem_before;
      pos = _finish - elem_after;
      try {
        if (elem_after > n) {
          iterator finish_n = _finish - difference_type(n);
          uninitialized_copy(finish_n, _finish, _finish);
          _finish = new_finish;
          std::copy_backward(pos, finish_n, old_finish);
          std::fill_n(pos, n, value);
        } else {
          uninitialized_copy(pos, _finish, uninitialized_fill_n(_finish, n - elem_after, value));
          _finish = new_finish;
          std::fill_n(pos, elem_after, value);
        }
      } catch (...) {
        __destroy_buffer(_finish.node + 1, new_finish.node + 1);
        throw;
      }
    }
  }

  template <typename InputIterator>
  void __insert_dispatch(iterator pos, InputIterator first, InputIterator last, size_type n, input_iterator_tag) {
    if (last <= first) {
      return;
    }
    const size_type elem_before = pos - _start;
    if (elem_before < size() / 2) {
      __reserve_elem_at_front(n);
    } else {
      __reserve_elem_at_back(n);
    }
    pos = _start + elem_before;
    InputIterator cur = --last;
    for (size_type i = 0; i < n; ++i, --cur) {
      insert(pos, *cur);
    }
  }

  template <typename ForwardIterator>
  void __insert_dispatch(iterator pos, ForwardIterator first, ForwardIterator last, size_type n, forward_iterator_tag) {
    const size_type elem_before = pos - _start;
    const size_type len = size();
    if (elem_before < len / 2) {
      iterator new_start = __reserve_elem_at_front(n);
      iterator old_start = _start;
      pos = _start + difference_type(elem_before);
      try {
        if (elem_before >= n) {
          iterator start_n = _start + difference_type(n);
          uninitialized_copy(_start, start_n, new_start);
          _start = new_start;
          std::copy(start_n, pos, old_start);
          std::copy(first, last, pos - difference_type(n));
        } else {
          ForwardIterator mid = first;
          advance(mid, n - elem_before);
          uninitialized_copy(first, mid, uninitialized_copy(_start, pos, new_start));
          _start = new_start;
          std::copy(mid, last, old_start);
        }
      } catch (...) {
        __destroy_buffer(new_start.node, _start.node);
        throw;
      }
    } else {
      const size_type elem_after = len - elem_before;
      iterator        new_finish = __reserve_elem_at_back(n);
      iterator        old_finish = _finish;
      pos = _finish - difference_type(elem_after);
      try {
        if (elem_after > n) {
          iterator finish_n = _finish - difference_type(n);
          uninitialized_copy(finish_n, _finish, _finish);
          _finish = new_finish;
          std::copy_backward(pos, finish_n, old_finish);
          std::copy(first, last, pos);
        } else {
          ForwardIterator mid = first;
          advance(mid, n - elem_after);
          uninitialized_copy(pos, _finish, uninitialized_copy(mid, last, _finish));
          _finish = new_finish;
          std::copy(first, mid, pos);
        }
      } catch (...) {
        __destroy_buffer(_finish.node + 1, new_finish.node + 1);
        throw;
      }
    }
  }

  template <typename... Args>
  void __push_front_aux(Args&&... args) {
    value_type value_copy = value_type(std::forward<Args>(args)...);
    __reserve_map_at_front(1);
    *(_start.node - 1) = __allocate_data();
    try {
      _start.set_node(_start.node - 1);
      _start.cur = _start.last - 1;
      construct(_start.cur, value_copy);
    } catch (...) {
      _start.set_node(_start.node + 1);
      _start.cur = _start.first;
      __deallocate_data(*(_start.node - 1));
      throw;
    }
  }

  template <typename... Args>
  void __push_back_aux(Args&&... args) {
    value_type value_copy = value_type(std::forward<Args>(args)...);
    __reserve_map_at_back(1);
    *(_finish.node + 1) = __allocate_data();
    try {
      construct(_finish.cur, value_copy);
      _finish.set_node(_finish.node + 1);
      _finish.cur = _finish.first;
    } catch (...) {
      __deallocate_data(*(_finish.node + 1));
      throw;
    }
  }

  void __reallocate_map(size_type need, bool add_front) {
    size_type old_num_node = _finish.node - _start.node + 1;
    size_type new_num_node = old_num_node + need;

    map_pointer new_start;
    if (_map_size > 2 * new_num_node) {
      // 如果当前的 map 大小比需要的 node 大两倍多,
      // 那么就不重新分配空间，而是把 node 移到整个 map 的中间部分
      new_start = _map + (_map_size - new_num_node) / 2 + (add_front ? need : 0);
      if (new_start < _start.node) {
        std::copy(_start.node, _finish.node + 1, new_start);
      } else {
        std::copy_backward(_start.node, _finish.node + 1, new_start + old_num_node);
      }
    } else {
      size_type   new_map_size = _map_size + std::max(_map_size, new_num_node) + 2;  // 两端多一个备用？
      map_pointer new_map = __allocate_map(new_map_size);
      // 这里的新开始位置后面或者前面要留一部分空间给待会儿(应该就是在这个函数返回到外层调用之时)需要加入的元素，上同
      new_start = new_map + (new_map_size - new_num_node) / 2 + (add_front ? need : 0);
      std::copy(_start.node, _finish.node + 1, new_start);

      __deallocate_map(_map, _map_size);
      _map = new_map;
      _map_size = new_map_size;
    }
    _start.set_node(new_start);
    _finish.set_node(new_start + old_num_node - 1);  // 始终记住 finish 直接指向最后一个 node，而不是 the one past！
  }

  void __reserve_map_at_back(size_type need) {
    if (need > _map_size - (_finish.node - _map + 1)) {
      // 尾端空间不够，则需要重新分配
      __reallocate_map(need, false);
    }
  }

  void __reserve_map_at_front(size_type need) {
    if (need > _start.node - _map) {
      __reallocate_map(need, true);
    }
  }

  iterator __reserve_elem_at_front(size_type need) {
    size_type vacancies = _start.cur - _start.first;
    if (need > vacancies) {
      __new_elem_at_front(need - vacancies);
    }
    return _start - difference_type(need);
  }

  iterator __reserve_elem_at_back(size_type need) {
    size_type vacancies = (_finish.last - _finish.cur) - 1;
    if (need > vacancies) {
      __new_elem_at_back(need - vacancies);
    }
    return _finish + difference_type(need);
  }

  void __new_elem_at_front(size_type n) {
    size_type new_num_node = (n + _buffer_size() - 1) / _buffer_size();  // 为什么要减 1 ？
    // 假设 n 刚好为 buffer_size 的倍数，那么如果不减一的话，就会多分配一个
    __reserve_map_at_front(new_num_node);
    size_type i;
    try {
      for (i = 1; i <= new_num_node; ++i) {
        *(_start.node - i) = __allocate_data();
      }
    } catch (...) {
      --i;
      for (; i > 0; --i) {
        __deallocate_data(*(_start.node - i));
      }
      throw;
    }
  }

  void __new_elem_at_back(size_type n) {
    size_type new_num_node = (n + _buffer_size() - 1) / _buffer_size();
    __reserve_map_at_back(new_num_node);
    size_type i;
    try {
      for (i = 1; i <= new_num_node; ++i) {
        *(_finish.node + i) = __allocate_data();
      }
    } catch (...) {
      --i;
      for (; i > 0; --i) {
        __deallocate_data(*(_finish.node + i));
      }
      throw;
    }
  }

  void __destroy_buffer(map_pointer first, map_pointer last) {
    for (map_pointer cur = first; cur != last; ++cur) {
      __deallocate_data(*cur);
    }
  }

  pointer __allocate_data() {
    return data_allocator::allocate(_buffer_size());
  }

  void __deallocate_data(pointer p) {
    if (p)
      data_allocator::deallocate(p, _buffer_size());
  }

  map_pointer __allocate_map(size_type n) {
    map_pointer mp = map_allocator::allocate(n);
    // 这里一定要把 *(mp+i) 初始化为空！！！太坑了！！！以后一定要养成初始化指针的良好习惯！
    for (size_type i = 0; i < n; ++i) {
      *(mp + i) = 0;
    }
    return mp;
  }

  void __deallocate_map(map_pointer p, size_type n) {
    if (p)
      map_allocator::deallocate(p, n);
  }

 public:  // constructors, copy and destructor
  deque() {
    __fill_init(0, value_type());
  }

  explicit deque(size_type n) {
    __fill_init(n, value_type());
  }

  deque(size_type n, const_reference value) {
    __fill_init(n, value);
  }

  template <typename InputIterator>
  deque(InputIterator first, InputIterator last) {
    __copy_init(first, last, iterator_category(first));
  }

  deque(const deque& rhs) {
    __copy_init(rhs._start, rhs._finish, forward_iterator_tag());
  }

  deque(deque&& rhs) : _start(rhs._start), _finish(rhs._finish), _map(rhs._map), _map_size(rhs._map_size) {
    rhs._map = 0;
    rhs._map_size = 0;
  }

  deque(std::initializer_list<value_type> il) {
    __copy_init(il.begin(), il.end(), forward_iterator_tag());
  }

  ~deque() {
    if (_map) {
      clear();
      __deallocate_data(*(_start.node));
      __deallocate_map(_map, _map_size);
    }
  }

  deque& operator=(const deque& rhs) {
    if (this != &rhs) {
      size_type sz = size();
      if (sz > rhs.size()) {
        erase(std::copy(rhs.begin(), rhs.end(), _start), _finish);
      } else {
        const_iterator mid = rhs.begin() + static_cast<difference_type>(sz);
        std::copy(rhs.begin(), mid, _start);
        insert(_finish, mid, rhs.end());
      }
    }
    return *this;
  }

  deque& operator=(deque&& rhs) {
    clear();

    _start = std::move(rhs._start);
    _finish = std::move(rhs._finish);
    _map = rhs._map;
    _map_size = rhs._map_size;

    rhs._map = 0;
    rhs._map_size = 0;

    return *this;
  }

  deque& operator=(std::initializer_list<T> il) {
    deque tmp(il.begin(), il.end());
    swap(tmp);
    return *this;
  }

  template <typename InputIterator>
  void assign(InputIterator first, InputIterator last) {
    __copy_assign(first, last, iterator_category(first));
  }

  void assign(size_type n, const_reference value) {
    if (n > size()) {
      std::fill(begin(), end(), value);
      insert(end(), n - size(), value);
    } else {
      erase(std::fill_n(begin(), n, value), end());
    }
  }

  void assign(std::initializer_list<value_type> il) {
    __copy_assign(il.begin(), il.end(), forward_iterator_tag());
  }

  allocator_type get_allocator() const noexcept {
    return allocator_type();
  }

 public:  // iterators
  iterator begin() noexcept {
    return _start;
  }

  const_iterator begin() const noexcept {
    return _start;
  }

  iterator end() noexcept {
    return _finish;
  }

  const_iterator end() const noexcept {
    return _finish;
  }

  const_iterator cbegin() const noexcept {
    return _start;
  }

  const_iterator cend() const noexcept {
    return _finish;
  }

 public:  // cpacity
  size_type size() const noexcept {
    return _finish - _start;
  }

  size_type max_size() const noexcept {
    return static_cast<size_type>(-1);
  }

  void resize(size_type n) {
    resize(n, value_type());
  }

  void resize(size_type n, const_reference value) {
    if (n < size()) {
      erase(begin() + n, end());
    } else {
      insert(end(), n - size(), value);
    }
  }

  void shrink_to_fit() {
    // 中控台留下
    for (map_pointer cur = _map; cur < _start.node; ++cur) {
      __deallocate_data(*cur);
      *cur = 0;
    }

    for (map_pointer cur = _finish.node + 1; cur < _map + _map_size; ++cur) {
      __deallocate_data(*cur);
      *cur = 0;
    }
  }

  bool empty() const noexcept {
    return _start == _finish;
  }

 public:  // element access
  reference operator[](size_type n) {
    return _start[n];
  }

  const_reference operator[](size_type n) const {
    return _start[n];
  }

  reference at(size_type n) {
    THROW_OUT_OF_RANGE_IF(n > size(), "deque<T>::at() subscript out of range.");
    return (*this)[n];
  }

  const_reference at(size_type n) const {
    THROW_OUT_OF_RANGE_IF(n > size(), "deque<T>::at() subscript out of range.");
    return (*this)[n];
  }

  reference front() {
    return *_start;
  }

  const_reference front() const {
    return *_start;
  }

  reference back() {
    return *(_finish - 1);
  }

  const_reference back() const {
    return *(_finish - 1);
  }

 public:  // modifiers
  template <typename... Args>
  void emplace_front(Args&&... args) {
    if (_start.cur != _start.first) {
      construct(_start.cur - 1, std::forward<Args>(args)...);
      --_start.cur;
    } else {
      __push_front_aux(std::forward<Args>(args)...);
    }
  }

  template <typename... Args>
  void emplace_back(Args&&... args) {
    // 始终记住 finish 中的 cur 是指向 one past the last element
    if (_finish.cur != _finish.last - 1) {
      construct(_finish.cur, std::forward<Args>(args)...);
      ++_finish.cur;
    } else {
      __push_back_aux(std::forward<Args>(args)...);
    }
  }

  template <typename... Args>
  iterator emplace(iterator pos, Args&&... args) {
    if (pos.cur == _start.cur) {
      emplace_front(std::forward<Args>(args)...);
      return _start;
    } else if (pos.cur == _finish.cur) {
      emplace_back(std::forward<Args>(args)...);
      return _finish;
    }
    return __insert_aux(pos, std::forward<Args>(args)...);
  }

  void push_front(const_reference value) {
    if (_start.cur != _start.first) {
      construct(_start.cur - 1, value);
      --_start.cur;
    } else {
      __push_front_aux(value);
    }
  }

  void push_front(value_type&& value) {
    emplace_front(std::move(value));
  }

  void push_back(const_reference value) {
    if (_finish.cur != _finish.last - 1) {
      construct(_finish.cur, value);
      ++_finish.cur;
    } else {
      __push_back_aux(value);
    }
  }

  void push_back(value_type&& value) {
    emplace_back(std::move(value));
  }

  iterator insert(iterator pos, const_reference value) {
    if (pos.cur == _start.cur) {
      push_front(value);
      return _start;
    } else if (pos.cur == _finish.cur - 1) {
      push_back(value);
      return _finish - difference_type(1);
    } else {
      return __insert_aux(pos, value);
    }
  }

  iterator insert(iterator pos, value_type&& value) {
    if (pos.cur == _start.cur) {
      push_front(std::move(value));
      return _start;
    } else if (pos.cur == _finish.cur) {
      push_back(std::move(value));
      return _finish - difference_type(1);
    } else {
      return __insert_aux(pos, std::move(value));
    }
  }

  // TODO(dong): not c++11 yet
  void insert(iterator pos, size_type n, const_reference value) {
    if (pos.cur == _start.cur) {
      iterator new_start = __reserve_elem_at_front(n);
      uninitialized_fill_n(new_start, n, value);
      _start = new_start;
    } else if (pos.cur == _finish.cur) {
      __reserve_elem_at_back(n);
      _finish = uninitialized_fill_n(_finish, n, value);
    } else {
      __insert_aux(pos, n, value);
    }
  }

  // TODO(dong): not c++11 yet
  template <typename InputIterator>
  void insert(iterator pos, InputIterator first, InputIterator last) {
    size_type n = distance(first, last);
    if (pos.cur == _start.cur) {
      iterator new_start = __reserve_elem_at_front(n);
      _finish = uninitialized_copy(first, last, new_start);
      _start = new_start;
    } else if (pos.cur == _finish.cur) {
      __reserve_elem_at_back(n);
      _finish = uninitialized_copy(first, last, _finish);
    } else {
      __insert_dispatch(pos, first, last, n, iterator_category(first));
    }
  }

  // TODO(dong): not c++11 yet
  void insert(iterator pos, std::initializer_list<value_type> il) {
    insert(pos, il.begin(), il.end());
  }

  void pop_front() {
    if (_start.cur != _start.last) {
      destroy(_start.cur);
      ++_start;
    } else {
      destroy(_start.cur);
      __deallocate_data(_start.first);
      _start.set_node(_start.node + 1);
      _start.cur = _start.first;
    }
  }

  void pop_back() {
    if (_finish.cur != _finish.first) {
      --_finish.cur;
      destroy(_finish.cur);
    } else {
      __deallocate_data(_finish.cur);
      _finish.set_node(_finish.node - 1);
      _finish.cur = _finish.last - 1;
      destroy(_finish.cur);
    }
  }

  iterator erase(iterator pos) {
    iterator next = pos;
    ++next;
    const size_type elem_before = pos - _start;
    if (elem_before < size() / 2) {
      std::copy_backward(_start, pos, next);
      pop_front();
    } else {
      std::copy(next, _finish, pos);
      pop_back();
    }
    return _start + elem_before;
  }

  iterator erase(iterator first, iterator last) {
    if (first == _start && last == _finish) {
      clear();
      return _finish;
    }

    const size_type len = last - first;
    const size_type elem_before = first - _start;
    if (elem_before < ((size() - len) / 2)) {
      std::copy_backward(_start, first, last);
      iterator new_start = _start + len;
      destroy(_start, new_start);
      _start = new_start;
    } else {
      std::copy(last, _finish, first);
      iterator new_finish = _finish - len;
      destroy(new_finish, _finish);
      _finish = new_finish;
    }
    return _start + elem_before;
  }

  void swap(deque& rhs) {
    if (this != &rhs) {
      std::swap(_start, rhs._start);
      std::swap(_finish, rhs._finish);
      std::swap(_map, rhs._map);
      std::swap(_map_size, rhs._map_size);
    }
  }

  void clear() {
    // 两边的先留着，待会儿再处理
    for (map_pointer cur = _start.node + 1; cur < _finish.node; ++cur) {
      destroy(*cur, *cur + _buffer_size());
    }

    if (_start.node != _finish.node) {
      // 两边都还在，把处理掉
      destroy(_start.cur, _start.last);
      destroy(_finish.first, _finish.cur);
    } else {
      // 只有首端了
      destroy(_start.cur, _finish.cur);
    }
    shrink_to_fit();
    _finish = _start;
  }
};

template <typename T, typename Alloc>
bool operator==(const deque<T, Alloc>& lhs, const deque<T, Alloc>& rhs) {
  return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename T, typename Alloc>
bool operator!=(const deque<T, Alloc>& lhs, const deque<T, Alloc>& rhs) {
  return !(lhs == rhs);
}

template <typename T, typename Alloc>
bool operator<(const deque<T, Alloc>& lhs, const deque<T, Alloc>& rhs) {
  return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename T, typename Alloc>
bool operator>(const deque<T, Alloc>& lhs, const deque<T, Alloc>& rhs) {
  return rhs < lhs;
}

template <typename T, typename Alloc>
bool operator>=(const deque<T, Alloc>& lhs, const deque<T, Alloc>& rhs) {
  return !(lhs < rhs);
}

template <typename T, typename Alloc>
bool operator<=(const deque<T, Alloc>& lhs, const deque<T, Alloc>& rhs) {
  return !(lhs > rhs);
}

template <typename T, typename Alloc>
void swap(deque<T, Alloc> lhs, deque<T, Alloc> rhs) {
  lhs.swap(rhs);
}

}  // namespace gd

#endif  // !__MY_DEQUE_H