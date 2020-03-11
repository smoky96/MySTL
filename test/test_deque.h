#ifndef __TEST_DEQUE_H
#define __TEST_DEQUE_H

#include <algorithm>
#include <ctime>
#include <deque>
#include <initializer_list>
#include <iostream>
#include <iterator>
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "my_alloc.h"
#include "my_deque.h"
#include "test_helper.h"

namespace gd {

using testing::ElementsAre;
using testing::ElementsAreArray;

TEST(DequeInitTest, Init) {
  int a[] = {1, 2, 3, 4, 5, 6};

  deque<int> d1;
  ASSERT_EQ(d1.size(), 0);

  deque<int> d2(6);
  ASSERT_EQ(d2.size(), 6);
  ASSERT_THAT(d2, ElementsAre(0, 0, 0, 0, 0, 0));

  deque<int> d3(size_t(6), 8);
  ASSERT_EQ(d3.size(), 6);
  ASSERT_THAT(d3, ElementsAre(8, 8, 8, 8, 8, 8));

  deque<int> d4(std::begin(a), std::end(a));
  ASSERT_EQ(d4.size(), 6);
  ASSERT_THAT(d4, ElementsAre(1, 2, 3, 4, 5, 6));

  deque<int> d5(d4);
  ASSERT_EQ(d5.size(), 6);
  ASSERT_THAT(d5, ElementsAre(1, 2, 3, 4, 5, 6));

  deque<int> d6(std::move(d4));
  ASSERT_EQ(d6.size(), 6);
  ASSERT_THAT(d6, ElementsAre(1, 2, 3, 4, 5, 6));

  deque<int> d7({1, 2, 3, 4, 5, 6, 7, 8});
  ASSERT_EQ(d7.size(), 8);
  ASSERT_THAT(d7, ElementsAre(1, 2, 3, 4, 5, 6, 7, 8));

  deque<int> d8;
  d8 = d7;
  ASSERT_EQ(d8.size(), 8);
  ASSERT_THAT(d8, ElementsAre(1, 2, 3, 4, 5, 6, 7, 8));

  deque<int> d9;
  d9 = std::move(d7);
  ASSERT_EQ(d9.size(), 8);
  ASSERT_THAT(d9, ElementsAre(1, 2, 3, 4, 5, 6, 7, 8));

  deque<int> d10;
  d10 = {1, 2, 3, 4, 5, 6, 7, 8};
  ASSERT_EQ(d10.size(), 8);
  ASSERT_THAT(d10, ElementsAre(1, 2, 3, 4, 5, 6, 7, 8));

  d10.assign(std::begin(a), std::end(a));
  ASSERT_EQ(d10.size(), 6);
  ASSERT_THAT(d10, ElementsAre(1, 2, 3, 4, 5, 6));

  d10.assign(size_t(8), 8);
  ASSERT_EQ(d10.size(), 8);
  ASSERT_THAT(d10, ElementsAre(8, 8, 8, 8, 8, 8, 8, 8));

  d10.assign({1, 2, 3, 4, 5, 6});
  ASSERT_EQ(d10.size(), 6);
  ASSERT_THAT(d10, ElementsAre(1, 2, 3, 4, 5, 6));
}

class DequeTest : public testing::Test {
 protected:
  deque<int, alloc>        d1;
  deque<nontrivial, alloc> d2;

  deque<int, alloc>        d3;
  deque<nontrivial, alloc> d4;

  // deque<int, malloc_alloc>        d1;
  // deque<nontrivial, malloc_alloc> d2;

  // deque<int, malloc_alloc>        d3;
  // deque<nontrivial, malloc_alloc> d4;

  deque<int>::iterator        it1;
  deque<nontrivial>::iterator it2;

  deque<int>::iterator        it3;
  deque<nontrivial>::iterator it4;

