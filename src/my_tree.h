#ifndef __MY_TREE__H
#define __MY_TREE__H

#include "my_alloc.h"
#include "my_iterator.h"

namespace gd {

enum _rb_tree_color_type { _rb_tree_red = false, _rb_tree_black = true };

struct _rb_tree_node_base {
  typedef _rb_tree_color_type color_type;
  typedef _rb_tree_node_base* base_ptr;

  color_type color;
  base_ptr   parent;
  base_ptr   left;
  base_ptr   right;

  static base_ptr minimum(base_ptr x) {
    while (x->left != 0)
      x = x->left;
    return x;
  }

  static base_ptr maximum(base_ptr x) {
    while (x->right != 0)
      x = x->right;
    return x;
  }
};

template <typename Value>
struct _rb_tree_node : public _rb_tree_node_base {
  typedef _rb_tree_node* link_type;
  Value                  value_field;  // 为什么把值放在派生类当中呢？
};

template <typename Value, typename Ref, typename Ptr>
struct _rb_tree_iterator {
  typedef Value                      value_type;
  typedef Ref                        reference;
  typedef Ptr                        pointer;
  typedef ptrdiff_t                  difference_type;
  typedef bidirectional_iterator_tag iterator_category;

  typedef _rb_tree_iterator                                    self;
  typedef _rb_tree_iterator<Value, Value&, Value*>             iterator;
  typedef _rb_tree_iterator<Value, const Value&, const Value*> const_iterator;

  typedef _rb_tree_node_base::base_ptr base_ptr;
  typedef _rb_tree_node<Value>*        link_type;

  base_ptr node;  // iterator 所指节点

  // constructors
  _rb_tree_iterator(){};
  _rb_tree_iterator(link_type x) {
    node = x;
  }
  _rb_tree_iterator(base_ptr x) {
    node = x;
  }
  _rb_tree_iterator(const iterator& rhs) {
    node = rhs.node;
  }

  // operators
  reference operator*() {
    return static_cast<link_type>(node)->value_field;
  }
  pointer operator->() {
    return &(operator*());
    // 若 value_field 定义了 -> 运算符，则会继续调用这个运算符
    // 知道返回内置指针为止，然后调用 *(x).member
  }

  self& operator++() {
    if (node->right != nullptr) {
      // 找到右子树的最小值
      node = _rb_tree_node_base::minimum(node->right);
    } else {
      base_ptr y = node->parent;
      while (node == y->right) {  // 找到以当前节点所在子树为左子树的根节点
        node = y;
        y = y->parent;
      }
      // 若当前节点为根节点，而根节点没有右子节点，
      // 则此时 node->right = y，而 node 刚好指向 end()
      if (node->right != y)
        node = y;  // 一般情况下，y 即为下一个节点
    }
    return *this;
  }

  self operator++(int) {
    iterator tmp(*this);
    ++*this;
    return tmp;
  }

  self& operator--() {
    if (node->color == _rb_tree_red && node->parent->parent == node) {  // node 当前指向 header
      node = node->right;                                               // 则让 node 指向最大值节点
    } else if (node->left != nullptr) {
      // 找到左子树的最大值
      node = _rb_tree_node_base::maximum(node->left);
    } else {  // 没有左子树了
      base_ptr y = node->parent;
      while (node == y->left) {  // 找到以当前节点所在子树为右子树的节点
        node = y;
        y = y->parent;
      }
      node = y;
      // 若 node 指向根节点且左子树为空，则 node 不变， 还是指向根节点
    }
    return *this;
  }

  self operator--(int) {
    self tmp(*this);
    --*this;
    return tmp;
  }

  bool operator==(const iterator& rhs) const {
    return node == rhs.node;
  }

  bool operator==(const const_iterator& rhs) const {
    return node == rhs.node;
  }

  bool operator!=(const iterator& rhs) const {
    return node != rhs.node;
  }

