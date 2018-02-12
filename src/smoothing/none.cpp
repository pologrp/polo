#ifndef NOSMOOTHING_CPP_
#define NOSMOOTHING_CPP_

namespace pbopt {
namespace smoothing {

template <class float_t> struct none {
  none() = default;

  none(const none &) = default;
  none &operator=(const none &) = default;
  none(none &&) = default;
  none &operator=(none &&) = default;

  template <class InputIt> void initialize(InputIt, InputIt) {}

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
  ~none() = default;
};

} // namespace smoothing
} // namespace pbopt

#endif
