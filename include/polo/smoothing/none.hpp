#ifndef NOSMOOTHING_HPP_
#define NOSMOOTHING_HPP_

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
  OutputIt smooth(const index_t k, InputIt1 xbegin, InputIt1 xend,
                  InputIt2 gold_begin, OutputIt gnew_begin) const {
    return std::transform(xbegin, xend, gold_begin, gnew_begin,
                          [](const value_t x, const value_t g) { return g; });
  }

protected:
  template <class InputIt> void initialize(InputIt, InputIt) {}

  ~none() = default;
};
} // namespace smoothing
} // namespace polo

#endif
