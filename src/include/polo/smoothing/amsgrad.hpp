#ifndef POLO_SMOOTHING_AMSGRAD_HPP_
#define POLO_SMOOTHING_AMSGRAD_HPP_

#include <algorithm>
#include <cmath>
#include <iterator>
#include <mutex>
#include <vector>

namespace polo {
namespace smoothing {
template <class value_t, class index_t> struct amsgrad {
  amsgrad(const value_t beta = 0.99, const value_t epsilon = 1E-6)
      : beta{beta}, epsilon{epsilon} {}

  amsgrad(const amsgrad &) = default;
  amsgrad &operator=(const amsgrad &) = default;
  amsgrad(amsgrad &&) = default;
  amsgrad &operator=(amsgrad &&) = default;

  template <class InputIt1, class InputIt2, class OutputIt>
  OutputIt smooth(const index_t, const index_t, InputIt1 xbegin, InputIt1 xend,
                  InputIt2 gprev, OutputIt gcurr) {
    std::lock_guard<std::mutex> lock(sync);
    value_t g_val{0};
    index_t idx{0};
    while (xbegin != xend) {
      xbegin++;
      g_val = *gprev++;
      nu[idx] = beta * nu[idx] + (1 - beta) * g_val * g_val;
      nu_hat[idx] = std::max(nu_hat[idx], nu[idx]);
      *gcurr++ = g_val / (std::sqrt(nu_hat[idx]) + epsilon);
      idx++;
    }
    return gcurr;
  }

protected:
  void parameters(const value_t beta, const value_t epsilon) {
    this->beta = beta;
    this->epsilon = epsilon;
  }

  template <class InputIt> void initialize(InputIt xbegin, InputIt xend) {
    nu = std::vector<value_t>(std::distance(xbegin, xend));
    nu_hat = std::vector<value_t>(nu);
  }

  ~amsgrad() = default;

private:
  value_t beta{0.99}, epsilon{1E-6};
  std::vector<value_t> nu, nu_hat;
  std::mutex sync;
};
} // namespace smoothing
} // namespace polo

#endif
