#ifndef __MY_ALLOC_H
#define __MY_ALLOC_H

#include <cstring>  // for memcpy
#include "my_construct.h"

namespace gd {

// 将一级或二级配置器包装起来，使其符合 STL 规格
template <typename T, typename Alloc>
class simple_alloc {
 public:
  typedef T value_type;
  typedef T* pointer;
  typedef const T* const_pointer;
  typedef T& reference;
  typedef const T& const_reference;
  typedef size_t size_type;
  typedef ptrdiff_t differene_type;

  // rebind allocator of type U
  template <typename U>
  struct rebind {
    typedef simple_alloc<U, Alloc> other;
  };

  static pointer allocate(size_t n) {
    return 0 == n ? 0 : static_cast<pointer>(Alloc::allocate(n * sizeof(value_type)));
  }

  static pointer allocate(void) {
    return static_cast<pointer>(Alloc::allocate(sizeof(value_type)));
  }

  static void deallocate(pointer p, size_t n) {
    Alloc::deallocate(p, n * sizeof(value_type));
  }

  static void deallocate(pointer p) {
    Alloc::deallocate(p, sizeof(value_type));
  }
};

// 无 template 型别参数，inst 没有用到
template <int inst>
class __malloc_alloc_template {
 private:
  // 处理内存不足
  static void* oom_malloc(size_t);
  static void* oom_realloc(void*, size_t);
  static void (*__malloc_alloc_oom_handler)();

 public:
  static void* allocate(size_t n) {
    // 一级配置器直接使用malloc
    void* result = malloc(n);
    // 若内存不够，调用 oom_malloc
    if (0 == result)
      result = oom_malloc(n);
    return result;
  }

  static void deallocate(void* p, size_t /* n */) {
    free(p);
  }

  static void* reallocate(void* p, size_t /* old_sz */, size_t new_sz) {
    void* result = realloc(p, new_sz);
    if (0 == result)
      result == oom_realloc(p, new_sz);
    return result;
  }

  // 仿真 c++ 的 set_new_handler()
  // 这是一种返回函数指针的写法，从里向外读，set_malloc_handler 有形参列表，
  // 所以是一个函数，其前面有一个 *，所以返回的是一个指针。
  // 进一步观察，这个指针也有一个参数列表，
  // 因此该指针指向一个函数，这个函数的参数为空，返回值为 void 类型。
  static void (*set_malloc_handler(void (*f)()))() {
    void (*old)() = __malloc_alloc_oom_handler;
    __malloc_alloc_oom_handler = f;
    return old;
  }
};

// malloc_alloc out-of-memory handling
// 初值为 0，由客户端设定
template <int inst>
void (*__malloc_alloc_template<inst>::__malloc_alloc_oom_handler)() = 0;

template <int inst>
void* __malloc_alloc_template<inst>::oom_malloc(size_t n) {
  void (*my_alloc_handler)();
  void* result;
  for (;;) {  // 循环直到抛出异常或分配到内存
    my_alloc_handler = __malloc_alloc_oom_handler;
    if (0 == my_alloc_handler) {
      throw std::bad_alloc();  // 若没有设定处理函数，则抛出异常
    }
    (*my_alloc_handler)();  // 调用用户设置的处理函数，企图释放内存
    result = malloc(n);     // 尝试再次分配
    if (result)
      return result;
  }
}

template <int inst>
void* __malloc_alloc_template<inst>::oom_realloc(void* p, size_t n) {
  void (*my_alloc_handler)();
  void* result;
  for (;;) {  // 循环直到抛出异常或分配到内存
    my_alloc_handler = __malloc_alloc_oom_handler;
    if (0 == my_alloc_handler) {
      throw std::bad_alloc();  // 若没有设定处理函数，则抛出异常
    }
    (*my_alloc_handler)();   // 调用用户设置的处理函数，企图释放内存
    result = realloc(p, n);  // 尝试再次分配
    if (result)
      return result;
  }
}

// 将参数 inst 指定为 0
typedef __malloc_alloc_template<0> malloc_alloc;

// 二级配置器，用于处理小区快，当区块小于 __MAX_BYTES
// 时使用内存池管理，否则直接用一级配置器分配
template <int inst>
class __default_alloc_template {
 private:
  enum { __ALIGN = 8 };                           // 小型区块的上调边界
  enum { __MAX_BYTES = 128 };                     // 小型区块的上限
  enum { __NFREELISTS = __MAX_BYTES / __ALIGN };  // free list 个数

