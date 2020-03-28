#ifndef __TEST_TREE__H
#define __TEST_TREE__H

#include <algorithm>
#include <set>
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "my_tree.h"
#include "my_vector.h"
#include "test_helper.h"

namespace gd {

template <typename _Tp>
struct identity {
  _Tp& operator()(_Tp& __x) const {
    return __x;
  }

  const _Tp& operator()(const _Tp& __x) const {
    return __x;
  }
};

vector<int>   v = {93, 86, 47, 60, 38, 30, 35, 75, 68, 2,  75, 60, 37, 86, 45, 26, 60, 5,  85, 83, 21, 51, 61, 48, 35,
                 41, 98, 60, 84, 28, 91, 79, 50, 61, 57, 25, 18, 73, 88, 88, 73, 95, 13, 66, 33, 63, 26, 24, 56, 12,
                 83, 7,  55, 49, 80, 47, 44, 47, 80, 31, 76, 53, 89, 49, 7,  78, 63, 71, 51, 21, 36, 95, 94, 79, 38,
                 99, 79, 87, 80, 94, 69, 16, 83, 8,  56, 70, 62, 91, 71, 21, 76, 27, 66, 19, 29, 21, 15, 42, 15, 16};
std::set<int> s(v.begin(), v.end());

class RbTreeTest : public testing::Test {
 protected:
  rb_tree<int, int, identity<int>, std::less<int>> u_tree;
  rb_tree<int, int, identity<int>, std::less<int>> m_tree;

  RbTreeTest() {
    for (const int& i : v)
      u_tree.insert_unique(i);
    for (const int& i : v)
      m_tree.insert_equal(i);
  }
};

TEST_F(RbTreeTest, Init) {
  rb_tree<int, int, identity<int>, std::less<int>> tmp1 = u_tree;
  CPRINT(tmp1, display_int);
  rb_tree<int, int, identity<int>, std::less<int>> tmp2 = std::move(u_tree);
  CPRINT(tmp2, display_int);
  rb_tree<int, int, identity<int>, std::less<int>> tmp3(m_tree);
  CPRINT(tmp3, display_int);
  rb_tree<int, int, identity<int>, std::less<int>> tmp4(std::move(m_tree));
  CPRINT(tmp4, display_int);
}

TEST_F(RbTreeTest, Capacity) {
  rb_tree<int, int, identity<int>, std::less<int>> tmp1;
  ASSERT_TRUE(tmp1.empty());
  ASSERT_FALSE(u_tree.empty());

  ASSERT_EQ(u_tree.size(), s.size());
}

TEST_F(RbTreeTest, Emplace) {
  std::pair<decltype(u_tree.begin()), bool> ret;

  AFTER_CALL(u_tree, ret = u_tree.emplace_unique(25), display_int);
  ASSERT_FALSE(ret.second);
  ASSERT_EQ(*(ret.first), 25);
  ASSERT_EQ(u_tree.size(), s.size());
  AFTER_CALL(u_tree, ret = u_tree.emplace_unique(32), display_int);
  ASSERT_TRUE(ret.second);
  ASSERT_EQ(*(ret.first), 32);
  ASSERT_EQ(u_tree.size(), s.size() + 1);

  decltype(m_tree.begin()) it;
  AFTER_CALL(m_tree, it = m_tree.emplace_equal(16), display_int);
  ASSERT_EQ(*++it, 18);
  ASSERT_EQ(m_tree.size(), 101);
}

TEST_F(RbTreeTest, Insert) {
  std::pair<decltype(u_tree.begin()), bool> ret;

  AFTER_CALL(u_tree, ret = u_tree.insert_unique(25), display_int);
  ASSERT_FALSE(ret.second);
  ASSERT_EQ(*(ret.first), 25);
  ASSERT_EQ(u_tree.size(), s.size());

  AFTER_CALL(u_tree, ret = u_tree.insert_unique(32), display_int);
  ASSERT_TRUE(ret.second);
  ASSERT_EQ(*(ret.first), 32);
  ASSERT_EQ(u_tree.size(), s.size() + 1);

  vector<int> v{5, 6, 7, 8};
  AFTER_CALL(u_tree, u_tree.insert_unique(v.begin(), v.end()), display_int);
  ASSERT_EQ(u_tree.size(), s.size() + 2);

  decltype(m_tree.begin()) it;
  AFTER_CALL(m_tree, it = m_tree.insert_equal(16), display_int);
  ASSERT_EQ(*++it, 18);
  ASSERT_EQ(m_tree.size(), 101);

  AFTER_CALL(m_tree, m_tree.insert_equal(v.begin(), v.end()), display_int);
  ASSERT_EQ(m_tree.size(), 105);
}

TEST_F(RbTreeTest, Erase) {
  auto it1 = u_tree.begin();
  auto it2 = u_tree.begin();
  advance(it1, 10);
  advance(it2, 20);

  AFTER_CALL(u_tree, u_tree.erase(it1), display_int);
  ASSERT_EQ(u_tree.size(), s.size() - 1);

  it1 = u_tree.begin();
  it2 = u_tree.begin();
  advance(it1, 10);
  advance(it2, 20);
  std::cout << *it1 << std::endl;
  std::cout << *it2 << std::endl;
  AFTER_CALL(u_tree, u_tree.erase(it1, it2), display_int);
  ASSERT_EQ(u_tree.size(), s.size() - 11);

  AFTER_CALL(m_tree, m_tree.erase(21), display_int);
  ASSERT_EQ(m_tree.size(), 96);
}

TEST_F(RbTreeTest, SetOp) {
  auto it = u_tree.find(29);
  ASSERT_EQ(*it, 29);

  it = m_tree.find(21);
  ASSERT_EQ(*(--it), 19);

  ASSERT_EQ(u_tree.count(21), 1);
  ASSERT_EQ(m_tree.count(21), 4);
  ASSERT_EQ(m_tree.count(6), 0);

  auto p = u_tree.equal_range(60);
  ASSERT_EQ(distance(p.first, p.second), 1);

  p = m_tree.equal_range(60);
  ASSERT_EQ(distance(p.first, p.second), 4);

  p = m_tree.equal_range(20);
  ASSERT_EQ(p.first, p.second);
}

TEST_F(RbTreeTest, Operator) {
  auto tmp1 = u_tree;
  auto tmp2 = m_tree;
  ASSERT_TRUE(tmp1 == u_tree);
  ASSERT_TRUE(tmp2 != u_tree);

  AFTER_CALL(tmp1, tmp1.erase(5), display_int);
  ASSERT_TRUE(u_tree < tmp1);
}

}  // namespace gd

#endif  //!__TEST_TREE__H