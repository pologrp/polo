#ifndef DIAGQP_CPP_
#define DIAGQP_CPP_

#include <vector>

namespace kth {
namespace optim {
namespace test {

template <class float_t> struct diagqp {
  static float_t wrapper(const float_t *x_first, const float_t *x_last,
                         float_t *dx, void *instance) {
    diagqp * = static_cast<diagqp *>(instance);
    return diagqp->operator()(x_first, x_last, dx);
  }
  diagqp(std::vector<float_t> Q, std::vector<float_t> q)
      : Q{std::move(Q)}, q{std::move(q)} {}
  float_t operator()(const float_t *x_first, const float_t *x_last,
                     float_t *dx) {
    float_t loss{0}, xval;
    std::size_t idx{0};
    while (x_first != x_last) {
      xval = *x_first++;
      loss += 0.5 * Q[idx] * xval * xval + q[idx] * xval;
      *dx++ = Q[idx] * xval + q[idx];
      idx++;
    }
    return loss;
  }

private:
  std::vector<float_t> Q, q;
};

} // namespace test
} // namespace optim
} // namespace kth

#endif
