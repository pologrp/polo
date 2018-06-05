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
  OutputIt prox(const value_t step, InputIt1 xprev_b, InputIt1 xprev_e,
                InputIt2 gcurr, OutputIt xcurr) const {
    return prox(step, xprev_b, xprev_e, gcurr, xcurr, side_t{});
  }

protected:
  template <class InputIt> void initialize(InputIt, InputIt) {}

  ~orthant() = default;

private:
  template <class InputIt1, class InputIt2, class OutputIt>
  OutputIt prox(const value_t step, InputIt1 xprev_b, InputIt1 xprev_e,
                InputIt2 gcurr, OutputIt xcurr, positive) const {
    while (xprev_b != xprev_e)
      *xcurr++ = std::max(*xprev_b++ - step * *gcurr++, value_t{0});
    return xcurr;
  }
  template <class InputIt1, class InputIt2, class OutputIt>
  OutputIt prox(const value_t step, InputIt1 xprev_b, InputIt1 xprev_e,
                InputIt2 gcurr, OutputIt xcurr, negative) const {
    while (xprev_b != xprev_e)
      *xcurr++ = std::min(*xprev_b++ - step * *gcurr++, value_t{0});
    return xcurr;
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