  DequeTest()
      : d3({1, 2, 3, 4, 5, 6, 7, 8}),
        d4({
            nontrivial(0, 1),
            nontrivial(2, 3),
            nontrivial(4, 5),
            nontrivial(6, 7),
            nontrivial(8, 9),
        }) {
    std::cout << "- Initial: " << std::endl;
    std::cout << "- d1: null" << std::endl;
    std::cout << "- d2: null" << std::endl;

    std::cout << "- d3: ";
    std::for_each(d3.begin(), d3.end(), display_int);
    std::cout << std::endl;

    std::cout << "- d4: ";
    std::for_each(d4.begin(), d4.end(), display_obj);
    std::cout << std::endl;
  }
};

TEST_F(DequeTest, Iterator) {
  ASSERT_TRUE(d1.begin() == d1.end());
  ASSERT_TRUE(d1.cbegin() == d1.cend());

  ASSERT_TRUE(d2.begin() == d2.end());
  ASSERT_TRUE(d2.cbegin() == d2.cend());

  int count;

  it3 = d3.begin();
  count = 0;
  while (it3 != d3.end()) {
    ++it3;
    ++count;
  }
  ASSERT_EQ(8, count);
  ASSERT_EQ(*d3.cbegin(), *d3.begin());

  count = 0;
  it4 = d4.begin();
  while (it4 != d4.end()) {
    ++it4;
    ++count;
  }
  ASSERT_EQ(5, count);
  ASSERT_EQ(*d4.cbegin(), *d4.begin());
}

TEST_F(DequeTest, Capacity) {
  ASSERT_TRUE(d1.empty());
  ASSERT_TRUE(d2.empty());

  ASSERT_FALSE(d3.empty());
  ASSERT_FALSE(d4.empty());

  ASSERT_EQ(0, d1.size());
  ASSERT_EQ(0, d2.size());

  ASSERT_EQ(8, d3.size());
  ASSERT_EQ(5, d4.size());

  std::cout << "- Max size of deque<int> on this machine: " << d1.max_size() << std::endl;

  AFTER_CALL(d1, d1.resize(6, 6), display_int);
  ASSERT_THAT(d1, ElementsAre(6, 6, 6, 6, 6, 6));

  AFTER_CALL(d2, d2.resize(5), display_obj);
  ASSERT_THAT(d2, ElementsAre(nontrivial(), nontrivial(), nontrivial(), nontrivial(), nontrivial()));

  AFTER_CALL(d3, d3.resize(5), display_int);
  ASSERT_THAT(d3, ElementsAre(1, 2, 3, 4, 5));

  AFTER_CALL(d4, d4.resize(4), display_obj);
  ASSERT_THAT(d4, ElementsAre(nontrivial(0, 1), nontrivial(2, 3), nontrivial(4, 5), nontrivial(6, 7)));
  ASSERT_EQ(d4.size(), 4);
}

TEST_F(DequeTest, ElemAccess) {
  ASSERT_EQ(d3.front(), 1);
  ASSERT_EQ(d4.front(), nontrivial(0, 1));

  ASSERT_EQ(d3.back(), 8);
  ASSERT_EQ(d4.back(), nontrivial(8, 9));

  ASSERT_EQ(d3[5], 6);
  ASSERT_EQ(d4[2], nontrivial(4, 5));

  ASSERT_EQ(d3.at(6), 7);
  ASSERT_EQ(d4.at(1), nontrivial(2, 3));
}

TEST_F(DequeTest, EmplaceAndPushAtBack) {
  AFTER_CALL(d1, d1.emplace_back(0), display_int);
  ASSERT_THAT(d1, ElementsAre(0));
  AFTER_CALL(d1, d1.pop_back(), display_int);

  AFTER_CALL(d2, d2.emplace_back(0, 1), display_obj);
  ASSERT_THAT(d2, ElementsAre(nontrivial(0, 1)));
  ASSERT_EQ(d2.size(), 1);
  AFTER_CALL(d2, d2.pop_back(), display_obj);
  ASSERT_THAT(d2, ElementsAre());
  ASSERT_EQ(d2.size(), 0);

  AFTER_CALL(d3, d3.emplace_back(9), display_int);
  ASSERT_THAT(d3, ElementsAre(1, 2, 3, 4, 5, 6, 7, 8, 9));
  AFTER_CALL(d4, d4.emplace_back(10, 11), display_obj);
  ASSERT_THAT(d4, ElementsAre(nontrivial(0, 1), nontrivial(2, 3), nontrivial(4, 5), nontrivial(6, 7), nontrivial(8, 9),
                              nontrivial(10, 11)));
  ASSERT_EQ(d4.size(), 6);

  AFTER_CALL(d3, d3.push_back(10), display_int);
  ASSERT_THAT(d3, ElementsAre(1, 2, 3, 4, 5, 6, 7, 8, 9, 10));
  AFTER_CALL(d4, d4.push_back(nontrivial(12, 13)), display_obj);
  ASSERT_THAT(d4, ElementsAre(nontrivial(0, 1), nontrivial(2, 3), nontrivial(4, 5), nontrivial(6, 7), nontrivial(8, 9),
                              nontrivial(10, 11), nontrivial(12, 13)));
  ASSERT_EQ(d4.size(), 7);

  it3 = d3.begin();
  advance(it3, 5);
  AFTER_CALL(d3, d3.emplace(it3, 11), display_int);
  ASSERT_THAT(d3, ElementsAre(1, 2, 3, 4, 5, 11, 6, 7, 8, 9, 10));

  it4 = d4.begin();
  advance(it4, 4);
  AFTER_CALL(d4, d4.emplace(it4, 14, 15), display_obj);
  ASSERT_THAT(d4, ElementsAre(nontrivial(0, 1), nontrivial(2, 3), nontrivial(4, 5), nontrivial(6, 7),
                              nontrivial(14, 15), nontrivial(8, 9), nontrivial(10, 11), nontrivial(12, 13)));
  ASSERT_EQ(d4.size(), 8);
}

TEST_F(DequeTest, EmplaceAndPushAtFront) {
  AFTER_CALL(d1, d1.emplace_front(0), display_int);
  ASSERT_THAT(d1, ElementsAre(0));
  AFTER_CALL(d1, d1.pop_front(), display_int);

  AFTER_CALL(d2, d2.emplace_front(0, 1), display_obj);
  ASSERT_THAT(d2, ElementsAre(nontrivial(0, 1)));
  ASSERT_EQ(d2.size(), 1);
  AFTER_CALL(d2, d2.pop_front(), display_obj);
  ASSERT_THAT(d2, ElementsAre());
  ASSERT_EQ(d2.size(), 0);

  AFTER_CALL(d3, d3.emplace_front(0), display_int);
  ASSERT_THAT(d3, ElementsAre(0, 1, 2, 3, 4, 5, 6, 7, 8));
  AFTER_CALL(d4, d4.emplace_front(-1, 0), display_obj);
  ASSERT_THAT(d4, ElementsAre(nontrivial(-1, 0), nontrivial(0, 1), nontrivial(2, 3), nontrivial(4, 5), nontrivial(6, 7),
                              nontrivial(8, 9)));
  ASSERT_EQ(d4.size(), 6);

  AFTER_CALL(d3, d3.push_front(-1), display_int);
  ASSERT_THAT(d3, ElementsAre(-1, 0, 1, 2, 3, 4, 5, 6, 7, 8));
  AFTER_CALL(d4, d4.push_front(nontrivial(-3, -2)), display_obj);
  ASSERT_THAT(d4, ElementsAre(nontrivial(-3, -2), nontrivial(-1, 0), nontrivial(0, 1), nontrivial(2, 3),
                              nontrivial(4, 5), nontrivial(6, 7), nontrivial(8, 9)));
  ASSERT_EQ(d4.size(), 7);

  it3 = d3.begin();
  advance(it3, 7);
  AFTER_CALL(d3, d3.emplace(it3, 11), display_int);
  ASSERT_THAT(d3, ElementsAre(-1, 0, 1, 2, 3, 4, 5, 11, 6, 7, 8));

  it4 = d4.begin();
  advance(it4, 6);
  AFTER_CALL(d4, d4.emplace(it4, 14, 15), display_obj);
  ASSERT_THAT(d4, ElementsAre(nontrivial(-3, -2), nontrivial(-1, 0), nontrivial(0, 1), nontrivial(2, 3),
                              nontrivial(4, 5), nontrivial(6, 7), nontrivial(14, 15), nontrivial(8, 9)));
  ASSERT_EQ(d4.size(), 8);
}

TEST_F(DequeTest, MultiEmplaceAndPush) {
  std::deque<int>        stl_d1;
  std::deque<nontrivial> stl_d2;

  auto pushRound = [&]() {
    for (int i = 0; i < 100; ++i) {
      d1.push_back(i);
      d2.push_back(i);
      stl_d1.push_back(i);
      stl_d2.push_back(i);
    }
  };

  auto emplaceRound = [&]() {
    for (int i = 0; i < 100; ++i) {
      d1.emplace_back(i);
      d2.emplace_back(i);
      stl_d1.emplace_back(i);
      stl_d2.emplace_back(i);
    }
  };

  pushRound();
  ASSERT_THAT(d1, ElementsAreArray(stl_d1));
  ASSERT_THAT(d2, ElementsAreArray(stl_d2));
  ASSERT_EQ(d1.size(), 100);
  ASSERT_EQ(d2.size(), 100);

  emplaceRound();
  ASSERT_THAT(d1, ElementsAreArray(stl_d1));
  ASSERT_THAT(d2, ElementsAreArray(stl_d2));
  ASSERT_EQ(d1.size(), 200);
  ASSERT_EQ(d2.size(), 200);
}

TEST_F(DequeTest, Insert) {
  it3 = d3.begin();
  advance(it3, 4);
  AFTER_CALL(d3, d3.insert(it3, 9), display_int);
  ASSERT_THAT(d3, ElementsAre(1, 2, 3, 4, 9, 5, 6, 7, 8));

  it4 = d4.begin();
  advance(it4, 3);
  AFTER_CALL(d4, d4.insert(it4, nontrivial(10, 11)), display_obj);
  ASSERT_THAT(d4, ElementsAre(nontrivial(0, 1), nontrivial(2, 3), nontrivial(4, 5), nontrivial(10, 11),
                              nontrivial(6, 7), nontrivial(8, 9)));
  ASSERT_EQ(d4.size(), 6);

  it3 = d3.begin();
  advance(it3, 4);
  AFTER_CALL(d3, d3.insert(it3, size_t(2), 10), display_int);
  ASSERT_THAT(d3, ElementsAre(1, 2, 3, 4, 10, 10, 9, 5, 6, 7, 8));

  it4 = d4.begin();
  advance(it4, 3);
  AFTER_CALL(d4, d4.insert(it4, 2, nontrivial(12, 13)), display_obj);
  ASSERT_THAT(d4, ElementsAre(nontrivial(0, 1), nontrivial(2, 3), nontrivial(4, 5), nontrivial(12, 13),
                              nontrivial(12, 13), nontrivial(10, 11), nontrivial(6, 7), nontrivial(8, 9)));
  ASSERT_EQ(d4.size(), 8);

  d1.insert(d1.end(), size_t(2), 11);
  it3 = d3.begin();
  advance(it3, 4);
  AFTER_CALL(d3, d3.insert(it3, d1.begin(), d1.end()), display_int);
  ASSERT_THAT(d3, ElementsAre(1, 2, 3, 4, 11, 11, 10, 10, 9, 5, 6, 7, 8));

  d2.insert(d2.end(), 2, nontrivial(14, 15));
  it4 = d4.begin();
  advance(it4, 3);
  AFTER_CALL(d4, d4.insert(it4, d2.begin(), d2.end()), display_obj);
  ASSERT_THAT(
      d4, ElementsAre(nontrivial(0, 1), nontrivial(2, 3), nontrivial(4, 5), nontrivial(14, 15), nontrivial(14, 15),
                      nontrivial(12, 13), nontrivial(12, 13), nontrivial(10, 11), nontrivial(6, 7), nontrivial(8, 9)));
  ASSERT_EQ(d4.size(), 10);

  it3 = d3.begin();
  advance(it3, 4);
  AFTER_CALL(d3, d3.insert(it3, std::initializer_list<int>{12, 13, 14, 15}), display_int);
  ASSERT_THAT(d3, ElementsAre(1, 2, 3, 4, 12, 13, 14, 15, 11, 11, 10, 10, 9, 5, 6, 7, 8));

  it4 = d4.begin();
  advance(it4, 3);
  AFTER_CALL(d4, d4.insert(it4, std::initializer_list<nontrivial>{16, 17, 18}), display_obj);
  ASSERT_THAT(
      d4, ElementsAre(nontrivial(0, 1), nontrivial(2, 3), nontrivial(4, 5), nontrivial(16, 0), nontrivial(17, 0),
                      nontrivial(18, 0), nontrivial(14, 15), nontrivial(14, 15), nontrivial(12, 13), nontrivial(12, 13),
                      nontrivial(10, 11), nontrivial(6, 7), nontrivial(8, 9)));
  ASSERT_EQ(d4.size(), 13);
}

TEST_F(DequeTest, Erase) {
  it3 = d3.begin();
  advance(it3, 4);
  AFTER_CALL(d3, d3.erase(it3), display_int);
  ASSERT_THAT(d3, ElementsAre(1, 2, 3, 4, 6, 7, 8));

  it4 = d4.begin();
  advance(it4, 2);
  AFTER_CALL(d4, d4.erase(it4), display_obj);
  ASSERT_THAT(d4, ElementsAre(nontrivial(0, 1), nontrivial(2, 3), nontrivial(6, 7), nontrivial(8, 9)));
  ASSERT_EQ(d4.size(), 4);

  it3 = d3.begin();
  advance(it3, 3);
  auto it_tmp3 = d3.begin();
  advance(it_tmp3, 1);
  AFTER_CALL(d3, d3.erase(it_tmp3, it3), display_int);
  ASSERT_THAT(d3, ElementsAre(1, 4, 6, 7, 8));

  it4 = d4.begin();
  advance(it4, 3);
  auto it_tmp4 = d4.begin();
  advance(it_tmp4, 1);
  AFTER_CALL(d4, d4.erase(it_tmp4, it4), display_obj);
  ASSERT_THAT(d4, ElementsAre(nontrivial(0, 1), nontrivial(8, 9)));
  ASSERT_EQ(d4.size(), 2);

  AFTER_CALL(d3, d3.clear(), display_int);
  ASSERT_THAT(d3, ElementsAre());

  AFTER_CALL(d4, d4.clear(), display_obj);
  ASSERT_THAT(d4, ElementsAre());
  ASSERT_EQ(d4.size(), 0);
}

TEST_F(DequeTest, Swap) {
  AFTER_CALL(d1, d1.swap(d3), display_int);
  ASSERT_THAT(d1, ElementsAre(1, 2, 3, 4, 5, 6, 7, 8));

  AFTER_CALL(d2, d2.swap(d4), display_obj);
  ASSERT_THAT(d2,
              ElementsAre(nontrivial(0, 1), nontrivial(2, 3), nontrivial(4, 5), nontrivial(6, 7), nontrivial(8, 9)));
  ASSERT_EQ(d2.size(), 5);
  ASSERT_EQ(d4.size(), 0);
}

TEST_F(DequeTest, Operator) {
  AFTER_CALL(d1, d1.assign({1, 2, 3, 4, 5, 6, 7, 8}), display_int);
  ASSERT_TRUE(d1 == d3);
  ASSERT_TRUE(d1 >= d3);
  ASSERT_TRUE(d1 <= d3);

  AFTER_CALL(d1, d1.assign({1, 2, 3, 4, 5, 6, 8, 8}), display_int);
  ASSERT_TRUE(d1 != d3);
  ASSERT_TRUE(d1 > d3);
  ASSERT_TRUE(d1 >= d3);

  AFTER_CALL(d2, d2.assign({nontrivial(0, 1), nontrivial(2, 3), nontrivial(4, 5), nontrivial(6, 7), nontrivial(8, 9)}),
             display_obj);
  ASSERT_TRUE(d2 == d4);

  AFTER_CALL(d2, d2.assign({nontrivial(0, 1), nontrivial(2, 3), nontrivial(5, 5), nontrivial(6, 7), nontrivial(8, 9)}),
             display_obj);
  ASSERT_TRUE(!(d2 == d4));
}

#if PERFORMANCE_TEST
TEST(DeqPerformTest, Performance) {
  std::deque<int> std_deq1;
  gd::deque<int>  my_deq1;

  std::deque<nontrivial> std_deq2;
  gd::deque<nontrivial>  my_deq2;

  PERFORM_TEST(std_deq1.push_back(6), 200000000);
  PERFORM_TEST(my_deq1.push_back(6), 200000000);

  PERFORM_TEST(std_deq2.push_back(8), 20000000);
  PERFORM_TEST(my_deq2.push_back(8), 20000000);
}
#endif

}  // namespace gd

#endif  // !__TEST_DEQUE_H