  static size_t round_up(size_t bytes) {
    // & ~(__ALIGN - 1) 将低三位置 0
    return ((bytes + __ALIGN - 1) & ~(__ALIGN - 1));
  }

  // free-lists 结点的构造
  union obj {
    union obj* next;
    char data[1];
  };

  // 16 个 free lists，free list 数组存的是每个 free list 的头结点的地址
  static obj* volatile free_list[__NFREELISTS];

  // 决定用哪个 free-list
  static size_t freelist_index(size_t bytes) {
    return ((bytes + __ALIGN - 1) / __ALIGN - 1);
  }

  // 返回大小为 n 的对象，并可能加入大小为 n 的其他区块到 free list
  static void* refill(size_t n);
  // 配置一大块空间，可容纳 nobjs 个大小为 'size' 的区块
  // 若配置 nobjs 个区块有所不便，nobjs 会减小
  static char* chunk_alloc(size_t size, int& nobjs);

  // chunk allocation state
  static char* start_free;  // 内存池起始位置，只在 chunk_alloc() 中变化
  static char* end_free;    // 内存池结束位置，只在 chunk_alloc() 中变化
  static size_t heap_size;

 public:
  // 分配 n bytes
  static void* allocate(size_t n) {
    obj* volatile* my_free_list;  // 一个指针，指向 free_list 数组 (obj*)
    obj* result;

    // 大于 128 bytes 直接调用一级配置器
    if (n > static_cast<size_t>(__MAX_BYTES)) {
      return malloc_alloc::allocate(n);
    }

    // 确定 16 个 free list 中的哪一个
    my_free_list = free_list + freelist_index(n);
    // 取出对应的 free list 的头结点
    result = *my_free_list;
    if (0 == result) {
      // 对应的 free list 已经用完了，重新填充这个 free list
      void* r = refill(round_up(n));
      return r;
    }

    // 解引用获得头结点，调整对应的 free list 的头结点位置，将之前的头结点返回
    *my_free_list = result->next;
    return result;
  }

  static void deallocate(void* p, size_t n) {
    obj* q = static_cast<obj*>(p);
    obj* volatile* my_free_list;

    if (n > 128) {
      malloc_alloc::deallocate(p, n);
      return;
    }

    my_free_list = free_list + freelist_index(n);
    // 将区块重新放到对应的 free_list 中，调整头结点位置
    q->next = *my_free_list;
    *my_free_list = q;
  }

