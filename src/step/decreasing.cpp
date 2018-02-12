#ifndef DECREASING_CPP_
#define DECREASING_CPP_

#include <cmath>
#include <cstddef>

namespace pbopt {
namespace step {

template <class float_t> struct decreasing {
  decreasing(const float_t gamma = 1) : gamma{gamma} {}

  decreasing(const decreasing &) = default;
  decreasing &operator=(const decreasing &) = default;
  decreasing(decreasing &&) = default;
  decreasing &operator=(decreasing &&) = default;

  void params(const float_t gamma) { this->gamma = gamma; }

  template <class InputIt> void initialize(InputIt, InputIt) {}

  template <class InputIt1, class InputIt2>
  float_t step(const std::size_t k, const float_t fval, InputIt1 xbegin,
               InputIt1 xend, InputIt2 gbegin) const {
    return gamma / std::sqrt(k);
  }

protected:
  ~decreasing() = default;

private:
  float_t gamma{1};
};

} // namespace step
} // namespace pbopt

#endif
