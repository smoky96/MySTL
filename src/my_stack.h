#ifndef __MY_STACK_H
#define __MY_STACK_H

#include "my_deque.h"

namespace gd {

template <typename T, typename Container = gd::deque<T>>
class stack {
 public:
  typedef typename Container::value_type      value_type;
  typedef typename Container::reference       reference;
  typedef typename Container::const_reference const_reference;
  typedef typename Container::size_type       size_type;
  typedef Container                           container_type;

 protected:
  Container _c;

 public:
  // constructors, copy and destructor
  stack() = default;

  explicit stack(size_type n) : _c(n) {}

  stack(size_type n, const value_type& value) : _c(n, value) {}

  stack(const Container& c) : _c(c) {}

  stack(Container&& c) : _c(std::move(c)) {}

  stack(const stack& rhs) : _c(rhs._c) {}

  stack(stack&& rhs) : _c(std::move(rhs._c)) {}

  stack& operator=(const stack& rhs) {
    _c = rhs._c;
    return *this;
  }

  stack& operator=(stack&& rhs) {
    _c = std::move(rhs._c);
    return *this;
  }

  ~stack() = default;

  // element access
  reference top() {
    return _c.back();
  }

  const_reference top() const {
    return _c.back();
  }

  // capacity
  bool empty() const noexcept {
    return _c.empty();
  }

  size_type size() const noexcept {
    return _c.size();
  }

  // modify
  template <typename... Args>
  void emplace(Args&&... args) {
    _c.emplace_back(std::forward<Args>(args)...);
  }

  void push(const value_type& value) {
    _c.push_back(value);
  }

  void push(value_type&& value) {
    _c.push_back(std::move(value));
  }

  void pop() {
    _c.pop_back();
  }

  void swap(stack& rhs) {
    gd::swap(_c, rhs._c);
  }

 public:
  friend bool operator==(const stack& lhs, const stack& rhs) {
    return lhs._c == rhs._c;
  }

  friend bool operator<(const stack& lhs, const stack& rhs) {
    return lhs._c < rhs._c;
  }
};

// operators
template <typename T, typename Container>
bool operator==(const stack<T, Container>& lhs, const stack<T, Container>& rhs) {
  return lhs == rhs;
}

template <typename T, typename Container>
bool operator!=(const stack<T, Container>& lhs, const stack<T, Container>& rhs) {
  return !(lhs == rhs);
}

template <typename T, typename Container>
bool operator<(const stack<T, Container>& lhs, const stack<T, Container>& rhs) {
  return lhs < rhs;
}

template <typename T, typename Container>
bool operator>(const stack<T, Container>& lhs, const stack<T, Container>& rhs) {
  return rhs < lhs;
}

template <typename T, typename Container>
bool operator>=(const stack<T, Container>& lhs, const stack<T, Container>& rhs) {
  return !(lhs < rhs);
}

template <typename T, typename Container>
bool operator<=(const stack<T, Container>& lhs, const stack<T, Container>& rhs) {
  return !(lhs > rhs);
}

// overload swap
template <typename T, typename Container>
void swap(stack<T, Container>& lhs, stack<T, Container>& rhs) {
  lhs.swap(rhs);
}

}  // namespace gd

#endif  // !__MY_STACK_H