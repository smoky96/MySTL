#ifndef __TEST_LIST_H
#define __TEST_LIST_H

#include <algorithm>
#include <initializer_list>
#include <iostream>
#include <iterator>
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "my_alloc.h"
#include "my_list.h"
#include "test_helper.h"

namespace gd {

using testing::ElementsAre;

TEST(ListInitialTest, Init) {
  int a[] = {1, 2, 3, 4, 5, 6};

  list<int> l1;
  ASSERT_EQ(l1.size(), 0);

  list<int> l2(6);
  ASSERT_EQ(l2.size(), 6);
  ASSERT_THAT(l2, ElementsAre(0, 0, 0, 0, 0, 0));

  list<int> l3(size_t(6), 8);
  ASSERT_EQ(l3.size(), 6);
  ASSERT_THAT(l3, ElementsAre(8, 8, 8, 8, 8, 8));

  list<int> l4(std::begin(a), std::end(a));
  ASSERT_EQ(l4.size(), 6);
  ASSERT_THAT(l4, ElementsAre(1, 2, 3, 4, 5, 6));

  list<int> l5(l4);
  ASSERT_EQ(l5.size(), 6);
  ASSERT_THAT(l5, ElementsAre(1, 2, 3, 4, 5, 6));

  list<int> l6(std::move(l4));
  ASSERT_EQ(l6.size(), 6);
  ASSERT_THAT(l6, ElementsAre(1, 2, 3, 4, 5, 6));

  list<int> l7({1, 2, 3, 4, 5, 6, 7, 8});
  ASSERT_EQ(l7.size(), 8);
  ASSERT_THAT(l7, ElementsAre(1, 2, 3, 4, 5, 6, 7, 8));

  list<int> l8;
  l8 = l7;
  ASSERT_EQ(l8.size(), 8);
  ASSERT_THAT(l8, ElementsAre(1, 2, 3, 4, 5, 6, 7, 8));

  list<int> l9;
  l9 = std::move(l7);
  ASSERT_EQ(l9.size(), 8);
  ASSERT_THAT(l9, ElementsAre(1, 2, 3, 4, 5, 6, 7, 8));

  list<int> l10;
  l10 = {1, 2, 3, 4, 5, 6, 7, 8};
  ASSERT_EQ(l10.size(), 8);
  ASSERT_THAT(l10, ElementsAre(1, 2, 3, 4, 5, 6, 7, 8));

  l10.assign(std::begin(a), std::end(a));
  ASSERT_EQ(l10.size(), 6);
  ASSERT_THAT(l10, ElementsAre(1, 2, 3, 4, 5, 6));

  l10.assign(size_t(8), 8);
  ASSERT_EQ(l10.size(), 8);
  ASSERT_THAT(l10, ElementsAre(8, 8, 8, 8, 8, 8, 8, 8));

  l10.assign({1, 2, 3, 4, 5, 6});
  ASSERT_EQ(l10.size(), 6);
  ASSERT_THAT(l10, ElementsAre(1, 2, 3, 4, 5, 6));
}

class ListTest : public testing::Test {
 protected:
  // list<int, alloc>        l1;
  // list<nontrivial, alloc> l2;

  // list<int, alloc>        l3;
  // list<nontrivial, alloc> l4;

  list<int, malloc_alloc>        l1;
  list<nontrivial, malloc_alloc> l2;

  list<int, malloc_alloc>        l3;
  list<nontrivial, malloc_alloc> l4;

  list<int>::iterator        it1;
  list<nontrivial>::iterator it2;

  list<int>::iterator        it3;
  list<nontrivial>::iterator it4;

