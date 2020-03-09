#ifndef __TEST_VECTOR_H
#define __TEST_VECTOR_H

#include <algorithm>
#include <ctime>
#include <initializer_list>
#include <iostream>
#include <iterator>
#include <vector>
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "my_alloc.h"
#include "my_vector.h"
#include "test_helper.h"

namespace gd {

using testing::ElementsAre;

TEST(VecInitialTest, Init) {
  int a[] = {1, 2, 3, 4, 5, 6};

  vector<int> v1;
  ASSERT_EQ(v1.size(), 0);

  vector<int> v2(6);
  ASSERT_EQ(v2.size(), 6);
  ASSERT_THAT(v2, ElementsAre(0, 0, 0, 0, 0, 0));

  vector<int> v3(size_t(6), 8);
  ASSERT_EQ(v3.size(), 6);
  ASSERT_THAT(v3, ElementsAre(8, 8, 8, 8, 8, 8));

  vector<int> v4(std::begin(a), std::end(a));
  ASSERT_EQ(v4.size(), 6);
  ASSERT_THAT(v4, ElementsAre(1, 2, 3, 4, 5, 6));

  vector<int> v5(v4);
  ASSERT_EQ(v5.size(), 6);
  ASSERT_THAT(v5, ElementsAre(1, 2, 3, 4, 5, 6));

  vector<int> v6(std::move(v4));
  ASSERT_EQ(v6.size(), 6);
  ASSERT_THAT(v6, ElementsAre(1, 2, 3, 4, 5, 6));

  vector<int> v7({1, 2, 3, 4, 5, 6, 7, 8});
  ASSERT_EQ(v7.size(), 8);
  ASSERT_THAT(v7, ElementsAre(1, 2, 3, 4, 5, 6, 7, 8));

  vector<int> v8;
  v8 = v7;
  ASSERT_EQ(v8.size(), 8);
  ASSERT_THAT(v8, ElementsAre(1, 2, 3, 4, 5, 6, 7, 8));

  vector<int> v9;
  v9 = std::move(v7);
  ASSERT_EQ(v9.size(), 8);
  ASSERT_THAT(v9, ElementsAre(1, 2, 3, 4, 5, 6, 7, 8));

  vector<int> v10;
  v10 = {1, 2, 3, 4, 5, 6, 7, 8};
  ASSERT_EQ(v10.size(), 8);
  ASSERT_THAT(v10, ElementsAre(1, 2, 3, 4, 5, 6, 7, 8));

  v10.assign(std::begin(a), std::end(a));
  ASSERT_EQ(v10.size(), 6);
  ASSERT_THAT(v10, ElementsAre(1, 2, 3, 4, 5, 6));

  v10.assign(size_t(8), 8);
  ASSERT_EQ(v10.size(), 8);
  ASSERT_THAT(v10, ElementsAre(8, 8, 8, 8, 8, 8, 8, 8));

  v10.assign({1, 2, 3, 4, 5, 6});
  ASSERT_EQ(v10.size(), 6);
  ASSERT_THAT(v10, ElementsAre(1, 2, 3, 4, 5, 6));
}

class VectorTest : public testing::Test {
 protected:
  vector<int, alloc>        v1;
  vector<nontrivial, alloc> v2;

  vector<int, alloc>        v3;
  vector<nontrivial, alloc> v4;

  // vector<int, malloc_alloc>        v1;
  // vector<nontrivial, malloc_alloc> v2;

  // vector<int, malloc_alloc>        v3;
  // vector<nontrivial, malloc_alloc> v4;

  vector<int>::iterator        it1;
  vector<nontrivial>::iterator it2;

  vector<int>::iterator        it3;
  vector<nontrivial>::iterator it4;

