#ifndef L2BALL_CPP_
#define L2BALL_CPP_

#include <algorithm>
#include <cmath>
#include <vector>

namespace kth {
namespace optim {
namespace projection {

template <class float_t> struct l2ball {
  l2ball(const float_t r = 1) r{r} {}

  l2ball(const l2ball &) = default;
  l2ball &operator=(const l2ball &) = default;
  l2ball(l2ball &&) = default;
  l2ball &operator=(l2ball &&) = default;

  template <class InputIt1, class InputIt2, class OutputIt>
  OutputIt project(InputIt1 g_begin, InputIt1 g_end, InputIt2 x_begin,
                   const float_t step, OutputIt xnew) {
    float_t temp, radius{0}, scaling;
    std::size_t idx{0};
    OutputIt xtemp{xnew};

    while (g_begin != g_end) {
      temp = *x_begin++ - step * *g_begin++;
      *xtemp++ = temp;
      radius += (temp - c[idx]) * (temp - c[idx]);
      idx++;
    }

    idx = 0;
    scaling = r / std::max(std::sqrt(radius), r);

    for (const auto cval : c)
      *xnew++ = cval + scaling * (*xnew++ - cval);

    return xnew;
  }

protected:
  ~l2ball();

private:
  float_t r;
  std::vector<float_t> c;
};

} // namespace projection
} // namespace optim
} // namespace kth

#endif
