#ifndef ADADELTA_CPP_
#define ADADELTA_CPP_

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <iterator>
#include <vector>

namespace pbopt {
namespace smoothing {

template <class float_t> struct adadelta {
  adadelta(const float_t rho = 0.95, const float_t epsilon = 1E-6)
      : rho{rho}, epsilon{epsilon} {}

  adadelta(const adadelta &) = default;
  adadelta &operator=(const adadelta &) = default;
  adadelta(adadelta &&) = default;
  adadelta &operator=(adadelta &&) = default;

  void params(const float_t rho, const float_t epsilon) {
    this->rho = rho;
    this->epsilon = epsilon;
  }

  template <class InputIt> void initialize(InputIt xbegin, InputIt xend) {
    const std::size_t dim = std::distance(xbegin, xend);
    x_prev = std::vector<float_t>(xbegin, xend);
    rms_g = std::vector<float_t>(dim);
    rms_x = std::vector<float_t>(dim);
  }

  template <class InputIt1, class InputIt2, class OutputIt>
  OutputIt smooth(const std::size_t k, InputIt1 xbegin, InputIt1 xend,
                  InputIt2 gold_begin, OutputIt gnew_begin) {
    float_t x_val{0}, x_del{0}, g_val{0};
    std::size_t idx{0};
    while (xbegin != xend) {
      x_val = *xbegin++;
      g_val = *gold_begin++;
      x_del = x_val - x_prev[idx];
      rms_g[idx] = rho * rms_g[idx] + (1 - rho) * g_val * g_val;
      rms_x[idx] = rho * rms_x[idx] + (1 - rho) * x_del * x_del;
      *gnew_begin++ = g_val * (std::sqrt(rms_x[idx]) + epsilon) /
                      (std::sqrt(rms_g[idx]) + epsilon);
      x_prev[idx] = x_val;
      idx++;
    }
    return gnew_begin;
  }

protected:
  ~adadelta() = default;

private:
  float_t rho{0.95}, epsilon{1E-6};
  std::vector<float_t> rms_g, rms_x, x_prev;
};

} // namespace smoothing
} // namespace pbopt

#endif
