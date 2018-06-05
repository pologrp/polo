#ifndef POLO_BOOSTING_MOMENTUM_HPP_
#define POLO_BOOSTING_MOMENTUM_HPP_

#include <algorithm>
#include <iterator>
#include <mutex>
#include <vector>

namespace polo {
namespace boosting {
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
  OutputIt boost(const index_t, const index_t, const index_t, InputIt gprev_b,
                 InputIt gprev_e, OutputIt gcurr) {
    std::lock_guard<std::mutex> lock(sync);
    return boost(gprev_b, gprev_e, gcurr, momentum_t{});
  }

protected:
  void parameters(const value_t mu, const value_t epsilon) {
    this->mu = mu;
    this->epsilon = epsilon;
  }

  template <class InputIt> void initialize(InputIt xbegin, InputIt xend) {
    nu = std::vector<value_t>(std::distance(xbegin, xend));
  }

  ~momentum() = default;

private:
  template <class InputIt, class OutputIt>
  OutputIt boost(InputIt gprev_b, InputIt gprev_e, OutputIt gcurr, classical) {
    index_t idx{0};
    value_t g_val;
    while (gprev_b != gprev_e) {
      g_val = *gprev_b++;
      nu[idx] = mu * nu[idx] + epsilon * g_val;
      *gcurr++ = nu[idx];
      idx++;
    }
    return gcurr;
  }

  template <class InputIt, class OutputIt>
  OutputIt boost(InputIt gprev_b, InputIt gprev_e, OutputIt gcurr, nesterov) {
    index_t idx{0};
    value_t nu_prev, g_val;
    while (gprev_b != gprev_e) {
      g_val = *gprev_b++;
      nu_prev = nu[idx];
      nu[idx] = mu * nu_prev + epsilon * g_val;
      *gcurr++ = mu * mu * nu_prev + (1 + mu) * epsilon * g_val;
      idx++;
    }
    return gcurr;
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
} // namespace boosting
} // namespace polo

#endif
