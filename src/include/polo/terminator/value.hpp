#ifndef POLO_TERMINATOR_VALUE_HPP_
#define POLO_TERMINATOR_VALUE_HPP_

#include <cmath>
#include <limits>

namespace polo {
namespace terminator {
template <class value_t = double, class index_t = int> struct value {
  value(const value_t abs, const value_t rel = 0,
        const value_t eps = std::numeric_limits<value_t>::epsilon())
      : abs{abs}, rel{rel}, eps{eps} {}

  template <class InputIt1, class InputIt2>
  bool operator()(const index_t k, const value_t fval, InputIt1 x_begin,
                  InputIt1 x_end, InputIt2 g_begin) {
    const value_t fdiff = fprev - fval;
    if ((std::abs(fdiff) < abs) | (std::abs(fdiff / (fprev + eps)) < rel))
      return true;
    fprev = fval;
    return false;
  }

private:
  const value_t abs;
  const value_t rel;
  const value_t eps;
  value_t fprev;
};
} // namespace terminator
} // namespace polo

#endif
