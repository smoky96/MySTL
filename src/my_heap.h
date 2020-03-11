#ifndef __MY_HEAP__H
#define __MY_HEAP__H

#include "my_iterator.h"

namespace gd {

template <typename RandomAccessIterator, typename Distance, typename T>
void push_heap_aux2(RandomAccessIterator first, Distance holeIndex, Distance topIndex, T value) {
  Distance parent = (holeIndex - 1) / 2;  // 父节点
  while (holeIndex > topIndex && *(first + parent) < value) {
    // 大根堆
    *(first + holeIndex) = *(first + parent);
    holeIndex = parent;
    parent = (holeIndex - 1) / 2;
  }
  *(first + holeIndex) = value;
}

template <typename RandomAccessIterator, typename Distance, typename T>
inline void push_heap_aux1(RandomAccessIterator first, RandomAccessIterator last, Distance*, T*) {
  push_heap_aux2(first, Distance((last - first) - 1), Distance(0), T(*(last - 1)));
}

template <typename RandomAccessIterator>
inline void push_heap(RandomAccessIterator first, RandomAccessIterator last) {
  // 新元素已经位于尾端
  push_heap_aux1(first, last, difference_type(first), value_type(first));
}

template <typename RandomAccessIterator, typename Distance, typename T>
void adjust_heap(RandomAccessIterator first, Distance holeIndex, Distance len, T value) {
  Distance topIndex = holeIndex;
  Distance rchild = 2 * holeIndex + 2;  // 右孩子
  // 下溯 (percolate down)
  while (rchild < len) {
    if (*(first + rchild) < *(first + rchild - 1))
      --rchild;
    *(first + holeIndex) = *(first + rchild);
    holeIndex = rchild;
    rchild = 2 * holeIndex + 2;
  }

  if (rchild == len) {
    // 到最后没有右孩子了，只有左孩子
    *(first + holeIndex) = *(first + (rchild - 1));
    holeIndex = rchild - 1;
  }
  // 此时的 holeIndex 不一定满足 value 的插入位置，调用 push heap 操作将 value 插入大根堆
  // 这里 topIndex 是要调整的子树的根节点，不一定是整棵树的根节点，在 make_heap 中会用到
  push_heap_aux2(first, holeIndex, topIndex, value);
}

template <typename RandomAccessIterator, typename T, typename Distance>
inline void pop_heap_aux2(RandomAccessIterator first, RandomAccessIterator last, RandomAccessIterator result, T value,
                          Distance*) {
  *result = *first;
  // 将尾值设为堆顶元素的值，之后直接 pop_back 即可
  // 之前尾部的值存储在 value 当中，无需担心被覆盖
  adjust_heap(first, Distance(0), Distance(last - first), value);
}

template <typename RandomAccessIterator, typename T>
inline void pop_heap_aux1(RandomAccessIterator first, RandomAccessIterator last, T*) {
  pop_heap_aux2(first, last - 1, last - 1, T(*(last - 1)), difference_type(first));
}

template <typename RandomAccessIterator>
inline void pop_heap(RandomAccessIterator first, RandomAccessIterator last) {
  pop_heap_aux1(first, last, value_type(first));
}

template <typename RandomAccessIterator>
void sort_heap(RandomAccessIterator first, RandomAccessIterator last) {
  // 每执行一次 pop heap，最大元素都被放到尾部
  while (last - first > 1) {
    pop_heap(first, last--);
  }
}

template <typename RandomAccessIterator, typename Distance>
void make_heap_aux(RandomAccessIterator first, RandomAccessIterator last, Distance*) {
  if (last - first < 2) {
    return;
  }
  Distance len = last - first;
  Distance parent = (len - 2) / 2;  // 最后一个节点的父节点(最后一个非叶子节点)
  while (true) {
    // 不停的将大值往上提
    adjust_heap(first, parent, len, *(first + parent));
    if (parent == 0)
      return;
    --parent;
  }
}

template <typename RandomAccessIterator>
inline void make_heap(RandomAccessIterator first, RandomAccessIterator last) {
  make_heap_aux(first, last, difference_type(first));
}

// 下面是接受谓词版本的 heap 算法

template <typename RandomAccessIterator, typename Distance, typename T, typename Compare>
void push_heap_aux2(RandomAccessIterator first, Distance holeIndex, Distance topIndex, T value, Compare comp) {
  Distance parent = (holeIndex - 1) / 2;  // 父节点
  while (holeIndex > topIndex && comp(*(first + parent), value)) {
    // 大根堆
    *(first + holeIndex) = *(first + parent);
    holeIndex = parent;
    parent = (holeIndex - 1) / 2;
  }
  *(first + holeIndex) = value;
}

template <typename RandomAccessIterator, typename Distance, typename T, typename Compare>
inline void push_heap_aux1(RandomAccessIterator first, RandomAccessIterator last, Distance*, T*, Compare comp) {
  push_heap_aux2(first, Distance((last - first) - 1), Distance(0), T(*(last - 1)), comp);
}

template <typename RandomAccessIterator, typename Compare>
inline void push_heap(RandomAccessIterator first, RandomAccessIterator last, Compare comp) {
  // 新元素已经位于尾端
  push_heap_aux1(first, last, difference_type(first), value_type(first), comp);
}

template <typename RandomAccessIterator, typename Distance, typename T, typename Compare>
void adjust_heap(RandomAccessIterator first, Distance holeIndex, Distance len, T value, Compare comp) {
  Distance topIndex = holeIndex;
  Distance rchild = 2 * holeIndex + 2;  // 右孩子
  // 下溯 (percolate down)
  while (rchild < len) {
    if (comp(*(first + rchild), *(first + rchild - 1)))
      --rchild;
    *(first + holeIndex) = *(first + rchild);
    holeIndex = rchild;
    rchild = 2 * holeIndex + 2;
  }

  if (rchild == len) {
    // 到最后没有右孩子了，只有左孩子
    *(first + holeIndex) = *(first + (rchild - 1));
    holeIndex = rchild - 1;
  }
  // 此时的 holeIndex 不一定满足 value 的插入位置，调用 push heap 操作将 value 插入大根堆
  // 这里 topIndex 是要调整的子树的根节点，不一定是整棵树的根节点，在 make_heap 中会用到
  push_heap_aux2(first, holeIndex, topIndex, value, comp);
}

template <typename RandomAccessIterator, typename T, typename Distance, typename Compare>
inline void pop_heap_aux2(RandomAccessIterator first, RandomAccessIterator last, RandomAccessIterator result, T value,
                          Distance*, Compare comp) {
  *result = *first;
  // 将尾值设为堆顶元素的值，之后直接 pop_back 即可
  // 之前尾部的值存储在 value 当中，无需担心被覆盖
  adjust_heap(first, Distance(0), Distance(last - first), value, comp);
}

template <typename RandomAccessIterator, typename T, typename Compare>
inline void pop_heap_aux1(RandomAccessIterator first, RandomAccessIterator last, T*, Compare comp) {
  pop_heap_aux2(first, last - 1, last - 1, T(*(last - 1)), difference_type(first), comp);
}

template <typename RandomAccessIterator, typename Compare>
inline void pop_heap(RandomAccessIterator first, RandomAccessIterator last, Compare comp) {
  pop_heap_aux1(first, last, value_type(first), comp);
}

template <typename RandomAccessIterator, typename Compare>
void sort_heap(RandomAccessIterator first, RandomAccessIterator last, Compare comp) {
  // 每执行一次 pop heap，最大元素都被放到尾部
  while (last - first > 1) {
    pop_heap(first, last--, comp);
  }
}

template <typename RandomAccessIterator, typename Distance, typename Compare>
void make_heap_aux(RandomAccessIterator first, RandomAccessIterator last, Distance*, Compare comp) {
  if (last - first < 2) {
    return;
  }
  Distance len = last - first;
  Distance parent = (len - 2) / 2;  // 最后一个节点的父节点(最后一个非叶子节点)
  while (true) {
    // 不停的将大值往上提
    adjust_heap(first, parent, len, *(first + parent), comp);
    if (parent == 0)
      return;
    --parent;
  }
}

template <typename RandomAccessIterator, typename Compare>
inline void make_heap(RandomAccessIterator first, RandomAccessIterator last, Compare comp) {
  make_heap_aux(first, last, difference_type(first), comp);
}

}  // namespace gd

#endif  //!__MY_HEAP__H