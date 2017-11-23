#ifndef CONSTANT_CPP_
#define CONSTANT_CPP_

namespace kth {
namespace optim {
namespace step {

/**
 * @brief Constant step-size policy, *i.e.*, \f$\gamma(k) = \gamma_{0}\f$.
 *
 * @tparam float_t Floating-point type, *e.g.*, `float` or `double`.
 * @tparam int_t Integer type, *e.g.*, `int32_t` or `int64_t`.
 */
template <class float_t, class int_t> struct constant {
  /**
   * @brief Default constructor with \f$\gamma_{0} = 0\f$.
   *
   */
  constant() = default;
  /**
   * @brief Constructor with a provided step-size.
   *
   * @param[in] step Non-negative initial step-size.
   */
  constant(float_t step) : step{step} {}
  /**
   * @brief Set step-size.
   *
   * @param[in] step New step-size.
   */
  void set(float_t step) { this->step = step; }
  /**
   * @brief Get current step-size.
   *
   * @return float_t Current step-size.
   */
  float_t get() const { return step; }
  /**
   * @brief Get current step-size based on the algorithm's state.
   *
   * @param[in] k Current iteration count.
   * @param[in] N Dimension of `x` and `dx`.
   * @param[in] x Current decision vector.
   * @param[in] dx Current first-order information.
   * @return float_t Current step-size.
   */
  float_t get(const int_t k, const int_t N, const float_t *x,
              const float_t *dx) const {
    return step;
  }

protected:
  ~constant() = default;

private:
  float_t step{0};
};

} // namespace step
} // namespace optim
} // namespace kth

#endif