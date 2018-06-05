#ifndef POLO_PROX_BOX_HPP_
#define POLO_PROX_BOX_HPP_

#include <algorithm>
#include <iterator>
#include <vector>

namespace polo {
namespace prox {
template <class value_t, class index_t> struct box {
  box() = default;

  box(const box &) = default;
  box &operator=(const box &) = default;
  box(box &&) = default;
  box &operator=(box &&) = default;

  template <class InputIt1, class InputIt2, class OutputIt>
  OutputIt prox(const value_t step, InputIt1 xprev_b, InputIt1 xprev_e,
                InputIt2 gcurr, OutputIt xcurr) const {
    value_t temp;
    index_t idx{0};
    while (xprev_b != xprev_e) {
      temp = *xprev_b++ - step * *gcurr++;
      *xcurr++ = std::min(std::max(temp, l[idx]), u[idx]);
      idx++;
    }
    return xcurr;
  }

protected:
  template <class InputIt1, class InputIt2>
  void parameters(InputIt1 lb_b, InputIt1 lb_e, InputIt2 ub_b, InputIt2 ub_e) {
    l = std::vector<value_t>(lb_b, lb_e);
    u = std::vector<value_t>(ub_b, ub_e);
  }
  template <class T1, class T2>
  void parameters(const std::vector<T1> &l, const std::vector<T2> &u) {
    parameters(std::begin(l), std::end(l), std::begin(u), std::end(u));
  }

  template <class InputIt> void initialize(InputIt, InputIt) {}

  ~box() = default;

private:
  std::vector<value_t> l, u;
};
} // namespace prox
} // namespace polo

#endif
