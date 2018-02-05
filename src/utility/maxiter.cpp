#ifndef MAXITER_CPP_
#define MAXITER_CPP_

#include <cstddef>

namespace kth {
namespace optim {
namespace utility {

struct maxiter {
  maxiter(const std::size_t K = 10000) : K{K} {}

  template <class float_t, class InputIt1, class InputIt2>
  bool operator()(const std::size_t k, const float_t fval, InputIt1 x_begin,
                  InputIt1 x_end, InputIt2 g_begin) {
    return k > K;
  }

private:
  std::size_t K;
};

} // namespace utility
} // namespace optim
} // namespace kth

#endif
