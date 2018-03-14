#ifndef BOX_HPP_
#define BOX_HPP_

#include <algorithm>
#include <iterator>
#include <vector>

namespace polo {
namespace projection {
template <class value_t> struct box {
  box() = default;

  box(const box &) = default;
  box &operator=(const box &) = default;
  box(box &&) = default;
  box &operator=(box &&) = default;

  template <class InputIt1, class InputIt2, class OutputIt>
  OutputIt poject(const value_t step, InputIt1 xold_begin, InputIt1 xold_end,
                  InputIt2 gbegin, OutputIt xnew_begin) {
    value_t temp;
    std::size_t idx{0};
    while (xold_begin != xold_end) {
      temp = *xold_begin++ - step * *gbegin++;
      *xnew_begin++ = std::min(std::max(temp, l[idx]), u[idx]);
      idx++;
    }
    return xnew_begin;
  }

protected:
  template <class InputIt1, class InputIt2>
  void params(InputIt1 lbegin, InputIt1 lend, InputIt2 ubegin, InputIt2 uend) {
    l = std::vector<value_t>(lbegin, lend);
    u = std::vector<value_t>(ubegin, uend);
  }
  template <class T1, class T2>
  void params(const std::vector<T1> &l, const std::vector<T2> &u) {
    params(std::begin(l), std::end(l), std::begin(u), std::end(u));
  }

  template <class InputIt> void initialize(InputIt, InputIt) {}

  ~box() = default;

private:
  std::vector<value_t> l, u;
};
} // namespace projection
} // namespace polo

#endif
