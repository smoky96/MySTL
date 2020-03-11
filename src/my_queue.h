#ifndef __MY_QUEUE__H
#define __MY_QUEUE__H

#include "my_deque.h"
#include "my_heap.h"
#include "my_vector.h"

namespace gd {

template <typename T, typename Container = gd::deque<T>>
class queue {
 public:
  typedef Container                           container_type;
  typedef typename Container::value_type      value_type;
  typedef typename Container::size_type       size_type;
  typedef typename Container::reference       reference;
  typedef typename Container::const_reference const_reference;

 protected:
  container_type _c;

 public:
  // constructors, copy and destructor
  queue() = default;

  explicit queue(size_type n) : _c(n) {}

  queue(const Container& c) : _c(c) {}

  queue(Container&& c) : _c(std::move(c)) {}

  queue(const queue& rhs) : _c(rhs._c) {}

  queue(queue&& rhs) : _c(rhs._c) {}

  queue& operator=(const queue& rhs) {
    _c = rhs._c;
    return *this;
  }

  queue& operator=(queue&& rhs) {
    _c = std::move(rhs._c);
    return *this;
  }

  ~queue() = default;

  // element access
  reference front() {
    return _c.front();
  }

  const_reference front() const {
    return _c.front();
  }

  reference back() {
    return _c.back();
  }

  reference back() const {
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
    _c.emplace_back(std::move(value));
  }

  void pop() {
    _c.pop_front();
  }

  void swap(queue& rhs) {
    gd::swap(_c, rhs._c);
  }

 public:
  friend bool operator==(const queue& lhs, const queue& rhs) {
    return lhs._c == rhs._c;
  }

  friend bool operator<(const queue& lhs, const queue& rhs) {
    return lhs._c < rhs._c;
  }
};

// operators
template <typename T, typename Container>
bool operator==(const queue<T, Container>& lhs, const queue<T, Container>& rhs) {
  return lhs == rhs;
}

template <typename T, typename Container>
bool operator!=(const queue<T, Container>& lhs, const queue<T, Container>& rhs) {
  return !(lhs == rhs);
}

template <typename T, typename Container>
bool operator<(const queue<T, Container>& lhs, const queue<T, Container>& rhs) {
  return lhs < rhs;
}

template <typename T, typename Container>
bool operator>(const queue<T, Container>& lhs, const queue<T, Container>& rhs) {
  return rhs < lhs;
}

template <typename T, typename Container>
bool operator<=(const queue<T, Container>& lhs, const queue<T, Container>& rhs) {
  return !(rhs < lhs);
}

template <typename T, typename Container>
bool operator>=(const queue<T, Container>& lhs, const queue<T, Container>& rhs) {
  return !(lhs < rhs);
}

// overload swap
template <typename T, typename Container>
void swap(queue<T, Container>& lhs, queue<T, Container>& rhs) {
  lhs.swap(rhs);
}

// 优先队列
template <typename T, typename Container = vector<T>, typename Compare = std::less<T>>
class priority_queue {
 public:
  typedef Container                           container_type;
  typedef Compare                             value_compare;
  typedef typename Container::value_type      value_type;
  typedef typename Container::size_type       size_type;
  typedef typename Container::reference       reference;
  typedef typename Container::const_reference const_reference;

 protected:
  container_type _c;
  value_compare  _comp;

 public:  // constructors, copy and destructor
  priority_queue() = default;

  explicit priority_queue(const Compare& c) : _c(), _comp(c) {}

  template <typename InputIterator>
  priority_queue(InputIterator first, InputIterator last) : _c(first, last) {
    gd::make_heap(_c.begin(), _c.end(), _comp);
  }

  priority_queue(const Container& c) : _c(c) {
    gd::make_heap(_c.begin(), _c.end(), _comp);
  }

  priority_queue(Container&& c) : _c(std::move(c)) {
    gd::make_heap(_c.begin(), _c.end(), _comp);
  }

  priority_queue(const priority_queue& rhs) : _c(rhs._c), _comp(rhs._comp) {
    gd::make_heap(_c.begin(), _c.end(), _comp);
  }

  priority_queue(priority_queue&& rhs) : _c(std::move(rhs._c)), _comp(rhs._comp) {
    gd::make_heap(_c.begin(), _c.end(), _comp);
  }

  priority_queue& operator=(const priority_queue& rhs) {
    _c = rhs._c;
    _comp = rhs._comp;
    gd::make_heap(_c.begin(), _c.end(), _comp);
    return *this;
  }

  priority_queue& operator=(priority_queue&& rhs) {
    _c = std::move(rhs._c);
    _comp = rhs._comp;
    gd::make_heap(_c.begin(), _c.end(), _comp);
    return *this;
  }

  ~priority_queue() = default;

 public:
  // element access
  const_reference top() const {
    return _c.front();
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
    gd::push_heap(_c.begin(), _c.end(), _comp);
  }

  void push(const_reference value) {
    _c.push_back(value);
    gd::push_heap(_c.begin(), _c.end(), _comp);
  }

  void push(value_type&& value) {
    _c.push_back(std::move(value));
    gd::push_heap(_c.begin(), _c.end(), _comp);
  }

  void pop() {
    gd::pop_heap(_c.begin(), _c.end(), _comp);
    _c.pop_back();
  }

  void swap(priority_queue& rhs) {
    gd::swap(_c, rhs._c);
    std::swap(_comp, rhs._comp);
  }

 public:
  friend bool operator==(const priority_queue& lhs, const priority_queue& rhs) {
    return lhs._c == rhs._c;
  }

  friend bool operator!=(const priority_queue& lhs, const priority_queue& rhs) {
    return lhs._c != rhs._c;
  }
};

// operators
template <typename T, typename Container, typename Compare>
bool operator==(const priority_queue<T, Container, Compare>& lhs, const priority_queue<T, Container, Compare>& rhs) {
  return lhs == rhs;
}

template <typename T, typename Container, typename Compare>
bool operator!=(const priority_queue<T, Container, Compare>& lhs, const priority_queue<T, Container, Compare>& rhs) {
  return !(lhs == rhs);
}

// overload swap
template <typename T, typename Container, typename Compare>
void swap(priority_queue<T, Container, Compare>& lhs, priority_queue<T, Container, Compare>& rhs) {
  lhs.swap(rhs);
}

}  // namespace gd

#endif  //!__MY_QUEUE__H