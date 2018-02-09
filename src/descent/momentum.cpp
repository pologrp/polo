#ifndef MOMENTUM_CPP_
#define MOMENTUM_CPP_

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <vector>

namespace pbopt {
namespace descent {

template <class float_t> struct momentum {
  momentum(const float_t mu = 0.95, const float_t epsilon = 1E-3)
      : mu{mu}, epsilon{epsilon} {}

  // Usual suspects; needed due to the protected destructor.
  momentum(const momentum &) = default;
  momentum &operator=(const momentum &) = default;
  momentum(momentum &&) = default;
  momentum &operator=(momentum &&) = default;

  template <class InputIt> void init(InputIt first, InputIt last) {
    nu = std::vector<float_t>(std::distance(first, last));
  }

  template <class InputIt, class OutputIt>
  OutputIt direction(InputIt g_first, InputIt g_last, OutputIt d) {
    std::size_t idx{0};
    float_t g_val;
    while (g_first != g_last) {
      // Read values
      g_val = *g_first++;
      // Update values
      nu[idx] = -mu * nu[idx] + epsilon * g_val;
      // Improve the gradient
      *d++ = nu[idx];
      // Increment index
      idx++;
    }
    return d;
  }

protected:
  ~momentum() = default;

private:
  float_t mu{0.9}, epsilon{1E-3};
  std::vector<float_t> nu;
};

} // namespace descent
} // namespace pbopt

#endif
