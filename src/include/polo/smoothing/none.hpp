#ifndef POLO_SMOOTHING_NONE_HPP_
#define POLO_SMOOTHING_NONE_HPP_

#include <algorithm>

namespace polo {
namespace smoothing {
template <class value_t, class index_t> struct none {
  none() = default;

  none(const none &) = default;
  none &operator=(const none &) = default;
  none(none &&) = default;
  none &operator=(none &&) = default;

  template <class InputIt1, class InputIt2, class OutputIt>
  OutputIt smooth(const index_t, const index_t, InputIt1 xbegin, InputIt1 xend,
                  InputIt2 gprev, OutputIt gcurr) const {
    return std::transform(xbegin, xend, gprev, gcurr,
                          [](const value_t x, const value_t g) { return g; });
  }

protected:
  template <class InputIt> void initialize(InputIt, InputIt) {}

  ~none() = default;
};
} // namespace smoothing
} // namespace polo

#endif
