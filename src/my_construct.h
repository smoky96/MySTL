#ifndef __MY_CONSTRUCT_H
#define __MY_CONSTRUCT_H

#include <new>            // placement new
#include <utility>        // for std::move std::forward
#include "my_iterator.h"  // for iterator_traits;
#include "type_traits.h"

namespace gd {

template <typename T1, typename T2>
inline void construct(T1* p, const T2& value) {
  new (p) T1(value);  // call constructor T1::T1(value)
}

template <typename T1, typename T2>
inline void construct(T1* p, T2&& value) {
  new (p) T1(std::forward<T2>(value));
}

template <typename T, typename... Args>
inline void construct(T* p, Args&&... args) {
  new (p) T(std::forward<Args>(args)...);
}

template <typename T>
inline void destroy(T* p) {
  p->~T();  // call deconstructor ~T()
}

// second version of destroy that takes two forward iterator
template <typename ForwardIterator>
inline void __destroy_dispatch(ForwardIterator first, ForwardIterator last, __false_type) {
  for (; first != last; ++first)
    destroy(first);
}

// if it doesn't matter, do nothing
template <typename ForwardIterator>
inline void __destroy_dispatch(ForwardIterator first, ForwardIterator last, __true_type) {}

template <typename ForwardIterator, typename T>
inline void __destroy(ForwardIterator first, ForwardIterator last, T*) {
  typedef typename __type_traits<T>::has_trivial_destructor trivial_destructor;
  __destroy_dispatch(first, last, trivial_destructor());
}

// second version of destroy that takes two forward iterator
template <typename ForwardIterator>
inline void destroy(ForwardIterator first, ForwardIterator last) {
  // call the middle function '__destroy' to determine
  // which destroy method should be used
  __destroy(first, last, value_type(first));
}

template <>
inline void destroy(char*, char*) {}

template <>
inline void destroy(wchar_t*, wchar_t*) {}

}  // namespace gd

#endif  // !__MY_CONSTRUCT_H