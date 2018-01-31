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
   * @param first Beginning of the decision variable.
   * @param last Past-the-end of the decision variable.
   */
  template <class InputIt> void init(InputIt first, InputIt last) {
    const std::size_t dim = std::distance(first, last);
    x_prev = std::vector<float_t>(first, last);
    rms_g = std::vector<float_t>(dim);
    rms_x = std::vector<float_t>(dim);
  }

  /**
   * @brief Smoothing function for the descent direction.
   *
   * @tparam InputIt1 InputIterator.
   * @tparam InputIt2 InputIterator.
   * @tparam OutputIt OutputIterator.
   * @param d_first Beginning of the descent direction.
   * @param d_last Past-the-end of the descent direction.
   * @param x Beginning of the decision variable.
   * @param k Current iteration count.
   * @param g Beginning of the smoothed direction.
   * @return OutputIt Iterator to the past-the-end element of the smoothed
   * direction.
   */
  template <class InputIt1, class InputIt2, class OutputIt>
  OutputIt smooth(InputIt1 d_first, InputIt1 d_last, InputIt2 x,
                  const std::size_t k, OutputIt g) {
    float_t x_val{0}, x_del{0}, d_val{0};
    std::size_t idx{0};
    while (d_first != d_last) {
      // Read values
      x_val = *x++;
      d_val = *d_first++;
      x_del = x_val - x_prev[idx];
      // Accumulate squared gradient
      rms_g[idx] = rho * rms_g[idx] + (1 - rho) * d_val * d_val;
      // Accumulate squared differences
      rms_x[idx] = rho * rms_x[idx] + (1 - rho) * x_del * x_del;
      // Scale the gradient
      *g++ = d_val * (std::sqrt(rms_x[idx]) + epsilon) /
             (std::sqrt(rms_g[idx]) + epsilon);
      // Save old data
      x_prev[idx] = x_val;
      // Increment index
      idx++;
    }
    // Return the past-the-end iterator
    return g;
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
