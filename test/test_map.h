#ifndef __TEST_MAP__H
#define __TEST_MAP__H

#include <map>
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "my_map.h"
#include "my_vector.h"
#include "test_helper.h"

namespace gd {
namespace test_map {

vector<int> v = {93, 86, 47, 60, 38, 30, 35, 75, 68, 2,  75, 60, 37, 86, 45, 26, 60, 5,  85, 83, 21, 51, 61, 48, 35,
                 41, 98, 60, 84, 28, 91, 79, 50, 61, 57, 25, 18, 73, 88, 88, 73, 95, 13, 66, 33, 63, 26, 24, 56, 12,
                 83, 7,  55, 49, 80, 47, 44, 47, 80, 31, 76, 53, 89, 49, 7,  78, 63, 71, 51, 21, 36, 95, 94, 79, 38,
                 99, 79, 87, 80, 94, 69, 16, 83, 8,  56, 70, 62, 91, 71, 21, 76, 27, 66, 19, 29, 21, 15, 42, 15, 16};

class MapTest : public testing::Test {
 protected:
  map<int, bool>      m;
  multimap<int, bool> mm;

  std::map<int, bool>      sm;
  std::multimap<int, bool> smm;

  // vector<std::pair<int, bool>> kv;
  display_map<int, bool> disp_m;

