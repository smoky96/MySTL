#ifndef __TESTDEF_H
#define __TESTDEF_H

namespace gd {

#define PRINT_SPLITER printf("=======================================\n")

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

}  // namespace gd

#endif  // !__TESTDEF_H