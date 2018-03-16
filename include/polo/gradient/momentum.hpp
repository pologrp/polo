#ifndef MOMENTUM_HPP_
#define MOMENTUM_HPP_

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <mutex>
#include <vector>

namespace polo {
namespace gradient {
namespace detail {
class classical {};
class nesterov {};

template <class value_t, class index_t, class momentum_t> struct momentum {
  momentum(const value_t mu = 0.9, const value_t epsilon = 1E-3)
      : mu{mu}, epsilon{epsilon} {}

  momentum(const momentum &) = default;
  momentum &operator=(const momentum &) = default;
  momentum(momentum &&) = default;
  momentum &operator=(momentum &&) = default;

  template <class InputIt, class OutputIt>
  OutputIt grad(InputIt gold_begin, InputIt gold_end, OutputIt gnew_begin) {
    std::lock_guard<std::mutex> lock(sync);
    return grad(gold_begin, gold_end, gnew_begin, momentum_t{});
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
  template <class InputIt, class OutputIt>
  OutputIt grad(InputIt gold_begin, InputIt gold_end, OutputIt gnew_begin,
                classical) {
    index_t idx{0};
    value_t g_val;
    while (gold_begin != gold_end) {
      g_val = *gold_begin++;
      nu[idx] = mu * nu[idx] + epsilon * g_val;
      *gnew_begin++ = nu[idx];
      idx++;
    }
    return gnew_begin;
  }

  template <class InputIt, class OutputIt>
  OutputIt grad(InputIt gold_begin, InputIt gold_end, OutputIt gnew_begin,
                nesterov) {
    index_t idx{0};
    value_t nu_prev, g_val;
    while (gold_begin != gold_end) {
      g_val = *gold_begin++;
      nu_prev = nu[idx];
      nu[idx] = mu * nu_prev + epsilon * g_val;
      *gnew_begin++ = mu * mu * nu_prev + (1 + mu) * epsilon * g_val;
      idx++;
    }
    return gnew_begin;
  }

  value_t mu{0.9}, epsilon{1E-3};
  std::vector<value_t> nu;
  std::mutex sync;
};
} // namespace detail

template <class value_t, class index_t>
using momentum = detail::momentum<value_t, index_t, detail::classical>;
template <class value_t, class index_t>
using nesterov = detail::momentum<value_t, index_t, detail::nesterov>;
} // namespace gradient
} // namespace polo

#endif
