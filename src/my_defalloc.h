#ifndef __MY_DEFALLOC_H
#define __MY_DEFALLOC_H

#include <climits>
#include <cstddef>   // for ptrdiff_t, size_t
#include <cstdlib>   // for exit()
#include <iostream>  // for cerr
#include <new>       // for placement new

namespace gd {

using std::cerr;

template <typename T>
inline T* _allocate(ptrdiff_t size, T*) {
  std::set_new_handler(0);
  // operator new allocate 'size' bytes of storage, returns a void* non-null
  // pointer to the first bytes of this block
  T* p = static_cast<T*>(::operator new(static_cast<size_t>(size * sizeof(T))));
  if (p == 0) {
    cerr << "out of memory" << std::endl;
    exit(1);
  }
  return p;
}

template <typename T>
inline void _deallocate(T* buffer) {
  ::operator delete(buffer);
}

template <typename T1, typename T2>
inline void _construct(T1* p, const T2& value) {
  new (p) T2(value);
}

template <typename T>
inline void _destroy(T* p) {
  p->~T();
}

template <typename T>
class allocator {
 public:
  typedef T         value_type;
  typedef T*        pointer;
  typedef const T*  const_pointer;
  typedef T&        reference;
  typedef const T&  const_reference;
  typedef size_t    size_type;
  typedef ptrdiff_t differene_type;

  // rebind allocator of type U
  template <typename U>
  struct rebind {
    typedef allocator<U> other;
  };

  pointer allocate(size_type n, const void* hint = 0) {
    return _allocate(n, static_cast<pointer>(0));
  }

  void deallocate(pointer p, size_type n) {
    _deallocate(p);
  }

  void construct(pointer p, const_reference value) {
    _construct(p, value);
  }

  void destroy(pointer p) {
    _destroy(p);
  }

  pointer address(reference x) {
    return static_cast<pointer>(&x);
  }

  const_pointer const_address(const_reference x) {
    return static_cast<const_pointer>(&x);
  }

  size_type max_size() const {
    return static_cast<size_type>(UINT_MAX / sizeof(T));
  }
};

template <>
class allocator<void> {
 public:
  typedef void* pointer;
};

}  // namespace gd

#endif  // !__MY_DEFALLOC_H