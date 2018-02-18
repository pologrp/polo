#ifndef QP_HPP_
#define QP_HPP_

#include <iterator>
#include <utility>
#include <vector>

namespace pbopt {
namespace problem {

template <class float_t> struct qp {
  qp(std::vector<float_t> Q, std::vector<float_t> q)
      : Q{std::move(Q)}, q{std::move(q)} {}

  template <class InputIt, class OutputIt>
  float_t operator()(InputIt xbegin, InputIt xend, OutputIt gbegin) {
    float_t loss{0}, xval;
    std::size_t idx{0};
    while (xbegin != xend) {
      xval = *xbegin++;
      loss += 0.5 * Q[idx] * xval * xval + q[idx] * xval;
      *gbegin++ = Q[idx] * xval + q[idx];
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
  float_t optimum(std::vector<float_t> &xopt) {
    return optimum(std::begin(xopt));
  }

private:
  std::vector<float_t> Q, q;
};

} // namespace problem
} // namespace pbopt

#endif
