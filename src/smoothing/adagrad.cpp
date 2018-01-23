#ifndef ADAGRAD_CPP_
#define ADAGRAD_CPP_

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <iterator>
#include <vector>

namespace kth {
namespace optim {
namespace smoothing {

/**
 * @brief [`AdaGrad`](http://www.jmlr.org/papers/v12/duchi11a.html) smoothing.
 *
 * @tparam float_t Floating-point type, *e.g.*, `float` or `double`.
 */
template <class float_t> struct adagrad {
  /**
   * @brief Construct with the provided information.
   *
   * @param[in] epsilon Fuzz factor; \f$\epsilon > 0\f$.
   */
  adagrad(const float_t epsilon = 1E-6) : epsilon{epsilon} {}

  // Usual suspects; needed due to the protected destructor.
  adagrad(const adagrad &) = default;
  adagrad &operator=(const adagrad &) = default;
  adagrad(adagrad &&) = default;
  adagrad &operator=(adagrad &&) = default;

  /**
   * @brief Initialize the smoother's internal variables.
   *
   * @tparam InputIt InputIterator.
   * @param first Beginning of the decision vector.
   * @param last Past-the-end of the decision vector.
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
   * @param d Beginning of the descent direction.
   * @param x Beginning of the decision variable.
   * @param k Current iteration count.
   * @param g Beginning of the smoothed direction.
   * @return OutputIt Iterator to the past-the-end element of the smoothed
   * direction.
   */
  template <class InputIt1, class InputIt2, class OutputIt>
  OutputIt smooth(InputIt1 d, InputIt2 x, const std::size_t k, OutputIt g) {
    // Accumulate squared gradient
    std::transform(
        rms_g.cbegin(), rms_g.cend(), d, rms_g.begin(),
        [](const float_t &lhs, const float_t &rhs) { return lhs + rhs * rhs; });
    // Scale the gradient
    return std::transform(rms_g.cbegin(), rms_g.cend(), d, g,
                          [&](const float_t &lhs, const float_t &rhs) {
                            return rhs / (std::sqrt(lhs) + epsilon);
                          });
  }

protected:
  ~adagrad() = default;

private:
  float_t epsilon{1E-6};
  std::vector<float_t> rms_g;
};

} // namespace smoothing
} // namespace optim
} // namespace kth

#endif
