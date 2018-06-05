#ifndef POLO_SMOOTHING_ADAGRAD_HPP_
#define POLO_SMOOTHING_ADAGRAD_HPP_

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <iterator>
#include <mutex>
#include <vector>

namespace polo {
namespace smoothing {
template <class value_t, class index_t> struct adagrad {
  adagrad(const value_t epsilon = 1E-6) : epsilon{epsilon} {}

  adagrad(const adagrad &) = default;
  adagrad &operator=(const adagrad &) = default;
  adagrad(adagrad &&) = default;
  adagrad &operator=(adagrad &&) = default;

  template <class InputIt1, class InputIt2, class OutputIt>
  OutputIt smooth(const index_t, const index_t, InputIt1 xbegin, InputIt1 xend,
                  InputIt2 gprev, OutputIt gcurr) {
    std::lock_guard<std::mutex> lock(sync);
    value_t g_val{0};
    index_t idx{0};
    while (xbegin != xend) {
      xbegin++;
      g_val = *gprev++;
      rms_g[idx] += g_val * g_val;
      *gcurr++ = g_val / (std::sqrt(rms_g[idx]) + epsilon);
      idx++;
    }
    return gcurr;
  }

protected:
  void parameters(const value_t epsilon) { this->epsilon = epsilon; }

  template <class InputIt> void initialize(InputIt xbegin, InputIt xend) {
    rms_g = std::vector<value_t>(std::distance(xbegin, xend));
  }

  ~adagrad() = default;

private:
  value_t epsilon{1E-6};
  std::vector<value_t> rms_g;
  std::mutex sync;
};
} // namespace smoothing
} // namespace polo

#endif