  bool operator!=(const const_iterator& rhs) const {
    return node != rhs.node;
  }
};

// tree operate
inline void _rb_tree_rotate_left(_rb_tree_node_base* x, _rb_tree_node_base*& root) {
  _rb_tree_node_base* y = x->right;
  x->right = y->left;
  if (y->left != nullptr)
    y->left->parent = x;
  y->parent = x->parent;

  if (x == root)
    root = y;
  else if (x == x->parent->left)
    x->parent->left = y;
  else
    x->parent->right = y;
  y->left = x;
  x->parent = y;
}

inline void _rb_tree_rotate_right(_rb_tree_node_base* x, _rb_tree_node_base*& root) {
  _rb_tree_node_base* y = x->left;
  x->left = y->right;
  if (y->right != nullptr)
    y->right->parent = x;
  y->parent = x->parent;

  if (x == root)
    root = y;
  else if (x == x->parent->right)
    x->parent->right = y;
  else
    x->parent->left = y;
  y->right = x;
  x->parent = y;
}

/*
  红黑树性质：
    1. 每个节点不是黑色就是红色
    2. 根节点是黑色
    3. 叶子节点是黑色
    4. 红色节点的子节点是黑色
    5. 每条路径上的黑色节点数目相同
*/

/*
  红黑树插入情况：（只有在插入节点的父节点为红色时才需要调整，若树为空，则将插入节点设为根节点，并将其设为黑色）
  以下为插入节点父节点为红色的情况：
    1. 叔节点存在且为红色：将父节点和叔节点都设为黑色，祖父节点设为红色，将祖父节点设为当前插入节点
    2. 叔节点不存在或为黑色，且插入节点的父节点为左孩子：
      2.1. 插入节点是父节点的右孩子：将父节点设为插入节点，并对其进行左旋（左旋后父节点边成孩子节点了），转到 2.2
      2.2. 插入节点是父节点的左孩子：将父节点设为黑色，祖父节点设为红色，对祖父节点右旋，调整结束
    3. 叔节点不存在或为黑色，且插入节点的父节点为右孩子：(与 2 相同，左右互换即可)
*/
inline void _rb_tree_rebalance_for_insert(_rb_tree_node_base* x, _rb_tree_node_base*& root) {
  x->color = _rb_tree_red;                                 // 所有插入节点都为红色
  while (x != root && x->parent->color == _rb_tree_red) {  // 循环直到父节点为黑色或当前节点为根节点为止
    if (x->parent == x->parent->parent->left) {
      _rb_tree_node_base* y = x->parent->parent->right;
      if (y && y->color == _rb_tree_red) {  // 情况 1
        x->parent->color = _rb_tree_black;
        y->color = _rb_tree_black;
        x->parent->parent->color = _rb_tree_red;
        x = x->parent->parent;
      } else {
        if (x == x->parent->right) {  // 情况 2.1
          x = x->parent;
          _rb_tree_rotate_left(x, root);
        }
        // 情况 2.2
        x->parent->color = _rb_tree_black;
        x->parent->parent->color = _rb_tree_red;
        _rb_tree_rotate_right(x->parent->parent, root);
      }
    } else {
      _rb_tree_node_base* y = x->parent->parent->left;
      if (y && y->color == _rb_tree_red) {  // 情况 1
        x->parent->color = _rb_tree_black;
        y->color = _rb_tree_black;
        x->parent->parent->color = _rb_tree_red;
        x = x->parent->parent;
      } else {
        if (x == x->parent->left) {  // 情况 2.1
          x = x->parent;
          _rb_tree_rotate_right(x, root);
        }
        // 情况 2.2
        x->parent->color = _rb_tree_black;
        x->parent->parent->color = _rb_tree_red;
        _rb_tree_rotate_left(x->parent->parent, root);
      }
    }
  }
  root->color = _rb_tree_black;  // 不要忘记根节点永远为黑色
}

inline _rb_tree_node_base* _rb_tree_rebalance_for_remove(_rb_tree_node_base* z, _rb_tree_node_base*& root,
                                                         _rb_tree_node_base*& leftmost,
                                                         _rb_tree_node_base*& rightmost) {
  _rb_tree_node_base* y = z;  // z 为待删除节点
  _rb_tree_node_base* x = nullptr;
  _rb_tree_node_base* x_parent = nullptr;

  // 红黑数的删除是用后继节点代替要删除的节点，然后进行调整
  if (y->left != nullptr && y->right != nullptr) {  // 若 y 有两个孩子
    // 让 y 指向 z 的后继
    y = _rb_tree_node_base::minimum(y->right);
    x = y->right;
  } else {  // 若 y 没有孩子或只有一个孩子
    x = y->left == nullptr ? y->right : y->left;
    // x 可能为空
  }

  if (y != z) {  // 若 y != z，则 z 有两个孩子，此时，y 指向 z 的后继，x 指向 y 的右孩子
    // 用 y 代替 z 的位置，并用 x 顶替 y

    // 先将左边接上（1）
    z->left->parent = y;
    y->left = z->left;

    // 若 y != z->right，则说明 z 的右孩子肯定有左孩子
    //（因为 y 指向 z 的后继，如果 z 的右孩子没有左孩子，则 z 的后继就是右孩子）
    if (y != z->right) {
      // 用 x 顶替 y
      x_parent = y->parent;
      if (x != nullptr)
        x->parent = x_parent;
      y->parent->left = x;

      // 再将右边接上（2）
      y->right = z->right;
      z->right->parent = y;
    } else {
      x_parent = y;
    }

    // 最后将 y 与 z 的父节点链接起来（3）
    if (root == z)
      root = y;
    else if (z->parent->left == z)
      z->parent->left = y;
    else
      z->parent->right = y;
    y->parent = z->parent;
    std::swap(y->color, z->color);
    y = z;  // y 指向最终要删除的节点
  } else {
    // 若 y == z，直接用 x 代替 z
    x_parent = y->parent;
    if (x)
      x->parent = x_parent;
    if (root == z)
      root = x;
    else if (z->parent->left == z)
      z->parent->left = x;
    else
      z->parent->right = x;

    if (leftmost == z) {
      if (z->right == nullptr)
        leftmost = z->parent;
      else
        leftmost = _rb_tree_node_base::minimum(x);
    }
    if (rightmost == z) {
      if (z->left == nullptr)
        rightmost = z->parent;
      else
        rightmost = _rb_tree_node_base::maximum(x);
    }
  }

  // 此时，y 指向真正要删除的节点，x 为代替 y 的节点（被 x 代替的可能是 z，也可能是 z 的后继）
  // 现在的情况如下，其中，x 为代替节点（其位置本来是 y，所以 y 处的节点才是正真要删的，一开始要删的节点已经被替换了），
  // P 是父节点，S 是兄弟节点，SL 和 SR 分别是兄弟节点的左右孩子：
  /*
          P
         / \
        x   S
           / \
          SL SR
    只有在被代替的节点的颜色为黑色时，才需要调整，如果被代替节点为红色，则无需调整，具体情况如下：
    1. 被 x 替换的节点是红色：将 x 变为黑色即可
    2. 被 x 替换的节点是黑色：
      2.1. x 是其父节点的左孩子(也就是上图所示情况)
        2.1.1 若 S 为红色：将 S 设为黑色，P 设为红色，对 P 左旋，转到情况 2.1.2.1
        2.1.2 若 S 为黑色：
          2.1.2.1 SR 和 SL 都是黑色：将 S 设为红色，P 设为新的替换节点，继续调整
          2.1.2.2 SR 为黑色，SL 为红色或空：将 S 设为红色，SL 设为黑色，对 S 右旋，转到 2.1.2.3
          2.1.2.3 SR 为红色，SL 任意：将 S 设为 P 的颜色，P 设为黑色，SR 设为黑色，对 P 左旋，调整结束
      2.2. x 是其父节点的右孩子(和 2.1 相同，左右互换即可)


    参考博客：https://www.jianshu.com/p/e136ec79235c
  */
  if (y->color != _rb_tree_red) {
    while (x != root && (x == nullptr || x->color == _rb_tree_black)) {  // 情况 2
      if (x == x_parent->left) {                                         // 情况 2.1
        _rb_tree_node_base* s = x_parent->right;
        if (s->color == _rb_tree_red) {  // 情况 2.1.1
          s->color = _rb_tree_black;
          x_parent->color = _rb_tree_red;
          _rb_tree_rotate_left(x_parent, root);
          s = x_parent->right;
        }
        if ((s->left == nullptr || s->left->color == _rb_tree_black) &&
            (s->right == nullptr || s->right->color == _rb_tree_black)) {  // 情况 2.1.2.1
          s->color = _rb_tree_red;
          x = x_parent;
          x_parent = x_parent->parent;
        } else {
          if (s->right == nullptr || s->right->color == _rb_tree_black) {  // 情况 2.1.2.2
            if (s->left)
              s->left->color = _rb_tree_black;
            s->color = _rb_tree_red;
            _rb_tree_rotate_right(s, root);
            s = x_parent->right;
          }
          // 情况 2.1.2.3
          s->color = x_parent->color;
          x_parent->color = _rb_tree_black;
          if (s->right)
            s->right->color = _rb_tree_black;
          _rb_tree_rotate_left(x_parent, root);
          break;
        }
      } else {
        _rb_tree_node_base* s = x_parent->left;
        if (s->color == _rb_tree_red) {  // 情况 2.1.1
          s->color = _rb_tree_black;
          x_parent->color = _rb_tree_red;
          _rb_tree_rotate_right(x_parent, root);
          s = x_parent->left;
        }
        if ((s->left == nullptr || s->left->color == _rb_tree_black) &&
            (s->right == nullptr || s->right->color == _rb_tree_black)) {  // 情况 2.1.2.1
          s->color = _rb_tree_red;
          x = x_parent;
          x_parent = x_parent->parent;
        } else {
          if (s->left == nullptr || s->left->color == _rb_tree_black) {  // 情况 2.1.2.2
            if (s->right)
              s->right->color = _rb_tree_black;
            s->color = _rb_tree_red;
            _rb_tree_rotate_left(s, root);
            s = x_parent->left;
          }
          // 情况 2.1.2.3
          s->color = x_parent->color;
          x_parent->color = _rb_tree_black;
          if (s->left)
            s->left->color = _rb_tree_black;
          _rb_tree_rotate_right(x_parent, root);
          break;
        }
      }
    }
    if (x)
      x->color = _rb_tree_black;
  }
  return y;
}

template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc = alloc>
class rb_tree {
 public:
  typedef Key               key_type;
  typedef Value             value_type;
  typedef value_type*       pointer;
  typedef const value_type* const_pointer;
  typedef value_type&       reference;
  typedef const value_type& const_reference;
  typedef size_t            size_type;
  typedef ptrdiff_t         difference_type;

