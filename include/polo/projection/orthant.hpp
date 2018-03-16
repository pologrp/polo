#ifndef ORTHANT_HPP_
#define ORTHANT_HPP_

#include <algorithm>
#include <type_traits>

namespace polo {
namespace projection {
namespace detail {
template <class value_t, class index_t, bool is_pos> struct orthant {
  orthant() = default;

  orthant(const orthant &) = default;
  orthant &operator=(const orthant &) = default;
  orthant(orthant &&) = default;
  orthant &operator=(orthant &&) = default;

  template <class InputIt1, class InputIt2, class OutputIt>
  OutputIt project(const value_t step, InputIt1 xold_begin, InputIt1 xold_end,
                   InputIt2 gbegin, OutputIt xnew_begin) const {
    return project(step, xold_begin, xold_end, gbegin, xnew_begin,
                   std::integral_constant<bool, is_pos>{});
  }

protected:
  template <class InputIt> void initialize(InputIt, InputIt) {}

  ~orthant() = default;

private:
  template <class InputIt1, class InputIt2, class OutputIt>
  OutputIt project(const value_t step, InputIt1 xold_begin, InputIt1 xold_end,
                   InputIt2 gbegin, OutputIt xnew_begin, std::true_type) const {
    while (xold_begin != xold_end)
      *xnew_begin++ = std::max(*xold_begin++ - step * *gbegin++, value_t{0});
    return xnew_begin;
  }
  template <class InputIt1, class InputIt2, class OutputIt>
  OutputIt project(const value_t step, InputIt1 xold_begin, InputIt1 xold_end,
                   InputIt2 gbegin, OutputIt xnew_begin,
                   std::false_type) const {
    while (xold_begin != xold_end)
      *xnew_begin++ = std::min(*xold_begin++ - step * *gbegin++, value_t{0});
    return xnew_begin;
  }
};
} // namespace detail

template <class value_t, class index_t>
using positive_orthant = detail::orthant<value_t, index_t, true>;
template <class value_t, class index_t>
using negative_orthant = detail::orthant<value_t, index_t, false>;
} // namespace projection
} // namespace polo

#endif
