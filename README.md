# MySTL

> 一个简单的、带有注释的 STL 部分实现

一个简单的、带有注释的 STL 部分实现，变量函数的命名简单清晰，非常易读。大部分函数的声明和定义是在一起的，方便跳转（可能不太符合规范，但我觉得这样方便一点，同时也减少一些代码量）。已经用 goolge test 对其进行了测试，但测试覆盖可能不够全面。
该实现主要参考 gcc 2.9.5 的实现（2.9.5 版本的代码较为清晰简洁，最新版本的实现过于复杂）。然后在此基础上添加了大部分 C++11 标准的接口。

## Getting Started

### Prerequisites 项目使用条件

* 在 gcc 7.4.0 以及 msvc 2017 下均可编译通过
* cmake >= 3.15 ([cmake 下载及安装](https://cmake.org/))

### Installation 安装

OS X & Linux 下需要重新编译 [google test](https://github.com/google/googletest)，依次执行下列命令即可。
```sh
$ git clone git@github.com:google/googletest.git
$ cd googletest
$ mkdir build
$ cd build

$ cmake -DCMAKE_BUILD_TYPE=Debug ..
# 或
$ cmake -DCMAKE_BUILD_TYPE=Release ..

$ make

$ cd lib
$ cp ./* ~/MySTL/dependencies/googletest/lib #(替换成你自己的路径)
```

依次执行以下命令对测试代码进行编译：

```sh
# 在 MySTL 目录下
$ mkdir build
$ cd build

$ cmake -DCMAKE_BUILD_TYPE=Debug ..
# 或
$ cmake -DCMAKE_BUILD_TYPE=Release ..

$ make
```

### Usage example 使用示例

```sh
# 在 build 目录下

$ ./Test.exe
# 或（在 linux 下）
$ ./Test
[==========] Running 61 tests from 13 test suites.
[----------] Global test environment set-up.
[----------] 1 test from DequeInitTest
[ RUN      ] DequeInitTest.Init
[       OK ] DequeInitTest.Init (0 ms)
[----------] 1 test from DequeInitTest (0 ms total)

[----------] 10 tests from DequeTest

# ... 省略部分输出

[----------] Global test environment tear-down
[==========] 61 tests from 13 test suites ran. (24 ms total)
[  PASSED  ] 61 tests.
```

若不想进行性能测试，将 MySTL 目录下的 CMakeLists.txt 中第 35 行注释掉即可。

## Release History 版本历史

* 截至 2020.04.03 已完成：
  * allocator（包括一级以及二级配置器）
  * 底层数据解构：heap
  * vector、list、deque、stack、queue、priority_queue
  * 底层数据解构：红黑树

## Authors 关于作者

* [GuoDong](https://guodong.plus)

## License 授权协议

[GNU General Public License v3.0](https://github.com/smoky96/MySTL/blob/master/LICENSE)

# Have Fun ! d=====(￣▽￣*)b