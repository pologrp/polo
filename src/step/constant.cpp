#ifndef CONSTANT_CPP_
#define CONSTANT_CPP_

namespace kth {
namespace optim {
namespace step {

/**
 * @brief Constant step-size policy, *i.e.*, \f$\gamma_{k} = \gamma\f$.
 *
 * @tparam float_t Floating-point type, *e.g.*, `float` or `double`.
 * @tparam uint_t Unsigned integer type, *e.g.*, `uint32_t` or `uint64_t`.
 */
template <class float_t, class uint_t> struct constant {
  /**
   * @brief Construct with \f$\gamma = 1.0\f$.
   *
   */
  constant() = default;
  constant(const constant &) = default;
  constant &operator=(const constant &) = default;
  constant(constant &&) = default;
  constant &operator=(constant &&) = default;
  /**
   * @brief Construct with the provided step-size, \f$\gamma\f$.
   *
   * @param[in] gamma Non-negative initial step-size.
   */
  constant(const float_t gamma) : gamma{gamma} {}
  /**
   * @brief Set step-size.
   *
   * @param[in] step New step-size.
   */
  void set(const float_t gamma) { this->gamma = gamma; }
  /**
   * @brief Get current step-size based on the information available.
   *
   * @param[in] df Current first-order information.
   * @param[in] x Current decision vector.
   * @param[in] d Dimension of `df` and `x`.
   * @param[in] k Current iteration count.
   * @return float_t Current step-size.
   */
  float_t get(const float_t *df, const float_t *x, const uint_t d,
              const uint_t k) const {
    return gamma;
  }

protected:
  ~constant() = default;

private:
  float_t gamma{1};
};

} // namespace step
} // namespace optim
} // namespace kth

#endif
