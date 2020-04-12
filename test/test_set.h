#ifndef __TEST_SET__H
#define __TEST_SET__H

#include <set>
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "my_set.h"
#include "my_vector.h"
#include "test_helper.h"

namespace gd {
namespace test_set {

vector<int>   v = {93, 86, 47, 60, 38, 30, 35, 75, 68, 2,  75, 60, 37, 86, 45, 26, 60, 5,  85, 83, 21, 51, 61, 48, 35,
                 41, 98, 60, 84, 28, 91, 79, 50, 61, 57, 25, 18, 73, 88, 88, 73, 95, 13, 66, 33, 63, 26, 24, 56, 12,
                 83, 7,  55, 49, 80, 47, 44, 47, 80, 31, 76, 53, 89, 49, 7,  78, 63, 71, 51, 21, 36, 95, 94, 79, 38,
                 99, 79, 87, 80, 94, 69, 16, 83, 8,  56, 70, 62, 91, 71, 21, 76, 27, 66, 19, 29, 21, 15, 42, 15, 16};
std::set<int> s(v.begin(), v.end());
std::multiset<int> ms(v.begin(), v.end());

class SetTest : public testing::Test {
 protected:
  set<int>      my_s;
  multiset<int> my_ms;

  SetTest() : my_s(v.begin(), v.end()), my_ms(v.begin(), v.end()) {}
};

TEST_F(SetTest, Init) {
  set<int> tmp1 = my_s;
  CPRINT(tmp1, display_int);
  set<int> tmp2 = std::move(my_s);
  CPRINT(tmp2, display_int);

  multiset<int> tmp3 = my_ms;
  CPRINT(tmp3, display_int);
  multiset<int> tmp4 = std::move(my_ms);
  CPRINT(tmp4, display_int);

  set<int> tmp5;
  tmp5 = std::move(tmp1);
  CPRINT(tmp5, display_int);
  ASSERT_TRUE(tmp1.empty());

  multiset<int> tmp6;
  tmp6 = std::move(tmp3);
  CPRINT(tmp6, display_int);
  ASSERT_TRUE(tmp3.empty());
}

TEST_F(SetTest, Capacity) {
  set<int> tmp1;

  ASSERT_TRUE(tmp1.empty());
  ASSERT_FALSE(my_s.empty());

  ASSERT_EQ(my_s.size(), s.size());
  ASSERT_EQ(my_ms.size(), ms.size());
}

TEST_F(SetTest, Emplace) {
  std::pair<decltype(my_s.begin()), bool> ret;

  AFTER_CALL(my_s, ret = my_s.emplace(25), display_int);
  ASSERT_FALSE(ret.second);
  ASSERT_EQ(*(ret.first), 25);

  AFTER_CALL(my_s, ret = my_s.emplace(32), display_int);
  ASSERT_TRUE(ret.second);
  ASSERT_EQ(*(ret.first), 32);
  ASSERT_EQ(my_s.size(), s.size() + 1);

  decltype(my_ms.begin()) it;
  AFTER_CALL(my_ms, it = my_ms.emplace(16), display_int);
  ASSERT_EQ(*++it, 18);
  ASSERT_EQ(my_ms.size(), 101);
}

TEST_F(SetTest, Insert) {
  std::pair<decltype(my_s.begin()), bool> ret;

  AFTER_CALL(my_s, ret = my_s.insert(25), display_int);
  ASSERT_FALSE(ret.second);
  ASSERT_EQ(*(ret.first), 25);
  ASSERT_EQ(my_s.size(), s.size());

  AFTER_CALL(my_s, ret = my_s.insert(32), display_int);
  ASSERT_TRUE(ret.second);
  ASSERT_EQ(*(ret.first), 32);
  ASSERT_EQ(my_s.size(), s.size() + 1);

  vector<int> v{5, 6, 7, 8};
  AFTER_CALL(my_s, my_s.insert(v.begin(), v.end()), display_int);
  ASSERT_EQ(my_s.size(), s.size() + 2);

  decltype(my_ms.begin()) it;
  AFTER_CALL(my_ms, it = my_ms.insert(16), display_int);
  ASSERT_EQ(*++it, 18);
  ASSERT_EQ(my_ms.size(), 101);

  AFTER_CALL(my_ms, my_ms.insert(v.begin(), v.end()), display_int);
  ASSERT_EQ(my_ms.size(), 105);
}

TEST_F(SetTest, Erase) {
  auto it1 = my_s.begin();
  auto it2 = my_s.begin();

  advance(it1, 10);
  advance(it2, 20);

  AFTER_CALL(my_s, my_s.erase(it1), display_int);
  ASSERT_EQ(my_s.size(), s.size() - 1);

  it1 = my_s.begin();
  it2 = my_s.begin();
  advance(it1, 10);
  advance(it2, 20);
  AFTER_CALL(my_s, my_s.erase(it1, it2), display_int);
  ASSERT_EQ(my_s.size(), s.size() - 11);

  AFTER_CALL(my_ms, my_ms.erase(21), display_int);
  ASSERT_EQ(my_ms.size(), 96);
}

TEST_F(SetTest, Swap) {
  set<int> tmp1(v.begin(), v.end());
  tmp1.insert(0);
  AFTER_CALL(my_s, swap(my_s, tmp1), display_int);
}

TEST_F(SetTest, Clear) {
  AFTER_CALL(my_s, my_s.clear(), display_int);
  ASSERT_TRUE(my_s.empty());
}

TEST_F(SetTest, SetOp) {
  auto it = my_s.find(29);
  ASSERT_EQ(*it, 29);

  it = my_ms.find(21);
  ASSERT_EQ(*(--it), 19);

  ASSERT_EQ(my_s.count(21), 1);
  ASSERT_EQ(my_ms.count(21), 4);
  ASSERT_EQ(my_ms.count(6), 0);

  auto p = my_s.equal_range(60);
  ASSERT_EQ(distance(p.first, p.second), 1);

  p = my_ms.equal_range(60);
  ASSERT_EQ(distance(p.first, p.second), 4);

  p = my_ms.equal_range(20);
  ASSERT_EQ(p.first, p.second);
}

TEST_F(SetTest, Operator) {
  auto tmp1 = my_s;
  ASSERT_TRUE(tmp1 == my_s);

  AFTER_CALL(tmp1, tmp1.erase(5), display_int);
  ASSERT_TRUE(my_s < tmp1);
  ASSERT_TRUE(my_s != tmp1);

  auto tmp2 = my_ms;
  CPRINT(tmp2, display_int);
  ASSERT_TRUE(tmp2 == my_ms);

  AFTER_CALL(tmp2, tmp2.erase(5), display_int);
  ASSERT_TRUE(my_ms < tmp2);
  ASSERT_TRUE(my_ms != tmp2);
}

}  // namespace test_set
}  // namespace gd

#endif  //!__TEST_SET__H