  typedef _rb_tree_node_base   base_type;
  typedef _rb_tree_node<Value> node_type;
  typedef base_type*           base_ptr;
  typedef node_type*           link_type;
  typedef _rb_tree_color_type  color_type;

  typedef _rb_tree_iterator<value_type, reference, pointer>             iterator;
  typedef _rb_tree_iterator<value_type, const_reference, const_pointer> const_iterator;

  typedef simple_alloc<Value, Alloc>     allocator_type;
  typedef simple_alloc<Value, Alloc>     data_allocator;
  typedef simple_alloc<base_type, Alloc> base_allocator;
  typedef simple_alloc<node_type, Alloc> node_allocator;

  allocator_type get_allocator() const {
    return allocator_type();
  }

 protected:
  link_type _header;
  size_type _node_count;
  Compare   _key_compare;

  link_type _get_node() {
    return node_allocator::allocate(1);
  }

  void _put_node(link_type p) {
    node_allocator::deallocate(p, 1);
  }

  template <typename... Args>
  link_type _create_node(Args&&... args) {
    link_type tmp = _get_node();
    try {
      construct(&(tmp->value_field), std::forward<Args>(args)...);
    } catch (...) {
      _put_node(tmp);
      throw;
    }
    return tmp;
  }

  link_type _clone_node(link_type x) {
    link_type tmp = _create_node(x->value_field);
    tmp->color = x->color;
    tmp->left = nullptr;
    tmp->right = nullptr;
    return tmp;
  }

