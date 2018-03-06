#ifndef L2BALL_HPP_
#define L2BALL_HPP_

#include <algorithm>
#include <cmath>
#include <vector>

namespace polo {
namespace projection {
template <class value_t> struct l2ball {
  l2ball(const value_t r = 1) : r{r} {}

  l2ball(const l2ball &) = default;
  l2ball &operator=(const l2ball &) = default;
  l2ball(l2ball &&) = default;
  l2ball &operator=(l2ball &&) = default;

  template <class InputIt1, class InputIt2, class OutputIt>
  OutputIt project(const value_t step, InputIt1 xold_begin, InputIt1 xold_end,
                   InputIt2 gbegin, OutputIt xnew_begin) {
    value_t temp, radius{0}, scaling;
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
  void params(const value_t r, std::vector<value_t> c) {
    this->r = r;
    this->c = std::move(c);
  }

  template <class InputIt> void initialize(InputIt, InputIt) {}

  ~l2ball();

private:
  value_t r;
  std::vector<value_t> c;
};
} // namespace projection
} // namespace polo

#endif