  VectorTest()
      : v3({1, 2, 3, 4, 5, 6, 7, 8}),
        v4({
            nontrivial(0, 1),
            nontrivial(2, 3),
            nontrivial(4, 5),
            nontrivial(6, 7),
            nontrivial(8, 9),
        }) {
    std::cout << "- Initial: " << std::endl;
    std::cout << "- v1: null" << std::endl;
    std::cout << "- v2: null" << std::endl;

    std::cout << "- v3: ";
    std::for_each(v3.begin(), v3.end(), display_int);
    std::cout << std::endl;

    std::cout << "- v4: ";
    std::for_each(v4.begin(), v4.end(), display_obj);
    std::cout << std::endl;
  }
};

TEST_F(VectorTest, Iterator) {
  ASSERT_TRUE(v1.begin() == v1.end());
  ASSERT_TRUE(v1.cbegin() == v1.cend());

  ASSERT_TRUE(v2.begin() == v2.end());
  ASSERT_TRUE(v2.cbegin() == v2.cend());

  int count;

  it3 = v3.begin();
  count = 0;
  while (it3 != v3.end()) {
    ++it3;
    ++count;
  }
  ASSERT_EQ(8, count);

  count = 0;
  it4 = v4.begin();
  while (it4 != v4.end()) {
    ++it4;
    ++count;
  }
  ASSERT_EQ(5, count);
}

TEST_F(VectorTest, Capacity) {
  ASSERT_TRUE(v1.empty());
  ASSERT_TRUE(v2.empty());

  ASSERT_FALSE(v3.empty());
  ASSERT_FALSE(v4.empty());

  ASSERT_EQ(0, v1.size());
  ASSERT_EQ(0, v2.size());

  ASSERT_EQ(8, v3.size());
  ASSERT_EQ(5, v4.size());

  std::cout << "- Max size of vector<int> on this machine: " << v1.max_size() << std::endl;

  AFTER_CALL(v1, v1.resize(6, 6), display_int);
  ASSERT_THAT(v1, ElementsAre(6, 6, 6, 6, 6, 6));

  AFTER_CALL(v2, v2.resize(5), display_obj);
  ASSERT_THAT(v2, ElementsAre(nontrivial(), nontrivial(), nontrivial(), nontrivial(), nontrivial()));

  AFTER_CALL(v3, v3.resize(5), display_int);
  ASSERT_THAT(v3, ElementsAre(1, 2, 3, 4, 5));

  AFTER_CALL(v4, v4.resize(4), display_obj);
  ASSERT_THAT(v4, ElementsAre(nontrivial(0, 1), nontrivial(2, 3), nontrivial(4, 5), nontrivial(6, 7)));
  ASSERT_EQ(v4.size(), 4);
}

TEST_F(VectorTest, ElemAccess) {
  ASSERT_EQ(v3.front(), 1);
  ASSERT_EQ(v4.front(), nontrivial(0, 1));

  ASSERT_EQ(v3.back(), 8);
  ASSERT_EQ(v4.back(), nontrivial(8, 9));

  ASSERT_EQ(v3[5], 6);
  ASSERT_EQ(v4[2], nontrivial(4, 5));

  ASSERT_EQ(v3.at(6), 7);
  ASSERT_EQ(v4.at(1), nontrivial(2, 3));
}

TEST_F(VectorTest, EmplaceAndPush) {
  AFTER_CALL(v1, v1.emplace_back(0), display_int);
  ASSERT_THAT(v1, ElementsAre(0));
  AFTER_CALL(v1, v1.pop_back(), display_int);

  AFTER_CALL(v2, v2.emplace_back(0, 1), display_obj);
  ASSERT_THAT(v2, ElementsAre(nontrivial(0, 1)));
  ASSERT_EQ(v2.size(), 1);
  AFTER_CALL(v2, v2.pop_back(), display_obj);
  ASSERT_THAT(v2, ElementsAre());
  ASSERT_EQ(v2.size(), 0);

  AFTER_CALL(v3, v3.emplace_back(9), display_int);
  ASSERT_THAT(v3, ElementsAre(1, 2, 3, 4, 5, 6, 7, 8, 9));
  AFTER_CALL(v4, v4.emplace_back(10, 11), display_obj);
  ASSERT_THAT(v4, ElementsAre(nontrivial(0, 1), nontrivial(2, 3), nontrivial(4, 5), nontrivial(6, 7), nontrivial(8, 9),
                              nontrivial(10, 11)));
  ASSERT_EQ(v4.size(), 6);

  AFTER_CALL(v3, v3.push_back(10), display_int);
  ASSERT_THAT(v3, ElementsAre(1, 2, 3, 4, 5, 6, 7, 8, 9, 10));
  AFTER_CALL(v4, v4.push_back(nontrivial(12, 13)), display_obj);
  ASSERT_THAT(v4, ElementsAre(nontrivial(0, 1), nontrivial(2, 3), nontrivial(4, 5), nontrivial(6, 7), nontrivial(8, 9),
                              nontrivial(10, 11), nontrivial(12, 13)));
  ASSERT_EQ(v4.size(), 7);

  it3 = v3.begin();
  advance(it3, 5);
  AFTER_CALL(v3, v3.emplace(it3, 11), display_int);
  ASSERT_THAT(v3, ElementsAre(1, 2, 3, 4, 5, 11, 6, 7, 8, 9, 10));

  it4 = v4.begin();
  advance(it4, 4);
  AFTER_CALL(v4, v4.emplace(it4, 14, 15), display_obj);
  ASSERT_THAT(v4, ElementsAre(nontrivial(0, 1), nontrivial(2, 3), nontrivial(4, 5), nontrivial(6, 7),
                              nontrivial(14, 15), nontrivial(8, 9), nontrivial(10, 11), nontrivial(12, 13)));
  ASSERT_EQ(v4.size(), 8);
}

TEST_F(VectorTest, Insert) {
  it3 = v3.begin();
  advance(it3, 4);
  AFTER_CALL(v3, v3.insert(it3, 9), display_int);
  ASSERT_THAT(v3, ElementsAre(1, 2, 3, 4, 9, 5, 6, 7, 8));

  it4 = v4.begin();
  advance(it4, 3);
  AFTER_CALL(v4, v4.insert(it4, nontrivial(10, 11)), display_obj);
  ASSERT_THAT(v4, ElementsAre(nontrivial(0, 1), nontrivial(2, 3), nontrivial(4, 5), nontrivial(10, 11),
                              nontrivial(6, 7), nontrivial(8, 9)));
  ASSERT_EQ(v4.size(), 6);

  it3 = v3.begin();
  advance(it3, 4);
  AFTER_CALL(v3, v3.insert(it3, size_t(2), 10), display_int);
  ASSERT_THAT(v3, ElementsAre(1, 2, 3, 4, 10, 10, 9, 5, 6, 7, 8));

  it4 = v4.begin();
  advance(it4, 3);
  AFTER_CALL(v4, v4.insert(it4, 2, nontrivial(12, 13)), display_obj);
  ASSERT_THAT(v4, ElementsAre(nontrivial(0, 1), nontrivial(2, 3), nontrivial(4, 5), nontrivial(12, 13),
                              nontrivial(12, 13), nontrivial(10, 11), nontrivial(6, 7), nontrivial(8, 9)));
  ASSERT_EQ(v4.size(), 8);

  v1.insert(v1.end(), size_t(2), 11);
  it3 = v3.begin();
  advance(it3, 4);
  AFTER_CALL(v3, v3.insert(it3, v1.begin(), v1.end()), display_int);
  ASSERT_THAT(v3, ElementsAre(1, 2, 3, 4, 11, 11, 10, 10, 9, 5, 6, 7, 8));

  v2.insert(v2.end(), 2, nontrivial(14, 15));
  it4 = v4.begin();
  advance(it4, 3);
  AFTER_CALL(v4, v4.insert(it4, v2.begin(), v2.end()), display_obj);
  ASSERT_THAT(
      v4, ElementsAre(nontrivial(0, 1), nontrivial(2, 3), nontrivial(4, 5), nontrivial(14, 15), nontrivial(14, 15),
                      nontrivial(12, 13), nontrivial(12, 13), nontrivial(10, 11), nontrivial(6, 7), nontrivial(8, 9)));
  ASSERT_EQ(v4.size(), 10);

  it3 = v3.begin();
  advance(it3, 4);
  AFTER_CALL(v3, v3.insert(it3, std::initializer_list<int>{12, 13, 14, 15}), display_int);
  ASSERT_THAT(v3, ElementsAre(1, 2, 3, 4, 12, 13, 14, 15, 11, 11, 10, 10, 9, 5, 6, 7, 8));

  it4 = v4.begin();
  advance(it4, 3);
  AFTER_CALL(v4, v4.insert(it4, std::initializer_list<nontrivial>{16, 17, 18}), display_obj);
  ASSERT_THAT(
      v4, ElementsAre(nontrivial(0, 1), nontrivial(2, 3), nontrivial(4, 5), nontrivial(16, 0), nontrivial(17, 0),
                      nontrivial(18, 0), nontrivial(14, 15), nontrivial(14, 15), nontrivial(12, 13), nontrivial(12, 13),
                      nontrivial(10, 11), nontrivial(6, 7), nontrivial(8, 9)));
  ASSERT_EQ(v4.size(), 13);
}

TEST_F(VectorTest, Erase) {
  it3 = v3.begin();
  advance(it3, 4);
  AFTER_CALL(v3, v3.erase(it3), display_int);
  ASSERT_THAT(v3, ElementsAre(1, 2, 3, 4, 6, 7, 8));

  it4 = v4.begin();
  advance(it4, 2);
  AFTER_CALL(v4, v4.erase(it4), display_obj);
  ASSERT_THAT(v4, ElementsAre(nontrivial(0, 1), nontrivial(2, 3), nontrivial(6, 7), nontrivial(8, 9)));
  ASSERT_EQ(v4.size(), 4);

  it3 = v3.begin();
  advance(it3, 3);
  auto it_tmp3 = v3.begin();
  advance(it_tmp3, 1);
  AFTER_CALL(v3, v3.erase(it_tmp3, it3), display_int);
  ASSERT_THAT(v3, ElementsAre(1, 4, 6, 7, 8));

  it4 = v4.begin();
  advance(it4, 3);
  auto it_tmp4 = v4.begin();
  advance(it_tmp4, 1);
  AFTER_CALL(v4, v4.erase(it_tmp4, it4), display_obj);
  ASSERT_THAT(v4, ElementsAre(nontrivial(0, 1), nontrivial(8, 9)));
  ASSERT_EQ(v4.size(), 2);

  AFTER_CALL(v3, v3.clear(), display_int);
  ASSERT_THAT(v3, ElementsAre());

  AFTER_CALL(v4, v4.clear(), display_obj);
  ASSERT_THAT(v4, ElementsAre());
  ASSERT_EQ(v4.size(), 0);
}

TEST_F(VectorTest, Swap) {
  AFTER_CALL(v1, v1.swap(v3), display_int);
  ASSERT_THAT(v1, ElementsAre(1, 2, 3, 4, 5, 6, 7, 8));

  AFTER_CALL(v2, v2.swap(v4), display_obj);
  ASSERT_THAT(v2,
              ElementsAre(nontrivial(0, 1), nontrivial(2, 3), nontrivial(4, 5), nontrivial(6, 7), nontrivial(8, 9)));
  ASSERT_EQ(v2.size(), 5);
  ASSERT_EQ(v4.size(), 0);
}

TEST_F(VectorTest, Operator) {
  AFTER_CALL(v1, v1.assign({1, 2, 3, 4, 5, 6, 7, 8}), display_int);
  ASSERT_TRUE(v1 == v3);
  ASSERT_TRUE(v1 >= v3);
  ASSERT_TRUE(v1 <= v3);

  AFTER_CALL(v1, v1.assign({1, 2, 3, 4, 5, 6, 8, 8}), display_int);
  ASSERT_TRUE(v1 != v3);
  ASSERT_TRUE(v1 > v3);
  ASSERT_TRUE(v1 >= v3);

  AFTER_CALL(v2, v2.assign({nontrivial(0, 1), nontrivial(2, 3), nontrivial(4, 5), nontrivial(6, 7), nontrivial(8, 9)}),
             display_obj);
  ASSERT_TRUE(v2 == v4);

  AFTER_CALL(v2, v2.assign({nontrivial(0, 1), nontrivial(2, 3), nontrivial(5, 5), nontrivial(6, 7), nontrivial(8, 9)}),
             display_obj);
  ASSERT_TRUE(!(v2 == v4));
}

TEST(VecPerformTest, Performance) {
  std::vector<int> std_vec1;
  gd::vector<int>  my_vec1;

  std::vector<nontrivial> std_vec2;
  gd::vector<nontrivial>  my_vec2;

  PERFORM_TEST(std_vec1.push_back(6), 100000000);
  PERFORM_TEST(my_vec1.push_back(6), 100000000);

  PERFORM_TEST(std_vec2.push_back(8), 10000000);
  PERFORM_TEST(my_vec2.push_back(8), 10000000);
}

}  // namespace gd

#endif  // !__TEST_VECTOR_H