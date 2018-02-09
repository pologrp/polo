#ifndef RMSPROP_CPP_
#define RMSPROP_CPP_

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <iterator>
#include <vector>

namespace pbopt {
namespace smoothing {

/**
 * @brief
 * [`RMSProp`](http://www.cs.toronto.edu/~tijmen/csc321/slides/lecture_slides_lec6.pdf)
 * smoothing.
 *
 * @tparam float_t Floating-point type, *e.g.*, `float` or `double`.
 */
template <class float_t> struct rmsprop {
  /**
   * @brief Constructor with the provided information.
   *
   * @param[in] rho Scaling factor; \f$\rho \in [0, 1)\f$.
   * @param[in] epsilon Fuzz factor; \f$\epsilon > 0\f$.
   */
  rmsprop(const float_t rho = 0.9, const float_t epsilon = 1E-6)
      : rho{rho}, epsilon{epsilon} {}

  // Usual suspects; needed due to the protected destructor.
  rmsprop(const rmsprop &) = default;
  rmsprop &operator=(const rmsprop &) = default;
  rmsprop(rmsprop &&) = default;
  rmsprop &operator=(rmsprop &&) = default;

  /**
   * @brief Initialize the smoother's internal variables.
   *
   * @tparam InputIt InputIterator.
   * @param first Beginning of the decision variable.
   * @param last Past-the-end of the decision variable.
   */
  template <class InputIt> void init(InputIt first, InputIt last) {
    rms_g = std::vector<float_t>(std::distance(first, last));
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
    float_t d_val{0};
    std::size_t idx{0};
    while (d_first != d_last) {
      // Read values
      d_val = *d_first++;
      // Accumulate squared gradient
      rms_g[idx] = rho * rms_g[idx] + (1 - rho) * d_val * d_val;
      // Scale the gradient
      *g++ = d_val / (std::sqrt(rms_g[idx]) + epsilon);
      // Increment index
      idx++;
    }
    // Return the past-the-end iterator
    return g;
  }

protected:
  ~rmsprop() = default;

private:
  float_t rho{0.9}, epsilon{1E-6};
  std::vector<float_t> rms_g;
};

} // namespace smoothing
} // namespace pbopt

#endif
