#ifndef QP_HPP_
#define QP_HPP_

#include <iterator>
#include <utility>
#include <vector>

namespace polo {
namespace problem {
template <class value_t> struct qp {
  qp(std::vector<value_t> Q, std::vector<value_t> q)
      : Q{std::move(Q)}, q{std::move(q)} {}

  template <class InputIt, class OutputIt>
  value_t operator()(InputIt xbegin, InputIt xend, OutputIt gbegin) {
    value_t loss{0}, xval;
    std::size_t idx{0};
    while (xbegin != xend) {
      xval = *xbegin++;
      loss += 0.5 * Q[idx] * xval * xval + q[idx] * xval;
      *gbegin++ = Q[idx] * xval + q[idx];
      idx++;
    }
    return loss;
  }

  template <class OutputIt> value_t optimum(OutputIt xbegin) {
    std::size_t idx{0};
    value_t fopt{0}, xopt;
    for (const auto qval : q) {
      xopt = -qval / Q[idx];
      fopt += 0.5 * Q[idx] * xopt * xopt + qval * xopt;
      *xbegin++ = xopt;
      idx++;
    }
    return fopt;
  }
  value_t optimum(std::vector<value_t> &xopt) {
    return optimum(std::begin(xopt));
  }

private:
  std::vector<value_t> Q, q;
};
} // namespace problem
} // namespace polo

#endif
