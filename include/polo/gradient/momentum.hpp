#ifndef MOMENTUM_HPP_
#define MOMENTUM_HPP_

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <vector>

namespace polo {
namespace gradient {

template <class value_t> struct momentum {
  momentum(const value_t mu = 0.95, const value_t epsilon = 1E-3)
      : mu{mu}, epsilon{epsilon} {}

  momentum(const momentum &) = default;
  momentum &operator=(const momentum &) = default;
  momentum(momentum &&) = default;
  momentum &operator=(momentum &&) = default;

  template <class InputIt, class OutputIt>
  OutputIt grad(InputIt gold_begin, InputIt gold_end, OutputIt gnew_begin) {
    std::size_t idx{0};
    value_t g_val;
    while (gold_begin != gold_end) {
      g_val = *gold_begin++;
      nu[idx] = mu * nu[idx] + epsilon * g_val;
      *gnew_begin++ = nu[idx];
      idx++;
    }
    return gnew_begin;
  }

protected:
  void params(const value_t mu, const value_t epsilon) {
    this->mu = mu;
    this->epsilon = epsilon;
  }

  template <class InputIt> void initialize(InputIt xbegin, InputIt xend) {
    nu = std::vector<value_t>(std::distance(xbegin, xend));
  }

  ~momentum() = default;

private:
  value_t mu{0.9}, epsilon{1E-3};
  std::vector<value_t> nu;
};

} // namespace gradient
} // namespace polo

#endif