  void destroy_node(link_type p) {
    destroy(&(p->value_field));
    _put_node(p);
  }

  link_type& _root() const {
    return (link_type&)_header->parent;
  }

  link_type& _leftmost() const {
    return (link_type&)_header->left;
  }

  link_type& _rightmost() const {
    return (link_type&)_header->right;
  }

  static link_type& _left(link_type x) {
    return (link_type&)(x->left);
  }

  static link_type& _right(link_type x) {
    return (link_type&)(x->right);
  }

  static link_type& _parent(link_type x) {
    return (link_type&)(x->parent);
  }

  static reference _value(link_type x) {
    return x->value_field;
  }

  static const key_type& _key(link_type x) {
    return KeyOfValue()(_value(x));
  }

  static color_type& _color(link_type x) {
    return (color_type&)(x->color);
  }

  static link_type& _left(base_ptr x) {
    return (link_type&)(x->left);
  }

  static link_type& _right(base_ptr x) {
    return (link_type&)(x->right);
  }

  static link_type& _parent(base_ptr x) {
    return (link_type&)(x->parent);
  }

  static reference _value(base_ptr x) {
    return static_cast<link_type>(x)->value_field;
  }

  static const key_type& _key(base_ptr x) {
    return KeyOfValue()(_value(x));
  }

  static color_type& _color(base_ptr x) {
    return (color_type&)(x->color);
  }

  static link_type _minimum(link_type x) {
    return static_cast<link_type>(base_type::minimum(x));
  }

  static link_type _maximum(link_type x) {
    return static_cast<link_type>(base_type::maximum(x));
  }

 private:  // helper functions
  void __empty_init() {
    _header = _get_node();  // _header 的 value_field 是没有初始化的
    _color(_header) = _rb_tree_red;
    _root() = nullptr;
    _leftmost() = _header;
    _rightmost() = _header;
    _node_count = 0;
  }