  MapTest() : m(), mm(), sm(), smm() {
    for (const int& i : v) {
      bool value = true;
      if (i % 2) {
        value = false;
      }
      // kv.push_back({i, value});
      m.insert({i, value});
      mm.insert({i, value});
      sm.insert({i, value});
      smm.insert({i, value});
    }
  }
};

TEST_F(MapTest, Init) {
  // map<int, bool> tmp1(kv.begin(), kv.end());
  map<int, bool> tmp1(m);
  CPRINT(tmp1, disp_m);

  map<int, bool> tmp2(tmp1);
  CPRINT(tmp2, disp_m);

  map<int, bool> tmp3(std::move(tmp1));
  CPRINT(tmp3, disp_m);

  map<int, bool> tmp4 = tmp2;
  CPRINT(tmp4, disp_m);

  map<int, bool> tmp5 = std::move(tmp2);
  CPRINT(tmp5, disp_m);

  // multimap<int, bool> tmp6(kv.begin(), kv.end());
  multimap<int, bool> tmp6(mm);
  CPRINT(tmp6, disp_m);

  multimap<int, bool> tmp7(tmp6);
  CPRINT(tmp7, disp_m);

  multimap<int, bool> tmp8(std::move(tmp6));
  CPRINT(tmp8, disp_m);

  multimap<int, bool> tmp9 = tmp7;
  CPRINT(tmp9, disp_m);

  multimap<int, bool> tmp10 = std::move(tmp7);
  CPRINT(tmp10, disp_m);
}

TEST_F(MapTest, Capacity) {
  map<int, bool>      tmp1;
  multimap<int, bool> tmp2;

  ASSERT_TRUE(tmp1.empty());
  ASSERT_TRUE(tmp2.empty());

  ASSERT_FALSE(m.empty());
  ASSERT_FALSE(mm.empty());

  ASSERT_EQ(m.size(), sm.size());
  ASSERT_EQ(mm.size(), smm.size());
}

TEST_F(MapTest, ElemAccess) {
  for (const int& i : v) {
    if (i % 2)
      ASSERT_FALSE(m[i]);
    else
      ASSERT_TRUE(m[i]);
  }
  AFTER_CALL(m, m[3], disp_m);
}

TEST_F(MapTest, Emplace) {
  AFTER_CALL(m, m.emplace(6, true), disp_m);
  ASSERT_EQ(m.size(), sm.size() + 1);

  AFTER_CALL(mm, mm.emplace(7, false), disp_m);
  ASSERT_EQ(mm.size(), smm.size() + 1);

  AFTER_CALL(m, m.emplace_hint(m.end(), 100, false), disp_m);
  ASSERT_EQ(m.size(), sm.size() + 2);

  AFTER_CALL(mm, mm.emplace_hint(----m.end(), 96, true), disp_m);
  ASSERT_EQ(mm.size(), smm.size() + 2);
}

TEST_F(MapTest, Insert) {
  AFTER_CALL(m, m.insert({9, false}), disp_m);
  ASSERT_EQ(m.size(), sm.size() + 1);

  AFTER_CALL(m, m.insert(++m.begin(), {3, false}), disp_m);
  ASSERT_EQ(m.size(), sm.size() + 2);

  AFTER_CALL(m, m.insert({3, false}), disp_m);
  ASSERT_EQ(m.size(), sm.size() + 2);

  AFTER_CALL(mm, mm.insert({5, false}), disp_m);
  ASSERT_EQ(mm.size(), smm.size() + 1);

  AFTER_CALL(mm, mm.insert(++mm.begin(), {5, false}), disp_m);
  ASSERT_EQ(mm.size(), smm.size() + 2);
}

TEST_F(MapTest, Erase) {
  AFTER_CALL(m, m.erase(++++m.begin()), disp_m);
  ASSERT_EQ(m.size(), sm.size() - 1);

  AFTER_CALL(m, m.erase(8), disp_m);
  ASSERT_EQ(m.size(), sm.size() - 2);

  auto it1 = m.begin();
  gd::advance(it1, 5);
  auto it2 = m.begin();
  gd::advance(it2, 10);
  AFTER_CALL(m, m.erase(it1, it2), disp_m);
  ASSERT_EQ(m.size(), sm.size() - 7);

  AFTER_CALL(mm, mm.erase(++++mm.begin()), disp_m);
  ASSERT_EQ(mm.size(), smm.size() - 1);

  AFTER_CALL(mm, mm.erase(16), disp_m);
  ASSERT_EQ(mm.size(), smm.size() - 3);

  auto it3 = mm.begin();
  gd::advance(it3, 5);
  auto it4 = mm.begin();
  gd::advance(it4, 10);
  AFTER_CALL(mm, mm.erase(it3, it4), disp_m);
  ASSERT_EQ(mm.size(), smm.size() - 8);

  AFTER_CALL(m, m.clear(), disp_m);
  AFTER_CALL(mm, mm.clear(), disp_m);
  ASSERT_EQ(m.size(), 0);
  ASSERT_EQ(mm.size(), 0);
}

TEST_F(MapTest, SetOp) {
  auto it = m.find(29);
  ASSERT_FALSE(it->second);

  it = mm.find(21);
  ASSERT_FALSE((--it)->second);

  ASSERT_EQ(m.count(21), 1);
  ASSERT_EQ(mm.count(21), 4);
  ASSERT_EQ(mm.count(6), 0);

  auto p = m.equal_range(60);
  ASSERT_EQ(gd::distance(p.first, p.second), 1);

  p = mm.equal_range(60);
  ASSERT_EQ(gd::distance(p.first, p.second), 4);

  p = mm.equal_range(20);
  ASSERT_EQ(p.first, p.second);
}

TEST_F(MapTest, Operator) {
  auto tmp1 = m;
  ASSERT_TRUE(tmp1 == m);

  AFTER_CALL(tmp1, tmp1.erase(5), disp_m);
  ASSERT_TRUE(m < tmp1);
  ASSERT_TRUE(m != tmp1);

  auto tmp2 = mm;
  CPRINT(tmp2, disp_m);
  ASSERT_TRUE(tmp2 == mm);

  AFTER_CALL(tmp2, tmp2.erase(5), disp_m);
  ASSERT_TRUE(mm < tmp2);
  ASSERT_TRUE(mm != tmp2);
}

}  // namespace test_map
}  // namespace gd

#endif  //!__TEST_MAP__H