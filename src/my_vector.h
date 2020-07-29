#ifndef __MY_VECTOR_H
#define __MY_VECTOR_H

#include <algorithm>
#include <initializer_list>
#include "exceptdef.h"
#include "my_alloc.h"
#include "my_uninitialized.h"

namespace gd {

template <typename T, typename Alloc = alloc>
class vector {
 public:
  typedef T                 value_type;
  typedef value_type *      pointer;
  typedef const value_type *const_pointer;
  typedef value_type *      iterator;
  typedef const value_type *const_iterator;
  typedef value_type &      reference;
  typedef const value_type &const_reference;
  typedef size_t            size_type;
  typedef ptrdiff_t         difference_type;

 protected:
  typedef simple_alloc<value_type, Alloc> allocator_type;
  typedef simple_alloc<value_type, Alloc> data_allocator;

  iterator _start;           // 空间起点
  iterator _finish;          // one past the 已使用空间
  iterator _end_of_storage;  // one past the 已分配空间

 private:  // helper functions
  void __alloc(size_type n) {
    try {
      // allocate 可能会抛出 bad_alloc
      _start = data_allocator::allocate(n);
      _end_of_storage = _start + n;
    } catch (...) {
      _start = 0;
      _end_of_storage = 0;
      throw;
    }
  }

  void __dealloc(pointer p) {
    if (p)
      data_allocator::deallocate(p);
  }

  void __dealloc(pointer p, size_type n) {
    if (p)
      data_allocator::deallocate(p, n);
  }

  iterator __alloc_and_fill(size_type n, const value_type &value) {
    __alloc(n);
    return uninitialized_fill_n(_start, n, value);
  }

  template <typename InputIterator>
  iterator __range_alloc_and_fill(InputIterator first, InputIterator last) {
    size_type n = distance(first, last);
    __alloc(n);
    return uninitialized_copy(first, last, _start);
  }

  template <typename InputIterator>
  void __copy_assign(InputIterator first, InputIterator last, input_iterator_tag) {
    iterator cur = begin();
    for (; first != last && cur != _finish; ++first, ++cur) {
      *cur = *first;
    }
    if (first == last) {
      erase(cur, _finish);
    } else {
      insert(cur, first, last);
    }
  }

  template <typename ForwardIterator>
  void __copy_assign(ForwardIterator first, ForwardIterator last, forward_iterator_tag) {
    size_type n = distance(first, last);
    if (n > capacity()) {
      vector tmp(first, last);
      swap(tmp);
    } else {
      if (n > size()) {
        ForwardIterator mid = first;
        advance(mid, size());
        std::copy(first, mid, begin());
        _finish = uninitialized_copy(mid, last, end());
      } else {
        auto it = std::copy(first, last, begin());
        erase(it, _finish);
      }
    }
  }

  template <typename InputIterator>
  void __copy_insert(iterator pos, InputIterator first, InputIterator last, input_iterator_tag) {
    iterator cur = pos;
    for (; first != last; ++first, ++cur) {
      cur = insert(pos, *first);
    }
  }

  template <typename ForwardIterator>
  void __copy_insert(iterator pos, ForwardIterator first, ForwardIterator last, forward_iterator_tag) {
    if (first != last) {
      size_type n = distance(first, last);
      if (static_cast<size_type>(_end_of_storage - _finish) >= n) {
        const size_type elem_after = _finish - pos;
        iterator        old_finish = _finish;
        if (elem_after > n) {
          _finish = uninitialized_copy(_finish - n, _finish, _finish);
          std::copy_backward(pos, old_finish - n, old_finish);
          std::copy(first, last, pos);
        } else {
          ForwardIterator mid = first;
          advance(mid, elem_after);
          _finish = uninitialized_copy(mid, last, _finish);
          _finish = uninitialized_copy(pos, old_finish, _finish);
          std::copy(first, mid, pos);
        }
      } else {
        iterator  old_start = _start;
        iterator  old_finish = _finish;
        size_type old_size = size();
        size_type new_size = old_size + std::max(old_size, n);
        __alloc(new_size);
        try {
          _finish = uninitialized_copy(old_start, pos, _start);
          _finish = uninitialized_copy(first, last, _finish);
          _finish = uninitialized_copy(pos, old_finish, _finish);
        } catch (...) {
          destroy(_start, _finish);
          __dealloc(_start, new_size);
          throw;
        }
        destroy(old_start, old_finish);
        __dealloc(old_start, old_size);
      }
    }
  }

