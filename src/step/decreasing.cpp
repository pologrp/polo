#ifndef DECREASING_CPP_
#define DECREASING_CPP_

#include <cmath>
#include <cstddef>

namespace pbopt {
namespace step {

/**
 * @brief Decreasing step-size policy, *i.e.*, \f$\gamma_{k} =
 * \gamma/\sqrt{k}\f$.
 *
 * @tparam float_t Floating-point type, *e.g.*, `float` or `double`.
 */
template <class float_t> struct decreasing {
  /**
   * @brief Construct with the provided step-size, \f$\gamma\f$.
   *
   * @param[in] gamma Non-negative initial step-size.
   */
  decreasing(const float_t gamma = 1) : gamma{gamma} {}

  // Usual suspects; needed due to the protected destructor.
  decreasing(const decreasing &) = default;
  decreasing &operator=(const decreasing &) = default;
  decreasing(decreasing &&) = default;
  decreasing &operator=(decreasing &&) = default;

  /**
   * @brief Set step-size.
   *
   * @param[in] step New step-size.
   */
  void set(const float_t gamma) { this->gamma = gamma; }

  /**
   * @brief Get current step-size based on the information available.
   *
   * @tparam InputIt1 InputIterator.
   * @tparam InputIt2 InputIterator.
   * @param d_first Beginning of the descent direction.
   * @param d_last Past-the-end of the descent direction.
   * @param x Beginning of the decision variable.
   * @param k Current iteration count.
   * @return float_t Current step-size.
   */
  template <class InputIt1, class InputIt2>
  float_t get(InputIt1 d_first, InputIt1 d_last, InputIt2 x,
              const std::size_t k) const {
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
