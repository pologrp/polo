#ifndef ADADELTA_CPP_
#define ADADELTA_CPP_

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <iterator>
#include <vector>

namespace kth {
namespace optim {
namespace smoothing {

/**
 * @brief [`ADADELTA`](https://arxiv.org/abs/1212.5701v1) smoothing.
 *
 * @tparam float_t Floating-point type, *e.g.*, `float` or `double`.
 */
template <class float_t> struct adadelta {
  /**
   * @brief Construct with the provided information.
   *
   * @param[in] rho Scaling factor; \f$\rho \in [0, 1)\f$.
   * @param[in] epsilon Fuzz factor; \f$\epsilon > 0\f$.
   */
  adadelta(const float_t rho = 0.95, const float_t epsilon = 1E-6)
      : rho{rho}, epsilon{epsilon} {}

  // Usual suspects; needed due to the protected destructor.
  adadelta(const adadelta &) = default;
  adadelta &operator=(const adadelta &) = default;
  adadelta(adadelta &&) = default;
  adadelta &operator=(adadelta &&) = default;

  /**
   * @brief Initialize the smoother's internal variables.
   *
   * @tparam InputIt InputIterator.
   * @param first Beginning of the decision vector.
   * @param last Past-the-end of the decision vector.
   */
  template <class InputIt> void init(InputIt first, InputIt last) {
    x_prev = std::vector<float_t>(first, last);
    rms_g = std::vector<float_t>(x_prev.size());
    rms_x = std::vector<float_t>(x_prev.size());
  }

  /**
   * @brief Smoothing function for the descent direction.
   *
   * @tparam InputIt1 InputIterator.
   * @tparam InputIt2 InputIterator.
   * @tparam OutputIt OutputIterator.
   * @param d Beginning of the descent direction.
   * @param x Beginning of the decision variable.
   * @param k Current iteration count.
   * @param g Beginning of the smoothed direction.
   * @return OutputIt Iterator to the past-the-end element of the smoothed
   * direction.
   */
  template <class InputIt1, class InputIt2, class OutputIt>
  OutputIt smooth(InputIt1 d, InputIt2 x, const std::size_t k, OutputIt g) {
    float_t delta_x{0};
    const std::size_t dim{x_prev.size()};
    // Accumulate squared gradient
    std::transform(rms_g.cbegin(), rms_g.cend(), d, rms_g.begin(),
                   [&](const float_t &lhs, const float_t &rhs) {
                     return rho * lhs + (1 - rho) * rhs * rhs;
                   });
    // Accumulate squared differences
    for (std::size_t idx = 0; idx < dim; idx++) {
      delta_x = x[idx] - x_prev[idx];
      rms_x[idx] = rho * rms_x[idx] + (1 - rho) * delta_x * delta_x;
    }
    // Copy new decision vector
    std::copy(x, x + dim, x_prev.begin());
    // Scale the gradient
    for (std::size_t idx = 0; idx < dim; idx++)
      g[idx] = df[idx] * (std::sqrt(rms_x[idx]) + epsilon) /
               (std::sqrt(rms_g[idx]) + epsilon);
    // Return the past-the-end iterator
    return g + dim;
  }

protected:
  ~adadelta() = default;

private:
  float_t rho{0.95}, epsilon{1E-6};
  std::vector<float_t> rms_g, rms_x, x_prev;
};

} // namespace smoothing
} // namespace optim
} // namespace kth

#endif