 public:  // contructors, copy, and deconstructor
  vector() noexcept : _start(0), _finish(0), _end_of_storage(0) {}

  explicit vector(size_type n) {
    _finish = __alloc_and_fill(n, value_type());
  }

  vector(size_type n, const value_type &value) {
    _finish = __alloc_and_fill(n, value);
  }

  template <typename InputIterator, typename std::enable_if<std::is_pointer<InputIterator>::value, int>::type = 0>
  vector(InputIterator first, InputIterator last) {
    _finish = __range_alloc_and_fill(first, last);
  }

  vector(std::initializer_list<value_type> il) {
    _finish = __range_alloc_and_fill(il.begin(), il.end());
  }

  vector(const vector &rhs) {
    _finish = __range_alloc_and_fill(rhs.begin(), rhs.end());
  }

  vector(vector &&rhs) noexcept : _start(rhs._start), _finish(rhs._finish), _end_of_storage(rhs._end_of_storage) {
    rhs._start = 0;
    rhs._finish = 0;
    rhs._end_of_storage = 0;
  }

  vector &operator=(const vector &rhs) {
    auto sz = rhs.size();
    if (&rhs != this) {
      if (capacity() < sz) {
        vector tmp(rhs.begin(), rhs.end());
        swap(tmp);
      } else if (sz <= size()) {
        iterator i = std::copy(rhs.begin(), rhs.end(), begin());
        destroy(i, _end_of_storage);
        _finish = _start + sz;
      } else {
        std::copy(rhs.begin(), rhs.begin() + size(), begin());
        _finish = uninitialized_copy(rhs.begin() + size(), rhs.end(), end());
      }
    }
    return *this;
  }

  vector &operator=(vector &&rhs) {
    destroy(_start, _finish);
    __dealloc(_start, _end_of_storage - _start);
    _start = rhs._start;
    _finish = rhs._finish;
    _end_of_storage = rhs._end_of_storage;

    rhs._start = 0;
    rhs._finish = 0;
    rhs._end_of_storage = 0;

    return *this;
  }

  template <typename InputIterator, typename std::enable_if<std::is_pointer<InputIterator>::value, int>::type = 0>
  void assign(InputIterator first, InputIterator last) {
    __copy_assign(first, last, iterator_category(first));
  }

  void assign(size_type n, const T &value) {
    if (n > capacity()) {
      vector tmp(n, value);
      swap(tmp);
    } else if (n <= size()) {
      erase(std::fill_n(begin(), n, value), end());
    } else {
      std::fill(begin(), end(), value);
      _finish = uninitialized_fill_n(end(), n - size(), value);
    }
  }

  void assign(std::initializer_list<value_type> il) {
    __copy_assign(il.begin(), il.end(), forward_iterator_tag());
  }

  ~vector() {
    destroy(_start, _finish);
    __dealloc(_start, _end_of_storage - _start);
  }

  allocator_type get_allocator() {
    return data_allocator();
  }

 public:  // iterators related
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

  // reverse iterators

 public:  // capacity related
  size_type size() const noexcept {
    return static_cast<size_type>(_finish - _start);
  }

  size_type max_size() const noexcept {
    return static_cast<size_type>(-1) / sizeof(value_type);
  }

  size_type capacity() const noexcept {
    return static_cast<size_type>(_end_of_storage - _start);
  }

  bool empty() const noexcept {
    return _start == _finish;
  }

  void reserve(size_type n) {
    if (capacity() < n) {
      iterator  old_start = _start;
      iterator  old_finish = _finish;
      size_type old_size = size();
      __alloc(n);
      _finish = std::copy(old_start, old_finish, _start);
      destroy(old_start, old_finish);
      __dealloc(old_start, old_size);
    }
  }

