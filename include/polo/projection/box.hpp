#ifndef BOX_HPP_
#define BOX_HPP_

#include <algorithm>
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
  void params(std::vector<value_t> l, std::vector<value_t> u) {
    this->l = std::move(l);
    this->u = std::move(u);
  }

  template <class InputIt> void initialize(InputIt, InputIt) {}

  ~box() = default;

private:
  std::vector<value_t> l, u;
};
} // namespace projection
} // namespace polo

#endif
