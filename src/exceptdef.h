#ifndef __EXCEPTDEF_H
#define __EXCEPTDEF_H

#include <stdexcept>

namespace gd {

#define THROW_OUT_OF_RANGE_IF(expr, what) \
  if (expr)                               \
  throw std::out_of_range(what)

#define THROW_RUNTIME_ERROR_IF(expr, what) \
  if (expr)                                \
  throw std::runtime_error(what)

}  // namespace gd

#endif  // __EXCEPTDEF