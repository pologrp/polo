#ifndef DIAGQP_CPP_
#define DIAGQP_CPP_

#include <utility>
#include <vector>

namespace pbopt {
namespace problem {

template <class float_t> struct diagqp {
  static float_t wrapper(const float_t *x_first, const float_t *x_last,
                         float_t *dx, void *instance) {
    diagqp *ptr = static_cast<diagqp *>(instance);
    return ptr->operator()(x_first, x_last, dx);
  }

  diagqp(std::vector<float_t> Q, std::vector<float_t> q)
      : Q{std::move(Q)}, q{std::move(q)} {}
  template <class InputIt, class OutputIt>
  float_t operator()(InputIt x_first, InputIt x_last, OutputIt dx) {
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

  template <class OutputIt> float_t optimum(OutputIt xbegin) {
    std::size_t idx{0};
    float_t fopt{0}, xopt;
    for (const auto qval : q) {
      xopt = -qval / Q[idx];
      fopt += 0.5 * Q[idx] * xopt * xopt + qval * xopt;
      *xbegin++ = xopt;
      idx++;
    }
    return fopt;
  }

private:
  std::vector<float_t> Q, q;
};

} // namespace problem
} // namespace pbopt

#endif
