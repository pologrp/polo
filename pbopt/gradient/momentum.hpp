#ifndef MOMENTUM_HPP_
#define MOMENTUM_HPP_

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <vector>

namespace pbopt {
namespace gradient {

template <class float_t> struct momentum {
  momentum(const float_t mu = 0.95, const float_t epsilon = 1E-3)
      : mu{mu}, epsilon{epsilon} {}

  momentum(const momentum &) = default;
  momentum &operator=(const momentum &) = default;
  momentum(momentum &&) = default;
  momentum &operator=(momentum &&) = default;

  void params(const float_t mu, const float_t epsilon) {
    this->mu = mu;
    this->epsilon = epsilon;
  }

  template <class InputIt> void initialize(InputIt xbegin, InputIt xend) {
    nu = std::vector<float_t>(std::distance(xbegin, xend));
  }

  template <class InputIt, class OutputIt>
  OutputIt grad(InputIt gold_begin, InputIt gold_end, OutputIt gnew_begin) {
    std::size_t idx{0};
    float_t g_val;
    while (gold_begin != gold_end) {
      g_val = *gold_begin++;
      nu[idx] = mu * nu[idx] + epsilon * g_val;
      *gnew_begin++ = nu[idx];
      idx++;
    }
    return gnew_begin;
  }

protected:
  ~momentum() = default;

private:
  float_t mu{0.9}, epsilon{1E-3};
  std::vector<float_t> nu;
};

} // namespace gradient
} // namespace pbopt

#endif
