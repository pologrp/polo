#ifndef NESTEROV_CPP_
#define NESTEROV_CPP_

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <vector>

namespace kth {
namespace optim {
namespace descent {

template <class float_t> struct nesterov {
  nesterov(const float_t mu = 0.95, const float_t epsilon = 1E-3)
      : mu{mu}, epsilon{epsilon} {}

  // Usual suspects; needed due to the protected destructor.
  nesterov(const nesterov &) = default;
  nesterov &operator=(const nesterov &) = default;
  nesterov(nesterov &&) = default;
  nesterov &operator=(nesterov &&) = default;

  template <class InputIt> void init(InputIt first, InputIt last) {
    nu = std::vector<float_t>(std::distance(first, last));
  }

  template <class InputIt, class OutputIt>
  OutputIt direction(InputIt g_first, InputIt g_last, OutputIt d) {
    std::size_t idx{0};
    float_t nu_prev, g_val;
    while (g_first != g_last) {
      // Read values
      g_val = *g_first++;
      nu_prev = nu[idx];
      // Update values
      nu[idx] = mu * nu_prev - epsilon * g_val;
      // Improve the gradient
      *d++ = - mu * mu * nu_prev + (1 + mu) * epsilon * g_val;
      // Increment index
      idx++;
    }
    return d;
  }

private:
  float_t mu{0.95}, epsilon{1E-3};
  std::vector<float_t> nu;
};

} // namespace descent
} // namespace optim
} // namespace kth

#endif