  static void* reallocate(void* p, size_t old_sz, size_t new_sz);
};

template <int inst>
char* __default_alloc_template<inst>::start_free = 0;

template <int inst>
char* __default_alloc_template<inst>::end_free = 0;

template <int inst>
size_t __default_alloc_template<inst>::heap_size = 0;

template <int inst>
typename __default_alloc_template<inst>::obj* volatile __default_alloc_template<inst>::free_list[__NFREELISTS] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

// 返回大小为 n 的对象，并且会为适当的 free list 增加结点
// 假设 n 已经对齐
template <int inst>
void* __default_alloc_template<inst>::refill(size_t n) {
  int nobjs = 20;  // 默认取得 20 个节点
  // 调用 chunk_alloc() 尝试取得 nobjs 个区块作为 free list 的新节点
  // nobjs 是引用传参
  char* chunk = chunk_alloc(n, nobjs);
  obj* volatile* my_free_list;
  obj* result;
  obj *current_obj, *next_obj;
  int i;

  // 若只获得了 1 个区块，这个区块就直接给调用者，free list 无新节点
  if (1 == nobjs)
    return chunk;

  my_free_list = free_list + freelist_index(n);
  result = reinterpret_cast<obj*>(chunk);                        // 第一块返回给调用者
  *my_free_list = next_obj = reinterpret_cast<obj*>(chunk + n);  // 指向下一个区块
  for (i = 1;; ++i) {
    current_obj = next_obj;
    next_obj = reinterpret_cast<obj*>((char*)current_obj + n);
    if (nobjs - 1 == i) {
      current_obj->next = 0;
      break;
    } else {
      current_obj->next = next_obj;
    }
  }
  return result;
}

// 假设 size 已经对齐
template <int inst>
char* __default_alloc_template<inst>::chunk_alloc(size_t size, int& nobjs) {
  char* result;
  size_t total_bytes = size * nobjs;
  size_t bytes_left = end_free - start_free;

  if (bytes_left >= total_bytes) {
    // 内存池足够，直接分配返回
    result = start_free;
    start_free += total_bytes;
    return result;
  } else if (bytes_left >= size) {
    // 内存池不够，但大于一个 nobjs 的大小，能分多少是多少
    nobjs = static_cast<int>(bytes_left / size);
    total_bytes = size * nobjs;
    result = start_free;
    start_free += total_bytes;
    return result;
  } else {
    // 内存池连一个区块都不够分了
    size_t bytes_to_get = 2 * total_bytes + round_up(heap_size >> 4);
    if (bytes_left > 0) {
      // 若内存池中还有内存，则先把它分配给合适的 free list
      obj* volatile* my_free_list = free_list + freelist_index(bytes_left);
      reinterpret_cast<obj*>(start_free)->next = *my_free_list;
      *my_free_list = reinterpret_cast<obj*>(start_free);
    }
    // 调用 malloc，补充内存池
    start_free = (char*)malloc(bytes_to_get);
    if (0 == start_free) {
      // heap 空间不足，malloc 失败
      int i;
      obj* volatile* my_free_list;
      obj* p;
      // 看看我们还有什么，搜寻适当的 free list(只往大的搜)，
      // 看看 free list 上还有没有没用且足够大的区块
      for (i = size; i <= __MAX_BYTES; i += __ALIGN) {
        my_free_list = free_list + freelist_index(i);
        p = *my_free_list;
        if (0 != p) {
          // 摘取当前区块
          *my_free_list = p->next;
          start_free = reinterpret_cast<char*>(p);
          end_free = start_free + i;
          // 递归调用自己，为了修正 nobjs
          return chunk_alloc(size, nobjs);
          // 任何残余的内存终将被放进适当的 free list
        }
      }

      // 没有任何内存可以使用了
      end_free = 0;
      // 调用一级配置器，看看 out-of-memory 的机制能否有用
      // 要么抛异常，要么内存不足的情况得以改善
      start_free = static_cast<char*>(malloc_alloc::allocate(bytes_to_get));
    }
    heap_size += bytes_to_get;
    end_free = start_free + bytes_to_get;
    // 递归调用自己，为了修正 nobjs
    return chunk_alloc(size, nobjs);
  }
}

template <int inst>
void* __default_alloc_template<inst>::reallocate(void* p, size_t old_sz, size_t new_sz) {
  void* result;
  size_t copy_sz;

  if (old_sz > static_cast<size_t>(__MAX_BYTES) && new_sz > static_cast<size_t>(__MAX_BYTES)) {
    return realloc(p, new_sz);
  }

  if (round_up(old_sz) == round_up(new_sz))
    return p;

  result = allocate(new_sz);
  copy_sz = new_sz > old_sz ? old_sz : new_sz;
  memcpy(result, p, copy_sz);
  deallocate(p, old_sz);
  return result;
}

typedef __default_alloc_template<0> alloc;

}  // namespace gd

#endif  // !__MY_ALLOC_H