  // z 为插入节点，y 为插入节点的父节点，x 为插入位置
  iterator __insert(link_type x, link_type y, link_type z) {
    // 这里 x 肯定为空了，x 在这个函数中有什么作用吗？
    if (y == _header || x != nullptr || _key_compare(_key(z), _key(y))) {
      // 若 z 比 y 小，则插在 y 的左边
      _left(y) = z;
      if (y == _header) {  // 若 y 是 header，则令 z 为根节点
        _root() = z;
        _rightmost() = z;
      } else if (y == _leftmost()) {
        _leftmost() = z;
      }
    } else {  // z 比 y 大，插在 y 的右边
      _right(y) = z;
      if (y == _rightmost())
        _rightmost() = z;
    }

    _parent(z) = y;
    _left(z) = nullptr;
    _right(z) = nullptr;
    _rb_tree_rebalance_for_insert(z, _header->parent);
    ++_node_count;
    return iterator(z);
  }

  // v 为插入值，y 为插入节点的父节点，x 为插入位置
  iterator __insert(link_type x, link_type y, const value_type& v) {
    link_type z = _create_node(v);

    // 这里 x 肯定为空了，x 在这个函数中有什么作用吗？
    if (y == _header || x != nullptr || _key_compare(_key(z), _key(y))) {
      // 若 z 比 y 小，则插在 y 的左边
      _left(y) = z;
      if (y == _header) {  // 若 y 是 header，则令 z 为根节点
        _root() = z;
        _rightmost() = z;
      } else if (y == _leftmost()) {
        _leftmost() = z;
      }
    } else {  // z 比 y 大，插在 y 的右边
      _right(y) = z;
      if (y == _rightmost())
        _rightmost() = z;
    }

    _parent(z) = y;
    _left(z) = nullptr;
    _right(z) = nullptr;
    _rb_tree_rebalance_for_insert(z, _header->parent);
    ++_node_count;
    return iterator(z);
  }

  template <typename InputIterator>
  void __insert_unique_dispatch(InputIterator first, InputIterator last, input_iterator_tag) {
    for (; first != last; ++first)
      insert_unique(*first);
  }

  template <typename ForwardIterator>
  void __insert_unique_dispatch(ForwardIterator first, ForwardIterator last, forward_iterator_tag) {
    // TODO(dong) 可能是由于在 my_map 中使用了 std::pair，这里的 distance 调用会与 std 中的 distance 出现歧义
    size_type n = gd::distance(first, last);
    for (; n > 0; --n, ++first)
      insert_unique(*first);
  }

  template <typename InputIterator>
  void __insert_equal_dispatch(InputIterator first, InputIterator last, input_iterator_tag) {
    for (; first != last; ++first)
      insert_equal(*first);
  }

  template <typename ForwardIterator>
  void __insert_equal_dispatch(ForwardIterator first, ForwardIterator last, forward_iterator_tag) {
    // TODO(dong) 可能是由于在 my_map 中使用了 std::pair，这里的 distance 调用会与 std 中的 distance 出现歧义
    size_type n = gd::distance(first, last);
    for (; n > 0; --n, ++first)
      insert_equal(*first);
  }

  // 将以 x 为根节点的树拷贝到 p 上
  link_type __copy(link_type x, link_type p) {
    link_type top = _clone_node(x);
    top->parent = p;

    try {
      if (x->right)  // 右子树递归拷贝
        top->right = __copy(_right(x), top);
      p = top;
      x = _left(x);  // 左子树不递归？

      while (x != 0) {
        link_type y = _clone_node(x);
        p->left = y;
        y->parent = p;
        if (x->right)
          y->right = __copy(_right(x), y);
        p = y;
        x = _left(x);
      }
    } catch (...) {
      __erase(top);
      throw;
    }

    return top;
  }

  // 只删除，不平衡，仅供内部使用
  void __erase(link_type x) {
    while (x != nullptr) {
      __erase(_right(x));  // 只递归一边，真的可以降低复杂度吗？
      link_type y = _left(x);
      destroy_node(x);
      x = y;
    }
  }

 public:  // constructors, copy, destructors
  rb_tree() : _key_compare() {
    __empty_init();
  }

  rb_tree(const rb_tree& rhs) {
    __empty_init();
    if (rhs._root() != nullptr) {
      _root() = __copy(rhs._root(), _header);
      _leftmost() = _minimum(_root());
      _rightmost() = _maximum(_root());
    }
    _node_count = rhs._node_count;
  }

  rb_tree(rb_tree&& rhs) noexcept {
    _header = rhs._header;
    _node_count = rhs._node_count;
    _key_compare = rhs._key_compare;

    rhs._header = nullptr;
    rhs._node_count = 0;
  }

  rb_tree& operator=(const rb_tree& rhs) {
    if (this != &rhs) {
      clear();
      _node_count = 0;
      _key_compare = rhs._key_compare;
      if (rhs._root() == nullptr) {
        _root() = nullptr;
        _leftmost() = _header;
        _rightmost() = _header;
      } else {
        _root() = __copy(rhs._root(), _header);
        _leftmost() = _minimum(_root());
        _rightmost() = _maximum(_root());
        _node_count = rhs._node_count;
      }
    }
    return *this;
  }

