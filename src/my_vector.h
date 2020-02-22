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

 public:  // contructors, copy, and deconstructor
  vector() noexcept : _start(0), _finish(0), _end_of_storage(0) {}

  explicit vector(size_type n) {
    __alloc(n);
    _finish = _start;
  }

  vector(size_type n, const value_type &value) {
    _finish = __alloc_and_fill(n, value);
  }

  template <typename InputIterator>
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

  // void assign(InputIterator first, InputIterator last);
  // void assign(size_type n, const T& t);

  ~vector() {
    destroy(_start, _finish);
    __dealloc(_start);
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
  // iterator insert(iterator pos, const value_type &&value);

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

  template <typename... Args>
  void emplace_back(Args &&... args) {
    if (_finish < _end_of_storage) {
      construct(_finish, std::forward<Args>(args)...);
      ++_finish;
    } else {
      emplace_aux(_finish, std::forward<Args>(args)...);
    }
  }

  template <typename... Args>
  iterator emplace(const_iterator pos, Args &&... args) {
    size_type n = pos - _start;
    if (_finish != _end_of_storage && pos == _finish) {
      construct(_finish, std::forward<Args>(args)...);
      ++_finish;
    } else {
      __emplace_aux(pos, std::forward<Args>(args)...);
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

  // void push_back(T&& x);

  iterator erase(const_iterator pos) {
    if (pos + 1 != _finish)
      std::copy(pos + 1, _finish, pos);
    --_finish;
    destroy(_finish);
    return pos;
  }

  iterator erase(iterator first, iterator last) {
    if (first != last) {
      iterator i = std::copy(last, _finish, first);
      destroy(i, _finish);
      _finish = _finish - (last - first);
      return first;
    }
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

}  // namespace gd

#endif  // !__MY_VECTOR_H