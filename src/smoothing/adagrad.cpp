#ifndef ADAGRAD_CPP_
#define ADAGRAD_CPP_

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <iterator>
#include <vector>

namespace pbopt {
namespace smoothing {

template <class float_t> struct adagrad {
  adagrad(const float_t epsilon = 1E-6) : epsilon{epsilon} {}

  adagrad(const adagrad &) = default;
  adagrad &operator=(const adagrad &) = default;
  adagrad(adagrad &&) = default;
  adagrad &operator=(adagrad &&) = default;

  void params(const float_t epsilon) { this->epsilon = epsilon; }

  template <class InputIt> void initialize(InputIt xbegin, InputIt xend) {
    rms_g = std::vector<float_t>(std::distance(xbegin, xend));
  }

  template <class InputIt1, class InputIt2, class OutputIt>
  OutputIt smooth(const std::size_t k, InputIt1 xbegin, InputIt1 xend,
                  InputIt2 gold_begin, OutputIt gnew_begin) {
    float_t g_val{0};
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
  ~adagrad() = default;

private:
  float_t epsilon{1E-6};
  std::vector<float_t> rms_g;
};

} // namespace smoothing
} // namespace pbopt

#endif
