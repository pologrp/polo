#ifndef POLO_PROX_L1NORM_HPP_
#define POLO_PROX_L1NORM_HPP_

#include <algorithm>
#include <cmath>

namespace polo {
namespace prox {
template <class value_t, class index_t> struct l1norm {
  l1norm(const value_t lambda = 1) : lambda{lambda} {}

  l1norm(const l1norm &) = default;
  l1norm &operator=(const l1norm &) = default;
  l1norm(l1norm &&) = default;
  l1norm &operator=(l1norm &&) = default;

  template <class InputIt1, class InputIt2, class OutputIt>
  OutputIt prox(const value_t step, InputIt1 xprev_b, InputIt1 xprev_e,
                InputIt2 gcurr, OutputIt xcurr) const {
    value_t xval, temp;
    while (xprev_b != xprev_e) {
      xval = *xprev_b++ - step * *gcurr++;
      temp = std::max(std::abs(xval) - lambda * step, value_t{0});
      *xcurr++ = xval < value_t{0} ? -temp : temp;
    }
    return xcurr;
  }

protected:
  void parameters(const value_t lambda) { this->lambda = lambda; }

  template <class InputIt> void initialize(InputIt, InputIt) {}

  ~l1norm() = default;

private:
  value_t lambda{1};
};
} // namespace prox
} // namespace polo

#endif
