#ifndef L1NORM_HPP_
#define L1NORM_HPP_

#include <algorithm>
#include <cmath>

namespace polo {
namespace projection {

template <class float_t> struct l1norm {
  l1norm(const float_t lambda = 1) : lambda{lambda} {}

  l1norm(const l1norm &) = default;
  l1norm &operator=(const l1norm &) = default;
  l1norm(l1norm &&) = default;
  l1norm &operator=(l1norm &&) = default;

  template <class InputIt1, class InputIt2, class OutputIt>
  OutputIt project(const float_t step, InputIt1 xold_begin, InputIt1 xold_end,
                   InputIt2 gbegin, OutputIt xnew_begin) {
    float_t xval, temp;
    while (xold_begin != xold_end) {
      xval = *xold_begin++ - step * *gbegin++;
      temp = std::max(std::fabs(xval - lambda * step), float_t{0});
      *xnew_begin++ = xval < float_t{0} ? -temp : temp;
    }
    return xnew_begin;
  }

protected:
  void params(const float_t lambda) { this->lambda = lambda; }

  template <class InputIt> void initialize(InputIt, InputIt) {}

  ~l1norm() = default;

private:
  float_t lambda{1};
};

} // namespace projection
} // namespace polo

#endif
