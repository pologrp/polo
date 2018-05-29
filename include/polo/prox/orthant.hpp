#ifndef POLO_PROX_ORTHANT_HPP_
#define POLO_PROX_ORTHANT_HPP_

#include <algorithm>
#include <type_traits>

namespace polo {
namespace prox {
namespace detail {
class positive {};
class negative {};

template <class value_t, class index_t, class side_t> struct orthant {
  orthant() = default;

  orthant(const orthant &) = default;
  orthant &operator=(const orthant &) = default;
  orthant(orthant &&) = default;
  orthant &operator=(orthant &&) = default;

  template <class InputIt1, class InputIt2, class OutputIt>
  OutputIt prox(const value_t step, InputIt1 xold_begin, InputIt1 xold_end,
                InputIt2 gbegin, OutputIt xnew_begin) const {
    return prox(step, xold_begin, xold_end, gbegin, xnew_begin, side_t{});
  }

protected:
  template <class InputIt> void initialize(InputIt, InputIt) {}

  ~orthant() = default;

private:
  template <class InputIt1, class InputIt2, class OutputIt>
  OutputIt prox(const value_t step, InputIt1 xold_begin, InputIt1 xold_end,
                InputIt2 gbegin, OutputIt xnew_begin, positive) const {
    while (xold_begin != xold_end)
      *xnew_begin++ = std::max(*xold_begin++ - step * *gbegin++, value_t{0});
    return xnew_begin;
  }
  template <class InputIt1, class InputIt2, class OutputIt>
  OutputIt prox(const value_t step, InputIt1 xold_begin, InputIt1 xold_end,
                InputIt2 gbegin, OutputIt xnew_begin, negative) const {
    while (xold_begin != xold_end)
      *xnew_begin++ = std::min(*xold_begin++ - step * *gbegin++, value_t{0});
    return xnew_begin;
  }
};
} // namespace detail

template <class value_t, class index_t>
using positive_orthant = detail::orthant<value_t, index_t, detail::positive>;
template <class value_t, class index_t>
using negative_orthant = detail::orthant<value_t, index_t, detail::negative>;
} // namespace prox
} // namespace polo

#endif
