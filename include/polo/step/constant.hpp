#ifndef POLO_STEP_CONSTANT_HPP_
#define POLO_STEP_CONSTANT_HPP_

namespace polo {
namespace step {
template <class value_t, class index_t> struct constant {
  constant(const value_t gamma = 1) : gamma{gamma} {}

  constant(const constant &) = default;
  constant &operator=(const constant &) = default;
  constant(constant &&) = default;
  constant &operator=(constant &&) = default;

  template <class InputIt1, class InputIt2>
  value_t step(const index_t k, const value_t fval, InputIt1 xbegin,
               InputIt1 xend, InputIt2 gbegin) const {
    return gamma;
  }

protected:
  void parameters(const value_t gamma) { this->gamma = gamma; }

  template <class InputIt> void initialize(InputIt, InputIt) {}

  ~constant() = default;

private:
  value_t gamma{1};
};
} // namespace step
} // namespace polo

#endif
