#ifndef __STACK_ALLOC_H
#define __STACK_ALLOC_H

#include <memory>
#include "my_construct.h"

namespace gd {

template <typename T>
struct stack_node {
  T           data;
  stack_node* prev;
};

template <typename T, typename Alloc = std::allocator<T>>
class stack_alloc {
 public:
  typedef stack_node<T>                                node;
  typedef typename Alloc::template rebind<node>::other allocator;

  stack_alloc() {
    __head = 0;
  }

  ~stack_alloc() {
    clear();
  }

  bool empty() {
    return __head == 0;
  }

  void clear() {
    node* cur = __head;
    while (cur != 0) {
      node* tmp = cur->prev;
      destroy(cur);
      __allocator.deallocate(cur, 1);
      cur = tmp;
    }
    __head = 0;
  }

  void push(T element) {
    node* new_node = __allocator.allocate(1);
    construct(new_node, node());
    new_node->data = element;
    new_node->prev = __head;
    __head = new_node;
  }

  T pop() {
    T     ret = __head->data;
    node* tmp = __head->prev;
    destroy(__head);
    __allocator.deallocate(__head);
    __head = tmp;
  }

  T top() {
    return __head->data;
  }

 private:
  allocator __allocator;
  node*     __head;
};

}  // namespace gd

#endif  // !__STACK_ALLOC_H