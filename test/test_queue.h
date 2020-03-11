#ifndef __TEST_QUEUE__H
#define __TEST_QUEUE__H

#include <algorithm>
#include <ctime>
#include <initializer_list>
#include <iostream>
#include <iterator>
#include <queue>
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "my_alloc.h"
#include "my_deque.h"
#include "my_queue.h"
#include "my_vector.h"
#include "test_helper.h"

namespace gd {

template <typename T, typename Display>
void queue_print(queue<T> q, Display f) {
  while (!q.empty()) {
    f(q.front());
    q.pop();
  }
  std::cout << std::endl;
}

template <typename PrioQue, typename Display>
void prio_queue_print(PrioQue pq, Display f) {
  while (!pq.empty()) {
    f(pq.top());
    pq.pop();
  }
  std::cout << std::endl;
}

#define QUEUE_PRINT(q, display_fun)     \
  do {                                  \
    std::string qname = #q;             \
    std::cout << "- " << qname << ": "; \
    queue_print(q, display_fun);        \
  } while (0)

#define P_QUEUE_PRINT(pq, display_fun)   \
  do {                                   \
    std::string pqname = #pq;            \
    std::cout << "- " << pqname << ": "; \
    prio_queue_print(pq, display_fun);   \
  } while (0)

#define QUEUE_CALL(q, fun, display_fun, assert_sz)         \
  do {                                                     \
    std::string fname = #fun;                              \
    std::cout << "- After called: " << fname << std::endl; \
    fun;                                                   \
    ASSERT_EQ(q.size(), assert_sz);                        \
    QUEUE_PRINT(q, display_fun);                           \
  } while (0)

#define P_QUEUE_CALL(pq, fun, display_fun, assert_sz)      \
  do {                                                     \
    std::string fname = #fun;                              \
    std::cout << "- After called: " << fname << std::endl; \
    fun;                                                   \
    ASSERT_EQ(pq.size(), assert_sz);                       \
    P_QUEUE_PRINT(pq, display_fun);                        \
  } while (0)

TEST(QueInitTest, Init) {
  queue<int> q1;
  ASSERT_EQ(q1.size(), 0);

  queue<int> q2(6);
  ASSERT_EQ(q2.size(), 6);

  deque<int> d1(6);
  queue<int> q3(d1);
  ASSERT_EQ(q3.size(), 6);

  queue<int> q4(std::move(d1));
  ASSERT_EQ(q4.size(), 6);

  queue<int> q5(q4);
  ASSERT_EQ(q5.size(), 6);

  queue<int> q6(std::move(q4));
  ASSERT_EQ(q6.size(), 6);

  queue<int> q7 = q6;
  ASSERT_EQ(q7.size(), 6);

  queue<int> q8 = std::move(q6);
  ASSERT_EQ(q8.size(), 6);
}

class QueueTest : public testing::Test {
 protected:
  queue<int>        q1;
  queue<nontrivial> q2;

