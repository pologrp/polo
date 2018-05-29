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
  OutputIt boost(InputIt gold_begin, InputIt gold_end,
                 OutputIt gnew_begin) const {
    return std::transform(gold_begin, gold_end, gnew_begin,
                          [](const value_t g) { return g; });
  }

protected:
  template <class InputIt> void initialize(InputIt, InputIt) {}

  ~none() = default;
};
} // namespace boosting
} // namespace polo

#endif
