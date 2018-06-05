#ifndef POLO_PROX_L2BALL_HPP_
#define POLO_PROX_L2BALL_HPP_

#include <algorithm>
#include <cmath>
#include <iterator>
#include <vector>

namespace polo {
namespace prox {
template <class value_t, class index_t> struct l2ball {
  l2ball(const value_t r = 1) : r{r} {}

  l2ball(const l2ball &) = default;
  l2ball &operator=(const l2ball &) = default;
  l2ball(l2ball &&) = default;
  l2ball &operator=(l2ball &&) = default;

  template <class InputIt1, class InputIt2, class OutputIt>
  OutputIt prox(const value_t step, InputIt1 xprev_b, InputIt1 xprev_e,
                InputIt2 gcurr, OutputIt xcurr) const {
    value_t temp, radius{0}, scaling;
    index_t idx{0};
    OutputIt xtemp{xcurr};

    while (xprev_b != xprev_e) {
      temp = *xprev_b++ - step * *gcurr++;
      *xtemp++ = temp;
      radius += (temp - c[idx]) * (temp - c[idx]);
      idx++;
    }

    scaling = r / std::max(std::sqrt(radius), r);

    for (const auto cval : c)
      *xcurr = cval + scaling * (*xcurr++ - cval);

    return xcurr;
  }

protected:
  template <class InputIt>
  void parameters(const value_t r, InputIt cbegin, InputIt cend) {
    this->r = r;
    c = std::vector<value_t>(cbegin, cend);
  }
  template <class T> void parameters(const value_t r, const std::vector<T> &c) {
    parameters(r, std::begin(c), std::end(c));
  }

  template <class InputIt> void initialize(InputIt, InputIt) {}

  ~l2ball();

private:
  value_t r;
  std::vector<value_t> c;
};
} // namespace prox
} // namespace polo

#endif
