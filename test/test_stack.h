#ifndef __TEST_STACK__H
#define __TEST_STACK__H

#include <algorithm>
#include <ctime>
#include <initializer_list>
#include <iostream>
#include <iterator>
#include <stack>
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "my_alloc.h"
#include "my_deque.h"
#include "my_stack.h"
#include "test_helper.h"

namespace gd {

template <typename T, typename Display>
void stack_print(stack<T> s, Display f) {
  while (!s.empty()) {
    f(s.top());
    s.pop();
  }
  std::cout << std::endl;
}

#define STACK_PRINT(s, display_fun)     \
  do {                                  \
    std::string sname = #s;             \
    std::cout << "- " << sname << ": "; \
    stack_print(s, display_fun);        \
  } while (0)

#define STACK_CALL(s, fun, display_fun, assert_sz)         \
  do {                                                     \
    std::string fname = #fun;                              \
    std::cout << "- After called: " << fname << std::endl; \
    fun;                                                   \
    ASSERT_EQ(s.size(), assert_sz);                        \
    STACK_PRINT(s, display_fun);                           \
  } while (0)

TEST(StackInitTest, Init) {
  stack<int> s1;
  ASSERT_EQ(s1.size(), 0);

  stack<int> s2(6);
  ASSERT_EQ(s2.size(), 6);

  deque<int> d1(6);
  stack<int> s3(d1);
  ASSERT_EQ(s3.size(), 6);

  stack<int> s4(std::move(d1));
  ASSERT_EQ(s4.size(), 6);

  stack<int> s5(s4);
  ASSERT_EQ(s5.size(), 6);

  stack<int> s6(std::move(s4));
  ASSERT_EQ(s6.size(), 6);

  stack<int> s7 = s6;
  ASSERT_EQ(s7.size(), 6);

  stack<int> s8 = std::move(s6);
  ASSERT_EQ(s8.size(), 6);
}

class StackTest : public testing::Test {
 protected:
  stack<int>        s1;
  stack<nontrivial> s2;

  StackTest() {
    deque<int>        d1 = {1, 2, 3, 4, 5, 6, 7, 8};
    deque<nontrivial> d2 = {1, 2, 3, 4, 5, 6, 7, 8};
    s1 = d1;
    s2 = d2;
  }
};

TEST_F(StackTest, Push) {
  STACK_CALL(s1, s1.push(9), display_int, 9);
  STACK_CALL(s2, s2.push(9), display_obj, 9);
}

TEST_F(StackTest, Emplace) {
  STACK_CALL(s1, s1.emplace(9), display_int, 9);
  STACK_CALL(s2, s2.emplace(9, 0), display_obj, 9);
}

TEST_F(StackTest, Pop) {
  STACK_CALL(s1, s1.pop(), display_int, 7);
  STACK_CALL(s2, s2.pop(), display_obj, 7);
}

TEST_F(StackTest, Swap) {
  deque<int> d1 = {6, 5, 4, 3, 2, 1};
  stack<int> s3(d1);
  STACK_CALL(s1, swap(s1, s3), display_int, 6);

  deque<nontrivial> d2 = {6, 5, 4, 3, 2, 1};
  stack<nontrivial> s4(d2);
  STACK_CALL(s2, swap(s2, s4), display_obj, 6);
}

TEST_F(StackTest, Operator) {
  deque<int> d1 = {1, 2, 3, 4, 5, 6, 7, 8};
  stack<int> s3(d1);

  ASSERT_TRUE(s1 == s3);
  ASSERT_TRUE(s1 >= s3);
  ASSERT_TRUE(s1 <= s3);

  s3.push(9);
  ASSERT_TRUE(s1 < s3);
  ASSERT_TRUE(s1 <= s3);
  ASSERT_TRUE(s3 > s1);
  ASSERT_TRUE(s3 >= s1);
}

}  // namespace gd

#endif  //!__TEST_STACK__H