  rb_tree& operator=(rb_tree&& rhs) {
    if (this != &rhs) {
      clear();
      _header = rhs._header;
      _node_count = rhs._node_count;
      _key_compare = rhs._key_compare;
      rhs._header = nullptr;
      rhs._node_count = 0;
    }
    return *this;
  }

  ~rb_tree() {
    clear();
    if (_header)
      _put_node(_header);
  }

 public:  // iterator
  iterator begin() noexcept {
    return _leftmost();
  }

  const_iterator begin() const noexcept {
    return _leftmost();
  }

  iterator end() noexcept {
    return _header;
  }

  const_iterator end() const noexcept {
    return _header;
  }

 public:  // capacity
  bool empty() const noexcept {
    return _node_count == 0;
  }

  size_type size() const noexcept {
    return _node_count;
  }

  size_type max_size() const noexcept {
    return static_cast<size_type>(-1);
  }

 public:  // modify
  template <typename... Args>
  std::pair<iterator, bool> emplace_unique(Args&&... args) {
    link_type y = _header;
    link_type x = _root();
    link_type z = _create_node(std::forward<Args>(args)...);

    bool comp = true;
    while (x != nullptr) {
      y = x;
      comp = _key_compare(_key(z), _key(x));
      x = comp ? _left(x) : _right(x);  // 比当前节点小就往左，比当前节点大或等于都往右
    }

    iterator j = iterator(y);  // y 指向插入位置的父节点

    if (comp) {          // 比父节点小，插在左边
      if (j == begin())  // 若 j 是最小节点了，则肯定无重复值了，因为 z 比最小值还小哦
        return std::make_pair(__insert(x, y, z), true);
      else  // 否则，看看 --j 的值，若 --j < z < j 的话，就说明没有重复值
        --j;
    }
    // 若插入点在右边，那么如果有重复值，只可能是 j，
    // 因为前面寻找插入点的时候，大于或等于都会往右走，于是： j 的父节点 < j <= z

    if (_key_compare(_key(j.node), _key(z)))
      return std::make_pair(__insert(x, y, z), true);

    // 插入失败，不要忘记释放已经分配的节点
    destroy_node(z);
    return std::make_pair(j, false);
  }

  template <typename... Args>
  iterator emplace_unique(iterator pos, Args... args) {
    link_type p = _create_node(std::forward<Args>(args)...);

    if (pos.node == _header->left) {                              // begin()
      if (size() > 0 && _key_compare(_key(p), _key(pos.node))) {  // 若比最小的还小
        return __insert((link_type)pos.node, (link_type)pos.node, p);
      } else {
        destroy_node(p);  // TODO(dong) 这里效率有待提高
        return emplace_unique(std::forward<Args>(args)...).first;
      }
    } else if (pos.node == _header) {                   // end()
      if (_key_compare(_key(_rightmost()), _key(p))) {  // 若比最大的还大
        return __insert((link_type)0, _rightmost(), p);  // 这里参数 x 设为空，直接在 __insert() 中提前进入第一种情况
      } else {
        destroy_node(p);  // TODO(dong) 这里效率有待提高
        return emplace_unique(std::forward<Args>(args)...).first;
      }
    } else {
      iterator before = pos;
      --before;
      if (_key_compare(_key(before.node), _key(p)) && _key_compare(_key(p), _key(pos.node))) {
        // 若 *(--pos) < p->value < *(pos)
        if (_right(before.node) == nullptr)  // 插到右边
          return __insert((link_type)0, (link_type)before.node, p);
        else
          return __insert((link_type)pos.node, (link_type)pos.node, p);
      } else {
        destroy_node(p);  // TODO(dong) 这里效率有待提高
        return emplace_unique(std::forward<Args>(args)...).first;
      }
    }
  }

  template <typename... Args>
  iterator emplace_equal(Args&&... args) {
    link_type y = _header;
    link_type x = _root();
    link_type z = _create_node(std::forward<Args>(args)...);

    while (x != nullptr) {
      y = x;
      x = _key_compare(_key(z), _key(x)) ? _left(x) : _right(x);
    }
    return __insert(x, y, z);
  }