  ListTest()
      : l3({1, 2, 3, 4, 5, 6, 7, 8}),
        l4({
            nontrivial(0, 1),
            nontrivial(2, 3),
            nontrivial(4, 5),
            nontrivial(6, 7),
            nontrivial(8, 9),
        }) {
    std::cout << "- Initial: " << std::endl;
    std::cout << "- l1: null" << std::endl;
    std::cout << "- l2: null" << std::endl;

    std::cout << "- l3: ";
    std::for_each(l3.begin(), l3.end(), display_int);
    std::cout << std::endl;

    std::cout << "- l4: ";
    std::for_each(l4.begin(), l4.end(), display_obj);
    std::cout << std::endl;
  }
};

TEST_F(ListTest, Iterator) {
  ASSERT_TRUE(l1.begin() == l1.end());
  ASSERT_TRUE(l1.cbegin() == l1.cend());

  ASSERT_TRUE(l2.begin() == l2.end());
  ASSERT_TRUE(l2.cbegin() == l2.cend());

  int count;

  it3 = l3.begin();
  count = 0;
  while (it3 != l3.end()) {
    ++it3;
    ++count;
  }
  ASSERT_EQ(8, count);

  count = 0;
  it4 = l4.begin();
  while (it4 != l4.end()) {
    ++it4;
    ++count;
  }
  ASSERT_EQ(5, count);
}

TEST_F(ListTest, Capacity) {
  ASSERT_TRUE(l1.empty());
  ASSERT_TRUE(l2.empty());

  ASSERT_FALSE(l3.empty());
  ASSERT_FALSE(l4.empty());

  ASSERT_EQ(0, l1.size());
  ASSERT_EQ(0, l2.size());

  ASSERT_EQ(8, l3.size());
  ASSERT_EQ(5, l4.size());

  std::cout << "- Max size of list<int> on this machine: " << l1.max_size() << std::endl;

  AFTER_CALL(l1, l1.resize(6, 6), display_int);
  ASSERT_THAT(l1, ElementsAre(6, 6, 6, 6, 6, 6));

  AFTER_CALL(l2, l2.resize(5), display_obj);
  ASSERT_THAT(l2, ElementsAre(nontrivial(), nontrivial(), nontrivial(), nontrivial(), nontrivial()));

  AFTER_CALL(l3, l3.resize(5), display_int);
  ASSERT_THAT(l3, ElementsAre(1, 2, 3, 4, 5));

  AFTER_CALL(l4, l4.resize(4), display_obj);
  ASSERT_THAT(l4, ElementsAre(nontrivial(0, 1), nontrivial(2, 3), nontrivial(4, 5), nontrivial(6, 7)));
  ASSERT_EQ(l4.size(), 4);
}

TEST_F(ListTest, ElemAccess) {
  ASSERT_EQ(l3.front(), 1);
  ASSERT_EQ(l4.front(), nontrivial(0, 1));

  ASSERT_EQ(l3.back(), 8);
  ASSERT_EQ(l4.back(), nontrivial(8, 9));
}

TEST_F(ListTest, EmplaceAndPush) {
  AFTER_CALL(l1, l1.emplace_front(0), display_int);
  ASSERT_THAT(l1, ElementsAre(0));

  AFTER_CALL(l1, l1.pop_front(), display_int);
  AFTER_CALL(l1, l1.emplace_back(0), display_int);
  ASSERT_THAT(l1, ElementsAre(0));

  AFTER_CALL(l2, l2.emplace_front(0, 1), display_obj);
  ASSERT_THAT(l2, ElementsAre(nontrivial(0, 1)));

  AFTER_CALL(l2, l2.pop_back(), display_obj);
  AFTER_CALL(l2, l2.emplace_back(0, 1), display_obj);
  ASSERT_THAT(l2, ElementsAre(nontrivial(0, 1)));

  AFTER_CALL(l3, l3.emplace_front(0), display_int);
  ASSERT_THAT(l3, ElementsAre(0, 1, 2, 3, 4, 5, 6, 7, 8));
  AFTER_CALL(l4, l4.emplace_front(-1, 0), display_obj);
  ASSERT_THAT(l4, ElementsAre(nontrivial(-1, 0), nontrivial(0, 1), nontrivial(2, 3), nontrivial(4, 5), nontrivial(6, 7),
                              nontrivial(8, 9)));
  ASSERT_EQ(l4.size(), 6);

  AFTER_CALL(l3, l3.emplace_back(9), display_int);
  ASSERT_THAT(l3, ElementsAre(0, 1, 2, 3, 4, 5, 6, 7, 8, 9));
  AFTER_CALL(l4, l4.emplace_back(10, 11), display_obj);
  ASSERT_THAT(l4, ElementsAre(nontrivial(-1, 0), nontrivial(0, 1), nontrivial(2, 3), nontrivial(4, 5), nontrivial(6, 7),
                              nontrivial(8, 9), nontrivial(10, 11)));
  ASSERT_EQ(l4.size(), 7);

  AFTER_CALL(l3, l3.push_front(-1), display_int);
  ASSERT_THAT(l3, ElementsAre(-1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9));
  AFTER_CALL(l4, l4.push_front(nontrivial(-3, -2)), display_obj);
  ASSERT_THAT(l4, ElementsAre(nontrivial(-3, -2), nontrivial(-1, 0), nontrivial(0, 1), nontrivial(2, 3),
                              nontrivial(4, 5), nontrivial(6, 7), nontrivial(8, 9), nontrivial(10, 11)));
  ASSERT_EQ(l4.size(), 8);

  AFTER_CALL(l3, l3.push_back(10), display_int);
  ASSERT_THAT(l3, ElementsAre(-1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10));
  AFTER_CALL(l4, l4.push_back(nontrivial(12, 13)), display_obj);
  ASSERT_THAT(l4,
              ElementsAre(nontrivial(-3, -2), nontrivial(-1, 0), nontrivial(0, 1), nontrivial(2, 3), nontrivial(4, 5),
                          nontrivial(6, 7), nontrivial(8, 9), nontrivial(10, 11), nontrivial(12, 13)));
  ASSERT_EQ(l4.size(), 9);

  it3 = l3.begin();
  advance(it3, 6);
  AFTER_CALL(l3, l3.emplace(it3, 11), display_int);
  ASSERT_THAT(l3, ElementsAre(-1, 0, 1, 2, 3, 4, 11, 5, 6, 7, 8, 9, 10));

  it4 = l4.begin();
  advance(it4, 6);
  AFTER_CALL(l4, l4.emplace(it4, 14, 15), display_obj);
  ASSERT_THAT(
      l4, ElementsAre(nontrivial(-3, -2), nontrivial(-1, 0), nontrivial(0, 1), nontrivial(2, 3), nontrivial(4, 5),
                      nontrivial(6, 7), nontrivial(14, 15), nontrivial(8, 9), nontrivial(10, 11), nontrivial(12, 13)));
  ASSERT_EQ(l4.size(), 10);
}

TEST_F(ListTest, Insert) {
  it3 = l3.begin();
  advance(it3, 4);
  AFTER_CALL(l3, l3.insert(it3, 9), display_int);
  ASSERT_THAT(l3, ElementsAre(1, 2, 3, 4, 9, 5, 6, 7, 8));

  it4 = l4.begin();
  advance(it4, 3);
  AFTER_CALL(l4, l4.insert(it4, nontrivial(10, 11)), display_obj);
  ASSERT_THAT(l4, ElementsAre(nontrivial(0, 1), nontrivial(2, 3), nontrivial(4, 5), nontrivial(10, 11),
                              nontrivial(6, 7), nontrivial(8, 9)));
  ASSERT_EQ(l4.size(), 6);

  it3 = l3.begin();
  advance(it3, 4);
  AFTER_CALL(l3, l3.insert(it3, size_t(2), 10), display_int);
  ASSERT_THAT(l3, ElementsAre(1, 2, 3, 4, 10, 10, 9, 5, 6, 7, 8));

  it4 = l4.begin();
  advance(it4, 3);
  AFTER_CALL(l4, l4.insert(it4, 2, nontrivial(12, 13)), display_obj);
  ASSERT_THAT(l4, ElementsAre(nontrivial(0, 1), nontrivial(2, 3), nontrivial(4, 5), nontrivial(12, 13),
                              nontrivial(12, 13), nontrivial(10, 11), nontrivial(6, 7), nontrivial(8, 9)));
  ASSERT_EQ(l4.size(), 8);

  l1.insert(l1.end(), size_t(2), 11);
  it3 = l3.begin();
  advance(it3, 4);
  AFTER_CALL(l3, l3.insert(it3, l1.begin(), l1.end()), display_int);
  ASSERT_THAT(l3, ElementsAre(1, 2, 3, 4, 11, 11, 10, 10, 9, 5, 6, 7, 8));

  l2.insert(l2.end(), 2, nontrivial(14, 15));
  it4 = l4.begin();
  advance(it4, 3);
  AFTER_CALL(l4, l4.insert(it4, l2.begin(), l2.end()), display_obj);
  ASSERT_THAT(
      l4, ElementsAre(nontrivial(0, 1), nontrivial(2, 3), nontrivial(4, 5), nontrivial(14, 15), nontrivial(14, 15),
                      nontrivial(12, 13), nontrivial(12, 13), nontrivial(10, 11), nontrivial(6, 7), nontrivial(8, 9)));
  ASSERT_EQ(l4.size(), 10);

  it3 = l3.begin();
  advance(it3, 4);
  AFTER_CALL(l3, l3.insert(it3, std::initializer_list<int>{12, 13, 14, 15}), display_int);
  ASSERT_THAT(l3, ElementsAre(1, 2, 3, 4, 12, 13, 14, 15, 11, 11, 10, 10, 9, 5, 6, 7, 8));

  it4 = l4.begin();
  advance(it4, 3);
  AFTER_CALL(l4, l4.insert(it4, std::initializer_list<nontrivial>{16, 17, 18}), display_obj);
  ASSERT_THAT(
      l4, ElementsAre(nontrivial(0, 1), nontrivial(2, 3), nontrivial(4, 5), nontrivial(16, 0), nontrivial(17, 0),
                      nontrivial(18, 0), nontrivial(14, 15), nontrivial(14, 15), nontrivial(12, 13), nontrivial(12, 13),
                      nontrivial(10, 11), nontrivial(6, 7), nontrivial(8, 9)));
  ASSERT_EQ(l4.size(), 13);
}

TEST_F(ListTest, Erase) {
  it3 = l3.begin();
  advance(it3, 4);
  AFTER_CALL(l3, l3.erase(it3), display_int);
  ASSERT_THAT(l3, ElementsAre(1, 2, 3, 4, 6, 7, 8));

  it4 = l4.begin();
  advance(it4, 2);
  AFTER_CALL(l4, l4.erase(it4), display_obj);
  ASSERT_THAT(l4, ElementsAre(nontrivial(0, 1), nontrivial(2, 3), nontrivial(6, 7), nontrivial(8, 9)));
  ASSERT_EQ(l4.size(), 4);

  it3 = l3.begin();
  advance(it3, 3);
  AFTER_CALL(l3, l3.erase(++l3.begin(), it3), display_int);
  ASSERT_THAT(l3, ElementsAre(1, 4, 6, 7, 8));

  it4 = l4.begin();
  advance(it4, 3);
  AFTER_CALL(l4, l4.erase(++l4.begin(), it4), display_obj);
  ASSERT_THAT(l4, ElementsAre(nontrivial(0, 1), nontrivial(8, 9)));
  ASSERT_EQ(l4.size(), 2);

  AFTER_CALL(l3, l3.clear(), display_int);
  ASSERT_THAT(l3, ElementsAre());

  AFTER_CALL(l4, l4.clear(), display_obj);
  ASSERT_THAT(l4, ElementsAre());
  ASSERT_EQ(l4.size(), 0);
}

TEST_F(ListTest, Splice) {
  AFTER_CALL(l1, l1.insert(l1.begin(), {6, 7, 8, 9}), display_int);
  AFTER_CALL(l2, l2.insert(l2.begin(), {6, 7, 8, 9}), display_obj);

  AFTER_CALL(l3, l3.splice(++++l3.begin(), l1), display_int);
  ASSERT_THAT(l3, ElementsAre(1, 2, 6, 7, 8, 9, 3, 4, 5, 6, 7, 8));
  ASSERT_THAT(l1, ElementsAre());

  AFTER_CALL(l4, l4.splice(++++l4.begin(), l2), display_obj);
  ASSERT_THAT(l4, ElementsAre(nontrivial(0, 1), nontrivial(2, 3), nontrivial(6, 0), nontrivial(7, 0), nontrivial(8, 0),
                              nontrivial(9, 0), nontrivial(4, 5), nontrivial(6, 7), nontrivial(8, 9)));
  ASSERT_THAT(l2, ElementsAre());
  ASSERT_EQ(l2.size(), 0);
  ASSERT_EQ(l4.size(), 9);

  AFTER_CALL(l1, l1.insert(l1.begin(), {6, 7, 8, 9}), display_int);
  AFTER_CALL(l2, l2.insert(l2.begin(), {6, 7, 8, 9}), display_obj);

  AFTER_CALL(l3, l3.splice(l3.begin(), l1, l1.begin()), display_int);
  ASSERT_THAT(l3, ElementsAre(6, 1, 2, 6, 7, 8, 9, 3, 4, 5, 6, 7, 8));
  ASSERT_THAT(l1, ElementsAre(7, 8, 9));

  AFTER_CALL(l4, l4.splice(l4.begin(), l2, l2.begin()), display_obj);
  ASSERT_THAT(l4,
              ElementsAre(nontrivial(6, 0), nontrivial(0, 1), nontrivial(2, 3), nontrivial(6, 0), nontrivial(7, 0),
                          nontrivial(8, 0), nontrivial(9, 0), nontrivial(4, 5), nontrivial(6, 7), nontrivial(8, 9)));
  ASSERT_THAT(l2, ElementsAre(nontrivial(7, 0), nontrivial(8, 0), nontrivial(9, 0)));
  ASSERT_EQ(l2.size(), 3);
  ASSERT_EQ(l4.size(), 10);

  AFTER_CALL(l3, l3.splice(l3.end(), l1, l1.begin(), ++++l1.begin()), display_int);
  ASSERT_THAT(l3, ElementsAre(6, 1, 2, 6, 7, 8, 9, 3, 4, 5, 6, 7, 8, 7, 8));
  ASSERT_THAT(l1, ElementsAre(9));

  AFTER_CALL(l4, l4.splice(l4.end(), l2, l2.begin(), ++++l2.begin()), display_obj);
  ASSERT_THAT(l4, ElementsAre(nontrivial(6, 0), nontrivial(0, 1), nontrivial(2, 3), nontrivial(6, 0), nontrivial(7, 0),
                              nontrivial(8, 0), nontrivial(9, 0), nontrivial(4, 5), nontrivial(6, 7), nontrivial(8, 9),
                              nontrivial(7, 0), nontrivial(8, 0)));
  ASSERT_THAT(l2, ElementsAre(nontrivial(9, 0)));
  ASSERT_EQ(l2.size(), 1);
  ASSERT_EQ(l4.size(), 12);
}

TEST_F(ListTest, Remove) {
  AFTER_CALL(l3, l3.remove(5), display_int);
  ASSERT_THAT(l3, ElementsAre(1, 2, 3, 4, 6, 7, 8));
  ASSERT_EQ(l3.size(), 7);

  AFTER_CALL(l4, l4.remove(nontrivial(4, 5)), display_obj);
  ASSERT_THAT(l4, ElementsAre(nontrivial(0, 1), nontrivial(2, 3), nontrivial(6, 7), nontrivial(8, 9)));

  AFTER_CALL(l3, l3.remove_if([&](int x) { return x % 2; }), display_int);
  ASSERT_THAT(l3, ElementsAre(2, 4, 6, 8));
  ASSERT_EQ(l3.size(), 4);

  AFTER_CALL(l4, l4.remove_if([&](nontrivial x) { return x.j % 2; }), display_obj);
  ASSERT_THAT(l4, ElementsAre());
}

TEST_F(ListTest, Unique) {
  AFTER_CALL(l3, l3.insert(++l3.begin(), size_t(5), 2), display_int);
  AFTER_CALL(l3, l3.unique(), display_int);
  ASSERT_THAT(l3, ElementsAre(1, 2, 3, 4, 5, 6, 7, 8));

  AFTER_CALL(l4, l4.insert(----l4.end(), 5, nontrivial(6, 7)), display_obj);
  AFTER_CALL(l4, l4.unique(), display_obj);
  ASSERT_THAT(l4,
              ElementsAre(nontrivial(0, 1), nontrivial(2, 3), nontrivial(4, 5), nontrivial(6, 7), nontrivial(8, 9)));
  ASSERT_EQ(l4.size(), 5);

  AFTER_CALL(l3, l3.unique([&](int a, int b) { return a + 1 == b; }), display_int);
  ASSERT_THAT(l3, ElementsAre(1, 3, 5, 7));

  AFTER_CALL(l4, l4.unique([&](nontrivial a, nontrivial b) { return a.j + 2 == b.j; }), display_obj);
  ASSERT_THAT(l4, ElementsAre(nontrivial(0, 1), nontrivial(4, 5), nontrivial(8, 9)));
  ASSERT_EQ(l4.size(), 3);
}

TEST_F(ListTest, Merge) {
  AFTER_CALL(l1, l1.insert(l1.begin(), {6, 7, 8, 9}), display_int);
  AFTER_CALL(l2, l2.insert(l2.begin(), {6, 7, 8, 9}), display_obj);

  AFTER_CALL(l3, l3.merge(l1), display_int);
  ASSERT_THAT(l3, ElementsAre(1, 2, 3, 4, 5, 6, 6, 7, 7, 8, 8, 9));
  ASSERT_EQ(l3.size(), 12);

  AFTER_CALL(l4, l4.merge(l2, [&](nontrivial a, nontrivial b) { return *a.i < *b.i; }), display_obj);
  ASSERT_THAT(l4, ElementsAre(nontrivial(0, 1), nontrivial(2, 3), nontrivial(4, 5), nontrivial(6, 7), nontrivial(6, 0),
                              nontrivial(7, 0), nontrivial(8, 9), nontrivial(8, 0), nontrivial(9, 0)));
  ASSERT_EQ(l4.size(), 9);
}

TEST_F(ListTest, Sort) {
  AFTER_CALL(l1, l1.insert(l1.begin(), {3, 2, 5, 7, 2, 4, 8, 1, 12, 6}), display_int);
  AFTER_CALL(l2, l2.insert(l2.begin(), {9, 2, 8, 1, 12, 6}), display_obj);

  AFTER_CALL(l1, l1.sort(), display_int);
  ASSERT_THAT(l1, ElementsAre(1, 2, 2, 3, 4, 5, 6, 7, 8, 12));
  ASSERT_EQ(l1.size(), 10);

  AFTER_CALL(l2, l2.sort([&](nontrivial a, nontrivial b) { return *a.i < *b.i; }), display_obj);
  ASSERT_THAT(l2, ElementsAre(nontrivial(1, 0), nontrivial(2, 0), nontrivial(6, 0), nontrivial(8, 0), nontrivial(9, 0),
                              nontrivial(12, 0)));
  ASSERT_EQ(l2.size(), 6);
}

TEST_F(ListTest, Reverse) {
  AFTER_CALL(l3, l3.reverse(), display_int);
  ASSERT_THAT(l3, ElementsAre(8, 7, 6, 5, 4, 3, 2, 1));
  ASSERT_EQ(l3.size(), 8);

  AFTER_CALL(l4, l4.reverse(), display_obj);
  ASSERT_THAT(l4,
              ElementsAre(nontrivial(8, 9), nontrivial(6, 7), nontrivial(4, 5), nontrivial(2, 3), nontrivial(0, 1)));
  ASSERT_EQ(l4.size(), 5);
}

TEST_F(ListTest, Operator) {
  AFTER_CALL(l1, l1.assign({1, 2, 3, 4, 5, 6, 7, 8}), display_int);
  ASSERT_TRUE(l1 == l3);
  ASSERT_TRUE(l1 >= l3);
  ASSERT_TRUE(l1 <= l3);

  AFTER_CALL(l1, l1.assign({1, 2, 3, 4, 5, 6, 8, 8}), display_int);
  ASSERT_TRUE(l1 != l3);
  ASSERT_TRUE(l1 > l3);
  ASSERT_TRUE(l1 >= l3);

  AFTER_CALL(l2, l2.assign({nontrivial(0, 1), nontrivial(2, 3), nontrivial(4, 5), nontrivial(6, 7), nontrivial(8, 9)}),
             display_obj);
  ASSERT_TRUE(l2 == l4);

  AFTER_CALL(l2, l2.assign({nontrivial(0, 1), nontrivial(2, 3), nontrivial(5, 5), nontrivial(6, 7), nontrivial(8, 9)}),
             display_obj);
  ASSERT_TRUE(!(l2 == l4));
}

}  // namespace gd

#endif  // !__TEST_LIST_H