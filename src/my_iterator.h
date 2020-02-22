#ifndef __MY_ITERATOR_H
#define __MY_ITERATOR_H

#include <cstddef>

namespace gd {

// 五种迭代器类型
struct input_iterator_tag {};
struct output_iterator_tag {};
struct forward_iterator_tag : public input_iterator_tag {};
struct bidirectional_iterator_tag : public forward_iterator_tag {};
struct random_access_iterator_tag : public bidirectional_iterator_tag {};

// 所有迭代器继承这个模板，以防有所疏漏
template <typename Category, typename T, typename Distance = ptrdiff_t, typename Point = T*, typename Reference = T&>
struct iterator {
  typedef Category  iterator_category;
  typedef T         value_type;
  typedef Distance  difference_type;
  typedef Point     pointer;
  typedef Reference reference;
};

template <typename T, typename Distance>
struct input_iterator : public iterator<input_iterator_tag, T, Distance, T*, T&> {};

// traits 技法，一个泛化，两个特化 (用于 native pointer)
template <typename Iterator>
struct iterator_traits {
  // 每种迭代器都有自己定义的内嵌型别，泛化版本的 traits 就用来提取这些型别
  typedef typename Iterator::iterator_category iterator_category;
  typedef typename Iterator::value_type        value_type;
  typedef typename Iterator::difference_type   difference_type;
  typedef typename Iterator::pointer           pointer;
  typedef typename Iterator::reference         reference;
};

// native pointer 没有内嵌型别，所以用偏特化版本的 traits 来提取类型
template <typename T>
struct iterator_traits<T*> {
  typedef random_access_iterator_tag iterator_category;
  typedef T                          value_type;
  typedef ptrdiff_t                  difference_type;
  typedef T*                         pointer;
  typedef T&                         reference;
};

template <typename T>
struct iterator_traits<const T*> {
  typedef random_access_iterator_tag iterator_category;
  typedef T                          value_type;
  typedef ptrdiff_t                  difference_type;
  typedef const T*                   pointer;
  typedef const T&                   reference;
};

// 萃取迭代器类型 (category)
template <typename Iterator>
inline typename iterator_traits<Iterator>::iterator_category iterator_category(const Iterator&) {
  typedef typename iterator_traits<Iterator>::iterator_category category;
  // 返回一个临时对象
  return category();
}

// 萃取迭代器的 distance type
template <typename Iterator>
inline typename iterator_traits<Iterator>::difference_type* difference_type(const Iterator&) {
  // 返回指针
  return static_cast<typename iterator_traits<Iterator>::difference_type*>(0);
}

// 萃取迭代器的 value type
template <typename Iterator>
inline typename iterator_traits<Iterator>::value_type* value_type(const Iterator&) {
  // 返回指针
  return static_cast<typename iterator_traits<Iterator>::value_type*>(0);
}

// 计算迭代器距离
template <typename InputIterator>
inline typename iterator_traits<InputIterator>::difference_type distance_dispatch(InputIterator first,
                                                                                  InputIterator last,
                                                                                  input_iterator_tag) {
  typename iterator_traits<InputIterator>::difference_type distance = 0;
  while (first != last) {
    ++first;
    ++distance;
  }
  return distance;
}

template <typename RandomAccessIterator>
inline typename iterator_traits<RandomAccessIterator>::difference_type distance_dispatch(RandomAccessIterator first,
                                                                                         RandomAccessIterator last,
                                                                                         random_access_iterator_tag) {
  return last - first;
}

template <typename InputIterator>
inline typename iterator_traits<InputIterator>::difference_type distance(InputIterator first, InputIterator last) {
  return distance_dispatch(first, last, iterator_category(first));
}

// 将迭代器前进 n 个距离
template <typename InputIterator, typename Distance>
inline void advance_dispatch(InputIterator& i, Distance n, input_iterator_tag) {
  while (n--)
    ++i;
}

template <typename BidirectionalIterator, typename Distance>
inline void advance_dispatch(BidirectionalIterator& i, Distance n, bidirectional_iterator_tag) {
  if (n >= 0)
    while (n--)
      ++i;
  else
    while (n++)
      --i;
}

template <typename RandomAccessIterator, typename Distance>
inline void advance_dispatch(RandomAccessIterator& i, Distance n, random_access_iterator_tag) {
  i += n;
}

template <typename InputIterator, typename Distance>
inline void advance(InputIterator& i, Distance n) {
  advance_dispatch(i, n, iterator_category(i));
}

}  // namespace gd

#endif  // !__MY_ITERATOR_H