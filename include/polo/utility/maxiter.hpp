#ifndef MAXITER_HPP_
#define MAXITER_HPP_

#include <cstddef>

namespace polo {
namespace utility {
namespace terminator {

struct maxiter {
  maxiter(const std::size_t K) : K{K} {}

  template <class float_t, class InputIt1, class InputIt2>
  bool operator()(const std::size_t k, const float_t fval, InputIt1 x_begin,
                  InputIt1 x_end, InputIt2 g_begin) {
    return k > K;
  }

private:
  std::size_t K;
};

} // namespace terminator
} // namespace utility
} // namespace polo

#endif
