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
  typedef T value_type;
  typedef value_type *pointer;
  typedef const value_type *const_pointer;
  typedef value_type *iterator;
  typedef const value_type *const_iterator;
  typedef value_type &reference;
  typedef const value_type &const_reference;
  typedef size_t size_type;
  typedef ptrdiff_t difference_type;

 protected:
  typedef simple_alloc<value_type, Alloc> allocator;

  iterator start;           // 空间起点
  iterator finish;          // one past the 已使用空间
  iterator end_of_storage;  // one past the 已分配空间

 private:  // helper functions
  void __alloc(size_type n) {
    try {
      // allocate 可能会抛出 bad_alloc
      start = allocator::allocate(n);
      end_of_storage = start + n;
    } catch (...) {
      start = 0;
      end_of_storage = 0;
      throw;
    }
  }

  void __dealloc(pointer p) {
    if (p)
      allocator::deallocate(p);
  }

  void __dealloc(pointer p, size_type n) {
    if (p)
      allocator::deallocate(p, n);
  }

  iterator __alloc_and_fill(size_type n, const value_type &value) {
    __alloc(n);
    finish = uninitialized_fill_n(start, n, value);
    return finish;
  }

  template <typename InputIterator>
  iterator __range_alloc_and_fill(InputIterator first, InputIterator last) {
    finish = __range_alloc_and_fill_dispatch(first, last, iterator_category(first));
  }

  template <typename ForwardIterator>
  iterator __range_alloc_and_fill_dispatch(ForwardIterator first, ForwardIterator last, forward_iterator_tag) {
    typedef typename iterator_traits<ForwardIterator>::difference_type difference_type;
    difference_type n = distance(first, last);
    __alloc(n);
    return uninitialized_copy(first, last, start);
  }

  template <typename RandomAccessIterator>
  iterator __range_alloc_and_fill_dispatch(RandomAccessIterator first, RandomAccessIterator last,
                                           random_access_iterator_tag) {
    typedef typename iterator_traits<RandomAccessIterator>::difference_type difference_type;
    difference_type n = last - first;
    __alloc(n);
    return uninitialized_copy(first, last, start);
  }

 public:  // contructors, copy, and deconstructor
  vector() noexcept : start(0), finish(0), end_of_storage(0) {}

  explicit vector(size_type n) {
    __alloc(n);
  }

  vector(size_type n, const value_type &value) {
    __alloc_and_fill(n, value);
  }

  template <typename InputIterator>
  vector(InputIterator first, InputIterator last) {
    __range_alloc_and_fill(first, last);
  }

  vector(std::initializer_list<value_type> il) {
    __range_alloc_and_fill(il.begin(), il.end());
  }

  vector(const vector &rhs) {
    __range_alloc_and_fill(rhs.begin(), rhs.end());
  }

  vector(const vector &&rhs) noexcept : start(rhs.start), finish(rhs.finish), end_of_storage(rhs.end_of_storage) {
    rhs.start = 0;
    rhs.finish = 0;
    rhs.end_of_storage = 0;
  }

  // void assign(InputIterator first, InputIterator last);
  // void assign(size_type n, const T& t);

  ~vector() {
    destroy(start, finish);
    __dealloc(start);
  }

 public:  // iterators related
  iterator begin() noexcept {
    return start;
  }

  const_iterator begin() const noexcept {
    return start;
  }

  iterator end() noexcept {
    return finish;
  }

  const_iterator end() const noexcept {
    return finish;
  }

  const_iterator cbegin() const noexcept {
    return start;
  }

  const_iterator cend() const noexcept {
    return finish;
  }

  // reverse iterators

 public:  // capacity related
  size_type size() const noexcept {
    return static_cast<size_type>(finish - start);
  }

  size_type max_size() const noexcept {
    return static_cast<size_type>(-1) / sizeof(value_type);
  }

  size_type capacity() const noexcept {
    return static_cast<size_type>(end_of_storage - start);
  }

  bool empty() const noexcept {
    return start == finish;
  }

  void reserve(size_type n) {
    if (capacity() < n) {
      iterator old_start = start;
      iterator old_finish = finish;
      size_type old_size = size();
      __alloc(n);
      finish = std::copy(old_start, old_finish, start);
      destroy(old_start, old_finish);
      __dealloc(old_start, old_size);
    }
  }

  void swap(vector &rhs) {
    if (this != &rhs) {
      std::swap(start, rhs.start);
      std::swap(finish, rhs.finish);
      std::swap(end_of_storage, rhs.end_of_storage);
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
    return start;
  }

  const_pointer data() const noexcept {
    return start;
  }

 public:  // modifiers
  // iterator insert(iterator pos, const value_type &&value);

  iterator insert(iterator pos, const_reference value) {
    size_type n = pos - start;
    if (finish != end_of_storage && pos == finish) {
      construct(finish, value);
      ++finish;
    } else {
      __insert_aux(pos, value);
    }
    return start + n;
  }

  iterator insert(iterator pos, size_type n, const_reference value) {
    if (n == 0) {
      return pos;
    }

    size_type ret_offset = pos - start;

    if (size() + n <= capacity()) {
      size_type num_after = pos - start;
      iterator old_finish = finish;
      if (num_after > n) {
        finish = uninitialized_copy(finish - n, finish, finish);
        std::copy_backward(pos, old_finish - n, old_finish);
        std::fill(pos, pos + n, value);
      } else {
        finish = uninitialized_fill_n(finish, n - num_after, value);
        finish = std::copy(pos, old_finish, finish);
        std::fill(pos, old_finish, value);
      }
    } else {
      iterator old_start = start;
      iterator old_finish = finish;
      size_type old_size = size();
      size_type new_size = old_size + std::max(old_size, n);
      __alloc(new_size);
      try {
        finish = uninitialized_copy(old_start, pos, start);
        finish = uninitialized_fill_n(finish, n, value);
        finish = uninitialized_copy(pos, old_finish, finish);
      } catch (...) {
        destroy(start, finish);
        __dealloc(start, new_size);
        throw;
      }
      destroy(old_start, old_finish);
      __dealloc(old_start, old_size);
    }

    // 返回插入的起点位置
    return start + ret_offset;
  }

  template <typename... Args>
  void emplace_back(Args &&... args) {
    if (finish < end_of_storage) {
      construct(finish, std::forward<Args>(args)...);
      ++finish;
    } else {
      emplace_aux(finish, std::forward<Args>(args)...);
    }
  }

  template <typename... Args>
  iterator emplace(const_iterator pos, Args &&... args) {
    size_type n = pos - start;
    if (finish != end_of_storage && pos == finish) {
      construct(finish, std::forward<Args>(args)...);
      ++finish;
    } else {
      __emplace_aux(pos, std::forward<Args>(args)...);
    }
    return start + n;
  }

  void push_back(const_reference value) {
    if (finish != end_of_storage) {
      construct(finish, value);
      ++finish;
    } else {
      __insert_aux(finish, value);
    }
  }

  // void push_back(T&& x);

  iterator erase(const_iterator pos) {
    if (pos + 1 != finish)
      std::copy(pos + 1, finish, pos);
    --finish;
    destroy(finish);
    return pos;
  }

  iterator erase(iterator first, iterator last) {
    if (first != last) {
      iterator i = std::copy(last, finish, first);
      destroy(i, finish);
      finish = finish - (last - first);
      return first;
    }
  }

  void clear() {
    erase(begin(), end());
  }

 private:
  template <typename... Args>
  void __emplace_aux(iterator pos, Args &&... args) {
    if (finish != end_of_storage) {
      construct(finish, *(finish - 1));
      ++finish;
      std::copy_backward(pos, finish - 2, finish - 1);
      *pos = value_type(std::forward<Args>(args)...);
    } else {
      iterator old_start = start;
      iterator old_finish = finish;
      size_type old_size = size();
      size_type new_size = old_size + std::max(old_size, size_type(1));
      __alloc(new_size);
      try {
        finish = uninitialized_copy(old_start, pos, start);
        construct(finish, std::forward<Args>(args)...);
        ++finish;
        finish = uninitialized_copy(pos, old_finish, finish);
      } catch (...) {
        destroy(start, finish);
        allocator::deallocate(start, new_size);
        throw;
      }
      destroy(old_start, old_finish);
      __dealloc(old_start, old_size);
    }
  }

  void __insert_aux(iterator pos, const value_type &value) {
    if (finish != end_of_storage) {
      construct(finish, *(finish - 1));
      ++finish;
      std::copy_backward(pos, finish - 2, finish - 1);
      *pos = value;
    } else {
      iterator old_start = start;
      iterator old_finish = finish;
      size_type old_size = end_of_storage - start;

      const size_type new_size = old_size != 0 ? 2 * old_size : 1;
      __alloc(new_size);
      try {
        finish = uninitialized_copy(old_start, pos, start);
        construct(finish, value);
        ++finish;
        finish = uninitialized_copy(pos, old_finish, finish);
      } catch (...) {
        destroy(start, finish);
        allocator::deallocate(start, new_size);
        throw;
      }
      // 析构并释放原来的元素, 如果是初始状态，old_start 为空，则不可进行 deallocate
      destroy(old_start, old_finish);
      __dealloc(old_start, old_size);
    }
  }

 public:  // element related
  reference operator[](size_type n) {
    return *(start + n);
  }

  const_reference operator[](size_type n) const {
    return *(start + n);
  }

  reference at(size_type n) {
    THROW_OUT_OF_RANGE_IF(!(n < size()), "vector<T>::at() subscript out of range");
    return (*this)[n];
  }

  reference front() {
    return *start;
  }

  const_reference front() const {
    return *start;
  }

  reference back() {
    return *(finish - 1);
  }

  const_reference back() const {
    return *(finish - 1);
  }
};

}  // namespace gd

#endif  // !__MY_VECTOR_H