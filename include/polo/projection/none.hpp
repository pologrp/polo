#ifndef NOPROJECTION_HPP_
#define NOPROJECTION_HPP_

namespace polo {
namespace projection {
template <class value_t> struct none {
  none() = default;

  none(const none &) = default;
  none &operator=(const none &) = default;
  none(none &&) = default;
  none &operator=(none &&) = default;

  template <class InputIt1, class InputIt2, class OutputIt>
  OutputIt project(const value_t step, InputIt1 xold_begin, InputIt1 xold_end,
                   InputIt2 gbegin, OutputIt xnew_begin) {
    while (xold_begin != xold_end)
      *xnew_begin++ = *xold_begin++ - step * *gbegin++;
    return xnew_begin;
  }

protected:
  template <class InputIt> void initialize(InputIt, InputIt) {}

  ~none() = default;
};
} // namespace projection
} // namespace polo

#endif
