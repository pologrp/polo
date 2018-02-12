#ifndef L2BALL_CPP_
#define L2BALL_CPP_

#include <algorithm>
#include <cmath>
#include <vector>

namespace pbopt {
namespace projection {

template <class float_t> struct l2ball {
  l2ball(const float_t r = 1) : r{r} {}

  l2ball(const l2ball &) = default;
  l2ball &operator=(const l2ball &) = default;
  l2ball(l2ball &&) = default;
  l2ball &operator=(l2ball &&) = default;

  void params(const float_t r, std::vector<float_t> c) {
    this->r = r;
    this->c = std::move(c);
  }

  template <class InputIt> void initialize(InputIt, InputIt) {}

  template <class InputIt1, class InputIt2, class OutputIt>
  OutputIt project(const float_t step, InputIt1 xold_begin, InputIt1 xold_end,
                   InputIt2 gbegin, OutputIt xnew_begin) {
    float_t temp, radius{0}, scaling;
    std::size_t idx{0};
    OutputIt xtemp{xnew_begin};

    while (xold_begin != xold_end) {
      temp = *xold_begin++ - step * *gbegin++;
      *xtemp++ = temp;
      radius += (temp - c[idx]) * (temp - c[idx]);
      idx++;
    }

    idx = 0;
    scaling = r / std::max(std::sqrt(radius), r);

    for (const auto cval : c)
      *xnew_begin++ = cval + scaling * (*xnew_begin++ - cval);

    return xnew_begin;
  }

protected:
  ~l2ball();

private:
  float_t r;
  std::vector<float_t> c;
};

} // namespace projection
} // namespace pbopt

#endif
