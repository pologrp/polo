#ifndef MAXITER_HPP_
#define MAXITER_HPP_

#include <cstddef>

namespace polo {
namespace utility {
namespace terminator {
template <class value_t = double, class index_t = int> struct maxiter {
  maxiter(const index_t K) : K{K} {}

  template <class InputIt1, class InputIt2>
  bool operator()(const index_t k, const value_t fval, InputIt1 x_begin,
                  InputIt1 x_end, InputIt2 g_begin) const {
    return k > K;
  }

private:
  const index_t K;
};
} // namespace terminator
} // namespace utility
} // namespace polo

#endif