  void swap(vector &rhs) {
    if (this != &rhs) {
      std::swap(_start, rhs._start);
      std::swap(_finish, rhs._finish);
      std::swap(_end_of_storage, rhs._end_of_storage);
    }
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

  // void shrink_to_fit();

 public:  // data
  pointer data() noexcept {
    return _start;
  }

  const_pointer data() const noexcept {
    return _start;
  }

 public:  // modifiers
  // iterator insert(iterator pos, value_type &&value) {
  //   size_type n = pos - _start;
  //   if (_finish != _end_of_storage && pos == _finish) {
  //     construct(_finish, std::move(value));
  //     ++_finish;
  //   } else {
  //     __insert_aux(pos, std::move(value));
  //   }
  // }

  iterator insert(iterator pos, const_reference value) {
    size_type n = pos - _start;
    if (_finish != _end_of_storage && pos == _finish) {
      construct(_finish, value);
      ++_finish;
    } else {
      __insert_aux(pos, value);
    }
    return _start + n;
  }

  iterator insert(iterator pos, size_type n, const_reference value) {
    if (n == 0) {
      return pos;
    }

    size_type ret_offset = pos - _start;

    if (size() + n <= capacity()) {
      size_type num_after = pos - _start;
      iterator  old_finish = _finish;
      if (num_after > n) {
        _finish = uninitialized_copy(_finish - n, _finish, _finish);
        std::copy_backward(pos, old_finish - n, old_finish);
        std::fill(pos, pos + n, value);
      } else {
        _finish = uninitialized_fill_n(_finish, n - num_after, value);
        _finish = std::copy(pos, old_finish, _finish);
        std::fill(pos, old_finish, value);
      }
    } else {
      iterator  old_start = _start;
      iterator  old_finish = _finish;
      size_type old_size = size();
      size_type new_size = old_size + std::max(old_size, n);
      __alloc(new_size);
      try {
        _finish = uninitialized_copy(old_start, pos, _start);
        _finish = uninitialized_fill_n(_finish, n, value);
        _finish = uninitialized_copy(pos, old_finish, _finish);
      } catch (...) {
        destroy(_start, _finish);
        __dealloc(_start, new_size);
        throw;
      }
      destroy(old_start, old_finish);
      __dealloc(old_start, old_size);
    }

    // 返回插入的起点位置
    return _start + ret_offset;
  }

  template <typename InputIterator>
  iterator insert(const_iterator pos, InputIterator first, InputIterator last) {
    size_type n = pos - _start;
    __copy_insert(const_cast<iterator>(pos), first, last, iterator_category(first));
    return _start + n;
  }

  iterator insert(const_iterator pos, std::initializer_list<value_type> il) {
    size_type n = pos - _start;
    __copy_insert(const_cast<iterator>(pos), il.begin(), il.end(), forward_iterator_tag());
    return _start + n;
  }

  template <typename... Args>
  void emplace_back(Args &&... args) {
    if (_finish < _end_of_storage) {
      construct(_finish, std::forward<Args>(args)...);
      ++_finish;
    } else {
      __emplace_aux(_finish, std::forward<Args>(args)...);
    }
  }

  template <typename... Args>
  iterator emplace(const_iterator pos, Args &&... args) {
    size_type n = pos - _start;
    // 需要一个非 const 的 iterator，感觉用 const_cast 应该也可以吧
    iterator pos_copy = _start + (pos - begin());
    if (_finish != _end_of_storage && pos == _finish) {
      construct(_finish, std::forward<Args>(args)...);
      ++_finish;
    } else {
      __emplace_aux(pos_copy, std::forward<Args>(args)...);
    }
    return _start + n;
  }

  void push_back(const_reference value) {
    if (_finish != _end_of_storage) {
      construct(_finish, value);
      ++_finish;
    } else {
      __insert_aux(_finish, value);
    }
  }

  void push_back(value_type &&value) {
    emplace_back(std::move(value));
  }

  void pop_back() {
    erase(_finish - 1);
  }

  iterator erase(const_iterator pos) {
    iterator pos_copy = _start + (pos - begin());
    if (pos + 1 != _finish)
      std::copy(pos_copy + 1, _finish, pos_copy);
    --_finish;
    destroy(_finish);
    return pos_copy;
  }

  iterator erase(iterator first, iterator last) {
    size_type n = first - _start;
    iterator  i = std::copy(last, _finish, first);
    destroy(i, _finish);
    _finish = _finish - (last - first);
    return _start + n;
  }

  void clear() {
    erase(begin(), end());
  }

 private:
  template <typename... Args>
  void __emplace_aux(iterator pos, Args &&... args) {
    if (_finish != _end_of_storage) {
      construct(_finish, *(_finish - 1));
      ++_finish;
      std::copy_backward(pos, _finish - 2, _finish - 1);
      *pos = value_type(std::forward<Args>(args)...);
    } else {
      iterator  old_start = _start;
      iterator  old_finish = _finish;
      size_type old_size = size();
      size_type new_size = old_size + std::max(old_size, size_type(1));
      __alloc(new_size);
      try {
        _finish = uninitialized_copy(old_start, pos, _start);
        construct(_finish, std::forward<Args>(args)...);
        ++_finish;
        _finish = uninitialized_copy(pos, old_finish, _finish);
      } catch (...) {
        destroy(_start, _finish);
        data_allocator::deallocate(_start, new_size);
        throw;
      }
      destroy(old_start, old_finish);
      __dealloc(old_start, old_size);
    }
  }

  void __insert_aux(iterator pos, const value_type &value) {
    if (_finish != _end_of_storage) {
      construct(_finish, *(_finish - 1));
      ++_finish;
      std::copy_backward(pos, _finish - 2, _finish - 1);
      *pos = value;
    } else {
      iterator  old_start = _start;
      iterator  old_finish = _finish;
      size_type old_size = _end_of_storage - _start;

      const size_type new_size = old_size != 0 ? 2 * old_size : 1;
      __alloc(new_size);
      try {
        _finish = uninitialized_copy(old_start, pos, _start);
        construct(_finish, value);
        ++_finish;
        _finish = uninitialized_copy(pos, old_finish, _finish);
      } catch (...) {
        destroy(_start, _finish);
        data_allocator::deallocate(_start, new_size);
        throw;
      }
      // 析构并释放原来的元素, 如果是初始状态，old_start 为空，则不可进行 deallocate
      destroy(old_start, old_finish);
      __dealloc(old_start, old_size);
    }
  }

 public:  // element related
  reference operator[](size_type n) {
    return *(_start + n);
  }

  const_reference operator[](size_type n) const {
    return *(_start + n);
  }

  reference at(size_type n) {
    THROW_OUT_OF_RANGE_IF(!(n < size()), "vector<T>::at() subscript out of range");
    return (*this)[n];
  }

  const_reference at(size_type n) const {
    THROW_OUT_OF_RANGE_IF(!(n < size()), "vector<T>::at() subscript out of range");
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
};

template <typename T, typename Alloc>
bool operator==(const vector<T, Alloc> &lhs, const vector<T, Alloc> &rhs) {
  return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename T, typename Alloc>
bool operator<(const vector<T, Alloc> &lhs, const vector<T, Alloc> &rhs) {
  return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename T, typename Alloc>
bool operator!=(const vector<T, Alloc> &lhs, const vector<T, Alloc> &rhs) {
  return !(lhs == rhs);
}

template <typename T, typename Alloc>
bool operator>(const vector<T, Alloc> &lhs, const vector<T, Alloc> &rhs) {
  return rhs < lhs;
}

template <typename T, typename Alloc>
bool operator<=(const vector<T, Alloc> &lhs, const vector<T, Alloc> &rhs) {
  return !(lhs > rhs);
}

template <typename T, typename Alloc>
bool operator>=(const vector<T, Alloc> &lhs, const vector<T, Alloc> &rhs) {
  return !(lhs < rhs);
}

// 偏特化
template <typename T, typename Alloc>
void swap(vector<T, Alloc> &lhs, vector<T, Alloc> &rhs) {
  lhs.swap(rhs);
}

}  // namespace gd

#endif  // !__MY_VECTOR_H