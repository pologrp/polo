#ifndef NOPROJECTION_CPP_
#define NOPROJECTION_CPP_

namespace kth {
namespace optim {
namespace projection {

template <class float_t> struct none {
  none() = default;

  // Usual suspects; needed due to the protected destructor.
  none(const none &) = default;
  none &operator=(const none &) = default;
  none(none &&) = default;
  none &operator=(none &&) = default;

  template <class InputIt1, class InputIt2, class OutputIt>
  OutputIt project(InputIt1 g_first, InputIt1 g_last, InputIt2 xold,
                   const float_t step, OutputIt xnew) {
    while (g_first != g_last)
      *xnew++ = *xold++ - step * *g_first++;
    return xnew;
  }

protected:
  ~none() = default;
};

} // namespace projection
} // namespace optim
} // namespace kth

#endif
