#ifndef ADAGRAD_HPP_
#define ADAGRAD_HPP_

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <iterator>
#include <vector>

namespace polo {
namespace smoothing {
template <class value_t> struct adagrad {
  adagrad(const value_t epsilon = 1E-6) : epsilon{epsilon} {}

  adagrad(const adagrad &) = default;
  adagrad &operator=(const adagrad &) = default;
  adagrad(adagrad &&) = default;
  adagrad &operator=(adagrad &&) = default;

  template <class InputIt1, class InputIt2, class OutputIt>
  OutputIt smooth(const std::size_t k, InputIt1 xbegin, InputIt1 xend,
                  InputIt2 gold_begin, OutputIt gnew_begin) {
    value_t g_val{0};
    std::size_t idx{0};
    while (xbegin != xend) {
      xbegin++;
      g_val = *gold_begin++;
      rms_g[idx] += g_val * g_val;
      *gnew_begin++ = g_val / (std::sqrt(rms_g[idx]) + epsilon);
      idx++;
    }
    return gnew_begin;
  }

protected:
  void params(const value_t epsilon) { this->epsilon = epsilon; }

  template <class InputIt> void initialize(InputIt xbegin, InputIt xend) {
    rms_g = std::vector<value_t>(std::distance(xbegin, xend));
  }

  ~adagrad() = default;

private:
  value_t epsilon{1E-6};
  std::vector<value_t> rms_g;
};
} // namespace smoothing
} // namespace polo

#endif
