#ifndef POLO_BOOSTING_NONE_HPP_
#define POLO_BOOSTING_NONE_HPP_

#include <algorithm>

namespace polo {
namespace boosting {
template <class value_t, class index_t> struct none {
  none() = default;

  none(const none &) = default;
  none &operator=(const none &) = default;
  none(none &&) = default;
  none &operator=(none &&) = default;

  template <class InputIt, class OutputIt>
  OutputIt boost(const index_t, const index_t, const index_t, InputIt gprev_b,
                 InputIt gprev_e, OutputIt gcurr) const {
    return std::transform(gprev_b, gprev_e, gcurr,
                          [](const value_t g) { return g; });
  }

protected:
  template <class InputIt> void initialize(InputIt, InputIt) {}

  ~none() = default;
};
} // namespace boosting
} // namespace polo

#endif
