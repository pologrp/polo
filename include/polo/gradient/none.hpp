#ifndef NOGRADIENT_HPP_
#define NOGRADIENT_HPP_

#include <algorithm>

namespace polo {
namespace gradient {

template <class value_t> struct none {
  none() = default;

  none(const none &) = default;
  none &operator=(const none &) = default;
  none(none &&) = default;
  none &operator=(none &&) = default;

  template <class InputIt, class OutputIt>
  OutputIt grad(InputIt gold_begin, InputIt gold_end, OutputIt gnew_begin) {
    return std::copy(gold_begin, gold_end, gnew_begin);
  }

protected:
  template <class InputIt> void initialize(InputIt, InputIt) {}

  ~none() = default;
};

} // namespace gradient
} // namespace polo

#endif
