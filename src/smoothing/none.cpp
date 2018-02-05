#ifndef NOSMOOTHING_CPP_
#define NOSMOOTHING_CPP_

namespace kth {
namespace optim {
namespace smoothing {

template <class float_t> struct none {
  none() = default;

  // Usual suspects; needed due to the protected destructor.
  none(const none &) = default;
  none &operator=(const none &) = default;
  none(none &&) = default;
  none &operator=(none &&) = default;

  template <class InputIt1, class InputIt2, class OutputIt>
  OutputIt smooth(InputIt1 d_first, InputIt1 d_last, InputIt2 x,
                  const std::size_t k, OutputIt g) {
    while (d_first != d_last)
      *g++ = *d_first++;
    return g;
  }

protected:
  ~none() = default;
};

} // namespace smoothing
} // namespace optim
} // namespace kth

#endif
