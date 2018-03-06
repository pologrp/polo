#ifndef RMSPROP_HPP_
#define RMSPROP_HPP_

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <iterator>
#include <vector>

namespace polo {
namespace smoothing {
template <class value_t> struct rmsprop {
  rmsprop(const value_t rho = 0.9, const value_t epsilon = 1E-6)
      : rho{rho}, epsilon{epsilon} {}

  rmsprop(const rmsprop &) = default;
  rmsprop &operator=(const rmsprop &) = default;
  rmsprop(rmsprop &&) = default;
  rmsprop &operator=(rmsprop &&) = default;

  template <class InputIt1, class InputIt2, class OutputIt>
  OutputIt smooth(const std::size_t k, InputIt1 xbegin, InputIt1 xend,
                  InputIt2 gold_begin, OutputIt gnew_begin) {
    value_t g_val{0};
    std::size_t idx{0};
    while (xbegin != xend) {
      xbegin++;
      g_val = *gold_begin++;
      rms_g[idx] = rho * rms_g[idx] + (1 - rho) * g_val * g_val;
      *gnew_begin++ = g_val / (std::sqrt(rms_g[idx]) + epsilon);
      idx++;
    }
    return gnew_begin;
  }

protected:
  void params(const value_t rho, const value_t epsilon) {
    this->rho = rho;
    this->epsilon = epsilon;
  }

  template <class InputIt> void initialize(InputIt xbegin, InputIt xend) {
    rms_g = std::vector<value_t>(std::distance(xbegin, xend));
  }

  ~rmsprop() = default;

private:
  value_t rho{0.9}, epsilon{1E-6};
  std::vector<value_t> rms_g;
};
} // namespace smoothing
} // namespace polo

#endif
