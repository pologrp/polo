#ifndef NOPROJECTION_CPP_
#define NOPROJECTION_CPP_

namespace pbopt {
namespace projection {

template <class float_t> struct none {
  none() = default;

  none(const none &) = default;
  none &operator=(const none &) = default;
  none(none &&) = default;
  none &operator=(none &&) = default;

  template <class InputIt> void initialize(InputIt, InputIt) {}

  template <class InputIt1, class InputIt2, class OutputIt>
  OutputIt project(const float_t step, InputIt1 xold_begin, InputIt1 xold_end,
                   InputIt2 gbegin, OutputIt xnew_begin) {
    while (xold_begin != xold_end)
      *xnew_begin++ = *xold_begin++ - step * *gbegin++;
    return xnew_begin;
  }

protected:
  ~none() = default;
};

} // namespace projection
} // namespace pbopt

#endif