  template <typename... Args>
  iterator emplace_equal(iterator pos, Args... args) {
    link_type p = _create_node(std::forward<Args>(args)...);

    if (pos.node == _header->left) {                              // begin()
      if (size() > 0 && _key_compare(_key(p), _key(pos.node))) {  // 若比最小的还小
        return __insert((link_type)pos.node, (link_type)pos.node, p);
      } else {
        destroy_node(p);  // TODO(dong) 这里效率有待提高
        return emplace_equal(std::forward<Args>(args)...);
      }
    } else if (pos.node == _header) {                   // end()
      if (_key_compare(_key(_rightmost()), _key(p))) {  // 若比最大的还大
        return __insert((link_type)0, _rightmost(), p);  // 这里参数 x 设为空，直接在 __insert() 中提前进入第一种情况
      } else {
        destroy_node(p);  // TODO(dong) 这里效率有待提高
        return emplace_equal(std::forward<Args>(args)...);
      }
    } else {
      iterator before = pos;
      --before;
      if (_key_compare(_key(before.node), _key(p)) && _key_compare(_key(p), _key(pos.node))) {
        // 若 *(--pos) < p->value < *(pos)
        if (_right(before.node) == nullptr)  // 插到右边
          return __insert((link_type)0, (link_type)before.node, p);
        else
          return __insert((link_type)pos.node, (link_type)pos.node, p);
      } else {
        destroy_node(p);  // TODO(dong) 这里效率有待提高
        return emplace_equal(std::forward<Args>(args)...);
      }
    }
  }

  std::pair<iterator, bool> insert_unique(const value_type& value) {
    link_type y = _header;
    link_type x = _root();

    bool comp = true;
    while (x != nullptr) {
      y = x;
      comp = _key_compare(KeyOfValue()(value), _key(x));
      x = comp ? _left(x) : _right(x);  // 比当前节点小就往左，比当前节点大或等于都往右
    }

    iterator j = iterator(y);  // y 指向插入位置的父节点

    if (comp) {          // 比父节点小，插在左边
      if (j == begin())  // 若 j 是最小节点了，则肯定无重复值了，因为 z 比最小值还小哦
        return std::make_pair(__insert(x, y, value), true);
      else  // 否则，看看 --j 的值，若 *--j < value < *j 的话，就说明没有重复值
        --j;
    }
    // 若插入点在右边，那么如果有重复值，只可能是 j，
    // 因为前面寻找插入点的时候，大于或等于都会往右走，于是： *(j 的父节点) < *j <= value

    if (_key_compare(_key(j.node), KeyOfValue()(value)))
      return std::make_pair(__insert(x, y, value), true);
    // 插入失败
    return std::make_pair(j, false);
  }

  std::pair<iterator, bool> insert_unique(value_type&& value) {
    return emplace_unique(std::move(value));
  }

  template <typename InputIterator>
  void insert_unique(InputIterator first, InputIterator last) {
    __insert_unique_dispatch(first, last, iterator_category(first));
  }

  iterator insert_unique(iterator pos, const value_type& value) {
    return emplace_unique(pos, value);
  }

  iterator insert_unique(iterator pos, const value_type&& value) {
    return emplace_unique(pos, std::move(value));
  }

  iterator insert_equal(const value_type& value) {
    link_type y = _header;
    link_type x = _root();

    while (x != nullptr) {
      y = x;
      x = _key_compare(KeyOfValue()(value), _key(x)) ? _left(x) : _right(x);
    }
    return __insert(x, y, value);
  }

  iterator insert_equal(value_type&& value) {
    return emplace_equal(std::move(value));
  }

  template <typename InputIterator>
  void insert_equal(InputIterator first, InputIterator last) {
    __insert_equal_dispatch(first, last, iterator_category(first));
  }

  iterator insert_equal(iterator pos, const value_type& value) {
    return emplace_equal(pos, value);
  }

  iterator insert_equal(iterator pos, const value_type&& value) {
    return emplace_equal(pos, std::move(value));
  }

  void erase(iterator pos) {
    link_type y =
        static_cast<link_type>(_rb_tree_rebalance_for_remove(pos.node, _header->parent, _header->left, _header->right));
    destroy_node(y);
    --_node_count;
  }

  size_type erase(const key_type& x) {
    std::pair<iterator, iterator> p = equal_range(x);
    // TODO(dong) 可能是由于在 my_map 中使用了 std::pair，这里的 distance 调用会与 std 中的 distance 出现歧义
    size_type n = gd::distance(p.first, p.second);
    erase(p.first, p.second);
    return n;
  }

  void erase(iterator first, iterator last) {
    if (first == begin() && last == end())
      clear();
    else
      // 这里一定要先将 first 往前进一步再删除，不然删除后原迭代器就失效了！
      while (first != last)
        erase(first++);
  }

  void clear() {
    if (_node_count != 0) {
      __erase(_root());
      _leftmost() = _header;
      _root() = nullptr;
      _rightmost() = _header;
      _node_count = 0;
    }
  }

