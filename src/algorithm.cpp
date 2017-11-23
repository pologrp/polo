/**
 * @brief Algorithm abstraction.
 *
 * @tparam float_t `float` or `double`.
 * @tparam int_t `int64_t` or similar.
 * @tparam StepPolicy Policy for selecting a step-size.
 * @tparam OraclePolicy Policy for getting first-order information.
 */
template <class float_t, class int_t, template <class, class> class StepPolicy,
          template <class, class> class OraclePolicy>
struct algorithm : public StepPolicy<float_t, int_t>,
                   public OraclePolicy<float_t, int_t> {
  /**
   * @brief Default constructor.
   *
   */
  Algorithm() = default;
  /**
   * @brief Constructor with a step-size.
   *
   * @param[in] step Non-negative step-size value.
   */
  Algorithm(float_t step) : StepPolicy<float_t, int_t>{step} {}

  /**
   * @brief Set the initial step-size of the algorithm.
   *
   * @param[in] step Non-negative step-size value.
   */
  void set_stepsize(float_t step) { StepPolicy<float_t, int_t>::set(step); }
  /**
   * @brief Get the current step-size of the algorithm.
   *
   * This does *not* change the state of StepPolicy.
   *
   * @return float_t Current step-size.
   */
  float_t get_stepsize() const { return StepPolicy<float_t, int_t>::get(); }
  /**
   * @brief Get current step-size based on the algorithm's state.
   *
   * @param[in] k Current iteration count.
   * @param[in] N Dimension of `x` and `dx`.
   * @param[in] x Current decision vector.
   * @param[in] dx Current first-order information.
   * @return float_t Current step-size.
   */
  float_t get_stepsize(const int_t k, const int_t N, const float_t *x,
                       const float_t *dx) {
    return StepPolicy<float_t, int_t>::get(k, N, x, dx);
  }

private:
  int_t k{0};
};
