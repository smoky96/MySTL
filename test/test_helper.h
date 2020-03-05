#ifndef TEST_HELPER_H
#define TEST_HELPER_H
#include <algorithm>
#include <cstdio>
#include <ctime>
#include <iostream>
#include <memory>

#define CPRINT(container, display_fun)                              \
  do {                                                              \
    std::string cname = #container;                                 \
    std::cout << "- " << cname << ": ";                             \
    std::for_each(container.begin(), container.end(), display_fun); \
    std::cout << std::endl;                                         \
  } while (0)

#define AFTER_CALL(container, fun, display_fun)            \
  do {                                                     \
    std::string fname = #fun;                              \
    std::cout << "- After called: " << fname << std::endl; \
    fun;                                                   \
    CPRINT(container, display_fun);                        \
  } while (0)

#define PERFORM_TEST(fun, count)                                                                      \
  do {                                                                                                \
    std::string fname = #fun;                                                                         \
    long        cnt = count;                                                                          \
    std::cout << "- Excute: \n- > " << fname << " \n- > " << cnt << " times\n";                       \
    clock_t start = clock();                                                                          \
    while (cnt--) {                                                                                   \
      fun;                                                                                            \
    }                                                                                                 \
    clock_t end = clock();                                                                            \
    std::cout << "- > Time cost: " << static_cast<double>(end - start) / CLOCKS_PER_SEC << std::endl; \
  } while (0)

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