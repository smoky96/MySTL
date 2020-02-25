#include "test_helper.h"

namespace gd {

bool operator==(const nontrivial& lhs, const nontrivial& rhs) {
  return *lhs.i == *rhs.i && lhs.j == rhs.j;
}

}  // namespace gd
