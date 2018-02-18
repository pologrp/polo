#ifndef NESTEROV_HPP_
#define NESTEROV_HPP_

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <vector>

namespace pbopt {
namespace gradient {

template <class float_t> struct nesterov {
  nesterov(const float_t mu = 0.95, const float_t epsilon = 1E-3)
      : mu{mu}, epsilon{epsilon} {}

  nesterov(const nesterov &) = default;
  nesterov &operator=(const nesterov &) = default;
  nesterov(nesterov &&) = default;
  nesterov &operator=(nesterov &&) = default;

  template <class InputIt, class OutputIt>
  OutputIt grad(InputIt gold_begin, InputIt gold_end, OutputIt gnew_begin) {
    std::size_t idx{0};
    float_t nu_prev, g_val;
    while (gold_begin != gold_end) {
      g_val = *gold_begin++;
      nu_prev = nu[idx];
      nu[idx] = mu * nu_prev + epsilon * g_val;
      *gnew_begin++ = mu * mu * nu_prev + (1 + mu) * epsilon * g_val;
      idx++;
    }
    return gnew_begin;
  }

protected:
  void params(const float_t mu, const float_t epsilon) {
    this->mu = mu;
    this->epsilon = epsilon;
  }

  template <class InputIt> void initialize(InputIt xbegin, InputIt xend) {
    nu = std::vector<float_t>(std::distance(xbegin, xend));
  }

  ~nesterov() = default;

private:
  float_t mu{0.95}, epsilon{1E-3};
  std::vector<float_t> nu;
};

} // namespace gradient
} // namespace pbopt

#endif
