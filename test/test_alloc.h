#ifndef __TEST_ALLOC_H
#define __TEST_ALLOC_H

#include <climits>
#include <cstdio>
#include <ctime>
#include <iostream>
#include <random>
#include <vector>
#include "my_alloc.h"
#include "my_defalloc.h"
#include "stack_alloc.h"
#include "testdef.h"

using std::cin;
using std::cout;
using std::endl;
using std::vector;

namespace gd {
namespace test_alloc {

int test() {
  unsigned long long num_elem = 10000000;

  std::default_random_engine rd_e;
  std::normal_distribution<> n_d(10, 1.5);
  // TODO(dong) std::vector 使用 simple_alloc 在 MSVC 的 Debug 模式下进行 build 会报错，暂时无法找到原因
  // vector<int>                                                   vector_stl_alloc;
  // vector<int, gd::simple_alloc<int, gd::alloc>>                 vector_my_alloc;
  gd::stack_alloc<int>                                          stl_alloc;
  gd::stack_alloc<int, gd::simple_alloc<int, gd::alloc>>        mempool_alloc;
  gd::stack_alloc<int, gd::simple_alloc<int, gd::malloc_alloc>> malloc_alloc;

  clock_t start_time;
  clock_t end_time;

  PRINT_SPLITER;
  printf("allocator test...\n");
  PRINT_SPLITER;
  // start_time = clock();
  // for (int i = 0; i < num_elem; ++i) {
  //   vector_stl_alloc.push_back(n_d(rd_e));
  // }
  // vector_stl_alloc.clear();
  // end_time = clock();
  // cout << "vector_stl_alloc: " << static_cast<double>(end_time - start_time) / CLOCKS_PER_SEC << endl;

  // start_time = clock();
  // for (int i = 0; i < num_elem; ++i) {
  //   vector_my_alloc.push_back(n_d(rd_e));
  // }
  // vector_my_alloc.clear();
  // end_time = clock();
  // cout << "vector_my_alloc: " << static_cast<double>(end_time - start_time) / CLOCKS_PER_SEC << endl << endl;
  // start_time = clock();

  for (int i = 0; i < num_elem; ++i) {
    stl_alloc.push(n_d(rd_e));
  }
  stl_alloc.clear();
  end_time = clock();
  cout << "stl_alloc: " << static_cast<double>(end_time - start_time) / CLOCKS_PER_SEC << endl;

  start_time = clock();
  for (int i = 0; i < num_elem; ++i) {
    mempool_alloc.push(n_d(rd_e));
  }
  mempool_alloc.clear();
  end_time = clock();
  cout << "mempool_alloc: " << static_cast<double>(end_time - start_time) / CLOCKS_PER_SEC << endl;

  start_time = clock();
  for (int i = 0; i < num_elem; ++i) {
    malloc_alloc.push(n_d(rd_e));
  }
  malloc_alloc.clear();
  end_time = clock();
  cout << "malloc_alloc: " << static_cast<double>(end_time - start_time) / CLOCKS_PER_SEC << endl << endl;

  return 0;
}

}  // namespace test_alloc
}  // namespace gd

#endif  // !__TEST_ALLOC_H