  QueueTest() {
    deque<int>        d1 = {1, 2, 3, 4, 5, 6, 7, 8};
    deque<nontrivial> d2 = {1, 2, 3, 4, 5, 6, 7, 8};
    q1 = d1;
    q2 = d2;
  }
};

TEST_F(QueueTest, Push) {
  QUEUE_CALL(q1, q1.push(9), display_int, 9);
  QUEUE_CALL(q2, q2.push(9), display_obj, 9);
}

TEST_F(QueueTest, Emplace) {
  QUEUE_CALL(q1, q1.emplace(9), display_int, 9);
  QUEUE_CALL(q2, q2.emplace(9, 0), display_obj, 9);
}

TEST_F(QueueTest, Pop) {
  QUEUE_CALL(q1, q1.pop(), display_int, 7);
  QUEUE_CALL(q2, q2.pop(), display_obj, 7);
}

TEST_F(QueueTest, ElementAccess) {
  ASSERT_EQ(q1.front(), 1);
  ASSERT_EQ(q1.back(), 8);

  ASSERT_EQ(q2.front(), nontrivial(1, 0));
  ASSERT_EQ(q2.back(), nontrivial(8, 0));
}

TEST_F(QueueTest, Swap) {
  deque<int> d1 = {1, 2, 3, 4, 5, 6};
  queue<int> q3(d1);
  QUEUE_CALL(q1, swap(q1, q3), display_int, 6);

  deque<nontrivial> d2 = {1, 2, 3, 4, 5, 6};
  queue<nontrivial> q4(d2);
  QUEUE_CALL(q2, swap(q2, q4), display_obj, 6);
}

TEST_F(QueueTest, Operator) {
  deque<int> d1 = {1, 2, 3, 4, 5, 6, 7, 8};
  queue<int> q3(d1);

  ASSERT_TRUE(q1 == q3);
  ASSERT_TRUE(q1 >= q3);
  ASSERT_TRUE(q1 <= q3);

  q3.push(9);
  ASSERT_TRUE(q1 < q3);
  ASSERT_TRUE(q1 <= q3);
  ASSERT_TRUE(q3 > q1);
  ASSERT_TRUE(q3 >= q1);
}

TEST(PrioQueInitTest, Init) {
  priority_queue<int> pq1;
  ASSERT_EQ(pq1.size(), 0);

  vector<int>         v1(6);
  priority_queue<int> pq2(v1.begin(), v1.end());
  ASSERT_EQ(pq2.size(), 6);

  priority_queue<int> pq3(v1);
  ASSERT_EQ(pq3.size(), 6);

  priority_queue<int> pq4(std::move(v1));
  ASSERT_EQ(pq4.size(), 6);

  priority_queue<int> pq5(pq4);
  ASSERT_EQ(pq5.size(), 6);

  priority_queue<int> pq6(std::move(pq4));
  ASSERT_EQ(pq6.size(), 6);

  priority_queue<int> pq7 = pq6;
  ASSERT_EQ(pq7.size(), 6);

  priority_queue<int> pq8 = std::move(pq6);
  ASSERT_EQ(pq8.size(), 6);
}

struct nontrivialComp {
  bool operator()(const nontrivial& a, const nontrivial& b) {
    return *a.i < *b.i;
  }
};

class PrioQueTest : public testing::Test {
 protected:
  priority_queue<int>                                            pq1;
  priority_queue<nontrivial, vector<nontrivial>, nontrivialComp> pq2;

  PrioQueTest() {
    vector<int>        v1 = {2, 1, 4, 3, 6, 5, 8, 7};
    vector<nontrivial> v2 = {2, 1, 4, 3, 6, 5, 8, 7};
    pq1 = v1;
    for (auto& n : v2) {
      pq2.push(n);
    }
  }
};

TEST_F(PrioQueTest, Push) {
  P_QUEUE_CALL(pq1, pq1.push(8), display_int, 9);
  P_QUEUE_CALL(pq2, pq2.push(8), display_obj, 9);
}

TEST_F(PrioQueTest, Emplace) {
  P_QUEUE_CALL(pq1, pq1.emplace(6), display_int, 9);
  P_QUEUE_CALL(pq2, pq2.emplace(6), display_obj, 9);
}

TEST_F(PrioQueTest, Pop) {
  P_QUEUE_CALL(pq1, pq1.pop(), display_int, 7);
  P_QUEUE_CALL(pq2, pq2.pop(), display_obj, 7);
}

TEST_F(PrioQueTest, ElementAccess) {
  ASSERT_EQ(pq1.top(), 8);
  ASSERT_EQ(pq2.top(), nontrivial(8, 0));
}

TEST_F(PrioQueTest, Swap) {
  vector<int>         v1 = {1, 2, 3, 4, 5, 6};
  priority_queue<int> pq3(v1);
  P_QUEUE_CALL(pq1, swap(pq1, pq3), display_int, 6);

  vector<nontrivial>                                             v2 = {1, 2, 3, 4, 5, 6};
  priority_queue<nontrivial, vector<nontrivial>, nontrivialComp> pq4(v2);
  P_QUEUE_CALL(pq2, swap(pq2, pq4), display_obj, 6);
}

TEST_F(PrioQueTest, Operator) {
  // 注意，底层容器初始值不同，所构造的堆也可能不同！
  vector<int>         v1 = {2, 1, 4, 3, 6, 5, 8, 7};
  priority_queue<int> pq3(v1);

  ASSERT_TRUE(pq1 == pq3);

  pq3.push(9);

  ASSERT_TRUE(pq1 != pq3);
}

}  // namespace gd

#endif  //!__TEST_QUEUE__H__