#ifndef CONSTANT_HPP_
#define CONSTANT_HPP_

#include <cstddef>

namespace pbopt {
namespace step {

template <class float_t> struct constant {
  constant(const float_t gamma = 1) : gamma{gamma} {}

  constant(const constant &) = default;
  constant &operator=(const constant &) = default;
  constant(constant &&) = default;
  constant &operator=(constant &&) = default;

  template <class InputIt1, class InputIt2>
  float_t step(const std::size_t k, const float_t fval, InputIt1 xbegin,
               InputIt1 xend, InputIt2 gbegin) const {
    return gamma;
  }

protected:
  void params(const float_t gamma) { this->gamma = gamma; }

  template <class InputIt> void initialize(InputIt, InputIt) {}

  ~constant() = default;

private:
  float_t gamma{1};
};

} // namespace step
} // namespace pbopt

#endif
