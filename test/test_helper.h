#ifndef TEST_HELPER_H
#define TEST_HELPER_H
#include <algorithm>
#include <cstdio>
#include <ctime>
#include <iostream>
#include <memory>
#include "testdef.h"

namespace gd {

class nontrivial {
  friend bool operator==(const nontrivial& lhs, const nontrivial& rhs);

 public:
  std::unique_ptr<int> i;
  int                  j;

 public:
  nontrivial() : i(new int(0)), j(0) {}
  nontrivial(int _i, int _j = 0) : i(new int(_i)), j(_j) {}
  nontrivial(const nontrivial& rhs) {
    if (&rhs != this) {
      i.reset(new int(*rhs.i));
      j = rhs.j;
    }
  }

  nontrivial operator=(const nontrivial& rhs) {
    if (&rhs != this) {
      i.reset(new int(*rhs.i));
      j = rhs.j;
    }
    return *this;
  }

  ~nontrivial() = default;

  void print() const {
    printf("(%d, %d) ", *i, j);
  }
};

bool operator==(const nontrivial& lhs, const nontrivial& rhs);

inline void display_obj(const nontrivial& obj) {
  obj.print();
}

inline void display_int(const int& i) {
  std::cout << i << " ";
}

inline void display_double(const double& i) {
  std::cout << i << " ";
}

}  // namespace gd

#endif  // !TEST_HELPER_H