  void swap(rb_tree& rhs) {
    std::swap(_header, rhs._header);
    std::swap(_node_count, rhs._node_count);
    std::swap(_key_compare, rhs._key_compare);
  }

 public:  // set operations
  iterator find(const key_type& k) {
    link_type y = _header;
    link_type x = _root();

    while (x != nullptr) {
      if (!_key_compare(_key(x), k)) {
        // 若 x >= k，则往左走，此时，若 x == k，
        // 则继续往左找，因为我们要找的是最靠前的 k（左子树比右子树靠前）
        y = x;
        x = _left(x);
      } else {
        x = _right(x);
      }
    }
    iterator j = iterator(y);

    // 如果 j == end() 或者 j 不等于 k，就没找到
    return (j == end() || _key_compare(k, _key(j.node))) ? end() : j;
  }

  const_iterator find(const key_type& k) const {
    link_type y = _header;
    link_type x = _root();

    while (x != nullptr) {
      if (!_key_compare(_key(x), k)) {
        // 若 x >= k，则往左走，此时，若 x == k，
        // 则继续往左找，因为我们要找的是最靠前的 k（左子树比右子树靠前）
        y = x;
        x = _left(x);
      } else {
        x = right(x);
      }
    }
    const_iterator j = const_iterator(y);

    // 如果 j == end() 或者 j 不等于 k，就没找到
    return (j == end() || _key_compare(k, _key(j.node))) ? end() : j;
  }

  size_type count(const key_type& k) const {
    auto p = equal_range(k);
    // TODO(dong) 可能是由于在 my_map 中使用了 std::pair，这里的 distance 调用会与 std 中的 distance 出现歧义
    size_type n = gd::distance(p.first, p.second);
    return n;
  }

  iterator lower_bound(const key_type& k) {
    link_type y = _header;
    link_type x = _root();

    while (x != nullptr) {
      if (!_key_compare(_key(x), k))  // x >= k, 跟 find() 一样，找最靠前的
        y = x, x = _left(x);
      else
        x = _right(x);
    }
    // 若没找到，则 y 会指向第一个比 k 大的节点
    return iterator(y);
  }

  const_iterator lower_bound(const key_type& k) const {
    link_type y = _header;
    link_type x = _root();

    while (x != nullptr) {
      if (!_key_compare(_key(x), k))  // x >= k, 跟 find() 一样，找最靠前的
        y = x, x = _left(x);
      else
        x = _right(x);
    }
    // 若没找到，则 y 会指向第一个比 k 大的节点
    return const_iterator(y);
  }

  iterator upper_bound(const key_type& k) {
    link_type y = _header;
    link_type x = _root();

    while (x != nullptr) {
      if (_key_compare(k, _key(x))) {  // k < x
        y = x;
        x = _left(x);
      } else {
        // 若 k >= x，则继续往右找，因为要找到最靠后的（右子树更靠后）
        x = _right(x);
      }
    }
    // 若没找到，y 会指向第一个比 k 大的节点
    return iterator(y);
  }

  const_iterator upper_bound(const key_type& k) const {
    link_type y = _header;
    link_type x = _root();

    while (x != nullptr) {
      if (_key_compare(k, _key(x))) {  // k < x
        y = x;
        x = _left(x);
      } else {
        // 若 k >= x，则继续往右找，因为要找到最靠后的（右子树更靠后）
        x = _right(x);
      }
    }
    // 若没找到，y 会指向第一个比 k 大的节点
    return const_iterator(y);
  }

  std::pair<iterator, iterator> equal_range(const key_type& k) {
    return std::make_pair(lower_bound(k), upper_bound(k));
  }

  std::pair<const_iterator, const_iterator> equal_range(const key_type& k) const {
    return std::make_pair(lower_bound(k), upper_bound(k));
  }
};

// operators
template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
inline bool operator==(const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& lhs,
                       const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& rhs) {
  return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
inline bool operator!=(const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& lhs,
                       const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& rhs) {
  return !(lhs == rhs);
}

template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
inline bool operator<(const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& lhs,
                      const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& rhs) {
  return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
inline bool operator>=(const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& lhs,
                       const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& rhs) {
  return !(lhs < rhs);
}

template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
inline bool operator>(const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& lhs,
                      const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& rhs) {
  return rhs < lhs;
}

template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
inline bool operator<=(const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& lhs,
                       const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& rhs) {
  return !(rhs < lhs);
}

// overload swap
template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
inline void swap(const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& lhs,
                 const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& rhs) {
  lhs.swap(rhs);
}

}  // namespace gd

#endif  //!__MY_TREE__H