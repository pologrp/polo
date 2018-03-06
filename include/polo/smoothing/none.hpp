#ifndef NOSMOOTHING_HPP_
#define NOSMOOTHING_HPP_

namespace polo {
namespace smoothing {
template <class value_t> struct none {
  none() = default;

  none(const none &) = default;
  none &operator=(const none &) = default;
  none(none &&) = default;
  none &operator=(none &&) = default;

  template <class InputIt1, class InputIt2, class OutputIt>
  OutputIt smooth(const std::size_t k, InputIt1 xbegin, InputIt1 xend,
                  InputIt2 gold_begin, OutputIt gnew_begin) {
    while (xbegin != xend) {
      xbegin++;
      *gnew_begin++ = *gold_begin++;
    }
    return gnew_begin;
  }

protected:
  template <class InputIt> void initialize(InputIt, InputIt) {}

  ~none() = default;
};
} // namespace smoothing
} // namespace polo

#endif
