#ifndef __MY_UNINITIALIZED_H
#define __MY_UNINITIALIZED_H

#include <algorithm>
#include <cstring>
#include "my_construct.h"
#include "my_iterator.h"
#include "type_traits.h"

namespace gd {

template <typename InputIterator, typename ForwardIterator>
inline ForwardIterator __uninitialized_copy_dispatch(InputIterator first, InputIterator last, ForwardIterator dest,
                                                     __true_type) {
  return std::copy(first, last, dest);
}

template <typename InputIterator, typename ForwardIterator>
inline ForwardIterator __uninitialized_copy_dispatch(InputIterator first, InputIterator last, ForwardIterator dest,
                                                     __false_type) {
  ForwardIterator cur = dest;
  for (; first != last; ++first, ++cur)
    construct(cur, *first);
  return cur;
}

template <typename InputIterator, typename ForwardIterator, typename T>
inline ForwardIterator __uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator dest, T*) {
  typedef typename __type_traits<T>::is_POD_type is_POD;
  return __uninitialized_copy_dispatch(first, last, dest, is_POD());
}

template <typename InputIterator, typename ForwardIterator>
inline ForwardIterator uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator dest) {
  return __uninitialized_copy(first, last, dest, value_type(dest));
}

template <>
inline char* uninitialized_copy(const char* first, const char* last, char* dest) {
  memmove(dest, first, last - first);
  return dest + (last - first);
}

template <>
inline wchar_t* uninitialized_copy(const wchar_t* first, const wchar_t* last, wchar_t* dest) {
  memmove(dest, first, sizeof(wchar_t) * (last - first));
  return dest + (last - first);
}

template <typename ForwardIterator, typename T>
inline void __uninitialized_fill_dispatch(ForwardIterator first, ForwardIterator last, const T& value, __true_type) {
  std::fill(first, last, value);
}

template <typename ForwardIterator, typename T>
inline void __uninitialized_fill_dispatch(ForwardIterator first, ForwardIterator last, const T& value, __false_type) {
  for (; first != last; ++first)
    construct(first, value);
}

template <typename ForwardIterator, typename T>
inline void __uninitialized_fill(ForwardIterator first, ForwardIterator last, const T& value, T*) {
  typedef typename __type_traits<T>::is_POD_type is_POD;
  __uninitialized_fill_dispatch(first, last, value, is_POD());
}

template <typename ForwardIterator, typename T>
inline void uninitialized_fill(ForwardIterator first, ForwardIterator last, const T& value) {
  __uninitialized_fill(first, last, value, value_type(first));
}

template <typename ForwardIterator, typename Size, typename T>
inline ForwardIterator __uninitialized_fill_n_dispatch(ForwardIterator i, Size n, const T& value, __true_type) {
  return std::fill_n(i, n, value);
}

template <typename ForwardIterator, typename Size, typename T>
inline ForwardIterator __uninitialized_fill_n_dispatch(ForwardIterator i, Size n, const T& value, __false_type) {
  ForwardIterator cur = i;
  for (; n > 0; --n, ++cur) {
    construct(cur, value);
  }
  return cur;
}

template <typename ForwardIterator, typename Size, typename T>
inline ForwardIterator __uninitialized_fill_n(ForwardIterator i, Size n, const T& value, T*) {
  typedef typename __type_traits<T>::is_POD_type is_POD;
  return __uninitialized_fill_n_dispatch(i, n, value, is_POD());
}

template <typename ForwardIterator, typename Size, typename T>
inline ForwardIterator uninitialized_fill_n(ForwardIterator i, Size n, const T& value) {
  return __uninitialized_fill_n(i, n, value, value_type(i));
}

}  // namespace gd

#endif  // !__MY_UNINITIALIZED_H
