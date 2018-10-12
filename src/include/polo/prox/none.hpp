#ifndef POLO_PROX_NONE_HPP_
#define POLO_PROX_NONE_HPP_

namespace polo {
namespace prox {
template <class value_t, class index_t> struct none {
  none() = default;

  none(const none &) = default;
  none &operator=(const none &) = default;
  none(none &&) = default;
  none &operator=(none &&) = default;

  template <class InputIt1, class InputIt2, class OutputIt>
  OutputIt prox(const value_t step, InputIt1 xprev_b, InputIt1 xprev_e,
                InputIt2 gcurr, OutputIt xcurr) const {
    while (xprev_b != xprev_e)
      *xcurr++ = *xprev_b++ - step * *gcurr++;
    return xcurr;
  }

protected:
  template <class InputIt> void initialize(InputIt, InputIt) {}

  ~none() = default;
};
} // namespace prox
} // namespace polo

#endif
