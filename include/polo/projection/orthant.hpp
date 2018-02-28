#ifndef ORTHANT_HPP_
#define ORTHANT_HPP_

#include <algorithm>

namespace polo {
namespace projection {

struct _pos_orthant {};
struct _neg_orthant {};

template <class value_t, class T> struct orthant {
  orthant() = default;

  orthant(const orthant &) = default;
  orthant &operator=(const orthant &) = default;
  orthant(orthant &&) = default;
  orthant &operator=(orthant &&) = default;

  template <class InputIt1, class InputIt2, class OutputIt>
  OutputIt project(const value_t step, InputIt1 xold_begin, InputIt1 xold_end,
                   InputIt2 gbegin, OutputIt xnew_begin) {
    return project(step, xold_begin, xold_end, gbegin, xnew_begin, T{});
  }

protected:
  template <class InputIt> void initialize(InputIt, InputIt) {}

  ~orthant() = default;

private:
  template <class InputIt1, class InputIt2, class OutputIt>
  OutputIt project(const value_t step, InputIt1 xold_begin, InputIt1 xold_end,
                   InputIt2 gbegin, OutputIt xnew_begin, _pos_orthant) {
    while (xold_begin != xold_end)
      *xnew_begin++ = std::max(*xold_begin++ - step * *gbegin++, value_t{0});
    return xnew_begin;
  }
  template <class InputIt1, class InputIt2, class OutputIt>
  OutputIt project(const value_t step, InputIt1 xold_begin, InputIt1 xold_end,
                   InputIt2 gbegin, OutputIt xnew_begin, _neg_orthant) {
    while (xold_begin != xold_end)
      *xnew_begin++ = std::min(*xold_begin++ - step * *gbegin++, value_t{0});
    return xnew_begin;
  }
};

template <class value_t>
using positive_orthant = orthant<value_t, _pos_orthant>;

template <class value_t>
using negative_orthant = orthant<value_t, _neg_orthant>;

} // namespace projection
} // namespace polo

#endif