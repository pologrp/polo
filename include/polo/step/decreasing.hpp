#ifndef POLO_STEP_DECREASING_HPP_
#define POLO_STEP_DECREASING_HPP_

#include <cmath>

namespace polo {
namespace step {
template <class value_t, class index_t> struct decreasing {
  decreasing(const value_t gamma = 1) : gamma{gamma} {}

  decreasing(const decreasing &) = default;
  decreasing &operator=(const decreasing &) = default;
  decreasing(decreasing &&) = default;
  decreasing &operator=(decreasing &&) = default;

  template <class InputIt1, class InputIt2>
  value_t step(const index_t klocal, const index_t kglobal, const value_t,
               InputIt1, InputIt1, InputIt2) const {
    return gamma / std::sqrt(kglobal);
  }

protected:
  void parameters(const value_t gamma) { this->gamma = gamma; }

  template <class InputIt> void initialize(InputIt, InputIt) {}

  ~decreasing() = default;

private:
  value_t gamma{1};
};
} // namespace step
} // namespace polo

#endif
