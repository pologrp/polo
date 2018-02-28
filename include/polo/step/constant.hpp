#ifndef CONSTANT_HPP_
#define CONSTANT_HPP_

#include <cstddef>

namespace polo {
namespace step {

template <class value_t> struct constant {
  constant(const value_t gamma = 1) : gamma{gamma} {}

  constant(const constant &) = default;
  constant &operator=(const constant &) = default;
  constant(constant &&) = default;
  constant &operator=(constant &&) = default;

  template <class InputIt1, class InputIt2>
  value_t step(const std::size_t k, const value_t fval, InputIt1 xbegin,
               InputIt1 xend, InputIt2 gbegin) const {
    return gamma;
  }

protected:
  void params(const value_t gamma) { this->gamma = gamma; }

  template <class InputIt> void initialize(InputIt, InputIt) {}

  ~constant() = default;

private:
  value_t gamma{1};
};

} // namespace step
} // namespace polo

#endif
