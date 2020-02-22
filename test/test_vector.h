#ifndef __TEST_VECTOR_H
#define __TEST_VECTOR_H

#include <cassert>
#include <climits>
#include <cstdio>
#include <ctime>
#include <initializer_list>
#include <iostream>
#include <iterator>
#include <random>
#include "my_alloc.h"
#include "my_defalloc.h"
#include "my_vector.h"
#include "stack_alloc.h"
#include "testdef.h"

using std::cin;
using std::cout;
using std::endl;

namespace test_vector {

struct non_trivial_class {
  int    i;
  double j;
  non_trivial_class(int _i = 0) : i(_i), j(1){};
};

int test() {
  int num_elem = 1024;

  gd::vector<non_trivial_class> my_vec;

  PRINT_SPLITER;
  printf("test: cpacity(), push_back()\n");
  PRINT_SPLITER;
  int cap = my_vec.capacity();
  printf("initial cpacity: %d\n", cap);
  printf("push back %d elements...\n", num_elem);
  for (int i = 0; i < num_elem; ++i) {
    if (cap != my_vec.capacity()) {
      cap = my_vec.capacity();
      printf("cpacity expand to: %d\n", cap);
    }
    my_vec.push_back(non_trivial_class(i));
  }
  for (int i = 0; i < num_elem; ++i)
    assert(i == my_vec[i].i);
  cout << endl;

  PRINT_SPLITER;
  printf(
      "test: size(), clear(), insert(pos, n, v), resize(), reserve(), begin(), end(), vector(first, last), swap()\n");
  PRINT_SPLITER;
  printf("size: %d\n", my_vec.size());
  printf("cpacity: %d\n\n", my_vec.capacity());

  printf("resize to double...\n");
  my_vec.resize(my_vec.size() * 2, 5);
  for (auto it = my_vec.begin() + my_vec.size() / 2; it != my_vec.end(); ++it) {
    assert(it->i == 5);
  }
  printf("size: %d\n", my_vec.size());
  printf("cpacity: %d\n\n", my_vec.capacity());

  printf("resize to half...\n");
  my_vec.resize(my_vec.size() / 2);
  printf("size: %d\n", my_vec.size());
  printf("cpacity: %d\n\n", my_vec.capacity());

  printf("clear vector...\n");
  my_vec.clear();
  printf("size: %d\n", my_vec.size());
  printf("cpacity: %d\n", my_vec.capacity());
  printf("begin() == end()?: %d\n\n", my_vec.begin() == my_vec.end());

  printf("insert %d elements with value 8 at the beginning\n", num_elem);
  my_vec.insert(my_vec.begin(), num_elem, 8);
  for (int i = 0; i < num_elem; ++i)
    assert(8 == my_vec[i].i);
  printf("size: %d\n", my_vec.size());
  printf("cpacity: %d\n\n", my_vec.capacity());

  printf("reserve double...\n");
  my_vec.reserve(my_vec.capacity() * 2);
  printf("size: %d\n", my_vec.size());
  printf("cpacity: %d\n\n", my_vec.capacity());

  printf("range initialize my_vec1...\n");
  auto                          il = {0, 2, 4, 6, 8, 10};
  gd::vector<non_trivial_class> my_vec1(il.begin(), il.end());
  for (int i = 0; i < 6; ++i) {
    assert(2 * i == my_vec1[i].i);
  }
  printf("my_vec1 size: %d\n", my_vec1.size());
  printf("my_vec1 cpacity: %d\n\n", my_vec1.capacity());

  printf("swap my_vec1 and my_vec...\n");
  my_vec.swap(my_vec1);
  for (int i = 0; i < 6; ++i) {
    assert(2 * i == my_vec[i].i);
  }
  for (auto it = my_vec1.begin(); it != my_vec1.end(); ++it) {
    assert(8 == it->i);
  }
  printf("my_vec size: %d\n", my_vec.size());
  printf("my_vec cpacity: %d\n\n", my_vec.capacity());

  printf("swap back...\n");
  my_vec.swap(my_vec1);
  for (int i = 0; i < 6; ++i) {
    assert(2 * i == my_vec1[i].i);
  }
  for (auto it = my_vec.begin(); it != my_vec.end(); ++it) {
    assert(8 == it->i);
  }
  printf("my_vec size: %d\n", my_vec.size());
  printf("my_vec cpacity: %d\n\n", my_vec.capacity());

  PRINT_SPLITER;
  printf("test: insert(pos, value), insert(pos, n, value)\n");
  PRINT_SPLITER;
  printf("insert at middle...\n");
  size_t n = my_vec.size() / 2;
  my_vec.insert(my_vec.begin() + n, 6);
  assert((my_vec.begin() + n)->i == 6);
  printf("size: %d\n", my_vec.size());
  printf("cpacity: %d\n\n", my_vec.capacity());

  printf("insert 6 elements at middle...\n");
  n = my_vec.size() / 2;
  my_vec.insert(my_vec.begin() + n, 6, 6);
  for (auto it = my_vec.begin() + n; it < my_vec.begin() + n + 6; ++it)
    assert(it->i == 6);
  printf("size: %d\n", my_vec.size());
  printf("cpacity: %d\n\n", my_vec.capacity());

  return 0;
}

}  // namespace test_vector

#endif  // !__TEST_VECTOR_H