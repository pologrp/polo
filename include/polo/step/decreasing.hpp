#ifndef DECREASING_HPP_
#define DECREASING_HPP_

#include <cmath>
#include <cstddef>

namespace polo {
namespace step {
template <class value_t, class index_t> struct decreasing {
  decreasing(const value_t gamma = 1) : gamma{gamma} {}

  decreasing(const decreasing &) = default;
  decreasing &operator=(const decreasing &) = default;
  decreasing(decreasing &&) = default;
  decreasing &operator=(decreasing &&) = default;

  template <class InputIt1, class InputIt2>
  value_t step(const index_t k, const value_t fval, InputIt1 xbegin,
               InputIt1 xend, InputIt2 gbegin) const {
    return gamma / std::sqrt(k);
  }

protected:
  void params(const value_t gamma) { this->gamma = gamma; }

  template <class InputIt> void initialize(InputIt, InputIt) {}

  ~decreasing() = default;

private:
  value_t gamma{1};
};
} // namespace step
} // namespace polo

#endif
